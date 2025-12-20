- 修改成server自定义处理方式
- `Connection::channel` 等变量是私有的，不能在外部直接访问，如果需要对私有成员变量进行操作，需要封装成对应的public方法
- server中通过设置`server->onConnection`，自定义服务器对连接的处理方式







