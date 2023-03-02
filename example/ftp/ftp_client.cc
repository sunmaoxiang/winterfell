/*
 * @Author: Maoxiang Sun
 * @Date: 2023-03-02 15:11:38
 * @Description: 实现命令get ls，get 命令是断点续传功能
 */

#include <iostream>
#include <string>
#include <sstream>



int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "need ./ftp_client ip port" << std::endl;
    return 1;
  }



  std::string cmd;
  while(std::getline(std::cin, cmd)) {
    std::stringstream ss(cmd);
    std::string opt;
    ss >> opt;
    if (opt == "ls") {
      // TODO
      // curl "http://192.168.158.11:8000/?ls"
    } else if (opt == "get") {
      // TODO
      // curl "http://192.168.158.11:8000/?get" - H "filename: test.txt" - H "Range: 0-2"
    } else {
      std::cerr << "no cmd called " << opt << std::endl;
    }

  }

  return 0; 
}
