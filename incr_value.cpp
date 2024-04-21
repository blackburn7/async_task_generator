#include <iostream>
#include <string>
#include <cstdlib>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

  try {
    int clientfd = open_clientfd(hostname.c_str(), port.c_str());
    rio_t rio;
    rio_readinitb(&rio, clientfd);

    // Send LOGIN request
    Message loginReq(MessageType::LOGIN, {username});
    std::string loginMsg;
    MessageSerialization::encode(loginReq, loginMsg);
    rio_writen(clientfd, loginMsg.c_str(), loginMsg.length());

    // Receive LOGIN response
    char buf[Message::MAX_ENCODED_LEN + 1];
    ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
    if (n < 0) {
        throw std::runtime_error("Failed to read response");
    }
    buf[n] = '\0';
    Message loginResp;
    MessageSerialization::decode(std::string(buf), loginResp);
    if (loginResp.get_message_type() != MessageType::OK) {
        throw std::runtime_error("Failed to log in");
    }

    // Send GET request
    Message getReq(MessageType::GET, {table, key});
    std::string getMsg;
    MessageSerialization::encode(getReq, getMsg);
    rio_writen(clientfd, getMsg.c_str(), getMsg.length());

    // Receive GET response
    n = rio_readlineb(&rio, buf, sizeof(buf));
    if (n < 0) {
        throw std::runtime_error("Failed to read response");
    }
    buf[n] = '\0';
    Message getResp;
    MessageSerialization::decode(std::string(buf), getResp);
    if (getResp.get_message_type() != MessageType::OK) {
        throw std::runtime_error("Failed to get value");
    }

    // Parse the value
    int currentValue = std::stoi(getResp.get_value());

    // Increment the value
    int newValue = currentValue + 1;

    // Send PUSH request
    Message pushReq(MessageType::PUSH, {std::to_string(newValue)});
    std::string pushMsg;
    MessageSerialization::encode(pushReq, pushMsg);
    rio_writen(clientfd, pushMsg.c_str(), pushMsg.length());

    // Receive PUSH response
    n = rio_readlineb(&rio, buf, sizeof(buf));
    if (n < 0) {
        throw std::runtime_error("Failed to read response");
    }
    buf[n] = '\0';
    Message pushResp;
    MessageSerialization::decode(std::string(buf), pushResp);
    if (pushResp.get_message_type() != MessageType::OK) {
        throw std::runtime_error("Failed to push value");
    }

    // If transaction requested, send BEGIN
    if (use_transaction) {
      Message beginReq(MessageType::BEGIN, {});
      std::string beginMsg;
      MessageSerialization::encode(beginReq, beginMsg);
      rio_writen(clientfd, beginMsg.c_str(), beginMsg.length());

      // Receive BEGIN response
      n = rio_readlineb(&rio, buf, sizeof(buf));
      if (n < 0) {
        throw std::runtime_error("Failed to read response");
      }
      buf[n] = '\0';
      Message beginResp;
      MessageSerialization::decode(std::string(buf), beginResp);
      if (beginResp.get_message_type() != MessageType::OK) {
        throw std::runtime_error("Failed to begin transaction");
      }
    }

    // Send SET request
    Message setReq(MessageType::SET, {table, key});
    std::string setMsg;
    MessageSerialization::encode(setReq, setMsg);
    rio_writen(clientfd, setMsg.c_str(), setMsg.length());

    // Receive SET response
    n = rio_readlineb(&rio, buf, sizeof(buf));
    if (n < 0) {
        throw std::runtime_error("Failed to read response");
    }
    buf[n] = '\0';
    Message setResp;
    MessageSerialization::decode(std::string(buf), setResp);
    if (setResp.get_message_type() != MessageType::OK) {
        throw std::runtime_error("Failed to set value");
    }

    // If transaction requested, send COMMIT
    if (use_transaction) {
        Message commitReq(MessageType::COMMIT, {});
        std::string commitMsg;
        MessageSerialization::encode(commitReq, commitMsg);
        rio_writen(clientfd, commitMsg.c_str(), commitMsg.length());

        // Receive COMMIT response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            throw std::runtime_error("Failed to read response");
        }
        buf[n] = '\0';
        Message commitResp;
        MessageSerialization::decode(std::string(buf), commitResp);
        if (commitResp.get_message_type() != MessageType::OK) {
            throw std::runtime_error("Failed to commit transaction");
        }
    }

    // Send BYE request
    Message byeReq(MessageType::BYE, {});
    std::string byeMsg;
    MessageSerialization::encode(byeReq, byeMsg);
    rio_writen(clientfd, byeMsg.c_str(), byeMsg.length());

    // Receive BYE response
    n = rio_readlineb(&rio, buf, sizeof(buf));
    if (n < 0) {
        throw std::runtime_error("Failed to read response");
    }
    buf[n] = '\0';
    Message byeResp;
    MessageSerialization::decode(std::string(buf), byeResp);
    if (byeResp.get_message_type() != MessageType::OK) {
        throw std::runtime_error("Failed to end connection");
    }

    close(clientfd);
    return 0;
} catch (std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
}
}
