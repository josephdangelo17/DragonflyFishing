///
/// A "keyboard" event over the network.
/// 

#ifndef NO_NETWORK

#ifndef __EVENT_KEYBOARD_NETWORK_H__
#define __EVENT_KEYBOARD_NETWORK_H__

// Engine includes.
#include "EventKeyboard.h"

namespace df {

const std::string NETWORK_KEYBOARD_EVENT = "df-keyboard-network";

class EventKeyboardNetwork : public EventKeyboard {

 private:
  int m_sock;	       // Socket of connection.

 public:
  EventKeyboardNetwork();

  // Set socket.
  void setSocket(int new_sock);

  // Get socket index.
  int getSocket() const;
};

} // end of namespace df
#endif // __EVENT_KEYBOARD_NETWORK_H__

#endif // NO_NETWORK
