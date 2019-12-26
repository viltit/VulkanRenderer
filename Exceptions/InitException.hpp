#pragma once

#include <string>
#include <stdexcept>

namespace moe {

struct InitException : public std::runtime_error {
public:
    InitException(const char *msg, const char *file, const char *func, int line)
            : runtime_error(msg),
              file{file},
              func{func},
              line{line} {}

    InitException(const std::string& msg, const char *file, const char *func, int line)
            : runtime_error(msg.c_str()),
              file{file},
              func{func},
              line{line} {}

    const char *file;
    const char *func;
    int line;

};
}