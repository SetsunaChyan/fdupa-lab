#include "sema.h"
#include "errorHandler.h"
#include "token.h"

using namespace fdlang;

void Sema::visit(Stmts *node) {
    for (ASTNode *child : node->children) {
        child->accept(this);
    }
}

void Sema::visit(Cond *node) {
    checkVariable(node->leftOperand);
    checkCondOp(node->op);
    checkNumber(node->rightOperand);
}

void Sema::visit(UnaryAssignStmt *node) {
    checkVariable(node->variable);
    checkValueOrInput(node->operand);
}

void Sema::visit(BinaryAssignStmt *node) {
    checkVariable(node->variable);
    checkValue(node->leftOperand);
    checkArithmeticOp(node->op);
    checkValue(node->rightOperand);
}

void Sema::visit(IfStmt *node) {
    node->cond->accept(this);
    node->trueBody->accept(this);
    node->falseBody->accept(this);
}

void Sema::visit(WhileStmt *node) {
    node->cond->accept(this);
    node->body->accept(this);
}

void Sema::visit(CheckStmt *node) {
    switch (node->check.type) {
    case TokenType::CALL_CHECK_INTERVAL: {
        checkVariable(node->params[0]);
        checkNumber(node->params[1]);
        checkNumber(node->params[2]);
        break;
    }
    default:
        hasError = true;
        error(node->check.line, "Sema error, expect CALL_CHECK got " +
                                    getTokenSpelling(node->check.type) + "(" +
                                    node->check.lexeme + ")");
    }
}

void Sema::visit(NopStmt *node) {}

bool Sema::checkVariable(const Token &token) {
    if (token.type != TokenType::IDENTIFIER) {
        hasError = true;
        error(token.line, "Sema error, expect " +
                              getTokenSpelling(TokenType::IDENTIFIER) +
                              " got " + getTokenSpelling(token.type) + "(" +
                              token.lexeme + ")");
        return false;
    }
    return true;
}

bool Sema::checkCondOp(const Token &token) {
    if (!token.isCondOp()) {
        hasError = true;
        error(token.line, "Sema error, expect CONDITIONAL_OPERATOR got " +
                              getTokenSpelling(token.type) + "(" +
                              token.lexeme + ")");
        return false;
    }
    return true;
}

bool Sema::checkNumber(const Token &token) {
    if (token.type != TokenType::NUMBER) {
        hasError = true;
        error(token.line, "Sema error, expect " +
                              getTokenSpelling(TokenType::NUMBER) + " got " +
                              getTokenSpelling(token.type) + "(" +
                              token.lexeme + ")");
        return false;
    }
    long long val = token.getLiteralAsNumber();
    if (val < 0 || val > 255) {
        hasError = true;
        error(token.line, "Sema error, the number should be between 0 and 255");
        return false;
    }
    return true;
}

bool Sema::checkArithmeticOp(const Token &token) {
    if (!token.isArithmeticOp()) {
        hasError = true;
        error(token.line, "Sema error, expect ARITHEMETIC_OPERATOR got " +
                              getTokenSpelling(token.type) + "(" +
                              token.lexeme + ")");
        return false;
    }
    return true;
}

bool Sema::checkValue(const Token &token) {
    if (!token.isValue()) {
        hasError = true;
        error(token.line, "Sema error, expect VALUE got " +
                              getTokenSpelling(token.type) + "(" +
                              token.lexeme + ")");
        return false;
    }
    return true;
}

bool Sema::checkValueOrInput(const Token &token) {
    if (!token.isValue() && token.type != TokenType::CALL_INPUT) {
        hasError = true;
        error(token.line, "Sema error, expect VALUE or CALL_INPUT got " +
                              getTokenSpelling(token.type) + "(" +
                              token.lexeme + ")");
        return false;
    }
    return true;
}

bool Sema::check() {
    root->accept(this);
    return !hasError;
}