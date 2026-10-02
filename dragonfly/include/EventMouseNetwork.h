///
/// A "mouse" event over the network.
///

#ifndef NO_NETWORK

#ifndef __EVENT_MOUSE_NETWORK_H__
#define __EVENT_MOUSE_NETWORK_H__

// Engine includes.
#include "EventMouse.h"

namespace df {

const std::string NETWORK_MSE_EVENT = "df-mouse-network";

class EventMouseNetwork : public EventMouse {

 private:
  int m_sock;	       // Socket of connection.

 public:
  EventMouseNetwork();

  // Set socket.
  void setSocket(int new_sock);

  // Get socket.
  int getSocket() const;
};

} // end of namespace df
#endif // __EVENT_MOUSE_H_NETWORK__

#endif // NO_NETWORK

