#ifndef FDLANG_SCANNER_H
#define FDLANG_SCANNER_H

#include "token.h"

#include <unordered_map>
#include <vector>

namespace fdlang {
class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    size_t line = 1;
    bool hasError = false;

    static const std::unordered_map<std::string, TokenType> keywords;

public:
    Scanner(const std::string &source) : source(source) {}

    std::vector<Token> scanTokens();

    bool hadError();

private:
    void scanToken();

    char advance();

    bool isAtEnd();

    void addToken(TokenType type);

    void addToken(TokenType type, const std::any &literal);

    bool match(char expected);

    char peek();

    void number();

    void identifier();
};

} // namespace fdlang

#endif