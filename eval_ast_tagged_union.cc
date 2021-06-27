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

class Expr {
public:
    enum Tag { X, NUM, ADD, SUB, MUL, DIV };

private:
    Tag tag;
    union {
        int num;
        struct {
            Expr* lhs;
            Expr* rhs;
        };
    };

public:
    Expr() : tag(X) {}
    Expr(int num) : tag(NUM), num(num) {}
    Expr(Tag tag, Expr* lhs, Expr* rhs) : tag(tag), lhs(lhs), rhs(rhs) {
        assert(tag >= ADD);
    }

    int eval(int x);

    ~Expr() {
        if (tag >= ADD) {
            delete lhs;
            delete rhs;
        }
    }
};

int Expr::eval(int x) {
    switch (tag) {
        case X: return x;
        case NUM: return num;
        case ADD: return lhs->eval(x) + rhs->eval(x);
        case SUB: return lhs->eval(x) - rhs->eval(x);
        case MUL: return lhs->eval(x) * rhs->eval(x);
        case DIV: return lhs->eval(x) / rhs->eval(x);
    }
    assert(false);
}

class Parser {
    std::vector<char> operators;
    std::vector<Expr*> values;

    char top_op() {
        return operators[operators.size() - 1];
    }

    char pop_op() {
        char op = top_op();
        operators.pop_back();
        return op;
    }

    Expr* pop_value() {
        Expr* value = values[values.size() - 1];
        values.pop_back();
        return value;
    }

    void handle_op(char op) {
        Expr* rhs = pop_value();
        Expr* lhs = pop_value();
        switch (op) {
            case '+':
            values.push_back(new Expr(Expr::ADD, lhs, rhs));
            break;

            case '-':
            values.push_back(new Expr(Expr::SUB, lhs, rhs));
            break;

            case '*':
            values.push_back(new Expr(Expr::MUL, lhs, rhs));
            break;

            case '/':
            values.push_back(new Expr(Expr::DIV, lhs, rhs));
            break;
        }
    }

public:
    Expr* parse(const std::string& expr);
};

Expr* Parser::parse(const std::string& expr) {
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
                values.push_back(new Expr(num));
                break;
            }

            case 'x':
                values.push_back(new Expr());
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
    assert(values.size() == 1);
    return values[0];
}

int main() {
    std::string line;
    std::getline(std::cin, line);
    Expr* expr = Parser().parse(line);
    int x;
    unsigned int sum = 0;
    while (std::cin >> x) {
        sum += expr->eval(x);
    }
    std::cout << sum << std::endl;
    delete expr;
    return 0;
}