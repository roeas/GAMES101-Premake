# GAMES101-Premake
[![MSBuild](https://github.com/Hinageshi01/GAMES101-Premake/actions/workflows/msbuild.yml/badge.svg?branch=main)](https://github.com/Hinageshi01/GAMES101-Premake/actions/workflows/msbuild.yml)

GAMES101-Premake 是一个使用 Premake5 一键配置 GAMES101 作业运行环境的框架。

旨在免去烦杂的 C++ 环境配置问题和虚拟机的使用，更好地专注于图形学原理和实现本身。

目前只支持 Windows + VS2022。

# 依赖
先安装 VS2022 与 CMake（3.15 或更高的版本）。

[VS2022](https://visualstudio.microsoft.com/zh-hans/downloads/)

[CMake Latest Release](https://cmake.org/download/#latest)

# 开始
### 1. 下载本仓库
`git clone https://github.com/roeas/GAMES101-Premake`

### 2. 编译第三方库
运行 `SetupThirdParty.bat`。

### 3. 生成 VS2022 solution
运行 `Setup.bat`。

### 4. 搞定！
打开根目录的 `Frame.sln` 即可。

# 运行
在 VS 中找到 `解决方案管理器`，右键任一你想运行的作业，点击 `设为启动项`，编译并运行。

# 答案
位于 `answer` branch，使用 `git stash` 暂存你当前的修改，`git checkout answer` 切换至答案分支。

使用 `git checkout main` 切换回主分支，`git stash pop` 还原你的修改。

# 其他
- 框架相关
  - 文档位于 `GAMES101-Premake/Document`。
  - 官方代码有不少问题，如果出现上下 / 前后颠倒的情况可以参考：[《GAMES101》作业框架问题详解](https://zhuanlan.zhihu.com/p/509902950)。
  - 作业之间没有任何关系，在一作业内的修改不会应用到另一作业中，编写时注意区分。
  - 修改了一些路径相关的代码以适配框架：
    - 模型位于 `GAMES101-Premake/Frame/Asset/model`。
    - 作业 5、6、7 的输出位于 `GAMES101-Premake/Frame/Asset/output`。
  - 魔改了作业 8 的依赖，遇到任何问题请通过 issue 让我知道。

- C++ 版本：20

- Eigen 版本：3.4.0

- OpenCV 版本：4.8.1
  - OpenCV 依赖项的下载源已设置为由 OpenCV 中国和 GitCode 维护的国内仓库 [OpenCV](https://gitcode.net/opencv/opencv)。
  - TODO：取消 OpenCV 不必要模块的编译。

- Freetype 版本：2.13.2

- GLEW 版本：2.1.0

- GLFW 版本：3.3.8

- 所有相关文件都在仓库目录下，不需要时删除整个文件夹即可。

# 相关链接
[GAMES101-现代计算机图形学入门-闫令琪](https://www.bilibili.com/video/BV1X7411F744/)

[Premake 文档](https://premake.github.io/)

[Eigen 文档](https://eigen.tuxfamily.org/dox/)

[OpenCV 文档](https://docs.opencv.org/4.8.0/index.html)

[GAMES101 Spring 2021 作业资源](https://games-cn.org/forums/topic/s2021-games101-zuoyehuizong/)

[作业 0 资源](https://github.com/slicol/Games101-Homework-Win)
