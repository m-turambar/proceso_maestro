#ifndef SOCKET_Y_SESION_H
#define SOCKET_Y_SESION_H

#include "asio.hpp"
#include "estructuras.h"

using asio::ip::tcp;
using namespace std;

class sesion
  : public enable_shared_from_this<sesion>
{
public:
  sesion(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  void iniciar();

private:
  void hacer_lectura();
  void hacer_escritura(std::string str);
  void procesar_lectura();
  void enviar_archivo(string archivo);

  tcp::socket socket_;
  enum {longitud_maxima = 4096};
  const int puerto_remoto_ftp_ = 1339;
  char data_[longitud_maxima];
  std::string str_;
};

class servidor
{
public:
  servidor(asio::io_service& io_service, short puerto)
    : acceptor_(io_service, tcp::endpoint(tcp::v4(), puerto)),
      socket_(io_service)
  {
    hacer_aceptacion();
  }

private:
  void hacer_aceptacion();
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

#endif // SOCKET_Y_SESION_H
