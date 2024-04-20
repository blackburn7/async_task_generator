#ifndef VALUE_STACK_H
#define VALUE_STACK_H

#include <vector>
#include <string>
#include <stack>

class ValueStack {
private:
  std::stack<std::string> stk;
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
