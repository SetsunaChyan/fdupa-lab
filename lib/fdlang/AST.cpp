#include "AST.h"
#include "ASTVisitor.h"

using namespace fdlang;

void Stmts::accept(ASTVisitor *visitor) { visitor->visit(this); }
void Cond::accept(ASTVisitor *visitor) { visitor->visit(this); }
void UnaryAssignStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void BinaryAssignStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void IfStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void WhileStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void CheckStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void NopStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void CallStmt::accept(ASTVisitor *visitor) { visitor->visit(this); }
void FunctionNodes::accept(ASTVisitor *visitor) { visitor->visit(this); }
void FunctionNode::accept(ASTVisitor *visitor) { visitor->visit(this); }

void Stmts::addChild(ASTNode *node) { children.push_back(node); }
void FunctionNodes::addChild(ASTNode *node) { children.push_back(node); }

void FunctionNodes::addCallee() {
    for (auto child : children)
        child->addCallee(this);
}
void FunctionNode::addCallee(ASTNode *nodes) { body->addCallee(nodes); }
void Stmts::addCallee(ASTNode *nodes) {
    for (auto &child : children) {
        child->addCallee(nodes);
    }
}
void CallStmt::addCallee(ASTNode *nodes) {
    auto functionNodes = dynamic_cast<FunctionNodes *>(nodes);
    for (auto child : functionNodes->children) {
        auto function = dynamic_cast<FunctionNode *>(child);
        if (function->funcName.lexeme == calleeName.lexeme) {
            callee = function;
            function->setRoot(false);
        }
    }
}

std::string fdlang::getASTNodeSpelling(const ASTNode &node) {
    switch (node.type) {
    case ASTNodeType::STMTS:
        return "STMTS";
    case ASTNodeType::BINARY_ASSIGN_STMT:
        return "BINARY_ASSIGN_STMT";
    case ASTNodeType::UNARY_ASSIGN_STMT:
        return "UNARY_ASSIGN_STMT";
    case ASTNodeType::CHECK_STMT:
        return "CHECK_STMT";
    case ASTNodeType::IF_STMT:
        return "IF_STMT";
    case ASTNodeType::WHILE_STMT:
        return "WHILE_STMT";
    case ASTNodeType::COND:
        return "COND";
    case ASTNodeType::NOP_STMT:
        return "NOP_STMT";
    case ASTNodeType::CALL_STMT:
        return "CALL_STMT";
    case ASTNodeType::FUNCTION:
        return "FUNCTION";
    default:
        break;
    }
    return "UNKNOWN";
}