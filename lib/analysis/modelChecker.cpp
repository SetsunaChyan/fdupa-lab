#include "modelChecker.h"

#include "fdlang/AST.h"
#include "fdlang/token.h"

#include <assert.h>
#include <bitset>
#include <iostream>
#include <sstream>
#include <string>

using namespace fdlang;
using namespace fdlang::analysis;

void NaiveModelChecker::evaluate(Stmts *node, Envs &envs) {
    for (auto child : node->children) {
        switch (child->type) {
        case ASTNodeType::UNARY_ASSIGN_STMT:
            evaluate((UnaryAssignStmt *)child, envs);
            break;
        case ASTNodeType::BINARY_ASSIGN_STMT:
            evaluate((BinaryAssignStmt *)child, envs);
            break;
        case ASTNodeType::CHECK_STMT:
            evaluate((CheckStmt *)child, envs);
            break;
        case ASTNodeType::IF_STMT:
            evaluate((IfStmt *)child, envs);
            break;
        case ASTNodeType::WHILE_STMT:
            evaluate((WhileStmt *)child, envs);
            break;
        case ASTNodeType::NOP_STMT:
            evaluate((NopStmt *)child, envs);
            break;
        case ASTNodeType::CALL_STMT:
            evaluate((CallStmt *)child, envs);
            break;
        case ASTNodeType::STMTS:
            evaluate((Stmts *)child, envs);
            break;
        default:
            std::cerr << "[NaiveModelChecker] got "
                      << getASTNodeSpelling(*child) << std::endl;
            assert(false);
        }
    }
}

bool NaiveModelChecker::evaluate(Cond *node, Env &env) {
    std::string variable = node->leftOperand.lexeme;
    long long x = env[variable];
    long long y = node->rightOperand.getLiteralAsNumber();
    switch (node->op.type) {
    case TokenType::EQUAL_EQUAL:
        return x == y;
    case TokenType::GREATER_EQUAL:
        return x >= y;
    case TokenType::LESS_EQUAL:
        return x <= y;
    case TokenType::GREATER:
        return x > y;
    case TokenType::LESS:
        return x < y;
    default:
        std::cerr << "[NaiveModelChecker] got " << getASTNodeSpelling(*node)
                  << std::endl;
        assert(false);
    }
    return false;
}

void NaiveModelChecker::evaluate(UnaryAssignStmt *node, Envs &envs) {
    Envs newEnvs;
    std::string variable = node->variable.lexeme;
    for (auto &env : envs) {
        switch (node->operand.type) {
        case TokenType::CALL_INPUT: {
            for (int i = 0; i < 256; i++) {
                Env newEnv = env;
                newEnv[variable] = i;
                newEnvs.insert(newEnv);
            }
            break;
        }
        case TokenType::NUMBER: {
            Env newEnv = env;
            long long x = node->operand.getLiteralAsNumber();
            newEnv[variable] = x;
            newEnvs.insert(newEnv);
            break;
        }
        case TokenType::IDENTIFIER: {
            Env newEnv = env;
            long long x = newEnv[node->operand.lexeme];
            newEnv[variable] = x;
            newEnvs.insert(newEnv);
            break;
        }
        default:
            std::cerr << "[NaiveModelChecker] got " << getASTNodeSpelling(*node)
                      << std::endl;
            assert(false);
        }
    }
    swap(envs, newEnvs);
}

void NaiveModelChecker::evaluate(BinaryAssignStmt *node, Envs &envs) {
    Envs newEnvs;
    std::string variable = node->variable.lexeme;
    for (auto &env : envs) {
        Env newEnv = env;
        long long x, y;
        if (node->leftOperand.type == TokenType::IDENTIFIER)
            x = newEnv[node->leftOperand.lexeme];
        else
            x = node->leftOperand.getLiteralAsNumber();
        if (node->rightOperand.type == TokenType::IDENTIFIER)
            y = newEnv[node->rightOperand.lexeme];
        else
            y = node->rightOperand.getLiteralAsNumber();
        switch (node->op.type) {
        case TokenType::PLUS:
            newEnv[variable] = std::min(255ll, x + y);
            break;
        case TokenType::MINUS:
            newEnv[variable] = std::max(0ll, x - y);
            break;
        default:
            assert(false);
        }
        newEnvs.insert(newEnv);
    }
    swap(envs, newEnvs);
}

