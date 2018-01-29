/*
 * 如何实现一个共享指针，在共享指针中保存了共享的数目。
 * 构造函数初始化为1
 * 一旦有赋值构造，值则加1
 * 析构函数则将数值减1，当数值最后为0时析构。
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

template<class T>
class sharedPtr{
private:
  T* t_;
  long num_;

public:
  sharedPtr() : t_(NULL), num_(1) {};
  sharedPtr(T *t) : t_(t), num_(1) {};
  sharedPtr<T>(const sharedPtr<T>& ptr) : t_(ptr.t_) {
    num_ = ptr.num_ + 1;
  };
  sharedPtr& operator=(const sharedPtr ptr) {
    num_++;
  }

  T* get() {
    return t_;
  }

  T& operator*() {
    return *t_;
  }
  T* operator->() {
    return t_;
  }

  long use_count() {
    return num_;
  }


  ~sharedPtr() {
    num_--;
    if (0 == num_) {
      delete t_;
    }
  }
};

template<class T>
class makeShared{
public:
  makeShared() : ptr_(){}

private:
  sharedPtr<T> ptr_;
};

struct Base
{
  Base() { std::cout << "  Base::Base()\n";  }
  // Note: non-virtual destructor is OK here
  ~Base() { std::cout << "  Base::~Base()\n";  }
};

struct Derived: public Base
{
 Derived() { std::cout << "  Derived::Derived()\n";  }
 ~Derived() { std::cout << "  Derived::~Derived()\n";  }
};

void thr(std::shared_ptr<Base> p)
{
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::shared_ptr<Base> lp = p; // thread-safe, even though the
  // shared use_count is incremented
  {
    static std::mutex io_mutex;
    std::lock_guard<std::mutex> lk(io_mutex);
    std::cout << "local pointer in a thread:\n"
    << "  lp.get() = " << lp.get()
    << ", lp.use_count() = " << lp.use_count() << '\n';
  }
}

int main()
{
  sharedPtr<Base> p = makeShared<Derived>();

  std::cout << "Created a shared Derived (as a pointer to Base)\n"
            << "  p.get() = " << p.get()
            << ", p.use_count() = " << p.use_count() << '\n';
  std::thread t1(thr, p), t2(thr, p), t3(thr, p);
  //p.reset(); // release ownership from main
  std::cout << "Shared ownership between 3 threads and released\n"
            << "ownership from main:\n"
            << "  p.get() = " << p.get()
            << ", p.use_count() = " << p.use_count() << '\n';
  t1.join(); t2.join(); t3.join();
  std::cout << "All threads completed, the last one deleted Derived\n";
}

