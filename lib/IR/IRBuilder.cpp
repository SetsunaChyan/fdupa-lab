#include "IRBuilder.h"
#include "IR/IR.h"
#include "fdlang/AST.h"
#include "fdlang/token.h"
#include <iostream>
#include <vector>

using namespace fdlang;
using namespace fdlang::IR;

static CmpOperator TokenOpType2CmpOp(TokenType type) {
    switch (type) {
    case TokenType::EQUAL_EQUAL:
        return CmpOperator::EQ;
    case TokenType::GREATER_EQUAL:
        return CmpOperator::GEQ;
    case TokenType::GREATER:
        return CmpOperator::GT;
    case TokenType::LESS_EQUAL:
        return CmpOperator::LEQ;
    case TokenType::LESS:
        return CmpOperator::LT;
    default:
        assert(false && "Unknown Operator");
    }
    return CmpOperator::EQ;
}

void IRBuilder::addInst(Inst *inst) { IR.emplace_back(inst); }
void IRBuilder::addFunction(Function *function) {
    functions.emplace_back(function);
}

Inst *IRBuilder::lastInst() {
    assert(!IR.empty());
    return IR.back().get();
}

Functions IRBuilder::build() {
    functions.clear();
    IR.clear();
    root->accept(this);

    std::vector<Function *> ret;
    size_t instId = 0;
    for (size_t funcID = 0; funcID < functions.size(); funcID++) {

        Function *function = functions[funcID].get();
        function->setBeginLabel(instId);
        Insts &insts = function->getInsts();
        instId++;

        for (int i = 0; i < insts.size(); i++) {
            Inst *inst = insts[i];
            inst->setLabel(instId);
            if (i + 1 < insts.size() && inst->type != InstType::GotoInst) {
                inst->addSuccessor(insts[i + 1]);
                insts[i + 1]->addPredecessor(inst);
                inst->setParent(function);
            }
            function->label2Inst[instId] = inst;
            instId++;
        }
        function->setEndLabel(instId);
        instId += 2;
    }

    for (size_t funcID = 0; funcID < functions.size(); funcID++) {
        Function *function = functions[funcID].get();
        Insts &insts = function->getInsts();

        for (int i = 0; i < insts.size(); i++) {
            Inst *inst = insts[i];
            if (inst->type == InstType::GotoInst) {
                GotoInst *gotoInst = (GotoInst *)inst;
                gotoInst->addSuccessor(gotoInst->getDestInst());
                gotoInst->getDestInst()->addPredecessor(inst);
            }
            if (inst->type == InstType::IfInst) {
                IfInst *ifInst = (IfInst *)inst;
                ifInst->addSuccessor(ifInst->getDestInst());
                ifInst->getDestInst()->addPredecessor(inst);
            }
            if (inst->type == InstType::CallInst) {
                CallInst *callInst = (CallInst *)inst;
                for (auto &func : functions) {
                    if (func->funcName == callInst->getCalleeName()) {
                        func->setRoot(false);
                        callInst->setCallee(func.get());
                    }
                }
            }
        }
        ret.push_back(function);
    }

    return ret;
}

void IRBuilder::visit(Stmts *node) {
    for (auto child : node->children) {
        child->accept(this);
    }
}

void IRBuilder::visit(Cond *node) {}

void IRBuilder::visit(UnaryAssignStmt *node) {
    switch (node->operand.type) {
    case TokenType::CALL_INPUT:
        addInst(new InputInst(new Value(node->variable)));
        break;
    case TokenType::IDENTIFIER:
    case TokenType::NUMBER:
        addInst(new AssignInst(new Value(node->variable),
                               new Value(node->operand)));
        break;
    default:
        assert(false);
    }
}

void IRBuilder::visit(BinaryAssignStmt *node) {
    switch (node->op.type) {
    case TokenType::PLUS:
        addInst(new AddInst(new Value(node->variable),
                            new Value(node->leftOperand),
                            new Value(node->rightOperand)));
        break;
    case TokenType::MINUS:
        addInst(new SubInst(new Value(node->variable),
                            new Value(node->leftOperand),
                            new Value(node->rightOperand)));
        break;
    default:
        assert(false);
    }
}

void IRBuilder::visit(IfStmt *node) {
    Cond *cond = (Cond *)node->cond;

    LabelInst *labelTrueBody = new LabelInst();
    LabelInst *labelFalseBody = new LabelInst();
    LabelInst *labelEnd = new LabelInst();

    IfInst *ifInst = new IfInst(new Value(cond->leftOperand),
                                TokenOpType2CmpOp(cond->op.type),
                                new Value(cond->rightOperand), labelTrueBody);

    GotoInst *gotoFalseBody = new GotoInst(labelFalseBody);
    GotoInst *gotoEnd = new GotoInst(labelEnd);

    addInst(ifInst);
    addInst(gotoFalseBody);
    addInst(labelTrueBody);
    node->trueBody->accept(this);
    addInst(gotoEnd);
    addInst(labelFalseBody);
    node->falseBody->accept(this);
    addInst(labelEnd);
}

void IRBuilder::visit(WhileStmt *node) {
    Cond *cond = (Cond *)node->cond;

    LabelInst *labelStart = new LabelInst();
    LabelInst *labelBody = new LabelInst();
    LabelInst *labelEnd = new LabelInst();

    IfInst *ifInst = new IfInst(new Value(cond->leftOperand),
                                TokenOpType2CmpOp(cond->op.type),
                                new Value(cond->rightOperand), labelBody);

    GotoInst *gotoStart = new GotoInst(labelStart);
    GotoInst *gotoEnd = new GotoInst(labelEnd);

    addInst(labelStart);
    addInst(ifInst);
    addInst(gotoEnd);
    addInst(labelBody);
    node->body->accept(this);
    addInst(gotoStart);
    addInst(labelEnd);
}

void IRBuilder::visit(CheckStmt *node) {
    CheckIntervalInst *checkIntervalInst = new CheckIntervalInst(
        new Value(node->params[0]), new Value(node->params[1]),
        new Value(node->params[2]));
    checkIntervalInst->setLine(node->check.line);
    addInst(checkIntervalInst);
}

void IRBuilder::visit(NopStmt *node) {}

void IRBuilder::visit(CallStmt *node) {
    std::vector<Value *> operands;
    for (auto param : node->args) {
        operands.push_back(new Value(param));
    }
    CallInst *callInst = new CallInst(node->calleeName.lexeme, operands);
    addInst(callInst);
}

void IRBuilder::visit(FunctionNodes *node) {
    for (auto child : node->children) {
        child->accept(this);
    }
}

void IRBuilder::visit(FunctionNode *node) {

    std::vector<Value *> args;
    for (auto arg : node->args) {
        args.push_back(new Value(arg));
    }

    LabelInst *FunctionStart = new LabelInst();
    LabelInst *labelBody = new LabelInst();
    LabelInst *labelEnd = new LabelInst();
    LabelInst *FunctionEnd = new LabelInst();

    //addInst(FunctionStart);
    size_t functionStartSize = IR.size();
    addInst(labelBody);
    node->body->accept(this);
    addInst(labelEnd);
    size_t functionEndSize = IR.size();

    std::vector<Inst *> insts;
    for (size_t i = functionStartSize; i < functionEndSize; ++i) {
        insts.push_back(IR[i].get());
    }

    Function *function = new Function(node->funcName.lexeme, args, insts);
    addFunction(function);
}