void NaiveModelChecker::evaluate(IfStmt *node, Envs &envs) {
    Envs newEnvs;
    for (auto &env : envs) {
        Env newEnv = env;
        Envs candidateEnvs = {newEnv};
        bool cond = evaluate((Cond *)node->cond, newEnv);
        if (cond)
            evaluate((Stmts *)node->trueBody, candidateEnvs);
        else
            evaluate((Stmts *)node->falseBody, candidateEnvs);
        newEnvs.insert(candidateEnvs.begin(), candidateEnvs.end());
    }
    swap(envs, newEnvs);
}

void NaiveModelChecker::evaluate(WhileStmt *node, Envs &envs) {
    Envs newEnvs;
    Envs visited = envs;
    std::queue<Env> q;
    for (auto &env : envs)
        q.push(env);

    while (!q.empty()) {
        Env newEnv = q.front();
        q.pop();
        bool cond = evaluate((Cond *)node->cond, newEnv);
        if (cond) {
            Envs candidateEnvs = {newEnv};
            evaluate((Stmts *)node->body, candidateEnvs);
            for (auto &env : candidateEnvs) {
                if (visited.count(env))
                    continue;
                visited.insert(env);
                q.push(env);
            }
        } else
            newEnvs.insert(newEnv);
    }

    swap(envs, newEnvs);
}

void NaiveModelChecker::evaluate(CheckStmt *node, Envs &envs) {
    std::string variable = node->params[0].lexeme;
    for (auto &env : envs) {
        long long v = env.count(variable) ? env.at(variable) : 0ll;
        reachableValue[node->label].set(v, 1);
    }
}

void NaiveModelChecker::evaluate(NopStmt *node, Envs &envs) {}

void NaiveModelChecker::evaluate(CallStmt *node, Envs &envs) {
    Envs newEnvs;
    for (auto env : envs) {
        Env newEnv;
        for (int i = 0; i < node->args.size(); i++) {
            newEnv[std::to_string(i)] = env[node->args[i].lexeme];
        }
        newEnvs.insert(newEnv);
    }
    evaluate(node->callee, newEnvs);
}

void NaiveModelChecker::evaluate(FunctionNodes *node, Envs &envs) {
    for (auto child : node->children) {
        auto function = dynamic_cast<FunctionNode *>(child);
        if (function->isRoot())
            evaluate((FunctionNode *)child, envs);
    }
}

void NaiveModelChecker::evaluate(FunctionNode *node, Envs &envs) {
    if (node->isRoot()) {
        evaluate((Stmts *)node->body, envs);
    } else {
        Envs newEnvs;
        for (auto env : envs) {
            Env newEnv;
            for (int i = 0; i < node->args.size(); i++) {
                newEnv[node->args[i].lexeme] = env[std::to_string(i)];
            }
            newEnvs.insert(newEnv);
        }
        evaluate((Stmts *)node->body, newEnvs);
    }
}

void NaiveModelChecker::run() {
    root->accept(&info);
    Env initEnv;
    Envs initEnvs = {initEnv};
    evaluate((FunctionNodes *)root, initEnvs);
}

void NaiveModelChecker::dumpResult(std::ostream &out) {
    for (auto &checkStmt : info.checks) {
        size_t id = checkStmt->label;
        std::string variable = checkStmt->params[0].lexeme;
        long long l = checkStmt->params[1].getLiteralAsNumber();
        long long r = checkStmt->params[2].getLiteralAsNumber();
        out << "Line " << checkStmt->check.line << ": ";
        if (!reachableValue.count(id)) {
            out << "Unreachable" << std::endl;
            continue;
        }
        std::stringstream ss;
        bool ok = true;
        const std::bitset<256> &vs = reachableValue[id];
        ss << variable << " in ";
        for (int i = 0, last, ready = 0, cnt = 0; i <= 256; i++) {
            if (i < 256 && vs[i] == 1 && !ready)
                last = i, ready = 1;
            else if ((i == 256 || vs[i] == 0) && ready) {
                if (cnt != 0)
                    ss << " U ";
                ss << "[" << last << ", " << i - 1 << "]";
                cnt++, ready = 0;
            }
            if (i < 256 && vs[i] == 1 && (i < l || i > r))
                ok = false;
        }
        out << (ok ? "YES" : " NO") << "; " << ss.str();
        out << std::endl;
    }
}
