#ifndef PUBLIC_BASE_THREAD_H_
#define PUBLIC_BASE_THREAD_H_
#include "base/basictypes.h"

// Usage:
// class MyThread : public base::Thread {
//  public:
//   MyThread() { }
//  protected:
//   virtual void Run();
//
// };

// MyThread mythread;
// mythread.Start();

namespace base {

class Thread {
 public:
  Thread(){}
  Thread(bool joinable = false) :
        started_(false), joinable_(joinable), stop_requested_(false) {}
  virtual ~Thread() {}
  pthread_t Tid() const { return tid_; }
  bool SetJoinable(bool joinable);
  void Start();
  void Join();
  void Stop();

 protected:
  virtual void Run() = 0;
  static void* ThreadRunner(void *arg);

  pthread_t tid_;
  bool started_;
  bool joinable_;
  bool stop_requested_;

  DISALLOW_COPY_AND_ASSIGN(Thread);
};

} // namespace base
#endif // PUBLIC_BASE_THREAD_H_
