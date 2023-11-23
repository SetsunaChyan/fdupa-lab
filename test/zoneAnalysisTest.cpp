#include "gtest/gtest.h"

#include "fdlang/AST.h"
#include "fdlang/ASTTraversePrinter.h"
#include "fdlang/parser.h"
#include "fdlang/scanner.h"
#include "fdlang/sema.h"

#include "analysis/modelChecker.h"
#include "analysis/relationalNumericalAnalysis.h"

#include "IR/IRBuilder.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace fdlang;

size_t total, true_positive, false_positive, false_negtive;

std::string readSrc(const std::string &path) {
    std::ifstream file(path);
    file.seekg(0, std::ios::end);
    std::streamoff fileLength = file.tellg();
    file.seekg(0, std::ios::beg);

    char *buffer = new char[fileLength + 1];
    file.read(buffer, fileLength);
    buffer[fileLength] = '\0';
    std::string ret = buffer;

    if (buffer)
        delete[] buffer;

    return ret;
}

void compare(const std::string &our, const std::string &expected) {
    enum Ans { YES, NO, UNREACHABLE };
    std::vector<Ans> v1, v2;
    std::string s;

    auto parseAnswer = [&](const std::string &text, std::vector<Ans> &v) {
        std::stringstream ss;
        ss << text;
        while (getline(ss, s)) {
            if (s.find("YES") != s.npos)
                v.push_back(YES);
            else if (s.find("NO") != s.npos)
                v.push_back(NO);
            else if (s.find("Unreachable") != s.npos)
                v.push_back(UNREACHABLE);
            else
                EXPECT_TRUE(false);
        }
    };

    parseAnswer(our, v1);
    parseAnswer(expected, v2);

    EXPECT_EQ(v1.size(), v2.size());
    for (int i = 0; i < v1.size(); i++) {
        total++;
        if (v1[i] == v2[i])
            true_positive++;
        else if (v2[i] == UNREACHABLE || v2[i] == YES && v1[i] == NO)
            false_positive++;
        else
            false_negtive++;
    }
}

void check(std::string &filepath) {
    std::string src = readSrc(TESTCASES_DIR "/" + filepath);
    std::string expected = readSrc(TESTCASES_DIR "/" + filepath + ".expected");
    std::stringstream result;

    fdlang::Scanner scanner(src);
    std::vector<fdlang::Token> tokens = scanner.scanTokens();
    EXPECT_FALSE(scanner.hadError());

    fdlang::Parser parser(tokens);
    fdlang::ASTNode *root = parser.parse();
    EXPECT_FALSE(parser.hadError());

    fdlang::Sema sema(root);
    EXPECT_TRUE(sema.check());

    fdlang::IR::IRBuilder irBuilder(root);
    fdlang::IR::Insts insts = irBuilder.build();

    fdlang::analysis::RelationalNumericalAnalysis analysis(insts);
    analysis.run();
    analysis.dumpResult(result);

    compare(result.str(), expected);
}

TEST(RelationalNumericalAnalysis, RunAll) {

    std::vector<std::string> files = {
        "branch1.fdlang",   "branch2.fdlang",   "corner.fdlang",
        "deadcode1.fdlang", "deadcode2.fdlang", "loop1.fdlang",
        "loop2.fdlang",     "loop3.fdlang",     "loop4.fdlang",
        "loop5.fdlang",     "nobranch1.fdlang", "nobranch2.fdlang",
        "nobranch3.fdlang", "rel1.fdlang",      "rel2.fdlang",
        "rel3.fdlang",      "rel4.fdlang"};

    for (auto &filepath : files)
        check(filepath);

    printf("Total: %lu\n", total);
    double precision =
        100.0 * (double)true_positive / double(true_positive + false_positive);
    if (true_positive + false_positive == 0)
        precision = 0;
    printf("Precision: %.3lf%%\n", precision);
    double recall =
        100.0 * (double)true_positive / double(true_positive + false_negtive);
    if (true_positive + false_negtive == 0)
        recall = 0;
    printf("Recall: %.3lf%%\n", recall);
}