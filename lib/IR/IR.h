#ifndef IR_IR_H
#define IR_IR_H

#include "fdlang/token.h"

#include <any>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

namespace fdlang::IR {

enum class ValueType { Number, Variable };

/**
 * EQ  =
 * GT  >
 * GEQ >=
 * LT  <
 * LEQ <=
 */
enum class CmpOperator { EQ, GT, GEQ, LT, LEQ };

enum class InstType {
    AddInst,
    SubInst,
    InputInst,
    AssignInst,
    CheckIntervalInst,
    IfInst,
    GotoInst,
    LabelInst
};

std::string getCmpOperatorSpelling(CmpOperator op);

class Value {
private:
    ValueType type;
    std::any value;

public:
    Value(const Token &token) {
        if (token.type == TokenType::IDENTIFIER) {
            value = token.lexeme;
            type = ValueType::Variable;
        } else if (token.type == TokenType::NUMBER) {
            value = token.getLiteralAsNumber();
            type = ValueType::Number;
        } else {
            assert(false && "Value must be long long integer or std::string!");
        }
    }

    bool isNumber() { return type == ValueType::Number; }

    bool isVariable() { return type == ValueType::Variable; };

    long long getAsNumber() {
        assert(isNumber());
        return std::any_cast<long long>(value);
    }

    std::string getAsVariable() {
        assert(isVariable());
        return std::any_cast<std::string>(value);
    }

    void dump(std::ostream &out, bool showType = false) {
        if (showType) {
            if (isNumber())
                out << "Number(" << getAsNumber() << ")";
            else if (isVariable())
                out << "Var(" << getAsVariable() << ")";
        } else {
            if (isNumber())
                out << getAsNumber();
            else if (isVariable())
                out << getAsVariable();
        }
    }
};

class Inst {
protected:
    InstType type;
    size_t label;
    std::vector<Value *> operands;
    std::vector<Inst *> successors, predecessors;
    Inst(const std::vector<Value *> &ops = {}) : operands(ops) {}

    friend class IRBuilder;

    void setLabel(size_t l) { label = l; }

    void addSuccessor(Inst *inst) { successors.push_back(inst); }

    void addPredecessor(Inst *inst) { predecessors.push_back(inst); }

public:
    virtual void dump(std::ostream &out) const = 0;

    Value *getOperand(size_t id) const {
        assert(id < operands.size() &&
               "The index cannot exceed the number of operands");
        return operands[id];
    }

    size_t getOperandSize() const { return operands.size(); }

    InstType getInstType() const { return type; };

    size_t getLabel() const { return label; }

    const std::vector<Inst *> &getSuccessors() const { return successors; }

    const std::vector<Inst *> &getPredecessors() const { return predecessors; }

    virtual ~Inst() {
        for (Value *v : operands)
            delete v;
    }
};

// operand0 = operand1 + operand2;
class AddInst : public Inst {
public:
    AddInst(Value *operand0, Value *operand1, Value *operand2)
        : Inst({operand0, operand1, operand2}) {
        type = InstType::AddInst;
    }

    virtual void dump(std::ostream &out) const override;
};

// operand0 = operand1 - operand2;
class SubInst : public Inst {
public:
    SubInst(Value *operand0, Value *operand1, Value *operand2)
        : Inst({operand0, operand1, operand2}) {
        type = InstType::SubInst;
    }

    virtual void dump(std::ostream &out) const override;
};

// operand0 = input();
class InputInst : public Inst {
public:
    InputInst(Value *operand0) : Inst({operand0}) {
        type = InstType::InputInst;
    }

    virtual void dump(std::ostream &out) const override;
};

// operand0 = operand1;
class AssignInst : public Inst {
public:
    AssignInst(Value *operand0, Value *operand1) : Inst({operand0, operand1}) {
        type = InstType::AssignInst;
    }

    virtual void dump(std::ostream &out) const override;
};

// check_interval(operand0, operand1, operand2);
class CheckIntervalInst : public Inst {
private:
    size_t line;

public:
    CheckIntervalInst(Value *operand0, Value *operand1, Value *operand2)
        : Inst({operand0, operand1, operand2}) {
        type = InstType::CheckIntervalInst;
    }

    virtual void dump(std::ostream &out) const override;

    void setLine(size_t line) { this->line = line; }

    size_t getLine() { return line; }
};

// if operand0 cmpop operand1 then goto dest;
class IfInst : public Inst {
private:
    std::vector<Value *> operands;
    CmpOperator cmpop;
    Inst *dest;

public:
    IfInst(Value *operand0, CmpOperator cmpop, Value *operand1, Inst *dest)
        : Inst({operand0, operand1}), cmpop(cmpop), dest(dest) {
        type = InstType::IfInst;
    }

    Inst *getDestInst() const { return dest; }

    CmpOperator getCmpOperator() const { return cmpop; }

    virtual void dump(std::ostream &out) const override;
};

// goto dest;
class GotoInst : public Inst {
private:
    Inst *dest;

public:
    GotoInst(Inst *dest) : dest(dest) { type = InstType::GotoInst; }

    Inst *getDestInst() { return dest; }

    virtual void dump(std::ostream &out) const override;
};

class LabelInst : public Inst {
public:
    LabelInst() { type = InstType::LabelInst; }

    virtual void dump(std::ostream &out) const override;
};

using Insts = std::vector<Inst *>;

} // namespace fdlang::IR

#endif