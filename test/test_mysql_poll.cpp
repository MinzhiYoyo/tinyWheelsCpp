#include "sqlConnectionPoll.h"
#include "MySQL.h"
#include <iostream>
#include <cstdlib>




int main() {
    std::cout << "test mysql poll" << std::endl;

    tinyWheels::SqlConnectionPoll<tinyWheels::MySQL> poll(
        tinyWheels::MySQL::Adder{
        .host = "127.0.0.1",
        .port = 3306,
        .user = "yutanwsl", //mysql_user,
        .password = "@zhizhi1130%lmz",
        .dbName = "webservercpp1"
        }, 10);

    auto plc = poll.GetConn();
//    // if (mysql_query(*plc, "select * from user;") != 0) {
//    auto ret = mysql_query(*plc, "select * from user;");
//    std::cout << ret << std::endl;
//    const auto res = mysql_store_result(*plc);
//    std::cout << res << std::endl;
//    const auto num_fields = mysql_num_fields(res);
//    const auto fields = mysql_fetch_fields(res);
//    while (const auto row = mysql_fetch_row(res)) {
//        for (unsigned int i = 0; i < num_fields; i++) {
//            std::cout << fields[i].name << ": " << row[i] << std::endl;
//        }
//    }
    // }else {
    //     // 打印错误消息
    //     std::cout << "mysql_query error: " << mysql_error(*plc) << std::endl;
    // }

    return 0;
}
