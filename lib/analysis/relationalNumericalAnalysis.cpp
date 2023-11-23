#include "relationalNumericalAnalysis.h"

#include "IR/IR.h"

#include <algorithm>
#include <array>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

using namespace fdlang;
using namespace fdlang::analysis;

void RelationalNumericalAnalysis::dumpStates(std::ostream &out,
                                             States &states) {
    states.dump(out);
}

RelationalNumericalAnalysis::States
RelationalNumericalAnalysis::transferAssignment(const IR::Inst *inst,
                                                States &input) {
    return input.assignInst(inst);
}

RelationalNumericalAnalysis::States
RelationalNumericalAnalysis::transferIfStmt(const IR::IfInst *inst,
                                            States &input, bool branch) {
    return input.filterInst(inst, branch);
}

RelationalNumericalAnalysis::States
RelationalNumericalAnalysis::transferIdentity(const IR::Inst *inst,
                                              States &input) {
    return input;
}

bool RelationalNumericalAnalysis::joinInto(const States &x, States &y) {
    States before = y;

    y = x.lub(y);

    return !before.eq(y);
}

void RelationalNumericalAnalysis::run() {

    // Collecting the name of variables
    // std::cerr << "[zone-analysis] Collecting the name of variables"
    //   << std::endl;
    std::unordered_set<std::string> varsSet;
    for (auto inst : insts) {
        for (int i = 0; i < inst->getOperandSize(); i++) {
            if (!inst->getOperand(i)->isVariable())
                continue;
            varsSet.insert(inst->getOperand(i)->getAsVariable());
        }
    }
    std::vector<std::string> vars(varsSet.begin(), varsSet.end());

    // Initializing the states
    // std::cerr << "[zone-analysis] Initializing the states" << std::endl;
    States initState(vars, true), bottomState(vars, false);
    for (auto inst : insts) {
        size_t label = inst->getLabel();
        if (label == 0)
            inputStates[label] = initState;
        else
            inputStates[label] = bottomState;
    }

    // Worklist algorithm
    // std::cerr << "[zone-analysis] Worklist algorithm" << std::endl;
    std::vector<bool> inQueue(insts.size(), false);
    std::queue<size_t> q;
    q.push(0), inQueue[0] = true;

    auto tryToEnqueue = [&](const States &outputState, const size_t succ) {
        if (joinInto(outputState, inputStates[succ]) && !inQueue[succ]) {
            inQueue[succ] = true;
            q.push(succ);
        }
    };

    while (!q.empty()) {
        size_t now = q.front();
        q.pop();
        inQueue[now] = false;

        IR::Inst *inst = insts[now];
        States &inputState = inputStates[now];
        States outputState;

        if (inst->getInstType() == IR::InstType::IfInst) {
            IR::IfInst *ifInst = (IR::IfInst *)inst;
            size_t succ;

            // false branch
            outputState = transferIfStmt(ifInst, inputState, false);
            succ = inst->getSuccessors()[0]->getLabel();
            if (!outputState.isEmpty())
                tryToEnqueue(outputState, succ);

            // true branch
            outputState = transferIfStmt(ifInst, inputState, true);
            succ = inst->getSuccessors()[1]->getLabel();
            if (!outputState.isEmpty())
                tryToEnqueue(outputState, succ);

            continue;
        }

        switch (inst->getInstType()) {
        case IR::InstType::AddInst:
        case IR::InstType::SubInst:
        case IR::InstType::AssignInst:
        case IR::InstType::InputInst:
            outputState = transferAssignment(inst, inputState);
            break;
        case IR::InstType::CheckIntervalInst:
        case IR::InstType::LabelInst:
        case IR::InstType::GotoInst:
            outputState = transferIdentity(inst, inputState);
            break;
        default:
            assert(false);
        }

        if (!inst->getSuccessors().empty()) {
            size_t succ = inst->getSuccessors()[0]->getLabel();
            tryToEnqueue(outputState, succ);
        }
    }

    // Answering the queries
    // std::cerr << "[zone-analysis] Answering the queries" << std::endl;
    for (auto inst : insts) {
        if (inst->getInstType() != IR::InstType::CheckIntervalInst)
            continue;
        IR::CheckIntervalInst *checkInst = (IR::CheckIntervalInst *)inst;
        std::string variable = checkInst->getOperand(0)->getAsVariable();
        long long l = checkInst->getOperand(1)->getAsNumber();
        long long r = checkInst->getOperand(2)->getAsNumber();

        if (inputStates[checkInst->getLabel()].isEmpty()) {
            results[checkInst] = ResultType::UNREACHABLE;
            continue;
        }

        IntervalDomain interval =
            inputStates[inst->getLabel()].projection(variable);
        if (l <= interval.l && interval.r <= r)
            results[checkInst] = ResultType::YES;
        else
            results[checkInst] = ResultType::NO;
    }

    // For debug, dumping the states
    // std::cerr << "[zone-analysis] Dumping the states" << std::endl;
    // for (auto inst : insts) {
    //     inputStates[inst->getLabel()].dump(std::cerr);
    //     inst->dump(std::cerr);
    //     std::cerr << std::endl;
    // }

    // std::cerr << "[zone-analysis] End of analysis" << std::endl << std::endl;
}
