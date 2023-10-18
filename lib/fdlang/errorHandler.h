#ifndef FDLANG_ERRORHANDLER_H
#define FDLANG_ERRORHANDLER_H

#include <string>

namespace fdlang {

void error(size_t line, const std::string &msg);

void report(size_t line, const std::string &where, const std::string &msg);

} // namespace fdlang

#endif