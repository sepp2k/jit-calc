#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "parser.hh"

class Evaluator {
    int x;
    std::vector<int> values;

public:
    Evaluator(int x) : x(x) {}
    void start() {}
    void handle_op(char op);
    void handle_number(int num) {
        values.push_back(num);
    }
    void handle_x() {
        values.push_back(x);
    }
    int finish() {
        assert(values.size() == 1);
        return values[0];
    }
};

 void Evaluator::handle_op(char op) {
    int rhs = pop(values);
    int lhs = pop(values);
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

int main() {
    std::string line;
    std::getline(std::cin, line);
    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += Parser<Evaluator>(x).parse(line);
    }
    std::cout << sum << std::endl;
    return 0;
}