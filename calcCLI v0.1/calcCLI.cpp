#include <cmath>
#include <complex>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <datatypes.h>

// EXISTS:
// Lexer (tokenizer)
// AST (abstract syntax tree)
// Parser 
// (kind of) Tensor Data Types
// 

// TODO:
// Center Banner
// Adding matrix and vector handling
// Eplore AST -> lambda conversion
// 

void bootInfo() {
std::cout                                                                      
<<"                       _/                  _/_/_/  _/        _/_/_/   \n"
<<"    _/_/_/    _/_/_/  _/    _/_/_/      _/        _/          _/      \n"
<<" _/        _/    _/  _/  _/            _/        _/          _/       \n"
<<"_/        _/    _/  _/  _/            _/        _/          _/        \n"
<<" _/_/_/    _/_/_/  _/    _/_/_/        _/_/_/  _/_/_/_/  _/_/_/       \n"
<< "A light weight calculator and solver for your command line interface\nversion 0.0.1 \n\"calc is short for calculator by the way\"\n\n"
;
}

enum class TokenType {
    // Value classifications
    Number,
    Identifier,

    // Binary operators
    Plus,
    Minus,
    Multiply,
    Divide,
    Power,

    // Parenthesis
    LeftParen,
    RightParen,

    // Linear Algebra
    Vector,
    Matrix,

    // Misc
    Comma,
    End
};

struct Token {
    TokenType type;
    std::string text;
    double value;
    ccVector<double> vector(); 
    ccMatrix<double> matrix();

    std::size_t position = 0;
};

class Lexer {
public:
    explicit Lexer(std::string input): input(std::move(input)) {}

    Token nextToken() {
        skipSpaces();

        if (position >= input.size()) {
            return {TokenType::End, "", 0.0, position};
        }

        char c = input[position];

        // This code breaks with vectors
        if (std::isdigit(c) || c == '.') {
            return readNumber();
        }

        if (std::isalpha(c) || c == '_') {
            return readIdentifier();
        }

        if (c == '[') {
            return readTensor();
        }

        std::size_t start = position++;

        switch (c) {
            case '+': return {TokenType::Plus, "+", 0.0, start};
            case '-': return {TokenType::Minus, "-", 0.0, start};
            case '*': return {TokenType::Multiply, "*", 0.0, start};
            case '/': return {TokenType::Divide, "/", 0.0, start};
            case '^': return {TokenType::Power, "^", 0.0, start};
            case '(': return {TokenType::LeftParen, "(", 0.0, start};
            case ')': return {TokenType::RightParen, ")", 0.0, start};
            case ',': return {TokenType::Comma, ",", 0.0, start};

            default:
                throw std::runtime_error(
                    "Unknown character at position " + std::to_string(start)
                );
        }
    }

private:
    std::string input;
    std::size_t position = 0;

    void skipSpaces() {
        while (position < input.size() && std::isspace(static_cast<unsigned char>(input[position]))) {
            ++position;
        }
    }

    Token readNumber() {
        std::size_t start = position;

        while (position < input.size() &&
              (std::isdigit(static_cast<unsigned char>(input[position])) || input[position] == '.')) {
            ++position;
        }

        std::string text = input.substr(start, position - start);
        return {TokenType::Number, text, std::stod(text), start};
    }

    Token readIdentifier() {
        std::size_t start = position;

        while (position < input.size() && (std::isalnum(static_cast<unsigned char>(input[position])) || input[position] == '_')) {
            ++position;
        }

        std::string text = input.substr(start, position - start);
        return {TokenType::Identifier, text, 0.0, start};
    }

    Token readTensor() {
        std::size_t start = position;
        bool isMatrix = false; 
        
        while (position < input.size() && (std::isdigit(static_cast<unsigned char>(input[position])) || input[position] == '[' || input[position] == ']' || input[position] == ';')) {
            if (input[position] == ';') 
                isMatrix = true;
            ++position;
        }

    }
};

// ---------------- AST ----------------

struct Expr {
    virtual ~Expr() = default;
    virtual double eval(const std::unordered_map<std::string, double>& vars) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

struct NumberExpr : Expr {
    double value;

    explicit NumberExpr(double value) : value(value) {}

    double eval(const std::unordered_map<std::string, double>&) const override {
        return value;
    }
};

struct VariableExpr : Expr {
    std::string name;

    explicit VariableExpr(std::string name) : name(std::move(name)) {}

    double eval(const std::unordered_map<std::string, double>& vars) const override {
        if (name == "pi") return M_PI;
        if (name == "e") return M_E;

        auto it = vars.find(name);
        if (it == vars.end())
            throw std::runtime_error("Unknown variable: " + name);

        return it->second;
    }
};

struct UnaryExpr : Expr {
    TokenType op;
    ExprPtr rhs;

    UnaryExpr(TokenType op, ExprPtr rhs)
        : op(op), rhs(std::move(rhs)) {}

    double eval(const std::unordered_map<std::string, double>& vars) const override {
        double x = rhs->eval(vars);

        if (op == TokenType::Minus) return -x;
        if (op == TokenType::Plus) return x;

        throw std::runtime_error("Unknown unary operator");
    }
};

struct BinaryExpr : Expr {
    TokenType op;
    ExprPtr lhs;
    ExprPtr rhs;

    BinaryExpr(TokenType op, ExprPtr lhs, ExprPtr rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    double eval(const std::unordered_map<std::string, double>& vars) const override {
        double a = lhs->eval(vars);
        double b = rhs->eval(vars);

        switch (op) {
            case TokenType::Plus:     return a + b;
            case TokenType::Minus:    return a - b;
            case TokenType::Multiply: return a * b;
            case TokenType::Divide:   return a / b;
            case TokenType::Power:    return std::pow(a, b);
            default:
                throw std::runtime_error("Unknown binary operator");
        }
    }
};

struct FunctionExpr : Expr {
    std::string name;
    std::vector<ExprPtr> args;

