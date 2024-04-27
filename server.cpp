/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

Server::Server(): ssock_fd() {
  server_tables = new std::map<std::string, Table*>();
  if (pthread_mutex_init(&tables_mutex, nullptr) != 0) {
    throw std::runtime_error("failed to initialize mutex");
  }
}

Server::~Server() {
  pthread_mutex_destroy(&tables_mutex);
  delete server_tables;
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

  while (1) {

    // wait for client connection
    int client_fd = accept(ssock_fd, NULL, NULL);

    // continue waiting if accept fails
    if (client_fd < 0) {
      log_error("Error accepting client connection");
      continue;
    }

    // start worker thread for connected client
    ClientConnection *client = new ClientConnection(this, client_fd);
    pthread_t thresad;
    if (pthread_create(&thresad, nullptr, client_worker, client) != 0) {
        log_error("Could not create client thread");
        delete client;
    }

  }

  close(ssock_fd);


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

  // clean up resources
  return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}


void Server::create_table(const std::string &name) {
  // add new server to map of servers
  pthread_mutex_lock(&tables_mutex);
  (*server_tables)[name] = new Table(name);
  pthread_mutex_unlock(&tables_mutex);
}

Table* Server::find_table(const std::string &name) {
  pthread_mutex_lock(&tables_mutex);

  // determine if can be found
  auto it = (*server_tables).find(name);

  Table* table = (it != (*server_tables).end()) ? it->second: nullptr;
  
  pthread_mutex_unlock(&tables_mutex);
  return table;
}

