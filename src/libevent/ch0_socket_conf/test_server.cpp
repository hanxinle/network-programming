﻿#include <event2/event.h>
#include <event2/listener.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <iostream>
using namespace std;
#define SPORT 5001

void listen_cb(struct evconnlistener *e, evutil_socket_t s, struct sockaddr *a,
               int socklen, void *arg) {
    cout << "listen_cb" << endl;
}
int main() {
#ifdef _WIN32
    // 初始化socket库
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
#else
    // Linux 需要，忽略管道信号，发送数据给已关闭的socket
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) return 1;
#endif

    std::cout << "test server!\n";
    // 创建 libevent 的上下文
    event_base *base = event_base_new();
    if (base) {
        cout << "event_base_new success!" << endl;
    }
    // 监听端口
    // socket ，bind，listen 集中在一个函数 evconnlistener_new_bind 中
    sockaddr_in sin;
    // ip 地址清零，接收任何 ip 发起的链接请求
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);

    evconnlistener *ev = evconnlistener_new_bind(
        base,       // libevent的上下文
        listen_cb,  // 接收到连接的回调函数
        base,       // 回调函数获取的参数 arg
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,  // 地址重用，evconnlistener
                                                    // 关闭同时关闭 socket
        10,                // 连接队列大小，对应listen函数
        (sockaddr *)&sin,  // 绑定的地址和端口
        sizeof(sin));      // 套接字大小
    //事件分发处理
    if (base) event_base_dispatch(base);
    if (ev) evconnlistener_free(ev);
    if (base) event_base_free(base);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}