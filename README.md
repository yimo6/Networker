# Networker
由C-Lang驱动的单进程网络容器

能够跨平台(Windows/Linux)的高性能网络框架

> 目前仅支持IPv4 + TCP

#### 构建项目

 1. Windows (mingW32)
```shell
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
ctest #运行测试
```
 2. Linux
```shell
mkdir build && cd build
cmake ..
make
ctest #运行测试
```

#### 项目介绍

1. 开发体验
 - 用户态极简主义(避免繁杂的调用工作)
 - 异步式开发(性能优越)
 - 内存池支持(增加内存利用率/减少内存碎片)
 - 毫秒级定时器(最低100ms)

2. 完整的定制内容-框架:
 - 支持自定义缓存大小
 - 多事件响应
 - 异步IO(异步连接 + 异步处理)
 - 自定义通信协议

特色功能:

 - [√] 跨平台支持性
 - [√] 提供多种基础设施

