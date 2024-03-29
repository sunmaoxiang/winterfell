/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-24 16:38:23
 * @Description: http request声明
 */

#pragma once

#include "winterfell/base/wf_timestamp.h"
#include <string>
#include <map>
#include <cassert>

namespace winterfell {
using std::string;
using std::map;
class HttpRequest {
public:
  enum Method {
    kInvalid, kGet, kPost, kHead, kPut, kDelete
  };
  enum Version {
    kUnkown, kHttp10, kHttp11
  };
  HttpRequest()
  : method_(kInvalid),
    version_(kUnkown) {}
  
  void setVersion(Version v) {
    version_ = v;
  }
  Version getVersion() const {
    return version_;
  }
  bool setMethod(const char* start, const char* end) {
    assert(method_ == kInvalid);
    string m(start, end);
    if (m == "GET") {
      method_ = kGet;
    } else if (m == "POST") {
      method_ = kPost;
    } else if (m == "HEAD") {
      method_ = kHead;
    } else if (m == "PUT") {
      method_ = kPut;
    } else if (m == "DELETE") {
      method_ = kDelete;
    } else {
      method_ = kInvalid;
    }
    return method_ != kInvalid;
  }
  Method method() const {
    return method_;
  }
  const char* methodString() const {
    const char* result = "UNKNOWN";
    switch (method_) {
      case kGet:
        result = "GET";
        break;
      case kPost:
        result = "POST";
        break;
      case kHead:
        result = "HEAD";
        break;
      case kPut:
        result = "PUT";
        break;
      case kDelete:
        result = "DELETE";
        break;
      default:
        break;
    }
    return result;
  }
  void setPath(const char* start, const char* end) {
    path_.assign(start, end);
  }
  const string& path() const {
    return path_;
  }
  void setQuery(const char* start, const char* end) {
    query_.assign(start, end);
  }
  const string& query() const {
    return query_;
  }
  void setReceiveTime(Timestamp t) {
    receiveTime_ = t;
  }
  Timestamp receiveTime() const {
    return receiveTime_;
  }
  void addHeader(const char* start, const char* colon, const char* end) {
    string field(start, colon);
    ++colon;
    while(colon < end && isspace(*colon)) {
      ++colon;
    }
    string value(colon, end);
    while(!value.empty() && isspace(value[value.size() - 1])) {
      value.resize(value.size() - 1);
    }
    headers_[field] = value;
  }
  string getHeader(const string& field) const {
    string result;
    const auto it = headers_.find(field);
    if (it != headers_.end()) {
      result = it->second;
    }
    return result;
  }
  const map<string,string>& headers() const {
    return headers_;
  }
  void swap(HttpRequest& that) {
    std::swap(method_, that.method_);
    std::swap(version_, that.version_);
    path_.swap(that.path_);
    query_.swap(that.query_);
    std::swap(receiveTime_,that.receiveTime_);
    std::swap(headers_, that.headers_);
  }
private: 
  Method method_;
  Version version_;
  string path_;
  string query_;
  Timestamp receiveTime_;
  map<string,string> headers_;


};

}