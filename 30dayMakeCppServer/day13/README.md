- 加入`CMake`来管理项目
- 通过配置`CMakeLists.txt`文件，来设置编译项目的配置
- 执行`cmake ..`时，首先读取参数目录的`CMakeLists.txt`，如果遇到`add_subdirectory()`命令，就进入指定的子目录，处理该子目录中的`CMakeLists.txt`文件
- 最后用生成的`MakeFile`进行编译

