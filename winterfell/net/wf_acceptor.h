/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-12 18:43:28
 * @Description: Acceptor class声明，用于accept新TCP连接，并通过回调通知使用者
 */

#include "winterfell/util/wf_noncopyable.h"
#include "winterfell/util/wf_callbacks.h"
#include "winterfell/net/wf_channel.h"
#include "winterfell/net/wf_socket.h"
namespace winterfell {
class EventLoop;
class Endpoint;
class Acceptor : Noncopyable {
public:
  Acceptor(EventLoop* loop, const Endpoint& listenAddr);
  void setNewConnectionCallback(const NewConnectionCallback& cb) 
  { newConnectionCallback_ = cb; }

  bool listening() const {return listenning_;}
  void listen();
  
private:
  /**
   * 处理当有连接到来时，进行accept操作
  */
  void handleRead();

  EventLoop* loop_;
  Socket acceptSocket_;  
  Channel acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listenning_;

};
}