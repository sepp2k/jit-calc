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

class Evaluator {
    std::vector<char> operators;
    std::vector<int> values;

    char top_op() {
        return operators[operators.size() - 1];
    }

    char pop_op() {
        char op = top_op();
        operators.pop_back();
        return op;
    }

    int pop_value() {
        int value = values[values.size() - 1];
        values.pop_back();
        return value;
    }

    void eval_op(char op) {
        int rhs = pop_value();
        int lhs = pop_value();
        switch (op) {
            case '+':
            values.push_back(lhs + rhs);
            return;

            case '-':
            values.push_back(lhs - rhs);
            return;

            case '*':
            values.push_back(lhs * rhs);
            return;

            case '/':
            values.push_back(lhs / rhs);
            return;
        }
        assert(false);
    }

public:
    int parse_eval(const std::string& expr, int x);
};

int Evaluator::parse_eval(const std::string& expr, int x) {
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
                values.push_back(num);
                break;
            }

            case 'x':
                values.push_back(x);
                c = expr[++i];
                break;

            case '+': case '-': case '*': case '/': {
                char op;
                while (operators.size() != 0 && (op = top_op()) != '(' && prec_gt(op, c)) {
                    eval_op(op);
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
                    eval_op(op);
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
        eval_op(pop_op());
    }
    assert(values.size() == 1);
    return values[0];
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += Evaluator().parse_eval(line, x);
    }
    std::cout << sum << std::endl;
    return 0;
}