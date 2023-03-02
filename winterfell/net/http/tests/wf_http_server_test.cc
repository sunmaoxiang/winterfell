/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-25 15:48:39
 * @Description: 测试http服务器
 */

#include "winterfell/net/http/wf_http_server.h"
#include "winterfell/net/http/wf_http_context.h"
#include "winterfell/net/http/wf_http_request.h"
#include "winterfell/net/http/wf_http_response.h"
#include "winterfell/net/wf_endpoint.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"
#include <iostream>
#include <fstream>
using namespace winterfell;

void onRequest(const HttpRequest& req, HttpResponse* resp) {
  std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
  auto headers = req.headers();
  for(auto& header: headers) {
    std::cout << header.first << ": " << header.second << std::endl;
  }
  if (req.path() == "/")
  {
    resp->setStatusCode(HttpResponse::k200ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/html");
    resp->addHeader("Server", "Winterfell");
    string now = Timestamp::now().toFormattedString();
    resp->setBody("<html><head><title>This is title</title></head>"
                  "<body><h1>Hello</h1>Now is " +
                  now +
                  "</body></html>");
  }
  else if(req.path() == "/image") {
    resp->setStatusCode(HttpResponse::k200ok);
    resp->setStatusMessage("OK");
    resp->setContentType("image/png");
    std::ifstream is("image/dog.png", std::ifstream::in | std::ios::binary);
    std::stringstream ss;
    ss << is.rdbuf();
    is.close();
    resp->setBody(ss.str());
  }else if(req.path() == "/video") {
    resp->setStatusCode(HttpResponse::k200ok);
    resp->setStatusMessage("OK");
    resp->setContentType("video/mp4");
    std::ifstream is("video/cat.mp4", std::ifstream::in | std::ios::binary);
    std::stringstream ss;
    ss << is.rdbuf();
    is.close();
    resp->setBody(ss.str());
  
  } else {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
  }
}


int main() {
  EventLoop loop;
  HttpServer server(&loop, Endpoint(8000), "dummy");
  server.setHttpCallback(onRequest);
  server.setThreadNum(4);
  server.start();
  loop.loop();
  return 0;
}