/*
 * @Author: Maoxiang Sun
 * @Date: 2023-03-02 15:11:26
 * @Description: 实现ftp服务器，主要执行传入当前文件夹路径参数
 */

//  ./bin/ftp_server text

#include "winterfell/net/http/wf_http_server.h"
#include "winterfell/net/http/wf_http_context.h"
#include "winterfell/net/http/wf_http_request.h"
#include "winterfell/net/http/wf_http_response.h"
#include "winterfell/net/wf_endpoint.h"
#include "winterfell/base/wf_log.h"
#include "winterfell/net/wf_event_loop.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <dirent.h>
#include <vector>
#include <cstring>
#include <sys/stat.h>
#include <numeric>
using namespace winterfell;
using namespace std;
std::string dirPath;
std::string list_files(const std::string& path) {
  DIR* dir = opendir(path.c_str());
  struct dirent* entry;
  vector<string> files;
  while((entry = readdir(dir)) != nullptr) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    struct stat st;
    string file_path = path + "/" + entry->d_name;
    if (stat(file_path.c_str(), &st) == -1)
    {
      continue;
    }

    if (S_ISREG(st.st_mode))
    {
      files.emplace_back(entry->d_name);
    }
  } 
  
  auto ret = accumulate(files.begin(), files.end(), std::string(), [](const std::string& a, const std::string&b) { return a.empty() ? b : a + "\n" + b; } );
  return ret;
}


void onRequest(const HttpRequest &req, HttpResponse *resp)
{
  std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
  auto headers = req.headers();
  for (auto &header : headers)
  {
    std::cout << header.first << ": " << header.second << std::endl;
  }
  
  if(req.query() == "?ls") {
    resp->setStatusCode(HttpResponse::k200ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/plain");
    resp->addHeader("server", "ftp");
    std::string listfiles = list_files(dirPath);
    resp->setBody(listfiles);

  } else if(req.query() == "?get") {
    string filename = req.getHeader("filename");
    // 打开文件，定位到需要的那一个range，读取并发送
    resp->setStatusCode(HttpResponse::k206PartialContent);
    resp->setContentType("application/octet-stream");
    resp->addHeader("server", "ftp");
    string range = req.getHeader("Range");
    stringstream ss(range);
    string sub_str;
    getline(ss, sub_str, '-');
    int beg = stoi(sub_str);
    getline(ss, sub_str);
    int end = stoi(sub_str);
    auto file_path = dirPath + "/" + filename;
    ifstream input_file(file_path, std::ios::binary);
    input_file.seekg(beg, std::ios::beg);
    char* buf = new char[end - beg + 1];
    input_file.read(buf, end - beg + 1);
    buf[end - beg + 1] = '\0';
    resp->setBody(buf);
    delete buf;
    input_file.close();
  }
  
  else
  {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Need ./ftp_server [指定路径]" << std::endl;
    return 1;
  }
  dirPath = std::string(argv[1]);
  EventLoop loop;
  HttpServer server(&loop, Endpoint(8000), "ftp");
  server.setHttpCallback(onRequest);
  server.setThreadNum(1);
  server.start();
  loop.loop();
  return 0;
}