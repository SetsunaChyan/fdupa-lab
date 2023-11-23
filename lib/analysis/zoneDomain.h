#ifndef ANALYSIS_QUADRIALATERALDOMAIN_H
#define ANALYSIS_QUADRIALATERALDOMAIN_H

#include "IR/IR.h"

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace fdlang::analysis {

class IntervalDomain {
public:
    long long l, r;
    IntervalDomain(long long l, long long r) : l(l), r(r) {}
};

class ZoneDomain {
private:
    static const long long INF;
    size_t n;

    std::vector<std::string> _id_to_var;
    std::unordered_map<std::string, size_t> _var_to_id;

    using Matrix = std::vector<std::vector<long long>>;
    /**
     * var_i - 0 <= _dbm[i][0]
     * 0 - var_i <= _dbm[0][i]
     * var_i - var_j <= _dbm[i][j]
     */
    Matrix _dbm;

    std::string getVar(size_t id) const {
        assert(0 <= id && id < _id_to_var.size());
        return _id_to_var[id];
    }

    size_t getID(const std::string &x) const {
        auto it = _var_to_id.find(x);
        assert(it != _var_to_id.end());
        return it->second;
    }

public:
    /**
     * @brief Construct a new Zone Domain
     *
     * @param vars names of appeared variables
     * @param isInitialization true for initialization(all zero) and false for
     * bottom
     */
    ZoneDomain(const std::vector<std::string> &vars, bool isInitialization);
    ZoneDomain() = default;

    void dump(std::ostream &out) const;

    /**
     * @brief Get the new zone which is the normal form of `*this'
     */
    ZoneDomain normalize() const;

    /**
     * @brief Test if `*this' is bottom
     */
    bool isEmpty() const;

    /**
     * @brief Test if `*this' is less or equal than `o' in partial order <=
     */
    bool leq(const ZoneDomain &o) const;

    /**
     * @brief Test if `*this' is equal to `o'
     */
    bool eq(const ZoneDomain &o) const;

    /**
     * @brief Get the projection of `*this' on the variable `x'
     */
    IntervalDomain projection(const std::string &x) const;

    /**
     * @brief Get the new zone which is the least upper bound of `*this' and `o'
     */
    ZoneDomain lub(const ZoneDomain &o) const;

    /**
     * @brief Get the new zone which forgets the variable `x'
     */
    ZoneDomain forget(const std::string &x) const;

    /**
     * @brief Get the new zone filtered by `inst'
     */
    ZoneDomain filterInst(const IR::IfInst *inst, bool branch) const;

    /**
     * @brief Get the new zone filtered by guard `x - y <= c'
     *
     * For case `x <= c', we set y = ""
     * For case `-y <= c', we set x = ""
     */
    ZoneDomain filter(const std::string &x, const std::string &y,
                      long long c) const;

    /**
     * @brief Get the new zone after excuting assigment/add/sub `inst'
     */
    ZoneDomain assignInst(const IR::Inst *inst) const;

    /**
     * @brief Get the new zone after excuting `x = x + c'
     */
    ZoneDomain assign_case1(const std::string &x, long long c) const;

    /**
     * @brief Get the new zone after excuting `x = y + c' or `x = c'
     *
     * For case `x = c', we set y = ""
     */
    ZoneDomain assign_case2(const std::string &x, const std::string &y,
                            long long c) const;

    /**
     * @brief Get the new zone after excuting `x = [l, r]'
     */
    ZoneDomain assign_case3(const std::string &x, long long l,
                            long long r) const;
};

} // namespace fdlang::analysis

#endif