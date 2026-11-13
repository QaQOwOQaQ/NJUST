# FAQ

## 1. 底层文件 I/O 与 ANSI 标准 I/O

底层文件 I/O 操作（系统调用）与 ANSI 标准 I/O 操作（C 标准库）在实现方式、性能特性和使用场景上有显著区别。以下是两者的详细对比：

| **特性**     | **底层文件 I/O（系统调用）**                               | **ANSI 标准 I/O（C 标准库）**                 |
| :----------- | :--------------------------------------------------------- | :-------------------------------------------- |
| **实现层级** | 操作系统提供的原生接口（如 Linux 的`open`/`read`/`write`） | C 标准库（如`fopen`/`fread`/`fprintf`）       |
| **头文件**   | `<unistd.h>`, `<fcntl.h>`                                  | `<stdio.h>`                                   |
| **函数示例** | `open()`, `read()`, `write()`, `close()`                   | `fopen()`, `fread()`, `fprintf()`, `fclose()` |

**系统 IO（read/write）** 这是操作系统直接提供的系统调用。每次调用都会不可避免地 <font color=24b3aa>触发</font>（从用户态切换到内核态）。虽然它直接操作内核缓冲区，但如果你的程序在**<font color=24b3aa>频繁读写小块数据</font>**，频繁切换状态的开销会非常大，严重拖慢程序性能。

**标准 IO（fread/fwrite）** 这是 C 标准库在系统 IO 之上做的一层封装，<font color=24b3aa>核心目的就是。</font><font color=ee11ff>它在用户态自己维护了一个缓冲区（User Buffer）：</font>

- **写数据：** `fwrite` 会先把数据攒在用户态缓冲区里，等攒满了（或者程序结束、手动 `fflush` 时），才在底层调用一次 `write`，把数据批量推给内核。
- **读数据：** `fread` 会在底层调用 `read` 一次性从内核搬一大块数据到用户态缓冲区，程序接下来的读取就直接从这块内存拿，速度极快。

无论是内核缓冲区还是用户缓冲区，都是内存空间。核心区别不在于硬件物理介质，而在于**<font color=24b3aa>操作系统的权限隔离</font>**（虚拟内存空间的划分）：

- **用户缓冲区（User Buffer）**：存在于**用户空间**。这是你的程序自己分配的内存（比如 C 语言里 `malloc` 或者数组开辟的一块空间）。你的代码可以直接、自由地读写它，速度极快，没有权限阻挡。
- **内核缓冲区（Kernel Buffer）**：存在于**内核空间**（常被称为 <font color=24b3aa>Page Cache）</font>。这是操作系统专属的内存区域。普通程序**绝对禁止**直接访问这里，必须通过系统调用（如 `read`/`write`），让操作系统作为“代理人”，帮你把数据在这两块内存之间来回拷贝。

以读取文件为例，数据实际上经历了两次搬运： 硬盘 ➡️ *（硬件 DMA 自动搬运）* ➡️ **内核缓冲区（内存）** ➡️ *（CPU 拷贝，跨越特权级）* ➡️ **用户缓冲区（内存）** ➡️ 你的程序。

**<font color=blue>1、缓冲机制</font>**

| **特性**     | **底层 I/O**                                 | **标准 I/O**                                     |
| :----------- | :------------------------------------------- | :----------------------------------------------- |
| **缓冲**     | 无缓冲（直接调用系统调用）                   | 带用户态缓冲（减少系统调用次数）                 |
| **缓冲类型** | 依赖内核的页缓存                             | 支持全缓冲、行缓冲、无缓冲（通过`setvbuf` 设置） |
| **数据同步** | 立即写入内核（需手动调用`fsync` 同步到磁盘） | 缓冲满或调用`fflush` 时写入                      |

**<font color=blue>2、性能</font>**

到涉及到小文件的频繁读写时，底层 I/O 需要频繁的进行系统调用，做用户态和内核态之间的转换，性能开销较大；标准 I/O 用户用户态缓冲区可以显著压缩系统调用的次数。

对于大文件的读写，底层 I/O 和标准 I/O 差别不大，甚至在读写大文件时，**底层 IO 反而更好用、更高效**。因为标准 I/O 引入了用户态缓冲区，也就多了一步内核缓冲区到用户缓冲区的拷贝，这就增加了 CPU 开销和内存占用。不过真涉及到了超大文件（几个 G 级别的），我们一般会使用 `mmap`  把内核缓冲区（Page Cache）映射到用户空间，避免掉内核缓冲区到用户缓冲区的拷贝。

**<font color=blue>3、功能与灵活性</font>**

| **特性**       | **底层 I/O**                     | **标准 I/O**                                       |
| :------------- | :------------------------------- | :------------------------------------------------- |
| **文件描述符** | 使用整数文件描述符（如`int fd`） | 使用`FILE*` 文件指针                               |
| **格式化 I/O** | 不支持（需手动转换数据类型）     | 支持（如`fprintf`、`fscanf`）                      |
| **错误处理**   | 通过`errno` 和返回值检查         | 通过`ferror`/`feof` 检查                           |
| **线程安全**   | 需自行加锁                       | **默认线程安全**（如 `fgetc_unlocked` 需手动控制） |

C 标准库会在内部为每一个文件流（`FILE *`）偷偷维护一把锁。当多个线程同时调用 `fread`、`fwrite` 或 `printf` 操作同一个文件时，标准库会自动帮你排队（加锁、执行、解锁）。因此，单次函数调用的数据是完整的，不会出现两个线程的字节在底层被切碎拼成乱码的情况。

**但在实际开发中，需要注意这两个“坑”：**

1. **内容依然会交错：** 底层的锁只能保证“单次函数调用”不被打断。如果线程 A 和线程 B 都在连续调用多次 `fwrite` 写日志，最终文件里的内容可能是 A 的第一句、B 的第一句、A 的第二句……逻辑上依然是乱杂的。要想保证一段完整的业务逻辑不被插队，你还是得自己在应用层加锁。
2. **隐藏的性能开销：** 每次调用标准 IO 都要自动加锁解锁，这在高并发场景下是一种浪费。如果你已经在代码外层加了业务锁，或者确定某个文件只被一个线程独占，可以使用带 `_unlocked` 后缀的无锁版本（比如 `fread_unlocked`），或者用 `flockfile()` / `funlockfile()` 手动控制锁的范围，把多次小读写打包在一次锁里完成。

在 C++ 里，虽然 C++11 保证了 `cout` 本身是线程安全的（程序不会崩溃），但我们平时写代码，通常是连着写好几个 `<<`，比如：

```cpp
// Thread1
cout << "Thread " << id << " is running.\n";
// Thread2
cout << "Hello, " << "World!\n";
```

在编译器眼里，这其实是**连续调用了 5 次打印函数**。 第一句被拆为 3 次：`"Thread "` ➡️ `id` ➡️ `" is running.\n"` 第二句被拆为 2 次：`"Hello, "` ➡️ `"World!"`

这是因为底层的锁只能保住单次 `<<` 操作。当多个线程同时输出时，它们就会在这些小函数调用的间隙互相插队，导致最终屏幕上的字符被切碎、拼得乱七八糟。

当多个线程（假设是线程 1 和 2）同时跑到这里时，它们手里的这些“词块”就会像洗牌一样随机交错。你极有可能看到以下的结果：

```
Thread Hello, 1 is running.
World!
```

对于上面的例子，我们以为是只有两个线程 t1、t2 在输出，但“观感上”是有 5 个线程在输出 t1-1、t1-2、t1-3、t2-1、t2-2

**<font color=blue>4、使用场景</font>**

| **场景**                 | **推荐选择**                     |
| :----------------------- | :------------------------------- |
| 高频小数据读写（如日志） | 标准 I/O（缓冲降低系统调用开销） |
| 二进制大文件读写         | 底层 I/O（可精细控制缓冲区）     |
| 需要直接访问设备文件     | 底层 I/O（如`/dev/mem`）         |
| 格式化文本处理           | 标准 I/O（简化代码               |

- **优先标准 I/O 的情况**：
  - 需要格式化输入输出（如 `printf`/`scanf`）。
  - 频繁读写小数据（缓冲提升性能）。
  - 追求代码可移植性（跨平台兼容）。
- **优先底层 I/O 的情况**：
  - 需要非阻塞 I/O 或异步操作（如 `O_NONBLOCK`）。
  - 操作特殊文件（如管道、套接字）。
  - 需要精细控制缓冲和同步（如数据库实现）。

**<font color=blue>5、混合使用</font>**

可通过 `fileno` 和 `fdopen` 转换两者：

```cpp
// 从 FILE* 获取文件描述符
FILE* fp = fopen("file.txt", "r");
int fd = fileno(fp);

// 从文件描述符创建 FILE*
FILE* new_fp = fdopen(fd, "r");
```



## 2. 创建套接字

### 2.1 指定套接字使用的协议

```cpp
#include <sys/socket.h>
int socket(int domin, int type, int protocol);
// 1.domin: 套接字中使用的协议族(Protocol Family)信息
    // PF_INET: IPv4 互联网协议族
    // PF_INET6: IPv6 互联网协议族
    // PF_LOCAL: 本地通信的 UNIX 协议族
    // PF_PACKET: 底层套接字的协议族
    // PF_IPX: IPX Novell 协议族
// 2.type: 套接字类型(套接字数据传输类型信息)
    // SOCK_STREAM: 可靠的、按序传递的、基于字节的面向连接的数据传输方式的套接字
    // SOCK_DGRAM: 不可靠的、不按序传递的、以数据的高速传输为目的的套接字
// 3.protocol: 计算机通信中使用的协议信息
    // 一般来说，通过前两个参数即可确定套接字使用的协议，因此第三个参数可以设置为 0
    // 只有当同一协议族中存在多个传输方式相同的协议时，需要通过第三个套接字具体指定协议信息
```

### 2.2 指定套接字的地址：IP 和 Port

```cpp
// <sys/socket.h>
struct sockaddr {
    sa_family_t sin_family; // address family
    char sa_data[14]; // address info
}
```

由于 socket 接受的 IP 地址需要有通用性：即可适用于 IPv4，也可以适用于 IPv6 和其他地址，所以这里 `struct sockaddr` 的地址实际上是比较抽象设计 — 直接使用一个 `char` 数组来保存地址信息。

`struct sockaddr` 就像是一个**基类**。因为操作系统的 `bind()`、`connect()` 等网络函数诞生得很早，为了兼容未来所有可能的网络协议，它们的参数写死了只认这个基类指针 `(struct sockaddr *)`。

具体针对 ipv4 和 ipv6 地址，有专门的特化 struct：

```cpp
// <netinet/in.h>
struct sockaddr_in {
  sa_family_t sin_family;
  in_port_t sin_port;			/* Port number.  */
  struct in_addr sin_addr;		/* Internet address.  */

  /* Pad to size of `struct sockaddr'. equal to 8 */
  unsigned char sin_zero[sizeof (struct sockaddr)
       - __SOCKADDR_COMMON_SIZE
       - sizeof (in_port_t)
       - sizeof (struct in_addr)];
};

typedef uint32_t in_addr_t;
struct in_addr {
  in_addr_t s_addr;
};

/* Ditto, for IPv6.  */
struct sockaddr_in6 {
  sa_family_t sin_family;
  in_port_t sin6_port;	/* Transport layer port # */
  uint32_t sin6_flowinfo;	/* IPv6 flow information */
  struct in6_addr sin6_addr;	/* IPv6 address */
  uint32_t sin6_scope_id;	/* IPv6 scope-id */
};
```

 `struct in_addr` 这种套了一层结构体/别名的设计是考虑了拓展性。

### 2.3 常见的（服务器）网络地址信息初始化方法

```cpp
struct sockaddr_in addr;
memset(&addr, 0, sizeof(addr));  

addr.sin_family = AF_INET;
addr.sin_port = htons(serv_port);
if (inet_pton(AF_INET, serv_ip, &addr.sin_addr) != 1) { 
    fprintf(stderr, "Invalid IP address: %s\n", serv_ip);
    return -1;
}
```

### 2.4 `INADDR_ANY`

```cpp
// #include <netinet/in.h>

/* Address to accept any incoming messages.  */
#define	INADDR_ANY		((in_addr_t) 0x00000000)
```

`INADDR_ANY` 是网络编程中的一个常量，实际对应的 IP 地址通常是 `0.0.0.0`。

它的核心作用是**<font color=24b3aa>让服务端 Socket 监听本机“所有”的网络接口（网卡 IP）。</font>**

在服务器绑定（`bind`）端口时，不同的 IP 选择会决定谁能连上你：

- **绑定特定 IP（如 `192.168.1.100`）**：服务器只接收发往这个特定局域网 IP 的请求。
- **绑定 `127.0.0.1`**：只能本机内部自己访问自己，外部网络完全隔离。
- **绑定 `INADDR_ANY`**：全通吃。无论客户端是通过局域网 IP、公网 IP，还是本机回环地址发来请求，只要端口对得上，服务器都能接收。

如果你的机器有多块网卡（比如同时有内网和外网），或者你不确定程序部署后机器的具体 IP，用 `INADDR_ANY` 是最省事的，它能确保客户端从任何途径都能连上你的服务。

### 2.5 客户端套接字的地址信息

客户端套接字的地址信息分为 IP 地址和端口号。试想一下，当客户端向服务器发送连接请求时，客户端的 IP 地址是固定的，也即本机 IP 地址，而端口号则是无关紧要的， <font color=24b3aa>因为客户端的端口号不需要对外公开，所以这里随机分配一个端口号即可。至于分配时间，是当客户端通过  函数发起连接请求时，内核自动分配的（包括 IP 地址和端口号）。</font>

## 3. TCP 不存在数据边界

在 TCP 协议中，**“不存在数据边界”**（No Message Boundaries）是指 TCP 传输的数据是 **<font color=24b3aa>连续的字节流</font>**（byte stream），发送方写入的多次数据可能在接收方被合并或拆分，而不会保持原始的写入分段。这与  UDP 等保留消息边界的协议形成鲜明对比。

* TCP 无边界现象

```
发送方：
write("Hello");  
write("World");

接收方可能读到：
read() → "HelloWorld"  （合并）
或：
read() → "Hel"; read() → "loWorld" （拆分）
```

* UDP 有边界保证

```
发送方：
sendto("Hello");  
sendto("World");

接收方必然读到：
recvfrom() → "Hello"  
recvfrom() → "World" （严格保持原消息分段）
```

## 4. 字节序问题

* 小端字节序（Little Endian）：低位在低地址/高位在高地址。方便计算机处理；
* 大端字节序（Big Endian）：低位在高地址/高位在低地址。符合人类阅读习惯，例如 123，1 是百位（高位），但是在最前面（低地址）；

互联网上跑着各种架构的机器（x86 基本上都是小端，而有些 ARM 或老式机型是大端）。为了保证大家交换数据时不跨服聊天，<font color=blue>TCP/IP 协议强行规定：。</font>

不过，虽然主机之间存在着字节序不一致的问题，但如果每次将数据发送到网络都先将数据转换为网络字节序，然后接受数据的时候将数据转换为主机序，未免也太难为程序员了。因此，<font color=blue>实际上在网络上收发数据的过程中，字节序的转换过程是隐式（自动）的，我们并不需要关心。</font>

<font color=blue>我们唯一需要关心字节序的地方就是在**给 socket 指定地址（IP 和 Port）**时。</font>并且，为了程序的健壮性，你应该总是进行这个转换，尽管你的主机字节序可能和网络字节序相同。

`<netinet/in.h>` 这个头文件里面给我们提供了完备的主机字节序和网络字节序转换的函数：

```cpp
extern uint32_t ntohl (uint32_t __netlong) 
    __THROW __attribute__ ((__const__));
extern uint16_t ntohs (uint16_t __netshort)
     __THROW __attribute__ ((__const__));
extern uint32_t htonl (uint32_t __hostlong)
     __THROW __attribute__ ((__const__));
extern uint16_t htons (uint16_t __hostshort)
     __THROW __attribute__ ((__const__));
```

其中，`ntohl` 和 `htonl` 主要用于 IP 地址（32b），`htons` 和 `ntohs` 主要用于 Port（16b）。

## 5. 十进制 IP 地址转 32 位整数

### 5.1  `inet_addr`、`inet_aton`、`inet_ntoa`

对于十进制 IP 地址（字符串），网络库为我们提供了 API 来将其转换为 32 位整数，并且此整数的值满足 **<font color=24b3aa>网络字节序</font>**，这也就意味着我们不需要在将转换的结果进行字节序转换。

```cpp
#include <arpa/inet.h>
extern in_addr_t inet_addr (const char *__cp) __THROW;

/* Internet address.  */
typedef uint32_t in_addr_t;
struct in_addr {
    in_addr_t s_addr;
};

#include <netinet/in.h>
#define	INADDR_NONE		((in_addr_t) 0xffffffff)
```

成功时返回 32 位网络序整数值，失败时返回 `INADDR_NONE`。

```cpp
const char *addr1 = "1.2.3.4";
const char *addr2 = "1.2.3.256";

uint32_t conv_addr1 = inet_addr(addr1);
uint32_t conv_addr2 = inet_addr(addr2);

if(conv_addr1 == INADDR_NONE)
    printf("Error occured!\n"); // 0x04030201
else  
    printf("%#lx\n", conv_addr1); 

if(conv_addr2 == INADDR_NONE)
    printf("Error occured!\n");
else  
    printf("%#lx\n", conv_addr2); 
```

但你必须了解它在现代开发中的几个核心结论：

- **自带致命 Bug**：如果传入的 IP 格式不对，它会返回 `-1`（常量 `INADDR_NONE`）来报错。尴尬的是，广播地址 `"255.255.255.255"` 转换成 32 位整数后，结果刚好也是 `-1`。这导致你的程序根本分不清到底是报错了，还是你真的想发广播。
- **不认识 IPv6**：它诞生得太早，只能处理传统的 IPv4 地址，面对新网络环境直接抓瞎。
- **已被官方打入冷宫**：因为上述缺陷，POSIX 标准和各大操作系统的手册里，都明确把它标记为废弃（deprecated）。

`inet_aton` 和 `inet_ntoa` 分别是 `inet_addr` 的“修补版”和“反向版”，但依然属于**上个世纪的老古董**。它们的核心特点和坑点如下：

- **`inet_aton`：修补了报错 Bug，但不够完美** 全称 ASCII to Network。它把返回值和转换结果拆开了：成功返回非 0，失败返回 0，而转换后的 32 位整数通过参数写进你提供的 `struct in_addr` 里。这就完美避开了 `inet_addr` 无法识别 `255.255.255.255` 的尴尬。
- **`inet_ntoa`：藏着多线程不安全的“致命毒药”** 全称 Network to ASCII，作用刚好相反，把底层的 32 位整数转回人类可读的字符串（我们在之前的代码里打印客户端 IP 时用过）。 **致命坑点**：它把转换出来的字符串存在了函数内部的一个静态内存（Static Buffer）里，然后返回这个地址。这意味着如果多线程同时调它，或者你在一个 `printf` 里连调两次，后一次的结果会直接把前一次覆盖掉。
- **共同硬伤：不认识 IPv6** 这俩兄弟依然只在 IPv4 的圈子里打转，完全无法兼容现代的网络环境。

### 5.2 `inet_pton`、`inet_ntop`

<font color=ee11ff>在现代 C/C++ 开发中，建议把 、、 这三个老古董集体拉黑。，带个 （Presentation）结尾的才是通吃 IPv4/IPv6 且线程安全的终极现代方案。</font>

* **`p` 代表 Presentation（人类看的字符串表达）**
* **`n` 代表 Numeric（机器用的网络字节序数字）**。

`inet_pton`：

```cpp
#include <arpa/inet.h>  // Linux/macOS
// #include <ws2tcpip.h> // Windows

int inet_pton(int af, const char *src, void *dst);
// af 地址族：AF_INET（IPv4）或 AF_INET6（IPv6
// src 指向 IP 地址字符串的指针（如 "192.168.1.1" 或 "::1"） 
// dst 指向存储结果的缓冲区的指针（IPv4 为 struct in_addr，IPv6 为 struct in6_addr） 
// 返回值
//     1: 转换成功
//     0: src 不是有效的 IP 地址格式（注意：这不是系统错误）
//     -1: 系统错误（如 af 不支持），此时会设置 errno

struct sockaddr_in sa;
const char *ip_str = "192.168.1.100";

if (inet_pton(AF_INET, ip_str, &sa.sin_addr) == 1) {
    printf("转换成功\n");
} else {
    printf("无效的IP地址\n");
}
```

`inet_ntop`：

```cpp
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
// af	地址族：AF_INET 或 AF_INET6
// src	指向二进制 IP 地址结构的指针
// dst	指向输出字符串缓冲区的指针
// size	dst 缓冲区的大小（字节数）

// 缓冲区大小建议：使用宏常量确保缓冲区足够大
// IPv4: INET_ADDRSTRLEN（16 字节）
// IPv6: INET6_ADDRSTRLEN（46 字节）

// 返回值
// 成功: 返回指向 dst 的指针
// 失败: 返回 NULL，并设置 errno（常见原因：size 太小或 af 无效）

char buf[INET6_ADDRSTRLEN];
struct in6_addr addr6;

// 假设 addr6 已经被赋值...
if (inet_ntop(AF_INET6, &addr6, buf, sizeof(buf)) != NULL) {
    printf("IPv6 地址: %s\n", buf);
} else {
    perror("inet_ntop failed");
}
```

关于缓冲区的大小，这里再详细解释一下：`INET_ADDRSTRLEN` 和 `INET6_ADDRSTRLEN` 这两个宏定义的不是“二进制地址的长度”，而是 **“转换为文本字符串后所需的最大缓冲区长度（含 `\0` 终止符）”**。

| 概念         | IPv4                          | IPv6                                                        | 说明                       |
| ------------ | ----------------------------- | ----------------------------------------------------------- | -------------------------- |
| 二进制长度   | 4 字节 (`struct in_addr`)     | 16 字节 (`struct in6_addr`)                                 | 内存中网络字节序的实际大小 |
| 最长文本表示 | `"255.255.255.255"` = 15 字符 | `"ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"` = 45 字符 | 人类可读的最长形式         |
| 宏常量值     | `INET_ADDRSTRLEN` = 16        | `INET6_ADDRSTRLEN` = 46                                     | 最长文本 + 1（`\0`）       |

## 6. `int accept()`

```cpp
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

| 参数      | 类型                | 说明                                                         |
| --------- | ------------------- | ------------------------------------------------------------ |
| `sockfd`  | `int`               | 监听套接字的文件描述符（由`socket()` + `bind()` + `listen()` 创建） |
| `addr`    | `struct sockaddr *` | 输出参数，用于存储客户端的地址信息。可传`NULL` 表示不关心客户端地址 |
| `addrlen` | `socklen_t *`       | 值-结果参数：调用时传入`addr` 缓冲区大小；返回时写入实际地址长度。若 `addr` 为 `NULL`，此参数也应为 `NULL` |

**返回值**

- **成功**：返回一个新的**<font color=blue>已连接套接字</font>**文件描述符（非负整数），后续与该客户端的通信使用这个新 fd
- **失败**：返回 `-1`，并设置 `errno`

传入的是**监听 socket**（等连接的），返回的是服务端新建的**已连接 socket**（跟某个客户端一对一通信的）。两者都在服务端，各司其职。

## 7. `sizeof(msg)` or `strlen(msg)`？

|                   | `sizeof()`                               | `strlen()`                              |
| :---------------- | :--------------------------------------- | --------------------------------------- |
| **本质**          | **编译期运算符**（不是函数）             | **运行时库函数**                        |
| **计算对象**      | 变量/类型在内存中占用的**字节大小**      | 字符串从首字符到`\0` 之前的**字符个数** |
| **是否包含 `\0`** | 对数组而言，包含（因为它是数组的一部分） | 不包含                                  |
| **何时确定**      | 编译时                                   | 运行时遍历                              |
| **参数**          | 类型名或表达式（不求值）                 | 必须是`char*` 指向的以 `\0`             |

例如：

```cpp
char str[] = "hello";      // 数组，6字节（含\0）
char *ptr = "hello";       // 指针

sizeof(str);   // 6 → 整个数组的大小（包含\0）
strlen(str);   // 5 → 字符串长度（不含\0）

sizeof(ptr);   // 8（64位系统上指针的大小）
strlen(ptr);   // 5 → 仍然是字符串长度
```

在网络编程场景下，对于 `char msg[]`：

* <font color=blue>服务器向客户端写入数据：</font>

  ```cpp
  write(clnt_sock, msg, strlen(msg));
  ```

  * **目的**：只发送字符串的**有效内容**，不发送 `\0`
  * <font color=24b3aa>网络协议传输的是数据流， 只是 C 语言的本地约定，接收方不需要它</font>
  * 如果用 `sizeof(msg)`，会把 `\0` 甚至数组剩余未初始化的垃圾字节都发出去

* <font color=blue>客户端从服务器读取数据：</font>

  ```cpp
  int read_len = read(serv_sock, msg, sizeof(msg) - 1);
  msg[read_len] = '\0';
  ```

  * **目的**：确保读入的数据**不会越界**，且留出空间手动补 `\0`
  * `sizeof(msg)` 是整个数组大小，减 1 保证最多只读 `size-1` 个字节，最后一个位置留给 `\0`
  * `read()` 返回实际读取的字节数（可能小于请求的大小），用它作为下标精确放置 `\0`
  * 如果用 `strlen(msg)` 作为 `read` 的长度参数 → **完全错误**，因为 `msg` 此时还没有有效字符串，`strlen` 结果未定义

## 8. `scanf` 空白字符处理机制

在 `scanf` 的格式字符串中，**空格**（包括 `' '`、`\t`、`\n`）具有特殊指令含义：

> <font color=24b3aa>**指示 `scanf` 跳过输入流中所有的连续空白字符，直到遇到第一个非空白字符为止。**</font>

理解 `scanf` 空白处理的关键，在于区分格式符是否 **<font color=blue>自带</font>** 跳过空白功能：

| 类别              | 格式符                  | 自动跳过前导空白 | 备注                           |
| :---------------- | :---------------------- | :--------------- | :----------------------------- |
| **数值/字符串类** | `%d` `%f` `%lf` `%s` 等 | ✅ 是             | 无需手动加空格                 |
| **字符/扫描集类** | `%c` `%[...]` `%n`      | ❌ 否             | **必须手动加空格**才能跳过空白 |

例如：用户输入数字后按下回车，缓冲区中会残留一个 `\n`：

```cpp
scanf("%d", &opnd_cnt);   // 用户输入 "3\n" → %d 读取 '3'，'\n' 残留在缓冲区
scanf("%c", &operator);   // ⚠️ %c 不跳过空白 → 直接读到 '\n' → 逻辑错误！
```

**正确做法：**在 `%c` 前添加空格，手动跳过残留空白：

```cpp
scanf("%d", &opnd_cnt);   // 正常读取数字
scanf(" %c", &operator);  // ✅ 空格消耗掉残留 '\n'，%c 等待读取真正的运算符
```

此外，<font color=blue>空格在格式字符串中的**位置**决定了完全不同的行为：</font>

| 位置     | 示例      | 效果                                 | 安全性               |
| :------- | :-------- | :----------------------------------- | :------------------- |
| **开头** | `" %c"`   | 跳过前导空白                         | ✅ 推荐               |
| **中间** | `"%d %d"` | 等价于 `"%d%d"`（`%d` 自带跳过）     | ✅ 无害但冗余         |
| **末尾** | `"%d "`   | 读完数字后**继续阻塞等待**非空白字符 | ❌ **危险，避免使用** |

> ⚠️**特别注意**：末尾空格会导致 `scanf` 在读取完有效数据后仍不返回，持续等待用户输入下一个非空白字符或 EOF，造成程序**意外挂起**。

**<font color=blue>最佳实践总结：</font>**

1. **读取字符时**：始终使用 `" %c"` 而非 `"%c"`，除非你明确需要读取空白字符本身。
2. **连续读取数值时**：`"%d%d"` 与 `"%d %d"` 效果相同，中间空格可省略。
3. **读取整行字符串时**：避免 `scanf("%s")`（遇空白截断），改用 `fgets()` 或 C++ 的 `std::getline()`。
4. **C++ 项目优先**：使用 `std::cin >> x`（类型安全）；需跳过空白读字符时用 `cin >> ws >> ch` 显式处理。

## 9. `AF_INET` 和 `PF_INET`

在 Linux/Unix 网络编程（Socket API）中，`AF_INET` 和 `PF_INET` 是两个极易混淆但 **<font color=blue>在实际使用中完全等价</font>** 的 **宏定义**。

以下是它们的核心区别与联系：

### **9.1 概念上的区别（历史设计意图）**

| 宏     | 全称                    | 含义   | 用途                                                         |
| :----- | :---------------------- | :----- | :----------------------------------------------------------- |
| **PF** | **P**rotocol **F**amily | 协议族 | 用于 `socket()` 函数的第一个参数，指定创建套接字时使用的**底层协议族**。 |
| **AF** | **A**ddress **F**amily  | 地址族 | 用于 `bind()`, `connect()`, `getaddrinfo()` 等函数，指定**地址结构体**的类型。 |

- <font color=24b3aa>**设计初衷：** 早期的 Unix 设计者设想一个“协议族”可能对应多个“地址族”。例如，同一个协议族理论上可以支持不同的地址格式。因此用 PF 来选协议，用 AF 来选地址格式。</font>
- <font color=24b3aa>**实际情况：** 这个“一对多”的设计从未真正实现过。**每个协议族都恰好只对应一个地址族**。</font>

### **9.2 实际实现（完全等价）**

<font color=24b3aa>在所有现代操作系统（Linux, macOS, BSD, Windows Winsock）的头文件中，它们的值是完全相同的。</font>

以 Linux 为例，`socket.h` 定义如下：

