#ifndef ANALYSIS_DATAFLOWANALYSIS_H
#define ANALYSIS_DATAFLOWANALYSIS_H

#include "IR/IR.h"

namespace fdlang::analysis {

class DataflowAnalysis {
protected:
    IR::Insts insts;
    IR::Functions funcs;

public:
    DataflowAnalysis(const IR::Functions &funcs) : funcs(funcs) {
        insts = funcs[0]->getInsts();
    }

    virtual void run() = 0;

    virtual void dumpResult(std::ostream &out) = 0;
};

} // namespace fdlang::analysis

#endif