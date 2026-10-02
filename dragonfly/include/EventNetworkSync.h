//
// EventNetworkSync.h
//
// Generated when an Object has been SYNCED in response
// to an EventNetwork message.
//

#ifndef NO_NETWORK

#ifndef __EVENT_NETWORK_SYNC_H__
#define __EVENT_NETWORK_SYNC_H__

// Engine includes.
#include "EventNetwork.h"
#include "Object.h"

namespace df {

const std::string NETWORK_SYNC_EVENT = "df-network-sync";

class EventNetworkSync : public EventNetwork {

 private:
  Object *m_p_o;		// Object synchronized.
  
 public:
  // Constructor must provide Object.
  EventNetworkSync(NetworkEventLabel label, Object *p_o);

  // Set Object.
  void setObject(Object *p_new_o);

  // Get Object.
  Object *getObject() const;
};

} // end of namespace df

#endif // __EVENT_NETWORK_SYNC_H__

#endif // NO_NETWORK
