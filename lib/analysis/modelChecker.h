#ifndef ANALYSIS_MODELCHECKER_H
#define ANALYSIS_MODELCHECKER_H

#include "fdlang/AST.h"
#include "fdlang/ASTVisitor.h"

#include <bitset>
#include <map>
#include <queue>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fdlang::analysis {

class InfoCollector : public ASTVisitor {
public:
    void visit(Stmts *node) override {
        for (ASTNode *child : node->children) {
            child->accept(this);
        }
    }
    void visit(Cond *node) override {}
    void visit(UnaryAssignStmt *node) override {}
    void visit(BinaryAssignStmt *node) override {}
    void visit(IfStmt *node) override {
        node->trueBody->accept(this);
        node->falseBody->accept(this);
    }
    void visit(WhileStmt *node) override { node->body->accept(this); }
    void visit(CheckStmt *node) override { checks.push_back(node); }
    void visit(NopStmt *node) override {}
    std::vector<CheckStmt *> checks;
    void visit(CallStmt *node) override {}
    void visit(FunctionNodes *node) override {
        for (ASTNode *child : node->children) {
            child->accept(this);
        }
    }
    void visit(FunctionNode *node) override { node->body->accept(this); }
};

class NaiveModelChecker {
private:
    ASTNode *root;

    using Env = std::map<std::string, long long>;
    using Envs = std::set<Env>; // list of list of (identifier, value)

    // label -> valueSet
    std::unordered_map<size_t, std::bitset<256>> reachableValue;

    InfoCollector info;

    void evaluate(Stmts *node, Envs &envs);
    bool evaluate(Cond *node, Env &env);
    void evaluate(UnaryAssignStmt *node, Envs &envs);
    void evaluate(BinaryAssignStmt *node, Envs &envs);
    void evaluate(IfStmt *node, Envs &envs);
    void evaluate(WhileStmt *node, Envs &envs);
    void evaluate(CheckStmt *node, Envs &envs);
    void evaluate(NopStmt *node, Envs &envs);
    void evaluate(CallStmt *node, Envs &env);
    void evaluate(FunctionNodes *node, Envs &env);
    void evaluate(FunctionNode *node, Envs &env);

public:
    NaiveModelChecker(ASTNode *root) : root(root) {}

    void run();
    void dumpResult(std::ostream &out);
};

} // namespace fdlang::analysis

#endif