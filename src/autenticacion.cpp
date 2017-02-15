#include "autenticacion.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <iostream>

using namespace std;

map<string, string> usuario::usuarios_validos = {};

void usuario::cargar_usuarios_contrasenias(const char* fname)
{
  ifstream ifs(fname);
  if(!ifs.is_open())
    throw invalid_argument("Error al abrir archivo de usuarios/contrasenias. Abortando.");
  string nombre, contrasenia;
  while(ifs >> nombre >> contrasenia)
  {
    usuario::usuarios_validos.emplace(pair<string,string>(nombre, contrasenia));
  }
}

bool usuario::autenticar_usuario(string& usuario_pswd)
{
  stringstream sss(usuario_pswd);
  string usr;
  string ctrs;
  getline(sss, usr, ';');
  getline(sss, ctrs, ';');
  if(usr.empty() or ctrs.empty())
    return false;
  auto b = usuario::usuarios_validos.find(usr);
  if(b != usuario::usuarios_validos.end()) {
    if (usuario::usuarios_validos[usr] == ctrs) {
      cout << usr << " ha iniciado sesion\n";
      /* instanciar un usuario aqui tal vez */
      return true;
    }
    else {
      cout << "usuario " << usr << " ingreso contrasenia invalida\n";
    }
  }
  else
    cout << "el usuario " << usr << " no existe\n";
  return false;
}

usuario::usuario(string& nombre)
{
  this->nombre = nombre;
}

usuario::~usuario()
{
  //dtor
}
