//
// Created by 24983 on 25-2-18.
//

#ifndef MYSQL_H
#define MYSQL_H

#include <mysql/mysql.h>

namespace tinyWheels {
    class MySQL {
    public:
        using SQL_HANDLE = MYSQL*;
        struct Adder {
            const char* host{nullptr};
            unsigned int port{0};
            const char* user{nullptr};
            const char* password{nullptr};
            const char* dbName{nullptr};
        };
    public:
        MySQL() = default;
        explicit MySQL(const Adder& adder);
        void connect(const Adder& adder);
        void close() const;
        ~MySQL();
        explicit operator bool() const;

        MySQL(const MySQL& sql);
        MySQL& operator=(const MySQL& sql);
        MySQL(MySQL&& sql) noexcept ;
        MySQL& operator=(MySQL&& sql) noexcept;

        MYSQL *operator*() const;
    private:
        MYSQL *sql_{nullptr};
    };
}

#endif //MYSQL_H
