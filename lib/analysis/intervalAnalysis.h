#ifndef ANALYSIS_INTERVALANALYSIS_H
#define ANALYSIS_INTERVALANALYSIS_H

#include "dataflowAnalysis.h"

#include <algorithm>
#include <map>
#include <vector>

namespace fdlang::analysis {

enum class ResultType { YES, NO, UNREACHABLE };

class IntervalAnalysis : public DataflowAnalysis {
private:
    std::map<IR::CheckIntervalInst *, ResultType> results;

public:
    IntervalAnalysis(const IR::Insts &insts) : DataflowAnalysis(insts) {}

    // DO NOT MODIFY THIS FUNCTION
    void dumpResult(std::ostream &out) override {
        using Location = std::pair<size_t, size_t>;

        std::vector<std::pair<Location, ResultType>> ans;
        for (auto [checkInst, result] : results) {
            ans.emplace_back(
                (Location){checkInst->getLine(), checkInst->getLabel()},
                result);
        }
        std::sort(
            ans.begin(), ans.end(),
            [](const std::pair<Location, ResultType> &x,
               const std::pair<Location, ResultType> &y) { return x < y; });

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
    /**
     * Your code starts here
     */
};

} // namespace fdlang::analysis
#endif