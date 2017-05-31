#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <future>
#include <thread>
#include <iterator>
#include "socket_y_sesion.h"
#include "nube.h"

using namespace std;

extern string cargar_valor(string);

void sesion::iniciar()
{
  cout << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << '\n';
  hacer_escritura("por favor inicia sesion:");
  iniciar_sesion_usuario();
}

void sesion::iniciar_sesion_usuario()
{
  auto tu_mismo(shared_from_this());
  socket_.async_read_some(asio::buffer(data_, longitud_maxima),
    [this, tu_mismo](std::error_code ec, std::size_t longitud)
  {
    if (!ec)
    {
      string lectura = data_;
      usuario_ = usuario::autenticar_usuario(lectura); //que pasa si usuario;contraseña; llega dividido en dos paquetes?
      if(usuario_!=nullptr)
      {
        memset(data_, '\0', longitud_maxima);
        hacer_escritura("bienvenido " + usuario_->nombre() + '\n');
        hacer_lectura();                               //el estatus correcto
      }

      else
      {
        cout << "usuario invalido: " << lectura << endl;
        socket_.close();
      }
    }
    else
    {
      /*y no volvemos a leer, si no, se cicla*/
      cout << "Error en inicio_sesion; error=" << ec.value() << ":" << ec.message() << endl;
      /*ec.value()==2 (End of file) típicamente significa que el otro lado cerró la conexión*/
    }
  });
}

/**Añade aquí entradas para procesar las lecturas entrantes
Cómo se paquetizan? Recuerda que TCP es un stream y no paquetes como UDP*/
void sesion::procesar_lectura()
{
  string lectura = data_;
  if(lectura.back() == 0xA) //LF, por el mensaje viene de la terminal. Este error ocurria con ncat. Causaba errores con ftp y version
    lectura.pop_back();

  if(!muting_)
    cout << lectura << '\n';

  /**Los mensajes de control no queremos retransmitirlos*/
  if(lectura.substr(0,3) == "ftp") //esta solicitud *sólo* deben hacérsela al puerto 1339 (puerto ftp)
  {
    string archivo = lectura.substr(4);
    //cout << "ftp:" << archivo << endl;
    enviar_archivo(archivo);
    memset(data_, '\0', longitud_maxima);
    return; //para evitar volver a leer. De nuevo, a este bloque lógico sólo entra la instancia que escucha en 1339
  }
  else if(lectura.substr(0,7) == "version")
  {
    //string version_cliente = lectura.substr(8);
    string version_serv = cargar_valor("version");
    //cout << "Cliente con version " << version_cliente << " solicita actualizacion" << version_serv << endl;
    hacer_escritura("version " + version_serv);
  }

  /*Problema: cómo suscribes a un ente y luego le transfieres el stream? despues de suscribirte las operaciones de control terminan*/
  else if(lectura.substr(0,7) == "ofrecer") //este socket proveerá un servicio de ahora en adelante
  {
    /* Añadimos un apuntador a este socket al mapa global */
    string nombre_servicio = lectura.substr(8);
    ofrecer(nombre_servicio);
  }

  //varios entes pueden subscribirse al mismo servicio
  else if(lectura.substr(0,9) == "suscribir") //este socket consumirá un servicio de ahora en adelante
  {
    string a_cual = lectura.substr(10);
    subscribirse(a_cual);
    //procesar_=false; //podriamos entrar a un escenario de transmision unidireccional
    /* Un socket que consume un servicio también re rutea?*/
  }
  else if(lectura.substr(0,9)=="servicios")
  {
    string s;
    for(auto it = nube::servicios.begin(); it != nube::servicios.end(); ++it)
      s += it->first +' ';

    if(s==string())
      s="Sin servicios";
    hacer_escritura(s);
  }
  else if(lectura.substr(0,5)=="debug")
  {
    nube::imprimir_mapas();
  }
  else
  {
    if(proveedor_)
    {
      /*soy un socket puente, re ruteo a los interesados*/
      re_routear_a_clientes();
    }
    if(consumidor_)
    {
      re_routear_a_proveedores();
    }
  }
}

