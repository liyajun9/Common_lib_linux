#ifndef PUBLIC_BASE_TASK_H_
#define PUBLIC_BASE_TASK_H_
#include "base/basictypes.h"

namespace base {

class Task {
 public:
  Task() {}
  virtual ~Task() {}
  virtual void Run() = 0;
  DISALLOW_COPY(Task);
};

template <bool hasReturn>
class TaskImpl : public Task {
 public:
  virtual void Run() {}
};

template <>
class TaskImpl<true> : public Task {
 public:
  virtual void Run() { base_->Run(); }
  DISALLOW_COPY(TaskImpl);

  template <typename ReturnT, typename CallbackT, typename... ArgsT>
  explicit TaskImpl(ReturnT *ret, CallbackT&& f, ArgsT&& ... args) {
    base_ = MakeRoutine(ret, std::bind(std::forward<CallbackT>(f),
                                       std::forward<ArgsT>(args)...));
  }

 private:
  class ImplBase {
   public:
    ImplBase() {}
    DISALLOW_COPY(ImplBase);
    virtual ~ImplBase() {}
    virtual void Run() = 0;
  };

  std::shared_ptr<ImplBase> base_;

  template <typename ReturnT, typename CallbackT>
  class Impl : public ImplBase {
   public:
    Impl(ReturnT *ret, CallbackT&& f)
      : ret_(ret), func_(std::forward<CallbackT>(f)) {}
    DISALLOW_COPY(Impl);
    virtual ~Impl() {}
    virtual void Run() { *ret_ = func_(); }
   private:
    ReturnT   *ret_;
    CallbackT func_;
  };

  template <typename ReturnT, typename CallbackT>
  std::shared_ptr<Impl<ReturnT, CallbackT>>
  MakeRoutine(ReturnT *ret, CallbackT&& f) {
    return std::make_shared<Impl<ReturnT, CallbackT>>(ret,
                std::forward<CallbackT>(f));
  }
};

template <>
class TaskImpl<false> : public Task {
 public:
  virtual void Run() { base_->Run(); }
  DISALLOW_COPY(TaskImpl);

  template <typename CallbackT, typename... ArgsT>
  explicit TaskImpl(CallbackT&& f, ArgsT&& ... args) {
    base_ = MakeRoutine(std::bind(std::forward<CallbackT>(f),
                                  std::forward<ArgsT>(args)...));
  }

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

} // namespace base
#endif // PUBLIC_BASE_TASK_H_
