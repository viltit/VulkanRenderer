#pragma once

#include <string>
#include <stdexcept>

namespace moe {

struct MoeInitError : public std::runtime_error {
public:
    MoeInitError(const char *msg, const char *file, const char *func, int line)
            : runtime_error(msg),
              file{file},
              func{func},
              line{line} {}

    MoeInitError(const std::string& msg, const char *file, const char *func, int line)
            : runtime_error(msg.c_str()),
              file{file},
              func{func},
              line{line} {}

    const char *file;
    const char *func;
    int line;
};

struct MoeFileError : public std::runtime_error {
    MoeFileError(const std::string& filename, const char *file, const char *func, int line)
            : runtime_error(filename),
              filename{filename},
              file{file},
              func{func},
              line{line} {}



    const std::string filename;
    const char *file;
    const char *func;
    int line;
};

struct MoeFileNotOpenedError : MoeFileError {
    MoeFileNotOpenedError(const std::string& filename, const char *file, const char *func, int line)
            : MoeFileError(filename, file, func, line) {}
};

struct MoeFileNotFoundError : MoeFileError {
    MoeFileNotFoundError(const std::string& filename, const char *file, const char *func, int line)
            : MoeFileError(filename, file, func, line) {}
};

}