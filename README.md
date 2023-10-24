# GAMES101-Premake
[![MSBuild](https://github.com/Hinageshi01/GAMES101-Premake/actions/workflows/msbuild.yml/badge.svg?branch=main)](https://github.com/Hinageshi01/GAMES101-Premake/actions/workflows/msbuild.yml)

由 Premake5 构建的 GAMES101 作业框架，只支持 Windows + VS2022。

# 依赖
先下载并安装 VS2022 与 CMake。

[VS2022](https://visualstudio.microsoft.com/zh-hans/downloads/)

[CMake Latest Release](https://cmake.org/download/)

# 开始
### 1.下载本仓库
`git clone https://github.com/Hinageshi01/GAMES101-Premake.git`

### 2.编译第三方库
双击 `SetupThirdParty.bat`

### 3.去吃个饭或者泡杯茶
大概要跑个十分钟左右。

### 4.构建 VS2022 solution
双击 `Setup.bat`

### 5.搞定！
双击在根目录生成的 `Frame.sln`，即可打开框架。

# 运行
在 VS 中找到 `解决方案管理器`，右键任一你想运行的作业，点击 `设为启动项`，编译并运行即可。

# 答案
位于 answer branch，使用 `git stash` 暂存你当前的修改，`git checkout answer` 切换至答案分支。

使用 `git checkout main` 切换回主分支，再使用 `git stash pop` 还原你的修改。

_答案的正确食用方式：理解之后把它关掉，切回主分支自己实现一遍。_

# 其他
- 作业之间没有任何关系，在一作业内的修改不会应用到另一作业中，编写时注意区分。
- 修改了一些路径相关的代码以适配框架。

- Eigen 版本：3.4.0
- Eigen 作为纯头文件库包含。

- OpenVC 版本：4.8.1
- OpenVC 依赖项的下载源已设置为由 OpenCV 中国和 GitCode 维护的国内仓库 [OpenCV](https://gitcode.net/opencv/opencv)。
- OpenVC 编译为动态链接库，如果出现找不到符号的情况可以尝试重新生成解决方案。
- TODO：取消 OpenVC 不必要模块的编译。
- 所有相关文件都在仓库目录下，不需要时删除整个文件夹即可。

# 相关链接
[Premake 文档](https://premake.github.io/)

[Eigen 文档](https://eigen.tuxfamily.org/dox/)

[OpenCV 文档](https://docs.opencv.org/4.8.0/index.html)

[GAMES101 Spring 2021 作业资源](https://games-cn.org/forums/topic/s2021-games101-zuoyehuizong/)

[作业 0 资源](https://github.com/slicol/Games101-Homework-Win)
