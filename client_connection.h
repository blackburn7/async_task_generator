#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"
#include "value_stack.h"

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;  // server
  int m_client_fd;   // client file descriptor
  rio_t m_fdbuf;     // client file descriptor buffer
  bool logged_in;    // client login status
  ValueStack client_stack;  // client stack
  
  
  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  // request command handlers
  void push_request_handler(Message &request);
  void set_request_handler(Message &request);
  void get_request_handler(Message &request);
  void add_request_handler(Message &request);
  

  


};

#endif // CLIENT_CONNECTION_H
