<!--
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 21:21:55
 * @Description: README
-->

# Winterfell

> Winter Is Coming.



# 项目日志

## 2023年1月6日：实现基本的日志库和线程库。

1. 日志库核心功能是提供6种日志输出等级(TRACE, DEBUG, INFO, WARN, ERROR, FATAL), 使用lazy单例模式, 全局使用唯一一个日志输出器。
2. 线程库对phtread系统调用中的phtread_create, pthread_join, pthread_detach进行了封装。

## 2023年1月7日：优化日志库的接口

1. 使用stringstream优化日志库用户接口(LOG_XX \<\< aa \<\< bb)，提高易用性。
2. 提供timestamp类，方便打印当前时间。
3. 提供线程库的测试代码同时测试日志库的线程安全性


