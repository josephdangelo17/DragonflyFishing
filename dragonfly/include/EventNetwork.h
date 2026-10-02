//
// A "network data" event, generated when a network packet arrives.
//

#ifndef NO_NETWORK

#ifndef __EVENT_NETWORK_H__
#define __EVENT_NETWORK_H__

// System includes.
#include <string>

// Engine includes.
#include "Event.h"

namespace df {

const std::string NETWORK_EVENT = "df-network";

enum class NetworkEventLabel {
  UNDEFINED = -1,
  ACCEPT,
  CONNECT,
  CLOSE,
  DATA,
};  

class EventNetwork : public Event {

 private:
  EventNetwork();              // Must provide type in constructor.
  NetworkEventLabel m_label;   // Label, depending upon type.
  int m_sock;		       // Socket of connection.
  int m_bytes;		       // Total number of bytes in message.
  void *m_p_buff;	       // Message bytes.
  
 public:
  // Constructor must provide label.  
  EventNetwork(NetworkEventLabel label);

  // Set label.
  void setLabel(NetworkEventLabel new_label);

  // Get label.
  NetworkEventLabel getLabel() const;

  // Set socket.
  void setSocket(int new_sock);

  // Get socket.
  int getSocket() const;

  // Set number of bytes in message.
  void setBytes(int new_bytes);

  // Get number of bytes in message.
  int getBytes() const;

  // Set message.
  void setMessage(void *p_new_buff);

  // Get message.
  void *getMessage() const;
};

} // end of namespace df

#endif // __EVENT_NETWORK_H__

#endif // NO_NETWORK
