#include "token.h"

#include <any>
#include <assert.h>

using namespace fdlang;

std::string fdlang::getTokenSpelling(TokenType type) {
    switch (type) {
    case TokenType::LEFT_PAREN:
        return "(";
    case TokenType::RIGHT_PAREN:
        return ")";
    case TokenType::LEFT_BRACE:
        return "{";
    case TokenType::RIGHT_BRACE:
        return "}";
    case TokenType::COMMA:
        return ",";
    case TokenType::MINUS:
        return "-";
    case TokenType::PLUS:
        return "+";
    case TokenType::SEMICOLON:
        return ";";
    case TokenType::EQUAL:
        return "=";
    case TokenType::EQUAL_EQUAL:
        return "==";
    case TokenType::GREATER:
        return ">";
    case TokenType::GREATER_EQUAL:
        return ">=";
    case TokenType::LESS:
        return "<";
    case TokenType::LESS_EQUAL:
        return "<=";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::NUMBER:
        return "NUMBER";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::WHILE:
        return "WHILE";
    case TokenType::CALL_INPUT:
        return "CALL_INPUT";
    case TokenType::CALL_CHECK_INTERVAL:
        return "CALL_CHECK_INTERVAL";
    case TokenType::CALL:
        return "CALL";
    case TokenType::NOP:
        return "NOP";
    case TokenType::FUNCTION:
        return "FUNCTION";
    case TokenType::END_OF_FILE:
        return "END_OF_FILE";
    default:
        break;
    }
    return "UNKNOWN";
}

long long Token::getLiteralAsNumber() const {
    assert(type == TokenType::NUMBER);
    return std::any_cast<long long>(literal);
}

bool Token::isCondOp() const {
    switch (type) {
    case TokenType::EQUAL_EQUAL:
    case TokenType::GREATER:
    case TokenType::GREATER_EQUAL:
    case TokenType::LESS:
    case TokenType::LESS_EQUAL:
        return true;
    default:
        break;
    }
    return false;
}

bool Token::isArithmeticOp() const {
    switch (type) {
    case TokenType::MINUS:
    case TokenType::PLUS:
        return true;
    default:
        break;
    }
    return false;
}

bool Token::isValue() const {
    switch (type) {
    case TokenType::IDENTIFIER:
    case TokenType::NUMBER:
        return true;
    default:
        break;
    }
    return false;
}

bool Token::isFunction() const {
    switch (type) {
    case TokenType::FUNCTION:
        return true;
    default:
        break;
    }
    return false;
}