```cpp
/* Protocol families.  */
#define PF_UNSPEC	0	/* Unspecified.  */
#define PF_LOCAL	1	/* Local to host (pipes and file-domain).  */
#define PF_UNIX		PF_LOCAL /* POSIX name for PF_LOCAL.  */
#define PF_FILE		PF_LOCAL /* Another non-standard name for PF_LOCAL.  */
#define PF_INET		2	/* IP protocol family.  */
#define PF_AX25		3	/* Amateur Radio AX.25.  */
#define PF_IPX		4	/* Novell Internet Protocol.  */
#define PF_APPLETALK	5	/* Appletalk DDP.  */
#define PF_NETROM	6	/* Amateur radio NetROM.  */
#define PF_BRIDGE	7	/* Multiprotocol bridge.  */
#define PF_ATMPVC	8	/* ATM PVCs.  */
#define PF_X25		9	/* Reserved for X.25 project.  */
#define PF_INET6	10	/* IP version 6.  */
#define PF_ROSE		11	/* Amateur Radio X.25 PLP.  */
#define PF_DECnet	12	/* Reserved for DECnet project.  */
#define PF_NETBEUI	13	/* Reserved for 802.2LLC project.  */
#define PF_SECURITY	14	/* Security callback pseudo AF.  */
#define PF_KEY		15	/* PF_KEY key management API.  */
#define PF_NETLINK	16
#define PF_ROUTE	PF_NETLINK /* Alias to emulate 4.4BSD.  */
#define PF_PACKET	17	/* Packet family.  */
#define PF_ASH		18	/* Ash.  */
#define PF_ECONET	19	/* Acorn Econet.  */
#define PF_ATMSVC	20	/* ATM SVCs.  */
#define PF_RDS		21	/* RDS sockets.  */
#define PF_SNA		22	/* Linux SNA Project */
#define PF_IRDA		23	/* IRDA sockets.  */
#define PF_PPPOX	24	/* PPPoX sockets.  */
#define PF_WANPIPE	25	/* Wanpipe API sockets.  */
#define PF_LLC		26	/* Linux LLC.  */
#define PF_IB		27	/* Native InfiniBand address.  */
#define PF_MPLS		28	/* MPLS.  */
#define PF_CAN		29	/* Controller Area Network.  */
#define PF_TIPC		30	/* TIPC sockets.  */
#define PF_BLUETOOTH	31	/* Bluetooth sockets.  */
#define PF_IUCV		32	/* IUCV sockets.  */
#define PF_RXRPC	33	/* RxRPC sockets.  */
#define PF_ISDN		34	/* mISDN sockets.  */
#define PF_PHONET	35	/* Phonet sockets.  */
#define PF_IEEE802154	36	/* IEEE 802.15.4 sockets.  */
#define PF_CAIF		37	/* CAIF sockets.  */
#define PF_ALG		38	/* Algorithm sockets.  */
#define PF_NFC		39	/* NFC sockets.  */
#define PF_VSOCK	40	/* vSockets.  */
#define PF_KCM		41	/* Kernel Connection Multiplexor.  */
#define PF_QIPCRTR	42	/* Qualcomm IPC Router.  */
#define PF_SMC		43	/* SMC sockets.  */
#define PF_XDP		44	/* XDP sockets.  */
#define PF_MCTP		45	/* Management component transport protocol.  */
#define PF_MAX		46	/* For now..  */

/* Address families.  */
#define AF_UNSPEC	PF_UNSPEC
#define AF_LOCAL	PF_LOCAL
#define AF_UNIX		PF_UNIX
#define AF_FILE		PF_FILE
#define AF_INET		PF_INET
#define AF_AX25		PF_AX25
#define AF_IPX		PF_IPX
#define AF_APPLETALK	PF_APPLETALK
#define AF_NETROM	PF_NETROM
#define AF_BRIDGE	PF_BRIDGE
#define AF_ATMPVC	PF_ATMPVC
#define AF_X25		PF_X25
#define AF_INET6	PF_INET6
#define AF_ROSE		PF_ROSE
#define AF_DECnet	PF_DECnet
#define AF_NETBEUI	PF_NETBEUI
#define AF_SECURITY	PF_SECURITY
#define AF_KEY		PF_KEY
#define AF_NETLINK	PF_NETLINK
#define AF_ROUTE	PF_ROUTE
#define AF_PACKET	PF_PACKET
#define AF_ASH		PF_ASH
#define AF_ECONET	PF_ECONET
#define AF_ATMSVC	PF_ATMSVC
#define AF_RDS		PF_RDS
#define AF_SNA		PF_SNA
#define AF_IRDA		PF_IRDA
#define AF_PPPOX	PF_PPPOX
#define AF_WANPIPE	PF_WANPIPE
#define AF_LLC		PF_LLC
#define AF_IB		PF_IB
#define AF_MPLS		PF_MPLS
#define AF_CAN		PF_CAN
#define AF_TIPC		PF_TIPC
#define AF_BLUETOOTH	PF_BLUETOOTH
#define AF_IUCV		PF_IUCV
#define AF_RXRPC	PF_RXRPC
#define AF_ISDN		PF_ISDN
#define AF_PHONET	PF_PHONET
#define AF_IEEE802154	PF_IEEE802154
#define AF_CAIF		PF_CAIF
#define AF_ALG		PF_ALG
#define AF_NFC		PF_NFC
#define AF_VSOCK	PF_VSOCK
#define AF_KCM		PF_KCM
#define AF_QIPCRTR	PF_QIPCRTR
#define AF_SMC		PF_SMC
#define AF_XDP		PF_XDP
#define AF_MCTP		PF_MCTP
#define AF_MAX		PF_MAX
```

在 Linux 环境下，你可以发现，`AF_*` 直接以 `PF_*` 作为值定义宏。

### **9.3 使用建议**

<font color=blue>尽管两者可互换，但社区有约定俗成的规范：</font>

| 场景                  | 推荐用法                              | 示例                              |
| :-------------------- | :------------------------------------ | :-------------------------------- |
| 创建套接字 `socket()` | `AF_INET` ✅ (主流) / `PF_INET` (传统) | `socket(AF_INET, SOCK_STREAM, 0)` |
| 设置地址结构体        | **必须** `AF_INET`                    | `addr.sin_family = AF_INET;`      |
| `getaddrinfo()` hints | **必须** `AF_INET`                    | `hints.ai_family = AF_INET;`      |

### **9.4 常见误区**

- ❌ "`socket()` 必须用 `PF_INET`，`bind()` 必须用 `AF_INET`" → **错误**。两者在任何位置都可互换。
- ❌ "`AF_INET6` 和 `PF_INET6` 值不同" → **错误**。它们同样相等。
- ✅ "用 `AF_INET` 写所有地方" → **正确且推荐**。

如果你在阅读旧代码时看到 `PF_INET`，直接将其理解为 `AF_INET` 即可，无需做任何修改就能正常运行。

## 10. TCP 服务器套接字职责

```cpp
int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
bind(serv_sock, ...);
listen(serv_sock, ...);

// accept 阻塞等待，当有客户端连接成功后，返回一个新的 fd
int clnt_sock = accept(serv_sock, NULL, NULL);
```

* `serv_sock`：监听套接字，仅用于接收新的连接请求，**不参与任何数据传输**。
* `clnt_sock`：已连接套接字，由内核在三次握手完成后自动创建并返回。**所有与该客户端的读写操作都通过这个 fd 进行**。

**在服务器端，针对某一个已建立的客户端连接，所有与该客户端的数据交互（无论读还是写）都通过同一个套接字文件描述符 `clnt_sock` 完成；而 `serv_sock` 仅负责接受新连接，不参与任何实际通信。**

## 11. 套接字选项

在之前我们介绍过通过 `setsockopt` 来设置套接字端口复用的方法，其实套接字有多种可选项，例如：

![sock_opt](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1751376242779.mckk78qx.webp)

<font color=24b3aa>套接字可选项分为两个层级：</font>

* <font color=24b3aa>协议层</font>
* <font color=24b3aa>选项名</font>

当然，上面只是做一个基本展示，事实上套接字选项非常多，你也无需全部记住，用到的时候在查就行了。

### 1. `getsockopt` & `setsockopt`

函数原型：

```cpp
#include <sys/socket.h>

/* Put the current value for socket FD's option OPTNAME at protocol level LEVEL
   into OPTVAL (which is *OPTLEN bytes long), and set *OPTLEN to the value's
   actual length.  Returns 0 on success, -1 for errors.  */
extern int getsockopt (int __fd, int __level, int __optname,
		       void *__restrict __optval,
		       socklen_t *__restrict __optlen) __THROW;

/* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */
#ifndef __USE_TIME_BITS64
extern int setsockopt (int __fd, int __level, int __optname,
		       const void *__optval, socklen_t __optlen) __THROW;
```

代码演示：获取 socket 的类型

```cpp
#include <iostream>
#include <cstring>
#include <algorithm>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

int main(int argc, char **argv)
{
    int sock_type;
    int tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
    int udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
    socklen_t optlen = sizeof(sock_type);
  
    int state = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
    if(state == -1) {
        cerr << "getsockopt() errpr" << endl;
        return 1;
    }
    cout << "socket opt: " << sock_type << endl; // 1
    cout << "socket len: " << optlen << endl; // 4

    state = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);
    if(state == -1) {
        cerr << "getsockopt() errpr" << endl;
        return 1;
    }
    cout << "socket opt: " << sock_type << endl; // 2
    cout << "socket len: " << optlen << endl; // 4

    return 0;
}
```

### 2. `SO_SNDBUF` & `SO_RCVBUF`

创建套接字将同时生成 I/O 缓冲，通过 `SO_SNDBUG` 和 `SO_RCVBUF` 可以读取当前 I/O 缓冲区大小，也可以进行修改。

```cpp
#include <iostream>
#include <cstring>
#include <algorithm>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

void cat_io_buffer(int sock)
{
    int snd_buf, rcv_buf;
    socklen_t len;

    len = sizeof(snd_buf);
    int state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);
    if(state == -1) {
        cerr << "getsockopt() error" << endl;
        exit(1);
    }
    cout << "Output Buffer: " << snd_buf << endl; // 16384

    len = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
    if(state == -1) {
        cerr << "getsockopt() error" << endl;
        exit(1);
    }
    cout << "Input Buffer: " << rcv_buf << endl; // 131072
}

void change_io_buffer(int sock) 
{
    const int BUF_SIZE = 4196;
    int snd_buf = BUF_SIZE, rcv_buf = BUF_SIZE;

    int state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));
    if(state == -1) {
        cerr << "getsockopt() error" << endl;
        exit(1);
    }
    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));
        if(state == -1) {
        cerr << "getsockopt() error" << endl;
        exit(1);
    }
}

int main(int argc, char **argv)
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    cat_io_buffer(sock);
    change_io_buffer(sock);
    cat_io_buffer(sock);
    // Output Buffer: 16384
    // Input Buffer: 131072
    // Output Buffer: 8392
    // Input Buffer: 8392
    return 0;
}
```

注意观察输出结果，我们设置的缓冲区大小为 `4196`，缺输出了 `8392`（`4196` 的两倍），这是因为操作系统内核在设置套接字缓冲区大小时，通常会进行一些内部调整：

- **双倍大小：** 内核通常会分配您请求大小的两倍空间。例如，如果您请求 `4196` 字节，内核可能会分配 `8392` 字节。这是因为内核需要额外的空间来管理套接字缓冲区本身的数据结构和元数据。
- **页对齐：** 缓冲区大小通常会被调整到内存页的倍数（例如，4KB 或 8KB）。如果您的请求大小不是页的倍数，内核会向上取整到最接近的页倍数。
- **最小值和最大值：** 系统对缓冲区大小有最小值和最大值的限制。如果您请求的值低于最小值或高于最大值，内核会将其调整到允许的范围内。

### 3. `SO_REUSEADDR`

```cpp
int option = true;
setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)*option, sizeof(option));
```

`SO_REUSEADDR` 默认值为 0（false），表示该选项未打开，因此我们需要将这个值修改为 1（true）。

### 4. `TCP_NODELAY` & Nagle Algorithm

Nagle`/ˈneɪɡəl/` 算法（Nagle's algorithm）是 TCP/IP 协议栈中的一种 **拥塞控制** 算法，其主要目的是为了解决 **“小数据包问题”（tinygram problem）**，提高网络利用率，减少网络中传输的小数据包数量。

#### 原理

Nagle 算法的核心思想是：**在有数据待发送且有未确认的已发送数据包时，只允许一个未确认的小数据包存在于网络中。在收到该小数据包的确认之前，会持续缓存应用程序提交的小数据，直到累积到一定量（例如达到最大分段大小 MSS），或者收到上一个数据包的确认，才会将缓存的数据一次性发送出去。**

简单来说，就是：

1. 当应用程序有数据要发送时，如果数据量很小（小于 MSS），且之前发送的数据包还没有收到确认 (ACK)。
2. Nagle 算法会暂时“hold住”这些小数据，不立即发送。
3. 它会等待：
   - 足够多的数据累积起来，形成一个更大的数据包（接近 MSS）。
   - 或者，之前发送的数据包的 ACK 到达。
4. 满足上述任一条件后，才将缓存的数据一次性发送出去。

#### 优点

- **减少小数据包的数量：** 避免了网络中充斥大量只包含少量数据的小数据包，从而减少了不必要的网络开销（每个数据包都有固定的头部开销）。
- **提高网络利用率：** 通过将多个小数据聚合成一个大数据包发送，提高了每个数据包的有效载荷比例，从而更有效地利用了网络带宽。
- **缓解网络拥塞：** 减少了发送的数据包数量，有助于减轻网络中的拥塞。

#### 缺点

- **引入延迟：** Nagle 算法的 “hold ”机制可能会引入额外的延迟。对于需要实时响应的应用程序（如在线游戏、远程桌面、Telnet 会话等），这种延迟是不可接受的，因为它会导致用户输入不能立即得到响应。
- **与延迟确认（Delayed ACK）的冲突：** Nagle 算法经常会与 TCP 的另一个优化机制“延迟确认”（Delayed ACK）产生“冲突”或“悲剧”。
  - **延迟确认** 的目的是避免为每个接收到的数据包立即发送 ACK，而是等待一段时间（通常是200毫秒），看是否有数据需要发送给对方，如果有，则将 ACK 捎带在数据包中一起发送；如果没有，则单独发送 ACK。
  - 当 Nagle 算法与延迟确认同时开启时，可能会出现这样的死锁情况：
    1. 发送方发送一个小数据包。
    2. 接收方收到后，启动延迟确认计时器，不立即发送 ACK。
    3. 发送方由于 Nagle 算法，因为上一个数据包的 ACK 未到，继续缓存后续的小数据包。
    4. 接收方等待计时器超时后才发送 ACK。
    5. 发送方收到 ACK 后才能发送后续数据。 这导致了一段不必要的等待，增加了通信延迟。

#### 何时禁用 Nagle 算法？

在以下情况下，通常建议禁用 Nagle 算法：

- **实时交互性应用：** 如 Telnet、SSH、游戏、远程桌面等，这些应用对延迟非常敏感，需要即时响应用户操作。
- **发送大量小数据包的应用：** 某些应用程序可能需要频繁地发送非常小的数据块，如果 Nagle 算法导致延迟过高，可以考虑禁用。
- **HTTP 协议（在某些场景下）：** 在 HTTP/1.1 中，浏览器通常会重用 TCP 连接，并且 Nagle 算法可能导致请求和响应之间的延迟。虽然 HTTP/2 改变了底层传输机制，但理解 Nagle 算法在 HTTP 早期版本中的影响仍然重要。

#### 如何禁用 Nagle 算法？

```cpp
int sock = socket(PF_INET, SOCK_STREAM, 0);
int enable = 1; // 1 表示启用 TCP_NODELAY，即禁用 Nagle 算法
setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
```

禁用 Nagle 算法会使得数据只要有，就会立即发送，无论大小，从而减少延迟，但可能会增加网络中的小数据包数量，牺牲一定的网络效率。因此，在决定是否禁用 Nagle 算法时，需要根据应用程序的具体需求权衡利弊。

## 12. 半关闭与优雅关闭

TCP 是全双工通信，这意味着在 TCP 连接建立后，数据可以：

1. 从**客户端流向服务器** (客户端发送，服务器接收)。
2. 从**服务器流向客户端** (服务器发送，客户端接收)。

因此说，对应通信的双方，它们都有两个流（一个输入流，一个输出流）。

基于这种全双工特性，TCP 连接的关闭也需要双方分别独立地终止各自的发送信道。在协议层面，这表现为通信双方各自发送一个 `FIN` 报文（即四次挥手）；在代码层面，则主要通过以下两个函数来实现：

- **`close()`** ：这是最常用的套接字资源释放方式。它会同时关闭套接字的读写通道，并将该 socket 的引用计数减 1。**仅当引用计数归零时** ，内核才会向对端发送 `FIN` 报文。需要注意的是，`close()` 的主要语义是“释放本地文件描述符资源”，发送 `FIN` 只是引用计数归零后的附带行为；在多进程/多线程共享 socket 的场景下，单次 `close()` 并不会立即触发连接关闭。
- **`shutdown()`** ：这是精确控制全双工信道关闭的函数。它允许选择性地关闭读端、写端或两端（`SHUT_RD`、`SHUT_WR`、`SHUT_RDWR`）。当调用 `shutdown(sockfd, SHUT_WR)` 时，内核会**立即**向对端发送 `FIN` 报文，表示“我方已停止发送”，但本地仍可继续接收对端数据。这才是真正对应 TCP 半关闭机制的代码实现。

<font color=blue>`shutdown()` **完全不受引用计数的影响** ，这正是它与 `close()` 最核心的区别之一。</font>

| 特性          | `close()`                    | `shutdown()`                                    |
| ------------- | ---------------------------- | ----------------------------------------------- |
| 操作对象      | 文件描述符（fd）             | 底层 socket 内核结构体                          |
| 引用计数      | 引用计数 -1，归零才发 FIN    | 无视引用计数 ，立即生效发送 FIN                 |
| 多进程/多线程 | 仅最后一个 fd 关闭时触发 FIN | 所有共享该 socket 的进程/线程立即感知到状态变化 |
| 本质语义      | “我释放了这个 fd”            | “这个 socket 的连接状态改变了”                  |

- **<font color=ee11ff>`close()` 操作的是“文件描述符”</font>**
  文件描述符是进程级别的资源。多个进程通过 `fork()` 或 `dup()` 可以持有同一个 socket 的多个 fd 副本。内核必须通过引用计数来判断“是否还有进程在使用这个 socket”，只有没人用了才能安全地发送 `FIN` 并释放底层资源。
- **<font color=ee11ff>`shutdown()` 操作的是“socket 本身”</font>**
  `shutdown()` 直接修改内核中 socket 结构体的状态标志位（如标记写端为 `SHUT_WR`）。这个状态是**全局于该 socket 的** ，与有多少个 fd 指向它无关。一旦调用，内核立即执行协议动作（发送 `FIN`），<font color=24b3aa>所有持有该 socket fd 的进程在后续读写时都会立即感知到新的状态（例如写操作返回 `-EPIPE`）。</font>

这正是 `shutdown()` 在多进程/多线程网络服务器中不可替代的原因：

> 假设父进程 `fork()` 出子进程处理客户端请求，父子进程共享同一个 socket fd：
>
> - 若子进程处理完数据后调用 `close()` ：仅减少引用计数，**不会发送 `FIN`** 。父进程仍持有 fd，连接保持打开，客户端无法感知服务端已发送完毕。
> - 若子进程调用 `shutdown(sockfd, SHUT_WR)` ：**立即发送 `FIN`** ，无论父进程是否还持有该 fd。客户端收到 `FIN` 后知道服务端数据已发完，可以优雅地进入半关闭状态。

### 12.1 shutdown

```cpp
#include <sys/socket.h>
int shutdown(int sockfd, int how);
```

- `sockfd`: 要操作的套接字文件描述符
- `how`: 指定关闭方式，可以是以下值之一：
  - `SHUT_RD` (0) - 关闭读通道，不再接收数据
  - `SHUT_WR` (1) - 关闭写通道，不再发送数据
  - `SHUT_RDWR` (2) - 同时关闭读写通道

成功时返回 0；失败时返回 -1，并设置 errno。

<font color=blue>在 TCP 中调用 `shutdown(SHUT_WR)` 本质上会导致发送 FIN 包，但这是整个过程的最终表现，而非全部本质：</font>

1. **FIN 包的发送**：
   - 当调用 `shutdown(sockfd, SHUT_WR)` 时，TCP 协议栈会发送 FIN 包给对方
   - 这是一个正常的 TCP 连接终止序列的一部分（四次挥手的第一步）
2. **更深层的本质**：
   - `shutdown()` 实际上是修改了内核中 TCP 套接字的状态机状态
   - <font color=24b3aa>将套接字状态从 ESTABLISHED 变为 FIN_WAIT_1（主动关闭方）</font>
   - 协议栈检测到状态变化后自动触发 FIN 发送
3. **与 `close()` 的区别**：
   - `close()` 也可能发送 FIN，但只有在引用计数为 0 时
   - `shutdown()` 会立即触发 FIN，无论引用计数

注意事项：

1. <font color=e113fe>`shutdown()` 不释放套接字资源，仍需调用 `close()`</font>
2. <font color=e113fe>多次调用 `shutdown()` 是允许的，但效果会叠加</font>
3. <font color=e113fe>在 UDP 中使用 `shutdown()` 效果有限，因为 UDP 是无连接的</font>

### 12.2 UDP 中 shutdown 的行为特点

1. **基本效果**：
   - 在 UDP 套接字上调用 `shutdown()` 不会像 TCP 那样发送 FIN 包（因为 UDP 没有连接的概念）
   - 函数调用本身会成功执行（返回 0），但实际网络行为与 TCP 完全不同
2. **不同参数的影响**：
   - **SHUT_RD (关闭读)**：
     - 后续的 `recv()`/`recvfrom()` 调用将返回失败（通常返回 -1，errno 设为 ENOTCONN 或 ESHUTDOWN）
     - 已经到达但尚未读取的数据报仍可被读取
     - 不影响发送数据的能力
   - **SHUT_WR (关闭写)**：
     - 后续的 `send()`/`sendto()` 调用将返回失败
     - 不影响接收数据的能力
     - 不会向对端发送任何通知（UDP 没有关闭连接的机制）
   - **SHUT_RDWR (同时关闭读写)**：
     - 相当于先后调用 SHUT_RD 和 SHUT_WR
     - 套接字将完全不可用，但仍需调用 `close()` 释放资源

在 UDP 中使用 `shutdown()` 的实际用途非常有限，因为：

1. UDP 本身是无状态的，不需要"优雅关闭"的概念

   > **TCP 的连接管理机制**：
   >
   > - TCP 是面向连接的协议，建立连接需要三次握手，终止需要四次挥手
   > - shutdown(SHUT_WR) 会触发 FIN 包的发送，这是 TCP 协议栈的标准行为
   > - 这种机制确保了数据传输的可靠性和有序性
   >
   > **UDP的无状态本质**：
   >
   > - UDP没有连接概念，每个数据报都是独立的
   > - 不存在连接建立和终止的过程
   > - 内核协议栈不会维护任何会话状态
   > - 调用 shutdown() 不会触发任何网络层操作，因为 UDP 根本没有FIN这类控制报文
   >
   > **示例对比**：
   >
   > ```cpp
   > // TCP中的优雅关闭
   > shutdown(tcp_sock, SHUT_WR);  // 发送 FIN 包，通知对端不再发送数据
   > recv(tcp_sock, ...);          // 仍然可以接收对端可能发送的剩余数据
   > 
   > // UDP 中的"伪关闭"
   > shutdown(udp_sock, SHUT_WR);  // 不会发送任何网络包，对端完全不知情
   > ```

2. 大多数 UDP 应用程序只需要简单的 `close()` 就足够了

   > **资源释放角度**：
   >
   > - 在 UDP 中，close() 已经足以完全释放套接字资源
   > - 不需要像 TCP 那样考虑半关闭状态或确保数据被对端接收
   >
   > **多进程/线程场景**：
   >
   > - 即使有多个进程共享同一个 UDP 套接字
   > - close() 只是减少引用计数，最后一个 close() 才真正释放资源
   > - shutdown() 的立即生效特性在 UDP 中几乎没有实用价值

3. 关闭一个方向的操作在无连接协议中意义不大

   > **协议设计视角**：
   >
   > - UDP 的数据报是自包含的，没有数据流的概念
   > - 关闭"读"或"写"方向不会影响网络通信的本质

简而言之，就是<font color=24b3aa>在 UDP 当中，没有流的概念，通信双方也没有建立连接，此时，调用 shutdown 虽然可以正常执行，但实际上没有任何意义，因为调用 shutdown 的一方不会给对端发送网络包，也因此对端感知不到调用方的 shutdown 命令。也即，shutdown 只对调用 shutdown 的一方形成约束效果，但对端是无感知的</font>

### 12.3 优雅关闭

所谓（TCP 协议层的）优雅关闭就是，先关闭发送，再处理未完成的读，等处理完读再整体关闭。

如果不按这个顺序，会出现两种典型问题：

* **直接 `close()`** ：读写通道同时关闭。若对端还有数据没发完，这些数据会被丢弃；若对端随后发来数据，本端内核会回复 `RST`，导致对端异常。
* **先读完再关写**：如果对端也在等你发完才关写，就会形成**死锁** ：双方都在等对方先关写，谁也不发 `FIN`，连接永远无法关闭。

**优雅关闭 = 用 `FIN` 作为“发送完毕”的信号，而非“连接结束”的信号；在读到对端的 `FIN`（EOF）之前，绝不关闭自己的读通道。**

这正是 TCP 全双工设计的精髓：**两个方向的流是独立的，关闭也必须是独立的、有序的** 。你的表述已经完全抓住了这个本质。

而**半关闭（Half-Close）是优雅关闭的必要前提和底层机制支撑** 。没有半关闭能力，TCP 的优雅关闭就无从谈起。

> 当我们谈论 Web 服务器（如 Nginx, Go HTTP Server, Spring Boot）的优雅关闭时，流程比你描述的更丰富：
>
> 1. **停止接收新连接/新请求：** 不再 Accept 新的 Socket，或者返回 503 状态码。
> 2. **等待存量请求完成：** 给正在处理的请求一个**宽限期**。不是无限期地“等读完”，而是设置一个 Timeout。
> 3. **资源清理：** 提交数据库事务、刷新日志缓冲、断开下游连接、释放锁等。**这一步是你描述中缺失的关键环节。**
> 4. **强制终止：** 如果宽限期到了还有请求没处理完，强制杀掉（避免僵尸进程）。
> 5. **关闭监听与连接：** 最后才执行底层的 close/shutdown。

## 13. 服务器“友好重启”

### 13.1 TIME-WAIT

有时候我们刚关闭服务器，再次重启时，会报错地址已经在使用：

```shell
gcc server.c -o server
./server 9190
bind() error: Address already in use
```

我们首先回忆一下 TCP 的四报文挥手：

![tcp_four_message_byebbye](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.3gooatjyni.webp)

可以发现，主动关闭的一方（一般称之为客户端）在 `FIN_WAIT2` 阶段之后，会等待 2MSL 的时间，这是为了确保服务器可以正确接收到客户端的 LAST-ACK 消息。

如果我们先关闭了服务器，服务器作为主动关闭的一方，在最后就需要等待 2MSL 的时间，此时，监听端口 `serv_sock` 仍处于 `TIME_WAIT` 状态，系统会阻止你再次绑定这个端口，从而报错“Address already in use”。

另外就是，让服务器等待一段时间再关闭，可以**防止“旧连接”的数据包干扰“新连接”**：如果一个新连接立即使用了同一个端口，而网络中仍然有属于旧连接的延迟数据包到达，这些旧数据包可能会被新连接错误地接收。

### 13.2 `SO_REUSEADDR` 选项

最常见的解决方案是使用 **`SO_REUSEADDR` 选项**。

```c
int optval = 1;
setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, 
           &optval, sizeof(optval));
```

`setsockopt()` 是一个用于**设置套接字选项**的函数。它的原型通常是这样的：

```c
int setsockopt(int sockfd, int level, int optname, 
               const void *optval, socklen_t optlen);
```

现在我们来看你代码中传入的每个参数：

- **`sockfd` (第一个参数): `serv_sock`**

  - 这个参数指定了你要设置选项的套接字文件描述符。
  - 在这里，它是你的**服务器监听套接字 `serv_sock`**。这意味着这个选项将应用于整个服务器程序的监听功能。

- **`level` (第二个参数): `SOL_SOCKET`**

  - 这个参数指定了选项所在的协议层。套接字选项可以存在于不同的协议层，例如套接字层、TCP 层、IP 层等。
  - `SOL_SOCKET`（SOL：Socket Option Level） 表示你正在设置的是**套接字层（socket level）**的通用选项，这些选项不特定于某个协议（如 TCP 或 UDP），而是适用于所有类型的套接字。

- **`optname` (第三个参数): `SO_REUSEADDR`**

  - 这个参数是具体的选项名称。
  - **`SO_REUSEADDR`** 是这个选项的关键。它的全称是 "Socket Option Reuse Address"。它的作用是：**允许一个套接字绑定到一个已经被使用（但处于 `TIME_WAIT` 状态）的本地地址和端口上。**

- **`optval` (第四个参数):**

  - **`&optval`** 这个参数是一个**指向**你希望设置的选项值的**指针**。`setsockopt()` 函数会通过这个指针去读取实际的值。

  - 在这里，它指向的是我们之前声明并赋值为 `1` 的 `optval` 变量，表示我们想**启用** `SO_REUSEADDR` 选项。

  - 在 `setsockopt()` 函数中，对于像 `SO_REUSEADDR` 这种**开关性质的选项**，通常使用整数 `1` 来表示**启用（true）**，而 `0` 则表示**禁用（false）**。

  - 在C语言中，并没有严格的 `true`/`false` 布尔类型（C99 标准引入了 `_Bool` 和 `<stdbool.h>`，但更早的 C 版本没有，且在系统编程中，整数仍然广泛用于布尔逻辑）。通常：

    - **非零值**（特别是 `1`）被视为**真（true）**。
    - **零值**（`0`）被视为**假（false）**。

    因此，将 `optval` 设置为 `1`，就是明确地告诉 `setsockopt()` 函数，我们希望**启用** `SO_REUSEADDR` 这个功能。

