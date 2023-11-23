#include "fdlang/AST.h"
#include "fdlang/ASTTraversePrinter.h"
#include "fdlang/parser.h"
#include "fdlang/scanner.h"
#include "fdlang/sema.h"

#include "analysis/intervalAnalysis.h"
#include "analysis/modelChecker.h"
#include "analysis/relationalNumericalAnalysis.h"

#include "IR/IRBuilder.h"

#include <fstream>
#include <iostream>

std::set<std::string> options;

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

int main(int argc, char *argv[]) {

    if (argc == 1) {
        std::cout << "Usage: fdlang "
                     "[-format] "
                     "[-modelchecker] "
                     "[-interval-analysis] "
                     "[-zone-analysis] "
                     "[-dumpir] "
                     "path-to-src-file"
                  << std::endl;
        std::cout << "e.g.: fdlang -interval-analysis src.fdlang" << std::endl;
        return 0;
    }

    std::string filepath;
    for (int i = 1; i < argc; i++) {
        options.emplace(argv[i]);
        if (i == argc - 1)
            filepath = argv[i];
    }
    bool doFormat = options.count("-format");
    bool doModelChecker = options.count("-modelchecker");
    bool doDumpir = options.count("-dumpir");
    bool doIntervalAnalysis = options.count("-interval-analysis");
    bool doZoneAnalysis = options.count("-zone-analysis");

    std::string src = readSrc(filepath);
    fdlang::Scanner scanner(src);
    std::vector<fdlang::Token> tokens = scanner.scanTokens();
    if (scanner.hadError())
        return 0;

    fdlang::Parser parser(tokens);
    fdlang::ASTNode *root = parser.parse();
    if (parser.hadError())
        return 0;

    fdlang::Sema sema(root);
    if (!sema.check())
        return 0;

    if (doFormat) {
        fdlang::ASTTraversePrinter printer(std::cout);
        root->accept(&printer);
    }

    if (doModelChecker) {
        fdlang::analysis::NaiveModelChecker modelChecker(root);
        modelChecker.run();
        modelChecker.dumpResult(std::cout);
    }

    if (doIntervalAnalysis || doZoneAnalysis || doDumpir) {
        fdlang::IR::IRBuilder irBuilder(root);
        fdlang::IR::Insts insts = irBuilder.build();

        if (doDumpir) {
            for (auto inst : insts) {
                inst->dump(std::cout);
                std::cout << std::endl;
            }
        }

        if (doIntervalAnalysis) {
            fdlang::analysis::IntervalAnalysis analysis(insts);
            analysis.run();
            analysis.dumpResult(std::cout);
        }

        if (doZoneAnalysis) {
            fdlang::analysis::RelationalNumericalAnalysis analysis(insts);
            analysis.run();
            analysis.dumpResult(std::cout);
        }
    }

    return 0;
}