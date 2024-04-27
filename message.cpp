/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"
#include <iostream>

// constructor 
Message::Message()
  : m_message_type(MessageType::NONE)
{
}

// constructor 
Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

// constructor 
Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

// destructor 
Message::~Message()
{
}

// resets message
void Message::resetMsg() {
  // returns message type to NONE
  m_message_type = MessageType::NONE;

  // assigns m_args to an empty vector
  m_args.clear();
}

// assignment operator
Message &Message::operator=( const Message &rhs )
{
  if (this != &rhs) {
    // match message type to message type
    this->m_message_type = rhs.m_message_type;

    // copy contents of vector over
    this->m_args = rhs.m_args;
  }

  // return current
  return *this;
}

// return message type
MessageType Message::get_message_type() const
{
  // returns message type of message
  return m_message_type;
}

// sets message type
void Message::set_message_type(MessageType message_type)
{
  // sets to arg message type
  m_message_type = message_type;
}

// gets username provided in message
std::string Message::get_username() const
{
  // given LOGIN statement, otherwise wouldnt have username
  if (this->get_message_type() == MessageType::LOGIN) { 
    
    return this->get_arg(0); // returns string format of username
  }

  // not a login statement
  return "";
}

// gets table name
std::string Message::get_table() const
{
  // only the instances where a table name would be provided
  if (this->get_message_type() == MessageType::CREATE || this->get_message_type() == MessageType::GET || this->get_message_type() == MessageType::SET) {
    return this->get_arg(0);
  }

  // not table dependent 
  return "";
}

// gets key name
std::string Message::get_key() const
{
  // only instances of messages that would use a key
  if (this->get_message_type() == MessageType::SET || this->get_message_type() == MessageType::GET) {
    return this->get_arg(1);
  }

  // no key
  return "";
}

// gets value from message
std::string Message::get_value() const
{
  // only instances that require a value in message
  if (this->get_message_type() == MessageType::PUSH || this->get_message_type() == MessageType::DATA) {
    return this->get_arg(0);
  }

  // no value for message type
  return "";
}

// get quoted text from message
std::string Message::get_quoted_text() const
{
  // instances with quoted text
  if (this->get_message_type() == MessageType::FAILED || this->get_message_type() == MessageType::ERROR) {
    return this->get_arg(0);
  }
  return "";
}

// add argument to vector 
void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg ); // pushes to vector of args
}

// determines if message is valid 
bool Message::is_valid() const
{
  if (!(this->valid_number_of_args())) { // first checks to detemrine number of args is valid
    return false;
  }

  // determines if correct placements dependent on message type
  switch(this->get_message_type()) {
    case MessageType::LOGIN:
    case MessageType::CREATE:
      if (!identifier_is_valid(m_args.at(0))) {
        return false;
      }
      break;
    case MessageType::SET:
    case MessageType::GET:
      if (!identifier_is_valid(m_args.at(0)) || !identifier_is_valid(m_args.at(1))) {
        return false;
      }
      break;
    case MessageType::PUSH:
    case MessageType::DATA:
      if (!value_is_valid(m_args.at(0))) {
        return false;
      }
      break;
    case MessageType::FAILED:
    case MessageType::ERROR:
      if (!quoted_text_is_valid(m_args.at(0))) {
        return false;
      }
      break;
    default:
      break;
  }  
  return true;
}

// determines valid number of args dependent on message type
bool Message::valid_number_of_args() const {
  switch(this->get_message_type()) {
    case MessageType::POP:
    case MessageType::TOP:
    case MessageType::ADD:
    case MessageType::MUL:
    case MessageType::SUB:
    case MessageType::DIV:
    case MessageType::BEGIN:
    case MessageType::COMMIT:
    case MessageType::BYE:
    case MessageType::OK:
    case MessageType::NONE:
      return this->get_num_args() == 0;
    case MessageType::LOGIN:
    case MessageType::CREATE:
    case MessageType::PUSH:
    case MessageType::FAILED:
    case MessageType::ERROR:
    case MessageType::DATA:
      return this->get_num_args() == 1;
    case MessageType::SET:
    case MessageType::GET:
      return this->get_num_args() == 2;
    default:
      return false;
  }
}

// is identifier valid check
bool identifier_is_valid(const std::string& identifier) {

  // make sure the first character begins with a letter
  if (identifier.length() < 1 || !((identifier.at(0) >= 64 && identifier.at(0) <= 90) || (identifier.at(0) >= 97 && identifier.at(0) <= 122))) {
    return false;
  }
  // check that each subsequent character is either a letter or underscore or digit
  for (int i = 1; i < (int)identifier.length(); i++) {
    char character = identifier.at(i);
    if (!((character >= 64 && character <= 90) || (character >= 97 && character <= 122) || character == 95 || (character >= 48 && character <= 57))) {
      return false;
    }
  }

  return true;
}

// is value valid
bool value_is_valid(const std::string& value) {

  // check that value has a non-zero length
  if (value.length() < 1) {
    return false;
  }

  // check that each character in value isn't a whitespace
  for (char character: value) {
    if (std::isspace(character)) {
      return false;
    }
  }

  return true;
}

// is quoted text valid
bool quoted_text_is_valid(const std::string& quoted_text) {
  
  // check that each character between the quotes isn't a quote
  for (int i = 1; i < (int)quoted_text.length() - 1; i++) {
    if (quoted_text.at(i) == 34) {
      std::cout << "failed 2" << std::endl;
      return false;
    }
  }

  return true;
}