- **`optlen` (第五个参数): `sizeof(optval)`**

  - 这个参数指定了 `optval` 指向的数据的大小。
  - `sizeof(optval)` 返回 `optval` 变量（即一个 `int` 类型）所占的字节数，确保函数能正确读取选项值。

正如我们之前讨论的，TCP 连接关闭后，主动关闭方（例如服务器进程）的连接端口会进入 `TIME_WAIT` 状态，并持续一段时间（通常是几十秒）。在这个状态下，操作系统认为该端口仍然被占用。

如果没有设置 `SO_REUSEADDR`，当你关闭服务器程序后立即尝试重启它，绑定到同一个监听端口时，操作系统会报错 **"Address already in use"** (地址已被使用)，因为端口还处于 `TIME_WAIT` 状态。

通过设置 `SO_REUSEADDR` 为 `1`，你告诉操作系统：

- “即使这个端口看起来正在被一个处于 `TIME_WAIT` 状态的旧连接占用，我也允许我的新程序绑定到它。”

这对于 <font color=24b3aa>**开发和测试**服务器程序</font> 来说极其方便，因为它允许你频繁地启动和停止服务器而无需等待。<font color=ee11ff>在 **生产环境** 中，它也能让服务器在崩溃或重启后更快地恢复服务。</font>

**需要注意的地方：**

<font color=24b3aa>虽然 `SO_REUSEADDR` 解决了“Address already in use”问题，但它并不能完全消除旧数据包干扰新连接的理论风险。不过，在实际应用中，这种风险通常很小，而且对于常见的服务器应用来说，快速恢复服务的重要性更高。</font>

## 14. `read` 时什么时候需要预留 1 个字节

先看下面的的代码：

```cpp
#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage : %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  
    // 1. 创建服务器 socket
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) {
        error_handling("socket() errror");
    }

    // 2. 绑定地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    // 端口复用
    int optval = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 3. 打开监听
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    // 4. 接受客户端
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz = sizeof(clnt_addr);
    if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz)) == -1) {
        error_handling("accept() error");
    }
    printf("client connected: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    // 5. 发送数据
    FILE* file = fopen("./data.txt", "rb");
    if(file == NULL) {
        error_handling("fopen() error");
    }
    // 注意这里要先检查 file 是否为空再初始化 fd
    // 这是因为 fileno(NULL) 是未定义行为
    int fd = fileno(file);

    char buf[BUF_SIZE] = {0};
    ssize_t read_cnt;
    while((read_cnt = read(fd, buf, BUF_SIZE)) > 0)
    {
        if(write(clnt_sock, buf, read_cnt) != read_cnt) {
            fclose(file); // 注意要关闭文件
            error_handling("write() error");
        }
    }
    if(read_cnt == -1) {
        fclose(file); // 注意要关闭文件
        error_handling("read() error");
    }

    // 6. 半关闭
    shutdown(clnt_sock, SHUT_WR);

    // 7. 接受客户端最终消息
    memset(buf, 0, BUF_SIZE);
    // 预留一个字节给 \0
    ssize_t final_read_cnt = read(clnt_sock, buf, BUF_SIZE - 1);
    buf[final_read_cnt] = '\0'; // 确保字符串以空字符结尾
    if(final_read_cnt == -1) {
        fclose(file); // 注意要关闭文件
        error_handling("read() error");
    }
    else if(final_read_cnt == 0) {
        printf("Client closed connection after receiving file.\n");
    }
    else {
        printf("Message from client: %s\n", buf);
    }

    // 8. 清理资源
    fclose(file);
    close(serv_sock);
    close(clnt_sock);

    return EXIT_SUCCESS;
}
```

这是一段服务器代码，其中有两段 `read()` 很有意思：

* 第一段：

  ```cpp
  char buf[BUF_SIZE] = {0};
  ssize_t read_cnt;
  while((read_cnt = read(fd, buf, BUF_SIZE)) > 0)
  {
      if(write(clnt_sock, buf, read_cnt) != read_cnt) {
          // ...
      }
  }
  ```

* 第二段：

  ```cpp
  // 预留一个字节给 \0
  ssize_t final_read_cnt = read(clnt_sock, buf, BUF_SIZE - 1);
  buf[final_read_cnt] = '\0'; // 确保字符串以空字符结尾
  if(final_read_cnt == -1) {
      // ...
  }
  else if(final_read_cnt == 0) {
      // ...
  }
  else {
      printf("Message from client: %s\n", buf);
  }
  ```

这两者的区别在于，第一段的代码中，在 `read` 时没有为 `\0` 预留一个字节，而在第二段代码时，我们为 `\0` 预留了一个字节，为什么？

这其实是一个关于 **数据类型处理** 和 **C 语言字符串规范** 的关键问题。两段代码在 `read` 时是否需要预留一个字节给 `\0` 的拆，完全取决于它们读取的 **数据类型** 以及这些数据在程序中 **后续的用途**。

### 为什么第一段 `read` (文件数据) 不需要预留 `\0` 空间？

```c
while((read_cnt = read(fd, buf, BUF_SIZE)) > 0)
{
    if(write(clnt_sock, buf, read_cnt) != read_cnt) {
        // ... 错误处理 ...
    }
}
```

这段代码的目的是从文件 (`data.txt`) 中读取内容，并将这些内容**作为原始的二进制字节流**发送给客户端。

- **数据性质**：`data.txt` 可以是任何类型的文件（文本、图片、视频、压缩包等）。它的内容是纯粹的字节序列，**不一定符合 C 语言字符串的规范**。换句话说，文件内容中可能包含任意字节值，包括 `0x00`（即空字符 `\0`）。
- **后续用途**：`read` 到的数据会立即通过 `write` 系统调用发送到网络套接字。在 `read` 和 `write` 之间，你没有对 `buf` 中的内容进行任何C语言字符串操作，比如使用 `printf("%s")`、`strlen()` 或 `strcpy()`。你只是简单地将原始字节从一个地方搬运到另一个地方。
- **不添加 `\0` 的原因**：如果在这里预留一个字节并在 `read` 后强制添加 `\0`，你实际上会额外向网络发送一个 `\0` 字节。这会**破坏原始文件数据的完整性**。例如，如果你发送的是一张图片，额外添加的 `\0` 会导致图片文件损坏，因为它改变了原始的字节序列。
- **效率考量**：为了最大化每次 `read` 和 `write` 的数据吞吐量，通常会尽可能地填满缓冲区，即使用 `BUF_SIZE` 作为读取长度。

### 为什么第二段 `read` (客户端消息) 需要预留 `\0` 空间？

```c
ssize_t final_read_cnt = read(clnt_sock, buf, BUF_SIZE - 1);
buf[final_read_cnt] = '\0'; // 确保字符串以空字符结尾
// ...
printf("Message from client: %s\n", buf);
```

这段代码的目的是接收客户端发送过来的**文本消息**，并在服务器端将其**作为C语言字符串打印到控制台**。

- **数据性质**：你期望从客户端接收的是一段可读的**文本字符串**。
- **后续用途**：最关键的一点是，你随后使用了 `printf("Message from client: %s\n", buf);`。
- **`printf("%s")` 的要求**：在C语言中，`%s` 格式说明符要求其对应的参数是一个**以空字符 `\0` 结尾的字符串**。`printf` 函数会从 `buf` 的起始地址开始，一直打印字符直到遇到第一个 `\0`。如果 `buf` 中没有 `\0`，`printf` 就会继续读取超出 `buf` 内存边界的数据，这会导致**未定义行为**，很可能造成程序崩溃（段错误）或打印出内存中的垃圾数据（**缓冲区溢出**）。
- **防止缓冲区溢出**：
  - 如果你使用 `read(clnt_sock, buf, BUF_SIZE)` 来读取数据，并且客户端发送的数据长度恰好是 `BUF_SIZE` 个字节，那么 `buf` 数组会被完全填满。
  - 此时，`buf` 中就没有额外的空间来存放字符串的终止符 `\0`。
  - 当你接着执行 `buf[final_read_cnt] = '\0';` 时，由于 `final_read_cnt` 等于 `BUF_SIZE`，你将尝试写入 `buf[BUF_SIZE]`，这已经超出了 `buf` 数组的合法索引范围（`buf` 的有效索引是从 `0` 到 `BUF_SIZE - 1`）。这正是经典的**缓冲区溢出**。
  - 通过将 `read` 的长度限制为 `BUF_SIZE - 1`，你确保了 `buf` 中**至少会有一个字节的空间**留给 `\0`。无论 `read` 实际读取了多少字节（最大 `BUF_SIZE - 1`），`buf[final_read_cnt]` 都将是 `buf` 数组内的有效索引，并且你可以在这里安全地添加 `\0`，从而创建一个合法的C字符串供 `printf` 使用。

### 总结

核心区别在于**数据的预期类型和后续处理方式**：

- <font color=24b3aa>当你处理的是 <font color=blue>原始的、无结构的**二进制数据流**</font>（如文件内容传输），且不需要将其解析为 C 语言字符串时，应尽可能地填充整个缓冲区 (`BUF_SIZE`)，并且 <font color=blue>**不应该**手动添加 `\0`。</font></font>
- <font color=24b3aa>**当你预期接收到的是** **<font color=blue>C 语言字符串</font>**（例如用户输入的文本消息），并且会使用 C 标准库的字符串处理函数（如 `printf("%s")`、`strlen()`、`strcpy()` 等）时，你 <font color=blue>必须**为字符串终止符 `\0` 预留一个字节**（`BUF_SIZE - 1`），并在 `read` 操作完成后**手动添加 `\0`**，</font>以确保字符串的合法性和防止缓冲区溢出。</font>

理解这两种数据处理模式，是进行健壮网络编程的基础。

## 15. `connect()`

TCP 和 UDP 都可以调用 `connect()` 函数，但是它们之间有着本质的区别，具体是因为 <font color=24b3aa>TCP 是面向连接的协议，`connect()` 会触发真正的“三次握手”建立连接，并维护连接状态。而在 UDP 当中，`connect()` 仅仅是用来在内核记录端对端地址，提高 `sendto` 函数的性能。</font>

**UDP 的 `connect()` 作用**

1. **绑定对端地址，避免每次 `sendto`/`recvfrom` 重复指定**
   - 未连接时：每次发送 (`sendto`) 需要指定目标地址，接收 (`recvfrom`) 需要解析来源地址。
   - 连接后：可以直接用 `send()` 和 `recv()`，系统会自动关联已连接的地址。
2. **过滤非目标地址的数据包**
   - 内核会丢弃非 `connect()` 指定地址发来的数据，减少无关数据干扰。
3. **提升性能（部分场景）**
   - 避免多次地址解析和校验，适合频繁与同一对端通信的场景（如 DNS 客户端）。

**关键特点**

- **无连接协议的本质未变**
  UDP 仍然是“无连接”的，`connect()` 仅在内核记录对端地址，不进行握手或维护连接状态。
- **可多次调用**
  可以重新 `connect()` 到新地址，或调用 `connect(fd, NULL, 0)` 解除绑定（返回未连接状态）。
- **错误反馈**
  调用 `connect()` 时内核会检查目标地址的合法性（如路由是否可达），但不会真正建立连接。

**TCP 的 `connect()` 核心作用**

1. **主动发起三次握手**
   - 客户端调用 `connect()` 后，内核会向服务器发送 `SYN` 包，进入 `SYN_SENT` 状态，完成握手后变为 `ESTABLISHED`。
   - 若服务器未监听端口或拒绝连接，`connect()` 会返回错误（如 `ECONNREFUSED`）。
2. **阻塞与非阻塞模式**
   - **阻塞模式**：`connect()` 直到握手成功或超时（默认行为）。
   - **非阻塞模式**：`connect()` 可能立即返回 `EINPROGRESS`，需通过 `select`/`poll` 检查套接字可写性来判断是否成功。
3. **连接的唯一性**
   - 一个 TCP 套接字只能 `connect()` 一次（除非关闭后重新创建）。
   - 试图重复调用会返回 `EISCONN` 错误（已连接）或 `EALREADY`（非阻塞模式下未完成前次连接）。

## 16. DNS

<font color=24b3aa>需要注意，`gethostbyname()` （根据 IP 地址获得域名）和 `gethostbyaddr()`（根据域名获取 IP 地址） 在 POSIX.1-2001 中已被标记为 **obsolete（过时）**，在 POSIX.1-2008 中被正式移除。</font>

- **<font color=blue>不支持 IPv6</font>**：`gethostbyname` 只能返回 IPv4 地址。
- **<font color=blue>线程不安全</font>**：它们返回指向静态缓冲区的指针，多线程环境下会互相覆盖数据。
- **<font color=blue>不可重入</font>**：信号处理函数中调用会导致未定义行为。

<font color=24b3aa>`h_errno` 同样是**过时的全局变量**，线程不安全。</font>

- 现代代码应使用 `gai_strerror()` 配合 `getaddrinfo/getnameinfo` 的返回值来获取错误信息。
- 如果必须维护旧代码，注意某些系统上 `h_errno` 实际上是宏展开为线程局部存储，但行为不一致。

推荐使用协议无关、线程安全的替代函数：

```cpp
#include <netdb.h>
#include <sys/socket.h>

// 域名 -> IP（支持 IPv4/IPv6）
int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints, struct addrinfo **res);

// IP -> 域名（反向解析）
int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags);

// 释放结果链表
void freeaddrinfo(struct addrinfo *res);
```

例如：

```cpp
#include <iostream>
// g++ -std=c++17 -o dns_demo dns_demo.cpp
// ./dns_demo
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 辅助：打印 sockaddr 中的 IP 和端口
static void print_sockaddr(const struct sockaddr* sa, socklen_t len) {
    char ip[INET6_ADDRSTRLEN];
    uint16_t port = 0;

    if (sa->sa_family == AF_INET) {
        auto* s = reinterpret_cast<const struct sockaddr_in*>(sa);
        inet_ntop(AF_INET, &s->sin_addr, ip, sizeof(ip));
        port = ntohs(s->sin_port);
    } else if (sa->sa_family == AF_INET6) {
        auto* s = reinterpret_cast<const struct sockaddr_in6*>(sa);
        inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));
        port = ntohs(s->sin6_port);
    } else {
        std::cout << "  Unknown family: " << sa->sa_family << "\n";
        return;
    }
    std::cout << "  IP: " << ip << "  Port: " << port << "\n";
}

int main() {
    // ===================== 1. getaddrinfo: 域名 → IP =====================
    std::cout << "===== getaddrinfo (forward DNS) =====\n";
    {
        const char* hostname = "www.baidu.com";
        struct addrinfo hints{};
        hints.ai_family   = AF_UNSPEC;      // IPv4 + IPv6
        hints.ai_socktype = SOCK_STREAM;    // TCP
        hints.ai_flags    = AI_CANONNAME;   // 请求官方主机名

        struct addrinfo* res = nullptr;
        int err = getaddrinfo(hostname, "80", &hints, &res);
        if (err != 0) {
            std::cerr << "getaddrinfo failed: " << gai_strerror(err) << "\n";
            return 1;
        }

        for (auto rp = res; rp; rp = rp->ai_next) {
            std::cout << "[" << (rp->ai_family == AF_INET ? "IPv4" : "IPv6") << "] ";
            print_sockaddr(rp->ai_addr, rp->ai_addrlen);
        }
        if (res && res->ai_canonname)
            std::cout << "Canonical name: " << res->ai_canonname << "\n";

        // ✅ freeaddrinfo: 释放整个链表
        freeaddrinfo(res);
        std::cout << "(result freed)\n\n";
    }

    // ===================== 2. getnameinfo: IP → 域名 =====================
    std::cout << "===== getnameinfo (reverse DNS) =====\n";
    {
        const char* ip_str = "8.8.8.8";

        struct sockaddr_in sa{};
        sa.sin_family = AF_INET;
        sa.sin_port   = htons(53);
        if (inet_pton(AF_INET, ip_str, &sa.sin_addr) != 1) {
            std::cerr << "Invalid IP: " << ip_str << "\n";
            return 1;
        }

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        // NI_NAMEREQD: 必须有 PTR 记录才成功；去掉则允许回退到数字IP
        int err = getnameinfo(
            reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa),
            host, sizeof(host),
            serv, sizeof(serv),
            NI_NAMEREQD
        );

        if (err != 0) {
            std::cerr << "getnameinfo failed: " << gai_strerror(err) << "\n";
        } else {
            std::cout << "IP " << ip_str
                      << " -> Hostname: " << host
                      << ", Service: " << serv << "\n";
        }
        std::cout << "\n";
    }

    // ===================== 3. 综合：正向解析后立即反向验证 =====================
    std::cout << "===== Round-trip: forward then reverse =====\n";
    {
        struct addrinfo hints{};
        hints.ai_family   = AF_INET;       // 仅 IPv4，简化反向解析
        hints.ai_socktype = SOCK_STREAM;

        struct addrinfo* res = nullptr;
        int err = getaddrinfo("dns.google", "53", &hints, &res);
        if (err != 0) {
            std::cerr << "getaddrinfo failed: " << gai_strerror(err) << "\n";
            return 1;
        }

        // 取第一个结果做反向解析
        if (res) {
            char host[NI_MAXHOST], serv[NI_MAXSERV];
            err = getnameinfo(res->ai_addr, res->ai_addrlen,
                              host, sizeof(host),
                              serv, sizeof(serv), 0);
            if (err == 0) {
                std::cout << "Forward: dns.google:53\n";
                std::cout << "Reverse: " << host << ":" << serv << "\n";
            } else {
                std::cerr << "getnameinfo failed: " << gai_strerror(err) << "\n";
            }
        }

        // ✅ 别忘了释放
        freeaddrinfo(res);
        std::cout << "(result freed)\n";
    }

    return 0;
}
```

## 17. epoll 的底层原理与触发模式

**epoll 为什么高效？**

- `select`/`poll` 每次调用都要把包含所有文件描述符（fd）的集合从用户态拷贝到内核态，并且内核需要线性遍历（$O(N)$）所有 fd 来检查状态。
- `epoll` 在内核中维护了一个**红黑树**（用于存储所有被监听的 fd，<font color=blue>增删查的时间复杂度是 $O(\log N)$）</font>和一个**双向链表**（就绪队列）。
- 当网卡收到数据触发硬件中断时，内核会调用回调函数，将对应的 fd 放入 **就绪队列**。<font color=blue>调用 `epoll_wait` 时，只需以 $O(M)$ 的时间复杂度将就绪队列中的事件拷贝到用户态（M 为**当前处于就绪状态的事件数量**。）</font>，完全避免了无意义的遍历。

**ET（边缘触发）与 LT（水平触发）的区别：**

- **LT 模式：** 只要 fd 缓冲区里还有数据，`epoll_wait` 就会一直不停地返回该事件。这是<font color=blue>默认模式</font>，<font color=blue>开发相对简单，但系统调用开销大</font>。
- **ET 模式：** 只有当 fd 的状态发生变化时（例如从不可读变成可读），`epoll_wait` 才会返回一次。

**<font color=blue>为什么 ET 模式必须搭配非阻塞 Socket？</font>**

- 因为 ET 模式只会通知一次，所以你必须在一个循环里不断调用 `read`，直到把缓冲区读空（返回 `EAGAIN` 错误）。如果使用阻塞 Socket，最后一次 `read` 会因为没有数据可读而死锁，直接阻塞整个线程。

**<font color=blue>那边缘触发不就实际上成为水平触发了吗？</font>**

表面上看，大家最终都要把缓冲区的数据读空，确实殊途同归。但它们在**驱动方式**和**性能开销**上有本质的区别，ET 并没有“变成” LT。

核心区别在于**谁来负责把数据读空**：

1. **<font color=ee11ff>系统调用的开销不同（内核态 vs 用户态）</font>**
   - **LT（水平触发）** 是**内核**在催你。如果你一次没读完，内核会在你每次调用 `epoll_wait` 时不断返回事件唤醒你。这会导致频繁的用户态与内核态切换。
   - **ET（边缘触发）** 是**你的代码**在循环。内核只通知你一次，后续的 `read` 循环完全发生在用户态。它通过将多次通知合并为一次，极大地减少了 `epoll_wait` 的系统调用开销。
2. **<font color=ee11ff>多线程并发的安全性</font>** 在多线程环境下，LT 模式容易引发事件的重复通知。比如线程 A 正在处理某个 Socket 的数据，还没读完，此时线程 B 调用 `epoll_wait`，又会拿到这个 Socket 的就绪事件，导致混乱。而 ET 模式每次状态突变只触发一次，天生契合多线程事件分发（搭配 `EPOLLONESHOT` 食用更佳）。
3. **<font color=ee11ff>调度权与灵活度</font>**
   - **LT 允许你“雨露均沾”**：为了防止某个连接数据量太大饿死其他连接，你可以每次只读 1KB，剩下的交由下一次 `epoll_wait` 调度。
   - **ET 强制你“买定离手”**：把控制权完全交给了应用程序，强制你必须用 `while(read)` 榨干缓冲区，否则数据就会滞留，直到下一次有新数据到来才能再次触发。

## 18. 零拷贝技术（Zero-copy）

**传统 I/O 瓶颈：** 从磁盘读文件再通过网络发送，通常需要 4 次上下文切换（用户态 $\leftrightarrow$ 内核态）和 4 次数据拷贝（2 次 CPU 拷贝，2 次 DMA 拷贝）。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost2/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F/%E9%9B%B6%E6%8B%B7%E8%B4%9D/%E4%BC%A0%E7%BB%9F%E6%96%87%E4%BB%B6%E4%BC%A0%E8%BE%93.png)

**mmap (内存映射)：** 将内核读取缓冲区的地址与用户空间的缓冲区进行映射，减少了一次 CPU 拷贝。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost2/操作系统/零拷贝/mmap %2B write 零拷贝.png)

**sendfile：** 在 Linux 中，`sendfile` 系统调用可以直接在内核态将数据从文件描述符传递给 Socket 描述符。配合网卡支持的 SG-DMA（Scatter-Gather），可以实现**真正的零 CPU 拷贝**，只有 2 次上下文切换和 2 次 DMA 拷贝。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost2/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F/%E9%9B%B6%E6%8B%B7%E8%B4%9D/senfile-3%E6%AC%A1%E6%8B%B7%E8%B4%9D.png)

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost2/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F/%E9%9B%B6%E6%8B%B7%E8%B4%9D/senfile-%E9%9B%B6%E6%8B%B7%E8%B4%9D.png)

## 19. Reactor / Proactor 模型

无论是 Reactor，还是 Proactor，都是一种基于**<font color=blue>「事件分发」</font>**的网络编程模式，区别在于 Reactor 模式是基于**<font color=blue>「待完成」</font>**的 I/O 事件，而 Proactor 模式则是基于**<font color=blue>「已完成」</font>**的 I/O 事件。

- **Reactor 是 <font color=blue>同步非阻塞</font> 网络模式，分发的是 <font color=blue>就绪可读写事件</font>**。在每次感知到有事件发生（比如可读就绪事件）后，就需要应用进程主动调用 read 方法来完成数据的读取，也就是 <font color=24b3aa>要应用进程主动将 socket 接收缓存中的数据读到应用进程内存中，这个过程是同步的，读取完数据后应用进程才能处理数据。</font>
- **Proactor 是 <font color=blue>异步</font> 网络模式， 分发的是 <font color=blue>已完成的读写事件</font>**。在发起异步读写请求时，需要传入数据缓冲区的地址（用来存放结果数据）等信息，这样系统内核才可以自动帮我们把数据的读写工作完成，这里的读写工作全程由操作系统来做，并不需要像 Reactor 那样还需要应用进程主动发起 read/write 来读写数据，<font color=24b3aa>操作系统完成读写工作后，就会通知应用进程直接处理数据。</font>

因此，**Reactor 可以理解为「来了事件操作系统通知应用进程，让应用进程来处理」**，而 **Proactor 可以理解为「来了事件操作系统来处理，处理完再通知应用进程」**。这里的「事件」就是有新连接、有数据可读、有数据可写的这些 I/O 事件这里的「处理」包含从驱动读取到内核以及从内核读取到用户空间。

## 20. 惊群效应（Thundering Herd）

多进程/多线程并发环境下的内核级坑点。

- **现象：** 多个线程/进程同时阻塞等待同一个事件（比如多个线程 `epoll_wait` 同一个 listen socket）。当一个新连接到来时，内核会唤醒所有线程，但最终只有一个线程能成功 `accept`，<font color=blue>其他线程都会收到 `EAGAIN` 并重新休眠。这会导致严重的 CPU 上下文切换浪费。</font>

- **解决：**

  - **Linux 内核层面：** 内核 2.6 版本后，`accept` 的惊群已经解决，只会唤醒一个等待的进程。

    > Linux 2.6 解决 `accept` 惊群的核心机制非常简单，就是给内核的等待队列（Wait Queue）引入了一个**排他性标记**（`WQ_FLAG_EXCLUSIVE`）。
    >
    > 具体分为这 3 个关键动作：
    >
    > 1. **打上独占标签**：当多个进程或线程调用 `accept()` 拿不到连接进入阻塞休眠时，内核会把它们加到 socket 的等待队列中，并自动给它们打上 `WQ_FLAG_EXCLUSIVE` 标记，表示它们是来“独占”处理这个连接的。
    > 2. **唤醒时及时收手**：当有新连接完成三次握手、进入全连接队列时，内核开始遍历等待队列叫醒进程。一旦叫醒了**第一个**带有 `WQ_FLAG_EXCLUSIVE` 标记的进程，内核的唤醒逻辑就会直接 `break` 退出循环，不再去碰后面的进程。
    > 3. **巧妙的队列排序**：为了防止这种“叫醒一个就中断”的操作，误伤到等待队列里其他不需要独占的普通进程，内核默认会把打了独占标记的进程统一挂载到等待队列的队尾。

  - **epoll 层面：** Linux 4.5 引入了 `EPOLLEXCLUSIVE` 标志位，保证事件到来时只唤醒挂载在该 fd 上的一个进程，解决了 `epoll_wait` 的惊群。

- **Nginx 的经典解法**：在 Linux 内核还没从底层解决 epoll 惊群之前，Nginx 是在应用层自己搞定的。它利用了跨进程的 `accept_mutex` 锁，保证同一时刻只有一个 worker 进程把 listen socket 加到自己的 epoll 里，谁抢到锁谁才去监听，直接从源头掐断了惊群。

- **性能杀手本质**：惊群最可怕的不是唤醒本身，而是**高频的无效上下文切换（Context Switch）**。并发量一上来，CPU 资源全被内核层的线程调度给吃干榨净了，真正的业务逻辑根本分不到时间片。

## 21. C10K 问题

为了让我们的服务端能够同时支撑 1 万个甚至更多的并发连接（也就是解决著名的 C10K 问题），我们有以下思路：

#### 阶段一：为什么不能简单地用“多线程/多进程”？

最直观的改造想法是：主线程不断 `accept`，每收到一个新连接，就 `new` 一个新线程去处理。

- **致命瓶颈：** Linux 下一个线程的默认栈大小通常是 8MB。如果创建 1 万个线程，<font color=blue>光是内存消耗就需要将近 80GB</font>。此外，<font color=blue>1 万个线程在内核态进行上下文切换（Context Switch）的 CPU 开销极其庞大</font>，会导致系统负载飙升，甚至根本无法响应请求。
- **结论：** “一连接一线程（Thread-per-connection）”的模型在海量并发前是走不通的。

#### 阶段二：核心突围 —— 引入 `epoll` 与非阻塞 I/O

<font color=24b3aa>要想用少量线程管理海量连接，必须引入 **I/O 多路复用**。</font>在 Linux 下，当仁不让的选择就是 `epoll`。

1. **改造 Socket 为非阻塞（Non-blocking）：** 利用 `fcntl` 将 Listen Socket 和后续所有接收到的 Client Socket 都设置为非阻塞模式。这样 `accept`、`read`、`write` 就不会卡死当前线程。
2. **引入 epoll 事件循环：**
   - 调用 `epoll_create` 创建一个 epoll 实例。
   - 把 Listen Socket 注册到 epoll 中，监听可读事件（`EPOLLIN`）。
   - 主循环从原本的 `accept` 阻塞，改为 `epoll_wait` 阻塞。
3. **事件分发（Dispatcher）：**
   - 当 `epoll_wait` 返回时，遍历触发事件的 fd。
   - 如果是 **Listen Socket** 触发，说明有新客户端连接。在一个 `while` 循环里不断调用 `accept`，直到返回 `EAGAIN`，并把所有新的 Client Socket 注册到 epoll 中。
   - 如果是 **Client Socket** 触发，说明客户端发来了数据，此时调用 `read` 读取并处理。

通过这一步改造，我们实现了**单线程管理 1 万个连接**。由于绝大多数网络连接在同一时刻是静默的（Idle），`epoll_wait` 只需要处理活跃的连接，效率极高。

#### 阶段三：架构升华 —— 主从 Reactor 多线程模型

单线程 `epoll` 虽然解决了网络连接的并发，但在我的**分布式缓存系统**场景下，收到请求后还需要进行协议解析、哈希计算、加锁访问内存缓存。如果某个请求的业务逻辑耗时较长，就会霸占这唯一的线程，导致其他已经就绪的网络 I/O 得不到处理。

因此，最终的工业级改造方案是采用 **<font color=blue>主从 Reactor 多线程架构（Main-Sub Reactor）</font>**：

1. **主 Reactor (Main Reactor)：**
   - 只负责一件事情：监听和处理新连接。
   - 它运行在一个独立的线程中，拥有自己的 `epoll` 实例，专门监控 Listen Socket。
   - 监听到新连接后，调用 `accept` 拿到 `client_fd`，然后将其**分发（通常使用轮询 Round Robin）**给后端的 Sub Reactor。
2. **从 Reactor (Sub Reactor 线程池)：**
   - 这是一组工作线程（数量通常等于 CPU 核心数，比如 4 或 8 个）。
   - 每个 Sub Reactor 都有**自己独立的 `epoll` 实例**。
   - 它们接收主 Reactor 丢过来的 `client_fd`，并将其注册到自己的 `epoll` 中。
   - 之后，这个连接上的读写、协议解析、缓存数据获取以及响应下发，都由这个 Sub Reactor 线程闭环完成。

