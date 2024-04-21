/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
{
  // initalize total number of values to 0
  total = 0;
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  // if total is 0, then it is by definiton an empty stack
  if (total == 0) {
    return true;
  }
  return false;
}

void ValueStack::push( const std::string &value )
{
  // push to stack
  stk.push(value);

  // increase total 
  total++;
}

std::string ValueStack::get_top() const
{
  // checks to determine if there is an element at top
  if (total == 0) {
    throw OperationException("Stack is empty");
  }

  // returns top
  return stk.top();
}

void ValueStack::pop()
{
  // checks to see if there is an element at top
  if (total == 0) {
    throw OperationException("Stack is empty");
  }
  // pops
  stk.pop();

  // decreases total
  total--;
}


