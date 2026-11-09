# GDB

## 简介

GDB 是 GNU Debugger，GNU 项目提供的通用调试器，最常用于调试 C 和 C++ 程序，也能支持 Fortran、Go，以及部分其他语言和目标平台。它的核心作用是：**让你在程序运行过程中“停下来、看现场、改状态、继续跑”**。

你可以把它理解成一个“程序运行显微镜”。

平时程序出问题，通常只看到结果：

- 崩溃了
- 输出不对
- 卡住了
- 某个变量莫名其妙变了
- 多线程行为异常

GDB 的价值在于，它能让你看到：

- 程序现在执行到哪一行
- 当前调用栈是什么
- 每个函数是怎么一步步调用过来的
- 某个变量当前值是多少
- 某个内存地址里是什么
- 某个条件下程序为什么会进入某条分支
- 崩溃时到底是谁触发的

所以它不是“查日志”的工具，而是**<font color=blur>直接观察程序运行状态</font>**的工具。

## 常用命令

### 1. 运行与执行流控制

- `run(r)`: 启动被调试的程序。若程序中设置了断点，执行流将在命中的第一个断点处挂起。
- `start`: 启动程序并临时在 `main` 函数入口处设置断点，随后挂起。
- `continue(c)`: 恢复挂起的程序执行，直到遇到下一个断点或程序正常结束。
- `next(n)`: 单步执行指令。当当前指令为函数调用时，不会陷入（Step over）该函数内部，而是将其作为单条指令执行完毕。
- `step(s)`: 单步执行指令。当当前指令为函数调用时，会陷入（Step into）该函数内部并在首行挂起。
- `until(u)`: 继续执行程序，直到指令指针跳出当前的循环体作用域。
- `until + 行号`: 继续执行，并无条件在指定的源代码行号处挂起。
- `finish(fi)`: 恢复执行，直到当前栈帧对应的函数正常返回，随后挂起并输出函数的返回值及调用参数。
- `return`: 强制当前函数立即弹出调用栈（抛弃剩余未执行的指令），并可携带指定的表达式结果作为返回值传递给上层调用者。
- `jump(j)`: 强制修改指令寄存器，使程序执行流无条件跳转至指定的代码行或内存地址。
- `call 函数(参数)`: 在当前执行上下文中同步调用目标程序内的指定函数，并传递参数（例如：`call gdb_test(55)`）。

### 2. 断点设置与管理

- `break(b) n`: 在指定的源代码第 n 行设置软件断点（支持路径指定，如 `b OAGUPDATE.cpp:578`）。
- `bfunc` / `b 函数名`: 在指定符号（函数入口地址）处设置断点。
- `b fn1 if a>b`: 设置条件断点。仅当指定的布尔表达式为真时，程序才会在此挂起。
- `rbreak [regex]`: 匹配正则表达式，在所有符合命名规则的函数入口处批量设置断点。
- `tbreak`: 设置一次性临时断点。该断点在被命中一次后会被 GDB 自动销毁。
- `info breakpoints` / `info b`: 枚举当前调试会话中配置的所有断点、观察点及捕获点的信息与状态。
- `disable n` / `enable n`: 禁用（保留配置但不生效）或重新启用编号为 n 的断点。
- `enable once [num]`: 启用指定断点，并在其被命中一次后自动将其状态回退为禁用。
- `delete n`: 销毁编号为 n 的断点。
- `delete breakpoints`: 销毁当前会话中的所有断点。
- `clear n`: 移除指定源代码行号或函数地址上的所有断点标记。

### 3. 数据观察、打印与修改

- `print(p) 表达式`: 计算并打印作用域内有效表达式的值（包括解引用、指针运算等）。亦可用于覆写内存态（如 `p variable_name = new_value`）。
- `set print element 0`: 取消数组或字符串输出时的默认截断长度限制，实现内存的全量打印。
- `display 表达式`: 将指定表达式加入自动求值列表。每次程序因单步或断点挂起时，自动计算并打印该列表中的值。
- `info display` / `delete display [id]`: 列出当前所有的 display 配置 / 移除全部或指定 ID 的 display 项。
- `watch [expr/field]`: 设置观察点。当指定表达式或内存地址的值发生**修改/写入**操作时触发中断。
- `rwatch [expr/field]`: 设置读观察点。当指定变量或内存地址被**读取**时触发中断。
- `awatch [expr/field]`: 设置访问观察点。当指定变量或内存地址被**读取或修改**时均触发中断。
- `watch expr thread [threadid]`: 针对特定线程上下文的变量表达式设置观察点。
- `whatis [field]`: 输出指定变量或表达式的顶层数据类型声明。
- `ptype [field]`: 输出指定变量或表达式的深层结构定义（例如递归展开 struct 的成员）。
- `info locals`: 输出当前栈帧（作用域）内所有局部变量的内存快照。
- `set variable [field]=[expr]`: 显式安全地修改作用域内指定变量的值。
- `set $r0=[value]`: 直接覆写指定 CPU 架构寄存器（如 `$r0`）的值。

### 4. 源代码与汇编查看

- `list(l)`: 输出当前执行点附近的源代码，默认步长为 10 行。不带参数连续执行将依序向下滚动输出。
- `list +` / `list -`: 向后（文件尾方向）或向前（文件头方向）输出源代码。
- `list 行号`: 输出以指定行号为绝对中心的源代码片段。
- `list 函数名`: 输出指定函数签名所在的源文件代码。
- `list [first],[last]`: 输出指定行号区间（起始行至终止行）的完整源代码。
- `list [filename:function]`: 输出特定源文件内指定函数的代码。
- `show listsize` / `set listsize [num]`: 查询或修改 `list` 命令单次输出的默认代码行数配置。
- `disassemble(dis)`: 将当前函数的机器码反汇编并输出汇编指令序列。
- `show disassembly-flavor`: 查询当前的汇编指令显示风格。
- `set disassembly-flavor intel`: 将反汇编语法风格切换为 Intel 格式（默认为 AT&T 格式）。

### 5. 堆栈、进程与环境信息

- `where` / `bt` / `backtrace`: 回溯并打印当前线程的完整调用栈（Call Stack）信息。
- `bt n` / `bt -n`: 仅打印调用栈的最内层（栈顶）n 个栈帧 / 最外层（栈底）n 个栈帧。
- `bt full`: 深度回溯。除栈帧信息外，一并打印每一层栈帧内部的局部变量状态。
- `up` / `down`: 在当前线程的调用栈中，向外层（调用者）或内层（被调用者）移动调试上下文的视点。
- `frame(f) n`: 将当前的调试上下文直接切换至编号为 n 的指定栈帧层级（0 为当前执行帧）。
- `info function`: 列出当前程序符号表中已加载的所有可见函数签名。
- `info program`: 输出当前被调试进程的执行状态、系统进程标识符（PID）及触发挂起的底层原因。
- `info sharedlibrary`: 列举目标进程当前已映射至内存空间的所有动态链接库及其基址信息。
- `set args 参数` / `show args`: 向目标程序注入或设定启动时的命令行参数 / 打印当前配置的参数列表。

### 6. 多线程、信号与高级捕获

- `info threads`: 枚举当前进程所属的所有线程。列表中带 `*` 号的代表 GDB 当前正聚焦的活动线程。
- `thread [id]`: 将当前的调试焦点（执行上下文）强行切换至指定编号（id）的线程。
- `thread apply all [cmd]`: 将指定的 GDB 命令广播至所有活动线程并批量执行（如 `thread apply all bt` 查看所有线程堆栈）。
- `thread apply [id1] [id2]... [cmd]`: 针对指定的多个线程批量执行指令（如 `thread apply 1 2 3 bt`）。
- `set scheduler-locking on`: 强锁定模式。仅允许当前被聚焦的线程获得 CPU 时间片执行指令，严格冻结其余所有线程。
- `set scheduler-locking off`: 自由并发模式（默认）。允许所有线程并发竞争 CPU。
- `set scheduler-locking step`: 步进保护模式。在执行单步（`step`/`next`）时冻结其他线程，但在执行跨越性指令（`continue`/`until`/`finish`）时允许其他线程并发。
- `catch` / `tcatch [事件]`: 挂载系统级事件捕获点（如 `catch syscall close` 拦截系统调用）。
- `signal SIGSEGV`: 强制由 GDB 调度器向目标进程注入并发送指定的系统信号。
- `handle SIGSEGV`: 配置 GDB 对特定异步信号的处理策略（如静默忽略、透传给程序或拦截挂起）。
- `checkpoint` / `restart [id]`: 为当前进程状态创建内存与寄存器快照，后续可通过 `restart` 回滚至该历史快照点。

