#ifndef FDLANG_AST_H
#define FDLANG_AST_H

#include "token.h"

#include <vector>

namespace fdlang {

enum class ASTNodeType {
    STMTS,
    COND,
    BINARY_ASSIGN_STMT,
    UNARY_ASSIGN_STMT,
    IF_STMT,
    WHILE_STMT,
    CHECK_STMT,
    NOP_STMT,
};

class ASTVisitor;

class ASTNode {
public:
    // size_t start, end;
    size_t label;
    ASTNodeType type;

    ASTNode(ASTNodeType type, size_t label) : type(type), label(label) {}

    virtual ~ASTNode() {}

    virtual void accept(ASTVisitor *visitor) = 0;
};

class Stmts : public ASTNode {
public:
    std::vector<ASTNode *> children;

    Stmts(size_t label) : ASTNode(ASTNodeType::STMTS, label) {}

    ~Stmts() override {
        for (ASTNode *child : children)
            if (child)
                delete child;
    }

    virtual void accept(ASTVisitor *visitor) override;

    void addChild(ASTNode *node);
};

class Cond : public ASTNode {
public:
    Token leftOperand, op, rightOperand;

    Cond(size_t label, const Token &leftOperand, const Token &op,
         const Token &rightOperand)
        : ASTNode(ASTNodeType::COND, label), leftOperand(leftOperand), op(op),
          rightOperand(rightOperand) {}

    ~Cond() override {}

    virtual void accept(ASTVisitor *visitor) override;
};

class BinaryAssignStmt : public ASTNode {
public:
    Token variable, leftOperand, op, rightOperand;

    BinaryAssignStmt(size_t label, const Token &variable,
                     const Token &leftOperand, const Token &op,
                     const Token &rightOperand)
        : ASTNode(ASTNodeType::BINARY_ASSIGN_STMT, label), variable(variable),
          leftOperand(leftOperand), op(op), rightOperand(rightOperand) {}

    ~BinaryAssignStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
};

class UnaryAssignStmt : public ASTNode {
public:
    Token variable, operand;

    UnaryAssignStmt(size_t label, const Token &variable, const Token &operand)
        : ASTNode(ASTNodeType::UNARY_ASSIGN_STMT, label), variable(variable),
          operand(operand) {}

    ~UnaryAssignStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
};

class IfStmt : public ASTNode {
public:
    ASTNode *cond, *trueBody, *falseBody;

    IfStmt(size_t label, ASTNode *cond, ASTNode *trueBody, ASTNode *falseBody)
        : ASTNode(ASTNodeType::IF_STMT, label), cond(cond), trueBody(trueBody),
          falseBody(falseBody) {}

    ~IfStmt() override {
        if (cond)
            delete cond;
        if (trueBody)
            delete trueBody;
        if (falseBody)
            delete falseBody;
    }

    virtual void accept(ASTVisitor *visitor) override;
};

class WhileStmt : public ASTNode {
public:
    ASTNode *cond, *body;

    WhileStmt(size_t label, ASTNode *cond, ASTNode *body)
        : ASTNode(ASTNodeType::WHILE_STMT, label), cond(cond), body(body) {}

    ~WhileStmt() override {
        if (cond)
            delete cond;
        if (body)
            delete body;
    }

    virtual void accept(ASTVisitor *visitor) override;
};

class CheckStmt : public ASTNode {
public:
    Token check;
    std::vector<Token> params;

    CheckStmt(size_t label, const Token &check,
              const std::vector<Token> &params)
        : ASTNode(ASTNodeType::CHECK_STMT, label), check(check),
          params(params) {}

    ~CheckStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
};

class NopStmt : public ASTNode {
public:
    NopStmt(size_t label) : ASTNode(ASTNodeType::NOP_STMT, label) {}

    ~NopStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
};

std::string getASTNodeSpelling(const ASTNode &node);

} // namespace fdlang

#endif