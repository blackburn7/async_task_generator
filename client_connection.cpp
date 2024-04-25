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
  uint64_t keep_going = 1;

  while (keep_going) {

    // initialize client request and server response messages
    Message request;
    Message response;

    // use client fd buffer to read in req into string
    std::string encoded_message;
    int ret = rio_readlineb(&m_fdbuf, &encoded_message, Message::MAX_ENCODED_LEN);

    if (ret <= 0) {
      // handle error reading in client request
    }

    // decode message
    MessageSerialization::decode(encoded_message, request);

    if (request.get_message_type() == MessageType::LOGIN) {
      logged_in = true;
      continue;
    } else if (!logged_in) {
      // handle not logged in error
    }
    switch (request.get_message_type()) {
      case MessageType::CREATE:
        m_server->create_table(request.get_table());
        break;
      case MessageType::PUSH:
        client_stack.push(request.get_value());
        response = Message(MessageType::OK);
        break;

      case MessageType::POP:
        client_stack.pop();
        response = Message(MessageType::OK);
        break;
      case MessageType::TOP:
        // set response to data at top of stack
        response = Message(MessageType::DATA, {client_stack.get_top()});
        break;
      case MessageType::SET:
        // Handle SET request
        response = set_request_handler(request);
        break;
      case MessageType::GET:
        // Handle GET request
        response = get_request_handler(request);
        break;
      case MessageType::ADD:
        // handle ADD request
        response = add_request_handler(request);
        break;
      case MessageType::MUL:
        // handle MUL request
        response = mul_request_handler(request);
        break;
      case MessageType::SUB:
        // handle SUB request
        response = sub_request_handler(request);
        break;
      case MessageType::DIV:
        // handle DIV request
        response = div_request_handler(request);
        break;
      case MessageType::BEGIN:
        break;
      case MessageType::COMMIT:
        break;
      case MessageType::BYE:
        // exit from loop
        keep_going = 0;
        break;
    }

    // encode and return  response
    std::string encoded_response;
    MessageSerialization::encode(response, encoded_message);
    rio_writen(m_client_fd, encoded_response.c_str(), encoded_message.size());
  }




}

Message ClientConnection::get_request_handler(Message &request) {

  // find requested table
  Table *cur_table = m_server->find_table(request.get_table());

  // get value at table key
  std::string cur_val = cur_table->get(request.get_key());

  // push value onto operand stack
  client_stack.push(cur_val);

  return Message(MessageType::OK);
}

Message ClientConnection::set_request_handler(Message &request) {

  // find requested table
  Table *cur_table = m_server->find_table(request.get_table());
 
  // get value at top of stack
  std::string popped_val = client_stack.get_top();
  client_stack.pop();

  // set popped value to requested key
  cur_table->set(request.get_key(), popped_val);

  return Message(MessageType::OK);

}

Message ClientConnection::add_request_handler(Message &request) {

  // get top two values from stack
  int64_t value_1; int64_t value_2;
  top_two_vals_stack(value_1, value_2);

  // add them together and push back onto stack
  client_stack.push(std::to_string(value_1 + value_2));
  return Message(MessageType::OK);

}

Message ClientConnection::mul_request_handler(Message &request) {

  // get top two values from stack
  int64_t value_1; int64_t value_2;
  top_two_vals_stack(value_1, value_2);

  // multiply them together and push back onto stack
  client_stack.push(std::to_string(value_1 * value_2));

  return Message(MessageType::OK);

}

Message ClientConnection::sub_request_handler(Message &request) {
  // get top two values from stack
  int64_t value_1; int64_t value_2;
  top_two_vals_stack(value_1, value_2);

  // subtract them together and push back onto stack
  client_stack.push(std::to_string(value_2 - value_1));

  return Message(MessageType::OK);

}

Message ClientConnection::div_request_handler(Message &request) {
  // get top two values from stack
  int64_t value_1; int64_t value_2;
  top_two_vals_stack(value_1, value_2);

  // divide them together and push back onto stack
  client_stack.push(std::to_string(value_2 / value_1));

  return Message(MessageType::OK);

}


void ClientConnection::top_two_vals_stack(int64_t &val1, int64_t &val2) {
  // NEED ERROR HANDLING FOR POPPING
  val1 = std::stoi(client_stack.get_top());
  client_stack.pop();
  val2 = std::stoi(client_stack.get_top());
  client_stack.pop();
}