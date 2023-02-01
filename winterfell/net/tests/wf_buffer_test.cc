/*
 * @Author: Maoxiang Sun
 * @Date: 2023-02-01 12:37:17
 * @Description: 测试buffer相关功能
 */

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "winterfell/net/wf_buffer.h"
#include <gtest/gtest.h>
#include <string>
using namespace winterfell;
using namespace std;


TEST(Buffer, testBufferAppendRetrieve) {
  Buffer buf;
  EXPECT_EQ(buf.readableBytes(), 0);  
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize);
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);

  const string str(200, 'x');
  buf.append(str);
  EXPECT_EQ(buf.readableBytes(), str.size());
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize - str.size());
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend);

  const string str2 = buf.retrieveAsString(50);
  EXPECT_EQ(str2.size(), 50);
  EXPECT_EQ(buf.readableBytes(), str.size() - str2.size());
  EXPECT_EQ(buf.writableBytes(), Buffer::kInitialSize - str.size());
  EXPECT_EQ(buf.prependableBytes(), Buffer::kCheapPrepend + str2.size());
  EXPECT_EQ(str2, string(50, 'x'));
}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}