#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "parser.hh"

class Expr {
public:
    virtual int eval(int x) = 0;
    virtual ~Expr() {}
};

class Num : public Expr {
    int num;
public:
    explicit Num(int num) : num(num) {}

    int eval(int x) override {
        return num;
    }
};

class X : public Expr {
public:
    int eval(int x) override {
        return x;
    }
};

class Infix : public Expr {
protected:
    Expr* lhs;
    Expr* rhs;
public:
    Infix(Expr* lhs, Expr* rhs) : lhs(lhs), rhs(rhs) {}
    virtual ~Infix() {
        delete lhs;
        delete rhs;
    }
};

class Add : public Infix {
public:
    Add(Expr* lhs, Expr* rhs) : Infix(lhs, rhs) {}

    int eval(int x) override {
        return lhs->eval(x) + rhs->eval(x);
    }
};

class Sub : public Infix {
public:
    Sub(Expr* lhs, Expr* rhs) : Infix(lhs, rhs) {}

    int eval(int x) override {
        return lhs->eval(x) - rhs->eval(x);
    }
};

class Mul : public Infix {
public:
    Mul(Expr* lhs, Expr* rhs) : Infix(lhs, rhs) {}

    int eval(int x) override {
        return lhs->eval(x) * rhs->eval(x);
    }
};

class Div : public Infix {
public:
    Div(Expr* lhs, Expr* rhs) : Infix(lhs, rhs) {}

    int eval(int x) override {
        return lhs->eval(x) / rhs->eval(x);
    }
};

class AstBuilder {
    std::vector<Expr*> nodes;

public:
    void start() {}
    void handle_op(char op);
    void handle_number(int num) {
        nodes.push_back(new Num(num));
    }
    void handle_x() {
        nodes.push_back(new X());
    }
    Expr* finish() {
        assert(nodes.size() == 1);
        return nodes[0];
    }
};

void AstBuilder::handle_op(char op) {
    Expr* rhs = pop(nodes);
    Expr* lhs = pop(nodes);
    switch (op) {
        case '+':
        nodes.push_back(new Add(lhs, rhs));
        break;

        case '-':
        nodes.push_back(new Sub(lhs, rhs));
        break;

        case '*':
        nodes.push_back(new Mul(lhs, rhs));
        break;

        case '/':
        nodes.push_back(new Div(lhs, rhs));
        break;
    }
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    Expr* expr = Parser<AstBuilder>().parse(line);
    
    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += expr->eval(x);
    }
    std::cout << sum << std::endl;
    delete expr;
    return 0;
}