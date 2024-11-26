#include "ASTTraversePrinter.h"
#include "fdlang/AST.h"

using namespace fdlang;

void ASTTraversePrinter::visit(Stmts *node) {
    for (ASTNode *child : node->children) {
        child->accept(this);
    }
}
void ASTTraversePrinter::visit(Cond *node) {
    out << node->leftOperand.lexeme << " ";
    out << node->op.lexeme << " ";
    out << node->rightOperand.lexeme;
}

void ASTTraversePrinter::visit(UnaryAssignStmt *node) {
    printIndent();
    out << node->variable.lexeme << " = ";
    out << node->operand.lexeme;
    if (node->operand.type == TokenType::CALL_INPUT)
        out << "()";
    out << ";" << std::endl;
}

void ASTTraversePrinter::visit(BinaryAssignStmt *node) {
    printIndent();
    out << node->variable.lexeme << " = ";
    out << node->leftOperand.lexeme << " ";
    out << node->op.lexeme << " ";
    out << node->rightOperand.lexeme << ";";
    out << std::endl;
}

void ASTTraversePrinter::visit(IfStmt *node) {
    printIndent();
    out << "if (";
    node->cond->accept(this);
    out << ") {" << std::endl;
    ident++;
    node->trueBody->accept(this);
    ident--;
    printIndent();
    out << "} else {" << std::endl;
    ident++;
    node->falseBody->accept(this);
    ident--;
    printIndent();
    out << "}" << std::endl;
}

void ASTTraversePrinter::visit(WhileStmt *node) {
    printIndent();
    out << "while (";
    node->cond->accept(this);
    out << ") {" << std::endl;
    ident++;
    node->body->accept(this);
    ident--;
    printIndent();
    out << "}" << std::endl;
}

void ASTTraversePrinter::visit(CheckStmt *node) {
    printIndent();
    if (node->check.type == TokenType::CALL_CHECK_INTERVAL) {
        out << node->check.lexeme << "(";
        out << node->params[0].lexeme << ", ";
        out << node->params[1].lexeme << ", ";
        out << node->params[2].lexeme << ");";
    }
    out << std::endl;
}

void ASTTraversePrinter::visit(NopStmt *node) {
    printIndent();
    out << "nop" << std::endl;
}

void ASTTraversePrinter::visit(CallStmt *node) {
    printIndent();
    out << "call" << " ";
    out << node->calleeName.lexeme << " (";
    for (int i = 0; i < node->args.size(); i++) {
        auto param = node->args[i];
        if (i == node->args.size() - 1) {
            out << param.lexeme << ");";

        } else {
            out << param.lexeme << ", ";
        }
    }
    out << std::endl;
}

void ASTTraversePrinter::visit(FunctionNodes *node) {
    for (ASTNode *child : node->children) {
        child->accept(this);
    }
}

void ASTTraversePrinter::visit(FunctionNode *node) {
    printIndent();
    out << "function " << node->funcName.lexeme << "(";
    if (node->args.size() == 0)
        out << "){" << std::endl;
    else {
        for (int i = 0; i < node->args.size(); i++) {
            auto arg = node->args[i];
            if (i == node->args.size() - 1) {
                out << arg.lexeme << "){" << std::endl;

            } else {
                out << arg.lexeme << ", ";
            }
        }
    }

    ident++;
    node->body->accept(this);
    ident--;
    printIndent();
    out << "}" << std::endl << std::endl;
}