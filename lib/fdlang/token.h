#ifndef FDLANG_TOKEN_H
#define FDLANG_TOKEN_H

#include <any>
#include <string>

namespace fdlang {

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    MINUS,
    PLUS,
    SEMICOLON,

    // One or two character tokens
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals
    IDENTIFIER,
    NUMBER,

    // Keywords
    IF,
    ELSE,
    WHILE,
    CALL_INPUT,
    CALL_CHECK_INTERVAL,
    NOP,

    END_OF_FILE
};

struct Token {
    const TokenType type;
    const std::string lexeme;
    const std::any literal;
    const size_t line;

    Token(TokenType type, const std::string &lexeme, const std::any &literal,
          size_t line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}

    long long getLiteralAsNumber() const;

    bool isCondOp() const;

    bool isArithmeticOp() const;

    bool isValue() const;
};

std::string getTokenSpelling(TokenType type);

} // namespace fdlang
#endif