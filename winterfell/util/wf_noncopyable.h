/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 11:27:39
 * @Description: 不可拷贝对象封装，继承之即可实现对象的noncopyable
 */

#pragma once

namespace winterfell {

class Noncopyable
{

public:
  /**
   * @brief 使用默认构造函数
  */
  Noncopyable() = default;
  ~Noncopyable() = default;

  /**
   * @brief 禁用拷贝构造
  */    
  Noncopyable(const Noncopyable&) = delete;

  /**
   * @brief 禁用赋值运算
  */
  Noncopyable& operator=(const Noncopyable&) = delete;
        
};

};