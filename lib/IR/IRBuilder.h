#ifndef IR_IRBuilder_H
#define IR_IRBuilder_H

#include "IR.h"

#include "fdlang/AST.h"
#include "fdlang/ASTVisitor.h"

#include <memory>

namespace fdlang::IR {

class IRBuilder : public ASTVisitor {
private:
    ASTNode *root;
    std::vector<std::unique_ptr<Inst>> IR;
    std::vector<std::unique_ptr<Function>> functions;

    void addInst(Inst *inst);
    void addFunction(Function *function);
    Inst *lastInst();

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

public:
    IRBuilder(ASTNode *root) : root(root) {}

    Functions build();
};

} // namespace fdlang::IR

#endif