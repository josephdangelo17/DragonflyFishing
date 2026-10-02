//
// EventNetworkDelete.h
//
// Generated when an Object has been marked for deletion in
// response to an EventNetwork message.
//

#ifndef NO_NETWORK

#ifndef __EVENT_NETWORK_DELETE_H__
#define __EVENT_NETWORK_DELETE_H__

// Engine includes.
#include "EventNetwork.h"
#include "Object.h"

namespace df {

const std::string NETWORK_DELETE_EVENT = "df-network-delete";

class EventNetworkDelete : public EventNetwork {

 private:
  Object *m_p_o;		// Object marked for deletion.
  
 public:
  // Constructor must provide Object.
  EventNetworkDelete(NetworkEventLabel label, Object *p_o);

  // Set Object.
  void setObject(Object *p_new_o);

  // Get Object.
  Object *getObject() const;
};

} // end of namespace df

#endif // __EVENT_NETWORK_DELETE_H__

#endif // NO_NETWORK
