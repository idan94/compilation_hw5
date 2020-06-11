#include "bp.hpp"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

// Utils Methods:
namespace utils_hw5
{
    int reg_count = 0;
    int fresh_var()
    {
        return reg_count++;
    }
    string make_var(int reg_number)
    {
        return "%var" + to_string(reg_number);
    }
    string assign_number_to_register(int reg_number, int number_to_assign)
    {
        stringstream to_emit;
        to_emit << make_var(reg_number) << "="
                << "add i32 0, " << number_to_assign;
        return to_emit.str();
    }
    void register_assign(int reg_number, int reg_number_a, const string& op, int reg_number_b)
    {
        stringstream to_emit;
        to_emit << make_var(reg_number) << "=" << reg_number_a << op << reg_number_b;
        // CodeBuffer::instance().emit(to_emit);
    }
}
