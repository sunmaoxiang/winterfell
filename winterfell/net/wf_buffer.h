/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-20 15:36:47
 * @Description: 声明buffer类，用于TcpConnection中读写数据
 */


#pragma once
#include "winterfell/net/wf_buffer.h"
#include <vector>
#include <string>
#include <errno.h>
#include <sys/uio.h>
#include <algorithm>
#include <cassert>
namespace winterfell {
using std::string;
class Buffer {
public:
  static const ssize_t kCheapPrepend = 8;
  static const ssize_t kInitialSize = 1024;

  Buffer();
  Buffer(Buffer&& buf); 
  void swap(Buffer& rhs);
  ssize_t readableBytes() const;
  ssize_t writableBytes() const;
  ssize_t prependableBytes() const ;
  char* peek();
  const char* peek() const;
  const char* findCRLF() {
    const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
    return crlf == beginWrite() ? NULL : crlf;
  }
  void retrieve(ssize_t n);
  void retrieveAll() {
     readerIndex_ = kCheapPrepend; writerIndex_ = kCheapPrepend; 
  }
  void retrieveUntil(const char* end) {
    assert(peek() <= end);
    assert(end <= beginWrite());
    retrieve(end - peek());
  }
  string retrieveAsString();
  string retrieveAsString(ssize_t n);
  void append(const string& appStr);
  void append(const void* appData, ssize_t len);
  void append(const char* appData, ssize_t len);
  void prepend(const void* preData, ssize_t len);

  ssize_t readFd(int fd, int* savedErrno);
  
private:
  char* begin() {
    return &*buffer_.begin();
  }
  const char* begin() const {
    return &*buffer_.begin();
  }
  char* beginWrite() {
    return begin() + writerIndex_;
  }
  void makeSpace(ssize_t len);
  std::vector<char> buffer_;
  ssize_t readerIndex_;
  ssize_t writerIndex_;

  static const char kCRLF[];
};


}