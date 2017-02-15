#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <string>
#include <vector>

class usuario
{
  public:
    usuario(std::string& nombre);
    virtual ~usuario();
    static void cargar_usuarios_contrasenias(const char* fname);

  protected:

  private:
    static std::vector<std::pair<std::string, std::string>> usuarios_validos;
    std::string nombre{};
    std::string constrasenia{};
};

#endif // AUTENTICACION_H
