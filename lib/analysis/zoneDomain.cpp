#include "zoneDomain.h"

#include <algorithm>
#include <cstddef>
#include <vector>

using namespace fdlang::analysis;

const long long ZoneDomain::INF = 0x3f3f3f3f;

/**
 * @brief Construct a new Zone Domain
 *
 * @param vars names of appeared variables
 * @param isInitialization true for initialization(all zero) and false for
 * bottom
 */
ZoneDomain::ZoneDomain(const std::vector<std::string> &vars,
                       bool isInitialization) {
    _id_to_var.push_back("");
    _var_to_id[""] = 0;
    for (size_t i = 0; i < vars.size(); i++) {
        size_t id = i + 1;
        _id_to_var.push_back(vars[i]);
        _var_to_id[vars[i]] = id;
    }
    n = _id_to_var.size();
    for (size_t i = 0; i < n; i++)
        _dbm.emplace_back(n, INF);
    if (isInitialization) {
        for (size_t i = 0; i < n; i++)
            _dbm[i][i] = _dbm[0][i] = _dbm[i][0] = 0;
    } else {
        for (size_t i = 0; i < n; i++)
            for (size_t j = 0; j < n; j++)
                _dbm[i][j] = -INF;
    }
}

void ZoneDomain::dump(std::ostream &out) const {
    if (isEmpty()) {
        out << "; Unreachable" << std::endl;
        return;
    }

    for (int i = 1; i < n; i++) {
        std::string x = getVar(i);
        IntervalDomain interval = this->projection(x);
        out << "; " << x << " = [" << interval.l << ", " << interval.r << "]"
            << std::endl;
    }

    for (int i = 1; i < n; i++) {
        std::string x = getVar(i);
        for (int j = 1; j < n; j++) {
            if (i == j)
                continue;
            std::string y = getVar(j);
            long long c = _dbm[i][j];
            if (c >= INF)
                continue;
            out << "; ";
            out << x << " ";
            out << "- " << y << " ";
            out << "<= " << c << std::endl;
        }
    }
}

/**
 * @brief Get the new zone which is the normal form of `*this'
 */
ZoneDomain ZoneDomain::normalize() const {
    ZoneDomain ret = *this;

    // todo: Floyd (about 4 lines)

    return ret;
}

/**
 * @brief Test if `*this' is bottom
 */
bool ZoneDomain::isEmpty() const {

    // todo: Determine if there are negative loops (about 4 lines)

    return false;
}

/**
 * @brief Test if `*this' is less or equal than `o' in partial order <=
 */
bool ZoneDomain::leq(const ZoneDomain &o) const {
    // Assume `*this' is already normalized
    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            if (this->_dbm[i][j] > o._dbm[i][j])
                return false;
    return true;
}

/**
 * @brief Test if `*this' is equal to `o'
 */
bool ZoneDomain::eq(const ZoneDomain &o) const {
    // Assume `*this' is already normalized
    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            if (this->_dbm[i][j] != o._dbm[i][j])
                return false;
    return true;
}

/**
 * @brief Get the projection of `*this' on the variable `x'
 */
IntervalDomain ZoneDomain::projection(const std::string &x) const {
    size_t id = getID(x);
    return IntervalDomain(-_dbm[id][0], _dbm[0][id]);
}

/**
 * @brief Get the new zone which is the least upper bound of `*this' and `o'
 */
ZoneDomain ZoneDomain::lub(const ZoneDomain &o) const {
    ZoneDomain ret = *this;

    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++)
            ret._dbm[i][j] = std::max(this->_dbm[i][j], o._dbm[i][j]);

    ret = ret.normalize();
    return ret;
}

/**
 * @brief Get the new zone which forgets the variable `x'
 */
