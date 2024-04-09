#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"
#include <iostream>

Message::Message()
  : m_message_type(MessageType::NONE)
{
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs )
{
  if (this != &rhs) {
    this->m_message_type = rhs.m_message_type;
    this->m_args = rhs.m_args;
  }
  return *this;
}

MessageType Message::get_message_type() const
{
  return m_message_type;
}

void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}

std::string Message::get_username() const
{
  if (this->get_message_type() == MessageType::LOGIN) {
    return this->get_arg(0);
  }
  return "";
}

std::string Message::get_table() const
{
  if (this->get_message_type() == MessageType::CREATE || this->get_message_type() == MessageType::GET || this->get_message_type() == MessageType::SET) {
    return this->get_arg(0);
  }
  return "";
}

std::string Message::get_key() const
{
  if (this->get_message_type() == MessageType::SET || this->get_message_type() == MessageType::GET) {
    return this->get_arg(1);
  }

  return "";
}

std::string Message::get_value() const
{
  if (this->get_message_type() == MessageType::PUSH || this->get_message_type() == MessageType::DATA) {
    return this->get_arg(0);
  }
  return "";
}

std::string Message::get_quoted_text() const
{
  if (this->get_message_type() == MessageType::FAILED || this->get_message_type() == MessageType::ERROR) {
    return this->get_arg(0);
  }
  return "";
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid() const
{
  if (!(this->valid_number_of_args())) {
    return false;
  }

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


bool identifier_is_valid(const std::string& identifier) {

  // char firstChar = identifier.at(0);

  // make sure the first character begins with a letter
  if (identifier.length() < 1 || !((identifier.at(0) >= 64 && identifier.at(0) <= 90) || (identifier.at(0) >= 97 && identifier.at(0) <= 122))) {
    return false;
  }
  // check that each subsequent character is either a letter or underscore
  for (int i = 1; i < (int)identifier.length(); i++) {
    char character = identifier.at(i);
    if (!((character >= 64 && character <= 90) || (character >= 97 && character <= 122) || character == 95 || (character >= 48 && character <= 57))) {
      return false;
    }
  }

  return true;
}

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

bool quoted_text_is_valid(const std::string& quoted_text) {

  // CHECK IF WE NEED TO DO THIS!!!!!!!!!
  // check that the first and last characters are quotes
  // if (quoted_text.length() < 2 || quoted_text.at(0) != 34 || quoted_text.at(quoted_text.length() - 1) != 34) {
  //   std::cout << quoted_text.at(0) << " " << quoted_text.at(1) << std::endl;
  //   std::cout << "failed 1" << std::endl;
  //   return false;
  // }

  // check that each character between the quotes isn't a quote
  for (int i = 1; i < (int)quoted_text.length() - 1; i++) {
    if (quoted_text.at(i) == 34) {
      std::cout << "failed 2" << std::endl;
      return false;
    }
  }

  return true;
}