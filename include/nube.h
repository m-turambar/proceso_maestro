#ifndef NUBE_H
#define NUBE_H

#include "socket_y_sesion.h"
#include <map>
#include <memory>
#include <set>

/**La idea es que varios servicios en la nube estén suscritos a este proceso.
La dificultad yace en los protocolos de comunicación que existirán entre ellos.
Una vez que tengo re-ruteo, pierdo control.
Una vez que pierdo control.
Una vez que consumo un servicio, como lo dejo de consumir? Cierro el socket? Parece ser lo más efectivo*/

namespace nube
{
  /**Solo puede haber un servicio con el mismo nombre.
  weak_ptr se usa porque el dueño de la sesion es la propia sesion. si esta se cierra estas referencias son invalidas*/
  extern std::map<std::string, std::weak_ptr<sesion> > servicios;

  /**pueden haber varios sockets suscritos al mismo servicio, como consumidores*/
  extern std::map<std::string, std::set<std::weak_ptr<sesion> > > suscritos;
}

#endif // NUBE_H
