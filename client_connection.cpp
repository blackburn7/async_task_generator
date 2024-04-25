#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection() {

}

void ClientConnection::chat_with_client() {

  // initialize client request and server response messages
  Message request;

  // use client fd buffer to read in req into string
  std::string encoded_message;
  int ret = rio_readlineb(&m_fdbuf, &encoded_message, Message::MAX_ENCODED_LEN);

  if (ret <= 0) {
    // handle error reading in client request
  }

  // decode message
  MessageSerialization::decode(encoded_message, request);




  switch (request.get_message_type()) {
    case MessageType::LOGIN:
      // Handle LOGIN request
      logged_in = true;
      break;
    default:
      // ensure user is logged in
      if (!logged_in) {
        // handle user not logged in error
      }
    case MessageType::CREATE:
      m_server->create_table(request.get_table());
      break;
    case MessageType::PUSH:
      client_stack.push(request.get_value());
      break;

    case MessageType::POP:
      client_stack.pop();
      break;
    case MessageType::TOP:
      // set response to data at top of stack
      Message response(MessageType::DATA, {client_stack.get_top()});
      break;
    case MessageType::SET:
      // Handle SET request
      set_request_handler(request);
      break;
    case MessageType::GET:
      // Handle GET request
      get_request_handler(request);
      break;
    case MessageType::ADD:
      // handle ADD request

  // Handle other message types similarly
  // ...

  }




  // encode and return  response
  std::string encoded_response;
  MessageSerialization::encode();
  rio_writen(m_client_fd, encoded_response.c_str(), encoded_message.size());





}

void ClientConnection::push_request_handler(Message &request) {
  
}

void ClientConnection::get_request_handler(Message &request) {

  // find requested table
  Table *cur_table = m_server->find_table(request.get_table());

  // get value at table key
  std::string cur_val = cur_table->get(request.get_key());

  // push value onto operand stack
  client_stack.push(cur_val);
}

void ClientConnection::set_request_handler(Message &request) {

  // find requested table
  Table *cur_table = m_server->find_table(request.get_table());
 
  // get value at top of stack
  std::string popped_val = client_stack.get_top();
  client_stack.pop();

  // set popped value to requested key
  cur_table->set(request.get_key(), popped_val);
}