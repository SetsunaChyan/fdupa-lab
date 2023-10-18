#include "IRBuilder.h"
#include "fdlang/token.h"

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

Inst *IRBuilder::lastInst() {
    assert(!IR.empty());
    return IR.back().get();
}

Insts IRBuilder::build() {
    IR.clear();
    root->accept(this);
    std::vector<Inst *> ret;
    for (size_t id = 0; id < IR.size(); id++) {
        Inst *inst = IR[id].get();
        inst->setLabel(id);
        if (id + 1 < IR.size() && inst->type != InstType::GotoInst) {
            inst->addSuccessor(IR[id + 1].get());
            IR[id + 1]->addPredecessor(inst);
        }
    }
    for (size_t id = 0; id < IR.size(); id++) {
        Inst *inst = IR[id].get();
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
        ret.push_back(inst);
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