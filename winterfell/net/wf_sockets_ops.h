/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 21:21:13
 * @Description: 一系列操作socket的封装
 */
#pragma once
#include <netinet/in.h>

namespace winterfell {

namespace sockets {
/**
 * @brief  创建一个非阻塞套接字
*/
int createNonblocking();
/**
 * @brief 绑定一个套接字到指定IPV4地址
*/
void bind(int sockfd, const struct sockaddr_in *addr);
/**
 * @brief: 用于使得sockfd可以进行accept操作
 * @param {int} sockfd TCP套接字，需要已经bind到一个地址上
 * @param {int} backlog 未完成连接队列（收到SYN，处于SYN_RCVD状态） + 已完成连接队列（ESTABLISHED，可以accept）
 * @return {*}
 */
void listen(int sockfd, int backlog=128);

/**
 * @brief 从已连接队列队头返回已连接套接字
*/
int accept(int sockfd, struct sockaddr_in *addr);

/**
 * @brief 不同于close直接释放套接字以及创建的资源，只是关闭读，对方会读一个长度为0的data，目的是收发数据完整性，当关闭后依旧可以读（优雅关闭）
*/
void shutdownWrite(int sockfd);

/**
 * @brief 设置SO_REUSEADDR选项，对应TCP套接字处于TIME_WAIT状态下的socket可以重复绑定
 */
void setReuseAddr(int sockfd, bool on);

/**
 * @brief 关闭Nagle算法，减少小包延迟
*/
void setTcpNodelay(int sockfd, bool on);

/**
 * @brief 关闭套接字，释放资源
*/
void close(int fd);

}

}