---

**总结来说：** 对那份几十行的基础代码的改造，本质上是从 **“同步阻塞”** 升级到 **“I/O 多路复用”**，最后升华到 **“Reactor 异步事件驱动架构”**。这种架构让网络 I/O 的收发与业务逻辑的执行完美解耦，这也是 Nginx、Netty 乃至 Redis 能支撑超高并发的核心秘诀所在。

## 22. One Loop Per Thread 模型

**One Loop Per Thread (OLPT)** 是一种在高性能服务器、游戏引擎和实时系统中广泛采用的并发编程模型。

它的核心思想非常直观：**<font color=blue>每个线程都拥有自己独立的事件循环（Event Loop），并独占处理分配给该线程的任务队列，线程之间不共享任务队列。</font>**

这种模型与传统的“多线程共享单一任务队列”模型形成了鲜明对比。以下是关于该模型的详细解析：

| 特性       | 传统线程池模型 (Shared Queue)    | One Loop Per Thread 模型                  |
| ---------- | -------------------------------- | ----------------------------------------- |
| 任务队列   | 所有线程竞争同一个全局队列       | 每个线程拥有私有队列                      |
| 锁竞争     | 高（入队/出队都需要加锁）        | 极低（仅跨线程通信时需要锁/原子操作）     |
| 缓存局部性 | 差（多线程频繁访问同一内存区域） | 好（数据主要在当前线程的 L1/L2 Cache 中） |
| 任务调度   | 动态窃取或竞争获取               | 静态绑定或显式投递                        |
| 适用场景   | 通用后端服务、短任务批处理       | 游戏逻辑、UI框架、网络IO、实时仿真        |

**One Loop Per Thread 的优势：**

- **<font color=ee11ff>消除锁争用（Lock Contention）</font>**：在传统模型中，当核心数增加时，所有线程抢夺同一个队列的锁会成为性能瓶颈。OLPT 将队列私有化，使得线程在执行自己的任务时完全无锁。
- **<font color=ee11ff>极致的缓存友好（Cache Locality）</font>**：由于线程只处理自己队列里的任务，相关的数据结构更有可能驻留在 CPU 的高速缓存中，减少了昂贵的内存访问（Cache Miss）。
- **确定性与可预测性**：任务被显式分配到特定线程，避免了操作系统或线程池调度器带来的随机延迟抖动。这对于需要稳定帧率的游戏或低延迟网络服务至关重要。
- **简化状态管理**：如果某个对象（如一个玩家实体、一个网络连接）始终由同一个线程处理，那么对该对象的读写就不需要加锁，天然实现了“单线程所有权”。

<font color=blue>OLPT 并非银弹，它引入了新的复杂性：</font>

- **<font color=ee11ff>负载均衡问题：</font>**如果某个线程的任务堆积，而其他线程空闲，系统整体吞吐量会下降。
  - *解决*：引入**工作窃取（Work Stealing）**机制（如 Tokio, Go Runtime），允许空闲线程从繁忙线程的队列尾部“偷”任务；或者采用更智能的**一致性哈希/轮询分发**策略。
- **<font color=ee11ff>跨线程通信成本：</font>**线程间不能直接共享数据，必须通过消息传递。
  - *解决*：使用**无锁环形缓冲区（Lock-free Ring Buffer）**、Channel 或 Actor 消息系统。虽然比直接读内存慢，但比全局锁快得多。
- <font color=ee11ff>编程心智负担：</font>开发者需要时刻思考“这段代码在哪个线程运行？”以及“如何安全地把结果传回去？”。
  - *解决*：现代异步运行时（如 Rust 的 Tokio/async-std, C++ 的 Seastar, Java 的 Vert.x）封装了底层细节，提供了 `spawn_local`, `send`, `await` 等高级抽象。

**典型应用场景**

1. 游戏服务器/客户端：
   - 主线程负责渲染/UI。
   - 物理线程负责碰撞检测。
   - 网络线程负责收发包。
   - 各线程通过消息队列同步状态，保证主循环不被阻塞。
2. 高性能网络代理（如 Envoy, Nginx）：
   - 每个 Worker 线程绑定一个 Socket 和 Event Loop。
   - 连接一旦建立就绑定到特定 Worker，后续 IO 全在该线程完成，避免上下文切换。
3. 现代异步运行时（Rust Tokio / Node.js Worker Threads）：
   - Tokio 的多线程调度器本质上就是 OLPT + Work Stealing。
   - Node.js 主线程是单 Loop，Worker Threads 则是额外的独立 Loop。
4. GUI 应用程序：
   - UI 线程永远只有一个 Loop，后台计算通过 Future/Promise 回调回 UI 线程更新界面。

总而言之，**One Loop Per Thread** 是用 <font color=blue>**“空间换时间”** 和 **“通信换同步”** 的典型实践。</font>

- 如果你的系统是 **CPU 密集型且任务高度独立**，或者对 **延迟极其敏感**，OLPT 通常是优于传统线程池的选择。
- 如果你的任务是 **大量同质化的短请求**（如简单的 HTTP CRUD API），传统的共享队列线程池可能实现更简单且足够高效。

## 23. 在高并发网络框架中，如何避免频繁的内存分配带来的开销？

**考点**：内存池（Memory Pool）和对象池（Object Pool）的设计。对于频繁创建销毁的 `Connection` 对象、读写 Buffer，通常会预先分配内存块以减少系统调用和内存碎片。

## 24. 如果服务器有 100 万个并发连接，如何高效地管理超时和心跳？

**考点**：<font color=blue>**绝对不能用一个线程去轮询 100 万个连接。**</font>

面对 100 万级别的高并发连接，传统的定时器方案（例如基于红黑树或最小堆的 `std::priority_queue`）会遇到难以逾越的瓶颈。虽然最小堆获取最小超时时间的时间复杂度是 $O(1)$，但每次收到心跳包需要更新超时时间时，删除或调整堆中元素的时间复杂度是 $O(\log N)$。在百万并发且心跳频繁的场景下，这会消耗极其庞大的 CPU 资源。

<font color=24b3aa>在构建高性能底层网络库或手写 RPC 框架时，业界公认最高效的超时和心跳管理方案是 <font color=blue>**时间轮（Timing Wheel）**结合**延迟删除（Lazy Deletion）**机制</font></font>。

以下是实现这种高效管理的几个核心设计：

> **<font color=ee11ff>1. 核心数据结构：时间轮 (Timing Wheel)</font>**

时间轮本质上是一个环形数组（Circular Array），数组的每个元素是一个链表或哈希集合（Bucket），代表一个时间刻度（Tick）。

- **$O(1)$ 的极致性能**：无论是添加新的定时任务，还是随着时间推进触发超时任务，时间轮的时间复杂度都是绝对的 $O(1)$。
- **层级时间轮 (Hierarchical Timing Wheel)**：如果超时跨度很大（例如既有 100 毫秒级别的 RPC 请求超时，又有 2 小时级别的 TCP Keep-Alive），单层时间轮会占用过多内存。此时可以采用类似水表的层级设计（毫秒轮、秒轮、分轮）。当高层级的指针移动时，再将其中的任务降级放入低层级轮中。

> **<font color=ee11ff>2. 与 One loop per thread 的完美结合</font>**

绝对不能使用一个全局的时间轮来管理这 100 万个连接，那会引发灾难性的锁竞争。

- **线程局部时间轮**：每个工作线程（Sub-Reactor）的 Event Loop 都维护一个**专属的时间轮**。
- 如果 10 个工作线程处理 100 万并发，每个线程只负责 10 万个连接的超时逻辑。定时器的添加、更新和触发全部在同一个线程内完成，**全程无锁**。
- Event Loop 每次迭代时，利用 `epoll_wait` 的 timeout 参数作为时间轮的 Tick 驱动力，或者利用 Linux 的 `timerfd` 将时间事件转化为 I/O 事件统一监听。

> **<font color=ee11ff>3. 心跳更新机制：引用计数与延迟删除 (C++ 经典实践)</font>**

当客户端发来心跳包时，我们需要延长该连接的超时时间。如果去时间轮的旧 Bucket 中寻找并删除旧节点，开销依然很大。在 C++ 中，利用智能指针可以极其优雅地解决这个问题。

- **连接上下文映射**：将连接对象封装为 `std::shared_ptr<Connection>`。
- **时间轮 Bucket 的内容**：时间轮的每个槽（Bucket）存放的是一个包含 `std::shared_ptr<Connection>` 的集合（例如 `std::unordered_set`）。
- **心跳刷新**：当收到某连接的心跳时，**不要去旧的 Bucket 里删除它**。而是直接将该连接的 `shared_ptr` 重新插入到当前时间指针加上超时时间对应的**新 Bucket** 中。
- **自动释放**：随着时间的推移，旧的 Bucket 会被时间指针扫到并清空。但此时只要该连接还在更靠后的新 Bucket 中，它的引用计数就不会归零，连接对象依然存活。只有当一个连接在整个超时周期内都没有发来任何数据，它在所有 Bucket 中的拷贝都被清空，引用计数才会降为 0，从而触发析构函数，真正关闭底层 Socket。

> **<font color=ee11ff>4. 隔离两类不同的超时</font>**

在具体的 RPC 模块实现中，为了极致的性能，通常会将两类超时分开管理：

- **连接级别心跳 (Keep-Alive)**：容忍度较高，精度要求低（例如 30 秒无响应则断开）。直接使用上述的时间轮机制。
- **单个请求超时 (RPC Timeout)**：精度要求高（例如 50ms 必须返回），且与具体的 Request ID 绑定。这类超时通常依附于具体的并发调用逻辑中，可以使用更轻量级的单次定时器，或者在将 RPC 请求放入发起方的发送队列时附带时间戳，在遍历队列或收到响应时进行超时校验。

通过将时间轮下放到每个工作线程，并利用 C++ 的智能指针特性实现无锁的延迟踢出，即便连接数飙升到百万，管理心跳的开销也能稳定在极低的水平。

## 25. 在非阻塞 Socket 上调用 `send` 或 `recv` 返回 -1，且 `errno` 为 `EAGAIN` 或 `EWOULDBLOCK`，这代表什么？

在非阻塞（Non-blocking）网络编程中，调用 `send` 或 `recv` 返回 `-1` 并且 `errno` 被设置为 `EAGAIN` 或 `EWOULDBLOCK`，**<font color=24b3aa>这并不是一个真正的错误，而是一种正常的系统状态反馈。</font>**

<font color=24b3aa>它的核心含义是：**“当前操作如果继续执行将会阻塞线程，因为资源暂时不可用，请稍后再试。”** </font>（在大多数现代操作系统如 Linux 中，`EAGAIN` 和 `EWOULDBLOCK` 的值是完全相同的）。

具体到读写操作，它代表着以下两种截然不同的物理状态：

**1. 对于读 `recv` (或 `read`)**

- **物理状态**：操作系统内核中，该 Socket 的**接收缓冲区（Receive Buffer）是空的**。
- **含义**：目前网络上没有任何新的数据到达。如果是阻塞模式，线程会被挂起（休眠）直到有数据到来；但在非阻塞模式下，内核为了防止线程卡死，会立即返回 `-1` 并置位 `EAGAIN`。
- **处理动作**：停止当前的读取操作。将这个 Socket FD 重新交还给 Event Loop（例如 `epoll`），等待下一次触发 **可读事件（`EPOLLIN`）** 时，再继续调用 `recv`。

**2. 对于写 `send` (或 `write`)**

- **物理状态**：操作系统内核中，该 Socket 的**发送缓冲区（Send Buffer）已经满了**，或者剩余空间不足以容纳你这次想要发送的数据量。
- **含义**：这通常是因为对端接收太慢（触发了 TCP 的滑动窗口控制），或者网络极度拥塞。内核无法立刻将你的用户态数据拷贝进内核空间。
- **处理动作**：**千万不要在一个紧凑的 `while` 循环里死命重试发送**（这会导致 CPU 瞬间飙升到 100%）。正确的做法是：
  1. 记录下当前还没发送完的数据（存入该连接对应的用户态 Output Buffer 中）。
  2. 向 Event Loop 注册该 Socket 的 **可写事件（`EPOLLOUT`）**。
  3. 当内核发送缓冲区有空闲空间时，`epoll_wait` 会唤醒你的线程，此时再从用户态 Output Buffer 中取出剩余数据继续 `send`。
  4. 数据全部发送完毕后，**务必注销**可写事件（否则 `EPOLLOUT` 会被疯狂触发，导致 CPU 空转）。

## 26. 什么是 `SO_REUSEADDR` 和 `SO_REUSEPORT`？在什么场景下必须设置它们？

这两个套接字选项（Socket Options）在构建高可用、高性能的底层网络库时是不可或缺的。虽然它们的名字看起来很像，但解决的却是网络编程中两个完全不同的痛点：**重启恢复** 与 **并发性能**。

以下是它们的具体含义以及必须设置的场景：

**<font color=red>1、`SO_REUSEADDR` (重用地址)</font>**

**它是什么？**

<font color=blue>`SO_REUSEADDR` 的核心作用是告诉操作系统内核：**允许将一个套接字绑定到一个处于 `TIME_WAIT` 状态的本地地址和端口上。**</font>

**底层逻辑：**

在 TCP 协议中，主动关闭连接的一方（比如服务端主动断开客户端，或者服务端进程直接崩溃/被杀死）最终会进入 `TIME_WAIT` 状态。这个状态通常会持续 2 倍的 MSL（Maximum Segment Lifetime，通常为 1 到 4 分钟）。在这几分钟内，操作系统内核会认为这个端口依然“被占用”。

**在什么场景下必须设置？**

- **几乎所有的 TCP 服务端都必须在 `bind()` 之前设置它。**
- **快速重启（Crash Recovery / Rolling Update）：** 如果你的服务端程序崩溃了，或者你需要进行常规的发版重启。如果没有设置 `SO_REUSEADDR`，当你立刻启动新进程尝试绑定同一个端口时，`bind()` 会直接报错 `EADDRINUSE` (Address already in use)，你只能无奈地等待几分钟直到内核清理掉那些 `TIME_WAIT` 的连接。设置了该选项后，新进程就可以立刻接管该端口。

**<font color=red>2、 `SO_REUSEPORT` (重用端口)</font>**

**它是什么？**

<FONT COLOR=BLUE>这是在较新的内核（Linux 3.9+）中引入的极其强大的特性。它允许**多个不同的套接字（不管是来自同一个进程的不同线程，还是完全不同的进程）同时绑定到完全相同的 IP 地址和端口上。**</FONT>

**底层逻辑：**

在没有 `SO_REUSEPORT` 之前，如果多个线程想处理同一个端口的新连接，通常只能由一个主线程监听，然后把连接分发给工作线程；或者多个线程同时阻塞在一个套接字的 `accept()` 上，这会引发著名的**“惊群效应”（Thundering Herd）**——一个新连接到来，所有线程被唤醒，但只有一个能 `accept` 成功，其他的全被浪费了。

<FONT COLOR=BLUE>启用了 `SO_REUSEPORT` 后，内核会在底层接管流量分发：内核会基于客户端的 IP 和端口进行哈希，将传入的 TCP `SYN` 包（新连接请求）极其均匀地分配给绑定在该端口的多个套接字。</FONT>

**在什么场景下必须设置？**

- **极致性能的多核并发架构：** 当你需要彻底消除单线程 `accept` 的瓶颈时。结合我们之前讨论的 "One loop per thread" 模型，你可以让每个工作线程都创建一个监听套接字并绑定同一端口，内核直接把新连接喂到各个线程的专属 Event Loop 中，实现了从接收到处理的**全程无锁化**。
- **热更新（Zero-Downtime Upgrade）：** 启动新的服务端进程（绑定相同端口），此时内核会自动将新连接分发给新老进程。然后通知老进程优雅关闭（处理完已有连接后退出），整个过程对客户端完全透明，不会丢失任何连接请求。

> 总结对比

| **特性**           | **核心解决的问题**               | **作用域**               | **典型应用场景**           |
| ------------------ | -------------------------------- | ------------------------ | -------------------------- |
| **`SO_REUSEADDR`** | 绕过`TIME_WAIT` 状态的端口锁定   | 单个新套接字重用旧地址   | 服务端崩溃后的立刻重启     |
| **`SO_REUSEPORT`** | 操作系统级别的负载均衡与请求分发 | 多个活动套接字共享同地址 | 多线程无锁监听、无缝热升级 |

这两个选项通常使用 `setsockopt` 函数在套接字创建之后、`bind` 之前进行设置。

## 27. 如何安全的管理 Socket fd 和业务对象的周明周期

在网络编程中，如果网络连接断开，但对应的业务逻辑还在异步执行，如何安全地管理 Socket fd 和业务对象的生命周期？（期望听到对 RAII 和 `std::shared_ptr` / `std::weak_ptr` 的应用）。

## 28. 大量的 CLOSE_WAIT 与 TIME_WAIT

**面试官关注点：** 真实的排障经验和对 TCP 状态机的熟悉度。

- **CLOSE_WAIT 的产生：**
  - 被动关闭方收到客户端的 FIN 包，内核自动回复 ACK，此时被动关闭方进入 `CLOSE_WAIT` 状态。
  - **排查思路：** 出现大量 `CLOSE_WAIT` **绝对是服务端代码有 Bug**。通常是因为业务代码卡死（例如死锁、访问数据库超时且没设超时时间）、线程池耗尽，导致服务端没有执行 `close(fd)` 去发送自己的 FIN 包。
- **TIME_WAIT 的产生：**
  - 主动关闭方（谁先发 FIN 谁就是主动方）在完成四次挥手后，必须在这个状态停留 2MSL（报文最大生存时间，通常是 60 秒）。
  - **目的：** 确保最后一个 ACK 能到达对方（丢失可重传），并保证网络中旧连接的残留数据包自然消亡。
  - **影响与优化：** 大量 `TIME_WAIT` 会耗尽本地端口号（默认 3 万多个）。可通过<FONT COLOR=BLUE>开启系统内核参数 `tcp_tw_reuse`</FONT>（复用处于 TIME_WAIT 状态的端口），或<FONT COLOR=BLUE>在业务层使用**连接池**</FONT>来减少频繁的短连接。

## 29. 应用层心跳 vs TCP Keepalive

**面试官关注点：** 协议分层思想，为什么系统有的功能应用层还要造轮子。

- **TCP Keepalive 的局限性：** 它是传输层（L4）机制，默认 2 小时才发一次探测。它只能判断“网络连通性”和“对端操作系统是否崩溃”。
- **为什么需要 HTTP/RPC 心跳：** 如果服务端的进程发生了**死锁、无限循环、或者因为 Full GC 停顿**，底层的 TCP 连接依然是正常的，但应用层已经无法处理请求了。你的 Go RPC 框架通过 HTTP 心跳（L7 应用层），能够真实地反映服务节点的“业务健康度”，从而及时进行路由剔除。

简单来说，**TCP Keepalive（保活机制）** 就像是网络底层自动进行的一场“心跳检测”或者“定期查岗”。它的核心作用是**检测一个长时间没有数据传输的 TCP 连接是否还真实存活**。

> 为什么需要 TCP Keepalive？

TCP 是一种有状态的连接。在正常情况下，双方通过三次握手建立连接后，如果没有数据要传输，这个连接就会一直安静地挂载着。

但网络环境是不可靠的。如果这个时候发生以下意外：

- 客户端突然断电或死机。
- 网线被拔掉。
- 中间的路由器发生故障。

因为没有经过正常的四次挥手（没有发送 `FIN` 或 `RST` 包），服务器端是完全不知情的。它会认为客户端还在，继续维持着这个所谓的“半打开（Half-Open）”连接。如果这样的“死连接”越来越多，服务器的 Socket 资源（比如文件描述符、内存）就会被白白耗尽。

> 它的工作原理是什么？

当你在 Socket 上开启了 TCP Keepalive 功能后，操作系统底层会接管这项工作，通常依赖三个核心参数：

1. **Keepalive Time（空闲时间）**：如果在这个设定的时间内（比如默认的 2 小时或者自定义的 2 小时）双方没有任何数据交互。
2. **Keepalive Interval（探测间隔）**：系统会向对方发送一个极小的探测包（Probe，通常没有载荷或者只有1个字节）。
3. **Keepalive Probes（探测次数）**：如果对方没有响应，系统会按照设定的间隔时间连续发送几次。

**检测结果的处理方式：**

- **对方正常存活**：收到探测包后回复一个 `ACK`。本地机器收到回复，确认连接正常，重新开始计算空闲时间。
- **对方主机崩溃或网络断开**：发送完所有规定次数的探测包后，依然没收到任何回应。系统会判定连接已死，并主动在本地关闭这个连接，释放资源。
- **对方主机崩溃并已经重启**：对方已经丢失了连接状态，收到探测包后会回复一个 `RST`（重置）包。本地收到后，会立刻关闭连接。

> TCP Keepalive 的两大主要应用场景

- **清理无效连接（防泄漏）**：在服务端及时发现并踢掉那些因为意外已经断开的客户端，防止资源枯竭。
- **防止连接被防火墙/NAT 悄悄切断（保活）**：许多网络中间设备（如 NAT 路由器、防火墙）为了节省自身的内存，会偷偷清理掉那些长时间没有数据活动的连接。TCP Keepalive 发送的探测包能产生微小的网络流量，相当于在告诉这些设备：“我们还在用这个连接，请不要清理它的记录。”

> 为什么有了 TCP Keepalive 还要有应用层心跳机制？

简单来说：**TCP Keepalive 只能证明“网络通道还在、操作系统还活着”，而应用层心跳才能证明“你的程序真的还能干活”。**

以下是为什么在有了底层 TCP 保活之后，我们在构建 RPC 框架或高可用服务时，依然必须实现应用层心跳的几个核心原因：

**1. 致命的“进程假死”问题（关注层级不同）**

TCP Keepalive 是由**操作系统内核（OS Kernel）**在网络协议栈（L4 传输层）层面接管并回复的。

- **TCP 的视角**：只要服务器没断电、网线没拔、内核没崩溃，内核就会自动回复探测包的 `ACK`。
- **应用的视角**：如果你的 RPC Server 进程发生了**死锁、死循环、OOM 导致频繁 GC 停顿**，或者干脆工作线程池被打满了。此时，应用层已经完全瘫痪，无法处理任何新的业务请求。
- **结果**：TCP 依然在欢快地回复“我还活着”，客户端继续把请求发过来，结果全部超时。只有应用层心跳（通常是在 L7 应用层由业务代码主动响应的包）才能真实反映应用进程的健康状态。

**2. 响应时效性与灵活性**

- **默认时间太长**：Linux 系统默认的 TCP Keepalive 探测时间通常是 2 小时（7200 秒）。在现代微服务架构中，如果一个节点宕机，等 2 个小时才发现并切换流量，这绝对是一场灾难。
- **配置局限性**：虽然你可以通过 Socket API（如 C++ 或 Go 里的 `SO_KEEPALIVE`, `TCP_KEEPIDLE` 等）去修改这个时间，但它属于底层网络配置。相比之下，应用层心跳可以在代码逻辑里极其灵活地控制，通常设置为几秒甚至几百毫秒级别，实现毫秒级的故障隔离。

**3. 心跳可以携带丰富的业务状态（Metadata）**

TCP Keepalive 的探测包是非常底层的，几乎不带任何载荷数据。但在复杂的系统中，我们需要知道的不仅仅是“死或活”。 应用层心跳的 Payload（载荷）里可以夹带很多关键信息：

- **负载情况**：当前节点的 CPU 占用、内存使用率、请求队列长度。
- **依赖健康度**：比如 RPC 节点虽然活着，但它连不通后端的数据库了，它可以在心跳包里标记自己为“不健康”，让负载均衡器不要再把流量打过来。
- **服务发现与路由**：借由心跳机制向注册中心（如 etcd、Consul）续约，报告自己的可用性。

**4. 应对复杂的网络中间件（L7 代理/网关）**

现在的网络请求往往不是直连的，中间会经过很多 L7（应用层）的反向代理或负载均衡器（比如 Nginx、Envoy）。 这些中间件常常会设置基于应用层协议的“空闲超时时间”。如果它们在一段时间内没有看到 HTTP/RPC 的**业务数据包**流过，为了节省资源，就会主动把这根连接掐断。底层的 TCP Keepalive 数据包对 L7 代理来说是透明的（它们看不到或不关心），因此无法阻止连接被 L7 中间件回收。

> 总结：如何配合使用？

在设计高性能的网络通信系统时，这两者并不是“二选一”的关系，而是**互补配合**的：

1. **TCP Keepalive 是底线（兜底）**：用来清理那些因为网络物理断开、客户端直接拔电源等导致的“死连接”，防止服务器文件描述符和内存泄漏。
2. **应用层 HTTP/RPC 心跳是核心**：用来做服务治理、精确的健康检查、快速的故障剔除以及负载均衡。

你想看看在手写 RPC 框架时，通常是如何在 Go 或 C++ 的代码里设计并实现这种应用层心跳机制（比如 Ping/Pong 协议设计）的吗？

## 30. UDP 客户端不可以调用 `connect()` 吗

UDP 可以调用 `connect`，但它不触发三次握手，只是在内核中把这个 Socket 和目的 IP/端口**绑定**起来。好处是：后续发送可以直接用 `send/write` 而不用每次都传地址；并且如果目标主机不可达，内核能将 ICMP 错误直接返回给该 Socket，提升多次发送的性能。

## 31. IO 函数

### 1. `write` & `read`

**`write`/`read`**  属于 **系统调用**（属于`unistd.h`），基于**无缓冲I/O**（Unbuffered I/O）。

- 操作对象是**文件描述符（`int fd`）**（如文件、管道、套接字等）。
- 适用于**底层I/O操作**（如设备文件、网络套接字）。
- 每次调用直接触发内核操作（直接从用户缓冲区提交到内核缓冲区），也即数据不会再用户缓冲区进行缓冲。

```cpp
#include <unistd.h>

// Read NBYTES into BUF from FD.  
// Return the number read, -1 for errors or 0 for EOF.
ssize_t read (int __fd, void *__buf, size_t __nbytes); 

// Write N bytes of BUF to FD.  
// Return the number written, or -1.
ssize_t write (int __fd, const void *__buf, size_t __n);
```

使用实例：

```cpp
#define BUF_SIZE 128

char buf[BUF_SIZE];
const char *file_read = "./data.txt";
const char *file_write = "./copy.txt";

int main(int argc, char **argv)
{
    ssize_t read_len;
    int fd_read = open(file_read, O_RDONLY);
    int fd_write = open(file_write, O_WRONLY | O_CREAT | O_TRUNC);
    while((read_len = read(fd_read, buf, BUF_SIZE)) > 0) {
        if(write(fd_write, buf, read_len) != read_len) {
            perror("write() error");
            exit(1);
        }
    }
    if(read_len == -1) {
        perror("read() error");
        exit(1);
    }

    return 0;
}
```

### 2. `send` & `recv`

**`send`/`recv`** 是专门为**套接字（sockets）**设计的 **系统调用**，仅用于 **网络通信**。提供了更多面向网络的 **控制选项**（如标志位）。

```cpp
#include <sys/socket.h>

// Send N bytes of BUF to socket FD.  
// Returns the number sent or -1.
ssize_t send (int __fd, const void *__buf, size_t __n, int __flags);


// Read N bytes into BUF from socket FD.
// Returns the number read or -1 for errors.
ssize_t recv (int __fd, void *__buf, size_t __n, int __flags);
```

下面是所有的标志位：

```cpp
/* Bits in the FLAGS argument to `send', `recv', et al.  */
enum {
  MSG_OOB = 0x01, /* Process out-of-band data.  */
#define MSG_OOB MSG_OOB
  MSG_PEEK = 0x02, /* Peek at incoming messages.  */
#define MSG_PEEK MSG_PEEK
  MSG_DONTROUTE = 0x04, /* Don't use local routing.  */
#define MSG_DONTROUTE MSG_DONTROUTE
#ifdef __USE_GNU
  /* DECnet uses a different name.  */
  MSG_TRYHARD = MSG_DONTROUTE,
#define MSG_TRYHARD MSG_DONTROUTE
#endif
  MSG_CTRUNC = 0x08, /* Control data lost before delivery.  */
#define MSG_CTRUNC MSG_CTRUNC
  MSG_PROXY = 0x10, /* Supply or ask second address.  */
#define MSG_PROXY MSG_PROXY
  MSG_TRUNC = 0x20,
#define MSG_TRUNC MSG_TRUNC
  MSG_DONTWAIT = 0x40, /* Nonblocking IO.  */
#define MSG_DONTWAIT MSG_DONTWAIT
  MSG_EOR = 0x80, /* End of record.  */
#define MSG_EOR MSG_EOR
  MSG_WAITALL = 0x100, /* Wait for a full request.  */
#define MSG_WAITALL MSG_WAITALL
  MSG_FIN = 0x200,
#define MSG_FIN MSG_FIN
  MSG_SYN = 0x400,
#define MSG_SYN MSG_SYN
  MSG_CONFIRM = 0x800, /* Confirm path validity.  */
#define MSG_CONFIRM MSG_CONFIRM
  MSG_RST = 0x1000,
#define MSG_RST MSG_RST
  MSG_ERRQUEUE = 0x2000, /* Fetch message from error queue.  */
#define MSG_ERRQUEUE MSG_ERRQUEUE
  MSG_NOSIGNAL = 0x4000, /* Do not generate SIGPIPE.  */
#define MSG_NOSIGNAL MSG_NOSIGNAL
  MSG_MORE = 0x8000, /* Sender will send more.  */
#define MSG_MORE MSG_MORE
  MSG_WAITFORONE = 0x10000, /* Wait for at least one packet to return.*/
#define MSG_WAITFORONE MSG_WAITFORONE
  MSG_BATCH = 0x40000, /* sendmmsg: more messages coming.  */
#define MSG_BATCH MSG_BATCH
  MSG_ZEROCOPY = 0x4000000, /* Use user data in kernel path.  */
#define MSG_ZEROCOPY MSG_ZEROCOPY
  MSG_FASTOPEN = 0x20000000, /* Send data in TCP SYN.  */
#define MSG_FASTOPEN MSG_FASTOPEN

