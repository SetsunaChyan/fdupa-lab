#ifndef FDLANG_ASTTRAVERSEPRINTER_H
#define FDLANG_ASTTRAVERSEPRINTER_H

#include "ASTVisitor.h"
#include "fdlang/AST.h"

#include <iostream>

namespace fdlang {

class ASTTraversePrinter : public ASTVisitor {
private:
    int ident = 0;
    std::ostream &out;

    void printIndent() {
        for (int i = 0; i < ident; i++)
            out << "    ";
    }

public:
    ASTTraversePrinter(std::ostream &out) : out(out) {}

    virtual void visit(Stmts *node) override;
    virtual void visit(Cond *node) override;
    virtual void visit(UnaryAssignStmt *node) override;
    virtual void visit(BinaryAssignStmt *node) override;
    virtual void visit(IfStmt *node) override;
    virtual void visit(WhileStmt *node) override;
    virtual void visit(CheckStmt *node) override;
    virtual void visit(NopStmt *node) override;
    virtual void visit(CallStmt *node) override;
    virtual void visit(FunctionNodes *node) override;
    virtual void visit(FunctionNode *node) override;
};

} // namespace fdlang

#endif