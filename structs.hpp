#ifndef __STRUCTS_HW3__
#define __STRUCTS_HW3__
#include <string>


using namespace std;

struct Formal{
   string id_name;
   string type;
   Formal() : id_name{""}, type{""} {}
};

struct Expression{
   string id_name;
   string type;
   bool bool_value;
   int number;
   string str;
   string op;
   Expression() : id_name{""}, type{""}, bool_value{false}, number{0}, str{""}, op{""} {}
};

typedef struct Formal Formal;
typedef struct Expression Expression;


#endif
