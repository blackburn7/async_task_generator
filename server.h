#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  // member variables
  int ssock_fd;
  int keep_going;
  std::map <std::string, Table*> server_tables; 
  pthread_mutex_t m_num_active_clients_mutex;
  int num_active_clients;



  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );
  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );

  void stop() {
    keep_going = 0;
  };

  void create_table( const std::string &name );

  Table *find_table( const std::string &name );

};


#endif // SERVER_H
