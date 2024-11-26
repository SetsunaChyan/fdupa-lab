#include "IR/IR.h"
#include "analysis/intervalAnalysis.h"
#include <cstddef>
#include <map>
#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

namespace fdlang::analysis {

inline void dumpStates(std::ostream &out, States &states) {
    {
        for (auto &[var, interval] : states) {
            out << " " << var << " = ";
            if (interval.isBottom) {
                out << "Bottom;";
            } else {
                out << "[" << interval.l << ", " << interval.r << "];";
            }
        }
    }
}

enum AnalysisPhase { nocall, caller, callee, returnedCaller };

class CallContext {
public:
    IR::Function *caller;
    IR::Function *callee;
    IR::Inst *callsite;
    States params;
    CallContext(IR::Function *caller, IR::CallInst *callsite,
                States inputStates)
        : caller(caller), callee(callsite->getCallee()), callsite(callsite) {

        for (int i = 0; i < callsite->getArgs().size(); i++) {
            auto callsiteArg = callsite->getArgs()[i];
            auto calleeArg = callee->getArgs()[i];
            params[calleeArg->getAsVariable()] =
                inputStates[callsiteArg->getAsVariable()];
        }
    }
};

class InterAnalysis {
public:
    IR::Functions funcs;
    enum class ResultType { YES, NO, UNREACHABLE };
    // <version,<function,functionEntryStates>>
    std::map<size_t, std::pair<IR::Function *, States>> functionEntryStates;
    // <version,<CheckIntervalInst,ResultType>>
    std::map<IR::CheckIntervalInst *, ResultType> results;

    // label -> states
    // label is regarded as the index of inst in insts
    std::map<size_t, States> inputStates;

    std::deque<IR::Function *> funcWorklist;
    std::queue<size_t> worklist;
    std::set<size_t> inlist;
    std::unordered_map<int, int> visitCount;

    std::stack<CallContext> callcontextStack;
    AnalysisPhase ap;

    InterAnalysis(const IR::Functions &funcs) : funcs(funcs) {
        ap = AnalysisPhase::nocall;
    }

    // DO NOT MODIFY THIS FUNCTION
    void dumpResult(std::ostream &out) {
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
    void run() {

        fixedPoint();

        checkInstsStates();
    };

    /****************************************************************
    ********************* Your code starts here *********************
    *****************************************************************/

    /**
     * @brief Fixed-point computation
     * @details 
        1.ini funcWorkList: push all root function into funcWorklist
        2.do inter-procedual fixpoint computation
     */
    void fixedPoint();

    /**
     * @brief Inter-procedural of Fixed-point computation
     * 
     */
    void interFixedPoint(IR::Function *function);

    /**
     * @brief reset intra-procedural fixpoint-cmp related (worklist,inputStates,etc.)
     * 
     */
    void resetIntra();

    /**
     * @brief Intra-procedural of Fixed-point computation
     * 
     */
    void intraFixedPoint(IR::Function *function);

    /**
     * @brief check insts states
     * 
     */
    void checkInstsStates();

    /**
     * @brief collect all variables(except args) and set their value to bottom
     * 
     */
    States iniStates(IR::Function *function);

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
     * @param nowInst 
     * @param outputState 
     * @see inst label:  inst->getLabel()  
     * inst Successor: inst->getSuccessors()
     * 
     */
    void addSuccessors(IR::Inst *nowInst, States outputState);
};
} // namespace fdlang::analysis