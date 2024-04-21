#include <iostream>
#include <string>
#include <cstdlib>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

int main(int argc, char **argv)
{
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

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

    // Send PUSH request
    Message pushReq(MessageType::PUSH, {value});
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
