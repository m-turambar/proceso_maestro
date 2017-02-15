#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <string>
#include <map>

class usuario
{
  public:
    usuario(std::string& nombre);
    virtual ~usuario();
    static void cargar_usuarios_contrasenias(const char* fname);
    static bool autenticar_usuario(std::string& usuario_pswd);

  protected:

  private:
    static std::map<std::string, std::string> usuarios_validos;
    std::string nombre{};
    std::string constrasenia{};
};

#endif // AUTENTICACION_H
