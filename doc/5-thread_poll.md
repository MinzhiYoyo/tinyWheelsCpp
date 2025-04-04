# 线程池

池化技术是一个空间换时间的做法，比如内存池、线程池或者数据库连接池。都是预先建立许多资源，然后在需要的时候直接使用，而不是每次都去创建和销毁资源。

# 功能

一般线程池需要实现：创建若干个线程，线程自动获取任务列表并执行，线程安全退出（等待子线程退出和等待任务队列执行完毕）等功能，下面是一个例子，给出声明。具体实现在后面

```cpp
class ThreadPoll {
    public:
    using size_type = size_t;

    struct Tasks {  // 定义任务队列，可以选择设置两条队列，一条是普通任务队列，一条是优先任务队列，根据优先级执行的任务队列
        public:
        std::queue<std::function<void()>> norm_tasks_;  // 普通任务
        std::mutex norm_mutex_;  // 普通任务队列互斥锁
        std::condition_variable norm_cv_;  // 普通任务队列条件变量
        [[nodiscard]] std::function<void()> get_norm_task() {  // 这个是获取任务队列中的一个任务
            std::function<void()> task = std::move(norm_tasks_.front());
            norm_tasks_.pop();
            return task;
        }
    };
    private:
    static void thread_function(  // 每个线程的主函数，称之为线程函数
        std::shared_ptr<Tasks> tasks,
        std::shared_ptr<std::atomic<bool>> m_stop,
        std::shared_ptr<std::atomic<size_type>> m_free_threads,
        std::shared_ptr<std::atomic<bool>> m_wait_task_end
    );
    public:  // 默认构造函数和赋值函数，基本上线程池也不会去动的，除非需要转移线程池所有权但不影响里面的线程运行
    explicit ThreadPoll(size_type threadNumber);
    ThreadPoll(size_type threadNumber, bool wait_task_end);
    [[nodiscard]] size_type get_free_threads() const;  // 获取空闲线程数
    [[nodiscard]] size_type get_max_threads() const;   // 获取最大线程数
    [[nodiscard]] inline bool is_stop() const;  // 主线程是否停止
    void stop(bool wait);  // 停止主线程，选择是否等待子线程结束（join or detach）
    ~ThreadPoll();
    void add_task(std::function<void()>&& task) const;
    void add_threads(size_type threads_number); // 增加线程并执行
    void set_wait_task(bool wait); 
    private:
    std::vector<std::thread> m_threads; // 线程池

    // 智能指针的作用在于，每个线程都会对这个任务队列进行操作
    // 并且主线程退出之后，子线程不一定退出，必须等所有的线程退出才能释放资源
    std::shared_ptr<Tasks> m_tasks = std::make_shared<Tasks>();
    std::shared_ptr<std::atomic<bool>> m_stop_flag = std::make_shared<std::atomic<bool>>(false);
    std::shared_ptr<std::atomic<size_type>> m_free_threads = std::make_shared<std::atomic<size_type>>(0);
    std::shared_ptr<std::atomic<bool>> m_wait_task_end = std::make_shared<std::atomic<bool>>(false);
};


```

1.   `m_wait_task_end`：是否等待所有任务结束再退出线程池，有可能主线程结束了，但是任务队列还没完全结束，所以可以设置是否等待
2.   `m_free_threads`：当前空闲的线程数量
3.   `m_stop_flag`：主线程退出标志
4.   `m_tasks`：任务队列，是一个`Tasks`结构体

# 实现

## 智能指针

使用智能指针的优点在于：主线程结束或者线程池销毁了，子线程不一定会结束。任务队列的变量又是存储在线程池对象里面，线程池如果销毁了，任务队列也会销毁，但是子线程还在运行，它们仍然需要访问队列，因此使用智能指针的方式可以保证子线程需要的变量能够等到所有子线程退出后再销毁。

还有一点需要注意：子线程的主函数必须传智能指针值，不能传引用，如果传引用的话，并不保证智能指针的引用次数。

## 任务队列

任务队列的逻辑应该是：主线程添加任务，子线程自己从任务队列中获取任务，这种方式最简单，而不是主线程获取任务了分配给子线程。

任务队列里的任务返回值应为`void`，这是由于在子线程中只负责执行任务，并不会把任务返回给谁。如果需要进行`I/O`读写，应该直接由子线程进行读写或者在外层传入任务的时候做好读写变量的生命周期。

任务队列里的任务可以选择使用`bind`函数来封装成函数包装器。

## 内存序

这里涉及到许多的原子操作，但是原子操作的默认内存序太严格了，所以可以使用适当放松的内存序。

