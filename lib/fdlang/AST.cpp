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

void Stmts::addChild(ASTNode *node) { children.push_back(node); }

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
    default:
        break;
    }
    return "UNKNOWN";
}