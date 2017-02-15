#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <string>
#include <map>
#include <memory>

class usuario
{
  public:
    usuario(std::string& nombre, std::string& contrasenia);
    virtual ~usuario();
    static void cargar_usuarios_contrasenias(const char* fname);
    static std::unique_ptr<usuario> autenticar_usuario(std::string& usuario_pswd);
    std::string nombre() const { return nombre_; }

  protected:

  private:
    static std::map<std::string, std::string> usuarios_validos;
    std::string nombre_{};
    std::string passwd_{};
};

#endif // AUTENTICACION_H
