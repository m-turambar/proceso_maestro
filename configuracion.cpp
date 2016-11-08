#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string cargar_valor(string arg)
{
  ifstream ifs("config.txt");
  string linea, valor;
  while(ifs >> linea >> valor)
  {
    if(linea==arg)
    {
      return valor;
    }
  }
  return std::string();
}
