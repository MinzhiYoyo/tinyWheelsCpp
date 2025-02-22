#include "MySQL.h"

#include <utility>

namespace tinyWheels {
    MySQL::MySQL(const Adder& adder) {
        connect(adder);
    }
    MySQL::MySQL(MySQL&& sql) noexcept {
        *this = std::move(sql);
    }
    MySQL::MySQL(const MySQL &sql) {
        sql_ = sql.sql_;
    }
    MySQL& MySQL::operator=(const MySQL &sql) {
        if(this != &sql) {
            sql_ = sql.sql_;
        }
        return *this;
    }
    MySQL& MySQL::operator=(MySQL &&sql) noexcept {
        if(this != &sql) {
            this->~MySQL();  // 先关闭自己，再移动
            *this = std::move(sql);
        }
        return *this;
    }



    void MySQL::connect(const Adder& adder) {
        sql_ = mysql_init(sql_);
        sql_ = mysql_real_connect(sql_, adder.host,
                                         adder.user, adder.password,
                                         adder.dbName, adder.port, nullptr, 0);
    }
    void MySQL::close() const {
        if (*this) {
            mysql_close(sql_);
        }
    }

    MySQL::operator bool() const {
        return sql_ != nullptr;
    }

    MYSQL *MySQL::operator*() const {
        return sql_;
    }

    MySQL::~MySQL() {
        close();
    }
}
