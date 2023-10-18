#include "intervalAnalysis.h"

#include <algorithm>
#include <array>
#include <queue>
#include <vector>

using namespace fdlang;
using namespace fdlang::analysis;

/**
 * Your code starts here
 */

void IntervalAnalysis::run() {

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
