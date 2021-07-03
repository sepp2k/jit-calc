#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "parser.hh"

extern "C" {
    #include <lightning.h>
}

static jit_state_t* _jit;

using JittedFunction = int(*)(int);

enum {
    X = -1, TEMP = -2
};

class Jitter {
    // -1: x (R0), -2: temporary, >=0: immediate
    std::vector<int> argument_stack;
    int used_registers = 0;
    const char* argv0;

public:
    Jitter(const char* argv0) : argv0(argv0) {}
    void start();
    void handle_op(char op);
    void handle_number(int num) {
        argument_stack.push_back(num);
    }
    void handle_x() {
        argument_stack.push_back(X);
    }
    JittedFunction finish();
};

void Jitter::start() {
    init_jit(argv0);
    _jit = jit_new_state();
    jit_prolog();
    jit_node_t* in = jit_arg();
    jit_getarg(JIT_R0, in);
}

JittedFunction Jitter::finish() {
    assert(used_registers == 1);
    jit_retr(JIT_R1);
    return (JittedFunction) jit_emit();
}

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

void Jitter::handle_op(char op) {
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

int main(int argc, char** argv) {
    std::string line;
    std::getline(std::cin, line);
    JittedFunction f = Parser<Jitter>(argv[0]).parse(line);
    
    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += f(x);
    }
    std::cout << sum << std::endl;
    return 0;
}