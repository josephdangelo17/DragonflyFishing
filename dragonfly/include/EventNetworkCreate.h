//
// EventNetworkCreate.h
//
// Generated when an Object has been CREATEED in response
// to an EventNetwork message.
//

#ifndef NO_NETWORK

#ifndef __EVENT_NETWORK_CREATE_H__
#define __EVENT_NETWORK_CREATE_H__

// Engine includes.
#include "EventNetwork.h"
#include "Object.h"

namespace df {

const std::string NETWORK_CREATE_EVENT = "df-network-create";

class EventNetworkCreate : public EventNetwork {

 private:
  Object *m_p_o;		// Object createhronized.
  
 public:
  // Constructor must provide Object.
  EventNetworkCreate(NetworkEventLabel label, Object *p_o);

  // Set Object.
  void setObject(Object *p_new_o);

  // Get Object.
  Object *getObject() const;
};

} // end of namespace df

#endif // __EVENT_NETWORK_CREATE_H__

#endif // NO_NETWORK
