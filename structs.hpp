#ifndef __STRUCTS_HW3__
#define __STRUCTS_HW3__

#include <string>
#include <vector>
#include <stack>
#include "bp.hpp"

using namespace std;

#define EMIT(to_emit) CodeBuffer::instance().emit(to_emit)
#define EMIT_GLOBAL(to_emit) CodeBuffer::instance().emitGlobal(to_emit)
#define GEN_LABEL() CodeBuffer::instance().genLabel()
#define BPATCH(address_list, label) CodeBuffer::instance().bpatch(address_list, label)
#define MERGE(list_1, list_2) CodeBuffer::merge(list_1, list_2)

struct Formal
{
   string id_name;
   string type;
   Formal() : id_name{""}, type{""} {}
};

struct Expression
{
   string id_name;
   string type;
   bool bool_value;
   int number;
   string str;
   string op;
   int register_number;
   vector<int> register_list;
   Expression() : id_name{""}, type{""}, bool_value{false}, number{0}, str{""}, op{""}, register_number{-1}, register_list{} {}
};

struct CallFunction
{
   string return_type;
   int register_number;
   CallFunction(string return_type_value) : return_type{return_type_value}, register_number{-1} {}
};

struct CurrentStackRegister
{
   stack<int> stack_register_stack;
   stack<int> stack_stack_counter;
   int stack_counter;
   int function_stack_register;
   CurrentStackRegister() : stack_counter{0} {}
   int top()
   {
      return stack_register_stack.top();
   }
   void pop()
   {
      stack_register_stack.pop();
      stack_stack_counter.pop();
   }
   void push(int stack_register_number)
   {
      if (stack_register_stack.empty())
      {
         // Means its the function scope
         function_stack_register = stack_register_number;
      }
      stack_register_stack.push(stack_register_number);
      stack_stack_counter.push(++stack_counter);
   }
   bool empty()
   {
      return stack_register_stack.empty();
   }
   int get_stack_counter()
   {
      return stack_stack_counter.top();
   }
};

// struct Statement{
//    vector<pair<int,BranchLabelIndex>> next_list;
//    Statement() : next_list{vector<pair<int,BranchLabelIndex>>{}} {}
// };

struct Scope
{
   string scope_type;
   Scope(const string &scope_type_input) : scope_type{scope_type_input} {}
   string get_scope_type() const
   {
      return scope_type;
   }
};

typedef struct Formal Formal;
typedef struct Expression Expression;
typedef struct Statement Statement;

#endif