### 7. 内存洞察、动态管控与源码环境

- `x/[n][f][u] 内存地址`: 读取并格式化输出指定内存地址中的裸数据（如 `x/10xw 0x7fffffffe000` 以十六进制、四字节为单位打印 10 个内存块）。
- `info registers` / `i r`: 输出当前 CPU 所有通用硬件寄存器的瞬时值（也可指定具体寄存器，如 `i r rsp`）。
- `attach [PID]`: 动态附加至操作系统中正在运行的独立进程，附加瞬间目标进程将被挂起。
- `detach`: 解除 GDB 对目标进程的控制权，自动恢复其正常运行。
- `core-file [core_path]`: 加载程序崩溃时生成的 Core Dump 文件，还原崩溃现场的内存与堆栈。
- `directory [path]` / `dir [path]`: 扩展源码搜索路径，解决 `list` 找不到源代码的问题。
- `pwd` / `cd [path]`: 打印 / 改变 GDB 当前的工作目录。
- `set environment [varname]=[value]`: 在启动程序前，为其注入或覆盖环境变量。
- `show environment [varname]`: 查询当前为目标进程配置的环境变量状态。

### 8. TUI 布局与多窗口分割

- `layout`: 激活或切换 GDB 的文本用户界面（TUI）终端视窗。
- `layout src`: 分割终端，开启实时显示当前执行指针所在源代码的视窗。
- `layout asm`: 分割终端，开启实时显示当前反汇编指令的视窗。
- `layout regs`: 开启寄存器监控视窗，实时映射当前 CPU 的硬件寄存器状态。
- `layout split`: 组合视窗，同步展示源代码视窗与反汇编视窗。
- `Ctrl + L`: 强制重绘终端屏幕，消除画面撕裂或乱码。

### 9. 帮助文档

在 GDB 内使用 `apropos {keyword}` 可以模糊查找某条命令：

```
(gdb) apropos b
+ -- Scroll window forward.
- -- Scroll window backward.
< -- Scroll window text to the left.
> -- Scroll window text to the right.
actions -- Specify the actions to be taken at a tracepoint.
add-inferior -- Add a new inferior.
add-symbol-file -- Load symbols from FILE, assuming FILE has been dynamically loaded.
add-symbol-file-from-memory -- Load the symbols out of memory from a dynamically loaded object file.
advance -- Continue the program up to the given location (same form as args for break command).
alias -- Define a new command that is an alias of an existing command.
append binary -- Append target code/data to a raw binary file.
append binary memory -- Append contents of memory to a raw binary file.
append binary value -- Append the value of an expression to a raw binary file.
append memory -- Append contents of memory to a raw binary file.
append value -- Append the value of an expression to a raw binary file.
apropos -- Search for commands matching a REGEXP.
attach -- Attach to a process or file outside of GDB.
awatch -- Set an access watchpoint for EXPRESSION.
backtrace, where, bt -- Print backtrace of all stack frames, or innermost COUNT frames.
backtrace, where, bt -- Print backtrace of all stack frames, or innermost COUNT frames.
bookmark -- Set a bookmark in the program's execution history.
break, brea, bre, br, b -- Set breakpoint at specified location.
break, brea, bre, br, b -- Set breakpoint at specified location.
--Type <RET> for more, q to quit, c to continue without paging--
```

使用 `help {command}` 可以查看某个具体命令的帮助文档：

```
(gdb) help break
break, brea, bre, br, b
Set breakpoint at specified location.
break [PROBE_MODIFIER] [LOCATION] [thread THREADNUM]
        [-force-condition] [if CONDITION]
PROBE_MODIFIER shall be present if the command is to be placed in a
probe point.  Accepted values are `-probe' (for a generic, automatically
guessed probe type), `-probe-stap' (for a SystemTap probe) or
`-probe-dtrace' (for a DTrace probe).
LOCATION may be a linespec, address, or explicit location as described
below.

With no LOCATION, uses current execution address of the selected
stack frame.  This is useful for breaking on return to a stack frame.

THREADNUM is the number from "info threads".
CONDITION is a boolean expression.

