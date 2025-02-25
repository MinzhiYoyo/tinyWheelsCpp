//
// Created by 24983 on 25-2-17.
//

#ifndef SQLCONNECTIONPOLL_DEF_H
#define SQLCONNECTIONPOLL_DEF_H

#include <semaphore>
#include <queue>

namespace tinyWheels {

    template<class SQL>
    class SqlConnectionPoll {
    public:
        using SQL_POINT = SQL*;
        using size_type = size_t;
        using Adder = typename SQL::Adder;
        using SQL_HANDLE = typename SQL::SQL_HANDLE;
        struct SQLConn {
            friend class SqlConnectionPoll;
        private:
            SQL_POINT sql_{nullptr};
            SqlConnectionPoll *poll_;
            explicit SQLConn(SqlConnectionPoll *poll): poll_(poll){}
        public:
            SQL_POINT operator->() const { return sql_; }
            SQL& operator*() const { return *sql_; }
            ~SQLConn() ;
            SQLConn(const SQLConn&) = default;
            SQLConn& operator=(const SQLConn&) = default;
            SQLConn(SQLConn&&) = default;
            SQLConn& operator=(SQLConn&& s) = default;

            explicit operator bool() const {return *sql_;}
        };
    private:
        void add_connection(size_type conn_size);
    public:
        explicit SqlConnectionPoll(Adder&& adder, size_type conn_size = 0);
        ~SqlConnectionPoll();

        SQLConn GetConn();
        void FreeConn(SQL_POINT);
        void ClosePoll();

        SqlConnectionPoll(const SqlConnectionPoll&) = delete;
        SqlConnectionPoll& operator=(const SqlConnectionPoll&) = delete;
        SqlConnectionPoll(const SqlConnectionPoll&&) = delete;
        SqlConnectionPoll& operator=(const SqlConnectionPoll&&) = delete;
    private:
        std::pmr::vector<SQL_POINT> poll_vector_;  // 原始指针
        std::queue<SQL_POINT> poll_;
        Adder adder_;
        std::mutex poll_mutex_;
        sem_t sem_{};
    };
}

#endif //SQLCONNECTIONPOLL_DEF_H
