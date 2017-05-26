#include "nube.h"

using namespace std;

map<string, weak_ptr<sesion> > nube::servicios;

multimap<string, weak_ptr<sesion> > nube::suscritos;
