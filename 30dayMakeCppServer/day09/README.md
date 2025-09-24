- 将`InetAddress`成员变量`address`、`addr_len`修改为私有，增加了`get`、`set`接口。
- 修改了`Socket`中的`Socket::bind`函数和`Socket::accept`函数，`::bind`局部变量地址后，调用`setInetAddr`修改`InetAddress`私有成员变量`address`、`addr_len`。调用`::accept`时`addr_len`要设置为`sizeof(address)`。
- 修改了`Acceptor`，删除了私有成员`InetAddress *addr`，改为每次使用时`new`，使用完马上`delete`。修改了`Acceptor::acceptConnection`将`accept`操作从`Server`中移到`Acceptor`中。
- `Server`在创建`Connection`对象时，需要设置`Connection`的`DeleteConnectionCallback`回调函数，用于析构`Connection`时将指针从`Server`的`map`中删除，以及释放内存空间。



