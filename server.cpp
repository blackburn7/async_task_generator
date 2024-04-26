#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

Server::Server(): ssock_fd(), keep_going(1), num_active_clients(0) {
  pthread_mutex_init(&m_num_active_clients_mutex, nullptr);
}

Server::~Server()
{
  pthread_mutex_destroy(&m_num_active_clients_mutex);

}

void Server::listen( const std::string &port ) {


  ssock_fd = open_listenfd(port.c_str());

  if (ssock_fd < 0) {
    // handle error
    log_error("Failed to open listen socket");
    throw std::runtime_error("Failed to open listen socket");
  }

}

void Server::server_loop() {

  while (keep_going) {

    // wait for client connection
    int client_fd = accept(ssock_fd, NULL, NULL);

    // continue waiting if accept fails
    if (client_fd < 0) {
      log_error("Error accepting client connection");
      continue;
    }

    // start worker thread for connected client
    ClientConnection *client = new ClientConnection(this, client_fd);
    pthread_t thr_id;
    if (pthread_create(&thr_id, nullptr, client_worker, client) != 0) {
        log_error("Could not create client thread");
        delete client;
    } else {
      // pthread_mutex_lock(&m_num_active_clients_mutex);
      // num_active_clients++;
      // pthread_mutex_unlock(&m_num_active_clients_mutex);

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
  // detach thread from main thread

  pthread_detach( pthread_self() );

  // take argument and cast as ClientConnection pointer
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  
  // allow server to chat with client
  client->chat_with_client();

  // pthread_mutex_lock(&(static_cast<Server*>(arg)->m_num_active_clients_mutex));
  // static_cast<Server*>(arg)->num_active_clients--;
  // if (static_cast<Server*>(arg)->num_active_clients == 0) {
  //     static_cast<Server*>(arg)->keep_going = 0;
  // }
  // pthread_mutex_unlock(&(static_cast<Server*>(arg)->m_num_active_clients_mutex));



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
  auto it = server_tables.find(name);
  if (it != server_tables.end()) {
      return it->second;
  }
  return nullptr;
}

