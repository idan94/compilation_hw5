#include "bp.hpp"
#include <string>

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
        return "%var" + to_string(register_number);
    }
    void assign_number_to_register(int reg_number, int number_to_assign)
    {
        streamstring to_emit;
        to_emit << makr_var(reg_number) << "="
                << "add i32 0, " << number_to_assign;
        // CodeBuffer::instance().emit(to_emit);
        // %var5 = add i32 0,
    }
    void register_assign(int reg_number, int reg_number_a, const &string op, int reg_number_b)
    {
        streamstring to_emit;
        to_emit << makr_var(reg_number) << "=" << reg_number_a << op << reg_number_b;
        // CodeBuffer::instance().emit(to_emit);
    }
}