  MSG_CMSG_CLOEXEC = 0x40000000 /* Set close_on_exit for file
            descriptor received through
            SCM_RIGHTS.  */
#define MSG_CMSG_CLOEXEC MSG_CMSG_CLOEXEC
};
```

### 3. `fread` & `fwrite`

 `fread` 和 `fwrite` 是 **标准库函数**（属于`stdio.h`），基于 **缓冲I/O**（Buffered I/O）。

- 操作对象是**文件指针（`FILE*`）**，而非文件描述符（`int fd`）。
- 适用于**高层文件操作**（如读写文本/二进制文件）。
- 自动处理缓冲，数据会先存放在用户缓冲区，当缓冲区满时才写入内核缓冲区，从而减少系统调用次数，提高效率。

```cpp
#include <stdio.h>

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
```

- **参数**：
  - `ptr`：数据缓冲区。
  - `size`：每个数据项的字节数。
  - `nmemb`：要读写的数据项数量。
  - `stream`：文件指针（如通过`fopen`打开）。
- **返回值：**
  - **成功读取了所有请求的项目**：如果函数成功读取了 `count` 个 `size` 字节的项目，它将返回 `count`。
  - **读取了部分项目**：如果由于到达文件末尾（EOF）或发生读取错误，导致无法读取完整的 `count` 个项目，`fread` 将返回**实际读取的完整项目的数量**，这个值会小于 `count`。
  - **读取失败**：
    - 如果返回值为 `0`，并且文件结束标志（EOF indicator）被设置（可以通过 `feof()` 检查），则表示在尝试读取任何数据之前就达到了文件末尾。
    - 如果返回值为 `0`，并且错误标志（error indicator）被设置（可以通过 `ferror()` 检查），则表示发生了读取错误。

示例代码：

```cpp
#define BUF_SIZE 128

char buf[BUF_SIZE];
const char *file_read = "./data.txt";
const char *file_write = "./copy.txt";

int main(int argc, char **argv)
{
    FILE *fd_read = fopen(file_read, "r");
    FILE *fd_write = fopen(file_write, "w");

    size_t read_len;
    while((read_len = fread(buf, sizeof(char), BUF_SIZE, fd_read)) > 0) {
        if(fwrite(buf, sizeof(char), read_len, fd_write) != read_len) {
            perror("write() error");
            exit(1);
        }
    }
    if(ferror(fd_read)) {
        perror("read() error");
        exit(1);
    }
    else if(feof(fd_read)) {
        printf("read eof...\n");
    }

    return 0;
}
```

需要格外注意的是，`fread/fwrite` 与 `read/write` 的返回值对错误的处理完全不同。

### 4. `fputs` & `fgets`

`fputs` 和 `fgets` 是 C 语言 **标准库** 中的两个函数，专门用于对文件流进行**字符串**的读写操作。它们与 `fread`/`fwrite` 的区别在于，`fputs`/`fgets` 处理的是以空字符 `\0` 结尾的字符串，而 `fread`/`fwrite` 处理的是任意的二进制数据块。

`fputs` 函数用于将一个**字符串**写入到指定的文件流中。

```cpp
#include <cstdio> // 或者 <stdio.h>

int fputs(const char *str, FILE *stream);
```

参数：

- **`str`**: 指向要写入的字符串的指针。这个字符串必须以空字符 `\0` 结尾。`fputs` **不会**将这个空字符写入文件。
- **`stream`**: 指向 `FILE` 对象的指针，表示要写入的文件流。

返回值：

- 成功时，返回一个**非负整数**。
- 失败时，返回 `EOF`（通常是 `-1`），并设置文件流的错误标志。可以通过 `ferror(stream)` 来检查具体错误。

注意：

* `fputs` 不会自动在写入的字符串末尾添加换行符，即使字符串本身没有换行符，它也不会添加。这意味着如果您想写入多行，需要确保字符串中包含 `\n`（`fgets` 会保留换行符）。

---

`fgets` 函数用于从指定的文件流中读取一个**字符串**。它比 `gets` 更安全，因为它允许您指定读取的最大字符数，从而防止缓冲区溢出。

```cpp
#include <cstdio> // 或者 <stdio.h>

char *fgets(char *str, int num, FILE *stream);
```

参数：

- **`str`**: 指向存储读取到的字符串的缓冲区的指针。
- **`num`**: 要读取的最大字符数（包括空字符 `\0`）。`fgets` 最多会读取 `num-1` 个字符，然后自动在末尾添加一个空字符。
- **`stream`**: 指向 `FILE` 对象的指针，表示要读取的文件流。

返回值：

- 成功时，返回指向 `str` 的指针（即传入的缓冲区地址）。
- 失败时（包括到达文件末尾或发生读取错误），返回 `NULL`。可以通过 `feof(stream)` 检查是否是文件末尾，通过 `ferror(stream)` 检查是否是读取错误。

---

使用示例：

```cpp
#define BUF_SIZE 128

char buf[BUF_SIZE];
const char *file_read = "./data.txt";
const char *file_write = "./copy.txt";

int main(int argc, char **argv)
{
    FILE *read_file = fopen("data.txt", "r");
    FILE *write_file = fopen("copy.txt", "w");
    while(fgets(buf, BUF_SIZE, read_file) != NULL) {
        if(fputs(buf, write_file) == -1) {
            perror("write error");
            exit(1);
        }
    }

    if(ferror(read_file)) {
        perror("fiel read error");
        exit(1);
    }
    else if(feof(read_file)) {
        printf("file read done...\n");
    }

    return 0;
}
```

### 5. `puts` & `gets`

`puts` 和 `gets` 是 C 语言标准库中的两个函数，用于字符串的输入和输出。它们在功能上与 `fputs`/`fgets` 类似，但主要区别在于它们默认操作的是**标准输出（stdout）和标准输入（stdin）**，而不是指定的文件流。

```cpp
#include <stdio.h>

int puts(const char *str);
// str: 指向要写入的字符串的指针。这个字符串必须以空字符 `\0` 结尾。
// 成功时，返回一个非负整数。
// 失败时，返回 EOF（通常是 -1）。

char *gets(char *str);
// str: 指向存储读取到的字符串的缓冲区的指针。
// 成功时，返回指向 str 的指针（即传入的缓冲区地址）。
// 失败时（包括到达文件末尾或发生读取错误），返回 NULL。
```

`gets` 函数用于从**标准输入**（通常是键盘）读取一个字符串，直到遇到换行符或文件结束符为止。它**不安全**，因为它没有办法限制读取的字符数量，这可能导致**缓冲区溢出**。因此，在现代 C/C++ 编程中，**强烈不推荐使用 `gets`**。

| 特性         | `puts`                                          | `gets`                                                       |
| ------------ | ----------------------------------------------- | ------------------------------------------------------------ |
| **I/O 方向** | 写入到**标准输出**                              | 从**标准输入**读取                                           |
| **自动换行** | **是**，会在字符串末尾自动添加一个换行符        | **否**，读取的字符串中不包含换行符（它会被移除）             |
| **安全性**   | 安全                                            | **不安全**，容易导致缓冲区溢出，**强烈不推荐使用**           |
| **空字符**   | 输入的字符串需以`\0` 结尾，`puts` 不写入 `\0`   | 读取后会在末尾自动添加`\0`                                   |
| **推荐替代** | `printf("%s\n", str);` 或 `fputs(str, stdout);` | **`fgets(buffer, sizeof(buffer), stdin);`** 或 `scanf` 系列函数 |

### 6. `readv` & `writev`

`readv` 和 `writev` 是 POSIX 标准中的两个 **系统调用**，它们被称为**分散读取（scatter read）**和**集中写入（gather write）**操作。这两个函数允许您在一次系统调用中，从多个缓冲区读取数据，或将数据写入到多个缓冲区，而不是像 `read` 和 `write` 那样，只能操作一个连续的缓冲区。

---

`readv` 函数用于从文件描述符中读取数据，并将其分散（scatter）到一系列不连续的缓冲区中。

```cpp
#include <sys/uio.h> // 包含结构体 iovec 和函数原型

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
```

参数

- **`fd`**: 要读取数据的文件描述符。
- **`iov`**: 指向 `struct iovec` 结构体数组的指针。每个 `struct iovec` 定义了一个要填充的缓冲区。
- **`iovcnt`**: `iov` 数组中元素的数量（即缓冲区的数量）。

`struct iovec` 定义了单个缓冲区的信息：

```c
struct iovec {
    void  *iov_base;    // 指向缓冲区的起始地址
    size_t iov_len;     // 缓冲区的长度（字节数）
};
```

返回值

- 成功时，返回实际读取的总字节数。这个值可能小于所有 `iov_len` 的总和。
- 失败时，返回 `-1`，并设置 `errno` 来指示错误原因。

`readv` 会按照 `iov` 数组中 `struct iovec` 元素的顺序，依次填充每个缓冲区。当一个缓冲区被填满后，接下来的数据会写入下一个缓冲区，直到所有数据都被读取完毕或所有缓冲区都被填满，或达到文件末尾，或发生错误。

```c
char buf1[8];
char buf2[4];
char buf3[16];

struct iovec iov[3];

int fd = open("bin/example.txt", O_WRONLY | O_CREAT | O_TRUNC);
write(fd, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26);
close(fd);

fd = open("bin/example.txt", O_RDONLY);

iov[0].iov_base = buf1, iov[0].iov_len = sizeof(buf1);
iov[1].iov_base = buf2, iov[1].iov_len = sizeof(buf2);
iov[2].iov_base = buf3, iov[2].iov_len = sizeof(buf3);

printf("Attempting to read %zu bytes into 3 buffers\n", sizeof(buf1) + sizeof(buf2)+ sizeof(buf3));

ssize_t bytes = readv(fd, iov, 3);
if(bytes == -1) {
    perror("readv() error");
}
else {
    printf("Successfully read %zd bytes.\n", bytes);
    // readv 函数会直接填充原始字节，buf 不一定以 \0 结尾，因此这里我们需要控制一下精度
    printf("buf1: %.*s\n", (int)iov[0].iov_len, (char*)iov[0].iov_base);
    printf("buf2: %.*s\n", (int)iov[1].iov_len, (char*)iov[1].iov_base);
    printf("buf3: %.*s\n", (int)iov[2].iov_len, (char*)iov[2].iov_base);
}

close(fd);
```

---

`writev` 函数用于将数据从一系列不连续的缓冲区中集中（gather）起来，然后一次性写入到文件描述符中。

```c
#include <sys/uio.h> // 包含结构体 iovec 和函数原型

ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
```

参数

- **`fd`**: 要写入数据的文件描述符。
- **`iov`**: 指向 `struct iovec` 结构体数组的指针。每个 `struct iovec` 定义了一个要写入的缓冲区。
- **`iovcnt`**: `iov` 数组中元素的数量（即缓冲区的数量）。

返回值

- 成功时，返回实际写入的总字节数。这个值可能小于所有 `iov_len` 的总和。
- 失败时，返回 `-1`，并设置 `errno` 来指示错误原因。

`writev` 会按照 `iov` 数组中 `struct iovec` 元素的顺序，依次从每个缓冲区中获取数据并写入文件。

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h> // For writev and iovec

int main() {
    int fd;
    const char *header = "HTTP/1.1 200 OK\r\n";
    const char *content_type = "Content-Type: text/plain\r\n";
    const char *content_length = "Content-Length: "; // 需要动态计算
    char length_str[10]; // 假设长度不会超过9位数
    const char *body = "Hello from writev!\nThis is a multi-part message.";
  
    struct iovec iov[4]; // 4 个部分：header, content_type, content_length, body
    ssize_t bytes_written;
    size_t total_len;

    // 计算 body 的长度并转换为字符串
    size_t body_len = strlen(body);
    sprintf(length_str, "%zu\r\n\r\n", body_len); // 添加 CRLF 和空行分隔符

    // 设置第一个 iovec：HTTP 头
    iov[0].iov_base = (void*)header;
    iov[0].iov_len = strlen(header);

    // 设置第二个 iovec：Content-Type
    iov[1].iov_base = (void*)content_type;
    iov[1].iov_len = strlen(content_type);

    // 设置第三个 iovec：Content-Length
    iov[2].iov_base = (void*)content_length;
    iov[2].iov_len = strlen(content_length);

    // 第四个 iovec 是动态计算的长度字符串
    iov[3].iov_base = (void*)length_str;
    iov[3].iov_len = strlen(length_str); // 注意这里是 length_str 的长度

    // 计算要写入的总字节数（不包括 body，因为它在另一个 iovec 中）
    total_len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len + iov[3].iov_len;

    // 将 body 作为第五个 iovec
    // 为了简化，我们只用 4 个 iovec，把 body 直接加在后面
    // 实际应用中，如果 body 也是独立的，会是 iov[4]
    // 重新设置 total_len 以包含 body
    total_len += body_len;


    // 实际在 writev 中，iov 数组需要是连续的
    // 为了这个例子，我们把 body 也加到 iov 数组中
    struct iovec full_iov[5];
    memcpy(&full_iov[0], &iov[0], sizeof(struct iovec) * 4); // 复制前4个
    full_iov[4].iov_base = (void*)body;
    full_iov[4].iov_len = body_len;

    // 以写入模式打开文件
    fd = open("http_response.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) { perror("open write"); return 1; }

    printf("Attempting to write %zu bytes from 5 buffers.\n", total_len);

    // 调用 writev
    bytes_written = writev(fd, full_iov, 5); // 5个 iovec 元素

    if (bytes_written == -1) {
        perror("writev error");
    } else {
        printf("Successfully wrote %zd bytes.\n", bytes_written);
    }

    close(fd);
    return 0;
}
```

* 注意：** 在上面的 `writev` 示例中，为了方便演示，我将 `body` 也作为一个 `iovec` 元素添加到数组中，这样总共有 5 个 `iovec` 元素。实际的总长度计算应该包括所有要写入的数据。

`readv` 和 `writev` 的优点：

1. **减少系统调用次数**：这是它们最主要的优势。如果没有 `readv` 和 `writev`，您需要为每个不连续的缓冲区进行单独的 `read` 或 `write` 调用。每次系统调用都会涉及到用户态到内核态的上下文切换，这会带来显著的开销。使用 `readv`/`writev` 可以将多个缓冲区操作合并为一次系统调用，从而提高性能，尤其是在处理大量小块数据时。
2. **原子性（对于部分操作）**：
   - **`writev`**：对于一个给定的 `writev` 调用，所有数据通常被视为一个原子操作写入。这意味着即使多个进程同时写入同一个文件，您写入的所有 `iovec` 中的数据也会作为一个整体被写入，而不会被其他进程的写入操作中断。这对于网络编程（如构建 HTTP 响应头和正文）尤其有用，可以确保数据包的完整性。
   - **`readv`**：虽然 `readv` 读取时也会按顺序填充缓冲区，但它的原子性不如 `writev` 那么强调。它会尽可能地读取数据并填充 `iovec` 数组。
3. **简化代码**：避免了手动将多个不连续的数据块复制到一个大的连续缓冲区中，再进行单次 `read`/`write` 操作（这被称为 **"内存拷贝"** 或 **"数据扁平化"**）。这可以节省 CPU 周期和内存带宽。

适用场景：

- **网络编程**：
  - `writev` 经常用于发送 HTTP 响应（头信息分散在多个缓冲区，正文在另一个缓冲区）或TCP/UDP数据包，可以避免额外的内存拷贝。
  - `readv` 可以用于接收复杂协议的数据包，将不同部分直接读取到不同的结构体成员中。
- **文件系统操作**：
  - 读取或写入文件时，数据的逻辑结构可能对应于内存中不连续的缓冲区。
- **零拷贝（Zero-Copy）优化**：虽然 `readv`/`writev` 本身不完全是零拷贝，但它们是实现零拷贝技术（如 `sendfile`）的基础或补充。通过减少内核态与用户态之间的数据拷贝，它们可以提高 I/O 密集型应用的性能。

---

总的来说，`readv` 和 `writev` 是在处理不连续内存区域的 I/O 时，提高性能和简化代码的强大工具。

### 7.  `MSG_OOB`

在 TCP 中，紧急消息并不是指需要优先传输的消息，因为 TCP 需要满足按序到达的要求，因此肯定不可能将后面的数据先送达，即使送达也不合法（序号不匹配）。在这里，紧急消息指的是在接收到以后，需要紧急处理的消息。

### 8. 实例

下面是使用了标准 I/O 函数的回声客户端和服务器。

#### server

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(const char *msg);

int main(int argc, char **argv)
{
    // check command line argument 
    if(argc != 2) {
        printf("Usage : %s <Port> \n", argv[0]);
        exit(1);
    }

    // create serv socket
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) {
        error_handling("socket() error");
    }
  
    // address reuse 
    int optval = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // bind address
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    // listen
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    printf("Server started on port %s\n", argv[1]);

    // echo
    while(1) {
        // accept client 
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_sz = sizeof(clnt_addr);
        int clnt_sock;
        if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz)) == -1) {
            error_handling("accept() error");
        }
        printf("Client connected %s:%d...\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

        // receive data and echo 
        FILE *readfp = fdopen(clnt_sock, "r");
        FILE *writefp = fdopen(dup(clnt_sock), "w");
        if(!readfp || !writefp) {
            error_handling("fdopen() error");
            continue;
        }

        char buf[BUF_SIZE];
        while(fgets(buf, BUF_SIZE, readfp)) {
            fputs(buf, writefp);
            fflush(writefp); // send immediately
        }
        fclose(readfp);
        fclose(writefp);
    }
    close(serv_sock);
    return 0;
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}
```

对于代码中，我们要注意一个地方：

```cpp
FILE *readfp = fdopen(clnt_sock, "r");
FILE *writefp = fdopen(dup(clnt_sock), "w");
```

在这里，我们是将套接字 `clnt_sock` 分离为两个 IO 流，分别用来读和写，同时，我们通过使用 `dup()` 复制文件描述符，避免两个流共享同一个文件描述符状态。相较于 `FILE *rwfp = fdopen(clnt_sock, "r+");` 来获得一个既可以读有可以写的 IO 流，分离成两个独立的 IO 流通常是更好的选择。

#### client

```cpp
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024
void error_handling(const char *msg);

