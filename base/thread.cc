#include "public/base/thread.h"

namespace base {

bool Thread::SetJoinable(bool joinable) {
  if (joinable == joinable_) return true;
  if (started_) return false;
  joinable_ = joinable;
  return true;
}

void Thread::Start() {
  pthread_attr_t attr;
  BCHECK(pthread_attr_init(&attr) == 0);
  if (joinable_) {
    BCHECK(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) == 0)
  }else {
    BCHECK(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) == 0)
  }
  BCHECK(pthread_create(&tid_, &attr, ThreadRunner, this) == 0);
  BCHECK(pthread_attr_destroy(&attr) == 0);

  started_ = true;
}

void Thread::Join() {
  BCHECK(joinable_)
  BCHECK(pthread_join(tid_, NULL) == 0)
}

void* Thread::ThreadRunner(void *arg) {
  Thread* t = reinterpret_cast<Thread*>(arg);
  t->Run();
  return NULL;
}


void Thread::Stop() {
  stop_requested_ = true;
}

} // namespace base