void sesion::hacer_lectura()
{
  auto si_mismo(shared_from_this());
  socket_.async_read_some(asio::buffer(data_, longitud_maxima),
    [this, si_mismo](std::error_code ec, std::size_t longitud)
  {
    if (!ec)
    {
      sz_rx_ult_ = longitud;
      //cout << "leidos: " << longitud << "\t";
      procesar_lectura(); /* soy un socket normal, de control. Proceso la lectura y vuelvo a leer (procesar lectura lee al final)*/

      memset(data_, '\0', longitud_maxima);
      hacer_lectura(); //siempre volvemos a "escuchar"
    }
    else
    {
      /*y no volvemos a leer, si no, se cicla*/
      if(ec.value()==10054 or ec.value()==2)
        cout << usuario_->nombre() << " cerro sesion con ec " << ec.value() <<'\n';
      else
        cout << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() <<"->lectura ec=" << ec.value() << ":" << ec.message() << endl;
      /*ec.value()==2 (End of file) típicamente significa que el otro lado cerró la conexión*/
    }
  });
}

/**Para todos los suscritos a mi servicio, les re-transmito el mensaje*/
void sesion::re_routear_a_clientes()
{
  //cout << nombre_servicio_ << ":re-routeando " << data_;
  try {
    if(nube::suscritos.find(nombre_servicio_) != nube::suscritos.end())
    {
      vector<char> ult_buffer(data_,data_ + sz_rx_ult_);
      shared_ptr<vector<char>> copia = make_shared<vector<char>>(std::move(ult_buffer));

      vector<weak_ptr<sesion>>& mis_suscritos = nube::suscritos.at(nombre_servicio_);
      for(weak_ptr<sesion> wp : mis_suscritos)
      {
        shared_ptr<sesion> sp = wp.lock();
        if(sp!=nullptr)
        {
          //sp->hacer_escritura(data_);
          sp->retransmitir(copia);
        }

      }
    }
    else
    {
      cout << nombre_servicio_ <<":sin clientes suscritos\n";
    }
  }
  catch(std::out_of_range const& e) { cout << nombre_servicio_ <<":sin clientes suscritos\n"; }
  catch(std::exception const& e) { cout << nombre_servicio_ << ':' << e.what() <<"\n"; }
  catch(...) { cout << "error desconocido en re_ruteo\n"; }
}

void sesion::re_routear_a_proveedores()
{
  vector<char> ult_buffer(data_,data_ + sz_rx_ult_);
  shared_ptr<vector<char>> copia = make_shared<vector<char>>(std::move(ult_buffer));

  for(string s : suscripciones_)
    {
      try{
        if(nube::servicios.find(s) != nube::servicios.end() )
        {
          auto wp = nube::servicios.at(s); //estamos suscritos a s. obtenemos un pointer al socket que provee ese servicio
          auto sp = wp.lock();
          if(sp!=nullptr)
            //sp->hacer_escritura(data_);
            sp->retransmitir(copia);
        }
      }
      /*Puede que nadie provea el servicio al que estamos suscritos. Obtendremos este error*/
      catch(std::out_of_range const& e) { cout << s <<":nadie provee este servicio"<< '\n'; }
      catch(std::exception const& e) { cout << s << ':' << e.what() <<"\n"; }
      catch(...) { cout << "error desconocido en re_rutear_a_proveedores\n"; }
    }
}

void sesion::subscribirse(std::string a_cual)
{
  try
    {
      //obtenemos una referencia al grupo de sockets suscritos a este nombre de servicio
      if(nube::suscritos.find(a_cual) != nube::suscritos.end() )
      {
        vector<weak_ptr<sesion>>& grupo = nube::suscritos.at(a_cual);
        grupo.emplace_back(shared_from_this() );
      }
      else
      {
        cout << a_cual << ":el grupo no existe, creandolo\n";
        vector<weak_ptr<sesion>> nvo_grupo;
        nvo_grupo.emplace_back(shared_from_this());
        nube::suscritos.emplace(make_pair(a_cual, nvo_grupo));
      }

    }
    catch(::std::out_of_range const& e) //probablemente el grupo no existia (nadie se habia suscrito a ese servico)
    {
      cout << "o-o-r\n";
    }
    catch(...)
    {
      cout << "excepcion extraordinaria durante la suscripcion\n";
    }
    suscripciones_.emplace_back(a_cual);
    consumidor_ = true;
}

