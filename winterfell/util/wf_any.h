/*
 * @Author: Maoxiang Sun
 * @Date: 2023-03-03 13:24:51
 * @Description: 实现一个std::any的相关功能 
 */

#include <typeindex>
#include <memory>
namespace winterfell {
struct Any
{
  Any() : ptr{0} {}
  template <typename T>
  Any(const T &t) : ptr{new Data<T>(t)} {};
  Any(const Any &rhs) {
    ptr = rhs.ptr->clone();
  }
  Any &operator=(const Any &rhs)
  {
    if (ptr)
      delete ptr;
    ptr = rhs.ptr->clone();
    return *this;
  }
  Any(Any &&rhs) noexcept
  {
    ptr = rhs.ptr;
    rhs.ptr = 0;
  }
  Any &operator=(Any &&rhs) noexcept
  {
    if (ptr)
      delete ptr;
    ptr = rhs.ptr;
    rhs.ptr = 0;
    return *this;
  }
  struct Base
  {
    virtual Base *clone() = 0;
    virtual ~Base() {}
  };
  template <typename T>
  struct Data : Base
  {
    T data;
    virtual Base *clone() { return new Data<T>(data); }
    Data(const T &t) : data(t) {}
  };
  Base *ptr;
  template <typename T>
  T &get_data()
  {
    return ( static_cast<Data<T> *>(ptr) )->data;
  }
  ~Any()
  {
    if (ptr)
      delete ptr;
  }
};
}