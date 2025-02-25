#include <iostream>

#include "ThreadPoll.h"
#include <sys/select.h>

std::mutex cout_mutex;

void task_function(int times) {
    while (times-- > 0) {
        // 睡眠 1s钟
        struct timeval tv{
            .tv_sec = 0,
            .tv_usec = 100000
        };
        select(0, nullptr, nullptr, nullptr, &tv);
        {
            std::unique_lock ul(cout_mutex);
            const auto my_id = std::this_thread::get_id();
            std::cout << my_id << ": " << times << std::endl;
        }
    }
}

int main() {
    tinyWheels::ThreadPoll threadPoll(8, true);
    for (int i = 0; i < 20; ++i) {
        auto task = std::bind(task_function, i);
        threadPoll.add_task(std::move(task));

        {
            std::unique_lock ul(cout_mutex);
            const auto my_id = std::this_thread::get_id();
            std::cout <<  my_id << ", add task: " << i << std::endl;
        }
    }
    {
        timeval tv{
            .tv_sec = 1,
            .tv_usec = 0
        };
        select(0, nullptr, nullptr, nullptr, &tv);
        std::unique_lock ul(cout_mutex);
        std::cout << "准备结束任务！！！" << std::endl;
    }
    threadPoll.stop(true);
    {
        std::unique_lock ul(cout_mutex);
        std::cout << "结束任务！！！" << std::endl;
    }
    return 0;
}
