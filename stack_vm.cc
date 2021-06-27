#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

bool prec_gt(char op1, char op2) {
    switch (op1) {
        case '*': case '/':
        return true;

        case '+': case '-':
        return op2 == '+' || op2 == '-';
    }
    assert(false);
}

// Since all integer constants are positive, we use positive ints to represent constant pushs and
// negative ints to represent all other instructions
enum Instruction : int { X=-1, ADD=-2, SUB=-3, MUL=-4, DIV=-5 };

template<typename T> T pop(std::vector<T>& stack) {
    assert(stack.size() > 0);
    T value = stack[stack.size() - 1];
    stack.pop_back();
    return value;
}

int run(const std::vector<Instruction>& program, int x) {
    std::vector<int> stack;
    stack.reserve(32);
    for (Instruction instr : program) {
        if (instr >= 0) {
            stack.push_back(instr);
        } else switch (instr) {
            case X:
                stack.push_back(x);
                break;
            case ADD: {
                int rhs = pop(stack);
                int lhs = pop(stack);
                stack.push_back(lhs + rhs);
                break;
            }
            case SUB: {
                int rhs = pop(stack);
                int lhs = pop(stack);
                stack.push_back(lhs - rhs);
                break;
            }
            case MUL: {
                int rhs = pop(stack);
                int lhs = pop(stack);
                stack.push_back(lhs * rhs);
                break;
            }
            case DIV: {
                int rhs = pop(stack);
                int lhs = pop(stack);
                stack.push_back(lhs / rhs);
                break;
            }
        }
    }
    assert(stack.size() == 1);
    return stack[0];
}

class Parser {
    std::vector<char> operators;
    std::vector<Instruction> instructions;

    char top_op() {
        return operators[operators.size() - 1];
    }

    char pop_op() {
        return pop(operators);
    }

    void handle_op(char op) {
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

public:
    void parse(const std::string& expr);
    const std::vector<Instruction>& get_instructions() {
        return instructions;
    }
};

void Parser::parse(const std::string& expr) {
    size_t i = 0;
    while (i < expr.length()) {
        char c = expr[i];
        switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                int num = c - '0';
                while (isdigit(c = expr[++i])) {
                    num *= 10;
                    num += c - '0';
                }
                instructions.push_back((Instruction) num);
                break;
            }

            case 'x':
                instructions.push_back(X);
                c = expr[++i];
                break;

            case '+': case '-': case '*': case '/': {
                char op;
                while (operators.size() != 0 && (op = top_op()) != '(' && prec_gt(op, c)) {
                    handle_op(op);
                    operators.pop_back();
                }
                operators.push_back(c);
                c = expr[++i];
                break;
            }

            case '(':
                operators.push_back('(');
                c = expr[++i];
                break;

            case ')': {
                char op = pop_op();
                while (op != '(') {
                    handle_op(op);
                    op = pop_op();
                }
                c = expr[++i];
                break;
            }

            case ' ': case '\t': case '\r': case '\n':
                c = expr[++i];
                break;

            default:
                std::cerr << "Unexpected character '" << c << "'\n";
                c = expr[++i];
                break;
        }
    }
    while (operators.size() > 0) {
        handle_op(pop_op());
    }
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    Parser parser;
    parser.parse(line);
    int x;
    while (std::cin >> x) {
        std::cout << run(parser.get_instructions(), x) << std::endl;
    }
    return 0;
}