#ifndef THREADPOLL_H
#define THREADPOLL_H
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <shared_mutex>

namespace tinyWheels {
    class ThreadPoll {
    public:
        using size_type = size_t;
        // 定义任务队列
        struct Tasks {
        public:
            std::queue<std::function<void()>> norm_tasks_;  // 普通任务
            std::mutex norm_mutex_;
            // std::recursive_mutex norm_mutex_;
            std::condition_variable norm_cv_;
            // std::condition_variable_any norm_cv_;
            [[nodiscard]] bool norm_tasks_empty() const {
                return norm_tasks_size() == 0;
            }
            [[nodiscard]] size_type norm_tasks_size() const {
                return norm_tasks_.size();
            }
            [[nodiscard]] std::function<void()> get_norm_task() {
                std::function<void()> task = std::move(norm_tasks_.front());
                norm_tasks_.pop();
                return task;
            }
        };
    private:
        static void thread_function(
            std::shared_ptr<Tasks> tasks,
            std::shared_ptr<std::atomic<bool>> m_stop,
            std::shared_ptr<std::atomic<size_type>> m_free_threads,
            std::shared_ptr<std::atomic<bool>> m_wait_task_end
            );
    public:
        ThreadPoll();
        ThreadPoll(const ThreadPoll& p) = delete;
        ThreadPoll(ThreadPoll&& p) = delete;
        ThreadPoll& operator=(const ThreadPoll& p) = delete;
        ThreadPoll& operator=(ThreadPoll&& p) = delete;

        explicit ThreadPoll(size_type threadNumber);
        explicit ThreadPoll(bool wait_task_end);
        ThreadPoll(size_type threadNumber, bool wait_task_end);
        [[nodiscard]] size_type get_free_threads() const;
        [[nodiscard]] size_type get_max_threads() const;
        [[nodiscard]] inline bool is_stop() const;
        void stop(bool wait);
        ~ThreadPoll();
        void add_task(std::function<void()>&& task) const;
        void add_threads(size_type threads_number); // 增加线程并执行
        void set_wait_task(bool wait);
    private:
        std::vector<std::thread> m_threads; // 线程池

        // 智能指针的作用在于，每个线程都会对这个任务队列进行操作，并且主线程退出之后，这个智能指针是不可以进行释放的，必须等所有的线程退出才行
        std::shared_ptr<Tasks> m_tasks = std::make_shared<Tasks>();
        std::shared_ptr<std::atomic<bool>> m_stop_flag = std::make_shared<std::atomic<bool>>(false);
        std::shared_ptr<std::atomic<size_type>> m_free_threads = std::make_shared<std::atomic<size_type>>(0);
        std::shared_ptr<std::atomic<bool>> m_wait_task_end = std::make_shared<std::atomic<bool>>(false);
    };
}
#endif //THREADPOLL_H
