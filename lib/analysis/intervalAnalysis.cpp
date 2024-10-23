#include "intervalAnalysis.h"
#include <queue>

using namespace fdlang;
using namespace fdlang::analysis;

void IntervalAnalysis::fixedPoint() {
    // ini States and worklist
    inputStates[0] = iniStates();
    worklist.push(0);

    // worklist Iteration here
    while (!worklist.empty()) {
        auto label = worklist.front();
        worklist.pop();
        auto inst = insts[label];
        States &inputState = inputStates[label];
        States outputState;
        // transfer Inst
        outputState = transfer(inst, inputState);
        // add Successors
        addSuccessors(label, outputState);
    }
}

/****************************************************************
********************* Your code starts here *********************
*****************************************************************/

void IntervalAnalysis::checkInstsStates() {
    // You can modify this function arbitrarily
    for (auto inst : insts) {
        if (inst->getInstType() != IR::InstType::CheckIntervalInst)
            continue;

        IR::CheckIntervalInst *checkInst = (IR::CheckIntervalInst *)inst;
        std::string variable = checkInst->getOperand(0)->getAsVariable();
        long long l = checkInst->getOperand(1)->getAsNumber();
        long long r = checkInst->getOperand(2)->getAsNumber();

        results[checkInst] = ResultType::UNREACHABLE;
    }
}

States IntervalAnalysis::iniStates() {
    // You can modify this function arbitrarily
    return States();
}

States IntervalAnalysis::transfer(IR::Inst *inst, States &input) {
    // You can modify this function arbitrarily
    return States();
}

bool IntervalAnalysis::joinInto(const States &x, States &y) {
    // You can modify this function arbitrarily
    return true;
}

void IntervalAnalysis::addSuccessors(size_t nowLabel, States outputState) {
    // You can modify this function arbitrarily
}