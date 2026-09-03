#include <cmath>
#include <complex>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

#include "tensorlib.h"

// EXISTS:
// Lexer (tokenizer)
// AST (abstract syntax tree)
// Parser
// (kind of) Tensor Data Types
// Session variables
// Value + / * (scalar, vector, matrix)

// TODO:
// Center Banner
// Explore AST -> lambda conversion

void bootInfo() {
std::cout
<<"                       _/                  _/_/_/  _/        _/_/_/   \n"
<<"    _/_/_/    _/_/_/  _/    _/_/_/      _/        _/          _/      \n"
<<" _/        _/    _/  _/  _/            _/        _/          _/       \n"
<<"_/        _/    _/  _/  _/            _/        _/          _/        \n"
<<" _/_/_/    _/_/_/  _/    _/_/_/        _/_/_/  _/_/_/_/  _/_/_/       \n"
<< "A light weight calculator and solver for your command line interface\nversion 0.1.1 \n\"calc is short for calculator by the way\"\n\n"
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
    Assign,

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
    TokenType type = TokenType::End;
    std::string text;
    double value = 0.0;
    ccVector<double> vector;
    ccMatrix<double> matrix;
    std::size_t position = 0;
};

// ---------------- Values ----------------

struct Value {
    enum class Kind { Scalar, Vector, Matrix } kind = Kind::Scalar;
    double scalar = 0.0;
    ccVector<double> vector;
    ccMatrix<double> matrix;

    static Value fromScalar(double x) {
        Value v;
        v.kind = Kind::Scalar;
        v.scalar = x;
        return v;
    }

    static Value fromVector(const ccVector<double>& vec) {
        Value v;
        v.kind = Kind::Vector;
        v.vector = vec;
        return v;
    }

    static Value fromMatrix(const ccMatrix<double>& mat) {
        Value v;
        v.kind = Kind::Matrix;
        v.matrix = mat;
        return v;
    }

    double asScalar(const std::string& context) const {
        if (kind != Kind::Scalar)
            throw std::runtime_error(context + " expects a scalar");
        return scalar;
    }
};

std::ostream& operator<<(std::ostream& os, const Value& value) {
    if (value.kind == Value::Kind::Scalar) {
        os << value.scalar;
        return os;
    }

    if (value.kind == Value::Kind::Vector) {
        os << '[';
        for (int i = 0; i < value.vector.size(); ++i) {
            if (i > 0)
                os << ' ';
            os << value.vector[i];
        }
        os << ']';
        return os;
    }

    os << '[';
    for (int i = 0; i < value.matrix.nrows(); ++i) {
        if (i > 0)
            os << "; ";
        for (int j = 0; j < value.matrix.ncols(); ++j) {
            if (j > 0)
                os << ' ';
            os << value.matrix[i][j];
        }
    }
    os << ']';
    return os;
}

// ---------------- Environment ----------------

struct Environment {
    std::unordered_map<std::string, Value> vars;
    // Future: user-defined functions, e.g.
    // struct UserFunction { std::vector<std::string> params; ExprPtr body; };
    // std::unordered_map<std::string, UserFunction> functions;

    Value get(const std::string& name) const {
        if (name == "pi") return Value::fromScalar(M_PI);
        if (name == "e") return Value::fromScalar(M_E);

        auto it = vars.find(name);
        if (it == vars.end())
            throw std::runtime_error("Unknown variable: " + name);
        return it->second;
    }

    void set(const std::string& name, Value value) {
        if (name == "pi" || name == "e")
            throw std::runtime_error("Cannot assign to constant '" + name + "'");
        vars[name] = std::move(value);
    }
};

// ---------------- Linear algebra ops ----------------

