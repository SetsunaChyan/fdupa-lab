#include "scanner.h"
#include "errorHandler.h"
#include "fdlang/token.h"

#include <cctype>
#include <cstdlib>

using namespace fdlang;

const std::unordered_map<std::string, TokenType> Scanner::keywords = {
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"input", TokenType::CALL_INPUT},
    {"check_interval", TokenType::CALL_CHECK_INTERVAL},
    {"nop", TokenType::NOP},
    {"call", TokenType::CALL},
    {"function", TokenType::FUNCTION}};

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "", nullptr, line);
    return tokens;
}

void Scanner::scanToken() {
    char c = advance();
    switch (c) {
    case '(':
        addToken(TokenType::LEFT_PAREN);
        break;
    case ')':
        addToken(TokenType::RIGHT_PAREN);
        break;
    case '{':
        addToken(TokenType::LEFT_BRACE);
        break;
    case '}':
        addToken(TokenType::RIGHT_BRACE);
        break;
    case ',':
        addToken(TokenType::COMMA);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
    case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
    case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
    case ' ':
    case '\t':
    case '\r':
        break;
    case '\n':
        line++;
        break;
    default:
        if (std::isdigit(c)) {
            number();
        } else if (std::isalpha(c) || c == '_') {
            identifier();
        } else {
            error(line, "Unexpected character " + std::string(c, 1));
        }
        break;
    }
}

char Scanner::advance() {
    current++;
    return source[current - 1];
}

bool Scanner::isAtEnd() { return current >= source.size(); }

void Scanner::addToken(TokenType type) { addToken(type, nullptr); }

void Scanner::addToken(TokenType type, const std::any &literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}

bool Scanner::match(char expected) {
    if (isAtEnd())
        return false;
    if (source[current] != expected)
        return false;

    current++;
    return true;
}

char Scanner::peek() {
    if (isAtEnd())
        return '\0';
    return source[current];
}

void Scanner::number() {
    while (std::isdigit(peek()))
        advance();

    long long num = std::stoll(source.substr(start, current - start));
    addToken(TokenType::NUMBER, num);
}

void Scanner::identifier() {
    while (std::isalnum(peek()) || peek() == '_')
        advance();

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    if (it == keywords.end())
        addToken(TokenType::IDENTIFIER);
    else
        addToken(it->second);
}

bool Scanner::hadError() { return hasError; }