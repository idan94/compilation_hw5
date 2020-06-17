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
    vector<stack<int>> *current_function_args_stack;
    vector<stack<int>> *current_var_stack;
    int reg_count = 0;

    int fresh_var()
    {
        return reg_count++;
    }

    string make_reg(int reg_number)
    {
        return "%reg" + to_string(reg_number);
    }

    // string make_id_var(const string &id_number)
    // {
    //     return "%var_" + id_number;
    // }

    string make_var(int var_offset, int stack_number)
    {
        if (var_offset < 0)
        {
            return "%arg_var" + to_string(-var_offset - 1) + "_stack" + to_string(stack_number);
        }
        return "%var" + to_string(var_offset) + "_scope" + to_string(stack_number);
    }

    string make_string_var(int reg_number)
    {
        return "@.string_var" + to_string(reg_number);
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
    void load_id_to_reg(int id_offset, int reg_number)
    {
        stringstream to_emit;
        int stack_number;
        if (id_offset < 0)
        {
            stack_number = ((*current_function_args_stack)[-id_offset - 1]).top();
        }
        else
        {
            stack_number = ((*current_var_stack)[id_offset]).top();
        }
        to_emit << make_reg(reg_number) << " = load i32, i32* " << make_var(id_offset, stack_number);
        EMIT(to_emit.str());
        // }
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
    void open_scope(CurrentStackRegister *current_stack_register, bool with_brace)
    {
        stringstream to_emit;

        current_stack_register->push(fresh_var());
        if (with_brace)
        {
            to_emit << "{    ";
        }
        to_emit << "; Open scope(" << to_string(current_stack_register->stack_counter) << ")";
        EMIT(to_emit.str());
        to_emit.str("");
        to_emit << make_reg(current_stack_register->top()) << " = alloca [50 x i32]   ; allocate stack for the new scope";
        EMIT(to_emit.str());
    }
    void close_scope(CurrentStackRegister *current_stack_register, bool with_brace)
    {
        stringstream to_emit;
        if (with_brace)
        {
            to_emit << "}    ";
        }
        to_emit << "; Close scope(" << to_string(current_stack_register->stack_counter) << ")";
        EMIT(to_emit.str());
        to_emit.str("");
        for (int i = 0; i < current_function_args_stack->size(); i++)
        {
            if ((*current_function_args_stack)[i].top() == current_stack_register->stack_counter)
            {
                (*current_function_args_stack)[i].pop();
            }
        }
        for (int i = 0; i < current_var_stack->size(); i++)
        {
            if ((*current_var_stack)[i].top() == current_stack_register->stack_counter)
            {
                (*current_var_stack)[i].pop();
            }
        }
        current_stack_register->pop();
    }
    void open_function(const string &return_type, const string &func_name, int args_number, CurrentStackRegister *current_stack_register)
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
        for (int i = 0; i < args_number; i++)
        {
            if (i == 0)
                to_emit << "i32";
            else
                to_emit << ", i32";
        }
        to_emit << ")";
        EMIT(to_emit.str());
        to_emit.str("");

        open_scope(current_stack_register, true);
        EMIT("; Arguments allocation:");
        current_function_args_stack = new vector<stack<int>>();
        current_var_stack = new vector<stack<int>>();
        stack<int> new_stack;
        for (int i = args_number - 1; i >= 0; i--)
        {
            to_emit << make_var(-i - 1, current_stack_register->stack_counter) << " = alloca i32";
            EMIT(to_emit.str());
            to_emit.str("");

            to_emit << "store i32 %" << i << ", i32* " << make_var(-i - 1, current_stack_register->stack_counter);
            EMIT(to_emit.str());
            to_emit.str("");
            stack<int> new_funciton_args_stack;
            new_funciton_args_stack.push(current_stack_register->stack_counter);
            current_function_args_stack->push_back(new_funciton_args_stack);
        }
        EMIT("");
        EMIT("; Function body:");
    }
    void close_function(bool is_void, CurrentStackRegister *current_stack_register)
    {
        if (is_void)
        {
            EMIT("ret void");
        }
        else
        {
            EMIT("ret i32 0");
        }
        close_scope(current_stack_register, true);
        delete current_function_args_stack;
        delete current_var_stack;
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
    void store_at_offset(int func_stack_pointer, int stack_number, int id_offset, int register_number, bool is_initilized = true)
    {
        stringstream to_emit;
        to_emit << make_var(id_offset, stack_number) << " = getelementptr [50 x i32], [50 x i32]* ";
        to_emit << make_reg(func_stack_pointer) << ", i32 0, i32 " << id_offset;
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
        to_emit << ", i32* " << make_var(id_offset, stack_number);
        stack<int> new_var_stack;
        new_var_stack.push(stack_number);
        current_var_stack->insert(current_var_stack->begin() + id_offset, new_var_stack);
        EMIT(to_emit.str());
    }

    void assign_to_id(int func_stack_pointer, int id_offset, int register_number, int stack_number, int function_stack_register)
    {
        stringstream to_emit;
        if (id_offset < 0)
        {
            if ((*current_function_args_stack)[-id_offset - 1].top() == stack_number)
            {
                to_emit << "store i32 " << make_reg(register_number) << ", i32* " << make_var(id_offset, stack_number);
            }
            else
            {
                to_emit << make_var(id_offset, stack_number) << " = alloca i32";
                EMIT(to_emit.str());
                to_emit.str("");

                to_emit << "store i32 " << make_reg(register_number) << ", i32* " << make_var(id_offset, stack_number);
                (*current_function_args_stack)[-id_offset - 1].push(stack_number);
            }
        }
        else
        {
            if ((*current_var_stack)[id_offset].top() == stack_number)
            {
                to_emit << "store i32 " << make_reg(register_number) << ", i32* " << make_var(id_offset, stack_number);
            }
            else
            {
                to_emit << make_var(id_offset, stack_number) << " = getelementptr [50 x i32], [50 x i32]* ";
                to_emit << make_reg(func_stack_pointer) << ", i32 0, i32 " << id_offset + 1;
                EMIT(to_emit.str());
                to_emit.str("");

                to_emit << "store i32 " << make_reg(register_number) << ", i32* " << make_var(id_offset, stack_number);
                (*current_var_stack)[id_offset].push(stack_number);
            }
        }

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
    void call_function_print(int string_reg_number, int string_length)
    {
        stringstream to_emit;
        to_emit << "call void @print(i8* getelementptr ([" << string_length + 1 << " x i8], [";
        to_emit << string_length + 1 << " x i8]* " << make_string_var(string_reg_number) << ", i32 0, i32 0))";
        EMIT(to_emit.str());
    }
    void short_circit_and(int output_registr,
                            int first_exp_reg,
                            vector<pair<int, BranchLabelIndex>> branch_after_first_exp,
                            string lable_of_second_exp,int second_exp_reg,
                            vector<pair<int,BranchLabelIndex>> branch_after_second_exp){

        // after calculating the first expression jump to short circit code
        stringstream to_emit;
        string short_circt_code_label = GEN_LABEL();
        BPATCH(branch_after_first_exp,short_circt_code_label);

        // create register for the result of the comparison  
        int first_cmp_reg = fresh_var();

        // alocate space on the stack (aside from the "stack") that will store the 
        // result of the computation
        int pointer_to_resulting = fresh_var();
        to_emit << make_reg(pointer_to_resulting) << " = alloca  i32 ";
        EMIT(to_emit.str());
        to_emit.str("");

        // check if to return false without computing the second expression
        to_emit << make_reg(first_cmp_reg) <<  " = icmp eq i32 %0, " << make_reg(first_exp_reg);
        EMIT(to_emit.str());
        to_emit.str("");
        to_emit << "br i1 " << make_reg(first_cmp_reg)<< " lable @, lable " << lable_of_second_exp;

        // save the branch location, it needs to jump to return false
        vector<pair<int,BranchLabelIndex>> return_false_branch = CodeBuffer::makelist({EMIT(to_emit.str()),FIRST});
        to_emit.str("");

        // return false, becuase we can only use each register once we will store the value on the stack
        // and after compliting the code segment we will load once the value from the stack
        string return_false = GEN_LABEL();
        BPATCH(return_false_branch,return_false);
        // store "flase" in the stack at the location that has been allocated to save the result
        to_emit << " store i32 %0, *i32" << make_reg(pointer_to_resulting);
        EMIT(to_emit.str());
        to_emit.str("");

        // now branch to exit
        to_emit << "br lable @";
        vector<pair<int,BranchLabelIndex>> after_stored_false = CodeBuffer::makelist({EMIT(to_emit.str()),FIRST});
        to_emit.str("");

        // if reached this segment of code, the first value was true, that means we need to
        // return the value of the second register
        string return_second_expression_value = GEN_LABEL();
        BPATCH(branch_after_second_exp,return_second_expression_value);
        to_emit << " store i32 " << make_reg(second_exp_reg) << ", *i32" << make_reg(pointer_to_resulting);
        EMIT(to_emit.str());
        to_emit.str("");

        // now branch to exit
        to_emit << "br lable @";
        vector<pair<int,BranchLabelIndex>> after_stored_second_exp = CodeBuffer::makelist({EMIT(to_emit.str()),FIRST});
        to_emit.str("");


        // after the result has been savec on the location allocated on the stack,
        // load it to the resulting register
        string exit = GEN_LABEL();
        BPATCH(MERGE(after_stored_second_exp,after_stored_false),exit);
        to_emit << make_reg(output_registr) << " = load i32, i32*" << make_reg(pointer_to_resulting);
        EMIT(to_emit.str());
                
    }
    void short_circit_or(int output_registr,
                            int first_exp_reg,
                            vector<pair<int, BranchLabelIndex>> branch_after_first_exp,
                            string lable_of_second_exp,int second_exp_reg,
                            vector<pair<int,BranchLabelIndex>> branch_after_second_exp){
        
        // after calculating the first expression jump to short circit code
        stringstream to_emit;
        string short_circt_code_label = GEN_LABEL();
        BPATCH(branch_after_first_exp,short_circt_code_label);

        // create register for the result of the comparison  
        int first_cmp_reg = fresh_var();

        // alocate space on the stack (aside from the "stack") that will store the 
        // result of the computation
        int pointer_to_resulting = fresh_var();
        to_emit << make_reg(pointer_to_resulting) << " = alloca  i32 ";
        EMIT(to_emit.str());
        to_emit.str("");

        // check if to return true without computing the second expression
        to_emit << make_reg(first_cmp_reg) <<  " = icmp ne i32 %0, " << make_reg(first_exp_reg);
        EMIT(to_emit.str());
        to_emit.str("");
        to_emit << "br i1 " << make_reg(first_cmp_reg)<< " lable @, lable " << lable_of_second_exp;

        // save the branch location, it needs to jump to return ture
        vector<pair<int,BranchLabelIndex>> return_true_branch = CodeBuffer::makelist({EMIT(to_emit.str()),FIRST});
        to_emit.str("");

        // return ture, becuase we can only use each register once we will store the value on the stack
        // and after compliting the code segment we will load once the value from the stack
        string return_true = GEN_LABEL();
        BPATCH(return_true_branch,return_true);
        // store "flase" in the stack at the location that has been allocated to save the result
        to_emit << " store i32 %1, *i32" << make_reg(pointer_to_resulting);
        EMIT(to_emit.str());
        to_emit.str("");

        // now branch to exit
        to_emit << "br lable @";
        vector<pair<int,BranchLabelIndex>> after_stored_true = CodeBuffer::makelist({EMIT(to_emit.str()),FIRST});
        to_emit.str("");

        // if reached this segment of code, the first value was false, that means we need to
        // return the value of the second register
        string return_second_expression_value = GEN_LABEL();
        BPATCH(branch_after_second_exp,return_second_expression_value);
        to_emit << " store i32 " << make_reg(second_exp_reg) << ", *i32" << make_reg(pointer_to_resulting);
        EMIT(to_emit.str());
        to_emit.str("");

        // now branch to exit
        to_emit << "br lable @";
        vector<pair<int,BranchLabelIndex>> after_stored_second_exp = CodeBuffer::makelist({EMIT(to_emit.str()),FIRST});
        to_emit.str("");


        // after the result has been savec on the location allocated on the stack,
        // load it to the resulting register
        string exit = GEN_LABEL();
        BPATCH(MERGE(after_stored_second_exp,after_stored_true),exit);
        to_emit << make_reg(output_registr) << " = load i32, i32*" << make_reg(pointer_to_resulting);
        EMIT(to_emit.str());
                
        
    }

} // namespace utils_hw5

#endif
