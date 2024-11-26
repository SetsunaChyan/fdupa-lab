#include "parser.h"
#include "errorHandler.h"
#include "fdlang/AST.h"
#include "token.h"

#include <assert.h>

using namespace fdlang;

ASTNode *Parser::parse() { return parseFunctions(); }

const Token &Parser::advance() {
    if (isAtEnd()) {
        hasError = true;
        error(peek().line, "Parsing error, got EOF");
        return tokens.back();
    }
    current++;
    return tokens[current - 1];
}

bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }

const Token &Parser::peek() { return tokens[current]; }

const Token &Parser::previous() { return tokens[current - 1]; }

bool Parser::consume(TokenType type) {
    const Token &token = advance();
    if (token.type != type) {
        hasError = true;
        error(peek().line, "Parsing error, expect " + getTokenSpelling(type) +
                               " got " + peek().lexeme);
        return false;
    }
    return true;
}

ASTNode *Parser::parseStmts() {
    Stmts *stmts = new Stmts(label++);
    while (!isAtEnd() && peek().type != TokenType::RIGHT_BRACE && !hasError) {
        stmts->addChild(parseStmt());
    }
    return stmts;
}

ASTNode *Parser::parseStmt() {
    const Token &token = advance();
    switch (token.type) {
    case TokenType::IDENTIFIER:
        return parseAssignStmt();
    case TokenType::IF:
        return parseIfStmt();
    case TokenType::WHILE:
        return parseWhileStmt();
    case TokenType::CALL_CHECK_INTERVAL:
        return parseCheckIntervalStmt();
    case TokenType::NOP:
        return parseNopStmt();
    case TokenType::CALL:
        return parseCallStmt();
    default:
        hasError = true;
        error(token.line, "Parsing error, got " + token.lexeme);
        break;
    }
    return nullptr;
}

ASTNode *Parser::parseAssignStmt() {
    if (current + 2 >= tokens.size()) {
        hasError = true;
        error(peek().line, "Parsing error, got " + peek().lexeme);
        return nullptr;
    }
    TokenType type = tokens[current + 2].type;
    if (type == TokenType::PLUS || type == TokenType::MINUS)
        return parseBinaryAssignStmt();
    return parseUnaryAssignStmt();
}

ASTNode *Parser::parseIfStmt() {
    if (!consume(TokenType::LEFT_PAREN))
        return nullptr;
    ASTNode *cond = parseCond();
    if (hasError)
        return nullptr;
    if (!consume(TokenType::RIGHT_PAREN))
        return nullptr;
    if (!consume(TokenType::LEFT_BRACE))
        return nullptr;
    ASTNode *trueBody = parseStmts();
    if (hasError)
        return nullptr;
    if (!consume(TokenType::RIGHT_BRACE))
        return nullptr;
    if (!consume(TokenType::ELSE))
        return nullptr;
    if (!consume(TokenType::LEFT_BRACE))
        return nullptr;
    ASTNode *falseBody = parseStmts();
    if (hasError)
        return nullptr;
    if (!consume(TokenType::RIGHT_BRACE))
        return nullptr;
    return new IfStmt(label++, cond, trueBody, falseBody);
}

ASTNode *Parser::parseWhileStmt() {
    if (!consume(TokenType::LEFT_PAREN))
        return nullptr;
    ASTNode *cond = parseCond();
    if (hasError)
        return nullptr;
    if (!consume(TokenType::RIGHT_PAREN))
        return nullptr;
    if (!consume(TokenType::LEFT_BRACE))
        return nullptr;
    ASTNode *body = parseStmts();
    if (hasError)
        return nullptr;
    if (!consume(TokenType::RIGHT_BRACE))
        return nullptr;
    return new WhileStmt(label++, cond, body);
}

