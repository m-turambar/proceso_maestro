#include "nube.h"

using namespace std;

/*e.g. sesion0 provee el servicio bascula1*/
map<string, weak_ptr<sesion> > nube::servicios; //1:1

/*e.g. los suscritos a bascula1 son sesion1, sesion2, sesion3*/
map<string, vector<weak_ptr<sesion> > > nube::suscritos; //1:N
