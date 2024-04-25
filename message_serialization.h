/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#ifndef MESSAGE_SERIALIZATION_H
#define MESSAGE_SERIALIZATION_H

#include "message.h"

namespace MessageSerialization {
  void encode(const Message &msg, std::string &encoded_msg);
  void decode(const std::string &encoded_msg, Message &msg);

  /**
  * Converts message type to string format
  */
  std::string messageTypeToString(const MessageType msgType);

  /**
  * Converts string to message type
  */
  MessageType stringToMessageType(const std::string str);
};

#endif // MESSAGE_SERIALIZATION_H
