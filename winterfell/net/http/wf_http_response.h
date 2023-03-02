/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-24 18:49:56
 * @Description: http response 声明
 */

#include <map>
#include <string>
namespace winterfell {
using std::string;
class Buffer;

class HttpResponse {
public:
  enum HttpStatusCode {
    kUnknown,
    k200ok = 200,
    k206PartialContent = 206,
    k301MovedPermanently = 301,
    k400BadRequest = 400,
    k404NotFound = 404,
  };
  
  explicit HttpResponse(bool close_) 
    : statusCode_(kUnknown),
      closeConnection_(close_) {}
  
  void setStatusCode(HttpStatusCode code)
  { statusCode_ = code; }

  void setStatusMessage(const string& message)
  { statusMessage_ = message; }

  void setCloseConnection(bool on)
  { closeConnection_ = on; }

  bool closeConnection() const
  { return closeConnection_; }

  void setContentType(const string& contentType)
  { addHeader("Content-Type", contentType); }

  void addHeader(const string& key, const string& value) {
    headers_[key] = value;
  }
  void setBody(const string& body) {
    body_ = body;
  }
  void appendToBuffer(Buffer* output) const;

private:
  std::map<string, string> headers_;
  HttpStatusCode statusCode_;
  string statusMessage_;
  bool closeConnection_;
  string body_;
};

}
