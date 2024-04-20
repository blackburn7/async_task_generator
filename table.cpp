#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"

Table::Table(const std::string &name)
    : m_name(name)
{
    theMap = new std::map<std::string, std::string>();
    tempMap = new std::map<std::string, std::string>();
    // Initialize the mutex y
    if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
        // Handle mutex initialization error
        throw std::runtime_error("Failed to initialize mutex");
    } 
    // test
}

Table::~Table()
{
  delete theMap;
  delete tempMap;
}

void Table::lock() {
  pthread_mutex_lock(&m_mutex);
}

void Table::unlock() {
  pthread_mutex_unlock(&m_mutex);
}

bool Table::trylock() {
  int result = pthread_mutex_trylock(&m_mutex);
  return (result == 0);
}

void Table::set( const std::string &key, const std::string &value )
{
  (*tempMap)[key] = value; 
}

std::string Table::get( const std::string &key )
{
  auto doesKeyExist1 = (*theMap).find(key);
  auto doesKeyExist2 = (*tempMap).find(key);
  if (doesKeyExist1 == (*theMap).end() && doesKeyExist2 == (*theMap).end()) {
    throw std::runtime_error("no key in table");
  } 
  if (doesKeyExist2 == (*tempMap).end()) {
    return (*theMap)[key];
  }
  return (*tempMap)[key];
}

bool Table::has_key( const std::string &key )
{
  auto doesKeyExist1 = (*theMap).find(key);
  auto doesKeyExist2 = (*tempMap).find(key);
  if (doesKeyExist1 == (*theMap).end() && doesKeyExist2 == (*tempMap).end()) {
    return false;
  } 
  return true;
}

void Table::commit_changes()
{
  for (const auto& pair : *tempMap) {
    const std::string& key = pair.first;
    const std::string& value = pair.second;
        
    // insert or update pair
    (*theMap)[key] = value;
  }

  delete tempMap;
  tempMap = new std::map<std::string, std::string>();
}

void Table::rollback_changes()
{
  delete tempMap;
  tempMap = new std::map<std::string, std::string>();
}