namespace ops {

Value negate(const Value& a) {
    if (a.kind == Value::Kind::Scalar)
        return Value::fromScalar(-a.scalar);

    if (a.kind == Value::Kind::Vector) {
        ccVector<double> out(a.vector.size());
        for (int i = 0; i < a.vector.size(); ++i)
            out[i] = -a.vector[i];
        return Value::fromVector(out);
    }

    ccMatrix<double> out(a.matrix.nrows(), a.matrix.ncols());
    for (int i = 0; i < a.matrix.nrows(); ++i)
        for (int j = 0; j < a.matrix.ncols(); ++j)
            out[i][j] = -a.matrix[i][j];
    return Value::fromMatrix(out);
}

Value add(const Value& a, const Value& b) {
    // Scalar + scalar
    if (a.kind == Value::Kind::Scalar && b.kind == Value::Kind::Scalar)
        return Value::fromScalar(a.scalar + b.scalar);

    // Scalar broadcast onto vector / matrix
    if (a.kind == Value::Kind::Scalar && b.kind == Value::Kind::Vector) {
        ccVector<double> out(b.vector.size());
        for (int i = 0; i < b.vector.size(); ++i)
            out[i] = a.scalar + b.vector[i];
        return Value::fromVector(out);
    }
    if (a.kind == Value::Kind::Vector && b.kind == Value::Kind::Scalar) {
        ccVector<double> out(a.vector.size());
        for (int i = 0; i < a.vector.size(); ++i)
            out[i] = a.vector[i] + b.scalar;
        return Value::fromVector(out);
    }
    if (a.kind == Value::Kind::Scalar && b.kind == Value::Kind::Matrix) {
        ccMatrix<double> out(b.matrix.nrows(), b.matrix.ncols());
        for (int i = 0; i < b.matrix.nrows(); ++i)
            for (int j = 0; j < b.matrix.ncols(); ++j)
                out[i][j] = a.scalar + b.matrix[i][j];
        return Value::fromMatrix(out);
    }
    if (a.kind == Value::Kind::Matrix && b.kind == Value::Kind::Scalar) {
        ccMatrix<double> out(a.matrix.nrows(), a.matrix.ncols());
        for (int i = 0; i < a.matrix.nrows(); ++i)
            for (int j = 0; j < a.matrix.ncols(); ++j)
                out[i][j] = a.matrix[i][j] + b.scalar;
        return Value::fromMatrix(out);
    }

    // Vector + vector
    if (a.kind == Value::Kind::Vector && b.kind == Value::Kind::Vector) {
        if (a.vector.size() != b.vector.size())
            throw std::runtime_error("Vector addition requires equal lengths");
        ccVector<double> out(a.vector.size());
        for (int i = 0; i < a.vector.size(); ++i)
            out[i] = a.vector[i] + b.vector[i];
        return Value::fromVector(out);
    }

    // Matrix + matrix
    if (a.kind == Value::Kind::Matrix && b.kind == Value::Kind::Matrix) {
        if (a.matrix.nrows() != b.matrix.nrows() ||
            a.matrix.ncols() != b.matrix.ncols()) {
            throw std::runtime_error("Matrix addition requires equal dimensions");
        }
        ccMatrix<double> out(a.matrix.nrows(), a.matrix.ncols());
        for (int i = 0; i < a.matrix.nrows(); ++i)
            for (int j = 0; j < a.matrix.ncols(); ++j)
                out[i][j] = a.matrix[i][j] + b.matrix[i][j];
        return Value::fromMatrix(out);
    }

    throw std::runtime_error("Unsupported operands for '+'");
}

Value multiply(const Value& a, const Value& b) {
    // Scalar * scalar
    if (a.kind == Value::Kind::Scalar && b.kind == Value::Kind::Scalar)
        return Value::fromScalar(a.scalar * b.scalar);

    // Scalar scaling
    if (a.kind == Value::Kind::Scalar && b.kind == Value::Kind::Vector) {
        ccVector<double> out(b.vector.size());
        for (int i = 0; i < b.vector.size(); ++i)
            out[i] = a.scalar * b.vector[i];
        return Value::fromVector(out);
    }
    if (a.kind == Value::Kind::Vector && b.kind == Value::Kind::Scalar) {
        ccVector<double> out(a.vector.size());
        for (int i = 0; i < a.vector.size(); ++i)
            out[i] = a.vector[i] * b.scalar;
        return Value::fromVector(out);
    }
    if (a.kind == Value::Kind::Scalar && b.kind == Value::Kind::Matrix) {
        ccMatrix<double> out(b.matrix.nrows(), b.matrix.ncols());
        for (int i = 0; i < b.matrix.nrows(); ++i)
            for (int j = 0; j < b.matrix.ncols(); ++j)
                out[i][j] = a.scalar * b.matrix[i][j];
        return Value::fromMatrix(out);
    }
    if (a.kind == Value::Kind::Matrix && b.kind == Value::Kind::Scalar) {
        ccMatrix<double> out(a.matrix.nrows(), a.matrix.ncols());
        for (int i = 0; i < a.matrix.nrows(); ++i)
            for (int j = 0; j < a.matrix.ncols(); ++j)
                out[i][j] = a.matrix[i][j] * b.scalar;
        return Value::fromMatrix(out);
    }

    // Dot product (row · row convenience; same length)
    if (a.kind == Value::Kind::Vector && b.kind == Value::Kind::Vector) {
        if (a.vector.size() != b.vector.size())
            throw std::runtime_error("Dot product requires equal lengths");
        double sum = 0.0;
        for (int i = 0; i < a.vector.size(); ++i)
            sum += a.vector[i] * b.vector[i];
        return Value::fromScalar(sum);
    }

    // Matrix * matrix
    if (a.kind == Value::Kind::Matrix && b.kind == Value::Kind::Matrix) {
        if (a.matrix.ncols() != b.matrix.nrows()) {
            throw std::runtime_error(
                "Matrix multiply requires lhs.ncols == rhs.nrows"
            );
        }
        ccMatrix<double> out(a.matrix.nrows(), b.matrix.ncols());
        for (int i = 0; i < a.matrix.nrows(); ++i) {
            for (int j = 0; j < b.matrix.ncols(); ++j) {
                double sum = 0.0;
                for (int k = 0; k < a.matrix.ncols(); ++k)
                    sum += a.matrix[i][k] * b.matrix[k][j];
                out[i][j] = sum;
            }
        }
        return Value::fromMatrix(out);
    }

    // Matrix * vector  (vector as column)
    if (a.kind == Value::Kind::Matrix && b.kind == Value::Kind::Vector) {
        if (a.matrix.ncols() != b.vector.size()) {
            throw std::runtime_error(
                "Matrix*vector requires matrix.ncols == vector.size"
            );
        }
        ccVector<double> out(a.matrix.nrows());
        for (int i = 0; i < a.matrix.nrows(); ++i) {
            double sum = 0.0;
            for (int k = 0; k < a.matrix.ncols(); ++k)
                sum += a.matrix[i][k] * b.vector[k];
            out[i] = sum;
        }
        return Value::fromVector(out);
    }

    // Vector * matrix  (vector as row)
    if (a.kind == Value::Kind::Vector && b.kind == Value::Kind::Matrix) {
        if (a.vector.size() != b.matrix.nrows()) {
            throw std::runtime_error(
                "Vector*matrix requires vector.size == matrix.nrows"
            );
        }
        ccVector<double> out(b.matrix.ncols());
        for (int j = 0; j < b.matrix.ncols(); ++j) {
            double sum = 0.0;
            for (int k = 0; k < a.vector.size(); ++k)
                sum += a.vector[k] * b.matrix[k][j];
            out[j] = sum;
        }
        return Value::fromVector(out);
    }

    throw std::runtime_error("Unsupported operands for '*'");
}

} // namespace ops

