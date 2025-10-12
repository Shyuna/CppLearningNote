- 使用`deleteConnectionCallback`回调函数时，需要注意检查没有多次调用，否则会double free
- 通过循环不断读取数据到`readBuffer`中，循环结束时调用`clear`清空`readBuffer`