ZoneDomain ZoneDomain::forget(const std::string &x) const {
    ZoneDomain ret = *this;
    size_t k = getID(x);

    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++) {
            if (i != k && j != k)
                ret._dbm[i][j] = std::min(this->_dbm[i][j],
                                          this->_dbm[i][k] + this->_dbm[k][j]);
            else if (i == j && j == k)
                ret._dbm[i][j] = 0;
            else
                ret._dbm[i][j] = INF;
        }
    ret._dbm[k][0] = 0;
    ret._dbm[0][k] = 255;

    ret = ret.normalize();
    return ret;
}

/**
 * @brief Get the new zone filtered by `inst'
 */
ZoneDomain ZoneDomain::filterInst(const IR::IfInst *inst, bool branch) const {

    std::string x = inst->getOperand(0)->getAsVariable();
    long long c = inst->getOperand(1)->getAsNumber();
    IR::CmpOperator op = inst->getCmpOperator();
    ZoneDomain ret = *this;

    if (!branch) {
        // In fact, the condition that `x != v' can also reduce the state
        // space, but it is only effective when the boundary is equal to v. For
        // simplicity, we do not filter it, which has almost no impact on
        // precision
        if (op == IR::CmpOperator::EQ)
            return ret;

        // In other cases, we invert the conditions and filter them
        switch (op) {
        case IR::CmpOperator::GT:
            op = IR::CmpOperator::LEQ;
            break;
        case IR::CmpOperator::GEQ:
            op = IR::CmpOperator::LT;
            break;
        case IR::CmpOperator::LT:
            op = IR::CmpOperator::GEQ;
            break;
        case IR::CmpOperator::LEQ:
            op = IR::CmpOperator::GT;
            break;
        default:
            assert(false);
        }
    }

    switch (op) {
    case IR::CmpOperator::EQ:
        ret = this->filter(x, "", c).filter("", x, -c);
        break;
    case IR::CmpOperator::GEQ:
        ret = this->filter("", x, -c);
        break;
    case IR::CmpOperator::GT:
        ret = this->filter("", x, -(c + 1));
        break;
    case IR::CmpOperator::LEQ:
        ret = this->filter(x, "", c);
        break;
    case IR::CmpOperator::LT:
        ret = this->filter(x, "", c - 1);
        break;
    default:
        assert(false);
    }

    ret = ret.normalize();

    return ret;
}

/**
 * @brief Get the new zone filtered by guard `x - y <= c'
 *
 * For case `x <= c', we set y = ""
 * For case `-y <= c', we set x = ""
 */
ZoneDomain ZoneDomain::filter(const std::string &x, const std::string &y,
                              long long c) const {
    ZoneDomain ret = *this;

    // todo: add constraint `x - y <= c' (about 3 lines)

    return ret;
}

/**
 * @brief Get the new zone after excuting assigment/add/sub `inst'
 */
