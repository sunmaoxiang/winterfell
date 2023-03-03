/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-23 15:52:28
 * @Description: HttpServer定义
 */

#include "winterfell/net/http/wf_http_server.h"
#include "winterfell/net/http/wf_http_request.h"
#include "winterfell/net/http/wf_http_response.h"
#include "winterfell/net/http/wf_http_context.h"
#include "winterfell/net/wf_tcp_connection.h"
#include "winterfell/base/wf_log.h"
#include <functional>
using namespace std::placeholders;
namespace winterfell {

namespace detail {
void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
  resp->setStatusCode(HttpResponse::k404NotFound);
  resp->setStatusMessage("Not Found");
  resp->setCloseConnection(true);
}
}

HttpServer::HttpServer(EventLoop *loop, const Endpoint &listenEnd, const string &name)
    : server_(loop, listenEnd, "http_server"),
      httpCallback_(detail::defaultHttpCallback)
{

server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

void HttpServer::start() {
  LOG_INFO << "HttpServer[" << server_.name() << "] starts listening on " << server_.ipPort();
  server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
  if (conn->connected()) {
    conn->setContext(new HttpContext());
  }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer& buf, Timestamp receiveTime) {
  HttpContext* context = conn->Context().get_data<HttpContext*>();
  if (!context->parseRequest(&buf, receiveTime)) {
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
  }
  if(context->getAll()) {
    onRequest(conn, context->request());
    context->reset();
  }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req) {
  const string& connection = req.getHeader("Connection");
  bool close = connection == "close" || (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive") ;
  HttpResponse response(close);
  httpCallback_(req, &response);
  Buffer buf;
  response.appendToBuffer(&buf);
  conn->send(buf.retrieveAsString());
  if (response.closeConnection()) {
    conn->shutdown();
  }
}


}

