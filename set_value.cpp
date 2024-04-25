/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

int main(int argc, char **argv)
{
  // checks to ensure number of args is correct
  if (argc != 7) {
    std::cerr << "Requored format: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
    return 1;
  }
  
  // parse values and assigned to string vars
  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

  try {
    // open client
    int clientfd = open_clientfd(hostname.c_str(), port.c_str());
    rio_t rio;
    rio_readinitb(&rio, clientfd);

    // send LOGIN request
    Message loginReq(MessageType::LOGIN, {username});
    std::string loginMsg;
    MessageSerialization::encode(loginReq, loginMsg);
    rio_writen(clientfd, loginMsg.c_str(), loginMsg.length());

    // obtain LOGIN response
    char buf[Message::MAX_ENCODED_LEN + 1];
    ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));

    // determine if read failed 
    if (n < 0) {
        // read did in fact fail
        std::cerr << "Error: Failed to read response";
        return 1;
    }

    buf[n] = '\0'; // add null termin

    Message loginResp; // LOGIN
    MessageSerialization::decode(std::string(buf), loginResp);
    if (loginResp.get_message_type() != MessageType::OK) {
        // OK was not returned, likely fail or error was returned
        std::cerr << "Error: " + loginResp.get_quoted_text();
        return 1;
    }

    // send PUSH request
    Message pushReq(MessageType::PUSH, {value});
    std::string pushMsg;
    MessageSerialization::encode(pushReq, pushMsg);
    rio_writen(clientfd, pushMsg.c_str(), pushMsg.length());

    // obtain PUSH response
    n = rio_readlineb(&rio, buf, sizeof(buf));

    // determine if read failed
    if (n < 0) {
        // read failed
        std::cerr << "Error: " + loginResp.get_quoted_text();
        return 1;
    }

    buf[n] = '\0';

    Message pushResp;
    MessageSerialization::decode(std::string(buf), pushResp);

    // determine if response signals for a failure
    if (pushResp.get_message_type() != MessageType::OK) {
        std::cerr << "Error: " + pushResp.get_quoted_text();
        return 1;
    }

    // send SET request
    Message setReq(MessageType::SET, {table, key});
    std::string setMsg;
    MessageSerialization::encode(setReq, setMsg);
    rio_writen(clientfd, setMsg.c_str(), setMsg.length());

    // obtain SET response
    n = rio_readlineb(&rio, buf, sizeof(buf));

    // determines if read failed
    if (n < 0) {
        std::cerr << "Error: Failed to read response"; // read failed
        return 1;
    }

    buf[n] = '\0';

    Message setResp;
    MessageSerialization::decode(std::string(buf), setResp);

    // determines if error or failire occured
    if (setResp.get_message_type() != MessageType::OK) {
        std::cerr << "Error: " + setResp.get_quoted_text();
        return 1;
    }

    // send BYE request
    Message byeReq(MessageType::BYE, {});
    std::string byeMsg;
    MessageSerialization::encode(byeReq, byeMsg);
    rio_writen(clientfd, byeMsg.c_str(), byeMsg.length());

    // obtain BYE response
    n = rio_readlineb(&rio, buf, sizeof(buf));

    // check if read failed
    if (n < 0) {
        std::cerr << "Error: Failed to read response";
        return 1;
    }

    buf[n] = '\0';

    Message byeResp;
    MessageSerialization::decode(std::string(buf), byeResp);
    if (byeResp.get_message_type() != MessageType::OK) {
        // error or failure occurred
        std::cerr << "Error: " + byeResp.get_quoted_text();
        return 1;
    }

    // close client 
    close(clientfd);

    return 0;
  } catch (std::exception &e) {
      // handles any other errors occuring
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
  }
}
