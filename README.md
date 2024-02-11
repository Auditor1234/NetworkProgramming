### 该项目包括一下内容:
- 共享内存
- 信号量
- socket编程（TCP和UDP）
- c风格的多线程（线程池的实现）
- 利用system v信号量和共享内存实现的循环缓冲区
- 获取主机IP
- 解析输入参数函数getopt()
- 内存映射函数mmap()
- poll非阻塞I/O多路复用

### 环境及配置
- g++ 11.4.0
- ubuntu 22.04.1 LTS
- cmake 3.22.1
- VSCode 1.86.1

### 使用方法

1. 对于shimfifo(共享内存的先进先出的循环缓冲区)，先进入文件夹目录下，用`./send`向共享内存发送消息，后用`./recv`命令接收消息，如果卡住了使用`./destroy`清理信号量

2. 对于threadpool(实现的线程池)，进入文件夹下后使用`./test`命令运行即可

3. 其他的使用g++ xxx.cpp -o main生成对应的可执行文件即可，部分项目需要链接库文件如`-lpthread`或`-lrt`

4. select, poll和epoll三种I/O多路复用方法仅实现了服务器部分，客户端用netcat模拟(用法：nc -v {ip}:{port})

### 参考项目

<https://github.com/834810071/NetworkProgramming>