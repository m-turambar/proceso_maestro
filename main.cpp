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

int main(int argc, char* argv[])
{
  try
  {
    asio::io_service io_service;
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