int main(int argc, char **argv)
{
    // check command line argument
    if(argc != 3) {
        printf("Usage : %s <IP> <Port> \n", argv[0]);
        exit(1);
    }

    // create client socket
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1) {
        error_handling("socket() error");
    }

    // connect server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    printf("connect to server at %s:%s\n", argv[1], argv[2]);

    // echo
    FILE *writefd = fdopen(dup(sock), "w"); // 用于写入
    FILE *readfd = fdopen(sock, "r");      // 用于读取

    if(!writefd || !readfd) {
        error_handling("fdopen() error");
    }

    char buf[BUF_SIZE];
    while(1) {
        printf("Input message(enter q to quit): ");
        if(!fgets(buf, BUF_SIZE, stdin)) {
            // fgets 遇到EOF或错误时返回NULL
            break;
        }

        if(strcmp(buf, "q\n") == 0 || strcmp(buf, "Q\n") == 0) {
            printf("client quitting...\n"); // 添加换行符
            break;
        }

        size_t data_len = strlen(buf);
        if (data_len == 0) { // 如果只输入了换行符，跳过本次循环
            continue;
        }

        // 写入数据
        fwrite(buf, sizeof(char), data_len, writefd);
        fflush(writefd);

        // 读取回显数据
        size_t total_read_bytes = 0;
        // 假设服务器会准确回显发送的字节数
        while (total_read_bytes < data_len) {
            size_t bytes_read = fread(buf + total_read_bytes, sizeof(char), data_len - total_read_bytes, readfd);
            if (bytes_read <= 0) {
                if (bytes_read == 0) {
                    printf("Server closed connection unexpectedly or no more data.\n");
                } 
                else {
                    error_handling("fread() error");
                }
                break; // 退出读取循环
            }
            total_read_bytes += bytes_read;
        }

        if (total_read_bytes > 0) {
            buf[total_read_bytes] = '\0'; // 确保接收到的数据以空字符结尾
            printf("client received: %s\n", buf);
        } 
        else {
            // 如果没有接收到数据，可能是因为服务器没有回显，或者连接已关闭
            printf("No data received or connection issue.\n");
        }
    }

    // 关闭文件流和套接字
    fclose(writefd);
    fclose(readfd);
    return 0;
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}
```

### 9. EOF

### 10. `fcntl`

# Linux 网络编程示例（C++17）

本文示例以 C++17 和 Linux/POSIX 为基准。epoll_create1、accept4、pipe2
是 Linux 专有 API。编译命令如下：

~~~bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -pthread example.cpp -o example
~~~

示例不再使用 inet_addr、inet_ntoa、gethostbyname、epoll_create、C 风格线程
入口或手工管理文件描述符。地址统一由 getaddrinfo/getnameinfo 处理，因此普通
TCP、UDP 示例可使用 IPv4 或 IPv6。

## 公共 RAII 工具

将下面代码保存为 net.hpp。后文示例均包含这个头文件。

~~~cpp
#pragma once

#include <cerrno>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

namespace net {

[[noreturn]] inline void throw_errno(const char* operation) {
    throw std::system_error(errno, std::generic_category(), operation);
}

class UniqueFd {
public:
    UniqueFd() noexcept = default;
    explicit UniqueFd(int fd) noexcept : fd_(fd) {}
    ~UniqueFd() { reset(); }

    UniqueFd(const UniqueFd&) = delete;
    UniqueFd& operator=(const UniqueFd&) = delete;

    UniqueFd(UniqueFd&& other) noexcept : fd_(other.release()) {}
    UniqueFd& operator=(UniqueFd&& other) noexcept {
        if (this != &other) reset(other.release());
        return *this;
    }

    int get() const noexcept { return fd_; }
    explicit operator bool() const noexcept { return fd_ >= 0; }
    int release() noexcept { return std::exchange(fd_, -1); }

    void reset(int replacement = -1) noexcept {
        if (fd_ >= 0) {
            // Linux 上 close 返回 EINTR 时也不应重试。
            ::close(fd_);
        }
        fd_ = replacement;
    }

private:
    int fd_ = -1;
};

struct AddrInfoDeleter {
    void operator()(addrinfo* value) const noexcept {
        ::freeaddrinfo(value);
    }
};
using AddrInfo = std::unique_ptr<addrinfo, AddrInfoDeleter>;

inline AddrInfo resolve(const char* host, const char* service, int socktype,
                        int flags = 0, int family = AF_UNSPEC) {
    addrinfo hints{};
    hints.ai_family = family;
    hints.ai_socktype = socktype;
    hints.ai_flags = flags;

    addrinfo* result = nullptr;
    const int error = ::getaddrinfo(host, service, &hints, &result);
    if (error != 0) {
        throw std::runtime_error(std::string{"getaddrinfo: "} +
                                 ::gai_strerror(error));
    }
    return AddrInfo{result};
}

inline UniqueFd tcp_listener(const char* service) {
    auto addresses = resolve(nullptr, service, SOCK_STREAM,
                             AI_PASSIVE | AI_NUMERICSERV);
    int last_error = EADDRNOTAVAIL;
    for (auto* address = addresses.get(); address; address = address->ai_next) {
        UniqueFd socket{::socket(address->ai_family,
                                 address->ai_socktype | SOCK_CLOEXEC,
                                 address->ai_protocol)};
        if (!socket) {
            last_error = errno;
            continue;
        }
        const int enabled = 1;
        if (::setsockopt(socket.get(), SOL_SOCKET, SO_REUSEADDR, &enabled,
                         sizeof(enabled)) == -1 ||
            ::bind(socket.get(), address->ai_addr, address->ai_addrlen) == -1 ||
            ::listen(socket.get(), SOMAXCONN) == -1) {
            last_error = errno;
            continue;
        }
        return socket;
    }
    errno = last_error;
    throw_errno("create TCP listener");
}

inline UniqueFd tcp_connect(const char* host, const char* service) {
    auto addresses = resolve(host, service, SOCK_STREAM, AI_NUMERICSERV);
    int last_error = ECONNREFUSED;
    for (auto* address = addresses.get(); address; address = address->ai_next) {
        UniqueFd socket{::socket(address->ai_family,
                                 address->ai_socktype | SOCK_CLOEXEC,
                                 address->ai_protocol)};
        if (!socket) {
            last_error = errno;
            continue;
        }
        if (::connect(socket.get(), address->ai_addr, address->ai_addrlen) == 0) {
            return socket;
        }
        last_error = errno;
    }
    errno = last_error;
    throw_errno("connect");
}

inline UniqueFd udp_connect(const char* host, const char* service) {
    auto addresses = resolve(host, service, SOCK_DGRAM, AI_NUMERICSERV);
    int last_error = EHOSTUNREACH;
    for (auto* address = addresses.get(); address; address = address->ai_next) {
        UniqueFd socket{::socket(address->ai_family,
                                 address->ai_socktype | SOCK_CLOEXEC,
                                 address->ai_protocol)};
        if (!socket) {
            last_error = errno;
            continue;
        }
        if (::connect(socket.get(), address->ai_addr, address->ai_addrlen) == 0) {
            return socket;
        }
        last_error = errno;
    }
    errno = last_error;
    throw_errno("connect UDP socket");
}

struct Endpoint {
    sockaddr_storage address{};
    socklen_t length = 0;
};

inline Endpoint endpoint(const char* host, const char* service, int socktype,
                         int family = AF_UNSPEC) {
    auto addresses = resolve(host, service, socktype,
                             AI_NUMERICHOST | AI_NUMERICSERV, family);
    Endpoint result;
    if (addresses->ai_addrlen > sizeof(result.address)) {
        throw std::runtime_error("socket address is too large");
    }
    result.length = static_cast<socklen_t>(addresses->ai_addrlen);
    std::memcpy(&result.address, addresses->ai_addr, addresses->ai_addrlen);
    return result;
}

inline UniqueFd udp_socket(int family) {
    UniqueFd socket{::socket(family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP)};
    if (!socket) throw_errno("socket");
    return socket;
}

inline UniqueFd udp_bind(const char* service, int family = AF_UNSPEC) {
    auto addresses = resolve(nullptr, service, SOCK_DGRAM,
                             AI_PASSIVE | AI_NUMERICSERV, family);
    int last_error = EADDRNOTAVAIL;
    for (auto* address = addresses.get(); address; address = address->ai_next) {
        UniqueFd socket{::socket(address->ai_family,
                                 address->ai_socktype | SOCK_CLOEXEC,
                                 address->ai_protocol)};
        if (!socket) {
            last_error = errno;
            continue;
        }
        const int enabled = 1;
        if (::setsockopt(socket.get(), SOL_SOCKET, SO_REUSEADDR, &enabled,
                         sizeof(enabled)) == 0 &&
            ::bind(socket.get(), address->ai_addr, address->ai_addrlen) == 0) {
            return socket;
        }
        last_error = errno;
    }
    errno = last_error;
    throw_errno("bind UDP socket");
}

inline std::pair<std::string, std::string> endpoint_name(
    const sockaddr* address, socklen_t length) {
    char host[NI_MAXHOST]{};
    char service[NI_MAXSERV]{};
    const int error = ::getnameinfo(address, length, host, sizeof(host), service,
                                    sizeof(service),
                                    NI_NUMERICHOST | NI_NUMERICSERV);
    if (error != 0) {
        throw std::runtime_error(std::string{"getnameinfo: "} +
                                 ::gai_strerror(error));
    }
    return {host, service};
}

inline UniqueFd accept(int listener, Endpoint& peer, int flags = SOCK_CLOEXEC) {
    for (;;) {
        peer.length = sizeof(peer.address);
        const int fd = ::accept4(
            listener, reinterpret_cast<sockaddr*>(&peer.address),
            &peer.length, flags);
        if (fd >= 0) return UniqueFd{fd};
        if (errno != EINTR) throw_errno("accept4");
    }
}

inline void send_all(int fd, std::string_view data) {
    while (!data.empty()) {
        const ssize_t count = ::send(fd, data.data(), data.size(), MSG_NOSIGNAL);
        if (count > 0) {
            data.remove_prefix(static_cast<std::size_t>(count));
        } else if (count == 0) {
            throw std::runtime_error("send returned zero");
        } else if (count < 0 && errno == EINTR) {
            continue;
        } else {
            throw_errno("send");
        }
    }
}

inline bool try_send_all(int fd, std::string_view data) {
    while (!data.empty()) {
        const ssize_t count = ::send(
            fd, data.data(), data.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
        if (count > 0) {
            data.remove_prefix(static_cast<std::size_t>(count));
        } else if (count == 0) {
            throw std::runtime_error("send returned zero");
        } else if (count < 0 && errno == EINTR) {
            continue;
        } else if (count < 0 &&
                   (errno == EAGAIN || errno == EWOULDBLOCK)) {
            return false;
        } else {
            throw_errno("send");
        }
    }
    return true;
}

inline bool receive_exact(int fd, void* output, std::size_t size) {
    auto* bytes = static_cast<char*>(output);
    std::size_t received = 0;
    while (received < size) {
        const ssize_t count = ::recv(fd, bytes + received, size - received, 0);
        if (count > 0) {
            received += static_cast<std::size_t>(count);
        } else if (count == 0 && received == 0) {
            return false;
        } else if (count == 0) {
            throw std::runtime_error("unexpected EOF");
        } else if (errno != EINTR) {
            throw_errno("recv");
        }
    }
    return true;
}

inline void set_nonblocking(int fd) {
    const int flags = ::fcntl(fd, F_GETFL);
    if (flags == -1 || ::fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw_errno("fcntl");
    }
}

}  // namespace net
~~~

UniqueFd 是只移动类型，离开作用域时自动 close。send_all 处理 TCP 短写和
EINTR，并使用 MSG_NOSIGNAL，避免对端断开时由 SIGPIPE 直接终止进程。

## 单读多写 TCP 服务器

### server.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <exception>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "用法: " << argv[0] << " <端口>\n";
            return 2;
        }
        auto listener = net::tcp_listener(argv[1]);
        const std::array<std::string_view, 3> messages{
            "Hello, World!\n", "Let's go!\n", "Network Programming!\n"};

        for (;;) {
            net::Endpoint peer;
            auto client = net::accept(listener.get(), peer);
            try {
                const auto [host, service] = net::endpoint_name(
                    reinterpret_cast<const sockaddr*>(&peer.address),
                    peer.length);
                std::cout << "客户端已连接: " << host << ':' << service << '\n';
                for (const auto message : messages) {
                    net::send_all(client.get(), message);
                }
            } catch (const std::exception& error) {
                std::cerr << "客户端通信失败: " << error.what() << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### client.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "用法: " << argv[0] << " <主机名或 IP> <端口>\n";
            return 2;
        }
        auto socket = net::tcp_connect(argv[1], argv[2]);
        std::array<char, 4096> buffer{};
        for (;;) {
            const ssize_t count =
                ::recv(socket.get(), buffer.data(), buffer.size(), 0);
            if (count > 0) {
                std::cout.write(buffer.data(), count);
            } else if (count == 0) {
                break;
            } else if (errno != EINTR) {
                net::throw_errno("recv");
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

客户端端口通常每次不同。它是客户端操作系统在 connect 时自动选择的临时端口；
命令行中的端口始终是服务器监听端口。

## TCP 回声服务器与客户端

TCP 是字节流，不保留应用层消息边界。客户端发送一行后，必须按协议接收同样
数量的字节，不能假设一次 recv 就能拿到整行。

### echo_server.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto listener = net::tcp_listener(argv[1]);
        for (;;) {
            net::Endpoint peer;
            auto client = net::accept(listener.get(), peer);
            try {
                std::array<char, 4096> buffer{};
                for (;;) {
                    const ssize_t count =
                        ::recv(client.get(), buffer.data(), buffer.size(), 0);
                    if (count > 0) {
                        net::send_all(
                            client.get(),
                            {buffer.data(), static_cast<std::size_t>(count)});
                    } else if (count == 0) {
                        break;
                    } else if (errno != EINTR) {
                        net::throw_errno("recv");
                    }
                }
            } catch (const std::exception& error) {
                std::cerr << "客户端通信失败: " << error.what() << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### echo_client.cpp

~~~cpp
#include "net.hpp"

#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) return 2;
        auto socket = net::tcp_connect(argv[1], argv[2]);
        for (std::string line; std::getline(std::cin, line);) {
            if (line == "q" || line == "Q") break;
            line.push_back('\n');
            net::send_all(socket.get(), line);

            std::string reply(line.size(), '\0');
            if (!net::receive_exact(socket.get(), reply.data(), reply.size())) {
                throw std::runtime_error("服务器提前关闭连接");
            }
            std::cout << "服务器回复: " << reply;
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

connect 成功后，套接字已与一条 TCP 连接关联。send 只是把数据交给内核发送
缓冲区；返回成功不表示对端应用已经读取。TCP 可能拆分或合并应用层的多次写入，
应用协议必须自己定义长度、分隔符或固定格式。

## 简单算术服务器

旧代码把 char 数组强制转换成 int 指针，存在对齐、严格别名和字节序问题。本例
使用明确的线协议：4 字节操作数数量、若干 4 字节有符号整数、1 字节运算符；
所有整数都用网络字节序。

### calc_server.cpp

~~~cpp
#include "net.hpp"

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

std::uint32_t receive_u32(int fd) {
    std::uint32_t wire = 0;
    if (!net::receive_exact(fd, &wire, sizeof(wire))) {
        throw std::runtime_error("连接已关闭");
    }
    return ntohl(wire);
}

std::int32_t receive_i32(int fd) {
    const std::uint32_t host_value = receive_u32(fd);
    std::int32_t value = 0;
    std::memcpy(&value, &host_value, sizeof(value));
    return value;
}

void send_i32(int fd, std::int32_t value) {
    std::uint32_t host_value = 0;
    std::memcpy(&host_value, &value, sizeof(value));
    const std::uint32_t wire = htonl(host_value);
    net::send_all(fd, {reinterpret_cast<const char*>(&wire), sizeof(wire)});
}

std::int32_t calculate(const std::vector<std::int32_t>& values, char op) {
    if (op != '+' && op != '-' && op != '*') {
        throw std::runtime_error("不支持的运算符");
    }
    std::int64_t result = values.front();
    for (std::size_t i = 1; i < values.size(); ++i) {
        if (op == '+') result += values[i];
        else if (op == '-') result -= values[i];
        else result *= values[i];

        if (result < std::numeric_limits<std::int32_t>::min() ||
            result > std::numeric_limits<std::int32_t>::max()) {
            throw std::overflow_error("计算结果超出 int32 范围");
        }
    }
    return static_cast<std::int32_t>(result);
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto listener = net::tcp_listener(argv[1]);
        for (;;) {
            net::Endpoint peer;
            auto client = net::accept(listener.get(), peer);
            try {
                const std::uint32_t count = receive_u32(client.get());
                if (count == 0 || count > 32) {
                    throw std::runtime_error(
                        "操作数数量必须在 1 到 32 之间");
                }
                std::vector<std::int32_t> values;
                values.reserve(count);
                for (std::uint32_t i = 0; i < count; ++i) {
                    values.push_back(receive_i32(client.get()));
                }
                char op = '\0';
                if (!net::receive_exact(client.get(), &op, 1)) {
                    throw std::runtime_error("缺少运算符");
                }
                send_i32(client.get(), calculate(values, op));
            } catch (const std::exception& error) {
                std::cerr << "客户端请求失败: " << error.what() << '\n';
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### calc_client.cpp

~~~cpp
#include "net.hpp"

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

void send_u32(int fd, std::uint32_t value) {
    const std::uint32_t wire = htonl(value);
    net::send_all(fd, {reinterpret_cast<const char*>(&wire), sizeof(wire)});
}

void send_i32(int fd, std::int32_t value) {
    std::uint32_t host_value = 0;
    std::memcpy(&host_value, &value, sizeof(value));
    send_u32(fd, host_value);
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) return 2;
        std::size_t count = 0;
        std::cout << "操作数数量: ";
        if (!(std::cin >> count) || count == 0 || count > 32) {
            throw std::runtime_error("操作数数量必须在 1 到 32 之间");
        }
        std::vector<std::int32_t> values(count);
        for (auto& value : values) {
            std::cout << "操作数: ";
            if (!(std::cin >> value)) throw std::runtime_error("无效操作数");
        }
        char op = '\0';
        std::cout << "运算符 (+、-、*): ";
        if (!(std::cin >> op)) throw std::runtime_error("无效运算符");

        auto socket = net::tcp_connect(argv[1], argv[2]);
        send_u32(socket.get(), static_cast<std::uint32_t>(values.size()));
        for (const auto value : values) {
            send_i32(socket.get(), value);
        }
        net::send_all(socket.get(), {&op, 1});

        std::uint32_t wire = 0;
        if (!net::receive_exact(socket.get(), &wire, sizeof(wire))) {
            throw std::runtime_error("服务器未返回结果");
        }
        const std::uint32_t host_result = ntohl(wire);
        std::int32_t result = 0;
        std::memcpy(&result, &host_result, sizeof(result));
        std::cout << "结果: " << result << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

## UDP 回声服务器与客户端

sendto 的目标地址是输入参数；recvfrom 要写入来源地址和实际长度，因此地址与
长度都是输出参数。每次调用 recvfrom 前，都应把地址长度恢复成缓冲区容量。
UDP 保留数据报边界，接收缓冲区过小时，数据报剩余部分会被丢弃。

### udp_server.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto socket = net::udp_bind(argv[1]);
        std::array<char, 65535> buffer{};
        for (;;) {
            net::Endpoint peer;
            peer.length = sizeof(peer.address);
            const ssize_t count = ::recvfrom(
                socket.get(), buffer.data(), buffer.size(), 0,
                reinterpret_cast<sockaddr*>(&peer.address), &peer.length);
            if (count < 0) {
                if (errno == EINTR) continue;
                net::throw_errno("recvfrom");
            }
            const ssize_t sent = ::sendto(
                socket.get(), buffer.data(), static_cast<std::size_t>(count), 0,
                reinterpret_cast<const sockaddr*>(&peer.address), peer.length);
            if (sent < 0) net::throw_errno("send UDP datagram");
            if (sent != count) {
                throw std::runtime_error("UDP 数据报只发送了一部分");
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### udp_client.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <exception>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) return 2;
        auto socket = net::udp_connect(argv[1], argv[2]);
        std::array<char, 65535> buffer{};

        for (std::string line; std::getline(std::cin, line);) {
            if (line == "q" || line == "Q") break;
            ssize_t sent = -1;
            do {
                sent = ::send(socket.get(), line.data(), line.size(),
                              MSG_NOSIGNAL);
            } while (sent < 0 && errno == EINTR);
            if (sent < 0) net::throw_errno("send UDP datagram");
            if (static_cast<std::size_t>(sent) != line.size()) {
                throw std::runtime_error("UDP 数据报只发送了一部分");
            }
            ssize_t count = -1;
            do {
                count =
                    ::recv(socket.get(), buffer.data(), buffer.size(), 0);
            } while (count < 0 && errno == EINTR);
            if (count < 0) net::throw_errno("receive UDP datagram");
            std::cout << "服务器回复: ";
            std::cout.write(buffer.data(), count);
            std::cout << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

客户端通常不需要显式 bind。调用 connect/send 时，内核会根据路由选择本地 IP
并分配临时端口。这里对 UDP 套接字调用 connect 只是在内核中固定默认对端，不会
进行 TCP 握手；之后 recv 只会返回这个对端的数据报。

## 多线程聊天室

聊天室广播没有定义消息边界，这里把它当作字节流演示。真实协议应再增加长度
前缀或换行解析。注册表锁只保护容器；每个客户端另有发送锁，避免多个线程向同一
TCP 流交错写入。

### chat_server.cpp

~~~cpp
#include "net.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

struct Client {
    explicit Client(net::UniqueFd socket) : socket(std::move(socket)) {}
    net::UniqueFd socket;
    std::mutex send_mutex;
};

std::mutex clients_mutex;
std::vector<std::shared_ptr<Client>> clients;

void broadcast(const char* data, std::size_t size) {
    std::vector<std::shared_ptr<Client>> snapshot;
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        snapshot = clients;
    }
    for (const auto& client : snapshot) {
        try {
            std::lock_guard<std::mutex> lock(client->send_mutex);
            net::send_all(client->socket.get(), {data, size});
        } catch (const std::exception&) {
            // 对端会由对应的接收线程移出注册表。
        }
    }
}

void serve(const std::shared_ptr<Client>& client) {
    std::array<char, 4096> buffer{};
    for (;;) {
        const ssize_t count =
            ::recv(client->socket.get(), buffer.data(), buffer.size(), 0);
        if (count > 0) {
            broadcast(buffer.data(), static_cast<std::size_t>(count));
        } else if (count == 0) {
            break;
        } else if (errno != EINTR) {
            break;
        }
    }

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client),
                  clients.end());
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto listener = net::tcp_listener(argv[1]);
        for (;;) {
            net::Endpoint peer;
            auto client =
                std::make_shared<Client>(net::accept(listener.get(), peer));
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(client);
            }
            std::thread(serve, std::move(client)).detach();
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### chat_client.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

void send_messages(int fd, const std::string& name) {
    try {
        for (std::string line; std::getline(std::cin, line);) {
            if (line == "q" || line == "Q") break;
            net::send_all(fd, '[' + name + "] " + line + '\n');
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
    }
    ::shutdown(fd, SHUT_WR);
}

void receive_messages(int fd) {
    std::array<char, 4096> buffer{};
    for (;;) {
        const ssize_t count = ::recv(fd, buffer.data(), buffer.size(), 0);
        if (count > 0) {
            std::cout.write(buffer.data(), count);
            std::cout.flush();
        } else if (count == 0) {
            return;
        } else if (errno != EINTR) {
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "用法: " << argv[0]
                      << " <主机名或 IP> <端口> <昵称>\n";
            return 2;
        }
        auto socket = net::tcp_connect(argv[1], argv[2]);
        std::thread sender(send_messages, socket.get(), argv[3]);
        std::thread receiver(receive_messages, socket.get());
        sender.join();
        receiver.join();
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

主线程持有 socket 的 UniqueFd，两个工作线程只借用其整数值，并且都在析构前
join，因此不会产生悬空文件描述符。

## epoll

不要从 glibc 头文件复制内部声明。应用只需包含 sys/epoll.h，并使用这些公开
接口：

- epoll_create1(EPOLL_CLOEXEC) 创建实例；
- epoll_ctl 增删改关注的文件描述符；
- epoll_wait 等待就绪事件；
- epoll_pwait2 在较新内核与 libc 上提供 timespec 精度的超时。

旧的 epoll_create(size) 中 size 早已只是被忽略的兼容参数，新代码应使用
epoll_create1。

### 水平触发回声服务器

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <cstdint>
#include <exception>
#include <iostream>
#include <unordered_map>

#include <sys/epoll.h>

void add_to_epoll(int epoll_fd, int fd, std::uint32_t flags) {
    epoll_event event{};
    event.events = flags;
    event.data.fd = fd;
    if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
        net::throw_errno("epoll_ctl");
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto listener = net::tcp_listener(argv[1]);
        net::UniqueFd epoll{::epoll_create1(EPOLL_CLOEXEC)};
        if (!epoll) net::throw_errno("epoll_create1");
        add_to_epoll(epoll.get(), listener.get(), EPOLLIN);

        std::unordered_map<int, net::UniqueFd> clients;
        std::array<epoll_event, 64> events{};
        std::array<char, 4096> buffer{};

        for (;;) {
            const int count = ::epoll_wait(
                epoll.get(), events.data(), static_cast<int>(events.size()), -1);
            if (count < 0) {
                if (errno == EINTR) continue;
                net::throw_errno("epoll_wait");
            }

            for (int i = 0; i < count; ++i) {
                const int fd = events[static_cast<std::size_t>(i)].data.fd;
                if (fd == listener.get()) {
                    net::Endpoint peer;
                    auto client = net::accept(listener.get(), peer);
                    const int client_fd = client.get();
                    add_to_epoll(epoll.get(), client_fd,
                                 EPOLLIN | EPOLLRDHUP);
                    clients.emplace(client_fd, std::move(client));
                    continue;
                }

                const ssize_t received =
                    ::recv(fd, buffer.data(), buffer.size(), 0);
                bool close_client = false;
                if (received > 0) {
                    try {
                        close_client = !net::try_send_all(
                            fd, {buffer.data(),
                                 static_cast<std::size_t>(received)});
                    } catch (const std::exception&) {
                        close_client = true;
                    }
                } else if (received == 0) {
                    close_client = true;
                } else if (errno != EINTR) {
                    close_client = true;
                }
                if (close_client) {
                    ::epoll_ctl(epoll.get(), EPOLL_CTL_DEL, fd, nullptr);
                    clients.erase(fd);
                }
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

水平触发下，只要仍有数据可读，epoll_wait 就会继续报告该文件描述符。这个示例
可配合前面的 echo_client.cpp 使用。为了保持代码简单，示例不维护输出队列；
如果回声数据不能立即写完，就关闭这个慢客户端，避免阻塞整个事件循环。

### 边缘触发回声服务器

边缘触发必须配合非阻塞文件描述符。accept、recv 和 send 都要执行到返回
EAGAIN/EWOULDBLOCK。send 也可能暂时写不下，所以待发送数据必须保存在连接
对象中，并临时关注 EPOLLOUT。示例把每个连接的待发送数据限制为 1 MiB，超过
限制时直接关闭连接。

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>

#include <sys/epoll.h>

struct Connection {
    net::UniqueFd socket;
    std::string pending;
    std::size_t sent = 0;
    bool peer_closed = false;
};

void control(int epoll_fd, int operation, int fd, std::uint32_t flags) {
    epoll_event event{};
    event.events = flags;
    event.data.fd = fd;
    if (::epoll_ctl(epoll_fd, operation, fd, &event) == -1) {
        net::throw_errno("epoll_ctl");
    }
}

void update_interest(int epoll_fd, int fd, const Connection& connection) {
    std::uint32_t flags = EPOLLIN | EPOLLRDHUP | EPOLLET;
    if (connection.sent < connection.pending.size()) flags |= EPOLLOUT;
    control(epoll_fd, EPOLL_CTL_MOD, fd, flags);
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto listener = net::tcp_listener(argv[1]);
        net::set_nonblocking(listener.get());
        net::UniqueFd epoll{::epoll_create1(EPOLL_CLOEXEC)};
        if (!epoll) net::throw_errno("epoll_create1");
        control(epoll.get(), EPOLL_CTL_ADD, listener.get(), EPOLLIN | EPOLLET);

        std::unordered_map<int, Connection> connections;
        std::array<epoll_event, 64> events{};
        std::array<char, 4096> buffer{};
        constexpr std::size_t max_pending = 1024 * 1024;

        for (;;) {
            const int ready = ::epoll_wait(
                epoll.get(), events.data(), static_cast<int>(events.size()), -1);
            if (ready < 0) {
                if (errno == EINTR) continue;
                net::throw_errno("epoll_wait");
            }

            for (int i = 0; i < ready; ++i) {
                const auto event = events[static_cast<std::size_t>(i)];
                const int fd = event.data.fd;
                if (fd == listener.get()) {
                    for (;;) {
                        net::Endpoint peer;
                        peer.length = sizeof(peer.address);
                        const int client_fd = ::accept4(
                            listener.get(),
                            reinterpret_cast<sockaddr*>(&peer.address),
                            &peer.length, SOCK_CLOEXEC | SOCK_NONBLOCK);
                        if (client_fd >= 0) {
                            connections.emplace(
                                client_fd,
                                Connection{net::UniqueFd{client_fd}, "", 0,
                                           false});
                            control(epoll.get(), EPOLL_CTL_ADD, client_fd,
                                    EPOLLIN | EPOLLRDHUP | EPOLLET);
                        } else if (errno == EINTR) {
                            continue;
                        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            break;
                        } else {
                            net::throw_errno("accept4");
                        }
                    }
                    continue;
                }

                auto found = connections.find(fd);
                if (found == connections.end()) continue;
                auto& connection = found->second;

                if ((event.events & EPOLLERR) != 0) {
                    connection.peer_closed = true;
                    connection.pending.clear();
                }

                if ((event.events & (EPOLLIN | EPOLLRDHUP | EPOLLHUP)) != 0) {
                    for (;;) {
                        const ssize_t count =
                            ::recv(fd, buffer.data(), buffer.size(), 0);
                        if (count > 0) {
                            const auto size = static_cast<std::size_t>(count);
                            if (size > max_pending -
                                           connection.pending.size()) {
                                connection.peer_closed = true;
                                connection.pending.clear();
                                break;
                            }
                            connection.pending.append(
                                buffer.data(), size);
                        } else if (count == 0) {
                            connection.peer_closed = true;
                            break;
                        } else if (errno == EINTR) {
                            continue;
                        } else if (errno == EAGAIN ||
                                   errno == EWOULDBLOCK) {
                            break;
                        } else {
                            connection.peer_closed = true;
                            connection.pending.clear();
                            break;
                        }
                    }
                }

                while (connection.sent < connection.pending.size()) {
                    const char* data =
                        connection.pending.data() + connection.sent;
                    const std::size_t size =
                        connection.pending.size() - connection.sent;
                    const ssize_t count =
                        ::send(fd, data, size, MSG_NOSIGNAL);
                    if (count > 0) {
                        connection.sent += static_cast<std::size_t>(count);
                    } else if (count < 0 && errno == EINTR) {
                        continue;
                    } else if (count < 0 &&
                               (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        break;
                    } else {
                        connection.peer_closed = true;
                        connection.pending.clear();
                        break;
                    }
                }

                if (connection.sent == connection.pending.size()) {
                    connection.pending.clear();
                    connection.sent = 0;
                }
                if (connection.peer_closed && connection.pending.empty()) {
                    ::epoll_ctl(epoll.get(), EPOLL_CTL_DEL, fd, nullptr);
                    connections.erase(found);
                } else {
                    update_interest(epoll.get(), fd, connection);
                }
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

## 多进程服务器

将 SIGCHLD 设置为自动回收，避免在异步信号处理函数里调用 iostream 等非信号
安全操作。fork 后父子进程拥有各自的文件描述符表；双方的 UniqueFd 必须分别
释放自己不再使用的副本。

### process_server.cpp

~~~cpp
#include "net.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <csignal>
#include <exception>
#include <iostream>

#include <sys/wait.h>
#include <unistd.h>

void serve_client(net::UniqueFd client) {
    std::array<char, 4096> buffer{};
    for (;;) {
        const ssize_t count =
            ::recv(client.get(), buffer.data(), buffer.size(), 0);
        if (count > 0) {
            net::send_all(client.get(),
                          {buffer.data(), static_cast<std::size_t>(count)});
        } else if (count == 0) {
            return;
        } else if (errno != EINTR) {
            net::throw_errno("recv");
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;

        struct sigaction action {};
        action.sa_handler = SIG_IGN;
        action.sa_flags = SA_NOCLDWAIT;
        ::sigemptyset(&action.sa_mask);
        if (::sigaction(SIGCHLD, &action, nullptr) == -1) {
            net::throw_errno("sigaction");
        }

        auto listener = net::tcp_listener(argv[1]);
        for (;;) {
            net::Endpoint peer;
            auto client = net::accept(listener.get(), peer);
            const pid_t pid = ::fork();
            if (pid == -1) {
                std::cerr << "fork: " << std::strerror(errno) << '\n';
                continue;
            }
            if (pid == 0) {
                listener.reset();
                serve_client(std::move(client));
                return 0;
            }
            client.reset();
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

客户端直接使用前文的 echo_client.cpp。底层连接只有在指向它的所有描述符都
关闭后才真正释放，所以父进程必须关闭已交给子进程处理的连接描述符。

### 用两个进程分离客户端读写

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <iostream>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) return 2;
        auto socket = net::tcp_connect(argv[1], argv[2]);
        const pid_t pid = ::fork();
        if (pid == -1) net::throw_errno("fork");

        if (pid == 0) {
            for (std::string line; std::getline(std::cin, line);) {
                if (line == "q" || line == "Q") break;
                line.push_back('\n');
                net::send_all(socket.get(), line);
            }
            if (::shutdown(socket.get(), SHUT_WR) == -1) {
                net::throw_errno("shutdown");
            }
            return 0;
        }

        std::array<char, 4096> buffer{};
        for (;;) {
            const ssize_t count =
                ::recv(socket.get(), buffer.data(), buffer.size(), 0);
            if (count > 0) {
                std::cout.write(buffer.data(), count);
                std::cout.flush();
            } else if (count == 0) {
                break;
            } else if (errno != EINTR) {
                net::throw_errno("recv");
            }
        }
        while (::waitpid(pid, nullptr, 0) == -1 && errno == EINTR) {}
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

shutdown(SHUT_WR) 作用于底层套接字连接，而不只是某一个描述符副本。子进程
结束发送后，父进程仍可继续接收，直到服务器关闭其发送方向。

## 进程间通信：管道

传统 pipe 本身没有被废弃；Linux 新代码可优先用 pipe2(O_CLOEXEC)，原子设置
close-on-exec，避免在多线程程序中产生竞态。匿名管道是单向字节流，双向通信
通常使用两条管道；需要全双工本地通道时也可考虑 socketpair。

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <iostream>
#include <string_view>

#include <sys/wait.h>
#include <unistd.h>

using Pipe = std::array<net::UniqueFd, 2>;

Pipe make_pipe() {
    std::array<int, 2> raw{};
    if (::pipe2(raw.data(), O_CLOEXEC) == -1) net::throw_errno("pipe2");
    return {net::UniqueFd{raw[0]}, net::UniqueFd{raw[1]}};
}

void write_all(int fd, std::string_view data) {
    while (!data.empty()) {
        const ssize_t count = ::write(fd, data.data(), data.size());
        if (count > 0) {
            data.remove_prefix(static_cast<std::size_t>(count));
        } else if (count < 0 && errno == EINTR) {
            continue;
        } else {
            net::throw_errno("write");
        }
    }
}

std::string read_once(int fd) {
    std::array<char, 128> buffer{};
    for (;;) {
        const ssize_t count = ::read(fd, buffer.data(), buffer.size());
        if (count >= 0) {
            return {buffer.data(), static_cast<std::size_t>(count)};
        }
        if (errno != EINTR) net::throw_errno("read");
    }
}

int main() {
    try {
        auto parent_to_child = make_pipe();
        auto child_to_parent = make_pipe();
        const pid_t pid = ::fork();
        if (pid == -1) net::throw_errno("fork");

        if (pid == 0) {
            parent_to_child[1].reset();
            child_to_parent[0].reset();
            std::cout << read_once(parent_to_child[0].get()) << '\n';
            write_all(child_to_parent[1].get(), "I am Fiona!");
            return 0;
        }

        parent_to_child[0].reset();
        child_to_parent[1].reset();
        write_all(parent_to_child[1].get(), "Who are you?");
        parent_to_child[1].reset();
        std::cout << read_once(child_to_parent[0].get()) << '\n';
        while (::waitpid(pid, nullptr, 0) == -1 && errno == EINTR) {}
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

## select

select 仍是有效的 POSIX API，并未被废弃；但它受 FD_SETSIZE 限制，而且每次
调用都要复制并线性扫描 fd_set。Linux 上的大量连接通常更适合 epoll。

### 等待标准输入

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <iostream>

#include <sys/select.h>
#include <unistd.h>

int main() {
    std::array<char, 128> buffer{};
    for (;;) {
        fd_set readable;
        FD_ZERO(&readable);
        FD_SET(STDIN_FILENO, &readable);
        timeval timeout{5, 0};  // select 会修改它，所以每轮重新创建。

        const int ready =
            ::select(STDIN_FILENO + 1, &readable, nullptr, nullptr, &timeout);
        if (ready < 0) {
            if (errno == EINTR) continue;
            std::cerr << "select: " << std::strerror(errno) << '\n';
            return 1;
        }
        if (ready == 0) {
            std::cout << "超时\n";
            continue;
        }

        const ssize_t count =
            ::read(STDIN_FILENO, buffer.data(), buffer.size());
        if (count <= 0) return count == 0 ? 0 : 1;
        std::cout << "标准输入: ";
        std::cout.write(buffer.data(), count);
    }
}
~~~

select 返回可读只表示此刻读取不会阻塞；数据已经位于对应文件描述符的内核接收
缓冲区。超时返回后，不代表连接发生了错误。

### select_echo_server.cpp

~~~cpp
#include "net.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <exception>
#include <iostream>
#include <unordered_map>

#include <sys/select.h>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto listener = net::tcp_listener(argv[1]);
        if (listener.get() >= FD_SETSIZE) {
            throw std::runtime_error("监听 fd 超出 FD_SETSIZE");
        }

        fd_set all;
        FD_ZERO(&all);
        FD_SET(listener.get(), &all);
        int max_fd = listener.get();
        std::unordered_map<int, net::UniqueFd> clients;
        std::array<char, 4096> buffer{};

        for (;;) {
            fd_set readable = all;
            timeval timeout{5, 0};
            const int ready =
                ::select(max_fd + 1, &readable, nullptr, nullptr, &timeout);
            if (ready < 0) {
                if (errno == EINTR) continue;
                net::throw_errno("select");
            }
            if (ready == 0) continue;

            if (FD_ISSET(listener.get(), &readable)) {
                net::Endpoint peer;
                auto client = net::accept(listener.get(), peer);
                const int fd = client.get();
                if (fd < FD_SETSIZE) {
                    FD_SET(fd, &all);
                    max_fd = std::max(max_fd, fd);
                    clients.emplace(fd, std::move(client));
                }
            }

            for (auto iterator = clients.begin(); iterator != clients.end();) {
                const int fd = iterator->first;
                if (!FD_ISSET(fd, &readable)) {
                    ++iterator;
                    continue;
                }
                const ssize_t count =
                    ::recv(fd, buffer.data(), buffer.size(), 0);
                bool close_client = false;
                if (count > 0) {
                    try {
                        close_client = !net::try_send_all(
                            fd, {buffer.data(),
                                 static_cast<std::size_t>(count)});
                    } catch (const std::exception&) {
                        close_client = true;
                    }
                } else if (count < 0 && errno == EINTR) {
                    close_client = false;
                } else {
                    close_client = true;
                }

                if (close_client) {
                    FD_CLR(fd, &all);
                    iterator = clients.erase(iterator);
                } else {
                    ++iterator;
                }
            }

            while (max_fd > listener.get() && !FD_ISSET(max_fd, &all)) {
                --max_fd;
            }
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

该服务器同样可配合 echo_client.cpp 使用。它也采用“写不下就关闭慢客户端”的
简单策略，避免在 select 事件循环中执行阻塞等待。

## 多播

多播发送方把 UDP 数据报发送到组地址；接收方绑定端口并加入该组。下面使用
RFC 3678 定义的 MCAST_JOIN_GROUP，而不是只支持 IPv4 的旧式 ip_mreq 写法。
接口索引为 0 表示由内核选择默认接口；生产程序通常应允许用户指定接口索引。

### multicast_sender.cpp

~~~cpp
#include "net.hpp"

#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <netinet/in.h>

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "用法: " << argv[0]
                      << " <组地址> <端口> <文件>\n";
            return 2;
        }
        const auto group = net::endpoint(argv[1], argv[2], SOCK_DGRAM);
        auto socket = net::udp_socket(group.address.ss_family);

        if (group.address.ss_family == AF_INET) {
            const unsigned char ttl = 32;
            if (::setsockopt(socket.get(), IPPROTO_IP, IP_MULTICAST_TTL,
                             &ttl, sizeof(ttl)) == -1) {
                net::throw_errno("setsockopt IP_MULTICAST_TTL");
            }
        } else {
            const int hops = 32;
            if (::setsockopt(socket.get(), IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
                             &hops, sizeof(hops)) == -1) {
                net::throw_errno("setsockopt IPV6_MULTICAST_HOPS");
            }
        }

        std::ifstream input(argv[3]);
        if (!input) throw std::runtime_error("无法打开输入文件");
        for (std::string line; std::getline(input, line);) {
            line.push_back('\n');
            const ssize_t count = ::sendto(
                socket.get(), line.data(), line.size(), 0,
                reinterpret_cast<const sockaddr*>(&group.address),
                group.length);
            if (count < 0) net::throw_errno("sendto");
            if (static_cast<std::size_t>(count) != line.size()) {
                throw std::runtime_error("UDP 数据报只发送了一部分");
            }
            std::this_thread::sleep_for(std::chrono::seconds{1});
        }
        if (!input.eof()) throw std::runtime_error("读取输入文件失败");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### multicast_receiver.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <exception>
#include <iostream>

#include <netinet/in.h>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "用法: " << argv[0] << " <组地址> <端口>\n";
            return 2;
        }
        const auto group = net::endpoint(argv[1], argv[2], SOCK_DGRAM);
        auto socket = net::udp_bind(argv[2], group.address.ss_family);

        group_req request{};
        request.gr_interface = 0;
        std::memcpy(&request.gr_group, &group.address, group.length);
        const int level =
            group.address.ss_family == AF_INET ? IPPROTO_IP : IPPROTO_IPV6;
        if (::setsockopt(socket.get(), level, MCAST_JOIN_GROUP,
                         &request, sizeof(request)) == -1) {
            net::throw_errno("setsockopt MCAST_JOIN_GROUP");
        }

        std::array<char, 65535> buffer{};
        for (;;) {
            const ssize_t count =
                ::recvfrom(socket.get(), buffer.data(), buffer.size(), 0,
                           nullptr, nullptr);
            if (count < 0) {
                if (errno == EINTR) continue;
                net::throw_errno("recvfrom");
            }
            std::cout.write(buffer.data(), count);
            std::cout.flush();
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

TTL 或 hop limit 限制数据报可经过的路由跳数。多播不是可靠传输，仍可能丢包、
重复或乱序；需要可靠性时应由应用协议补充。

## 广播

IP 广播只适用于 IPv4。发送方必须设置 SO_BROADCAST；该选项的值是整数布尔值，
不是目标 sockaddr。255.255.255.255 是本地受限广播，路由器通常不会转发；
定向广播也通常因安全策略被路由器禁用。

### broadcast_sender.cpp

~~~cpp
#include "net.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "用法: " << argv[0]
                      << " <广播地址> <端口> <文件>\n";
            return 2;
        }
        const auto target =
            net::endpoint(argv[1], argv[2], SOCK_DGRAM, AF_INET);
        auto socket = net::udp_socket(AF_INET);
        const int enabled = 1;
        if (::setsockopt(socket.get(), SOL_SOCKET, SO_BROADCAST,
                         &enabled, sizeof(enabled)) == -1) {
            net::throw_errno("setsockopt SO_BROADCAST");
        }

        std::ifstream input(argv[3]);
        if (!input) throw std::runtime_error("无法打开输入文件");
        for (std::string line; std::getline(input, line);) {
            line.push_back('\n');
            const ssize_t count = ::sendto(
                socket.get(), line.data(), line.size(), 0,
                reinterpret_cast<const sockaddr*>(&target.address),
                target.length);
            if (count < 0) net::throw_errno("sendto");
            if (static_cast<std::size_t>(count) != line.size()) {
                throw std::runtime_error("UDP 数据报只发送了一部分");
            }
        }
        if (!input.eof()) throw std::runtime_error("读取输入文件失败");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### broadcast_receiver.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) return 2;
        auto socket = net::udp_bind(argv[1], AF_INET);
        std::array<char, 65535> buffer{};
        for (;;) {
            const ssize_t count =
                ::recvfrom(socket.get(), buffer.data(), buffer.size(), 0,
                           nullptr, nullptr);
            if (count < 0) {
                if (errno == EINTR) continue;
                net::throw_errno("recvfrom");
            }
            std::cout.write(buffer.data(), count);
            std::cout.flush();
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

## TCP 半关闭与文件传输

服务器发送完文件后对连接套接字调用 shutdown(SHUT_WR)。客户端因此能从 recv
得到 0，明确知道文件结束，同时连接的反向仍然可用。监听套接字不参与数据传输，
不能对它做这个半关闭。

### file_server.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "用法: " << argv[0] << " <端口> <文件>\n";
            return 2;
        }
        auto listener = net::tcp_listener(argv[1]);
        net::Endpoint peer;
        auto client = net::accept(listener.get(), peer);

        std::ifstream input(argv[2], std::ios::binary);
        if (!input) throw std::runtime_error("无法打开输入文件");
        std::array<char, 16384> buffer{};
        while (input) {
            input.read(buffer.data(), buffer.size());
            const std::streamsize count = input.gcount();
            if (count > 0) {
                net::send_all(
                    client.get(),
                    {buffer.data(), static_cast<std::size_t>(count)});
            }
        }
        if (!input.eof()) throw std::runtime_error("读取文件失败");
        if (::shutdown(client.get(), SHUT_WR) == -1) {
            net::throw_errno("shutdown");
        }

        std::string acknowledgement;
        constexpr std::size_t max_acknowledgement = 4096;
        for (;;) {
            const ssize_t count =
                ::recv(client.get(), buffer.data(), buffer.size(), 0);
            if (count > 0) {
                const auto size = static_cast<std::size_t>(count);
                if (size > max_acknowledgement -
                               acknowledgement.size()) {
                    throw std::runtime_error("客户端回复过长");
                }
                acknowledgement.append(
                    buffer.data(), size);
            } else if (count == 0) {
                break;
            } else if (errno != EINTR) {
                net::throw_errno("recv");
            }
        }
        std::cout << "客户端回复: " << acknowledgement << '\n';
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

### file_client.cpp

~~~cpp
#include "net.hpp"

#include <array>
#include <cerrno>
#include <exception>
#include <fstream>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            std::cerr << "用法: " << argv[0]
                      << " <主机名或 IP> <端口> <输出文件>\n";
            return 2;
        }
        auto socket = net::tcp_connect(argv[1], argv[2]);
        std::ofstream output(argv[3], std::ios::binary);
        if (!output) throw std::runtime_error("无法打开输出文件");

        std::array<char, 16384> buffer{};
        for (;;) {
            const ssize_t count =
                ::recv(socket.get(), buffer.data(), buffer.size(), 0);
            if (count > 0) {
                output.write(buffer.data(), count);
                if (!output) throw std::runtime_error("写入文件失败");
            } else if (count == 0) {
                break;
            } else if (errno != EINTR) {
                net::throw_errno("recv");
            }
        }
        output.close();
        if (!output) throw std::runtime_error("关闭输出文件失败");

        constexpr std::string_view reply = "Thank you, bye!";
        net::send_all(socket.get(), reply);
        if (::shutdown(socket.get(), SHUT_WR) == -1) {
            net::throw_errno("shutdown");
        }
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
~~~

直接 close 连接套接字会同时放弃两个方向；半关闭则能把协议中的“我已发送完毕”
表达为对端可观察的 EOF，同时保留另一方向完成确认消息。

# C with Class 网络编程示例

## 单读多写 TCP Server

### x.1 server

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(const std::string& message) {
    perror(message.c_str());
    exit(1);
}

int main(int argc, char** argv) {
    // 修正：只需要1个端口参数，所以 argc 应该是 2
    if (argc != 2) {
        std::cerr << "Usage : " << argv[0] << " <port>\n";
        exit(1);
    }

    // 修正：统一使用 serv_sock
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    // C++11 统一初始化（{} 会自动清零，省去 memset）
    struct sockaddr_in serv_addr{}; 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(std::stoi(argv[1])); // 改用 std::stoi

    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    std::cout << "Server started on port " << argv[1] << std::endl;

    while (true) {
        struct sockaddr_in clnt_addr{};
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

        if (clnt_sock == -1) {
            perror("accept() error");
            continue;
        }

        std::cout << "Client connected: " 
                  << inet_ntoa(clnt_addr.sin_addr) << ":" 
                  << ntohs(clnt_addr.sin_port) << std::endl;

        // 改用 C++ 的 vector 和 string 来管理字符串序列
        std::vector<std::string> messages = {
            "Hello,World!\n",
            "Let's go!\n",
            "Network Programming!\n"
        };

        for (const auto& msg : messages) {
            if (write(clnt_sock, msg.c_str(), msg.length()) == -1) {
                perror("write() error");
                break;
            }
        }
        
        close(clnt_sock);
        std::cout << "Client disconnected\n";
    }
    close(serv_sock);
    return 0;
}
```

