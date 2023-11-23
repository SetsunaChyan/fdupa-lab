#ifndef ANALYSIS_RELATIONALNUMERICALANALYSIS_H
#define ANALYSIS_RELATIONALNUMERICALANALYSIS_H

#include "dataflowAnalysis.h"
#include "zoneDomain.h"

#include <algorithm>
#include <map>
#include <vector>

namespace fdlang::analysis {

class RelationalNumericalAnalysis : public DataflowAnalysis {
public:
    enum class ResultType { YES, NO, UNREACHABLE };

private:
    std::map<IR::CheckIntervalInst *, ResultType> results;

public:
    RelationalNumericalAnalysis(const IR::Insts &insts)
        : DataflowAnalysis(insts) {}

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

    void run() override;

private:
    using States = ZoneDomain;

    // label -> states
    std::map<size_t, States> inputStates;

    States transferAssignment(const IR::Inst *inst, States &input);
    States transferIdentity(const IR::Inst *inst, States &input);
    States transferIfStmt(const IR::IfInst *inst, States &input, bool branch);

    // x join into y
    bool joinInto(const States &x, States &y);

    void dumpStates(std::ostream &out, States &states);
};

} // namespace fdlang::analysis
#endif