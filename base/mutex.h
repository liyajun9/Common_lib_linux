// Description : A simple mutex wrapper, supporting locks and read-write locks.
// NOTE        : modifiy from google chromium source code

#ifndef PUBLIC_BASE_MUTEX_H_
#define PUBLIC_BASE_MUTEX_H_

#include <sys/time.h>

#include "public/base/basictypes.h"

namespace base {

class Mutex {
 public:
  // Create a Mutex that is not held by anybody
  inline Mutex();
  inline ~Mutex();

  inline void Lock();
  inline void Unlock();
  inline bool TryLock();

 private:
  pthread_mutex_t mutex_;
  friend class CondVar;

  Mutex(Mutex* /*ignored*/) {}
  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

class RwMutex {
 public:
  // Create a RwMutex that is not held by anybody
  inline RwMutex();
  inline ~RwMutex();

  inline void ReaderLock();   // Block until free or shared then acquire a share
  inline void ReaderUnlock(); // Release a read share of this Mutex
  inline void WriterLock()   { Lock(); }   // Acquire an exclusive lock
  inline void WriterUnlock() { Unlock(); } // Release a lock from writer lock

 private:
  inline void Lock();
  inline void Unlock();
  inline bool TryLock();

  pthread_rwlock_t mutex_;

  RwMutex(RwMutex* /*ignored*/) {}
  DISALLOW_COPY_AND_ASSIGN(RwMutex);
};

RwMutex::RwMutex()           { BCHECK(pthread_rwlock_init(&mutex_, NULL) == 0) }
RwMutex::~RwMutex()          { BCHECK(pthread_rwlock_destroy(&mutex_) == 0)    }
void RwMutex::Lock()         { BCHECK(pthread_rwlock_wrlock(&mutex_) == 0)     }
void RwMutex::Unlock()       { BCHECK(pthread_rwlock_unlock(&mutex_) == 0)     }
bool RwMutex::TryLock()      { return pthread_rwlock_trywrlock(&mutex_) == 0;  }
void RwMutex::ReaderLock()   { BCHECK(pthread_rwlock_rdlock(&mutex_) == 0)     }
void RwMutex::ReaderUnlock() { BCHECK(pthread_rwlock_unlock(&mutex_) == 0)     }

Mutex::Mutex()               { BCHECK(pthread_mutex_init(&mutex_, NULL) == 0)  }
Mutex::~Mutex()              { BCHECK(pthread_mutex_destroy(&mutex_) == 0)     }
void Mutex::Lock()           { BCHECK(pthread_mutex_lock(&mutex_) == 0)        }
void Mutex::Unlock()         { BCHECK(pthread_mutex_unlock(&mutex_) == 0)      }
bool Mutex::TryLock()        { return pthread_mutex_trylock(&mutex_) == 0;     }

// ----------------------------------------------------------------------------
// Some helper classes
// MutexLock(mu) acquires mu when constructed and releases it whern destroyed.

class MutexLock {
 public:
  explicit MutexLock(Mutex *mu) : mu_(mu) { mu_->Lock(); }
  ~MutexLock() { mu_->Unlock(); }

 private:
  Mutex * const mu_;
  DISALLOW_COPY_AND_ASSIGN(MutexLock);
};

// ReaderMutexLock and WriterMutexLock do the same, for rwlocks
class ReaderMutexLock {
 public:
  explicit ReaderMutexLock(RwMutex *mu) : mu_(mu) { mu_->ReaderLock(); }
  ~ReaderMutexLock() { mu_->ReaderUnlock(); }

 private:
  RwMutex * const mu_;
  DISALLOW_COPY_AND_ASSIGN(ReaderMutexLock);
};

class WriterMutexLock {
 public:
  explicit WriterMutexLock(RwMutex *mu) : mu_(mu) { mu_->WriterLock(); }
  ~WriterMutexLock() { mu_->WriterUnlock(); }

 private:
  RwMutex * const mu_;
  DISALLOW_COPY_AND_ASSIGN(WriterMutexLock);
};

// forbidden ‘MutexLock mu = MutexLock(x)’
//#define MutexLock(x)       static_assert(0, "mutex_lock_decl_missing_var_name");
#define ReaderMutexLock(x) static_assert(0, "rmutex_lock_decl_missing_var_name");
#define WriterMutexLock(x) static_assert(0, "wmutex_lock_decl_missing_var_name");

// Provide safe way to declare and use global, linker-initialized mutex. Sigh.
#define GLOBAL_MUTEX(name) \
        static pthread_mutex_t (name) = PTHREAD_MUTEX_INITIALIZER
#define GLOBAL_MUTEX_LOCK(name)    pthread_mutex_lock(&(name))
#define GLOBAL_MUTEX_UNLOCK(name)  pthread_mutex_unlock(&(name))

class CondVar {
 public:
  explicit CondVar(Mutex *mu) : mu_(&mu->mutex_)
                         { BCHECK(pthread_cond_init(&cv_, NULL) == 0) }
  inline ~CondVar()      { BCHECK(pthread_cond_destroy(&cv_) == 0) }

  DISALLOW_COPY_AND_ASSIGN(CondVar);

  inline void Wait()     { BCHECK(pthread_cond_wait(&cv_, mu_) == 0) }
  inline void Signal()   { BCHECK(pthread_cond_signal(&cv_) == 0) }
  inline void SignalAll(){ BCHECK(pthread_cond_broadcast(&cv_) == 0) }

  inline void WaitWithTimeout(int ms) {
    struct timeval now;
    struct timespec timeout;
    gettimeofday(&now, NULL);
    uint64_t to = (uint64_t)now.tv_sec  * 1000000000LL +
                  (uint64_t)now.tv_usec * 1000LL +
                  (uint64_t)ms          * 1000000LL;
    timeout.tv_sec  = to / 1000000000LL;
    timeout.tv_nsec = to % 1000000000LL;

    BCHECK(pthread_cond_timedwait(&cv_, mu_, &timeout) == 0)
  }

 private:
  pthread_cond_t cv_;
  pthread_mutex_t* mu_;
};

} // namespace base

#endif // PUBLIC_BASE_MUTEX_H_
