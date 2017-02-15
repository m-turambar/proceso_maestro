#include "autenticacion.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

std::vector<std::pair<std::string, std::string>> usuario::usuarios_validos = {};

void usuario::cargar_usuarios_contrasenias(const char* fname)
{
  ifstream ifs(fname);
  if(!ifs.is_open())
    throw invalid_argument("Error al abrir archivo de usuarios/contrasenias. Abortando.");
  string nombre, contrasenia;
  while(ifs >> nombre >> contrasenia)
  {
    usuario::usuarios_validos.emplace_back(pair<string,string>(nombre, contrasenia));
  }
}

usuario::usuario(string& nombre)
{
  this->nombre = nombre;
}

usuario::~usuario()
{
  //dtor
}
