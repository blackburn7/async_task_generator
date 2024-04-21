/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#include <iostream>
#include <string>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"

int main(int argc, char* argv[]) {
    // bool to determine if a transaction is in use
    bool useTransaction = false;

    int argIndex = 1; // arg pos counter

    if (argc == 7 && std::string(argv[1]) == "-t") { // -t for the use of a fake client 
        useTransaction = true;
        argIndex = 2;
    } else if (argc != 6) {
        std::cerr << "Requires format: ./incr_value [-t] hostname port username table key" << std::endl;
        return 1;
    }

    // parse thrpugh each arg and assign
    std::string hostname = argv[argIndex++];
    std::string port = argv[argIndex++]; // num
    std::string username = argv[argIndex++];
    std::string table = argv[argIndex++];
    std::string key = argv[argIndex++];

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

        // receive LOGIN response
        char buf[Message::MAX_ENCODED_LEN + 1];
        ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Error: Failed to read response";
            return 1;
        }

        // create buffer char
        buf[n] = '\0';
        Message loginResp;
        MessageSerialization::decode(std::string(buf), loginResp);
        if (loginResp.get_message_type() != MessageType::OK) {
            // the response ended up being Failed or Error
            std::cerr << "Error: " + loginResp.get_quoted_text();
            return 1;
        }

        // send BEGIN request if using transaction
        if (useTransaction) {
            // transaction is in use
            Message beginReq(MessageType::BEGIN, {});
            std::string beginMsg;
            MessageSerialization::encode(beginReq, beginMsg);
            rio_writen(clientfd, beginMsg.c_str(), beginMsg.length());

            // Receive BEGIN response
            n = rio_readlineb(&rio, buf, sizeof(buf));
            if (n < 0) {
                std::cerr << "Error: Failed to read response";
                return 1;
            }
            buf[n] = '\0';
            Message beginResp;
            MessageSerialization::decode(std::string(buf), beginResp);
            if (beginResp.get_message_type() != MessageType::OK) {
                // not OK, thus error occurred 
                std::cerr << "Error: " + beginResp.get_quoted_text();
                return 1;
            }
        }

        // send GET request
        Message getReq(MessageType::GET, {table, key});
        std::string getMsg;
        MessageSerialization::encode(getReq, getMsg);
        rio_writen(clientfd, getMsg.c_str(), getMsg.length());

        // receive GET response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            // no read
            std::cerr << "Error: Failed to read response";
            return 1;
        }

        buf[n] = '\0';

        Message getResp;
        MessageSerialization::decode(std::string(buf), getResp);
        if (getResp.get_message_type() != MessageType::OK) {
            // error as OK was not returned 
            std::cerr << "Error: " + getResp.get_quoted_text();
            return 1;
        }

        // send PUSH request
        Message pushReq(MessageType::PUSH, {"1"});
        std::string pushMsg;
        MessageSerialization::encode(pushReq, pushMsg);
        rio_writen(clientfd, pushMsg.c_str(), pushMsg.length());

        // receive PUSH response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            // read failed
            std::cerr << "Error: Failed to read response";
            return 1;
        }
        buf[n] = '\0';
        Message pushResp;
        MessageSerialization::decode(std::string(buf), pushResp);
        if (pushResp.get_message_type() != MessageType::OK) {
            // error occurred as OK was not returned
            std::cerr << "Error: " + pushResp.get_quoted_text();
            return 1;
        }

        // send ADD request
        Message addReq(MessageType::ADD, {});
        std::string addMsg;
        MessageSerialization::encode(addReq, addMsg);
        rio_writen(clientfd, addMsg.c_str(), addMsg.length());

        // receive ADD response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Error: Failed to read response";
            return 1;
        }

        //buffer assigned
        buf[n] = '\0';

        Message addResp;
        MessageSerialization::decode(std::string(buf), addResp);
        if (addResp.get_message_type() != MessageType::OK) {
            std::cerr << "Error: " + addResp.get_quoted_text();
            return 1;
        }

        // send SET request
        Message setReq(MessageType::SET, {table, key});
        std::string setMsg;
        MessageSerialization::encode(setReq, setMsg);
        rio_writen(clientfd, setMsg.c_str(), setMsg.length());

        // receive SET response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Error: Failed to read response";
            return 1;
        }

        buf[n] = '\0';

        Message setResp;
        MessageSerialization::decode(std::string(buf), setResp);
        if (setResp.get_message_type() != MessageType::OK) {
            std::cerr << "Error: " + setResp.get_quoted_text();
            return 1;
        }

        // send COMMIT request w/ transaction
        if (useTransaction) {
            Message commitReq(MessageType::COMMIT, {});
            std::string commitMsg;
            MessageSerialization::encode(commitReq, commitMsg);
            rio_writen(clientfd, commitMsg.c_str(), commitMsg.length());

            // receive COMMIT response
            n = rio_readlineb(&rio, buf, sizeof(buf));
            if (n < 0) {
                std::cerr << "Error: Failed to read response";
                return 1;
            }
            buf[n] = '\0';
            Message commitResp;
            MessageSerialization::decode(std::string(buf), commitResp);
            if (commitResp.get_message_type() != MessageType::OK) {
                // error response was received 
                std::cerr << "Error: " + commitResp.get_quoted_text();
                return 1;
            }
        }

        // send BYE request
        Message byeReq(MessageType::BYE, {});
        std::string byeMsg;
        MessageSerialization::encode(byeReq, byeMsg);
        rio_writen(clientfd, byeMsg.c_str(), byeMsg.length());

        // receive BYE response
        n = rio_readlineb(&rio, buf, sizeof(buf));
        if (n < 0) {
            std::cerr << "Error: Failed to read response";
            return 1;
        }

        buf[n] = '\0'; // buffer

        Message byeResp; // BYE
        MessageSerialization::decode(std::string(buf), byeResp);
        if (byeResp.get_message_type() != MessageType::OK) {
            // got a non OK response
            std::cerr << "Error: " + byeResp.get_quoted_text();
            return 1;
        }

        // close client 
        close(clientfd);

        return 0;
    } catch (std::exception &e) {
        // handles any other errors that may have occured
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}