    FunctionExpr(std::string name, std::vector<ExprPtr> args)
        : name(std::move(name)), args(std::move(args)) {}

    double eval(const std::unordered_map<std::string, double>& vars) const override {
        std::vector<double> values;
        for (const auto& arg : args)
            values.push_back(arg->eval(vars));

        if (name == "sin")   return std::sin(values.at(0));
        if (name == "cos")   return std::cos(values.at(0));
        if (name == "tan")   return std::tan(values.at(0));
        if (name == "log" || name == "ln")   return std::log(values.at(0));
        if (name == "exp")   return std::exp(values.at(0));
        if (name == "sqrt")  return std::sqrt(values.at(0));
        if (name == "gamma") return std::tgamma(values.at(0));

        throw std::runtime_error("Unknown function: " + name);
    }
};

// ---------------- Parser ----------------

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : tokens_(std::move(tokens)) {}

    ExprPtr parse() {
        ExprPtr expr = expression(0);

        if (peek().type != TokenType::End)
            throw std::runtime_error("Unexpected token: " + peek().text);

        return expr;
    }

private:
    std::vector<Token> tokens_;
    std::size_t pos_ = 0;

    const Token& peek() const {
        return tokens_[pos_];
    }

    Token advance() {
        return tokens_[pos_++];
    }

    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    int precedence(TokenType type) const {
        switch (type) {
            case TokenType::Plus:
            case TokenType::Minus:
                return 10;

            case TokenType::Multiply:
            case TokenType::Divide:
                return 20;

            case TokenType::Power:
                return 30;

            default:
                return -1;
        }
    }

    bool is_right_assoc(TokenType type) const {
        return type == TokenType::Power;
    }

    bool is_binary_op(TokenType type) const {
        return precedence(type) != -1;
    }

    ExprPtr expression(int min_prec) {
        ExprPtr lhs = prefix();

        while (is_binary_op(peek().type) &&
               precedence(peek().type) >= min_prec) {
            Token op = advance();

            int prec = precedence(op.type);
            int next_min_prec = is_right_assoc(op.type) ? prec : prec + 1;

            ExprPtr rhs = expression(next_min_prec);

            lhs = std::make_unique<BinaryExpr>(
                op.type,
                std::move(lhs),
                std::move(rhs)
            );
        }

        return lhs;
    }

    ExprPtr prefix() {
        Token tok = advance();

        switch (tok.type) {
            case TokenType::Number:
                return std::make_unique<NumberExpr>(tok.value);

            case TokenType::Identifier: {
                if (match(TokenType::LeftParen)) {
                    std::vector<ExprPtr> args;

                    if (!match(TokenType::RightParen)) {
                        do {
                            args.push_back(expression(0));
                        } while (match(TokenType::Comma));

                        if (!match(TokenType::RightParen))
                            throw std::runtime_error("Expected ')'");
                    }
                    return std::make_unique<FunctionExpr>(tok.text, std::move(args));
                }
                return std::make_unique<VariableExpr>(tok.text);
            }

            case TokenType::Minus:
                return std::make_unique<UnaryExpr>(
                    TokenType::Minus,
                    expression(25)
                );

            case TokenType::Plus:
                return std::make_unique<UnaryExpr>(
                    TokenType::Plus,
                    expression(25)
                );

            case TokenType::LeftParen: {
                ExprPtr inside = expression(0);

                if (!match(TokenType::RightParen))
                    throw std::runtime_error("Expected ')'");

                return inside;
            }

            default:
                throw std::runtime_error("Unexpected token: " + tok.text);
        }
    }
};

// ---------------- Helpers ----------------

std::vector<Token> lex_all(const std::string& input) {
    Lexer lexer(input);
    std::vector<Token> tokens;

    while (true) {
        Token tok = lexer.nextToken();
        tokens.push_back(tok);

        if (tok.type == TokenType::End)
            break;
    }

    return tokens;
}


int main(){

    // Key: int satus
    // 2 => first use
    // 1 => in active use (don't close)
    // 3 => debug/admin mode

    bootInfo();
    int status = 2;
    do {
        if (status == 2){
            std::string userInput {};
            std::getline(std::cin, userInput);
            if(userInput == "exit" || userInput == "Exit" || userInput == "exit()" || userInput == "ext") status = 1;

            if(userInput == "admin"){ 
                status = 3;
                std::cout << "Debug Mode is Active"; 
            }
            else{
                status = 0;
                try {
                    auto tokens = lex_all(userInput);
                    Parser parser(std::move(tokens));
                    ExprPtr ast = parser.parse();
                    std::unordered_map<std::string, double> vars;
                    vars["x"] = 2.0;
                    double result = ast->eval(vars);
                    std::cout << result << '\n';
                }
                catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << '\n';
                }
            }
        }
        else if (status == 0){
            std::string userInput {};
            std::getline(std::cin, userInput);
            if(userInput == "exit" || userInput == "Exit" || userInput == "exit()" || userInput == "ext") {
                status = 1;
            }
            else{
                try {
                    auto tokens = lex_all(userInput);
                    Parser parser(std::move(tokens));
                    ExprPtr ast = parser.parse();
                    std::unordered_map<std::string, double> vars;
                    vars["x"] = 2.0;
                    double result = ast->eval(vars);
                    std::cout << result << '\n';
                }
                catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << '\n';
                }
            }
        }
    } while ( status != 1 );
    std::cout << "Ending Session";
    return 0;
}