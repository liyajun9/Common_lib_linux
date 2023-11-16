#ifndef PUBLIC_BASE_THREAD_POOL_H_
#define PUBLIC_BASE_THREAD_POOL_H_

#include "base/basic.h"
#include "base/task.h"
#include "base/concurrent_queue.h"

namespace base {

/*
class Task {
 public:
  void Run() { base_->Run(); }

  template <typename CallbackT, typename... ArgsT>
  Task(CallbackT&& f, ArgsT&& ... args) {
    base_ = MakeRoutine(
      std::bind(std::forward<CallbackT>(f), std::forward<ArgsT>(args)...));
  }

  DISALLOW_COPY(Task);
  virtual ~Task() {}

 private:
  class ImplBase {
   public:
    ImplBase() {}
    DISALLOW_COPY(ImplBase);
    virtual ~ImplBase() {}
    virtual void Run() = 0;
  };

  std::shared_ptr<ImplBase> base_;

  template <typename CallbackT>
  class Impl : public ImplBase {
   public:
    Impl(CallbackT&& f) : func_(std::forward<CallbackT>(f)) {}
    DISALLOW_COPY(Impl);
    virtual ~Impl() {}
    virtual void Run() { func_(); }
   private:
    CallbackT func_;
  };

  template <typename CallbackT>
  std::shared_ptr<Impl<CallbackT>>
  MakeRoutine(CallbackT&& f) {
    return std::make_shared<Impl<CallbackT>>(std::forward<CallbackT>(f));
  }

};
*/

class ThreadPool {
 public:
  ThreadPool(int worker_num=20)
    : worker_num_(worker_num), queue_(worker_num) {
    for (int i=0; i<worker_num_; ++i)
      workers.push_back(std::thread(Work, this));
  }

  ~ThreadPool() {
    for (int i=0; i<worker_num_; ++i)
      AddTask(nullptr);
    for (auto& th : workers) th.join();
  }

  void AddTask(std::shared_ptr<Task> task) {
    queue_.Push(task);
  }

 private:
  const int worker_num_;
  FixedSizeConcurrentQueue<std::shared_ptr<Task>> queue_;
  std::vector<std::thread> workers;

  static void Work(ThreadPool *pool) {
    std::shared_ptr<Task> task;
    while (true) {
      pool->queue_.Pop(task);
      if (task == nullptr) return;
      task->Run();
    }
  }

  DISALLOW_COPY(ThreadPool);
};

} // namespace base

#endif // PUBLIC_BASE_THREAD_POOL_H_
