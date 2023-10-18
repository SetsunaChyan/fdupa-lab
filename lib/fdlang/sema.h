#ifndef FDLANG_SEMA_H
#define FDLANG_SEMA_H

#include "AST.h"
#include "ASTVisitor.h"

namespace fdlang {

class Sema : public ASTVisitor {
private:
    ASTNode *root;
    bool hasError = false;

    void visit(Stmts *node) override;
    void visit(Cond *node) override;
    void visit(UnaryAssignStmt *node) override;
    void visit(BinaryAssignStmt *node) override;
    void visit(IfStmt *node) override;
    void visit(WhileStmt *node) override;
    void visit(CheckStmt *node) override;
    void visit(NopStmt *node) override;

    bool checkVariable(const Token &token);
    bool checkCondOp(const Token &token);
    bool checkNumber(const Token &token);
    bool checkArithmeticOp(const Token &token);
    bool checkValue(const Token &token);
    bool checkValueOrInput(const Token &token);

public:
    Sema(ASTNode *root) : root(root) {}

    bool check();
};

} // namespace fdlang

#endif