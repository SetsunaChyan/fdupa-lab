#ifndef ANALYSIS_DATAFLOWANALYSIS_H
#define ANALYSIS_DATAFLOWANALYSIS_H

#include "IR/IR.h"

namespace fdlang::analysis {

class DataflowAnalysis {
protected:
    IR::Insts insts;

public:
    DataflowAnalysis(const IR::Insts &insts) : insts(insts) {}

    virtual void run() = 0;

    virtual void dumpResult(std::ostream &out) = 0;
};

} // namespace fdlang::analysis

#endif