ASTNode *Parser::parseCheckIntervalStmt() {
    const Token &check = previous();
    if (!consume(TokenType::LEFT_PAREN))
        return nullptr;
    const Token &param0 = advance();
    if (!consume(TokenType::COMMA))
        return nullptr;
    const Token &param1 = advance();
    if (!consume(TokenType::COMMA))
        return nullptr;
    const Token &param2 = advance();
    if (!consume(TokenType::RIGHT_PAREN))
        return nullptr;
    if (!consume(TokenType::SEMICOLON))
        return nullptr;
    return new CheckStmt(label++, check, {param0, param1, param2});
}

ASTNode *Parser::parseNopStmt() {
    const Token &nop = previous();
    if (!consume(TokenType::SEMICOLON))
        return nullptr;
    return new NopStmt(label++);
}

ASTNode *Parser::parseBinaryAssignStmt() {
    const Token &variable = previous();

    if (!consume(TokenType::EQUAL))
        return nullptr;
    const Token &leftOperand = advance();
    const Token &op = advance();
    const Token &rightOperand = advance();
    if (!consume(TokenType::SEMICOLON))
        return nullptr;

    return new BinaryAssignStmt(label++, variable, leftOperand, op,
                                rightOperand);
}

ASTNode *Parser::parseUnaryAssignStmt() {
    const Token &variable = previous();

    if (!consume(TokenType::EQUAL))
        return nullptr;
    const Token &operand = advance();
    if (operand.type == TokenType::CALL_INPUT) {
        if (!consume(TokenType::LEFT_PAREN))
            return nullptr;
        if (!consume(TokenType::RIGHT_PAREN))
            return nullptr;
    }
    if (!consume(TokenType::SEMICOLON))
        return nullptr;

    return new UnaryAssignStmt(label++, variable, operand);
}

ASTNode *Parser::parseCond() {
    const Token &leftOperand = advance();
    const Token &op = advance();
    const Token &rightOperand = advance();

    return new Cond(label++, leftOperand, op, rightOperand);
}

ASTNode *Parser::parseCallStmt() {
    const Token &funcName = advance();
    if (!consume(TokenType::LEFT_PAREN))
        return nullptr;

    std::vector<Token> args;
    TokenType type = tokens[current].type;
    while (type != TokenType::RIGHT_PAREN) {
        if (type == TokenType::COMMA) {
            if (!consume(TokenType::COMMA))
                return nullptr;
            type = tokens[current].type;
            continue;
        }
        args.push_back(advance());
        type = tokens[current].type;
    }

    if (!consume(TokenType::RIGHT_PAREN))
        return nullptr;
    if (!consume(TokenType::SEMICOLON))
        return nullptr;

    return new CallStmt(label++, funcName, args);
}

ASTNode *Parser::parseFunctions() {
    FunctionNodes *functions = new FunctionNodes(label++);
    while (!isAtEnd() && peek().type != TokenType::RIGHT_BRACE && !hasError) {
        functions->addChild(parseFunction());
    }
    functions->addCallee();

    return functions;
}

ASTNode *Parser::parseFunction() {
    if (!consume(TokenType::FUNCTION))
        return nullptr;

    const Token &funcName = advance();

    if (!consume(TokenType::LEFT_PAREN))
        return nullptr;
    std::vector<Token> args;
    TokenType type = tokens[current].type;
    while (type != TokenType::RIGHT_PAREN) {
        if (type == TokenType::COMMA) {
            if (!consume(TokenType::COMMA))
                return nullptr;
            type = tokens[current].type;
            continue;
        }
        args.push_back(advance());
        type = tokens[current].type;
    }

    if (!consume(TokenType::RIGHT_PAREN))
        return nullptr;
    if (!consume(TokenType::LEFT_BRACE))
        return nullptr;

    ASTNode *body = parseStmts();
    if (hasError)
        return nullptr;
    if (!consume(TokenType::RIGHT_BRACE))
        return nullptr;

    return new FunctionNode(label++, funcName, args, body);
}

bool Parser::hadError() { return hasError; }