With the "-force-condition" flag, the condition is defined even when
it is invalid for all current locations.
A debugging session is active.
Linespecs are colon-separated lists of location parameters, such asr
source filename, function name, label name, and line number.
Example: To specify the start of a label named "the_top" in thereakpoints.
--Type <RET> for more, q to quit, c to continue without paging--
```

### 10. 输出日志 

``set logging`` 可以把 GDB 的所有输出打印到日志里，作进一步分析。

需要执行这两个命令：

```txt
(gdb) set logging file gdb.txt
(gdb) set logging on
copying output to gdb.txt
```

这样任何命令的输出便会写到 `gdb.txt`，前提是 shell 拥有该文件的写入权限。

配合以下命令，确保输出完整内容：

```txt
set print repeats 0       // 否则相同的连续字符会被合并
set print elements 0      // 否则过长的数组会被省略
set height 0              // 否则如果一页显示不完，会停下来要求 continue
set width 0  
```

### 11. 断点命令列表

可以通过 `commands` 命令给断点绑定一组自定义命令，当命中断点后会自动执行，如打印变量的值，或者设置另一个断点。

语法：先指定要绑定的断点编号，然后输入自定义命令，最后以 `end` 结束。例如：

```txt
（gdb) commands 1
（gdb) p foo
（gdb) end
```

断点编号可以通过 `i b` 或 `i wat` 获取。如果不给 `commands` 传入任何编号，则默认绑定到最近触发的断点上。

`commands` 的应用场景之一是收集信息。比如在某行代码后面插入一行 debug 日志，打印变量或调用栈。由于每次命中断点后，必须输入 `cond` 命令才会继续运行程序，因此可以在 `end` 前面加一个 `cont` 命令，这样程序便可以无需干预、自动运行：

```txt
(gdb) b foo.cpp:123
(gdb) commands
(gdb) p bar
(gdb) cont
(gdb) end
```

`commands` 的另一个应用场景是临时修复一个 bug，以便让程序正常运行。比如在某一行错误代码后面，给变量设置正确的值。同样要以 `continue` 命令结尾：

```txt
(gdb) b foo.cpp:123
(gdb) commands
(gdb) silent  // 这个命令后面的命令不会有任何输出
(gdb) set x = y + 4
(gdb) cont
(gdb) end
```

> [GDB - Breakpoint command lists](https://ftp.gnu.org/old-gnu/Manuals/gdb/html_node/gdb_34.html#SEC35)

### 12. 打印内存 `x`

`x` 可以查看一个内存地址的值，以指定的格式打印。

```gdb
(gdb) x/s 0x7ffc734ff250  // 以字符串形式打印
0x7ffc734ff250:	"hello,world"
```

x 支持的**格式化说明符**：

- `x/c`：将地址解释为字符 char，打印为字符

- `x/o`：使用八进制打印变量

- `x/x`：使用十六进制打印变量

- `x/u`：将地址解释为无符号整数 unsigned，使用十进制打印

- `x/s`：将地址解释为字符串

- `help x` 查看全部：

  ```txt
  o(octal), x(hex), d(decimal), u(unsigned decimal),
  t(binary), f(float), a(address), i(instruction), 
  c(char), s(string) and z(hex, zero padded on the left)
  ```

`x` 和 `p` 的区别：

- 传入一个数字，`p` 会当作一个数字字面量，输出原始值的十进制；而 `x` 会当作一个地址，输出对应内存区域的值。比如：

  ```txt
  (gdb) p 0x10    // 字面量
  $1 = 16  	      // 输出十进制值
  (gdb) p/x 0x10  // 以十六进制形式输出
  $2 = 0x10
    
  (gdb) x/s 0x10  // 这个内存地址解释为字符串
  0x10 "hello, world"    
  (gdb) x/c 0x10  // 把这个地址上的内容解释为单个字符
  0x10:	'h'
  (gdb) x/d 0x10  // 把这个地址上的内容解释为整数
  0x10:	104
  ```

- 传入一个指针，`p` 会输出指针的值，即一个十六进制地址；而 `x` 会输出指针指向的内存区域的值：

  ```txt
  （gdb) p str_pointer;
  $1 = 0x7ffc
    
  (gdb) x/s 0x7ffc
  0x7ffc "hello world"
  ```

`x` 的完整语法：`x/FMT ADDRESS`，`F` / `M` / `T` 是可选的参数。

- `F`：一个数字，表示输出几个内存单元，默认是 1
- `M`：格式化说明符，`o` / `x` / `d` / `u` / `s` 等
- `T`：一个内存单元的字节数，默认是 4 个字节，可选的是 b(byte), h(halfword), w(word), g(giant, 8 bytes)
- `ADDRESS`：一个内存地址，可以是一个字面量，也可以是一个指针类型的变量

> 例如，`x/3uh 0x1234` 表示从内存地址 0x1234 开始，以双字节为单位，输出 3 个无符号整数。

### 13. 打印各种信息 `i`

- `info locals`：打印当前栈帧的所有局部变量
- `info args`：打印所有函数参数
- `info threads`： 打印进程的线程信息
- `info registers`： 打印当前线程的寄存器信息
- `info sharedlibrary`：打印当前加载的动态连接库
- `info proc mappings`：打印地址空间中的内存 map，用来确定某个地址的类型
- `help info`：所有 info 支持的命令

### 14. 打印变量: `p`

#### 基本使用

`print` / `p` 可以打印一个变量的值，支持数字、字符串、结构体、指针等变量类型：

```txt
(gdb) p a // int a = 3;
$1 = 3
```

打印出来的值会存在名为 `$1`、`$2`、… 的变量里，后续可以直接复用：

```txt
(gdb) p $1 // 等价于 p a
$2 = 3
```

`p` 有一些可选参数：

- `-elements`：限制字符串或者数组打印的元素数量
- `-max-depth`：限制嵌套结构体的最大打印层数
- …，`help p` 查看所有参数

💡 `p` 可以打印当前栈帧和全局作用域内的变量。如果打印变量时提示变量已经 `optimized`，可以尝试用 `f` 切换到更上层的栈帧。

#### 打印指针

##### 指针变量

`p` 后面跟一个指针类型的变量，打印的是指针的值，即指针所指向的地址：

```txt
(gdb) p b // int* b = &a;
$1 = (int *) 0x7ffd3dcfa27c
```

可以用解引用运算符，打印指针指向的值：

```txt
(gdb) p *b
$2 = 1
```

如果是字符串指针，`p` 会同时输出*指针指向的地址*和*字符串的内容*：

```txt
p str
$3 = (char*) 0x7ffc734ff250 "hello,world"
```

如果希望只打印地址，可以使用说明符 `/a`：

```gdb
(gdb) p/a str
$4 = 0x7ffc734ff250
```

> `/a` 表示 `address`，即把变量的值以地址的形式打印。

##### 地址字面量

`p` 默认会把十六进制的字面量看成是数字，输出一个十进制的整数：

```txt
(gdb) p 0x7ffd3dcfa27c
$1 = 140725640471164
(gdb) p 140725640471164 == 0x7ffd3dcfa27c
$2 = true
```

如果想把数字解释为地址、打印地址上的内容，需要先指定变量类型，然后解引用：

```txt
(gdb) p *(int*)0x7ffd3dcfa27c
$3 = 1
```

更简单的语法是 `{TYPE}ADDRESS：`

```txt
(gdb) p {int}0x7ffd3dcfa27c
$4 = 1
```

也可以用 `x` 命令打印地址。

##### 转换指针类型

指针的类型可以转换，以不同方式解释其指向的内存区域：

```txt
// char* c = "hello, world";
(gdb) p c
$1 = (char *) 0x7ffc734ff250 "hello, world";
(gdb) p *(int*)c
$2 = 1819043176
(gdb) p {int}c
$3 = 1819043176
```

打印内存可以发现，`1819043176` 就是把 `h e l l` 四个字符解释成了一个整数：

```txt
(gdb) x/w 0x7ffc734ff250    // 以 word 形式打印，4 个字节
0x7ffc734ff250:	1819043176  // 上述 4 个字符的 ASCII 码转成整数
```

`1819043176` 对应的十六进制是 `0x6C6C6568`，恰好依次是 `l` , `l` , `e` 和 `h` 的 ASCII 码。

##### 打印结构体的字段

如果指针 `p` 指向某个结构体，可以用 `p ptr->field` 打印字段的值。

在 GDB 里，`.` 和 `->` 是一样的，所以无论 `ptr` 是否是指针，都可以用 `p.field` 打印字段的值。

#### 打印数组

语法：`p ELEMENT@LEN`。从 `ELEMENT` 的地址开始向后解释 `LEN` 大小的内存单元，内存单元的大小是 `sizeof(T)`。

##### 栈上数组

如果 array 是栈上数组，可以直接 `p array`，会打印数组的所有元素：

```plaintext
// int array[] = {1, 2, 3, 4};
(gdb) p array
$1 = {1, 2, 3, 4}
```

也可以 `p array[INDEX]@LEN`，从某个下标开始打印指定的长度：

```txt
(gdb) p array[1]@[3] // array[1] 的类型是 int
$2 = {1, 2, 3}
```

但不能 `p array@LEN`，因为栈上数组 array 的类型是 `int[4]` 而不是 `int`：

```txt
(gdb) p array@3
$3 = {{1, 2, 3, 4}, {-693741568, 32764, 1033857024, -1536906435}, {0, 0, -793505661, 32580}}
```

##### 堆上数组

如果 array 是堆上数组，可以 `p *array@LEN`：

```txt
// int* array = (int*)malloc(3 * sizeof(int));
(gdb) p *array@3 // *array 是数组的第一个元素，类型是 int
$1 = {1, 2, 3}
```

或者 `p array[INDEX]@LEN`，从某个下标开始打印：

```txt
(gdb) p array[1]@3 // array[1] 的类型是 int
$2 = {2, 3, 4}
```

但不能 `p array` ，因为堆上数组 array 的类型是 `int*` 指针，值是一个地址：

```txt
(gdb) p array
$3 = 0x55669a743eb0
```

也不能 `p array@LEN`，理由同上。array 是一个 `int*` 指针，保存在栈上，这里会输出栈上相邻内存的值，没有任何意义：

```txt
(gdb) p array@3
$4 = {0x55669a743eb0, 0x55669a255330, 0x200000001}
```

如果只有一个地址字面量，可以把它强制转换为指针类型，然后用同样的语法打印：

```txt
(gdb) p ((int*)0x55669a743eb0))[2]
$5 = 3
```

#### 格式化输出

可以在 `p` 后面添加说明符 (specifier)，把一个变量解释为给定的类型：

```txt
(gdb) p foo // int foo = 98;
$1 = 98
(gdb) p/c foo // 将 98 解释为字符
$2 = 98 'b'
```

所有说明符：

- `p/a`：将变量解释为指针 address，使用十六进制打印

- `p/c`：将变量解释为字符 char，打印为字符

- `p/o`：使用八进制打印变量

- `p/x`：使用十六进制打印变量

- `p/u`：将变量解释为无符号整数 unsigned，使用十进制打印

- `p/s`：将变量解释为字符串，打印输出

- `help x` 查看全部：

  ```txt
  o(octal), x(hex), d(decimal), u(unsigned decimal),
  t(binary), f(float), a(address), i(instruction), 
  c(char), s(string) and z(hex, zero padded on the left)
  ```

#### STL 容器

##### std::shared_ptr

直接打印：

```txt
// std::shared_ptr<int> ptr = std::make_shared<int>(1);
(gdb) p ptr
$1 = std::shared_ptr<int> (use count 1, weak count 0) = {
  get() = 0x5596169122f0}
