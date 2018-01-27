/*
 * 如何实现一个共享指针，在共享指针中保存了共享的数目。
 * 构造函数初始化为1
 * 一旦有赋值构造，值则加1
 * 析构函数则将数值减1，当数值最后为0时析构。
 */

template<class T>
class sharedPtr{
private:
  T* t_;
  int num_;

public:
  sharedPtr(T *t) : t_(t), num_(1) {};
  sharedPtr operator=(sharedPtr ptr) {
    num_++;
  }

  ~sharedPtr() {
    num_--;
    if (0 == num_) {
      delete t_;
    }
  }
};

int main()
{
  int *a = new int[10];
  a[0] = 1;
  sharedPtr<int> ptr(a);
  sharedPtr<int> ptr1 = ptr;

  return 0;
}
