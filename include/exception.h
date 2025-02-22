//
// Created by 24983 on 25-2-22.
//

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace tinyWheels{
    class exception final : public std::exception {
        char *msg;
    public:
        explicit __attribute__((format(printf, 2, 3)))
        exception(const char *fmt, ...) {
            va_list args;
            va_start(args, fmt);
            const int len = vsnprintf(nullptr, 0, fmt, args);
            va_end(args);
            msg = static_cast<char *>(malloc(len+2));
            va_start(args, fmt);
            vsnprintf(msg, len+1, fmt, args);
            va_end(args);
        }
        ~exception() override {
            delete[] msg;
        };
        [[nodiscard]] const char *what() const noexcept override { return msg; }
    };
}

#endif //EXCEPTION_H