(gdb) p *ptr
$2 = 1
```

或者根据上面 `get()` 方法给出的地址打印：

```txt
(gdb) p {int}0x5596169122f0
$3 = 1
```

或者根据 shard_ptr 内部的私有变量 `_M_ptr` 打印：

```txt
(gdb) p ptr._M_ptr
$4 = 0x5596169122f0
(gdb) p *(ptr._M_ptr)
$5 = 1
```

##### std::vector

直接打印：

```txt
// std::vector<int> vec = {1, 2, 3, 4};
(gdb) p vec
$1 = std::vector of length 4, capacity 4 = {1, 2, 3, 4}
```

vector 也有私有变量保存了数据的实际存储位置：

- `_M_impl._M_start`：数组起始地址
- `_M_impl._M_finish`：数组结束地址 (数组最后一个元素的下一个)

可以根据这个指针打印：

```txt
(gdb) p {int}vec._M_impl._M_start
$2 = 1
(gdb) p {int}vec._M_impl._M_start@3
$3 = {1, 2, 3}
(gdb) p ({int}vec._M_impl._M_start)[2]
$4 = 3
```

##### std::string

直接打印：

```txt
(gdb) p str
$1 = "hello,world"
```

或者根据私有变量 `_M_dataplus._M_p` 打印，其类型是 `char*`：

```txt
(gdb) p str._M_dataplus._M_p
$2 = (std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::pointer) 0x7ffc734ff250 "hello,world"
```

##### 使用插件 STL-Views

使用 GDB 直接打印 set、stack、map 等 STL 类型是十分困难的。GDB 支持使用 python 编写 printer。[GDB 官网](https://sourceware.org/gdb/wiki/STLSupport)提供了现成的 STL 容器的 printer，安装十分容易，开箱即用。

先下载源代码到 home 目录，如果终端不支持 VPN，可以网页里打开后复制内容，然后在 vim 里粘贴源代码：

```txt
$ wget https://sourceware.org/gdb/wiki/STLSupport?action=AttachFile&do=get&target=stl-views-1.0.3.gdb -O ~/stl-views-1.0.3.gdb
```

进入 gdb，加载插件，查看帮助：

```txt
(gdb) source ~/stl-views-1.0.3.gdb
(gdb) help pset
(gdb) help pmap
```

使用：

```txt
(gdb) pset s
(gdb) pset s int
(gdb) pset s int 20
```

#### 如果打印内容被省略

打印字符串的时候，如果有重复的字符，可能会被合并成一个：

```txt
(gdb) p "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
$1 = 'a' <repeats 30 times>
```

可以通过命令 `set print repeats 0` 设置为不合并：

```txt
(gdb) set print repeats 0
(gdb) p "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
$2 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
```

打印数组的时候，如果元素过多，中间的元素会被省略。可以通过以下设置为不省略：

```txt
set print elements 0
```

#### 查看历史变量

通过 `p` 打印出来的值会存在名为 `$1`、`$2`、… 的变量里 ([value history](https://sourceware.org/gdb/onlinedocs/gdb/Value-History.html#Value-History))，后续可以直接复用：

```txt
(gdb) p a
$1 = 123
(gdb) p $1 // 等价于 p a
$2 = 123
```

一些特殊的变量：

- `$`：最近打印的变量
- `$$`：`$` 之前的变量，倒数第二个
- `$$n`：最后一个变量往前的第 n 个变量，比如 `$$0` 就是 `$`， `$$1` 就是 `$$`

可以批量打印历史变量：

- `show values`：打印最后 10 个历史变量
- `show values +`：打印刚才打印过的历史变量的后 10 个历史变量

## TUI 调试

tui 调试是 gdb 自带的功能，ctrl+X+A 可以开启 TUI 模式，开启 TUI 模式后默认会有两个窗口，上窗口是当前执行位置的代码，下窗口是 gdb 环境

```gdb
(gdb) layout src   # 显示源代码窗口
(gdb) layout asm   # 显示汇编窗口
(gdb) layout regs  # 显示寄存器窗口
(gdb) layout split # 同时显示源代码和汇编
```

cgdb 可以看作 gdb 的界面增强版，用来替代 gdb 的 `gdb -tui`。cgdb 主要功能是在调试时进行代码的同步显示，这无疑增加了调试的方便性，提高了调试效率。界面类似 vi，符合 unix/linux 下开发人员习惯；如果熟悉 gdb 和 vi，几乎可以立即使用 cgdb。

## GDB 核心原理

GDB（GNU Debugger）的核心原理，可以概括成一句话：**<font color=blur>GDB 作为一个独立进程，通过操作系统提供的调试接口控制目标进程，暂停它、读取/修改它的内存和寄存器，并结合程序里的调试信息，把机器地址还原成源码中的变量、函数和行号。</font>**

以 Linux 为例，理解 GDB 最关键的是三个东西：**`ptrace`、断点、DWARF 调试信息**。

程序正常运行时，大致是：

```
源代码
  ↓ 编译
机器指令
  ↓
CPU 执行
```

使用 GDB 后变成：

```
                ┌───────────────┐
                │      GDB      │
                │               │
                │ 设置断点       │
                │ 读取寄存器     │
                │ 读取内存       │
                │ 单步执行       │
                └───────┬───────┘
                        │ ptrace 等系统接口
                        ↓
                ┌───────────────┐
                │ 被调试进程     │
                │               │
                │ CPU寄存器      │
                │ 虚拟内存       │
                │ 机器指令       │
                └───────────────┘
```

### 1. GDB 为什么能控制另一个进程？

Linux 内核提供了 `ptrace()` 系统调用。

GDB 启动一个程序时，通常会建立类似这样的关系：

```
GDB
 │
 │ fork()
 ↓
子进程
 │
 │ ptrace(PTRACE_TRACEME)
 ↓
execve("./a.out")
```

之后内核知道：这个程序正在被 GDB 调试。

所以当程序因为断点、信号、单步等原因停止时，内核会通知 GDB。

GDB 可以通过 `ptrace` 做很多事情，例如：

* 读取寄存器
* 修改寄存器
* 读取目标进程内存
* 修改目标进程内存
* 让程序继续执行
* 单步执行一条指令

因此：

```
(gdb) info registers
```

本质上就是 GDB 从内核那里获取目标进程的寄存器状态。

而：

```
(gdb) set $rax = 10
```

则是在**直接修改目标进程的寄存器**。

------

### 2. 断点到底是什么？

这是 GDB 最经典的实现机制。

假设机器代码是：

```
0x401000: push rbp
0x401001: mov rbp, rsp
0x401004: sub rsp, 0x20
```

你执行：

```
(gdb) break foo
```

GDB 会先找到 `foo` 对应的机器地址，比如：

```
foo → 0x401000
```

然后把该地址上的第一条机器指令的某个字节**临时替换成断点指令**。

在 x86/x86-64 上通常是：

```
INT3
```

机器码只有一个字节：

```
0xCC
```

因此原本：

```
0x401000: 55 ...
```

会变成：

```
0x401000: CC ...
```

CPU 执行到这里：

```
CPU 执行 INT3
      ↓
产生调试异常
      ↓
Linux 内核捕获
      ↓
目标进程停止
      ↓
通知 GDB
```

所以所谓：

```
Breakpoint 1, foo ()
```

并不是 GDB 一直盯着 CPU 问：

> “到 foo 了吗？到 foo 了吗？”

而是 **GDB 修改了程序代码，让 CPU 执行到这里时主动触发异常。**

暂停以后，GDB 还需要做一个小操作，因为原来的指令被覆盖掉了。

过程大概是：

```
原指令:
0x401000: 55


