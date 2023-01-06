/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 14:54:12
 * @Description: 单例模式封装
 */

namespace winterfell {

template <class T>
class Singleton
{
public:
  /**
   * @brief 返回单例裸指针，懒汉型单例模式，在C++11下static是线程安全的
   */
  static T *GetInstance()
  {
    static T v;
    return &v;
  }
};

}