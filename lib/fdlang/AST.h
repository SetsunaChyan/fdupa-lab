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
    CALL_STMT,
    FUNCTION,
    FUNCTIONS
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
    virtual void addCallee(ASTNode *nodes) = 0;
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
    virtual void addCallee(ASTNode *nodes) override;

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
    virtual void addCallee(ASTNode *nodes) override {};
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
    virtual void addCallee(ASTNode *nodes) override {};
};

class UnaryAssignStmt : public ASTNode {
public:
    Token variable, operand;

    UnaryAssignStmt(size_t label, const Token &variable, const Token &operand)
        : ASTNode(ASTNodeType::UNARY_ASSIGN_STMT, label), variable(variable),
          operand(operand) {}

    ~UnaryAssignStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
    virtual void addCallee(ASTNode *nodes) override {};
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
    virtual void addCallee(ASTNode *nodes) override {};
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
    virtual void addCallee(ASTNode *nodes) override {};
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
    virtual void addCallee(ASTNode *nodes) override {};
};

class NopStmt : public ASTNode {
public:
    NopStmt(size_t label) : ASTNode(ASTNodeType::NOP_STMT, label) {}

    ~NopStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
    virtual void addCallee(ASTNode *nodes) override {};
};

class FunctionNode;
class CallStmt : public ASTNode {
public:
    Token calleeName;
    std::vector<Token> args;
    FunctionNode *callee;

    CallStmt(size_t label, const Token calleeName,
             const std::vector<Token> &args)
        : ASTNode(ASTNodeType::CALL_STMT, label), calleeName(calleeName),
          args(args) {
        callee = nullptr;
    }

    ~CallStmt() override {}

    virtual void accept(ASTVisitor *visitor) override;
    virtual void addCallee(ASTNode *nodes) override;
};

class FunctionNode : public ASTNode {
public:
    Token funcName;
    std::vector<Token> args;
    ASTNode *body;
    bool root;

    FunctionNode(size_t label, const Token funcName,
                 const std::vector<Token> &args, ASTNode *body)
        : ASTNode(ASTNodeType::FUNCTION, label), funcName(funcName), args(args),
          body(body) {
        root = true;
    }

    ~FunctionNode() override {
        if (body)
            delete body;
    }

    virtual void accept(ASTVisitor *visitor) override;
    virtual void addCallee(ASTNode *nodes) override;
    void setRoot(bool isRoot) { root = isRoot; }
    bool isRoot() { return root; }
};

class FunctionNodes : public ASTNode {
public:
    std::vector<ASTNode *> children;

    FunctionNodes(size_t label) : ASTNode(ASTNodeType::FUNCTIONS, label) {}

    ~FunctionNodes() override {
        for (ASTNode *child : children)
            if (child)
                delete child;
    }

    virtual void accept(ASTVisitor *visitor) override;
    virtual void addCallee(ASTNode *nodes) override {};

    void addChild(ASTNode *node);

    void addCallee();
};

std::string getASTNodeSpelling(const ASTNode &node);

} // namespace fdlang

#endif