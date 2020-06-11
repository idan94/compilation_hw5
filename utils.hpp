#ifndef UTILS_HW5
#define UTILS_HW5

#include <string>
#include <sstream>
#include <iostream>
#include "bp.hpp"

#define EMIT(to_emit) CodeBuffer::instance().emit(to_emit)
#define EMIT_GLOBAL(to_emit) CodeBuffer::instance().emitGlobal(to_emit)
#define GEN_LABEL() CodeBuffer::instance().genLabel()
#define BPATCH(address_list, label) CodeBuffer::instance().bpatch(address_list, label)

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

    string make_id_var(const string &id_number)
    {
        return "%var_" + id_number;
    }

    string make_string_var(int reg_number)
    {
        return "%string_var" + to_string(reg_number);
    }

    void assign_number_to_register(int left_reg_number, int number_to_assign)
    {
        stringstream to_emit;
        to_emit << make_var(left_reg_number) << " = "
                << "add i32 0, " << number_to_assign;
        EMIT(to_emit.str());
        // %var5 = add i32 0, number_to_assign
    }

    // void register_assign_with_op(int left_reg_number, int reg_number_a, const string &op, int reg_number_b)
    // {
    //     stringstream to_emit;
    //     to_emit << make_var(left_reg_number) << " = " << op " i32 " << make_var(reg_number_a) << ", " << make_var(reg_number_b);
    //     EMIT(to_emit.str());
    // }

    void binop_int(int left_reg_number, int reg_number_a, const string &op, int reg_number_b)
    {
        stringstream to_emit;
        string op_command;
        if (!op.compare("+"))
        {
            op_command = "add";
        }
        if (!op.compare("-"))
        {
            op_command = "sub";
        }
        to_emit << make_var(left_reg_number) << " = " << op_command << " " << reg_number_a << ", " << reg_number_b;
        EMIT(to_emit.str());
    }

    void binop_byte(int left_reg_number, int reg_number_a, const string &op, int reg_number_b)
    {
        int temp_register = fresh_var();
        binop_int(temp_register, reg_number_a, op, reg_number_b);
        stringstream to_emit;
        to_emit << make_var(left_reg_number) << " = and i32 255, " << make_var(temp_register);
        EMIT(to_emit.str());
    }

    void relop(int left_reg_number, int reg_number_a, const string &op, int reg_number_b)
    {
        stringstream to_emit;
        string op_command;
        int temp_register = fresh_var();
        if (!op.compare("=="))
        {
            op_command = "eq";
        }
        if (!op.compare("!="))
        {
            op_command = "ne";
        }
        if (!op.compare(">"))
        {
            op_command = "sgt";
        }
        if (!op.compare("<"))
        {
            op_command = "slt";
        }
        if (!op.compare(">="))
        {
            op_command = "sge";
        }
        if (!op.compare("<="))
        {
            op_command = "sle";
        }
        to_emit << make_var(temp_register) << " = icmp " << op_command << " i32 " << make_var(reg_number_a) << ", " << make_var(reg_number_b);
        EMIT(to_emit.str());

        to_emit.str("");
        to_emit << make_string_var(left_reg_number) << " = zext i1" << make_var(temp_register) << "to i32";
        EMIT(to_emit.str());
    }
    void add_string_constant(const string &str, int reg_number)
    {
        stringstream to_emit;
        to_emit << make_string_var(reg_number) << " = constant [" << (str.size() + 1) << " x i8] c\"" << str << "\\00\"";
        EMIT_GLOBAL(to_emit.str());
    }

    void print_buffers()
    {
        cout << "\n~~~~~~~~~~~PRINTING BUFFERS!~~~~~~~~~~~~" << endl;
        cout << "\n~~~Global Buffer:~~~" << endl;
        CodeBuffer::instance().printGlobalBuffer();
        cout << "\n~~~Code Buffer:~~~" << endl;
        CodeBuffer::instance().printCodeBuffer();
    }
    void assign_id(const string &id_reg, int right_reg)
    {
        stringstream to_emit;
        to_emit << "store i32 " << make_var(right_reg) << ", i32 " << make_id_var(id_reg);
        EMIT(to_emit.str());
    }
    void load_id_to_reg(const string &id_reg, int reg_number)
    {
        stringstream to_emit;
        to_emit << make_var(reg_number) << " = load i32, i32* " << make_id_var(id_reg);
        EMIT(to_emit.str());
    }
    void start_llvm_code()
    {
        EMIT("declare i32 @printf(i8*, ...)");
        EMIT("declarevoid @exit(i32)");
    }

    void add_print_function()
    {
        EMIT("define void @printi(i32) {");
        EMIT("    call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)");
        EMIT("    ret void");
        EMIT("}");
    }

    void add_printi_function()
    {
        EMIT("define void @print(i8*) {");
        EMIT("    call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
        EMIT("    ret void");
        EMIT("}");
    }
} // namespace utils_hw5

#endif