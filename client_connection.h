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
  Message response;  // response message to be sent to client
  bool transaction_mode; // determines whether the client is in a transaction
  std::set<Table*> locked_tables; // map of currently locked tables
  
  
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
  void mul_request_handler(Message &request);
  void sub_request_handler(Message &request);
  void div_request_handler(Message &request);


  // extra helper functions
  Message top_two_vals_stack(int64_t &val1, int64_t &val2);  // pops and returns two values from the stack
  void return_response_to_client(Message &response);         // outputs given response message back to the client
  bool table_is_locked(Table* table);                        // determines if a given table is locked
  void handle_table_locking(Table *table);                   // handles table locking for transactions and non-transactions
  void rollback_tables();                                    // unlocks and rolls back all changes if a transaction fails
  void commit_tables();                                      // unlcoks and commits all tables if a transaction succeeds
  

  


};

#endif // CLIENT_CONNECTION_H
