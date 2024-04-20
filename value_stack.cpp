#include "value_stack.h"
#include "exceptions.h"

ValueStack::ValueStack()
  // TODO: initialize member variable(s) (if necessary)
{
  total = 0;
}

ValueStack::~ValueStack()
{
}

bool ValueStack::is_empty() const
{
  if (total == 0) {
    return true;
  }
  return false;
}

void ValueStack::push( const std::string &value )
{
  stk.push(value);
  total++;
}

std::string ValueStack::get_top() const
{
  if (total == 0) {
    throw std::runtime_error("Stack is empty");
  }
  return stk.top();
}

void ValueStack::pop()
{
  if (total == 0) {
    throw std::runtime_error("Stack is empty");
  }
  stk.pop();
  total--;
}


