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

using asio::ip::tcp;
using namespace std;

/*PROCESO MAESTRO*/
int main(int argc, char* argv[])
{

  /*Espacio de recuperación de errores. En teoría este programa rara vez inicia.
    La lógica que aquí se diseñe repercutirá en que la recuperación sea adecuada*/
  /*if(mapa.empty())
      rellenar_mapa(algun_archivo)*/

  try
  {
    asio::io_service io_service;

    servidor s(io_service, 1337);
    servidor ftp(io_service, 1339); //aunque sean idénticos, no queremos solicitudes de control a media transferencia
    cout << "pm escuchando en 127.0.0.1::1337" << endl;

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

