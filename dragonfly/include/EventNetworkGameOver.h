//
// EventNetworkGameOver.h
//
// Generated when a SET_GAME_OVER has been acted on
// in response to an EventNetwork message.
//

#ifndef NO_NETWORK

#ifndef __EVENT_NETWORK_GAME_OVER_H__
#define __EVENT_NETWORK_GAME_OVER_H__

#include "EventNetwork.h"

namespace df {

const std::string NETWORK_GAME_OVER_EVENT = "df-network-game-over";

class EventNetworkGameOver : public EventNetwork {

 public:
  EventNetworkGameOver(NetworkEventLabel label);
};

} // end of namespace df

#endif // __EVENT_NETWORK_GAME_OVER_H__

#endif // NO_NETWORK
