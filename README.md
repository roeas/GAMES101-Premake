# GAMES101-Premake
使用 Premake5 一键构建 GAMES101 作业框架

# 依赖
[VS2022](https://visualstudio.microsoft.com/zh-hans/downloads/)

[CMake Latest Release](https://cmake.org/download/)

# 开始
## 下载本仓库
`git clone https://github.com/Hinageshi01/GAMES101-Premake.git`

## 编译第三方库
双击 `SetupThirdParty.bat`
将进行第三方库依赖项的下载以及编译，可能要等待十分钟左右。

## 构建 VS2022 solution
双击 `Setup.bat`
将在根目录生成 `Frame.sln`，双击即可打开框架。

# 运行
在 VS 中找到 `解决方案管理器`，右键任一你想运行的作业，点击 `设为启动项`，编译并运行即可。

# 其他
- 不同作业之间没有任何关系，在一作业内的修改不会应用到另一作业中，编写时注意区分。
- Eigen 作为纯头文件库包含。
- OpenVC 依赖项的下载源已设置为由 OpenCV 中国和 GitCode 维护的国内仓库 [OpenCV](https://gitcode.net/opencv/opencv)。
- OpenVC 编译为动态链接库，如果出现找不到符号的情可以尝试重新生成解决方案。
- TODO : 取消 OpenVC 不必要模块的编译。

# 相关链接
[Premake 文档](https://premake.github.io/)
[GAMES101 Spring 2021 作业资源](https://games-cn.org/forums/topic/s2021-games101-zuoyehuizong/)
[作业 0 来源](https://github.com/slicol/Games101-Homework-Win)
