# 测试框架使用说明

## 介绍

目前测试框架更名为 `test`，并且做了许多简化和使用改进。现在不再是每个测试一个子目录，而是所有测试都在这个目录中。

测试框架支持以下功能：

- 一键回归测试
- 编译/运行单个单元测试
- 调试单个单元测试（native 和 x86_64-qemu）

### 目录结构说明

```
.
├── build       # 存放编译后的可执行文件
├── out         # 存放测试输出（stdout重定向）
├── include     # 存放头文件
└── units       # 存放单元测试
```

### 运行架构

当前支持 `native` 和 `x86_64-qemu`.

### 行为说明

下面以 `make run` 为例，说明当你运行回归测试时发生了什么：

1. GNU make 程序读取 `test/Makefile` 文件并执行 `run` target
2. 将会扫描 `units` 下的每一个 .c 文件，并为它生成对应的 `Makefile.xxx`
3. 接着，脚本将会自动运行 `make Makefile.xxx`，进行编译并运行。main 函数将自动被执行
4. 程序的输出将重定向到 `out/xxx.out` 目录中，脚本将会检查运行是否成功
    - 如果是 `native` 架构，直接检查返回值即可
    - 如果是 `x86-64_qemu` 架构，将会检查 `out/xxx.out` 中 CPU 的 halt 返回值，若非0则判断测试失败
5. 打印出测试结果

## 使用

### 编写单元测试

你可以在 `units` 目录下编写单元测试：

- 新建 `xxx.c` 文件，这里的文件名 `xxx` 就是单元测试的名称
- 编写单元测试时，使用 `check` 函数判断 condition

你可以参考 `add.c` 文件，使用数组对多种输入输出进行判断。

在编写时，如果你的测试比较庞大，建议使用 `logger` 而不是 `printf` 打印结果。

### 运行测试

**运行全部测试：**

```shell
make run ARCH=x86_64-qemu
```

若 ARCH 不指定，则默认为 `native`.

**编译单个测试：**

```shell
make build ARCH=x86_64-qemu target=xxx
```

必须要指定 `target=xxx`，其中 `xxx` 是在 `units` 目录下的文件名（没有.c后缀）。

如果你想运行单个测试，在编译后直接运行 `build` 目录对应的可执行文件即可。

**调试单个测试：**

为了便于调试单个测试，编写了 vscode 的相应调试配置，你只需要把 `.vscode` 目录下的模板文件复制一份（注意不要删除或直接改名）并更名为 `launch.json` 和 `tasks.json` 即可使用。

如果你要调试native，在对应单元测试文件上直接调试，配置选择 `Debug test unit (native)` 即可。

如果你要调试QEMU，运行如下命令：

```shell
make debug ARCH=x86_64-qemu target=xxx
```

并在对应单元测试文件运行调试配置 `Debug test unit (QEMU)` 即可。

**清除编译文件**

```shell
make clean
```

若清除后仍出现错误的文件链接或使用了此前的临时文件，请删除项目内所有 build 目录（包括 am/klib/kernel 等目录下的 build 目录）并重试。

