#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table(const std::string &name)
    : m_name(name),
      inTransaction(false)
{
    //theMap = new std::map<std::string, std::string>();
    //tempMap = new std::map<std::string, std::string>();
    // Initialize the mutex y
    /*if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
        // Handle mutex initialization error
        throw std::runtime_error("Failed to initialize mutex");
    } */
    // test
}

Table::~Table()
{
  //delete theMap;
  //delete tempMap;
}

void Table::lock()
{
  // TODO: implement
}

void Table::unlock()
{
  // TODO: implement
}

bool Table::trylock()
{
  // TODO: implement
  return true;
}

void Table::set( const std::string &key, const std::string &value )
{
  /*theMap[key] = value;  */
}

std::string Table::get( const std::string &key )
{
  /*
  if (has_key(key)) {
    return map[key];
  } */
  //return theMap[key];
  return "y";
}

bool Table::has_key( const std::string &key )
{
  /*
  auto doesKeyExist = map.find(key);
  if (doesKeyExist == map.end()) {
    return false;
  } 
  */
  return true;
}

void Table::commit_changes()
{
  // TODO: implement
}

void Table::rollback_changes()
{
  // TODO: implement
}
