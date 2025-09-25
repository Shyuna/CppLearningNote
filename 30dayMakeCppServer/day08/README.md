- 将`InetAddress`成员变量`address`、`addr_len`修改为私有，增加了`get`、`set`接口。
- 修改了`Socket`中的`Socket::bind`函数和`Socket::accept`函数，`::bind`局部变量地址后，调用`setInetAddr`修改`InetAddress`私有成员变量`address`、`addr_len`。调用`::accept`时`addr_len`要设置为`sizeof(address)`。
- 修改了`Acceptor`，删除了私有成员`InetAddress *addr`，改为每次使用时`new`，使用完马上`delete`。修改了`Acceptor::acceptConnection`将`accept`操作从`Server`中移到`Acceptor`中。
- 添加`Connection`类封装管理TCP连接，一个`Connection`对象管理一个client-server连接的socket。
- `Server`在创建`Connection`对象时，需要设置`Connection`的`DeleteConnectionCallback`回调函数，用于析构`Connection`时将指针从`Server`的`map`中删除，以及释放内存空间。



【单线程服务器总结】

`InetAddress`封装`sockaddr_in`简化地址的初始化流程。

`Socket`管理`fd`，提供了新的`bind`、`listen`、`accept`、`connect`函数。

`Channel`管理`fd`，维护了监听事件、回调函数、是否被监听。通过回调函数执行任务。

`Epoll`封装`epoll`的`epoll_create1`、`epoll_ctl`、`epoll_wait`函数，将监听到的事件中的`fd`改为`Channel`指针。

`Eventloop`实现了`loop`函数，`Epoll`循环监听获取`Channel`。

`Acceptor`管理`server_socket`，负责处理`client`的连接请求，调用回调函数构造`Connection`对象管理连接。

`Connection`管理`client_socket`，负责处理`client`的写请求，调用回调函数接收`client_socket`发送的数据，进行处理。

`Server`维护一个`EventLoop`、一个`Acceptor`和多个`Connection`。
