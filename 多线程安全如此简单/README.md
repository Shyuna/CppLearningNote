代码实现了简单的多线程读写。

参考的教程是：[【C++11】多线程安全如此简单_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1VJ4m1g7Nh?spm_id_from=333.788.videopod.sections&vd_source=679917d9d5376d925f23bae253e6f8d8)

可以使用多线程安全的队列mt_queue来实现各种功能：

- 向队列中push对操作共享数据结构的函数，可以实现这些函数的串行执行，从而保证多线程安全。
- 设定队列的数量上限，实现类似信号量的功能。
- 队列数量设置为1时，实现mutex的功能。
- 使用variant可以对队列中不同类型的数据分别执行相应的处理。