ZoneDomain ZoneDomain::assignInst(const IR::Inst *inst) const {
    std::string x;
    ZoneDomain ret;

    if (const IR::AddInst *addInst = dynamic_cast<const IR::AddInst *>(inst)) {
        x = addInst->getOperand(0)->getAsVariable();
        IR::Value *operand1 = addInst->getOperand(1);
        IR::Value *operand2 = addInst->getOperand(2);

        // case: x <- c1 + c2
        if (operand1->isNumber() && operand2->isNumber())
            ret = this->assign_case2(
                x, "", operand1->getAsNumber() + operand2->getAsNumber());
        // case: x <- y + c || x <- c + y
        else if (operand1->isNumber() || operand2->isNumber()) {
            if (operand1->isNumber())
                std::swap(operand1, operand2);
            std::string y = operand1->getAsVariable();
            long long c = operand2->getAsNumber();
            // subcase: x == y
            if (x == y)
                ret = this->assign_case1(x, c);
            // subcase: x != y
            else
                ret = this->assign_case2(x, y, c);
        }
        // case: x <- y + z
        else {
            IntervalDomain interval1 =
                this->projection(operand1->getAsVariable());
            IntervalDomain interval2 =
                this->projection(operand2->getAsVariable());
            long long l = interval1.l + interval2.l;
            long long r = interval1.r + interval2.r;
            ret = this->assign_case3(x, l, r);
        }
    }
    if (const IR::SubInst *subInst = dynamic_cast<const IR::SubInst *>(inst)) {
        x = subInst->getOperand(0)->getAsVariable();
        IR::Value *operand1 = subInst->getOperand(1);
        IR::Value *operand2 = subInst->getOperand(2);

        // case: x <- c1 - c2
        if (operand1->isNumber() && operand2->isNumber())
            ret = this->assign_case2(
                x, "", operand1->getAsNumber() - operand2->getAsNumber());
        // case: x <- y - c
        else if (operand2->isNumber()) {
            std::string y = operand1->getAsVariable();
            long long c = operand2->getAsNumber();
            // subcase: x == y
            if (x == y)
                ret = this->assign_case1(x, -c);
            // subcase: x != y
            else
                ret = this->assign_case2(x, y, -c);
        }
        // case: x <- c - y
        else if (operand1->isNumber()) {
            long long c = operand1->getAsNumber();
            IntervalDomain interval =
                this->projection(operand2->getAsVariable());
            long long l = c - interval.r;
            long long r = c - interval.l;
            ret = this->assign_case3(x, l, r);
        }
        // case: x <- y - z
        else {
            IntervalDomain interval1 =
                this->projection(operand1->getAsVariable());
            IntervalDomain interval2 =
                this->projection(operand2->getAsVariable());
            long long l = interval1.l - interval2.r;
            long long r = interval1.r - interval2.l;
            ret = this->assign_case3(x, l, r);
        }
    }
    if (const IR::AssignInst *assignInst =
            dynamic_cast<const IR::AssignInst *>(inst)) {
        x = assignInst->getOperand(0)->getAsVariable();
        IR::Value *operand = assignInst->getOperand(1);

        // case: x <- c
        if (operand->isNumber())
            ret = this->assign_case2(x, "", operand->getAsNumber());
        // case: x <- y
        else if (operand->isVariable()) {
            std::string y = operand->getAsVariable();
            // subcase: x == y
            if (x == y)
                ret = *this;
            // subcase: x != y
            else
                ret = this->assign_case2(x, y, 0);
        }
    }
    if (const IR::InputInst *inputInst =
            dynamic_cast<const IR::InputInst *>(inst)) {
        // x <- [0, 255]
        x = inputInst->getOperand(0)->getAsVariable();
        ret = this->assign_case3(x, 0, 255);
    }

    ret = ret.normalize();
    return ret;
}

/**
 * @brief Get the new zone after excuting `x = x + c'
 */
ZoneDomain ZoneDomain::assign_case1(const std::string &x, long long c) const {
    size_t i0 = getID(x);
    ZoneDomain ret = *this;

    long long pc = c;
    pc = std::min(pc, 255ll - ret._dbm[0][i0]);
    pc = std::max(pc, -ret._dbm[0][i0]);
    long long mc = c;
    mc = std::min(mc, 255ll + ret._dbm[i0][0]);
    mc = std::max(mc, ret._dbm[i0][0]);

    for (size_t i = 0; i < n; i++)
        for (size_t j = 0; j < n; j++) {
            if (i == i0 && j != i0) {
                ret._dbm[i][j] -= mc;
            }
            if (i != i0 && j == i0) {
                ret._dbm[i][j] += pc;
            }
        }

    return ret;
}

/**
 * @brief Get the new zone after excuting `x = y + c' or `x = c'
 *
 * For case `x = c', we set y = ""
 */
ZoneDomain ZoneDomain::assign_case2(const std::string &x, const std::string &y,
                                    long long c) const {
    ZoneDomain ret;

    // todo: (about 1 line)

    return ret;
}

/**
 * @brief Get the new zone after excuting `x = [l, r]'
 */
ZoneDomain ZoneDomain::assign_case3(const std::string &x, long long l,
                                    long long r) const {
    l = std::max(l, 0ll);
    r = std::min(r, 255ll);

    // todo: (about 4 lines)

    return ret;
}
