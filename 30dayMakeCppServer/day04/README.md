- 在自定义的bind函数中调用系统bind函数需要使用全局作用解析符`::bind`
- Epoll初始化时，给events分配空间，析构时释放空间
- 将events置零`bzero(events,sizeof(*events)*MAX_EVENTS);`，sizeof计算大小
- close fd之后将fd置-1

  
