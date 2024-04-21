#include <iostream>
#include <string>
#include <cstdlib>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

using namespace std;

int main(int argc, char **argv) {
  if (argc != 6) {
      cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
      return 1;
  }

  string hostname = argv[1];
  string port = argv[2];
  string username = argv[3];
  string table = argv[4];
  string key = argv[5];

  try {
      int clientfd = open_clientfd(hostname.c_str(), port.c_str());
      rio_t rio;
      rio_readinitb(&rio, clientfd);

      // Send LOGIN request
      Message loginReq(MessageType::LOGIN, {username});
      string loginMsg;
      MessageSerialization::encode(loginReq, loginMsg);
      rio_writen(clientfd, loginMsg.c_str(), loginMsg.length());

      // Receive LOGIN response
      Message loginResp;
      rio_readlineb(&rio, &loginResp, sizeof(loginResp));
      if (loginResp.get_message_type() != MessageType::OK) {
          throw runtime_error("Failed to log in");
      }

      // Send GET request
      Message getReq(MessageType::GET, {table, key});
      string getMsg;
      MessageSerialization::encode(getReq, getMsg);
      rio_writen(clientfd, getMsg.c_str(), getMsg.length());

      // Receive GET response
      Message getResp;
      rio_readlineb(&rio, getResp, size_of(getResp));
      if (getResp.get_message_type() != MessageType::OK) {
          throw runtime_error("Failed to get value");
      }

      // Send TOP request
      Message topReq(MessageType::TOP, {});
      string topMsg;
      MessageSerialization::encode(topReq, topMsg);
      rio_writen(clientfd, topMsg.c_str(), topMsg.length());

      // Receive TOP response
      MessageType topResp;
      rio_readlineb(&rio, topResp, size_of(topResp));
      if (topResp.get_messge_type() != MessageType::DATA) {
          throw runtime_error("Failed to get top value");
      }

      // Print the retrieved value
      cout << topResp.get_value() << "\n";

      // Send BYE request
      Message byeReq(MessageType::BYE, {});
      string byeMsg;
      MessageSerialization::encode(byeReq, byeMsg);
      rio_writen(clientfd, byeMsg.c_str(), byeMsg.length());

      // Receive BYE response
      Message byeResp; 
      rio_readlineb(&rio, byeResp, size_of(byeResp));
      if (byeResp.get_message_type() != MessageType::OK) {
          throw runtime_error("Failed to end connection");
      }

      close(clientfd);
      return 0;
  }
  catch (exception &e) {
      cerr << "Error: " << e.what() << "\n";
      return 1;
  }
}