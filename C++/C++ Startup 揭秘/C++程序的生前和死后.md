# REF

[youtobe](https://www.youtube.com/watch?v=PmMOgoTodys&list=PLJlgxk8narLfStNCt5utxgBDCxv5p6Ejq&index=1)

# FAQ

## CRT 是什么

虽然我们写代码时从 `main` 函数开始，但实际上在 `main` 之前和之后，**C Runtime Library (CRT，C 运行时库)** 都在默默地做着大量繁重的工作。

简单来说，**CRT 是 C 语言程序能够运行的“基础设施”**。它不仅实现了 ISO C 标准规定的函数库（如 `printf`, `malloc`），还负责程序的启动、退出、内存管理以及与操作系统的交互。

以下是对 C Runtime Library 的详细技术介绍：

------

### 1. CRT 的核心职责

CRT 的作用可以分为两个层面：**可见的（标准库）和不可见的（运行时支持）**。

#### A. 实现 C 标准库 (ISO C Standard Library)

这是我们平时编程直接调用的部分。CRT 封装了底层操作系统的差异，提供了一套跨平台的 API：

- **I/O 操作**：`stdio.h` (`printf`, `fopen` 等)。CRT 负责维护文件句柄、缓冲区（Buffering），最终调用操作系统的 System Call（如 Linux 的 `write` 或 Windows 的 `WriteFile`）。
- **内存管理**：`stdlib.h` (`malloc`, `free`)。CRT 向操作系统申请大块内存（如通过 `brk` 或 `mmap`），然后在用户态维护一个堆（Heap）管理器来分配小块内存。
- **字符串与数学**：`string.h`, `math.h` 等纯算法实现。
- **时间与日期**：`time.h`。

#### B. 程序的启动与退出 (Startup & Exit)

这是 CRT 最重要但常被忽视的功能。

- **Entry Point (入口点)**：操作系统并不直接调用 `main`。链接器（Linker）通常会将入口点设置为 CRT 提供的某个符号（例如 Linux 下的 `_start`，Windows 下的 `mainCRTStartup`）。
- **环境初始化**：
  - 初始化栈（Stack）指针。
  - 初始化堆（Heap）。
  - 处理全局变量和静态变量（`.data` 段的赋值，`.bss` 段的清零）。
  - 获取环境变量和命令行参数（`argv`, `argc`）。
  - **C++ 特有**：调用全局对象的构造函数。
- **调用 Main**：一切准备就绪后，CRT 调用你的 `int main(argc, argv)`。
- **清理与终止**：
  - `main` 返回后，CRT 捕获返回值。
  - 调用通过 `atexit()` 注册的函数。
  - **C++ 特有**：调用全局对象的析构函数。
  - 刷新并关闭所有打开的 I/O 流。
  - 调用操作系统 API 结束进程（如 Linux 的 `exit()`）。

------

### 2. CRT 与操作系统的关系

CRT 处于 **用户代码** 和 **操作系统内核 (Kernel)** 之间。

```
+---------------------+
|     用户代码 (User) |  ->  main()
+---------------------+
          |
          v
+---------------------+
|  C Runtime (CRT)    |  ->  malloc(), printf(), _start
+---------------------+
          |
          v
+---------------------+
|  系统调用接口       |  ->  Linux Syscalls / Win32 API
+---------------------+
          |
          v
+---------------------+
|  操作系统内核 (OS)  |  ->  管理硬件、内存、进程
+---------------------+
```

- **封装性**：在 Linux 上，`fopen` 底层可能调用 `open` 系统调用；在 Windows 上，则调用 `CreateFile`。CRT 让你用同一套 C 代码在不同 OS 上运行。
- **用户态运行**：CRT 代码运行在用户态（User Mode），只有在进行 I/O 或申请系统资源时才会陷入内核态。

------

### 3. 主流的 CRT 实现

不同的平台和编译器有不同的 CRT 实现：

#### Linux / Unix

- **glibc (GNU C Library)**：Linux 上最标准的 CRT，功能极其强大但体积庞大。
- **musl libc**：轻量级，遵循标准，常用于 Alpine Linux 和嵌入式容器。
- **uClibc / dietlibc**：专为嵌入式系统设计。

#### Windows (Microsoft Visual C++)

Windows 的情况比较复杂，经历过多次迭代：

- **MSVCRT.DLL**：旧版本的 CRT，虽然系统里还有，但现代编译器尽量不再依赖它。
- **UCRT (Universal C Runtime)**：从 Windows 10 和 VS2015 开始引入。微软将 CRT 重构为系统组件的一部分，随 Windows 更新而更新，解决了长久以来的“版本地狱”问题。
- **VCRuntime**：包含编译器特定的代码（如异常处理、调试支持）。

------

### 4. 静态链接 vs 动态链接

在使用 CRT 时，程序员需要在编译选项中做出选择：

| **特性** | **静态链接 CRT (/MT on Windows, -static on GCC)**       | **动态链接 CRT (/MD on Windows, default on GCC)**         |
| -------- | ------------------------------------------------------- | --------------------------------------------------------- |
| **原理** | 将 CRT 的目标代码直接复制到你的 EXE 中。                | EXE 中只保留引用，运行时加载 `.dll` 或 `.so`。            |
| **优点** | **部署简单**，只需一个 EXE，不依赖目标机器的环境。      | **文件小**，内存占用低（多个程序共享内存中的 CRT 代码）。 |
| **缺点** | **文件大**；如果 CRT 有安全漏洞，必须重新编译发布程序。 | **部署麻烦**，目标机器缺少对应的 CRT 库会导致无法运行。   |

> **注意**：在 Windows 开发中，这是常见的“坑”。如果你的库用了 `/MT`，而主程序用了 `/MD`，链接时会报错（如 LNK2005），因为它们试图使用不同的堆管理器。

------

### 5. 如果没有 CRT 会怎样？(Freestanding 环境)

作为程序员，你可能会接触到 **嵌入式开发** 或 **操作系统内核开发**。在这些场景下，通常没有完整的 CRT 支持（称为 **Freestanding Environment**）。

在这种模式下：

1. 你不能使用 `stdio.h`（没有 `printf`，因为没有 OS 帮你打印）。
2. 你不能使用 `malloc`（除非你自己写一个内存分配器）。
3. 你需要自己编写入口汇编代码（设置栈指针，跳转到 C 函数）。
4. 你通常只能使用 `<stdint.h>`, `<stddef.h>` 等不依赖 OS 的头文件。

------

### 总结

C Runtime Library 不仅仅是一堆函数，它是 C 程序的**生命维持系统**。

1. 它负责程序的**生**（初始化环境）和**死**（清理资源）。
2. 它是连接**抽象 C 语言规范**与**具体操作系统硬件**的桥梁。
3. 理解 CRT 有助于你解决链接错误、内存泄漏以及跨平台移植问题。

## Linux Startup Code 在哪？

对于 Linux 上的 C 程序来说，Startup Code（启动代码）并不是单一的一个文件，而是分散在几个**对象文件（Object Files）**中，在链接阶段由编译器自动“偷偷”加进去。

要找到它们，我们需要从三个维度来看：**物理文件路径**、**编译链接过程**以及**源码位置**。

------

### 1. 物理位置：二进制文件在哪里？

在大多数现代 Linux 发行版（如 Ubuntu/Debian/CentOS）上，启动代码编译好的对象文件通常位于 `/usr/lib` 下的特定架构目录中。

核心的启动文件是 **`crt1.o`** (或者支持 PIE 的 `Scrt1.o`)。

你可以尝试在终端中运行以下命令来查找它们：

```bash
# Ubuntu / Debian (x86_64)
ls /usr/lib/x86_64-linux-gnu/crt*.o

# CentOS / RHEL / Fedora
ls /usr/lib64/crt*.o
```

你会看到以下几个关键文件（也就是传说中的 CRT Startup Files）：

- **`crt1.o`** (C RunTime 1): 包含程序的真正入口点 **`_start`**。它的任务是设置参数，调用 `__libc_start_main`。
- **`crti.o`** (C RunTime Init): 包含 `.init` 段的**头部**（Prologue），用于程序初始化时的函数调用（如全局构造函数）。
- **`crtn.o`** (C RunTime Ne): 包含 `.init` 段的**尾部**（Epilogue）。
- *(注：如果是编译位置无关可执行文件 PIE，`crt1.o` 会变成 `Scrt1.o`)*

------

### 2. 链接过程：GCC 是怎么把它们塞进去的？

你可以使用 `gcc -v` 参数来“抓包”编译器的链接过程，看看它到底传入了什么给链接器 (`collect2` 或 `ld`)。

> `gcc -v` 时 gcc 编译器的 verbose（详细输出）模式，用于显示编译过程中的额外信息，包括：
>
> 1. 编译期的版本和配置信息
> 2. 详细的编译过程信息
> 3. 显示 include 搜索路径

创建一个空的 `main.c`，然后运行：

```bash
gcc -v main.c -o main
```

在输出信息的最后几行（通常很长），你会看到类似这样的 `collect2` 命令（我简化了路径）：

Plaintext

```bash
-dynamic-linker /lib64/ld-linux-x86-64.so.2 
...
/usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/Scrt1.o  
/usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/crti.o   
/usr/lib/gcc/x86_64-linux-gnu/11/crtbeginS.o 
...
/tmp/cccvaNQm.o  
...
/usr/lib/gcc/x86_64-linux-gnu/11/crtendS.o  
/usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/crtn.o
```

* `Scrt1.o`：含程序入口 `_start`（不是 `main`！）。它由 C 运行时（CRT）提供，负责： 
  * 设置栈
  * 调用 `_init` 
  * 调用 `main` 
  * 处理 `main` 返回值并调用 `exit`
* `crti.o`：“Constructors init” —— 初始化段的**开头**。用于构造函数（如 C++ 全局对象、`__attribute__((constructor))`）的 `.init` 段起始桩代码
* `crtn.o`：“Constructors termination” —— 初始化段的**结尾**。与 `crti.o` 配对，形成完整的 `.init`/`.fini` 段
* `crtbeginS.o`/`crtendS.o`：GCC 特有的运行时支持文件： 
  * 用于 C++ 全局构造/析构（调用 `__do_global_ctors` / `__do_global_dtors`） 
  * 支持异常处理（EH）和栈展开 
  * `S` 表示 “shared”（用于 PIE 或共享库；静态链接用 `crtbegin.o`）
* `/tmp/cccvaNQm.o`：你的 `main.c` 编译后生成的**目标文件**（包含 `main` 函数等）
* `-dynamic-linker /lib64/ld-linux-x86-64.so.2`：指定动态链接器（program interpreter），程序加载时由内核调用它来加载共享库（如 `libc.so`）

**关键顺序**： 这就好比一个三明治，你的代码被夹在中间： `Scrt1.o` + `crti.o` + `crtbegin.o` + **[你的代码]** + `crtend.o` + `crtn.o`

------

### 3. 源码位置：代码长什么样？

这些二进制文件最终都是由 **glibc (GNU C Library)** 的源码编译出来的。如果你去下载 glibc 的源码，Startup Code 通常位于架构相关的目录中。

以 x86_64 架构为例，核心的 `_start` 实现通常在： **`sysdeps/x86_64/start.S`** (这是一个汇编文件)

**它的核心逻辑大致如下（伪汇编）：**

```asm
_start:
    # 1. 清空 %rbp (栈帧指针)，标记这是栈的最底端，方便调试工具识别
    xor %rbp, %rbp 

    # 2. 从栈上获取 argc 和 argv
    pop %rdi        # argc 放入第一个参数寄存器
    mov %rsp, %rsi  # argv 指针放入第二个参数寄存器

    # 3. 栈对齐 (16字节对齐，这是 x86_64 ABI 的要求)
    and  $~15, %rsp

    # 4. 准备调用 __libc_start_main 的参数
    # 参数包括：main函数的地址、_init、_fini 等
    push %rax       # 压栈对齐
    push %rsp       # 栈顶
    mov  $main, %rdi # 放入 main 函数地址
    
    # 5. 调用 glibc 的初始化主控函数
    call __libc_start_main  
    
    # 6. 正常情况下 __libc_start_main 不会返回，如果返回了则调用 hlt 停机
    hlt
```

### 4. 如何在自己的程序中“看见”它？

既然你是程序员，我们可以直接反汇编一个简单的程序来看看这个入口。

1. 编译一个程序：`gcc main.c -o main`

2. 查看 ELF 头，找到 Entry point address（入口地址）：

   ```bash
   readelf -h main | grep Entry
   # 输出示例: Entry point address: 0x1040 (这个地址就是 _start)
   ```
   
3. 反汇编查看该地址的代码：

   ```bash
   objdump -d main | grep _start -A 20
   ```

你会清楚地看到 `_start` 里面有一行 `call` 指令指向了 `__libc_start_main@plt`。

### 总结

- **物理上**：在 `/usr/lib/.../crt1.o` (或 `Scrt1.o`)。
- **逻辑上**：是 ELF 文件的 Entry Point (符号 `_start`)。
- **源码上**：在 glibc 源码的 `sysdeps/x86_64/start.S`。

## `main()` 的 calling convertion

`main()` 函数的 **Calling Convention（调用约定）** 是一个非常经典但也容易被误解的话题。

简单直接的答案是：**在大多数常见的 C/C++ 环境中，`main()` 遵循该平台的“标准 C 调用约定”。**

具体来说：

- **x86 (32-bit):** 通常是 **`__cdecl`**。
- **x64 (64-bit):** 遵循各平台的 **标准 x64 ABI**（Windows 和 Linux/macOS 有所不同）。

由于你也是程序员，我们可以深入到底层，看看 `main` 具体是如何被调用、参数如何传递以及堆栈如何处理的。

### 1. 谁真正调用了 `main`？

在讨论调用约定之前，必须澄清一个误区：**操作系统（Kernel）并不直接调用 `main`。**

操作系统的加载器（Loader）将程序加载到内存后，会跳转到编译器链接进去的 **Entry Point（入口点）**。这个入口点通常是一段汇编代码（属于 C Runtime/CRT），它负责初始化环境，然后才调用 `main`。

- **Linux (GCC):** 入口通常是 `_start`，它调用 `__libc_start_main`，最后调用 `main`。
- **Windows (MSVC):** 入口通常是 `mainCRTStartup` 或 `WinMainCRTStartup`。

### 2. 具体的调用约定详解

#### 2.1 x86 (32-bit) 环境：`__cdecl`

在 32 位系统上，`main` 几乎总是使用 **`__cdecl`**（C Declaration）调用约定。

- **参数传递：** 通过 **栈（Stack）** 传递。
- **压栈顺序：** **从右向左** (`envp` -> `argv` -> `argc`)。
- **栈清理（Stack Cleanup）：** **调用者（Caller）负责清理**。这是 `cdecl` 的关键特征，它允许函数（如 `printf`）接受可变数量的参数，而 Callee 不知道参数的具体数量，无法正确知道要 pop 掉多少的栈空间。
- **名称修饰：** 编译器通常会在函数名前加一个下划线（例如 `_main`）。

**汇编视角 (Intel Syntax):**

```asm
; 假设由 CRT 启动代码调用
push    envp        ; 环境变量指针压栈
push    argv        ; 参数字符串数组压栈
push    argc        ; 参数个数压栈
call    _main       ; 调用 main 函数
add     esp, 12     ; Caller (CRT) 清理堆栈 (3个参数 * 4字节 = 12)
```

#### 2.2 x64 (64-bit) 环境

在 64 位时代，`__cdecl`、`__stdcall` 等复杂的区分基本消失了，取而代之的是统一的平台 ABI。

**2.2.1 Linux / macOS (System V AMD64 ABI)**

这是 Unix-like 系统的标准。

- **参数传递：** 优先使用 **寄存器**。
  - `argc` (参数 1) -> **`RDI`**
  - `argv` (参数 2) -> **`RSI`**
  - `envp` (参数 3) -> **`RDX`**
- **栈清理：** 只有当参数过多寄存器放不下时才用栈，且由调用者清理。
- **返回值：** 存放在 `RAX` 中。

**2.2.2 Windows (x64 calling convention)**

Windows 有自己的一套 x64 约定。

- **参数传递：** 优先使用 **寄存器**，但寄存器不同。
  - `argc` (参数 1) -> **`RCX`**
  - `argv` (参数 2) -> **`RDX`**
  - `envp` (参数 3) -> **`R8`**
- **Shadow Space：** 调用者必须在栈上预留 32 字节（Shadow Store），尽管前 4 个参数通过寄存器传递，`main` 函数也可以将寄存器内容回写到这块栈空间中以便调试。
- **返回值：** 存放在 `RAX` 中。

### 3. 为什么是 `cdecl` (在 32 位时)?

你可能会问，为什么不把 `main` 设为 `__stdcall`（被调用者清理堆栈，生成的代码更小）？

主要原因是 历史兼容性 和 可变参数 的设计哲学。

虽然标准 main 只有 (int argc, char* argv[])，但在早期的 C 语言实现以及某些特定扩展中，main 可能会被视作类似 printf 那样的函数，或者允许传递额外的参数（如 envp）。cdecl 是唯一原生支持可变参数（Varargs）的调用约定，因为它允许调用者知道到底压入了多少参数，从而由调用者安全地恢复堆栈指针。

### 4. 数据对照表

| **特性**     | **x86 (32-bit)** | **x64 (Linux/System V)**  | **x64 (Windows)**        |
| ------------ | ---------------- | ------------------------- | ------------------------ |
| **约定名称** | `__cdecl`        | System V AMD64 ABI        | Microsoft x64            |
| **参数位置** | 栈 (Stack)       | 寄存器 (RDI, RSI, RDX...) | 寄存器 (RCX, RDX, R8...) |
| **压栈顺序** | 右 -> 左         | (如果有栈参数) 右 -> 左   | (如果有栈参数) 右 -> 左  |
| **栈清理方** | **Caller (CRT)** | Caller                    | Caller                   |
| **返回值**   | `EAX`            | `RAX`                     | `RAX`                    |

### 特殊情况：嵌入式系统

如果你是在写裸机（Bare-metal）嵌入式程序（如 ARM Cortex-M），情况会完全不同。

- 通常没有 OS，也没有标准的 CRT。
- Reset Handler（复位中断处理函数）会初始化 `.data` 和 `.bss` 段。
- 最后直接跳转（Branch）到 `main`。
- 在这种情况下，`main` 就是一个普通的函数，遵循 ARM AAPCS 标准（参数在 R0-R3）。

### 总结

对于一般的应用程序开发：

> `main()` 由 **CRT 启动代码** 调用，在 32 位下遵循 **`__cdecl`**（参数入栈，调用者清理），在 64 位下遵循 **平台默认的寄存器传参约定**。

返回值（`return 0;`）最终会被放在累加寄存器（EAX/RAX）中，CRT 代码会读取这个值，并将其传递给 `exit()` 系统调用，从而成为进程的退出码（Exit Code）。

## PPT 46 页

右图来自微软 CRT（msvcrt/ucrt）内部的 `ioinit.c`：

**① 分配并初始化 io 系列结构体（ioinfo）**

右图第一句：

```
Allocates and initializes initial array(s) of ioinfo structs.
```

说明 `_ioinit()` 会建立内部的 I/O 管理结构体数组，例如：

- `_osfile[]`
- `_osfhnd[]`
- `_pioinfo[]`

这些结构体保存每个 **FILE** 对应的信息（是否文本模式、是否 init，缓冲模式等）。

这些结构最终会与 `FILE*` 的 `_file` 成员关联起来。

**② 获取从父进程继承的文件句柄（STARTUPINFO）**

右图：

```
Obtains the StartupInfo structure from the OS.
The inherited file handle info is pointed to by lpReserved2...
```

这表示 Windows 的父进程（例如 cmd.exe）可能会把某些句柄传给子进程。

这些句柄包括：

- stdin
- stdout
- stderr
- pipe
- 重定向后的文件

**_ioinit() 负责读取这些继承的句柄，并建立 FILE 对象。**

**③ StartupInfo 中句柄信息的格式**

右图描述了结构体的内存布局：

```
bytes 0~3:   N，句柄数量
bytes 4~(N+3):  N values for osfile
bytes (N+4)~(5*N+3): N HANDLE values
```

也就是说：

```
父进程传下来的结构 = [数量N][每个句柄的flags][每个句柄的实际HANDLE]
```

这些信息会被子进程的 `_ioinit`() 解析，用于构造 0/1/2 号标准文件。

**④ 初始化 fd 0、1、2（核心）**

右图关键段落：

```
Next, osfhnd and osfile for the first three ioinfo structs,
corresponding to handles 0, 1, and 2, are initialized:
```

意味着：

```
ioinfo[0] → stdin
ioinfo[1] → stdout
ioinfo[2] → stderr
```

然后：

```
If inherited handle is INVALID_HANDLE_VALUE:
    then call GetStdHandle() to obtain HANDLE
```

说明：

- 如果父进程没有继承某个句柄
- CRT 会自己调用 `GetStdHandle(STD_INPUT_HANDLE)` 等函数来获取默认句柄

也就是：

> **无论是否继承，_ioinit 都会确保 stdin/stdout/stderr 一定存在。**

**⑤ 获取句柄类型并设置为文本模式**

```
GetFileType()
```

用于确认：

- 是字符设备（console）
- 是管道
- 是文件

并设置：

- 文本模式
- 二进制模式

右图也特别提到：

```
force it to text mode
(standard input/output/error are to always start out in text mode)
```

即：

> C 标准 I/O 的 stdin/stdout/stderr 默认都是文本模式。

**⑥ 注意事项**

1. 父进程传下来的句柄并不一定全部是 open 的。
2. 确保 io struct 数组不溢出（避免超过最大 fd 数）。
3. 用于和 exec/spawn 交互时正确传递句柄。

**执行完毕后，最终能保证：**

1. `stdin` 的 `FILE` 正确初始化，包含：

   * 一个有效 HANDLE
   * 缓冲模式为“行缓冲”

2. `stdout` 的 `FILE` 正确初始化

   * HANDLE 来自父进程或默认控制台
   * 文本模式

3. `stderr` 的 `FILE` 正确初始化

   * 无缓冲（为了立即输出错误）

4. `_osfile[]`、`_osfhnd[]`、`_pioinfo[]` 结构都设置完毕

   标准文件的 flags 就位。

   所以 `main()` 刚开始执行时你就可以使用：

   ```cpp
   printf();
   scanf();
   fprintf(stderr, ...);
   ```

## PPT 47 页

这一页展示的是：

> **Windows CRT 中 FILE、ioinfo、_iob[]、_piob[]、_initstdio()、fopen() 的完整关系。**

这是整个 MSVC CRT I/O 系统最核心的一页，我会把所有编号（①②③等）按图中逻辑解释清楚。

**<font color=red>我的理解</font>**

在 Windows 的 MSVC CRT 里，大致是这样：

1. **所有的 `FILE*` 都挂在 `__piob[512]` 这个指针数组里**
   - `__piob[0..19]` 指向静态的 `_iob[20]`（前三个是 stdin/out/err）
   - `__piob[20..511]` 是后面 `fopen()` 动态分配出来的 `FILE` / `_FILEX`
2. **每个 `FILE` 里有一个整数字段（通常叫 `_file`）**
   - 里面存的是“C 级别文件编号”的**索引**
   - 这个索引用来定位到 `__pioinfo[row][col]` 中的某个 `ioinfo` 结构
3. **`__pioinfo`（或 `_pioinfo`）是“C 层的 opened file table”**
   - 它是二维数组：`ioinfo* _pioinfo[IOINFO_ARRAYS];`，每个指向 32 个 `ioinfo`
   - 每个 `ioinfo` 对应一个“C 文件描述符”（类似 Linux 的 fd）
   - 里面保存了和 **kernel 模式文件对象关联的各种信息**

**<font color=red>FILE</font>**

C 语言标准层面：`FILE` 是“黑盒”，C 标准只说：

- `FILE` 用来表示一个“流”（stream）
- 你只能通过 `fopen/fread/fwrite/fclose` 等函数操作它
- 结构体内容由实现决定，程序员不应该直接访问它的成员

也就是说：**标准不要求它有哪几个字段**，但是典型实现都非常相似。

实现层面：`FILE` 里通常包含哪些东西？以 MSVC / glibc 这类实现为例，典型 `FILE` 里会有这些类别的信息（名字不一定完全一样，但含义相似）：

① 关联的底层文件标识

- **fd 或 handle**
  - 在 Linux：一个 `int` 文件描述符（fd）
  - 在 Windows：一个编码过的索引 `_file`，再映射到 `ioinfo` 里的 `HANDLE`
- 这是把 **高层 stdio** 和 **内核文件对象** 连起来的纽带

② 缓冲区相关信息（最重要的一块）

用于实现带缓冲的 I/O：

常见字段概念（名字类似）：

- `unsigned char* _base`
  - 指向缓冲区起始地址
- `unsigned char* _ptr`
  - 当前读/写指针在缓冲区中的位置
- `int _cnt` / `size_t _cnt`
  - 缓冲区中还剩多少可读/可写的字节
- `int _bufsiz`
  - 缓冲区总大小

作用：

- `fread()` 先从缓冲区读，不够再找 OS
- `fwrite()` 先写到缓冲区，必要时才 flush 到 OS
- `setvbuf()`/`setbuf()` 就是在改这些字段

③ 文件/流的状态标志（flags）

比如一个 `int _flag` 或一堆 bitfield，记录：

- 当前模式：只读 / 只写 / 读写（`r`/`w`/`r+` 等）
- 是否是文本模式 / 二进制模式
- 缓冲模式：全缓冲 / 行缓冲 / 无缓冲
- 是否已经遇到 EOF（`feof()` 看的就是这个）
- 是否已经出现错误（`ferror()` 看的就是这个）
- 当前是处于读模式还是写模式（混合模式时切换需要 `fseek` 或 `fflush`）

④ 文件位置 & 额外状态

实际上真正的文件偏移由内核维护，但 stdio 层面通常还会记一些辅助信息，例如：

- 最近一次 `fseek` 的位置缓存
- 对 `ungetc()` / `ungetwc()` 的一个“回推字符槽”
- 多字节编码 / 宽字符 I/O 时的转换状态（`mbstate_t`）

⑤ 多线程同步信息（在 _MT 版本里）

在 MSVC 的多线程 CRT 里，`FILE` 通常被包装成 `_FILEX`：

- 里面有一个 `FILE f;`
- 后面再加一个 `CRITICAL_SECTION lock` 或类似的锁

用于：

- `fprintf()`/`fputs()` 等在多线程下对同一个 FILE 做 I/O 时加锁
- 保证同一条输出不要被别的线程“夹断”

⑥ 与临时文件 / 文件名相关的字段

例如：

- 临时文件名指针（`tmpfile()` 生成的名字）
- 对调用 `remove()` 时是否需要自动删除的标记

这些字段一般只在部分实现里存在。

**<font color=red>总览：Windows CRT 的 FILE 与 HANDLE 映射机制</font>**

图最核心的三组结构：

```
FILE 对象数组：_iob[20]        ← CRT 固定静态 FILE（最前 3 个是 stdin/out/err）

FILE* 指针数组：_piob[512]     ← 用于 fopen() 分配 FILE*

文件句柄信息：ioinfo[][]       ← 每个 FILE 关联的 OS HANDLE、模式等
```

最终关系链如下：

```
FILE.fh  ← 索引 → ioinfo[row][col] → osfhnd（OS HANDLE）
```

也就是说：

- FILE 对象本身只是缓冲区、模式等信息
- 其 `_file` 这个整型字段指向 ioinfo 数组
- ioinfo 才是保存 OS HANDLE 的地方

**<font color=red> ① CRT 的 ioinfo 数组：`_pioinfo[IOINFO_ARRAYS]`</font>**

图中第一行：

```cpp
#define IOINFO_ARRAYS 64
_CRTIMP ioinfo * _pioinfo[IOINFO_ARRAYS];
```

解释：

- CRT 准备了 **64 个指针**，每个指针指向一个 ioinfo 数组
- 每个 ioinfo 数组有 32 个元素（见后面 ②）
- 所以最多文件数：64 × 32 = 2048 个文件描述槽位

与 Linux 的 fd table 类似，但加了二维结构方便扩展。

**<font color=red>② 第一次需要文件槽时动态分配 32 个 ioinfo</font>**

图中黄色注释：

```cpp
// 每次動態分配 32 個 struct ioinfo
_malloc_crt(32 * sizeof(ioinfo))
```

说明：

- ioinfo 槽位按 **32 个一组** 分配
- 只有真正需要（如 fopen）时才 malloc
- 并将其挂接到 `_pioinfo[x]`

二维结构组织如下：

```cpp
pioinfo[0] // → 32 个 ioinfo
pioinfo[1] // → 32 个 ioinfo
...
```

这样可以节省未使用文件描述槽位的内存。

**<font color=red>③ `_ioinit()` 解析继承的 FILE HANDLE</font>**

图中写：

* `ioinit()` 经由 `GetStartupInfo()` 取得 inherited opened file handles

说明：

- Windows 父进程（如 cmd）可能将句柄继承给子进程
- ioinfo 用于存储继承的 stdin/stdout/stderr 的 HANDLE 和模式

**<font color=red>④ CRT 建立静态 `_iob[20]` 数组，元素是 FILE</font>**

图写：

* CRT 维护一個静态的 `_iob[20]`

意思是：

- CRT 内部有 20 个静态 FILE 对象：MSVC 早期设计里，希望 fopen() **前 17 次** 打开文件时不需要调用 malloc()，而是直接使用 `_iob[]` 的预分配 FILE 对象。
- 其中前三个固定是：

```cpp
_iob[0] = stdin;
_iob[1] = stdout;
_iob[2] = stderr;
```

对应宏定义：

```cpp
#define stdin  (&_iob[0])
#define stdout (&_iob[1])
#define stderr (&_iob[2])
```

这些 FILE 对象用于默认标准 I/O。

**<font color=red>⑤ FILE 的 `_file` 字段用于指向 ioinfo 槽位</font>**

``` cpp
#ifdef _MT 
typedef struct {
    FILE f;
    CRITICAL_SECTION lock;
} _FILEX;
#else
typedef FILE _FILEX;
#endif // #define _MT
```

图中重点一句：

* FILE 中的 int _file 就是 index of opened file table

并且：

* 高 5 bits 是第二维索引（哪一组 0~63）
* 低 6 bits 是第一维索引（0~31）

FILE._file 是怎样编码的？

```
_file = (row << 6) | col
```

其中：

- row = 0~63
- col = 0~31

可以用 _file 解出 ioinfo 的具体位置。

这是 FILE → HANDLE 的关键跳转机制。

**<font color=red>⑥ `_initstdio()` 动态分配 `_piob[512]`</font>**

图中写：

* _initstdio() 动态分配 __piob[512]

解释：

- _iob[20] 是静态 FILE（前 20 个）
- _piob[512] 是 FILE* 指针数组，用于 fopen() 分配 FILE*

结构如下：

```cpp
_piob[0]~_piob[19]    // 指向 _iob[0]~_iob[19]
_piob[20]~_piob[511]  // 初始为 NULL（等 fopen 分配）
```

因此 fopen() 会优先在 `_piob[]` 中找 free slot。

**<font color=red>⑦ fopen() 的分配流程</font>**

图中写：

* fopen() 首先在 _piob[512] 中找 free entry

fopen 的流程：

1. 在 `_piob[512]` 中找一个空槽

   如果没有，malloc 一个新的 FILE 并放入 `_piob[i]`。

2. 分配 ioinfo 槽位

   在所有 `_pioinfo[row][col]` 里寻找空位。

   把该槽位的索引编码进 `FILE._file`。

3. 调用 CreateFile()

   得到 OS HANDLE → 写入到：

   ```cpp
   ioinfo[row][col].osfhnd
   ```

**<font color=red>⑧ FILEEX（多线程版本的 FILE）</font>**

右下角黄色区域：

```cpp
typedef struct {
    FILE f;
    CRITICAL_SECTION lock;
} _FILEX;
```

说明：

- 多线程版本（_MT）时，FILE 会被扩展为 _FILEX
- 多了一个 Windows 的锁

但 FILE 的前部布局不变，因此内部系统仍可用 FILE 指针操作。

**<font color=red>⑨ _piob[] 的前 20 项指向 _iob[]</font>**

图中右侧箭头标明：

* _piob[i] 指向对应 _iob[20] 中的 FILE

也就是：

```cpp
_piob[0] = &_iob[0]   → stdin
_piob[1] = &_iob[1]   → stdout
_piob[2] = &_iob[2]   → stderr
...
_piob[19] = &_iob[19] ← 其他预留 FILE
```

之后 20~511 用于 fopen()。

**<font color=red>总结</font>**

```
                    +-----------------------------+
                    |    静态 FILE：_iob[20]      |
                    +-----------------------------+
                     ↓  (前3项 stdin/out/err)
_piob[0..19] ------> _iob[0..19]

_piob[20..511] --- malloc(FILE 或 FILEX) --- fopen() 使用的 FILE*

每个 FILE 有一个 _file 字段，编码成：
   row = _file >> 6      (0~63)
   col = _file & 0x3F    (0~31)

最终映射到：
_pioinfo[row] → ioinfo[32]
    ↓
    ioinfo[row][col].osfhnd  ← OS HANDLE（由 CreateFile() 或继承句柄）
```

这是 Windows CRT I/O 的核心机制。









