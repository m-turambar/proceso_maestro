#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <future>
#include <thread>
#include <iterator>
#include "socket_y_sesion.h"

extern string cargar_valor(string);

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
    enviar_archivo(archivo);
    return; //para evitar volver a leer
  }
  else if(lectura.substr(0,7) == "version")
  {
    string version_cliente = lectura.substr(8);
    string version_serv = cargar_valor("version");
    if(version_cliente!=version_serv)
    {
      cout << "veriones diferentes\n";
      hacer_escritura("advertencia actualizar"); //advertencia + actualizar. son palabras clave
    }
  }
  else
  {
    cout << data_ << '\n';
  }
  memset(data_, '\0', longitud_maxima);
  hacer_lectura(); //siempre volvemos a "escuchar"
}

void sesion::enviar_archivo(string archivo)
{
  string buf;
  std::ifstream ifs(archivo, std::ios::binary);
  if(!ifs)
  {
    cout << "Error abriendo archivo:" << archivo << '\n';
    return;
  }

  ifs.seekg(0, std::ios_base::end);
  buf.resize(ifs.tellg()); /*Reservamos la memoria*/
  ifs.seekg(0); //regresamos el iterador de ifs a su inicio
  ifs.read(&buf[0], buf.size()); /*Leemos el archivo de golpe*/
  hacer_escritura_terminante(buf);
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
    else
    {
      cout << "Error escritura:" << ec.value() << ":" << ec.message() << '\n';
    }
  });
}

void sesion::hacer_escritura_terminante(std::string str)
{
  str_ = str;
  auto si_mismo(shared_from_this());
  asio::async_write(socket_, asio::buffer(str_.data(), str_.size()),
    [this, si_mismo](std::error_code ec, std::size_t /*length*/)
  {
    if (!ec)
    {
      socket_.close();
    }
    else
    {
      cout << "Error escritura:" << ec.value() << ":" << ec.message() << '\n';
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
