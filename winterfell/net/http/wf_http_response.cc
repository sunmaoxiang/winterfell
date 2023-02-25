/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-24 18:50:08
 * @Description: http response 定义
 */

#include "winterfell/net/http/wf_http_response.h"
#include "winterfell/net/wf_buffer.h"
#include <cstdio>

using namespace winterfell;

void HttpResponse::appendToBuffer(Buffer* output) const {
  char buf[32];
  snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
  output->append(buf);
  output->append(statusMessage_);
  output->append("\r\n");
  
  if (closeConnection_) {
    output->append("Connection: close\r\n");
  }
  else {
    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
    output->append(buf);
    output->append("Connection: Keep-Alive\r\n");
  }

  for(const auto& header : headers_) {
    output->append(header.first);
    output->append(": ");
    output->append(header.second);
    output->append("\r\n");
  }

  output->append("\r\n");
  output->append(body_);
}