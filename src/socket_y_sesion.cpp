#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <future>
#include <thread>
#include <iterator>
#include "socket_y_sesion.h"

void sesion::iniciar()
{
  cout << socket_.remote_endpoint().address().to_string() << ":" << socket_.remote_endpoint().port() << '\n';
  hacer_lectura();
}

/**Añade aquí entradas para procesar las lecturas entrantes
Cómo se paquetizan? Recuerda que TCP es un stream y no paquetes como UDP*/
void sesion::procesar_lectura()
{
  string lectura = data_;
  if(lectura.substr(0,3) == "ftp")
  {
    string archivo = lectura.substr(4);
    cout << "ftp:" << archivo << endl;
    std::thread hilo(&sesion::enviar_archivo, this, archivo); //no preguntes
    hilo.detach();
  }
  else
  {
    cout << data_ << '\n';
  }
  memset(data_, '\0', longitud_maxima);

}

void sesion::enviar_archivo(string archivo)
{
  string buf;
  std::ifstream ifs(archivo, std::ios::binary);
  if(!ifs)
  {
    //no se pudo abrir archivo. Posiblemente notificar al cliente
    cout << "Error abriendo archivo:" << archivo << '\n';
    return;
  }
  ifs.seekg(0, std::ios_base::end);

  /*Reservamos la memoria*/
  buf.resize(ifs.tellg());

  ifs.seekg(0); //regresamos el iterador de ifs a su inicio

  /*Leemos el archivo de golpe*/
  ifs.read(&buf[0], buf.size());

  try
  {
    asio::io_service iosvc2;
    asio::ip::tcp::socket sk(iosvc2);

    //conectar al cliente que inició esta petición de archivo a su puerto 1339
    cout << "conectando a " << socket_.remote_endpoint().address().to_string()
       << ":" << puerto_remoto_ftp_ << " para transferir " << archivo << " con " << buf.size() << " bytes\n";
    asio::ip::tcp::endpoint endpoint(socket_.remote_endpoint().address(), puerto_remoto_ftp_); //arriesgado llamer a socket_
    sk.connect(endpoint);
    asio::write(sk, asio::buffer(buf.data(), buf.size()));
    sk.close();
    cout << "transferencia finalizada" << endl;
  }
  catch (const std::exception &exc)
  {
    // catch anything thrown within try block that derives from std::exception
    std::cerr << "Error enviando archivo: " << exc.what();
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
      procesar_lectura();
      /*procesar lectura y responder si es necesario*/
      hacer_lectura();
    }
    else
    {
      /*y no volvemos a leer, si no, se cicla*/
      cout << "lectura ec=" << ec.value() << ":" << ec.message() << endl;
    }

  });
}

void sesion::hacer_escritura(std::string str)
{
  str_ = str;
  auto si_mismo(shared_from_this());
  asio::async_write(socket_, asio::buffer(str_.data(), str_.size()),
    [this, si_mismo](std::error_code ec, std::size_t /*length*/)
  {
    if (!ec)
    {
      /*procesar escritura exitosa*/
    }
  });
}

//ssssssssssssssssssssssssssssssssssssssssssssssssssss

void servidor::hacer_aceptacion()
{
  acceptor_.async_accept(socket_,
    [this](std::error_code ec)
  {
    if (!ec)
    {
      auto sp = std::make_shared<sesion>(std::move(socket_));
      sp->iniciar();
      //causa error cout << "conexion establecida por " << socket_.remote_endpoint() << endl;
    }

    hacer_aceptacion();
  });
}