void sesion::ofrecer(std::string ofrecer_que)
{
    nombre_servicio_ = ofrecer_que;
    if(nube::servicios.find(ofrecer_que) != nube::servicios.end() )
      nube::servicios.at(ofrecer_que) = shared_from_this();
    else
      nube::servicios.emplace(make_pair(nombre_servicio_, shared_from_this())); //será correcto?
    proveedor_ = true; //las proximas lecturas pasaran directo a la otra branch, la de forwardeo
    if(nombre_servicio_=="desk")
      muting_ = true;
}

void sesion::retransmitir(shared_ptr<vector<char>> copia)//copia de un pointer a copias. whooou
{
  auto si_mismo(shared_from_this());
  asio::async_write(socket_, asio::buffer(*copia),
    [this, si_mismo](std::error_code ec, std::size_t bytes_escritos)
  {
    if(!ec)
    {
      //cout << "retr: " << bytes_escritos << '\n';
    }
    else
    {
      cout << "Error retrans:" << ec.value() << ":" << ec.message() << '\n';
    }
  });

}

void sesion::hacer_escritura(std::string str)
{
  str_ = str;
  auto si_mismo(shared_from_this());

  asio::async_write(socket_, asio::buffer(str_.data(), str_.size()),
    [this, si_mismo](std::error_code ec, std::size_t bytes_escritos)
  {
    if (!ec)
    {
      cout << "escr: " << bytes_escritos << '\n';
      /*procesar escritura exitosa*/
    }
    else
    {
      cout << "Error escritura:" << ec.value() << ":" << ec.message() << '\n';
    }
  });
}

void sesion::hacer_escritura_terminante(std::string str)
{
  /*Hacemos que la variable str_ de la instancia de sesion contenga una copia de los datos a recibir.
    Esto es acertado por que un lambda no debe contener referencias a datos temporales*/
  str_ = str;
  auto si_mismo(shared_from_this());
  asio::async_write(socket_, asio::buffer(str_.data(), str_.size()),
    [this, si_mismo](std::error_code ec, std::size_t /*length*/)
  {
    if (!ec)
    {
      cout << "Escritura terminante, cerrando socket "  << socket_.remote_endpoint().address().to_string()
           << ":" << socket_.remote_endpoint().port() <<'\n';
      socket_.close();
    }
    else
    {
      cout << "Error escritura:" << ec.value() << ":" << ec.message() << '\n';
    }
  });
}

void sesion::enviar_archivo(string archivo)
{
  string buf;
  std::ifstream ifs(archivo/*, std::ios::binary*/);
  if(!ifs.is_open())
  {
    cout << "longitud de string: " << archivo.size() << '\t';
    for(auto c : archivo)
      cout << c << ':' << (int)c << '\n';
    cout << "Error abriendo archivo " << archivo << strerror(errno) << '\n';
    return;
  }

  ifs.seekg(0, std::ios_base::end);
  buf.resize(ifs.tellg()); /*Reservamos la memoria*/
  ifs.seekg(0); //regresamos el iterador de ifs a su inicio
  ifs.read(&buf[0], buf.size()); /*Leemos el archivo de golpe*/
  hacer_escritura_terminante(buf);
}

//ssssssssssssssssssssssssssssssssssssssssssssssssssss

/**Acepta conexiones entrantes y las mueve a un objeto sesión. Se llama a sí misma al terminar*/
void servidor::hacer_aceptacion()
{
  acceptor_.async_accept(socket_,
    [this](std::error_code ec)
  {
    if (!ec)
    {
      asio::socket_base::keep_alive opcion(true);
      socket_.set_option(opcion);
      auto sp = std::make_shared<sesion>(std::move(socket_));
      sp->iniciar();
      //causa error cout << "conexion establecida por " << socket_.remote_endpoint() << endl;
    }
    else
    {
      cout << "Error aceptacion:" << ec.value() << ":" << ec.message() << '\n';
    }

    hacer_aceptacion();
  });
}
