#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

Server::Server(): ssock_fd(), keep_going(1) {
  
}

Server::~Server()
{
  // TODO: implement
}

void Server::listen( const std::string &port ) {


  int ssock_fd = open_listenfd(port.c_str());
  
  if (ssock_fd < 0) {
    // handle error
  }

}

void Server::server_loop() {
  
  while (keep_going) {

    // wait for client connection
    int client_fd = accept(ssock_fd, NULL, NULL);

    // error handling
    if (client_fd < 0) {
      log_error("something"); // handle this
    }

    // start worker thread for connected client
    ClientConnection *client = new ClientConnection(this, client_fd);
    pthread_t thr_id;
    if (pthread_create(&thr_id, nullptr, client_worker, client) != 0) {
      log_error("Could not create client thread");
    }


  }

  close(ssock_fd);

  // Note that your code to start a worker thread for a newly-connected
  // client might look something like this:
}


void *Server::client_worker( void *arg )
{

  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:

  // take argument and cast as ClientConnection pointer
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  
  // allow server to chat with client
  client->chat_with_client();

  // clean up resources
  return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}


void Server::create_table(const std::string &name) {
  // add new server to map of servers
  server_tables[name] = new Table(name);
}

Table* Server::find_table(const std::string &name) {
  // return table with given name
  return server_tables[name];
}


