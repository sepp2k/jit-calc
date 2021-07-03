#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

inline bool prec_gt(char op1, char op2) {
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

template<class TListener>
class Parser {
    TListener listener;
    using ReturnType = decltype(listener.finish());
    std::vector<char> operators;

public:
    template<typename ...Args>
    Parser(Args... args) : listener(args...) {}

    ReturnType parse(const std::string& expr);
};

template<class TListener>
typename Parser<TListener>::ReturnType Parser<TListener>::parse(const std::string& expr) {
    listener.start();

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
                listener.handle_number(num);
                break;
            }

            case 'x':
                listener.handle_x();
                c = expr[++i];
                break;

            case '+': case '-': case '*': case '/': {
                char op;
                while (operators.size() != 0 && (op = operators.back()) != '(' && prec_gt(op, c)) {
                    listener.handle_op(op);
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
                char op = pop(operators);
                while (op != '(') {
                    listener.handle_op(op);
                    op = pop(operators);
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
        listener.handle_op(pop(operators));
    }
    return listener.finish();
}