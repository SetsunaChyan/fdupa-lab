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

    // 1. 收集变量名
    std::unordered_set<std::string> varsSet;
    for (auto inst : insts) {
        for (int i = 0; i < inst->getOperandSize(); i++) {
            if (!inst->getOperand(i)->isVariable())
                continue;
            varsSet.insert(inst->getOperand(i)->getAsVariable());
        }
    }
    std::vector<std::string> vars(varsSet.begin(), varsSet.end());

    // 2. 为所有 label 建立映射，并找到 entryLabel / maxLabel
    std::map<size_t, IR::Inst*> label2Inst;
    size_t maxLabel = 0;
    size_t entryLabel = (size_t)-1;

    for (auto inst : insts) {
        size_t label = inst->getLabel();
        label2Inst[label] = inst;
        maxLabel = std::max(maxLabel, label);
        entryLabel = std::min(entryLabel, label);
    }

    // 3. 初始化各个程序点的状态
    States initState(vars, true), bottomState(vars, false);
    for (auto inst : insts) {
        size_t label = inst->getLabel();
        if (label == entryLabel)
            inputStates[label] = initState;
        else
            inputStates[label] = bottomState;
    }

    // 4. Worklist 算法
    std::vector<bool> inQueue(maxLabel + 1, false); 
    std::queue<size_t> q;
    q.push(entryLabel);
    inQueue[entryLabel] = true;

    auto tryToEnqueue = [&](const States &outputState, const size_t succLabel) {
        if (joinInto(outputState, inputStates[succLabel]) && !inQueue[succLabel]) {
            inQueue[succLabel] = true;
            q.push(succLabel);
        }
    };

    while (!q.empty()) {
        size_t nowLabel = q.front();
        q.pop();
        inQueue[nowLabel] = false;

        // 用 label 查 Inst，而不是 insts[nowLabel]
        IR::Inst *inst = label2Inst[nowLabel];
        States &inputState = inputStates[nowLabel];
        States outputState;

        if (inst->getInstType() == IR::InstType::IfInst) {
            IR::IfInst *ifInst = (IR::IfInst *)inst;
            size_t succ;

            // false branch
            outputState = transferIfStmt(ifInst, inputState, false);
            succ = ifInst->getSuccessors()[0]->getLabel();
            if (!outputState.isEmpty())
                tryToEnqueue(outputState, succ);

            // true branch
            outputState = transferIfStmt(ifInst, inputState, true);
            succ = ifInst->getSuccessors()[1]->getLabel();
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

    // 5. 回答 CheckIntervalInst 查询
    for (auto inst : insts) {
        if (inst->getInstType() != IR::InstType::CheckIntervalInst)
            continue;
        IR::CheckIntervalInst *checkInst = (IR::CheckIntervalInst *)inst;
        std::string variable = checkInst->getOperand(0)->getAsVariable();
        long long l = checkInst->getOperand(1)->getAsNumber();
        long long r = checkInst->getOperand(2)->getAsNumber();

        size_t label = checkInst->getLabel();

        if (inputStates[label].isEmpty()) {
            results[checkInst] = ResultType::UNREACHABLE;
            continue;
        }

        IntervalDomain interval =
            inputStates[label].projection(variable);
        if (l <= interval.l && interval.r <= r)
            results[checkInst] = ResultType::YES;
        else
            results[checkInst] = ResultType::NO;
    }
}
