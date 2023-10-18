#include "gtest/gtest.h"

#include "fdlang/scanner.h"

#include <numeric>
#include <vector>

using namespace fdlang;

std::vector<Token> scan(const std::string &src) {
    Scanner scanner(src);
    return Scanner(src).scanTokens();
}

TEST(Tokenize, TokenType) {
    std::string src = "( ) { } , - + ; = == > >= < <= ab_cd 1234 if else while "
                      "input check_interval nop";
    std::vector<Token> tokens = scan(src);
    std::vector<TokenType> expected = {TokenType::LEFT_PAREN,
                                       TokenType::RIGHT_PAREN,
                                       TokenType::LEFT_BRACE,
                                       TokenType::RIGHT_BRACE,
                                       TokenType::COMMA,
                                       TokenType::MINUS,
                                       TokenType::PLUS,
                                       TokenType::SEMICOLON,

                                       TokenType::EQUAL,
                                       TokenType::EQUAL_EQUAL,
                                       TokenType::GREATER,
                                       TokenType::GREATER_EQUAL,
                                       TokenType::LESS,
                                       TokenType::LESS_EQUAL,

                                       TokenType::IDENTIFIER,
                                       TokenType::NUMBER,

                                       TokenType::IF,
                                       TokenType::ELSE,
                                       TokenType::WHILE,
                                       TokenType::CALL_INPUT,
                                       TokenType::CALL_CHECK_INTERVAL,
                                       TokenType::NOP,

                                       TokenType::END_OF_FILE};

    EXPECT_EQ(tokens.size(), expected.size());

    bool eq = true;
    for (int i = 0; i < tokens.size(); i++)
        if (tokens[i].type != expected[i])
            eq = false;

    EXPECT_TRUE(eq);
}

TEST(Tokenize, Identifier) {
    std::string src = "a ab a123";
    std::vector<Token> tokens = scan(src);
    std::vector<std::string> expected = {"a", "ab", "a123", ""};

    EXPECT_EQ(tokens.size(), expected.size());

    bool eq = true;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::END_OF_FILE)
            continue;
        if (tokens[i].lexeme != expected[i]) {
            eq = false;
            break;
        }
    }

    EXPECT_TRUE(eq);
}

TEST(Tokenize, Number) {
    std::string src = "1 12 123 1234 12345 123456 0 00";
    std::vector<Token> tokens = scan(src);
    std::vector<long long> expected = {1,      12, 123, 1234, 12345,
                                       123456, 0,  0,   0};

    EXPECT_EQ(tokens.size(), expected.size());

    bool eq = true;
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::END_OF_FILE)
            continue;
        if (!tokens[i].literal.has_value() ||
            tokens[i].literal.type() != typeid(long long)) {
            eq = false;
            break;
        }
        if (std::any_cast<long long>(tokens[i].literal) != expected[i]) {
            eq = false;
            break;
        }
    }

    EXPECT_TRUE(eq);
}