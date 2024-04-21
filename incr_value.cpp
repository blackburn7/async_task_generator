#include <iostream>
#include <string>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

int main(int argc, char* argv[]) {
  bool useTransaction = false;
  int argIndex = 1;

  if (argc == 7 && std::string(argv[1]) == "-t") {
    useTransaction = true;
    argIndex = 2;
  } else if (argc != 6) {
    std::cerr << "Usage: ./incr_value [-t] hostname port username table key" << std::endl;
    return 1;
  }

  std::string hostname = argv[argIndex];
  int port = std::stoi(argv[argIndex + 1]);
  std::string username = argv[argIndex + 2];
  std::string table = argv[argIndex + 3];
  std::string key = argv[argIndex + 4];

  int clientfd = Open_clientfd(hostname.c_str(), std::to_string(port).c_str());

  rio_t rio;
  Rio_readinitb(&rio, clientfd);

  Message loginMsg(MessageType::LOGIN, {username});
  std::string serializedLoginMsg;
  MessageSerialization::encode(loginMsg, serializedLoginMsg);
  Rio_writen(clientfd, serializedLoginMsg.c_str(), serializedLoginMsg.length());

  Message response;
  std::string responseStr;
  Rio_readlineb(&rio, responseStr.data(), responseStr.max_size());

  if (responseStr.empty()) {
    std::cerr << "Error: Received an empty response from the server" << std::endl;
    Close(clientfd);
    return 1;
  }

  MessageSerialization::decode(responseStr, response);

  if (response.get_message_type() != MessageType::OK) {
    std::cerr << "Error: " << response.get_quoted_text() << std::endl;
    Close(clientfd);
    return 1;
  }

  bool transactionSuccess = false;
  while (!transactionSuccess) {
    if (useTransaction) {
      Message beginMsg(MessageType::BEGIN);
      std::string serializedBeginMsg;
      MessageSerialization::encode(beginMsg, serializedBeginMsg);
      Rio_writen(clientfd, serializedBeginMsg.c_str(), serializedBeginMsg.length());

      responseStr.clear();
      Rio_readlineb(&rio, responseStr.data(), responseStr.max_size());

      if (responseStr.empty()) {
        continue;
      }

      MessageSerialization::decode(responseStr, response);

      if (response.get_message_type() != MessageType::OK) {
        std::cerr << "Error: " << response.get_quoted_text() << std::endl;
        Close(clientfd);
        return 1;
      }
    }

    Message getMsg(MessageType::GET, {table, key});
    std::string serializedGetMsg;
    MessageSerialization::encode(getMsg, serializedGetMsg);
    Rio_writen(clientfd, serializedGetMsg.c_str(), serializedGetMsg.length());

    responseStr.clear();
    Rio_readlineb(&rio, responseStr.data(), responseStr.max_size());

    if (responseStr.empty()) {
      if (useTransaction) {
        continue;
      } else {
        std::cerr << "Error: Received an empty response from the server" << std::endl;
        Close(clientfd);
        return 1;
      }
    }

    MessageSerialization::decode(responseStr, response);

    if (response.get_message_type() != MessageType::OK) {
      if (useTransaction) {
        continue;
      } else {
        std::cerr << "Error: " << response.get_quoted_text() << std::endl;
        Close(clientfd);
        return 1;
      }
    }

    int value = std::stoi(response.get_value());
    value++;
    std::string incrementedValue = std::to_string(value);

    Message setMsg(MessageType::SET, {table, key, incrementedValue});
    std::string serializedSetMsg;
    MessageSerialization::encode(setMsg, serializedSetMsg);
    Rio_writen(clientfd, serializedSetMsg.c_str(), serializedSetMsg.length());

    responseStr.clear();
    Rio_readlineb(&rio, responseStr.data(), responseStr.max_size());

    if (responseStr.empty()) {
      if (useTransaction) {
        continue;
      } else {
        std::cerr << "Error: Received an empty response from the server" << std::endl;
        Close(clientfd);
        return 1;
      }
    }

    MessageSerialization::decode(responseStr, response);

    if (response.get_message_type() != MessageType::OK) {
      if (useTransaction) {
        continue;
      } else {
        std::cerr << "Error: " << response.get_quoted_text() << std::endl;
        Close(clientfd);
        return 1;
      }
    }

    if (useTransaction) {
      Message commitMsg(MessageType::COMMIT);
      std::string serializedCommitMsg;
      MessageSerialization::encode(commitMsg, serializedCommitMsg);
      Rio_writen(clientfd, serializedCommitMsg.c_str(), serializedCommitMsg.length());

      responseStr.clear();
      Rio_readlineb(&rio, responseStr.data(), responseStr.max_size());

      if (responseStr.empty()) {
        continue;
      }

      MessageSerialization::decode(responseStr, response);

      if (response.get_message_type() != MessageType::OK) {
        continue;
      }
    }

    transactionSuccess = true;
  }

  Message byeMsg(MessageType::BYE);
  std::string serializedByeMsg;
  MessageSerialization::encode(byeMsg, serializedByeMsg);
  Rio_writen(clientfd, serializedByeMsg.c_str(), serializedByeMsg.length());

  Close(clientfd);
  return 0;
}