### x.2 client

```cpp
#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(const std::string& message) {
    std::cerr << message << std::endl;
    exit(1);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage : " << argv[0] << " <IP> <port>\n";
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    // C++11 统一初始化清零
    struct sockaddr_in serv_addr{}; 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(std::stoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error!");
    }

    std::cout << "Connected to server " << argv[1] << ":" << argv[2] << "\n";

    std::string final_message;
    char buffer[256]; // 固定大小的小缓冲区，循环接水

    while (true) {
        ssize_t read_len = read(sock, buffer, sizeof(buffer));
        if (read_len == -1) {
            error_handling("read() error");
        } else if (read_len == 0) {
            std::cout << "Server closed the connection\n";
            break;
        }
        // 直接将读到的字节追加到 string 中，自动扩容
        final_message.append(buffer, read_len); 
    }

    std::cout << "Received from server: \n" << final_message;

    close(sock);
    return 0;
}
```

注意，这里我们指定的 `port` 是服务器的，客户端的端口号无关紧要，这一点我们从输出可以看出：

```bash
# server
Client connected: 127.0.0.1:35298
Client disconnected
Client connected: 127.0.0.1:35314
Client disconnected
Client connected: 127.0.0.1:33618
Client disconnected

──────────────────────────────────────────────────────────────

# client
Server closed the connection
Recvived from server: 
Hello,World!
Let's go!
Network Programming!
```

可以发现，客户端的端口号每次都在变化。

## 客户端如何给服务器发送数据

下面是一个 echo 客户端服务器的代码：

```cpp
// server.c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>

int serv_sock; // 全局变量，用于信号处理

void error_handling(const char* message) {
    perror(message);
    exit(1);
}

void sigint_handler(int sig) {
    printf("\n Shutting down server...\n");
    close(serv_sock);
    exit(0);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, sigint_handler); // 捕获信号 Ctrl+C

    // 1. socket() 创建服务器套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serv_sock == -1) {
        error_handling("socket() error");
    }

    // 2. bind() 绑定服务器套接字地址信息
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    // 3. listen() 开始监听
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    printf("Server listening on port %s...\n", argv[1]);

    while(1) {
        // 4. accept() 接受客户端连接
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addrlen = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addrlen);
        if(clnt_sock == -1) {
            printf("accpet() error");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

        // 5. write()/read() 具体的 I/O 操作
        char buf[1024];
        int read_len;
        while((read_len = read(clnt_sock, buf, sizeof(buf)))) {
            if(read_len == -1) {
                perror("read() error");
                break;
            }
            buf[read_len] = '\0';
        
            time_t now;
            time(&now);
            char time_str[100];
            sprintf(time_str, "[Server Time: %s]", ctime(&now));
            // ctime() 生成的时间字符串默认以 \n 结尾，这里去除掉移除最后的换行符
            time_str[strlen(time_str) - 1] = '\0'; 

            char reply[1200];
            sprintf(reply, "Echo: %s %s\n", buf, time_str);

            write(clnt_sock, reply, strlen(reply));
            printf("Send to client: %s", reply);
        }
        printf("Client disconnected: %s:%d", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

        // 6. close() 关闭客户端套接字
        close(clnt_sock);
    }

  
    // 7. close() 关闭服务器套接字
    close(serv_sock);
    return 0;
}
```

我们主要关注一下客户端的代码：

```cpp
// client.c
// client.c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(const char *message)
{
    perror(message);
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    // 1. 创建客户端套接字
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1) {
        error_handling("socket() error");
    }

    // 2. 设置服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 3. 连接到服务器
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }
    printf("Connected to server %s:%s\n", argv[1], argv[2]);

    // 4. write/read
    char message[BUF_SIZE];
    char response[BUF_SIZE];
    while(1) {
        printf("Input message(Q to quit): ");
        fgets(message, BUF_SIZE, stdin);
        if(strcmp(message, "Q\n") == 0 || strcmp(message, "q\n") == 0) {
            printf("Close connection...\n");
            break;
        }
        write(sock, message, strlen(message));
        int read_len = read(sock, response, BUF_SIZE - 1);
        if(read_len == -1) {
            error_handling("read() error");
        }
        response[read_len] = '\0';
        printf("Server response: %s\n", response);
    }

    // 5. close client
    close(sock);
    return 0;
}
```

在客户端代码中，我们是通过 `write(sock, message, strlen(message));` 向服务器写入数据的，奇怪的是，这里我们是向 `sock`（客户端 socket）写入数据，那么数据是怎么传到服务器的呢？

这其实是底层的 TCP/IP 协议栈和操作系统内核共同协作的结果，

当客户端调用 `connect()` 成功后，`sock` 不再是一个普通的文件描述符，而是绑定到一条 **已建立的 TCP 连接**。这条连接的两端分别是：

- 客户端：IP + 临时端口（由操作系统自动分配）
- 服务器：IP + 监听端口（如 `9190`）

操作系统内核会为每个 `sock` 维护一个 **发送缓冲区** 和 **接收缓冲区**，处理底层网络通信细节。

当你调用 `write(sock, message, strlen(message))` 时，数据流动分为以下几个阶段：

1. 用户空间 → 内核缓冲区：数据会首先从你的应用程序（用户空间）复制到内核的 **发送缓冲区**
2. 发送缓冲区的数据会经过网络协议栈从客户端的 **发送缓冲区** 传输到服务器 的 **接收缓冲区**
3. 服务器调用 `read` 将数据从内核中的接收缓冲区复制到服务器的用户空间

**阶段 1：用户空间 → 内核发送缓冲区**

- 数据从你的应用程序（用户空间）被复制到内核的 **发送缓冲区**（内核空间）。
- `write()` 的返回值表示成功复制到内核缓冲区的字节数（不保证数据已发送到网络）。

**阶段 2：内核 → 网络协议栈**

- 内核的 TCP 协议栈会：
  1. 将数据分割为合适的 **TCP 段**（Segment）。
  2. 添加 TCP 头部（包含序列号、确认号等）。
  3. 交给 IP 层封装为 IP 数据包。
  4. 通过网卡发送到网络。

**阶段 3：网络 → 服务器内核**

- 数据包经过路由到达服务器的网卡。
- 服务器内核的 TCP 协议栈：
  1. 校验数据包完整性。
  2. 根据 TCP 头部中的端口号，将数据放入对应连接的 **接收缓冲区**。

**阶段 4：服务器应用读取数据**

- 服务器调用 `read()` 时，数据从内核接收缓冲区复制到服务器的用户空间。

最后，在做一个总结：

为什么写入客户端的 `sock` 能发送给服务器？

* 因为 `sock` 已通过 `connect()` 绑定到一条 **特定的 TCP 连接**。所有对该 `sock` 的读写操作，都会自动关联到这条连接的对端（服务器）。

数据是否立即发送？

* 不一定！TCP 默认启用 **Nagle 算法**，会合并小数据包或等待一定延迟后再发送。可通过设置 `TCP_NODELAY` 禁用：

## 进行简单算数运算的服务器

```cpp
// server.h
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>

#define OP_SIZE 4
#define BUF_SIZE 1024

int serv_sock; // 全局变量，用于信号处理

void error_handling(const char* message) {
    perror(message);
    exit(1);
}

void sigint_handler(int sig) {
    printf("\n Shutting down server...\n");
    close(serv_sock);
    exit(0);
}

int calculate(int opnum, int opnds[], char op)
{
    int result = opnds[0], i;
  
    fputs("[opinfo] ", stdout);
    for(i = 0; i < opnum; i ++ )
        printf("%d ", opnds[i]);
    fputs("\n", stdout);

    switch(op)
    {
        case '+':
            for(i = 1; i < opnum; i ++ ) result += opnds[i];
            break;
        case '-':
            for(i = 1; i < opnum; i ++ ) result -= opnds[i];
            break;
        case '*':
            for(i = 1; i < opnum; i ++ ) result *= opnds[i];
            break;
        default:
            error_handling("calculate() error: invalid operator");
    }
    return result;
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage: %s <Port>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, sigint_handler); // 捕获信号 Ctrl+C

    // 1. socket() 创建服务器套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serv_sock == -1) {
        error_handling("socket() error");
    }

    // 2. bind() 绑定服务器套接字地址信息
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = PF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    // 3. listen() 开始监听
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    printf("Server listening on port %s...\n", argv[1]);

    while(1) {
        // 4. accept() 接受客户端连接
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addrlen = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addrlen);
        if(clnt_sock == -1) {
            printf("accpet() error");
            continue;
        }
        printf("Client connected: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

        // 5. write()/read() 具体的 I/O 操作
        char opinfo[BUF_SIZE];
        int opnd_cnt = 0;
        if(read(clnt_sock, &opnd_cnt, 1) == -1) {
            perror("read() error");
            break;
        }

        int recv_len = 0;
        while(recv_len < opnd_cnt * OP_SIZE + 1) {
            int recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE - 1);
            recv_len += recv_cnt;
        }
        int result = calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len - 1]);
        printf("result: %d\n", result);
        write(clnt_sock, &result, sizeof(result));

        // 6. close() 关闭客户端套接字
        close(clnt_sock);
    }

  
    // 7. close() 关闭服务器套接字
    close(serv_sock);
    return 0;
}
```

```cpp
// client.h
// client.c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OP_SIZE 4

void error_handling(const char *message)
{
    perror(message);
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    // 1. 创建客户端套接字
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1) {
        error_handling("socket() error");
    }

    // 2. 设置服务器地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 3. 连接到服务器
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }
    printf("Connected to server %s:%s\n", argv[1], argv[2]);

    // 4. write/read
    int result, opnd_cnt;
    char opmsg[BUF_SIZE];
    fputs("Operand Count: ", stdout);
    scanf("%d", &opnd_cnt);
    opmsg[0] = (char)opnd_cnt;
    for(int i = 0; i < opnd_cnt; i ++ ) {
        printf("Operand %d: ", i + 1);
        scanf("%d", (int*)&opmsg[1 + i * OP_SIZE]);
    }
    fputs("operator: ", stdout);
    scanf(" %c", &opmsg[1 + opnd_cnt * OP_SIZE]); // 忽略之前的回车

    write(sock, opmsg, 2 + opnd_cnt * OP_SIZE);
    read(sock, &result, RLT_SIZE);
  
    printf("Operation result: %d\n", result);

    // 5. close client
    close(sock);
    return 0;
}
```

## 基于 UDP 的服务器/客户端

### 12.1 发送数据

```cpp
#include <sys/socket.h>
# define __CONST_SOCKADDR_ARG	const struct sockaddr *
// Send N bytes of BUF on socket FD to peer at address ADDR which is ADDR_LEN bytes long.  
// Returns the number sent, or -1 for errors.
// This function is a cancellation point and therefore not marked with __THROW.  
extern ssize_t sendto (
    int __fd, // 用于发送数据的 UDP 套接字文件描述符
	const void *__buf, // 用于传输数据的缓冲区的地址
	size_t __n, // 待传输的数据长度，以字节为单位
    int __flags, // 可选项参数，若没有则传递0
    __CONST_SOCKADDR_ARG __addr, // 存有目标地址信息的sockaddr结构体变量的地址值
    socklen_t __addr_len );
```

### 12.2 接受数据

```cpp
#include <sys/socket.h>
# define __SOCKADDR_ARG		struct sockaddr *__restrict
// Read N bytes into BUF through socket FD.
// If ADDR is not NULL, fill in *ADDR_LEN bytes of it with tha address of the sender, and store the actual size of the address in *ADDR_LEN.
// Returns the number of bytes read or -1 for errors.
// This function is a cancellation point and therefore not marked with __THROW. 
extern ssize_t recvfrom (
    int __fd,  // 用于接受数据的 UDP 套接字文件描述符
    void *__restrict __buf, // 接受数据的缓冲区
    size_t __n, // 可接受的最大字节数
    int __flags,
    __SOCKADDR_ARG __addr, // 保存发送端地址
    socklen_t *__restrict __addr_len);
```

### 12.3 `recvfrom` 和 `sendto` 的区别

可以发现，`recvfrom()` 和 `sendto()` 两个函数的参数几乎一模一样，只有在 `sockaddr` 和 `addrlent_t` 这两个参数有细微的差别。

（1） `sendto()` 中，由于地址 `sockaddr` 不会改变，因此是一个 const，而在 `recvfrom()` 中需要对地址赋值，因此不是 const。

（2）是在 `recvfrom()`，`addlen_t` 需要传入一个指针，这是因为 `recvfrom()` 的 `addrlen` 是一个 **输入输出参数（in-out parameter）**：

- **输入时**：调用者告诉内核 `sockaddr` 缓冲区的初始大小（避免内核写入越界）。
- **输出时**：内核返回实际写入的地址结构大小（可能小于初始值）。

例如，我们在客户端使用的地址类型可能是 `sockaddr`，但实际的地址类型可能是 `sockaddr_in` 或 `sockaddr_in6`，而这三者之间的地址长度是不相同的。

### 12.4 回声服务器/客户端实例

注意，UDP 不同于 TCP，不存在连接的请求和受理过程，因此某种意义上无法明确区分服务器和客户端。在这里，一般是根据将提供服务的一方称之为服务器，请求服务的一方称之为客户端。

```cpp
// server.c
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <signal.h>

#define BUF_SIZE 32

void error_handling(char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(serv_sock == -1) {
        error_handling("socket() error");
    }
  
    int optval = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind() error");
    }

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz = sizeof(clnt_addr);
    char message[BUF_SIZE] = {0};

    printf("UDP Echo Server started on port: %s\n", argv[1]);

    while(1) {
        printf("[server] go while(1)\n");
        int read_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);  
        printf("[server] read success\n");
        if(read_len == -1) {
            error_handling("recvfrom() error");
        }
        else if(read_len >= BUF_SIZE) {
            error_handling("Message truncated!");
        }
        message[read_len] = 0;
        printf("Server receive message: %s\n", message);
        if(sendto(serv_sock, message, read_len, 0, (struct sockaddr*)&clnt_addr, clnt_addr_sz) == -1) {
            error_handling("sendto() error");
        }
    }
    close(serv_sock);

    return 0;
}
```

---

```cpp
// client.c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 32

void error_handling(char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }
  
    int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock == -1) {
        error_handling("socket() error");
    }


    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    char message[BUF_SIZE] = {0};

    while(1) {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if(strcmp(message, "q\n") == 0  || strcmp(message, "Q\n") == 0) {
            break;
        }
        printf("[client] strlen of message: %ld\n", strlen(message));
        if(sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
            error_handling("sendto() error");
        }
        struct sockaddr_in from_addr;
        socklen_t from_addr_sz = sizeof(from_addr);
        int read_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_addr, &from_addr_sz);
        if(read_len == -1) {
            error_handling("recvfrom() error");
        }
        message[read_len] = 0;
    
        printf("[client] read len: %d\n", read_len);
        printf("Message from server: %s", message);
    }
    close(sock);

    return 0;
}
```

在客户端代码中我们可以发现，我们没有给客户端的 socket 分配地址，这里其实是在客户端调用 `sendto` 函数时自动分配 IP 地址和端口号。

## 多线程

#### chat_server

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define BUF_SIZE 128
#define MAX_CLNT 256

void *handler_clnt(void *arg);
void send_msg(const char *msg, int len);
void error_handling(const char *msg);

int clnt_cnt; // condition variable
int clnt_socks[MAX_CLNT]; // condition variable
pthread_mutex_t mutex;

#define PTHREAD_LOCK_MUTEX pthread_mutex_lock(&mutex)
#define PTHREAD_UNLOCK_MUTEX pthread_mutex_unlock(&mutex)

int main(int argc, char **argv)
{
    // check command line 
    if(argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
  
    // create serv_sock, bind address and listen 
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    // create mutex 
    pthread_mutex_init(&mutex, NULL);
  
    // 
    while(1) {
        // accept 
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_sz = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
  
        // client limit
        PTHREAD_LOCK_MUTEX;
        if (clnt_cnt >= MAX_CLNT) {
            close(clnt_sock);
            pthread_mutex_unlock(&mutex);
            continue;
        }
        clnt_socks[clnt_cnt++] = clnt_sock;
        PTHREAD_UNLOCK_MUTEX;
  
        // assign a new thread to this client
        pthread_t t_id;
            // convert int to pointer, but in the most of time, INT is 4B and POINTER is 8B
            // so it may be not saft convert INT(4B) to POINTER(8B) directly
            // LONG has the same size to POINTER, so we need to covert INT to LONG first
        pthread_create(&t_id, NULL, handler_clnt, (void*)(long)clnt_sock);
        pthread_detach(t_id);
        printf("Connected client IP: %s\n", inet_ntoa(clnt_addr.sin_addr));
    }
    close(serv_sock);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void *handler_clnt(void *arg)
{
    int clnt_sock = (long)arg;
    ssize_t str_len = 0;
    char msg[BUF_SIZE];
    while((str_len = read(clnt_sock, msg, sizeof(msg))) != 0) {
        send_msg(msg, str_len);
    }
    // remove this client socket 
    PTHREAD_LOCK_MUTEX;
    for(int i = 0; i < clnt_cnt; i ++ ) {
        if(clnt_sock == clnt_socks[i]) {
            clnt_socks[i] = clnt_socks[clnt_cnt - 1];
            clnt_cnt -- ;
            break;
        }
    }
    PTHREAD_UNLOCK_MUTEX;
    close(clnt_sock);
    return NULL;
}

void send_msg(const char *msg, int len)
{
    PTHREAD_LOCK_MUTEX;
    for(int i = 0; i < clnt_cnt; i ++ ) 
        write(clnt_socks[i], msg, len);
    PTHREAD_UNLOCK_MUTEX;
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}
```

#### chat_client

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void* send_msg(void *arg);
void* recv_msg(void *arg);
void error_handling(const char* msg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char **argv)
{
    if(argc != 4) {
        printf("Usgae : %s <IP> <Port> <Name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    pthread_t send_thread, recv_thread;
    pthread_create(&send_thread, NULL, send_msg, (void*)(long)sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void*)(long)sock);

    void *thread_return;
    pthread_join(send_thread, &thread_return);
    pthread_join(recv_thread, &thread_return);

    close(sock);
    return 0;
}

void* send_msg(void *arg)
{
    int sock = (long)arg;
    char name_msg[NAME_SIZE + BUF_SIZE];
    while(1) {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}

void* recv_msg(void *arg)
{
    int sock = (long)arg;
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while(1) {
        str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
        if(str_len == -1) 
            return (void*)-1;
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    return NULL;
}

void error_handling(const char* msg)
{
    perror(msg);
    exit(1);
}
```

## EPOLL

### 1. API

关于 Linux epoll 相关 API，查看 `sys/socket.h` 这个头文件即可：

```cpp
/* Copyright (C) 2002-2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef	_SYS_EPOLL_H
#define	_SYS_EPOLL_H	1

#include <stdint.h>
#include <sys/types.h>

#include <bits/types/sigset_t.h>
#include <bits/types/struct_timespec.h>

/* Get the platform-dependent flags.  */
#include <bits/epoll.h>

#ifndef __EPOLL_PACKED
# define __EPOLL_PACKED
#endif


enum EPOLL_EVENTS
  {
    EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
    EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
    EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
    EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
    EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
    EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
    EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
    EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
    EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
    EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
    EPOLLRDHUP = 0x2000,
#define EPOLLRDHUP EPOLLRDHUP
    EPOLLEXCLUSIVE = 1u << 28,
#define EPOLLEXCLUSIVE EPOLLEXCLUSIVE
    EPOLLWAKEUP = 1u << 29,
#define EPOLLWAKEUP EPOLLWAKEUP
    EPOLLONESHOT = 1u << 30,
#define EPOLLONESHOT EPOLLONESHOT
    EPOLLET = 1u << 31
#define EPOLLET EPOLLET
  };


/* Valid opcodes ( "op" parameter ) to issue to epoll_ctl().  */
#define EPOLL_CTL_ADD 1	/* Add a file descriptor to the interface.  */
#define EPOLL_CTL_DEL 2	/* Remove a file descriptor from the interface.  */
#define EPOLL_CTL_MOD 3	/* Change file descriptor epoll_event structure.  */


typedef union epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} epoll_data_t;

struct epoll_event
{
  uint32_t events;	/* Epoll events */
  epoll_data_t data;	/* User data variable */
} __EPOLL_PACKED;


__BEGIN_DECLS

/* Creates an epoll instance.  Returns an fd for the new instance.
   The "size" parameter is a hint specifying the number of file
   descriptors to be associated with the new instance.  The fd
   returned by epoll_create() should be closed with close().  */
extern int epoll_create (int __size) __THROW;

/* Same as epoll_create but with an FLAGS parameter.  The unused SIZE
   parameter has been dropped.  */
extern int epoll_create1 (int __flags) __THROW;


/* Manipulate an epoll instance "epfd". Returns 0 in case of success,
   -1 in case of error ( the "errno" variable will contain the
   specific error code ) The "op" parameter is one of the EPOLL_CTL_*
   constants defined above. The "fd" parameter is the target of the
   operation. The "event" parameter describes which events the caller
   is interested in and any associated user data.  */
extern int epoll_ctl (int __epfd, int __op, int __fd,
		      struct epoll_event *__event) __THROW;


/* Wait for events on an epoll instance "epfd". Returns the number of
   triggered events returned in "events" buffer. Or -1 in case of
   error with the "errno" variable set to the specific error code. The
   "events" parameter is a buffer that will contain triggered
   events. The "maxevents" is the maximum number of events to be
   returned ( usually size of "events" ). The "timeout" parameter
   specifies the maximum wait time in milliseconds (-1 == infinite).

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int epoll_wait (int __epfd, struct epoll_event *__events,
		       int __maxevents, int __timeout);


/* Same as epoll_wait, but the thread's signal mask is temporarily
   and atomically replaced with the one provided as parameter.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int epoll_pwait (int __epfd, struct epoll_event *__events,
			int __maxevents, int __timeout,
			const __sigset_t *__ss);

/* Same as epoll_pwait, but the timeout as a timespec.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
#ifndef __USE_TIME_BITS64
extern int epoll_pwait2 (int __epfd, struct epoll_event *__events,
			 int __maxevents, const struct timespec *__timeout,
			 const __sigset_t *__ss);
#else
# ifdef __REDIRECT
extern int __REDIRECT (epoll_pwait2, (int __epfd, struct epoll_event *__ev,
				      int __maxevs,
				      const struct timespec *__timeout,
				      const __sigset_t *__ss),
		       __epoll_pwait2_time64);
# else
#  define epoll_pwait2 __epoll_pwait2_time64
# endif
#endif

__END_DECLS

#endif /* sys/epoll.h */
```

### 2. 实例

下面是基于 EPOLL 的回声客户端与服务器。

#### server

```cpp
// Sender just need to create UDP socket and send data to multicast group :)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define EPOLL_SIZE 50
#define BUF_SIZE 4 // 小 buffer 可以测试 epoll_wait 的默认水平触发
void error_handling(const char* message);

int main(int argc, char **argv) 
{
    if(argc != 2) {
        printf("Usage : %s <Port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    int epfd = epoll_create(EPOLL_SIZE);
    struct epoll_event *ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    struct epoll_event event;
  
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
  
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    int idx = 1;

    while(1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1) {
            error_handling("epoll_wait() error");
        }
        printf("epoll wait [%d] \n", idx ++ );
        for(int i = 0; i < event_cnt; i ++ ) {
            if(ep_events[i].data.fd == serv_sock) {
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_sz = sizeof(clnt_addr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
                if(clnt_sock == -1) {
                    error_handling("accept() error");
                }
                printf("client connected: %d\n", clnt_sock);
                event.events = EPOLLIN;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
            }
            else {
                int clnt_sock = ep_events[i].data.fd;
                char buf[BUF_SIZE];
                ssize_t str_len = read(clnt_sock, buf, BUF_SIZE);
                if(str_len == 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, clnt_sock, NULL);
                    printf("client closed: %d\n", clnt_sock);
                    close(clnt_sock);
                }
                else {
                    buf[str_len] = 0;
                    printf("Received data: %s\n", buf);
                    write(clnt_sock, buf, str_len);
                }
            }
        }
    }
  
    close(epfd);
    close(serv_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}
```

#### client

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 1024
void error_handling(const char *msg);

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    printf("client connecting...\n");

    FILE *read_fd = fdopen(sock, "r");
    FILE *write_fp = fdopen(dup(sock), "w");

    char buf[BUF_SIZE];
    while(1)
    {   
        printf("Input message(q to quit): ");
        fgets(buf, BUF_SIZE, stdin);
        if(strcmp(buf, "q\n") == 0 || strcmp(buf, "Q\n") == 0) {
            printf("client quitting...\n");
            break;
        }
        fputs(buf, write_fp);
        fflush(write_fp);
        fgets(buf, BUF_SIZE, read_fd);
        printf("Message received: %s\n", buf);
    }

    fclose(read_fd);
    fclose(write_fp);

    return 0;
}

void error_handling(const char *msg)
{
    perror(msg);
    exit(1);
}
```

### 3. LT & ET

上面给出的例子是 `epoll` 在默认（水平触发）下的测试，下面是 `epoll` 在边缘触发下的测试，客户端沿用上面的即可。

```cpp
// server.c
// Sender just need to create UDP socket and send data to multicast group :)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

#define EPOLL_SIZE 50
#define BUF_SIZE 4 // 小 buffer 可以测试 epoll_wait 的默认水平触发
void error_handling(const char* message);
void setnonblockingmode(int fd); // 设置为非阻塞模式

int main(int argc, char **argv) 
{
    if(argc != 2) {
        printf("Usage : %s <Port>\n", argv[0]);
        exit(1);
    }

    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    setnonblockingmode(serv_sock);

    int epfd = epoll_create(EPOLL_SIZE);
    struct epoll_event *ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
    struct epoll_event event;
  
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
  
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

    int idx = 1;

    while(1)
    {
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1) {
            error_handling("epoll_wait() error");
        }
        printf("epoll wait [%d] \n", idx ++ );
        for(int i = 0; i < event_cnt; i ++ ) {
            if(ep_events[i].data.fd == serv_sock) {
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_sz = sizeof(clnt_addr);
                int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
                setnonblockingmode(clnt_sock); // 设置为非阻塞模式
                if(clnt_sock == -1) {
                    error_handling("accept() error");
                }
                printf("client connected: %d\n", clnt_sock);
                // set edge trigger
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
            }
            else {
                int clnt_sock = ep_events[i].data.fd;
                char buf[BUF_SIZE];
                while(1) { // 直到 read error 或者没有数据可读时(errno=EAGAIN)结束
                    ssize_t str_len = read(clnt_sock, buf, BUF_SIZE);
                    if(str_len == 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, clnt_sock, NULL);
                        close(clnt_sock);
                        printf("client disconnected: %d\n", clnt_sock);
                        break;
                    }
                    if(str_len < 0) {
                        // 非阻塞模式下，稍后再试
                        if(errno == EAGAIN || errno == EWOULDBLOCK) break; 
                        error_handling("read error");
                    }
                    else {
                        write(clnt_sock, buf, str_len);
                        buf[str_len] = 0;
                        printf("Received: %s \n", buf);
                    }
                }
            }
        }
    }
  
    close(epfd);
    close(serv_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}

