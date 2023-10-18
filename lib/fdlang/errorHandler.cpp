#include "errorHandler.h"

#include <iostream>

void fdlang::error(size_t line, const std::string &msg) {
    fdlang::report(line, "", msg);
}

void fdlang::report(size_t line, const std::string &where,
                    const std::string &msg) {
    std::cerr << "[line " << line << "] Error" << where << ": " << msg
              << std::endl;
}