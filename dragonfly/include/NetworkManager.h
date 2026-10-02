// 
// NetworkManager.h
// 
// Manage network connections to/from engine. 
// 

#ifndef NO_NETWORK

#ifndef __NETWORK_MANAGER_H__
#define __NETWORK_MANAGER_H__

// System includes.
#include <queue>
#include <string>
#include <vector>

// Engine includes.
#include "Manager.h"

// Two-letter acronym for easier access to manager.
#define NM df::NetworkManager::getInstance()

namespace df {

const std::string DRAGONFLY_PORT = "9876";

// For delayed message sending.
using delayedMessage = std::tuple<int, char*, size_t>;
using delayQueue = std::queue<delayedMessage>;

// Generate data events, when appropriate.  If complete network
// message, (first int is message size, in bytes) generate
// EventNetwork (DATA).  Return 1 if generated event, 0 if no complete
// message, -1 if error.
int genDataEvents(int sock=0);

class NetworkManager : public Manager {

 private:
  NetworkManager();                       // Private since a singleton.
  NetworkManager(NetworkManager const&);  // Don't allow copy.
  void operator=(NetworkManager const&);  // Don't allow assignment.
  int m_accept_sock;	                  // int for accept connections.
  int m_max_sock;                         // Limit of connected sockets.
  std::vector<int> m_sock;		  // Connected network sockets.
  std::vector<int> m_delay;		  // Delay (in ticks) for each socket.
  std::vector<delayQueue> m_delay_q;      // Queue of delayed messages.
  
public:

  /// Get the one and only instance of the NetworkManager.
  static NetworkManager &getInstance();

  /// Start up NetworkManager.
  int startUp() override;

  /// Shut down NetworkManager.
  void shutDown() override;

  /// Return true only for network events.
  /// Return false for other engine events.
  bool isValid(std::string event_type) const override;

  /// Setup NetworkManager as server (if false, reset to client).
  /// Return 0 if ok, else -1.
  int setServer(bool server=true, std::string port = DRAGONFLY_PORT);

  /// Return true if successufully setup as server, else false.
  bool isServer() const;

  /// Accept network connection.
  /// If successful, generate EventNetwork (accept).
  /// Return socket if new connection.
  /// Return -1 if no new connection, but no error.
  /// Return -2 if error.
  int accept();

  /// Make network connection to host at port.
  /// If successful, generate EventNetwork (connect).
  /// Return return socket if success, -1 if error.
  int connect(std::string host, std::string port = DRAGONFLY_PORT);

  /// Set maximum number of connections.
  void setMaxConnections(int new_max_sock);

  /// Get maximum number of connections.
  int getMaxConnections() const;

  /// Get number of connected sockets.
  int getNumConnections() const;

  /// Send bytes from buffer to connected network socket (-1 for all).
  /// Return 0 if success, -1 if error.
  int send(const void *buffer, int bytes, int sock = -1);

  /// Receive from connected network socket (no more than bytes).
  /// If leave is true, don't remove data from socket (peek).
  /// Return number of bytes received, -1 if error.
  int receive(void *buffer, int bytes, bool leave, int sock = 0);

  /// Close network connection on indicated socket (-1 for all).
  /// If successful, generate EventNetwork (close).
  /// Return 0 if success, else -1.
  int close(int sock = -1);

  /// Return OS socket at indicated index. -1 if out of range.
  int getSocket(int index = 0) const;

  /// Check if socket is connected.
  /// Default (-1) checks if any socket is connected.
  /// Return true if (any) socket is connected, else false.
  bool isConnected(int sock = -1) const;

  /// Check if network data on indicated socket.
  /// Return amount of data (0 if no data), -1 if not connected or error.
  int isData(int sock = 0) const;
 
  /// Set delay amount added to outgoing messages (in ticks).
  /// Return 0 if ok, else -1.
  int setDelay(int new_delay, int sock = 0);

  ////////////////////////////////////////////////////
  // Helper methods for adding delay to outgoing messages.

  // Send any delayed messages that have expired.
  // Return 0 if success, -1 if error.
  int sendDelayed();

  // Send bytes from buffer to connected network socket
  // immediately, without delay added via setDelay().
  // Return 0 if success, -1 if error.
  int sendNow(const void *buffer, int bytes, int sock = 0);
  ////////////////////////////////////////////////////

};

} // end of namespace df

#endif // __NETWORK_MANAGER_H__

#endif // NO_NETWORK
