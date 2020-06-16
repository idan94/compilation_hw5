#ifndef UTILS_HW5
#define UTILS_HW5

#include <string>
#include <sstream>
#include <iostream>
#include "bp.hpp"
#include "structs.hpp"

using namespace std;

// Utils Methods:
namespace utils_hw5
{
    int reg_count = 0;

    int fresh_var()
    {
        return reg_count++;
    }

    string make_reg(int reg_number)
    {
        return "%reg" + to_string(reg_number);
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
        to_emit << make_reg(left_reg_number) << " = "
                << "add i32 0, " << number_to_assign;
        EMIT(to_emit.str());
        // %var5 = add i32 0, number_to_assign
    }
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
        if (!op.compare("*"))
        {
            op_command = "mul";
        }
        to_emit << make_reg(left_reg_number) << " = " << op_command << " i32 " << make_reg(reg_number_a) << ", " << make_reg(reg_number_b);
        EMIT(to_emit.str());
    }

    void binop_byte(int left_reg_number, int reg_number_a, const string &op, int reg_number_b)
    {
        int temp_register = fresh_var();
        binop_int(temp_register, reg_number_a, op, reg_number_b);
        stringstream to_emit;
        to_emit << make_reg(left_reg_number) << " = and i32 255, " << make_reg(temp_register);
        EMIT(to_emit.str());
    }
    void binop_div(int left_reg_number, int numerator_reg, int denominator_reg)
    {
        stringstream to_emit;
        // Check if the denominator equals zero(error):
        int cond_temp_register = fresh_var();
        to_emit << make_reg(cond_temp_register) << " = icmp eq i32 0, " << make_reg(denominator_reg);
        EMIT(to_emit.str());
        to_emit.str("");

        to_emit << "br i1 " + make_reg(cond_temp_register) + ", label @, label @";
        int branch_pointer = EMIT(to_emit.str());
        to_emit.str("");
        // Denominator equals zero, error ? :
        string div_by_zero_label = GEN_LABEL();
        EMIT("call void @print(i8* getelementptr ([23 x i8], [23 x i8]* @.div_zero_error, i32 0, i32 0))");
        EMIT("call void @exit(i32 1)");
        int close_block_pointer = EMIT("br label @");

        // Denominator is not zero, so perform division:
        string divide_label = GEN_LABEL();
        to_emit << make_reg(left_reg_number) << " = sdiv i32 " << make_reg(numerator_reg) << ", " << make_reg(denominator_reg);
        EMIT(to_emit.str());
        BPATCH(CodeBuffer::makelist({branch_pointer, FIRST}), div_by_zero_label);
        BPATCH(CodeBuffer::makelist({branch_pointer, SECOND}), divide_label);
        // Last backpatching is just for closing the basic block:
        BPATCH(CodeBuffer::makelist({close_block_pointer, FIRST}), divide_label);
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
        to_emit << make_reg(temp_register) << " = icmp " << op_command << " i32 " << make_reg(reg_number_a) << ", " << make_reg(reg_number_b);
        EMIT(to_emit.str());

        to_emit.str("");
        to_emit << make_reg(left_reg_number) << " = zext i1 " << make_reg(temp_register) << " to i32";
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
        cout << endl;
        CodeBuffer::instance().printGlobalBuffer();
        cout << endl;
        CodeBuffer::instance().printCodeBuffer();
        cout << endl;
    }
    void assign_id(const string &id_reg, int right_reg)
    {
        stringstream to_emit;
        to_emit << "store i32 " << make_reg(right_reg) << ", i32 " << make_id_var(id_reg);
        EMIT(to_emit.str());
    }
    void load_id_to_reg(const string &id_reg, int reg_number)
    {
        stringstream to_emit;
        //TODO: fix this, the id's are in the table on the stack, they need to be alocated using the
        // aloca function
        to_emit << make_reg(reg_number) << " = load i32, i32* " << make_id_var(id_reg);
        EMIT(to_emit.str());
    }
    void start_llvm_code()
    {
        EMIT("declare i32 @printf(i8*, ...)");
        EMIT("declare void @exit(i32)");
        EMIT_GLOBAL("@.div_zero_error = constant [23 x i8] c\"Error division by zero\\00\"");
    }

    void add_print_function()
    {
        EMIT_GLOBAL("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
        EMIT("define void @printi(i32) {");
        EMIT("    call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)");
        EMIT("    ret void");
        EMIT("}");
    }

    void add_printi_function()
    {
        EMIT_GLOBAL("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
        EMIT("define void @print(i8*) {");
        EMIT("    call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
        EMIT("    ret void");
        EMIT("}");
    }
    void flip_bool_value(int output_reg, int input_reg)
    {
        stringstream to_emit;
        to_emit << make_reg(output_reg) << " = "
                << "sub i32 1," << make_reg(input_reg);
        EMIT(to_emit.str());
    }
    void bit_by_bit_operand(int output_reg, int input_reg_a, int input_reg_b, string op)
    {
        string op_code;
        if (!op.compare("OR"))
        {
            op_code = " or i32";
        }
        else
        {
            op_code = " and i32";
        }
        stringstream to_emit;
        to_emit << make_reg(output_reg) << " = " << op_code << make_reg(input_reg_a) << ", " << make_reg(input_reg_b);
        EMIT(to_emit.str());
    }
    void open_function(const string &return_type, const string &func_name, vector<string> arg_names,
                       vector<string> arg_types, stack<int> *current_stack_register)
    {
        stringstream to_emit;
        string return_type_code;
        if (return_type == "VOID")
        {
            return_type_code = "void";
        }
        else
        {
            return_type_code = "i32";
        }
        to_emit << "; " << func_name << " function decleration:";
        EMIT("");
        EMIT(to_emit.str());
        to_emit.str("");

        to_emit << "define " << return_type_code << " @" << func_name << "(";
        for (int i = 0; i < arg_names.size(); i++)
        {
            if (i == 0)
                to_emit << "i32";
            else
                to_emit << ", i32";
        }
        to_emit << ") {";
        EMIT(to_emit.str());
        to_emit.str("");

        for (int i = 0; i < arg_names.size(); i++)
        {
            to_emit << make_id_var(arg_names[i]) << " = alloca i32";
            EMIT(to_emit.str());
            to_emit.str("");

            to_emit << "store i32 %" << i << ", i32* " << make_id_var(arg_names[i]);
            EMIT(to_emit.str());
            to_emit.str("");
        }
        current_stack_register->push(fresh_var());
        to_emit << make_reg(current_stack_register->top()) << " = alloca [50 x i32]";
        EMIT(to_emit.str());
        to_emit.str("");
    }
    void close_function(bool is_void)
    {
        if (is_void)
        {
            EMIT("ret void");
        }
        else
        {
            EMIT("ret i32 0");
        }
        EMIT("}");
        EMIT("");
    }
    vector<pair<int, BranchLabelIndex>> *branch_to_next_list()
    {
        return new vector<pair<int, BranchLabelIndex>>(CodeBuffer::makelist({EMIT("br label @"), FIRST}));
    }
    //returns the pointer to the if condition branch pointer
    int gen_if_branch(int exp_reg_number)
    {
        stringstream to_emit;
        int temp_i1_bool = fresh_var();
        to_emit << make_reg(temp_i1_bool) << " = trunc i32 " << make_reg(exp_reg_number) << " to i1";
        EMIT(to_emit.str());
        to_emit.str("");

        to_emit << "br i1 " << make_reg(temp_i1_bool) << ", label @, label @";
        return EMIT(to_emit.str());
    }
    int gen_branch()
    {
        return EMIT("br label @");
    }
    void if_statement(int condition_branch_pointer, const string &if_block_label,
                      vector<pair<int, BranchLabelIndex>> next_list)
    {
        BPATCH(CodeBuffer::makelist({condition_branch_pointer, FIRST}), if_block_label);
        next_list = MERGE(CodeBuffer::makelist({condition_branch_pointer, SECOND}), next_list);
        string new_block_label = GEN_LABEL();
        BPATCH(next_list, new_block_label);
    }
    void if_else_statement(int condition_branch_pointer, const string &if_block_label,
                           const string &else_block_label, vector<pair<int, BranchLabelIndex>> next_list)
    {
        BPATCH(CodeBuffer::makelist({condition_branch_pointer, FIRST}), if_block_label);
        BPATCH(CodeBuffer::makelist({condition_branch_pointer, SECOND}), else_block_label);
        string new_block_label = GEN_LABEL();
        BPATCH(next_list, new_block_label);
    }
    void while_statement(const string &condition_pointer, int condition_branch_pointer, const string &while_block_label,
                         vector<pair<int, BranchLabelIndex>> loop_branch, vector<pair<int, BranchLabelIndex>> next_list)
    {
        BPATCH(CodeBuffer::makelist({condition_branch_pointer, FIRST}), while_block_label);
        BPATCH(loop_branch, condition_pointer);
        next_list = MERGE(CodeBuffer::makelist({condition_branch_pointer, SECOND}), next_list);
        string new_block_label = GEN_LABEL();
        BPATCH(next_list, new_block_label);
    }
    void while_else_statement(const string &condition_pointer, int condition_branch_pointer, const string &while_block_label,
                              const string &else_block_label, vector<pair<int, BranchLabelIndex>> loop_branch, vector<pair<int, BranchLabelIndex>> next_list)
    {
        BPATCH(CodeBuffer::makelist({condition_branch_pointer, FIRST}), while_block_label);
        BPATCH(CodeBuffer::makelist({condition_branch_pointer, SECOND}), else_block_label);
        BPATCH(loop_branch, condition_pointer);
        string new_block_label = GEN_LABEL();
        BPATCH(next_list, new_block_label);
    }
    void store_at_offset(int func_stack_pointer, int offset, const string &id_name, int register_number, bool is_initilized = true)
    {
        stringstream to_emit;
        to_emit << make_id_var(id_name) << " = getelementptr [50 x i32], [50 x i32]* ";
        to_emit << make_reg(func_stack_pointer) << ", i32 0, i32 " << offset;
        EMIT(to_emit.str());
        to_emit.str("");

        if (is_initilized)
        {
            to_emit << "store i32 " << make_reg(register_number);
        }
        else
        {
            to_emit << "store i32 " << 0;
        }
        to_emit << ", i32* " << make_id_var(id_name);
        EMIT(to_emit.str());
    }

    void assign_to_id(const string &id_name, int register_number)
    {
        stringstream to_emit;
        to_emit << "store i32 " << make_reg(register_number) << ", i32* " << make_id_var(id_name);
        EMIT(to_emit.str());
    }

    // void allocate_new_stack(int reg_number)
    // {
    //     stringstream to_emit;
    //     to_emit << make_reg(reg_number) << " = aloca [50 x i32]";
    //     EMIT(to_emit.str());
    // }
    vector<pair<int, BranchLabelIndex>> create_unconditional_branch(const string &comment)
    {
        return CodeBuffer::makelist({EMIT("br label @ ;" + comment), FIRST});
    }

    void return_statement(int return_exp_register = (-2))
    {
        stringstream to_emit;
        if (return_exp_register == (-2))
        {
            to_emit << "ret void";
        }
        else
        {
            to_emit << "ret i32 " << make_reg(return_exp_register);
        }
        EMIT(to_emit.str());
    }
    void call_function(string function_label, vector<int> arg_regs, int output_register = -10)
    {
        stringstream to_emit;
        string return_type_code;
        if (output_register != -10)
        {
            to_emit << make_reg(output_register) << " = ";
            return_type_code = "i32";
        }
        else
        {
            return_type_code = "void";
        }

        to_emit << "call " << return_type_code << " @" << function_label << "(";
        if (arg_regs.size() > 0)
        {
            to_emit << "i32 " << make_reg(arg_regs[0]);
        }
        for (int i = 1; i < arg_regs.size(); i++)
        {
            to_emit << ", i32 " << make_reg(arg_regs[i]);
        }
        to_emit << ")";
        EMIT(to_emit.str());
    }

} // namespace utils_hw5

#endif
