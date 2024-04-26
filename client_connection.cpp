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
  , response(Message(MessageType::NONE))
  , transaction_mode(false)
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection() {

}

void ClientConnection::chat_with_client() {
  // std::cout << "1" <<  std::endl;

  uint64_t keep_going = 1;

  while (keep_going) {

    // initialize client request and server response messages
    Message request;
    // std::cout << "2" <<  std::endl;

    // use client fd buffer to read in req into string
    char buf[Message::MAX_ENCODED_LEN+1];
    int ret = rio_readlineb(&m_fdbuf, buf, sizeof(buf));
    if (ret <= 0) {
      std::cout << "error reading" << std::endl;
      // handle error reading in client request
    }
    // std::cout << "3" <<  std::endl;
    // convert buf to string and decode
    std::string encoded_message(buf);
    MessageSerialization::decode(encoded_message, request);
    // std::cout << "4" <<  std::endl;
    if (!request.is_valid()) {
      response = Message(MessageType::ERROR, {"Error:invalid message sent"});
      return_response_to_client(response);
      return;
    }


    // std::cout << "5" <<  std::endl;
    if (request.get_message_type() == MessageType::LOGIN) {
      logged_in = true;
      response = Message(MessageType::OK);
      return_response_to_client(response);
      continue;
    } else if (!logged_in) {
      // handle not logged in error
    }
    // std::cout << "6" <<  std::endl;

    switch (request.get_message_type()) {
      case MessageType::CREATE:
        m_server->create_table(request.get_table());
        response = Message(MessageType::OK);
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
        set_request_handler(request);
        break;
      case MessageType::GET:
        // Handle GET request
        get_request_handler(request);
        break;
      case MessageType::ADD:
        // handle ADD request
        add_request_handler(request);
        break;
      case MessageType::MUL:
        // handle MUL request
        mul_request_handler(request);
        break;
      case MessageType::SUB:
        // handle SUB request
        sub_request_handler(request);
        break;
      case MessageType::DIV:
        // handle DIV request
        div_request_handler(request);
        break;
      case MessageType::BEGIN:
        response = Message(MessageType::OK);
        transaction_mode = true;
        break;
      case MessageType::COMMIT:
        response = Message(MessageType::OK);
        transaction_mode = false;
        break;
      case MessageType::BYE:
        // exit from loop
        response = Message(MessageType::OK);
        keep_going = 0;
        break;
      default:
        response = Message(MessageType::ERROR, {"Error:invalid message sent"});
        return_response_to_client(response);
        return;
    }

    // encode and return  response
    return_response_to_client(response);
  }
  close(m_client_fd);

}

void ClientConnection::get_request_handler(Message &request) {

  // find requested table
  Table *cur_table = m_server->find_table(request.get_table());


  if (!transaction_mode) {
    cur_table->lock();
  } else {
    handle_transaction_locking(cur_table);
  }


  // get value at table key
  std::string cur_val = cur_table->get(request.get_key());

  // push value onto operand stack
  client_stack.push(cur_val);

  response = Message(MessageType::OK);
}

void ClientConnection::set_request_handler(Message &request) {

  // find requested table
  Table *cur_table = m_server->find_table(request.get_table());
 
  // get value at top of stack
  std::string popped_val = client_stack.get_top();
  client_stack.pop();

  // set popped value to requested key
  cur_table->set(request.get_key(), popped_val);

  response = Message(MessageType::OK);

}

void ClientConnection::add_request_handler(Message &request) {

  // get top two values from stack
  int64_t value_1; int64_t value_2;
  response = top_two_vals_stack(value_1, value_2);

  // add them together and push back onto stack
  client_stack.push(std::to_string(value_1 + value_2));
}

void ClientConnection::mul_request_handler(Message &request) {

  // get top two values from stack
  int64_t value_1; int64_t value_2;
  response = top_two_vals_stack(value_1, value_2);

  // multiply them together and push back onto stack
  client_stack.push(std::to_string(value_1 * value_2));
}

void ClientConnection::sub_request_handler(Message &request) {
  // get top two values from stack
  int64_t value_1; int64_t value_2;
  response = top_two_vals_stack(value_1, value_2);

  // subtract them together and push back onto stack
  client_stack.push(std::to_string(value_2 - value_1));
}

void ClientConnection::div_request_handler(Message &request) {
  // get top two values from stack
  int64_t value_1; int64_t value_2;
  response = top_two_vals_stack(value_1, value_2);

  // divide them together and push back onto stack
  client_stack.push(std::to_string(value_2 / value_1));
}


Message ClientConnection::top_two_vals_stack(int64_t &val1, int64_t &val2) {
  std::string pop1; std::string pop2;
  try {
    std::cout <<"TOP BEFORE: " << client_stack.get_top() << std::endl;
    pop1 = client_stack.get_top();
    client_stack.pop();
    std::cout <<"TOP: " << client_stack.get_top() << std::endl;
  } catch (const OperationException& e) {
    std::cout << "HEY" << client_stack.is_empty() << std::endl;
    return Message(MessageType::ERROR, {"stack is empty or operands are not valid"});
  }

  try {
    std::cout << "1" <<std::endl;

    pop2 = client_stack.get_top();
    std::cout << "1" <<std::endl;
    std::cout << "TP" << client_stack.get_top() << std::endl;
    std::cout << client_stack.is_empty() << std::endl;

    client_stack.pop();
    std::cout << "1" <<std::endl;
    std::cout << client_stack.is_empty() << std::endl;

  } catch (const OperationException& e) {
        std::cout << "3" <<std::endl;

    client_stack.push(pop1);
        std::cout << "3" <<std::endl;

    return Message(MessageType::ERROR, {"stack is empty or operands are not valid"});
  }
  val1 = std::stoi(pop1);
  val2 = std::stoi(pop2);

  return Message(MessageType::OK);
}

void ClientConnection::return_response_to_client(Message &response) {
  std::string encoded_response;
  MessageSerialization::encode(response, encoded_response);
  rio_writen(m_client_fd, encoded_response.c_str(), encoded_response.size());
}

bool ClientConnection::table_is_locked(Table* table) {
    return locked_tables.find(table) != locked_tables.end();
}

void handle_transaction_locking(Table * table) {
  
}