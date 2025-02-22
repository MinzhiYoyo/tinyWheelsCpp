#ifndef SQLCONNECTIONPOLL_IMPL_H
#define SQLCONNECTIONPOLL_IMPL_H

#include "sqlConnectionPoll.def.h"
#include <mutex>

namespace tinyWheels {

    template<class SQL>
    SqlConnectionPoll<SQL>::SQLConn::~SQLConn() {
        poll_->FreeConn(sql_);
    }

    template<class SQL>
    SqlConnectionPoll<SQL>::SqlConnectionPoll(Adder&& adder, const size_type conn_size) {
        adder_ = adder;
        add_connection(conn_size);
    }

    template<class SQL>
    void SqlConnectionPoll<SQL>::add_connection(size_type conn_size) {
        std::unique_lock ul(poll_mutex_);
        size_type err_count = 0;

        while (conn_size-- > 0) {
            if (const auto sql = new SQL(adder_)) {
                poll_.push(sql);
                poll_vector_.push_back(sql);
            }
            else {
                ++err_count;
            }
        }
        sem_init(&sem_, 0, poll_vector_.size());
    }

    template<class SQL>
    typename SqlConnectionPoll<SQL>::SQLConn SqlConnectionPoll<SQL>::GetConn() {
        SQLConn sql_(this);
        sem_wait(&sem_);
        std::unique_lock ul(poll_mutex_);
        if (not poll_.empty()) {
            auto sql = poll_.front();
            poll_.pop();
            sql_.sql_ = sql;
        }
        return sql_;
    }

    template<class SQL>
    void SqlConnectionPoll<SQL>::FreeConn(SQL_POINT sql_point) {
        if (sql_point) {
            std::unique_lock ul(poll_mutex_);
            poll_.push(sql_point);
            sem_post(&sem_);
        }
    }

    template<class SQL>
    void SqlConnectionPoll<SQL>::ClosePoll() {
        std::unique_lock ul(poll_mutex_);
        for (auto& sql: poll_vector_) {
            delete sql;
        }
        poll_vector_.clear();
        while (not poll_.empty()) poll_.pop();
        sem_destroy(&sem_);
    }

    template<class SQL>
    SqlConnectionPoll<SQL>::~SqlConnectionPoll() {
        ClosePoll();
    }
}

#endif //SQLCONNECTIONPOLL_IMPL_H
