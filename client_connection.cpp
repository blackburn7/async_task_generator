#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection(Server *server, int client_fd)
    : m_server(server), m_client_fd(client_fd), response(Message(MessageType::NONE)), transaction_mode(false) {
    rio_readinitb(&m_fdbuf, m_client_fd);
}

ClientConnection::~ClientConnection() {
    close(m_client_fd);
}

void ClientConnection::chat_with_client() {
  uint64_t keep_going = 1;

  while (keep_going) {
    Message request;

    try {
      // Read and decode client request
      char buf[Message::MAX_ENCODED_LEN + 1];
      int ret = rio_readlineb(&m_fdbuf, buf, sizeof(buf));
      if (ret <= 0) {
        throw CommException("Error reading client input");
      }

      std::string encoded_message(buf);
      MessageSerialization::decode(encoded_message, request);
      if (!request.is_valid()) {
        throw InvalidMessage("\"Invalid arguments (number and/or format)\"");
      }

      // Handle login request
      if (request.get_message_type() == MessageType::LOGIN) {
        logged_in = true;
        response = Message(MessageType::OK);
        return_response_to_client();
        continue;
      } else if (!logged_in) {
        throw InvalidMessage("User not logged in");
      }

      // Handle client requests
      switch (request.get_message_type()) {
        case MessageType::CREATE:
          create_request_handler(request);
          break;
        case MessageType::PUSH:
          push_request_handler(request);
          break;
        case MessageType::POP:
          pop_request_handler(request);
          break;
        case MessageType::TOP:
          top_request_handler(request);
          break;
        case MessageType::SET:
          set_request_handler(request);
          break;
        case MessageType::GET:
          get_request_handler(request);
          break;
        case MessageType::ADD:
          arithmetic_request_handler(request, [](int64_t a, int64_t b) { return a + b; });
          break;
        case MessageType::MUL:
          arithmetic_request_handler(request, [](int64_t a, int64_t b) { return a * b; });
          break;
        case MessageType::SUB:
          arithmetic_request_handler(request, [](int64_t a, int64_t b) { return b - a; });
          break;
        case MessageType::DIV:
          arithmetic_request_handler(request, [](int64_t a, int64_t b) { return b / a; });
          break;
        case MessageType::BEGIN:
          response = Message(MessageType::OK);
          transaction_mode = true;
          break;
        case MessageType::COMMIT:
          commit_tables();
          transaction_mode = false;
          response = Message(MessageType::OK);
          break;
        case MessageType::BYE:
          response = Message(MessageType::OK);
          keep_going = 0;
          break;
        default:
          throw InvalidMessage("Invalid message sent");
      }
    } catch (const OperationException &e) {
      if (transaction_mode) {
        throw FailedTransaction("operation failed during transaction");
      } else {
        response = Message(MessageType::FAILED, {e.what()});
      }
    } catch (const FailedTransaction &e) {
      rollback_tables();
      transaction_mode = false;
      response = Message(MessageType::FAILED, {e.what()});
    } catch (const InvalidMessage &e) {
      response = Message(MessageType::ERROR, {e.what()});
      keep_going = 0;
    } catch (const CommException &e) {
      response = Message(MessageType::ERROR, {e.what()});
      keep_going = 0;
    }
    return_response_to_client();
  }
}

void ClientConnection::create_request_handler(Message &request) {
  if (m_server->find_table(request.get_table()) != nullptr) {
    throw InvalidMessage("Table has already been created");
  }
  m_server->create_table(request.get_table());
  response = Message(MessageType::OK);
}

void ClientConnection::push_request_handler(Message &request) {
  client_stack.push(request.get_value());
  response = Message(MessageType::OK);
}

void ClientConnection::pop_request_handler(Message &request) {
  if (client_stack.is_empty()) {
    throw OperationException("Stack is empty");
  }
  client_stack.pop();
  response = Message(MessageType::OK);
}

void ClientConnection::top_request_handler(Message &request) {
  if (client_stack.is_empty()) {
    throw OperationException("Stack is empty");
  }
  response = Message(MessageType::DATA, {client_stack.get_top()});
}

void ClientConnection::get_request_handler(Message &request) {
  Table *cur_table = m_server->find_table(request.get_table());
  if (cur_table == nullptr) {
    throw InvalidMessage("Table not found");
  }

  handle_table_locking(cur_table);

  std::string cur_val = cur_table->get(request.get_key());
  client_stack.push(cur_val);

  if (!transaction_mode) {
    cur_table->commit_changes();
    cur_table->unlock();
  }

  response = Message(MessageType::OK);
}

void ClientConnection::set_request_handler(Message &request) {
    Table *cur_table = m_server->find_table(request.get_table());
    if (cur_table == nullptr) {
        throw InvalidMessage("Table not found");
    }

    handle_table_locking(cur_table);

    if (client_stack.is_empty()) {
        throw OperationException("Stack is empty");
    }
    std::string popped_val = client_stack.get_top();
    client_stack.pop();

    cur_table->set(request.get_key(), popped_val);

    if (!transaction_mode) {
        cur_table->commit_changes();
        cur_table->unlock();
    }

    response = Message(MessageType::OK);
}

template <typename ArithmeticOperation>
void ClientConnection::arithmetic_request_handler(Message &request, ArithmeticOperation operation) {
    int64_t value_1, value_2;
    if (!pop_two_values(value_1, value_2)) {
        throw OperationException("Stack is empty or operands are not valid");
    }

    int64_t result = operation(value_1, value_2);
    client_stack.push(std::to_string(result));
    response = Message(MessageType::OK);
}

bool ClientConnection::pop_two_values(int64_t &val1, int64_t &val2) {
    try {
        std::string pop1 = client_stack.get_top();
        client_stack.pop();
        std::string pop2 = client_stack.get_top();
        client_stack.pop();

        val1 = std::stoi(pop1);
        val2 = std::stoi(pop2);
    } catch (const std::exception &e) {
        return false;
    }
    return true;
}

void ClientConnection::return_response_to_client() {
    std::string encoded_response;
    MessageSerialization::encode(response, encoded_response);
    rio_writen(m_client_fd, encoded_response.c_str(), encoded_response.size());
}

bool ClientConnection::table_is_locked(Table *table) {
    return locked_tables.find(table) != locked_tables.end();
}

void ClientConnection::handle_table_locking(Table *table) {
    if (!transaction_mode) {
        table->lock();
        return;
    }

    if (table_is_locked(table)) {
        return;
    }

    if (table->trylock()) {
        locked_tables.insert(table);
    } else {
        throw FailedTransaction("Failed to acquire lock");
    }
}

void ClientConnection::rollback_tables() {
    for (Table *table : locked_tables) {
        table->rollback_changes();
        table->unlock();
    }
    locked_tables.clear();
}

void ClientConnection::commit_tables() {
    for (Table *table : locked_tables) {
        table->commit_changes();
        table->unlock();
    }
    locked_tables.clear();
}