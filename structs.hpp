#ifndef __STRUCTS_HW3__
#define __STRUCTS_HW3__

#include <string>
#include <vector>
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
   Expression() : id_name{""}, type{""}, bool_value{false}, number{0}, str{""}, op{""}, register_number{-1},register_list{} {}
};

struct CallFunction{
   string return_type;
   int register_number;
   CallFunction(string return_type_value) : return_type{return_type_value} , register_number{-1} {}
};

// struct Statement{
//    vector<pair<int,BranchLabelIndex>> next_list;
//    Statement() : next_list{vector<pair<int,BranchLabelIndex>>{}} {}
// };

struct Scope
{
   string scope_type;
   vector<pair<int, BranchLabelIndex>> while_next_list;
   Scope(const string& scope_type_input) : scope_type{scope_type_input} {}
   string get_scope_type() const {
      return scope_type;
   }
};

typedef struct Formal Formal;
typedef struct Expression Expression;
typedef struct Statement Statement;

#endif
