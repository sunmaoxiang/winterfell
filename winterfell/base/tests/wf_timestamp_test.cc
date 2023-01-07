/*
 * @Author: Maoxiang Sun
 * @Date: 2023-01-07 20:40:21
 * @Description: 测试时间戳类
 */

#include "winterfell/base/wf_timestamp.h"
#include <iostream>
using namespace winterfell;
using namespace std;
int main() {
  auto now = Timestamp::now(); 
  cout << now.toFormattedString() << endl;
  cout << now.toString() << endl;
}
