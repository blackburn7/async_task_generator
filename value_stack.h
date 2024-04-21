/*
Assignment 5

Atticus Colwell
acolwel2@jh.edu

Matthew Blackburn
mblackb8@jh.edu
*/

#ifndef VALUE_STACK_H
#define VALUE_STACK_H

#include <vector>
#include <string>
#include <stack>

class ValueStack {
private:
  // stack to hold values
  std::stack<std::string> stk;

  // total number of values present in stack
  int total = 0;

public:
  ValueStack();
  ~ValueStack();

  bool is_empty() const;
  void push( const std::string &value );

  // Note: get_top() and pop() should throw OperationException
  // if called when the stack is empty

  std::string get_top() const;
  void pop();
};

#endif // VALUE_STACK_H
