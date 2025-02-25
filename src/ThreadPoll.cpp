// ReSharper disable CppPassValueParameterByConstReference
#include "../include/ThreadPoll.h"

#include <iomanip>

namespace tinyWheels {
    // 线程函数，是线程实际运行的函数
    // 主线程退出，该线程不一定退出，因此，需要使用智能指针管理任务
    void ThreadPoll::thread_function(
        std::shared_ptr<Tasks> tasks,
        std::shared_ptr<std::atomic<bool>> m_stop,
        std::shared_ptr<std::atomic<size_type>> m_free_threads,
        std::shared_ptr<std::atomic<bool>> m_wait_task_end
    ) {
        // 增加一个空闲任务线程
        m_free_threads->fetch_add(1, std::memory_order_release);
        std::unique_lock norm_lock(tasks->norm_mutex_);

        auto run_flag = [m_stop, m_wait_task_end, tasks] {
            // 整体还没停止，不能停止线程池
            const bool flag_stop = not m_stop->load(std::memory_order_acquire);
            // 还需要等待任务结束，不能停止线程池
            const bool flag_wait = m_wait_task_end->load(std::memory_order_acquire) and not tasks->norm_tasks_empty();
            return flag_stop or flag_wait;
        };

        while (run_flag()) {
            if (tasks->norm_tasks_empty()) {
                tasks->norm_cv_.wait(norm_lock);
            }else {
                auto task = tasks->get_norm_task();
                norm_lock.unlock();  // 执行任务应该主动解锁，否则会卡住

                m_free_threads->fetch_sub(1, std::memory_order_release);
                if (task) {
                    task();
                }
                m_free_threads->fetch_add(1, std::memory_order_release);

                norm_lock.lock();
            }
        }
    }

    ThreadPoll::size_type ThreadPoll::get_free_threads() const {
        return m_free_threads->load(std::memory_order_acquire);
    }

    ThreadPoll::size_type ThreadPoll::get_max_threads() const {
        return m_threads.size();
    }

    bool ThreadPoll::is_stop() const {
        return m_stop_flag->load(std::memory_order_acquire);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void ThreadPoll::stop(const bool wait) {
        m_stop_flag->store(true, std::memory_order_release);
        auto wait1 = [&] {  // 等待任务队列为空
            const auto w = this->m_wait_task_end->load(std::memory_order_acquire);
            // std::unique_lock ul(this->m_tasks->norm_mutex_);
            const auto e = this->m_tasks->norm_tasks_empty();
            return w and not e and wait;
        };
        while (wait1()){}
        m_tasks->norm_cv_.notify_all();  // 通知所有执行普通任务的线程都结束


        for (auto& t : m_threads) {
            if (t.joinable()) {
                if (wait) t.join();
                else t.detach();
            }
        }
    }

    void ThreadPoll::add_task(std::function<void()>&& task) const {
        std::unique_lock ul(m_tasks->norm_mutex_);
        m_tasks->norm_tasks_.emplace(std::forward<std::function<void()>>(task));
        m_tasks->norm_cv_.notify_one();
    }

    void ThreadPoll::add_threads(const size_type threads_number) {
        m_threads.reserve(get_max_threads() + threads_number);
        for (auto i = 0; i < threads_number; ++i) {
            m_threads.emplace_back(&ThreadPoll::thread_function, m_tasks, m_stop_flag, m_free_threads, m_wait_task_end);
        }
    }

    ThreadPoll::ThreadPoll(const size_type threadNumber) {
        add_threads(threadNumber);
    }

    ThreadPoll::ThreadPoll(const size_type threadNumber, const bool wait_task_end) {
        set_wait_task(wait_task_end);
        add_threads(threadNumber);
    }
    ThreadPoll::ThreadPoll() {
        // 获取CPU的线程数量
        const auto threadNumber = std::thread::hardware_concurrency();
        add_threads(threadNumber);
    }
    ThreadPoll::ThreadPoll(const bool wait_task_end) {
        set_wait_task(wait_task_end);
        const auto threadNumber = std::thread::hardware_concurrency();
        add_threads(threadNumber);
    }


    // ReSharper disable once CppMemberFunctionMayBeConst
    void ThreadPoll::set_wait_task(const bool wait) {
        m_wait_task_end->store(wait, std::memory_order_release);
    }

    ThreadPoll::~ThreadPoll() {
        stop(false);  // 不需要等待其他线程结束，等他们自己结束
    }
}
