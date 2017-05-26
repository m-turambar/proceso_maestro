#ifndef NUBE_H
#define NUBE_H

#include "socket_y_sesion.h"
#include <map>
#include <memory>


namespace nube
{
  /**Solo puede haber un servicio con el mismo nombre.
  weak_ptr se usa porque el dueño de la sesion es la propia sesion. si esta se cierra estas referencias son invalidas*/
  extern std::map<std::string, std::weak_ptr<sesion> > servicios;

  /**pueden haber varios sockets suscritos al mismo servicio, como consumidores*/
  extern std::multimap<std::string, std::weak_ptr<sesion> > suscritos;
}

#endif // NUBE_H
