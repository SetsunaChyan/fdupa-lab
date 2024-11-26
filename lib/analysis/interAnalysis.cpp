#include "interAnalysis.h"
#include "IR/IR.h"
#include "analysis/intervalAnalysis.h"
#include <iostream>
#include <queue>

using namespace fdlang;
using namespace fdlang::analysis;

/****************************************************************
********************* Your code starts here *********************
*****************************************************************/
void InterAnalysis::fixedPoint() {
    //
}

void InterAnalysis::intraFixedPoint(IR::Function *function) {
    //transfer
}

void InterAnalysis::interFixedPoint(IR::Function *function) {
    // ini States and worklist
}

void InterAnalysis::checkInstsStates() {
    for (auto func : funcs) {
        for (auto inst : func->getInsts()) {
            if (inst->getInstType() != IR::InstType::CheckIntervalInst)
                continue;

            IR::CheckIntervalInst *checkInst = (IR::CheckIntervalInst *)inst;
            std::string variable = checkInst->getOperand(0)->getAsVariable();
            long long l = checkInst->getOperand(1)->getAsNumber();
            long long r = checkInst->getOperand(2)->getAsNumber();

            results[checkInst] = ResultType::UNREACHABLE;
        }
    }
}

States InterAnalysis::iniStates(IR::Function *function) {
    // You can modify this function arbitrarily
    return States();
}

States InterAnalysis::transfer(IR::Inst *inst, States &input) {
    // You can modify this function arbitrarily
    return States();
}

bool InterAnalysis::joinInto(const States &x, States &y) {
    // You can modify this function arbitrarily
    return true;
}

void InterAnalysis::addSuccessors(IR::Inst *nowInst, States outputState) {
    // You can modify this function arbitrarily
}
