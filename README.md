<!--
 * @Author: Maoxiang Sun
 * @Date: 2023-01-06 21:21:55
 * @Description: README
-->

# Winterfell

> Winter Is Coming.



# 项目日志

* 2023年1月6日：实现基本的日志库和线程库。日志库核心功能是提供五种日志输出等级(DEBUG, INFO, WARN, ERROR, FATAL), 使用lazy单例模式, 全局使用唯一一个日志输出器。线程库对phtread系统调用中的phtread_create, pthread_join, pthread_detach进行了封装。