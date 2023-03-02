###
 # @Author: Maoxiang Sun
 # @Date: 2023-03-02 16:49:57
 # @Description: 生成1000个随机ASIC码，总共大小为1000bye
### 

head -c 1000 /dev/urandom | LC_CTYPE=C tr -dc '[:print:]' > test.txt