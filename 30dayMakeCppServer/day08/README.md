- 修改了`Socket`中的`bind`函数和`accept`函数，`bind`后添加了对`InetAddress`私有成员变量`address`、`addr_len`的修改。`accept`中`addr_len`要设置为`sizeof(address)`。
- 修改了`Acceptor`，删除了私有成员`InetAddress *addr`;改为每次需要使用时`new`，使用完马上`delete`。修改了`Acceptor::acceptConnection`将`accept`操作从`Server`中移到`Acceptor`中。
- `Server`在创建`Connection`对象时，需要设置它的`DeleteConnectionCallback`回调函数，用于将对象从`map`中删除，以及释放内存空间。