// ---------------- Lexer ----------------

class Lexer {
public:
    explicit Lexer(std::string input) : input(std::move(input)) {}

    Token nextToken() {
        skipSpaces();

        if (position >= input.size()) {
            Token tok;
            tok.type = TokenType::End;
            tok.position = position;
            return tok;
        }

        char c = input[position];

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            return readNumber();
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            return readIdentifier();
        }

        if (c == '[') {
            return readTensor();
        }

        std::size_t start = position++;

        Token tok;
        tok.position = start;

        switch (c) {
            case '+': tok.type = TokenType::Plus; tok.text = "+"; return tok;
            case '-': tok.type = TokenType::Minus; tok.text = "-"; return tok;
            case '*': tok.type = TokenType::Multiply; tok.text = "*"; return tok;
            case '/': tok.type = TokenType::Divide; tok.text = "/"; return tok;
            case '^': tok.type = TokenType::Power; tok.text = "^"; return tok;
            case '=': tok.type = TokenType::Assign; tok.text = "="; return tok;
            case '(': tok.type = TokenType::LeftParen; tok.text = "("; return tok;
            case ')': tok.type = TokenType::RightParen; tok.text = ")"; return tok;
            case ',': tok.type = TokenType::Comma; tok.text = ","; return tok;
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
        while (position < input.size() &&
               std::isspace(static_cast<unsigned char>(input[position]))) {
            ++position;
        }
    }

    Token readNumber() {
        std::size_t start = position;

        while (position < input.size() &&
               (std::isdigit(static_cast<unsigned char>(input[position])) ||
                input[position] == '.')) {
            ++position;
        }

        std::string text = input.substr(start, position - start);
        Token tok;
        tok.type = TokenType::Number;
        tok.text = text;
        tok.value = std::stod(text);
        tok.position = start;
        return tok;
    }

    Token readIdentifier() {
        std::size_t start = position;

        while (position < input.size() &&
               (std::isalnum(static_cast<unsigned char>(input[position])) ||
                input[position] == '_')) {
            ++position;
        }

        Token tok;
        tok.type = TokenType::Identifier;
        tok.text = input.substr(start, position - start);
        tok.position = start;
        return tok;
    }

