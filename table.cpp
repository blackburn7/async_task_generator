/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#include <cassert>
#include "table.h"
#include "exceptions.h"
#include "guard.h"
#include <iostream>

Table::Table(const std::string &name)
    : m_name(name)
{
    theMap = new std::map<std::string, std::string>();
    tempMap = new std::map<std::string, std::string>();

    // Initialize the mutex as this is a key part of determining locks
    if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
        // Handle mutex initialization error
        throw std::runtime_error("Failed to initialize mutex");
    } 
    
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
  (*tempMap)[key] = value; // sets temp map key with value
}

std::string Table::get( const std::string &key )
{
  // traverses each map to obtain value
  auto doesKeyExist1 = (*theMap).find(key);
  auto doesKeyExist2 = (*tempMap).find(key);
  if (doesKeyExist1 == (*theMap).end() && doesKeyExist2 == (*theMap).end()) {
    throw std::runtime_error("no key in table"); // key doesnt exist
  } 
  // ensure we return most recently edited value which would be present if tempMap contains
  if (doesKeyExist2 == (*tempMap).end()) {
    // no recent changes made to this key/value
    return (*theMap)[key];
  }

  // was contained in temp map
  return (*tempMap)[key];
}

bool Table::has_key( const std::string &key )
{
  // traverses to determine if key exists in either map
  auto doesKeyExist1 = (*theMap).find(key);
  auto doesKeyExist2 = (*tempMap).find(key);
  if (doesKeyExist1 == (*theMap).end() && doesKeyExist2 == (*tempMap).end()) {
    // key exists in neither
    return false;
  } 
  // key exists
  return true;
}

void Table::commit_changes()
{
  // copies every pair from temp map over to the map
  for (const auto& pair : *tempMap) {
    const std::string& key = pair.first;
    const std::string& value = pair.second;
        
    // insert or update pair
    (*theMap)[key] = value;
  }

  // resets temp map to be empty
  delete tempMap;
  tempMap = new std::map<std::string, std::string>();
}

void Table::rollback_changes()
{
  // eliminates temp map
  delete tempMap;
  tempMap = new std::map<std::string, std::string>();
}
