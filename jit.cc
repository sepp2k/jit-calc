#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

extern "C" {
    #include <lightning.h>
}

static jit_state_t* _jit;

using JittedFunction = int(*)(int);

bool prec_gt(char op1, char op2) {
    switch (op1) {
        case '*': case '/':
        return true;

        case '+': case '-':
        return op2 == '+' || op2 == '-';
    }
    assert(false);
}

template<typename T> T pop(std::vector<T>& stack) {
    assert(stack.size() > 0);
    T value = stack[stack.size() - 1];
    stack.pop_back();
    return value;
}

enum {
    X = -1, TEMP = -2
};

class Parser {
    std::vector<char> operators;
    // -1: x (R0), -2: temporary, >=0: immediate
    std::vector<int> argument_stack;
    int used_registers = 0;

    char top_op() {
        return operators[operators.size() - 1];
    }

    char pop_op() {
        return pop(operators);
    }

    void handle_op(char op);

public:
    JittedFunction jit(const char* argv0, const std::string& expr);
};

void jitop_r(char op, int targetr, int lhsr, int rhsr) {
    switch (op) {
        case '+':
            jit_addr(JIT_R(targetr), JIT_R(lhsr), JIT_R(rhsr));
            return;
        case '-':
            jit_subr(JIT_R(targetr), JIT_R(lhsr), JIT_R(rhsr));
            return;
        case '*':
            jit_mulr(JIT_R(targetr), JIT_R(lhsr), JIT_R(rhsr));
            return;
        case '/':
            jit_divr(JIT_R(targetr), JIT_R(lhsr), JIT_R(rhsr));
            return;
    }
    assert(false);
}

void jitop_i(char op, int targetr, int lhsr, int rhsi) {
    switch (op) {
        case '+':
            jit_addi(JIT_R(targetr), JIT_R(lhsr), rhsi);
            return;
        case '-':
            jit_subi(JIT_R(targetr), JIT_R(lhsr), rhsi);
            return;
        case '*':
            jit_muli(JIT_R(targetr), JIT_R(lhsr), rhsi);
            return;
        case '/':
            jit_divi(JIT_R(targetr), JIT_R(lhsr), rhsi);
            return;
    }
    assert(false);
}

int const_eval(char op, int lhs, int rhs) {
    switch (op) {
        case '+':
            return lhs + rhs;
        case '-':
            return lhs - rhs;
        case '*':
            return lhs * rhs;
        case '/':
            return lhs / rhs;
    }
    assert(false);
}

void Parser::handle_op(char op) {
    int rhs = pop(argument_stack);
    int lhs = pop(argument_stack);
    if (lhs == TEMP) {
        if (rhs == TEMP) {
            int target = used_registers - 1;
            jitop_r(op, target, target, used_registers);
            used_registers--;
        } else if (rhs == X) {
            jitop_r(op, used_registers, used_registers, 0);
        } else {
            jitop_i(op, used_registers, used_registers, rhs);
        }
        argument_stack.push_back(TEMP);
    } else if (lhs == -1) {
        if (rhs == TEMP) {
            jitop_r(op, used_registers, 0, used_registers);
        } else if (rhs == X) {
            used_registers++;
            jitop_r(op, used_registers, 0, 0);
        } else {
            used_registers++;
            jitop_i(op, used_registers, 0, rhs);
        }
        argument_stack.push_back(TEMP);
    } else {
        if (rhs == TEMP) {
            int lhsr = used_registers + 1;
            jit_movi(lhsr, lhs);
            jitop_r(op, used_registers, lhsr, used_registers);
            argument_stack.push_back(TEMP);
        } else if (rhs == X) {
            used_registers++;
            int lhsr = used_registers + 1;
            jit_movi(lhsr, lhs);
            jitop_r(op, used_registers, lhsr, 0);
            argument_stack.push_back(TEMP);
        } else {
            argument_stack.push_back(const_eval(op, lhs, rhs));
        }
    }
}

JittedFunction Parser::jit(const char* argv0, const std::string& expr) {
    init_jit(argv0);
    _jit = jit_new_state();
    jit_prolog();
    jit_node_t* in = jit_arg();
    jit_getarg(JIT_R0, in);

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
                argument_stack.push_back(num);
                break;
            }

            case 'x':
                argument_stack.push_back(X);
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
    assert(used_registers == 1);
    jit_retr(JIT_R1);
    return (JittedFunction) jit_emit();
}

int main(int argc, char** argv) {
    std::string line;
    std::getline(std::cin, line);
    Parser parser;
    JittedFunction f = parser.jit(argv[0], line);
    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += f(x);
    }
    std::cout << sum << std::endl;
    return 0;
}