    // Optional leading +/- for tensor entries (MATLAB-style literals).
    double readSignedNumberLiteral() {
        skipSpaces();
        if (position >= input.size()) {
            throw std::runtime_error("Expected number inside [...]");
        }

        int sign = 1;
        if (input[position] == '+' || input[position] == '-') {
            if (input[position] == '-')
                sign = -1;
            ++position;
            skipSpaces();
        }

        if (position >= input.size() ||
            !(std::isdigit(static_cast<unsigned char>(input[position])) ||
              input[position] == '.')) {
            throw std::runtime_error(
                "Expected number inside [...] at position " + std::to_string(position)
            );
        }

        std::size_t start = position;
        while (position < input.size() &&
               (std::isdigit(static_cast<unsigned char>(input[position])) ||
                input[position] == '.')) {
            ++position;
        }

        return sign * std::stod(input.substr(start, position - start));
    }

    Token readTensor() {
        std::size_t start = position;
        ++position; // consume '['

        std::vector<std::vector<double>> rows;
        std::vector<double> currentRow;
        bool sawSemicolon = false;

        skipSpaces();
        while (position < input.size() && input[position] != ']') {
            skipSpaces();
            if (position >= input.size())
                break;

            if (input[position] == ';') {
                if (currentRow.empty()) {
                    throw std::runtime_error(
                        "Empty matrix row at position " + std::to_string(position)
                    );
                }
                rows.push_back(currentRow);
                currentRow.clear();
                sawSemicolon = true;
                ++position;
                skipSpaces();
                continue;
            }

            currentRow.push_back(readSignedNumberLiteral());
            skipSpaces();

            if (position < input.size() && input[position] == ',') {
                ++position;
            }
        }

        if (position >= input.size() || input[position] != ']') {
            throw std::runtime_error(
                "Unclosed '[' starting at position " + std::to_string(start)
            );
        }
        ++position; // consume ']'

        if (!currentRow.empty()) {
            rows.push_back(currentRow);
        }

        if (rows.empty()) {
            throw std::runtime_error("Empty [] is not a vector or matrix");
        }

        const int ncols = static_cast<int>(rows[0].size());
        if (ncols == 0) {
            throw std::runtime_error("Empty matrix row");
        }
        for (std::size_t r = 1; r < rows.size(); ++r) {
            if (static_cast<int>(rows[r].size()) != ncols) {
                throw std::runtime_error(
                    "Matrix rows must have the same number of columns"
                );
            }
        }

        Token tok;
        tok.position = start;
        tok.text = input.substr(start, position - start);

        if (!sawSemicolon && rows.size() == 1) {
            tok.type = TokenType::Vector;
            tok.vector = ccVector<double>(ncols);
            for (int j = 0; j < ncols; ++j)
                tok.vector[j] = rows[0][static_cast<std::size_t>(j)];
            return tok;
        }

        const int nrows = static_cast<int>(rows.size());
        tok.type = TokenType::Matrix;
        tok.matrix = ccMatrix<double>(nrows, ncols);
        for (int i = 0; i < nrows; ++i)
            for (int j = 0; j < ncols; ++j)
                tok.matrix[i][j] = rows[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)];
        return tok;
    }
};

// ---------------- AST ----------------

struct Expr {
    virtual ~Expr() = default;
    virtual Value eval(Environment& env) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;

struct NumberExpr : Expr {
    double value;

    explicit NumberExpr(double value) : value(value) {}

    Value eval(Environment&) const override {
        return Value::fromScalar(value);
    }
};

struct VariableExpr : Expr {
    std::string name;

    explicit VariableExpr(std::string name) : name(std::move(name)) {}

    Value eval(Environment& env) const override {
        return env.get(name);
    }
};

struct VectorExpr : Expr {
    ccVector<double> vector;

    explicit VectorExpr(ccVector<double> vector) : vector(std::move(vector)) {}

    Value eval(Environment&) const override {
        return Value::fromVector(vector);
    }
};

struct MatrixExpr : Expr {
    ccMatrix<double> matrix;

    explicit MatrixExpr(ccMatrix<double> matrix) : matrix(std::move(matrix)) {}

    Value eval(Environment&) const override {
        return Value::fromMatrix(matrix);
    }
};

struct AssignExpr : Expr {
    std::string name;
    ExprPtr rhs;

    AssignExpr(std::string name, ExprPtr rhs)
        : name(std::move(name)), rhs(std::move(rhs)) {}

    Value eval(Environment& env) const override {
        Value value = rhs->eval(env);
        env.set(name, value);
        return value;
    }
};

struct UnaryExpr : Expr {
    TokenType op;
    ExprPtr rhs;

