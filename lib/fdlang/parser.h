#ifndef FDLANG_PARSER_H
#define FDLANG_PARSER_H

#include "AST.h"
#include "token.h"

#include <vector>

namespace fdlang {

class Parser {
private:
    const std::vector<Token> tokens;
    size_t start = 0;
    size_t current = 0;
    size_t label = 0;
    bool hasError = false;

public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    ASTNode *parse();

    const Token &advance();

    bool isAtEnd();

    const Token &peek();

    const Token &previous();

    bool consume(TokenType type);

    ASTNode *parseStmts();

    ASTNode *parseStmt();

    ASTNode *parseAssignStmt();

    ASTNode *parseCheckIntervalStmt();

    ASTNode *parseIfStmt();

    ASTNode *parseWhileStmt();

    ASTNode *parseNopStmt();

    ASTNode *parseBinaryAssignStmt();

    ASTNode *parseUnaryAssignStmt();

    ASTNode *parseCond();

    bool hadError();
};

} // namespace fdlang

#endif