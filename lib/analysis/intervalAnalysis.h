#ifndef ANALYSIS_INTERVALANALYSIS_H
#define ANALYSIS_INTERVALANALYSIS_H

#include "IR/IR.h"
#include "dataflowAnalysis.h"

#include <algorithm>
#include <map>
#include <queue>
#include <vector>

namespace fdlang::analysis {
struct Interval {
    long long l = 0, r = 0;
    bool isBottom = true;
    bool operator==(const Interval &o) const {
        if (isBottom != o.isBottom)
            return false;
        if (isBottom == true)
            return true;
        return l == o.l && r == o.r;
    }
};
// variable -> value
using States = std::map<std::string, Interval>;

class IntervalAnalysis : public DataflowAnalysis {
public:
    enum class ResultType { YES, NO, UNREACHABLE };

private:
    std::map<IR::CheckIntervalInst *, ResultType> results;

    // label -> states
    // label is regarded as the index of inst in insts
    std::map<size_t, States> inputStates;

    std::queue<size_t> worklist;

public:
    IntervalAnalysis(const IR::Functions &funcs) : DataflowAnalysis(funcs) {}

    // DO NOT MODIFY THIS FUNCTION
    void dumpResult(std::ostream &out) override {
        using Location = std::pair<size_t, size_t>;

        std::vector<std::pair<Location, ResultType>> ans;
        for (auto [checkInst, result] : results) {
            ans.emplace_back(
                (Location){checkInst->getLine(), checkInst->getLabel()},
                result);
        }
        std::sort(ans.begin(), ans.end());

        for (auto [loc, result] : ans) {
            auto [line, label] = loc;
            out << "Line " << line << ": ";
            if (result == ResultType::UNREACHABLE) {
                out << "Unreachable" << std::endl;
                continue;
            }
            out << (result == ResultType::YES ? "YES" : " NO") << std::endl;
        }
    }

    void run() override {

        fixedPoint();

        checkInstsStates();
    };

    /**
     * @brief compute Fix Point
     * 
     */
    void fixedPoint();

    /****************************************************************
    ********************* Your code starts here *********************
    *****************************************************************/
public:
    /**
     * @brief check insts states
     * 
     */
    void checkInstsStates();

    /**
     * @brief collect all variables and set their value to bottom
     * 
     */
    States iniStates();

    /**
     * @brief transfer function
     * 
     * @param inst 
     * @param input 
     * @return States 
     */
    States transfer(IR::Inst *inst, States &input);

    /**
     * @brief check if sucInputStates == sucInputStates ⊔ outputState  or not
     * @param outputState 
     * @param sucInputStates 
     * @return true 
     * @return false 
     */
    bool joinInto(const States &outputState, States &sucInputStates);

    /**
     * @brief check if now inst outputState change will influence its successors inputState change
     * and add its influenced successors to worklist
     * 
     * @param nowLabel 
     * @param outputState 
     * @see inst label:  inst->getLabel()  
     * inst Successor: inst->getSuccessors()
     * 
     */
    void addSuccessors(size_t nowLabel, States outputState);
};

} // namespace fdlang::analysis
#endif