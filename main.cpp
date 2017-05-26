#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <map>
#include <fstream>

#include "asio.hpp"
#include "estructuras.h"
#include "socket_y_sesion.h"
#include "autenticacion.h"

#define PUERTO_CONTROL 3214
#define PUERTO_FTP 3215

using asio::ip::tcp;
using namespace std;

/*PROCESO MAESTRO*/
int main(int argc, char* argv[])
{

  /*Espacio de recuperación de errores. En teoría este programa rara vez inicia.
    La lógica que aquí se diseñe repercutirá en que la recuperación sea adecuada*/
  /*if(mapa.empty())
      rellenar_mapa(algun_archivo)*/


/*
  string buf;
  string archivo = "main.cpp";
  std::ifstream ifs(archivo);
  if(!ifs.is_open())
  {
    cout << "Error abriendo archivo " << archivo << strerror(errno) << '\n';
    return 0;
  }

  ifs.seekg(0, std::ios_base::end);
  buf.resize(ifs.tellg());
  ifs.seekg(0); //regresamos el iterador de ifs a su inicio
  ifs.read(&buf[0], buf.size());

  cout << "longitud de string: " << archivo.size() << '\t';
  for(auto c : archivo)
    cout << c << ':' << (int)c << '\n';

  return 0;*/

  try
  {
    asio::io_service io_service;
    usuario::cargar_usuarios_contrasenias("usuarios_contrasenias.txt");
    servidor servidor_de_control(io_service, PUERTO_CONTROL);
    servidor servidor_ftp(io_service, PUERTO_FTP); //aunque sean idénticos, no queremos solicitudes de control a media transferencia
    cout << "pm escuchando en puertos "<< PUERTO_CONTROL << "(ctrl) y " << PUERTO_FTP << "(ftp)" << endl;

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Excepcion en main proceso maestro: " << e.what() << "\n";
  }

  return 0;
}