设置断点:
0x401000: CC


CPU 执行 CC
↓
停下来


GDB 恢复:
0x401000: 55


让 CPU 执行一次原指令


然后再次写回:
0x401000: CC
```

这就是 **<font color=blue>软件断点</font>**。

------

### 3. `next` 和 `step` 为什么能“单步”？

例如：

```
int x = foo();
x++;
```

执行：

```
(gdb) step
```

看起来像是“执行一行 C”。

但 CPU 根本不知道什么叫“一行 C”。

CPU 只知道：

```
mov
call
add
jmp
...
```

所以 GDB 真正做的是：

```
源码行
  ↓
DWARF 调试信息
  ↓
这一行对应哪些机器指令地址
  ↓
控制 CPU 执行到下一个源码位置
```

硬件本身也提供了单步机制。

以 x86 为例，`RFLAGS` 寄存器里面有一个：

```
TF = Trap Flag
```

设置：

```
TF = 1
```

之后 CPU 每执行**一条指令**就会产生一次调试异常。

于是：

```
(gdb) stepi
```

本质上接近：

```
设置 CPU 单步标志
↓
执行一条机器指令
↓
CPU 产生调试异常
↓
内核停止程序
↓
GDB 获得控制权
```

`step` / `next` 又在此基础上结合调试信息实现源码级单步。

特别是 `next`，遇到：

```
foo();
```

时并不是简单执行一条机器指令，因为：

```
call foo
```

会进入 `foo`。

GDB 通常会想办法让整个 `foo()` 执行完，然后在返回位置停下来，比如通过**临时断点**等方式实现。

------

### 4. GDB 为什么知道变量 `x` 在哪里？

例如：

```cpp
int main() {
    int x = 42;
}
```

你输入：

```
(gdb) print x
```

问题是 CPU 内存里其实根本不存在：

```
名字叫 "x" 的东西
```

运行时只有：

* 寄存器
* 地址
* 字节

例如编译器可能把 `x` 放在：

```
rbp - 4
```

也可能优化后放在：

```
eax
```

甚至优化掉。

GDB 能知道这些，是因为使用：

```
gcc -g main.c
```

后，编译器会生成 **<font color=blue>调试信息</font>**。

Linux 上最常见的是 <font color=blue>**DWARF**</font>。

它记录诸如：

* 函数 main 对应哪些地址
* 源码第 10 行对应哪些地址
* 变量 x 是 int
* 变量 x 当前位于 rbp-4
* 结构体 User 有哪些字段
* 函数参数是什么
* 调用栈如何展开

所以：

```
(gdb) print x
```

大致经历：

```
"x"
 ↓
查 DWARF
 ↓
发现 x 类型为 int
 ↓
发现当前 x 位于 rbp - 4
 ↓
读取 rbp 寄存器
 ↓
计算地址 rbp - 4
 ↓
读取目标进程内存
 ↓
把 4 个字节解释成 int
 ↓
显示 42
```

这也是为什么编译调试版本一般要加：

```cpp
gcc -g test.c
```

没有 `-g`，GDB 仍然可以：

```
disassemble
info registers
x/10gx ...
```

但很难知道源码变量叫什么。

------

### 5. 为什么优化后 GDB 经常显示 `<optimized out>`？

例如：

```bash
gcc -O2 -g test.c
```

编译器可能发现：

```cpp
int x = 10;
printf("%d", x);
```

根本没必要真的创建 `x`。

直接生成：

```assembly
mov esi, 10
call printf
```

于是运行时 x 根本不存在。

DWARF 最多告诉 GDB：这个源码曾经有一个变量 x。

但它现在已经没有一个稳定的存储位置。

于是：

```
(gdb) print x
$1 = <optimized out>
```

所以调试时经常使用：

```bash
gcc -g -O0
```

原因就在这里。

------

### 6. GDB 如何显示调用栈？

比如：

```
main()
  ↓
foo()
  ↓
bar()
  ↓
crash()
```

输入：

```
(gdb) backtrace
```

得到：

```
#0 crash
#1 bar
#2 foo
#3 main
```

GDB 本质是在做 **<font color=blue>stack unwinding（栈展开）</font>**。

函数调用通常会留下：

* 返回地址
* 栈指针
* 保存的寄存器

经典情况下：

```
高地址


main 栈帧
────────────
foo 返回地址
foo 栈帧
────────────
bar 返回地址
bar 栈帧
────────────
当前 rsp


低地址
```

GDB 根据：

```
RIP
RSP
RBP
DWARF unwind information
```

不断计算：

* 当前函数是谁？
* 调用它的函数在哪里？
* 那个函数又是谁调用的？

最终得到 backtrace。

现代编译器经常开启：

```bash
-fomit-frame-pointer
```

因此不一定能够单纯依赖 `rbp`，GDB 还会使用 ELF/DWARF 中的 unwind 信息，例如 `.eh_frame`。

------

### 7. `watch` 为什么能监控变量变化？

例如：

```
(gdb) watch x
```

GDB 可以利用 CPU 的**<font color=blue>硬件调试寄存器</font>**。

x86 提供：

```
DR0
DR1
DR2
DR3
```

可以让 CPU 监控某个地址：

```
如果有人写这个地址
→ 产生调试异常
```

所以：

```
x 地址 = 0x7fffffffe12c


GDB 设置硬件 watchpoint
        ↓
CPU 正常执行
        ↓
某条指令修改 0x7fffffffe12c
        ↓
CPU 产生 debug exception
        ↓
程序暂停
        ↓
GDB 告诉你 Old value / New value
```

因此硬件 watchpoint 通常非常高效，但数量有限。

------

### 8. `attach PID` 又是什么原理？

比如：

```bash
gdb -p 1234
```

此时程序本来就在运行。

GDB 会使用类似：

```cpp
ptrace(PTRACE_ATTACH, 1234, ...)
```

让内核：

```
暂停 PID 1234
↓
将它置于可被 GDB 调试的状态
```

之后一样可以：

```
(gdb) bt
(gdb) info registers
(gdb) break foo
(gdb) continue
```

现代 GDB 也可能使用更现代的 `PTRACE_SEIZE` 等机制，具体行为会随平台和场景变化，但核心思想一样。

------

如果把这些机制串起来，GDB 的整体原理其实非常清晰：

```
                 编译阶段
                   │
           gcc -g / clang -g
                   │
        ┌──────────▼──────────┐
        │      ELF 文件       │
        │                     │
        │ 机器代码 .text      │
        │ 符号表              │
        │ DWARF 调试信息      │
        └──────────┬──────────┘
                   │
                   ▼
┌────────────┐   ptrace   ┌──────────────┐
│    GDB     │◄──────────►│ Linux Kernel │
└─────┬──────┘            └──────┬───────┘
      │                           │
      │ break                     │
      │ continue                  ▼
      │ registers          ┌──────────────┐
      │ memory             │ debuggee     │
      └───────────────────►│              │
                           │ CPU寄存器     │
                           │ 内存         │
                           │ 机器指令     │
                           └──────────────┘
