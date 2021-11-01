### sftt

Simple and Security File Transport Tool (简单并且安全的文件传输工具)

### 支持:

a) 字符文件传输与二进制文件传输

b) 支持目录文件传输

c）产生随机端口传输(避免监听数据)

d）支持数据分块加密传输

e) 服务端支持多进程接收文件

f）服务端和客户端协商传输大小



### 后续上线功能：
a) 客户端支持多连接同时发送数据

b) 客户端支持数据压缩功能

c）服务端支持解压缩功能

d）sftt服务支持断点续传

e）使用更安全的数据加密算法

f）支持超大文件的同时分块传输

g) 服务端实现端口动态更新，支持多端口多连接



### 使用方法：

```
#git clone https://github.com/chowqiang/sftt.git
#cd sftt/
#ls
bin     conf   Dockerfile  lib   Makefile   server  test
client  debug  head        logs  README.md  src     tool

# make
gcc -o ./src/random_port.o -I ./head/ -c ./src/random_port.c
gcc -o ./src/config.o -I ./head/ -c ./src/config.c
gcc -o ./src/encrypt.o -I ./head/ -c ./src/encrypt.c
gcc -o ./src/net_trans.o -I ./head/ -c ./src/net_trans.c
gcc -o ./server/server -I ./head/ ./server/server.c ./src/random_port.o ./src/config.o ./src/encrypt.o ./src/net_trans.o
gcc -o ./client/client -I ./head/ ./client/client.c ./src/random_port.o ./src/config.o ./src/encrypt.o ./src/net_trans.o


服务端:
进入server目录起动server服务：./server
port is 12556
开始等待数据传输。。。。。
get empty config line!
conf  block_size is 10240
store path: /usr/local/sftt/

ps：数据默认在/usr/local/sftp  可以再/etc/sftt/sftt_server.conf中配置


客户端传输数据:
进入client目录带起动参数传输
	传输文件：./client   文件名
	传输目录: ./client   目录名

```
