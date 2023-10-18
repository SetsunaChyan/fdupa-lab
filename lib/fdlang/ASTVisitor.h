#ifndef FDLANG_ASTVISITOR_H
#define FDLANG_ASTVISITOR_H

#include "AST.h"

namespace fdlang {

class ASTVisitor {
public:
    virtual void visit(Stmts *node) = 0;
    virtual void visit(Cond *node) = 0;
    virtual void visit(UnaryAssignStmt *node) = 0;
    virtual void visit(BinaryAssignStmt *node) = 0;
    virtual void visit(IfStmt *node) = 0;
    virtual void visit(WhileStmt *node) = 0;
    virtual void visit(CheckStmt *node) = 0;
    virtual void visit(NopStmt *node) = 0;
};

} // namespace fdlang

#endif