```

## `ptrace()`

linux系统上有一个非常厉害的系统调用：**ptrace()**，在 linux 上的声明是这样子的

```cpp
extern long int ptrace (enum __ptrace_request __request, ...) __THROW;
```

- `long ptrace(request, pid_t pid, void *addr, void *data)`

- - `request`: 宏定义，比如说 [PTRACE_TRACEME](https://zhida.zhihu.com/search?content_id=646266275&content_type=Answer&match_order=1&q=PTRACE_TRACEME&zhida_source=entity), PTRACE_PEEKDATA 等，表示一个动作
  - `pid` 就是一个进程的进程号
  - `addr` 是地址
  - `data` 是一个数据，写入动作的话，就是将这个数据写入到这个地址上，如果是读出，那么会读到这个地址上去。

他可以接管一个子进程的执行，就是跟踪一个进程，linux 上的进程产生差不多是这样子：父进程先 `fork()` 一遍产生一个几乎一摸一样的自己，然后再 `execl()` 一个新进程，这时候就会将 `fork` 出来的进程替换成要执行的进程。使用 `ptrace()` 这个命令就可以跟踪这个新产生的进程。并且进行调试。代码差不多是这样子的：

```cpp
#include <sys/ptrace.h>
#include <unistd.h>
#include <sched.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        return -1;
    }

    std::string program_name { argv[1] };
    pid_t pid = fork();

    // 子进程
    if (pid == 0) {
        // 设置跟踪状态
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        execl(program_name.c_str(), program_name.c_str(), nullptr);
    }
    // 父进程
    if (pid > 0) {
        // 进行调试等操作
    }
}
```

## 断点

我觉得断点的原理跟修改代码差不多，使用 `ptrace()` 这个无敌的函数调用可以对函数的汇编代码进行一定的修改，在 x86 上有一条汇编指令，叫做 **[INT 3](https://zhida.zhihu.com/search?content_id=646266275&content_type=Answer&match_order=1&q=INT+3&zhida_source=entity)（**这里的 INT 不是 int，而是 interrupt 的缩写），当出现这个指令的时候，程序就会发生中断，也就是 trap 了，然后父进程就可以捕获这个信号，子进程就会中断在那里，等待下一步的命令，也就是说当 [gdb](https://zhida.zhihu.com/search?content_id=646266275&content_type=Answer&match_order=1&q=gdb&zhida_source=entity) 进行打断点的时候，就是将一条汇编代码修改一下，将其修改成 INT 3 的机器码，而 INT 3 的机器码是多少呢？是 0xcc, 所以说当打断点的时候，就会将原来的汇编代码对应的机器码中的机器码指令换成0xcc，当程序执行到一步的时候，就会触发断点，等到调试，那如何跳出这个断点？将要被替换的机器码指令保存下来就可以了，等到执行的时候将其重新替换上去就可以了（相当于恢复现场）。

ptrace 一次是四个字节的修改，但是 0xcc 是一个字节，你就需要修改四字节的低位进行处理，然后替换的时候就是将其替换到低位（也就是末位)，替换到低位是因为大部分机器是小端存储，等到一执行到这里的时候就会立马发生中断，等到恢复的时候，不仅要恢复现场（将修改的数值修改到原来的位置上），并且要将寄存器的PC指针 - 1（pc寄存器向上递增的，这里要将寄存器的数值 -1 就是0xcc之前的那个位置，然后继续执行，执行到下一个断点）

打断点的本质上是在机器码上进行打断点的，那么如何知道机器码和你写的代码之间的关系呢，有一个调试信息文件，也就是 [dwarf文件](https://zhida.zhihu.com/search?content_id=646266275&content_type=Answer&match_order=1&q=dwarf文件&zhida_source=entity)，dwarf 上会记录上一些相关的信息，如函数的 low_pc() 和 high_pc()，（low_pc() 就是最低的那个地址，是函数开头，可以通过 [objdump](https://zhida.zhihu.com/search?content_id=646266275&content_type=Answer&match_order=1&q=objdump&zhida_source=entity) 这个工具反汇编得到的信息和 dwarf 文件上的信息对比一下，刚好是对应上的）函数名所在的机器码指令在哪，都可以找到，所以说在 gdb 上就可以直接进行在函数名上打断点了。

ptrace() 能干的事情有很多，具体可以查一查手册，其他原理如 bt（gdb 打印出函数调用）等，涉及到了函数栈帧的调用，保存在寄存器上，ptrace() 也可以直接看寄存器上保存的数值，然后读取出来，做一些有用的事情。

后续更新一下变量的位置和大小的信息，这个很复杂。因为数据在变量中可能存在不同的地方，比如说在内存里面，在寄存器上，或者说编译器能够开一些优化直接将数据优化掉。更甚至有些对象象数据甚至有可能一半是在寄存器中，一半还在内存中，这时候读取并且修改就高了。）dwarf 文件也记录了这些，但是比前面的那个要复杂的多了，当然，这些都是一些基础原理，平常进行 C/C++ 调试，用 gdb 就行了，

这里再更新一下评论区里面的 0xcc 烫烫烫的梗（如果说你写上这样的代码，输出就有可能出现烫烫烫的字样，就是因为在 x86 上 int3 是软中断的命令，没有初始化的内存会用 0xcc 来填补，std::cout 需要最后的 \0 来表示这个 char s[] 的结束，结果没有，就将 0xcc 的字样输出上去了，结果就成了烫烫烫的字样了（  下面是VS输出的图片

![img](https://pica.zhimg.com/80/v2-dc67c56c39c9e2aa88cd5b6e1308ffa1_720w.webp?source=2c26e567)

> 直白地说，GDB 就像是一个“提线木偶的操纵者”，而你的程序就是那个木偶。GDB 之所以能掌控程序，核心在于操作系统提供了一根“提线”以及一套“偷梁换柱”的机制。
>
> 它的底层原理主要靠以下 4 个核心点：
>
> 1. **核心权限：`ptrace` 系统调用** 在 Linux 下，GDB 主要是通过 `ptrace` 这个系统调用来“附身”到你的程序上的。一旦附身成功，GDB 就获得了最高权限，可以直接读取和修改你程序的内存、CPU 寄存器，还能拦截程序的各种运行状态。
> 2. **断点原理：偷梁换柱 (`int 3`)** 当你在某一行打断点时，GDB 会悄悄把那行代码在内存里的真实机器指令保存起来，然后替换成一个特殊的**中断指令**（比如 x86 架构下的 `0xCC`，也就是 `int 3`）。 当程序傻乎乎地运行到这里时，CPU 读到这个特殊指令会触发异常，操作系统随即暂停程序，并发送一个 `SIGTRAP` 信号通知 GDB：“你的程序跑到断点停下了，交给你接管”。
> 3. **单步与恢复：放回去再拿出来** 当你输入 `n` (next) 或 `c` (continue) 准备往下走时，GDB 会把刚才偷偷换掉的真实指令“塞”回内存，让程序执行原本该执行的那条代码。执行完这一步后，如果需要继续拦截，GDB 又会迅速把中断指令替换回去。
> 4. **代码映射：调试符号 (DWARF)** CPU 在跑的时候根本不认识变量 `a`，也不知道什么是“第 14 行”，它只认识内存地址。你能看到具体的代码和变量，是因为你编译时加了 `-g` 参数，生成了调试信息（通常是 DWARF 格式）。这就相当于一张“地图”，GDB 每次拿到内存地址，都会去查这张地图，翻译成人能看懂的变量名和行号展示给你。

## FAQ

### 1. `step` 和 `next`

- `step`：进入当前行调用的函数
- `next`：把当前行执行完，但不进入函数体

例如：

```cpp
z = add(x, y);
```

在这一行：

- 用 `step` 会进 `add`
- 用 `next` 会直接算完，停到下一行

### 2. `print` 和 `x`

- `print` 更适合查看“有类型的变量”
- `x` 更适合直接看“原始内存”

比如：

```bash
p x
p *node
x/16bx buffer
x/s str
```

------

### 3. `backtrace` 和 `info locals`

- `backtrace` 看“怎么走到这里的”
- `info locals` 看“当前这一层的局部变量是什么”

两者经常一起用。

### 4. LLDB

GDB 是 GNU 体系的经典调试器。

LLDB 是 LLVM 体系的调试器，在 macOS 和 Clang 生态里常见。

大体上：

- Linux 上 GDB 很常见
- macOS 上 LLDB 更常见
- 两者核心思想类似：断点、单步、栈、变量、内存

### 5. GDB 调试的三种方法

1. `gdb filename`：这是最基础的用法，主要用于开发阶段的边写边调。确保代码编译时带上了 `-g` 参数，然后直接让 GDB 带着你的程序跑起来。
2. `gdb attach pid`：如果你的程序（比如后台服务）已经在跑着，突然出现死锁或卡顿，且不能随便重启，就可以用这种方法。找出进程的 PID，让 GDB 强行“插队”接管它，查看当前状态。
3. `gdb filename corename`：如果程序已经崩溃死掉了（比如发生了段错误），系统通常会留下一个包含崩溃瞬间内存状态的 Core 文件。把程序和 Core 文件一起喂给 GDB，就能精准还原案发现场。

> 当你使用 GDB 附加（比如使用 `gdb -p <进程ID>` 或在 GDB 内输入 `attach <进程ID>`）到一个正在运行的程序时，目标程序会被瞬间“冻结”。
>
> 这个过程的核心细节如下：
>
> - **强行踩下刹车：** 附加成功的一瞬间，GDB 会通过操作系统的 `ptrace` 机制向目标进程发送一个暂停信号（`SIGSTOP`）。系统接到信号后，会立刻挂起该进程，让它停在当前正在执行的那条机器指令上。
> - **保护案发现场：** 程序被挂起后，它的内存数据、CPU 寄存器状态和各个线程的运行位置都是完全静止的。这就为你创造了一个绝对安全的排查环境，你可以从容地查看当前变量、打印调用栈（`bt`），或者在前面的代码里补充打上断点。
> - **如何恢复自由：** 只要你在 GDB 里不输入 `continue`（简写 `c`）让它继续跑，它就会一直停着。排查结束后，如果你输入 `detach` 命令，GDB 就会撤销控制权，程序会立刻从刚才暂停的地方接着往下运行，就好像什么都没发生过一样。

### 6. 自定义 core 文件名称

core 文件的默认路径和名称为当前工作目录下生成的 "core"，通过如下方式可修改 core 的目录和文件名称：

```bash
echo "/root/test/core-%e-%p-%t" > /proc/sys/kernel/core_pattern
```

- %p->pid
- %u->uid
- %g->gid
- %s->signal
- %t->timestamp
- %h->hostname
- %e->exectable name

### 7. `~/.gdbinit`

像 `~/.vimrc`、`~/.zshrc` 一样，GDB 也有默认的配置文件 `~/.gdbinit`。可以把一些常用的配置、插件、自定义命令放在 `~/.gdbinit`。

Github 上有一些开箱即用的 `~/.gdbinit` 文件：

- https://github.com/gdbinit/Gdbinit/blob/master/gdbinit
- [gdb-dashboard](https://github.com/cyrus-and/gdb-dashboard)：可视化界面、丰富的功能
- [gef](https://github.com/hugsy/gef)：可视化界面、丰富的功能
- [pwndbg](https://github.com/pwndbg/pwndbg)

gdb-dashboard 使用笔记：

- 使用 `-output` 命令将某些组件在其他终端显示，比如终端 A 执行 gdb 命令，终端 B 显示断点、变量值、调用栈。在终端输入 `tty` 命令就可以查看当前终端的序号。
- 介绍文章：https://zhuanlan.zhihu.com/p/435918702

```bash
# 显示漂亮的打印
set print pretty on

