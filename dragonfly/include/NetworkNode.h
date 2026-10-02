///
/// NetworkNode.h 
///
/// Handle incoming network messages.
///

#ifndef NO_NETWORK
#ifndef NETWORK_LIGHT

#ifndef __NETWORK_NODE_H__
#define __NETWORK_NODE_H__

// Engine includes.
#include "EventKeyboard.h"
#include "EventMouse.h"
#include "EventNetwork.h"
#include "Object.h"

namespace df {

// Network message types, used with DATA messages.
enum class MessageType {
  UNDEFINED_MESSAGE=-1,
  SYNC_OBJECT,    // Create or update object, as appropriate.
  DELETE_OBJECT,  // Delete object.
  GAME_OVER,      // Indicate game over.
  KEYBOARD_INPUT, // Keyboard event.
  MOUSE_INPUT,    // Mouse event.
  CUSTOM_MESSAGE, // Game code defined.
};

// HEADER:
// 0) Total size is an int (total number of bytes in message).
// 1) Message type is an int (an enum).

// SYNC_OBJECT:
// 2) Object id as int.
// 3) String length as int.
// 4) Object type as string.
// 5) Serialized Object data (string of bytes).

// DELETE_OBJECT:
// 2) Object id as int.

// SET_GAME_OVER:
// (nothing)

// KEYBOARD:
// 2) Keyboard action as int (a EventKeyboardAction enum).
// 3) Key as int (a Keyboard::Key enum).

// MOUSE:
// 2) Mouse action as int (a EventMouseAction enum).
// 3) Mouse button as int (a Mouse::Button enum).
// 4) Mouse-x as float.
// 5) Mouse-y as float.

// CUSTOM:
// 2) Bytes as blob.

// Convert MessageType to string, returning string.
std::string toString(MessageType message);
  
class NetworkNode : public Object {

 protected:
  char *m_p_buff;   // Data buffer for network communication.
  int m_buff_size;  // Number of bytes allocated.
  
 public:
  NetworkNode();
  ~NetworkNode();

  /// Handle network and step events.
  /// Step events: call genDataEvents().
  /// Network events: if type DATA, call handleData().
  /// Return 1 if handled, else 0 if ignored.
  virtual int eventHandler(const Event *p_e) override;

  /// Handle and generate events based on type.
  /// GAME_OVER: generate game over network event.
  /// DELETE: markForDelete() and generate delete network event.
  /// SYNC: create if needed, then deserialize and generate sync network event.
  /// KEYBOARD: generate send keyboard network event.
  /// MOUSE: generate send mouse network event.
  /// CUSTOM: generate custom network event.
  /// Return 1 if handled, else 0 if ignored.
  virtual int handleData(const EventNetwork *p_e);

  /// Handle Network ACCEPT events.
  /// Default does nothing, returns 1.
  virtual int handleAccept(const df::EventNetwork *p_e);

  /// Handle Network CONNECT events.
  /// Default does nothing, returns 1.
  virtual int handleConnect(const df::EventNetwork *p_e);

  /// Handle Network CLOSE events.
  /// Default does nothing, returns 1.
  virtual int handleClose(const df::EventNetwork *p_e);

  ////////////////////////////////////////////////////////////
  /// Send message (supporting various message types).

  /// Send message from Server to Client.
  /// SET_GAME_OVER
  /// Return 1 if something sent, 0 if nothing sent, -1 if error.
  int sendMessage(MessageType msg_type, int sock=-1);

  /// Send message from Server to Client.
  /// SYNC_OBJECT or DELETE_OBJECT
  /// Synchronize attr, passed to serialize() (default is sync all).
  /// Return 1 if something sent, 0 if nothing sent, -1 if error.
  int sendMessage(MessageType msg_type, Object *p_obj,
		  unsigned int attr=0, int sock=-1);

  /// Send message from Client to Server.
  /// KEYBOARD_INPUT
  /// Return 1 if something sent, 0 if nothing sent, -1 if error.
  int sendMessage(MessageType msg_type, EventKeyboardAction action,
		  Keyboard::Key key, int sock=-1);

  /// Send message from Client to Server.
  /// MOUSE_INPUT
  /// Return 1 if something sent, 0 if nothing sent, -1 if error.
  int sendMessage(MessageType msg_type, EventMouseAction action,
		  Mouse::Button button, Vector mouse_position,
		  int sock=-1);

  /// Send message from either Client to Server or Server to Client.
  /// CUSTOM_MESSAGE
  /// Return 1 if something sent, 0 if nothing sent, -1 if error.
  int sendMessage(MessageType msg_type, int num_bytes, const void *bytes, 
		  int sock=-1);

  ////////////////////////////////////////////////////////////

  /// Create (new) Object of given type.
  /// Return pointer to Object (NULL if creation failed).
  virtual Object *createObject(std::string object_type);
  
 private:

  /// Increase size of network message buffer (if needed).
  /// Return 0 if ok, else -1 if error.
  int sizeBuffer(int msg_size);

  /// Prepare message buffer and header.
  /// Return 0 if ok, else -1 if error.
  int prepMessageHeader(MessageType msg_type, int msg_size);
};

} // end of namespace df

#endif // __NETWORK_NODE_H__

#endif // NETWORK_LIGHT
#endif // NO_NETWORK 
