#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "parser.hh"

// Since all integer constants are positive, we use positive ints to represent constant pushs and
// negative ints to represent all other instructions
enum Instruction : int { X=-1, ADD=-2, SUB=-3, MUL=-4, DIV=-5 };

const int max_stack_size = 32;

int run(const std::vector<Instruction>& program, int x) {
#define PUSH(arg) stack[n++] = (arg)
#define POP() stack[--n]
    int stack[max_stack_size];
    int n = 0;
    for (Instruction instr : program) {
        assert(n <= max_stack_size);
        if (instr >= 0) {
            PUSH(instr);
        } else switch (instr) {
            case X:
                PUSH(x);
                break;
            case ADD: {
                int rhs = POP();
                int lhs = POP();
                PUSH(lhs + rhs);
                break;
            }
            case SUB: {
                int rhs = POP();
                int lhs = POP();
                PUSH(lhs - rhs);
                break;
            }
            case MUL: {
                int rhs = POP();
                int lhs = POP();
                PUSH(lhs * rhs);
                break;
            }
            case DIV: {
                int rhs = POP();
                int lhs = POP();
                PUSH(lhs / rhs);
                break;
            }
        }
    }
    assert(n == 1);
    return stack[0];
}

class ByteCodeCompiler {
    std::vector<Instruction> instructions;

public:
    void start() {}
    void handle_op(char op);
    void handle_number(int num) {
        instructions.push_back((Instruction) num);
    }
    void handle_x() {
        instructions.push_back(X);
    }
    std::vector<Instruction>&& finish() {
        return std::move(instructions);
    }
};

void ByteCodeCompiler::handle_op(char op) {
    switch (op) {
        case '+':
        instructions.push_back(ADD);
        return;

        case '-':
        instructions.push_back(SUB);
        return;

        case '*':
        instructions.push_back(MUL);
        return;

        case '/':
        instructions.push_back(DIV);
        return;
    }
    assert(false);
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    std::vector<Instruction> instructions(Parser<ByteCodeCompiler>().parse(line));

    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += run(instructions, x);
    }
    std::cout << sum << std::endl;
    return 0;
}