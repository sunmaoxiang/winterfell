/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-24 19:37:52
 * @Description: 状态机
 */
#pragma once

#include "winterfell/net/http/wf_http_request.h"

namespace winterfell {

class Buffer;

class HttpContext {
public:
  enum HttpRequestParseState {
    kExpectRequestLine,
    kExpectHeaders,
    kExpectBody,
    kGotAll,
  };
  HttpContext()
    : state_(kExpectRequestLine)
  {}

  bool parseRequest(Buffer* buf, Timestamp receiveTime);
  
  bool getAll() const {
    return state_ == kGotAll;
  }

  void reset() {
    state_ = kExpectRequestLine;
    HttpRequest dummy;
    request_.swap(dummy);
  }

  const HttpRequest& request() const {
    return request_;
  }

  HttpRequest& request() {
    return request_;
  }

private:
  bool processRequestLine(const char* begin, const char* end);

  HttpRequestParseState state_;
  HttpRequest request_;
};

}
