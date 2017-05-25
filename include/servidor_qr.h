#ifndef SERVIDOR_QR_H
#define SERVIDOR_QR_H


#include "asio.hpp"
#include "estructuras.h"
#include "autenticacion.h"

using asio::ip::tcp;
using namespace std;

class sesion_qr
  : public enable_shared_from_this<sesion>
{
public:
  sesion_qr(tcp::socket socket)
    : socket_(std::move(socket)),
      usuario_(nullptr)
  {
  }

  void iniciar();

private:
  void iniciar_sesion_usuario();
  void hacer_lectura();
  void hacer_escritura(std::string str);
  void hacer_escritura_terminante(std::string str);
  void procesar_lectura();
  void enviar_archivo(string archivo);

  tcp::socket socket_;
  enum {longitud_maxima = 4096};
  char data_[longitud_maxima];
  std::string str_;
  std::unique_ptr<usuario> usuario_;
};

class servidor_qr
{
public:
  servidor_qr(asio::io_service& io_service, short puerto)
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

#endif // SERVIDOR_QR_H
