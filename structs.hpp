#ifndef __STRUCTS_HW3__
#define __STRUCTS_HW3__

#include <string>
#include <vector>
#include "bp.hpp"

using namespace std;

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
   Expression() : id_name{""}, type{""}, bool_value{false}, number{0}, str{""}, op{""}, register_number{-1} {}
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
