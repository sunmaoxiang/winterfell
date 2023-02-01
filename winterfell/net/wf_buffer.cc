/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-20 15:36:55
 * @Description: 声明buffer类，用于TcpConnection中读写数据
 */
#include "winterfell/net/wf_buffer.h"
#include <algorithm>
#include <assert.h>

namespace winterfell {

const ssize_t Buffer::kCheapPrepend;
const ssize_t Buffer::kInitialSize;

Buffer::Buffer()
: buffer_(kCheapPrepend + kInitialSize),
  readerIndex_(kCheapPrepend),
  writerIndex_(kCheapPrepend) {
    assert(readableBytes() == 0);
    assert(writableBytes() == kInitialSize);
    assert(prependableBytes() == kCheapPrepend);
}
Buffer::Buffer(Buffer&& buf) 
  : buffer_(std::move(buf.buffer_)), 
    readerIndex_(std::move(buf.readerIndex_)),
    writerIndex_(std::move(buf.writerIndex_)) {
}

void Buffer::swap(Buffer &rhs) {
  buffer_.swap(rhs.buffer_);
  std::swap(readerIndex_, rhs.readerIndex_);
  std::swap(writerIndex_, rhs.writerIndex_);
}

ssize_t Buffer::readableBytes() const {
  return writerIndex_ - readerIndex_;
}
ssize_t Buffer::writableBytes() const {
  return buffer_.size() - writerIndex_;
}
ssize_t Buffer::prependableBytes() const {
  return readerIndex_;
}
const char* Buffer::peek() const {
  return begin() + readerIndex_;
}
void Buffer::retrieve(ssize_t n) {
  assert(n <= readableBytes());
  readerIndex_ += n;
}
string Buffer::retrieveAsString() {
  string str(peek(), readableBytes());
  retrieveAll();
  return str;
}
string Buffer::retrieveAsString(ssize_t n) {
  string str(peek(), n);
  retrieve(n);
  return str;
}
void Buffer::append(const string &appStr) {
  append(appStr.c_str(), appStr.size());
}
void Buffer::append(const char* appData, ssize_t len) {
  if (writableBytes() < len) {
    makeSpace(len);
  }
  std::copy(appData, appData + len, beginWrite());
  writerIndex_ += len;
}
void Buffer::append(const void* appData, ssize_t len) {
  append(static_cast<const char*>(appData), len);
}
void Buffer::prepend(const void* preData, ssize_t len) {
  assert(len <= prependableBytes());
  readerIndex_ -= len;
  const char* d = static_cast<const char*>(preData);
  std::copy(d, d+len, begin() + readerIndex_);
}

void Buffer::makeSpace(ssize_t len) {
  if (writableBytes() + prependableBytes() < len + kCheapPrepend)
  {
    buffer_.resize(writerIndex_ + len);
  }
  else
  {
    size_t readable = readableBytes();
    std::copy(begin() + readerIndex_, begin() + writerIndex_, begin() + kCheapPrepend);
    readerIndex_ = kCheapPrepend;
    writerIndex_ = readerIndex_ + readable;
  }
}

ssize_t Buffer::readFd(int fd, int* savedErrno) {
  char extrabuf[65536];
  struct iovec vec[2];
  const ssize_t writable = writableBytes();
  vec[0].iov_base = begin() + writerIndex_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;
  const ssize_t n = readv(fd, vec, 2);
  if (n < 0) {
    *savedErrno = errno;
  } else if (n <= writable) {
    writerIndex_ += n;
  } else {
    writerIndex_ = buffer_.size();
    append(extrabuf, n - writable);
  }
  return n;
}

}