# 设置历史记录大小
set history save on
set history size 1000

# 自定义命令
define printarray
    set $i = 0
    while $i &lt; $arg0
        printf &quot;array[%d] = %dn&quot;, $i, $arg1[$i]
        set $i = $i + 1
    end
end
```

### 8. 声明语句的特殊情况

像 `int i;` 这种纯粹的变量声明（没有赋初值），编译器往往不会为其生成专门的执行指令（只是在底层预留了内存空间）。在实际调试中，GDB 会聪明地把断点自动顺延到下一条有实质动作的代码（例如 `long result = 0;`），并停在它的执行前。

### 9. 断点在何处停止？

我们对 `int val = 10;` 这条语句加了断点，然后执行 `run` 命令，那么 gdb 是在执行这条语句前停止，还是在执行这条语句后停止？

<font color=blur>**断点的本质是“拦截”**：当你在某一行打下断点并 `run` 到那里时，GDB 会在程序**即将**执行该行指令的那一刻将其冻结。**此时，这行代码的实际动作还没有发生。**</font>

### 10. Core Dump

<font color=blur>当进程崩溃时，操作系统会把进程当前的所有内存和寄存器状态信息保存到 core dump 文件中。</font>Core dump file 是一个二进制文件，需要配合 debug info 来赋予其含义。GDB 可以读取 core dump 文件，协助分析进程崩溃的瞬间发生了什么。

可能会产生 core dump 文件的场景：

- 段错误 Segmentation Fault
  - Null Pointer Dereference (NPD)
  - Stack Overflow / Buffer Overflow
  - Use After Free (UAF)
  - Double Free
  - Out Of Memory (OOM)
- 其他一些会引起 core dump 的 signal

https://cgi.cse.unsw.edu.au/~learn/debugging/modules/gdb_coredumps/

### 11. Debug Info

Debug 是编译器生成的调试用的符号表，保留了源代码的信息，如标识符名称、可执行文件中第几条机器指令对应源代码的第几行等，但并不是把整个源文件嵌入到可执行文件中。

<font color=blur>gcc 或 g++ 在编译时，可以通过 `-g` 选项生成 debug info。</font>如果没有 debug info，GDB 就无法按源码行打断点、输出变量的值、或者展示 coredump 文件中的调用栈信息。

<font color=blue>DWARF（Debugging With Attributed Record Formats）是现在操作系统 debug info 的主要标准。</font>Debug info 保存在程序 ELF 文件的 `.debug_info` 段中。

> [The GDB developer’s GNU Debugger tutorial, Part 2: All about debuginfo](https://developers.redhat.com/articles/2022/01/10/gdb-developers-gnu-debugger-tutorial-part-2-all-about-debuginfo)

### 12. 栈帧

调用栈 (call stack) 被分成若干个栈帧 (stack frame)，每个栈帧包括和一次函数调用相关的所有数据：函数的参数、函数的局部变量、以及函数的返回地址等。

程序启动时只有一个栈帧，即 main 函数，又称初始栈帧或最外层栈帧。每次函数调用都会创建一个新的栈帧，每次函数返回时一个栈帧也会被弹出。当前执行的函数所对应的栈帧又称最内层栈帧。

GDB 给每个栈帧分配了一个数字，最内层栈帧的编号是 0，外层栈帧依次加 1。可以通过 `bt` 命令展示所有栈帧，通过 `f` 命令加上编号进入到对应的栈帧。

### 13. DWARF

DWARF 的全称是 **Debugging With Attributed Record Formats**（带属性记录格式的调试信息）。

这里面其实藏着一个非常经典的极客幽默：

* Linux 系统下标准的程序可执行文件格式叫做 **ELF**（Executable and Linkable Format）。在奇幻文学（比如《魔戒》或者各种 RPG 游戏）里，精灵（Elf）和矮人（Dwarf）通常都是结伴出现的。
* 既然程序本体叫 ELF（精灵），那给它做配套支持的调试信息，自然就被大佬们顺理成章地命名为 DWARF（矮人）了。
* 简单来说，DWARF 就是专门给调试器（比如 GDB）看的一张**“翻译地图”**。

CPU 只懂内存地址和机器指令，完全不知道你代码里的变量叫 `a`，也不知道哪段内存对应你源码的“第 14 行”。DWARF 的核心作用，就是把“底层机器态”翻译回“人类源码态”。

它的核心主要体现为以下 3 点：

1. **源码与地址的映射：** 它记录了每一段机器码的内存地址，对应着你哪个文件（比如 `main.c`）的第几行。这就是为什么你在 GDB 里敲 `n`，它能精准停在下一行源码。
2. **变量与内存的追踪：** 它详细描述了程序里所有变量的名字、类型（比如 `int` 占几个字节，某个 `struct` 内部的布局），以及运行时这个变量到底是存在哪个寄存器里，还是在栈空间的哪个偏移量上。
3. **它的存在形式：** 当你敲下 `gcc -g` 编译时，编译器就会生成这套 DWARF 格式的数据。它默认会被直接塞进最终的可执行文件（ELF）内部某个特定的段（如 `.debug_info`）里。它完全不参与程序的实际运行，只是默默占点磁盘空间。

因为包含的信息太细致，带 DWARF 信息的程序体积往往会大好几倍。所以在生产环境中，大家通常会用 `strip` 命令把这段信息剥离出去，或者单独存成额外的 `.debug` 文件。

## 场景题

### 1. 线上服务突然崩溃产生 Core Dump 退出，怎么排查

遇到线上服务崩溃产生 Core Dump，别慌。排查的核心思路就是“找现场、看堆栈、查变量”。只需掌握以下 4 个最关键的步骤：

**1. 加载 Core 文件** 将带有调试信息的程序可执行文件和 core 文件放在一起，启动 GDB 还原案发现场：

```bash
gdb ./your_program ./core_file
```

如果线上的包去掉了调试符号（strip），最好能在本地或测试机找一份代码版本完全对应的、带符号的未裁剪文件来加载。

**2. 找准崩溃堆栈** 进入 GDB 后，第一件事就是输入 `bt`（或者 `bt full` 查看包含局部变量的详细信息），这能直接打印出崩溃瞬间的函数调用链。 浏览输出，找到属于你业务逻辑的那一行代码，用 `f 帧号`（例如 `f 3`）直接切进对应的函数调用层。

**3. 揪出异常变量** 切到案发层级后，开始顺藤摸瓜。输入 `info locals` 可以一览当前所有的局部变量；或者直接用 `p 变量名`（比如 `p *my_ptr`）查看某个重点嫌疑变量的具体值。 大部分的空指针引用（SIGSEGV）或数组越界，在看到变量值是 `0x0` 或者离谱的脏数据时就能破案了。

**4. 多线程全景排查** 线上服务基本都是多线程，有时候崩溃的连锁反应不在当前线程。 先用 `info threads` 扫一眼所有线程状态，带 `*` 号的是当前触发崩溃的线程。如果怀疑是并发乱序或死锁，直接上大招 `thread apply all bt`，把所有线程的堆栈全打出来排查。

### 2. 多线程环境下怎么排查死锁

排查死锁听起来让人头大，但其实就是个“抓现行”的活儿。死锁发生时，程序的特征往往很明显：进程还在，CPU 占用率可能反而变低了，但业务就是卡死没响应。

抓住核心，排查只需要这 3 步：

**1. 锁定“案发现场”（找阻塞线程）** 首先得把停滞的线程揪出来。

- **C/C++（用 GDB）：** 直接附加到目标进程 `gdb -p 进程ID`，然后一键打印所有线程堆栈 `thread apply all bt`。视线重点扫过那些停留在 `__lll_lock_wait`、`pthread_mutex_lock` 或 `std::mutex::lock` 的线程。
- **Java：** 最省事，直接执行 `jstack 进程ID`。JDK 往往自带死锁检测，如果有死锁，日志底部通常会直接打出 `Found one Java-level deadlock:`，把谁在等谁的锁全给你列出来。

**2. 顺藤摸瓜找“锁”的主人** 如果你看堆栈发现线程 A 被卡在了某把锁上，下一步就是查这把锁现在到底在谁手里。 在 GDB 中，切到那个被阻塞的线程，找到卡住的那一帧，直接打印那个互斥锁变量（例如 `p my_mutex`）。 重点盯住锁结构体里的 `__owner` 字段（在 Linux glibc 中通常有这个字段），这个数字就是当前死死捏着锁不放的线程 ID（LWP）。

**3. 对簿公堂审查代码** 拿着上一步找到的 `__owner` ID，去 `info threads` 列表里对号入座，找到这个“占着茅坑”的线程。 切进它的堆栈看看它在干嘛：它大概率也是停在了另一个等锁的调用上，这就形成了经典的“交叉拿锁”（你等我的锁 1，我等你的锁 2）。 结合这两边的调用链切回代码，梳理它们各自的加锁顺序。排查到这里，只要把代码里的加锁顺序强行改成一致，或者引入超时机制（try_lock），死锁也就破了。

### 3. 如果发现某个关键对象的状态值经常被莫名其妙地篡改，导致连接断开，常规断点很难抓到，你该怎么排查？

常规断点是守株待兔，防的是“代码执行”，抓这种变量被暗中篡改的“内鬼”，得直接盯死“数据”。在 GDB 里，这叫**硬件数据断点（Watchpoint）**。

核心排查就这 3 步：

**1. 锁定目标的内存地址** 先在程序初始化这个关键对象的地方打个临时断点，或者在它还没被篡改前把程序暂停。 打印出那个状态变量的绝对内存地址，比如 `p &obj->status`，假设拿到的是 `0x7fff1234`。

**2. 挂上硬件观察点** 拿到地址后，直接对这块内存下硬件监控：

```
watch *(int*)0x7fff1234
```

（强转成 `int*` 是为了明确告诉 GDB 盯住这 4 个字节）。只要有任何代码（不管在哪个线程、哪个犄角旮旯）试图往这个物理地址写入数据，CPU 级别的硬件中断就会直接把程序按停。

**3. 抓现行查堆栈** 输入 `c` 让程序继续狂奔。当那次“莫名其妙”的篡改发生瞬间，GDB 会立刻停住，并在屏幕上打印出旧值和新值。 这时候毫不犹豫地输入 `bt`，调用堆栈会清清楚楚地告诉你，究竟是哪个函数的哪行代码动了你的变量。

------

如果篡改它的不是常规逻辑错误，而是严重的内存越界（比如旁边的数组写穿了顺带覆盖了它），堆栈信息可能会显得比较诡异。你们团队在线上或者测试环境会常态化开启 ASAN（AddressSanitizer）这类内存溢出检测工具吗？

### 4. 怎么让程序在 GDB 里带参数跑起来？如果调试完了想让运行中的进程继续服务，该怎么做？

#### **带参数跑起来**

最省事的做法有两种，看你习惯哪个：

- **在外面带参启动（推荐）：** 直接用 `--args` 把程序和参数全包进去。

  ```bash
  gdb --args ./your_program arg1 arg2
  ```

  进去 GDB 后，直接敲 `r` (run) 程序就带参跑起来了。

- **在里面敲定参数：** 先 `gdb ./your_program` 进去，然后用 `set args` 喂参数，再跑。

  ```bash
  (gdb) set args arg1 arg2
  (gdb) r
  ```

  *注：其实也可以直接暴力点，进去后敲 `r arg1 arg2`。*

#### **调试完让进程“刑满释放”**

如果你排查的是线上运行中的服务（通常是用 `gdb -p PID` 挂上去的），查完一定要正确退出，否则进程会被直接干掉或者一直冻结。

- **第一步：主动脱离** 在 GDB 命令行里敲 `detach`。这会让 GDB 松开对该进程的控制权，进程会立刻原地满血复活，继续处理业务。
- **第二步：安全退出** 看到系统提示 detached 成功后，再敲 `q` (quit) 离开 GDB。此时你的调试动作对线上服务就是完全无损的。

## 参考资料

- GDB 官网：https://sourceware.org/gdb/
- [Debugging with GDB](https://sourceware.org/gdb/onlinedocs/gdb/)
- [gdb debug full examples](https://www.brendangregg.com/blog/2016-08-09/gdb-example-ncurses.html)
- [100个 GDB 小技巧](https://wizardforcel.gitbooks.io/100-gdb-tips/content/index.html)
- [https://pernos.co](https://pernos.co/)：在线 GDB 平台
- [【Linux】GDB 入门笔记](https://imageslr.com/2023/gdb)
- [Linux 下常用的程序调试器 gdb 是什么原理？](https://www.zhihu.com/question/578172542/answer/3389041105)
- [GDB - Core Dumps](https://cgi.cse.unsw.edu.au/~learn/debugging/modules/gdb_coredumps/)
- [The GDB developer’s GNU Debugger tutorial, Part 2: All about debuginfo](https://developers.redhat.com/articles/2022/01/10/gdb-developers-gnu-debugger-tutorial-part-2-all-about-debuginfo#where_is_debuginfo_stored_and_how_can_you_get_it_)
- [gdb调试---多进程、线程](https://zhuanlan.zhihu.com/p/666440042)
