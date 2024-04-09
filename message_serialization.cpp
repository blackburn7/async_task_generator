#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // ensures string is empty
  encoded_msg = "";

  // begins with message type command
  encoded_msg += messageTypeToString(msg.get_message_type());

  // traverse through arguments and appends
  for (unsigned int i = 0; i < msg.get_num_args(); i++) {
    encoded_msg += " ";
    encoded_msg += msg.get_arg(i);
  }
  // terminates message with new line char
  encoded_msg += "\n";

  // exceeds max message length check
  if (encoded_msg.length() > msg.MAX_ENCODED_LEN) {
    throw InvalidMessage("Message length exceeds max length");
  }
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{   
  
  // exceeds max message length check
  if (encoded_msg_.length() > msg.MAX_ENCODED_LEN) {
    throw InvalidMessage("Message length exceeds max length");
  }

  // not terminated by \n
  if (encoded_msg_.at(encoded_msg_.length() - 1) != '\n') {
    throw InvalidMessage("Message not terminated with new line char");
  }

  unsigned int i = 0;
  std::string messageType = "";
  // extracts message type in string format
  while (encoded_msg_.at(i) != ' ' && encoded_msg_.at(i) != '\n') {
    messageType += encoded_msg_.at(i);
    i++;
  }

  // determines enum message type and maps to it
  msg.set_message_type(stringToMessageType(messageType));

  // if no other args
  if (encoded_msg_.at(i) != '\n') {
    // skip over space char ' '
    i++;

    // create temp arg string holder
    std::string arg = "";

    while (encoded_msg_.at(i) != '\n') {
      if (encoded_msg_.at(i) != ' ') {
        arg += encoded_msg_.at(i);
      } else {
        msg.push_arg(arg);
        arg = "";
      }
      i++;
    }
  }

  // confirm validity
  if (!msg.is_valid()) {
    throw InvalidMessage("Message not valid");
  }
}

std::string MessageSerialization::messageTypeToString(const MessageType msgType) {
  if (msgType == MessageType::LOGIN) {
    return "LOGIN";
  } 
  else if (msgType == MessageType::CREATE) {
    return "CREATE";
  }
  else if (msgType == MessageType::PUSH) {
    return "PUSH";
  }
  else if (msgType == MessageType::POP) {
    return "POP";
  }
  else if (msgType == MessageType::TOP) {
    return "TOP";
  }
  else if (msgType == MessageType::SET) {
    return "SET";
  }
  else if (msgType == MessageType::GET) {
    return "GET";
  }
  else if (msgType == MessageType::ADD) {
    return "ADD";
  }
  else if (msgType == MessageType::SUB) {
    return "SUB";
  }
  else if (msgType == MessageType::MUL) {
    return "MUL";
  }
  else if (msgType == MessageType::DIV) {
    return "DIV";
  }
  else if (msgType == MessageType::BEGIN) {
    return "BEGIN";
  }
  else if (msgType == MessageType::COMMIT) {
    return "COMMIT";
  }
  else if (msgType == MessageType::BYE) {
    return "BYE";
  }
  else if (msgType == MessageType::OK) {
    return "OK";
  }
  else if (msgType == MessageType::FAILED) {
    return "FAILED";
  }
  else if (msgType == MessageType::ERROR) {
    return "ERROR";
  }
  else if (msgType == MessageType::DATA) {
    return "DATA";
  } else {
    return "";
  }
}

MessageType MessageSerialization::stringToMessageType(const std::string str) {
  if (str == "LOGIN") {
    return MessageType::LOGIN;
  } 
  else if (str == "CREATE") {
    return MessageType::CREATE;
  }
  else if (str == "PUSH") {
    return MessageType::PUSH;
  }
  else if (str == "POP") {
    return MessageType::POP;
  }
  else if (str == "TOP") {
    return MessageType::TOP;
  }
  else if (str == "SET") {
    return MessageType::SET;
  }
  else if (str == "GET") {
    return MessageType::GET;
  }
  else if (str == "ADD") {
    return MessageType::ADD;
  }
  else if (str == "SUB") {
    return MessageType::SUB;
  }
  else if (str == "MUL") {
    return MessageType::MUL;
  }
  else if (str == "DIV") {
    return MessageType::DIV;
  }
  else if (str == "BEGIN") {
    return MessageType::BEGIN;
  }
  else if (str == "COMMIT") {
    return MessageType::COMMIT;
  }
  else if (str == "BYE") {
    return MessageType::BYE;
  }
  else if (str == "OK") {
    return MessageType::OK;
  }
  else if (str == "FAILED") {
    return MessageType::FAILED;
  }
  else if (str == "ERROR") {
    return MessageType::ERROR;
  }
  else if (str == "DATA") {
    return MessageType::DATA;
  }
  
  return MessageType::NONE;
}


