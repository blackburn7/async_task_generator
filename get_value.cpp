#include <iostream>
#include <string>
#include <cstdlib>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "exceptions.h"

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
        char buf[Message::MAX_ENCODED_LEN + 1];
        ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Failed to read response";
        }
        buf[n] = '\0';
        Message loginResp;
        MessageSerialization::decode(string(buf), loginResp);
        if (loginResp.get_message_type() != MessageType::OK) {
            std::cerr << "Error: " + loginResp.get_quoted_text();
        }

        // Send GET request
        Message getReq(MessageType::GET, {table, key});
        string getMsg;
        MessageSerialization::encode(getReq, getMsg);
        rio_writen(clientfd, getMsg.c_str(), getMsg.length());

        // Receive GET response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Failed to read response";
        }
        buf[n] = '\0';
        Message getResp;
        MessageSerialization::decode(string(buf), getResp);
        if (getResp.get_message_type() != MessageType::OK) {
            std::cerr << "Error: " + getResp.get_quoted_text();
            return 1;
        }

        // Send TOP request
        Message topReq(MessageType::TOP, {});
        string topMsg;
        MessageSerialization::encode(topReq, topMsg);
        rio_writen(clientfd, topMsg.c_str(), topMsg.length());

        // Receive TOP response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Failed to read response";
            return 1;
        }
        buf[n] = '\0';
        Message topResp;
        MessageSerialization::decode(string(buf), topResp);
        if (topResp.get_message_type() != MessageType::DATA) {
            std::cerr << "Error: " + topResp.get_quoted_text();
            return 1;
        }

        // Print the retrieved value
        cout << topResp.get_value() << "\n";

        // Send BYE request
        Message byeReq(MessageType::BYE, {});
        string byeMsg;
        MessageSerialization::encode(byeReq, byeMsg);
        rio_writen(clientfd, byeMsg.c_str(), byeMsg.length());

        // Receive BYE response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Error: Failed to read response";
            return 1;
        }
        buf[n] = '\0';
        Message byeResp;
        MessageSerialization::decode(string(buf), byeResp);
        if (byeResp.get_message_type() != MessageType::OK) {
            std::cerr << "Error: " + byeResp.get_quoted_text();
            return 1;
        }

        close(clientfd);
    } catch (std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
    return 0;
}