    UnaryExpr(TokenType op, ExprPtr rhs)
        : op(op), rhs(std::move(rhs)) {}

    Value eval(Environment& env) const override {
        Value x = rhs->eval(env);

        if (op == TokenType::Minus) return ops::negate(x);
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

    Value eval(Environment& env) const override {
        Value a = lhs->eval(env);
        Value b = rhs->eval(env);

        switch (op) {
            case TokenType::Plus:
                return ops::add(a, b);
            case TokenType::Multiply:
                return ops::multiply(a, b);
            case TokenType::Minus: {
                // a - b  ==  a + (-b); keeps one code path for tensors
                return ops::add(a, ops::negate(b));
            }
            case TokenType::Divide: {
                double x = a.asScalar("Division");
                double y = b.asScalar("Division");
                return Value::fromScalar(x / y);
            }
            case TokenType::Power: {
                double x = a.asScalar("Power");
                double y = b.asScalar("Power");
                return Value::fromScalar(std::pow(x, y));
            }
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

    Value eval(Environment& env) const override {
        // Builtins first. Later: look up env.functions[name] and bind params.
        std::vector<double> values;
        for (const auto& arg : args)
            values.push_back(arg->eval(env).asScalar("Function '" + name + "'"));

        if (name == "sin")   return Value::fromScalar(std::sin(values.at(0)));
        if (name == "cos")   return Value::fromScalar(std::cos(values.at(0)));
        if (name == "tan")   return Value::fromScalar(std::tan(values.at(0)));
        if (name == "log" || name == "ln") return Value::fromScalar(std::log(values.at(0)));
        if (name == "exp")   return Value::fromScalar(std::exp(values.at(0)));
        if (name == "sqrt")  return Value::fromScalar(std::sqrt(values.at(0)));
        if (name == "gamma") return Value::fromScalar(std::tgamma(values.at(0)));

        throw std::runtime_error("Unknown function: " + name);
    }
};

// ---------------- Parser ----------------

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
        : tokens_(std::move(tokens)) {}

    //f(x) = ... can branch here later.
    ExprPtr parse() {
        ExprPtr expr = statement();

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

    const Token& peek_at(std::size_t offset) const {
        std::size_t i = pos_ + offset;
        if (i >= tokens_.size())
            return tokens_.back();
        return tokens_[i];
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
            case TokenType::Assign:
                return 1;
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
        return type == TokenType::Power || type == TokenType::Assign;
    }

    bool is_binary_op(TokenType type) const {
        return type != TokenType::Assign && precedence(type) != -1;
    }

    ExprPtr statement() {
        // name = expr
        if (peek().type == TokenType::Identifier &&
            peek_at(1).type == TokenType::Assign) {
            std::string name = advance().text;
            advance(); // '='
            return std::make_unique<AssignExpr>(std::move(name), expression(0));
        }

        return expression(0);
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

            case TokenType::Vector:
                return std::make_unique<VectorExpr>(std::move(tok.vector));

            case TokenType::Matrix:
                return std::make_unique<MatrixExpr>(std::move(tok.matrix));

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
        tokens.push_back(std::move(tok));

        if (tokens.back().type == TokenType::End)
            break;
    }

    return tokens;
}

Value evaluate_line(const std::string& userInput, Environment& env, bool& should_print) {
    auto tokens = lex_all(userInput);
    Parser parser(std::move(tokens));
    ExprPtr ast = parser.parse();
    // Assignments bind silently; expressions still echo their result.
    should_print = dynamic_cast<const AssignExpr*>(ast.get()) == nullptr;
    return ast->eval(env);
}

bool is_exit_command(const std::string& userInput) {
    return userInput == "exit" || userInput == "Exit" ||
           userInput == "exit()" || userInput == "ext";
}

int main() {
    // Key: int satus
    // 2 => first use
    // 1 => in active use (don't close)
    // 3 => debug/admin mode

    bootInfo();
    Environment env;
    int status = 2;
    do {
        std::string userInput {};
        std::getline(std::cin, userInput);

        if (userInput.empty())
            continue;

        if (is_exit_command(userInput)) {
            status = 1;
            continue;
        }

        if (status == 2 && userInput == "admin") {
            status = 3;
            std::cout << "Debug Mode is Active";
            continue;
        }

        if (status == 2)
            status = 0;

        if (status == 3) {
            // Placeholder admin mode: still evaluate expressions.
        }

        try {
            bool should_print = true;
            Value result = evaluate_line(userInput, env, should_print);
            if (should_print)
                std::cout << result << '\n';
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    } while (status != 1);
    std::cout << "Ending Session";
    return 0;
}
