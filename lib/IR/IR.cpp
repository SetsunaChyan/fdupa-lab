#include "IR.h"
#include <string>

using namespace fdlang::IR;

std::string fdlang::IR::getCmpOperatorSpelling(CmpOperator op) {
    switch (op) {
    case CmpOperator::EQ:
        return "==";
    case CmpOperator::GEQ:
        return ">=";
    case CmpOperator::GT:
        return ">";
    case CmpOperator::LEQ:
        return "<=";
    case CmpOperator::LT:
        return "<";
    default:
        break;
    }
    return "UNKNOWN";
}

static std::string labelPrefix(size_t label, size_t size = 3) {
    std::string ret = "L" + std::to_string(label);
    while (ret.size() < size)
        ret.push_back(' ');
    return ret + ":  ";
}

// operand0 = operand1 + operand2;
void AddInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    getOperand(0)->dump(out);
    out << " = ";
    getOperand(1)->dump(out);
    out << " + ";
    getOperand(2)->dump(out);
    out << ";";
}

// operand0 = operand1 - operand2;
void SubInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    getOperand(0)->dump(out);
    out << " = ";
    getOperand(1)->dump(out);
    out << " - ";
    getOperand(2)->dump(out);
    out << ";";
}

// operand0 = input();
void InputInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    getOperand(0)->dump(out);
    out << " = input();";
}

// operand0 = operand1;
void AssignInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    getOperand(0)->dump(out);
    out << " = ";
    getOperand(1)->dump(out);
    out << ";";
}

// check_interval(operand0, operand1, operand2);
void CheckIntervalInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    out << "check_interval(";
    getOperand(0)->dump(out);
    out << ", ";
    getOperand(1)->dump(out);
    out << ", ";
    getOperand(2)->dump(out);
    out << ");";
}

// if operand0 cmpop operand1 then goto dest;
void IfInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    out << "if ";
    getOperand(0)->dump(out);
    out << " " << getCmpOperatorSpelling(getCmpOperator()) << " ";
    getOperand(1)->dump(out);
    out << " then goto L" << dest->getLabel() << ";";
}

// goto dest;
void GotoInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
    out << "goto L" << dest->getLabel() << ";";
}

void LabelInst::dump(std::ostream &out) const {
    out << labelPrefix(getLabel());
}