```cpp
anything->load(std::memory_order_acquire); // 任何读操作都是获取内存序
anything->store(value, std::memory_order_release); // 任何写操作都是释放序
anything->fetch_add(1, std::memory_order_release); // 任何写操作都是释放序
anything->fetch_sub(1, std::memory_order_release); // 任何写操作都是释放序
```

## 子线程主函数

```cpp
void ThreadPoll::thread_function(
    // 这里传入的是智能指针值而不是 const shared_ptr&
    // 传引用的话，相当于所有权只有主线程一个线程拥有
    // 传值的话，子线程才会拥有共同所有权，智能指针才不会随着主线程退出而销毁
    std::shared_ptr<Tasks> tasks,
    std::shared_ptr<std::atomic<bool>> m_stop,
    std::shared_ptr<std::atomic<size_type>> m_free_threads,
    std::shared_ptr<std::atomic<bool>> m_wait_task_end
) {
    // 增加一个空闲任务线程，用于记录空闲任务的数量
    m_free_threads->fetch_add(1, std::memory_order_release);
    
    // 任务队列锁
    std::unique_lock norm_lock(tasks->norm_mutex_);
	
    // 计算是否保持子线程主函数循环
    auto run_flag = [m_stop, m_wait_task_end, tasks] {
        // 整体还没停止，不能停止线程池
        const bool flag_stop = not m_stop->load(std::memory_order_acquire);
        // 还需要等待任务结束，不能停止线程池
        const bool flag_wait = m_wait_task_end->load(std::memory_order_acquire) and not tasks->norm_tasks_empty();
        return flag_stop or flag_wait;
    };
	
    while (run_flag()) {
        // 如果任务队列为空才需要等待
        if (tasks->norm_tasks_empty()) {
            tasks->norm_cv_.wait(norm_lock);
        }else {  // 如果任务队列不为空，那就需要一直执行
            auto task = tasks->get_norm_task();
            norm_lock.unlock();  // 执行任务应该主动解锁，否则相当于单线程了

            m_free_threads->fetch_sub(1, std::memory_order_release);
            if (task) {
                task();
            }
            m_free_threads->fetch_add(1, std::memory_order_release);

            norm_lock.lock();
        }
    }
}
```

## 增加任务与获取任务

```cpp
struct Tasks{
    [[nodiscard]] std::function<void()> get_norm_task() {  // 这个是获取任务队列中的一个任务
            std::function<void()> task = std::move(norm_tasks_.front());
            norm_tasks_.pop();
            return task;
    }
}

void ThreadPoll::add_task(std::function<void()>&& task) const {
    // 增加任务需要用锁，只要涉及到任务队列的，都需要加锁
    std::unique_lock ul(m_tasks->norm_mutex_);
    m_tasks->norm_tasks_.emplace(std::forward<std::function<void()>>(task));
    m_tasks->norm_cv_.notify_one();
}
```

## 停止线程池

等待子线程退出和等待任务队列完毕是两个独立的功能

1.   等待子线程结束，子线程从当前任务结束就可以退出的，不需要执行完任务队列，（这个等待是主线程等待）。
2.   等待任务队列完毕，子线程必须把任务队列中的所有任务作为才能结束，（这个等待是子线程要等待）。

```cpp
void ThreadPoll::stop(const bool wait) {  // wait 是等待子线程，即 join 方法
    
    // 设置停止标志
    m_stop_flag->store(true, std::memory_order_release);
    auto wait1 = [this] {
        auto w = this->m_wait_task_end->load(std::memory_order_acquire);
        std::unique_lock ul(this->m_tasks->norm_mutex_);
        auto e = this->m_tasks->norm_tasks_empty();
        return w and not e;
    };
    while (wait1()) {}  // 可以选择加延时函数，或者再加上一个自由变量也行
    m_tasks->norm_cv_.notify_all();  // 通知所有执行普通任务的线程都结束
    for (auto& t : m_threads) {
        if (t.joinable()) {
            if (wait) t.join();
            else t.detach();
        }
    }
}
```

## 其他

```cpp
ThreadPoll::ThreadPoll() {
    // 获取CPU的线程数量，也有使用线程数量+1的
    const auto threadNumber = std::thread::hardware_concurrency();
    add_threads(threadNumber);
}
ThreadPoll::~ThreadPoll() {
    stop(false);  // 不需要等待其他线程结束，等他们自己结束
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
ThreadPoll::ThreadPoll(const size_type threadNumber) {
    add_threads(threadNumber);
}

ThreadPoll::ThreadPoll(const size_type threadNumber, const bool wait_task_end) {
    set_wait_task(wait_task_end);
    add_threads(threadNumber);
}
```
