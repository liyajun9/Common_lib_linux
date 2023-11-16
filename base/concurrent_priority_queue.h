#ifndef __UTIL_QUEUE_CONCURRENT_PRIORITY_QUEUE_H__
#define __UTIL_QUEUE_CONCURRENT_PRIORITY_QUEUE_H__

#include "util/sync/mutex.h"
#include "util/sync/cond_var.h"
UL_NS_DEF

template<typename T, typename Container=std::vector<T>, 
         typename Compare=std::less<typename Container::value_type> >
class ConcurrentPriorityQueue {
 public:
  ConcurrentPriorityQueue() : cv_(&mutex_) {}
  virtual ~ConcurrentPriorityQueue() {}

  virtual void Push(const T& data) {
    MutexLock ml(&mutex_);
    queue_.push(data);
    cv_.Signal();
  }

  virtual void Pop(T& data) {
    bool suc = false;
    while (!suc) {
      {
        MutexLock ml(&mutex_);
        if (queue_.empty()) cv_.Wait();
        if (!queue_.empty()) {
          data = queue_.top();
          queue_.pop();
          suc = true;
        }
      }
    }
  }

  virtual bool TryPop(T& data){
    MutexLock ml(&mutex_);
    if (!queue_.empty()) {
      data = queue_.top();
      queue_.pop();
      return true;
    }
    return false;
  }

  T const& Top() const {
    MutexLock ml(&mutex_);
    return queue_.top();
  }

  int Size() const {
    MutexLock ml(&mutex_);
    return queue_.size();
  }
  
  int Empty() const {
    MutexLock ml(&mutex_);
    return queue_.empty();
  }

 protected:
  std::priority_queue<T, Container, Compare> queue_;
  Mutex mutex_;
  CondVar cv_;

 private:
  DISALLOW_COPY(ConcurrentPriorityQueue);
};

template<typename T>
class SmallNConcurrentPriorityQueue
    : public ConcurrentPriorityQueue<T, std::vector<T>, std::less<T> > {
  public:
   SmallNConcurrentPriorityQueue(int n=256): capacity_(n) {}
   virtual ~SmallNConcurrentPriorityQueue() {}

  virtual void Push(const T& data) {
    MutexLock ml(&this->mutex_);
    if ((int)this->queue_.size() < capacity_) {
       this->queue_.push(data);
       this->cv_.Signal();
    } else {
      if (data < this->queue_.top()) {
        this->queue_.pop();
        this->queue_.push(data);
      }
    }
  }
  
  bool Full() const {
    MutexLock ml(&this->mutex_);
    return this->queue_.size() >= capacity_;
  }
  
  int Capacity() const { return capacity_; }

 private:
  const int capacity_;
  DISALLOW_COPY(SmallNConcurrentPriorityQueue);
};

template<typename T>
class BigNConcurrentPriorityQueue 
    : public ConcurrentPriorityQueue<T, std::vector<T>, std::greater<T> > {
 public:
  BigNConcurrentPriorityQueue(int n=256): capacity_(n) {}
  virtual ~BigNConcurrentPriorityQueue() {}
  
  virtual void Push(const T& data) {
    MutexLock ml(&this->mutex_);
    if ((int)this->queue_.size() < capacity_) {
      this->queue_.push(data);
      this->cv_.Signal();
    } else {
      if (data > this->queue_.top()) {
        this->queue_.pop();
        this->queue_.push(data);
      }
    }
  }

  bool Full() const {
    MutexLock ml(&this->mutex_);
    return this->queue_.size() >= capacity_; 
  }
 
  int Capacity() const { return capacity_; }
 
 private:
  const int capacity_;

  DISALLOW_COPY(BigNConcurrentPriorityQueue);
};

UL_NS_END
#endif