void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}
```

## 多进程服务器端

### server

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void read_childproc_sig_handler(int sig);
void error_handler(const char *message);

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 1. Register SIGCHLD, clean up zombie porcess.
    struct sigaction act;
    act.sa_handler = read_childproc_sig_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    if(sigaction(SIGCHLD, &act, 0) == -1) {
        error_handler("sigaction() error");
    }

    // 2. create serv_sock && bind && listen
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) {
        error_handler("server socket() error");
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handler("bind() error");
    }
    if(listen(serv_sock, 5) == -1) {
        error_handler("listen() error");
    }
  
    // 3. set address reuse
    int optval = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


    // 4. communicate with client 
    while(1)
    {
        // accept 
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_sz = sizeof(clnt_addr);

        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz);
        if(clnt_sock == -1) {
            perror("client accept() error");
            continue;
        }
        printf("Client connected %s:%d...\n", inet_ntoa(clnt_addr.sin_addr) , ntohs(clnt_addr.sin_port));
  
        // 创建子进程
        pid_t pid = fork();
        if(pid == -1) {
            close(clnt_sock);
            perror("fork() error");
            continue;
        }
        if(pid == 0) { // 子进程
            close(serv_sock);
            printf("Serve client in child proc: %d\n", getpid());
            char buf[BUF_SIZE]; // no need to initialize
            ssize_t read_len; // use ssize_t but int 
            while((read_len = read(clnt_sock, buf, BUF_SIZE - 1)) > 0) {
                buf[read_len] = 0;
                printf("[Received from %s:%d] %s\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port), buf);
                if(write(clnt_sock, buf, read_len) != read_len) {
                    error_handler("write() error in child proc");
                }
            }
            if(read_len == -1) {
                error_handler("read() error in child proc");
            }
            close(clnt_sock);
            printf("Child process %d finished for client %s:%d\n", getpid(), inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
            return 0;
        }
        else {
            close(clnt_sock);
        }
    }
    close(serv_sock);
    return 0;
}

void read_childproc_sig_handler(int sig)
{
    if(sig == SIGCHLD) {
        int status;
        pid_t pid;
        // 这里使用 while 而不是 if 是因为如果短时间内有多个子进程同时终止，SIGCHLD 信号可能只触发一次
        while((pid = waitpid(-1, &status, 0)) > 0) {
            printf("removed proc id: %d\n", pid);
        }
    }
}

void error_handler(const char *message)
{
    perror(message);
    exit(1);
}
```

注意观察代码我们可以发现，在 `fork()` 出来的父进程和子进程当中，我们都需要 `close(clnt_sock)`。这是因为当父进程通过 `fork()` 创建一个子进程时，子进程会**继承父进程所有打开的文件描述符的副本**。这意味着：

1. **父进程**有一个 `clnt_sock`，它代表了与客户端建立的连接。
2. `fork()` 后，**子进程**也会得到一个 `clnt_sock` 的**副本**，这个副本也指向同一个客户端连接。

你可以把这想象成一个文件（也就是客户端连接）有两把独立的钥匙：一把在父进程手里，一把在子进程手里。这两把钥匙都能打开同一个文件。

另外就是要区分文件描述符和底层资源（如套接字连接）之间的关系，一个底层资源可以对应多个文件描述符，例如在上面的代码种，套接字连接 `clnt_sock` 就分别被父进程和子进程引用，而只有当 **所有引用它的文件描述符都被关闭时**，底层资源才会被真正关闭。

### client

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handler(const char *message);

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    // 1. create client socket 
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock == -1) 
        error_handler("socket() error");

    // 2. prepare serv address 
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 3. connect 
    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
        error_handler("connect() error");
    else
        puts("Connected to server...");

    // 4. read/write 
    char msg[BUF_SIZE];
    ssize_t read_len;
    while(1) {
        // write to server 
        printf("Input message(Q to quit): ");
        if(fgets(msg, BUF_SIZE, stdin) == NULL) 
            error_handler("fgets() error");
        if(strcmp(msg, "q\n") == 0 || strcmp(msg, "Q\n") == 0) 
            break;
        msg[strlen(msg) - 1] = 0; // remove '\n' read by fgets 
        if(write(clnt_sock, msg, strlen(msg)) == -1) 
            error_handler("write() error");

        // receive from server 
        memset(msg, 0, sizeof msg);
        read_len = read(clnt_sock, msg, BUF_SIZE - 1);
        if(read_len == -1) {
            error_handler("read() error");
        }
        else if(read_len == 0) {
            puts("Server disconnected.");
            break;
        }
        else {
            msg[read_len] = 0;
            printf("Message from server: %s", msg);
        }
    }

    close(clnt_sock);
    printf("Connection closed.");
    return 0;
}

void error_handler(const char *message)
{
    perror(message);
    exit(1);
}
```

### I/O 程序分隔 client

我们也可以利用多进程的思想，分隔 I/O 程序，让父进程负责读，子进程负责写，或者反过来，这样做可以简化 I/O 的逻辑，因为一个进程要么读，要么写，逻辑上就很清晰。

此外，分隔 I/O 程序到多个进程上时，也可以利用多进程的特性，提高 I/O 效率，例如原本的 `read` 和 `write` 都是阻塞 I/O，现在通过多进程，即使 `read` 处于阻塞状态，`write` 也可以发送数据，因为它们处于不同的进程。

下面是 I/O 程序分隔的客户端

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handler(const char *message);
void write_routine(int sock, char buf[]);
void read_routine(int sock, char buf[]);

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    // 1. create client socket 
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock == -1) 
        error_handler("socket() error");

    // 2. prepare serv address 
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 3. connect 
    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
        error_handler("connect() error");
    else
        puts("Connected to server...");

    // 4. seperate read/write  
    char buf[BUF_SIZE];
    pid_t pid = fork();
    if(pid == 0) 
        write_routine(clnt_sock, buf);
    else
        read_routine(clnt_sock, buf);
    close(clnt_sock);
    return 0;
}

void error_handler(const char *message)
{
    perror(message);
    exit(1);
}

void write_routine(int sock, char buf[])
{
    while(1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if(strcmp(buf, "q\n") == 0 || strcmp("Q\n", buf) == 0) {
            shutdown(sock, SHUT_WR);
            return ;
        }
        write(sock, buf, strlen(buf));
    }
}

void read_routine(int sock, char buf[])
{
    while(1)
    {
        int read_len = read(sock, buf, BUF_SIZE);
        if(read_len == 0) 
            return ;
        buf[read_len] = 0;
        printf("Message from server: %s", buf);

    }
}
```

## 进程间通信

### 1. 管道

```cpp
#include <unistd.h>
/* Create a one-way communication channel (pipe).
If successful, two file descriptors are stored in PIPEDES;
bytes written on PIPEDES[1] can be read from PIPEDES[0].
Returns 0 if successful, -1 if not. */
int pipe(int fileders[2]);
```

示例代码，下面是一个父子进程双向通信的例子：

```cpp
#include <iostream>
#include <cstring>
#include <algorithm>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/wait.h>

using namespace std;

#define BUF_SIZE 32

int main()
{
    int fds1[2], fds2[2];
    char str1[] = "Who are you?";
    char str2[] = "I am Fiona!";
    char buf[BUF_SIZE];

    if(pipe(fds1) == -1) {
        perror("pipe() error");
        exit(1);
    }
    if(pipe(fds2) == -1) {
        perror("pipe() error");
        exit(1);
    }

    pid_t pid = fork();
    if(pid == 0) {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], buf, BUF_SIZE);
        puts(buf);
    }
    else {
        read(fds1[0], buf, BUF_SIZE);
        puts(buf);
        write(fds2[1], str2, sizeof(str2));
    }
    return 0;
}
```

思考一下，可以使用一个管道完成双向通信吗？

## IO 复用服务器

### 1. select

```cpp
#include <sys/select.h>
#include <time.h>

/* Access macros for `fd_set'.  */
#define	FD_SET(fd, fdsetp) 	 __FD_SET (fd, fdsetp)
#define	FD_CLR(fd, fdsetp) 	 __FD_CLR (fd, fdsetp)
#define	FD_ISSET(fd, fdsetp) __FD_ISSET (fd, fdsetp)
#define	FD_ZERO(fdsetp)		 __FD_ZERO (fdsetp)

extern int select (int __nfds, fd_set *__restrict __readfds,
		   fd_set *__restrict __writefds,
		   fd_set *__restrict __exceptfds,
		   struct timeval *__restrict __timeout);


/* A time value that is accurate to the nearest
   microsecond but also has a range of years.  */
struct timeval
{
  __time_t tv_sec;		/* Seconds.  */
  __suseconds_t tv_usec;	/* Microseconds.  */
};
```

演示代码：

```cpp
#include <stdio.h>    // For printf, puts, perror
#include <stdlib.h>   // For exit
#include <string.h>   // For memset, strcspn
#include <unistd.h>   // For read, close
#include <sys/select.h> // For select, FD_ZERO, FD_SET, FD_ISSET, timeval

using namespace std;

#define BUF_SIZE 32

int main()
{
    fd_set reads, temps;
    struct timeval timeout;
    char buf[BUF_SIZE];

    FD_ZERO(&reads);
    FD_SET(0, &reads); // fd 0 is the standard input 

    while(1)
    {
        // 因为 select 会修改 reads，所以这里使用 reads 的拷贝
        temps = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        int result = select(1, &temps, 0, 0, &timeout);
        if(result == -1) {
            puts("select() errror");
            break;
        }
        else if(result == 0) {
            puts("Timt-out!");
        }
        else {
            if(FD_ISSET(0, &temps)) {
                int read_len = read(0, buf, BUF_SIZE);
                buf[read_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}
```

注意，在我们调用 `select()` 之后，结构体 `timeval` 中的成员 `tv_sec` 和 `tv_tv_usec` 的值会被替换为超时前剩余时间，因此，调用 `select` 函数前，每次都需要初始化 `timeval` 结构体变量。

另外，在 `select()`（以及类似的 `poll()` 和 `epoll()`）机制中，当 `select()` 调用返回，并且指示某个文件描述符（例如标准输入 `fd 0`）已经就绪时，这确实意味着：

1. **事件已经发生：** 比如，用户已经在键盘上按下了字符并按下了回车键。
2. **数据已经存在于内核缓冲区中：** 这些数据已经被操作系统从设备（如键盘）读取，并暂时存储在与该文件描述符关联的**内核输入缓冲区**中。

`select()` 的作用就是**通知你**，现在可以**无阻塞地**从这个文件描述符读取数据了，因为数据已经准备好了。如果你此时调用 `read()`，它会立即返回，而不会被阻塞，并且能够从内核缓冲区中取出数据。

### 2. 基于 select 的 echo 服务器端

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>

#define BUF_SIZE 128

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage : %s <port> \n", argv[0]);
        exit(1);
    }

    // 1. 创建服务器套接字、绑定地址、打开监听
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) {
        error_handling("server socket() error");
    }
    int enable = 1;
    if(setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        error_handling("setsockopt() error");
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    // 2. 设置 IO 多路复用
    char buf[BUF_SIZE];
    fd_set reads, cpy_reads;
    int result;
    struct timeval timeout;
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    int fd_max = serv_sock;

    // 3. IO
    while(1) {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
  
        // error 
        if((result = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1) 
            error_handling("select() error");
        // timeout 
        if(result == 0)
            continue;

        for(int i = 0; i < fd_max + 1; i ++ ) {
            if(FD_ISSET(i, &cpy_reads)) {
                // 客户端连接请求
                if(i == serv_sock) {
                    struct sockaddr_in clnt_addr;
                    socklen_t addr_sz = sizeof(clnt_addr);
                    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &addr_sz);
                    printf("Client connected...");
                    if(clnt_sock == -1) {
                        printf("client connected fault...");
                        continue;
                    }
                    FD_SET(clnt_sock, &reads);
                    if(clnt_sock > fd_max) 
                        fd_max = clnt_sock;
                    printf("Connected client: %s:%d (%d)\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port), clnt_sock);
                }
                // 接受客户端消息
                else {
                    ssize_t read_len = read(i, buf, BUF_SIZE);
                    if(read_len > 0) {
                        write(i, buf, read_len);
                    }
                    else {
                        FD_CLR(i, &reads);
                        close(i);
                        // 更新 fd_max
                        int new_fd_max = serv_sock;
                        for(int j = 0; j < fd_max + 1; j ++ ) {
                            if(FD_ISSET(j, &reads)) {
                                if(j > new_fd_max)
                                    new_fd_max = j;
                            }
                        }
                        fd_max = new_fd_max;
                        if(read_len == 0) 
                            printf("closed client: %d\n", i);
                        else // i == -1 
                            perror("read() error");
                    }
                }
            }
        }
    }
    close(serv_sock);
    return 0;
}
```

下面是使用多进程分离 IO 的客户端代码：

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handler(const char *message);
void write_routine(int sock, char buf[]);
void read_routine(int sock, char buf[]);

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    // 1. create client socket 
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock == -1) 
        error_handler("socket() error");

    // 2. prepare serv address 
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 3. connect 
    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) 
        error_handler("connect() error");
    else
        puts("Connected to server...");

    // 4. seperate read/write  
    char buf[BUF_SIZE];
    pid_t pid = fork();
    if(pid == 0) 
        write_routine(clnt_sock, buf);
    else
        read_routine(clnt_sock, buf);
    close(clnt_sock);
    return 0;
}

void error_handler(const char *message)
{
    perror(message);
    exit(1);
}

void write_routine(int sock, char buf[])
{
    while(1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if(strcmp(buf, "q\n") == 0 || strcmp("Q\n", buf) == 0) {
            shutdown(sock, SHUT_WR);
            return ;
        }
        write(sock, buf, strlen(buf));
    }
}

void read_routine(int sock, char buf[])
{
    while(1)
    {
        int read_len = read(sock, buf, BUF_SIZE);
        if(read_len == 0) 
            return ;
        buf[read_len] = 0;
        printf("Message from server: %s", buf);

    }
}
```

## 多播与广播

多播的设计哲学是**“尽可能简单，必要时在应用层增强”**，而 **UDP** 恰好提供了这种灵活性。

### 1. 设置 TTL

通过套接字选项设置 TTL。

```cpp
#define TTL 64   
int time_to_live = TTL;
setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL,
		(void*)&time_to_live, sizeof(time_to_live));
```

### 2. 加入多播组

多播组地址结构：

```cpp
struc ip_mreq { // IP Multicast REQuest
    struct in_addr imr_multiaddr; // 多播组的 IP 地址
    struct in_addr imr_interface; // 加入改组的套接字所属主机的 IP 地址
};
```

通过套接字选项加入多播组。

```cpp
struct ip_mreq join_addr;
join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
join_addr.imr_interface.s_addr = htonl(INADDR_ANY);
setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, 
           (void*)&join_addr, sizeof(join_addr));
```

### 3. 多播实例

在多播通信中，发送方（Sender）直接向多播组地址发送数据，而接收方（Receiver）需绑定端口并加入该组才能接收数据。双方均使用 UDP 套接字，但接收方必须通过 `IP_ADD_MEMBERSHIP` 显式订阅多播组，且发送方可通过 TTL 控制多播范围。

#### sender

```cpp
// Sender just need to create UDP socket and send data to multicast group :)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 128
void error_handling(const char* message);

int main(int argc, char **argv)
{
    // check argument 
    if(argc != 3) {
        printf("Usage : %s <GroupIP> <Port> \n", argv[0]);
        exit(1);
    }

    // create send socket and multicase group address 
    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in mul_addr;
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family = AF_INET;
    mul_addr.sin_port = htons(atoi(argv[2]));
    mul_addr.sin_addr.s_addr = inet_addr(argv[1]);
  
    // set TTL 
    int time_to_live = TTL;
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_to_live, sizeof(time_to_live));

    // open file and send data 
    FILE *fp;
    char buf[BUF_SIZE];

    if((fp = fopen("data.txt", "r")) == NULL)
        error_handling("fopen() error");

    while(!feof(fp)) {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_addr, sizeof(mul_addr));
        sleep(2);
    }

    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}
```

#### receiver

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(const char* message);

int main(int argc, char **argv)
{
    // check command line argument 
    if(argc != 3) {
        printf("Usage : %s <GroupIP> <Port> n", argv[0]);
        exit(1);
    }

    // create receive(localhost -> <INADDR_ANY,port>) socket and bind address
    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[2]));
    if(bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        error_handling("bind() error");

    // request to join in multicast group
    struct ip_mreq join_addr;
    join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_addr, sizeof(join_addr));

    // receive multicast data 
    char buf[BUF_SIZE];
    int line = 0;
    while(1)
    {
        ssize_t read_len = recvfrom(recv_sock, buf, BUF_SIZE- 1, 0, NULL, 0);
        if(read_len < 0)
            break;
        buf[read_len] = 0;
        printf("[%d] %s", ++ line, buf);
    }

    close(recv_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}
```

### 4. 广播

注意 **多播并不是广播的子集**，这是因为广播只能向同一网络的主机发送数据，而多播则可以通过路由器转发实现跨网络通信（忘了多播需要使用 TTL 了吗）。不过与多播相同的是，广播也是用 UDP 进行通信。

另外还需要注意的是，广播不是仅限于本网络，事实上，某个主机可以向任意网络发送广播。具体来说，广播分为两类：

1. **Local Broadcast**
   - **地址**：`255.255.255.255`
   - **范围**：**仅限发送方所在的本地网络**（即当前子网）。
   - **行为**：
     - 数据包不会被路由器转发，直接丢弃。
     - 本地网络内的所有主机都会处理该数据包（若应用层监听对应端口）。
2. **Directed Broadcast**
   - **地址**：子网的广播地址（如 `192.168.1.255` 对应 `192.168.1.0/24` 子网）。
   - **范围**：**可指定目标子网**，但需满足严格条件：
     - **发送方不在目标子网时**：
       - 默认情况下，路由器会丢弃此类数据包（安全原因，防止广播风暴）。
       - 需路由器显式启用 **Directed Broadcast Forwarding**（如 Cisco 的 `ip directed-broadcast` 命令）。
     - **发送方在目标子网时**：
       - 行为等同于受限广播，仅限本地子网。

将多播的代码稍加修改即可得到广播通信的代码：

#### sender

```cpp
// Sender just need to create UDP socket and send data to multicast group :)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 128
void error_handling(const char* message);

int main(int argc, char **argv)
{
    // check command line argument 
    if(argc != 3) {
        printf("Usage : %s <Broadcast IP> <Port> \n", argv[0]);
        exit(1);
    }

    // create send socket
    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);

    // set boardcast 
    struct sockaddr_in broad_addr;
    memset(&broad_addr, 0, sizeof(broad_addr));
    broad_addr.sin_family = AF_INET;
    broad_addr.sin_port = htons(atoi(argv[2]));
    broad_addr.sin_addr.s_addr = inet_addr(argv[1]);
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)&broad_addr, sizeof(broad_addr));
  
    // open file and send data 
    FILE *fp;
    char buf[BUF_SIZE];

    if((fp = fopen("data.txt", "r")) == NULL)
        error_handling("fopen() error");

    while(fgets(buf, BUF_SIZE, fp) != NULL) {
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&broad_addr, sizeof(broad_addr));
        sleep(1);
    }

    printf("send data done\n");

    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}
```

#### receiver

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(const char* message);

int main(int argc, char **argv)
{
    // check command line argument 
    if(argc != 2) {
        printf("Usage : %s <Port> n", argv[0]);
        exit(1);
    }

    // create receive socket and bind address
    int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[1]));
    if(bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        error_handling("bind() error");

    // receive multicast data 
    char buf[BUF_SIZE];
    int line = 0;
    while(1)
    {
        ssize_t read_len = recvfrom(recv_sock, buf, BUF_SIZE- 1, 0, NULL, 0);
        if(read_len < 0)
            break;
        buf[read_len] = 0;
        printf("[%d] %s", ++ line, buf);
    }

    close(recv_sock);
    return 0;
}

void error_handling(const char* message)
{
    perror(message);
    exit(1);
}
```

## 优雅关闭

#### 客户端

```cpp
// client.c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message) {
    perror(message);
    exit(1);
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 1. 创建客户端套接字
    int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock == -1) {
        error_handling("socket() error");
    }

    // 2. 连接服务器
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    // 3. 接受数据
    FILE *file = fopen("./receive.txt", "wb");
    if(file == NULL) {
        // 此时文件打开失败，不需要关闭文件
        error_handling("fopen() error");
    }
    // 确保调用 fileno(file) 时 file!=NULL
    int fd = fileno(file);
  
    char buf[BUF_SIZE] = {0};
    ssize_t read_cnt;
    while((read_cnt = read(clnt_sock, buf, BUF_SIZE)) > 0) {
        if(write(fd, buf, read_cnt) != read_cnt) {
            fclose(file); // 注意要关闭文件
            error_handling("write() error");
        }
    }
    if(read_cnt == -1) {
        fclose(file); // 注意要关闭文件
        error_handling("read() error");
    }
    puts("File received successfully!");
  
    // 4. 发送最终消息
    char message[] = "Thank you, bye!";
    if(write(clnt_sock, message, sizeof(message)) == -1) {
        fclose(file); // 注意要关闭文件
        error_handling("write() close message error");
    }
  
    // 5. 关闭资源
    fclose(file);
    close(clnt_sock);

    return EXIT_SUCCESS;
}
```

#### 服务器

```cpp
// server.c
#include <asm-generic/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handling(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage : %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
  
    // 1. 创建服务器 socket
    int serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) {
        error_handling("socket() errror");
    }

    // 2. 绑定地址
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    // 端口复用
    int optval = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // 3. 打开监听
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    // 4. 接受客户端
    int clnt_sock;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_sz = sizeof(clnt_addr);
    if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_sz)) == -1) {
        error_handling("accept() error");
    }
    printf("client connected: %s:%d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    // 5. 发送数据
    FILE* file = fopen("./data.txt", "rb");
    if(file == NULL) {
        error_handling("fopen() error");
    }
    // 注意这里要先检查 file 是否为空再初始化 fd
    // 这是因为 fileno(NULL) 是未定义行为
    int fd = fileno(file);

    char buf[BUF_SIZE] = {0};
    ssize_t read_cnt;
    while((read_cnt = read(fd, buf, BUF_SIZE)) > 0)
    {
        if(write(clnt_sock, buf, read_cnt) != read_cnt) {
            fclose(file); // 注意要关闭文件
            error_handling("write() error");
        }
    }
    if(read_cnt == -1) {
        fclose(file); // 注意要关闭文件
        error_handling("read() error");
    }

    // 6. 半关闭
    shutdown(clnt_sock, SHUT_WR);

    // 7. 接受客户端最终消息
    memset(buf, 0, BUF_SIZE);
    // 预留一个字节给 \0
    ssize_t final_read_cnt = read(clnt_sock, buf, BUF_SIZE - 1);
    buf[final_read_cnt] = '\0'; // 确保字符串以空字符结尾
    if(final_read_cnt == -1) {
        fclose(file); // 注意要关闭文件
        error_handling("read() error");
    }
    else if(final_read_cnt == 0) {
        printf("Client closed connection after receiving file.\n");
    }
    else {
        printf("Message from client: %s\n", buf);
    }

    // 8. 清理资源
    fclose(file);
    close(serv_sock);
    close(clnt_sock);

    return EXIT_SUCCESS;
}
```

注意在服务器中，我们是关闭客户端的 `SHUT_WR`，而不是关闭服务器 `serv_sock` 的 `SHUT_WR`，这是一个容易被忽视的问题。首先，`serv_sock` 是监听套接字，它只负责接受新的客户端连接，**不参与数据传输**，而 `clnt_sock` 是客户端连接套接字，服务器通过它与客户端通信（发送/接受数据），所以我们这里关闭的是 `clnt_sock`。

而这里服务器发送完数据之后，服务器需要高速客户端，服务器已经发送完数据了，通过调用 `shutdown(clnt_sock, SHUT_WR)`，表示不会再发送更多数据，此时客户端的 `read()` 会返回 0，知道服务器已经结束了发送。但要注意此时服务器仍然可以 `read(clnt_sock)`，等待客户端可能的相应（比如确认消息）。

如果不只用半关闭：

- 客户端不知道服务器是否还有数据要发送，可能会一直等待。
- 如果直接 `close(clnt_sock)`，客户端可能收到 `ECONNRESET` 错误（连接被重置），而不是正常的 EOF (`read()` 返回 `0`)。
