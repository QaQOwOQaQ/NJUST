# C++ Prime

## 一、IO库

### 1. 宽字符

一般来说，我们使用的 `IO` 类型和对象都是操纵 `char` 字符的，不过为了支持宽字符的语言，标准库定义了一组*类型*和*对象*来操纵 `wchar_t` 类型的数据。宽字符版本的类型和函数以 `w` 开头，例如 `wcin`、`wcout` 和 `wcerr` 分别对应 `cin`、`cout` 和` cerr` 的宽字符版本。`wchar_t` 版本的类型和对象与其对应的普通 `char` 类型的版本定义在同一个头文件中，例如头文件 `fstream` 定义了 `ifstream` 和 `wifstream` 类型。

### 2. IO 类的继承关系

``` C++
iostream: 基本IO（控制台IO），基本流
    -istream, wistream
    -ostream, wostream
    -iostream, wiostream

fstream: 文件IO，文件流
    -ifstream, wifstream
    -ofstream, wostream
    -fstream, wfstream

sstream: 内存IO(通过string实现)，string流
    -istringstream, wistringstream
    -ostringstream, wostringstream
    -stringstream, wstringstream
```

从概念上，设备类型（文件/终端/内存中的字符串）和字符大小（`char`/`wchar_t`）都不会影响我们要执行的 IO 操作。例如我们可以通过 ``>>`` 读取数据，而不管是从一个控制台窗口还是从一个磁盘文件读取，也不必关心读取的字符是要存入一个 `char` 对象内还是 `wchar_t` 对象内。

标准库之所以能使我们忽略这些差异是通过**继承机制**实现的，通过继承机制，我们可以将一个派生类对象当作基类对象使用。它们的继承关系如下图所示：

![IO class](https://images0.cnblogs.com/blog/378920/201405/082149512609442.png)

### 3. IO 对象是 noncopable 类型

我们不能拷贝 IO 对象或者为其赋值。这是因为 I/O 对象（如 `fstream`、`ostream`）涉及 **底层资源（文件句柄、流缓冲区等）**，这些资源不能简单地进行拷贝，否则会导致资源管理问题：

1. **拷贝可能导致资源冲突**
   - 如果复制 `fstream`，那么多个对象可能会同时访问同一个文件，导致 **数据混乱** 或 **文件指针错乱**。
2. **文件流对象通常是独占资源**
   - 一个 `fstream` 对象通常与 **一个特定的文件描述符（file descriptor）** 绑定，拷贝它意味着多个对象共享同一个文件描述符，这可能导致 **数据错乱**。
3. **标准输入输出流也不能被拷贝**
   - 例如，`std::cin`、`std::cout` 这些标准流是<font color=blue>**全局唯一**</font>的，它们**不能被复制**，否则程序可能会发生 **未定义行为**。

虽然 I/O 对象 **不能拷贝**，但**可以移动（move）**。C++11 引入了 **移动语义**，可以使用 `std::move()` 转移 I/O 对象的所有权：

```cpp
#include <iostream>
#include <fstream>

int main() {
    std::ifstream file1("example.txt"); // 创建一个文件输入流

    std::ifstream file2 = std::move(file1); // ✅ 允许，转移所有权

    if (!file1) {
        std::cout << "file1 现在已经被移动，无法使用" << std::endl;
    }

    return 0;
}
```

### 4. **如何正确传递 I/O 对象**

#### 方法 1：使用引用

如果想要在函数中使用 I/O 流对象，应该使用 **引用（reference）** 传递：

```cpp
#include <iostream>
#include <fstream>

void readFile(std::ifstream& file) {  // 传递引用
    std::string line;
    while (getline(file, line)) {
        std::cout << line << std::endl;
    }
}

int main() {
    std::ifstream file("example.txt");
    if (!file) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }
    readFile(file); // 传递引用
    return 0;
}
```

这样 `readFile()` **不会复制 `std::ifstream` 对象**，而是直接操作它。

#### 方法 2：使用智能指针

如果 I/O 对象的 **生命周期需要动态管理**，可以使用 `std::unique_ptr`：

```cpp
#include <iostream>
#include <fstream>
#include <memory>

std::unique_ptr<std::ifstream> openFile(const std::string& filename) {
    auto file = std::make_unique<std::ifstream>(filename);
    if (!file->is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return nullptr;
    }
    return file;
}

int main() {
    auto file = openFile("example.txt");
    if (file) {
        std::string line;
        while (getline(*file, line)) {
            std::cout << line << std::endl;
        }
    }
    return 0;
}
```

**优点：**

- `std::unique_ptr<std::ifstream>` **保证了文件流的唯一所有权**，不会发生错误拷贝问题。
- **可以动态分配 I/O 对象，并自动管理其生命周期**。

### 5. 条件状态

`IO` 库提供了一些标志以及与标志有关的函数，可以帮助我们访问和操纵**流的条件状态**。在 `<ios/ios_base.h>` 里面有这样一段内容：

``` C++
enum _Ios_Iostate
{ 
    _S_goodbit 		= 0,		// 0
    _S_badbit 		= 1L << 0,  // 1
    _S_eofbit 		= 1L << 1,  // 2
    _S_failbit		= 1L << 2,  // 4
    _S_ios_iostate_end = 1L << 16,
    _S_ios_iostate_max = __INT_MAX__,
    _S_ios_iostate_min = ~__INT_MAX__
};

typedef _Ios_Iostate iostate;

/// Indicates a loss of integrity in an input or output sequence (such as an irrecoverable read error from a file).
static const iostate badbit = _S_badbit;

/// Indicates that an input operation reached the end of an input sequence.
static const iostate eofbit = _S_eofbit;

/// Indicates that an input operation failed to read the expected characters, or that an output operation failed to generate the desired characters.
static const iostate failbit = _S_failbit;

/// Indicates all is well.
static const iostate goodbit = _S_goodbit;
```

在 C++ 中，输入输出流的状态就是通过 `ios_bast.h` 里定义的 4 个枚举量管理的：

* `strm::goodbit`：流未处于错误状态且没有到达文件结束位置，此值保证为 0
* `strm::badbit`：不可恢复错误，通常情况下流不可继续使用
* `strm::eofbit`：流到达了文件结束位置，此时`eofbit` 和 `failbit` 都为置位
* `strm::failbit`：可恢复错误，流还可以继续使用

> 注意处于 `eof` 时，`goodbit`  为 `false`。

另外，`strm::iostate` 是上面 4 中状态的集合，相当于：`iostate = goodbit | badbit | eodbit | failbit`。以及一些操作这些标志的函数：

``` C++
// 非标准库实现，但含义是一样的
bool eof()     {  return eofbit; }
bool bad()     {  return badbit; }
bool fail()    {  return failbit | badbit; }
bool good()    {  return !failbit && !eofbit; } // !eofbit
void clear()   { iostate = goodbit;} 
void setstate(iostate state = goodbit) { iostate = state; }
iostate rdstate() const                { return iostate; }
```

对于下面的代码，如果我们输出 `CTRL+D`（文件结束符），那么程序输出为：

``` C++
int x;
cin >> x;
auto cur_state = cin.rdstate();
cout << cur_state << endl;	// 6
cin.clear(cur_state & ~cin.eofbit & ~cin.failbit);
cout << cin.rdstate() << endl; // 0
```

编写一个函数，接受一个 `istream&` 对象，返回值也是 `istream&`。此函数需从流中读取数据并输出，直至遇到 `EOF` 停止。完成这些操作后，在返回流之前，对流进行复位：

``` c++
constexpr auto max_size = numeric_limits<streamsize>::max();

template<typename T>
istream& read(istream& is) 
{
    T x;
    while(cin >> x, !is.eof()) 
    {
        if(is.bad()) 
        {
            throw(runtime_error("IO stream error"));
        }
        if(is.fail())
        {
            cerr << "Data error, pls retry: " << endl;
            is.clear();
            // 在流中忽略最多max_size个字符直到遇到'\n'
            // 如果max_size==streamsize的最大值
            // 那么该数将被忽略，即可以忽略无限个字符直到遇到'\n'
            is.ignore(max_size, '\n');
            continue;
        }
        cout << "data: " << x << endl;
    }
    is.clear();
    return is;
}

int main() 
{
    read<int>(cin);
    return 0;
}
```

### 6. *ignore*

`std::istream::ignore()` 是 C++ 标准库中的输入流方法，属于 `std::istream` 类（如 `std::cin`、`std::ifstream` 等）。它用于**跳过输入流中的字符**，常用于丢弃无关数据、清除缓冲区、修复输入错误等场景。

#### **6.1 函数原型**

```cpp
std::istream& ignore (std::streamsize n = 1, int delim = EOF);
```

**参数说明：**

- `n`（默认值：`1`）：**最多跳过 `n` 个字符**。
- `delim`（默认值：`EOF`）：**遇到 `delim`（如 `'\n'`）时提前停止**。

**返回值：**

- 返回当前 `std::istream` 流对象的引用，支持链式调用。

#### **6.2 `ignore()` 的主要用途**

##### **2.1 跳过一个字符（通常是换行符 `\n`）**

```cpp
#include <iostream>

int main() {
    char ch;
    std::cout << "输入一个字符: ";
    std::cin.get(ch); // 读取一个字符
    std::cout << "你输入的是: " << ch << std::endl;

    std::cin.ignore(); // 跳过一个字符（默认是 1 个）
    
    std::cout << "请输入一个字符串: ";
    std::string str;
    std::getline(std::cin, str); // 读取整行
    std::cout << "你输入的字符串是: " << str << std::endl;

    return 0;
}
```

**🔹 作用：**

- `std::cin.get(ch);` 读取一个字符，但**换行符 `\n` 仍然留在输入缓冲区**。
- `std::cin.ignore();` 忽略这个 `\n`，否则 `std::getline()` 可能直接读到空行。

##### **2.2 跳过多个字符**

```cpp
#include <iostream>

int main() {
    std::cout << "输入一些文本（最多跳过 5 个字符）: ";
    std::cin.ignore(5); // 跳过最多 5 个字符

    std::string input;
    std::getline(std::cin, input);
    std::cout << "剩余的输入是: " << input << std::endl;

    return 0;
}
```

**🔹 作用：**

- `std::cin.ignore(5);` **跳过前 5 个字符**，然后再读取输入。

##### **2.3 跳过直到遇到指定字符**

```cpp
#include <iostream>

int main() {
    std::cout << "输入一些文本（跳过直到 `,`）: ";
    std::cin.ignore(100, ','); // 跳过最多 100 个字符，直到遇到 `,`

    std::string input;
    std::getline(std::cin, input);
    std::cout << "剩余的输入是: " << input << std::endl;

    return 0;
}
```

**🔹 作用：**

- `std::cin.ignore(100, ',');` **跳过前 100 个字符，或者遇到 `,` 就停止**。
- 适用于解析 CSV 数据或格式化输入。

##### **2.4 清除整个输入缓冲区**

如果你不确定输入有多少，可以用：

```cpp
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
```

完整示例：

```cpp
#include <iostream>
#include <limits>

int main() {
    std::cout << "请输入一个整数: ";
    int num;
    while (!(std::cin >> num)) {
        std::cout << "输入错误，请输入整数: ";
        std::cin.clear(); // 清除错误状态
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 清空错误输入
    }

    std::cout << "你输入的整数是: " << num << std::endl;
    return 0;
}
```

**🔹 作用：**

- `std::cin.clear();` 清除 `cin` 的错误状态（如输入 `abc` 导致 `cin` 失败）。
- `std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');` **清除输入缓冲区的所有内容**，直到遇到 `\n`。

#### 6.3 ignore 时机

``` c++
string s;
cin.ignore(100, ',');
cin >> s;
cout << s << endl;
```

注意我们是先执行 `cin.ignore`，才执行 `cin.operator>>`。这是因为对于我们输入设备上的数据，它已经在缓冲区了，`cin.operator>>` 是在缓冲区（内存）中读取数据，而不是直接从输入设备上读取数据。而 `cin.ignore` 就是忽略我们指定的缓冲区数据。

### 7. 管理输出缓冲

导致缓冲刷新（即，数据从内存中的缓冲区真正写到输出设备或文件）的原因有很多：

* 程序正常结束

* 缓冲区满

* 显式刷新缓冲区：例如使用操作符 `endl`

* 一个输出流可能被关联到另一个流：在这种情况下，当读写被关联的流时，关联到的流的缓冲区也会被刷新。例如默认情况下 `cin` 和 `cerr` 都关联到 `cout`，因此读 `cin` 或写 `cerr` 都会导致 `cout` 的缓冲区被刷新。（这也很显然，因为它们是共用缓冲区的，如果不刷新的话，那么一个缓冲区就保存了不同流的数据，显然不合理）

* 在输出操作之后，可以用操纵符 `unitbutf` 设置流的内部状态，来清空缓冲区。默认情况下，对 `cerr` 时设置 `unitbuf` 的，因此 `cerr` 的内容会立即刷新。

对当前输出刷新缓冲区：

* endl：换行并刷新缓冲区
* ends：输出空字符并刷新缓冲区（注意空字符(`\0`)不是空格）
* flush：刷新缓冲区

> 在 C/C++ 中，**插入空字符（`'\0'`）** 的主要用途是 **标记字符串的结束**，因为 C 风格字符串（以 `char[]` 或 `char*` 存储）依赖 `'\0'` 来确定字符串的长度。`std::ends` 的主要应用场景是 **字符串流（如 `std::ostringstream`）**，而不是普通的输出流（如 `std::cout`）。

对每次输出都刷新缓冲区：

* unitbuf：放在输出的开头，在他之后的输出都会刷新缓冲区
* nounitbuf：回到正常的缓冲方式

通过组合 unitbuf 和 nounitbuf 可以实现 flush 的效果，下面两种写法是等价的：

``` c++
cout << "4" << flush;
cout << unitbuf << "4" << nounitbuf;
```

**如果从程序崩溃或异常终止，输出缓冲区不会被刷新。因此在使用输出语句调式代码时，确保每次都刷新缓冲区，从而得到正确的 debug 信息。**

----

关联输入和输出流：当一个输入流被关联到一个输出流时，任何试图从输入流读取数据的操作都会先刷新输出流。

**交互式系统通常应该关联输入流和输出流。**这意味着所有输出，包括用户提示信息，都会在读操作之前被打印出来。我们可以通过 `tie()` 修改流的关联关系和查看当前输入流绑定的输出流。

### 7. 文件流

当我们使用文件 IO 时，和普通 IO 并无什么区别，我们依然是通过 `>>`，`<<`，或者 `getline` 等函数进行输入输出，只不过输入输出的设备发生了变化。

`fstream` 特有的操作：（`fstream` 表示文件 IO 类型）

``` c++
fstream fstrm;    // 创建一个未绑定的文件流

fstream fstrm(s); // 创建一个文件流并打开文件s
				  // mode依赖于fstream的类型
				  // 此时open自动被调用
fstream fstrm(s, mode);

fstrm.open(s);	  // 如果定义了一个未绑定的文件流
				  // 可以调用open来将它与文件绑定

fstrm.close();    // 当一个对象被销毁时，close会被自动调用
fstrm.is_open();
```

> 注意 `open()` 可能失败，此时状态标志 `failbit` 会被置位。当我们打开文件时，检查文件是否成功被打开是一个好习惯。
>
> 对一个已经绑定文件的流执行 `open()` 会失败，并且 `failbit` 会置位。随后的试图使用该文件流的操作都会失败。如果我们想换绑文件，需要先 `close()` 再重新 `open()`

----

文件模式：

``` c++
in      // 读
out     // 写，默认会清空文件
app		// 追加
ate		// 打开文件后定位到文件末尾
trunc   // 清空文件内容（如果文件存在）
binary  // 二进制模式
```

> ⚠️ **注意**：
>
> - `std::ios::out` **默认会启用 `std::ios::trunc`**（即清空文件），除非同时指定 `std::ios::app`。
> - `std::ios::binary` 用于避免 `\n` 被转换为 `\r\n`（Windows 下默认会转换）。

文件模式有一些限制：

* 设置 `app` 时无需设置 `out`
* 必须指定了 `out` 才能 `trunc`
* `trunc` 和 `app` 是互斥的
* `ate` 和 `binary` 模式可以用于任何类型的文件流对象，并且可以与其它任何文件模式组合使用
* 当我们以 `out` 模式打开文件时，即使指定 `ate` 也会截断文件

每个文件流类型都定义了默认的文件模式：

* `ifstream`: `in`
* `ofstream`: `out`
* `fstream`: `in` |  `out`

`trunc`：

- 如果文件 **已存在**，`trunc` 会**清空文件内容**（相当于删除后重新创建）。
- 如果文件 **不存在**，会**创建新文件**（和普通写入模式一样）。
- **默认情况下**，`std::ofstream` 会隐式启用 `trunc`，除非指定了 `std::ios::app`（追加模式）。

### *8. close*

在 C++ 中，IO 对象（如 `std::ifstream`、`std::ofstream`、`std::fstream`）的析构函数会自动调用 `close()`，因此 **在大多数情况下不需要显式调用 `close()`**。但某些特定场景下，**显式调用 `close()` 仍然是有必要的**。

#### **8.1 何时不需要显式调用 `close()`？**

##### **（1）正常作用域结束时自动关闭**

如果文件流对象在离开作用域时被销毁，`close()` 会自动调用，无需手动干预：

```cpp
#include <fstream>

void readFile() {
    std::ifstream file("example.txt"); // 打开文件
    // 读取文件...
} // 离开作用域，file 析构，自动调用 close()
```

✅ **推荐做法**：依赖 RAII（资源获取即初始化），让析构函数自动管理资源。

#### **8.2 何时需要显式调用 `close()`？**

尽管析构函数会调用 `close()`，但在以下情况下，**显式调用 `close()` 更合适**：

##### **（1）需要立即释放文件资源**

如果希望 **尽早释放文件句柄**（而不是等待析构时），可以手动 `close()`：

```cpp
std::ofstream file("output.txt");
file << "Hello, World!"; // 写入数据
file.close(); // 显式关闭，立即释放文件锁

// 其他操作（如重命名、删除文件）
std::remove("output.txt"); // 如果没 close()，可能导致操作失败
```

🚀 **适用场景**：

- 需要 **立即让其他进程访问该文件**（如日志文件轮转）。
- 在长时间运行的函数中，尽早释放资源。

##### **（2）检查文件是否成功关闭**

`close()` 会刷新缓冲区并检查错误，如果写入失败（如磁盘已满），可以捕获错误：

```cpp
std::ofstream file("data.bin");
file.write(data, size);

if (!file.close()) { // 显式调用 close() 并检查是否成功
    std::cerr << "写入文件失败！" << std::endl;
}
```

📌 **注意**：

- <font color=blue>析构函数 **不会抛出异常**（即使 `close()` 失败），所以如果需要对错误进行处理，必须显式调用 `close()`。</font>

##### **（3）重复使用同一个文件流对象**

如果想 **重用文件流对象**（避免重复构造），必须先 `close()` 再重新打开：

```cpp
std::fstream file;

file.open("first.txt");
// 操作文件...
file.close(); // 必须先关闭

file.open("second.txt"); // 重新打开另一个文件
// 继续操作...
```

### 9. 内存流（*stringstream*）

在 C++ 中，内存 `IO` 以 `string` 作为 `IO` 对象，因此一般称内存流为 `string` 流。

`sstringstream` 特有的操作：（`sstream` 表示内存 `IO` 类型）

``` c++
sstream strm;	 // 定义一个未绑定的stringstream对象
sstream strm(s); // 定义一个绑定到字符串s的strignstream对象

strm.str();		 // 返回strm保存的string的拷贝
strm.str(s);	 // 更换strm的绑定为s
```

## 二、顺序容器

### 1. 顺序容器概述

顺序容器指的是**元素在内存中的位置与元素加入容器的顺序有关**。顺序容器都提供了**快速顺序访问元素**的能力。但是，容器在以下方面都有不同的性能折中：

* 向容器添加或从容器中删除元素的代价
* 随机访问容器中元素的代价

``` c++
array
vector
string
list
forward_list
deque // 容易忽略，它是顺序容器并且支持随机访问
```

> 注意顺序容器的**顺序**并不是说元素在**内存**上是连续分布的，而是元素在<font color=blue>**逻辑**</font>上是**有序**的，这里的**有序**指的是，元素在**逻辑**上的**顺序**与元素加入容器的**顺序**相同。
>
> 注意，顺序容器只是提供了快速<font color=blue>**顺序访问**</font>元素的能力，而不是快速**随机访问**元素的能力。

* `list` 和 `forward_list` 在内部需要为每个元素额外保存两个或一个指针，因此它们的额外内存开销比较大。指针的使用也体现了顺序容器中的元素在内存中未必连续。
* `forward_list` 的设计目标是达到与最好的手写的单向链表数据结构相当的性能。因此 `forward_list` 没有 `size` 操作，因为保存或计算其大小就会比手写链表多出额外的开销。对其他容器而言，`size` 保证是一个快速的常量时间的操作。
* 由于 `array` 的大小固定，因为 `array` 的大小也是类型的一部分：`std::array<Type,Size> arr;`

当我们在输入阶段需要在容器中间位置插入元素，输入完成后又需要随机访问元素，该如何选择容器呢？

* 如果我们在中间位置插入元素是为了确保数据的有序性，那么可以追加到 `vector` 末尾再排序
* 如果必须在中间位置插入元素，可以先用 `list` 保存元素，最后再将 `list` 中的内容拷贝到 `vector`

### 2. 容器 API 概览

所有容器都适用的操作：

``` c++
// 类型别名
iterator;
const_iterator;
size_type;       // 无符号整数，足够保存此容器类型最大可能容器的大小
difference_type; // 带符号整数，足够保存两个迭代器之间的距离
value_type;
reference;       // value_type&
const_reference; // const value_type&

// 构造函数
C c;
C c1(c2);
C c(b, e);    	// array不支持
C c{a, b, c, ...};

// 赋值与swap
c1 = c2;
c1 = {a, b, c, ...};
a.swap(b);
swap(a, b);

// 大小 
c.size();     // forward_list不支持
c.max_size(); // c可保存的最大元素数目
c.empty();

// 添加或删除元素（array除外）
c.insert(args);
c.emplace(inits); // 直接使用构造函数在c中构造一个元素
c.erase(args);
c.clear();

// 关系运算符
==, !=;
<, <=, >, >=;

// 获取迭代器
c.begin(), c.end();
c.cbegin(), c.cend();

// 反向容器的额外成员
reverse_iterator;
const_reverse_iterator;
c.rbegin(), c.rend();
c.crbegin(), c.crend();
```

### 3. *array*

#### 3.1 无法使用迭代器构造array

在 C++ 中，`C c(b, e);` 这种构造方式通常用于使用迭代器范围来构造容器（比如 `std::vector`、`std::list` 等），其中 `b` 是起始迭代器，`e` 是结束迭代器。然而，`std::array` 是 C++ 中的一个**固定大小**的数组容器，它**不支持用迭代器范围构造**，因为 `std::array` 的大小在编译时就确定了，不能在运行时动态调整。而迭代器范围 `[b, e)` 的长度在编译时是未知的，因此无法在构造时确定 `std::array` 的大小。

#### 3.2 array存储位置

在 C++ 中，`std::array` 的数据存储位置取决于它的声明方式，但它的存储是 **完全嵌入在对象本身** 的，不涉及动态内存分配（即不存储在堆上）。具体来说：

##### （1） **如果 `std::array` 是局部变量（在栈上声明）**

```cpp
void foo() {
    std::array<int, 5> arr = {1, 2, 3, 4, 5};  // 数据存储在栈上
}
```

- **存储位置**：`arr` 的所有数据（5 个 `int`）都存储在 **栈（stack）** 上。
- **特点**：
  - 内存自动管理（函数结束时自动释放）。
  - 访问速度快（栈内存通常有更好的缓存局部性）。
  - 大小固定，无法调整。

##### （2）**如果 `std::array` 是全局/静态变量**

```cpp
std::array<int, 5> global_arr = {1, 2, 3, 4, 5};  // 数据存储在全局/静态存储区

void bar() {
    static std::array<int, 5> static_arr = {1, 2, 3, 4, 5};  // 数据存储在静态存储区
}
```

- **存储位置**：
  - `global_arr` 存储在 **全局/静态存储区**（类似于 `static` 变量）。
  - `static_arr` 也存储在 **静态存储区**（即使它在函数内部声明）。
- **特点**：
  - 生命周期直到程序结束。
  - 不涉及堆分配。

##### （3）**如果 `std::array` 是类的成员变量**

```cpp
class MyClass {
    std::array<int, 5> member_arr = {1, 2, 3, 4, 5};  // 存储位置取决于对象的存储位置
};
```

- **存储位置**：
  - 如果 `MyClass` 的对象在栈上，`member_arr` 也在栈上。
  - 如果 `MyClass` 的对象在堆上（例如 `new MyClass`），`member_arr` 也在堆上。
- **特点**：
  - `std::array` 的数据始终跟随它的对象。

##### **（4） `std::array` 不会存储在堆（heap）上（除非它是堆对象的成员）**

- `std::array` **本身不进行动态内存分配**，所以它的数据不会单独存储在堆上。
- 但如果 `std::array` 是某个堆对象的一部分（例如 `new` 出来的类成员），那么它的数据也会在堆上。

### 4. 迭代器

[distance, traits](https://ivanzz1001.github.io/records/post/cplusplus/2018/03/14/cpluscplus_stl_iterator#34-%E5%88%A9%E7%94%A8%E8%BF%AD%E4%BB%A3%E5%99%A8%E7%A7%8D%E7%B1%BB%E6%9B%B4%E6%9C%89%E6%95%88%E7%9A%84%E5%AE%9E%E7%8E%B0distance%E5%87%BD%E6%95%B0)

#### 4.1 范围迭代器

我们对构成范围的迭代器 `(begin，end]`有以下要求（保证）：

1. **同一个容器**：`begin` 和 `end` 必须指向同一个容器中的元素，或者 `end` 可以是该容器“末尾之后”的位置（即 `container.end()`）。这意味着不能将两个不同容器的迭代器组合成一个范围。
2. **有效性**：`begin` 可以通过不断递增（`++begin`）最终到达 `end`。这意味着 `end` 不能在 `begin` 之前，即不能出现 `begin` 已经“越过” `end` 的情况。

#### 4.2 遍历迭代器

在 C++ 中，遍历迭代器范围 `[begin, end)` 时，通常使用 `!=` 或 `==` 来判断是否到达 `end`，而不是 `<` 或 `<=`。原因如下：

1. **迭代器类别支持**：
   - **所有迭代器**都支持 `==` 和 `!=`（因为它们是 EqualityComparable 的）。
   - 但只有**随机访问迭代器**（如 `vector`、`deque`、`array` 的迭代器）支持 `<`、`<=`、`>`、`>=`。
   - 其他迭代器（如 `list`、`forward_list`、`set`、`map` 的迭代器）不支持关系运算符。
2. **通用性**：
   - 使用 `!=` 或 `==` 可以保证代码在所有迭代器类型上都能工作（输入、前向、双向、随机访问迭代器）。
   - 如果使用 `<`，代码只能在随机访问迭代器上运行，限制了容器的选择。
3. **性能**：
   - 对于随机访问迭代器，`it != end` 和 `it < end` 的性能几乎相同。
   - 但对于非随机访问迭代器（如链表），`it < end` 无法高效实现（需要线性时间计算距离），而 `it != end` 是 `O(1)`。
4. **标准库约定**：
   - C++ 标准库的所有算法（如 `std::for_each`、`std::find`）都使用 `!=` 判断终止条件，而不是 `<`，以保证通用性。

#### 4.3 反向迭代器与base()

反向迭代器（`reverse_iterator`）是 C++ 标准库提供的一种适配器，它允许我们以**逆序**方式遍历容器。它的设计使得我们可以用相同的 `begin()` 和 `end()` 模型来处理反向遍历，但它的操作语义与普通迭代器相反：

| 操作         | 普通迭代器 (`iterator`) | 反向迭代器 (`reverse_iterator`) |
| :----------- | :---------------------- | :------------------------------ |
| `++it`       | 移动到**下一个**元素    | 移动到**上一个**元素            |
| `--it`       | 移动到**上一个**元素    | 移动到**下一个**元素            |
| `it->member` | 访问当前元素的成员      | 访问当前元素的成员              |
| `*it`        | 解引用当前元素          | 解引用当前元素                  |

##### 反向迭代器的关键点

1. **`rbegin()` 和 `rend()` 的物理位置**：
   - `rbegin()` 指向容器的**最后一个元素**（即 `end() - 1`）。
   - `rend()` 指向容器的**第一个元素之前**（即 `begin() - 1`）。
   - <font color=blue>因此，`[rbegin(), rend())` 是一个左闭右开区间，但逻辑上是反向的。</font>
2. **解引用 (`*`) 的语义**：
   - `*rbegin()` 返回最后一个元素。
   - `*rend()` 是未定义的（因为 `rend()` 是一个“哨兵”位置，类似于 `end()`）。
3. **`base()` 方法**：
   - 反向迭代器可以通过 `base()` 转换为普通迭代器。
   - `rit.base()` 返回 `rit` 对应的普通迭代器，但位置会偏移：
     - <font color=blue>`rit.base()` 指向 `*rit` 的**下一个位置**。</font>
     - 例如，`rbegin().base() == end()`，`rend().base() == begin()`。

一般来说，如果我们使用反向迭代器时需要调用库函数，往往会把方向迭代器通过 `base()` 转换为普通迭代器再作为参数调用库函数。 

#### 4.4 迭代器称呼

作为一个合格的 cpper，绝对不能将 `end` 称为尾迭代器：

* `begin`：首迭代器
* `end`：尾后迭代器
* `rbegin`：尾迭代器
* `rend`：首前迭代器

> 有时我们会将尾后迭代器 `end` 称为 `last`，不过 `last` 具有一定的歧义，因为 `end` 所指向元素并不是容器的最后一个(last)元素。

#### 4.5 迭代器重载

不以 `c` 开头的函数都是被重载过的。这就是说，实际上有两个名为 `begin` 的成员。一个是非常量成员，返回 `iterator`；一个是常量成员，返回 `const_iterator`。因此说当我们使用 `begin` 时，它并不总是返回 `iterator`，还有可能返回 `const_iterator`。

``` C++
iterator begin() noexcept;
const_iterator begin() const noexcept;

const_iterator cbegin() const noexcept; // C++11引入，永远返回常量迭代器
```

为什么需要两个 `begin` 重载？

1. **常量正确性（Const Correctness）**：
   - 如果容器是常量（如 `const std::vector<int>`），调用 `begin()` 会自动选择常量版本，返回 `const_iterator`。
   - 如果容器是非常量，调用 `begin()` 会选择非常量版本，返回 `iterator`。
2. **避免代码重复**：
   - 通过重载，可以用相同的函数名 `begin` 处理常量和非常量情况，无需为常量容器单独命名（如 `const_begin`）。

#### 4.6 迭代器失效

迭代器失效是指当容器发生某些修改操作后，原先获取的迭代器不再指向有效的元素或位置，继续使用这些迭代器会导致未定义行为，类似于“悬空指针”。

迭代器失效类型：

- 由于插入元素，使得容器元素整体`迁移`导致存放原容器元素的空间不再有效，从而使得指向原空间的迭代器失效；
- 由于删除元素，使得某些元素`次序`发生变化导致原本指向某元素的迭代器不再指向期望指向的元素。

-----

在向容器添加元素后：

* list，forward_list：不影响
* vector，string：
  * 如果存储空间重新分配，全都指引迭失效
  * 否则，插入位置之后的全部指引迭失效
* deque：
  * 在首尾插入：迭代器失效，指针和引用不会
  * 在中间插入，全部指引迭失效

当我们删除一个元素后，指向被删除元素的指针、引用和迭代器会失效。对于其它指针、引用和迭代器来说：

* list，forward_list：不影响
* vector，string：被删除元素之后的全部指引迭失效
* deque：
  * 在首删除：不受影响
  * 在尾删除：尾后迭代器会失效，其余不受影响
  * 否则，全部指引迭失效

建议：管理迭代器

* 当使用（指针、引用）迭代器时，最小化要求迭代器必须保持有效的程序片段是一个好方法
* 由于向增删元素可能导致迭代器失效，因此必须保证每次改变容器的操作之后都正确的重新定位迭代器
* 不要保存 end 返回的迭代器，因为 end 在绝大部分增删的情况下都会失效。通常 C++ 标准库的实现中 end 操作都很快，部分也是因为我们需要频繁调用 end。

### 5. 容器定义和初始化

``` c++
C c;	// 默认初始化

C c1(c2); // c1和c2必须是相同类型：容器类型相同，保存的元素类型相同,对于array类型，还要求大小相同
C c1=c2;  

C c{a,b,c...};  // 元素类型相容即可，对于array要求元素个数必须<=array的大小，遗漏元素值初始化
C c={a,b,c...}; 

C c(b,e); // 元素类型相容即可

// 只有顺序容器(不包括array)的ctor才能接受大小参数
c seq(n);	// 值初始化
c seq(n,val);
```

对于 array，如果我们没有指定初始值，容器元素会被默认初始化，就像一个内置数组一样：

``` c++
array<int, 10> a; // 默认初始化: 垃圾值
for(int i = 0; i < 10; i ++ )
    cout << a[i] << ' ';    
cout << endl;

vector<int> b(10); // 值初始化
for(int i = 0; i < 10; i ++ )
    cout << b[i] << ' ';
cout << endl;


int c[10]; // 默认初始化: 垃圾值
for(int i = 0; i < 10; i ++ )
    cout << c[i] << ' ';
cout << endl;
```

### 6. *assign*

顺序容器（array 除外）还定义了 assign 成员函数，允许我们从一个不同但相容的类型赋值。

assign 的主要功能是：

* 清除容器当前所有内容
* 用新指定的内容替换原有内容
* 比先 clear() 再插入更高效

``` c++
seq.assign(b,e);
seq.assign(n,val);
seq.assign(initializer_list);
```

注意当我们使用初始值列表进行赋值时，不允许我们 narrowing conversion

``` C++
vector<int> a(10, 1);
//a.assign({1, 3.14}); // error: narrowing conversion
cout << a.size() << ' ' << a.capacity() << endl; // 10 10
a.assign({1,2,3,4});
cout << a.size() << ' ' << a.capacity() << endl; // 4 10
```

### 7. *string*的额外操作

string 的专门函数除了可以接受迭代器之外，还可以接受下标。

#### 7.1 构造函数

``` C++
// n,len2,pos2都是无符号值
// 如果以const char*构造string，相当于substr(0,n)
// 如果以string构造string，相当于substr(pos2,len2)
string s(cp,n);	// s是cp指向的数组中前n个字符的拷贝，此数组应至少包含n个字符，否则行为是UB的

string s(s2,pos2);      // s=s2.substr(pos2), 若pos2>s2.size()，UB

string s(s2,pos2,len2);	// s=s2.substr(pos2,len2), 若pos2>s2.size()，UB	
						// 如果pos2+len2>s2.size，会自动调整到字符串的末尾
```

示例：

``` C++
// 1. 从 const char* 拷贝
const char arr[] = "abc\0def";  

string s1(arr);     // 以 char* 构造，遇到 \0 终止
string s2(arr, 7);  // 明确指定长度 7，会包含 \0

cout << "arr: " << arr << endl; // abc
cout << "s1: " << s1 << endl;   // abc
cout << "s2: " << s2 << endl;   // abcdef

// 2. 从 string 拷贝
string s3("abc\0def", 7);  // 让 s2 显式包含 \0
string s4(s3);

cout << "s4: " << s4 << endl;  // abcdef
```

* 对于从 C 风格字符串 `const char*` 拷贝的情况，遇到空字符时会停止，因为编译器认为此时到达 `const char*` 的终点了。但如果我们显式指定了长度，遇到空字符时会继续拷贝，并且空字符也占用一个字符。
* 对于从 `string` 拷贝的情况，`\0` 只会被认为是一个普通字符。

#### 7.2 数值转换

数值转换时，不需要保证整个 `string` 是一个合法的数值形式，只需要保证 `string` 中的一个**子串**是合法的数值形式。如果我们不显式的指定第一个合法数值子串的位置，这个字串就是 `string` 的前缀。

* 需要保证字符串的第一个非空白字符必须是符号（``+/-``）或数字
* 它可以是 `0x` 或 `0X` 表示的十六进制数，可以包含字母字符，对应大于 `9` 的数
* 若是浮点数，也可以以小数点（`.`）开头，可以包含 `e/E` 表示指数部分

``` C++
to_string(val);	// 一组重载函数
				// val可以是任何算数类型，小整数会自动提升

stoi(s,p,b);  // b表示转换的基数，默认是10进制
stol(s,p,b);  // p是size_t指针，用来保存s中第一个非数值字符的下标，默认为0
stoll(s,p,b);
stoull(s,p,b);

stof(s,p);
stod(s,p);
stold(s,p);
```

#### 7.3 修改

``` c++
s.insert(pos,args); // 在pos之前插入args指定的字符
					// 如果args是下标，返回指向s的引用
					// 如果args是迭代器，返回第一个插入字符的迭代器
s.erase(pos,len);	// 删除从pos开始的len个字符
					// len可以省略，表示一直删除到结尾
					// 返回指向s的引用
s.assign(args);		// 将s替换为args指定的字符
					// 返回一个指向s的引用
s.append(args);		// 返回指向s的引用
s.replace(range,args);	// 删除s中range范围内的字符
						// 替换为args指定的字符
						// 返回指向s的引用
	// args可以是下列形式之一：
str;	// 字符串str
str,pos,len;	// str.substr(pos,len);
cp,len; // char*的前len个字符
cp;		// char*
n,c;	// n个字符c
b,e;	// [b,e)
il;		// initializer_list
```

#### 7.4 查询

``` C++
s.find(args);          	   // 在s中查找第一个出现的args
s.rfind(args);			   // 在s中查找最后一个出现的args

s.find_first_of(args);     // 在s中查找第一个在{args}中的字符
s.find_last_of(args);	   // 在s中查找最后一个在{args}中的字符
s.find_first_not_of(args); // 在s中查找第一个不在{args}中的字符
s.find_last_not_of(args);  // 在s中查找最后一个不在{args}中的字符

	// args 可以是下列形式之一：
c,pos;		// pos默认为0，字符c
s2,pos;		// pos默认为0，字符串s2
cp,pos;		// pos默认为0，字符数组c
cp,pos,n;	// pos无默认值
```

* 如果查询成功，返回字符所在下标，`string::size_type` 类型值（无符号）。
* 否则，返回名为 `string::npos` 的 `static` 成员对象。标准库将 `npos` 定义为一个 `const string::size_type` 类型，并初始化为 `-1`，即 `size_type` 类型的最大可能值。

#### 7.5 子串

``` C++
s.substr(pos,n);  
```

* 如果开始位置 `pos` 超过了 `s` 的最后一个元素的下标，抛出 `out_of_range` 异常。
* 如果 `pos+n` 超过 `s` 最后一个元素的下标，`substr` 会调整长度 `n`，只拷贝到 `s` 的末尾。如果不指定该参数，会拷贝到 `s` 的末尾。

### 8. *swap*

#### 8.1 常数时间

swap 用于交换两个相同类型容器的内容。除了 array 外，交换两个容器内容的操作保证会很快 —— 元素本身并未交换，swap 只是交换了两个容器的内部数据结构。也即，**swap 方法的原理是交换两个容器的内部指针以达到“交换整个容器”的效果。**

之所以 swap 用于交换 array 很慢是因为 array 保存数组使用的不是动态内存，因此没有一个指向容器数据的指针。所以在交换元素时只能一个一个元素的交换，而不能直接交换指向元素的指针。

#### 8.2 迭代器、指针

swap 只是交换容器所指向的底层数据对象意味着，除 array 外，swap 不会对任何元素进行拷贝、删除或插入操作，因此可以保证在常数时间内完成。并且元素不会移动的事实意味着，除 string 外，指向容器的迭代器、引用和指针在 swap 操作后不会失效。

> swap 会导致指向 array 和 string 的迭代器、指针和引用失效。对于 array 好理解，它移动了元素。但对于 string 是为什么呢？
>
> 原因在于：SSO（Short String Optimization），指 C++ 针对短字符串的优化。
>
> 默认情况下，C++ 的 std::string 都是存储在 heap 中，导致访问 std::string 需要经过一次寻址过程，速度较慢，并且这种实现的空间局部性不好，对 cache 的利用较低。
>
> 考虑到很多 string 的字符串长度很小，这个时候，我们可以把字符串存储到栈上，从而不需要进行内存分配，优化创建速度，并且访问栈上数据的局部性很好，速度比较快。
>
> 即 C++ 会自动把较短的字符串放到对象内部，较长的字符串放到动态内存。假如 std::string 用 SSO 实现，而待交换的两个对象中的字符串恰好一长一短，则原先指向短字符串中的迭代器会全部失效。

``` c++
vector<int> a{1, 2, 3, 4, 5};
vector<int> b{11, 12, 13};
int *p = &a[1];
cout << *p << endl;	// 2
cout << *(p + 1) << endl; // 3
swap(a, b);
// 指针p未失效，且仍然指向底层数据对象 {1,2,3,4,5}
cout << *p << endl;	// 2
cout << *(p + 1) << endl; // 3
```

##### 8.3 swap与vector

对于 vector 来说，swap 还交换了 capacity 的大小。

``` c++
vector<int> a(10, 1);
vector<int> b(20, 3);
swap(a, b);
cout << a.size() << ' ' << a.capacity() << endl; // 20, 20
cout << b.size() << ' ' << b.capacity() << endl; // 10, 10
```

通过这个性质，可以得到 swap 对于 vector 还有一个重要用途，先来看下面的代码：

``` C++
vector<int> a(10000);
cout << a.size() << endl;   // 10000
cout << a.capacity() << endl; // 10000
a.resize(10);
cout << a.size() << endl; // 10
cout << a.capacity() << endl; // 10000 
```

一开始我们希望创建一个很大的 vector，有 10000 个元素。但后来我们不需要这么多元素了，于是通过 resize 将元素数量调整到 10。此时 vector 虽然通过删除元素缩减了 vector 的大小（size），但是并没有减小它的容量（capacity），即实际在 heap 中分配的内存空间。另外对于 clear 也是同样的情况。

那么有没有什么办法既减少 size，又减少 capacity 呢？答案是 swap，通过将当前 vector 与一个局部 vector 进行 swap：

``` c++
vector<int> a(10000);
cout << a.size() << endl;   // 10000
cout << a.capacity() << endl; // 10000
vector<int>(10).swap(a);
cout << a.size() << endl;   // 10
cout << a.capacity() << endl; // 10
```

通过创建一个匿名局部对象 `vector<int>(10)` 与 a 交换，来缩减 a 的 capacity。又因为匿名对象使用完就会被自动释放，所以交换之后的局部 vector，也就是之前的 a 便销毁了。

#### 8.4 非成员版本swap

统一使用非成员版本的 swap 是一个好习惯，因为它更适用于泛型编程。

### 9. 容器的比较

容器的比较和 string 比较类似。都是先比较容器内元素再比较容器大小。

所有容器都支持 `==,!=` 运算符；无序关联式容器不支持 `<=,>=,<,>` 运算符。

注意不要把容器的比较与迭代器的比较搞混了。容器支持 `<` 运算符不代表该容器的迭代器支持 `<` 运算符，例如 `list`。

### 10. 添加元素

``` c++
c.push_back(t);			// copy ctor
c.emplace_back(args);	// ctor

c.push_front(t);
c.emplace_front(t);

c.insert(p,t);
c.emplace(p,args);
c.insert(p,n,t);
c.insert(p,b,e);
c.insert(p,il);
```

forward_list 不支持 push_back 和 emplace_back，这也显然，因为它没有指向尾元素的指针。vector 和 string 不支持 push_front 和 emplace_front。

所有 insert 都是在迭代器 p **之前**插入元素，返回值都是指向**新添加的第一个元素**的迭代器。如果我们传入的是 initializer_list 或一对迭代器并且范围为空，那么将直接返回 p。迭代器 p 可以指向容器中的任何位置，包括容器尾部之后的下一个位置。注意指针并不等价于迭代器，我们不可以将一个指针作为第一参数。

push 和 insert 传递的是元素类型的对象，这些对象被拷贝到容器中。emplace 传递的是构造函数所需的参数，从而直接在容器管理的动态内存中构造元素。不过 push 和 insert 实际上也可以传递构造函数所需的参数，只不过参数个数必须为1，此时本质上是通过转换构造函数创建了一个临时匿名对象，然后将其拷贝到容器中，所以仍然会产生额外开销。特别的，当我们希望传递一个调用默认构造函数的对象或构造函数需要多个参数时，push 和 emplace 在形式上有所不同：

``` C++
struct Foo {
    Foo()             {cout << "Foo" << endl;}
    Foo(int x)        {cout << "FooI" << endl;}
    Foo(double x)     {cout << "FooD" << endl;}
    Foo(int x, int y) {cout << "FooII" << endl;}
};

int main() 
{
    vector<Foo> c;
    
    c.push_back({});       // 必须添加{}        
    c.push_back(1);
    c.push_back({3.14});
    c.push_back({1, 2});   // 必须添加{} 
    
    // 不能添加{}
    c.emplace_back();
    c.emplace_back(1);
    c.emplace_back(3.14);
    c.emplace_back(1, 2);

    return 0;
}
```

### 11. 访问元素

``` C++
c.front();	// 返回引用，如果c为空，UB
c.back();	// 返回引用，如果c为空，UB
c[n];	    // 返回引用，n是无符号数，如果越界，UB
c.at(n);	// 如果越界，抛出out_of_range异常
```

包括 array 在内的每一个顺序容器都有一个 front 成员函数，而除了 forward_list 之外的所有顺序容器都有一个 back 成员函数。在使用 front 和 back 获取容器元素之前，要确保容器非空。如果容器为空，那么 front 和 back 的行为是未定义的。

在容器中访问元素的成员函数返回的都是引用，如果容器是一个 const 对象，则返回值是 const 的引用。

### 12. 删除元素

``` C++
c.pop_back();  // 若c为空，UB。返回void
c.pop_front(); // 若c为空，UB，返回void
c.earse(p);	   // 删除迭代器p所指定的元素，若p是尾后迭代器，UB
               // 返回一个指向被删元素“之后”元素的迭代器
               // 若p是尾元素，返回尾后迭代器
c.erase(b,e);  // 删除迭代器[b,e)所指定范围内的元素
			   // 返回一个指向最后一个被删除元素“之后”元素的迭代器
			   // 若e本身就是尾后迭代器，则返回尾后迭代器
			   // 若b==e，返回b
c.clear();     // 删除所有元素，返回void。只会清空size不会清空capacity
```

* forward_list 有特殊版本的 erase，不支持 pop_back
* vector 和 string 不支持 pop_front

删除 deque 中除首尾位置之外的任何元素都会使所有迭代器、引用和指针失效。指向 vector 或 string 中删除点之后位置的迭代器、引用和指针都会失效。

删除元素的成员函数并不会检查其参数。在删除元素之前，程序员必须保证它们是存在的。否则产生的行为是为定义的。

### 13. *forward_list*的特殊操作

``` C++
l.before_begin();	// 返回首前迭代器，不能解引用
l.cbefore_begin();

l.insert_after(p,t);	// 在p之后插入元素，返回一个指向“最后一个”插入元素的迭代器
l.insert_after(p,b,e);  // 若p为尾后迭代器，UB
l.insert_after(p,il);
l.emplace_after(p,args);	

l.erase_after(p);	// 删除p之后的元素，返回一个指向被删除元素之后的迭代器
                    // 若不存在则返回尾后迭代器
					// 如果p指向尾元素或尾后元素，UB
l.erase_after(b,e); // 删除(b,e]内的元素
```

forward_list 有自己特殊版本的插入（insert_after, emplace_after）和删除（erase_after）操作，但是为什么呢？

我们先考虑从一个单向链表中删除某个元素 p 会发生什么。我们需要修改 p 的前驱，让它的指针指向 p 的后继。但是 forward_list 是一个单向链表，我们没有简单的办法从一个单向链表中获取前驱。因此 forward_list 的做法是我们直接给出前驱，然后去操作它的后继。

> 还有一种做法是，修改当前元素值为下一个元素的值，删除下一个元素，但这种方法无法解决删除最后一个节点的情况。

由于这种操作与其它容器上的操作的实现方式不同， forward_list 并未定义 insert、emplace 和 erase，而是定义了名为 insert_after、emplace_after 和 erase_after 的操作。并且为了支持这些操作，forward_list 还定义了 before_begin，它返回一个首前迭代器。这个迭代器允许我们在首前元素之后增删元素，即在首元素之前增删元素。

另外，和 insert 返回指向第一个插入的元素的迭代器不同，forward_list 的 insert_after 返回的是指向**最后一个**插入元素的迭代器。这也是有理可循的，因为在链表中，我们需要按顺序插入元素。

### 14. 改变容器的大小

``` c++
c.resize(n);
c.resize(n,t); // 指定新插入元素的值
```

显然的，array 不支持 resize。

* 如果容器大小 `>` 所要求的大小，容器后部的元素会被删除；
* 如果容器大小 `<` 所要求的大小，会将新元素（值初始化）添加到容器后部。当然也可以通过其重载函数指定新元素的值。

### 15.  *deque* 的底层细节

deque 是一个双端队列，可以实现在头尾两端的相关操作，并且在头尾两端的操作十分高效。

* 与 vector 相比，vector 虽然也可以实现在头部的操作，但实现起来比较复杂，要挪动后面的所有元素
* 与 list 相比，由于其底层空间是<font color=blue>间断连续</font>空间，所以空间利用率要高于list，并且 list 不支持下标的随机访问，而 deque 则支持。

因此，可以说 deque 是集合了 list 与 vector 各自的优点（头尾高效操作+随机访问元素），但是自古以来鱼与熊掌不可兼得，deque 虽集合了各自的优点，但是却做不到 vector 与 list 那么极致。deque 的数据结构较为复杂，尤其是其迭代器。

一言以蔽之，deque 的底层结构有点类似于哈希表（指针数组 + 数组）。

#### 15.1 deque 的内存结构

![img](https://developer.qcloudimg.com/http-save/yehe-10181992/fba1ff96195245bcce753133ac72faad.png)

 如上图所示，deque 采用 map 作为主控，这里的 map 并非 STL 容器中的map，这里的 map 是一小块<font color=blue>连续</font>的空间，每个元素都是一个指针，该指针指向了一块缓冲区，这里的缓冲区用来存储数据。当 map 已经全部被使用，便需要再找一块更大的空间来作为 map。

另外，deque 要求 map 前后各**预留**一个 node 节点，以便扩充可用。

#### 15.2 deque 的迭代器

deque 的迭代器设计十分复杂，它在底层并不是相当于一个普通指针，如下所示：

![img](https://developer.qcloudimg.com/http-save/yehe-10181992/9f6c75f6ee56ce3c1bb7a280f63d2242.png)

通过右下角的迭代器结构可以发现，实现一个迭代器需要 4 个指针。除了指向元素地址的指针 cur，还需要：

* node：元素所处缓冲区在 map 中的位置
* first：元素所处缓冲区的头
* last：元素所处缓冲区的尾

由于 deque 的元素在底层并不是全部连续的，它实际上 buffer 内部连续，buffer 之间不连续。因此我们需要一个迭代器 node 来指向当前所在 buffer。当 cur 处于 last 时在执行 `++` 或当 cur 指向 first 时在执行`--`，就需要修改 node 指向的 buffer。

并且由于无法得知当前 cur 到底是 first 还是 last 还是处于中间，因此每次 `++` 和 `--` 操作都需要检测迭代器是否指向 buffer 的两端。这显然是一个费时的操作，因此 deque 随机访问的能力不如 vector 高效。

我们前面在讨论迭代器失效的情况时提到过，如果我们在 deque 的首尾插入元素，迭代器会失效，而指针和引用则不会。这是因为在首尾插入元素可能导致 map 重新分配，因此 node 会失效，迭代器也就失效了。但是指针和引用并不会，因为缓冲区中元素本身地址并未变化。

#### 15.3 deque 的扩容机制

deque 的空间不够用时，只需要在增加一个 buffer 用来保存数据就好了，当然此时还需要在 map 中增加一个指针。但如果 map 无法存放更多指向 buffer 的指针，就需要重新分配 map。

不同于 vector 的重新分配，vector 的重新分配可能涉及深拷贝的问题，即需要迁移所有数据。而 deque 则不会，因为 map 中存放的都是指向 buffer 的指针，重新分配时只需要迁移指针，不需要迁移指针所指向的具体元素，因此说 deque 的扩容效率比 vector 高得多。

#### 15.4 deque 的随机访问

deque 虽然支持随机访问，但是其效率也是不如 vector 的，这里假如我们第一个缓冲区已经存在了 3 个数据，且每一个缓冲区的大小固定为 10，这里我们要想实现访问第 25 个数据，在 vector 中则只需要 vector[24] 即可访问到该数据，而在deque 中则需要：

1. 先找到其所在的缓冲区
2. 再找到在缓冲区的第几个位置

可以看到，缓冲区的大小实际上影响了访问效率。如果缓冲区大小固定，那么第一步就很快，可以通过数学运算直接找到该缓冲区在 map 中的位置；而如果缓冲区大小不固定，那么实现上可能就比较麻烦了，可能需要保存每个缓冲区的大小，然后一个接一个缓冲区的判断。

#### 15.5 为什么stack和queue的默认容器是deque

1. deque 首位两端的插入和删除都非常高效
2. deque 的扩容比 vector 高效
3. deque 的空间利用率比 list 高效

最后，虽然 deque 的随机访问能力不如 vector，但 stack 和 queue 不需要随机访问呀。

### 16. 管理容量的成员函数

* shrink_to_fit 只适用于 vector，string 和 deque
* capacity 和 reserve 只适用于 vector 和 string

``` C++
c.shrink_to_fit();	// 将capacity减少为与size大小相同
c.capacity();	    // c可以保存的元素的数量
c.reserve(n);	    // 分配至少能容纳n个元素的内存空间
				    // 注意n是指容器中的元素个数
```

在前面我们介绍过通过创建临时对象和 swap 来收缩内存，实际上 C++ 库提供了 shrink_to_fit 来收缩内存。

* 只有当需要的内存空间超过当前容量（capacity）时，reserve 才会改变 vector 的大，并且分配至少与需求一样大的空间（可能更大）。
* 如果需要的内存空间小于等于当前容量，reserve 什么也不做（这意味着它不会缩小空间）。

另外，deque 不支持 capacity 和 reserve。首先，前面提到过，deque 动态分配内存以及调整内存非常快，因此 reserve 并没有太大意义。其次，deque 的内存是分段的，容量（capacity）这个概念并不好度量。

## 三、适配器

**适配器（adaptor）**是标准库中的一个通用概念。容器、迭代器和函数都有适配器：

* 改变容器的接口，称为容器适配器；
* 改变迭代器的接口，称为迭代器适配器；
* 改变仿函数的接口，称为仿函数适配器。

本质上，适配器是一种**机制（设计模式）**，使一种事物的行为类似于另外一种事物行为。以便以一种更符合特定场景的方式提供数据存储和检索功能。

例如我们可以通过修改 vector 或者 deque 的接口，使他们的行为看起来像 stack（一个先进后出的容器）。也可以修改 vector 和 list 的接口，使他们的行为看起来像一个 queue（一个先进先出的容器）。例如将 vector 的 push_back 修改为 push，将 pop_back 修改为 pop。由此，我们并没有修改 vector，却使他的行为既能看起像 stack，又能看起来像 queue。从而更符合特定场景的应用。所以说，通过适配器，开发者可以在不改变底层对象的情况下拓展和改变其接口和行为。

### 1. 容器适配器

> [*reference*](https://www.cnblogs.com/zjuhaohaoxuexi/p/16786660.html)

除了顺序容器外，标准库还定义了三个顺序容器适配器。注意<font color=blue>**容器适配器并不是容器**</font>，它们不具有容器的某些特点（例如：有迭代器）。有迭代器就乱套了，通过迭代器来访问一个 stack 的元素，它的行为就不是一个 stack 了。所以，我们无法使用另一个 stack 适配器的一个范围来对 stack 进行初始化操作。

* stack：先进后出，对 deque 的包装，也可以是 vector，list
* queue：先进先出对 deque 的包装，也可以是 list
* priority_queue：对 vector 的包装，也可以是 deque

``` c++
// stack
s.pop();
s.push(item);
s.emplace(args);
s.top();

// queue, priority_queue
q.push(item);
q.emplace(args);
q.pop();
q.front();	// queue
q.back();	// queue
q.top();	// priority_queue
```

注意上面提到的 `front()`、`back()` 和 `top()` 返回的都是引用类型，如果容器是 `const` 属性，则返回常引用。还有就是，容器适配器没有 `begin()` 和 `end()` 成员，因为这两个成员本质上就是返回的迭代器。

底层容器是作为适配器类模板的第二参数出现的（第一参数是元素类型），因此我们可以显示指定底层容器的类型。并且可以通过 `container_type` 查看适配器的底层容器类型，它是一个类型别名。

``` c++
stack<int, vector<int>> s;
stack<int, vector<int>>::container_type v;
cout << typeid(v).name() << endl; // St6vectorIiSaIiEE
```

#### 1.1 *prioity_queue*

优先队列也是队列，因此它也有普通队列的一些基本特征：

- **即使用此容器适配器存储元素只能“从一端进（称为队尾）**
- **从另一端出（称为队头）”**
- **且每次只能访问priority_queue 中位于队头的元素**

但是，priority_queue 容器适配器中元素的存和取，遵循的并不是 “First in,First out”（先入先出）原则，**先进队列的元素并不一定先出队列，而是优先级最大的元素最先出队列**。

`STL` 中，`priority_queue` 容器适配器的定义如下：

```cpp
template<typename _Tp, typename _Sequence = vector<_Tp>, 
						typename _Compare  = less<typename _Sequence::value_type> >
class priority_queue
```

`std::priority_queue` 默认用 `std::less` 实现最大堆，虽然看起来反直觉，但记住“默认是最大堆”即可。另外可以发现，如果指定了比较规则，则实例化时第二个模板类型参数 `_Sequence`（底层容器） 也要指定，此时不能使用默认的。

**可以使用普通数组或其它容器（和容器适配器底层所用容器类型不同的容器）中指定范围内的数据，对 priority_queue 容器适配器进行初始化**：（**个人**：注意这里和 stack，queue 不同，stack 和 queue 不可以这样做，可能是因为用来初始化 priority_queue 的数据不需要有序，priority_queue 会自动对它们进行排序）　

然后我们发现 `list` 无法作为 `std::priority_queue` 的默认容器，这是因为 `priority_queue` 底层调用的 `make_heap` 函数的参数要求是一个随机访问迭代器范围，`list` 是双向迭代器而不是随机访问迭代器。

``` CPP
template<typename _RandomAccessIterator, typename _Compare>
_GLIBCXX20_CONSTEXPR
inline void
make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
          _Compare __comp)
{
    // concept requirements
    __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
                                _RandomAccessIterator>)
        __glibcxx_requires_valid_range(__first, __last);
    __glibcxx_requires_irreflexive_pred(__first, __last, __comp);

    typedef __decltype(__comp) _Cmp;
    __gnu_cxx::__ops::_Iter_comp_iter<_Cmp> __cmp(_GLIBCXX_MOVE(__comp));
    std::__make_heap(__first, __last, __cmp);
}
```



#### 1.2 *stack*

可以**用一个 stack 适配器来初始化另一个 stack 适配器，只要它们存储的元素类型以及底层采用的基础容器类型相同即可**。

``` c++
std::list<int> values{ 1, 2, 3 };
// 栈顶元素是3
std::stack<int, std::list<int>> my_stack1(values);
std::stack<int, std::list<int>> my_stack=my_stack1; // copt ctor
//std::stack<int, std::list<int>> my_stack(my_stack1);
```

#### 1.3 *queue*

**可以直接通过 queue 容器适配器来初始化另一个 queue 容器适配器，只要它们存储的元素类型以及底层采用的基础容器类型相同即可**。例如：

``` c++
std::deque<int> values{1,2,3};
// 1是队头
std::queue<int> my_queue1(values);
std::queue<int> my_queue(my_queue1);
//或者使用
//std::queue<int> my_queue = my_queue1;
```

### 2. 迭代器适配器

STL 提供了很多应用于迭代器的适配器，我们可以通过头文件 `iterator` 使用它们：

* back_insert_iterator
* front_insert_iterator
* insert_iterator：
* reverse_iterator：rbegin 和 rend 返回的迭代器
* istream_iterator
* ostream_iterator
* istreambuf_iterator
* ostreambuf_iterator
* 等等

### 3. 仿函数适配器

仿函数适配器是数量最庞大的适配器族群，使用也是最灵活的，可以嵌套使用。我们可以头文件 `functional` 使用它们：

* bind
* negate
* compose

## 四、泛型算法

很大程序上，容器只定义了**极少**的操作。比如构造函数，添加和删除元素，确定容器大小以及返回指向特定元素的迭代器的操作。其它一些有用的操作，如排序和搜索，并不是由容器定义的，而是由标准库算法实现的。

### 1. 泛型算法与迭代器

大多数算法都定义在头文件 `<algorithm>` 中。标准库还在头文件 `<numeric>` 中定义了一组数值泛型算法。

一般情况下，这些算法并不直接操纵容器，而是遍历由两个**迭代器**指定的一个元素范围。因此不能直接向容器中添加或删除元素。泛型算法运行在迭代器之上而不会执行容器操作的特性带来了一个令人惊讶但非常必要的编程假定：<font color=blue>**泛型算法永远不会改变底层容器的大小**</font>。这也解释了为啥泛型算法无法执行容器的操作，因为迭代器并不是容器的对象。

> 容器内置的算法一般是改变底层容器大小的，例如增加/删除元素。泛型算法一般不会改变底层容器的大小，它只是在已有元素的基础上进行操作，例如查询/排序，

不过，如果迭代器是 `insert_iterator`，是可以改变容器大小的。这是因为泛型算法是通过迭代器来操纵容器的，<font  color=blue>**迭代器的能力决定了泛型算法的能力**</font>。

泛型算法的一大优点是“泛型”，这就是一个算法可以用于多种不同的数据类型，算法与所操作的数据结构分离。而做法就是将迭代器作为算法和容器之间的桥梁，算法从不操作具体的容器，也就不存在与特殊容器绑定，不适用于其他容器的问题。也可以说，<font color=blue>**算法感知不到容器的存在**</font>，算法只不过是通过迭代器操作某种类型的元素。

### 2. 泛型算法举例

#### （1）accumulate

accumulate 的第三个参数的类型决定了函数中使用那个加法运算以及返回值的类型。这有时会导致一些难以察觉的错误：

``` c++
vector<double> a{1.7, 2.8, 3.9};
auto s1 = accumulate(a.begin(), a.end(), 0);   // 6
auto s2 = accumulate(a.begin(), a.end(), 0.0); // 8.4
```

第二行代码，由于我们将第三个元素指定为 0，它会被视为 int 类型，因此这里求和时会将 double 元素视为 int 元素。

#### （2）equal

`equal` 接受三个迭代器，前两个表示第一个序列的元素范围，第三个表示第二个序列的首元素。

注意对于这种接受单一迭代器来表示第二个序列的算法，都假定第二个序列至少和第一个序列一样长。和数组一样，编译器并不能检查出越界错误，此时产生的行为是未定义的。

#### （3）fill_n

`fill` 用于向指定范围内的元素填充给定的值：

``` c++
vector<int> v;
v.reserve(10);
fill_n(v.begin(), 10, 1024);
//cout << v.size() << ' ' << v.capacity() << endl; // 0 10 
for(auto &x : v)    
    cout << x << endl;
```

对于上面的代码，你以为会输出 10 个 1024？错误的，实际上代码的行为是未定义的。因为 reserve 只是为 vector 预留了空间（capacity），但实际上并没有在这个空间中创建对象（size）。因此 fill_n 是在没有任何元素的容器中执行赋值的，它的行为是未定义的。

实质上，泛型算法对容器的要求并不是有足够的空间，而是有足够的元素。

### 3. 定制操作

可以通过<font color=blue>**谓词（predict）**</font>来自定义泛型算法的一些条件判断。

<font color=blue>**谓词是一个可调用的表达式，其返回结果是一个能用作条件的值。**</font>根据接受参数的不同，标准库算法所使用的谓词分为两类：一元谓词（unary predict）和二元谓词（binary predict）。接受谓词参数的算法对输入序列中的元素调用谓词。因此，元素类型必须能转换为谓词的参数类型。

不过有时候，我们希望进行的操作需要更多参数，超出了算法对谓词的限制（只能接受一个或两个参数）。这时我们可以向算法传递任意类别的**<font color=blue>可调用对象</font>**。对于一个对象或一个表达式,如果可以对其使用调用运算符，则称其为可调用的。例如函数，函数指针，lambda 表达式。

### 4. *lambda*

在 C++ 中，**Lambda 表达式**是一种用于创建匿名函数对象的便捷方式，常用于简化代码（如 STL 算法、回调等）。

#### 4.1 基本语法

##### 1. 定义

```cpp
[capture-list] (parameters) mutable? -> return-type { body }
```

- **`capture-list`**：捕获外部变量（见下文）。
- **`parameters`**：参数列表（可省略，如 `[] { ... }`）。
- **`mutable`**：允许修改按值捕获的变量（可选）。
- **`return-type`**：返回类型（可自动推导时省略）。

##### 2 **变量捕获方式**

*Lambda* 可以捕获外部作用域的变量，方式如下：

| 捕获方式     | 描述                                                   |
| :----------- | :----------------------------------------------------- |
| `[]`         | 不捕获任何变量。                                       |
| `[x, &y]`    | 按值捕获 `x`，按引用捕获 `y`。                         |
| `[=]`        | 默认按值捕获所有外部变量（隐式捕获）。                 |
| `[&]`        | 默认按引用捕获所有外部变量（隐式捕获）。               |
| `[this]`     | 捕获当前类的 `this` 指针（用于成员函数中的 Lambda）。  |
| `[x = expr]` | C++14 起，用表达式初始化捕获变量（广义 Lambda 捕获）。 |

**示例**：

```cpp
int a = 1, b = 2;
auto lambda = [a, &b] { 
    cout << a << " " << b;  // a 是副本，b 是引用
    // a = 10;  // 错误：按值捕获默认不可修改（除非加 mutable）
    b = 20;     // 正确：按引用捕获可直接修改
};
```

##### 3 **`mutable` 关键字**

- <font color=blue>**默认情况下**，按值捕获的变量在 Lambda 内部是 `const` 的（不可修改）。</font>
- **`mutable`** 允许修改按值捕获的变量（但不影响外部变量）。

**示例**：

```cpp
int x = 10;
auto lambda = [x]() mutable {
    x = 20;  // 允许修改内部副本
    cout << x;  // 输出 20
};
lambda();
cout << x;  // 输出 10（外部 x 不变）
```

##### 4 **捕获多个变量时的修改控制**

- **问题**：如果 Lambda 表达式通过 **值捕获（`[=]` 或显式 `[a, b, c]`）** 多个变量，但只想修改其中一个变量，仍然需要使用 `mutable` 关键字。

- **解决方案**：

  - **方法 1**：混合捕获（部分按引用）。

    ```cpp
    int a = 1, b = 2;
    auto lambda = [a, &b] { b = 20; };  // 只修改 b（外部受影响）
    ```

  - **方法 2**：C++14 广义捕获（仅修改内部副本）。

    ```cpp
    auto lambda = [a, b_copy = b]() mutable { 
        b_copy = 200;  // 仅修改内部副本
    };
    ```

##### 5 注意事项

1. **生命周期问题**：按引用捕获时，确保被引用的变量在 Lambda 执行时仍有效。

   ```cpp
   std::function<void()> func;
   {
       int x = 10;
       func = [&x] { cout << x; };  // x 是悬垂引用！
   }
   func();  // UB！
   ```

2. **性能**：频繁调用的 Lambda 避免按值捕获大对象（改用引用或移动语义）。

##### 6 **C++14/17 增强**

- **广义捕获**（C++14）：

  ```cpp
  auto ptr = std::make_unique<int>(42);
  auto lambda = [p = std::move(ptr)] { cout << *p; };  // 移动捕获
  ```

- **`constexpr` Lambda**（C++17）：

  ```cpp
  constexpr auto square = [](int x) { return x * x; };
  static_assert(square(3) == 9);
  ```

#### 4.2 捕获变量

当我们使用值捕获时，必须保证被捕获对象可拷贝。**拷贝发生在 *lambda* 创建时**，因此在 *lambda* 创建之后，修改被拷贝对象的值不会影响 *lambda*：

``` c++
int val = 123;
auto func = [val]() {
    cout << "val: " << val << endl;
};
val = 456;
func();   // 123
```

当我们使用引用捕获时，必须确保被引用对象不会销毁。

---

*lambda* 的捕获列表只适用于<font color=blue>**局部非 *static* 变量**</font>，*lambda* 可以直接使用并修改局部 *static* 变量和在它所在函数之外的声明。

``` C++
int gi = 1024;

int main() 
{
    static int si = 666;

    []() {
        cout << gi << endl;
        cout << si << endl;
        gi = 11;
        si = 22;
    }();

    cout << gi << endl;
    cout << si << endl;

    return 0;
}
```

### 5. *bind*

尽管 *lambda* 很好用，但是如果我们需要在不同函数中使用 *lambda* 的话，唯一的办法就是在每个函数中写一遍 *lambda*，这显然不如直接定义一个函数来的好。但是函数很多情况下无法作为谓词，因为函数中的参数个数可能与算法的要求不匹配，通过 *bind* 可以解决这个问题。

*bind* 是一个仿函数适配器，它定义在头文件 `functional` 中。它接受一个可调用对象，生成一个新的可调用对象来“使用”原对象的参数列表。调用 *bind* 的一般形式为：

``` c++
auto newCallable = bind(callable, arg_list);
```

其中，`arg_list` 是一个逗号分隔的参数列表，对应给定的 `callable` 的参数。即，当我们调用 `newCallable` 时，``newCallable` 会调用 `callable`，并传入 `arg_list`。

`arg_list` 中的参数可能包含形如 `_n` 的名字，其中 `n` 是一个整数。这些参数是**“占位符”**，`_n` 就表示我们传递给 `newCallable` 的第 `n` 个参数。这些明明都定义在一个名为 `placeholders` 的命名空间中。而 `placeholders` 又定义在 `std` 中（`std::placeholders::_n`）。

**默认情况下，*bind* 的那些不是占位符的参数被拷贝到 *bind* 返回的可调用对象中。**但是，与 *lambda* 类似，有时我们希望以引用的形式传递参数。此时我们可以通过标准库的 `ref` 或者 `cref` 来解决这个问题，它们定义在头文件 `functional` 中，分别返回对象的引用和 *const* 引用。

``` C++
using namespace std;
using namespace std::placeholders;

void f(const int &a, int &b, int &c, int &d, const int &e)
{
    cout << a << ' ' << b << ' ' << c << ' ' << d << ' ' << e << endl;
    b >>= 1, c >>= 1, d >>= 1;
}

int global_val = 10;

int main() 
{
    static int static_val = 20;
    int local_val = 30;
    
    auto newf = bind(f, _2, ref(local_val), ref(static_val), global_val, _1);
    newf(40, 50); // 50, 30, 20, 10, 40
    cout << local_val << endl;  // 15
    cout << static_val << endl; // 10 
    cout << global_val << endl; // 10
    return 0;
}
```

*bind* 本质上就是通过绑定外部对象来实现传递多个参数的，例如上例中的 `ref(local_val), ref(static_val), global_val`，这些参数对于函数调用是不可见的。对于调用 `newf(40, 50);` 我们形式上传递了两个参数，但实际上传递了五个参数。

### 6. 迭代器

除了为每个容器定义的迭代器之外，标准库在头文件 `iterator` 中还定义了额外几种迭代器模板（迭代器适配器）：

* insert iterator
  * insert_iterator
  * front_insert_iterator
  * back_insert_iterator
* stream iterator
  * istream_iterator
  * ostream_iterator
* reverse iterator：除了 forward_list 之外的标准库容器都有反向迭代器
* move iterator

#### 6.1 insert iterator

``` C++
it = t;	// 在it指定的当前位置插入值t。
	    // 插入的方式依赖于it绑定的容器c，可能是：
		// c.push_back(t); 
		// c.push_front(t);
		// c.insert(t,p); 在p的之前位置插入元素t
*it, ++it, it++;
```

插入迭代器是一种迭代器适配器，用于将元素插入到容器中的特定位置。当我们通过一个插入迭代器进行赋值时，该迭代器**调用容器操作**来向给定容器的指定位置插入一个元素。

插入迭代器有三种类型，差异在于元素插入的位置：

* `back_inserter(container)`：创建一个使用 `push_back `的 `back_insert_iterator` 类型的对象
* `front_inserter(container)`：创建一个使用 `push_front `的 `front_insert_iterator` 类型的对象
* `inserter(container, iter)`：创建一个使用 `insert `的 `insert_iterator` 类型的对象，在 `iter` 之前插入元素

注意，`std::inserter`，`std::back_inserter` 和 `std::front_inserter` 都是<font color=blue>**函数**</font>，它们的作用是创建不同类型的**插入迭代器**。`std::insert_iterator`，`std::back_insert_iterator` 和 `std::front_insert_iterator` 是<font color=blue>**类模板**</font>，我们可以直接定义并初始化它们的对象：

``` c++
vector<int> v = {1,2,3,4,5,6,7,8,9};
vector<int> p = {11,22,33,44,55};
insert_iterator<vector<int>> insertIter(p, p.begin() + 1);
unique_copy(v.begin(), v.end(), insertIter);
// p: 11 1 2 3 4 5 6 7 8 9 22 33 44 55 
```

#### 6.2 stream iterator

`istream_iterator` 读取输入流，`ostream_iterator` 向一个输出流写数据。这两个迭代器将它们对应的流当作一个特定类型的**<font color=blue>元素序列</font>**来处理。通过使用流迭代器，我们可以使用泛型算法从流对象读取数据以及向其写入数据。

##### 6.2.1 istream_iterator 操作

`istream_iterator` 使用 ``>>`` 来读取流，因此 `istream_iterator` 要读取的对象必须定义了输入运算符。

``` c++
istream_iterator<T> in(is); // in从输入流is读取类型为T的值
istream_iterator<T> end;	// 表示尾后位置（EOF或IO错误）
							// 相当于end迭代器

in1 == in2;	// in1和in2必须是相同类型,如果它们都是尾后迭代器或绑定到同一个输入对象则相等
in1 != in2; 

*in;		// 返回从流中读取的值
in->mem;	// <=>(*in).mem
++in, in++; // 调用元素类型所定义的>>运算符从输入流中读取下一个值
```

<font color=blue>`istream_iterator` 允许使用**懒惰求值（Lazy Evaluation）**</font>。

标准保证的行为：

- **首次解引用前完成读取**：标准要求实现必须保证在第一次解引用迭代器时，值已经被读取并缓存。
- **构造时可能延迟读取**：实现可以选择在构造函数中立即读取，也可以延迟到首次解引用时。

标准库中的实现所保证的是，在我们第一次解引用迭代器之前，从流中读取数据的操作已经完成了。对于大多数程序来说，立即读取和推迟读取没什么差别。但是，如果我们创建了一个 `istream_iterator` 对象，没有使用就立即销毁了（此时应不应该读取数据），或者我们正在从两个不同的对象同步读取一个流（谁先读取数据），那么何时读取可能就很重要了。

``` c++
istream_iterator<int> in(cin), eof;
// 计算我们读取的整型元素之和
cout << accumulate(in, eof, 0) << endl;
```

##### 6.2.2 ostream_iterator 操作

``` c++
ostream_iterator<T> out(os);         // 基本形式，无分隔符
ostream_iterator<T> out(os, d);      // 带分隔符d（d必须是C风格字符串）
out = val;      // 使用<<运算符将val写入到out所绑定的ostream
*out;           // 无操作，返回out的引用（为了保持迭代器接口）
++out;          // 无操作，返回out的引用
out++;          // 无操作，返回out的引用
```

`ostream_iterator` 使用 `operator <<` 来写入流，因此 `ostream_iterator` 要写入的对象必须定义了输出运算符。当创建一个 `ostream_iterator` 时，我们可以提供（可选的）第二参数，它是一个 C 风格字符串（即，一个字符串字面值常量 `const char*` 或一个指向空字符结尾的字符数组的指针`char ch[]`），在输出每个元素之后都会打印此字符串。

必须将 `ostream_iterator` 绑定到一个流，不允许空的或表示尾后位置的 `ostream_iterator`。

``` c++
vector<int> v{1, 2, 3, 4, 5};
ostream_iterator<int> out(cout, ",");   // ","作为分隔符
for(auto &e : v)
    *out ++  = e; // 等价于 out=e;
cout << endl;
// 1,2,3,4,5,
```

虽然 `*out ++ =e` 与 `out = e` 在效果上是一样的，但是仍然推荐第一种形式。在这种写法中，流迭代器的使用和其他迭代器的使用形式保持一致。对于读者来说，行为也更加清晰。

不过我们还有一种更巧妙地写法：

``` C++
vector<int> v{1, 2, 3, 4, 5};
ostream_iterator<int> out(cout, ",");  
copy(v.begin(), v.end(), out); // 1,2,3,4,5,
```

#### 6.3 reverse iterator

反向迭代器就是在容器中从尾元素向首元素反向移动的迭代器。对于反向迭代器，递增和递减的含义会颠倒过来：递增一个反向迭代器会向前移动元素，递减一个反向迭代器会向后移动元素。

除了 `forward_list`，其它容器都有反向迭代器。我们可以通过 `rbegin()`，``rend()``，``crbegin()``，``crend()`` 来获取反向迭代器。其中 `rbegin()` 指向最后一个元素，``rend()`` 指向第一个元素的前一个位置。

通过反向迭代器可以方便的实现逆序 `sort`（从大到小排序）：

``` CPP
sort(v.rbegin(), v.rend());
```

通过 `std::reverse_iterator<Iter>::base` 可以将反向迭代器转换为对应的正向迭代器：

``` cpp
vector<int> v{0,1,2,3,4,5};

vector<int>::iterator it = v.begin() + 1; 
vector<int>::reverse_iterator rit(it);    // 我们可以将一个正向迭代器直接赋值给反向迭代器
vector<int>::iterator rrit = rit.base();
cout << *it << ' ' << *rit << ' ' << *rrit << endl; // 1 0 1
```

正向迭代器和反向迭代器所指向位置如图所示：

``` C++
     begin ----------------------------------> end   
       |      |      |      |      |      |     |
       ⬇      ⬇      ⬇      ⬇      ⬇      ⬇      ⬇
      [0] -> [1] -> [2] -> [3] -> [4] -> [5]
⬆      ⬆       ⬆      ⬆      ⬆      ⬆      ⬆   
|      |       |      |      |      |     |
rend <--------------------------------- rbegin
```

这种设计主要是为了确保在相互转换时，正向迭代器和反向迭代器指向相同的元素范围，从而保证对某些元素，无论是正向处理还是逆向处理都是相同的。

* 逆向转正向（*next*）：对于逆向迭代器 `[c.rbegin(), comma)`，相应的正向迭代器范围为 `[comma.next(), c.end())`。
* 正向转逆向（*prev*）：对于正向迭代器 `[c.begin(), comma)`，相应的反向迭代器范围为 `[comma.prev(), c.rend())`。

这其实也反映了左闭右开的区间特性，只不过对应逆向迭代器来说，`rbegin` 就是它的左区间。

#### 6.4 move iterator

`move_iterator` 将解引用操作转换为 `std::move`，允许移动元素而非复制：

```cpp
std::vector<std::string> vec = {"hello", "world"};
std::vector<std::string> vec2(std::make_move_iterator(vec.begin()),
                             std::make_move_iterator(vec.end()));
// vec 中的字符串被移动到 vec2，vec 中的元素处于有效但未定义状态。
```

#### 附：惰性求值

##### 1. **惰性求值（Lazy Evaluation）**

- **定义**：只有在真正需要值的时候才从流中读取数据。
- **行为**：
  - 构造 `istream_iterator` 时**不会立即读取输入**。
  - 第一次解引用迭代器（如 `*it`）时，才从流中读取数据并缓存。
- **理论优势**：避免不必要的输入操作（如果迭代器未被使用）。

##### 2. **立即读取（Eager Evaluation）**

- **定义**：在构造 `istream_iterator` 时立即从流中读取数据。
- **行为**：
  - 构造 `istream_iterator` 时**立即阻塞并等待输入**。
  - 解引用时直接返回已缓存的值。
- **实际表现**：所有主流编译器（GCC、Clang、MSVC）均采用这种方式。

##### 3. **代码示例对比**

假设标准允许惰性求值（理论行为）：

```CPP
#include <iostream>
#include <iterator>
using namespace std;

int main() 
{
    cout << "构造迭代器（惰性求值）..." << endl;
    istream_iterator<int> in(cin);  // 此时不会读取输入

    cout << "解引用迭代器..." << endl;
    if (in != istream_iterator<int>()) {
        cout << "第一个值: " << *in << endl;  // 此时才读取输入
    }

    return 0;
}
```

**预期交互**：

1. 程序输出 `构造迭代器（惰性求值）...`，此时不会阻塞。
2. 当执行到 `*in` 时，程序阻塞并等待用户输入。

实际行为（立即读取）：

```CPP
#include <iostream>
#include <iterator>
using namespace std;

int main() {
    cout << "构造迭代器（立即读取）..." << endl;
    istream_iterator<int> in(cin);  // 此处立即阻塞等待输入

    cout << "解引用迭代器..." << endl;
    if (in != istream_iterator<int>()) {
        cout << "第一个值: " << *in << endl;  // 直接返回缓存值
    }

    return 0;
}
```

**实际交互**：

1. 程序输出 `构造迭代器（立即读取）...` 后**立即阻塞**，等待用户输入。
2. 输入一个值（如 `42`）后，程序继续执行，输出 `第一个值: 42`。

##### 4. **关键区别场景**

场景1：迭代器构造后未使用

```CPP
{
    istream_iterator<int> unused(cin);  // 立即读取（实际行为）
    // 未使用unused，但已消耗一个输入
}
```

- **惰性求值**：不会读取输入，无副作用。
- **立即读取**：已消耗一个输入，可能导致数据丢失。

场景2：多迭代器竞争

```CPP
istream_iterator<int> it1(cin);  // 读取第一个输入
istream_iterator<int> it2(cin);  // 读取第二个输入
```

- **惰性求值**：两个迭代器均未读取，后续操作可能混乱。
- **立即读取**：明确按构造顺序消耗输入（`it1` 拿第一个值，`it2` 拿第二个值）。

##### 5. **为什么标准允许惰性求值，但实现却用立即读取？**

- **标准灵活性**：允许编译器优化（如延迟读取）。
- **实际需求**：立即读取能更早发现输入错误（如类型不匹配），并提供可预测的行为。

##### 6. **如何验证你的编译器的行为？**

运行以下代码：

```CPP
#include <iostream>
#include <iterator>
using namespace std;

int main() {
    cout << "构造迭代器前（注意是否阻塞）..." << endl;
    istream_iterator<int> in(cin);
    cout << "构造迭代器后（如果未阻塞，则是惰性求值）" << endl;
    return 0;
}
```

- **如果程序在构造 `in` 后阻塞**：说明是立即读取（实际行为）。
- **如果程序直接退出**：说明是惰性求值（理论上可能，但实际不存在）。

经过测试，在 `g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0` 下，是立即求值。

##### 7. 总结

| 特性               | 惰性求值（理论） | 立即读取（实际）      |
| :----------------- | :--------------- | :-------------------- |
| **读取时机**       | 首次解引用时     | 构造函数调用时        |
| **用户输入阻塞点** | 在 `*it` 时阻塞  | 在构造 `it` 时阻塞    |
| **未使用的副作用** | 无               | 消耗一个输入          |
| **主流实现**       | 无               | GCC/Clang/MSVC 均采用 |

**最终结论**：虽然标准允许惰性求值，但在实践中可以认为 `istream_iterator` 总是立即读取输入，这种设计更简单、更可预测。

### 7. 泛型算法结构

#### 7.1 五类迭代器

泛型算法是通过迭代器操纵容器的，因此任何算符的最基本的特性就是它要求其迭代器提供哪些操作。不同的泛型算法对迭代器提出了不同的要求。例如 `find` 要求迭代器有可以访问元素、递增和比较迭代器是否相等这些能力；`sort` 要求迭代器有读写和随机访问的能力。根据这些要求，泛型算法所要求的迭代器可以分为 5 种类别。每个泛型算法都会对它的每个迭代器参数指明须提供哪类迭代器（实际上是最小能力要求的迭代器）：

``` c++
input iterator;			// 只读，不写；单遍扫描，只能递增
output iterator;		// 只写，不读；单遍扫描，只能递增
forward iterator;	    // 可读写；多遍扫描，只能递增
bidirectional iterator; // 可读写；多遍扫描，可递增递减
random-access iterator; // 可读写；多遍扫描，支持全部迭代器运算


input iterator  output_iterator
            /     \
         forward iterator
               ⬇
     bidirectional iterator
               ⬇
      random-access iterator
```

迭代器是按照它们所提供别的操作来分类的，而这种分类形成了一种层次。除了输出迭代器之外，一个高层类别的的迭代器支持底层类别迭代器的所有操作。

* 输入迭代器只用于顺序访问。对于一个输入迭代器，`*it++` 保证是有效的，但递增它可能导致所有其他指向流的迭代器失效。其结果就是，不能保证输入迭代器的状态可以保存下来并用于访问元素。因此，输入迭代器只能用于单遍扫描算法。例如 `find` 和 `accumulate` 函数；而 `istream_iterator` 是一种输入迭代器。
* 输出迭代器可以看作输入迭代器功能上的补集——只写而不读元素。我们只能向一个输出迭代器赋值一次。类似输入迭代器，输出迭代器只能用于单遍扫描算法。用作目的位置的迭代器通常都是输出迭代器。例如 `copy` 函数的第三个参数。`ostream_iterator` 是输出迭代器。
* 向前迭代器可以多次读写同一个元素，因此我们可以保存向前迭代器状态，从而多次扫描元素。`forward_list` 上的迭代器就是向前迭代器。
* 双向迭代器除了支持向前迭代器的操作之外，还支持递减运算符。算法 `reverse_iterator` 要求双向迭代器。
* 随机访问迭代器支持在常量时间内随机访问序列中任意元素的能力。此类迭代器支持双向迭代器的所有功能。下标运算符 `iter[n]` 与 `*(iter[n])` 等价。算法 sort 要求随机访问迭代器。

#### 7.2 算法形参模式

大多数算法具有如下 4 种形式之一：

``` c+
func(beg, end, other args);
func(beg, end, dest, other args);
func(beg, end, beg2, other args);
func(beg, end, beg2, end2, other args);
```

* 几乎所有算法都接受一个输入范围 `[beg，end)` 来指定算法所操作的输入元素序列。
* 此外算法还可以指定一个目标位置（`dest`）或者第二个元素序列（`beg2` 或 `[beg2,end2)`）。
* 除了这些迭代器参数，一些算法还接受额外的，非迭代器的特定参数。例如接受一个函数指针来替代内置的 `operator <` 运算符。

对于第三个参数是一个目标位置（`dest`）或者是用一个首迭代器（`beg2`）表示一个元素范围的情况，算法会基于以下假设：

* `dest` 参数是一个表示算法可以写入的目的位置的迭代器。算法假定：按其需要写入数据，不管写入多少个元素都是安全的。
* 算法假定从 `beg2` 开始的范围与 `beg` 和 `end` 所表示的范围至少一样大。

如果 `dest` 是一个直接指向容器的迭代器，那么算法将输出数据写入到容器中已存在元素内。更常见的情况是，`dest` 被绑定到一个插入迭代器或一个输出迭代器，这两种情况下，不管要写入多少个元素都没问题。

#### 7.3 算法命名和重载规范

除了参数规范，算法还遵循一套命名和重载规范。这些规范处理诸如：

* 如何提供一个操作代替默认的 `operator <` 或 `operator ==` 运算符
* 算法是否将输出数据写入输入序列还是一个分离的目的位置

##### (1) 一些算法使用重载形式传递一个谓词

接受谓词参数来代替 `operator <` 或 `operator ==` 运算符的算法，以及那些不接受额外参数的算法，通常都是重载的函数。例如：

``` c++
unique(beg,end);		// 使用 == 运算符比较元素
unique(beg,end,comp);	// 使用 comp 比较元素
```

由于这两个版本在参数个数上不相等，因此具体应该调用哪个版本不会产生歧义。

##### (2) _if 版本的算法

接受一个元素值的算法通常有一个不同命的（不是重载的）版本，该版本接受一个谓词代替元素值。接受谓词参数的算法都有附加的 `_if` 前缀：

``` c++
find(beg,end,val);		// 查找输入范围中val第一次出现的位置
find_if(beg,end,pred);  // 查找输入范围中第一个令pred为真的位置
```

因为两个版本接受相同数量的参数，可能产生重载歧义，虽然很罕见，毕竟类型不相同。但为了避免任何可能的歧义，标准库选择提供不同名字的版本而不是重载。

##### (3) 区分拷贝元素的版本和不拷贝的版本

默认情况下，重排元素的算法将重排后的元素写回给定的输入序列中。这些算法还提供另一个版本，将元素写到一个指定的输出目标位置。如我们所见，写到额外目的空间的算法都在名字后面附加一个 `_copy`：

``` c++
reverse(beg,end);
reverse_copy(beg,end,dest);
```

一些算法还同时提供 `_copy` 和 `_if` 版本。这些版本接受一个目的位置迭代器和一个谓词：

``` c++
// 去掉容器中的奇数元素
remove_if(v1.begin(), v1.end(), [](int i){
    return i & 1;
});
remove_copy_if(v1.begin(), v1.end(), back_inserter(v2), [](int i){ 
    return i&1;
});
```

### 8. 特定容器操作

与其他容器不同，容器类型 `list` 和 `forward_list` 定义了几个成员函数形式的算法。特别是，他们定义了独有的 `sort`、`merge`、`remove`、`reverse` 和 `unique`。通用版本的 `sort` 要求随机访问迭代器，因此不能用于 `list` 和 `forward_list`。

链表类型定义的其他算法的某些通用版本可以用于链表，但代价太高。这些算法需要交换输入序列中的元素。一个链表可以通过改变元素间的链接而不是真的交换它们的值来“交换”元素。因此，这些链表版本的算法的性能比对应的通用版本好得多，特别是节点对象特别大的情况。

**对于 list 和 forward_list，应该优先使用成员函数版本的算法而不是通用的算法。**这很显然，既然它们额外定义了成员版本的算法，而不是使用泛型算法，就说明标准库定义的泛型算法要么不适用，要么性能不好。

`list` 和 `forward_list` 成员函数版本的算法（这些操作都返回 `void`）：

```  C++
// 标准库也有的
lst.merge(lst2);      // 排序合并。lst和lst2都必须是有序的，合并之后lst2的元素会被删除
lst.merge(lst2,comp); 

lst.remove(val);     // 调用erase删除掉与给定值相等(operator==)或pred为真的元素
lst.remove_if(pred); 

lst.reverse();

lst.sort();          
lst.sort(comp);

lst.unique();        // 调用erase删除同一个值的连续拷贝，使用operator==
lst.unique(pred);    // 使用pred代替operator==

// 链表特有的splice操作: 将另一个链表的一部分移动到当前链表的特定位置
	// 时间复杂度为常数，因为它不需要复制或重新分配节点内存，而是只需要修改指针
	// 即，直接将被移动链表的数据link到新链表当中，因此被移动的元素会在原链表中移除

	// p：当前链表中的迭代器,表示插入的具体位置
	// other_list：被移到的另一个链表，可以是当前链表，但p不能指向给定范围中的元素
	// args用于指定other_list移动元素的范围,如果没有指定args，移动整个other_list
lst.splice(p,other_list, args);
forward_lst.splice_after(p,other_list, args);
	// args 的形式：
(b,e); // 移动 [b,e)
(b);  // 对于list，将移动b；对于forward_list，移动b之后的元素
```

链表的 `splice` 具有常数级别的时间复杂度，这是在牺牲 `size` 操作的情况下实现的，对于链表来说，`size` 的时间复杂度是线性级别的。因为如果我们要实现常数级别的 `size` 操作，就需要在 `splice` 的过程中维护两个链表的 `size`，那么对于 `args=(b,e)` 的情况，我们只能通过线性遍历的办法得知 `[b,e)` 这个区间中链表元素的个数，这就会导致 `splice` 的时间复杂度是线性级别的。因此对于 `size` 和 `splice` 两个操作，鱼和熊掌不可兼得，而相较于 `size`，`splice` 是一个更常用的操作，因此这里选择牺牲 `size` 操作的时间性能。

我们前面提到过，泛型算法并不直接改变他们所操作的容器的大小。他们会将元素从一个位置拷贝到另一个位置，但不会直接添加或删除元素。其实为什么不能改变容器大小的原因也很简单，因为它们不是成员函数，无法直接访问容器封装的底层数据。但是对于链表的成员函数来说，它们有权限访问链表的底层数据，因此链表的操作往往会改变底层数据来提高操作效率。诸如 `splice`，`merge`，`unique` 都会改变底层容器，例如 `merge` 和 `splice` 会移除链表节点。

### 9. 针对迭代器的泛型算法

对于链表类型，由于它的迭代器不是随机访问迭代器，我们不能通过递增和递减运算符移动迭代器，也不能对迭代器进行加减运算。这导致如果我们想获取一个元素中间位置的迭代器比较麻烦。例如我们想获取第三个位置的迭代器，我们不能 `c.begin() + 2`，而是要通过一个 for 循环将迭代器从 `c.begin()` 移动两步，这简直太麻烦了！

C++ 11 新标准引入了一些针对迭代器的算法：

``` c++
// C++11 之前就有的算法
template< class InputIt, class Distance >
void 
    advance( InputIt& it, Distance n ); // n可正可负，负数表示向后移动（仅对双向迭代器适用）

template< class InputIt >
typename std::iterator_traits<InputIt>::difference_type 
	distance( InputIt first, InputIt last );

// C++11 之后引入的算法
	// 相当于advance的n为负数（仅对双向迭代器适用）
template< class BidirIt >
constexpr BidirIt 
	prev( BidirIt it, typename std::iterator_traits<BidirIt>::difference_type n = 1 );

	// 相当于advance的n为整数
template< class InputIt >
constexpr InputIt 
    next( InputIt it, typename std::iterator_traits<InputIt>::difference_type n = 1 );
```

上面这些针对移动迭代器或计算两个迭代器之间距离的操作，对于随机访问迭代器的时间复杂度是 O(1)；对于其他可行迭代器的时间复杂度为 O(N)。

## 五、关联容器

``` c++
// ordered
map
set
multimap
multiset
// unordered
unordered_map
unordered_set
unordered_multimap
unordered_multiset
```

### 1. 迭代器

对关联容器解引用得到的是一个 `value_type` 类型的迭代器：

* 对于 `map` 来说，`value_type` 是一个 `pair<const Key, Value>`，因此我们不能修改 `map` 的关键字；
* 对于 `set` 来说，虽然 `value_type` 是 `Key`，并且 `set` 同时定义了 `iterator` 和 `const_iterator`，但两种类型都只允许访问 `set` 中的元素。

`std::set` 的迭代器设计体现了**逻辑 constness**（即使是非 `const` 迭代器也不允许修改元素），这是其作为有序关联容器的核心约束。因为 `set` 依赖于红黑树的有序结构，如果允许通过迭代器修改键值，可能破坏树的排序不变式。事实上我们没有任何办法修改 `set` 中的元素。除非先删除它再插入我们想要的元素。

关联容器也有 `begin` 和 `end` 成员函数，并且可以像顺序容器一样遍历容器元素。而且关联容器的迭代器都是双向的，这意味着它支持 `++` 和 `--` 操作。 

### 2. 构造函数

关联容器的构造函数类型是比较丰富的。我们可以用范围迭代器来初始化关联容器，只要元素类型可以转换。也可以通过 `initializer_list` 进行初始化。并且可以指定 `Compare` 和 `Allocator`。具体的可以见 [cppreference](https://en.cppreference.com/w/cpp/container/set/set)。

### 3. 关键字类型的要求

#### 3.1 有序容器

对于有序容器，关键字类型必须提供元素比较的方法。默认情况下，标准库通过关键字类型的 `less<T>` 来比较两个元素。

有两种自定义比较函数的方法：

* 在 class 内部定义比较函数，此时定义 set 时无需显示指定比较函数

``` C++
struct Node {
    int val;
    bool operator<(const Node &x) const {
        return val < x.val;
    }
};

set<Node> s;
```

* 在 class 外部定义比较函数，此时定义 set 时需要传入比较对象

 ``` C++
struct Node {
    int val;
};

bool cmp(int a, int b) 
{
    return a < b;
}

// set<int, decltype(&cmp)> s(cmp);
	// decltype(&cmp) 指定了 compare 的类型
	// s(cmp) 传入具体的 compare 对象
set<int, decltype(cmp)*> s(cmp);
 ```

在解释第二种方法之前，先来看一下 STL 中 map 和 set 的定义：

``` c++
// map
template<typename Key, typename Value, typename Compare = less<Key>, typename Alloc = alloc>
class map {
    typedef Key                     key_type;
    typedef Value                   mapped_type;
    typedef pair<const Key, Value>  value_type;	// const key
private:
    typedef rb_tree<key_type, value_type, select1st<value_type>, 
    					key_compare, Alloc> rep_type;
    rep_type t;
};

// set
template<typename Key, typename Compare = less<Key>, typename Alloc = alloc>
class set {
    typedef Key key_type;
    typedef Key value_type;
private:
    typedef rb_tree<key_type, identity<value_type>,
                    key_compare, Aloc> rep_type;
    rep_type t;
};
```

<font color=blue>《C++ Prime》中说：用来组织一个容器中元素的操作的类型也是该容器类型的一部分，因此说一般来说，用于比较的 Compare 和用于分配的 Alloc 都是容器类型的一部分。</font>

因此说，`decltype(cmp)*` 就是在指定比较函数的类型，注意这个类型应该是一个函数指针，因此这里需要加上一个 `*`，因为 `decltype` 作用于数组和函数时，不会自动发生数组和函数到指针的转换。当然也可以直接对函数取地址：`decltype(&cmp)`。

另外这里仅仅是指定了比较函数的类型，我们还需要将比较函数的对象（函数指针）传入容器的构造函数当中：`s(cmp)`。

#### 3.2 无序容器

对于无序容器，关键字类型必须提供哈希函数和 `operator==`。我们可以看一下具体的 class 定义：

``` c++
// unordered_set
template<
    class Key,
    class Hash      = std::hash<Key>,		// 哈希函数
    class KeyEqual  = std::equal_to<Key>,   // operator==
    class Allocator = std::allocator<Key>   // Alloc
> class unordered_set;
explicit unordered_set( 
    size_type        bucket_count,          // 桶的最少数量,初始化为0将由容器自动管理
	const Hash&      hash  = Hash(),
	const key_equal& equal = key_equal(),
	const Allocator& alloc = Allocator() 
);

// unordered_multiset
template<
    class Key,
    class Hash      = std::hash<Key>,
    class KeyEqual  = std::equal_to<Key>,
    class Allocator = std::allocator<Key>
> class unordered_multiset;
explicit unordered_map( 
    size_type        bucket_count,
    const Hash&      hash  = Hash(),
    const key_equal& equal = key_equal(),
    const Allocator& alloc = Allocator() 
);

// unordered_map
template<
    class Key,
    class Value,
    class Hash      = std::hash<Key>,
    class KeyEqual  = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, T>>
> class unordered_map;
explicit unordered_multimap( 
    size_type        bucket_count,
    const Hash&      hash  = Hash(),
    const key_equal& equal = key_equal(),
    const Allocator& alloc = Allocator() 
);

// unordered_multimap
template<
    class Key,
    class Value,
    class Hash      = std::hash<Key>,
    class KeyEqual  = std::equal_to<Key>,
    class Allocator = std::allocator<std::pair<const Key, T>>
> class unordered_multimap;
explicit unordered_multiset( 
    size_type        bucket_count,
    const Hash&      hash = Hash(),
    const key_equal& equal = key_equal(),
    const Allocator& alloc = Allocator() 
);
```

和有序容器一样，我们也有多种方法来实现自定义类型的哈希函数和 `operator==`。

对于比较函数，我们可以在 class 内部定义，也可以在 class 外部定义。当在 class 外部定义时，我们也需要指定函数指针的类型并传入该对象。

对于哈希函数（哈希仿函数模板），我们可以在 `std` 命名空间中偏特化 `hash` 模板，也可以在外部定义一个哈希函数。

* class 内的 `operator==` 函数和偏特化 `hash` 模板:

``` c++
struct Node {
    int val;
    bool operator==(const Node &x) const {
        return val == x.val;
    }
};

// 偏特化的hash模板需要放在std命名空间
namespace std {
    template<>
    struct hash<Node> {
        size_t operator()(const Node &x) const {
            return hash<int>()(x.val);
        }
    };
}

int main()
{
    unordered_set<Node> ass;
    for(int i = 0; i < 10; i ++ )
        ass.insert({i});
    cout << ass.bucket_count() << endl;    // 13
    return 0;
}
```

* class 外部的 `operator==` 和哈希函数:

``` c++
struct Node {
    int val;
};

bool is_equal(const Node &lhs, const Node &rhs)
{
    return lhs.val == rhs.val;
}

size_t hasher(const Node &x)
{
    return std::hash<int>()(x.val);
}

int main()
{
    using node_unordered_set = unordered_set<Node, 
                decltype(&hasher), 
                decltype(&is_equal)>; 
    // unordered_set第一个参数是桶的数量,传入0让容器自动管理
    node_unordered_set ass(0, hasher, is_equal);

    for(int i = 0; i < 10; i ++ )
        ass.insert({i});
    cout << ass.bucket_count();
    return 0;
}
```

### 5. 泛型算法

我们通常不对关联容器使用泛型算法。关键字是 const 意味着不能将关联容器传递给修改或重排容器元素的算法。

关联容器可用于只读元素的算法，但是，很多这类算法都要遍历容器。而泛型算法不知道关联容器的内部结构（如 `std::set` 是有序的，`std::unordered_set` 是哈希存储），因此**直接使用泛型算法（如 `std::find`）进行搜索通常是低效的，甚至是错误的做法**。不过关联容器一般是实现了自己的搜索成员函数，例如关联容器专用的 find 成员函数就比泛型的 find 函数快得多，泛型 find 函数会进行顺序搜索。

该设计体现了 STL 的核心思想：**算法与数据结构严格匹配**。关联容器通过专属方法暴露其内部优化，而泛型算法仅作为通用后备方案。

### 6. 添加元素

``` c++
c.insert(v);
c.emplace(args);

c.insert(b,e);	// 返回void
c.insert(il);	// 返回void

c.insert(pos,v);	   // pos(position_hint)：一个迭代器，提示新元素可能的插入位置。
c.emplace(pos,args);   // 具体的实现可以忽略此提示
```

在向关联容器插入元素时，提供**位置提示（hint）**可以优化插入效率。**标准不强制要求必须准确**，但正确的提示可减少红黑树的旋转操作。

- 如果提示正确（即新元素应紧邻 `position_hint` 之后插入），时间复杂度可降至 **均摊 `O(1)`**。
- 如果提示错误，退化为普通插入（`O(log n)`），**不影响正确性**。

由于 map 和 set（以及对应的无序类型）包含不重复的关键字，因此插入一个已存在的元素对容器没有任何影响，即该插入“失败”了。特别的，对于 map 来说，如果插入的键已经不在，该 key 对应的 value 值不会改变：

``` cpp
map<int,int> ass;
ass.insert({0, 1});
ass.insert({0, 2});
cout << ass[0] << endl; // 1
```

insert（或 emplace）返回的值依赖于容器类型和参数，对于传入一个迭代器或者一个 initializer_list 的 insert 版本，返回 void。否则：

* 如果容器类型是 non-multi 版本：返回 `pair<iterator,bool>`，其中 iterator 是指向插入元素的迭代器；bool 是标志插入是否成功
* 如果容器类型是 multi 版本：返回指向插入元素的迭代器。multi 版本可以插入重复的关键字，因此总是插入成功，无需返回 bool 值

### 7. 删除元素

``` C++
c.erase(key); // 根据关键字key删除，返回实际删除的元素数量
c.erase(p);	  // 返回一个指向迭代器p之后元素的迭代器，p不能为end	
c.erase(b,e); // 删除[b,e)，返回e
```

### 8. map的下标操作

map 又称为关联数组，即通过关键字而不是位置来索引的数组。

map 和 unordered_map 容器提供了下标运算符和一个对应的 at 成员函数。set 不支持下标，因为下标是“获取与一个关键字相关联的值”的操作，而 set 中关键字就是值。

与其它下标运算符不同的是，**如果关键字不在 map 中，会为它创建一个元素并插入到 map 中，关联值会进行值初始化。**例如，如果我们编写下面代码：

``` c++
map<string, int> word_count;
word_count["hello"] = 1;
```

对于第二条语句，将会执行如下操作：

* 在 `word_count` 中搜索关键字为 `"hello"` 的元素，未找到
* 将 `pair<string,int>{"hello",int()}` 插入到 `word_count`
* 提取出新插入的元素，并将值 1 赋予它

这就是说，对于语句 `word_count["hello"] = 1;` ，它实际上是先进行了一次插入（初始化），又进行了一次查找+赋值。

**由于下标运算符会自动插入元素，因此对于 const 类型的 map，无法使用下标运算符。**

我们可以使用 at 成员函数来避免下标运算符自动插入元素的问题，当关键字不存在时，at 会抛出 out_of_range 异常。另外也可以使用 find 成员函数。

另外一个与其它下标运算符不同的点就是，map 的下标运算符和迭代器解引用返回类型不相同。下标返回 mapped_type；迭代器解引用返回 value_type。

还有就是，**`std::multimap` 不支持 `operator[]` 访问**，因为 `std::multimap` 允许多个元素拥有相同 key（即 `{0,1}` 和 `{0,2}` 可以共存），因此无法像 `std::map` 那样用 `operator[]` 唯一映射一个值。下面是一些访问 `multimap` 的方法：

``` C++
// 打印map中所有值为key的元素的value 
multimap<string, int> ass = {
    {"a1", 1}, {"a2", 2}, {"b1", 3},
    {"b1", 4}, {"b1", 5}, {"b2", 6},     
};

int main() 
{
    // (1) find + count
    {
        auto start = ass.find("b1");
        auto count = ass.count("b1");
        while(count -- ) {
            cout << start ++ ->second << ' ';
        }
        cout << endl;
    }
    
    // (2) lower_bound + upper_bound
    {
        for(auto begin = ass.lower_bound("b1"), 
            end = ass.upper_bound("b1"); begin != end; ++ begin) {
            cout << begin->second << ' ';
        }
        cout << endl;
    }
    
    // (3) equal_range
    {
        for(auto pos = ass.equal_range("b1"); 
            pos.first != pos.second; pos.first ++ ) {
            cout << pos.first->second << ' ';
        }
        cout << endl;
    }
    return 0;
}
```

### 9. 访问元素

``` C++
// []只适用于非const的map和unordered_map（不包括multimap,unordered_multimap）
// at只适用于map和unordered_map
c[key];
c.at(key);

c.find(key);// 返回迭代器，若找不到返回end
			// 对于multi版本，返回第一个关键字为key的元素的迭代器

c.count(key);

// lower_bound和upper_bound不适用于无序容器
c.lower_bound(key); // 第一个>=key的元素
c.upper_bound(key);	// 第一个>key的元素
c.equal_range(key);	// 返回一个迭代器对，表示关键字等于key的元素的范围（前闭后开）
					// 若key不存在，返回两个end
```

其中，`equal_range` 返回的迭代器对就等价于 `pair{c.lower_bound(key), c.upper_bound(key)}`。

### 10. 小练习

``` C++
map<string, string> buildMap(ifstream &mapped)
{
    map<string, string> word_map;
    string line;
    while(getline(mapped, line))
    {
        string key, value;
        istringstream s(line);
        s >> key;
        getline(s, value);
        // cout << "size: " << value.size() << endl;
        // cout << "value: " << value << endl;
        if(value.size() > 1)
            word_map.insert({key, value.substr(1)});
        else 
            throw(runtime_error("no rule for " + key));
    }
    return word_map;
}

string transform(string word, const map<string,string>& word_map)
{
    auto it = word_map.find(word);
    if(it == word_map.end())
        return word;
    return it->second;
}

// mapped储存了K-V形式的映射关系
// input是输入
void word_transform(ifstream &mapped, ifstream &input)
{
    auto word_map = buildMap(mapped);
    
    string line;
    while(getline(input, line))
    {
        istringstream s(line);
        string word;
        bool firstword = true;
        while(s >> word)
        {
            if(firstword)
                firstword = false;
            else 
                cout << " ";
            cout << transform(word, word_map);
        }
        cout << endl;
    }
}

int main()
{
    ifstream mapped_file("mapped.txt");
    ifstream input("input.txt");
    word_transform(mapped_file, input);
    return 0;
}
```

### 11. 无序容器

无序容器不再使用比较运算符组织元素，而是使用一个哈希函数（将给定类型的值映射到整形 `size_t` 值的函数）和关键字类型的 `==` 运算符来比较元素。对于关键字可重复的无序容器，相同关键字被组织在同一个桶中。

无序容器还用一个 `hash<key_type>` 类型的对象来生成每个元素的哈希值。标准库为内置类型（包括指针）提供了 `hash` 模板。还为一些标准库类型（`string`、智能指针）定义了 `hash` 模板。如果我们关键字的类型是未提供 `hash` 模板的类型或自定义类型，则必须提供我们自己的 `hash` 模板版本。

> C++ 中，`hash` 是一个仿函数模板。我们可以通过**模板偏特化**来为自定义类型定义 `hash` 函数。不过要注意 `std::hash` 是定义在 `std` 命名空间中的，因此要特化它时，需要使用 **嵌套命名规范**。
>
> 在哈希模板中定义 `size_t operaotr()() const noexcept {}` 即可自定义我们的哈希函数了。

除了哈希管理操作之外，无序容器还提供了与有序容器相同的操作。这意味我们可以在无需容器和有序容器可以相互替换。

哈希表的底层数据结构是哈希桶数组（拉链法），C++ 提供了一组管理桶的函数。这些成员函数允许我们查询容器的状态以及在必要时强制容器进行重组：

``` c++
// 桶接口
bucket_count();	
max_bucket_count();     // 容器能容纳的最多的桶的数量
bucket_size(n);         // 第n个桶有多少元素
bucket(k);	            // 关键字为k的元素在那个桶

// 桶迭代
local_iterator;	        // 桶的迭代器，用于访问桶中元素
const_local_iterator;
begin(n), end(n);   	// 桶n的begin和end迭代器
cbegin(n), cend(n);

// 哈希策略
load_factor();	   // 每个桶的平均元素数量，返回float值

max_load_factor(); // 容器允许的最大负载因子阈值（默认值通常为 1.0）
				   // 当load_factor > max_load_factor 时，容器会自动扩容（rehash）
				   // 通过增加桶数量以降低负载因子，使load_factor<=max_load_factor

max_load_factor(float ml); // 设置新的最大负载因子阈值（触发条件可能立即生效或延迟到下次插入操作）

rehash(n);  // 强制将哈希表的桶数量调整为至少n个，使bucket_count>=max(n,size/max_load_factor)
reserve(n); // 保证哈希表能容纳至少n个元素而不触发自动扩容,bucket_count>=size/max_load_factor
```

### 12. rehash 和 reserve

#### **12.1 `rehash(n)`：直接控制桶数量**

**作用**

* 强制将哈希表的桶数量调整为至少 `n` 个，同时满足：

$$
bucket\_count≥max⁡(n,\frac{size()}{max\_load\_factor})
$$

**典型用途**

- 明确知道需要多少桶时（如从数据特征预计算得出）
- 手动优化哈希冲突（桶数量直接影响冲突率）

**示例**

```cpp
std::unordered_map<int, std::string> umap;
umap.max_load_factor(0.7f);
umap.rehash(100);  // 强制桶数量≥100

// 验证
std::cout << "Buckets: " << umap.bucket_count();  // 103
```

**底层行为**

1. 分配新的桶数组（大小≥`n` 的<font color=blue>**质数**</font>）
2. 将所有元素重新哈希到新桶中
3. 迭代器/引用可能失效（除非无扩容）

#### **12.2 `reserve(n)`：预分配元素空间**

**作用**

* 保证哈希表能容纳至少 `n` 个元素而**不触发自动扩容**，实际桶数量满足：

$$
bucket\_count≥\frac{n}{max\_load\_factor}
$$

**典型用途**

- 已知要插入的元素数量时（避免多次扩容）
- 性能敏感场景（减少动态扩容开销）

**示例**

```cpp
std::unordered_set<int> uset;
uset.max_load_factor(0.5f);
uset.reserve(50);  // 预分配空间以容纳50个元素

// 计算实际桶数量下限
// 桶数 ≥ 50 / 0.5 = 100
std::cout << "Buckets: " << uset.bucket_count();  // 103
```

**底层行为**

1. 计算所需最小桶数量：`ceil(n / max_load_factor)`
2. 调用 `rehash(calculated_buckets)`

#### **12.3 关键对比**

| 特性             | `rehash(n)`                          | `reserve(n)`                         |
| :--------------- | :----------------------------------- | :----------------------------------- |
| **参数意义**     | 直接指定桶数量下限                   | 指定元素数量下限                     |
| **计算公式**     | `bucket_count ≥ max(n, size/max_lf)` | `bucket_count ≥ n / max_load_factor` |
| **适用场景**     | 需要精确控制桶数量时                 | 已知元素数量时                       |
| **性能影响**     | 更底层，更灵活                       | 更高层，更直观                       |
| **典型调用时机** | 数据加载后、高频操作前               | 批量插入前                           |

#### **12.4 使用建议**

* 优先使用 `reserve` 的情况**

```cpp
// 场景：准备插入1000个元素
std::unordered_map<int, Data> map;
map.reserve(1000);  // 一次性分配足够空间

for (int i = 0; i < 1000; ++i) {
    map.insert({i, generate_data()});  // 避免插入时多次扩容
}
```

* 需要手动 `rehash` 的情况**

```cpp
// 场景：发现哈希冲突严重
std::unordered_set<std::string> heavy_conflict_set;

// 插入数据后检测冲突
if (has_high_collision(heavy_conflict_set)) {
    // 将桶数量翻倍
    heavy_conflict_set.rehash(heavy_conflict_set.bucket_count() * 2);
}
```

#### **12.5. 性能优化技巧**

1. **结合 `max_load_factor` 调整**

   ```cpp
   umap.max_load_factor(0.5f);  // 更激进的空间换时间
   umap.reserve(1000);          // 按新负载因子预分配
   ```

2. **避免多次增量扩容**

   - 默认扩容策略可能导致多次 `rehash`
   - 提前 `reserve` 可减少总操作时间

3. **实时监控负载状态**

   ```cpp
   if (umap.load_factor() > 0.8 * umap.max_load_factor()) {
       umap.rehash(umap.bucket_count() * 1.5);  // 主动扩容
   }
   ```

#### **12.6 底层实现差异**

| 操作          | 可能触发的行为                                           |
| :------------ | :------------------------------------------------------- |
| **`rehash`**  | 立即重新分配桶并重新哈希所有元素（严格保证桶数量）       |
| **`reserve`** | 可能延迟到下次插入操作时执行（某些实现会优化为惰性操作） |

#### **12.7总结**

- **`rehash(n)`**：动态调整桶
- **`reserve(n)`**：预分配桶

正确选择二者可以显著提升无序容器的性能，尤其在数据规模已知或需要避免动态扩容的场景中。

## 六、动态内存

标准库在头文件 `memory` 中定义了两种类型的智能指针模板，它们对底层指针采取不同的管理方式：`shared_ptr` 共享底层指针，`unique_ptr` 独占底层指针。此外，标准库还定义了一个名为 `weak_ptr` 的<font color=blue>**伴随类**</font>，它是一种弱引用，指向 `shared_ptr` 所管理的对象。

> 在 C++ 中，**伴随类**(*Companion Class*)是指与另一个类紧密关联、共同工作的类。它们通常成对出现，相互协作完成特定功能。伴随类不是 C++ 语言正式定义的术语，而是一种常见的设计模式或编程实践。

### 1. 成员函数

`shared_ptr` 和 `unique_ptr` 都支持的操作：

``` c++
shared_ptr<T> sp; // 空智能指针，指向nullptr
unique_ptr<T> up;
p;	// 将p作为一个条件判断，若p指向一个对象则为true
*p;
p->memthod;
p.get(); // 返回p中保存的指针。要小心使用，若智能指针释放了其对象，返回的指针所指向的对象也就消失了
		 // 也即此时p.get()返回的指针将指向无效内存（Dangling Pointer）
swap(p, q);
p.swap(q);
```

`shared_ptr` 独有的操作：

```c++
make_shared<T>(args);
shared_ptr<T> p(q);
p = q;
p.unique();    // 若p.use_count()为1，返回true
p.use_count(); // 速度可能比较慢
```

### 2. *use_count*

#### 2.1 为什么 *use_count* 比较慢

##### 1. **原子操作（Atomic Operations）**

`shared_ptr` 的引用计数通常使用 `std::atomic`（原子变量）实现，以保证线程安全。

- **原子操作** 确保引用计数的修改（`++`/`--`）是**不可分割**的，不会出现数据竞争（data race）。
- 但原子操作比普通变量操作更慢，因为：
  - 它可能需要**锁总线（lock bus）** 或 **内存屏障（memory fence）**，防止 CPU 乱序执行。
  - 它可能阻止 CPU 的**缓存优化**（见下文）。

> #### **1. 锁总线（Lock Bus）**
>
> ##### **定义**
>
> 锁总线是一种**硬件级别的同步机制**，当一个 CPU 核心需要执行原子操作（如`std::atomic`的修改）时，它会**锁定整个内存总线**，阻止其他 CPU 核心同时访问内存，直到操作完成。
>
> ##### **为什么需要锁总线？**
>
> - **防止多核竞争**：如果没有锁总线，两个 CPU 核心可能同时修改同一个内存位置，导致数据不一致。
> - **确保原子性**：例如，`x++`（读-改-写操作）在汇编层面可能是多个指令，锁总线确保这些指令作为一个整体执行。
>
> ##### **缺点**
>
> - **性能开销大**：锁总线期间，其他 CPU 核心无法访问内存，导致整个系统变慢。
> - **影响可扩展性**：随着 CPU 核心数增加，锁总线会成为瓶颈。
>
> ##### **现代 CPU 的优化**
>
> 现代 CPU（如 x86）通常使用**缓存一致性协议（如 MESI）**来减少锁总线的使用，但仍然在某些原子操作（如`LOCK XCHG`）中使用。
>
> ----
>
> #### **2. 内存屏障（Memory Fence / Memory Barrier）**
>
> ##### **定义**
>
> 内存屏障是一种 **CPU 指令**，用于控制指令执行顺序，确保：
>
> 1. **某些指令不会重排**（编译器和 CPU 都可能优化指令顺序）。
> 2. **内存访问对其他线程可见**（强制刷新 CPU 缓存）。
>
> ##### **为什么需要内存屏障？**
>
> 现代 CPU 和编译器会**乱序执行（Out-of-Order Execution）**以提高性能，但在多线程环境下，这可能导致问题。例如：
>
> ```cpp
> // 线程1
> x = 1;           // (1)
> ready = true;    // (2)
> 
> // 线程2
> while (!ready); // (3)
> std::cout << x; // (4)
> ```
>
> 如果没有内存屏障，CPU 或编译器可能重排  (1) 和  (2)，导致线程 2 看到 `ready == true` 但  `x` 仍然是旧值。
>
> ### **内存屏障的类型**
>
> | 屏障类型                | 作用                                                         |
> | :---------------------- | :----------------------------------------------------------- |
> | **Load-Load Barrier**   | 确保屏障前的读操作先于屏障后的读操作完成                     |
> | **Store-Store Barrier** | 确保屏障前的写操作先于屏障后的写操作完成                     |
> | **Load-Store Barrier**  | 确保屏障前的读操作先于屏障后的写操作完成                     |
> | **Store-Load Barrier**  | 确保屏障前的写操作先于屏障后的读操作完成（最强屏障，开销最大） |
>
> ### **C++中的内存屏障**
>
> 在 C++中，内存屏障通过 `std::atomic` 和内存序（Memory Order）控制：
>
> ```cpp
> std::atomic<int> x;
> x.store(1, std::memory_order_release);       // Store-Store 屏障
> int val = x.load(std::memory_order_acquire); // Load-Load 屏障
> ```
>
> ### **常见内存序（Memory Order）**
>
> | 内存序                 | 作用                           | 屏障强度                 |
> | :--------------------- | :----------------------------- | :----------------------- |
> | `memory_order_relaxed` | 无同步，仅保证原子性           | 无屏障                   |
> | `memory_order_acquire` | 确保之后的读操作不会重排到前面 | Load-Load + Load-Store   |
> | `memory_order_release` | 确保之前的写操作不会重排到后面 | Store-Store + Load-Store |
> | `memory_order_seq_cst` | 完全顺序一致性（默认）         | Store-Load（最强屏障）   |

##### 2. **CPU 缓存 vs. 主内存**

现代 CPU 有**多级缓存（L1/L2/L3）**，访问缓存比访问主内存快得多。

- **普通变量** 通常会被 CPU 缓存，减少访问主内存的次数。
- **原子变量**（如 `shared_ptr` 的引用计数）由于需要保证多线程一致性，可能**强制从主内存读取最新值**，而不是使用 CPU 缓存中的旧值（即使该值在当前线程中未改变）。
  - 这称为 **缓存一致性协议（Cache Coherence）**，例如 MESI（Modified/Exclusive/Shared/Invalid）。
  - 每次 `use_count()` 可能需要**同步缓存**，导致额外开销。

##### 3. **多线程竞争**

如果多个线程频繁修改 `shared_ptr`（如拷贝/析构），`use_count()` 可能需要等待：

- **缓存行竞争（False Sharing）**：如果引用计数和其他变量共享同一个缓存行（通常 64 字节），即使只修改引用计数，也会导致其他 CPU 核心的缓存失效，增加同步开销。

#### **2.2 示例说明**

```cpp
std::shared_ptr<int> p = std::make_shared<int>(42);

// 线程1：
auto count1 = p.use_count(); // 可能触发主内存访问（较慢）

// 线程2：
auto p2 = p; // 增加引用计数（原子操作，可能影响线程1的 use_count()）
```

- 如果线程2 修改了引用计数，线程1 的 `use_count()` 必须读取最新的值，不能使用缓存中的旧值，因此可能需要访问主内存。

#### **2.3 如何优化？**

1. **避免频繁调用 `use_count()`**

   - 它通常用于调试或特殊逻辑，不应在性能关键代码中频繁使用。

2. **改用 `std::weak_ptr::lock()` 检查对象存活**

   - 比 `use_count() > 0` 更高效，且更安全：

     ```cpp
     std::weak_ptr<int> wp = p;
     if (auto sp = wp.lock()) { // 比 use_count() 更推荐
         // 对象仍然存在
     }
     ```

3. **考虑 `std::unique_ptr`（如果不需共享所有权）**

   - 避免引用计数的开销。

4. **使用侵入式智能指针（如 `boost::intrusive_ptr`）**

   - 引用计数直接存储在对象内部，可能减少原子操作开销。

#### **2.4 总结**

- `use_count()` 较慢的原因是：
  - **原子操作**（线程安全保证）。
  - **缓存同步**（可能强制访问主内存）。
  - **多线程竞争**（缓存行失效）。
- **优化建议**：减少调用次数，改用 `weak_ptr::lock()`，或选择更高效的所有权模型。



#### 2.5 *lock*

`wp.lock()` 是 `std::weak_ptr` 的成员函数，用于安全地尝试获取一个 `std::shared_ptr`。它是比直接使用 `use_count()` 更推荐的检查智能指针有效性的方式。

##### 1. 基本用法

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(42);
std::weak_ptr<int>   wp = sp;  // 从shared_ptr创建weak_ptr

// 尝试获取shared_ptr
if (auto locked = wp.lock()) {
    // 对象仍然存在，可以使用locked
    std::cout << *locked << std::endl;
} else {
    // 对象已被释放
    std::cout << "对象已释放" << std::endl;
}
```

##### 2. 为什么比 `use_count()` 更好？

1. `use_count() > 0` **不能保证**对象仍然存活，因为：
   - 检查 (`use_count()`) 和提升 (`lock()`) 是**两步操作**，中间可能被其他线程修改。
   - 即使 `use_count() == 1`，另一个线程可能刚好在检查后释放对象。
2. `lock()` 只需要检查对象是否有效（强引用计数是否大于 0），而不需要获取具体的引用计数。`use_count()` 返回当前 `shared_ptr` 的强引用计数的精确值。这个值可能被多个线程同时修改，因此读取它需要严格的同步（通常是原子加载）。
3. 标准库实现通常会优化这个操作

##### 3. 典型使用场景

1. **缓存系统**：

```cpp
std::weak_ptr<CacheEntry> cached;

auto getData() {
    if (auto entry = cached.lock()) {
        return entry;  // 缓存命中
    }
    auto entry = std::make_shared<CacheEntry>(loadData());
    cached = entry;
    return entry;  // 缓存未命中，创建新条目
}
```

2. **观察者模式**：

```cpp
class Observer {
    std::weak_ptr<Subject> subject;
public:
    void notify() {
        if (auto s = subject.lock()) {
            s->update();
        }
    }
};
```

3. **打破循环引用**：

```cpp
struct Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;  // 使用weak_ptr避免循环引用
};
```

### 3. 控制块

典型的 `shared_ptr` 实现包含两个主要部分：

1. **控制块**：存储引用计数、删除器等元数据
2. **原始指针**：指向实际管理的对象

```cpp
template<typename T>
class shared_ptr {
    T* ptr;                // 管理的原始指针
    control_block* ctrl;   // 控制块指针
};
```

#### 3.1 控制块设计

控制块通常包含：

```cpp
struct control_block {
    long shared_count;     // 共享引用计数
    long weak_count;       // weak_ptr 引用计数
    Deleter deleter;       // 删除器对象
    Allocator allocator;   // 分配器对象
    // 其他元数据...
};
```

#### 3.2 关键操作实现

##### 1. 构造函数

```cpp
template<typename T>
shared_ptr<T>::shared_ptr(T* p) : ptr(p), ctrl(new control_block) {
    if (p) {
        ctrl->shared_count = 1;
    } else {
        ctrl->shared_count = 0;
    }
}
```

##### 2. 拷贝构造函数

```cpp
template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr& other) 
    : ptr(other.ptr), ctrl(other.ctrl) {
    if (ctrl) {
        ++ctrl->shared_count;
    }
}
```

##### 3. 析构函数

```cpp
template<typename T>
shared_ptr<T>::~shared_ptr() {
    if (ctrl) {
        --ctrl->shared_count;
        if (ctrl->shared_count == 0) {
            // 删除管理对象
            deleter(ptr);
            // 如果 weak_count 也为0，删除控制块
            if (ctrl->weak_count == 0) {
                delete ctrl;
            }
        }
    }
}
```

#### 3.3 线程安全性

`shared_ptr` 的线程安全性保证：

1. 引用计数的修改是原子操作（通常使用原子指令或互斥锁）
2. 多个线程可以同时拷贝/销毁不同的 `shared_ptr` 实例
3. 对同一对象的访问需要外部同步

#### 3.4 性能考虑

1. **内存开销**：每个 `shared_ptr` 需要额外的控制块内存
2. **时间开销**：引用计数的原子操作带来额外开销
3. **缓存局部性**：控制块和对象可能不在同一缓存行

#### 3.5 使用注意事项

1. 避免循环引用（应使用 `weak_ptr` 打破循环）
2. 不要从原始指针创建多个独立的 `shared_ptr`
3. 优先使用 `make_shared` 以提高效率
4. 注意多线程环境下的数据竞争问题

#### 3.6 现代优化 - make_shared

`make_shared` 通常将对象和控制块分配在**连续内存**中，提高局部性并减少内存分配次数：

```cpp
template<typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args) {
    // 分配组合的内存块，包含控制块和对象空间
    auto block = allocate_shared_block<T>(sizeof(T), sizeof(control_block));
    
    // 在对象位置构造 T
    new (block->object) T(std::forward<Args>(args)...);
    
    // 构造 shared_ptr
    return shared_ptr<T>(block);
}
```

这种设计体现了 C++ 资源管理的现代理念，兼顾了安全性和效率。

### 4. 直接管理内存

相较于使用智能指针，当我们直接使用管理内存的类时，不能依赖类对象拷贝、赋值和销毁操作的任何默认定义。

默认情况下，动态分配的对象是默认初始化的，这意味着内置类型或组合类型的对象的值是未定义的，而类类型对象使用默认构造函数进行初始化。因此，出于与变量初始化相同的原因，对动态分配的对象进行初始化通常是个好主意。

类似于其它任何 const 对象，一个动态分配的 const 对象必须进行初始化。对于有默认构造函数的类类型来说，可以隐式初始化。

``` c++
const int *p = new const int(10);
```

通常情况下，编译器不能分辨一个指针指向的是静态还是动态分配的对象。类似的，编译器也不能分辨一个指针所指向的内存是否已经被释放了。当我们 delete 掉一个指向静态对象或重复 delete 时，编译器并不会报错。

> 使用 new 和 delete 管理动态内存存在三个常见问题：
>
> 1. 忘记 delete 内存
> 2. 同一块内存释放两次
> 3. 使用已经释放掉的对象

当我们 delete 掉一个指针之后，指针值就变得无效了，但在很多机器上指针仍然保存着已经释放了的动态内存的地址。此时的指针就是所谓的空悬指针（dangling pointer），即，只想一块已经被释放了的内存的指针。

未初始化指针的所有缺点空悬指针也有。有一种办法可以避免空悬指针，那就是在 delete 掉动态内存之后，将指针的值设为 NULL。但这也只是提供了有限的保护，因为实际应用中可能有多个指针指向同一块动态内存，那么我们在 delete 掉这个动态内存之后，总不能手动将所有指针的值置位 NULL 吧。查找所有指向同一块动态内存的指针可不是一件容易的事情。

### 5. *shared_ptr* 

``` c++
shared_ptr<T> p(q);	   // q可以是shared_ptr和内置指针
shared_ptr<T> p(q, d); // d是我们指定的delete函数

p.reset();	   // p置为空
p.reset(q);    // p置为q
p.reset(q,d);  // p置为q，并指定delete函数

make_shared<T>();
```

前面提到过，如果我们不初始化一个智能指针，它就会被初始化为空指针。除了使用 make_shared 初始化外，还可以使用 new 来初始化。

``` C++
shared_ptr<int> p(new int(10));     // ctor
shared_ptr<int> p = new int(10);	// copy ctor
```

不过要注意上面第二种写法是错误的，因为我们**不能进行内置指针到智能指针间的隐式转换**。

注意不能直接将 unique_ptr 赋值给 shared_ptr，应该使用 move：

``` C++
unique_ptr<Foo> u(new Foo(10));
shared_ptr<Foo> s = move(u);
if(u == nullptr)    
    cout << "yes";	// yes
```

尽管 shared_ptr 可以自动销毁，但这并不意味着它就可以避免内存浪费。例如我们将 shared_ptr 存放于一个容器中，而后不再需要全部元素，只使用其中的一部分，要记得用 erase 删除不需要的元素。

### 6. 不要混合使用普通指针和智能指针

对于下面的代码：

``` c++
int *p = new int(10);
int *q = new int(20);
shared_ptr<int> sq(p);
shared_ptr<int> uq(q);
cout << sq.use_count() << endl; // 1
cout << uq.use_count() << endl; // 1
```

我们说这是一种不好的写法：

* 对于 shared_ptr 来说，如果它被销毁了，那么 p 就成了一个悬空指针，而且我们无法得知何时对象会被 delete 掉，因为在某些机器中，即使对象被 delete 掉，指针也仍然指向原来对象所处地址。
* 对于 unique_ptr 来说，就更糟糕了，因为 unique_ptr 的本意是独占对象，但实际上我们可以通过 q 来修改对象。同样的，unique_ptr 也存在空悬指针的问题。

并且如果我们无意中 delete 掉了底层指针（p 和 q），那么智能指针再 delete 时就会两次 delete 错误。

例如下面的代码，我们会在不经意间创造空悬指针：

``` c++
void process(shared_ptr<int> q) {}

int main()
{
    int *p = new int(10);
    process(shared_ptr<int>(p)); // p所指向对象被释放
    delete p; // double delete
    return 0;
}
```

在上面的代码中，由于我们无法将 `int*` 转换为 `shared_ptr` 类型，因此我们传入一个匿名 `shared_ptr` 对象，但这将导致 `p` 成为一个空悬指针。对于代码 `process(shared_ptr<int>(p));`，由于我们将 `p` 传给了智能指针，就相当于我们让智能指针来接管指针对象，但是后续我们又 `delete p;` ，即在将内置指针托付给智能指针使用之后，又使用内置指针来访问对象，这是不合法的，也是容易犯的错误。

因此说，不要用内置指针初始化智能指针，特别是先动态创建一个内置指针，再用来初始化智能指针。这是一种很不好的写法。推荐的写法是通过工厂函数 make_shared 和 make_unique 来初始化智能指针，或者直接用 new 来初始化。

### 7. *get*

不要使用 `get` 初始化另一个智能指针或为智能指针赋值。**`get` 的设计只为针对一种情况：向不能使用智能指针的代码传递一个内置指针。**并且使用 `get` 返回指针的代码不能 `delete` 此指针。

为什么标准库这样设计：

1. **明确所有权边界**：`get()` 明确表示"借出"指针，不转移所有权
2. **避免隐式所有权转换**：防止意外创建多个独立的所有权控制块
3. **保持资源管理的清晰性**：所有权的传递必须显式通过智能指针接口进行

记住：**`get()` 只用于*临时*将指针"借给"不理解智能指针的代码，绝不应用于所有权相关的操作**。这是 C++ 资源管理模型中的一个重要安全边界。

### 8. *deleter*

前面我们提到过异常安全的代码可以确保异常发生后资源能被正确的释放，而一个简单的确保资源被正确释放的方法就是使用智能指针。智能指针默认执行 delete 操作，对于类通过析构函数清理对象使用的资源。

但是并不是所有的类都定义了析构函数，特别是那些为 C 和 C++ 两种语言设计的类，通常都要求用户显示的释放所使用的任何资源。这个时候我们可以在智能指针中显式指定删除器（删除函数）。要注意删除器的格式有要求：

``` c++
// 返回类型为void
// 参数类型为指向对象类型的指针
// 实际上就等价于类的析构函数，这个指针就是this指针
void deleter(T *ptr) {
    // ...
}
```

例如下面的例子，假设我们有一个网络库，通过 connect 返回连接对象，最后需要调用 disconnect 来清理资源，我们把清理资源的步骤托管给智能指针：

``` c++
class connection {};

connection connect()
{  
    connection c;
    // ...
    return c;
}

void disconnect(connection c)
{
    // ...
    cout << "disconnect" << endl;
}

// 包装disconnect适配deleter的格式
void end_connection(connection *c)
{
    disconnect(*c);
}

void process()
{
    connection c = connect();

    // (1) lambda
    shared_ptr<connection> p(&c, [](connection *c){
        disconnect(*c);
    });

    // (2) function
    // shared_ptr<connection> q(&c, end_connection);
}
```

### 9. 智能指针使用规范

1. 不使用相同的内置指针初始化（或 reset）多个智能指针
2. 不 delete get() 返回的指针
3. 当你使用 get() 返回的指针时，注意最后一个智能指针被销毁后，该内置指针变为无效
4. 如果你使用的智能指针管理的资源不是 new 分配的内存，记得传递给他一个删除器
5. 不要混合使用智能指针和内置指针。如果不得不需要先定义内置指针再用智能指针管理内置指针，那么内置指针往后便不要再使用了

对于第一点，有一个可能忽视的错误：

``` c++
struct Foo {
    ~Foo() {
        cout << "~Foo" << endl;
    }
};

Foo *f = new Foo();

void f1()
{
    shared_ptr<Foo> sp1(f);
    shared_ptr<Foo> sp2(f);
    cout << sp1.use_count() << ',' << sp2.use_count() << endl; // 1,1 
}
```

对于上面的代码，使用 f 分别初始化了 sp1 和 sp2 两个智能指针，但要注意虽然这两个智能指针都以 f 初始化，但它们并不共享引用计数。这意味着他们的引用计数都是 1，因此在函数结束之后都会执行 delete，所以此时 f 会被 double delete。

### 10. *unique_ptr*

``` C++
unique_ptr<T> u;
unique_ptr<T,D> u;
unique_ptr<T,D> u(d);

u = nullptr;
u;
*u;
u->mem;

u.release()； // 返回底层指针并释放对底层对象的所有权（u置为空）
u.get();
    
u.reset();
u.reset(q);
u.reset(nullptr);

p.swap(q);
swap(p, q);
```

可以发现一个与 shared_ptr 显著不同的点，那就是如果我们想要指定删除器，需要同时在模板中指定删除器的类型。

``` c++
void deleter(int *p)
{
    cout << "delete int" << endl;
    delete p;
}

unique_ptr<int, decltype(&deleter)> q(new int(10), deleter);
```

另外就是 `release` 和 `get` 本质上都是返回底层指针，但 `release` 会释放对底层指针的所有权，而 `get` 只是借出底层指针的使用权。

虽然我们不能拷贝或赋值 `unique_ptr`，但可以通过调用 `release` 或 `reset` 将指针的所有权从一个（非 const）`unique_ptr` 转移给另一个 `unique_ptr`。

不过不能拷贝` unique_ptr` 的规则有一个例外：我们可以拷贝或赋值一个将要被销毁的 `unique_ptr`。最常见的例子是从函数返回一个` unique_ptr`：

``` C++
template<typename T>
unique_ptr<T> ret_unique(const T &val)
{
    return unique_ptr<T>(new T(val));
}
```

### 11. *weak_ptr*

``` cpp
weak_ptr<T> w;     // 空指针
weak_ptr<T> w(sp); // 绑定到shared_ptr
w = p;	           // p可以是shared_ptr也可以是weak_ptr

w.reset();         // 将w置为空
w.use_count();
w.expired(); // 若use_count为0返回true，否则返回false
w.lock();	 // 如果expired为true，返回空shared_ptr，否则返回一个指向w的对象的shared_ptr
```

`weak_ptr` 是一种不控制所指向对象生存期的智能指针，它指向一个 `shared_ptr` 管理的对象。**将一个 `weak_ptr` 绑定到一个 `shared_ptr` 不会改变 `shared_ptr` 的引用计数。**

由于 `weak_ptr` 指向的对象可能被不知不觉的释放，因此我们不能使用 `weak_ptr` 直接访问对象，而必须调用 `lock`，此函数会检查 `weak_ptr` 指向的对象是否仍然存在：

``` c++
shared_ptr<int> sp(new int(10));
weak_ptr<int> wp(sp);
if(wp.lock()) {
    // ....
    cout << *wp.lock() << endl;
}
```

### 12. 动态数组

大多数应用应该使用标准库容器而不是动态分配的数组。使用容器更为简单、更不容易出现内存管理错误并且可能有更好的性能。

####  12.1 *array new*

所谓的动态数组，实际上就是 _array new_：`T *p = new T[size];` 其中方括号中的 `size` 必须是个整数，但不必是常量，因为这并不是编译时行为。

虽然我们通常称 `new T[]` 分配的内存为“动态数组”，但这种叫法某种程度上有些误导。<font color=blue>当用 new 分配一个数组时，我们并未得到一个数组类型的对象，而是得到一个数组元素类型的指针（指向数组首元素的指针）。</font>而由于分配的内存并不是一个内置数组类型，所以我们不能对动态数组调用 `begin` 或 `end`，也不能用范围 `for` 循环来处理所谓动态数组中的元素：

``` c++
// 数组可以调用begin、end和范围for循环
int a[] = {1, 2, 3, 4, 5};
for(auto &x : a)
    cout << x << ' ';
cout << endl;
for(auto b = begin(a), e = end(a); b != e; b ++ )
    cout << *b << ' ';
cout << endl;
```

#### 12.2 *array delete*

_array new_ 正序构造元素，_array delete_ 逆序销毁元素：

``` C++
struct Foo {
    Foo(int _val = 0) : val(_val) { cout << "ctor: " << _val << endl; }
    ~Foo() { cout << "dtor: " << val << endl; }
    int val;
};

int main() 
{ 
    Foo *p = new Foo[3]{1, 2, 3};
    delete[] p;
    return 0;
}
// ctor: 1
// ctor: 2
// ctor: 3
// dtor: 3
// dtor: 2
// dtor: 1
```

对于 *array new* 分配的元素，如果我们没有使用 *array delete* 进行销毁而是使用普通的 *delete*，其行为是未定义的。

#### 12.3 初始化

默认情况下，`new` 分配的对象，无论是单个分配还是数组中的，都是默认初始化的。可以对数组中的元素进行值初始化，方法是在方括号之后跟一对圆括号或一对花括号：

``` c++
int *p = new int[5]{1, 2, 3, 4, 5};       // 1,2,3,4,5
int *q = new int[4]{1};                   // 1,0,0,0,0
vector<int> *v  = new vector<int>(3, 1);  // 1,1,1
vector<int> *v2 = new vector<int>{3, 1};  // 3,1
```

与内置数组对象的初始化一样，如果初始化列表元素个数少于数组大小，剩余元素会值初始化。

#### 12.4 零长数组

[*reference1*](https://zhuanlan.zhihu.com/p/573081617)

[*reference2*](https://gcc.gnu.org/onlinedocs/gcc/Zero-Length.html)

----

在 C/C++ 中，我们不能创建一个大小为 0 的静态数组对象，这样会导致编译错误。但是动态分配一个空数组是合法的：

``` c++
int *p = new int[0];	
if(p == nullptr)    puts("yes");
else    puts("no");
// no
delete[] p;
```

并且我们可以发现，`new` 返回的指针不指向 `nullptr`，也即，此时 `new T[0]` 返回一个合法的**非空指针**。

此指针与 `new` 返回的其他任何指针都不相同。它就像一个<font color=blue>**尾后指针**</font>一样，我们可以像尾后迭代器一样使用这个指针，例如加 0 或者减去 0、进行比较操作或与自身相减从而得到 0。但不能对该指针解引用——毕竟他不指向任何元素。另外，指针非空意味着我们可以对它执行 `delete[]` 操作。

这种行为的设计是为了**保证一致性和可预测性**。即使数组大小为 0，返回一个有效指针可以简化代码中的一些逻辑检查，比如在动态数组的使用中无需特别判断大小是否为 0，从而减少特殊情况的处理。

----

不过在实际测试中发现，声明一个大小为 0 的静态数组并不会导致编译错误（`g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`）。这似乎是因为：

*Declaring zero-length arrays is allowed in GNU C as an **extension**. A zero-length array can be useful as the **last element** of a structure that is really a header for a variable-length object.*  

也即可以将零长数组作为 *last struct member*，代替指针来实现动态数组的效果，这么做有两个好处：

1. 零长数组的不会额外占用 struct 的空间，而如果我们放入一个指针的话，就会额外占用一个指针的空间。
2. 零长数组可以在 malloc 时一次性分配内存，而指针需要一次额外分配（先分配 struct 对象，再分配指针所指向的动态数组）

例如：

``` C++
struct Array {
    int length;
    int arr[0]; // flexible array: int arr[];
};

struct ArrayWithPointer {
    int length;
    int *arr;
};

int main()
{
    cout << sizeof(Array) << endl;            // 4
    cout << sizeof(ArrayWithPointer) << endl; // 16，由于struct对齐的原因，不仅仅带来了指针的8B开销

    size_t length = 3;

    // 一次性分配
    Array *a1 = (Array*)malloc(sizeof(Array) + sizeof(int) * length); 

    // 两次分配
    ArrayWithPointer *a2 = (ArrayWithPointer*)malloc(sizeof(ArrayWithPointer));
    a2->arr = (int*) malloc(sizeof(int) * length);

    // 如果是 int arr[];
    // 报错：error: invalid application of ‘sizeof’ to incomplete type ‘int []’
    cout << sizeof(a1->arr) << endl; // 0    
    return 0;
}
```

另外在 C99 中引入了 *flexible array member*，实际上就是一个零长数组，只不过不需要在 `[]` 中指定数组大小 0，它只能作为非空结构体的最后一个成员出现。柔性数组成员属于不完整类型，因此不能对其使用 `sizeof` 运算符。作为零长度数组原始实现的特殊行为（`arr[0]`），`sizeof` 会返回零。

包含柔性数组成员的结构体，或包含此类结构体（可能递归包含）的联合体，不得作为其他结构体的成员或数组的元素，但 GCC 扩展允许这些用法：

1. 柔性数组成员位于外层结构体的末尾： 此用法在 GCC 扩展中被允许（尽管不符合 ISO C99 标准），因为柔性数组成员实际位于内存布局的末尾。
2. 柔性数组成员位于外层结构体的非末尾位置：此时访问柔性数组可能导致**未定义行为**。不同编译器对此类情况的处理不一致（例如内存越界或错误对齐）。使用编译选项 `-Wflex-array-member-not-at-end` 可检测此类问题。

注，现在此扩展已被标记为“废弃”（deprecated）。

#### 12.5 智能指针

##### 1. *unique_ptr*

标准库提供了一个可以管理 `new` 分配的数组的 `unique_ptr` 版本。为了使用一个 `unique_ptr` 管理动态数组，我们必须在对象类型后面跟一对空方括号：

``` c++
unique_ptr<int[]> p(new int[5]{1,2,3,4,5});
for(int i = 0; i < 5; i ++ )    cout << p[i] << endl;
p.release(); // 自动用delete[]销毁其指针
```

指向数组的 `unique_ptr` 提供的操作与指向单个对象的 `unique_ptr` 有些不同。例如<FONT COLOR=BLUE>我们不能使用点和箭头成员运算符</FONT>，毕竟 `unique_ptr` 指向的是一个数组而不是单个对象，因此这些运算符是无意义的。另一方面，当一个 `unique_ptr `指向一个数组时，我们可以使用下标运算符来访问数组中的元素：

``` c++
// 指向数组的 unique_ptr 不支持成员访问运算符（点和箭头运算符）
// 其它 unique_ptr 操作不变
unique_ptr<T[]> u;
unique_ptr<T[]> u(p);
u[i];

u.release();
// ....
```

##### 2. *shared_ptr*

与 `unique_ptr` 不同，`shared_ptr` 不直接支持管理动态数组。如果希望使用 `shared_ptr` 管理一个动态数组，必须提供自己的删除器，因为 `shared_ptr` 默认使用 `delete` 而不是 `delete[]` 销毁元素：

``` c++
// 注意元素类型是Foo，不是 Foo[]
shared_ptr<Foo> sp(new Foo[3]{1,2,3}, [](Foo *p){
    delete[] p;
});
```

另外 `shared_ptr` 不直接支持管理动态数组的特性也会影响我们如何访问数组中的元素：

``` C++
// shared_ptr未定义下标运算符。并且不支持指针的算数运算
shared_ptr<int> sp(new int[3]{1,2,3}, [](int *p){delete[] p;});
for(int i = 0; i < 3; i ++ ) 
    // 先获取内置类型的指针，再进行指针加法来移动指针
    cout << *(sp.get() + i) << endl;
```

由于 `shared_ptr` 未定义下标运算符，并且智能指针不支持指针算数运算。因此，为了访问数组中的元素，必须使用 `get` 获取内置指针，然后用它来访问数组元素。

### 13. *allocator*

#### 13. new的局限性

`new` 和 `delete` 有一些局限性：这体现在 `new` 将分配内存和构造函数绑定在了一起，而 `delete` 将释放内存和析构函数绑定在了一起。但有些时候，我们只想分配一块大内存，然后在这块内存上按需构造对象，即，我们希望将内存分配与对象构造分离，从而在真正需要时才创建对象。

而且，一般情况下将内存分配与对象构造绑定在一起可能会导致不需要的浪费：

``` c++
string *const p = new string[n];
string *q = p;
string s;
while(cin >> s && q != p + n)
    *q ++ = s;	// 默认初始化之后就赋值了
delete[] p;   
```

咋还上面的代码中，我们分配并初始化了 n 个 string 对象。但是，我们可能不需要 n 个 string，少量 string 可能就足够了。这样，我们就可能创建了一些永远也用不到的对象。而且，对于那些确实要用到的对象，我们也在初始化之后立即赋予了它们信的值。每个对象被赋值了两次：第一次是在默认初始化时，随后是在赋值时。

更重要的是，那些没有默认构造函数的类*可能*就不能动态分配内存了。

#### 13.2 allocator类

标准库 `allocator` 类定义在头文件 `<memory>` 中，它帮助我们将内存分配与对象构造分离开来。它提供一种类型感知的内存分配方法，它分配的内存是原始的、未构造的。

类似 `vector`，`allocator` 是一个模板。为了定义一个 `allocator` 对象，我们必须指明这个 `allocator` 可以分配的对象类型。当一个 `allocator` 对象分配内存时，它会根据给定的对象类型来确定恰当的内存大小和对齐位置：

``` c++
allocator<T> a;		// 定义一个allocator对象，它可以为类型为T的对象分配内存

a.allocate(n);		// 分配一块原始的未构造的内存，这段内存可以保存n和T类型的对象
a.deallocate(p,n);  // 释放T*指针p中地址开始的内存
                    // p必须是一个由allocate(n)返回的指针，且分配与销毁时的n必须一致
                    // 在销毁之前用户必须对每个在这块内存中的对象调用 destory

a.construct(p,args); // p为T*类型的指针，只想一块原始内存
					 // 以args为参数调用类型为T的构造函数
a.destory(p);	// p为T*类型的指针，对p指向的对象执行析构函数
				// 只能为真正构造了的对象执行destory
				// 被销毁后的对象做出内存可以重新构造来储存新对象
```

简而言之，`allocator` 的使用步骤大致为：

1. 创建一个类型为 `T` 的 `allocator` 对象
2. 调用 `allocate` 函数分配原始内存
3. 调用 `construct` 函数执行构造函数
4. 调用 `destory` 函数执行析构函数
5. 调用 `deallocate` 函数销毁内存

实际上就是把 new 的分配内存和调用构造函数的操作分开了。

``` c++
allocator<Foo> alloc; // 创建一个allocator对象
auto *p = alloc.allocate(3); 
cout << "allocate done" << endl;
for(int i = 0; i < 3; i ++ )
    alloc.construct(p + i, i);
for(int i = 0; i < 3; i ++ ) 
    cout << p[i].val << endl;
for(int i = 0; i < 3; i ++ )
    alloc.destroy(p + i);
cout << "destory done" << endl;
alloc.deallocate(p, 3);
cout << "deallocate done" << endl;
```

#### 13.3 拷贝和填充未初始化内存的算法

标准库还为 `allocator` 类定义了两个伴随算法，可以在未初始化的内存中创建对象：

``` c++
// 以下所有操作都假定目标位置内存足够
	// 从[b,e)中拷贝元素到原始内存b2
	// 返回指向最后一个已构造元素之后位置的迭代器
iter = uninitialized_copy(b,e,b2);  
	// 从b开始的n个元素拷贝到原始内存b2
	// 返回指向最后一个已构造元素之后位置的迭代器
iter = uninitialized_copy_n(b,n,b2);
	// 在迭代器[b,e]指定的原始内存范围内填充对象t
void uninitialized_fill(b,e,t);   
	// 在迭代器b开始的n个原始内存中填充对象t
void uninitialized_fill_n(b,n,t);
```

#### 13.4 我们很少使用allocator

`allocator` 是一种底层内存管理工具，适用于分配和释放内存，但使用起来相对较复杂。C++ 的 `std::vector` 和其他容器类已经很好的封装了内存管理，通常不需要手动使用 `allocator`。

在现代 C++ 中，直接使用 `allocator` 的场景较少，因为 STL 容器已经默认处理了这些细节。

## 七、拷贝控制

当我们定义一个类时，我们显式或隐式地指定在此类型的对象拷贝、移动、赋值和销毁时做什么。一个类通过定义五种特殊类型的成员函数来控制这些操作：

1. *copy constructor*
2. *copy-assignment operator*
3. *move constructor*
4. *move-assignment operator*
5. *destructor*

> The Big 3 refers to **the destructor, the assignment operator and the copy constructor**.

如果一个类没有定义这些拷贝控制成员，编译器会自动为它定义默认的操作。但有时候，依赖这些操作的默认定义会导致灾难。**通常，拷贝控制最难的地方是首先认识到什么时候需要定义这些操作。**

### 1. 拷贝初始化

当我们使用 `operator=` 来初始化对象的时候，会通过拷贝构造函数或移动构造函数来完成“拷贝初始化”。

拷贝初始化不仅会在我们使用 `operator=` （显式拷贝）时发生，在下列情况下（隐式拷贝）也会发生：

* 将一个对象作为实参传递给非引用类型的形参
* 从一个返回类型为非引用类型的函数返回一个对象
* 用花括号列表初始化一个数组中的元素或一个聚合类中的成员，数组元素或者聚合类中的成员会执行拷贝初始化
* 某些类类型还会对它们所分配的对象使用拷贝初始化。例如 `insert` 或 `push` 成员。与之相对的是 `emplace` 直接初始化。

``` c++
class Foo {
public:
    Foo(int _val = 0) : val(_val) { cout << "ctor" << endl; }
    Foo(const Foo &f) : val(f.val) { cout << "copy ctor" << endl; }
private:
    int val;
};

int main()
{
    vector<Foo> vec;
    vec.reserve(10); // 避免因为扩容时迁移元素导致的copy ctor
    vec.push_back({0}); // ctor, copy ctor
    vec.emplace_back(1); // ctor 
    return 0;
}
```

不过编译器有时会绕过拷贝初始化，即将拷贝初始化改写为直接初始化：

``` c++
string s = "hello";
// 编译器将其优化为
string s("hello");
```

但是，即便是这样，拷贝构造/移动构造也必须是存在且可访问的。

### 2. 拷贝构造函数

* 可以有多个参数（但必须有默认值）
* const 属性
* 引用属性（必须有）

如果一个构造函数的第一个参数是自身的引用，且多余参数都**有默认值**，则此构造函数是拷贝构造函数。拷贝构造函数单纯的拷贝所有非 static 数据成员，对于数组成员会逐个拷贝数组元素。

``` c++
class Foo {
public:
    Foo() {}
    Foo(const Foo &x);
private:
    int i;
    double d;
    string s;
    class Bar b;
};

// 与合成的拷贝构造函数等价
Foo::Foo(const Foo &x) : 
    i(x.i), d(x.d), s(x.s), b(x.b)	// 单纯的拷贝
    {}
```

一般来说，我们会把自身的引用声明为 const。并且如果有隐式初始化的需要，一般也不会把拷贝构造函数声明为 explicit。除了避免额外的拷贝开销，将引用声明为 const 有一个额外的好处，那就是如果我们传入的是一个匿名（临时）对象，也是可以执行拷贝初始化的：

``` c++
class Foo {
public:
    Foo(int _val = 0) : val(_val) { cout << "ctor" << endl; }
    Foo(const Foo &x) { cout << "copy ctor" << endl; }
private:
    int val;
};

Foo f = 10;	// copy ctor
```

如果我们将上面代码中拷贝构造函数中的 const 去掉，那么编译器会报错：

``` shell
error: cannot bind non-const lvalue reference of type ‘Foo&’ to an rvalue of type ‘Foo’
   13 | Foo f = 10;
```

即，我们将一个 *rvalue* 绑定到了一个 *non-const lvalue reference*，这是不合法的。

----

拷贝构造函数可以用来初始化非引用类类型参数，这一特性解释了为何<font color=blue>**拷贝构造函数自己的参数必须是引用类型**</font>：如果其参数不是引用类型，那么调用构造函数就必须拷贝它的实参，而拷贝实参又需要调用拷贝构造函数，如此无限循环。

``` C++
class Foo {
public:
    Foo() {}
    Foo(Foo x) {  }
};

int main()
{
    Foo f1;
    Foo f2 = f1;
}
```

* f2 想要以 f1 为实参进行拷贝初始化，就必须先把 f1 拷贝到 x
* 而 x 如果想以 f1 为实参进行拷贝初始化，就必须先把 f1 拷贝到 x'
* 而 x' 如果想以 f1 为实参进行拷贝初始化，就必须先把 f1 拷贝到 x''
* 无限循环

### 3. 拷贝赋值运算符

* 拷贝赋值运算符需要返回指向他自己的引用，这是与内置的 `operator=` 保持一致。
* 与拷贝构造不同的是，拷贝赋值的第一个参数不必须是自身的引用，因为这里不涉及到无限拷贝构造的问题。
* 当类需要资源管理并且其行为像值时，在定义拷贝赋值时需要注意自我赋值的问题，另外就是异常安全的问题。大多数赋值运算符组合了析构函数和拷贝构造函数的工作。

``` C++
class Foo {
public:
    Foo() {}
    Foo(const Foo &x);
    Foo& operator=(const Foo &x); // 可以是 const Foo x
private:
    int i;
    double d;
    string s;
    class Bar b;
};

// 与合成的拷贝赋值运算符等价
Foo& Foo::operator=(const Foo &x) 
{
    i = x.i;
    d = x.d;
    s = x.s;
    b = x.b;
}
```

### 4. 析构函数

析构函数无返回值，不接受参数。正因为析构函数不接受参数，因此它也不允许重载。

**析构函数的函数体本身并不执行销毁成员的工作，成员在析构函数体执行完毕后自动销毁。**

对于编译器合成的默认析构函数，**对象销毁的顺序和对象初始化的顺序相反**。这是因为在初始化对象时，如果先初始化 A 在初始化 B，那么 A 是有可能作为 B 的一部分的，此时如果我们先析构 A，那么 B 就被破坏了。这就是所谓的“构造由内而外，析构由外而内”。而内置类型没有析构函数，因此什么也不需要做。

销毁一个内置类型指针或引用时，不会 `delete` 掉它指向的对象。因此，对于 `Type *ptr = new Type(args);` 来说，`ptr` 是一个内置指针类型，它指向 `Type` 类型的对象，销毁 `ptr` 时只会在栈上销毁指针 `ptr`，但不会调用 `Type` 的析构函数。

### 5. 三/五法则

拷贝构造、拷贝赋值和析构称为 *Big Three*，加上移动构造和移动赋值就是 *Big Five*。通常我们应该把 *Big Three* 或 *Big Five* 当作一个整体看待，只要定义了一个函数就应该定义其它几个。这也是所谓的**“三/五法则”**。

而是否应该定义其中某个函数，一个基本原则是首先确定这个类是否需要一个析构函数。通常，对析构函数的需求比拷贝构造和拷贝赋值的需求更明显（例如动态内存的 *delete*）。**如果一个类需要一个析构函数，我们几乎可以肯定它也需要一个拷贝构造和一个拷贝赋值。**

**需要拷贝操作的类也需要赋值操作，反之亦然。**凡事总有例外，有时候有些类的工作只需要拷贝或赋值操作，并不需要析构函数。例如，我们需要为每个类分配一个独有的、独一无二的 *id*。这个类需要一个拷贝构造函数为每个新创建的对象生成一个新的、独一无二的 *id*。也需要一个自定义拷贝赋值运算符来避免将 *id* 赋予目的对象。但是这个类不需要析构函数。

### 6. 阻止拷贝

有两种方式阻止拷贝的发生：

1. 编译时错误：将拷贝函数指定为 `=delete`

``` c++
class Foo {
public:
    Foo() = default;
    Foo(const Foo&) = delete;
};

Foo f;
Foo g(f); // error: use of deleted function ‘Foo::Foo(const Foo&)’
```

2. 链接时错误：将拷贝函数声明为 `private` 且不定义。因为友元函数依然可以访问 `private` 成员，所以只声明为 `private` 还不行，还需要不实现

``` C++
class Foo {
    friend class Bar;
public:
    Foo() = default;
private:
    Foo(const Foo&);
private:
    int val;
};

class Bar {
public:
    Bar() = default;
    void func(const Foo &f) {
        Foo g(f);
    }
};

int main()
{
    Foo f;
    Bar b;
    b.func(f);
    return 0;
}
// 1.cpp:(.text._ZN3Bar4funcERK3Foo[_ZN3Bar4funcERK3Foo]+0x32): undefined reference to `Foo::Foo(Foo const&)'
// collect2: error: ld returned 1 exit status
```

与 `default` 的一个不同之处是，我们可以对任何函数指定 `=delete`，但我们只能对编译器可以合成的默认构造函数或拷贝控制成员使用 `=default`。虽然理论上我们可以将析构函数指定为 `=delete`，但应用上我们不可以这样做，因为当析构函数被删除时，我们无法通过构造函数构造静态对象，虽然我们此时可以创建动态对象，但我们无法删除该动态分配的对象。

除了我们自行指定为 `private` 或 `=delete` 外，在某些情况下合成的拷贝控制成员可能是删除的：**简而言之，本质上就是如果一个类有数据成员不能被默认构造、析构、拷贝或赋值，那么该类的成员函数将被定义为删除的。**

1. 如果类的某个成员的默认构造函数或析构函数是删除的或不可访问的，或是类有一个引用成员，它没有类内初始化器，或是类有一个 *const* 成员，他没有类内初始化器且其类型未显式定义默认构造函数，则该类的默认 *ctor* 是被删除的，因此我们必须对没有类内初始化的 *const* 成员和引用成员进行初始化。
2. 如果类的某个成员的拷贝构造函数或析构函数是删除的或不可访问的，则类的合成 *copy ctor* 函数被定义为删除的。
3. 如果类的某个成员的拷贝赋值运算符是删除的或不可访问的，或是类有一个 *const* 的或引用成员。则类的合成 *copy-assignment* 运算符被定义为删除的。（对引用成员赋值只是修改被引用对象的值，而不是修改引用的对象。这种行为并不是我们想要的，因此对于有引用成员的类，合成的拷贝赋值运算符被定义为删除的；对于 *const* 成员，显然的，我们不能对其进行赋值）

4. 如果类的某个成员的析构函数是删除的或不可访问的（例如是 *private*），则类的合成 *dtor* 函数被定义为删除的

### 7. 拷贝语义

通常，管理类外资源的类必须定义拷贝控制成员。为了定义这些成员，我们首先必须确定此类对象的拷贝语义。一般来说有两种选择：可以定义拷贝操作，使类的行为看起来像一个值或一个指针。

* 类的行为像一个值：*string*
* 类的行为像一个指针：*shared_ptr*

### 8. swap 函数

与拷贝控制成员不同，*swap* 并不是必要的，因为 *swap* 的实现借助了拷贝成员。但是对于分配了资源的类，定义 *swap* 可能是一种很重要的**优化手段**。

#### （1）性能优化

例如对于我们自定义的 `String` 类，``swap(s1, s2)`` 的行为类似：

``` c++
String tmp = s1; // 构造tmp
s1 = s2;		 // 拷贝赋值
s2 = tmp;		 // 拷贝赋值
```

我们需要先通过拷贝构造，将 `s1` 拷贝给一个临时对象 `tmp`，然后进行两次赋值操作。而我们知道，对于行为像值的类，赋值操作会涉及到构造与析构（底层对象的 `new` 和 `delete`）。这意味着，一个 `swap` 操作共进行了三次构造，两次析构操作，这开销也太大了。并且每次构造都会分配内存，析构会销毁内存。

理论上，这些内存开销都是不必要的，我们更希望 `s1` 和 `s2` 直接交换底层指针，而不是分配 `String` 的新副本。即：

``` c++
char* tmp = s1.sp;
s1.sp = s2.sp;
s2.sp = data;
// <==>
swap(s1.sp, s2.sp);
```

#### （2）无限定

我们在调用 `swap` 时，不应该加上 `std::` 限定，即类似 `std::swap` 。每个调用都应该是 `swap`，如果存在类型特定的 `swap` 版本，其匹配程度会优先于 *std* 中定义的版本。即，优先调用类型特定的 `swap` 版本；如果不存在类型特定的版本，则会调用 `std` 中的版本。

#### （3）在赋值运算符中使用swap

定义 `swap` 的类通常用 `swap` 来定义它们的赋值运算符。这些运算符使用一种名为<font color=blue>**拷贝并交换（copy and swap）**</font>的技术，这种技术将左侧对象与右侧对象的一份拷贝副本进行交换：

``` c++
// 注意rhs是按值传递的，意味着HasPtr的拷贝构造函数将右侧运算对象中的string拷贝到rhs
HasPtr& HasPtr::operaotr=(HasPtr rhs)
{
    swap(*this, rhs);
    return *this;
}
```

这种技术的有趣之处在于，它既能保证<font color=blue>**自我赋值下的安全性**</font>，又能保证<font color=blue>**异常安全**</font>。这两种保证都是通过在修改左侧对象前，完成可能出问题的操作 —— 如果左侧对象修改了一半，但是有异常抛出，由于左侧对象被修改且无法恢复，所以说不是异常安全的；自我赋值也是同样的道理：

* 异常安全：赋值运算符唯一可能异常的就是 `new` 语句，而这里 `new` 语句用来构造局部对象 `rhs`。如果真的发生异常，函数体还未执行，左侧对象没有改变，因此是异常安全的
* 自我赋值：提前通过 `rhs` 拷贝自己，也发生在函数体执行之前（即左侧对象改变之前）

### 9.  对象移动

对象移动的需求不仅仅是性能上的考虑，也源于 `IO` 类或 `unique_ptr` 这样包含不能被共享资源（如指针或 `IO` 缓冲）的类。这些类型的对象不能拷贝但可以移动。

#### （1）右值引用

右值引用是为了支持移动操作而引入的，一个右值引用只能绑定到一个**将要销毁的对象**。因此，我们可以将一个右值引用的资源“移动”到另一个对象中。

对象将要销毁意味着没有其它用户使用它，因此我们可以自由的接管所引用的对象的资源。一个将要销毁的对象可以是一个右值，也可以是一个左值。但我们不可以直接将一个右值引用绑定到左值上，我们需要使用 `std::move` 来显式的将一个左值绑定到右值引用上。调用 `move` 就意味着承诺：除了对该左值赋值或销毁之外，我们不再使用它。

> 左值和右值是表达式的属性。一般而言，一个左值表达式表示的是对象的身份，一个右值表达式表示的是对象的值。

一个好的编程规范是通过 `std::move` 来调用 `move`，以避免潜在的歧义或命名冲突。

#### （2）保证销毁是无害的

从一个对象移动数据不会销毁此对象，但有时在移动操作完成后，源对象会被销毁。因此，移动操作除了完成资源移动，还必须确保移动后源对象处于这样一个状态 —— 销毁它是无害的。特别是，一旦资源完成移动，源对象必须不再指向被移动的资源——这些资源的所有权已经归属新创建的对象。

``` cpp
// 移动操作不应该抛出任何异常
// 由于我们需要修改源对象，因此rhs不应该是cosnt的
Smart_Pointer(Smart_Pointer &&rhs) noexcept 
: ptr(rhs.ptr) // 接管源对象资源
{ 
    // 令源对象处于这样的状态 -- 对其运行析构函数是安全的
    // 因此不能让源对象还指向被移动的资源
    rhs.ptr = nullptr; 
}
```

如果我们不修改源对象的 `ptr` 指针指向 `NULL`，它会仍然指向被移动的资源，此时对它调用析构函数会导致 *double delete*。

另外我们也可以发现，**“移动”行为本身是我们自己通过代码实现的**，例如上面的 `ptr(x.ptr)`，我们通过浅拷贝接管了 `rhs.ptr`，然后将 `rhs.ptr` 置位 `NULL`。右值引用相当于一种约定，对于右值引用，我们可以随意接管引用的对象。

#### （3）移动操作、标准库容器和异常

由于移动操作“窃取”资源，它通产不分配任何资源。因此，移动操作通常不应该抛出任何异常。而我们编写的移动操作不会抛出任何异常时，我们应该事先通知标准库（例如 `noexcept`）。否则标准库会认为移动我们的类对象时可能抛出异常，并且为了处理这种可能性而做出一些“额外的操作”（通常是为了确保异常安全）。

在不抛出异常时，将移动函数指定为 `noexcept` 基于两个相互关联的事实：

1. 虽然移动操作通常不抛出异常，但抛出异常也是允许的。
2. 标准库容器能对异常发生时其自身的行为提供保障。例如，`vector` 保证，如果我们调用 `push_back` 时发生异常，`vector` 自身不会发生改变。

对于第二点，例如，当我们调用 `push_bac`k 时，可能需要扩容 `vector` 并且将容器元素从旧空间移动到新空间。此时如果我们使用移动构造函数加速扩容时的元素迁移，且在移动了部分而不是全部元素后抛出了一个异常，就会产生问题。旧空间的移动源元素已经被改变了，而在新空间中为构造的元素可能尚不存在。在此情况下，`vector` 不能满足自身保持不变的要求，也即此时不是异常安全的。

另一方面，如果 `vector` 使用拷贝构造函数且发生了异常，它可以保持 `vector` 自身不会发生改变。在此情况下，当在新内存中构造元素时，旧元素保持不变。如果此时发生了异常，`vector` 可以直接释放新分配的内存并返回。`vector` 原有的元素仍然存在。

为了避免这种潜在错误，除非 `vector` 知道元素类型的移动构造函数不会抛出异常，否则在重新分配内存的过程中，他就必须使用拷贝构造函数而不是移动构造函数。如果希望在 `vector` 重新分配内存这类型下对我们自定义类型的对象进行移动而不是拷贝，就必须显示的告诉标准库我们的移动构造函数可以安全使用。我们可以通过将移动构造函数（及移动赋值运算符）标记为 `noexcept` 来做到这一点。

#### （4）移动后源对象仍然有效

这意味着我们可以对其重新赋值，不过在此之前，我们不能对其值做任何假设。

#### （5）合成的移动操作

与拷贝操作不同，编译器根本不会为某些类合成移动操作。特别是，如果一个类定义了自己的构造函数、拷贝赋值运算符或析构函数，编译器就不会为它合成移动构造和移动赋值运算符了。**只有一个类没有定义任何自己版本的拷贝控制成员，且类的每个非 static 数据成员都可以移动时，编译器才会为它合成移动构造函数和移动赋值运算符。**

与拷贝操作不同，移动操作永远不会隐式定义为删除的函数，即使编译器没有默认生成它，因为**<font color=blue>如果一个类没有移动操作，通过正常的函数匹配，类会使用相应的拷贝操作来代替移动操作。</font>**不过，在使用拷贝操作来代替移动操作时，编译器并不保证拷贝操作的正确性：

``` c++
class Foo {
public:
    Foo() { puts("ctor"); }
    Foo(const Foo&) = delete;
private:
};

int main()
{
    Foo a;
    Foo b = std::move(a);  
    return 0;
}
/*
1.cpp: In function ‘int main()’:
1.cpp:34:24: error: use of deleted function ‘Foo::Foo(const Foo&)’
   34 |     Foo b = std::move(a);  
      |                        ^
1.cpp:27:5: note: declared here
   27 |     Foo(const Foo&) = delete;
      |     ^~~
*/
```

在上面的代码中，尽管我们已经将 `Foo::Foo(const Foo&)` 声明为 `delete`，但在 `main` 中编译器依然用该拷贝构造函数代替移动构造函数。

但是，如果我们显式的要求编译器生成 `=default` 移动操作，且编译器不能移动所有成员，则编译器会将移动操作定义为删除的函数。

``` C++
struct Bar {
    Bar() = default;
    Bar(Bar&&) = delete;
};

struct Foo {
    Foo() = default;
    Foo(Foo&& x) = default;
    Bar b;
};

int main() 
{ 
    Foo a;
    Foo b = std::move(a);  
     // function "Foo::Foo(const Foo &)" (declared implicitly) 
     // cannot be referenced
     // it is a deleted functionC/C++
    return 0;
}
```

在上面的代码中，我们希望 `Foo` 生成一个默认的移动构造函数，但编译器报告该函数已被删除。

除了一个重要例外，什么时候将合成的移动操作定义为删除的函数遵循与定义删除的合成拷贝操作类似的原则：

* 类似拷贝赋值运算符，如果有类成员是 `const` 的或引用，则类的***移动赋值运算符***被定义为删除的。
* 类似拷贝构造函数，如果类的析构函数被定义为删除的（`=delete`）或不可访问的（`private` 且无友元），则类的***移动构造函数***被定义为删除的
* 类似拷贝构造和拷贝赋值，如果有类成员的移动构造函数或移动赋值运算符被定义为删除的或是不可访问的，则类的***移动构造函数***或***移动赋值运算符***被定义为删除的
* 与拷贝构造函数不同，移动构造函数被定义为删除的函数的条件是：有类成员定义了自己的拷贝构造函数且未定义移动构造函数，或是有类成员未定义自己的拷贝构造函数且编译器不能为其合成移动构造函数。移动赋值运算符的情况类似。

<font color=blue>注意，这里的删除，不是真正意义上的删除（即我们自行通过 `=delete` 指定的删除函数）。我们是可以重新定义编译器默认“删除”的函数的。</font>

例如，对于上面代码中的 `class Foo`，它的移动赋值运算符是默认删除的，不过我们可以自定义我们自己的移动赋值运算符，但要注意解决例如 `const` 和引用不能拷贝等问题：

``` c++
struct Bar {
    Bar() = default;
    Bar(Bar&&) { puts("move"); }
};

struct Foo {
    Foo() = default;
    Foo(Foo&& x) = default;
    Foo& operator=(Foo&&) {
        puts("operator==");
        return *this;
    }

    const int val = 10;
    Bar b;
};

int main() 
{ 
    Foo a;
    Foo b;
    b = std::move(a);  // operator==
    return 0;
}
```

移动操作和合成的拷贝控制成员间还有最后一个相互作用关系：一个类是否定义了自己的移动操作对拷贝操作如何合成有影响。如果一个类定义了一个移动构造或移动赋值运算符，则类的合成拷贝构造函数和拷贝赋值运算符会被定义为删除的。因此，**定义了一个移动构造函数或移动赋值运算符的类必须也定义自己的拷贝操作，否则这些成员默认的被定义为删除的。**

#### （6）移动操作的实现在函数内部

并不是说，我们执行了移动构造函数或移动赋值运算符，编译器就自动帮我们“移动”元素，我们仍然需要手动移动一些元素：

```  C++
class Bar {
public:
    Bar() = default;
    Bar(const Bar &x) { puts("Bar::copy ctor"); }
    Bar(Bar &&x) { puts("Bar::move ctor"); }
private:
    const int val = 10;
};

class Foo {
public:
    Foo() { puts("Foo::ctor"); }
    Foo(const Foo &f) { puts("Foo::copy ctor"); }
    // 移动 f.mem
    // Foo(Foo&& f) : mem(std::move(f.mem)) // Bar::move ctor
    // { puts("Foo::move ctor"); } 
    
    // 拷贝 f.mem
    Foo(Foo&& f) : mem(f.mem) // Bar::copy ctor 
    { puts("Foo::move ctor"); } 
private:
    Bar mem;
};

int main() 
{ 
    Foo a;
    Foo b = std::move(a); 
    return 0;
}
```

在上面 `Foo` 的移动构造函数中，对于成员变量 `mem`，如果我们不显示指定 `std::move(f.mem)`，它调用的依然是 `Bar` 的拷贝构造函数，虽然我们通过 `std::move(a)` 调用的是 `Foo` 的移动构造函数。

不过对于内置类型则不需要显式指定 `move`，因为内置类型本身并没有复杂的内存管理。所以内置类型直接拷贝即可，毕竟其拷贝操作的开销也不大，编译器还能从中进行各种优化。

#### （7）右值引用和成员函数

除了构造和赋值，一个普通成员函数也可以同时提供拷贝和移动版本，它的参数模式一般都是相同的 —— 拷贝版本接受一个 `const` 的左值引用，移动版本接受一个非 `const` 的右值引用。其中，左值版本可以接受左值引用和右值引用，右值版本只能接受右值引用。

### 10. 更新三/五法则

在加入了移动操作之后，所有五个拷贝控制成员应该看作一个整体：一般来说，如果一个类定义了任何一个拷贝操作，他就应该定义所有五个操作：拷贝构造、拷贝赋值、移动构造、移动赋值、析构。

### 11. 移动迭代器

前面我们介绍过 `uninitialized_copy(b,e,b2)` 函数，他将迭代器 `[b,e)` 指定的元素拷贝到 `b2` 指定的未构造内存当中。不过，标准库中并没有类似的函数将对象“移动”到未构造的内存中。

新标准库定义看了一种**移动迭代器（move iterator）适配器**。一个移动迭代器适配器通过改变给定迭代器的解引用运算符的行为来适配次迭代器。一般来说，一个迭代器的解引用运算符返回一个指向元素的左值引用。与其他迭代器不同，移动迭代器的解引用运算符返回一个右值引用。

我们通过标准库的 `make_move_iterator` 函数将一个普通迭代器转换为一个移动迭代器。此函数接受一个迭代器参数，返回一个移动迭代器。

``` C++
auto last = uninitialized_copy(
    make_move_iterator(begin())),
	make_move_iterator(end()),
	first);
```

### 12. 引用限定符

#### （1）引入

通常，我们在一个对象上调用成员函数，而不管该对象是一个左值还是一个右值。例如：

``` C++
string s1 = "hello,", s2 = "world";
auto n = (s1 + s2).find('d');
```

此例中，我们在一个临时 `string` 对象（右值）上调用 `find` 成员。看起来也还好，不过还有更令人惊讶的：	

``` c++
struct Foo {
    Foo(int _val = 0) : val(_val) {}
    Foo& operator=(const Foo& x)  { 
        val = x.val; 
        return *this;
    }
    int val;
};

Foo operator+(const Foo &a, const Foo &b)
{
    return Foo(a.val + b.val);
}

int main() 
{ 
    Foo a(1), b(2);
    a + b = 3;	// 合法
    return 0;
}
```

此处我们对两个 `string` 的连接结果 —— 一个右值，进行赋值。

> 右值通常是临时对象、字面量或表达式结果，没有持久的内存地址，不能被取地址（如 `&(s1 + s2)` 是非法操作）。

在旧标准中，我们没有办法阻止这种对右值的赋值行为。**为了维持向后兼容性，新标准库类仍允许向右值赋值。**但是，我们可能希望在自己的类中阻止这种用法。即，我们希望强制左侧运算对象（即，this 指向的对象）是一个左值。

**我们指定 this 的左值/右值属性**的方式与定义 const 成员函数的形式相同。即，在参数列表后放置一个**引用限定符**：

``` c++
struct Foo {
    Foo(int _val = 0) : val(_val) {}
    Foo& operator=(const Foo& x) & // 指定this必须是一个左值
    {
        val = x.val; 
        return *this;
    }
    int val;
};
```

此时再执行上面的 `a+b=3` 就不合法了，因为 `a+b` 的返回结果是一个右值，所以它无法调用成员函数 `operator=`。

``` C++
struct Foo {
    Foo(int _val = 0) : val(_val) {}
    Foo& operator=(const Foo& x) & { // 左值this限定
        val = x.val; 
        return *this;
    }
    int val;
};

Foo operator+(const Foo &a, const Foo &b)
{
    return Foo(a.val + b.val);
}

int main() 
{ 
    Foo a(1), b(2);
    a + b = 3;	// error: passing ‘Foo’ as ‘this’ argument discards qualifiers [-fpermissive]
    return 0;
}
```

如果引用限定符和 `const` 限定符同时出现，引用限定符需要放在 `const` 限定符的后面，这可能与引用限定符出现的时间比 `const` 限定符晚有关？

#### （2）重载

除了限定调用对象是左值之外，引用限定符还有更大的作用。同 `const` 限定符 一样，引用限定符也是函数签名的一部分，因此也可以用来**重载函数**。因此可以根据调用对象是左值还是右值来对函数的行为做出优化。例如，在类中有一个函数 `sorted`，它返回对内部 `vector` 元素的排序：

``` c++
class Foo {
public:
    Foo() = default;
    Foo(const vector<int>& _data) : data(_data) {}
    Foo(vector<int> &&_data) : data(std::move(_data)) {}
    Foo(initializer_list<int> il) : data(il) {}
    Foo(const Foo &f) : data(f.data) {}
    Foo(Foo &&f) : data(std::move(f.data)) {}
public:
    Foo sorted() const &;
    Foo sorted() &&;
private:
    vector<int> data;
};

Foo Foo::sorted() const &
{
    puts("lvalue");
    // 对于左值，需要拷贝其内部数据再修改
    Foo ret(*this);
    sort(ret.data.begin(), ret.data.end());
    return ret;
}

Foo Foo::sorted() &&
{
    puts("rvalue");
    // 对于右值，直接修改内部数据
    sort(data.begin(), data.end());
    return *this;
}

int main() 
{ 
    Foo f = Foo().sorted(); // rvalue
    Foo f2 = f.sorted();    // lvalue
    return 0;
}
```

同 `const` 一样，引用限定符在声明和定义处都需要指定，因为它是**函数签名**的一部分。

有一点和 `const` 不同，对于上面的两个 `sorted`，我们一个指定为 `const`，一个未指定为 `const`。但引用限定符则不一样，如果我们定义了两个或两个以上**具有相同名字和相同参数列表**的成员函数，就必须对所有函数都加上引用限定符，或所有都不加：

``` c++
class Foo {
public:
    Foo() = default;
public:
    Foo sorted() const &;
    Foo sorted() const &&;
    Foo sorted(); /* ERROR:
    overloading two member functions with the same parameter types 
    requires that they both have ref-qualifiers or both lack ref-qualifiers
    */
private:
    vector<int> data;
};
```

对面上面的三个 `<Foo(void)> sorted;` 我们要么都加上引用限定符，要么都不加。

## 八、重载运算与类型转换

重载的运算符是具有特殊名字的函数：它们的名字由关键字 `operator` 和后其要定义的运算符号共同组成。重载运算符的优先级、结合律和参数个数与其内置形式一致。

对于一个运算符函数来说，它或者是类的成员函数，或者至少含有一个类类型的参数。这一约定意味着当运算符作用于内置类型时，我们无法改变该运算符的含义。

有些运算符有多重含义，例如 `+`，``-``，`*`，`&`。可以从参数个数推断运算符的含义。换言之，重载运算符参数的个数是有要求的，因为我们需要据此推断运算符的含义。但是返回类型并无要求，我们可以对 `operator+` 返回一个 `void` 类型（返回类型不作为函数签名）。不过并不建议这么做，因为**运算符重载的核心要义应该是与该运算符作用于内置类型时的含义保持一致。**例如逻辑操作应该返回 `bool` 类型，加法应该返回一个与操作数一样的类型等。

> 除了重载的函数调用运算符  `operator()` 除外，其他重载运算符不能含有默认实参。

我们只能重载已有的运算符，而不能发明新的运算符。而且不是所有运算符我们都可以重载，不能被重载的运算符：`::`，`.*`，`.`，`?:`。虽然这意味着我们可以重载诸如 `&&`，`||` 和 `,` 等逻辑运算符，但并不建议这么做，因为重载后运算符无法保留**运算对象求值顺序**。例如当我们重载逻辑与运算符时，在函数中两个运算对象总是被求值，这就破坏了其短路求值的特性。

> 通常情况下，不应该重载逗号、取地址、逻辑与和逻辑或运算符。

区别于普通函数，运算符函数有两种调用形式，一种是通过**运算符形式**进行隐式调用，另一种是通过**普通函数形式**进行显式调用：

``` c++
struct Foo {
    Foo(int _val = 0) : val(_val) {}
    int val;
    Foo& operator+=(const Foo &x) {
        val += x.val;
        return *this;
    }
};

void operator+(const Foo &a, const Foo &b)
{
    // return Foo(a.val + b.val);
}

int main()
{
    Foo a(3), b(4);
    
    a + b;	// 隐式
    operator+(a, b); // 显式

    a += b; // 隐式
    a.operator+=(b); // 显式
    
    return 0;
}
```

和其它成员函数的设计思想一样：例如如果我们有一个非 `const` 的成员函数，可能也需要一个 `const` 的成员函数；如果我们需要自定义拷贝构造函数，那么一般也需要自定义拷贝赋值函数。同理的，如果我们定义了 `operator==`，那么一般来说我们也需要 `operator!=`；如果我们定义了 `operator<`，那么一般也需要其它关系运算符；如果我们定义了 `operator+`，一般也需要定义相应的复合加法 `operator+=`。

最后，不要滥用运算符重载：

* 考虑好某个功能到底使用运算符实现还是普通函数实现

* 尽量明智的使用运算符重载。只有当运算符操作的含义对于用户来说清晰明了时才使用运算符。如果用户对运算符可能有几种不同的理解，则使用这样的运算符将产生二义性。

### 1. 成员函数还是非成员函数

[*StackOverflow: What are the basic rules and idioms for operator overloading?*](https://stackoverflow.com/questions/4421706/what-are-the-basic-rules-and-idioms-for-operator-overloading)

#### The Three Basic Rules of Operator Overloading in C++

When it comes to operator overloading in C++, there are ***three basic rules you should follow***. As with all such rules, there are indeed exceptions. Sometimes people have deviated from them and the outcome was not bad code, but such positive deviations are few and far between. At the very least, 99 out of 100 such deviations I have seen were unjustified. However, it might just as well have been 999 out of 1000. So you’d better stick to the following rules.

1. ***Whenever the meaning of an operator is not obviously clear and undisputed, it should not be overloaded.*** *Instead, provide a function with a well-chosen name.*
   Basically, the first and foremost rule for overloading operators, at its very heart, says: *Don’t do it*. That might seem strange, because there is a lot to be known about operator overloading and so a lot of articles, book chapters, and other texts deal with all this. But despite this seemingly obvious evidence, *there are only a surprisingly few cases where operator overloading is appropriate*. The reason is that actually it is hard to understand the semantics behind the application of an operator unless the use of the operator in the application domain is well known and undisputed. Contrary to popular belief, this is hardly ever the case.
2. ***Always stick to the operator’s well-known semantics.***
   C++ poses no limitations on the semantics of overloaded operators. Your compiler will happily accept code that implements the binary `+` operator to subtract from its right operand. However, the users of such an operator would never suspect the expression `a + b` to subtract `a` from `b`. Of course, this supposes that the semantics of the operator in the application domain is undisputed.
3. ***Always provide all out of a set of related operations.***
   *Operators are related to each other* and to other operations. If your type supports `a + b`, users will expect to be able to call `a += b`, too. If it supports prefix increment `++a`, they will expect `a++` to work as well. If they can check whether `a < b`, they will most certainly expect to also to be able to check whether `a > b`. If they can copy-construct your type, they expect assignment to work as well.

#### The Decision between Member and Non-member

| Category                   | Operators                  | Decision                                       |
| -------------------------- | -------------------------- | ---------------------------------------------- |
| Mandatory member functions | `[]`, `()`, `=`, `->`, ... | member function (mandated by the C++ standard) |
| Pointer-to-member access   | `->*`                      | member function                                |
| Unary                      | `++`, `-`, `*`, `new`, ... | member function, except for enumerations       |
| Compound assignment        | `+=`, `|=`, `*=`, ...      | member function, except for enumerations       |
| Other operators            | `+`, `==`, `<=>`, `/`, ... | prefer non-member                              |

The binary operators `=` (assignment), `[]` (array subscription), `->` (member access), as well as the n-ary `()` (function call) operator, must always be implemented as ***member functions***, because the syntax of the language requires them to.

Other operators can be implemented either as members or as non-members. Some of them, however, usually have to be implemented as non-member functions, because ***their left operand cannot be modified by you***. The most prominent of these are the input and output operators `<<` and `>>`, whose left operands are stream classes from the standard library which you cannot change.

For all operators where you have to choose to either implement them as a member function or a non-member function, ***use the following rules of thumb*** to decide:

1. If it is a ***unary operator***, implement it as a ***member*** function.
2. If a binary operator treats ***both operands equally*** (it leaves them unchanged), implement this operator as a ***non-member*** function.
3. If a binary operator does ***not*** treat both of its operands ***equally*** (usually it will change its left operand), it might be useful to make it a ***member*** function of its left operand’s type, if it has to access the operand's private parts.

Of course, as with all rules of thumb, there are exceptions. If you have a type

```cpp
enum Month {Jan, Feb, ..., Nov, Dec}
```

and you want to overload the increment and decrement operators for it, you cannot do this as a member functions, **since in C++, enum types cannot have member functions.** So you have to overload it as a free function. And `operator<()` for a class template nested within a class template is much easier to write and read when done as a member function inline in the class definition. But these are indeed rare exceptions.

(However, *if* you make an exception, do not forget the issue of `const`-ness for the operand that, for member functions, becomes the implicit `this` argument. If the operator as a non-member function would take its left-most argument as a `const` reference, the same operator as a member function needs to have a `const` at the end to make `*this` a `const` reference.)

#### 总结

尽管有些运算符只能作为类的成员函数，例如需要使用 `this` 指针的操作（`->`），但大部分函数还是不容易判断是否应该作为成员函数。下面是一些准则帮助我们判断某个重载运算符是作为成员函数还是非成员函数（或友元函数）。

有明确要求的：

* 赋值（`=`）、下标（`[]`）、函数调用（`()`）、类型转换和成员访问箭头（`->`）运算符必须是【成员函数】，这是 C++ 语言标准要求的
* 流运算符一定是普通的【非成员函数】，因为左操作数是流对象，我们无法修改其左侧运算对象

经验法则：

* 单目运算符一般作为【成员函数】
* <font color=blue>改变对象状态</font>的运算符或者与<font color=blue>给定类型密切相关</font>的运算符，例如递增、递减、解引用和复合赋值运算符运算符，一般应该是【成员函数】
* 如果二元操作符将两个操作数同等对待（保持不变），则将该操作符实现为【非成员函数】。例如具有对称性的运算符（`a op b` 等价于 `b op a`），例如算数、相等性、关系和位运算符等，一般是普通的【非成员函数】

对称性运算通常作为非成员是因为它常常含有混合类型的表达式，例如我们能求一个 `int` 和一个 `double` 的和，能对 `string` 对象和 `const char*` 对象做加法。并且这两个操作数的顺序可以交换：

``` c++
String s = "hello";
String t = s + "world"; // "world"隐式构造
String u = "world" + s;	// 可能不合法
```

如果 `operator+` 是类的成员函数，那么第三条语句的加法运算不合法，因为没有针对 `“hi”.operator+(s);` 的重载形式。

### 2. 输入和输出运算符

输入输出运算符必须是**非成员函数**，因为左操作数是流对象，无法修改其类。

输出运算符尽量**减少格式化操作**，这是因为用于内置类型的输出运算符本身就不太考虑格式化操作，尤其不会打印换行符。并且减少格式化操作可以使用户有权控制输出的细节。

输入运算符需要处理输入可能失败的情况，输出运算符则不需要。执行输入运算符时可能发生下列错误：

* 当流含有错误类型的数据时读取操作可能失败。
* 当流读取到文件末尾或者遇到输入流的其它错误时也会失败。

当读取操作发生错误时，输入运算符应该负责从错误中恢复，即重置一些值（当读取错误时这些值可能是未定义的），这样对象可以保持一个合法的状态，从而略微保护使用者免于收到输入错误的影响。

例如：

``` c++
class Person {
public:
    friend std::istream& operator>>(std::istream& is, Person &p);
    friend std::ostream& operator<<(std::ostream& os, const Person &p); 

public:
    Person(std::string _name = std::string(), int _age = 0, bool _isman = 1);
    
private:
    std::string name;
    int age;
    bool isman;
};

std::istream& operator>>(std::istream& is, Person &p)
{
    is >> p.name >> p.age >> p.isman;
    // 当输入发生错误时，重置一些值以确保对象p是合法的
    if(!is) p = {"", 0, 1};
    return is;
}
```

### 3. 算数运算符

通常情况下，我们会把算数定义成**非成员函数**以允许对左侧和右侧的运算对象进行转换。

由于 这些运算符一般不需要改变运算对象的状态，所以形参都是常量的引用。计算结果常常保存在一个**局部变量**之内（注意不是引用），操作完成后返回该局部变量的副本作为结果。

如果同时定义了算数运算符和相关的复合赋值运算符，则通常情况下应该使用复合赋值运算符来实现算数运算符。

``` C++
Foo operator+(const Foo &lhs, const Foo &rhs) {
    Foo res = lhs;
    lhs += rhs;
    return res;
}
```

显而易见的，我们不能通过算数运算符实现复合赋值运算符。因为算数运算符会返回一个临时对象，这会带来额外的拷贝和析构消耗。其次体现在代码上也不简洁。

### 4. 关系运算符

和算数运算符一样，我们会把关系运算符定义成**非成员函数**以允许对左侧和右侧的运算对象进行转换。

关系运算符一般分为两类：

* 相等运算符 `==`，`!=`
* 比较运算符：`<`，`<=`，`>`，`>=`

当我们定义了一类中的某个运算符时，其余运算符一般也需要定义。

从内置类型的关系运算符语义上来说，相等运算符和比较运算符是有联系的：**⚠**<font color=blue>**如果两个对象是 `!=` 的，那么一个对象应该 `<` 另外一个。**</font>因此如果我们在类中同时定义了相等运算符和比较运算符，那么就要考虑它们是否保证上面的语义。看下面的例子：

``` c++
class Sales_data {
public:
    friend bool operator==(const Sales_data &lhs, const Sales_data &rhs);
    friend bool operator!=(const Sales_data &lhs, const Sales_data &rhs);
    friend bool operator<(const Sales_data &lhs, const Sales_data &rhs);
public:
    Sales_data(std::string _isbn = std::string(), double _price = 0, int sales_count = 0)
        : isbn(_isbn), price(_price), revenue(sales_count * _price) 
        {}
private:
    std::string isbn;
    double price;
    double revenue;
};

bool operator==(const Sales_data &lhs, const Sales_data &rhs)
{
    return lhs.isbn == rhs.isbn && lhs.price == rhs.price && lhs.revenue == rhs.revenue;
}

bool operator!=(const Sales_data &lhs, const Sales_data &rhs)
{
    return !(lhs == rhs);
}

// 是否合适呢?
bool operator<(const Sales_data &lhs, const Sales_data &rhs)
{
    return lhs.isbn < rhs.isbn;
}
```

对于上面的类，我们定义的 `operator<` 是否合适呢？显然是不合适的，例如我们有下面两个对象：

``` c++
Foo a('0123', 1, 1);
Foo b('0123', 2, 2);
```

由于 `price` 和 `revenue` 不相等，所以这两个对象是不相等的。但由 `operator<` 的定义，可以看出这两个对象哪个都不比另外一个小，则从道理上来说这两个对象应该是相等的。所以说 `operaotr<` 的定义和 `operator==` 的定义就冲突了。

### 5. 赋值运算符

除了拷贝赋值和移动赋值运算符之外，类还可以定义其他赋值运算符以使用别的类型作为右侧运算对象。例如 `vector` 定义了接受花括号内的元素列表（`initializer_list`）作为参数的赋值运算符。

``` C++
class Foo {
public:
    Foo(int _a = 0, int _b = 0, int _c = 0) : a(_a), b(_b), c(_c) {}
    Foo& operator=(const std::initializer_list<int> &il) {
        a = *il.begin();
        b = *(il.begin() + 1);
        c = *(il.begin() + 2);
        return *this;
    }

private:
    int a, b, c;
};
```

赋值运算符必须是类的成员，复合赋值运算符不必是类的成员，但我们还是倾向于把包括复合赋值运算符在内的所有赋值运算都定义在类的内部。

**之所以规定赋值运算符必须是类的成员，是因为如果我们没有为类提供一个赋值运算符的话，编译器会为类提供一个合成的默认拷贝赋值运算符，此时如果我们在全局定义了一个赋值运算符，就会发生二义性。**

### 6. 下标运算符

下标运算符必须是**成员函数**。

下标运算符必须以所访问元素的**引用**作为返回值。

进一步，我们最好同时定义下标运算符的**常量版本**和**非常量版本**。

### 7. 递增和递减运算符

C++ 并不要求递增和递减运算符是类的成员，但是因为它们改变的正好是所操作对象的状态，所以建议将其设定为**成员函数**。

同内置类型的递增和递减运算符一样，我们也需要同时定义递增或递减运算符的**前置版本**和**后置版本**。并且通常借助前置版本来实现后置版本。

后置版本接受一个额外的（不被使用）`int` 类型参数。当我们使用后置运算符时，编译器会为这个形参提供一个值为 `0` 的实参。尽管从语法上我们可以使用这个额外的参数，但是在实际过程中通常不会这么做。这个形参的唯一作用就是用来区分前置版本和后置版本的函数。

如果我们像显式的调用后置版本，需要指定这个不被使用的 `int` 类型参数。尽管这个值通常会被运算符函数忽略，但他却必不可少，否则编译器无法得知这是后置版本。

### 8. 成员访问运算符

在迭代器和智能指针类中常常用到解引用运算符（`*`）和箭头运算符（`->`）。它们都必须作为类的成员，并且通常是**常量成员函数**。

#### （1）解引用运算符

对于 `operator*` 的重载比较简单，返回对象的**引用**即可。不过实际上，就如我们前面提到的，C++ 并没有强行规定运算符的返回类型，也意味着即使我们返回一个字面量或其他类型也是可以的。但并不推荐这么做，因为这与内置类型的解引用语义不一致。

#### （2）箭头运算符

不过，C++ 对于 `operator->` 的返回类型是有要求的，它永远不能丢掉**“成员访问”**这个最基本的含义。当我们重载箭头时，可以改变的是箭头从哪个对象当中获取成员，而箭头获取成员这一事实则永远不变。

对于形如 `point->mem` 的表达式来说，`point` 必须是指向类对象的指针或一个重载了 `operator->` 的类对象。根据 `point` 类型的不同，`point->mem` 分别等价于：

``` c++
(*point).mem;			  // point是一个内置的指针类型
point.operator->()->mem;  // point是一个类对象
```

除此之外，代码都将发生错误。`point->mem` 的执行过程如下所示：

1. 如果 `point` 是内置指针类型，则我们应用内置的箭头运算符，表达式等价于 `(*point).mem`。首先解引用该指针，然后从所得的对象中获取指定的成员。
2. 如果 `point` 是定义了 `operator->` 的类的一个对象，则我们使用 `point.operator->()` 的结果来获取 `mem`。其中，如果该结果是一个指针，则执行第一步；如果该结果是一个重载了 `operator->` 的类，则重复当前步骤。

可以发现，对于 `operator->` 的解析，是一个**“递归”**的过程，并且最终的工作还是要交给 `operator*`。这本身也符合 `point->mem <==> (*point).mem` 的语义。

> 注意关于内置指针类型，强调的是指针，而不是指针所指向的对象。也就是说，对于 `T *obj;` 无论 `T` 是什么类型，`obj` 的类型都是一个内置指针类型。

可以看一个例子，更好的理解 `operator->` 针对内置指针和类对象的解析流程：

``` C++
class A{
public:
    A() { x = 1; }
    void action(){ cout << "Action in class A!" << endl; }

public:
    int x;
};

class B{
public:
    B() { x = 2; }
    A* operator->() { return &a; }
    void action() { cout << "Action in class B!" << endl; }
private:
    int x;
    A a;
};

class C{
    B b;
public:
    C () { x = 3; }
    B operator->(){ return b; }
    void action(){ cout << "Action in class C!" << endl; }
private:
    int x;
};

int main(int argc, char **argv)
{
    C* pc = new C;
    pc->action();   // Action in class C!
                    // 这个调用会被当作内置指针类型的箭头运算符调用
                    // 相当于 (*pc).action();
    
    C c;
    c->action();    // Action in class A!
                    // 由于 c 不是内置指针类型,会调用重载了的operator->()
                    // 相当于 c.operator->()->action
                    // 根据 C::operator->() 的定义
                    // 相当于 b->action,其中 b 不是内置指针类型
                    // 根据 B::operator->()的定义
                    // 相当于 a->action,其中 a 是内置指针类型
                    // 故最终解析为 (*a).action()
    
    int x = c->x;   // 1
                    // 和 c->action() 的分析一样
                    // 这里最终解析为 (*a).x;  
    cout << x << endl;
    return 0;
}
```

#### （3）为什么箭头运算符特殊

这其实算是一种官方的约定：重载箭头操作符必须返回指向类类型的指针，或者返回定义了自己的箭头操作符的类类型对象。

* 如果返回类型是指针，则内置箭头操作符可用于该指针，编译器对该指针解引用并从结果对象获取指定成员。如果被指向的类型没有定义那个成员，则编译器产生一个错误。
* 如果返回类型是类类型的其他对象（或是这种对象的引用），则将递归应用该操作符。编译器检查返回对象所属类型是否具有成员箭头，如果有，就应用那个操作符；否则，编译器产生一个错误。这个过程继续下去，直到返回一个指向带有指定成员的的对象的指针，或者返回某些其他值，在后一种情况下，代码出错。

### 9. 函数调用运算符

如果类重载了函数调用运算符，则我们可以像使用函数一样使用类的对象，因此这种对象也成为“函数对象/仿函数”。因为这样的类同时也能存储状态，所以与普通函数相比它们更加灵活。

函数调用运算符必须是**成员函数**。一个类可以定义多个不同版本的调用运算符。

注意对函数调用运算符的调用都是通过调用进行的，并且通常是通过匿名对象来调用。例如：

``` c++
sort(a.begin(), a.end(), greater<int>());
```

通过创建一个标准库函数匿名对象 `greater<int>()` 来调用。

#### （1）lambda是函数对象

当我们编写了一个 lambda  表达式后，编译器将表达式翻译成一个**未命名类的未命名对象**。（注意是通过对象来调用调用运算符的）

* 这个未命名类含有一个重载的函数调用运算符。
* 默认情况下 lambda 不能改变它捕获的变量，因此重载的函数调用运算符是常量成员函数。如果我们为 lambda 指定 mutable 关键字，那么函数调用就不是 const 的了。
* 按值捕获的变量会作为类的私有数据成员。lambda 对按值引用变量的修改不会作用于被捕获的变量；在 lambda 创建后，对值捕获变量的修改于 lambda 是不可见的。
* 按引用捕获的变量不会作为类的数据成员。lambda 对按引用捕获变量的修改会同时作用域被捕获的变量；在 lambda 创建后，对引用捕获变量的修改于 lambda 是可见的。

``` c++
int main()
{
    int val = 10, r = 10;

    // 在lambda创建之前对val的修改于f可见
    val = 30, r = 30;

    auto func = [val, &r]() mutable ->void {
        cout << "func: " << val << ' ' << r << endl;
        val = 100;
        r = 100;
    };

    func();                                      // 30, 30

    // f中对val的修改于外部的val不可见
    cout << "main: " << val << ' ' << r << endl; // 30, 100

    // 在lambda创建之后对val的修改于f不可见
    val = 50, r = 50;
    func();                                      // 100, 50 
}
```

#### （2）标准库定义的函数对象

标准库定义了一组表示算术运算符、关系运算符和逻辑运算符的**类模板**，每个类分别定义了一个执行命名操作的调用运算符。例如 `plus` 类定义了一个函数调用运算符用于对运算对象执行 `+` 操作。

这些类都被定义成模板的形式，我们可以为其指定具体的应用类型，即运算符的形参类型。例如 `plus<int>`  的运算对象是 `int`。

``` c++
plus<int> intAdd;
int sum = intAdd(20, 30); // 50
multiplies<int> intMulti;
int mul = intMulti(10, 2); // 20
```

其余的标准库函数对象有：

```c++
// 算数运算
plus<T>;	        //  +
minus<T>;      		//  -
multiplies<T>; 	    //  *
divides<T>;    	    //  /
modulus<T>;	        //  %
negate<T>;          //  -

// 关系运算
equal_to<T>;		//  ==
not_equal_to<T>;	//  !=
greater<T>;			//  >
greater_equal<T>;	//  >=
less<T>;			//  <
less_equal<T>;		//  <=

// 逻辑运算
logical_and<T>;	    //  &&
logical_or<T>;	    //  ||
logical_not<T>;	    //  !
```

特别需要注意的是，**标准库规定其函数对象对于指针同样使用**。在 C++ 中，比较两个无关指针将会产生未定义的行为，然而我们有时希望通过比较指针的内存地址来 `sort` 存放指针的 `vector`。直接这么做将产生未定义的行为，因此我们可以通过一个标准库对象来实现该目的：

``` c++
vector<string*> vec;

// 错误：指针彼此之间没有关系,所以<将产生未定义的行为
sort(vec.begin(), vec.end(), [](string *a, string *b){
    return a < b;
});

// 正确：标准库规定指针的less是定义良好的
sort(vec.begin(), vec.end(), less<string*>());
```

#### （3）可调用对象与 function

C++ 语言中有几种可调用的对象：

* 普通函数
* 函数指针
* 仿函数
* lambda 表达式
* bind 创建的对象

和其他对象一样，可调用的对象也有类型。例如，每个 lambda 都有他自己唯一的（未命名）类类型；函数及函数指针的类型则由其返回值类型和实参类型决定，等等。

然后，两个不同类型的可调用对象却可能共享同一种<font color=blue>**调用形式**</font>。调用形式指明了**调用返回的类型**以及**传递给调用的实参类型**。实际上，一种调用类型对应一种函数类型，例如：`int(int,int)` 是一个调用形式，它接受两个 `int`，返回一个 `int`。

注意可调用形式不同于函数指针，事实上，可调用形式和函数指针的形式非常类似：

``` c++
return_type(args);		// 调用形式
return_type(*)(args);	// 函数指针
```

对于几个可调用对象共享同一种调用形式的情况，有时我们会希望把它们看作具有相同的类型。这样我们就可以统一化所有可调用对象，从而实现格式上的同一。例如一个函数指针 `int(*)(int,int)`，它既可以指向函数，也可以指向一个相同调用形式的 lambda 表达式或仿函数。

C++ 提供了一个名为 `function` 的标准库模板类型来解决上述问题，它将**调用形式**作为模板参数，一次统一不同可调用对象。`function` 定义在 `functional` 头文件中：

``` C++
// f接受一个可调用对象，其调用形式必须与T相同
function<T> f;			// 隐式构造一个空function
function<T> f(nullptr); // 显式构造一个空function
function<T> f(obj);		// 在f中存储可调用对象的副本
f;	                    // 将f作为条件判断,若f存储可调用对象为真;反之为假
f(args); 			    // 调用f中存储的可调用对象

// 类型成员 
result_type;			// 可调用成员对象的返回类型
argument_type;  		// 如果T中只有一个参数
first_argument_type;	// 如果T中只有两个参数
second_argument_type;
```

注意，不能直接将重载函数的名字存入 function 类型的对象中，即使重载函数之间参数的个数也不相同：

``` c++
int add(int a, int b)        {return a + b;}
int add(int a, int b, int c) {return a + b + c;}

function<int(int,int)> f(add); // 编译错误
```

解决该问题的一条途径是存储函数指针，而非函数的名字：

``` C++
int add(int a, int b)        {return a + b;}
int add(int a, int b, int c) {return a + b + c;}

int (*fp)(int,int) = add; 
function<int(int,int)> f(fp);
```

### 10. 类型转换运算符

转换构造函数（只含有一个实参的 non-explicit 构造函数）和类型转换运算符共同定义了**类类型转换**（用户定义的类型转换）。

类型转换运算符是类的一种特殊成员函数，它负责将一个类类型的值转换成其它类型。其形式一般为：

``` C++
operator type() const; // type为我们想要转换的类型
```

可以发现类型转换运算符有以下特点：

* 函数名就是我们要转换的类型（和构造函数类型）
* 无返回类型（实际上返回类型就是 type）
* 无参数（需要隐式执行。参数有默认值也不行）
* 必须作为类的成员函数
* 一般要定义为常量成员函数

类型转换运算怒发可以面向任意类型（void 除外）进行定义，只要改类型能作为函数的返回类型。因此，我们不允许转换为数组或函数类型（函数不能返回数组和函数），但可以转换成函数指针、数组指针或引用类型。

例如，我们定义一个只能存放 0~255 之间的整数的类：

``` c++
class SmallInt {
public:
    SmallInt(int _val = 0) : val(_val)
    {
        if(val < 0 || val > 255)
            throw runtime_error("Bad SmallInt value");
    }
    operator int() const { return val; }
public:
    size_t val;
};

int main()
{
    SmallInt a(16);
    a = 3.14;   // 3.14隐式转换为int然后再转换为SmallInt
    auto f = a + 3.14;  // a隐式的转换为int,然后与3.14执行加法,结果为double
    return 0;
}
```

注意**编译器一次只能执行一个用户定义的类型转换**，但隐式的用户定义类型转换可以置于一个标准（内置）类型转换之前或之后，并与其一起使用。

#### （1）向bool的类型转换问题

在编程实践当中，类很少提供类型转换运算符。因为在大不多情况下，类型转换是隐式的、自动发生的，当它自动发生时，用户可能感觉比较意外，而不是感觉受到了帮助。然而这条法则有一个例外：对于类来说，定义向 bool 的类型转换还是比较普遍且使用的现象。

但是如果类想定义一个向 bool 的类型转换，则它常常会遇到一个问题：因为 bool 是一种算数类型，所以类类型的对象转换成 bool 之后就能被用在任何需要算数类型的上下文中。这样的类型转换可能引发意想不到的结果，特别是当 istream 含有向 bool 的类型转换时，下面的代码在【早期 C++ 版本】中将通过编译：

``` c++
int i = 42;
cin << i; // 如果istream向bool的转换不是显式的,该行代码将编译通过
```

按理来说对输入流执行输出运算符显然是错误的。然而，改代码能使用 istream 的 bool 类型转换将 cin 转换成 bool，而这个 bool 紧接着会被提升为 int 并用作内置的左移运算符的左侧运算对象。这样一来，这一行语句的效果相当于对一个 bool 值（0 或 1）左移 42 个位置。这太让人惊讶了！

为了防止这种情况发生，C++11 新标准引入了**“显式的类型转换”（explicit conversion operator）**，即我们可以将 `explicit` 关键字应用于类型转换运算符：

``` c++
class SmallInt {
public:
    // 编译器不会自动执行这一行类型转换
    explicit operator int() const { return val; }
};
```

此时我们只能使用显式的类型转换： `static_cast<T>(arg)`。不过该规定存在一个例外，即如果表达式被用作“条件”，则编译器会将显式的类型转换自动应用于它。换言之，**当表达式出现在下列位置时，显示的类型转换被隐式的自动执行：**

* if、while 即 do 语句的条件部分
* for 语句头的条件表达式
* 逻辑非（`!`）、逻辑或（`||`）、逻辑与（`&&`）的运算对象
* 条件运算符（`?:`）的条件表达式

<font color=blue>**可以发现向 bool 的类型转换通常用在条件部分，所以 `operator bool` 一般定义成 `explicit` 的，在不影响其作为条件表达式进行条件判断的前提下保证其算数上的正确性。**</font>

在早期的 C++ 版本中，标准库通过对 IO 类型定义向 `void*` 的转换规则以避免自动类型转换的问题（返回一个 `void*` 而不是 `bool`）。在 C++11 新标准下，IO 标准库定义一个向 bool 的显式类型转换实现相同的目的。

#### （2）避免有二义性的类型转换

如果类中包含一个或多个类型转换，则必须确保在类类型和目标类型之间只存在唯一的一种转换方式。否则的话，我们编写的代码很可能具有二义性。

在两种情况下可能产生多重转换路径：

1. 两个类提供相同的类型转换

例如下面两个类都提供了从 B 到 A 的类型转换：

``` c++
struct B;

struct A {
    A() = default;
    A(const B&) { // 将一个B转换为A
        cout << "A::constructor(B)" << endl;
    }
};

struct B {
    operator A() const { // 将一个B转换为A
        cout << "B::operaotr A()" << endl;
        return A();
    }
};

B b;

void func(A a) {}

int main()
{
    func(b.operator A()); // 合法：显式调用
    func(A(b));           // 合法：显式调用
    
    func(b);  // 二义性错误：到底是 func(B::operator A())
              // 还是 func(A::A(const B&))
    
    return 0;
}
```

值得注意的是，我们无法使用强制类型转换来解决二义性问题，因为强制类型转换本身也面临二义性。

> 虽然但是，在我的电脑上（`g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`）测试，输出的都是 `A::constructor(B)` ，也就是优先调用 A 的构造函数。
>
> 这可能是某些编译器（如较老版本的 GCC/MSVC）可能会**默认选择一种转换方式**，而不是严格遵循 C++ 标准。使用 **`-pedantic-errors`（GCC/Clang）** 或 **`/permissive-`（MSVC）** 编译选项，强制编译器严格遵循标准，编译器就会报错。`error: conversion from ‘B’ to ‘A’ is ambiguous`

2. 类定义了多个转换规则，而这些转换涉及的类型本身可以通过其他类型转换联系在一起。最典型的例子就是算术运算符。对某个给定的类来说，最好只定义一个与算术类型有关的类型转换。

例如下面的类：

``` c++
struct A {
    A(int _i = 0) : i(_i) { puts("A(int)"); }
    A(double _d = 0) : d(_d) { puts("A(double)"); }
    operator int() const { return i; }
    operator double() const { return d; }

    int i;
    double d;
};

void func(short) {}

void main()
{
    A a1(3.134);    // A::A(double)
    func(a1);       // 二义性错误：含义是 func(A::operator int())
                   // 还是 func(A::operator double())
    
    long long x = 1;    
    A a2(x);       // 二义性错误：含义是 A::A(int)
                   // 还是A::A(double)
    
    short s = 3;
    A a3(s);       // A::A(int)
                   // short提升为int的优先级高于short提升为double
}
```

总而言之，除了显式的向 `bool` 类型的转换之外，我们应该**尽量避免定义类型转换函数**并**尽可能地限制那些“显然正确”的非显示构造函数**。

#### （3）函数重载与转换构造函数

当调用重载函数时，如果多个用户定义的类型转换都提供了可行匹配，我们认为这些匹配类型**一样好**。在这个过程中，我们不会考虑任何可能出现的标准类型转换的级别。这是基于**<font color=blue>用户定义的类型转换优先级相同</font>**。

例如下面的例子，尽管通过字面量整数值 10 （默认为 `int` 类型）来构造类型 B 的对象是精确匹配，依然产生二义性错误：

``` c++
struct A {
    A(double) {}
};

struct B {
    B(int) {}
};


void func(const A&) {}
void func(const B&) {}

int main()
{
    func(10);   // 二义性错误：含义是 func(A::A(double))
                // 还是 func(B::B(int))
    return 0;
}
```

只有当重载函数能通过同一个类型转换函数得到匹配时，我们才会考虑其中出现的标准类型转换。例如：

``` c++
struct A {
    operator int() { return 0; }
};

void func(double) { puts("double"); }
void func(int)    { puts("int"); }

int main()
{
    A a;
    func(a);    // int
    return 0;
}
```

### 11. 函数匹配与重载运算符

和普通函数调用不同，**对于重载运算符，我们不能通过调用的形式来区分当前调用的是成员函数还是非成员函数。**例如：

``` c++
struct A {
    friend A operator+(const A &a);
    A(int _val) : val(_val) {}
    int val;
};
A operator+(const A &a, const A &b) { // 非成员函数
    A res = a;
    res.val += b.val;
    return res;
}
/*======================================================================================*/
struct A {
    A(int _val) : val(_val) {}
    A operator+(const A &a) const { // 成员函数
        A res = a;
        res.val += val;
        return res;
    }
    int val;
};
```

对于上面 A 的两种定义，我们都可以通过下面的形式进行调用：

``` C++
int main()
{
    A a(1), b(2);
    A c = a + b;	// 调用 operator+
    cout << c.val << endl;
    return 0;
}
```

可以发现调用的形式是一样的。

另外就是注意 class 内部的函数和 class 外部的函数属于两个作用域，类似于下面的形式是编译通过的：

``` c++
struct A {
    void func() { puts("member"); }
};

void func()
{
    puts("non-member");
}
```

不同于运算符调用时，我们不知道调用的到底是成员函数还是非成员函数，对于普通函数，通过类成员调用的就是成员函数，否则就是非成员函数。

## 九、面向对象程序设计

### 1. 多态

在 C++ 中，动态分为静态多态和动态多态。

* 静态多态由函数重载实现。
* 动态多态是通过动态绑定实现的，当我们通过基类的指针或引用调用虚函数时，会触发动态绑定（运行时绑定）。

**引用或指针的静态类型与动态类型不同这一事实正是 C++ 支持动态多态的根本所在。**

* 如果不使用基类的指针或引用就无法实现动态类型。
* 如果不是虚函数就无法触发动态绑定。非虚函数根据静态类型执行，虚函数根据动态类型执行。

当我们使用存在继承关系的类型时，必须将一个变量或表达式的静态类型与该表达式表示对象的动态类型区分开来。表达式的静态类型在编译时总是已知的， 它是变量声明时的类型或表达式生成的类型；动态类型则是变量或表达式表示的内存中的对象的类型。动态类型直到运行时才能知道。如果表达式既不是指针也不是引用，则它的静态类型永远与动态类型一致。

### 2. 对象切片

当派生类对象被赋值或初始化给基类对象时，**只有基类部分的数据成员会被复制/移动**，派生类独有的部分会被“切掉”。这是因为：

- 基类的拷贝/移动操作（构造函数、赋值运算符）**只能感知基类的内存布局**。
- 这些函数接受的参数是 `const Base&` 或 `Base&&`，虽然可以绑定到派生类对象，但仅能访问基类部分。

### 3. 继承

#### 3.1 虚析构函数

在继承关系下，基类通常都应该定义一个**虚析构函数**，即使该函数不执行任何操作也是如此，这么做是确保当通过**基类指针**删除派生类对象时，确保调用正确的**析构链**。

``` cpp
class Base {
public:
    Base()          { cout << "Base::ctor" << endl; }
    virtual ~Base() { cout << "Base::dtor" << endl; }
};

class Derived : public Base {
public:
    Derived()          { cout << "Derived::ctor" << endl; }
    virtual ~Derived() { cout << "Derived::dtor" << endl; }
};

int main()
{
    Base *b = new Derived;
    delete b;
    return 0;
}
// Base::ctor
// Derived::ctor
// Derived::dtor
// Base::dto
```

如果我们没有将积累的析构函数声明为 `virtual`，输出为：

``` shell
Base::ctor
Derived::ctor
Base::dtor
```

可以发现，派生类对象的析构函数没有正确调用。

其底层机制为，编译器会在派生类析构函数末尾**自动插入基类析构函数的调用代码**，类似：

```cpp
// 编译器生成的 Derived::~Derived() 伪代码
virtual ~Derived() {
    // 1. 执行用户编写的派生类析构代码
    cout << "Derived::dtor" << endl;

    // 2. 编译器自动调用基类析构函数
    Base::~Base(); 
}
```

#### 3.2 内存布局

在一个对象中，继承自基类的部分和派生类自定义的部分在内存分布上不一定是连续的。具体取决于继承方式、编译器实现以及可能的优化（如空基类优化）。

**可能不连续的情况：**

- 当派生类继承多个基类时，不同基类子对象之间可能存在填充（padding），或编译器为了对齐插入空隙。
- 虚继承为了解决菱形继承问题，虚基类子对象会被共享，其位置通常由编译器通过指针或偏移量间接访问。
- 若基类是空类（无成员），编译器可能将其优化到派生类的其他空隙中，而非单独分配空间（基类可能不占用额外内存）。
- 虚函数表指针（vptr）的插入也可能影响布局。

**验证方法：**可以通过打印对象地址和成员偏移量观察内存布局：

```cpp
struct Base { int a; short b; int c; };
struct Derived : public Base { char d; };

int main() 
{
    Derived d;
    cout << offsetof(Derived, a) << endl; // 0
    cout << offsetof(Derived, b) << endl; // 4
    cout << offsetof(Derived, c) << endl; // 8
    cout << offsetof(Derived, d) << endl; // 12
    return 0;
}
```

如果添加了虚函数，vptr 还会占用 8 字节：

``` C++
struct Base { int a; short b; int c; virtual void f(){} };
struct Derived : public Base { char d; };

int main() 
{
    Derived d;
    cout << offsetof(Derived, a) << endl; // 8
    cout << offsetof(Derived, b) << endl; // 12
    cout << offsetof(Derived, c) << endl; // 16
    cout << offsetof(Derived, d) << endl; // 20
    return 0;
}
```

#### 3.3 基类成员的初始化

尽管在派生类对象中包含从基类继承而来的成员，但是派生类不可以直接初始化基类的成员，即使这些成员的访问权限是 `public`：

``` c++
class Base {
public:
    Base(int _a = 0) : a(_a) {}
    int a;
};

class Derived : public Base {
public:
    Derived(int _a = 0, int _b = 0, int _c = 0)
        : a(_a), b(_b), c(_c) {}
    // error: class ‘Derived’ does not have any field named ‘a’
    // "a" is not a nonstatic data member or base class of class "Derived"
    int b, c;
};
```

每个类控制它自己的成员的初始化过程，因此应该将 Derived 的构造函数修改为：

``` C++
class Derived : public Base {
public:
    Derived(int _a = 0, int _b = 0, int _c = 0)
        : Base(_a), b(_b), c(_c) {}
    int b, c;
};
```

不过，虽然派生类不能直接初始化基类的成员，但是从语法上来说，我们可以在类构造函数体内给它的 public 或 protected 成员赋值，此时基类会调用它的默认构造函数为基类成员初始化：

``` c++
class Base {
public:
    Base(int _a = 0) : a(_a) { cout << "Base::ctor" << endl; }
public:
    int a;
};

class Derived : public Base {
public:
    Derived(int _a = 0, int _b = 0, int _c = 0)
        : b(_b), c(_c) { a = _a; }
    int b, c;
};

Derived d; // Base::ctor
```

但是最好不要这样做，和使用基类的其他场合一样，派生类应该遵循基类的接口。派生类应该使用基类的构造函数来初始化那些从基类中继承而来的成员。

> <font color=blue>编程规范：遵循类的接口</font>
>
> 每个类负责定义各自的接口。想要与类的对象交互必须使用该类的接口，即使这个对象是派生类的基类部分也是如此。

最后就是在初始化时，应该首先初始化基类的部分，然后按照声明的顺序依次初始化派生类的成员。也即，**构造由内而外，析构由外而内。**

#### 3.4 静态成员

静态成员也是类的成员，也会被派生类继承。但是无论如何继承，静态成员只会存在唯一的实例。

``` C++
class Foo {
public:
    static void func() { cout << "Foo::static function" << endl; }
    static int val;
};

int Foo::val = 1024;

class Bar : public Foo {
public:
};

int main()
{
    cout << Bar::val << endl;	// 1024
    Bar::func();                // Foo::static function
    return 0;
}
```

#### 3.5 派生类的声明

当我们声明一个派生类时，不应该包含它的派生列表。一条声明语句的目的是令程序知晓某个名字的存在以及改名字表示一个什么样的实体，如一个类、一个函数、一个变量等。派生列表以及与定义有关的其它细节必须与类的主体一起出现。

``` C++
class Foo {};
class Bar : public Foo; // error: class or struct definition is missing
class Bar : public Foo {};
```

如果我们想将某个类用作基类，则该类必须已经定义而非仅仅声明：

``` C++
class Foo;
class Bar : public Foo { // incomplete type "Foo" is not allowed
};
```

这一规定的原因显而易见：当我们指定了派生列表时，实际上就是在定义类了，定义类肯定要确定类的大小，但基类还没定义，大小就不知道。

#### 3.6 final

##### 1. 修饰类

如果我们想定义一个不能被其它类继承的类，或者不想考虑它是否适合作为一个基类。可以使用 C++11 新标准提供的 `final` 关键字：

``` c++
class Foo final {};
class Bar : Foo {} // error: a 'final' class type cannot be used as a base class
```

注意使用 `final` 修饰一个类时，需要提供完整的定义，我们不可以对一个类的前向声明使用 `final`。

##### 2. 修饰虚函数

除了用于修饰类，`final` 还可以修饰虚函数，但是不同于类，修饰于函数的 `final` 可以用于声明：

``` C++
class Base {
public:
    virtual void func() final;
};

void Base::func() {
    // ...
}
```

但是我们无法在类外定义处添加 `final`，否则会报错：

``` shell
error: virt-specifiers in ‘func’ not allowed outside a class definition
```

### 4. 虚函数

#### 4.1 virtual 的声明

一旦某个函数被声明为虚函数，则在所有派生类中它都是虚函数，因此可以无需再次使用 `virtual` 关键字声明。

关键字 `virtual` 只能出现在类内部的声明语句而不能出现在类外部的函数定义。这是语言设计的明确规则，目的是将虚函数的**接口声明**和**实现定义**分离。	

``` C++
class Graph {
public:
    virtual void draw() = 0;
};

class Circle : public Graph {
public:
    virtual void draw() override;
};

virtual void Circle::draw() { //  error: ‘virtual’ outside class declaration
    // ...
}
```

**1. 底层原理**

- **虚函数表（vtable）的生成**：
  编译器在解析类定义时，根据类内声明中的 `virtual` 关键字生成虚函数表。类外定义只是实现，无需重复标记。
- **接口与实现分离**：
  `virtual` 属于接口规范（告诉编译器“支持多态”），而类外定义是具体实现细节。

**2. 特殊场景：纯虚函数**

对于纯虚函数（`= 0`），`virtual` 同样只在类内声明时使用：

```cpp
class AbstractBase {
public:
    virtual void pureFunc() = 0; // 类内声明为纯虚函数
};

void AbstractBase::pureFunc() {  // 类外定义（可选实现）
    std::cout << "Pure virtual impl\n"; 
}
```

**3. 为什么这样设计？**

- **避免二义性**：若类外定义允许 `virtual`，可能导致同一函数在不同编译单元中被误标记为虚或非虚。
- **代码清晰性**：虚函数的性质应在类定义中明确，而非隐藏在实现文件中。

#### 4.2 虚函数必须给出定义

通常情况下，如果我们不使用一个函数，是无需给出定义的。但是虚函数必须给出定义，因为虚函数在运行时通过虚表（vtable）调用，但**虚表里的函数指针必须指向一个有效的地址**，否则链接器会报错。而未定义的函数是没有保存在代码段，是没有地址的。

``` C++
class Foo {
public:
    virtual void f();
};
Foo f;  // /usr/bin/ld: /tmp/ccQlW5fd.o:(.data.rel+0x0): undefined reference to `vtable for Foo'
		// collect2: error: ld returned 1 exit status
/*-------------------------------------------------*/

class Foo {
public:
     void f();
};
Foo f;  // 编译通过
```

不过，纯虚函数除外，包裹继承而来的纯虚函数：

``` C++
class Base {
public:
    virtual void f() = 0;
};

class Derived : public Base {};
```

此时 `Base` 和 `Derived` 都是包含了纯虚函数的抽象类。

#### 4.3 协变返回类型

在C++中，**协变（Covariance）** 指的是派生类在覆盖（override）基类的虚函数时，可以修改返回类型，但必须满足：

- **基类虚函数返回 `B*` 或 `B&`**
- **派生类虚函数返回 `D*` 或 `D&`**
- **`D` 必须是 `B` 的公有派生类（即 `D` 继承自 `B`）**

这种特性称为 **协变返回类型（Covariant Return Types）**，它允许派生类返回比基类更具体的类型。

----

##### **1. 为什么叫 "协变"？**

- **"协"（Co-）** 表示 "共同变化"（一起变化）。
- **"变"（Variance）** 指的是类型关系的变化方式。

在类型系统中：

- **协变（Covariance）**：如果 `D` 继承自 `B`，那么 `D*` 也可以当作 `B*` 使用（方向一致）。
- **逆变（Contravariance）**：参数类型的相反关系（C++ 不支持）。
- **不变（Invariance）**：类型必须严格匹配（C++ 普通函数的默认行为）。

C++ 的虚函数返回类型支持 **协变**，即：

- 如果 `D` 是 `B` 的子类，那么 `D*` 可以替代 `B*`。

----

##### **2. 协变返回类型的例子**

```cpp
class Animal {
public:
    virtual Animal* clone() const {
        return new Animal(*this);
    }
};

class Dog : public Animal {
public:
    virtual Dog* clone() const override {  // ✅ 协变返回类型：Dog* 代替 Animal*
        return new Dog(*this);
    }
};

int main() {
    Dog dog;
    Animal* animal = dog.clone();  // Dog* 可以隐式转换为 Animal*
    return 0;
}
```

----

##### **3. 为什么需要协变返回类型？**

**（1）避免类型转换**

如果没有协变返回类型，`Dog::clone()` 必须返回 `Animal*`，使用时需要强制转换：

```cpp
class Dog : public Animal {
public:
    virtual Animal* clone() const override {
        return new Dog(*this);  // 返回 Animal*，但实际是 Dog*
    }
};

int main() {
    Dog dog;
    Dog* clonedDog = static_cast<Dog*>(dog.clone());  // 需要强制转换
    return 0;
}
```

**协变返回类型让代码更自然，避免冗余的类型转换。**

**（2）支持多态工厂模式**

例如，实现一个通用的 `clone()` 方法，派生类可以直接返回自己的类型：

```cpp
class Shape {
public:
    virtual Shape* clone() const = 0;
};

class Circle : public Shape {
public:
    virtual Circle* clone() const override {  // 协变返回类型
        return new Circle(*this);
    }
};
```

这样，调用 `Circle::clone()` 时直接得到 `Circle*`，而不是 `Shape*`。

----

##### **4. 协变返回类型的限制**

**（1）仅适用于指针或引用**

- 可以返回 `D*` 或 `D&`（协变）。

- **不能返回 `D` 对象**（值类型），例如：

  ```cpp
  class Animal {
  public:
      virtual Animal clone() const; 
  };
  
  class Dog : public Animal {
  public:
      virtual Dog clone() const override;  // error: invalid covariant return type for ‘virtual Dog Dog::clone() const’
  };
  ```

**（2）必须是公有继承**

```cpp
class B {};
class D : private B {}; 

class Base {
public:
    virtual B* f();
};

class Derived : public Base {
public:
    virtual D* f() override;  //  error: invalid covariant return type for ‘virtual D* Derived::f()
};
```

**（3）不能用于模板类型**

```C++
template<typename T>
class Base {
public:
    virtual T* foo() { /*...*/ }
};

template<typename T>
class Derived : public Base<T> {
public:
    // 错误：不能重写，因为返回类型不同
    Derived<T>* foo() override { /*...*/ }
};
```

原因：

1. **类型系统限制**：模板实例化会产生完全不同的类型。`Base<T>`和`Base<U>`即使T和U有继承关系，这两个模板实例也没有继承关系。
2. **实例化时机**：模板在实例化前不是完整类型，编译器无法在模板定义时验证协变返回类型的有效性。
3. **语言规范限制**：C++标准明确规定了协变返回类型必须是指向类的指针或引用，且派生类中的返回类型必须派生自基类中的返回类型。模板类型参数打破了这种直接继承关系。

#### 4.4 overide

如果派生类定义了一个与基类中虚函数名字相同但参数列表不同的类，编译器会认定该函数与基类中的虚函数是相互独立的。这时派生类中的函数并没有覆盖掉基类的虚函数，但有时我们的本意是覆盖虚函数，但搞错了参数列表，这样的错误调试起来是比较困难的。不过好在 C++ 新标准提供了 override 关键字，用来告诉编译器我们这个函数是用来覆盖基类中的某个虚函数，从而让编译器检查虚函数的参数。

#### 4.5 默认实参

和其它函数一样，虚函数也可以拥有默认实参，并且同一个虚函数在继承体系中的默认实参的**默认值可以不同**。

值得注意的是，如果某次函数调用使用了默认实参，则该实参的值由本次调用的**静态类型**决定。其本质这是因为默认参数值是根据**声明类型**（编译时类型）来确定的，而虚函数调用的**函数体**是根据对象的**实际类型**（运行时类型）来确定的。

``` C++
class Foo {
public:
    virtual void f(int val = 100) { cout << "Foo: " << val << endl; }
};

class Bar : public Foo {
public:
    virtual void f(int val = 200) { cout << "Bar: " << val << endl; }
};

Foo f;
Bar b;

int main()
{
    Foo &cur = b;     // 静态绑定的默认值是100
    cur.f();          // Bar: 100
    return 0;
}
```

在上面的例子中，我们通过动态绑定，调用的是 `Bar::f()` ，但输出的 `val` 的值却是 100，这可能与我们的本意不符。因此为了避免这种反直觉的情况发生，最好将基类和派生类中定义的默认实参值设置为一样的。

#### 4.6 回避虚函数的默认机制

在某些情况下，我们可能希望对虚函数的调用不要进行的动态绑定，或是希望其执行虚函数的某个特定版本。使用**作用域运算符**可以实现这一目的：

``` C++
class Foo {
public:
    virtual void f(int val = 100) { cout << "Foo: " << val << endl; }
};

class Bar : public Foo {
public:
    virtual void f(int val = 200) { cout << "Bar: " << val << endl; }
};

class Dad : public Bar {
public:
    virtual void f(int val = 300) { cout << "Bar: " << val << endl; }
};

Foo f;
Bar b;
Dad d;

int main()
{
    Foo &cur = d;     // 静态绑定的默认值是100
    cur.f();          // Dad: 100
    cur.Foo::f();     // Foo: 100
    // cur.Bar::f();  // error: ‘Bar’ is not a base of ‘Foo’
    return 0;
}
```

在上面的代码中，要注意 `cur` 不能调用 `Bar` 版本的虚函数 `f()`，因为 `cur` 的静态类型是 `Foo`，而表达式 `cur.Bar::f()` 相当于通过 `Foo`（基类） 对象调用 `Bar`（派生类） 的成员函数，而从基类到派生类的转换是不合法的。我们可以这样修改代码：

``` c++
Bar &cur = d;     // 静态绑定的值是 200
cur.f();          // Dad: 100
cur.Foo::f();     // Foo: 100
cur.Bar::f();     // Bar: 200
```

将引用的类型修改为 `Bar` 之后，调用就合法了，因为 `Bar`（派生类）对象可以转换为 `Foo`（基类）对象。

另外一个值得注意的点就是，**当我们显式指定虚函数版本时，参数默认值使用的是我们指定的虚函数版本的默认值，而不是静态类型虚函数版本的默认值。**

最后，什么时候需要回避虚函数的默认机制呢？通常是当一个派生类的虚函数需要调用他覆盖的基类的虚函数版本时。在此情况下，基类的版本通常完成继承层次中所有类型都要完成的共同任务，而派生类中定义的版本需要执行一些与派生类本身密切相关的操作。

### 5. 抽象基类

和普通的虚函数不同，<font color=blue>纯虚函数</font>无需定义。我们可以在虚函数声明后面加上 `=0` 来标识这是一个纯虚函数。其中 `=0` 只能出现在虚函数声明语句处。另外如果我们确实想为纯虚函数提供定义，只能在类外提供。

含有（或者未经覆盖直接继承）纯虚函数的类称为<font color=blue>抽象基类</font>。抽象基类负责定义接口（纯虚函数），而后续的其它类可以覆盖该接口。我们不能（直接）创建一个抽象基类的对象。即使我们在类的外部提供了纯虚函数的定义也不行。

### 6. 访问控制与继承

#### 6.1 protected

`protected` 访问权限除了可以让派生类访问之外，还有一条重要的性质：派生类的成员或友元只能通过派生类对象来访问基类的受保护成员。**派生类对于一个基类对象中的受保护成员没有任何访问权限。**例如：

``` C++
class Base {
protected:
    int proc_mem;
};

class Derived : public Base {
    friend void func(Derived& d);
    friend void func(Base& b);
public:
    int val;
};

void func(Derived& d) {
    d.proc_mem = d.val = 100;
}

// protected member "Base::proc_mem" is not accessible 
// through a "Base" pointer or objectC/C++(410)
void func(Base& b) {
    b.proc_mem = 100;
}
```

我们不可以通过派生类的友元函数去访问基类对象的受保护成员；只能通过派生类对象去访问基类的受保护成员。

这样做的目的是为了确定基类对象的访问安全（受保护成员对用户应该是不可见的）。如果我们可以通过派生类的友元函数来访问基类对象的受保护成员，那么我们（用户）只需要通过继承就可以规避掉基类的访问控制。

#### 6.2 派生访问说明符

* `public` 继承**：基类的 `public` 成员在派生类中依然是 `public`，`protected` 成员依然是 `protected`，`private` 成员保持不可访问。**
* `protected` 继承**：基类的 `public` 和 `protected` 成员在派生类中都变为 `protected`，`private` 成员保持不可访问。**
* **`private` 继承**：基类的 `public` 和 `protected` 成员在派生类中都变为 `private`，`private` 成员保持不可访问。

<font color=blue>首先需要说明的一点是，对于派生访问说明符，无论是 public、private 还是 protected 继承，它们对派生类的成员（以及友元）能否访问其直接基类的成员没有任何影响。对基类成员的访问权限只与基类中的访问说明符有关。派生访问说明符的目的是控制派生类用户（派生类以及派生类的派生类）对于基类成员的访问权限。</font>

>  例如，即便类 D 以 private 继承类 B，类 D 依然可以访问类 B 中的 public 和 protected 成员，只不过这些成员在类 D 中的访问级别是 private。
>
>  因此此时如果类 DD 继承自类 D，无论 DD 采取何种方式继承，它都不可以访问类 B 中的任何成员，因为类 B 的成员此时在类 D 中已经变成 private 权限了。

#### 6.3 派生类向基类转换的可访问性

派生类向基类的转换是否可访问由使用该转换的代码决定（用户还是成员函数或友元），同时派生类的派生访问说明符也会有影响。

* 用户：只有当 D 共有地继承 B 时，用户代码才能使用派生类向基类的转换。

``` c++
class B {};
class PucD : public B {};
class ProtD : protected B {};
class PriD : private B {};

B b;
PucD pucd;
ProtD protd;
PriD prid;

int main()
{
    b = pucd;
    b = protd; // conversion to inaccessible base class "B" is not allowed
    b = prid;  // conversion to inaccessible base class "B" is not allowed
    return 0;
}
```

* D 的成员和友元：无论 D 以什么方式继承 B，D 的成员和友元都能使用派生类到基类的转换。

``` C++
class PucD : public B {
    void f() {
        B b;
        PucD pucd;
        b = pucd;
    }
};
class ProtD : protected B {
    void f() {
        B b;
        ProtD protd;
        b = protd;
    }
};
class PriD : private B {
    void f() {
        B b;
        PriD prid;
        b = prid;
    }
};
```

* D 的派生类的成员函数和友元：如果 D 继承 B 的方式是共有的或受保护的，则 D 的派生类的成员和友元可以使用 D 向 B 的类型转换。

``` C++
class B {};

class PucD : public B {};
class ProtD : protected B {};
class PriD : private B {};

class PucDD : public PucD {
    void f() {
        B b;
        PucD pucd;
        b = pucd;
    }
};

class ProtDD : protected ProtD {
    void f() {
        B b;
        ProtD protd;
        b = protd;
    }
};

class PriDD : private PriD {
    void f() {
        B b; // type "B::B" (declared at line 5) is inaccessible
        PriD prid;
        b = prid;
    }
};
```

<font color=blue>一言以蔽之，对于代码中的某个节点来说，如果基类的公有成员是可访问的，则派生类想基类的类型转换是可访问的；反之则不行。</font>

#### 6.4 关于访问权限的理解

在 C++ 中，类的访问权限主要是考虑到以下三种用户：

* 类设计者（成员函数及友元）
* 普通用户
* 派生类

如果我们将 public、protected 和 private 三种权限抽象为是否可以访问，那么对于类设计者来说，三种访问权限都是可访问的；对于派生类来说，public 和 protected 是可访问的；对于普通用户来说，只有 public 权限是可访问的。

#### 6.5 友元与继承

<font color=blue>友元关系是单向的。</font>

前面提到过，友元关系是不可传递的。例如：B 是 A 的友元类，C 是 B 的友元类，但 C 不是 A 的友元类。

同样的，友元关系也不能继承。例如 A 是类 C 的友元类，但 A 不是 C 的派生类的友元类；A 的派生类也不是了 C 的友元类。

``` C++
class A;

class B{
    friend class A;
protected:
    int b = 20;
};

class D : public B {
protected:
    int d = 30;
};

class A {
public:
    int fc(B b) {
        return b.b;        
    }
    // 这里可能有些疑惑,我们可以访问D中的受保护成员
    // 但确实是合法的
    int fa(D d) {
        return d.b;
    }
    int fd(D d) {
        return d.d; // 不可访问
    }
};

class DA : public A {
public:
    void f(B b) {
        return b.b;	// 不可访问
    }
};
```

对于函数 `fa`，我们可以访问派生类 D 中的基类部分，这着实令我们疑惑。但如果我们尝试访问 D 的对象，例如 `d.d`，就发导致不可访问错误。

其原因在于，在 C++ 中，每个类负责控制自己的成员的访问权限。因为 A 是 B 的友元，所有 A 能够访问 B 对象的成员，这种可访问包括了 B 对象内嵌在其派生类对象中的情况。

#### 6.6 改变个别成员的可访问性

有时候我们只希望将个别基类成员的可访问性改为 private，其余的可访问性不变或者将基类成员分为不同的访问级别。通过 using 声明可以达到这一目的：

``` c++
class B {
protected:
    int a = 1;
    int b = 2;
public:
    int c = 3;
private:
    int d = 4;
};

class D : public B {
private:
    using B::a;
public:
    using B::b;
private:
    using B::d; // member "B::d" is inaccessible
};

int main()
{
    D d;
    cout << d.a << endl; // member "B::a" is inaccessible
    cout << d.b << endl;
    cout << d.c << endl;
    return 0;
}
```

但要注意，只可以为那些派生类可见的名字提供 using 声明，对于不可兼得名字（private），即使我们将它放在 private 访问级别下也不行。

> 这显然是合理的，不然我们通过一个 using 声明就破坏了基类的成员保护。太可怕了！

#### 6.7 class和struct

在 C++ 中，struct 和 class 的**唯一区别**就是默认访问权限和默认继承曲权限的不同。

* class 默认 private 继承，成员是 private 属性
* struct 默认 public 继承，成员是 public 属性

虽然有默认权限，但我们在使用时，最好还是显式声明出访问权限而不是依赖于默认的访问权限。

### 7. 继承中的类作用域

前面提到过每个类都是一个单独的作用域，并且这个作用域处于类的外层作用域之中。而在类的继承情况下，派生类的作用域嵌套在基类作用域当中。派生类会隐藏与基类同名的成员，不过如果我们想使用基类中的同名变量或函数，可以使用作用域运算符：

``` c++
struct B {
    int val = 10;
    void f() { cout << "B: " << val << endl; }
};

struct D : public B {
    int val = 20;
    // 即使函数签名不一致,基类成员的同名函数也会被隐藏掉
    void f(int x) {
        cout << B::val << endl; // 10
        cout << "D: " << val << endl; // D: 20
        f();	//  error: no matching function for call to ‘D::f()’
    }
};
```

**编程规范：除了覆盖继承而来的虚函数之外，派生类最好不要重用其他定义在基类的名字。**

对于重载函数，派生类会隐藏基类**所有**同名的重载函数：

``` c++
struct B {
    void f()      { puts("B::f(void)"); }
    void f(int a) { puts("B::f(int)"); }
};

struct D : public B {
    void f(int a, int b) { puts("D::f(int,int)"); }
};

void func()
{   
    D d;
    d.f(1, 1); // D::f(int,int)
    // d.f(); // too few arguments in function call
}
```

因此如果我们希望基类的所有重载版本在派生类中都是可见的，那他派生类就需要覆盖所有版本，或者一个因为不覆盖。不过一种更好的解决办法是使用 `using` 声明，这样我们就无需覆盖基类中的每一个版本了。另外，由于 `using` 声明只是指定了一个名字而不指定具体的形参列表，所以一条基类成员函数的 `using` 声明语句就可以把该函数的所有重载实例添加到派生类作用域中。此时，派生类只需要定义其特有的函数即可：

``` c++
struct B {
    void f()         { puts("B::f(void)"); }
    void f(int a)    { puts("B::f(int)"); }
    void f(string s) { puts("B::f(string)"); }
};

struct D : public B {
    // using的位置并无要求
    void f(int a, int b) { puts("D::f(int,int)"); }
    using B::f;
};

void func()
{   
    D d;
    d.f(); 
    d.f(1);
    d.f("hello");
    d.f(1, 1);
}
```

### 8. 名字查找与继承

假定我们调用 `p->mem()` 或 `r.mem()`，则依次执行以下 4 个步骤：

1. 首先确定 `p` 的静态类型。因为我们调用的是一个成员，因此 `p` 一定是类类型。
2. 在 `p` 的静态类型对应的类中查找 `mem`。如果找不到，在继承链中往上查找。如果最终找不到，报错。
3. 一旦找到了 `mem`，进行类型检查。如果调用不合法，报错。
4. 如果 `mem` 是虚函数，且是通过指针 `p` 或引用 `r` 调用，那么编译器产生的代码将在运行时根据动态类型确定到底运行该虚函数的哪个版本；反之，编译器将产生一个常规函数调用。

<font color=blue>一如既往，**名字查找先于类型检查**，毕竟先通过名字查找找到对应函数才能进行类型检查。</font>

### 9. 构造函数与拷贝控制

和其它类一样，位于继承体系中的类也需要控制当其对象执行一系列操作时发生什么样的行为，这些操作包括创建、拷贝、移动、赋值和析构。

#### 9.1 虚析构函数

继承关系对基类拷贝控制最直接的影响是基类通常应该定义一个虚析构函数，这样我们就能动态分配并销毁继承体系中的对象了。

``` c++
class Base {
public:
    ~Base() { puts("base"); }
};

class Derived : public Base {
public:
    ~Derived() { puts("derived"); }
};

void test()
{
    Base *d = new Derived();
    delete d; // base
}
```

这里我们通过基类指针定义了一个指向派生类的对象，但是当我们 `delete` 时，调用的是基类的虚构函数，这显然不符合我们的本意。

``` C++
class Base {
public:
    // 虚函数
    virtual ~Base() { puts("base"); }
};

class Derived : public Base {
public:
    ~Derived() { puts("derived"); }
};

class Derived2 : public Derived {
public:
    ~Derived2() { puts("derived2"); }
};

int main() 
{
    Base *d = new Derived();
    delete d; 
    Derived *d2 = new Derived2();
    delete d2;
    return 0;
}

// derived
// base
// derived2
// derived
// base
```

和其他虚函数一样，析构函数的虚属性也会被继承。只要基类的析构函数是虚函数，就能确保当我们 delete 基类指针使将运行正确的析构版本。

而且我们可以发现，调用派生类的析构函数会同时调用基类的析构函数，这和非动态对象的析构函数执行过程是一样的。

``` c++
class Base {
public:
    virtual ~Base() { puts("base"); }
};

class Derived : public Base {
public:
    ~Derived() { puts("derived"); }
};

class Derived2 : public Derived {
public:
    ~Derived2() { puts("derived2"); }
};

int main() 
{
    Derived d;
    Derived2 d2;
    return 0;
}

// derived
// base
// derived2
// derived
// base
```

之前我们曾介绍过一条经验准则，即一个类如果需要析构函数，那么它也同样需要拷贝和赋值操作。基类的析构函数并不遵循上述准则，它是一个重要的例外。一个基类总是需要析构函数，而且它能将析构函数设定为虚函数。

基类需要一个虚析构函数这一事实还会对基类和派生类的定义产生另外一个影响：如果一个类定义了析构函数，即使它通过 `=default` 的形式使用了合成的版本，编译器也不会为这类合成移动操作。

#### 9.2 合成拷贝控制与继承

基类或派生类的合成拷贝控制成员的行为与其它合成的构造函数、赋值运算符或析构函数类似：它们对类本身的成员依次进行初始化、赋值和销毁的操作。**此外，这些合成成员还负责使用<font color=blue>直接基类</font>中对应的操作对一个对象的直接积累部分进行初始化、赋值和销毁的操作**。例如：

``` c++
class Base {
public:
    Base(int _v1 = 0) : v1(_v1) {}
    Base& operator=(const Base &b) {
        puts("Base::op=");
        v1 = b.v1;
        return *this;
    }
    virtual ~Base() = default;
    int v1;
};

class Derived : public Base {
public:
    Derived(int _v1 = 0, int _v2 = 0)
        : Base(_v1), v2(_v2) {}
    int v2;
};

class Derived2 : public Derived {
public:
    Derived2(int _v1 = 0, int _v2 = 0, int _v3 = 0)
        : Derived(_v1, _v2), v3(_v3) {}
    int v3;
};

Derived2 a(1, 2, 3);
Derived2 b(10, 20, 30);

int main() 
{
    a = b;
    cout << a.v1 << ' ' << a.v2 << ' ' << a.v3 << endl;
    // Base::op=
    // 10 20 30
    return 0;
}
```

这里我们使用 `Derived2` 的合成的赋值运算符进行赋值，这个合成的赋值运算符会拷贝 `Derived2` 自己的成员变量 `v3`，然后调用直接基类 `Derived` 的赋值运算符。而由于 `Derived`  没有定义赋值运算符，所以它的默认行为是拷贝它自己的成员变量 `v2` 然后调用它的直接基类 `Base` 的赋值运算符。

如果我们定义了 `Derived` 的赋值运算符：

``` C++
class Base {
public:
    Base(int _v1 = 0) : v1(_v1) {}
    Base& operator=(const Base &b) {
        puts("Base::op=");
        v1 = b.v1;
        return *this;
    }
    virtual ~Base() = default;
    int v1;
};

class Derived : public Base {
public:
    Derived(int _v1 = 0, int _v2 = 0)
        : Base(_v1), v2(_v2) {}
    Derived& operator=(const Derived &b) {
        puts("Derived:op=");
        v2 = b.v2;
        return *this;
    }
    int v2;
};

class Derived2 : public Derived {
public:
    Derived2(int _v1 = 0, int _v2 = 0, int _v3 = 0)
        : Derived(_v1, _v2), v3(_v3) {}
    int v3;
};

Derived2 a(1, 2, 3);
Derived2 b(10, 20, 30);

int main() 
{
    a = b;
    cout << a.v1 << ' ' << a.v2 << ' ' << a.v3 << endl;
    // Derived:op=
    // 1 20 30
    return 0;
}
```

`Derived2` 的合成赋值运算符依然拷贝自己的成员变量 `v3`，然后调用它的直接基类 `Derived` 的赋值运算符。此时由于 `Derived` 定义了赋值运算符，因此他会执行自定义的赋值运算符的内容，由于我们自定义的赋值运算符只是拷贝了 `Derived` 的成员变量 `v2`，所以这里 `Derived2` 的赋值运算符只能拷贝 `v3` 和 `v2`。

那如果我们自定义了 `Derived2` 的赋值运算符呢？

``` c++
class Base {
public:
    Base(int _v1 = 0) : v1(_v1) {}
    Base& operator=(const Base &b) {
        puts("Base::op=");
        v1 = b.v1;
        return *this;
    }
    virtual ~Base() = default;
    int v1;
};

class Derived : public Base {
public:
    Derived(int _v1 = 0, int _v2 = 0)
        : Base(_v1), v2(_v2) {}
    Derived& operator=(const Derived &b) {
        puts("Derived:op=");
        v2 = b.v2;
        return *this;
    }
    int v2;
};

class Derived2 : public Derived {
public:
    Derived2(int _v1 = 0, int _v2 = 0, int _v3 = 0)
        : Derived(_v1, _v2), v3(_v3) {}
    Derived2& operator=(const Derived2 &b) {
        puts("Derived2::op=");
        v3 = b.v3;
        return *this;
    }
    int v3;
};

Derived2 a(1, 2, 3);
Derived2 b(10, 20, 30);

int main() 
{
    a = b;
    cout << a.v1 << ' ' << a.v2 << ' ' << a.v3 << endl;
    // Derived2::op=
    // 1 2 30
    return 0;
}
```

由于我们自定义了 `Derived2` 的赋值运算符，所以这里只会执行自定义赋值运算符函数体中的内容 —— 拷贝 `v3`。**注意此时不会调用其基类的赋值运算符。**

不过注意对于构造和析构函数来说，即使我们自定义了派生类的构造和析构函数，也依然会执行直接基类的构造和析构函数，这是因为编译器实现上会将基类的构造函数/析构函数代码插入到派生类当中：

``` C++
struct Foo {
    Foo() { puts("Foo::ctor"); }
    ~Foo() { puts("Foo::~"); }
};

struct Bar : public Foo {
    // 自定义了Bar的构造和析构行为
    Bar() { puts("Bar::ctor"); }
    ~Bar() { puts("Bar::~"); }
};

int main() 
{
    Bar b;
    // Foo::ctor
    // Bar::ctor
    // Bar::~
    // Foo::~
    return 0;
}
```

拷贝构造函数也是一样，不过要注意的是，在派生类的拷贝构造函数当中，会默认调用直接基类的默认构造函数而不是拷贝构造函数。

``` c++
struct Foo {
    Foo()           { puts("Foo::ctor"); }
    Foo(const Foo&) { puts("Foo::copy ctor"); }
};

struct Bar : public Foo {
    // 自定义了Bar的构造和析构行为
    Bar()           { puts("Bar::ctor"); }
    Bar(const Bar&) { puts("Bar::copy ctor"); }
};

int main() 
{
    Bar b;
    Bar c(b);
// Foo::ctor
// Bar::ctor
// Foo::ctor
// Bar::copy ctor
    return 0;
}
```

如果我们想在派生类的拷贝构造函数当中调用基类的拷贝构造函数，可以显式调用基类的拷贝构造函数：

``` C++
struct Foo {
    Foo(int _v1 = 10) : v1(_v1) { puts("Foo::ctor"); }
    Foo(const Foo&)             { puts("Foo::copy ctor"); }
};

struct Bar : public Foo {
    // 自定义了Bar的构造和析构行为
    Bar(int _v2 = 1000) : v2(_v2) { puts("Bar::ctor"); }
    // 调用直接基类的copy ctor
    Bar(const Bar& x)   : Foo(x)  { puts("Bar::copy ctor"); }
};

int main() 
{
    Bar b(10);
    Bar c(b);
    // Foo::ctor
    // Bar::ctor
    // Foo::copy ctor
    // Bar::copy ctor
    return 0;
}
```

不过，虽然派生类自定义的拷贝构造函数依然可以调用直接基类的构造函数，但派生类自己的拷贝构造行为被我们写死在了函数体当中，也即此时他不会默认拷贝派生类自己的成员变量：

``` c++
struct Foo {
    Foo(int _v1 = 10) : v1(_v1) { puts("Foo::ctor"); }
    Foo(const Foo&) { puts("Foo::copy ctor"); }
    int v1;
};

struct Bar : public Foo {
    // 自定义了Bar的构造和析构行为
    Bar(int _v2 = 1000) : v2(_v2) { puts("Bar::ctor"); }
    Bar(const Bar& x) : Foo(x) { puts("Bar::copy ctor"); }
    int v2;
};

int main() 
{
    Bar b(10);
    cout << b.v1 << ' ' << b.v2 << endl;
    // Foo::ctor
    // Bar::ctor
    // 10 10
    Bar c(b);
    cout << c.v1 << ' ' << c.v2 << endl;
    // Foo::copy ctor
    // Bar::copy ctor
    // 791099984 32711
    return 0;
}
```

在上面的代码中，我们没有在拷贝构造中手动完成对象的拷贝工作，因此这里对象 `c` 的值是未定义的。

#### 9.3 派生类中删除的拷贝构造和基类的关系

* 如果在基类中有一个不可访问或删除掉的析构函数，则派生类中合成的默认的拷贝（移动）构造函数是被删除的，因为编译器无法销毁派生类对象的基类部分
* 如果基类中的默认构造函数、拷贝（移动）构造函数、拷贝（移动）赋值运算符或析构函数是被删除的或不可访问的，则派生类中对应的成员函数是被删除的，因为编译器不能使用基类成员来执行派生类对象积累部分的构造、拷贝、移动或销毁操作

#### 9.4 移动操作与继承

前面提到过，大多数基类都需要定义一个虚析构函数。因此在默认情况下，积累通常不会有合成的移动操作，而且在它的派生类中也没有合成的移动操作。

因为基类缺少移动操作会阻止派生类拥有自己的合成移动操作，所以当我们确实需要执行移动操作时，应该首先在基类中进行定义：

``` c++
struct Foo {
    Foo() = default;

    Foo(Foo &&) = default;
    Foo& operator=(Foo &&) = default;

    Foo(const Foo&) = default; 
    Foo& operator=(const Foo&) = default;
    
    virtual ~Foo() = default;
};
```

并且，由于我们定义了移动操作，所以编译器不会为我们生成合成的拷贝操作，因此我们还需要定义拷贝构造和拷贝赋值。

### 10. 派生类的拷贝控制成员

#### 10.1 构造、赋值和析构

派生类构造函数在其初始化阶段不但要初始化派生类自己的成员，还负责初始化派生类对象的基类部分。**注意初始化基类部分只能通过基类的构造函数**：

``` C++
class Foo {
public:
    Foo()                     {puts("Foo::ctor");}
	Foo(double _pi) : pi(_pi) {puts("Foo::ctor(double)");}
    
    Foo(const Foo&)            = default; 
    Foo(Foo &&)                = default;
    
    Foo& operator=(Foo &&)     = default;
    Foo& operator=(const Foo&) = default;
    
    virtual ~Foo() = default;
private:
    double pi;
};

class Bar : public Foo {
public:
    Bar(double _pi, int _val, string _s)
        : Foo(_pi), val(_val), s(_s)
        {puts("Bar::ctor");}
private:
    int val;
    string s;
};
```

派生类的拷贝和移动构造函数在拷贝和移动自有成员的同时，也要拷贝和移动积累部分的成员。类似的，派生类的拷贝赋值运算符和移动赋值运算符也需要为其基类部分赋值。**为基类部分构造或赋值的操作都是通过<font color=blue>基类的接口</font>来实现的。**

如果我们没有显式调用基类的构造函数来初始化，编译器会执行基类的默认构造函数。但如果我们没有显式调用基类的赋值函数来赋值，编译器不会执行基类部分的赋值运算符。因为**构造是必须的，赋值不是必须的**：

``` c++
struct Foo {
    Foo()             : pi(99.99) {}
	Foo(double _pi)   : pi(_pi)   {}
    Foo(const Foo &x) : pi(x.pi)  {}
    
    Foo& operator=(const Foo &x) {
        pi = x.pi;
        return *this;
    }

    virtual ~Foo() = default;

    double pi;
};

struct Bar : Foo {
    Bar(double _pi, int _val, string _s) : Foo(_pi), val(_val), s(_s) {}
    Bar(const Bar &x)                    : val(x.val), s(x.s) {}
    
    Bar& operator=(const Bar &x) {
        val = x.val;
        s = x.s; 
        return *this;
    }
    
    int val;
    string s;
};

int main() 
{
    Bar b(3.14, 1024, "hello");
    Bar a(12.34, 1234, "1234");

    // 拷贝构造
    Bar c(b);
    cout << c.pi << ' ' << c.val << ' ' << c.s << endl; // 99.99 1024 hello
    // 赋值运算符
    a = b;
    cout << a.pi << ' ' << a.val << ' ' << a.s << endl; // 12.34 1024 hello
    return 0;
}
```

在上面的派生类 `Bar` 中，我们在其拷贝构造和拷贝赋值部分只完成了派生类对象部分构造和拷贝操作。

* 在拷贝构造函数中：派生类调用基类的默认构造函数将积累部分的 `pi` 初始化为 99.99
* 在拷贝赋值函数中：派生类只完成了自身成员变量的赋值，因此这里派生类对象 `a` 中基类部分的 `pi` 的值仍然是 12.34

无论怎样，这都不是我们想要的结果。正确的写法应该是通过动态绑定，调用基类的接口完成构造和赋值：（移动赋值和拷贝赋值是同样的道理）

``` c++
struct Foo {
    Foo() : pi(99.99) {}
	Foo(double _pi) : pi(_pi) {}

    Foo(const Foo &x) : pi(x.pi) {}
    Foo& operator=(const Foo &rhs) {
        pi = rhs.pi;
        return *this;
    }

    virtual ~Foo() = default;

    double pi;
};

struct Bar : Foo {
    Bar(double _pi, int _val, string _s)
        : Foo(_pi), val(_val), s(_s) {}
    Bar(const Bar &x) 
        : Foo(x), // 显式调用基类的拷贝构造函数 
    	  val(x.val), s(x.s) {}
    Bar& operator=(const Bar &rhs) {
        // 显式调用基类的赋值运算符
        Foo::operator=(rhs);
        val = rhs.val;
        s = rhs.s; 
        return *this;
    }
    
    int val;
    string s;
};

int main() 
{
    Bar b(3.14, 1024, "hello");
    Bar a(12.34, 1234, "1234");

    // 拷贝构造
    Bar c(b);
    cout << c.pi << ' ' << c.val << ' ' << c.s << endl;
    // 赋值运算符
    a = b;
    cout << a.pi << ' ' << a.val << ' ' << a.s << endl;
    return 0;
}
// 3.14 1024 hello
// 3.14 1024 hello
```

#### 10.2 在构造函数和析构函数中调用虚函数

当我们在构造函数或析构函数中调用了虚函数，则我们应该执行与构造函数或析构函数所属类型相同的虚函数版本。这意味着，**在构造函数和析构函数当中，虚函数不会进行动态绑定从而绑定到派生类的版本。**原因也很简单：

* 在基类的构造函数当中，派生类部分还未初始化。
* 在基类的析构函数中，派生类部分已经销毁了。

#### 10.3 “继承”的构造函数

<font color=blue>**在 C++ 中，“继承”的构造函数指的是子类通过“继承”基类的构造函数来实现自己的构造函数。**</font>C++11 引入了“继承”构造函数的概念，允许子类使用基类的构造函数，而无需重新定义。使用 `using` 关键字可以让基类的构造函数直接在派生类中可用。这样可以**减少代码重复，同时让子类更简洁**。

假设我们有一个基类 `Base` 和一个派生类 `Derived`：

``` c++
class Base {
public:
    Base(int x)    { cout << "Base constructor with int: " << x << endl;    }
    Base(double y) { cout << "Base constructor with double: " << y << endl; }
};

class Derived : public Base {
public:
    using Base::Base;
};

int main() 
{
    Derived d1(16);
    Derived d2(3.14);
    return 0;
}
// Base constructor with int: 16
// Base constructor with double: 3.14
```

我们对这里的继承加上了引号，是因为这种继承并不是像普通成员函数那样的继承。C++ 中的继承构造函数是在编译器层面实现的。编译器在编译时生成了一组**“中间构造函数”**，以便让派生类可以直接调用基类的构造函数。

##### 1. 实现原理

当我们在派生类中使用 `using Base::Base` 来继承基类的构造函数时，编译器实际上为每个基类构造函数在派生类中生成了一个**隐式的构造函数**。这些隐式构造函数将接收的参数直接传递给基类的对应构造函数，然后完成派生类的初始化。这样就实现了构造函数的继承。

通常情况下，`using` 声明语句只是令某个名字在当前作用域内可见。而当作用域构造函数时，`using` 声明语句将令编译器产生代码。**对于基类的每个构造函数，编译器都生成一个与之对应的派生类构造函数。**换言之，对于基类的每个构造函数，编译器都生成一个形参列表完全相同的构造函数。其形式如下：

``` c++
derived(parms) : base(args) {}
// derived：派生类的名字
// base：基类的名字
// params：构造函数的形参
// args：将派生类的形参传递给基类的构造函数
```

例如，考虑以下代码：

```cpp
class Base {
public:
    Base(int x) { /* ... */ }
    Base(double y) { /* ... */ }
};

class Derived : public Base {
    using Base::Base; // 继承构造函数
};
```

在编译时，编译器会自动为 `Derived` 类生成如下的构造函数：

```cpp
class Derived : public Base {
public:
    // 编译器生成的桥接构造函数
    Derived(int x)    : Base(x) {}   // 调用 Base(int)
    Derived(double y) : Base(y) {}   // 调用 Base(double)
};
```

##### 2. “继承”的构造函数的特点

* 和普通成员的 `using` 声明不一样，一个构造函数的 `using` 声明不会改变该构造函数的访问级别。即，不管 `using` 声明出现在哪儿，基类的私有构造函数在派生类中还是一个私有构造函数；受保护的构造函数和公有的构造函数也是同样的规则。

* 一个 `using` 声明不能指定 `explicit` 和 `constexpr`。如果基类的构造函数是 `explicit` 或者 `constexpr`，则继承的构造函数也拥有相同的属性。

* 当一个基类构造函数含有**默认实参时**，这些实参并不会被继承。此外，派生类将获得多个继承的构造函数，其中每个构造函数分别省略掉一个有默认实参的形参。例如：

  ``` C++
  class Base {
  public:
      Base(int _a = 1, int _b = 2, int _c = 3) 
      : a(_a), b(_b), c(_c)  {}
  private:
      int a, b, c;
  };
  
  class Derived : public Base {
  public:
      using Base::Base;
  };
  
  /*====================== 编译器生成 ========================*/
  class Derived : public Base {
  public:
      Derived(int _a, int _b, int _c) : Base(_a, _b, _c) {}
     	Derived(int _a, int _b) : Base(_a, _b) {}
     	Derived(int _a) : Base(_a) {}
     	Derived() : Base() {}
  };
  /*========================================================*/
  ```

虽然在派生类继承过来的构造函数中忽略掉了默认实参（默认值），但当我们通过 `Base` 进行构造的时候，依然可以在 `Base` 中使用这些默认值，例如：

``` cpp
Derived d0;                // 1 2 3
Derived d1(10);            // 10 2 3
Derived d2(10, 20);        // 10 20 3
Derived d3(10, 20, 30);    // 10 20 30
```

如果基类含有多个构造函数，则除了以下这两种外，大多数时候派生类会继承所有这些构造函数：

1. 如果派生类定义的构造函数与基类的构造函数具有相同的参数列表。此时继承而来的构造函数会被派生类定义的构造函数覆盖。
2. 默认、拷贝和移动构造函数不会被继承。这些构造函数按照正常规则被合成。继承的构造函数不会被作为用户定义的构造函数来使用，因此，如果一个类只含有继承的构造函数，则它也将拥有一个合成的默认构造函数。

### 11. 容器与继承

当我们希望在容器中存放具有继承关系的对象时，我们实际上存放的通常是基类的指针（更好的选择是智能指针）。这些指针可以指向基类对象，也可以指向派生类对象。

## 十、模板

### 1. 实例化

#### 1.1 C++ 模板在编译期进行实例化

1. **模板不是实际代码**

模板本身只是一份"蓝图"或"配方"，编译器在看到模板定义时并不会立即生成任何机器代码。例如下面的代码，编译器不会生成任何机器代码：

``` c++
template<typename T>
T add(const T &a, const T &b) {
    return a + b;
}
```

如果我们实际使用该模板，例如：

``` C++
template<typename T>
T add(const T &a, const T &b) {
    return a + b;
}

int c = add(1, 2);
double d = add(1.1, 2.2); 
```

生成机器代码为：

``` assembly
c:
        .zero   4
d:
        .zero   8
int add<int>(int const&, int const&):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        mov     QWORD PTR [rbp-16], rsi
        mov     rax, QWORD PTR [rbp-8]
        mov     edx, DWORD PTR [rax]
        mov     rax, QWORD PTR [rbp-16]
        mov     eax, DWORD PTR [rax]
        add     eax, edx
        pop     rbp
        ret
double add<double>(double const&, double const&):
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        mov     QWORD PTR [rbp-16], rsi
        mov     rax, QWORD PTR [rbp-8]
        movsd   xmm1, QWORD PTR [rax]
        mov     rax, QWORD PTR [rbp-16]
        movsd   xmm0, QWORD PTR [rax]
        addsd   xmm0, xmm1
        pop     rbp
        ret
__static_initialization_and_destruction_0():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     DWORD PTR [rbp-24], 2
        mov     DWORD PTR [rbp-20], 1
        lea     rdx, [rbp-24]
        lea     rax, [rbp-20]
        mov     rsi, rdx
        mov     rdi, rax
        call    int add<int>(int const&, int const&)
        mov     DWORD PTR c[rip], eax
        movsd   xmm0, QWORD PTR .LC0[rip]
        movsd   QWORD PTR [rbp-16], xmm0
        movsd   xmm0, QWORD PTR .LC1[rip]
        movsd   QWORD PTR [rbp-8], xmm0
        lea     rdx, [rbp-16]
        lea     rax, [rbp-8]
        mov     rsi, rdx
        mov     rdi, rax
        call    double add<double>(double const&, double const&)
        movq    rax, xmm0
        mov     QWORD PTR d[rip], rax
        nop
        leave
        ret
_GLOBAL__sub_I_c:
        push    rbp
        mov     rbp, rsp
        call    __static_initialization_and_destruction_0()
        pop     rbp
        ret
.LC0:
        .long   -1717986918
        .long   1073846681
.LC1:
        .long   -1717986918
        .long   1072798105
```

2. **实例化发生在使用时**
   当代码中实际使用模板（如创建模板类对象或调用模板函数）时，编译器会根据具体类型参数生成特定的代码。

3. **生成具体类型的代码**
   例如，当你使用 `std::vector<int>` 时，编译器会生成专门处理 `int` 类型的 vector 代码，这个过程就是实例化。

4. **每个类型组合产生独立代码**
   `std::vector<int>` 和 `std::vector<std::string>` 会产生完全不同的机器代码。

#### 1.2 为什么不是运行期

1. **性能考虑**
   在编译期完成所有类型特化可以避免运行时的类型检查和转换开销。
2. **类型安全**
   编译期实例化保证了所有类型检查都在编译时完成，不会在运行时出现类型错误。
3. **无运行时机制**
   C++ 运行时没有模板的概念，也没有动态生成代码的能力。

#### 1.3 验证方法

你可以通过以下方式验证模板实例化是编译期的：

1. 查看编译器生成的汇编代码，会发现不同的实例化版本
2. 模板错误大都是**编译错误**，而不是运行时错误
3. 使用 `typeid` 或 `sizeof` 等编译期操作可以证明实例化发生在编译期

```cpp
template <typename T>
void checkSizeIsHalfByte() {
    static_assert(sizeof(T) == 4, "Size must be 4");
}

int main() 
{
    checkSizeIsHalfByte<int>();
    checkSizeIsHalfByte<double>(); //  error: static assertion failed: Size must be 4
    return 0;
}
```

### 2. 非类型模板参数

在 C++ 模板中，非类型模板参数（Non-type Template Parameters）允许你使用具体的值（而非类型）作为模板参数。非类型参数可以是以下几种类型：

1. **整型**（包括整数、字符、枚举等）
2. **指针或引用**（指向对象或函数）
3. **C++20 起支持浮点类型和字面量类类型（Literal Class Types）**

#### 2.1 整型非类型参数

```CPP
template <int N>
struct Array {
    int data[N];  // 使用非类型参数 N 指定数组大小
};

int main() {
    Array<5> arr; // 实例化一个大小为 5 的数组
    static_assert(sizeof(arr.data) / sizeof(int) == 5, "Size must be 5");
}
```

- `N` 必须是 **常量表达式**（编译期可知的值）。
- 例如 `Array<5+3>` 是合法的，因为 `5+3` 是常量表达式。

#### 2.2 指针或引用非类型参数

```cpp
template <const char* Message>
struct Logger {
    void print() { std::cout << Message << std::endl; }
};

// 必须具有静态存储期（全局或 static）
const char globalMsg[] = "Hello, World!";
const char* DynamicMsg = new char[12]{"hello,world"};

int main() 
{
    Logger<globalMsg> logger; // 使用全局变量的地址作为非类型参数
    logger.print();  // 输出 "Hello, World!"
    
    Logger<DynamicMsg> logger2; // error: the value of variable "DynamicMsg" (declared at line 31) cannot be used as a constant
    logger2.print();
    return 0;
}
```

- 指针或引用必须指向 **具有<font color=blue>静态</font>生存期的对象**（全局变量、`static` 变量等）。
- C++17 后放宽限制，允许某些情况下的 `constexpr` 变量。

#### 2.3 函数指针非类型参数

```cpp
template<void(*func)()>
struct Caller {
    void operator()() const {
        func();
    }
};

void print() {
    cout << "hello" << endl;
}

int main()
{
    Caller<print> caller;
    caller(); // hello
    return 0; 
}
```

#### 2.4 C++20 支持浮点非类型参数

```cpp
template <double Value>
struct FloatWrapper {
    constexpr double get() { return Value; }
};

int main() {
    FloatWrapper<3.14> wrapper;
    static_assert(wrapper.get() == 3.14);
}
```

#### 2.5 关键限制

- **必须是编译期常量**（`constexpr`）。
- **不能使用动态内存或局部变量**（除非是 `static` 或 `constexpr`）。
- **指针/引用必须指向静态存储期的对象**。

#### 2.6 典型应用

- 固定大小的数组（如 `std::array<T, N>`）。
- 编译期计算（如模板元编程）。
- 策略模式（传入函数指针或策略类）。

示例：编译期阶乘计算

```cpp
template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

int main() {
    static_assert(Factorial<5>::value == 120); // 编译期计算
}
```

在 `struct Factorial` 中对于 `value` 的声明中，`static` 和 `constexpr` 关键字必不可少：

1. `static` 在这里的作用是让 `value` 成为**类的静态成员**，而不是实例成员。这意味着：
   * **单一实例**：无论创建多少个 `Factorial<N>` 的实例，`value` 只有一份存储
   * **直接访问**：可以通过 `Factorial<N>::value` 直接访问，不需要创建类的对象
   * **编译期可用**：静态成员在编译期就存在，适合模板元编程

* 如果去掉 `static`，那么：
  * `value` 会成为每个 `Factorial<N>` 实例的普通成员变量
  * 必须创建对象才能访问它
  * 无法在编译期直接通过 `Factorial<N>::value` 获取值

2. `constexpr` 表示这个值是一个**编译期常量**，这意味着：
   * 编译期计算**：编译器会在编译时就计算出`value`的值**
   * 可用于模板参数**：可以作为非类型模板参数使用**
   * 优化机会**：编译器可以进行更多的优化**
   * 可用于static_assert**：可以在编译期断言中使用

* 如果去掉`constexpr`：
  * 值可能在运行时才计算
  * 不能用于需要编译期常量的场合（如数组大小、模板参数等）
  * 无法使用 `static_assert` 验证结果

在模板元编程中，我们通常需要：

1. **编译期计算**（由`constexpr`保证）
2. **无需实例化即可访问**（由`static`保证）

```cpp
// 正确的写法
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
    // static: 可以直接通过Factorial<N>::value访问
    // constexpr: 保证编译期计算
};

// 如果只写static不写constexpr
template<int N>
struct Factorial {
    static int value;  // 需要在类外定义，且不是编译期常量
};

// 如果只写constexpr不写static
template<int N>
struct Factorial {
    constexpr int value = N * Factorial<N-1>::value;  // 需要创建对象才能访问
};
```

### 3. 模板编译与错误检测

当编译器遇到一个模板定义时，它并不生成代码。只有当我们实例化出模板的一个特定版本时，编译器才会生成代码。这一特性影响了模板中的错误检测，即大多数编译错误在实例化期间报告：

``` c++
template<unsigned N, unsigned M>
int compare(const char (&p1)[N], const char (&p2)[M])
{
    return "hello"; // 没有实例化该模板,编译通过
}

int f()
{
    return "hello";// error: return value type does not match the function type
}
```

对于模板，编译器会在**三个阶段**报告错误：

1. **编译模板本身时**：此时编译器只是进行语法检查，例如忘记括号或者关键字拼错等。
2. **编译器遇到模板使用时**：检查实参数目是否正确，参数类型是否匹配。
3. **模板实例化时：**类型相关的错误。

另外，与普通函数和普通类成员函数的声明和定义分离（分别放在头文件和源文件中）不同，为了生成一个模板的实例化版本，编译器需要掌握函数模板或类模板成员函数的定义。因此，与非模板代码相比，**模板的头文件通常既包含声明也包括定义。**例如：

``` C++
// header.h
template<typename T>
T ret(T x);

// source.cpp
template<typename T>
T ret(T x)
{
    return x;
}

// main.cpp
template<typename T>
T ret(T);

int main()
{
    cout << ret<int>(10) << endl;
    return 0;
}
```

连接器会报错找不到 `ret` 这个函数的定义：

``` shell
/usr/bin/ld: /tmp/cc2BGbdB.o: in function `main':
/home/jyyyyx/cpp/main.cpp:10:(.text+0xd8): undefined reference to `int ret<int>(int)'
collect2: error: ld returned 1 exit status
```

这个错误是**链接错误（linker error）**，表明编译器在生成目标文件后，链接器（`ld`）找不到某个函数的具体实现。具体来说：

```cpp
/usr/bin/ld: /tmp/cc2BGbdB.o: in function `main':
/home/jyyyyx/cpp/main.cpp:10:(.text+0xd8): undefined reference to `int ret<int>(int)'
```

- **错误类型**：`undefined reference`（未定义的引用）
- **问题函数**：`int ret<int>(int)`（一个模板函数 `ret` 的 `int` 特化版本）
- **原因**：你声明并调用了模板函数 `ret`，但链接器找不到它的**定义**（即实现代码）。

模板函数在编译时需要实例化，如果定义不在当前编译单元（`.cpp` 文件），链接器会找不到实现。

### 4. 函数模板

对于函数模板，编译器能自动推导模板参数值的类型，而无需显式指定；与此相反，类模板必须显式指定模板参数的类型。

``` c++
template<typename T, size_t N>
size_t getArraySize(const T (&arr)[N]) {
    return N;
}

int arr[10];

int main() 
{
    cout << getArraySize(arr) << endl; // 10
    return 0;
}
```

### 5. 类模板

#### 5.1 类模板不是一个类型

类模板的名字并不是一个类型，例如下面的 `Array`  并不是一个类型。只有将类模板实例化，才能产生一个类型，例如 `Array<int,5>`。

``` c++
template<typename T, size_t N>
struct Array {
    T arr[N];
};

Array<int, 5> arr;
```

#### 5.2 成员函数不是函数模板

虽然类是一个模板，但类模板的成员函数本身是一个普通函数，不要误认为类模板的成员函数也是模板。我们可以从另一个角度理解，对于实例化后的类型 `Array<int,5>`，它就是一个普通的类。

#### 5.3 模板代码膨胀

同一个类模板的不同实例本质上是不同的类，类模板的每个实例都有其自己版本的成员函数，尽管这些成员函数可能一模一样，但它们仍然是独立的，这意味着：

1. 它们不会共享代码段，编译器会为每个用到的类型参数生成一套完整的类代码，这被称为**"模板代码膨胀"**问题。
2. 在类外定义成员函数时，需要指定模板类型来区分不同实例。

#### 5.4 成员函数惰性实例化

前面我们提到过（十、3），当编译器遇到一个模板定义时，它并不生成代码。只有当我们实例化出模板的一个特定版本时，编译器才会生成代码。这一特性影响了模板中的错误检测，即大多数编译错误在**实例化期间**报告。

特别的，对于类模板的成员函数，只有模板类或模板的成员函数只有在真正被使用时才会被实例化。这意味着编译器不会为未使用的成员函数生成代码。即使某些成员函数在实例化时会导致编译错误，只要这些函数没有被实际调用，程序仍然是合法的。

我们可以进行验证，即使我们完全不使用模板也会报错：???

```C++
template<typename T>
class Verifier {
public:
    void valid() {}
    void invalid() { 
        int s = "hello"; 

        unDeclaredFunc(); // error: there are no arguments to ‘unDeclaredFunc’ that depend on a template parameter, so a declaration of ‘unDeclaredFunc’ must be available [-fpermissive]

        static_assert(false); // error: static assertion failed
                              // static_assert(false);
        
        int N = 10;
        int arr[N];      // error: ISO C++ forbids variable length array ‘arr’ [-Wvla]
    } 
};
```

#### 5.5 作用域

当我们使用类模板类型时必须提供模板实参，但这一规则有一个例外：在类模板自己的作用域中，我们可以直接使用模板名而不提供实参：

``` c++
template<typename T>
class Foo {
public:
    Foo(T _val = T()) : val(_val) {}
    // 无需使用 Foo<T>&
    Foo& operator++() {
        ++ val;
        return *this;
    }
private:
    T val;
};
```

同样的，当我们在类外定义时，出现在类名之后的内容属于类模板自己的作用域：

``` c++
template<typename T>
class Foo {
public:
    Foo(T _val = T()) : val(_val) {}
    // 无需加 <T>
    Foo operator++(int);
private:
    T val;
};

template<typename T>
// 经典的，返回类型需要加 <T>
Foo<T> Foo<T>::operator++(int)
{
    // 无需加 <T>
    Foo tmp = *this;
    ++ this->val;
    return tmp;
}
```

#### 5.6 友元

当一个类包含一个友元声明时，类与友元各自是否是模板是相互无关的：

1. 如果一个类模板包含一个非模板友元，则友元被授予可以访问所有模板实例

``` cpp
template<typename T>
class Bar;

class Foo {
public:
    template<typename T>
    void test(const Bar<T> &b) {
        cout << b.val << endl;
    }
};

template<typename T>
class Bar {
    friend class Foo; // Foo可以访问所有Bar的实例
    T val;
public:
    Bar(T _val) : val(_val) {}
};

int main() 
{
    Bar<int> a(1024);
    Bar<double> b(3.14);
    Foo f;
    f.test(a); // 1024
    f.test(b); // 3.14
    return 0;
}
```

2. 如果友元自身是模板，类可以授予给所有友元模板实例，也可以只授予给特定实例

``` cpp
template<typename T>
class Bar;

template<typename T>
class Cat;

class Foo {
    friend class Bar<int>;                      // Bar的特定实例
    template<typename T> friend class Cat;      // Cat的所有实例，注意不能直接写为 friend class Cat;
public:
    Foo(int _val) : val(_val) {}
private:
    int val;
};

template<typename T>
class Bar {
public:
    void test(const Foo &f) const {
        cout << f.val << endl;
    }
};

template<typename T>
class Cat {
    public:
    void test(const Foo &f) const {
        cout << f.val << endl;
    }
};

Foo f(1024);

int main() 
{
    Cat<int> a;
    a.test(f); // 1024
    Cat<string> b;
    b.test(f); // 1024

    Bar<int> c;
    c.test(f); // 1024
    Bar<string> d;
    //d.test(f); // error: ‘int Foo::val’ is private within this context
    return 0;
}
```

一般来说，我们会将一个类或函数作为友元，但本质上，类不过是一个自定义类型，因此实际上，我们可以将一个**类型**或函数作为友元。

* 这意味着我们可以将一个内置类型作为友元，但此时没有任何作用或效果

允许与类型的友好关系主要方便我们将模板参数作为自己的友元：

``` c++
template<typename T>
class Foo {
    // 将模板类型参数 T 声明为友元，适用于 T 是一个类类型的情况
    friend T;
public:
    Foo(int _val) : val(_val) {}
private:
    int val;
};

class Bar {
public:
    template<typename T>
    void test(const Foo<T> &f) const {
        cout << f.val << endl;
    }
};

int main() 
{
    Foo<Bar> f1(16);
    Foo<int> f2(1024);
    Bar b;
    b.test(f1); // 16
    //b.test(f2); // error: ‘int Foo<int>::val’ is private within this context
    return 0;
}
```

#### 5.7 模板类型别名

1. 简化模板类型参数：

``` c++
template<typename T>
using twin = pair<T, T>;

twin<int> p;
```

2. 固定一个或多个模板参数：

``` c++
template<typename T>
using pint = pair<T, int>;

pint<string> p;
```

#### 5.8 static成员

当我们在类模板中定义了 **static** 成员时，每个类模板实例都有自己的 **static** 成员实例。例如：

``` c++
template<typename T>
class Foo {
public:
    Foo() { ++ val; }
    static std::size_t count();
private:
    static int val;
};

template<typename T>
std::size_t Foo<T>::count() { return val; }

// 我们也可以特殊处理某些类型，即使在同一定义之前定义也是可行的
template<>
int Foo<int>::val = 100;

// 在定义类成员时需要加上模板类型
template<typename T>
int Foo<T>::val = 10;


int main()
{
    // Foo<int>和Foo<string>分别有他们自己的static成员
    Foo<int> f1, f2, f3;
    cout << Foo<int>::count() << endl; // 103
    cout << f1.count() << ' ' << f2.count() << ' ' << f3.count() << endl; // 103 103 103 

    Foo<string> f4, f5;
    cout << Foo<string>::count() << endl; // 12
    cout << f4.count() << ' ' << f5.count() << endl; // 12 12

    return 0;
}
```

#### 5.9 作用域运算符

当我们使用作用域运算符（`::`）来访问类的静态成员和类型成员时，对于普通（非模板）代码，编译器掌握类的定义，因此它知道通过作用域运算符访问的名字是类型成员还是 static 成员。

但是对于模板代码就存在困难。例如，`T` 是一个模板类型参数，当编译器遇到类似 `T::mem` 这样的代码时，它不知道 `mem` 是一个类型成员还是一个 `static` 数据成员，直到实例化时才能知道。但是为了处理模板，编译器必须知道 `mem` 是否表示一个类型。例如：

``` C++
template<typename T>
class Bar {
public:
    void f() {
        T::PII * p;
    }
};
```

其中的 p ，编译器需要知道它到底是一个 PII 类型的指针，还是与一个名为 PII 的静态变量的乘积。

<font color=blue>**默认情况下，C++ 语言假定通过作用域运算符访问的名字不是类型。**</font>因此，如果我们希望使用一个模板类型参数的类型成员，就必须显式告诉编译器该名字是一个类型。我们通过关键字 `typename` 来实现这一点：

``` c++
template<typename T>
class Bar {
public:
    void f() {
        typename T::PII *p;
    }
};
```

注意这里不能用 `class` 代替 `typename`。

#### 5.10 默认模板参数

和普通函数的参数一样，我们可以为函数或类的模板参数提供默认值。实际上 STL 就大量为模板实参提供默认值，例如容器适配器的底层容器，分配器等。提供默认值的规则与普通函数一样，不再赘述。

``` c++
// cmp 有一个默认模板实参 less<T> 和一个默认函数实参 F()
template<typename T, typename F = less<T>>
bool cmp(const T &a, const T &b, F f = F())
{
    return f(a, b);
}

int main()
{
    cout << cmp(1, 2) << endl;
    cout << cmp<int, greater<int>>(1, 2) << endl;
    return 0;
}
```

由于类模板无法推导模板参数，所以我们必须显式指定模板参数，不过当所有模板参数都有默认值时，只需要提供一个空的尖括号即可：

``` c++
template<typename T = int>
class Foo {};
Foo<> f;
```

#### 5.11 成员函数模板

##### 1. 成员函数模板不能是虚函数

在C++中，成员函数模板不能声明为虚函数（`virtual`），这是由语言标准明确规定的。主要原因包括以下几个方面：

###### 1. **虚函数表的实现限制**

- 虚函数的调用通过**虚函数表（vtable）**实现，每个包含虚函数的类会在编译时生成一个固定的虚函数表，表中存储了虚函数的实际地址。
- 成员函数模板的本质是**代码生成器**，会在实例化时根据模板参数生成不同的函数实例（如 `foo<int>()` 和 `foo<double>()` 是两个不同的函数）。
- 虚函数表需要在编译时确定大小和内容，但模板实例化是运行时（或链接时）的行为，导致无法在编译时为所有可能的模板实例预留虚函数表条目。

###### 2. **语义矛盾**

- 虚函数的意义是支持**运行时多态**，即通过基类指针或引用调用派生类实现的函数。
- 函数模板的意义是**编译时多态**，即根据模板参数生成不同的代码。
- 若允许虚函数模板，则需要在运行时动态决定调用哪个模板实例（如 `obj->template foo<int>()` 还是 `obj->template foo<double>()`），这与C++的编译时模板实例化机制冲突。

替代方案

如果需要结合运行时多态和模板的灵活性，可以通过以下方式实现类似功能：

方案1：将模板函数包装为非模板虚函数

```cpp
class Base {
public:
    virtual void foo(int arg) = 0;    // 非模板虚函数
    virtual void foo(double arg) = 0; // 重载
};

class Derived : public Base {
public:
    void foo(int arg) override { /* int 实现 */ }
    void foo(double arg) override { /* double 实现 */ }
};
```

方案2：使用类型擦除（Type Erasure）

例如，通过 `std::any` 或自定义包装器将类型信息延迟到运行时：

```cpp
class Base {
public:
    virtual void foo(std::any arg) = 0; // 类型擦除
};

class Derived : public Base {
public:
    void foo(std::any arg) override {
        if (arg.type() == typeid(int)) {
            auto val = std::any_cast<int>(arg);
            // 处理 int
        } else if (arg.type() == typeid(double)) {
            auto val = std::any_cast<double>(arg);
            // 处理 double
        }
    }
};
```

方案3：CRTP（编译时多态替代运行时多态）

如果不需要运行时多态，可以使用CRTP（Curiously Recurring Template Pattern）：

```cpp
template <typename Derived>
class Base {
public:
    template <typename T>
    void foo(T arg) {
        static_cast<Derived*>(this)->foo_impl(arg);
    }
};

class Derived : public Base<Derived> {
public:
    template <typename T>
    void foo_impl(T arg) { /* 具体实现 */ }
};
```

##### 2. 非模板类的成员模板

``` C++
class DebugDelete {
public:
    DebugDelete(std::ostream &_os = std::cerr) : os(_os) {}
    
    template<typename T>
    void operator()(T *p) const {
        os << "deleting unique_ptr" << std::endl;
        delete p;
    }
private:
    std::ostream &os;
};

int main()
{
    unique_ptr<int, DebugDelete> p(new int(10), DebugDelete());
    return 0;
}
```

##### 3. 模板类的成员模板

``` c++
template<typename T>
class Vector {
public:
    // 模板成员函数
    template<typename It>
    Vector(It b, It e) : data(b, e) {} 
    size_t size() const { return data.size(); }
    T& operator[](size_t i) { return data[i]; }
private:
    vector<T> data;
};

int main()
{
    vector<int> a{1, 2, 3, 4, 5};
    Vector<int> v(a.begin(), a.end());
    for(size_t i = 0; i < v.size(); i ++ )
        cout << v[i] << endl;
    return 0;
}
```

与类模板的普通函数成员不同，成员模板是函数模板。当我们在类模板外定义一个成员模板时，必须同时为类模板和成员模板提供模板参数列表。类模板的参数列表在前，后跟成员自己的模板参数列表。

``` C++
template<typename T>
class Foo {
public:
    template<typename U>
    U func(U x);
};

// 在类外定义成员函数
template<typename T>
template<typename U>
U Foo<T>::func(U x)
{
    return x;
}

int main()
{
    Foo<int> f;
    cout << f.func(10) << endl;
    cout << f.func(3.14) << endl;
    cout << f.func("String") << endl;
    return 0;
}
```

#### 5.12 控制模板实例化

模板被使用时才会实例化意味着相同的实例可能出现在多个对象文件中。当两个或多个独立编译的源文件使用了相同的模板并提供了相同的模板参数时，每个文件中就都会有该模板的一个实例。

在大系统中，在多个文件中实例化相同模板的额外开销可能非常大。在新标准中，我们可以通过<font color=blue>**显式实例化（explicit instantiation）**</font>。一个显式实例化有如下形式：

``` c++
extern template declaration;	// instantiation declare
template declaration;			// instantiation define
```

`declaration` 是一个类或函数声明，其中所有模板参数已被替换为模板实参。例如：

``` c++
extern template class Foo<int>;					        // declare
template        int compare(const int&, const int&);	// define
```

可以发现只需要在声明的前面加上一个 `template` 即可。

当编译器遇到 `extern` 模板声明时，它不会在本文件中生成实例化代码。因为将一个实例化声明为 `extern` 就表示承诺在程序其他位置有该实例化的一个非 `extern` 的定义。对于一个给定的实例化版本，只能有一个定义，但可以有多个 `extern` 声明。

由于编译器在使用一个模板时自动对其实例化，因为 `extern` 声明必须出现在任何使用此实例化版本的代码之前。

> 实例化定义不能放在头文件中，不然被如果多个源文件引入就会导致重复定义错误。

**一个类模板的实例化定义会实例化该模板的所有成员**（包括构造函数、析构函数、内联的成员函数和友元函数等）。这意味着，编译器将生成这些成员函数对应的代码。

因此，与处理类模板的普通实例化不同（只实例化使用到的成员函数），编译器会实例化该类的所有成员。即使我们不使用某个成员，他也会被实例化。因此，**我们用来显式实例化一个类模板的类型，必须能用于模板的所有成员。**

例如，`NoDefault` 是一个没有默认构造函数的类型，我们不可以用它来显示实例化 `vector<NoDefault>`。因为编译器需要实例化 `vector` 的构造函数，尽管我们可能并不用到它，由于该类型没有构造函数，因此会编译失败：

``` c++
class NoDefault {
public:
    NoDefault() = delete;
};

template class vector<NoDefault>;   // 编译错误
vector<NoDefault> v;                // 编译通过
```

### 6. 引用折叠

在 C++ 中我们是不能定义“引用的引用”的，但是如果我们间接创建（如类型别名，模板参数）了一个“引用的引用”，则这些引用形成了“折叠”。

C++ 中的**引用折叠（reference collapsing）**规则，是当多层引用组合在一起时，编译器应用的一套规则，用来决定最终的引用类型。

引用折叠规则的基本原则是：

* **左值引用优先于右值引用。**即：如果有一个左值引用参与折叠，那么最终结果一定是左值引用。

具体规则如下：

| 第一层引用类型 | 第二层引用类型 | 折叠结果 |
| -------------- | -------------- | -------- |
| T&             | &              | T&       |
| T&             | &&             | T&       |
| T&&            | &              | T&       |
| T&&            | &&             | T&&      |

> 从表格中可以看出，只有在 `T&& &&` 的情况下，结果才是 `T&&`，其它情况下都是 `T&`。

### 7. 模板实参推导

对于一个函数模板，当我们调用它时，编译器会利用调用中的函数来推断其模板参数，从而实例化出与我们的函数调用最匹配的版本，这个过程就称为模板实参推导。

#### 7.1 类型转换

由于模板在遇到新类型时通常是生成一个新的模板实例而不是对实参进行类型转换，所以对于模板参数的类型转换非常有限。和往常一样，顶层 `const` 无论是在形参还是在实参中，都会被忽略（顶层 `const` 在按值传递时无意义）。在其它类型转换中，能在调用中应用于函数模板的包括如下两项。

1. 底层 `const` 转换：可以将一个非 `const` 对象的引用（或指针）传递给一个 `const` 的引用（或指针）形参。
2. 数组或函数指针转换：如果函数形参不是引用类型，则可以对数组或函数实参应用正常的指针转换。一个数组实参可以转换为一个指向其首元素的指针，一个函数实参可以转换为一个该函数类型的指针。

至于其他类型转换，如算数类型转换、派生类向基类的类型转换以及用户自定义的转换，都不能应用于函数模板。

看下面的例子：

``` c++
template<typename T>
void fobj(T a, T b) {}

template<typename T>
void fref(const T &a, const T &b) {}

int a[3], b[4], c[4];
double d[4];

int main()
{
    fobj(a, b);
    fobj(b, d); // type not match
    fref(a, b); // size not match
    fref(b, c);
    fref(c, d); // type not match
    return 0;
}
```

在函数 `fobj` 中，数组转换为指针；在 `fref` 中，数组不会进行隐式类型转换，都作为数组本身传递，此时**数组大小也是数组类型的一部分。**

另一个标准库的例子就是 `max` 和 `min` 函数，它们两个是模板函数。以 `max` 为例，当我们通过 `max` 比较两个对象时，这两个对象的类型必须相同，例如：

``` C++
cout << max(3, 3.14) << endl; // no instance of overloaded function "max" matches the argument list
```

上面的 `max` 函数调用是不合法的，因为 `max` 只接受一个模板参数，但我们传入的两个函数调用实参的类型却不相同，因此编译器无法为我们生成一个模板实例，尽管 `int` 可以隐式转换为 `double`。我们有两种解决方法：

``` c++
// 保证实参类型相同
cout << max((double)3, 3.14) << endl; 
// 显式指定模板参数
cout << max<double>(3, 3.14) << endl;
```

#### 7.2 显式指定模板实参

大多数情况下，我们不会显式指定函数模板的模板参数，而是依赖于函数模板对模板类型参数的自动推导，但对于一些特殊情况，例如当函数返回类型与参数列表中任何类型都不相同时，常常有两个问题：

1. 编译器无法推断出模板参数的类型
2. 我们希望允许用户控制返回结果的精度

例如，我们有一个求和的函数 `sum`，我们将返回类型设置为模板参数，以允许用户指定结果的类型。这样，用户就可以选择合适的精度。

``` C++
template<typename RT, typename T1, typename T2>
RT sum(T1 a, T2 b) { return a + b; }
```

但是在这个例子中，没有任何函数实参的类型可用来推断 `RT` 的类型。因此，每次调用 `sum` 都必须为 `RT` 提供一个显式模板实参。

> 不要误认为编译器会自动通过 `a+b` 的结果来推断返回类型。最简单的情况，如果 `a` 和 `b` 的类型不同呢？

同一般的“提供默认值”一样，我们只能自左向右为模板参数提供显式实参，因此，如果这里的 `RT` 出现在 `T1` 和 `T2` 之后，那么我们只能为 `T1` 和 `T2` 提供了显式实参之后才能为 `RT` 提供显式实参，但其实大多数情况下，我们无需显式指定 `T1` 和 `T2` 。

``` C++
template<typename RT, typename T1, typename T2>
RT sum(T1 a, T2 b) { return a + b; }

int main()
{
    cout << sum<int>(1.5, 2.7) << endl;    // 4
    cout << sum<double>(1.5, 2.7) << endl; // 4.2
    return 0;
}
```

#### 7.3 尾置返回类型

当我们希望用户确定返回类型时，用显式模板实参表示模板函数的返回类型是很有效的。但在其他情况下（例如用户不希望自己确定返回类型），要求显式指定模板实参会给用户增添额外负担，而且不会带来什么好处。例如，我们可能希望编写一个函数，接受表示序列的一对迭代器和返回序列中一个元素的引用：

``` C++
template<typename It>
??? &func(It beg, It end)
{
    // process
    return *beg; // return reference
}
```

我们并不知道返回结果的准确类型，但知道所需类型是所处理的序列的元素类型，也即此时返回类型是固定的，无需用户进行精度上的选择：

``` C++
vector<int> v{1, 2, 3};
auto &it = func(v.begin(), v.end()); // func应该返回int&
```

在上面的例子中，我们知道函数应该返回 `*beg`，并且我们知道可以用 `decltype(*beg)` 来获取此表达式的类型。但是，在编译器遇到函数的参数列表 `(It beg, It end)` 之前，`beg` 都是不存在的，也就是说我们无法直接通过 `decltype(*beg)` 来指定返回类型。为了解决该问题，我们必须使用尾置返回类型。由于尾置返回类型出现在函数列表之后，所以它可以使用函数的参数。

``` c++
template<typename It>
auto travel(It beg, It end) -> decltype(*beg)
{
    auto start = beg;
    while(beg != end) 
        cout << *beg ++  << ' ';
    return *start;
}

int main()
{
    vector<int> v{1, 2, 3};
    auto &it = travel(v.begin(), v.end()); // 1 2 3
    it = 10;
    cout << v[0] << endl;   // 10
    return 0;
}
```

通过结合使用尾置返回和 `decltype`，我们就可以让编译器推导出返回类型。

#### 7.4 类型转换模板

有时我们无法直接获得所需要的类型。例如上面的 `travel` 函数，我们希望返回元素的值而非引用。但是在编写这个函数的过程中，我们面临一个问题：对于传递的参数类型，我们几乎一无所知。在此函数中，我们知道唯一可以使用的操作是迭代器操作，而所有的迭代器操作都不会生成元素，只能生成元素的引用。

为了获得元素类型，我们可以使用标准库的**类型转换模板（type transformation template）**。这些模板定义在头文件 **`type_traits`** 中。这个头文件中的类通常用于所谓的模板元编程。但是，类型转换模板在普通编程中也很有用。

在本例中，我们可以使用 `remove_reference` 来获得元素类型。`remove_reference` 模板有一个模板类型参数和一个名为 `type` 的公开类型成员。如果我们用一个引用类型实例化 `remove_reference`，则 `type` 将表示被引用的类型。例如，如果我们实例化 `remove_reference<int&>` ，则 `type` 成员将是 `int`。更一般的，给定一个迭代器 `beg`：

```C++
remove_reference<decltype(*beg)>::type;
```

将获得 `beg` 引用的元素的类型。

通过组合 `remove_reference`、尾置返回及 `decltype`，我们就可以在函数中返回元素值的拷贝：

``` c++
template<typename It>
auto func(It beg, It end) -> typename remove_reference<decltype(*beg)>::type
{
    auto start = beg;
    while(beg != end) {
        cout << *beg << ' ';
        ++ beg;
    }
    cout << endl;
    return *start;
}

int main()
{
    vector<int> v{1, 2, 3};
    auto it = func(v.begin(), v.end());
    it = 10;
    cout << v[0] << endl;   // 1
    return 0;
}
```

注意在 C++ 中，在编译器不确定作用域运算符后面的成员到底是静态成员还是类型成员时，会优先将其解释为静态成员，因此这里需要在前面显式指定 `typename`，表示 `type` 是一个类型成员，

**表：标准类型转换模板**

| mod                  | Description        | t               | mod<t>::type |
| -------------------- | ------------------ | --------------- | ------------ |
| remove_reference     | 去引用             | X&,X&&          | X            |
|                      |                    | 其他            | T            |
| add_const            | 添加顶层const      | X&,const X,函数 | T            |
|                      |                    | 其他            | const T      |
| add_lvalue_reference | 添加左值引用       | X&              | X&           |
|                      |                    | X&&             | X&           |
|                      |                    | 其他            | T&           |
| add_rvalue_reference | 添加右值引用，     | X&              | X&           |
|                      | 且遵循引用折叠规则 | 其他            | T&&          |
|                      |                    |                 |              |
| remove_pointer       |                    | X\*             | X            |
|                      |                    | 其它            | T            |
| add_pointer          |                    | X&,X&&          | X*           |
|                      |                    | 其它            | T*           |
| make_signed          |                    | unsigned X      | X            |
|                      |                    | 其它            | T            |
| make_unsigned        |                    | signed X        | unsigned X   |
|                      |                    | 其它            | T            |
| remove_extent        |                    | X\[n\]          | X            |
|                      |                    | 其它            | T            |
| remove_all_extents   |                    | X\[n1\]\[n2\]…  | X            |
|                      |                    | 否则            | T            |

说明：

1. C++14 提供了一种更方便的获取 `::type` 的方法，那就是在模板名后面加上 `_t`。例如：`remove_reference<int&>::type` 可以写成 `remove_reference_t<int&>`。
2. **`add_const`**：C++ 中，`const` 限定符应用于引用类型时，它实际上会作用域引用所指向的第对象（底层 `const`）。而我们这里的 `add_const` 实际上是添加顶层 `const`，因此对于 `X&`，依然返回 `X&` 而不是 `const X&`。同样的，对于指针来说，`add_const` 会将指针修饰为常量指针而不是“指向常量”的指针。
3. **`add_rvalue_reference`**：对于 `add_lvalue_reference` 左值引用，无论是左值还是右值都会变成左值；但是对于转换成右值引用来说，左值依然是左值，右值却变成了非引用类型；以上结果就是因为 C++ 的<font color=blue>**引用折叠**</font>规则。

#### 7.5 函数指针

当我们用一个函数模板初始化一个函数指针或为一个函数指针赋值时，编译器使用指针的类型来推断模板实参。例如：

``` c++
template<typename T>
int compare(const T &lhs, const T &rhs)
{
    return lhs < rhs;
}

using fptr = int(*)(const int&, const int&);

int main()
{
    fptr fp1 = compare; 
    // 这里compare的实参会被推导为int
    cout << fp1(1, 2) << endl;
    return 0;
}
```

#### 7.6 引用

##### 1. 从[左值引用函数参数]推断类型

当一个函数参数是模板类型的一个[普通左值引用]时（即，形如 `T&`），绑定规则告诉我们，只能传递给它一个左值。实参可以是 `const` 类型，也可以不是。如果实参是 `const` 的，则 `T` 将被推断为 `const` 类型。

如果一个函数参数的类型是 `const T&`，正常的绑定规则告诉我们可以传递给它任何类型的实参。当函数参数本身是 `const` 时，`T` 的类型推断结果不会是一个 `const` 类型。

##### 2. 从[右值引用函数参数]推断类型

如果一个函数参数是一个右值引用（即，形如 `T&&` 时），正常绑定规则告诉我们可以传递给它一个右值。当我们这样做时，类型推断过程类似普通左值引用函数参数的推断过程。推断出的 `T` 的类型是该**右值实参的类型**。

##### 3. 引用折叠和[右值引用参数]

对于一个接受[右值引用函数参数]的模板函数来说，我们可能认为下面的调用是不合法的：

``` C++
template<typename T>
void f(T &&x) { x = 1024; }

int main()
{
    int x = 10;
    f(x);	// 通过一个左值来绑定右值
    cout << x << endl;
    return 0;
}
```

即我们是不能直接将左值绑定到右值上的。但是，C++ 语言在正常绑定规则之外定义了两个额外规则，允许这种绑定。这两个额外规则是 `move` 这种标准库设施正确工作的基础：

1. <font color=blue>**右值引用的特殊类型推断规则：**</font>第一个例外规则影响右值引用参数的推断如何进行。当我们将一个左值传递给函数的右值引用参数，并且**此右值指向模板类型参数**（如 `T&&`）时，编译器推断模板类型参数为实参的左值引用类型。因此，这里我们调用 `f(x)` 时，编译器推断 T 的类型为 `int&`，而非 `int`。
2. <font color=blue>**引用折叠规则：**</font>T 的类型被推断为 `T&` 意味着 `f` 的函数参数类型是一个类型为 `int&` 的右值引用，即我们间接的创建了一个“引用的引用”。在 C++ 中引用的引用是不合法的，在这种情况下，我们可以使用第二个例外绑定规则：如果我们间接创建了一个“引用的引用”，引用会进行折叠。

通过右值引用的特殊类型推断规则和引用折叠规则，我们就能实现<font color=blue>**万能引用**。</font>

不过要注意，对于非模板形式的右值引用，是不能接受左值的：

``` cpp
void f(int &&x) {}

int main()
{
    int x = 10;
    f(x); // an rvalue reference cannot be bound to an lvalue
    return 0;
}
```

##### 4. 编写接受[右值引用函数参数]的模板函数

模板参数可以推断为一个引用类型，这一特性对模板内的代码可能有令人惊讶的影响：

``` c++
template<typename T>
void func(T&& val)
{
    T t = val; // t是val的拷贝还是绑定到val的引用呢？
    t = 1024;  // 仅仅修改t还是同时修改t和val呢？
}
```

当我们对一个右值调用 `func` 时，`T` 为 `int`，此时 `t` 的类型为 `int`，它通过 `val` 拷贝初始化；当我们对一个左值调用 `func` 时，`T` 是 `int&`，此时 `t` 的类型时 `int&`，他是一个绑定到 `val` 的引用。

``` C++
int x = 10;
func(x);
cout << x << endl; //1024
```

可以发现，对于代码中的一条初始化语句，它的行为可以会因为传入的实参类型而改变，这就导致代码的编写比较困难。

在实际中，右值引用通常用于两种情况：

1. 模板转发：`std::forward`

2. 模板重载：

   ``` c++
   template<typename T> void func(T&& val);
   template<typename T> void func(const T& val);
   ```

### 8. std::move

#### 8.1 实现思路

``` c++
template<typename T>
remove_reference_t<T>&& move(T&& t) {
    return static_cast<remove_reference_t<T>&&>(t);
}
```

从一个左值 `static_cast` 到一个右值引用是允许的。这里有一条针对右值引用的特许规则：虽然不能隐式的将一个左值转换为右值引用，但我们可以用 `static_cast` 显式的将一个左值转换为一个右值引用。

1. 在模板编程中，`static_cast<T&&>` 可以确保我们在**完美转发**中正确地保持参数的左右值属性。
2. 当我们调用一个函数需要**移动对象资源**时，比如将对象传递给一个构造函数或赋值操作，`static_cast<Type&&>` 可以将一个左值转换为右值引用，使得函数能通过移动语义来处理这个对象，从而避免不必要的拷贝。

在 C++ 中，**不能直接将一个右值转换为左值引用**，因为右值没有持久的存储空间，无法为左值引用提供有效的绑定目标。

#### 8.2 万能转发

某些函数需要将其一个或多个实参连同类型不变的转发给其它函数。在此情况下，我们需要保持被转发实参的所有性质，包括：

1. 实参类型是否是 `low-level const`
2. 实参是左值还是右值

看下面的例子，我们编写一个模板函数 `flip`，它接受一个可调用表达式和两个参数。`flip` 将调用这个可调用表达式并将接受的两个参数传递给它：

``` c++
void f(int v1, int &v2) {
    ++ v2;
}

// flip的顶层const和引用信息丢失了
template<typename F, typename T1, typename T2>
void flip(F f, T1 t1, T2 t2) {
    f(t1, t2);
}

int main()
{   
    int x = 1, y = 2;
    flip(f, x, y);   
    cout << x << ' ' << y << endl; //1 2
    return 0;
}
```

在上面的代码中，我们的本意是通过 `f` 修改 `y` 的值，但因为在将 `y` 传递给 `flip` 时丢失了引用信息，导致实际上修改的是 `flip` 的 `t2`。

---

通过将一个函数参数定义为一个指向模板参数类型的右值引用，我们可以保持其对应参数的所有类型信息（包括左右值和 `low-level const`）。

1. 通过引用折叠可以保存左值和右值信息。
2. 通过引用可以保存 `const` 信息，引用的 `const` 是底层的。

我们修改 `flip` 函数如下：

``` c++
void f(int v1, int &v2) {
    ++ v2;
}

template<typename F, typename T1, typename T2>
void flip(F f, T1&& t1, T2&& t2) {
    f(t1, t2);
}

int main()
{   
    int x = 1, y = 2;
    flip(f, x, y);   
    cout << x << ' ' << y << endl;  // 1 3
    return 0;
}
```

但这个版本的 `flip` 也只是解决的一半问题。它对于接受一个左值引用的函数工作的很好，但不能用于接受右值引用参数的函数。例如：

``` C++
void g(int&& v1, int &v2) {
    cout << v1 << ' ' << v2 << endl;
}

template<typename F, typename T1, typename T2>
void flip(F f, T1&& t1, T2&& t2) {
    f(t1, t2); // error:cannot bind rvalue reference of type ‘int&&’ to lvalue of type ‘int 
}

int main()
{   
    int x = 1;
    flip(g, 42, x);   
    return 0;
}
```

在上面的代码中，我们传递给 `t1` 一个临时对象 `42`，`t1` 的类型 `T1` 被推导为一个右值引用。但是如果我们把 `t1` 传递给函数 `g` 的 `v1`，会报错：我们将一个左值传递给右值引用。这是因为，<font color=blue>**尽管 `t1` 的类型是右值引用，但 `t1` 本身（一个具名变量）和任何变量一样，作为表达式本身它都是一个左值。**</font>因此这里 `t1` 实际上是作为一个左值传递给函数 `g` 的。

这一点下面的情况是一样的，在这里 `f(int x)` 中的 `x` 作为一个具名变量，永远是左值。

``` c++
int rf(const int &x) { cout << "lvalue" << endl; }
int rf(int &&x)      { cout << "rvalue" << endl; }
int f(int x)         { rf(x); }
int main()
{   
    f(11); // lvalue
    return 0;
}
```

----

<font color=blue>上面说的这一种无法处理右值引用的情况非常关键，理解好了这一点，才能理解我们为什么需要**“完美转发”（forward）**。</font>

我们可以使用一个名为 `forward` 的新标准库设施来传递 `flip` 的参数，它能保持原始参数的类型。

`std::move` 和 `std::forward` 都定义在 **`<utility>`** 头文件中。与 `move` 不同的是，`forward` 必须通过<font color=blue>**显式模板实参**</font>来调用。**`forward` 返回该显式实参类型的<font color=blue>右值引用</font>。即，`forward<T>` 的返回类型是 `T&&`，即万能引用**。通过万能引用，`forward` 可以保证转发后的类型依然是 `T` 本来的类型。其底层原理是引用折叠。

`forward` 为什么要显式指定模板参数 `T` 的原因也很简单，因为 `T` 保存的就是模板推导出的调用者传递的实参的类型。

通常情况下，我们使用 `forward` 传递那些定义为模板参数类型的右值引用，即，`forward` 依赖万能引用实现，同时也配合万能引用使用。

* 当万能引用接受一个左值时，`forward` 转发一个左值
* 当万能引用接受一个右值时，`forward` 转发一个右值

``` c++
void f(const int& x) { cout<<"lvalue"<<endl; }
void f(int&& x)      { cout<<"rvalue"<<endl; }

template<typename T>
void warp(T&& arg) // 万能引用
{
    f(std::forward<T>(arg));    // 完美转发
}

int main()
{   
    int x = 1;
    warp(1); // rvalie
    warp(x); // lvalue
    return 0;
}
```

----

你可能会问，正如我们前面所说的，`forward` 本质上是返回 `T` 的类型，那么我们直接通过 `static_cast` 将元素类型显式转换为 `T` 不就好了：`static_cast<T>(arg);` 当然可以！不过此时，如果 `T` 的类型是 `int`，注意不是 `int&&`，`static_cast` 就会丧失 `T&&` 的移动语义。这就导致，原本使用移动进行的操作，现在不得不依赖于拷贝，从而产生不必要的拷贝操作。例如：

``` c++
struct Foo {
    Foo(int _x) {};
    Foo(const Foo&x) { puts("copy ctor"); }
    Foo(Foo&&x)      { puts("move ctor"); }
};

void f(Foo x) {}

template<typename T>
void warp(T&& arg) // 万能引用
{
    f(std::forward<T>(arg));    // move ctor
    f(static_cast<T>(arg));     // copy ctor
}
```

一言以蔽之，那就是 `static_cast<T>(arg)` 会丢失右值引用属性。

----

那你可以又问，既然如此，我们直接让 `T` 变成 `T&&`，即 `static_cast<T&&>(arg);` 不就好了？当然好了！因为这就是 `std::forward` 的实现：

``` c++
template<typename T>
remove_reference_t<T>&& move(T&& t) {
    return static_cast<remove_reference_t<T>&&>(t);
}	

template<class _Ty>
_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>& _Arg) noexcept
{	
        // forward an lvalue as either an lvalue or an rvalue
	return (static_cast<_Ty&&>(_Arg));
}

template<class _Ty>
_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept
{	
        // forward an rvalue as an rvalue
	static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
	return (static_cast<_Ty&&>(_Arg));
}
```

你可能又又又要问了，为什么这里要为 `forward` 设计两个版本，分别接受一个左值引用和一个右值引用。我们直接用下面的形式不行吗？

``` C++
template<typename T>
T&& forward(T&& arg) noexcept { // 直接使用万能引用，既能接受左值，又能接受右值
    return static_cast<T&&>(arg);
}
```

事实上，我们可以测试一下，看看到底调用那个函数：

``` cpp
template<class _Ty>
constexpr _Ty&& Forward(remove_reference_t<_Ty>& _Arg) noexcept
{	
        // forward an lvalue as either an lvalue or an rvalue
    cout << "call forward &" << endl;
	return (static_cast<_Ty&&>(_Arg));
}

template<class _Ty>
constexpr _Ty&& Forward(remove_reference_t<_Ty>&& _Arg) noexcept
{	
        // forward an rvalue as an rvalue
    cout << "call forward &&" << endl;
	static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
	return (static_cast<_Ty&&>(_Arg));
}

void func1(int a, int& b)
{
    std::cout << "in an_orther_fun(): a = " << a << ", b = " << ++b << std::endl;
}

void func2(int a, int&& b)
{
    std::cout << "in an_orther_fun(): a = " << a << ", b = " << ++b << std::endl;
}

template <typename F, typename T1, typename T2>
void transmit(F f, T1&& t1, T2&& t2)
{
    f(Forward<T1>(t1), Forward<T2>(t2));
}

void test1()
{
    int x = 1, y = 2;
    transmit(func1, x, y);
    cout << x << ' ' << y << endl;
    // call forward &
    // call forward &
    // in an_orther_fun(): a = 1, b = 3
    // 1 3
}

void test2()
{
    int x = 1;
    transmit(func2, x, 10);
    cout << x << endl; 
    // call forward &
    // call forward &
    // in an_orther_fun(): a = 1, b = 11
    // 1
}
```

可以发现，在上面的两个 `test` 函数中，我们分别给 `t2` 传入了一个左值 `y` 和一个右值 `10`，然而两次对 `t2` 的转发都使用了接受左值的 `forward`。

那么，为什么会这样呢？如果只使用到一种重载，定义第二种重载的意义在哪里呢？

仔细分析一下第二种情况，`10` 是一个右值，在对 `transmit` 进行模板参数推导时，由于参数列表使用了万能引用，`T2 `将会被推导为 `int`， `T1` 将会被推导为 `int&`，实例化的函数为：

```cpp
void transmit(void (*f)(int, int&&), int& t1, int&& t2)
{
    f(std::forward<int&>(t1), std::forward<int>(t2));
}
```

需要注意的是，`t2` 虽然是一个右值引用，但其本身确是个左值。根据 `std::forward<int>(t2)`，假如匹配第二个重载，那么模板参数 `_Ty` 为 `int`， `_Arg` 的类型为 `int&&`，会尝试将一个`int`型的左值绑定到`int`的右值引用，语法上不允许，编译不通过。因此，`std::forward<int>(t2)` 还是会使用第一种重载，`_Ty` 为 `int`，`_Arg`的类型为`int&`，是 `t2` 的一个左值引用，完全正确。

那么，既然只会匹配到第一个模板，那么第二个模板的意义在哪里呢？毕竟，按照经验，`transmit `函数中，所有的参数（具名变量）都会是左值，根本就不会是右值。

事实就是，有一些情况，参数还真会变成右值。

``` cpp
struct Foo {
    Foo(const int &f) { cout << "Foo::cint&" << endl; }
    Foo(int &&f) { cout << "Foo::int&&" << endl; }
};

// transforming wrapper
template<class T>
void wrapper(T&& arg)
{
    Foo
    ( 
        Forward
        <decltype(Forward<T>(arg).get())>
        (
            Forward<T>(arg).get()
        )
    );

    // 令 Forward<T>(arg).get() 为 u
    // 上式等价于 Foo(Forward<decltype(u)>(u));
}

struct Arg {
    int i = 1;
    int  get() && { return i; } // call to this overload is rvalue
    int& get() &  { return i; } // call to this overload is lvalue
};

Arg a;

int main()
{   
    wrapper(a);
    // call forward &
    // call forward &
    // Foo::cint&
    wrapper(Arg());
    // call forward &
    // call forward &&
    // Foo::int&&
    return 0;
}
```

也就是说，如果不直接转发 `arg`，而是转发 `arg` 的成员函数的返回值，那么就需要使用第二种重载。在上面的例子中，

**分步解析**：

1. **第一次 `Forward<T>(arg)`**：

   - 转发 `arg` 本身（保持左值/右值性质）。
   - 根据 `arg` 调用 `get()` 时：
     - 如果 `arg` 是左值，调用 `int& get() &`。
     - 如果 `arg` 是右值，调用 `int get() &&`。

   如果我们不适用 `Forward<T>(arg).get()` 而是直接调用 `arg.get()`，那么这里无论 `arg` 是左值还是右值，都会执行 `int& get() &  { return i; }`，这是因为 `arg` 作为一个具名变量，它本身是左值的，而我们这里需要的类型不是 `arg` 本身的类型，而是我们传入的类型 `T`，因此，必须使用万能转发。

2. **`decltype(Forward<T>(arg).get())`**：

   - 推断 `get()` 的返回类型：
     - 左值 `get()` 返回 `int&`。
     - 右值 `get()` 返回 `int`。

3. **第二次 `Forward<...>(...)`**：

   - 将 `get()` 的结果转发给 `Foo` 的构造函数。

在得到 `get()` 的返回值之后，我们还希望根据返回值的类型去调用 `Foo` 的构造函数，因此对于 `Forward<T>(arg).get()` 的结果，我们依然需要通过 `Forward` 传递给 `Foo` 的构造函数。

此时，如果 `Forward<T>(arg).get()` 的结果是一个右值，那么对于接受左值的 `Forward` 来说，就无法执行了。因此这里有必要添加一个接受右值的 `Forward`。

不过，目前为止，我们只是证明了为什么需要一个右值引用的版本，我们还没有回答最初的问题，即，为什么不把这两个函数合并为一个呢？

``` cpp
// 合并为一个
template<typename T>
T&& Forward(T&& arg) noexcept { // 直接使用万能引用，既能接受左值，又能接受右值
    return static_cast<T&&>(arg);
}
```

那其实说了这么多，应该能想到了，那就是无论传入给 `arg` 的参数类型是左值还是右值，`arg` 作为一个具名变量本身永远是左值，因此：

1. 对于传入左值的情况，`T&&` 和 `arg` 是匹配的，它们都是左值
2. 对于传入右值的情况，`T&&` 是右值，但是 `arg` 是左值，它们不匹配

我们也可以实验一下：

``` cpp
template<class _Ty>
constexpr _Ty&& Forward(_Ty&& _Arg) noexcept
{	
    cout << "call forward &" << endl;
	return (static_cast<_Ty&&>(_Arg));
}

// template<class _Ty>
// constexpr _Ty&& Forward(remove_reference_t<_Ty>& _Arg) noexcept
// {	
//         // forward an lvalue as either an lvalue or an rvalue
//     cout << "call forward &" << endl;
// 	return (static_cast<_Ty&&>(_Arg));
// }

// template<class _Ty>
// constexpr _Ty&& Forward(remove_reference_t<_Ty>&& _Arg) noexcept
// {	
//         // forward an rvalue as an rvalue
//     cout << "call forward &&" << endl;
// 	static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
// 	return (static_cast<_Ty&&>(_Arg));
// }

struct Foo {
    Foo(const int &f) { cout << "Foo::cint&" << endl; }
    Foo(int &&f) { cout << "Foo::int&&" << endl; }
};

// transforming wrapper
template<class T>
void wrapper(T&& arg)
{
    Foo
    ( 
        Forward
        <decltype(Forward<T>(arg).get())>
        (
            Forward<T>(arg).get()
        )
    );

    // 令 Forward<T>(arg).get() 为 u
    // 上式等价于 Foo(Forward<decltype(u)>(u));
}

struct Arg {
    int i = 1;
    int  get() && { cout << "rvalue get" << endl;  return i; } // call to this overload is rvalue
    int& get() &  { cout << "lvalue get" << endl;  return i; } // call to this overload is lvalue
};

Arg a;

int main()
{   
    wrapper(a);
    // call forward &
    // lvalue get
    // call forward &
    // Foo::cint&

    wrapper(Arg()); //  error: cannot bind rvalue reference of type ‘Arg&&’ to lvalue of type ‘Arg’
    return 0;
}
```

可以发现，对于传入一个右值 `Arg()` 的情况，编译器报错“我们不能将一个右值类型 `Arg&&` 绑定到一个左值 `arg` 上”，和我们的分析一致。

[***reference here***](https://www.matsjiang.com/article/12)

### 9. 可变模板参数

一个可变参数模板就是一个接受可变数目参数的模板函数或模板类。可变数目的参数被称为**参数包**。存在两种参数包：

1. 模板参数包：表示零个或多个模板参数，形式为 `typename...` 或 `class...`。
2. 函数参数包：表示零个或多个函数参数，形式为在函数参数类型名后面跟一个省略号。

例如：

``` c++
void print() {
    cout << endl;
}

template<typename FirstArg, typename... Args>
void print(const FirstArg& firstArg, const Args&... args) {
    cout << firstArg << ' ';
    print(args...);
}


int main()
{
    print('A', 16, 3.14, "hello"); // A 16 3.14 hello 
    return 0;
}
```

#### 9.1 sizeof… 运算符

当我们需要知道包中有多少元素时，可以使用 sizeof… 运算符。与 sizeof 运算符相同，sizeof… 运算符也返回一个常量表达式：

``` c++
template<typename FirstArg, typename... Args>
void getCount(const FirstArg& firstArg, const Args&... args) {
    cout << sizeof...(Args) << endl;
    cout << sizeof...(args) << endl;
}


int main()
{
    getCount('A', 16, 3.14, "hello");
    return 0;
}
// 3
// 3
```

#### 9.2 递归

我们前面提到过使用 `initializer_list<T>` 来实现可变参数，但是它只能接受相同类型的可变参数。而可变模板参数可以接受不同类型的参数。

 使用可变模板参数时，通常通过**递归**的形式处理可变参数。与普通递归一样，这里的递归也需要有终点，因此我们通常定义一个不包含可变模板参数的模板特例化作为递归的终点。

``` c++
/* 可变参数函数模板 */
template<typename T>
void print(const T &t) // 递归终点
{   
    cout << t;
}
template<typename T, typename... Args>
void print(const T& t, const Args&... args)
{
    cout << t << ' ';
    print(args...);
}

int main()
{   
    print(1, 3.14, "hello");
    return 0;
}
```

#### 9.3 包拓展

##### 1. 语法

先来看一个可变模板参数的应用：

``` c++
template<typename T, typename... Args>
void print(const T& t, const Args&... args) // 拓展Args
{
    cout << t << ' ';
    print(args...);	// 拓展args
}
```

其中，`typename...` 我们称之为**模板参数包**，`Args&...` 称之为**函数参数包**。那么，`print` 中的 `args...` 又是什么用法呢？我们之前好像从没在意过！

对于一个参数包（模板参数包/函数参数包），除了获取其大小外，我们能对它做的唯一的事情就是**拓展（expand）**它。包扩展的语法是：

```cpp
(pattern)...
```

其中，`pattern` 是一个包含参数包的表达式，`...` 会将该模式**逐项展开**到参数包的每个元素上，展开后的结果是一个**逗号分隔的列表**。

> <font color=blue>**拓展一个包**就是将参数分解为构成的元素，对每个元素应用**模式**，获得扩展后的**列表**。</FONT>

##### 2. 常见拓展模式

###### （1）直接使用参数包

例如，对于一个调用 `print(1,3.14,"hello");`  经过拓展之后的模板相当于：

``` c++
void print(const int &t, const double& arg1, const string &arg2) {
    cout << t << ' ';
    print(arg1, arg2);
}
```

 `print(args...);` 中函数参数包的模式相当于什么也不做。

###### （2）**表达式模式**

``` C++
template<typename T>
const T& check(const T &arg)
{
    cout << arg << endl;
    return arg;
}

template<typename T>
void print(const T &t) 
{   
    cout << t << endl;
}

template<typename T, typename... Args>
void print(const T& t, const Args&... args)
{
    cout << t << endl;
    print(check(args)...);
}
```

其中 `check` 就是我们为 `args` 指定的模式，`print` 相当于：

``` c++
print(check(arg1), check(arg2));
```

因此，`check` 的返回值不能为 `void`，因为我们需要在 `print` 中打印 `check(arg1)` 和 `check(arg2)` 的值。

###### （3）**折叠表达式（C++17）**

折叠表达式是包扩展的增强，直接对参数包进行二元运算：

| 折叠形式                | 展开方式                                    | 示例                 |
| :---------------------- | :------------------------------------------ | :------------------- |
| `(pack op ...)`         | `((arg1 op arg2) op arg3)...`               | `((1 - 2) - 3) = -4` |
| `(... op pack)`         | `(arg1 op (arg2 op arg3))...`               | `(1 - (2 - 3)) = 2`  |
| `(init op ... op pack)` | `(((init op arg1) op arg2) op ...) op argN` |                      |
| `(... op pack op init)` | `arg1 op (arg2 op ... (argN op init))`      |                      |

**示例：**

```cpp
template<typename... Args>
auto sum_with_one(Args... args) {
    // init op ... op pack
    // init -> 1
    // pack -> args 
    return (1 + ... + args);  
}

template<typename... Args>
void print_plus_one(Args... args) {
    // pack op ...
    // pack -> cout << (args + 1) << " "
    // op   -> ,
    ((cout << args + 1 << " ") , ...);
}


int main()
{
    cout << sum_with_one(1, 2, 3) << endl; // 7
    print_plus_one(1, 2, 3);      // 2 3 4
    return 0;  
}
```

#### 9.4 转发参数包

在新标准下，我们可以组合使用可变参数模板与 `forward` 机制编写函数，实现将其<FONT COLOR=BLUE>**实参不变**</FONT>地传递给其它函数。

标准库的 `emplace_back` 就是一个很好的例子。`emplace_back` 直接在数据内存处调用构造函数，而构造函数可能有多个，其参数个数和类型不同，因此使用可变参数模板是个很好的例子。

另外在将参数从 `emplace_back` 传递给构造函数时，我们需要确保参数的类型信息不变（左值/右值），同之前讲到的，`emplace_back` 的参数类型需要是右值引用（保留左值和右值信息），传递参数时需要使用 `forward`（避免变量作为表达式时永远是左值）。

``` c++
class Foo {
public:
    template<typename... Args>
    void emplace_back(Args&&... args) // 模式为&&
    {
        // 分配内存
        alloc.construct(first_free++, 
        	std::forward<Args>(args)...); 
        	// 对args中的每个参数应用模式forward<Args>
    }
};
```

值得注意的是，对于语句 `std::forward<Args>(args)...` ，它不仅拓展了函数参数包 `args`，还拓展了模板参数包 `Args`。它相当于：

``` C++
std::forward<Args1>(args1), std::forward<Args2>(args2), ...
```

同理，`make_shared` 等函数也是同样的设计原理：

1. 接受参数包
2. 拓展并转发给 new 进行实际的内存分配

``` c++
template<typename T, typename... Args>
std::shared_ptr<T> make_shared(Args&&... args)
{
    return shared_ptr<T>(new T(std::forward<Args>(args)...));
}
```

### 10. 模板特例化

在某些情况下，通用模板的定义对特定类型是不合适的：通用定义可能编译失败或做的不正确。此时，我们可以定义函数或类模板的一个特例化版本。

一个模板特例化就是一个用户显式提供的模板实例，**它将一个或多个模板参数绑定到特定的类型或值上。**它分为两种主要形式：全特例化（full specialization）和偏特例化（partial specialization）。偏特化版本的模板参数列表是原始模板参数列表的一个子集（例如指针或引用形式）或是一个特例化版本（指定了具体的类型）。

模板特例化的语法实在类模板名或者函数模板名的后面加上 `<...>` 显式指定模板参数。

#### 10.1 偏特化和全特化

``` C++
template<typename T, typename U>
class Foo {
public:
    void print() {
        cout << "T,U" << endl;
    }
};

// 1. 偏特化
template<typename T>
class Foo<T,T> {
public:
    void print() {
        cout << "T,T" << endl;
    }
};

// 2. 偏特化
template<typename T, typename U>
class Foo<T*,U*> {
public:
    void print() {
        cout << "T*,U*" << endl;
    }
};

// 3. 偏特化
template<typename T>
class Foo<T, long long> {
public:
    void print() {
        cout << "T,long long" << endl;
    }
};

// 4. 全特化
template<>
class Foo<int,long long> {
public:
    void print() {
        cout << "int,long long" << endl;
    }
};

// 5. 全特化
template<>
class Foo<string,int> {
public:
    void print() {
        cout << "string,int" << endl;
    }
};

int main()
{
    Foo<int,long long>().print(); // int,long long
    Foo<string,int>().print();     // string,int
    Foo<int,int>().print();        // int,int
    Foo<int*,int>().print();       // T,U
    Foo<int*,double*>().print();   // T*,U*
    //Foo<int*,int*>().print();    // more than one partial specialization matches the template argument list of class "Foo<int *, int *>"C/C++(838)
                                   // 1.cpp(79, 5): "Foo<T, T>" (declared at line 40)
                                   // 1.cpp(79, 5): "Foo<T *, U *>" (declared at line 49)
    

    return 0;
}
```

可以发现：

* 如果同时有偏特化和全特化满足函数匹配，会优先选择全特化的版本，例如 `Foo<int,long long>`  
* 如果偏特化有多个版本满足函数匹配，编译器会报错，例如 `Foo<int*,int*>`

#### 10.2 函数模板特例化

正如我们上面看到的，对于类模板，我们可以对其偏特化和全特化。但是当我们特例化一个函数模板时，必须为原模板中的<font color=blue>**每个模板参数**</font>都提供实参。

``` cpp
template<typename T, typename U>
void func(T a, U b) {
    cout << "T,U" << endl;
}

// 1. 偏特化
template<typename T>
void func<T,T>(T a, T b) {  // error: non-class, non-variable partial specialization ‘func<T, T>’ is not allowed
    cout << "T,T" << endl;
}

// 2. 偏特化
template<typename T>
void func<T,int>(T a, int b) {  // error: non-class, non-variable partial specialization ‘func<T, int>’ is not allowed
    cout << "T,int" << endl;
}

// 3. 全特化
template<>
void func<int,int>(int a, int b) {  
    cout << "int,int" << endl;
}

int main()
{
    func(int(), int());  // int,int
    return 0;
}
```

函数模板不支持偏特化，而类模板是支持的。这是语言设计上的一个有意为之的选择，主要原因如下：

##### 1. **函数重载已经提供了类似的功能**

C++ 允许函数重载（overloading），即可以定义多个同名函数，只要它们的参数列表不同。通过函数重载，你可以为特定的类型或类型组合提供定制化的实现，这在一定程度上可以替代函数模板偏特化的需求。例如：

```cpp
template <typename T, typename U>
void foo(T a, U b) { /* 通用实现 */ }

// 函数重载
template <typename T>
void foo(T a, int b) { /* 针对第二个参数是 int 的特化实现 */ }
```

虽然这不是真正的偏特化（语法不同），但能达到类似的效果。

##### 2. **避免复杂性和歧义**

函数模板的偏特化可能会引入复杂的重载解析规则，增加编译器的实现难度，并可能导致代码的歧义。例如，如果有多个偏特化版本匹配同一个调用，编译器可能需要更复杂的规则来决定选择哪一个。而通过函数重载，重载解析的规则已经足够清晰（基于参数类型匹配）。

##### 3. **语言设计的简洁性**

类模板的偏特化主要用于模板元编程和类型推导，而函数模板的重载已经足够灵活，可以满足大多数需求。因此，C++ 标准委员会认为不需要为函数模板引入偏特化，以避免语言特性的冗余。

##### 4. **全特化仍然支持**

虽然不支持偏特化，但 C++ 允许函数模板的全特化（full specialization）：

```cpp
template <>
void foo<int, double>(int a, double b) { /* 全特化实现 */ }
```

全特化可以处理完全明确的类型组合，而偏特化的情况可以通过重载或结合类模板实现。

##### 替代方案：使用类模板 + 静态成员函数

如果需要偏特化的功能，可以将函数逻辑封装到一个类模板中，利用类模板支持偏特化的特性：

```cpp
// 分发器
template <typename T, typename U>
struct FooImpl {
    static void foo(T a, U b) { /* 通用实现 */ }
};

template <typename T>
struct FooImpl<T, int> {
    static void foo(T a, int b) { /* 针对 U=int 的偏特化实现 */ }
};

// 包装函数调用
template <typename T, typename U>
void foo(T a, U b) {
    FooImpl<T, U>::foo(a, b);
}
```

#### 10.3 函数重载与模板特例化

##### 1. 特例化不是重载

当定义函数模板的特例化（全特化）版本时，我们本质上接管了编译器的工作。即，我们为原模板的一个特殊实例提供了自己的定义。重要的是：<font color=blue>**一个全特化版本本质上是一个实例，而非函数名的一个重载版本。**</font>

因此，**全特化不影响重载决议（函数匹配）**，因为它本质上并没有产生新的函数版本（参数个数或参数类型不同），特例化影响的只是函数匹配到特定模板时会选择的“实例”。

``` cpp
				              - 通用模板
                   |- v1(模板) -全特化实例1
function match ----|          - 全特化实例2
                   |- v2(重载)
                   |- v3(重载)
```

##### 2. 注意特例化的语法

现在，假设我们想借助可变参数实现一个 `tuple`，初步框架如下：

``` c++
template<typename T> // 递归终点
class Tuple {};

template<typename T, typename... Args>
class Tuple : public Tuple<Args...> {};//  error: redeclared with 2 template parameters
```

在这里，我们初步是想通过递归定义的方式实现 `tuple`，但是编译器却告诉我们这两个 `tuple` 重定义错误了。这是因为在这里，第一个 `tuple` 的定义是 `class tuple` 而不是 `class tuple<>`，即，该 `tuple` 并不是以特例化的语法定义的。

因此，对于这两个 `tuple` 来说，它们的关系是 “重载” 而不是 “特例” 的关系。而对于类来说，是不存在重载的，因此这里编译器报告重定义错误。正确的写法是应该将第一个 `tuple` 写为特例化的形式，即在类名的后面加上尖括号：

``` c++
template<typename T> // 递归终点
class Tuple<T>/*特例*/ {}; // ‘Tuple’ is not a class template

template<typename T, typename... Args>
class Tuple : public Tuple<Args...> {};
```

但是编译器依然报告错误，它说第一个 `tuple` 不是一个类模板。这是因为第一个模板作为第二个模板的“特例”，它出现在第二个模板之前了，因此这里需要更换一下两个模板之间的声明顺序：

``` c++
template<typename T, typename... Args>
class Tuple : public Tuple<Args...> {};

template<typename T> // 递归终点
class Tuple<T>/*特例*/ {};
```

#### 10.4 特例化版本的声明必须可见

如我们先前提到的一样，对于普通类和函数，声明丢失的情况很容易发现，因为编译器会编译错误。但是对于模板特例化的声明，编译器可以通过原模板产生代码，导致编译器调用的并非我们想要的函数，由于此时函数调用正常执行，但是调用了错误的函数，因此出现错误时很难排查。

#### 10.5 成员函数特例化

类模板的成员模板、普通静态数据成员、普通成员函数都可以进行**全特化**。每个类模板作用域都需要一个`template<>`前缀。如果要对一个成员模板进行特化，则必须加上另一个`template<>`前缀，来说明该声明表示的是一个特化。

``` c++
template<typename T>
struct Foo {
    void f() { cout << "T::f" << endl; }
    void g() { cout << "T::g" << endl; }
};

template<>
void Foo<int*>::f() { cout << "int*::f" << endl; }

template<>
void Foo<int>::f() { cout << "int::f" << endl; }

int main()
{
    Foo<double> f1;
    f1.f(); // T::f
    f1.g(); // T::g

    Foo<int*> f2;
    f2.f(); // int*::f
    f2.g(); // T::g 

    Foo<int> f3;
    f3.f(); // int::f
    f3.g(); // T::g

    return 0;
}
```

#### 10.6 类模板特例化实例

##### 1. hash 

标准库中类模板特例化的一个典型例子就是 hash 模板：

``` c++
struct Foo {
    int val;
    string s;
    bool operator==(const Foo &rhs) const ;
};

namespace std {
    template<> 
    struct hash<Foo> { // 类模板全特化
        typedef size_t result_type;
        typedef Foo    augument_type;
        size_t operator()(const Foo &f) const {
            return hash<int>{}(f.val) + hash<string>{}(f.s);
        }
    };
}

// 这里的operator==必须在类外定义,否则不可见hash的定义
bool Foo::operator==(const Foo &rhs) const {
    return hash<Foo>{}(*this) == hash<Foo>{}(rhs);
}

int main()
{   
    unordered_set<Foo> s;
    s.insert({1,"a"});
    s.insert({2,"b"});
    s.insert({3,"c"});
    s.insert({2,"b"});

    for(auto &x : s) {
        cout << x.val << ' ' << x.s << endl;
    }
    return 0;                        
}
```

##### 2. remove_reference

``` c++
template<typename T> struct Remove_reference {
    typedef T type;
};
template<typename T> struct Remove_reference<T&> {
    typedef T type;
};
template<typename T> struct Remove_reference<T&&> {
    typedef T type;
};
```

### 11. 重载与模板

#### 11.1 函数匹配

函数模板可以被“另外一个模板”或“非模板函数”重载。依然按照函数模板的定义，重载函数必须有不同数量或类型的参数。

注意，这里是是另一个“模板”而不是另一个“模板实例”。“模板实例”与原模板的关系是“特例化”而不是“重载”。在函数匹配时，先考虑重载，当匹配到函数模板时，再考该模板的虑特例化。

如果涉及模板函数，则函数匹配规则会在以下几方面收到影响：

* 对于一个调用，其候选函数包括所有模板实参推断成功的函数模板实例。

* 候选的函数模板总是可行的，因为模板实参推断会排除任何不可行的模板。

与往常一样，可行函数（模板与非模板）按类型转换（如果对此调用需要的话）来排序。此时，如果恰有一个函数提供比任何其他函数都更好的匹配，则选择此函数，但是，如果有多个函数提供同样好的匹配，则：

1. 如果同样好的函数中只有一个是非模板函数，则选择此函数。
2. 如果同样好的函数中没有非模板函数，而有多个函数模板，且其中一个模板比其他模板更特例化，则选择此模板。
3. 否则，此函数调用有歧义

不过要注意，可以用于函数模板调用的类型转换是非常有限的：

1. 函数/数组转换为对应指针
2. 添加底层 const
3. 派生类到基类的指针转换

总而言之，当模板函数与普通函数并存，且它们的调用“一样好”时，编译器会优先调用**“最特例化”**的。

* 对于普通函数而言，我们可以将其理解为比“全特化”还好特化的模板，因此它不仅特化了所有参数，甚至把模板都特化“没了”。
* 对于 `T` 和 `T*` 来说，显然 `T*` 更特例化，因为 `T` 可以表示任何类型，而 `T*` 只能表示指针，

例如：

``` c++
// 通用模板
template<typename A, typename B>
void f(A a, B b) { puts("AB"); }

// 偏特化
template<typename B>void f(int a, B b){puts("IB");}

// 全特化
template<>void f(int a, int b){puts("[tmplate]II");}

// 普通函数
void f(int a, int b) {puts("II");}

// 普通函数
void f(int a, double b){puts("ID");}

int main()
{   
    f(1, 2);       // II
    f(3.14, 3.14); // AB
    f(1, "hello"); // IB
    return 0;
}
```

下面是一个稍微有点误导性的匹配：

``` c++
// 通用模板
template<typename T>
void f(T x) { puts("T"); }

// 偏特化
template<typename T>
void f(const T *p) { puts("const T*"); }

int main()
{   
    int *p = new int(10);
    f(p);
    return 0;
}
```

实际上，``f(p)`` 匹配的是通用模板。我们现在来分析一下：`p` 本身的类型是 `int *` 。对于第一个版本，参数类型推导为 `int*`；对于第二个版本参数类型被推导为 `const int*`。而在 C++ 中，指针的常量性会影响匹配优先级，因此这里认为通用模板更好。

#### 11.2 缺少声明可能导致程序行为异常

通常，如果使用了一个忘记声明的函数，代码将编译失败。但在涉及模板函数时，编译器可以从模板实例化出与调用匹配的版本。

因此，当涉及函数模板的重载时：在定义任何函数之前，记得声明所有重载的函数版本。这样，就不必担心由于未遇到我们希望调用的函数而实例化一个并非你所需的版本。

注意，如果声明位于调用函数之后，也属于“缺少声明”的情况，因为在调用函数之后的声明对调用函数而言依然是不可见的。

## 十一、标准库特殊设施

### 1. tuple

`tuple`，一个<font color=blue>**快速而随意**</font>的数据结构。

`std::tuple` 是 C++11 引入的一个模板类，用于将多个不同类型的值组合成一个单一对象。它类似于结构体，但不需要预先定义类型名称。

常用 **API**：

``` C++
#include <tuple>

tuple<T1,T2,...,Tn> t; // 默认构造函数，值初始化
tuple<T1,T2,...,Tn> t(v1,v2,...,vn); 
make_tuple(v1,v2,...,vn);	

t1 == t2;	 // 只有两个tuple大小相等时才能比较（==、<...）,
t1 != t2;    // 字典序比较，类型不必对应相同，但应该可以相互转换,显然比较一个string和一个int不合法
t1 relop t2; 

/*========================= 下面三个都是编译时操作 ======================*/
// e.g. using TupleType = tuple<int,double>;
get<i>(t);	 // 返回第i个成员的引用（左值/右值）,i从0开始,必须是常量表达式
tuple_element<i,TupleType>::type; // 返回第i个成员的类型,i是常量表达式
tuple_size<TupleType>::value; // 一个类模板,可以通过一个tuple类型来初始化。
							  // 他有一个名为value的public constexpr static数据成员,
							  // 类型为size_t，表示给定tuple类型中成员的个数
```

#### 1.1 创建和初始化

```cpp
#include <tuple>
#include <string>

// 创建tuple
std::tuple<int, double, std::string> t1(10, 3.14, "hello");

// 使用make_tuple自动推导类型
auto t2 = std::make_tuple(20, 2.718, "world");

// C++17结构化绑定
auto [i, d, s] = t1;
```

#### 1.2 访问元素

```cpp
// 使用std::get按索引访问
std::cout << std::get<0>(t1) << "\n";  // 输出10
std::cout << std::get<2>(t1) << "\n";  // 输出"hello"

// C++14起可以使用类型访问(类型必须唯一)
std::cout << std::get<int>(t1) << "\n";  // 输出10
```

#### 1.3 比较操作

```cpp
auto t1 = make_tuple(1, 2);   // tuple<int,int>
auto t2 = make_tuple(1, 2);
auto t3 = make_tuple(1, 2.0); // tuple<int,double>
auto t4 = make_tuple(1, "hello");
auto t5 = make_tuple(1, 2, 3);
cout << boolalpha;
cout << (t1 == t2) << endl; // true
cout << (t1 == t3) << endl; // true
cout << (t1 == t4) << endl; // error: ISO C++ forbids comparison between pointer and integer
cout << (t1 == t5) << endl; // error: static assertion failed: tuple objects can only be compared if they have equal sizes.
```

#### 1.4 tuple拆包

```cpp
int x;
double y;
std::string z;

// 使用tie拆包
std::tie(x, y, z) = t1;

int a, b, c;
std::tie(a, b, c) = make_tuple<int,int,int>(1, 2, 3);

// 忽略某些元素
std::tie(x, std::ignore, z) = t1;
```

`std::tie(x, y, z) = t1;` 是 C++ 中一种非常实用的语法，用于**将 `tuple` 的元素解包（unpack）到指定的变量中**。

* 右侧 `t1` 必须是一个 `tuple`，且元素数量与左侧的变量数量相同。元素的类型必须兼容（可隐式转换或相同）。
* `std::tie(x, y, z)` 会生成一个 `tuple`，其中每个元素是对变量 `x, y, z` 的**左值引用**（即 `std::tuple<int&, double&, std::string&>`）。当执行 `= t1` 时，实际上调用的是 `tuple` 的赋值运算符，将 `t1` 的每个元素依次赋值给 `std::tie` 中对应的引用变量。

`std::tie` 的简化实现：

```cpp
template<typename... Args>
auto tie(Args&... args) {
    return std::tuple<Args&...>(args...);  // 返回一个引用组成的tuple
}
```

- 它返回的是一个包含**左值引用**的 `tuple`，因此赋值操作会直接修改绑定的变量。

C++17 引入了更简洁的**结构化绑定**（Structured Binding），可以替代 `std::tie`：

```cpp
auto [x, y, z] = t1;  // 直接解包到新变量（无需预先声明变量）
```

#### 1.5 tuple连接

```cpp
auto t4 = std::tuple_cat(t1, t2);  // 包含6个元素
```

#### 1.6 运行时索引访问

##### 1. `if constexpr`

```cpp
template<std::size_t I = 0, typename... Tp> // I必须有默认值0
void print_tuple(const std::tuple<Tp...>& t) {
    if constexpr(I < sizeof...(Tp)) {
        std::cout << std::get<I>(t) << " ";
        print_tuple<I+1>(t);
    }
}
```

`I` 被初始化为0的原因有：

1. **索引起始点**：在 C++ 中，元组元素的索引从 0 开始，设置  `I = 0`  让递归从第一个元素开始。

2. **递归入口**：当你调用 `print_tuple(t)` 而不指定模板参数时，`I` 会使用默认值 0，这提供了一个方便的入口点。

3. 递归展开：这个函数通过在每次递归调用时增加 `I` 来实现从元组的第一个元素到最后一个元素的遍历：

   ```cpp
   print_tuple<0>(t) -> print_tuple<1>(t) -> print_tuple<2>(t) -> ...
   ```

   如果不设置 `I` 的默认值为0，用户每次调用时都需要显式指定起始索引：`print_tuple<0>(t)`，这会使函数使用起来不那么方便。

关键特性:

1. **编译时递归**：
   - 递归展开完全在**编译时**完成
   - 生成的代码是线性的，没有运行时递归开销
2. **`if constexpr`**：
   - C++17 特性，确保未使用的分支**不会实例化**
   - 避免编译错误(如当 `I` 超出范围时尝试 `get<I>`)

在这里，如果我们不使用 `if constexpr`，那么在当 `I == sizeof...(Tp)` 时，编译器依然会生成 `std::cout << std::get<I>(t) << " "`  的代码。而由于 `std::get<I>(t)` 对于 `t` 的访问越界，因此这里会报错。

##### 2. `std::enable_if_t`

如果不用 `if constexpr`，需要两个模板，并且借助 `std::enable_if_t`。

`std::enable_if_t` 是 C++ 标准库中的一个模板元编程工具，用于在**编译期**根据条件**选择性启用或禁用**某个函数或类模板。它是 `std::enable_if` 的简化版本（`_t` 表示 `::type` 的缩写）。

`std::enable_if` 的作用是：

- 如果某个编译期条件为 `true`，则提供一个有效的类型（通常是 `void` 或指定的类型）。

- 如果条件为 `false`，则**不提供任何类型**，导致模板实例化失败（但不会报错，而是被 **SFINAE（Substitution Failure Is Not An Error** 规则排除）。

  > **FINAE（Substitution Failure Is Not An Error）** 是 C++ 模板元编程中的一条核心规则，直译为“替换失败并非错误”。它的作用是：**在模板参数推导或替换过程中，如果某个模板实例化失败，编译器不会直接报错，而是静默忽略该候选，继续尝试其他可行的重载**。

```cpp
template<std::size_t I, typename... Tp>
std::enable_if_t<I == sizeof...(Tp)> print_tuple(const std::tuple<Tp...> &t)
{
    // do nothing
}

template<std::size_t I = 0, typename... Tp>
std::enable_if_t<I < sizeof...(Tp)> print_tuple(const std::tuple<Tp...> &t)
{
    std::cout << get<I>(t) << std::endl;
    print_tuple<I + 1>(t);
}
```

##### 3. `std::apply`

C++17 引入了 `std::apply`、结构化绑定和折叠表达式，我们可以利用这些新特性来实现更为现代化的 `print_tuple`，并且不需要借助递归。



```cpp
template<typename... Args>
void print_tuple_modern(const std::tuple<Args...> &t)
{
    //std::apply([](const Args&... args){
    std::apply([](const auto&... args){
        ((cout << args << endl), ...);
    }, t);
}
```

#### 1.7 Tuple-like Types

在 C++ 中，**类元组类型（Tuple-like Types）** 是指那些 **行为类似 `std::tuple` 的自定义或标准库类型**，它们可以通过 `std::get` 和 `std::tuple_size` 进行结构化访问。这种设计允许 `std::apply` 等工具不仅支持 `std::tuple`，还能兼容其他符合“类元组接口”的类型。

##### **1. 类元组类型的核心特征**

一个类型要成为“类元组类型”，必须满足以下条件（通过**模板特化**实现）：

| 特性                  | 所需支持                                                     | 示例类型                  |
| :-------------------- | :----------------------------------------------------------- | :------------------------ |
| **`std::tuple_size`** | 提供元素数量的编译时常量（通过 `std::tuple_size<T>::value`） | `std::array`, `std::pair` |
| **`std::get`**        | 支持通过索引访问元素（通过 `std::get<I>(obj)`）              | 自定义结构体（需特化）    |
| **元素类型可推导**    | 每个位置的元素类型可通过 `std::tuple_element<I, T>::type` 获取 | `std::pair<int, double>`  |

``` cpp
pair<int,pair<int,int>> p{1,{2,3}};
cout << std::tuple_size<decltype(p)>::value << endl; // 2
cout << get<0>(p) << endl; // 1

array<int,5> arr{1,2,3,4,5};
cout << std::tuple_size<decltype(arr)>::value << endl; // 5
cout << get<1>(arr) << endl; // 2

tuple<int,double,string> t{1,3.14,"hello"};
cout << std::tuple_size<decltype(t)>::value << endl; // 3
cout << get<2>(t) << endl; // hello
```

##### **2. 标准库中的类元组类型**

以下类型天然支持类元组接口，可直接用于 `std::apply`：

###### **(1) `std::pair`**

```cpp
std::pair<std::string, int> p = {"age", 25};
auto [key, value] = p;  // 结构化绑定
std::apply([](const auto& k, const auto& v) { 
    std::cout << k << ": " << v; 
}, p);  // 输出 "age: 25"
```

###### **(2) `std::array`**

```cpp
std::array<int, 3> arr = {1, 2, 3};
std::apply([](int a, int b, int c) { 
    std::cout << a + b + c; 
}, arr);  // 输出 6
```

###### **(3) 普通结构体（需手动特化）**

若希望自定义类型支持类元组接口，需特化 `std::tuple_size` 和 `std::get`：

```cpp
#include <iostream>
#include <tuple>

struct Point {
    int x;
    double y;
};

// 特化类元组接口
namespace std {
    // 特化 tuple_size
    template<>
    struct tuple_size<Point> : std::integral_constant<size_t, 2> {};
    
    // 特化 tuple_element
    template<>
    struct tuple_element<0, Point> { using type = int; };
    
    template<>
    struct tuple_element<1, Point> { using type = double; };
}

// 非const对象的get函数
template<std::size_t I>
decltype(auto) get(Point& p) {
    if constexpr (I == 0) return p.x;
    else if constexpr (I == 1) return p.y;
}

// const对象的get函数
template<std::size_t I>
decltype(auto) get(const Point& p) {
    if constexpr (I == 0) return p.x;
    else if constexpr (I == 1) return p.y;
}

// 右值引用的get函数
template<std::size_t I>
decltype(auto) get(Point&& p) {
    if constexpr (I == 0) return std::move(p.x);
    else if constexpr (I == 1) return std::move(p.y);
}

int main() {
    Point p {10, 3.14};
    
    // 正确使用结构化绑定
    auto [x, y] = p;  // 复制值
    std::cout << "结构化绑定(复制): " << x << ", " << y << std::endl;
    
    // 使用引用绑定
    auto& [xr, yr] = p;  // 引用绑定
    xr = 20;  // 修改原始值
    std::cout << "修改后: " << p.x << ", " << p.y << std::endl;
    
    // 使用 std::apply
    std::apply([](int a, double b) {
        std::cout << "通过apply: " << a << ", " << b << std::endl;
    }, std::tie(p.x, p.y));
    
    return 0;
}
```

##### **3. 为什么需要类元组类型？**

- **一致性**：允许 `std::apply`、结构化绑定等工具统一处理多种类型。
- **扩展性**：用户可自定义类型融入元组生态。
- **避免冗余**：无需为每种类型单独实现遍历逻辑。

##### **4. 实际应用场景**

###### **(1) 解包函数返回值**

```cpp
std::tuple<int, std::string> get_data() { 
    return {42, "answer"}; 
}

auto [num, str] = get_data();  // 结构化绑定
```

###### **(2) 遍历异构数据**

```cpp
std::array<std::variant<int, std::string>, 3> data = {1, "hello", 42};
std::apply([](const auto&... args) { 
    ((std::cout << args << " "), ...); 
}, data);
```

###### **(3) 与结构化绑定结合**

```cpp
std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
for (const auto& [key, value] : m) {  // 类元组解包
    std::cout << key << ": " << value << std::endl;
}
```

##### **5. 注意事项**

1. **编译期检查**：类元组接口必须在编译时完全定义。
2. **性能**：无运行时开销，所有操作在编译期解析。
3. **C++版本**：需 C++17 或更高（结构化绑定和 `std::apply` 的支持）。

##### **6. 总结**

- **类元组类型**是通过特化 `std::tuple_size` 和 `std::get` 来模拟 `std::tuple` 行为的类型。
- 标准库中的 `std::pair`、`std::array` 等已内置支持，自定义类型需手动实现。
- 核心价值在于**统一元组操作接口**，提升代码复用性和可读性。

#### 1.8 `std::integral_constant`

`std::integral_constant` 是 C++ 标准库中的一个**模板类**，用于表示**编译时常量**（即编译期已知的常量值）。它是 <font color=blue>**类型与值的双重封装**</font>，既包含类型信息，也包含具体的值。

##### 1. 基本定义

```cpp
template<class T, T v>
struct integral_constant {
    static constexpr T value = v;    // 存储的值
    using value_type = T;                 // 值的类型
    using type       = integral_constant; // 自身类型（便于元编程）
    constexpr operator value_type() const noexcept { return value; }   // 隐式转换为值
    constexpr value_type operator()() const noexcept { return value; } // 函数调用运算符
};
```

- **模板参数**：
  - `T`：常量的类型（如 `int`、`size_t`、`bool` 等）。
  - `v`：常量的值（必须是编译期常量）。
- **核心成员**：
  - `value`：存储的常量值。
  - `value_type`：值的类型别名。
  - `type`：自身的类型别名（用于元编程中的类型推导）。

##### 2. 常见用途

###### (1) 表示编译期常量

直接作为值使用：

```cpp
using Two = std::integral_constant<int, 2>;
static_assert(Two::value == 2);  // 编译期断言
```

###### (2) 标准库中的预定义别名

标准库提供了两个常用的特化：

```cpp
// 表示 true/false 的类型
using true_type  = std::integral_constant<bool, true>;
using false_type = std::integral_constant<bool, false>;
```

可以用于元编程中的条件分发，通过模板特化选择不同逻辑：

```cpp
template<typename T>
void foo(T t, std::true_type) { /* T 是整数类型时的处理 */ }

template<typename T>
void foo(T t, std::false_type) { /* 其他情况 */ }

// 使用时：
foo(42, std::is_integral<int>{}); // 调用 true_type 版本
```

##### 4. 示例：自定义编译期常量

```cpp
using Answer = std::integral_constant<int, 42>;

constexpr int a = Answer::value;  // a = 42
constexpr int b = Answer{};       // b = 42（隐式转换）
constexpr int c = Answer{}();     // c = 42（调用 operator()）
```

##### 5. 总结

| 关键点                  | 说明                                                         |
| :---------------------- | :----------------------------------------------------------- |
| **本质**                | 封装了类型 `T` 和值 `v` 的编译期常量。                       |
| **核心用途**            | 元编程中表示类型和值，尤其在类型特性（type traits）中。      |
| **与 `constexpr` 区别** | `integral_constant` 是类型级别的表示，而 `constexpr` 是值级别的修饰。 |
| **常见场景**            | `tuple_size`、`true_type`/`false_type`、条件编译等。         |

通过 `integral_constant`，C++ 可以在编译期将值和类型统一处理，为模板元编程提供了基础支持。

#### 1.9 `std::apply`

`std::apply` 是 C++17 标准库中引入的一个函数模板，用于**将元组（`std::tuple`）或类元组类型的元素展开作为参数传递给可调用对象（函数、Lambda 表达式等）**。它提供了一种简洁的方式来处理元组中的元素，避免了手动使用 `std::get` 或递归展开的复杂性。

##### **1. 核心功能**

- **参数展开**：将元组的元素解包，作为单独的参数传递给函数。
- **支持任意可调用对象**：包括普通函数、函数对象、Lambda 表达式等。
- **编译期完成**：所有操作在编译期确定，无运行时开销。

##### **2. 函数原型**

```cpp
template <typename Fn, typename Tuple>
decltype(auto) apply(Fn&& fn, Tuple&& t);
```

- **`Fn&& fn`**：可调用对象（函数、Lambda 等）。
- **`Tuple&& t`**：元组或类元组对象（如 `std::array`、`std::pair`）。
- **返回值**：返回 `fn` 调用的结果类型。

##### **3. 基本用法**

###### **示例 1：传递元组给普通函数**

```cpp
#include <iostream>
#include <tuple>
#include <utility> // for std::apply

void print_sum(int a, int b, int c) {
    std::cout << a + b + c << std::endl;
}

int main() {
    std::tuple<int, int, int> t = {1, 2, 3};
    std::apply(print_sum, t); // 输出 6
}
```

###### **示例 2：结合 Lambda 表达式**

```cpp
auto t = std::make_tuple(42, 3.14, "hello");
std::apply([](int x, double y, const std::string& z) {
    std::cout << x << " " << y << " " << z << std::endl;
}, t);
// 输出: 42 3.14 hello
```

##### **4. 高级用法**

###### **(1) 使用 `auto` 自动推导参数类型**

```cpp
std::apply([](const auto&... args) {
    ((std::cout << args << " "), ...); // 折叠表达式打印所有元素
}, t);
```

###### **(2) 处理类元组类型**

`std::apply` 也支持其他类元组类型（需满足 `std::tuple_size` 和 `std::get` 的接口）：

```cpp
std::array<int, 3> arr = {1, 2, 3};
std::apply([](int a, int b, int c) { /* ... */ }, arr);

std::pair<std::string, int> p = {"key", 42};
std::apply([](const auto& key, const auto& value) { /* ... */ }, p);
```

###### **(3) 返回值处理**

```cpp
auto t = std::make_tuple(2, 3);
int result = std::apply([](int a, int b) { return a * b; }, t);
std::cout << result; // 输出 6
```

#### 1.10 实现原理

`std::tuple` 是 C++ 标准库提供的通用多类型聚合容器，其核心原理是通过 **递归模板继承** 和 **编译期计算** 实现的。以下是它的实现机制分解：

##### **1. 基本结构：递归模板继承**

`tuple` 的本质是一个**模板递归结构**，每个元素通过继承链存储。例如，`tuple<int, double, char>` 的简化实现如下：

```cpp
// 基础模板（空元组）
template<typename... Types> 
class tuple;

// 特化：递归继承
template<typename Head, typename... Tail>
class tuple<Head, Tail...> : private tuple<Tail...> { 
    Head value;  // 存储当前元素
public:
    // 构造函数等...
};
```

- **递归继承**：
  - `tuple<int, double, char>` 继承自 `tuple<double, char>`
  - `tuple<double, char>` 继承自 `tuple<char>`
  - `tuple<char>` 继承自 `tuple<>`（空基类）
- **存储布局**：
  - 最终对象的内存布局是递归嵌套的，元素按声明顺序存储（可能受空基类优化影响）。

##### **2. 元素访问：编译期索引**

通过模板元编程在编译期确定元素位置，典型实现使用 `std::get` 和 `std::tuple_element`。

###### (1) `tuple_element` 获取类型

```cpp
template<size_t I, typename T>
struct tuple_element;

// 递归终止条件
template<typename Head, typename... Tail>
struct tuple_element<0, tuple<Head, Tail...>> {
    using type = Head;
};

// 递归解析
template<size_t I, typename Head, typename... Tail>
struct tuple_element<I, tuple<Head, Tail...>> 
    : tuple_element<I-1, tuple<Tail...>> {};
```

- **原理**：递归递减索引 `I` 直到 0，返回对应类型。

###### (2) `get` 函数获取值

```cpp
template<size_t I, typename... Types>
auto& get(tuple<Types...>& t) {
    using Base = typename tuple_element<I, tuple<Types...>>::BaseType;
    return static_cast<Base&>(t).value;  // 通过继承链向上转型
}
```

- **关键点**：
  - 通过 `static_cast` 沿继承链跳转到正确的基类。
  - 返回基类中存储的 `value`。

##### **3. 构造与赋值**

###### (1) 构造函数

```cpp
template<typename... UTypes>
tuple(UTypes&&... args) 
    : Base(std::forward<UTypes>(args.tail)...), 
      value(std::forward<UTypes>(args.head)...) {}
```

- 使用完美转发（`std::forward`）初始化每个元素。

###### (2) 赋值操作

通过递归展开参数包实现逐元素赋值。

##### **4. 编译期计算：`tuple_size`**

```cpp
template<typename... Types>
struct tuple_size<tuple<Types...>> 
    : integral_constant<size_t, sizeof...(Types)> {};
```

- 直接通过 `sizeof...(Types)` 获取元素数量。

##### **5. 空基类优化（EBCO）**

- 如果 `tuple` 的某个基类是空类（如无状态的 lambda 或空结构体），编译器会优化其存储，不占用额外空间。

##### **6. 结构化绑定支持**

C++17 的结构化绑定依赖以下接口：

```cpp
// 1. 特化 std::tuple_size
template<> struct tuple_size<MyType> : integral_constant<size_t, N> {};

// 2. 特化 std::tuple_element
template<size_t I> struct tuple_element<I, MyType> { using type = /*...*/; };

// 3. 提供 get<I>() 函数
template<size_t I> auto get(const MyType& obj);
```

##### **7. 简化实现示例**

```cpp
template<typename... Types>
class Tuple;

template<>
class Tuple<> {};

template<typename Head, typename... Tail>
class Tuple<Head, Tail...> : Tuple<Tail...>
{
    Head val{};
public:
    Tuple() = default;
    Tuple(const Head &_val, const Tail&... tail) : Tuple<Tail...>(tail...), val(_val) {}
    
    template<size_t I>
    auto& get() {
        if constexpr(I == 0)    return val;
        else return Tuple<Tail...>::template get<I-1>();
    }

    template<size_t I>
    const auto& get() const {
        if constexpr (I == 0) return val;
        else return Tuple<Tail...>::template get<I - 1>();
    }
};


int main() 
{
    // Tuple<int,double,string> t{1, 3.14, "hello"};
    Tuple<int,double,string> t;
    cout << t.get<0>() << endl;
    cout << t.get<1>() << endl;
    cout << t.get<2>() << endl;

    return 0;
}
```

##### **8. 性能与设计特点**

1. **编译期解析**：所有类型计算和索引访问均在编译期完成，无运行时开销。
2. **类型安全**：通过模板确保类型匹配，避免运行时错误。
3. **内存紧凑**：依赖继承和 EBCO 优化存储布局。
4. **通用性**：可容纳任意数量和类型的元素。

##### **9. 与 `std::pair` 和 `std::array` 的对比**

| 特性         | `std::tuple`  | `std::pair`         | `std::array` |
| :----------- | :------------ | :------------------ | :----------- |
| **元素数量** | 任意          | 固定 2 个           | 固定 `N` 个  |
| **元素类型** | 可不同        | 可不同              | 必须相同     |
| **访问方式** | `std::get<I>` | `.first`, `.second` | `operator[]` |
| **内存布局** | 递归继承      | 扁平结构            | 连续数组     |

### 2. bitset

`bitset` 使得位运算的使用更为容易，它能够处理超过最长整型类型大小的位集合。

* 作为一个类模板，`bitset` 类似 `array` 类，有固定的大小，需要用一个**常量表达式**指定。

* `bitset` 中的二进制位从 **0** 开始编号。编号从 **0** 开始的位称为**低位（low-order）**，编号到 **n-1** 结束的二进制位称为**高位（high-order）**

#### 2.1 初始化方式

``` cpp
bitset<n> b;          // 创建一个n位的bitset，所有位初始化为0
bitset<n> b(u);       // 用无符号长整型u初始化
bitset<n> b(s, pos, m, zero='0', one='1');  // 从字符串s的pos位置开始取m个字符
bitset<n> b(cp, m, zero='0', one='1');      // 从字符数组cp中取m个字符
```

* 注意：字符串的最左字符（下标 **0**）对应 `bitset` 的最高位

#### 2.2 状态检查

``` cpp
b.any();    // 是否有任何位被设置为1
b.all();    // 是否所有位都被设置为1
b.none();   // 是否没有位被设置为1
b.count();  // 返回被设置为1的位的数量
b.size();   // 返回bitset的位数（即模板参数n）
```

#### 2.3 位操作

``` cpp
b.test(pos);   // 检查pos位置是否为1（会进行边界检查）
b[pos];        // 访问pos位的引用（不进行边界检查）

b.set(pos, v); // 将pos位置设置为v（v默认为true）
b.set();       // 将所有位设置为1

b.reset(pos);  // 将pos位置设置为0
b.reset();     // 将所有位设置为0

b.flip(pos);   // 翻转pos位的状态
b.flip();      // 翻转所有位的状态
```

#### 2.4 类型转换

``` cpp
b.to_ulong();   // 转换为unsigned long（可能抛出overflow_error）
b.to_ullong();  // 转换为unsigned long long（C++11）
b.to_string(zero='0', one='1');  // 转换为字符串
```

#### 2.5 流操作

``` cpp
os << b;  // 以"0"/"1"形式输出
is >> b;  // 从输入流读取，遇到非0/1字符或读满n位停止
```

### 3. 正则表达式

C++11 引入了 `<regex>` 头文件，提供了强大的正则表达式支持。

基本组件:

1. **正则表达式对象**：`std::regex`
2. **匹配结果容器（针对 string 类型）:** **`std::smatch`** 
3. **匹配结果**：`std::match_results`
4. **算法函数**：
   - `std::regex_match` - 完全匹配
   - `std::regex_search` - 搜索匹配
   - `std::regex_replace` - 替换匹配
5. **迭代器**：
   - `std::regex_iterator` - 遍历所有匹配
   - `std::regex_token_iterator` - 遍历所有子匹配

#### 3.0 API 介绍

##### 1. 正则表达式对象：`std::regex`

构造函数：

```cpp
// 主要构造函数
regex(const std::string& pattern, 
      std::regex_constants::syntax_option_type flags = std::regex_constants::ECMAScript);
```

主要参数：

- `pattern`: 正则表达式模式字符串
- `flags`: 正则表达式选项，常用值包括：
  - `std::regex_constants::ECMAScript` (默认) - 使用 **ECMAScript** 语法
  - `std::regex_constants::icase` - 忽略大小写
  - `std::regex_constants::nosubs` - 不存储子表达式匹配
  - `std::regex_constants::optimize` - 优化匹配性能

主要方法：

- `flags()` - 返回构造时使用的标志
- `mark_count()` - 返回捕获组数量

##### 2. 匹配结果容器：`std::smatch`

`std::smatch` 是 `std::match_results<std::string::const_iterator>` 的别名，用于处理 `string` 类型。

主要方法：

- `empty()` - 是否匹配成功
- `size()` - 返回子匹配数量
- `str(n)` - 返回第 **n** 个子匹配的字符串
- `position(n)` - 返回第 **n** 个子匹配的位置
- `length(n)` - 返回第 **n** 个子匹配的长度
- `prefix()` - 返回匹配之前的部分
- `suffix()` - 返回匹配之后的部分
- `operator[](n)` - 访问第 **n** 个子匹配

##### 3. 匹配结果：`std::match_results`

`std::match_results` 是模板类，针对不同字符容器类型有不同特化:

- `std::cmatch`: `match_results<const char*>`
- `std::smatch`: `match_results<string::const_iterator>`
- `std::wsmatch`: `match_results<wstring::const_iterator>`

功能和方法与 `std::smatch` 相同。

##### 4. 算法函数

###### （1）`std::regex_match` - 完全匹配

```cpp
template <class BidirIt, class Alloc, class CharT, class Traits>
bool regex_match(BidirIt first, BidirIt last,
                 match_results<BidirIt, Alloc>& m,
                 const basic_regex<CharT, Traits>& e,
                 regex_constants::match_flag_type flags = regex_constants::match_default);

// 简化版本
bool regex_match(const std::string& s, std::smatch& m, const std::regex& e,
                 std::regex_constants::match_flag_type flags = std::regex_constants::match_default);

// 无需存储结果的版本
bool regex_match(const std::string& s, const std::regex& e,
                 std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
```

参数：

- `first`, `last` 或 `s`: 要匹配的字符串或迭代器范围
- `m`: 接收匹配结果的 `match_results` 对象
- `e`: 正则表达式对象
- `flags`: 匹配选项

返回值：

- `bool`: 整个字符串是否完全匹配正则表达式

###### （2）`std::regex_search` - 搜索匹配

```cpp
template <class BidirIt, class Alloc, class CharT, class Traits>
bool regex_search(BidirIt first, BidirIt last,
                  match_results<BidirIt, Alloc>& m,
                  const basic_regex<CharT, Traits>& e,
                  regex_constants::match_flag_type flags = regex_constants::match_default);

// 简化版本
bool regex_search(const std::string& s, std::smatch& m, const std::regex& e,
                  std::regex_constants::match_flag_type flags = std::regex_constants::match_default);

// 无需存储结果的版本
bool regex_search(const std::string& s, const std::regex& e,
                  std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
```

参数：

* 与 `regex_match` 相同

返回值：

- `bool`: 是否找到匹配

###### （3）`std::regex_replace` - 替换匹配

```cpp
template <class OutputIt, class BidirIt, class Traits, class CharT, class STraits>
OutputIt regex_replace(OutputIt out, BidirIt first, BidirIt last,
                       const basic_regex<CharT, Traits>& e,
                       const basic_string<CharT, STraits>& fmt,
                       regex_constants::match_flag_type flags = regex_constants::match_default);

// 返回字符串的版本
template <class Traits, class CharT, class STraits>
basic_string<CharT> regex_replace(const basic_string<CharT, STraits>& s,
                                 const basic_regex<CharT, Traits>& e,
                                 const basic_string<CharT, STraits>& fmt,
                                 regex_constants::match_flag_type flags = regex_constants::match_default);
```

参数：

- `out`: 输出迭代器
- `first`, `last` 或 `s`: 要处理的字符串或迭代器范围
- `e`: 正则表达式对象
- `fmt`: 替换格式字符串
- `flags`: 替换选项

返回值：

- 第一版本: 最后一次写入后的输出迭代器位置
- 第二版本: 替换后的新字符串

格式字符串中特殊序列：

- `$n`: 表示第 **n** 个捕获组
- `$&`: 表示整个匹配
- `$'`: 表示匹配前的文本
- `$'`: 表示匹配后的文本
- `$$`: 表示字面量的 **$** 符号

##### 5. 迭代器

###### （1）`std::regex_iterator` - 遍历所有匹配

```cpp
template <class BidirIt,
          class CharT = typename iterator_traits<BidirIt>::value_type,
          class Traits = regex_traits<CharT>>
class regex_iterator;

// 常用实例化
typedef regex_iterator<const char*> cregex_iterator;
typedef regex_iterator<string::const_iterator> sregex_iterator;
```

构造函数：

```cpp
regex_iterator(BidirIt first, BidirIt last, const regex_type& re, 
               regex_constants::match_flag_type m = regex_constants::match_default);
```

使用示例：

```cpp
std::string s = "Email: example@mail.com, another@example.com";
std::regex e("\\w+@\\w+\\.\\w+");
std::sregex_iterator iter(s.begin(), s.end(), e);
std::sregex_iterator end;
while (iter != end) {
    std::cout << iter->str() << std::endl;
    ++iter;
}
```

###### （2）`std::regex_token_iterator` - 遍历所有子匹配

```cpp
template <class BidirIt,
          class CharT = typename iterator_traits<BidirIt>::value_type,
          class Traits = regex_traits<CharT>>
class regex_token_iterator;

// 常用实例化
typedef regex_token_iterator<const char*> cregex_token_iterator;
typedef regex_token_iterator<string::const_iterator> sregex_token_iterator;
```

构造函数：

```cpp
regex_token_iterator(BidirIt first, BidirIt last, const regex_type& re,
                    int submatch = 0,
                    regex_constants::match_flag_type m = regex_constants::match_default);

regex_token_iterator(BidirIt first, BidirIt last, const regex_type& re,
                    initializer_list<int> submatches,
                    regex_constants::match_flag_type m = regex_constants::match_default);
```

使用示例：

```cpp
std::string s = "2023-03-15,2023-04-21,2023-05-30";
std::regex e("(\\d{4})-(\\d{2})-(\\d{2})");
// 提取所有年份（第一个捕获组）
std::sregex_token_iterator iter(s.begin(), s.end(), e, 1);
std::sregex_token_iterator end;
while (iter != end) {
    std::cout << *iter++ << std::endl; // 输出: 2023 2023 2023
}
```

或者用于拆分字符串：

```cpp
std::string s = "apple,banana,cherry";
std::regex e(",");
// -1表示返回分隔符之间的文本
std::sregex_token_iterator iter(s.begin(), s.end(), e, -1);
std::sregex_token_iterator end;
while (iter != end) {
    std::cout << *iter++ << std::endl; // 输出: apple banana cherry
}
```

#### 3.1 完全匹配

``` cpp
#include <iostream>
#include <regex>
#include <string>

int main() 
{
    std::string input = "Hello123";
    std::regex pattern("Hello\\d+"); // 匹配"Hello"后跟一个或多个数字
    
    if (std::regex_match(input, pattern))
        std::cout << "完全匹配!\n";
    else
        std::cout << "不匹配!\n";
    
    return 0;
}
```

#### 3.2 搜索匹配

``` cpp
#include <iostream>
#include <regex>
#include <string>

int main() 
{
    std::string input = "Subject: C++ regex tutorial";
    std::regex pattern("(\\w+): (.*)"); // 捕获组1: 冒号前的单词, 组2: 冒号后的内容
    
    std::smatch matches;
    if (std::regex_search(input, matches, pattern)) {
        std::cout << "匹配数量: " << matches.size() << "\n";
        std::cout << "完整匹配: " << matches[0] << "\n";
        std::cout << "组1: " << matches[1] << "\n";
        std::cout << "组2: " << matches[2] << "\n";
    }
    
    return 0;
}
```

#### 3.3 替换匹配

``` cpp
#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string text = "Quick brown fox";
    std::regex vowel_re("a|e|i|o|u", std::regex_constants::icase);
    
    // 替换所有元音为*
    std::string result = std::regex_replace(text, vowel_re, "*");
    std::cout << result << "\n"; // 输出: Q**ck br*wn f*x
    
    return 0;
}
```

#### 3.4 遍历所有匹配

``` cpp
#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string s = "Some numbers: 123, 456, 789";
    std::regex number_re("\\d+");
    
    auto numbers_begin = std::sregex_iterator(s.begin(), s.end(), number_re);
    auto numbers_end = std::sregex_iterator();
    
    std::cout << "找到 " << std::distance(numbers_begin, numbers_end) << " 个数字:\n";
    
    for (std::sregex_iterator i = numbers_begin; i != numbers_end; ++i) {
        std::smatch match = *i;
        std::cout << match.str() << "\n";
    }
    
    return 0;
}
```

#### 3.5 正则表达式语法选项

``` cpp
std::regex re("pattern", std::regex_constants::pattern);
```

常用 **pattern**：

- `std::regex_constants::icase` - 忽略大小写
- `std::regex_constants::nosubs` - 不存储子表达式匹配
- `std::regex_constants::optimize` - 优化匹配性能
- `std::regex_constants::ECMAScript` - 使用 **ECMAScript** 正则语法**（默认）**
- `std::regex_constants::basic` - 使用 **POSIX** 基本正则语法
- `std::regex_constants::extended` - 使用 **POSIX** 扩展正则语法
- `std::regex_constants::awk` - 使用 **AWK** 语法
- `std::regex_constants::grep` - 使用 **grep** 语法
- `std::regex_constants::egrep` - 使用 **egrep** 语法

#### 3.6 性能考虑

1. 编译正则表达式 (`std::regex` 构造) 是相对昂贵的操作，应尽可能重用**已编译**的正则表达式对象。
2. 对于简单字符串操作，有时标准字符串函数可能比正则表达式更高效。

#### 3.7 字符串字面量

* C++ 中的正则表达式在字符串中需要双重转义，例如 `\\d` 表示数字，因为 `\` 本身在 C++ 字符串中是转义字符
* 对于复杂的正则表达式，可以使用原始**字符串字面量 (raw string literals)**： `R"(\d+)"` 避免过多转义

#### 3.8 错误处理

正则表达式可能抛出 `std::regex_error` 异常：

```cpp
try {
    std::regex invalid_re("(\\d+"); // 缺少右括号
} catch (const std::regex_error& e) {
    std::cout << "正则表达式错误: " << e.what() << "\n";
}
```

#### 3.9 附: 基本正则表达式语法

在使用正则表达式之前需要注意：

1. 在许多编程语言中，正则表达式字符串需要转义反斜杠，所以 `\d` 要写成 `\\d`
2. 在测试复杂正则表达式时，可以使用在线工具如 [regex101.com](regex101.com)
3. 过于复杂的正则表达式可能会导致性能问题，尤其是在处理大文本时

##### 1. 基本匹配符

1. **字面字符**：直接匹配自身，如 `a` 匹配字符 "a"
2. **点号 `.`**：匹配任意单个字符（通常除了换行符）
3. **转义字符 `\`**：用于转义特殊字符，如 `\.` 匹配实际的点号

##### 2. 字符类

1. 方括号 `[...]`

   ：匹配括号内的任意一个字符

   - `[abc]` 匹配 "a"、"b" 或 "c"
   - `[a-z]` 匹配任何小写字母
   - `[A-Z0-9]` 匹配任何大写字母或数字

2. 否定字符类 `[^...]`

   ：匹配不在括号内的任意字符

   - `[^abc]` 匹配除 "a"、"b" 和 "c" 之外的任何字符

##### 3. 预定义字符类

1. **`\d`**：匹配任何数字，等同于 `[0-9]`
2. **`\D`**：匹配任何非数字，等同于 `[^0-9]`
3. **`\w`**：匹配任何字母、数字或下划线，等同于 `[a-zA-Z0-9_]`
4. **`\W`**：匹配任何非字母、数字或下划线
5. **`\s`**：匹配任何空白字符（空格、制表符、换行符等）
6. **`\S`**：匹配任何非空白字符

##### 4. 边界匹配

1. **`^`**：匹配行的开头
2. **`$`**：匹配行的结尾
3. **`\b`**：匹配单词边界
4. **`\B`**：匹配非单词边界

##### 5. 数量限定符

1. `*`

   ：匹配前面的元素零次或多次（0+）

   - `a*` 匹配 ""、"a"、"aa"、"aaa" 等

2. `+`

   ：匹配前面的元素一次或多次（1+）

   - `a+` 匹配 "a"、"aa"、"aaa" 等，但不匹配 ""

3. `?`

   ：匹配前面的元素零次或一次（0-1）

   - `a?` 匹配 "" 或 "a"

4. `{n}`

   ：匹配前面的元素恰好 n 次

   - `a{3}` 匹配 "aaa"

5. `{n,}`

   ：匹配前面的元素至少 n 次

   - `a{2,}` 匹配 "aa"、"aaa" 等

6. `{n,m}`

   ：匹配前面的元素 n 到 m 次

   - `a{1,3}` 匹配 "a"、"aa" 或 "aaa"

##### 6. 贪婪与非贪婪匹配

默认情况下，量词是贪婪的（尽可能多地匹配）：

- **贪婪匹配**：`.*` 尽可能匹配更多字符
- **非贪婪匹配**：`.*?` 尽可能匹配更少字符（在量词后加 `?`）

##### 7. 分组和捕获

1. 括号 `(...)`

   ：创建捕获组

   - `(abc)` 匹配并捕获 "abc"

2. 非捕获组 `(?:...)`

   ：匹配但不捕获

   - `(?:abc)` 匹配 "abc" 但不创建捕获组

##### 8. 分支结构

1. 管道符 `|`

   ：表示"或"关系

   - `a|b` 匹配 "a" 或 "b"
   - `cat|dog` 匹配 "cat" 或 "dog"

### 4. 随机数

直观的、笼统的随机数生成器的两个评估指标：

1. 生成效率
2. 随机数质量

评估**随机数生成器（RNG）**的质量是确保其适用于特定应用场景的关键步骤。以下是评估随机数生成器的主要指标：

1. **统计随机性测试**
   1. 频数测试（Monobit Test）
      1. 检查0和1的分布是否均匀
      2. 理想情况下，0和1的比例应接近50%/50%
   2. 频数块测试（Frequency Test within a Block）
      1. 检查固定大小块中1的比例是否符合预期
      2. 通常使用128位或256位的块大小
   3. 游程测试（Runs Test）
      1. 检查连续相同值(游程)的长度和数量
      2. 例如：0和1的交替不应呈现明显模式
   4. 最长游程测试（Longest Run Test）
      1. 检查序列中最长的连续0或1序列是否在预期范围内
   5. 离散傅里叶变换测试（Spectral Test）
      1. 检测序列中的周期性模式
2. **信息论指标**
   1. 熵（Entropy）
      1. 测量序列的不确定性
      2. 计算公式：$H = -\sum p(x)*log_2^{p(x)}$
      3. 理想值：对于均匀分布的n位随机数，熵应为n
   2. 最小熵（Min-Entropy）
      1. $H_{ₘᵢₙ} = -log_₂^{max(p(x))}$
      2. 对密码学应用尤为重要
3. **密码学安全性指标**
   1. 不可预测性
      1. 即使知道之前的所有输出，也无法预测下一个输出
   2. 抗碰撞性
      1. 两个不同输入产生相同输出的概率极低
   3. 种子敏感性
      1. 微小种子变化应导致完全不同的输出序列
   4. 前向安全性
      1. 即使内部状态被泄露，之前的输出也无法被重构
4. **性能指标**
   1. 生成速度
      1. 每秒能生成的随机数数量
      2. 通常以MB/s或百万数/秒衡量
   2. 初始化时间
      1. 从种子到第一个输出所需时间
   3. 内存使用
      1. 生成器维持状态所需内存
5. **其他重要指标**
   1. 周期长度
      1. 序列开始重复前的长度
      2. 优质RNG应有极长周期(如2^128或更长)
   2. 重复性
      1. 给定相同种子是否产生相同序列
      2. 对某些应用(如仿真)是必要特性
   3. 均匀性
      1. 输出值在定义区间内的分布均匀程度
   4. 独立性
      1. 序列中相邻数值之间不应存在统计相关性

**常用测试套件：**

* Diehard测试套件：包含多种统计测试
* NIST SP 800-22：美国国家标准与技术研究院的测试套件
* TestU01：包含SmallCrush、Crush和BigCrush三个级别的测试
* PractRand：现代、严格的测试套件

**应用场景考虑：**不同应用对RNG的要求不同：

* 仿真/Monte Carlo模拟：侧重统计特性
* 密码学应用：侧重不可预测性和安全性
* 游戏：通常需要平衡性能和随机性
* 科学计算：需要可重复性和长周期

评估 RNG 时应根据具体应用场景选择合适的指标组合。

#### 4.1 C 库函数 rand 的问题

**1.随机性和质量较差**：

* `rand()` 生成的伪随机数序列质量一般，随机性不强，特别是在现代计算需求下很难满足要求。它使用的算法通常是**线性同余法**，容易产生**明显的模式**，不适合对随机性有高要求的应用，比如密码学。

**2.范围有限**：

* `rand()` 生成的数在 `[0, RAND_MAX]` 范围内，而 `RAND_MAX` 通常较小（如 `32767` 或 `2147483647`），因此随机数范围受限，不适合需要更大范围或精度的应用。

**3.缺乏分布控制**：

* `rand()` 只能生成均匀分布的随机数，无法直接生成特定分布的随机数（如正态分布或伯努利分布）。如果需要其他分布的随机数，需要手动实现，增加了代码复杂度。

**4.功能有限**：

* `rand()` 只能生成在 `[0, RAND_MAX]` 范围内的整数，因此对于例如浮点数的生成，程序员只能试图转换 `rand` 生成的浮点数的范围、类型或分布，而这个过程常常会引入非随机性以及其它问题。

  * 例如，由于 `RAND_MAX` 通常较小，通过除法缩放得到的浮点数精度有限。

    ``` cpp
    float random_float = 
        	a + static_cast<float>(rand()) / RAND_MAX * (b - a);
    ```

  * 其次 `rand()` 的伪随机特性本身质量不高，经过缩放后浮点数的分布可能不均匀，无法保证生成结果的均匀性和随机性。

  * 浮点数的缩放和偏移操作会引入精度误差，使得生成的数在边界处分布不均匀。

#### 4.2 C++ 随机数

<FONT COLOR=BLUE>C++ 不应该使用 C 库函数 `rand`</FONT>，而应该使用 **`default_random_engine` 类**和恰当的**分布类对象**来生成随机数。

在 C++ 中，定义在头文件 `random` 中的随机数库通过一组**协作**的类和对象来解决 `rand` 的问题，这些类分为三个层次：

1. <font color=blue>随机数引擎（类）</font>：生成 **raw unsigned** 随机数序列
2. <font color=blue>随机数分布（类）</font>：“生成”（映射）指定类型的、在给定范围内的、服从特定概率分布的随机数
3. <font color=blue>随机数生成设备（随机数种子）</font>：调整引擎

**随机数引擎对象**和**随机数分布对象**的结合构成了 C++ 的**随机数发生器**。其中**随机数引擎对象**用于生成原始 `unsigned` 随机数序列，而**随机数分布对象**则将**随机数引擎**生成的原始 `unsigned` 随机数序列<font color=blue>映射</font>为我们要求的特定类型、特定范围和特定分布概率的随机数序列。从而解决了 C 库函数 `rand` 无法指定分布的限制。

值得注意的一点是，虽然从形式上来看，随机数生成引擎需要绑定一个随机数分布，但这并不意味着这个绑定是永久的，**对于一个随机数引擎，它可以按照需要切换绑定的随机数分布**，这一点从后面的代码中可以清晰的看到。

#### 4.3 随机数引擎

随机数引擎是**函数类**对象，它们定义了一个**调用运算符**，该运算符不接收参数并返回一个随机 `unsigned` 整数。我们可以通过调用一个随机数引擎对象生成“原始”随机数：

``` C++
default_random_engine e;
for(int i = 0; i < 10; i ++ )
    cout << e() << endl;
```

标准库定义了多个随机数引擎类，区别在于**性能**和**随机性质量**不同。

* 不同编译器会指定不同的默认随机数引擎 `default_random_engine`。

对于大多数场合，随机数引擎是**不能直接使用**的。

* 问题在于原始随机数的值范围通常与我们需要的不符，而正确转换随机数的范围是极其困难的。
* 这也是为什么我们称他为原始随机数（通过随机数引擎生成的随机数看起来和 `rand` 并没有什么大的差别）。

随机数引擎 **API**：

| API 格式                  | 参数说明                        | 返回值/效果                      | 使用场景                             | 注意事项                                                     |
| :------------------------ | :------------------------------ | :------------------------------- | :----------------------------------- | :----------------------------------------------------------- |
| **`Engine e;`**           | 无参数                          | 使用该引擎类型的默认种子构造引擎 | 需要快速初始化引擎，不关心种子值     | 不同编译器可能使用不同默认种子，会导致相同程序产生相同随机序列 |
| **`Engine e(s);`**        | `s`：整型种子值                 | 使用指定种子构造引擎             | 需要可重复的随机序列（如测试、仿真） | 相同种子会产生完全相同的随机序列                             |
| **`e.seed(s);`**          | `s`：整型种子值                 | 重置引擎状态，使用新种子         | 需要重用引擎但改变随机序列           | 会完全重置引擎状态，之前生成的序列无法继续                   |
| **`e.min();`**            | 无参数                          | 返回该引擎能生成的最小值         | 了解引擎输出范围                     | 对于标准引擎通常是 0 或该类型的最小值                        |
| **`e.max();`**            | 无参数                          | 返回该引擎能生成的最大值         | 了解引擎输出范围                     | 对于标准引擎通常是 2^w-1（w 是引擎位数）                     |
| **`Engine::result_type`** | 无参数                          | 引擎生成的整数类型别名           | 声明变量存储随机数时使用             | 通常是 `unsigned int` 或 `unsigned long long`                |
| **`e.discard(u);`**       | `u`：要跳过的步数（无符号整型） | 将引擎状态推进 u 步              | 需要跳过部分随机数序列               | 性能取决于引擎实现，对于大u值可能较慢                        |

#### 4.4 随机数分布

类似随机数引擎，随机数分布也是**函数对象类**。常见分布类型及特性表格：

| 分布类型       | 类名                             | 典型构造参数     | 生成值范围    | 有无状态 |
| :------------- | :------------------------------- | :--------------- | :------------ | :------- |
| 均匀分布(整数) | `std::uniform_int_distribution`  | `(a, b)`         | [a, b]        | 无状态   |
| 均匀分布(实数) | `std::uniform_real_distribution` | `(a, b)`         | [a, b)        | 无状态   |
| 正态分布       | `std::normal_distribution`       | `(均值, 标准差)` | (-∞, +∞)      | 有状态   |
| 泊松分布       | `std::poisson_distribution`      | `(均值)`         | [0, +∞)       | 有状态   |
| 伯努利分布     | `std::bernoulli_distribution`    | `(成功概率)`     | {false, true} | 无状态   |
| 离散分布       | `std::discrete_distribution`     | 权重列表         | 按权重分布    | 有状态   |

分布类型 API：

``` c++
Dist d;    // 默认构造随机数分布类对象，Dist为我们希望使用的随机数分布类
d(e);      // e是一个随机数引擎对象，返回一个符合分布的随机数
d.min();   // 返回该分布能生成的最小值和最大值
d.max();
d.reset(); // 重置分布类对象的内部状态，使得下一次生成的数序列可以从头开始，仿佛分布对象是新创建的一样。
```

* `reset()`:
  * 通常情况下，标准的分布对象在每次调用时都会按照设定好的分布规律生成数值，且**不依赖于前一次的生成结果**。因此，大部分分布对象的状态在生成数值时不会改变，所以对于这些分布，调用 `reset()` 并不会有明显的效果。
  * 不过，对于某些分布（例如 `std::poisson_distribution` 或 `std::geometric_distribution`），它们可能在生成数值时会**改变内部状态**，以提升效率或满足特定的生成要求。在这些情况下，`reset()` 可以确保分布对象的内部状态归零，以避免后续生成数值时出现偏差。

C++ 随机数生成优于 `rand` 的核心在于 C++ 提供了**分布类型**，通过分布类型，我们可以指定随机数的范围和类型（浮点数/大整数/小整数）等。

``` C++
default_random_engine e;
uniform_int_distribution<unsigned> d(0, 9);
for(int i = 0; i < 10; i ++ )
    cout << d(e) << endl;
```

* 注意，我们传递给分布对象的是**引擎本身**，即 `d(e)`。如果我们写成 `d(e())`，含义就变成将 `e` 的**下一个值**传递给 `d`，会导致编译错误。之所以要传递引擎而不是引擎生成的值是因为某些分布可能需要调用多次引擎才能生成一个值。

##### 1. 生成随机实数

程序常需要一个随机浮点数的源。特别是，程序经常需要 **0~1** 之间的随机数。在 C 语言中，我们通过 `rand() % RAND_MAX;` 来实现 **0~1** 随机浮点数的生成。但这种方式有一个显著的缺点，那就是通常整数的范围远小于浮点数的范围，这就会导致随机整数的精度通常低于随机浮点数，从而导致**一些浮点数永远不会生成**。

在 C++ 中，我们可以通过定义一个 `uniform_real_distribution` 类型的对象，并让标准库来处理从随机整数到随机浮点数的映射。

``` c++
default_random_engine e(time(nullptr));
uniform_real_distribution u;
for(int i = 0; i < 3; i ++ )
    cout << u(e) << endl;
```

##### 2. 随机数的类型

分布类型都是模板，具有单一的模板参数类型，表示分布生成的**随机数的类型**，对此有一个例外，我们将在下面介绍。除此之外，这些分布类型要么生成浮点数，要么生成整数类型。

* 实际上，只有一种情况可以作为例外：那就是 `bool` 值。``bool` 值的结果要么是 **0** 要么是 **1**，它不需要指定特殊的类型。

每个分布类型都有一个默认模板实参（指定随机数的默认类型）。

* 生成浮点数的分布默认模板实参类型是浮点数
* 生成整数的分布默认模板实参类型是整数。由于分布类型只有一个实参

因此当我们希望使用默认模板实参时，需要显示指定 `<>`：

``` c++
default_random_engine e(time(nullptr));
uniform_real_distribution<> u;
for(int i = 0; i < 3; i ++ )
    cout << u(e) << endl;
```

* 实际上不加 `<>` 也是以可以的。

##### 3. 正态分布

除了正确生成在指定范围内的数之外，C++ 新标准库的另一个优势是可以生成非均匀分布的随机数。事实上，新标准库定义了二十种多分布类型，可以说是相当多了。

作为例子，我们可以生成一个正态分布的值的序列，并画出值的分布。

* 由于生成正态（高斯）随机数的类 `normal_distribution` 生成浮点数，我们使用头文件 `cmath` 中的 `lround` 函数（四舍五入并返回 `long` 类型整数，普通的 `round` 返回 `double` 类型）将每个随机数舍入到下界。
* 例如以 **4** 为中心，**1.5** 为标准差的正太分布，根据 **68-95-99.7** 规则，我们希望 **99%** 的值都在 **[0-8]** 之间。

``` c++
int main()
{
    default_random_engine e(time(nullptr));
    normal_distribution<> u(4, 1.5);
    vector<unsigned> vals(9);
    for(int i = 0; i < 20000; i ++ ) {
        unsigned val = lround(u(e));
        if(val < vals.size()) ++ vals[val];
    }
    for(int i = 0; i < vals.size(); i ++ )
        cout << i << ": " << string(int(vals[i] * 1.0 / 100), '*') << endl;
    return 0;
}
```

输出：

``` c++
0: *
1: *******
2: **********************
3: *****************************************
4: ****************************************************
5: ******************************************
6: **********************
7: *******
8: *
```

##### 4. 伯努利分布

有一个分布不接受模板参数，即伯努利分布类 `bernoulli_distribution`，因为它是一个<font color=blue>非模板类</font>。

* 此分布总是返回一个**伯努利分布**（要么为 **0**，要么为 **1**）。
* 它返回 **1** 的概率是一个常数，此概率的默认值为 **0.5**。

我们可以修改这个默认值，使得返回 **1** 的概率为 **0.6**：

``` C++
int main()
{
    default_random_engine e(time(nullptr));
    bernoulli_distribution u(0.6); //返回1的概率为0.6
    int true_cnt = 0, false_cnt = 0;
    for(int i = 0; i < 10000; i ++ )
    {
        bool r = u(e);
        r ? ++ true_cnt : ++ false_cnt;
    }
    cout << true_cnt * 1.0 / 10000  << endl  // 0.6026
    cout << false_cnt * 1.0 / 10000 << endl; // 0.3974
    return 0;
}
```

#### 4.5 伪随机

随机数引擎本质上并不是“真正的随机数”生成器，而是一个**伪随机数生成器（PRNG，Pseudo-Random Number Generator）**。它根据某个算法和种子生成一系列具有特定模式的数值，这个序列看上去随机，但实际上是**可重复**和**可预测**的。

* 随机数引擎使用某种数学算法（如线性同余算法、梅森旋转算法）来生成数值序列。
* 只要算法和初始种子相同，该引擎每次生成的序列也是完全相同的。
* 这种确定性使得伪随机数序列在实际运行时可重复，而不是完全随机的。

由于随机数引擎的算法是有限的数学计算过程，生成的数列是**周期性**的，即经过一定次数后会重新循环生成相同的数列。例如，`std::mt19937` 引擎的周期非常长（约为 $2^{19937}−1 $），因此它在较短时间内表现为随机，但从理论上讲，仍然会回到序列的起点并重复。

随机数引擎实际上是“伪随机”，这意味着随机数有一个容易让人迷惑的特性，那就是即使生成的数看起来是随机的，但对于一个给定发生器，每次运行程序她都会返回**相同的数值序列**。

**序列不变这一特性在调式时非常有用**。但另一方面，使用随机数发生器的程序也必须考虑这一特性。例如下面的代码：

``` C++
vector<unsigned> bad_randVec()
{
    vector<unsigned> v;
    default_random_engine e;
    uniform_int_distribution u;
    for(int i = 0; i < 3; i ++ )	v.push_back(u(e));
    return v;
}
```

对于上面的代码，每次返回的 `vector` 都是一样的。编写此代码的正确方式是将引擎和关联的分布对象定义为 `static`：

``` c++
vector<unsigned> bad_randVec()
{
    vector<unsigned> v;
    static default_random_engine e;
    static uniform_int_distribution u;
    for(int i = 0; i < 3; i ++ )	v.push_back(u(e));
    return v;
}
```

这样就可以确保每次调用该函数之后，随机数引擎和分布的状态都会改变，并且还可以避免每次调用该函数都需要创建并销毁一次随机数引擎和随机数分布类对象。

#### 4.6 随机数发生器种子

随机数发生器会生成相同的随机数序列这一特性在调试时很有用。但是，一旦我们的程序调式完毕，我们通常希望每次运行程序都会生成不同的随机结果。

对于上一节讨论的使用 `static` 来修改随机数发生器状态的方法：

* 在一次程序执行过程中的不同调用中，每次调用的结果并不相同。
* 但两次程序执行过程中的对应每一次调用的结果仍然是相同的。

因此，我们还需要一种更好的方法来修改随机数发生器的状态。

我们可以通过提供一个**种子（seed）**来达成这一目的。

* 种子就是一个数值，引擎可以利用它从序列中一个新位置重新开始生成随机数。
* 我们可以在创建引擎时指定种子，也可以在创建后使用引擎的 `seed()` 成员提供种子。

如上所述，可以把种子理解为一个**起点**，它决定了生成器在伪随机数序列中指向的**位置**。<font color=blue>从数学角度来看，**种子**只是生成器算法的一个**初始输入**，它不会改变生成器的算法本身逻辑，因此算法产生的数值序列以及其周期性不会改变，但会使生成器从不同的起点开始生成数列。</font>

* 伪随机数生成器通过数学算法生成一系列看似随机的数值，而种子是输入给算法的第一个值，决定了生成序列的起始位置。
* 每个不同的种子值会生成一个**不同的伪随机数序列**（严格来说，生成的随机数序列是相同的，不过由于起始位置不同，再加上由于周期性，序列长度实际上是无穷的，所以说生成的是不同的随机数序列），种子相同的情况下，生成器将始终产生相同的随机数序列。这使得伪随机数生成器是确定性的，也就是说，它的行为是可以重复和预测的。
* 种子的选择会影响伪随机数生成器的表现，但不同生成器对种子敏感性不一样。对于高质量的生成器（如 `std::mt19937`），种子的分布性好，即不同种子可以很好地分散生成的序列，使得伪随机数更具随机性和均匀性。
* 简单的生成器可能对种子敏感，某些种子值可能导致生成器产生不均匀的数列，甚至是非常接近的序列。因此，选择合适的生成器和种子组合，对生成伪随机数的质量有直接影响。

由此可见，选择一个好的随机数种子是极其重要且极其苦难的。<font color=blue>最常用的方法是调用系统函数 `time`</font>，它返回从一个**Unix 纪元（`1970年1月1日 00:00:00 UTC`）**到当前时间经历了多少秒。函数 `time` 接受单个函数指针参数，它指向用于写入时间的数据结构。如果此指针为空，则函数简单的返回当前时间距离 **Unix 纪元**的时间：

``` c++
vector<unsigned> bad_randVec()
{
    vector<unsigned> v;
    default_random_engine e(time(nullptr));
    uniform_int_distribution u;
    for(int i = 0; i < 3; i ++ )	v.push_back(u(e));
    return v;
}
```

* 值得一提的是，`time` 是以秒为单位计时的，因此这种方法只适用于生成种子的间隔为秒级或更长的应用。
* 如果生成种子的间隔小于一秒，那么这里种子其实是相同的。

#### 4.7 `mt19937` 和 `random_device`

##### 1. `random_device`

`std::random_device` 是 C++11 标准库中提供的一个**随机数引擎**，用于生成**<font color=blue>硬件随机数</font>或伪随机数**。

* 注意 `random_device` 是一个**随机数引擎**，只不过它生成的随机数往往**与硬件有关**。
* `std::random_device` 通常是基于**硬件熵源**，提供比传统伪随机数生成器更不可预测的随机性。
* 因此它可以产生**高质量的种子数**，所以通常用于初始化其它伪随机数生成器，比如`std::mt19937`（梅森旋转算法）。

不过它是依赖于平台的：

* 在某些平台上（如 **Linux**），`std::random_device`  确实使用了硬件熵源。
* 而在另一些平台（如某些 **Windows** 实现）上，它可能会退化为伪随机数生成器。

这就是为什么说它用于生成硬件随机数**或伪随机数**。

##### 2. `mt19937`

`std::mt19937` 是 C++11 标准库中的一个**伪随机数引擎**，它基于**梅森旋转算法（Mersenne Twister）**。可以高效、快速且均匀的随机数生成器，广泛用于需要大量随机数的场景。

* 其中 `mt` 表示**“梅森旋转”（Mersenne Twister）**。
* `19937` 表示随机数的周期为 $2^{19937} - 1$。

`std::mt19937_64` 与 `std::mt19937` 类似，但使用 **64位** 的梅森旋转算法，适合需要生成更大范围随机数的应用。

* 和 `std::mt19937` 一样，它基于 **Mersenne Twister** 算法，但使用 **64** 位精度。

##### 3. 实例

``` C++
std::random_device rd;
std::mt19937 gen(rd()); // 使用rd产生的随机数作为种子初始化mt19937
std::uniform_int_distribution<uint32_t> u(0, 9); // [0.9]

int main()
{
    vector<size_t> cnt(10);
    for(int i = 0; i < 10000000; i ++ )	cnt[u(gen)] ++ ;
    for(int i = 0; i < 10; i ++ )	cout << i << ": " << cnt[i] * 1.0 / 10000000 << endl;
    return 0;
}
```

### 5. IO库再探

标准库的操纵符大都定义在头文件 `iomanip` 中。

#### 5.1 格式控制

在前面我们着重介绍了 **IO** 对象的条件状态，不过，除了<font color=blue>**条件状态**</font>外，每个 `iostream` 对象还维护一个<font color=blue>**格式状态**</font>来控制 **IO** 如何格式化的细节。包括：

* 整型数是几进制
* 浮点数的精度
* `bool` 值是否以字符串的形式输出
* 一个输出元素的宽度
* 十六进制是否打印 `0x`，`x` 是打印大写还是小写
* 控制对其的方式和用于补白的元素

标准库定义了一组**操纵符（manipulator）**来修改流的格式状态。

* <font color=blue>一个操纵符是一个**函数**或是一个**对象**，会影响流的状态，并能用作输入或输出运算符的**运算对象**。</font>
* 类似输入和输出运算符，操纵符也返回它所处理的流对象，因此我们可以在一条语句中组合操纵符和数据。
* 例如我们常用的操纵符 `endl`，我们将它写到输出流，就像是写对象的值一样。但 `endl` 不是一个普通值，而是一个**操作**：它输出一个换行符并刷新缓冲区。

笼统来说，操作符用于两大类输出控制：

1. 控制数值的输出形式
2. 控制补白的数量和位置

大多数改变格式状态的操纵符都是**成对（set/reset）**的：

* **set** 来将格式状态设置为一个新值。
* **reset** 用来将其复原，恢复为正常的默认格式。

由于操纵符本质上改变的是流的**格式状态**（在底层用一个数值表示），因此，当操纵符改变流的格式状态时，通常改变后的状态对所有后续 **IO** 都生效。因此，通常最好在不再需要特殊格式时尽快将流恢复到默认状态。

注意，**格式状态**的值是基于**流**而不是 **IO 语句**的。这意味着，你在第一条 **IO 语句**中修改了格式状态的值并且没有复原，在另外一条 **IO 语句**中仍然会生效。例如 `cin` 和 `cout` 语句，它们共享 `iostream` 流的**格式状态**。

##### 1. 控制布尔值的格式

> * boolalpha、noboolalpha

默认情况下，`bool` 打印 **0** 或 **1**。我们可以通过使用 `boolalpha` 操纵符将 **0** 和 **1** 替换为 **true** 和 **false**：

``` C++
{
    bool f1 = false, f2 = true;
    cout << boolalpha << f1 << ' ' << f2 << endl; // false true
}
bool f3 = true;
cout << f3 << endl;  // true
cout << noboolalpha; // reset
cout << f3 << endl;  // 1
```

##### 2. 指定整型值的进制

> * dec、oct、hex
> * showbase、nobaseshow
> * uppercase、nouppercase

`hex`，`oct`，`dec` 可以将进制分别改为十六进制、八进制、十进制。默认为十进制：

``` C++
cout << "default: "               << 20 << ' '  << 1024 << endl; // 20 1024
cout << "hex: "     << hex        << 20 << ' '  << 1024 << endl; // 14 400
cout << "octal: "   << oct        << 20 << ' '  << 1024 << endl; // 24 2000
cout << "decimal: " << dec        << 20 << ' '  << 1024 << endl; // 20 1024
```

但我们发现，这种形式的输出不太直观，例如十六进制，它不是 **0x14**，而是直接输出 **14**。我们可以使用 `showbase` 来显示的输出当前数字的进制（`0x` 表示十六进制，`0` 表示八进制，无前导字符串表示十进制）：

``` C++
cout << showbase;
cout << "default: "               << 20 << ' '  << 1024 << endl; // 20 1024
cout << "hex: "     << hex        << 20 << ' '  << 1024 << endl; // 0x14 0x400
cout << "octal: "   << oct        << 20 << ' '  << 1024 << endl; // 024 02000
cout << "decimal: " << dec        << 20 << ' '  << 1024 << endl; // 20 1024
cout << noshowbase;
```

不过我们可以发现，十六进制下 进制前缀 `0x` 的 `x` 和 数据部分的 `e` 都是以小写形式输出的。我们可以通过 `uppercase` 操纵符来以大写形式输出：

``` C++
cout << showbase << uppercase << hex << 14 << ' '  << 1024; // 0XE 0X400
cout << nouppercase << noshowbase << endl; 
// 注意如果我们后续希望改回 10 进制，需要 cout<<dec;
cout << 1024 << endl; // 400，依然按照 16 进制输出
```

##### 3. 控制浮点数格式

我们可以控制浮点数输出三种格式：

1. 以多高的精度（有效数字，包括整数部分和小数部分）打印浮点数
2. 大数值以十六进制、定点十进制还是科学计数法形式表示
3. 对于没有小数部分的浮点数是否打印小数点

###### 1）指定浮点数的打印精度

| **方法**            | **作用**                             | **返回值**       | **是否持久生效** | **推荐场景**             |
| :------------------ | :----------------------------------- | :--------------- | :--------------- | :----------------------- |
| `precision()`       | 获取当前精度                         | 当前精度值       | 无               | 需要查询当前精度时       |
| `precision(int)`    | 设置精度并返回旧精度                 | 旧精度值         | 是               | 需要临时修改并恢复精度时 |
| `setprecision(int)` | 通过操纵符设置精度（需 `<iomanip>`） | 无（返回操纵符） | 是               | 流式输出时（代码更简洁） |

<font color=blue> 默认情况下，精度会控制打印的数字的**总数（包括整数部分和小数部分）**。</font>

* 当打印时，浮点数按当前精度**四舍五入**。

  ``` cpp
  double d = 3.141592653589793;
  cout << d << endl;                    // 3.14159, GCC 编译器下默认精度为 6 
  cout << setprecision(2) << d << endl; // 3.1
  cout << setprecision(3) << d << endl; // 3.14
  cout << setprecision(4) << d << endl; // 3.142, 四舍五入
  cout << setprecision(5) << d << endl; // 3.1416
  cout << setprecision(6) << d << endl; // 3.14159
  ```

* 精度控制的是打印的数字的总数意味着，如果浮点数的整数部分就超过了精度大小，那么浮点数可能**舍入**小数部分（注意不是截断小数部分）。

  ``` cpp
  double d = 12.56789;
  cout << setprecision(2) << d << endl; // 13, 小数部分被舍入
  ```

除了控制精度外，我们还可以获取某个流的精度：

``` c++
double p = 3.1415926;
cout << "cur precision: " << cout.precision() << endl;  // cur precision: 6
cout << sqrt(2.0) << endl;                              // 1.41421
cout << "old precision: " << cout.precision(8) << endl; // old precision: 6
cout << "new precision: " << cout.precision() << endl;  // new precision: 8
cout << sqrt(2.0) << endl;                              // 1.4142136
```

注意，标准库似乎没有设置默认浮点数精度（复原）的操纵符，因此如果我们想要暂时修改浮点数精度，需要先将原来的精度保存下来，这也是 `precision(int)` 的目的。

###### 2）指定浮点数记数法

除非你需要控制浮点数的表示形式（如，按列打印（列对齐）数据或打印金额或百分比的数据），否则，**由标准库选择记数法**是最好的方式。

``` c++
cout << sqrt(2.0) << endl;           // 1.41421
cout << 1000 * sqrt(2.0) << endl;    // 1414.21
cout << 100000 * sqrt(2.0) << endl;  // 141421
cout << 1000000 * sqrt(2.0) << endl; // 1.41421e+06    
cout << float(123456789) << endl;    // 1.23457e+08, 舍入
```

可以发现，当舍入小数部分后浮点数的精度依然不足以表示浮点数，标准库会自动使用科学计数法表示浮点数，此时对于整数，也会发生舍入。

* 可以发现，对于科学计数法，所能表示的有效数字位数依然受到浮点数精度限制

| **操纵符**          | **输出格式**                  | **示例（输入 `123.456`）**                    | **适用场景**                         |
| :------------------ | :---------------------------- | :-------------------------------------------- | :----------------------------------- |
| `std::scientific`   | 科学计数法（`e±dd`）          | `1.23456e+02`                                 | 需要统一科学计数法格式（如科研数据） |
| `std::fixed`        | 定点小数（`ddd.ddd...`）      | `123.456000`                                  | 需要固定小数位数（如财务计算）       |
| `std::hexfloat`     | 十六进制科学计数法（`0xp±d`） | `0x1.edd2f1a9fbe77p+6`                        | 底层调试或浮点数二进制分析           |
| `std::defaultfloat` | 自动选择（默认）              | `123.456`（小数值）或 `1.23456e+08`（大数值） | 通用场景，保持代码简洁               |

* 这些操纵符也会**改变流的精度的默认含义**。<font color=blue>在执行 `scientific`、`fixed` 或 `hexfloat` 后，浮点数精度控制的是小数部分，而默认情况下是指整数部分和小数部分</font>。
* 默认情况下，十六进制数字和科学记数法中的 `e` 都打印成小写形式。我们可以通过 `uppercase` 操纵符打印这些字母的大写形式。

``` cpp
cout << fixed << setprecision(7) << sqrt(2) << endl; // 1.4142136
cout << scientific << 10000 * sqrt(2) << endl;       // 1.4142136e+04
cout << hexfloat << sqrt(2) << endl;                 // 0x1.6a09e667f3bcdp+0
```

----

使用 `fixed` 和 `scientific` 时结合 `set(w)` 可以**按列打印**数值，因为小数点距小数部分的距离是固定的。

``` cpp
int main() 
{
    std::vector<std::string> headers = {"Item", "Price"};
    std::vector<std::string> items = {"Apple", "Banana", "Cherry"};
    std::vector<double> prices = {2.939, 1.520, 0.375};

    // 打印表头
    std::cout << std::left << std::setw(10) << headers[0] 
              << std::right << std::setw(10) << headers[1] << "\n"
              << std::string(20, '-') << std::endl;

    // 打印数据（固定2位小数）
    std::cout << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < items.size(); ++ i) {
        std::cout << std::left << std::setw(10) << items[i]
                  << std::right << std::setw(10) << prices[i] << std::endl;
    }
    return 0;
}
```

输出为：

``` cpp
Item           Price
--------------------
Apple           2.94
Banana          1.52
Cherry          0.38
```

###### 3）打印小数点

| **操纵符**         | 行为                             | 示例（输入 `123.0`） | 示例（输入 `123`） |
| :----------------- | :------------------------------- | :------------------- | :----------------- |
| `std::showpoint`   | 强制显示小数点及**无效零**       | `123.000`            | `123.456`          |
| `std::noshowpoint` | 仅当小数非零时显示小数点（默认） | `123`                | `123`              |

``` c++
double d = 1234;
cout << d << endl;                 // 1234
cout << showpoint << d << endl;    // 1234.00
double d2 = 123456;
cout << noshowpoint << d2 << endl; // 123456
cout << showpoint << d2 << endl;   // 123456.00
```

##### 4. 输出补白

当按列打印数据时，我们常常需要非常精确的**数据格式控制**。标准库提供了一些操纵符帮助我们完成所需的控制：

| **操纵符**    | 作用                                                         | 常用场景               |
| :------------ | :----------------------------------------------------------- | :--------------------- |
| `setw(w)`     | 设置字段宽度（仅对下一个输出生效）                           | 表格对齐、固定列宽     |
| `left`        | 左对齐内容，右侧填充                                         | 文本对齐               |
| `right`       | 右对齐内容**（默认）**，左侧填充                             | 数字对齐               |
| `internal`    | 两边对齐：符号左对齐，数值右对齐**（用于负数）**，对正数无影响 | 财务数据               |
| `setfill(ch)` | 自定义填充字符**（默认空格）**                               | 美化输出（如 `***42`） |

``` c++
int main()
{
    int    i = -16;
    double d = 3.14159;
    // 默认对齐方式
    cout << "i: " << setw(12) << i << "next col" << "\n"
         << "d: " << setw(12) << d << "next col" << "\n";
    // 左对齐
    cout << left 
         << "i: " << setw(12) << i << "next col" << "\n"
         << "d: " << setw(12) << d << "next col" << "\n";
    // 右对齐
    cout << right
         << "i: " << setw(12) << i << "next col" << "\n"
         << "d: " << setw(12) << d << "next col" << "\n";
    // 符号和值分离对齐
    cout << internal
         << "i: " << setw(12) << i << "next col" << "\n"
         << "d: " << setw(12) << d << "next col" << "\n";
    // 以#取代空格填充空白
    cout << setfill('#')
         << "i: " << setw(12) << i << "next col" << "\n"
         << "d: " << setw(12) << d << "next col" << "\n"
         << setfill(' '); // 恢复正常的补白字符    
    return 0;
}
```

``` C++
i:          -16next col
d:      3.14159next col
i: -16         next col
d: 3.14159     next col
i:          -16next col
d:      3.14159next col
i: -         16next col
d:      3.14159next col
i: -#########16next col
d: #####3.14159next col
```

##### 5. 控制输入格式

默认情况下，输入运算符会忽略空白符（空格，制表符，换行符、回车符和换行符）。操纵符 `noskipws` 会令运算符读取空白符，而不是跳过他们。

| **操纵符**      | 行为                         | 示例输入 `" A"` 的输出 |
| :-------------- | :--------------------------- | :--------------------- |
| `std::skipws`   | 跳过前导空白字符**（默认）** | 读取 `A`               |
| `std::noskipws` | 不跳过空白字符，逐字符读取   | 读取第一个空格（` `）  |

* **仅影响输入流**：
  `skipws`/`noskipws` 仅对 `istream`（如 `cin`）有效，不影响输出流（如 `cout`）。
* **与 `getline` 的差异**：
  `std::getline` 默认读取包括空格在内的整行内容（不受 `skipws` 影响），但会丢弃换行符。
* **持久性**：
  设置 `noskipws` 后，会一直生效，直到显式恢复 `skipws`。

``` cpp
#include <iostream>
#include <iomanip>

int main() {
    char c;
    std::cout << "输入字符串（含空格）: ";
    while (std::cin >> std::noskipws >> c) { // 逐个读取字符（包括空格）
        std::cout << "[" << c << "]";
    }
    return 0;
}
```

输入：

``` cpp
 hello world !
```

输出：

``` cpp
[ ][h][e][l][l][o][ ][w][o][r][l][d][ ][!]
```

#### 5.2 未初始化IO

除了通过操纵符实现输入和输出的**格式化** **IO** 之外，标准库还提供了一组底层操作，支持**未格式化** **IO**。这些操作允许我们将一个流当作一个<font color=blue>**无解释的字节序列**</font>来处理。这些操作不涉及数据解析或转换，适合处理二进制数据、原始字节流或自定义协议。

**1. 未格式化 I/O 是什么？**

- **定义**：
  直接以**原始字节**形式读写数据，不进行任何数据解析、转换或格式处理（如不自动跳过空格、不转换数字或字符串）。

- **特点**：

  - **二进制安全**：保留所有字节（包括空格、换行符、NULL 字符等），适合处理非文本数据（如图片、音频、加密数据）。
  - **无缓冲倾向**：通常绕过流的缓冲区，直接操作设备或文件（性能更高，但需手动优化）。
  - **底层控制**：程序员需自行处理字节序、对齐、错误检测等细节。

- **对比格式化 I/O：**

  | **特性**     | 格式化 I/O（如 `cin >> x`）    | 未格式化 I/O（如 `read()`）    |
  | :----------- | :----------------------------- | :----------------------------- |
  | **数据解释** | 自动转换类型（如字符串转数字） | 直接读写原始字节               |
  | **空白处理** | 默认跳过空格（`skipws`）       | 保留所有字符（包括空格）       |
  | **适用场景** | 文本输入/输出（人类可读）      | 二进制数据、协议解析、底层操作 |

**2. 无解释的字节序列 是什么？**

- **定义**：
  数据在传输或存储时被视为**纯粹的字节流**，程序不假定其内容结构（如不区分整数、浮点数、字符串等），需开发者自行解释。
- **关键点**：
  - **无类型信息**：字节流不包含数据类型元数据，读取时需明确知道原始格式。
  - **按需解析**：开发者需根据协议或文件格式，手动解析字节的含义（如读取 4 字节视为 `int`）。
  - **跨平台风险**：字节序（大端/小端）、对齐方式可能影响解析结果。

**3. 示例：二进制文件中的整数**

假设文件 `data.bin` 存储了一个 4 字节的 `int` 值 `0x12345678`（十六进制）：

- **未格式化读取**：

  ```cpp
  std::ifstream file("data.bin", std::ios::binary);
  int value;
  file.read(reinterpret_cast<char*>(&value), sizeof(int));
  ```

  - 直接读取 4 字节到 `value` 的内存中，不关心其内容是否为合法整数。

- **需注意**：

  - 字节序：若文件与程序运行的平台字节序不同，需手动转换。
  - 对齐：某些架构要求数据按特定对齐方式访问。

- **二进制 vs 文本模式**：

  - 在 Windows 中，文本模式（默认）会将 `\n` 转换为 `\r\n`，二进制模式（`ios::binary`）则禁止转换。

**4. 未格式化** **IO** 提供了对流的**底层字节级控制**，适用于：

- 二进制文件读写（如图像、压缩文件）。
- 网络协议或自定义数据格式的解析。
- 需要高性能、避免数据转换的场景。

##### 1. 底层字符级 I/O 操作（单字节操作）

 单字节操作每次一个字节地处理流。他们会**读取而不是忽略空白符**。

| **函数**         | 操作流类型 | 作用                    | 返回值     | 典型用途                     |
| :--------------- | :--------- | :---------------------- | :--------- | :--------------------------- |
| `is.get(ch)`     | `istream`  | 读取下一个字节到 `ch`   | `istream&` | 逐字符读取**（包括空白符）** |
| `is.get()`       | `istream`  | 返回下一个字节（`int`） | `int`      | 检查 `EOF` 或直接处理字节    |
| `is.putback(ch)` | `istream`  | 将 `ch` 放回流中        | `istream&` | 回溯或重新解析               |
| `is.unget()`     | `istream`  | 回退一个字节            | `istream&` | 撤销最近一次读取             |
| `is.peek()`      | `istream`  | 查看但不移除下一个字节  | `int`      | 预判下一个字符               |
| `os.put(ch)`     | `ostream`  | 写入字符 `ch`           | `ostream&` | 低开销字符输出               |

有时我们需要读取一个字符才能知道还未准备好处理它。在这种情况下，我们希望**将字符放回流中**。标准库提供了三种方法**退回字符**，他们有着细微的区别：

* `peek` 返回输入流中下一个字符的**副本**，但不会从流中删除它。
* `unget` 使得输入流向后移动，并且将最近读取的字符放回流的顶部，因此最后读取的值又回到流中。即使我们不知道最后从流中读取什么值，仍然可以调用 `unget`。
* `putback` 是更特殊版本的 `unget`：它退回从流中读取的最后一个值，但它接受一个参数，**此参数必须和最后读取的值相同。**

一般情况下，在读取下一个值之前，标准库保证我们可以**退回最多一个值**。即，标准库不保证在中间不进行读取操作的情况下能连续调用 `putback` 或 `unget`。

**注意事项：**

- **错误处理**：
  - 检查流状态（如 `is.fail()`）以处理读取失败（如到达文件尾）。
  - `putback` 和 `unget` 不一定在所有情况下都可用（如流开头）。
- **`EOF` 处理**：
  - `get()` 返回 `int` 而非 `char`，以便区分有效字符（0~255）和 `EOF`（通常为 `-1`）。
- **性能**：
  - 这些函数通常**无缓冲**，频繁调用可能影响性能（需权衡灵活性）。
- **二进制 vs 文本模式**：
  - 在 Windows 中，文本模式（默认）会将 `\n` 转换为 `\r\n`，二进制模式（`ios::binary`）则禁止转换。

``` cpp
// 1. 输入流操作
	// (1) is.get(ch)
char ch;
while (cin.get(ch)) {  // 读取所有字符（包括空白符）
    cout << ch;
}

	// (2) is.get()
int byte;
while ((byte = cin.get()) != EOF) {
    cout << static_cast<char>(byte);
}

	// (3) is.putback(ch)
char ch;
cin.get(ch);       // 读取一个字符
cin.putback(ch);   // 放回流中

	// (4) is.unget()
char ch;
cin.get(ch);      // 读取 'A'
cin.unget();      // 回退，下次读取还是 'A'

	// (5) is.peek()
int next = cin.peek();
if (next == '#') {
    cout << "Found a comment!";
}

// 2. 输出流操作
	// (1) os.put(ch)
cout.put('H').put('i');  // 输出 "Hi"
```

##### 2. 底层字节流 I/O 操作（多字节操作）

一些**未格式化 IO** 操作一次处理**一块数据**。

* 如果速度是要考虑的重点的话，这些操作是很重要的，但类似其他底层操作，这些操作也容易出错（在操作底层时，人总是会出错）。
* 特别是，这些操作要求我们**自己分配并管理用来保存和提取数据的字符数组**。

| **函数签名**                                | **关键行为**                | **典型用途**   |
| :------------------------------------------ | :-------------------------- | :------------- |
| `istream& read(char*, streamsize)`          | 读取原始字节（不处理 `\0`） | 二进制文件读取 |
| `ostream& write(const char*, streamsize)`   | 写入原始字节                | 二进制文件写入 |
| `istream& get(char*, streamsize, char)`     | 读取到分隔符（保留分隔符）  | 结构化字段解析 |
| `istream& getline(char*, streamsize, char)` | 读取到分隔符（丢弃分隔符）  | 逐行读取文本   |
| `streamsize gcount()`                       | 返回实际读取字节数          | 检查读取结果   |
| `istream& ignore(streamsize, int)`          | 跳过指定字符                | 清理输入缓冲区 |

``` c++
os.write(source, size);         // 将字符数组source的size个字节写入os
is.get(sink, size, delim);      // 读取最多size个字节或遇到字符delim或EOF时停止。
						        // 遇到delim时，将其留在输入流中，不读取出来存入sink
is.getline(sink, size, delim)； // 和get的区别在于，遇到delim时会读取出来并丢弃掉
is.ignore(size=1, delim=EOF);	// 读取并忽略最多size个字符，包括delim。
is.gcount(); // 返回上一个未格式化读取操作从is中读取的字节数
	         // 将字符退回流的单字符操作也属于未格式化读取（输入）操作
			 // 因此对于peek、unget、putback会返回0
```

###### Ⅰ. 不带分隔符的读取

**(1) `istream& read(char* sink, streamsize size)`**

- **参数**：

  - `sink`：目标字符数组（需预先分配足够内存）。
  - `size`：要读取的最大字节数。

- **返回值**：`istream&`（支持链式调用），可通过 `gcount()` 获取实际读取字节数。

- **行为**：

  - 从流中读取最多 `size` 个字节到 `sink`。
  - 不添加终止符 `\0`，可能读取到任意字节（包括 `\0`）。

- **示例**：

  ```cpp
  char buf[1024];
  ifstream file("data.bin", ios::binary);
  file.read(buf, sizeof(buf));
  ```

**(2) `ostream& write(const char* source, streamsize size)`**

- **参数**：

  - `source`：源字符数组（包含待写入数据）。
  - `size`：要写入的字节数。

- **返回值**：`ostream&`（支持链式调用）。

- **行为**：

  - 将 `source` 的 `size` 个字节写入流，不检查 `\0`。

- **示例**：

  ```cpp
  const char data[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
  ofstream file("out.bin", ios::binary);
  file.write(data, sizeof(data));
  ```

###### **Ⅱ. 带分隔符的读取**

**(1) `istream& get(char* sink, streamsize size, char delim = '\n')`**

- **参数**：

  - `sink`：目标字符数组。
  - `size`：缓冲区大小（最多读取 `size-1` 字节）。
  - `delim`：分隔符**（默认 `\n`）**，遇到时停止读取。

- **返回值**：`istream&`。

- **行为**：

  - 读取到 `delim` 前停止，**保留 `delim` 在流中**。
  - 自动在 `sink` 末尾添加 `\0`（最多写入 `size-1` 字节）。

- **示例**：

  ```cpp
  char line[256];
  cin.get(line, sizeof(line), ';'); // 读取到分号（不包含分号）
  ```

**(2) `istream& getline(char* sink, streamsize size, char delim = '\n')`**

- **参数**：同 `get()`。

- **返回值**：`istream&`。

- **行为**：

  - 类似 `get()`，但**提取并丢弃 `delim`**。

- **示例**：

  ```cpp
  cin.getline(line, sizeof(line), '#'); // 读取并丢弃 #
  ```

###### **Ⅲ. 辅助操作**

**(1) `streamsize gcount() const`**

- **参数**：无。

- **返回值**：上一次未格式化读取（`read()`/`get()`/`getline()`）的实际字节数。

  - 对 `peek()`、`putback()`、`unget()` 返回 `0`。

- **示例**：

  ```cpp
  char buf[100];
  cin.read(buf, 50);
  cout << "Read " << cin.gcount() << " bytes";
  ```

**(2) `istream& ignore(streamsize size = 1, int delim = EOF)`**

- **参数**：

  - `size`：最大忽略字符数。
  - `delim`：停止忽略的字符（默认忽略 `size` 个字符）。

- **返回值**：`istream&`。

- **行为**：

  - 跳过并丢弃字符，直到忽略 `size` 个字符或遇到 `delim`。

- **示例**：

  ```cpp
  cin.ignore(1000, '\n'); // 跳过当前行剩余内容
  ```

##### 3. 操纵底层函数容易出错

一般情况下，我们主张使用标准库提供的高层抽象。

返回 `int` 的 **IO** 操作很好的解释了原因：

* 一个常见的编程错误就是将 `get` 或 `peek` 的返回值赋予一个 `char` 而不是一个 `int`。这是错误的，但编译器不能发现这个错误。最终会发生什么依赖于程序运行所在的机器以及输入数据是什么。例如，在一台 `char` 被实现为 `unsigned char` 的机器上，下面的循环永远不会停止：

``` C++
char ch;
while((ch = cin.get()) != EOF)
    cout.put(ch);
```

* 如果 `EOF` 对应的值为 `-1`，那么循环将永远为 `true`。除此之外，如果 `cin.get()` 的值超出了`unsigned  char` 所能表示的范围，程序的行为也是未定义的。

#### 5.3 流随机访问

各种流通常都支持对流中数据的随机访问。我们可以重定位流，使之跳过一些数据，首先读取最后一行，然后最后第一行，以此类推。标准库提供了一堆函数，来定位（`seek`）到流中给定的位置，以及告诉（`tell`）我们当前位置。

* 随机 **IO** 本质上是**依赖于系统的**。因此如何使用这些特性需要查询系统文档。

虽然标准库为所有流类型都定义了 `seek` 和 `tell` 函数，但它们是否做有意义的事情依赖于流绑定到哪个设备。在大多数系统中，绑定到 `cin`、`cout`、`cerr` 和 `clog` 的流不支持随机访问。

* 毕竟，当我们向 `cout` 直接输出数据时，类似向回跳十个位置这种操作是没有意义的。
* 对这些流我们可以调用 `seek` 和 `tell` 函数，但在运行时会出错，将流置于一个无效状态。

由于 `istream` 和 `ostream` 类型通常不支持随机访问，所以这里主要讨论的是 `fstream` 和 `sstream` 类型。

##### 1. seek 和 tell

为了支持随机访问，**IO** 类型维护一个标记来确定下一个读写操作要在哪里进行。标准库提供了两个函数 `seek` 和 `tell` 用来重定位标记的位置以及返回当前标记的位置。事实上标准库定义了两对 `seek` 和 `tell`，分别用于输入流和输出流。其中后缀是 **g(get)** 的表示输入流；后缀是 **p(put)** 的表示输出流版本。

``` c++
tellg();
tellp();

seekg(pos);      // 绝对地址
seekp(pos);

seekg(off,from); // 相对地址，将标记定位到from之前或之后off个字符
seekp(off,from); // from：
					 // beg：偏移量相对于流开始的位置
					 // cur：偏移量相对于流当前位置
					 // end：偏移量相对于流结束位置
```

##### 2. 只有一个流位置标记

在 `seek` 和 `tell` 函数中区分了输入流（**get**）和输出流（**put**）的不同版本可能导致一个误解：认为输入流和输出流分别有一个独立的标记。事实上，**单个流内部只存在唯一的标记，并不存在独立的读标记和写标记。**

* **单一标记原则**：
  每个流（`iostream`，`fstream`、`stringstream`）内部**只有一个位置标记**，用于指示当前读写位置。
  - **不存在独立的读/写标记**，输入（`get`）和输出（`put`）操作共享同一个标记。
  - 无论是 `istream` 的 `seekg` 还是 `ostream` 的 `seekp`，最终操作的均是同一个标记。
* **标记属于数据源/目标**：
  位置标记是流关联的**底层序列**（如文件、内存缓冲区）的属性，而非流对象本身的属性。

由于只有唯一的标记，因此只要我们在读写操作间切换，就需要进行 `seek` 操作来重定位标记。

##### 3. 重定位位置标记

`seek` 函数有两个版本：

* 一个移动到文件中的**绝对位置**。
* 另一个移动到文件中的**相对位置**。

``` cpp
seekg(pos_type new_position); // 输入流版本
seekp(pos_type new_position); // 输出流版本

seekg(off_type offset, ios_base::seekdir from); // 输入流版本
seekp(off_type offset, ios_base::seekdir from); // 输出流版本
```

其中，类型 `pos_type` 和 `off_type` 是机器相关的，`off_type` 的值可正可负，他们定义在头文件 `istream` 和 `ostream` 中。

##### 4. 访问标记

tellg 和 tellp 返回一个 pos_type 值，它通常用来记住一个位置，以便稍后再定位回来：

``` c++
ostringstream writeStr;
ostringstream::pos_type mark = writeStr.tellp();
// ...
if(cancelEntry) {
    writeStr.seekp(mark);
}
```

##### 5. 读写同一个文件

我们给出一个编程实例，来体会一下 seek 和 tell 的应用。在这里编程实例当中，假定已经给定了一个要读取的文件，我们要在此文件的末尾写入新的一行，这一行包含文件中每行的相对起始位置。第一行的起始位置固定为 0，因此不必写入。例如，给定下面文件：

``` c++
abcd
efg
hi
j
```

则程序应该生成如下修改过的文件：

``` c++
abcd
efg
hi
j
5 9 12 14
```

注意别把每行最后的换行符忽略了。

``` c++
fstream file("./data/io.txt", fstream::ate | fstream::in | fstream::out);
if(!file) //好的编程习惯
{
    cout << "[error]open file failed" << endl;
    return EXIT_FAILURE;
}
auto end_mark = file.tellg();
file.seekg(0, fstream::beg);
size_t cnt = 0;
string line;
while(file && file.tellg() != end_mark && getline(file, line)) 
{
    cnt += line.size() + 1; //换行符
    auto cur_mark = file.tellg();
    file.seekp(0, fstream::end);
    file << cnt;
    if(cur_mark != end_mark)    file << " ";
    file.seekg(cur_mark);
}
file.seekp(0, fstream::end);
file << "\n";
```

#### 5.4 附录: 定义在 iostream 中的操纵符

| 操纵符          | 含义（*表示默认流格式状态）                 |
| --------------- | ------------------------------------------- |
| boolalpha       | 将 bool 值以 true/false 的形式输出          |
| \*noboolalpha   |                                             |
| showbase        | 八进制加前导 0，十六进制加前导 0x           |
| \*noshowbase    |                                             |
| showpos         | pos:positive，对非负数显示+，               |
| \*noshowpos     |                                             |
| showpoint       | 对浮点数总是显示小数点                      |
| \*noshowpoint   | 只有当浮点数包含小数部分时才显示小数点      |
| uppercase       | 数字中的字母显示为大写，例如E/e，0X/0x，F/f |
| *nouppercase    |                                             |
| *dec            |                                             |
| oct             |                                             |
| dec             |                                             |
| left            | 在值的左侧添加填充字符                      |
| right           | 在值的右侧添加填充字符                      |
| internal        | 在负号和值之间甜茶填充字符                  |
| fixed           | 浮点数显示为定点十进制                      |
| scientific      | 浮点数显示为科学计数法                      |
| unitbuf         | 每次输出操作后都刷新缓冲区                  |
| \*nounitbuf     |                                             |
| \*skipws        | 输入运算符提奥果空白符                      |
| noskipws        |                                             |
| flush           | 刷新ostream缓冲区                           |
| ends            | 插入空字符然后刷新ostream缓冲区             |
| endl            | 插入换行然后刷新ostream缓冲区               |
| setfill(ch)     | 用ch填充空白                                |
| setprecision(n) | 将浮点精度设置为n                           |
| setw(w)         | 下一个读或写值的宽度为w个字符               |
| setbase(b)      | 将整数输出为b进制                           |

## 十二、适用于大型程序的工具

与仅需几个程序员就能开发完成的系统相比，大规模编程对程序设计语言的要求更高。大规模应用程序的特殊要求包括：

* 在独立开发的子系统之间协同处理错误的能力
* 使用各种库（可能包含独立开发的库）进行协同开发的能力
* 对比较复杂的应用概念建模的能力

本章介绍的三种 C++ 语言特性正好能满足上述要求，它们是：异常处理、命名空间和多重继承。

### 1. 异常处理

异常处理机制允许程序中独立开发的部分能够在**运行时**就出现的问题进行通信并做出相应的处理。异常使得我们能够**将问题的检测与解决过程分离开来**。程序的一部分负责检测问题的出现，然后解决该问题的任务传递给程序的另一部分。检测环节无需知道问题处理模块的所有细节，反之亦然。

#### 1.1 抛出异常

在 C++ 语言中，我们通过抛出（throwing）一条表达式来引发（raise）一个异常。被抛出的表达式的类型以及当前的调用链共同决定了哪段处理代码（handler）将被用来处理该异常。被选中的处理代码是在调用链中与抛出对象类型匹配的最近的处理代码。程序的异常抛出部分会告知异常处理部分到底发生了什么错误。

当执行一个 throw 时，跟在 throw 后面的语句将不再被执行。相反，程序的控制权从 throw 转移到与之匹配的 catch 模块。catch 可能是同一个函数中的局部 catch，也可能位于直接或间接调用了发生异常的函数的另一个函数中。控制权从一处转移到另一处，这有两个重要的意义：

* 沿着调用链的函数可能会提早推出
* 一旦程序开始执行异常处理代码，则沿着调用链创建的对象将被销毁

#### 1.2 栈展开

在抛出一个异常后，查找对应 catch 的过程是一个栈展开（stack unwinding）过程。栈展开过程沿着嵌套函数的调用链不断回退查找，直到找到了与异常匹配的 catch 子句为止；或者也可能一直没找到匹配的 catch，则退出主函数后查找过程终止。

假如找到了一个与之匹配的 catch 子句，则程序进入该子句并执行其中的代码。当执行完这个 catch 子句之后，找到与 try 块关联的最后一个 catch 子句之后的点，并从这里继续执行。

如果没找到匹配的 catch 子句，程序将终止。因为异常通常被认为是妨碍程序正常执行的事件，所以一旦引发了某个异常，就不能对它置之不理。因此当找不到匹配的 catch 子句时，程序将调用标准库函数 **terminate**，顾名思义，**terminate** 负责终止程序的执行过程。

#### 1.3 栈展开过程中对象自动销毁

如果在栈展开过程中退出了某个块，编译器将负责确保这个块中创建的对象能被正确的销毁。如果某个局部对象的类型是类类型，则该对象的析构函数将被自动调用。与往常一样，编译器在销毁内置类型的对象时不需要做任何事。

注意对于指针来说，我们只会销毁指针本身，而不会销毁指针所指向的对象。而异常处理代码所做的一部分就是负责释放这些动态分配的内存。

如果异常发生在构造函数中，则当前的对象可能只构造了一部分，有的成员已经初始化了，而另外一些成员在异常发生前也许还没有初始化。此时，C++ 保证：

- 构造函数抛出异常时，对象的析构函数**不会被调用**（因为对象构造未完成）。
- 但已构造的成员变量（如类类型成员、智能指针等）会按**初始化顺序的逆序自动销毁**。

我们可以测试一下：

``` cpp
struct C1 {
    C1() { cout << "C1" << endl; }
    ~C1() { cout << "~C1" << endl; }
};

struct C2 {
    C2() { cout << "C2" << endl; }
    ~C2() { cout << "~C2" << endl; }
};

struct Foo {
    Foo(C1 _c1 = C1(), C2 *_c2 = new C2()) : c1(_c1), c2(_c2) { throw bad_alloc(); }
    ~Foo() { 
        delete c2; 
        cout << "~Foo" << endl; 
    }

    C1 c1;
    C2 *c2;
};

int main()
{
    try {
        Foo f;
    }
    catch(const bad_alloc &e) {
        cout << "catch bad_alloc" << endl;
    }

    return 0;
}
```

输出结果为：

```cpp
C2
C1
~C1
~C1
catch bad_alloc
```

我们在 `Foo` 构造函数体中抛出了异常，此时已经初始化的成员变量 `c1` 和 `c2*` 的析构函数都被正常调用，但是 `Foo` 的析构函数并没有调用，因为从此时 `Foo` 的初始化工作并未完成。这就会导致对于 `c2*` 的析构工作没有完全完成，栈展开的过程只是自动销毁了 `c2*` 本身，但是并没有销毁 `c2*` 所指向的对象，从而产生内存泄漏。

因此，为确保已初始化的成员能被正确销毁，有以下建议：

1. 避免在构造函数中直接管理裸资源。
2. 确保所有成员变量自身具备RAII（Resource Acquisition Is Initialization）特性（如`std::vector`、`std::unique_ptr`等）。当异常发生时，已构造的成员会自动调用其析构函数。

类似的，异常也可能发生在数组或标准库容器的元素初始化过程中。与之前类似，如果在异常发生前已经构造了一部分对象，则我们应该确保这部分元素被正确的销毁：

* 释放已经分配的资源
* 将变量恢复到合法的状态

#### 1.4 析构函数与异常

从语法上来说，析构函数可以抛出异常。但从逻辑上和风险控制上，析构函数中不要抛出异常，因为栈展开容器导致资源泄露和程序崩溃，所以**别让异常逃离析构函数**。具体原因在《More Effective C++》中提到两个： 

1. 如果析构函数抛出异常，则异常点之后的程序不会执行，如果析构函数在异常点之后执行了某些必要的动作比如释放某些资源，则这些动作不会执行，会造成诸如资源泄漏的问题。

2. 通常异常发生时，C++ 的异常处理机制在异常的传播过程中会进行栈展开（stack-unwinding），因发生异常而逐步退出复合语句和函数定义的过程，被称为栈展开。在栈展开的过程中就会调用已经在栈构造好的对象的析构函数来释放资源，此时若其他析构函数本身也抛出异常，则前一个异常尚未处理，又有新的异常，会造成程序崩溃。（即所谓的双重异常）

   > 在 **C++**中，**双重异常**（*Double Exception*）指的是在异常处理过程中（如栈展开时）又抛出了另一个异常。这种情况会导致程序直接调用 `std::terminate()`，强制终止程序。
   >
   > 这是基于 **C++** 异常处理机制的核心原则：
   >
   > - **同一时间只能有一个异常在传播**。
   > - 如果析构函数在栈展开期间抛出异常，**C++** 无法同时处理两个异常，因此直接终止程序。

在实际的编程过程中，因为析构函数仅仅是释放资源，所以他不太可能抛出异常。所有标准库类型都能确保它们的析构函数不会引发异常。因此，为了确保析构函数不会抛出异常，尽量不要在析构函数中执行可能抛出异常的操作，例如申请资源等。如果析构函数的某个操作真的可能抛出异常，则该操作应该被放置在一个 try 语句块当中，并且在析构函数内部得到处理。

#### 1.5 异常对象

异常对象是一种特殊的对象，编译器使用异常抛出表达式来对异常对象进行 **Copy Construction**。因此：

1. throw 语句中的表达式必须拥有**完全类型（Defination）**。**不完全类型（Forward Declaration）** 只有声明，没有定义，编译器不知道它的：

   - 大小（无法分配内存）
   - 构造函数/析构函数（无法正确构造或销毁）
   - 成员和方法（无法进行拷贝或移动）

2. 而且如果该表达式是类类型的话，则相应的类必须含有一个可访问的析构函数和一个可访问的拷贝或移动构造函数。

   * 异常对象在 `catch` 块结束后需要被销毁

   * 创建和抛出异常对象都需要用到拷贝/移动构造函数

3. 如果该表达式是数组类型或函数类型，则表达式将被转换成与之对应的指针类型

   * **数组和函数不能直接拷贝**，但它们的指针可以

   * 退化成指针后，异常处理机制可以统一处理。

4. 如果是临时对象，它的生命周期会延长到捕获它的那个 catch 子句。

   ```cpp
   try {
       throw std::string("temp"); // 临时 string 不会立即销毁
   } 
   catch (const std::string& s) {
       // s 仍然有效
   } // 在这里被捕获之后才销毁
   ```

   **如果不延长生命周期**：

   - 如果异常对象在 `throw` 后立即销毁，`catch` 块会引用无效内存，导致未定义行为（UB）。

如我们所知，当一个异常被抛出时，沿着调用链的块将依次退出直至找到异常匹配的处理代码。如果退出了某个块，则同时释放块中局部对象使用的内存。因此，抛出一个**指向局部对象的指针**几乎肯定是一种错误的行为。如果指针所指向的对象位于某个块中，而该块在 catch 语句之前就已经退出了，则意味着在执行 catch 语句之前局部对象就已经被销毁了。

<font color=blue>当我们抛出一条表达式时，该表达式的**静态编译时类型**决定了异常对象的类型。</font>例如，一条 throw 表达式解引用一个基类指针，而该指针实际指向的是派生类对象，则抛出的对象将被切掉一部分，只有基类部分被抛出。

* 这是因为在 **C++** 的设计哲学中，**性能**始终是优先考虑的因素。静态类型在编译时就已经确定了异常对象的具体类型，编译器可以直接生成用于创建和传递该类型对象的代码，而不需要在运行时对异常对象进行额外的类型检查或解析。这种编译时的确定性减少了运行时开销，使得异常处理机制更加高效。
* 动态类型检查会引入额外的开销，特别是因为异常处理在许多程序中很少发生，因此 C++ 更倾向于使用静态类型来在编译阶段确定异常对象的类型。这样可以避免运行时的额外复杂性，也符合 C++ 的 **“零开销异常”** 设计理念**（Zero-cost exception handling）**。

#### 1.6 捕获异常

catch 子句中的异常声明形式上和只包含一个形参的函数形参列表类似。并且和普通函数类似，如果 catch 无需访问抛出的表达式的话，则我们可以忽略捕获形参的名字。

#### 1.7 异常对象只能使用左值捕获

catch 子句声明的类型决定了处理代码所能捕获的异常类型。

* 这个类型必须是完全类型。
* 它可以是左值引用，但**不能是右值引用**。

和普通函数的参数类似，如果 catch 的参数类型是非引用，则该参数是异常对象的一个副本；相反，如果参数类型时左值引用，则该参数是异常对象的一个别名，因此此时改变参数也会改变异常对象。

看下面的代码：

``` cpp
void f3() { throw 100; }

void f2() { f3(); }

void f1()
{
    try { 
        f2(); 
    }
    catch(int &x) { 
        cout << x << endl;  // 100 
    }
}

int main() 
{
    f1();
    return 0;
}
```

如果我们将 catch 中的 `int &x` 修改为 `int &&x`，程序会报错：

``` cpp
 error: cannot declare ‘catch’ parameter to be of rvalue reference type ‘int&&’
```

这就很奇怪，明明我们在 `f3()` 中抛出了一个临时对象 100，但是我们却不能用右值引用接受。事实上，根据 **C++ 标准（[except.handle]）**：

> The exception-declaration in a `catch` clause shall not be an **rvalue reference**. It can be a **value type (`T`)** or **lvalue reference (`const T&`/`T&`)**.

之所以这么设计，简而言之，是因为异常对象在抛出时是一个**持久的临时对象**，只能通过左值引用或按值传递来捕获，不能用右值引用。

**1. 异常对象的生命周期已延长**

- 当抛出异常时，异常对象会被存储在 **特殊内存区域**（通常称为“异常存储区”或“异常缓冲区”，这个区域会在栈展开的过程中**持续有效**）。直到异常被捕获并处理完毕，其生命周期延长至 `catch` 块结束。

- 这个异常缓冲区一般是**线程的异常处理栈（thread-local storage）或某种运行时的异常对象区域中**。

  也就是说：这个异常对象不是放在函数栈上、也不是全局的，而是由 **C++ 的异常运行时系统管理的一块内存区域**。

**2. 右值引用的语义不适用于异常处理**

* 右值引用意味着你打算“接管”或“偷走”资源。但异常机制的本质是**传递错误信息**而不是**转移资源**。
  * 异常应该是可复用、可观察的，不应该在中途被“破坏”。

**3. 异常对象在 `catch` 块外仍然存在**

* 异常对象在抛出时会在某个地方被构造，并在 `catch` 块**外部**的生命周期持续到 `catch` 块结束。
  * 如果 `catch (T&& e)` 被允许，开发者可能会在 `catch` 中把 `e` 移动出去（比如 `std::move(e)`），但异常对象在函数堆栈中依然存在。
  * 移动后，对象可能处于“已被搬空”的状态，而之后访问它可能出错，造成**悬空引用**或**未定义行为**。
* 换言之，异常对象在内部有一套自己的生命周期管理机制。异常对象的生命周期不是你代码里能直接访问或控制的，是由 **C++ 异常运行时系统内部实现管理的内存区**。

**4. 历史兼容性**

- C++11 引入移动语义前，异常机制已稳定运作多年。
- 允许 `catch (T&&)` 会破坏现有代码的假设（如 `catch (const T&)` 的普遍使用）。

**错误示例**（假设允许 `catch (T&&)`）：

```cpp
try {
    throw std::string("hello");
} 
catch (std::string&& e) {  // 假设允许
    std::string stolen = std::move(e);  // "移动" e
    // 此时 e 可能为空（资源被转移），但异常对象仍需在 catch 结束后销毁（由于异常对象本身的生命周期管理）
}   // 析构 e 时可能出错（如果 e 的资源已被转移）
```

因此说，最好使用 `T e` 或 `const T &e` 来捕获异常。

----

ok，回到原程序，在函数 `f3` 的作用域中我们抛出了一个临时对象 `100`，按理说，在函数 `f3` 结束之后，这个临时对象应该被销毁。但这是异常对象，如果把一个还没处理的异常对象销毁也太逆天了。

正如我们前面提到的，异常对象会被存储在 **特殊内存区域**（通常称为“异常存储区”或“异常缓冲区”），这个区域会在栈展开的过程中**持续有效**，因此这里随着栈展开，临时异常对象并不会被销毁，它始终存在于异常缓冲区。

#### 1.8 异常对象本身依然区分左右值

尽管我们上面说，只能使用左值捕获异常，但异常本身依然是**区分左值和右值**的，这种设计看似矛盾，实则背后有深刻的语义和性能考量。

**1. 异常对象的构造阶段：左值/右值属性直接影响构造方式**

虽然 `catch` 时只能用左值引用捕获，但**抛出（`throw`）表达式时的左值/右值属性决定了异常对象的构造方式**：

```cpp
// 案例1：抛出右值（触发移动构造）
throw std::string("temp"); // 右值 → 优先调用移动构造函数（若存在）

// 案例2：抛出左值（强制拷贝构造）
std::string ex = "error";
throw ex; // 左值 → 调用拷贝构造函数
```

- **右值属性优化性能**：
  如果抛出的是临时对象（右值），编译器会优先尝试移动构造（而非拷贝），减少不必要的资源复制。
- **左值属性保证安全**：
  如果抛出的是具名对象（左值），编译器强制拷贝构造，避免原对象被修改或销毁后导致悬空引用。

如果 `throw` 不区分左值/右值，所有异常对象都会强制拷贝，导致性能损失。

**2. 类型系统的完备性**

- C++ 的对象模型要求所有表达式都有明确的左值/右值分类，异常抛出表达式（`throw expr`）也不例外。
- 统一规则能避免特殊例外，简化语言设计。

#### 1.9 派生体系下的异常对象

C++ 异常处理机制的核心在于 **编译期静态类型检查**，这体现在 `catch` 子句的形参类型（即异常声明）决定了它能捕获的异常类型及后续操作。这种设计既保证了类型安全，又为编译器优化提供了基础。

* `catch` 子句的形参类型在**编译期**确定，其静态类型将决定 `catch` 语句所能执行的操作。
* 如果 `catch` 形参类型是基类的指针或引用，那么也可以使用派生类对象进行初始化，但是，此时只会保留基类部分，派生类部分将被切掉。

##### **1. 静态类型决定捕获能力**

`catch` 子句的形参类型（如 `catch (const std::exception& e)`）在 **编译期** 确定，编译器会：

1. **检查类型是否可访问**（如析构函数、拷贝构造函数是否合法）。
2. **生成类型匹配的跳转表**（用于运行时匹配抛出的异常类型）。
3. **限制 `catch` 块内的操作**（例如，`const` 修饰的引用禁止修改异常对象）。

```cpp
try {
    throw std::runtime_error("error");
} catch (const std::exception& e) {  // 静态类型为 const std::exception&
    // e 只能调用 std::exception 的成员函数（如 what()）
    // 不能修改 e（因为 const 修饰）
}
```

##### **2. 为什么必须在编译期确定类型？**

###### **2.1 异常处理的高效实现**

C++ 异常机制通常通过 **表驱动（Table-Driven）** 实现：

* 编译器会为每个 `try-catch` 块生成 **类型匹配表**（如 `.gcc_except_table`）。
* 抛异常时，运行时系统（libunwind）根据抛出的类型动态查找匹配的 `catch` 块。
* **静态类型信息** 是生成这些表的基础。

> ## **gcc_except_table**
>
> `gcc_except_table` 是 **GCC 编译器在生成 C++ 异常处理代码时产生的一个特殊段（section）**，用来存储异常处理信息。
>
> 这个段的主要作用是：
>
> * 为每个 `try` 块记录其对应的 `catch` 子句信息和跳转处理逻辑，供运行时抛出异常时做查找匹配用。
>
> ------
>
> ### 🚦 举个例子：
>
> ```cpp
> try {
>     may_throw();
> } catch (const MyError& e) {
>     handle();
> }
> ```
>
> 在编译时，GCC 会为这个 `try-catch` 生成一段数据，描述：
>
> - 哪段代码属于 `try` 块；
> - 它能 catch 到什么类型；
> - 匹配上后跳转到哪里执行；
> - 是 catch by value 还是 catch by reference；
>
> 这些信息就被放进了 `.gcc_except_table` 段中。
>
> ------
>
> ## 📦 它和异常对象缓冲区的区别？
>
> | 项目               | 说明                                                         |
> | ------------------ | ------------------------------------------------------------ |
> | `gcc_except_table` | 编译器生成的**元数据**，描述 `try-catch` 的结构、类型匹配等  |
> | 异常对象缓冲区     | 在运行时动态分配的**内存区域**，实际存储 `throw` 出去的异常对象 |
>
> * 🔹 `gcc_except_table` 是 **静态** 的，编译时就存在于二进制文件中（类似调试信息，但参与运行）
> * 🔹 异常对象缓冲区是 **动态** 的，运行时 `throw` 时才分配，用于传递实际对象
>
> ------
>
> ## 📍 它在内存的哪里？
>
> `.gcc_except_table` 是一个 **ELF（或 Mach-O / PE）可执行文件的段**，在二进制中你可以看到它。
>
> 如果你运行：
>
> ```cpp
> readelf -SW a.out | grep gcc_except_table
> ```
>
> 你可能会看到：
>
> ```
> [23] .gcc_except_table   PROGBITS        0000000000402000  00002000
> ```
>
> 说明它是一个独立的段，内容是在加载程序时映射进内存的一部分。
>
> ------
>
> ## 🧠 它是干啥用的？
>
> 当程序执行过程中发生异常，C++ 异常运行时会：
>
> 1. 依据当前指令位置，查找 `.gcc_except_table` 对应的条目；
> 2. 比对异常类型是否匹配当前的 `catch`；
> 3. 若匹配成功，根据表中信息跳转到对应的 `catch` 块执行；
> 4. 若没有匹配，就继续向上抛出异常。
>
> 这个过程通常称为 **栈展开（stack unwinding）**，`gcc_except_table` 就是这个过程的**“地图”**。
>
> ------
>
> ## ✅ 总结一下
>
> | 项目                | 说明                                            |
> | ------------------- | ----------------------------------------------- |
> | `.gcc_except_table` | 存放异常处理的**类型信息和跳转表**，静态数据    |
> | 作用                | 辅助异常匹配和控制流跳转                        |
> | 存在位置            | 可执行文件的一个段（section），运行时映射入内存 |
> | 和异常对象关系      | 无直接关系，异常对象缓冲区是运行时分配的堆内存  |

###### **2.2 类型安全保证**

- 如果允许运行时动态类型（如 `catch (auto&& e)`），会破坏 C++ 的静态类型系统。
- 编译期确定类型可避免 `catch` 块内出现非法操作（如调用不存在的成员函数）。

###### **2.3 与 RTTI 的协作**

* `catch` 的类型匹配依赖 **运行时类型信息（RTTI）**，但其类型检查仍在编译期完成：

```cpp
try {
    throw Derived();
} catch (const Base& e) {  // 运行时检查 e 的实际类型是否为 Base 或其派生类
    // 但 catch 的静态类型 Base 已在编译期固定
}
```

##### **3. 现代 C++ 的增强与限制**

###### **3.1 允许 `noexcept` 和移动语义**

虽然 `catch` 的类型系统保持严格，但现代 C++ 仍通过其他方式优化异常：

- **移动语义**：`throw` 表达式中的右值可触发移动构造（如 `throw std::string("temp")`）。
- **`noexcept`**：标记不会抛出的函数，帮助编译器优化代码。

###### **3.2 禁止 `catch` 使用模板或 `auto`**

以下代码是非法的：

```cpp
try { throw 42; }
catch (auto e) { ... }  // 错误：catch 不能自动推导类型

template <typename T>
catch (T e) { ... }     // 错误：catch 不能是模板
```

- **原因**：模板和 `auto` 会推迟类型检查到运行时，违背异常处理的静态类型要求。

例如下面的代码：	

``` c++
struct Foo {
    void f() {puts("f");}
};

struct Bar : Foo {
    void g() {puts("g");}
};

int main() 
{
    try {
        throw Bar();
    } 
    catch(Foo& x) {
        x.g(); // class "Foo" has no member "g"
    }
    return 0;
}
```

由于对于静态类型类型 `Foo` 来说，它本身并没有函数 `g` 的信息，所以这里调用函数 `g` 失败。但是对于虚函数来说，`Foo` 中有它的定义，所以虚函数的调用没有问题，并且会触发动态绑定：

``` c++
struct Foo {
    void f() const {puts("f");}
    virtual void v() const {puts("Foo::v");}
};

struct Bar : Foo {
    void g() const {puts("g");}
    void v() const override {puts("Bar::v");}
};

int main() 
{
    try {
        throw Bar();
    } 
    catch(Foo& x) {
        x.v(); // Bar::v
    }
    return 0;
}
```

最后，在编写异常代码时，如果在多个 `catch` 语句的类型之间存在着继承关系，则应该把**最底端（most derived type）**的类放在前面。否则，如果我们把基类放在前面，那么这个基类会 `catch` 掉该派生体系下的所有派生类。

#### 1.10 泛型捕获

为了一次性捕获所有异常，我们使用省略号作为异常声明，记为 `catch(...)`，这样的处理代码称为 ***catch-all*** （泛型捕获）的代码。

由于通过 `...` 我们并不能直到异常的具体类型，甚至得不到异常对象，因此 ***catch-all*** 一般会**结合重新抛出语句 `throw;` 一起使用**，其中 ***catch-all*** 执行当前局部能完成的工作，随后重新抛出异常。

***catch-all*** 可以与其它 `catch` 一起出现，但 ***catch-all*** 显然的要放在最后。

```cpp
try {
    // 可能抛出异常的代码
    throw 42;  // 抛出一个 int 类型异常
} 
catch (const std::exception& e) {
    // 捕获派生自 std::exception 的异常
} 
catch (...) {
    // 捕获所有其他类型的异常（如 int、自定义类等）
    std::cout << "Caught an unknown exception!" << std::endl;
}
```

- **功能**：`catch (...)` 是一个“兜底”处理块，可以捕获任意类型的异常（包括基本类型、自定义类、甚至 C 风格的异常）。
- **限制**：无法直接获取异常对象的类型或值（因为没有形参）。

#### 1.11 重新抛出

重新抛出异常时，不需要包任何和表达式，直接 `throw` 即可。

* `throw;` 这种空的 `throw` 语句只能出现在 `catch` 语句或 `catch` 语句直接或间接调用的函数之内。
* 如果在处理代码之外的区域遇到了空 `throw` 语句，相当于我们抛出了一个“无法被 `catch`”的异常，因此编译器将调用 `terminate`。
* 注意，如果我们重新抛出的异常含有一个表达式，就不算是“重新”抛出而是“新”抛出。

有时候，`catch` 语句会改变其异常参数的内容。如果在改变了参数的内容后 `catch` 语句重新抛出异常，则只有当 `catch` 语句声明是引用类型时我们对参数所做的改变才会被保留并继续传播。

例如：

``` C++
void f1()
{
    try {
        throw 10;
    }
    catch(int x) { // 实际上这里会调用拷贝构造函数传入异常缓冲区的异常对象
        cout << "f1: " << x << endl; // f1: 10
        x = 100;
        throw;     // 重新抛出
    }
}

void f2()
{
    try {
        f1();
    }
    catch(int x) {
        cout << "f2: " << x << endl; // f2: 10
    }
}

int main() 
{
    f2();
    return 0;
}
```

如果我们修改参数类型为引用：

``` C++
void f1()
{
    try {
        throw 10;
    }
    catch(int &x) {
        cout << "f1: " << x << endl; // f1: 10
        x = 100;
        throw;
    }
}

void f2()
{
    try {
        f1();
    }
    catch(int &x) {
        cout << "f2: " << x << endl; // f2: 100
    }
}

int main() 
{
    f2();
    return 0;
}
```

#### 1.12 函数 try 语句块

通常情况下，程序执行的任何时刻都可能发生异常，特别是函数异常发生在处理构造函数初始值的过程中。由于构造函数体内的 `try` 语句无法处理初始值列表抛出异常的情况，我们必须将构造函数写成**函数 `try` 语句块（也称为函数测试块，function try block）**。函数 `try` 语句块使得一组 `catch` 语句既能处理构造函数体（或析构函数体），又能处理构造函数的初始化过程（或析构函数的析构过程）。

``` C++
class Foo {
public:
    // 关键字try出现在初始值列表初始值列表以及函数体之前，即，成员变量初始化之前
    Foo(int _val) try 
    : val(_val)  { /*init*/ } 
    catch(int e) {
        cout << e << endl;
    }
private:
    int val;
};
```

对面上面的构造函数 `try` 语句块，我们可以理解为：

``` cpp
//Foo(int _val) 
try {
    val(_val)  
    /*init*/
}
catch(int e) {
    cout << e << endl;
}
```

也就是 `try-catch` 检查的是负责成员变量初始化的初始值列表和花括号函数体。但除了初始值列表和花括号函数题，构造函数还有一种抛出异常的情况，那就是在参数初始化的过程中发生了异常。例如调用者在给 `_val` 传参时发生了异常，不过参数初始化属于调用表达式的一部分，这个异常会在调用者所在的上下文中处理。

#### 1.13 noexcept 异常说明

对于用户和编译器来说，预先知道某个函数不会抛出异常显然大有裨益。

* 首先，知道函数不会抛出异常有助于简化调用该函数的代码（例如，不必使用 `try-catch` 语句块），也因此，编译器不必在编译期处理 `try-catch` 语句块，生成一些 `gcc_except_table` 之类的信息。
* 其次，如果编译器确定函数不会抛出异常，它就能执行某些特殊的优化操作（例如，移动语义），而这些操作并不适用于可能出错的代码。

在 C++11 新标准中，通过 **noexcept 说明（noexcept specification）**指定某个函数不会抛出异常。

* noexcept 说明后面也可以跟一个可选的实参，它会转化为一个 bool 值，用来显式指定 noexcept 说明的真假。

noexcept 紧跟在函数的参数列表后面：

``` C++
retType func(parameters) [const] [&/&&] [noexcept(expr)] [final][override] [=0];  
```

上面语句大致说明了各种“函数说明符”的出现顺序：

1. const 限定符
2. 引用限定符
3. noexcept 说明
4. override/final 说明
5. 纯虚函数的 =0

此外，我们还需要注意 noexcept 的出现时机：

* **对于一个函数来说，noexcept 说明必须出现在该函数的所有声明语句和定义语句中。**
* 我们可以在函数指针的声明和定义中指定 noexcept。指定为 noexcept(true) 的函数指针只能指向定义为 noexcept 的函数
* 在 typedef 或类型别名中则不能出现 noexcept

最后，注意 **noexcept 说明符并不是函数签名的一部分**。

#### 1.14 违反异常说明

编译器并不会在编译时检查 noexcept 说明。实际上，如果一个函数在说明了 noexcept 说明的同时又含有 throw 语句或者调用了可能抛出异常的其它函数，编译器将顺利通过，并不会因为这种违反异常说明的情况而报错。（个别编译器会对这种用法做出警告）

因此可能出现这样一种情况：尽管函数声明了它不会抛出异常，但实际上还是抛出了。一旦一个 noexcept 函数抛出了异常，程序就会调用 terminate 以确保遵守不会在函数运行时抛出异常的承诺。上述过程是否执行栈展开未作约定，因此，实际上，noexcept 可以用在两种情况下：

1. 确认函数不会抛出异常
2. 根本不知道该如何处理异常 —— 因此一旦抛出异常就 terminate

尽管我们通过 try-catch 去捕获异常，也无法捕获 noexcept 函数抛出的异常：

``` C++
void throw_int() { throw 5; }
void call_throw_int() noexcept  { throw_int(); }

int main() 
{
    try {
		call_throw_int();
    }
    catch(int e) {
        cout << e << endl;
    }
    return 0;
}
//terminate called after throwing an instance of 'int'
```

#### 1.15 异常说明的实参与异常运算符

noexcept 说明符接受一个可选的实参，该实参必须能转换为 bool 类型：

* 如果实参为 true，则函数不会抛出异常
* 如果实参为 false，则函数<font color=blue>**可能**</font>抛出异常

noexcept 说明符的实参常常与 **noexcept operator** 混合使用。noexcept 运算符是一个一元表达式，它的返回值是一个 bool 类型的右值常量表达式，用于表示给定的表达式是否会抛出异常。**和 sizeof 运算符类似，noexcept 运算符在编译期求值，且不会对运算对象求值。**

``` C++
struct Foo {
    void f1() noexcept(false) {}
    void f2() noexcept(true)  {}
    void f3() { throw 100; }
    ~Foo() {}
};
void throw_int() noexcept(false) {}

vector<int> v;
string s;
int i;
double d;
void(*fptr)() = &throw_int;

void func() 
{
    cout << noexcept(throw_int()) << endl ; // false
    cout << noexcept(fptr) << endl;         // true
    cout << noexcept(throw_int) << endl;    // true
    
    cout << noexcept(Foo()) << endl;        // true
    cout << noexcept(Foo().~Foo()) << endl; // true
    cout << noexcept(Foo().f1()) << endl;   // false
    cout << noexcept(Foo().f2()) << endl;   // true
    cout << noexcept(Foo().f3()) << endl;   // false
    
    cout << noexcept(v) << endl;            // true
    cout << noexcept(s) << endl;            // true
    cout << noexcept(i) << endl;            // true
    cout << noexcept(d) << endl;            // true
}
```

可以发现：

* 函数默认是可能抛出异常的，析构函数除外

* 如果传入一个函数的调用形式 `noexcept(throw_int())` 会返回函数的 noexcept 属性
* 如果传入的是一个指向函数的指针 `noexcept(fptr)` 或者普通对象，返回 true，甚至空指针也返回 p（一个对象本身怎么会抛出异常呢）

**noexcept 有两层含义：**

1. **当跟在函数参数列表后面时，它是异常说明符；**
2. **当作为 noexcept 异常说明的 bool 实参出现时，他是一个运算符**

#### 1.16 异常说明与指针、虚函数和拷贝控制

尽管 noexcept 说明符不属于函数类型的一部分，但是函数的异常说明仍会影响函数的使用：

1. 函数指针以及该指针所指的对象必须具有一致的异常说明。这一点和 const 类似，一个声明为不抛出异常的指针只能指向不抛出异常的函数；而一个没有声明为不抛出异常的指针既可以指向不抛出异常的函数，也可以指向抛出异常的函数
2. 如果一个虚函数承诺了它不会抛出异常，则后续派生出来的虚函数也必须做出同样的承诺；与之相反，如果基类的虚函数允许抛出异常，则派生类的对应函数既可以允许抛出异常，也可以不允许抛出异常

``` C++
class Foo {
public:
    virtual void f() {}
    virtual ~Foo() = default;
};

class Bar : public Foo {
public:
    void f() override {}
};

class Mike : public Bar {
public:
    void f() noexcept override {}
};

class Cat : public Mike {
public:
// exception specification for virtual function "Cat::f" 
// is incompatible with that of overridden function "Mike::f"
    void f() override {}
};
```

3. 当编译器合成拷贝控制成员时，同时也默认生成一个异常说明。如果对所有成员和基类的所有操作都承诺了不会抛出异常，则合成的成员是 noexcept 的。如果合成成员调用的任意一个函数可能抛出异常，则合成的成员是 noexcept(false)。而且，如果我们定于了一个析构函数但是没有为它提供异常说明，则编译器将合成一个，合成的异常说明将与假设由编译器为类合成析构函数时所得的异常说明一致。

#### 1.17 异常类层次

``` shell
exception
├── bad_cast
├── bad_alloc
└── runtime_error
│   ├── overflow_error
│   ├── unrderflow_error
│   ├── range_error
│   └── ...
└── logic_error
    ├── domin_error      # 参数不在允许的范围内
    ├── invalid_argument # 传递给函数的参数无效
    ├── out_of_range     
    ├── length_error     # 试图创建一个超出容器最大大小的异常
    └── ...
```

类型 `exception` 仅仅定义了虚析构函数、拷贝构造函数（移动构造函数）、拷贝赋值运算符（移动赋值运算符）、和一个名为 `what` 的虚成员。

* 也即 **BigFive** 和一个成员函数 `what()`。
* 其中 `what` 函数返回一个 `const char*`，该指针指向一个一 `NULL` 结尾的字符数组，并且确保不会抛出任何异常：

``` c++
class exception
{
public:
    exception() _GLIBCXX_NOTHROW { }
    virtual ~exception() _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
#if __cplusplus >= 201103L
    exception(const exception&) = default;
    exception(exception&&) = default;
    exception& operator=(const exception&) = default;
    exception& operator=(exception&&) = default;
#endif

 	// Returns a C-style character string describing the general cause of the current error.
    // Promise what() do not throw exception
    virtual const char*
        what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW;
};
```

类 `exception`、`bad_alloc` 和 `bad_cast` 定义了默认构造函数，类 `runtime_error` 和 `logic_error` 没有默认构造函数，但是有一个可以接受 `C` 风格字符串或者标准库 `string` 类型实参的构造函数，这些实参负责提供关于错误的更多信息。

在这些类中，`what` 负责返回用于初始化异常对象的信息。因为 `what` 是虚函数，所以当我们捕获基类的引用时，对 `what` 函数的调用将执行与异常对象动态类型对应的版本。

#### 1.18 自定义异常

``` C++
class divide_by_zero : public runtime_error {
public:
    divide_by_zero(const std::string &s)
        : runtime_error(s) {}
    virtual const char* what() const noexcept override {
        return runtime_error::what();
    }
};

int main() 
{
    int a = 1, b = 0;
    try {
        if(!b)  throw divide_by_zero("error: divide by zero, Floating pointe exception");
        int r = a / b;
    } 
    catch(runtime_error &e) {
        cout << e.what() << endl;
    }
    return 0;
}
```

### 2. 命名空间

大型程序往往会使用多个独立开发的库，这些库又会定义大量的全局名字，如类、函数和模板等。当应用程序用到多个供应商提供的库时，不可避免地会发生某些名字相互冲突的情况。多个库将名字放置在全局命名空间中将引发**命名空间污染（namespace pollution）**。

命名空间为防止名字冲突提供了更加可控的机制。

* 命名空间分割了**全局命名空间**，其中每个命名空间是一个作用域。
* 命名空间的结尾花括号不需要分号结尾。

#### 2.1 命名空间定义

##### 1. 每个命名空间都是一个作用域

定义在某个命名空间中的名字可以被该命名空间内的其他成员直接访问，也可以被这些成员内嵌作用域中的任何对象访问。位于命名空间之外的代码则必须明确指出所有名字属于那个命名空间 `using namespace namespace_name;`。

##### 2. 命名空间可以是不连续的

回想一下我们为某个自定义类型指定 `hash` 模板特例化时，此时 `std` 命名空间就是不连续的。

命名空间不连续这个特性也允许我们将命名空间中的成员的声明和定义分离，负责声明的命名空间放在头文件中，负责定义的命名空间部分则放在源文件中。

同样的，不同的命名空间也应该放在不同的文件中。

##### 3. 定义命名空间成员

在通常情况下，我们不把 `#include` 放在命名空间内部。如果我们这么做了， 隐含的意思是把头文件中所有的名字定义成该命名空间的成员。

可以在命名空间的外部定义命名空间的成员，此时该成员必须使用含有命名空间前缀的名字。对于函数来说，命名空间成员的外部定义和类成员的外部定义类似，在名字处指定命名空间之后，参数列表以及函数体内就无需再指定命名空间前缀了：

``` c++
namespace prime {
    class Foo;
    Foo operator+(const Foo &lhs, const Foo &rhs);
}

// 只需要在类名处指定命名空间前缀
class prime::Foo {
public:
    Foo(int _val = 0) : val(_val) {}
    Foo& operator+=(const Foo& rhs);
    int get() const { return val; }
private:
    int val;
};

// 需要在返回类型和函数名处指定命名空间前缀
// 和普通成员函数一样，如果我们在函数名中指定了 prime::Foo，就无需在形参中再次指定了
prime::Foo& prime::Foo::operator+=(const /*prime::*/Foo& rhs) {
    val += rhs.val;
    return *this;
}

// 需要在返回类型和函数名处指定命名空间前缀
prime::Foo prime::operator+(const Foo &lhs, const Foo &rhs)
{
    Foo res(lhs);
    return res += rhs;
}

int main() 
{
    prime::Foo a(10), b(20);
    a += b;
    cout << a.get() << endl; 
    cout << (a + b).get() << endl;
    return 0;
}
```

##### 4. 模板特例化

模板特例化必须定义在原始模板所属的命名空间中。

``` c++
struct Foo {
    int val;
    bool operator==(const Foo &rhs) const {
        return val == rhs.val;
    }
};

// struct hash定义在命名空间std中
namespace std {
    template<>
    struct hash<Foo> {
        size_t operator()(const Foo &f) const {
            return hash<int>{}(f.val);
        }
    };
}

int main() 
{
    unordered_set<Foo> s;
    s.insert({1});
    s.insert({1});
    s.insert({2});
    s.insert({3});
    for(auto &x : s)
        cout << x.val << endl;
    return 0;
}
```

##### 5. 全局命名空间

全局作用域中定义的名字定义在全局命名空间（**global namespace**）中。全局命名空间以**隐式**的方式声明，并且在所有程序中都存在。全局作用域中定义的名字是被隐式地添加到全局命名空间中的。

由于全局作用域是隐式的，所以他没有名字（匿名）：

``` c++
int x = 10;
int y = ::x; // 10
```

##### 6. 嵌套的命名空间

``` C++
namespace outer {
    int x = 10;
    namespace inner {
        int y = x;
        double x = 3.14;
    }
    double z = inner::x;
}
```

嵌套的命名空间同时也是一个嵌套的作用域。它遵循一般的嵌套作用于的名字规则：

* 内层命名空间声明的名字将隐藏外层命名空间声明的名字
* 在嵌套的命名空间中定义的名字只在内层命名空间中有效，外层命名空间中的代码想访问它必须在名字前添加限定符

##### 7. 内联命名空间

C++11 新标准引入了一种新的嵌套命名空间（nested namespace），称为内联命名空间（inline namespace）。

**与普通的嵌套命名空间不同，内联命名空间的名字可以被外层命名空间直接使用：**

``` c++
namespace outer {
    int x = 10;
    inline namespace inner {
        int y = x;
    }
    int z = y;
}
```

关键字 inline 必须出现在命名空间第一次定义的地方，后续在打开命名空间时 inline 可有可无。

内联命名空间可以用于**版本发布**。例如，我么可以将当前版本的代码放在内联命名空间中，而之前的代码都放在一个非内联命名空间中。

##### 8. 未命名的命名空间

未命名的命名空间就是没有名字的命名空间。未命名的命名空间中定义的变量拥有**静态生命周期**：他们在第一次使用前创建，并且直到程序结束才销毁。

``` C++
namespace {
    int x = 10;
    template<typename T>
    T get_self_value(const T &x) { return x; }
}

int main() 
{  
    cout << x << endl;
    cout << get_self_value(10) << endl;
    return 0;
}
```

我们可以直接使用未命名的命名空间中的成员，毕竟我们也找不到什么命名空间的名字来限定他们；也因此，我们也不能对未命名的命名空间的成员使用作用域运算符。

另外我们也可以发现，未命名的命名空间的主要作用就是用于声明静态成员。事实上，**在 C++ 中，我们正是以未命名的命名空间取代文件中的静态声明。**

> 在标准 C++ 引入命名空间的概念之前，程序需要将名字声明成 static 以使其只对于整个文件有效。这种在文件中进行静态声明的做法是从 C 语言继承而来的。在 C 语言中，声明为 static的全局实体在其所在的文件外不可见。

也正因为用于文件内静态声明的特性，一个未命名的命名空间可以在某个给定的文件内不连续，但是不能跨越多个文件。每个文件定义自己的未命名的命名空间，不同文件中的未命名的命名空间互相无关。如果一个头文件定义了未命名的命名空间，则该命名空间中定义的名字将在每个包含了该头文件的文件中对应不同的实体。

未命名的命名空间中定义的名字的作用域与该命名空间所在的作用域相同：

``` C++
/*=====================================================*/
int val = 1024;

namespace {
    int val = 16; // val的作用域为全局作用域
}

int main() 
{  
    cout << val << endl; // "val" is ambiguous
    return 0;
}
/*=====================================================*/
int val = 1024;

namespace outer {
    namespace {
        int val = 16; // val的作用域为::outer
    }
}

int main() 
{  
    cout << val << endl;        // 1024
    cout << outer::val << endl; // 16
    return 0;
}
```

#### 2.2 使用命名空间成员

通过作用域运算符获取命名空间成员太过繁琐，下面是一些更为简便的使用命名空间成员的方法：

##### 1. 命名空间别名

``` C++
namespace outer {
    namespace inner {
        int val = 10;
    }
}

// 通过namespace来制定别名
namespace space = outer::inner;

int main()
{
    cout << space::val << endl;
    return 0;
}
```

##### 2. using 声明

一条 using 声明语句一次只引入命名空间的一个成员。对于 using 声明引入的名字，我们可以直接使用而不需要再加上作用域标志。

**using 声明受到作用域的限制**：

* 一条 using 声明语句可以出现在全局作用域、局部作用域、命名空间作用域以及类的作用域中。
* using 声明所引入变量的作用域与 using 所在作用域相同。它的有效范围从 using 声明的地方开始，一直到 using 声明所在的作用域结束为止。
* 在此过程中，外层作用域的同名实体将被隐藏。

例如下面的代码，虽然在 namespace outer 中定义的 val 作用域为 `::outer`，但如果我们将其通过 using 声明引入到 `f()` 中的 for 循环，其作用域就位于 for 循环这个作用域，也即此时 `using outer::val` 相当于 `int val = 10;`。因此此时如果我们在 for 循环中再定义一个名为 val 的变量，就会导致命名冲突。

``` C++
namespace outer {
    int val = 10;
}

int val = 20;

void f()
{
    // outer::val不可见
    cout << ::val << endl;        // 20
    cout << ::outer::val << endl; // 10
    int val = 30;
    cout << val << endl;          // 30
    for(int i = 0; i < 3; i ++ ) {
        // int val = 20; // 会导致命名冲突
        using outer::val;
        cout << val << " \n"[i == 2]; // 10 10 10
    }
    // outer::val不可见
}
```

##### 3. using 指示

**using 指示（using directive）**和 using 声明类似的地方是，我们可以使用命名空间名字的简写形式；和 using 声明不同的是，我们无法控制那些名字是可见的，因为 using 指示会引入所有名字。

using 指示以 using 关键字开始，后面是关键字 namespace 和命名空间的名字。这个命名空间必须是已经定义好的命名空间的名字，否则程序将发生错误。

##### 4. using 与作用域

我们先来看两个例子：

``` C++
int a = 1;

namespace A {
    int a = 11, b = 22, c = 33;
}

int main()
{
    using A::a;        // using 声明
    cout << a << endl; // 11
    return 0;
}

/*=======================================================================*/

int a = 1;

namespace A {
    int a = 11, b = 22, c = 33;
}

int main()
{
    using namespace A; // using 指示
    cout << a << endl; // "a" is ambiguous
    return 0;
}
```

一样的代码，只是一个使用 using 声明，一个使用 using 指示，但结果天差地别。

* 我们前边提到过 using 声明的名字的作用域与 using 声明语句本身的作用域一致，效果上就好像将 using 声明所引入的变量直接定义在了该作用域一样。
* <font color=blue>但 using 指示所做的绝非声明别名这么简单。相反，**它具有将命名空间成员提升到包含命名空间本身和 using 指示的最近作用域的能力**。</font>由于命名空间一般是定义在全局作用域，因此这里命名空间成员的名字通常都被提升为全局作用域。所以说，通过 using 指示引入命名空间成员是一种很危险的行为，它很容易导致命名冲突

#### 2.3 类、命名空间与作用域

##### 1. 参数依赖查找

**参数依赖查找（Argument-Dependent Lookup, ADL）**，也称为 **Koenig 查找**（以提出者 Andrew Koenig 命名），是 C++ 中一种特殊的函数查找规则。它的核心思想是：<font color=blue>**当调用一个函数时，编译器不仅会在当前作用域查找该函数，还会在函数参数所属的命名空间中查找**。</font>

ADL 的主要目的是让 **与类相关的函数**（如运算符重载、友元函数）能够被直接调用，而无需显式指定命名空间。例如

``` c++
std::string s;
std::cin >> s; // 等价于operator>>(std::cin, s);
```

`operator>>` 函数定义在标准库 `string` 中，`string` 又定义在命名空间 `std` 中，但是我们不需要 `using` 声明和 `std::` 限定符就可以调用该函数。

当编译器发现对 `operator>>` 的调用时，首先在当前作用域寻找合适的函数，接着查找输出语句的外层作用域。随后，因为 `>>` 表达式的形参是类类型的，所以编译器还会查找 `cin` 和 `s` 的类所属的命名空间。也就是说，对于这个调用来说，编译器会查找定义了 `istream` 和 `string` 的命名空间 `std`。当在 `std` 中查找时，编译器找到了 `string` 的输出运算符函数。

假如该规则不存在，则我们将不得不为输出运算符专门提供一个 `using` 声明或使用命名空间限定符：

``` cpp
using std::operator>>;
// 或者：
std::operator>>(std::cin, s);
```

显然上面两种形式都比较笨拙。

下面是一个具体的例子：

``` c++
namespace A {
    struct Foo { int val; };
    Foo operator+(const Foo &a, const Foo &b) { return Foo{a.val + b.val}; }
    void print(const Foo &f) { cout << f.val << endl; }
}

int main()
{
    A::Foo a{10}, b{20};
    // 无需加限定符或 using 声明就可以使用 namespace A 中的函数
    A::Foo c = a + b;
    cout << c.val << endl; // 30
    print(c);              // 30
    return 0;
}
```

**ADL 的查找范围:**

1. **参数类型所属的命名空间**（如 `A::X` 会查找 `A`）。
2. **参数类型的直接和间接基类所属的命名空间**。
3. **模板参数的命名空间**（如果是模板类）。

##### 2. 查找与 std::move 和 std::forward 

前面我们提到过，对于 `move`（以及 `forward` 函数）的调用，要使用完全限定符的形式 `std::move` 和 `std::forward`。

* 因为标准库 `move` 和 `forward` 函数都是模板函数，它们都接受一个右值引用的函数形参。而在函数模板中，右值引用形参是万能匹配，它可以匹配任何类型。因此如果我们定义了一个 `move` 或 `forward` 函数，那么不管函数接受什么类型的参数，都会与标准库的版本冲突。
* 因此，如果我们希望使用标准库版本的 `move` 和 `forward`，总是加上 `std::` 是最保险的。

##### 3. 友元声明与实参相关的查找

在友元中，我们提到过，当类声明了一个友元时，该友元声明并没有使得友元本身可见，我们仍然需要在类外定义友元。然而，C++ 标准规定：

* <font color=blue>**如果某个类或函数在友元声明中首次出现（之前未声明），且未被显式限定（如 `::` 或 `A::`），则它会被认为是最近的外层命名空间的成员。**
  （参考：ISO C++ [namespace.memdef]/3）</font>

因此，在下面的例子中：

- `foo` 首次出现在 `friend void foo(C);` 中，且无 `::` 或 `A::` 限定，因此被认为是 `A::foo`。
- 后续定义必须位于 `A` 命名空间内（否则链接错误）。

``` c++
namespace A {
    class C {
        friend void f(const C&) {}
        friend void f2() {}
    };
}

int main()
{
    A::C objs;
    f(objs);
    f2(); // identifier "f2" is undefined
    return 0;
}
```

* `f(const C&)` 由于 **参数依赖查找（ADL, Argument-Dependent Lookup）**，编译器可以在 `A` 命名空间中找到它。
* `f2()` **没有参数**，因此 ADL 不适用，编译器无法找到它的声明。

#### 2.4 重载与命名空间

命名空间对函数的匹配过程有两方面的影响：

1. `using` 声明或 `using` 指示能将某些函数添加到候选函数集
2. 对于接受类类型参数的函数来说，其名字查找将在实参类所属的命名空间中进行

##### 1. 重载与 using 声明

对于一个 `using` 声明来说，要注意**它声明的是一个名字，而非一个特定的函数**。因此对于 `using NS::print;` 来说，该函数的**所有版本**都被引入到当前作用域。

1. 一个 `using` 声明引入的函数将重载该声明语句所属作用域中其它已有的同名函数
2. 如果 `using` 声明出现在局部作用域当中，则引入的名字将隐藏外层作用域的相关声明
3. 如果 `using` 声明所在的作用域中已有一个同名且形参列表相同的函数，该 using 声明将引发错误

注意第 2 点所说的隐藏外层作用于的相关声明，是指**隐藏外层作用于的所有声明**，因为我们本质上是隐藏了外层作用域中的**“同名函数”**。而所有重载函数都是同名函数！

``` C++
namespace NS {
    void print(int x) { cout << "NS::int" << endl; }
}

void print(const char *x) { cout << "::const char *" << endl; }
void print(double x)      { cout << "::double" << endl; }
void print(char ch)       { cout << "::char" << endl; }

int main()
{
    using NS::print; // 隐藏全局作用于中的三个print函数
    print(1);        // NS::int
    print('a');      // NS::int
    print("hello");  // error: invalid conversion from ‘const char*’ to ‘int’
    return 0;
}
```

##### 2. 重载与 using 指示

`using` 指示将命名空间的成员提升到外层作用域中，如果命名空间的某个函数与该命名空间所属作用域的某个函数同名，则命名空间的函数被添加到重载集合中。

与 `using` 声明不同的是，对于 `using` 指示来说，**引入一个已有函数形参列表完全相同的函数并不会产生错误**。此时，只要我们指明调用的是命名空间中的函数版本还是当前作用域的版本即可：

``` c++
namespace NS {
    void print(const char *x) { cout << x << endl; }
    void print(double x)      { cout << x << endl; }
    void print(int x)         { cout << x << endl; }
    int val = 10;
}

int val = 10;
void print(int x) { cout << x << endl; }

// 使用using指示如果出现同名同形参列表的函数并不会报错,普通变量也是
using namespace NS;

int main()
{
    ::print(::val);
    NS::print(NS::val);
    return 0;
}

/*=====================================================*/

namespace NS {
    void print(const char *x) { cout << x << endl; }
    void print(double x)      { cout << x << endl; }
    void print(int x)         { cout << x << endl; }
    int val = 10;
}

int val = 10;
void print(int x) { cout << x << endl; }

// 如果使用using声明就会报错
using NS::print;    // conflicits
using NS::val;      // conflicits
```

### 3. 多重继承与虚继承

使用多重继承时要十分小心，经常会出现二义性问题。

许多专业人员认为：**不要提倡在程序中使用多重继承**，只有在比较简单和不易出现二义性的情况或实在必要时才使用多重继承，能用单一继承解决的问题就不要使用多重继承。也是由于这个原因，有些面向对象的程序设计语言（如Java，Smalltalk）并不支持多重继承。

#### 3.1 多重继承

每个基类包含一个可选的访问说明符。如果忽略，`class` 默认为 `private`，`struct` 默认为 `public`。

和单继承一样，多重继承的派生列表也只能包含已经被定义的类，而且这些类不能是 `final` 的。C++ 并没有对最大继承个数做出限制，但在某个给定的派生列表中，同一个基类只能出现一次。

##### 1. 派生类构造函数初始化所有基类

和单继承一样：

* 构造一个派生类的对象将同时构造并初始化它的所有基类子对象
* 多重继承的派生类的构造函数初始值也只能初始化它的直接基类
* 基类的构造顺序与派生列表中基类出现的顺序保持一致，而与派生类构造函数初始值列表中基类的顺序无关。（这一点和）

##### 2. 继承的构造函数与多重继承

C++11 新标准中，允许派生类从它的一个或多个基类中继承构造函数。但是如果从多个基类中继承了相同的构造函数（即形参列表完全相同），则程序将产生错误：

``` C++
struct B1 {
    B1() = default;
    B1(int x) {}
};

struct B2 {
    B2() = default;
    B2(int x) {}
};

struct D : public B1, public B2 {
    using B1::B1;   // 引入B1的构造函数
    using B2::B2;   // 引入B2的构造函数

};

int main()
{
    D d(10);
    return 0;
}

// more than one instance of constructor "D::D" matches the argument list:C/C++(309)
// main.cpp(29, 9): function "B1::B1(int x)" (declared at line 12), inherited via using decl at line 21
// main.cpp(29, 9): function "B2::B2(int x)" (declared at line 17), inherited via using decl at line 22
// main.cpp(29, 9): argument types are: (int)
```

其实通过“继承”的构造函数的原理我们可以更清晰的观察出来，当我们引入 `B1` 和 `B2` 的构造函数时，编译器实际上会为 `D` 生成两个构造函数：

``` c++
struct D : public B1, public B2 {
    D(int x) : B1(x) {}
    D(int x) : B2(x) {}
    // 参数列表完全相同，重复定义错误
};
```

此时我们必须自定义重复定义的函数，例如：

``` c++
struct D : public B1, public B2 {
    D(int x) : B1(x), B2(x) {}
};
```

##### 3. 析构函数与多重继承

和单继承一样，析构函数只需要负责清除类本身分配的资源即可，派生类的资源以及基类都是自动销毁的。

合成的析构函数函数体为空。

##### 4. 多重继承的派生类的拷贝和移动操作

与单继承一样，多重继承的派生类如果定义了自己的拷贝/赋值构造函数和赋值运算符，则必须在完整的对象上执行拷贝、移动或赋值操作。即，我们要确保这些操作的“完整性”，对于派生类的拷贝、移动或赋值操作而言，你不能仅仅只完成派生类对象的拷贝、移动或赋值，还需要完成其直接基类部分的拷贝、移动和赋值。

因为对于自定义的拷贝、移动和赋值成员，在执行时不会调用直接基类的对应成员。只有当派生类使用的是合成版本的拷贝、移动和赋值成员时，才会自动对其基类执行这些操作，即每个基类分别使用自己的对应成员隐式的完成构造、移动和销毁等操作。

``` c++
struct B1 {
    B1() = default;
    B1(B1&) { puts("B1::B1"); }
};

struct B2 {
    B2() = default;
    B2(B2&) { puts("B2::B2"); }
};

struct D : public B1, public B2 {
    D() = default;
};

int main()
{
    D a;
    // 由于我们没有自定义D的拷贝构造函数,因此会为D合成一个拷贝构造函数
    // 对于D的合成的拷贝构造函数,会隐式调用基类的对应成员
    D b(a);
    return 0;
}
// B1::B1
// B2::B2

/*=======================================================================================*/

struct B1 {
    B1() = default;
    B1(B1&) { puts("B1::B1"); }
};

struct B2 {
    B2() = default;
    B2(B2&) { puts("B2::B2"); }
};

struct D : public B1, public B2 {
    D() = default;
    D(D&) { puts("D::D"); }
};

int main()
{
    D a;
    // 由于我们自定义了D的拷贝构造函数,因此这里只会执行D的拷贝构造函数的函数体
    D b(a);
    return 0;
}
// D::D
```

#### 3.2 类型转换与多个基类

##### 1. 重载决议

和单继承一样，在多继承体系中，基类的指针和引用可以接受一个派生类对象。

值得注意的是，**编译器不会在派生类向<font color=blue>直接基类</font>的几种转换中进行比较和选择，因为在它看来转换到任意一种<font color=blue>直接基类</font>都一样好**：

``` c++
struct B {};
struct B1 : public B {};
struct B2 {};
struct D : public B1, public B2 {};

void print(B&)  { puts("B"); }
void print(B1&) { puts("B1"); }

int main()
{
    D d;
    // 由于B1是D的直接基类，因此这里会优先调用参数类型为B1的print
    print(d); // B1
    return 0;
}

/*==============================================================*/

struct B {};
struct B1 : public B {};
struct B2 {};
struct D : public B1, public B2 {};

void print(B2&) { puts("B2"); }
void print(B1&) { puts("B1"); }

int main()
{
    D d;
    print(d); // ambigous
    return 0;
}
```

##### 2. 基于指针类型或引用类型的查找

与单继承一样，对象、指针和引用的静态类型决定了我们能够使用那些成员。特殊的，当类型是指针或引用，调用函数是虚函数时，会进行类型动态绑定。

#### 3.3 多重继承下的类作用域

##### 1. 使用二义性

在只有一个基类的情况下，派生类的作用域嵌套在直接基类和间接基类的作用域中。查找过程沿着继承体系自底向上进行，直到找到所需的名字。派生类的名字将隐藏基类的同名成员。

在多重继承的情况下，相同的查找过程在所有直接基类中**同时进行**（逻辑上？）。如果名字在多个基类中都被找到，则对该名字的“使用”将具有二义性。**只有在“使用”时具有二义性意味着，对于一个派生类来说，从它的几个基类中分别继承名字相同的成员是完全合法的，只不过在使用这个名字时必须明确指出他的版本**：

``` c++
struct A {
    int max_weight() { return 0; }
};

struct B {
    int max_weight() { return 1; }
};

struct C : public A, public B {
    int max_weight_A() { return A::max_weight(); }
    int max_weight_B() { return B::max_weight(); }
};

int main()
{
    C c;
    cout << c.max_weight_A() << endl;  // 0
    cout << c.max_weight_B() << endl;  // 1
    cout << c.max_weight() << endl;    //  error: request for member ‘max_weight’ is ambiguous
    return 0;
}
```

* 只有在“使用”时具有二义性也可以从报错中看出来，这里在我们调用 `c.max_weight()` 时，报错信息说的是对 `max_weight` 的请求具有二义性，而不是说对 `max_weight` 的定义具有二义性。

##### 2. 名字查找先于重载决议

在C++中，即使基类中的同名函数参数不同，**多重继承时仍然会产生歧义**，这是由C++的名字查找（name lookup）和重载决议（overload resolution）规则共同决定的。根本原因在于：**名字查找（Name Lookup）先于重载决议**。

``` cpp
struct B {
    int max_weight() { return 0; }
};

struct B1 : public B {};

struct C {
    int max_weight(int x) { return x; }
};

struct D : public B1, public C {};

int main()
{
    D d;
    // B2 中的 max_weight 和 B1 中继承自 B 的 max_weight 参数列表不相同
    cout << d.max_weight(1) << endl; // error: request for member ‘max_weight’ is ambiguous
    return 0;
}
```

C++在解析函数调用时分为两步：

1. **名字查找阶段**：先在当前类及其所有基类中查找所有同名函数（**不考虑参数匹配**）。
2. **重载决议阶段**：在找到的函数集中选择最匹配的版本。

**关键点**：

- 如果名字查找阶段在多个基类中找到同名函数（即使参数不同），编译器会**直接报告歧义**，**不会进入重载决议阶段**。
- 这是为了防止意外的函数隐藏（accidental hiding），确保程序员必须明确指定意图。

**为什么不允许自动选择最匹配的版本？**

​	C++标准明确禁止这种行为，原因包括：

1. **避免意外行为**
   - 如果允许自动选择参数最匹配的版本，当未来基类新增同名函数时，可能 ***silently change*** 程序行为。
   - 例如：若 `B` 后来新增了 `max_weight(int)`，原本调用 `d.max_weight(1)` 的行为可能从 `C` 的版本悄无声息地切换到 `B` 的版本。

我们可以通过指明限定符来调用特定的版本，但最好的做法是在派生类中为该函数定义一个型版本：

``` c++
struct B {
    int max_weight() { return 0; }
};

struct B1 : public B {};

struct C {
    int max_weight(int x) { return x; }
};

struct D : public B1, public C {
    // 定义D自己的版本，从而隐藏直接基类中的同名函数
    int max_weight(int x) {
        return max(B1::max_weight(), C::max_weight(x));
    }
};

int main()
{
    D d;
    cout << d.max_weight(-1) << endl; // 0
    return 0;
}
```

##### 3. 虚继承与虚基类

尽管在派生列表中同一个基类只能出现一次，但实际上派生类可以多次继承同一个类。即，派生类可以通过它的多个直接基类分别继承同一个间接基类，也可以直接继承某个基类，然后再间接继承该类。

``` C++
struct B { int val; };
struct B1 : public B {};
struct B2 : public B {};
struct D : public B1, public B2 {};

int main()
{
    D d;
    // 这里通过 B1 和 B2 继承了两份 B，因此 D 中包含两个 int 变量
    cout << sizeof(d) << endl; // 8
    return 0;
}
```

基类每出现一次就意味着派生类中有一份它的拷贝，这显然不是我们想要的。除了空间上的浪费外，还会导致二义性等比较严重的问题，因此我们一般称在派生类中有多份拷贝的基类为<font color=blue>**“二义基类”**</font>。例如使用该基类的成员，显然会导致二义性错误，还有一些不容易察觉的错误例如将一个派生类对象传给二义基类的指针或引用：

``` C++
struct B { int val = 10; };
struct B1 : public B {};
struct B2 : public B {};

struct D : public B1, public B2 {
    void func() {
        // cout << val << endl; // error: reference to ‘val’ is ambiguous
        cout << B1::val << endl; // 10
        cout << B2::val << endl; // 10
    }
};

int main()
{
    B *b = new D(); // error: ‘B’ is an ambiguous base of ‘D’
    return 0;
}
```

在上面的例子中，由于 `D` 中包含基类 `B` 的两份拷贝，所以这里在使用 `B *b = new D()` 创建派生类对象时，编译器不知道这里的 `B` 到底是派生类 `D` 所继承而来的两个基类 `B` 中的哪一个。

C++ 语言中通过**虚继承（virtual inheritance）**机制解决基类被多次继承的问题。<font color=blue>虚继承的目的是令某个类做出说明，承诺愿意**共享它的基类**。其中，共享的基类子对象称为**虚基类**。</font>在这种机制下，无论虚基类在继承体系中出现多少次，在派生类都只包含唯一一个共享的虚基类子对象。

我们指定虚基类的方式是在派生列表中添加关键字 `virtual`，表示该基类是 `virtual` 继承。

通常，为了保证虚基类在派生类中只继承一次，应当在该基类的所有直接派生类中声明为虚基类。否则仍然会出现对基类的多次继承。例如：

``` c++
class A {};
class B : public virtual A {};
class C : public virtual A {};
class D : public A {};
class E : protected B, public C, private D {}; 
```

在上面的例子中，虽然我们在 `B` 和 `C` 中虚继承 `A`，但是在 `D` 中我们不是虚继承的，因此当 `E` 直接继承 `B,C,D` 时 `B,C` 共享一份 `A` 的成员，`D` 单独保留一份 `A` 的成员。

注意，如果此时 `E` 继承自 `A,B,C,D`，那么 `E` 中仍然会包含两份基类 `A`：

* 其中一份来自于直接基类 `A`
* 另一份来自于 `B,C,D` 的共享直接基类

另外就是虚继承有一个不太直观的特征：必须在虚派生的真实需求出现前就已经完成虚派生的操作。简而言之，我们无法预知未来，因此我们实际上并不清楚未来是否有虚继承的需求。例如我们有：

``` C++
struct B {};
struct B1 : B {};
struct B2 : B {};
struct D : B1, B2 {};
```

只有当定义 `D` 时才有对虚继承的需求，但如果 `B1` 和 `B2` 不是从 `B` 虚继承得到的，那么 `D` 的设计者就显得不太幸运了！

不过在实际的编程过程中，位于中间层次的基类将其继承声明为虚继承一般不会带来什么问题。通常情况下，使用虚继承的类层次一般是由一个人或一个项目组一次性设计完成的。对于一个独立开发的类来说，很少需要基类中的某一个是虚基类，况且新基类的开发者也无法改变已存在的类体系。

最后，虚基类是相当于虚继承的派生类而言的。即，对于虚继承的派生类而言，虚继承的基类是虚基类；但如果这个基类也被其他类普通继承，那么它就不是虚基类了。

最后，让我们分析一下虚继承下类的大小：

``` C++
struct A {};
struct B : virtual A {};
struct C : virtual A {};
struct D : A {};
struct E : B, C, D {};

int main()
{
    cout << sizeof(A) << endl; // 1
    cout << sizeof(B) << endl; // 8
    cout << sizeof(C) << endl; // 8
    cout << sizeof(D) << endl; // 1
    cout << sizeof(E) << endl; // 24
    return 0;
}
```

`sizeof(A) = 1`

- 空类（没有成员变量）的大小至少为 1 字节，这是为了确保每个实例有唯一的地址。

`sizeof(B) = 8` 和 `sizeof(C) = 8`

- 虚继承会引入一个 **虚基类指针（vbptr）**，通常是一个指针大小（64位系统下为8字节）。

`sizeof(D) = 1`

- 普通继承空类不会增加大小（Empty Base Optimization）
- 因为 D 只是简单地继承 A，没有虚继承的开销

`sizeof(E) = 24`

- 这个结果可能因编译器和平台而异 64 位系统，`gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0` 环境下，我们测试一下每个成员的偏移量：

  ``` shell
  g++ -g 1.cpp -o app 
  pahole -C E app
  ```

  输出：

  ``` cpp
  struct E : B, C, D {
          /* struct B                   <ancestor>; */     /*     0     8 */
          /* struct C                   <ancestor>; */     /*     8     8 */
  
          /* XXX 65520 bytes hole, try to pack */
  
          /* struct D                   <ancestor>; */     /*     0     1 */
  
          /* XXX last struct has 1 byte of padding */
          void ~E(struct E *, int, const void  * *);
  
          void E(struct E *, int, const void  * *, );
  
          void E(struct E *, int, const void  * *, const struct E  &);
  
          void E(struct E *, int, const void  * *);
  
  
          /* size: 24, cachelines: 1, members: 3 */
          /* padding: 23 */
          /* paddings: 1, sum paddings: 1 */
          /* last cacheline: 24 bytes */
  
          /* BRAIN FART ALERT! 24 bytes != 0 (member bytes) + 0 (member bits) + 65520 (byte holes) + 0 (bit holes), diff = -524152 bits */
  };
  ```

  由此可得 `E` 的内存布局为：

  ``` cpp
  | B 的虚基类指针   | 8 字节
  | C 的虚基类指针   | 8 字节 
  | D 的子对象      | 1 字节
  | 填充           | 7 字节（对齐到 8 字节）
  ```

  我们可以再测试一下不是虚继承环境下类的大小：

  ``` cpp
  struct A {};
  struct B {};
  struct C { A a; B b; };
  struct D : A, B {};
  
  int main()
  {
      cout << sizeof(A) << endl; // 1
      cout << sizeof(B) << endl; // 1
      cout << sizeof(C) << endl; // 2
      cout << sizeof(D) << endl; // 1
      return 0;
  }
  ```

#### 3.4 构造函数和虚继承

##### 1. 虚基类右最底层的派生类初始化

在普通的继承体系中，我们只能在派生类的构造函数中初始化其**直接基类**。<font color=blue>但在虚继承体系中，为了避免虚基类被重复初始化，我们规定**虚基类应该由最底层的派生类初始化**。</font>例如：`B1` 和 `B2` 都虚继承自 `B`，`A` 继承自 `B1` 和 `B2`，如果我们将虚基类 `B` 的初始化工作交给 `A` 的直接基类 `B1` 和 `B2`，那么就会导致 `B` 重复初始化。因此：

* 只要我们能创建虚基类的派生类对象，该派生类的构造函数就必须初始化它的虚基类（因为该派生类可能是最底层的派生类）。
* 如果我们没有在最底层派生类显示初始化其虚基类，会调用虚基类的默认构造函数；如果虚基类没有默认构造函数，则代码发生错误

例如，在非虚继承体系下，我们不能在派生类中初始化间接基类：

``` C++
struct B {
    B() = default;
    B(int _b) : b(_b) {}
    int b;
};
struct B1 : B {
    B1() = default;
    B1(int _b, int _b1) : B(_b), b1(_b1) {}
    int b1;
};
struct B2 : B {
    B2() = default;
    B2(int _b, int _b2) : B(_b), b2(_b2) {}
    int b2;
};
struct A : B1, B2 {
    A(int _b, int _a) 
        : B(_b), a(_a) {} // error: type ‘B’ is not a direct base of ‘A’
    int a;
};
```

在虚继承体系下可以：

``` c++
struct B {
    B(int _val=-1) :val(_val) {puts("B");}
    int val;
};

struct B1 : virtual B {
    B1() : B(1) {puts("B1");}
};

struct B2 : virtual B {
    B2() : B(2) {puts("B2");}
};

struct C : B1, B2 {
    C() : B(1), B1(), B2() {puts("C");}
};

int main() 
{
    C c;
    cout << c.val << endl;
    return 0;
}
// B
// B1
// B2
// C
// 1
```

在上面的代码中，在 `C` 中我们使用其直接基类 `B1` 和 `B2` 的构造函数来完成初始化，看起来好像我们会在 `B1` 和 `B2` 中分别对 `B` 调用构造函数 `B(1)` 和 `B(2)`，但其实这两个构造函数都不会被调用。

由于我们在 `C` 中显式指定了虚基类 `B` 的构造函数，所以我们会在 `C` 中调用 `B` 的构造函数从而完成 `B` 的初始化工作。然后再调用 `B1` 和 `B2` 的构造函数时，由于 `B` 已经完成了初始化，此时便不会再调用 `B` 的构造函数。

##### 2. 含有虚基类的对象的构造方式

含有虚基类的对象的构造顺序与一般的顺序稍有不同：

* 首先使用提供给最底层派生类构造函数的初始值初始化该对象的虚基类子部分
* 接下来直接按照基类在派生列表中出现的次序依次对其进行初始化。

这意味着，**虚基类总是先于非虚基类构造，与他们在基成体系中的次序和位置无关。**

> 注意，如果虚基类还继承自某些基类，那么虚基类的构造函数还是先调用其直接基类的构造函数。

这样可以保证虚基类只会被初始化一次，从上一节最后的例子可以看出来：在 `C` 的初始化工作中，最先初始化虚基类 `B`，然后才是其直接基类 `B1` 和 `B2`。

##### 3. 构造函数与析构函数的次序

**一个类可以有多个虚基类。**此时，这些虚的子对象按照它们在派生列表中出现的顺序从左到右依次构造。接下来就是直接按照基类在派生列表中出现的次序依次对其初始化了。

和往常一样，对象的析构顺序和构造顺序正好相反。

## 十三、特殊工具与技术

C++ 的语言设计者希望它能够处理各种各样的问题。因此，C++ 的某些特征对于一些特殊的应用非常重要，而在另外一些情况下没什么作用。

### 1. 控制内存分配

#### 1.1 new 和 delete 的底层原理

尽管我们说能够重载 `new` 和 `delete` 表达式，但是实际上重载这两个运算符与重载其它运算符的过程大不相同。我们首先需要知道一条 `new` 或 `delete` 表达式实际上做了什么事情：

``` c++
// T *p = new T(args);
void *mem = operator new(sizeof T); // 1.分配内存
mem->T(args);					    // 2.构造对象
T *p = static_cast<T*>(mem);		// 3.返回指针
// delete p;
p->~T();							// 1.销毁对象
operator delete(p);				    // 2.释放内存
```

由此可见，`new` 和 `delete` 对内存的管理实际上在底层是依赖于 `operator new` 和 `operator delete` 两个低级函数。当编译器发现一条 `new` 表达式或 `delete` 表达式后，会在程序中查找可调用的 `operator new` 或 `operator delete`。我们也可以通过作用域运算符来显式指定我们想要调用的 `operator new` 或 `operator delete` 函数版本（类版本或者全局版本）。

当我们将 `operator new` 或 `operator delete` 定义为类的成员函数时，他们是<font color=blue>**隐式静态**</font>的，而且他们不能操作类的任何数据成员。

* `operator new` 发生在构造函数之前，此时对象还未初始化
* `operator delete` 发生在析构函数之后，此时对象已被销毁

也正以为这两点，`operator new` 和 `operator delete` 也只能是静态函数，因为我们无法通过类对象来调用这两个函数。

如果我们想要重载 `new` 和 `delete` 以实现对内存分配和释放的管理，需要重载 `operator new` 和` operator delete`。并且，对于某个重载版本，即使标准库中存在了这个函数的定义，编译器依然允许我们定义自己的版本，并且使用自定义的版本替换标准库定义的版本。

注意，本质上，我们并不能重载 `new` 和 `delete` 表达式，我们重载的是其内部调用的 `operator new` 和 `operator delete` 函数。事实上，我们根本无法自定义 `new` 表达式和 `delete` 表达式的行为。

由于在 C++ 中，手动内存管理几乎总是通过 `new` 和 `delete` 实现的，所以对 `operator new` 和 `operator delete` 的重载特别是全局作用于下，影响是深远的。

#### 1.2 动态内存管理低级函数接口

标准库定义了 `operator new` 函数和 `operator delete` 函数的 8 个重载版本。其中前 4 个可能抛出 `bad_alloc` 异常，后 4 个版本则不会抛出异常：

``` C++
void* operator new(size_t);
void* operator new[](size_t);
void operator delete(void*) noexcept;
void operator delete[](void*) noexcept;

void* operator new(size_t, nothrow_t&) noexcept;
void* operator new[](size_t, nothrow_t&) noexcept;
void operator delete(void*, nothrow_t&) noexcept;
void operator delete[](void*, nothrow_t&) noexcept;
```

> 当然现在肯定不止 8 个

用户程序可以自定义上面函数版本中的任意一个，前提是自定义的版本必须位于全局作用域或类作用域中。

`operator new` 和 `operator delete` 归根结底还是使用 C 库函数 `malloc` 和 `free` 来实现内存的分配和释放的。他们定义在 `cstdlib` 头文件中。

* `malloc` 函数接受一个表示待分配字节数的 `size_t`，返回指向分配内存的指针或 0 表示分配失败。
* `free` 函数接受一个 `void*`，它是 `malloc` 返回的指针的副本，`free` 将相关内存返回给系统。

##### 1. operator new & placement new

对于 `operator new` 函数或者 `operator new[]` 函数来说，它的返回类型必须是 `void*`，第一个形参的类型必须是 `size_t` 且该形参**不能含有默认实参**。当我们调用这两个函数时，编译器会（自动）把存储指定类型对象或数组所需的字节数传给 `size_t` 参数。

如果我们想要自定义 `operator new` 函数，可以为它提供额外的形参。此时，用到这些自定义函数的 `new` 表达式必须使用 `new` 的定位形式将实参传递给新增的形参。

**定位形式（placement syntax）** 指的是 `new` 表达式的语法是：

```cpp
new (arg1, arg2, ...) Type(args...);
```

这里括号里的 `arg1, arg2, ...` 是传给你自定义的 `operator new` 的**额外参数**，而不是构造函数的参数。构造函数参数是写在 `Type(args...)` 部分的。

尽管我们在一般情况下可以自定义具有任何形参的 `operator new`，但下面这个函数却无论如何不能被用户重载：

``` C++
void *operator new(size_t, void*); 
```

这种形式只供标准库使用，不能被用户重新定义。事实上，这种形式的 `new` 又称为 `placement new`，`placement` 指的是对象构造的位置，该函数用来在特定的位置上“放置”对象。使用 `placement new`，我们可以在已经分配好的内存中调用对象的构造函数，这样可以避免额外的内存分配开销，直接在指定的位置上构造对象。

所以说，`placement new` 区别于其它` operator new`，它用来构造对象而不是分配内存。

区分几个概念：

| 表达式形式                   | 用法                                | 说明                                      |
| ---------------------------- | ----------------------------------- | ----------------------------------------- |
| `new Type`                   | 默认 `operator new`                 | 最常见的形式                              |
| `new (ptr) Type`             | **标准 placement new**              | 在指定地址 `ptr` 上构造对象               |
| `new (arg1, arg2, ...) Type` | **自定义 operator new（定位形式）** | 用于传递额外参数给自定义的 `operator new` |

例如：

```cpp
#include <iostream>
#include <cstdlib>

void* operator new(std::size_t size, const char* tag) {
    std::cout << "Custom operator new called with tag: " << tag << ", size: " << size << std::endl;
    return std::malloc(size);
}

struct MyClass {
    MyClass() {
        std::cout << "MyClass constructor\n";
    }
};

int main() {
    // 使用定位形式调用自定义 operator new
    MyClass* obj = new ("[MyTag]") MyClass;

    // 别忘了释放内存（这里我们用 malloc，所以也要用 free）
    // 事实上 delete 底层也是用的 free
    std::free(obj);

    return 0;
}
```

总而言之，定位形式（placement syntax）的 `new` 就是你在 `new` 后面加一对括号并传实参，用来匹配你自己定义的带额外参数的 `operator new` 函数。

##### 2. operator delete

* `operator delete` 和 `operator delete[]` 的返回类型必须是 `void`。第一个形参的类型必须是 `void*`。
* 编译器用指向待释放内存的指针来初始化 `void*` 参数。
* 和类的析构函数一样，涉及内存释放的操作不应该返回异常，因此重载 `operator delete` 应该在可以时添加 `noexcept`。
* `free` 一个空指针即 `free(0)` 是安全地，但也没有任何意义，它仅仅是为了编程上的方便和统一。

``` C++
void *operator new(size_t size) 
{
    puts("call operator new");
    if(void *mem = malloc(size))
        return mem;
    throw bad_alloc();
}

void operator delete(void *mem) noexcept 
{
    puts("operator delete");
    free(mem);
}
```

#### 1.3 operator delete 作为类成员函数

当将 `operator delete` 或 `operator delete[]` 定义为类的成员函数时，可以选择为其添加一个额外的 `size_t` 参数。

- 这个参数表示第一个参数（指针）所指向对象的字节数。

##### **1. 继承体系中的应用**：

在继承体系中，如果基类有虚析构函数，通过基类指针删除派生类对象时：

1. 会调用派生类的析构函数（多态行为）
2. 然后会调用适当的 `operator delete` 函数

此时，`size_t` 参数的值会根据对象的实际（动态）类型而变化，反映派生类对象的真实大小。

##### 2. **动态分派机制**：

- 与虚析构函数类似，`operator delete` 也会根据对象的动态类型被调用。
- 这意味着即使通过基类指针删除对象，也会调用派生类的 `operator delete`（如果有）。

示例说明：

```cpp
class Base {
public:
    virtual ~Base() {}  // 虚析构函数是关键
    static void operator delete(void* ptr, size_t size) {
        std::cout << "Deleting Base, size = " << size << "\n";
        ::operator delete(ptr);
    }
};

class Derived : public Base {
    // vptr: 8B
    int extra_data[10]; // 40B
public:
    static void operator delete(void* ptr, size_t size) {
        std::cout << "Deleting Derived, size = " << size << "\n";
        ::operator delete(ptr);
    }
};

int main() {
    Base* p = new Derived;
    delete p;  // 会调用 Derived::operator delete 并传入 sizeof(Derived)
}
```

输出可能类似于：

```
Deleting Derived, size = 48
```

有意思的一点是，虽然我们可以为 `operator delete` 指定带额外参数的重载版本，但编译器不会调用也无法调用该版本：

``` C++
void* operator new(size_t size, string s)
{
    cout << "call operator new: " << s << endl;
    if(void *mem = malloc(size))
        return mem;
    throw bad_alloc();
}

void operator delete(void *mem, string s) noexcept 
{
    cout << "call operator delete: " << s << endl;
    free(mem);
}

int main()
{
    int *p2 = new("int") int(1024);
    cout << *p2 << endl;
    delete("int") p2; // error: expected ‘;’ before ‘p2’
    return 0;
}
```

这可能是以下几方面导致的：

1. 避免运行时开销：如果 `delete` 要支持带参数的 `operator delete`，则编译器需要在构造时保存额外的参数信息，并在析构时解析并传递这些参数，这样会带来额外的内存开销和复杂的编译期逻辑。而 C++ 语言设计上是非常注重性能的，尤其在低级内存管理方面，所以简化 `delete` 以仅支持无参数的 `operator delete` 可以避免额外的运行时开销。
2. 确保 `delete` 操作的设计保持简单：在 C++ 中，`operator new` 可以支持 `placement new` 或其他带参数的形式，为特定用途（如内存池、特定地址分配等）提供灵活性。但对于 `delete` 操作，C++ 设计更倾向于保持简单，减少依赖，开发者可以通过自定义释放函数或其他内存管理策略来满足更复杂的需求。例如：智能指针与自定义删除器，自定义释放函数等。

#### 1.4 placement new

> 注意这里所说的 `placement new` 指的是用于构造对象的版本。除此之外，自定义的带额外参数的 `operator new` 也可以说是 `placement new`。

`placement new` 用于在**已分配**的内存上**构造对象**，它允许我们手动指定对象的内存地址，从而在特定的内存区域上直接构造对象。它通常用于内存池、嵌入式编程或性能敏感的代码中。

尽管 `operator new` 和 `operator delete` 函数一般用于 `new` 和 `delete` 表达式的底层调用，但他们毕竟是标准库的两个普通函数，因此普通的代码也可以直接的调用它们。

在 C++ 的早期版本中，`allocator` 类还不是标准库的一部分。应用程序如果想把内存分配与初始化分离开来的话，需要调用 `operator new` 和 `operator delete`。这两个函数的行为与 `allocator` 的 `allocate` 成员和 `deallocate` 成员非常类似，它们负责分配或释放空间，但是不会构造或销毁对象。

与 `allocator` 不同的是，对于 `operator new` 分配的空间我们无法使用 `construct`  函数构造对象。相反，我们应该使用 `new` 的**定位 new（`placement new`）**形式构造对象。我们可以使用 `placement new`  传递一个地址，此时 `placement new` 的形式如下：

``` C++
// operator new(size,void*);
new (place_address) type
new (place_address) type (initializers)
new (place_address) type [size]
new (place_address) type [size]
new (place_address) type [size] {braced initializer list}
```

其中 `place_address` 必须是一个指针（地址），同时在 `initializer` 中提供一个（可能为空的）以逗号分隔的初始值列表，用于构造新分配的对象。

当仅通过一个地址值调用时，`placement new` 使用 `operator new(size_t,void*)` 构造对象。这是一个我们无法自定义的 `placement new` 版本。该函数不分配任何内存，他只是简单的返回指针实参；然后由 `new` 表达式负责在给定的地址初始化对象以完成整个工作。<font color=blue>事实上，`placement new`允许我们在一个**特定的**、**预先分配**的内存地址上**构造对象**。</font>

尽管在很多时候使用 `placement new` 与 `allocator` 的 `construct` 成员非常类似，但它们之间也有一个重要的区别。我们传给 `construct` 的指针必须指向同一个 `allocator` 对象分配的空间，但是传给 `placement new` 的指针无需指向 `operator new` 分配的内存。事实上，这块内存也不必是动态分配在堆上的内存，它也可以是栈上的内存，只要是一块“有效的内存”即可。

``` c++
int val;
new(&val) int(1024);
cout << val << endl; //1024

string s;
new(&s) string("hello");
cout << s << endl; //hello
s.~string();
new(&s) string("world"); // 甚至还可以在析构后重新狗仔
cout << s << endl; //world

int *p = new int;
new(p) int(16);
cout << *p << endl; //16
new(p) int(256);	// 不析构直接构造
cout << *p << endl; //256
```

### 2. 运行时类型识别

**运行时类型识别（run-time type identification，RTTI）**的功能由两个运算符实现：

* `typeid` 运算符，用于返回表达式的类型
* `dynamic_cast` 运算符，用于将基类的指针或引用安全地转换成派生类的指针或引用

这两个运算符特别适用于以下情况：**我们想使用基类的指针或引用执行某个派生类操作而且该操作不是虚函数。**因为一般来说，当我们通过指针或引用所调用的函数不是虚函数时，是不会执行动态绑定的。此时，我们可以使用 RTTI 运算符。

#### 2.1 dynamic_cast 运算符

``` c++
// type必须是一个类类型，而且在通常情况下该类型应该含有虚函数
dynamic_cast<type*>(e);
dynamic_cast<type&>(e);
dynamic_cast<type&&>(e);
```

在上面的所有形式中，`e` 的类型必须符合以下三个条件中的任意一个：

1. `e` 的类型是目标 `type` 的公有派生类
2. `e` 的类型是目标 `type` 的公有基类
3. `e` 的类型是目标 `type` 的类型

如果类型符合，则可以转换成功；否则，转换失败。

* 如果一条 `dynamic_cast` 语句的转换目标是指针并且转换失败了，则结果为 `NULL`；
* 如果转换目标是引用类型并且失败了，则 `dynamic_cast` 运算符将抛出一个 `bad_cast` 异常。

##### 1. 指针类型的 dynamic_cast

**将一个基类对象提升为派生类对象是一个合理的需求**，因为不是所有的派生类都包含其专属的数据成员，也有可能仅仅比派生类多了一些函数功能接口。

假定 `Base` 类至少含有一个虚函数，`Derived` 是 `Base` 的公有派生类。如果有一个指向 `Base` 的指针 `bp`，则我们可以在运行时将它转换成指向 `Derived` 的指针：

``` C++
if(Derived *dp = dynamic_cast<Derived*>(bp)) {
    // 转换成功,使用dp指向的Derived对象
}
else {
    // 转换失败,使用bp指向的Base对象
}
```

> 我们可以对一个空指针执行 `dynamic_cast`，结果是所需类型的空指针。

在代码中，我们在条件部分定义了派生类对象指针 `dp`，这么做有两个好处：

1. 在一个操作中同时完成类型转换和条件检查两项任务。
2. 指针 `dp` 在 `if` 语句之外是不可访问的。这意味着，一旦转换失败，即使后续的代码忘了做相应判断，也不会接触到这个未绑定的指针，从而确保程序是安全地。

具体的例子：

``` cpp
struct Base {
    virtual void vfunc() { cout << "Base::vfunc" << endl; }
    void f() { cout << "Base::f" << endl; } 
    void g() { cout << "Base::g" << endl; }
};

struct Derived : Base {
    virtual void vfunc() override { cout << "Derived::vfunc" << endl; }
    // void f() 会隐藏 Base 的 f()
    // void g() 会继承而来
    void f() { cout << "Derived::f" << endl; } 
    void h() { cout << "Derived::h" << endl; } 
};

int main()
{
    Base *p = new Derived();
    if(Derived *dp = dynamic_cast<Derived*>(p)) {
        dp->vfunc();
        dp->f();
        dp->g();
        dp->h();
    }  
    else {
        cout << "dynamic_case fault" << endl;
    }
    return 0;
}
```

我们可以将基类的指针转换为指向派生类的指针，但前提是基类的指针要指向派生类对象。如果这里我们令 `Base *p = new Base()`，那么下面的 `if-else` 就会进入 `else`。

##### 2. 引用类型的 dynamic_cast

引用类型的 `dynamic_cast` 与指针类型的 `dynamic_cast` 在表示错误发生的方式上略有不同。这是因为指针和引用对于 `NULL` 这一概念的处理不同，因为不存在所谓的空引用，所以对于引用类型来说无法使用与指针类型完全相同的错误报告策略。当对引用的类型转换失败时，程序抛出一个名为 `std::bad_cast` 的异常，该异常定义在 `typeinfo` 头文件中。

``` c++
void f(const Base &b)
{
    try {
        const Derived &d = dynamic_cast<const Derived&>(b);
    } 
    catch(bad_cast e) {
        cout << e.what() << endl;
    }
}
```

#### 2.2 typeid 运算符

为 **RTTI** 提供的第二个运算符是 `typeid` 运算符，它允许向程序表达式提问：你的对象是什么类型。

`typeid` 表达式的形式是 `typeid(e)`，其中 `e` 可以是任意表达式或类型的名字。

* `typeid` 操作的结果是一个**常量对象的<font color=blue>引用</font>（`type_info&`）**，该对象的类型是标准库类型 `type_info`  或 `type_info` 的公有派生类型。
* `type_info` 类定义在 `typeinfo` 头文件中。

`typeid` 运算符可以作用于任意类型的表达式，和往常一样，顶层 `const` 会被忽略。如果表达式是一个引用，则 `typeid` 返回该引用索引对象的类型。值得注意的是，`typeid` 作用域数组或指针时，并不会执行向指针的标准类型转换。也就是说，对于数组类型，则所得结果是数组类型而非指针类型：

``` C++
int arr[3];
cout << typeid(arr).name() << endl; // A3_i
```

当运算对象不属于类类型或者是一个不包含任何虚函数的类时，`typeid` 运算符的结果是对象的静态类型。而当运算对象是定义了至少一个虚函数的类的左值时，`typeid` 的结果直到运行时才会求得：

``` C++
struct Base {
    virtual ~Base() {}
};

struct Derived : public Base {
    virtual ~Derived() {}
};

#define print_typename(x) cout << typeid(x).name() << endl;

int main()
{
    Base *p = new Derived();
    print_typename(*p); //7Derived
    return 0;
}
```

**`typeid` 是否需要运行时检查类型决定了表达式是否会被求值。**只有当类函数含有虚函数时编译器才会对表达式求值；反之，如果类型不含有虚函数，则 `typeid` 运算符返回表达式的静态类型，对于静态类型编译器不需要求值也能得到。

运行时检查类型需要求值的特性意味着，如果我们使用了 `typeid(*p)`，需要确保 `*p` 是合法的。如果 `p` 是一个空指针，则 `typeid(*p)` 将抛出一个名为 `bad_typeid` 的异常：

``` C++
struct Base {
    Base(int _val = 0) : val(_val) {}
    virtual ~Base() {}
    int val;
};

struct Derived : public Base {
    Derived(int _val = 0) : Base(_val) {} 
    virtual ~Derived() {}
};

#define print_typename(x) cout << typeid(x).name() << endl;

int main()
{
    Base *p = nullptr;
    print_typename(*p); //terminate called after throwing an instance of 'std::bad_typeid'
    return 0;
}
```

虽然我们可以通过 `typeid` 运算符来获取某一对象的类型，但是大多数情况下获取的类型信息比较“晦涩”，看起来不直观。更多的情况是使用 `typeid` 运算符来判断两条表达式的类型是否相同或一条表达式的类型是否与指定类型相同。

#### 2.3 type\_info 类

`type_info` 类的精确定义随着编译器的不同而略有差异。不过，C++ 标准规定 `type_info` 类必须定义在 `typeinfo` 头文件中，并且至少提供以下操作：

``` C++
t1 == t2; 	  //如果type_info对象t1和t2表示同一种类型，返回true；
t1 != t2;     //否则返回false
t.name();     //返回一个C风格字符串,表示类型名字的可打印形式，名字生成的规则因系统而异
t1.before(t2) //返回一个bool值，表示t1是否在t2之前，所采用的顺序关系依赖于编译器
```

除此之外，因为 `type_info` 类一般是作为一个**基类**出现，所以他还应该提供一个虚析构函数。

* 当编译器希望提供额外类型信息的时候，通常在 `type_info` 的派生类中完成。

`type_info` 类没有默认构造函数，而且它的拷贝构造函数和移动构造函数以及赋值运算符都被定义成删除的。因此，我们无法定义或拷贝 `type_info` 类型的对象，更也不能为其赋值。

* 创建 `type_info` 对象的唯一途径是使用 `typeid` 运算符，`typeid` 运算符会返回一个 `type_info&`，我们可以用引用的形式保存这个对象。

```` C++
struct Base {
    Base(int _val = 0) : val(_val) {}
    virtual ~Base() {}
    int val;
};

struct Derived : public Base {
    Derived(int _val = 0) : Base(_val) {} 
    virtual ~Derived() {}
};

#define print_typename(x) cout << typeid(x).name() << endl;

int main()
{
    Base *p = new Derived;
    print_typename(*p);           // 7Derived
    print_typename(p);            // P4Base
    print_typename(char);         // c
    print_typename(short);        // s
    print_typename(int);          // i
    print_typename(unsigned int); // j
    print_typename(double);       // d
    print_typename(long long);    // x
    print_typename(string);       // NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE
    print_typename(int*);         // Pi
    return 0;
}
````

`type_info` 类在不同的编译器上有所区别。有的编译器提供了额外的成员函数以提供程序中所用类型的额外信息。读者应该仔细阅读你所用编译器的使用手册，从而获取关于 `type_info` 的更多细节。

#### 2.4 使用 RTTI

在某些情况下 **RTTI** 非常有用，比如当我们想为具有继承关系的类实现相等运算符时：

* 对于两个对象来说，如果它们的类型相同并且对应的数据成员取值相同，则我们说这两个对象是相等的。
* 考虑到在类的继承体系中，每个派生类负责添加自己的数据成员，因此派生类的相等运算符必须把派生类的新成员考虑进来。

一种容易想到的解决方案是定义一套虚函数，令其在继承体系的各个层次上分别执行相等性判断。此时，我们可以为基类的引用定义一个相等运算符，该运算符将它的工作委托给虚函数 `equal`，由 `equal` 负责实际的操作。

遗憾的是，上述方案很难奏效。**虚函数的基类版本和派生类版本必须具有相同的形参类型。**如果我们想定义一个虚函数 `equal`，则该函数的形参必须是基类的引用。此时，`equal` 函数将只能使用基类的成员，而不能比较派生类独有的成员。

> 指向派生类的基类指针只能通过动态绑定调用派生类的虚函数，但是不能调用派生类的其它独有成员（成员变量和成员函数）。

要想实现真正有效的相等比较操作，我们需要首先清楚一个事实：即如果参与比较的两个对象类型不同，则比较结果为 `false`。例如，如果我们试图比较一个基类对象和一个派生类对象，显然相等运算符应该返回 `false`。基于上述推论，我们就可以使用 **RTTI** 解决问题了。我们定义的相等运算符的形参是基类的引用，然后使 `typeid` 检查两个运算对象的类型是否一致。如果运算对象的类型不一致，则返回 `false`；类型一致才调用 `equal` 函数。每个类定义的 `equal` 函数负责比较类型自己的成员。这些运算符接受 `Base&` 形参，但是在进行比较操作前先把运算对象转换成运算符所属的类类型。

``` C++
/*============================================*/
/*                   Base                     */
/*============================================*/
class Base {
    friend bool operator==(const Base&, const Base&);
public:
    Base(int _ival) : ival(_ival) {}
protected:
    virtual bool equal(const Base&) const;
    int ival;
};

bool operator==(const Base &lhs, const Base &rhs)
{
    return typeid(lhs) == typeid(rhs) && lhs.equal(rhs);
}

bool Base::equal(const Base& x) const
{
    return ival == x.ival;
}

/*================================================*/
/*                   Derived                      */
/*================================================*/
class Derived : public Base {
public:
    Derived(int _ival, string _sval) : Base(_ival), sval(_sval) {}
protected:
    bool equal(const Base&) const;
    string sval;
};

bool Derived::equal(const Base& x) const
{
    auto y = dynamic_cast<const Derived&>(x);
    return ival == y.ival && sval == y.sval;
}

#define println_is_equal(x, y) \
    cout << boolalpha << (x == y) << noboolalpha << endl;

int main()
{
    Base b1(10), b2(20), b3(10);
    Derived d1(10, "hello"), d2(10, "fuck"), d3(10, "hello");

    println_is_equal(b1, b3); // true
    println_is_equal(b2, b3); // false

    println_is_equal(d1, d3); // true
    println_is_equal(d2, d3); // false

    println_is_equal(b1, d1); // false 
    println_is_equal(b2, d1); // false

    return 0;
}
```

### 3. 枚举类型

枚举类型将一组整形常量组织在一起。和类一样，每个枚举类型定义了一种**新的类型**。枚举属于**字面值常量类型**。

C++ 包含两种枚举：限定作用域的（C++ 11引入）和不限定作用域的。

* 定义<font color=blue>**限定作用域**</font>的枚举类型需要在 `enum` 后面跟 `class`（或 `struct`），名字不能为空。
* 定义<font color=blue>**不限定作用域**</font>的枚举类型时忽略掉关键字 `class`（或 `struct`），枚举类型的名字是可选的。如果 `enum` 是未命名的，则我们只能在定义该 `enum` 时定义它的对象，我们可以将未命名的 `enum` 视为一种名字为 **unnamed** 的 `enum`。

#### 3.1 枚举的定义与访问

和 `class` 一样，具名 `enum` 也定了新的类型。

* 只要 `enum` 有名字，我们就能定义并初始化该类型的成员。
* 想要初始化 `enum` 对象或为 `enum` 对象赋值，必须使用该类的一个枚举成员或者该类型的另一个对象。不同类型之间不可以互相赋值：

``` C++
enum color {BLUE, RED};     // color枚举
enum SIX   {MAN, WOMAN};	// six枚举
enum       {TRUE, FALSE}; 	// unnamed枚举

color c1 = MAN;     // error: cannot convert ‘SIX’ to ‘color’ in initialization
color c2 = TRUE;    // error: cannot convert ‘<unnamed enum>’ to ‘color’ in initialization
color c3 = BLUE;
color c4 = c3;
color c5 = 0;       // error: invalid conversion from ‘int’ to ‘color’
```

一个**<font color=blue>不限定作用域</font>的枚举类型的对象或枚举成员**自动的转换为整形。因此，我们可以在任何需要整型值的地方使用它们：

``` c++
int v1 = RED;      // 正确：不限定作用于的枚举类型的枚举成员隐式的转换为int
int v2 = SIX::MAN; // 错误：限定作用域的枚举类型不会进行隐式转换
                   // error: cannot convert ‘SIX’ to ‘int’ in initialization
int v3 = static_cast<int>(SIX::MAN); // 正确：强制类型转化
color c = RED;
int v4 = c;
```

当我们将一个枚举成员传给整形参数时，`enum` 的值会被提升为 `int` 或更大的整形，实际提升的结果由枚举类型的<font color=blue>**潜在类型**</font>决定：

``` C++
enum Bool : bool          {TRUE, FALSE};
enum Color                {RED, BLUE, GREEN};
enum Vals : unsigned char {V1, V2, V3};

void f(int x) 				 {puts("int");}
void f(unsigned char c) 	 {puts("uchar");}
void f(unsigned long long l) {puts("llong");}

int main()
{
    f(Bool::TRUE); // int 
    f(Color::RED); // int 
    f(Vals::V1);   // uchar
    return 0;
}
```

同样的，由于枚举是一种类型，我们也可以通过作用域运算符来访问枚举的成员：

* 对于限定了作用域的枚举类型，我们只能通过作用域运算符来访问其枚举成员。
* 对于未限定作用域的枚举类型，我们既可以直接访问，也可以通过作用域运算符来访问。

``` c++
enum color {BLUE, RED};
enum class Bool {TRUE, FALSE};

color c1 = color::BLUE;
color c2 = RED;
Bool flag1 = Bool::TRUE;
Bool flag2 = TRUE; //identifier "TRUE" is undefined
```

#### 3.2 枚举成员的值

默认情况下，枚举值是从 0 开始，依次加 1，不过我们也能为一个或多个枚举成员指定专门的值。值得注意的是，**枚举值不一定唯一**。如果我们只为部分枚举成员指定了枚举值，那么未指定的枚举成员的值是前一个成员的值加一：

``` c++
enum {
    A, B, C, D = 10, E = 10, F, G = 30, H, U = 100,
    // 0 1 2 10 10 11 30 31 100
};
```

枚举成员是 `const`，因此在初始化枚举成员时提供的值必须是**常量表达式**。这也意味着，我们可以将枚举成员用到任何需要常量表达式的地方。事实上，这也是枚举的目的。

#### 3.3 enum 成员的类型

尽管每个 `enum` 都定义了唯一的类型，但实际上 `enum` 是由某种整数类型表示的。在 C++11 新标准中，我们可以在 `enum` 的名字后加上冒号以及我们想在该 `enum` 中使用的类型：

``` c++
enum intVals : unsigned long long {
    char_max = 255,
    short_max = 65536,
    int_max = 4294967295UL,
};
```

> 在这个代码中，`int_max` 的赋值确实需要指定 `UL` 后缀，以确保值 `4294967295` 被解析为 **无符号长整数（`unsigned long long`）** 类型。
>
> 虽然在大多数编译器中，`4294967295` 即使不加后缀也会被解析正确（因为 `unsigned long long` 足够大），但为了确保代码的 **可读性** 和 **移植性**，明确地添加 `UL` 后缀是个好习惯。

如果我们没有指定 `enum` 的**潜在类型**（枚举成员的类型），则默认情况下：

* 限定作用域的 `enum` 成员类型是 `int` 。
* 对于不限定作用于的枚举类型来说，其枚举成员不存在默认类型，我们只知道成员的潜在类型足够大，肯定能容纳枚举值。

如果我们指定了枚举成员的潜在类型，则一旦某个枚举成员的值超过了该类型所能容纳的范围，将引发程序错误。

``` C++
enum class IntVals { // 默认为 int 类型
    C = 2147483648 // error: enumerator value ‘2147483648’ is outside the range of underlying type ‘int’
};
```

对 `enum` 来说，`sizeof` 运算符返回的就是 `enum` 成员类型的大小，注意不同于指针，其大小不是 `sizeof(Type)* count`，而是 `sizeof(Type)`：

``` cpp
enum E1 : int { A, B, C };
enum E2 : long long { D, E };

int main()
{
    cout << sizeof(E1) << endl; // 4
    cout << sizeof(E2) << endl; // 8
    return 0;
}
```

#### 3.4 获取 enum 的底层类型

`std::underlying_type_t` 是 C++ 标准库提供的一个 **类型萃取（type trait）**，用于获取枚举类型（`enum` 或 `enum class`）的**底层类型（underlying type）**。

* 它是 C++14 引入的便捷别名（alias），等价于 `typename std::underlying_type<T>::type`。

为了方便复用，我们可以将对 `enum` 底层类型的判断封装成一个宏：

``` cpp
enum IntVals { 
    A = -1,
    C = 2147483648,
};

#define ASSERT_ENUM_UNDERLYING_TYPE(ENUM_TYPE, EXPECT_TYPE) \
    using ENUM_TYPE##UnderlyingType = std::underlying_type_t<ENUM_TYPE>; \
    static_assert(std::is_same_v<ENUM_TYPE##UnderlyingType, EXPECT_TYPE>, "#ENUM_TYPE is not match #EXPECT_TYPE");

int main()
{
    // using IntValsUnderlying = std::underlying_type_t<IntVals>;
    // static_assert(std::is_same_v<IntValsUnderlying, long>); 
    ASSERT_ENUM_UNDERLYING_TYPE(IntVals, long);
    return 0;
}
```

#### 3.5 枚举类型的前置声明

在 C++11 中，我们可以提前声明 `enum`。`enum` 的前置声明（无论隐式地还是显式的）必须指定其成员的大小（潜在类型）：

* 由于限定作用域的枚举类型的潜在类型默认为 `int`，因此我们可以不显式指定其潜在类型。
* 但未限定作用域的枚举类型由于没有潜在类型，因此我们必须显式指定。

``` C++
enum intVals : unsigned long long;
enum doubleVals;         //error: use of enum ‘doubleVals’ without previous declaration
enum class open_modes;
```

和其它声明语句一样，`enum` 的声明和定义必须匹配，这意味着 `enum` 的所有声明和定义需要保证其大小相同。而且，我们不能声明同名的限定作用域的 `enum` 类型以及不限定作用域的 `enum` 类型：

``` C++
enum intVals : int;
enum class intVals; // enum "intVals" (declared at line 8) was previously declared as a different kind of enum type
```

#### 3.6 为什么 enum 的前置声明必须指定类型

在 C++ 中，**`class` 的前置声明不需要指定大小，而 `enum` 的前置声明必须指定底层类型**，这一差异源于两者在内存管理和类型系统设计上的本质区别。以下是详细解释：

##### 1. **内存布局的确定性**

**`class` 的前置声明**

- **指针大小已知**：
  即使不知道类的完整定义，编译器也能确定指针的大小（如 `sizeof(MyClass*)` 在 64 位系统上恒为 8 字节）。

  ```cpp
  class MyClass; // 前置声明
  void foo(MyClass* obj); // 合法，指针大小与类定义无关
  ```

- **延迟布局计算**：
  类的内存布局（成员变量偏移、虚函数表等）可以等到完整定义时再计算，前置声明仅告知编译器“存在此类”。

**`enum` 的前置声明**

- **值类型直接存储**：
  枚举是值类型（非指针），其变量直接存储底层类型的值（如 `int`、`char`）。若未指定底层类型，编译器无法确定 `sizeof(Enum)`。

  ```cpp
  enum MyEnum;          // 错误：不知道底层类型，无法确定大小
  enum MyEnum : int;    // 正确：明确底层类型为 int
  ```

##### 2. **类型系统的差异**

**`class` 的不透明性**

- **允许不完整类型**：
  类的前置声明引入一个“不完整类型”（incomplete type），仅支持指针或引用操作（如 `MyClass*`），直到完整定义后才可实例化或访问成员。

  ```cpp
  class MyClass;
  MyClass* p ; // 合法
  MyClass obj; // 错误：不完整类型无法实例化
  ```

**`enum` 的完全性要求**

- **必须完整定义**：
  枚举是标量类型（scalar type），其值直接参与运算和存储。**前置声明时必须明确底层类型以保证编译时能确定其行为。**

  ```cpp
  enum Color : int; // 必须指定底层类型
  Color c = Color::Red; // 需要完整定义
  ```

##### 3. **编译器实现的限制**

**`class` 的灵活性**

- **符号表管理**：
  类的符号（如名称、继承关系）可在链接时解析，内存布局可延迟到定义时处理。编译器只需知道“存在此类”即可处理指针或引用。

**`enum` 的硬性要求**

- **立即数值解析**：
  枚举值（如 `RED = 0`）是编译时常量，必须立即确定其类型范围。若未指定底层类型，编译器无法判断 `enum { A = 255 }` 应该用 `char`（可能溢出）还是 `int`。

##### 4. **历史与标准演进**

**C++98 的传统 `enum`**

- 无前置声明机制，默认底层类型为 `int`，但不同编译器可能优化为更小的类型（如 `char`），导致跨平台问题。

**C++11 的改进**

- 引入显式底层类型（如 `enum : short`）和前置声明，解决可移植性问题。
- 强类型枚举（`enum class`）默认底层类型为 `int`，但仍需显式声明以保证一致性。

#### 3.7 命名冲突

##### 1. 传统 enum 的命名冲突问题

问题表现：

```cpp
enum Color { RED, GREEN, BLUE };   // error: ‘RED’ conflicts with a previous declaration
enum TrafficLight { RED, YELLOW, GREEN }; // error: ‘GREEN’ conflicts with a previous declaration
```

- **原因**：传统枚举的成员直接暴露在**外层作用域**，导致相同名称冲突。

##### 2. C++11 的解决方案：强类型枚举 (`enum class`)

```cpp
enum class Color { RED, GREEN, BLUE };
enum class TrafficLight { RED, YELLOW, GREEN }; // 合法
```

- **作用域隔离**：成员**必须**通过枚举类型名访问（如 `Color::RED`），彻底避免冲突。
- **类型安全**：不能隐式转换为整数，避免误用。

访问方式：

```cpp
auto c = Color::RED;      // 正确
auto t = TrafficLight::GREEN; // 正确
```

##### 3. **传统 `enum` 的临时解决方案**

**(1) 手动添加前缀（不推荐）**

```cpp
enum Color { COLOR_RED, COLOR_GREEN, COLOR_BLUE };
enum TrafficLight { LIGHT_RED, LIGHT_YELLOW, LIGHT_GREEN };
```

- **缺点**：代码冗长，维护成本高。

**(2) 嵌套在命名空间/类中**

```cpp
namespace Colors {
    enum Type { RED, GREEN, BLUE };
}
namespace Lights {
    enum Type { RED, YELLOW, GREEN };
}

auto c = Colors::RED;    // 通过命名空间隔离
auto t = Lights::GREEN;
```

* 有点类似于 C++11 引入的强枚举类型，只不过是通过 `namespace` 实现命名隔离的

#### 3.8 enum 是一个类型

在最后，我要再次重申，`enum` 是一个类型，它不是一个对象，因此，当我们写下如下代码时：

```cpp
class Foo{
    enum class Color { RED, GREEN, BLUE };
};
```

`enum class Color` 是一个类型成员。所以 `Foo` 的大小实际上为 `1`，即 `Foo` 是一个空类。

因此说，对 `enum` 声明为 `static` 的做法显然是错误的，因为它不是一个变量或函数。

### 4. 类成员指针

**成员指针（*point to number*）**是指可以指向类的**非静态数据成员**的指针。

* 一般情况下，指针指向一个对象，但是成员指针指示的是类的成员，而非类的对象。
* 类的静态数据成员不属于任何对象，因此无需特殊的指向静态成员的指针，指向静态成员的指针域普通指针没有什么区别。

**成员指针的类型囊括了类的类型和成员的类型**。

* 当初始化一个这样的指针时，我们令其指向类的某个成员，但是不指定该成员所属的对象；
* 直到使用成员指针时，才提供成员所属的对象。

一般来说，成员指针的类型为 `Type ClassName::*p = &ClassNane::member;`。

* 如果你觉得复杂，也可以直接使用 `auto`。

> 不过在我的机器上测试，显示 `auto` 无法推导？？

#### 4.1 成员指针的地址定位

正如我们前面提到过的，当我们初始化一个成员指针或为成员指针赋值时，该指针并没有指向任何内存中的对象：

``` c++
Type ClassName::*p = &ClassNane::member;
```

这条语句在为成员指针 `p` 赋初值之后，它只是保存了一个“偏移量”。即，成员指针的赋值只是保存了成员在类中的“相对位置”，只有当解引用成员指针时我们才提供对象的信息。

因此 C++ 中的**成员指针**并不直接指向内存地址，而是相对于类对象的一个<font color=blue>“偏移量”</font>。只有在指定了对象之后，成员指针才能通过该对象的具体地址（首地址）来定位到该对象的成员。

简而言之，不同于普通指针的直接指定内存地址来定位对象，成员指针的“内存地址定位”分为两个阶段：

1. 定位成员相对于类对象的“偏移量” ***offset***
2. 定位类对象的起始地址 ***start***

通过 （***start+offset***）即可定位某个类对象的某个成员的内存地址。

#### 4.2 为什么使用成员指针时才提供成员所属的对象

如我们前面分析的，成员指针的地址定位是**相对内存地址**：先确定偏移量，再确定首地址。

那为什么不直接通过对类对象的某个成员取地址来实现**绝对内存地址**定位呢？

* 例如 `&obj.val;`。这样只需要一次取地址就可以实现类成员的定位，不是更方便吗？

##### 1. 成员指针不是普通指针

成员指针的语义是绑定到类的某个成员，而不是绑定到某个类的某个成员（引用），也不是绑定到某个类的某个成员的类型（普通指针）。

* 成员指针绑定到类的某个成员意味着，我们可以使用任意该类的类对象来使用成员指针访问该对象的某个成员。

而如果使用 `&classAobj.val` 的形式来提供地址，看起来成员指针和普通指针并没有什么区别。

* 假如 `val` 的类型是 `T`，那么此时看起来成员指针就是一个绑定到类型 `T` 的普通指针，只不过此时我们是使用类对象 `classAobj` 的类型为 `T` 的某个成员来绑定地址。
* 这是否意味着
  * 我们是否可以使用 `classAobj` 的另一个类型为 `T` 的成员来绑定地址？
  * 是否可以使用 `classBobj` 或者 `classCobj` 的类型为 `T` 的成员来绑定地址？
  * 甚至可以直接使用某个类型为 `T` 的内置类型变量来初始化指针呢？

所以说，这种绝对地址定位的方法在语义上就是有问题的。相反的，如果我们让成员指针绑定的地址为类成员相对于类对象的偏移量，那么成员指针的语义就很明确了：

* 我们只能使用该成员所属类的类对象来定位起始地址，而不能使用其它类，更不能使用内置类型
* 将指针绑定到特定的对象还有一个好处，就是每次使用时只需要指定类对象来调用指针即可，而不必重新对指针赋值，使其指向我们要绑定的对象。因此使其用来也更为方便。

对于普通指针，我们是直接通过指针来操纵指针所指向的对象的。但是对于成员指针来说，从它的用法可以看出，我们需要通过类对象来调用成员指针。看上去，这个指针是**类的一个“成员”**一样。

最后，**即使我们的目的不是只绑定具体的成员，对于类成员而言，使用成员指针也是一个好的选择。**因为使用成员指针时，我们在初始化时就会绑定到类的成员。这样编译器就知道哪个类类型的成员将被访问，从而进行**类型检查**。确保在操作特定对象时使用正确的成员。直接通过成员地址访问的话，编译器不会进行成员归属检查，更容易导致访问非法内存。

##### 2. 虚函数

显然普通指针是无法解决虚函数动态绑定的问题的，因为普通指针在绑定时并没有记录与类有关的信息，它至于所绑定对象的类型有关系。

``` cpp
class Base {
public:
    virtual void vfunc() { cout << "Base::vfunc" << endl; }
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    virtual void vfunc() { cout << "Derived::vfunc" << endl; } 
};

void (Base::*fptr)() = &Base::vfunc
    
Base *b = new Base();
Base *d = new Derived();

int main()
{
    (b->*fptr)(); 
    (d->*fptr)();   
    return 0;
}
```

#### 4.3 成员指针可以换绑

我们前面一直强调，成员指针是绑定到类的某个成员的，只要绑定到该成员，那么后续通过该成员指针访问的对象都是该成员，不可能是类的其它成员或内置类型。但是，成员指针是可以**<font color=blue>换绑（一对多）</font>**的，我们可以看一下成员指针的定义：

* 指向成员变量：`int Foo::*ptr = &Foo::x; `
* 指向成员函数：`int (Foo::*fptr)() const = &Foo::get;`

不那么准确来说：

* `ptr` 的类型就是 `int Foo::*`，它可以绑定到类 `Foo` 的任何类型为 `int` 的成员变量
* `fptr` 的类型就是 `int (Foo::*)() const`，它可以绑定到类 `Foo` 的任何函数签名为 `int()const` 的成员函数

利用好成员指针可以换绑，或者说可以一对多的特性，我们可以实现一些有意思的特性，例如函数表，这一点在下面的成员函数指针部分中会介绍。

#### 4.4 数据成员指针

指定类的数据成员指针时，需要包含成员所属的类，即在 `*` 之前添加 `classname::` 以表示当前定义的指针可以指向 `classname` 的成员：

``` C++
struct Foo {
    Foo(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    int x;
    int y;
};

Foo a(1, 2);
Foo *b = new Foo(10, 20);

int Foo::*ptr = &Foo::x; 

int main() 
{
    a.*ptr = 10;
    b->*ptr = 30;
    return 0;
}
```

与成员访问运算符 `.` 和 `->` 类似，也有两种成员指针访问运算符 `.*` 和 `->*`。无论是哪一种形式，都是限制性解引用运算符 `*`，再执行成员访问运算符。

另外就是要注意，常规的访问控制权限对成员指针同样有效：

``` C++
struct Foo {
    Foo(int _x = 0, int _y = 0) : x(_x), y(_y) {}
private:
    int x;
    int y;
};

int Foo::*ptr = &Foo::x; //member "Foo::x" (declared at line 11) is inaccessible
```

由于数据成员一般情况下是私有的，所以我们通常不能直接获取数据成员的指针。常规的做法是定义一个函数，令其返回值是指向该成员的指针，然后我们就能通过该指针访问数据成员了。

``` C++
struct Foo {
    Foo(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    static int Foo::* getPtrX() { return &Foo::x; }
private:
    int x;
    int y;
};

static int Foo::* getPtrX();

Foo a(1, 2);
Foo *b = new Foo(10, 20);

int Foo::*ptr = Foo::getPtrX();

int main() 
{
    a.*ptr = 10; 
    b->*ptr = 30;
    cout << a.getx() << endl; // 10
    cout << b->getx() << endl; // 30
    return 0;
}
```

不过我们可以发现，从函数返回指向类的数据成员的指针的方法似乎破坏了类的“封装”特性，因此这里要格外小心！

* `a.*ptr = 10;` 直接改变了类的私有成员的值。

#### 4.5 成员函数指针

##### 1. 定义与使用成员函数指针

``` C++
struct Foo {
    Foo(int _x = 0) : x(_x) {}
    int get() const  { return x; }
    void set(int _x) { x= _x; }
public:
    int x;
};

Foo a(16);
Foo *b = new Foo(1024);

// 由于const属于函数签名，所以在的定义成员函数指针时也需要指定
int (Foo::*getptr)() const = &Foo::get;
auto setptr = &Foo::set;

int main() 
{
    cout << (a.*getptr)() << endl; // 16
    (a.*setptr)(123);
    cout << (a.*getptr)() << endl; // 123

    cout << (b->*getptr)() << endl; // 1024
    (b->*setptr)(789);
    cout << (b->*getptr)() << endl; // 789
    return 0;
}
```

其实成员函数指针的类型并不复杂，按步骤走即可：

1. 函数类型为 `int get() const;` <font color=blue>**注意别忘了 `const` 和引用限定符，它们也属于函数签名的一部分。**</font>
2. 将函数名替换为指针名 `int (*getptr)() const;`
3. 由于是成员函数指针，还需要添加类限定符：`int (Foo::*ptr) const;`

当然，更简单的方法是直接使用 `auto`😀，但是如果该函数有多个重载版本，就不能使用 `auto`，此时必须显示指定函数类型 😭。

另外就是，注意成员函数的调用形式，对于 `a.f()`，它相当于 `(a.f)()` 而不是 `a.(f())` 。即，先通过成员访问运算符获得类对象 `a` 的成员 `f`，发现它是一个函数，然后通过成员访问运算符 `()` 传递参数。

当我们通过成员函数指针调用函数时，必须将 `a.*getptr` 用括号括起来，否则对于 `a.*getptr()` ，它会被解释为 `a.*(getptr())`。因为函数调用运算符的优先级高于指针解引用运算符的优先级。

另外，和普通函数指针一样，我们也可以使用 `typedef` 或 `using` 来指定类型别名可以让成员函数指针更好理解：

``` C++
struct Foo {
    Foo(int _x = 0) : x(_x) {}
    int get() const  { return x; }
    void set(int _x) { x= _x; }
public:
    int x;
};

using FPtr = int(Foo::*)() const;
FPtr getptr = &Foo::get;
```

##### 2. 成员指针函数表

对于普通函数指针和指向成员函数的指针来说，一种常见的用法是将其存入一个函数表当中。如果一个类含有几个类型相同的成员，则这样一张函数表可以帮助我们从这些成员中选择一个。现在我们有一个光标类，它保存了光标的坐标，并且可以通过调用成员函数改变光标的位置：

``` c++
class Cursor {
public:
    Cursor& left (int d = 1);
    Cursor& right(int d = 1);
    Cursor& up   (int d = 1);
    Cursor& down (int d = 1);

    int getX() const { return x; }
    int gety() const { return y; }
    void setX(int _x, int _y) {x = _x, y = _y;}

private:
    int x, y;
};
```

其中，`left`、`right`、`up` 和 `down` 这几个函数的类型完全相同。我们希望定义一个 `move` 函数，使其可以调用上面四个函数中的任意一个并执行相应的操作。

我们可以在 `Cursor` 类中添加一个静态成员，它是一个指向光标移动函数的指针的数组。另外我们也可以添加一个枚举用来方便的表示和调用我们当前使用的成员函数是哪一个：

``` c++
class Cursor {
public:
    using Motion = Cursor&(Cursor::*)(int); // 成员函数指针的类型

    enum Direction {LEFT, RIGHT, UP, DOWN};

    Cursor() : x(0), y(0) {}
    Cursor(int _x, int _y) : x(_x), y(_y) {}
    
    Cursor& move(Direction mf, int d = 1) { return (this->*menu[mf])(d); }
    
    Cursor& left (int d = 1)  { y -= d; return *this; }
    Cursor& right(int d = 1)  { y += d; return *this; }
    Cursor& up   (int d = 1)  { x += d; return *this; }
    Cursor& down (int d = 1)  { x -= d; return *this; }
    
    int getX() const { return x; }
    int getY() const { return y; }
    void set(int _x, int _y) { x = _x, y = _y; }

private:
    int x, y;
    static Motion menu[];
};

// 赋值的顺序要与enum的顺序一致
// 注意这里要写为 Cursor::menu, 否则编译器会认为这只是一个普通的全局变量
Cursor::Motion Cursor::menu[] = {
    &Cursor::left, 
    &Cursor::right, 
    &Cursor::up, 
    &Cursor::down,
};

ostream& operator<<(ostream &os, const Cursor &c)
{
    return os << "(" << c.getX() << "," << c.getY() << ")";
} 

int main() 
{

    Cursor c(1, 2);
    cout << c << endl;
    cout << c.move(Cursor::DOWN, 3) << endl;
    cout << c.move(Cursor::RIGHT, 2) << endl;
    cout << c.move(Cursor::LEFT, 1) << endl;
    cout << c.move(Cursor::UP, 2) << endl;

    return 0;
}
```

##### 3. 成员函数与类模板

当涉及模板时，使用 `using` 来定义别名比 `typedef` 更简单。

``` c++
template<typename T>
struct Less {
    bool operator()(T a, T b) {
        return a < b;
    }
};

template<typename T>
using fptr = bool (Less<T>::*)(T,T);

Less<int> obj;
fptr<int> cmp = &Less<int>::operator();

int main()
{
    int a = 10, b = 20;
    cout << (obj.*cmp)(a, b) << endl;
    return 0;
}
```

#### 4.5 将成员函数转换为可调用对象

如我们所知，想要通过一个指向成员函数的指针进行函数调用，必须首先调用 `.*` 或 `->*` 运算符将该指针绑定到特定的对象上。因此<font color=blue>**与普通的指针不同，成员指针不是一个可调用对象，这样的指针不支持函数调用运算符。**</font>

由于成员指针不是可调用对象，所以我们不能直接将一个指向成员函数的指针传递给算法。以下是解决该问题的一些方法，用到的设施都在 `function` 头文件中，可以将一个成员函数指针转换为可调用对象。

要注意，即使将成员函数指针转换为一个可调用对象，也依然要通过实例化对象来调用转换后的可调用对象。就比如 `string` 类的成员函数生成的可调用对象，我们显然不能将这个可调用对象放在有关 `int` 的场景中，因为 `int` 的实例化对象无法调用  `string` 的成员函数。

##### 1. `mem_fn `

> Function template `std::mem_fn` generates **wrapper objects** for **pointers to members**, which can store, copy, and invoke a [pointer to member](https://en.cppreference.com/w/cpp/language/pointer#Pointers_to_members). Both references and pointers (including smart pointers) to an object can be used when invoking a `std::mem_fn`.

标准库设施 `mem_fn` 可以让编译器负责推断成员的类型，从而可以从成员指针生成一个可调用对象。它类似于 `std::bind`，但专门用于成员函数。

``` cpp
vector<string> s = {"hello", "", "world"};

auto it = find_if(s.begin(), s.end(), mem_fn(&string::empty));
cout << it - s.begin() << endl;        // 1

auto f1 = mem_fn(&string::empty);
cout << f1(s[0]) << endl;             // 0
cout << f1(std::move(s[0])) << endl;  // 0

auto f2 = mem_fn(&vector<string>::size);
cout << f2(&s) << endl;               // 3 
```

其本质上是根据成员函数指针生成一个**仿函数**。

##### 2. `bind`

`bind` 相较于 `mem_fn` 的一个好处就是，`bind` 可以为成员函数<font color=blue>**绑定参数**</font>，以及使用<font color=blue>**占位符**</font>，因此更加灵活。

``` cpp
vector<string> s = {"hello", "", "world"};

auto it = find_if(s.begin(), s.end(), bind(&string::empty, _1));
cout << it - s.begin() << endl;   // 1

auto f1 = bind(&string::empty, _1);
cout << f1(s[0]) << endl;         // 0

auto f2 = bind(&vector<string>::size, _1);
cout << f2(&s) << endl;           // 3 
cout << f2(s) << endl;            // 3 
```

事实上，我们可以发现，`bind` 生成的可调用对象既可以接受**引用参数**，也可以接受**指针参数**。这是因为 `bind` 生成的可调用对象含有重载的函数调用运算符：一个接受指针，另一个接受引用（既可以接受左值引用，也可以接受右值引用）。`mem_fn` 也是同理。

##### 3. 使用 `function` 包装成员函数指针

``` C++
vector<string> s = {"hello", "", "world"};

// 接受一个左值引用（也可以接受右值引用）
function<bool(const string&)> str_empty = &string::empty;
auto it = find_if(s.begin(), s.end(), str_empty);
cout << it - s.begin() << endl; // 1

// 接受一个指针
function<size_t(const vector<string>*)> vec_size = &vector<string>::size;
cout << vec_size(&s) << endl;  // 3
```

不同于 `mem_fn` 和 `bind`，`function` 包装的成员函数所接受的参数的类型被我们指定好了，因此我们只能传入与参数匹配的类型。

另外我们可以发现，我们没有传入隐式地 `this` 指针。通常情况下，执行成员函数的对象将被隐式地传递 `this` 指针。

### 5. 嵌套类

一个类可以定义在另一个类的内部，前者称为嵌套类（nested class）或嵌套类型（nested type）。嵌套类常用于定义作为**实现部分**的类。

#### 5.1 嵌套类和外层类的关系

嵌套类是一个**独立**的类。

1. 外层类的对象和嵌套类的对象是相互**独立**的。
   * 在嵌套类的对象中不包含任何外层类**定义**的成员；外层类的对象中也不包含任何嵌套类定义的成员。
   * 不过，嵌套类可以直接使用外层类的**类型名称（如 `typedef`、`using` 别名）**、**静态成员**和**枚举常量**（即使这些成员是 `private`）；外层类的 **非静态成员**（普通成员变量或函数）必须通过外层类的实例（对象、指针、引用）访问。
2. 嵌套类中成员的种类和非嵌套类是一样的。和其他类类似，嵌套类也使用访问限定符来控制外界对其成员的访问权限。外层类对嵌套类的成员没有特殊的访问权限，同样，嵌套类对外层类的成员也没有特殊的访问权限。
3. 嵌套类就是外层类中定义的一个<font color=blue>**类型成员**</font>。和其他类型成员类似，该类型成员的访问权限由访问限定符 `public`、`protected` 和 `private` 限定。
4. 由于嵌套类嵌套在外层类当中，嵌套类的名字在外层类作用域中是可见的，在外层类作用于之外不可见。和其它嵌套的名字一样，嵌套类的名字不会和别的作用域中的同一个名字冲突。
5. 嵌套类默认不是外层类的友元（除非显式声明）。

#### 5.2 嵌套类的名字查找

嵌套类中的名字查找按以下顺序进行（由内向外）：

1. **当前嵌套类的作用域**（包括基类）。
2. **外层类的作用域**（包括外层类的基类）。
3. **外层类所在的命名空间作用域**（逐步向外，直到全局命名空间）。
4. **其他关联的作用域**（如 ADL 或友元声明引入的作用域）。

> 注意：名字查找在**首次找到匹配的名字时停止**，不会继续向外层搜索。

##### 1. 名字隐藏

如果嵌套类和外层类有同名的成员，嵌套类内部的名称会**隐藏外层类的同名成员**。此时需要显式指定作用域：

``` cpp
class Outer {
public:
    static int value;  // 外层类的静态成员

    class Nested {
    public:
        static int value;  // 嵌套类的同名静态成员

        void foo() {
            value = 42;        // 访问 Nested::value
            Outer::value = 10; // 显式访问外层类的 value
        }
    };
};
```

##### 2. **直接访问外层类的成员**

嵌套类可以**直接**访问以下外层类成员（无需对象/指针/引用）：

- **类型名称**（`typedef`、`using`、内部类）。
- **静态成员变量和函数**。
- **枚举常量**。

```cpp
class Outer {
public:
    typedef int MyInt;          // 类型名称
    static int staticVar;       // 静态成员
    enum Color { RED, GREEN };  // 枚举常量

    class Nested {
    public:
        void foo() {
            MyInt x = 10;        // 直接访问外层类的类型名称
            staticVar = 42;      // 直接访问外层类的静态成员
            Color c = RED;       // 直接访问外层类的枚举常量
        }
    };
};
```

不过有意思的一点是，我们知道，静态成员、类型成员和枚举成员虽然不占用类对象的内存空间和无需通过类对象调用，但他们仍然属于类的成员，因此它们仍然受到访问限定符的限制。

但是对于嵌套类来说，即使这些成员的访问限定符是 `private`，依然可以访问：

``` cpp
class Outer {
private:
    typedef int MyInt;          // 类型名称
    enum Color { RED, GREEN };  // 枚举常量
    static const int staticVar = 1024;       // 静态成员
public:
    class Nested {
    public:
        void func() {
            MyInt x = 10;        // 直接访问外层类的类型名称
            cout << x << endl;
            Color c = RED;       // 直接访问外层类的枚举常量
            cout << c << endl;
            cout << staticVar << endl; // // 直接访问外层类的静态成员
        }
    };
};

int main()
{
    Outer::Nested{}.func(); // valid
    Outer::Color c; // error: ‘enum Outer::Color’ is private within this context
    int x = Outer::staticVar; // error: ‘const int Outer::staticVar’ is private within this context
    Outer::MyInt x = 10; // error: ‘typedef int Outer::MyInt’ is private within this context
    return 0;
}
```

##### 3. **访问外层类的非静态成员**

嵌套类必须通过外层类的**对象、指针或引用**访问非静态成员，因为非静态成员属于具体实例。

```cpp
class Outer {
public:
    int instanceVar;

    class Nested {
    public:
        void bar(Outer& outer) {
            outer.instanceVar = 10;  // 必须通过对象访问
        }
    };
};
```

##### 4. 嵌套类可以访问“外层类的外层类”

``` cpp
struct Outer {
    const static int outer_val = 1024;
    struct Inner {
        const static int inner_val = 521;
        struct Nested {
            void func() {
                cout << inner_val << endl; // 512
                cout << outer_val << endl; // 1024
            }
        };
    };
};
```

* 名字查找按 **由内向外** 的顺序进行，支持跨多层嵌套访问。

##### 5. **依赖外层类模板参数的情况**

如果外层类是模板类，嵌套类的名字查找会分两个阶段：

1. **模板定义阶段**：查找不依赖模板参数的名字（如外层类的静态成员、类型名称）。
2. **模板实例化阶段**：查找依赖模板参数的名字（如外层类的非静态成员）。

```cpp
template <typename T>
class Outer {
public:
    static T staticVar;  // 静态成员（依赖 T）
    T instanceVar;       // 非静态成员（依赖 T）

    class Nested {
    public:
        void foo(Outer& outer) {
            staticVar = T();         // 直接访问静态成员（模板实例化时解析）
            outer.instanceVar = T(); // 必须通过对象访问非静态成员
        }
    };
};
```

#### 5.3 嵌套类的类外定义

``` C++
struct Outer {
    typedef int value_type;

    struct Inner {
        Inner() = default;
        Inner(value_type _num);

        value_type get() const;
        void set(value_type x);

        static value_type MAX;
    private:
        value_type num;
    };
};

Outer::Inner::Inner(value_type _num) : num(_num) {}
Outer::value_type Outer::Inner::MAX = 1024;
Outer::value_type Outer::Inner::get() const { return num; }  
void Outer::Inner::set(value_type x) { num = x; }
```

* 其实就是多加了一层 `Outer::` 作用域。

### 6. union：一种节省空间的类

`union` 是 C++ 中的一种特殊数据类型，它允许**在同一内存位置存储不同的数据类型**。这意味着：

1. 一个 `union` 可以有多个数据成员，但是在任意时刻就只有一个数据成员的值有意义。
2. 当你修改一个成员的值时，其他成员的值也会受到影响，因为它们共享同一块内存。
3. 所有成员共享同一块内存空间，并且 `union` 的大小等于其最大成员的大小。

当我们给 `union` 的一个成员赋值之后，其他成员就变成**未定义**的状态了。

* 这是因为不同对象之间底层数据的存储和解释是不同的，例如我们给一个 `int` 对象传入数据 $12$，在其内部会将其解释为二进制数据 `0x0c`，但是 `0x0c` 在 `double` 的解释下就不是 $12$ 了。

和枚举类型与类类型一样，一个 `union` 定义了一种新类型。

类的某些特性对 `union` 同样适用，但并非所有特性都如此。

1. **`union` 不能含有引用类型的成员**，除此之外，它的成员可以是多大多数类型。
2. 在 C++11 新标准中，含有构造函数或析构函数的类类型也可以作为 `union` 的成员类型。
3. `union` 可以为其成员指定 `public`、`protected` 和 `private` 等保护标记。
4. 和 `struct` 一样，**默认情况下，`union` 的成员都是公有的。**
5. `union` 既不能继承自其他类，也不能作为基类使用，所以在 `union` 中不能含有虚函数。

#### 6.1 使用 union

如果我们为 `union` 提供了初始值，则该初始值被用于初始化**第一个成员**。

为 `union` 的一个数据成员赋值会令其它数据成员变成未定义的状态。

##### 1. 默认初始化

简单的默认初始化不会为 `union` 的任何成员赋值：

```cpp
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data d; // 未初始化，所有成员都有未定义的值
    return 0;
}
```

##### 2. 初始化第一个成员（C++98 风格）

在传统 C++ 中，你只能初始化 union 的第一个成员：

```cpp
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data d = {10}; // 初始化第一个成员 i 为 10
    return 0;
}
```

##### 3. 显式指定成员初始化（C++11 及以后）

从 C++11 开始，你可以使用指定初始化器来明确初始化哪个成员：

```cpp
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data d1 = {.i = 10};  // 初始化 i 为 10
    Data d2 = {.f = 3.14}; // 初始化 f 为 3.14
    return 0;
}
```

##### 4. 使用构造函数（C++11 及以后）

C++11 之后，`union` 可以包含具有非平凡构造函数的类型，但需要使用特殊的处理方式：

```cpp
#include <new>   // 用于 placement new
#include <string>

union Data {
    int i;
    float f;
    std::string s; // 具有非平凡构造函数的类型
    
    Data() : i(0) {} // 默认构造函数初始化 i
    
    // 析构函数需要知道当前哪个成员是活跃的
    ~Data() {
        // 假设我们在某处跟踪了活跃的成员
    }
};

int main() {
    Data d;
    // 如果想初始化 string 成员，需要使用 placement new:
    new (&d.s) std::string("Hello");
    
    // 使用完后需要显式调用析构函数
    d.s.~basic_string();
    
    return 0;
}
```

##### 5. 使用匿名 union

匿名 `union` 的成员可以直接在外部作用域中访问：

```cpp
struct Value {
    enum Type { INT, FLOAT } type;
    
    union { // 匿名 union
        int i;
        float f;
    };
    
    Value(int val)   : type(INT),   i(val) {}
    Value(float val) : type(FLOAT), f(val) {}
};

int main() {
    Value v1(10);    // 初始化为整数 10
    Value v2(3.14f); // 初始化为浮点数 3.14
    return 0;
}
```

注意事项：

1. 初始化 `union` 时只能初始化一个成员，因为所有成员共享同一内存位置。
2. 当 `union` 包含具有非平凡构造函数、析构函数或拷贝/移动操作的类型时，需要特别小心管理其生命周期。
3. 在现代 C++ 中，对于需要保存多种类型的情况，通常推荐使用 `std::variant` 而不是 `union`，因为它更安全且能自动管理成员的生命周期。

#### 6.2 匿名 union

一旦我们定义了一个匿名 `union`，编译器就自动的为该 `union` 创建一个未命名的对象。

* 匿名 `union` 不能包含受保护的成员或私有成员，也不能定义成员函数。

**在匿名 `union` 的定义所在的作用域内**该 `union` 的成员都是可以直接访问的：

``` C++
union {
    char cval;
    int ival;
    double dval;
};
ival = 0x12345678;
cout << hex << showbase << uppercase << ival << endl;
```

注意，我们上面所说的在“同一个作用域内”是很严格的限制，例如下面的代码：

``` cpp
union { // error: namespace-scope anonymous aggregates must be static
    char cval;
    int ival;
    double dval;
};

int main()
{
    ival = 0x12345678; // error: ‘ival’ was not declared in this scope
    cout << hex << showbase << uppercase << ival << endl;
    return 0;
}
```

当我们将匿名 `union` 定义在全局作用域时，在局部作用域 `::main` 中就无法访问匿名 `union` 中的对象。

#### 6.3 union 的原理

我们前面一直说，为 `union` 的一个数据成员赋值会导致其它数据成员的值处于未定义状态，为什么呢？

这是因为 `union` 的成员变量共享“一块”内存空间，所有成员的修改都在这块内存空间上进行。

例如 `union` 可能包含一个 `char`（一字节）对象， 一个 `int`（四字节）对象，那么这个 `union` 的内存空间可能就是 $4B$，而 `char` 只使用 $1B$。如果我们修改了 `char` 对象的值，那么 `char` 就会直接在这块内存上修改一个字节的数据，这就会导致 `int` 的 $4B$ 其中一个字节的值被修改了。

但是对于这种类型比较简单的情况，我们是可以推算出其它变量值的变化的：

``` C++
union {
    char cval;
    int ival;
};
cout << hex << showbase << uppercase;
ival = 0x12345678;
cout << ival << endl; // 0X12345678
cval = 0xaf;
cout << ival << endl; // 0X123456AF
```

> 从这个例子上来看，`char` 修改了 `int` 对象的低位值，这说明我们的电脑是“小端存储方式”，即低位的数据存放在低地址。

但是对于像包含类类型，递归 `union` 等情况，问题就复杂的多了，我们就很难去推断其他值的变化了，此时一般是默认其它类型的值是 $UB$。

#### 6.4 union 与类

##### 1. union 与特殊成员的关系

在 C++ 早期版本中，在 `union` 中不能含有定义了构造函数和拷贝控制成员的类类型成员。 C++11 新标准取消了这一限制。不过，如果 `union` 的成员类型定义了自己的构造函数和/或拷贝控制成员，则该 `union` 的用法要比只含有内置类型成员的 `union` 复杂的多。

**(1) 内置类型的 `union`**：

- 当 `union` 只包含内置类型（如 `int`、`float` 等）时，编译器可以轻松合成默认构造函数和拷贝控制成员。
- 这些合成的函数会按照成员声明的顺序处理第一个成员。

**(2) 含有类类型成员的 `union`**：

- 当 `union` 包含像 `std::string` 这样的类类型成员时，情况变得复杂。
- <font color=lightred>编译器不知道应该如何正确初始化或拷贝 `union`，因为它无法确定哪个成员是"活跃的"（当前 `union` 保存的是哪个对象）。</font>
- 因此，编译器会将默认构造函数和拷贝控制成员（拷贝构造、拷贝赋值、移动构造、移动赋值、析构）标记为删除的（`= delete`）。

这里的 [编译器不知道应该如何正确初始化或拷贝 `union`，因为它无法确定哪个成员是"活跃的"] 意思是说：

* 假如 `union` 包含了三个类对象 `c1`，`c2` 和 `c3`，在当前状态下 `union` 保存的是 `c1` 的值
* 但是如果我们后续想用 `union` 保存 `c3` 的值，就需要先调用 `c1` 的析构函数，再调用 `c3` 的构造函数
* 可是问题是，`union` 并不知道当前状态下保存的对象是什么类型，它无法主动调用 `c1` 的构造函数
* 而由于无法析构之前保存的对象，就无法构造当前要保存的对象

``` cpp
union MyUnion {
    int ival;
    string sval;
} u; // error: use of deleted function ‘MyUnion::MyUnion()’
```

**(3) 包含 `union` 成员的类**：

- 如果一个类包含具有删除的特殊成员函数的 `union` 成员，那么该类对应的特殊成员函数也会被标记为删除的。
- 这是成员传递效应的结果 - 如果一个成员不能被复制/移动/销毁，那么包含它的类也不能。

``` cpp
struct Foo {
    union MyUnion {
        int ival;
        string sval;
    } u; 
} f; // error: use of deleted function ‘Foo::Foo()’
```

##### 2. 必须手动调用构造函数和析构函数

当 `union` 包含的是内置类型的成员时，我们可以使用普通的赋值语句改变 `union` 保存的值。但是对于含有特殊类类型成员的 `union` 就没这么简单了，如果我们想将 `union` 的值改为类类型成员对应的值，或者将类类型成员的值改为一个其他值，则必须分别构造或析构该类类型的成员：

* 当我们将 `union` 的值改为类类型成员对应的值时，必须运行该类型的构造函数；
* 反之，当我们将类类型成员的值改为一个其他值时，必须运行该类型的析构函数。

<font color=blue>这是因为类类型通常会管理资源（如动态分配的内存、文件句柄等），而 `union` 只允许一个成员同时存在，并且它**不会自动销毁“不存在”的成员**，因此如果不小心管理这些资源，可能会导致资源泄漏或未定义行为。</font>

例如，当你为一个 `union` 成员赋值时，如果该成员是一个类类型（比如 `std::string`），编译器不能自动处理它的构造或析构，因为 `union` 并没有为每个成员单独分配构造或析构过程。所以你需要手动处理这些细节。你可以使用 `placement new` 来手动调用构造函数，同时确保调用析构函数。

``` C++
union MyUnion {
    int ival;
    string sval;
    MyUnion() {} // 注意不能写为 MuUnion() = default; 因为默认生成的构造函数是删除的
    ~MyUnion() {}
};

int main()
{
    MyUnion u;
    
    // 保存类型为 string 的对象
    new(&u.sval) string("hello,world");
    cout << u.sval << endl; // hello,world

    // 保存类型为 int 的对象
    u.sval.~string();
    u.ival = 42;
    cout << u.ival << endl; // 42

    return 0;
}
```

#### 6.5 使用类管理 union

通过上面也可以看出，对于 `union` 来说，想要构造或销毁类类型的成员必须执行非常复杂的操作，因此，我们通常把含有类类型成员的 `union` 内嵌在一个类当中。

* 这个类可以管理并控制与 `union` 的类类型成员有关的类型转换。
* 为了追踪 `union` 中到底存储了什么类型的值，我们通常会定义一个独立的对象，该对象称为 `union` 的**判别式（*discriminant*）**。我们可以使用判别式辨别 `union` 存储的值。为了保持 `union` 与其判别式同步，我们将判别式也作为类的成员。通常我们使用枚举类型来实现 `union` 的判别式。

``` c++
class TestToken;

class Token {
    friend class TestToken;
    using enum_type = unsigned int;
    enum EnumType : enum_type {CHAR, INT, DOUBLE, STRING};

public:
    Token() : flag(INT), ival(0) {}
    ~Token() { free(); }
    
    Token(const Token &t) { copyUnion(t); }
    Token(Token &&t) noexcept /* 移动操作不应该抛出异常 */ { moveUnion(std::move(t)); }

    Token& operator=(const Token &t) { 
        if(this != &t) {
            copyUnion(t);
        }
         return *this; 
    }
    Token& operator=(Token &&t) /* 移动操作不应该抛出异常 */  { 
        if(this != &t) {
            moveUnion(std::move(t)); 
        }
        return *this; 
    }

    Token& operator=(char);
    Token& operator=(int);
    Token& operator=(double);
    Token& operator=(const std::string &);
    Token& operator=(std::string &&) noexcept /* 移动操作不应该抛出异常 */;

    // 获取当前类型的字符串表示
    std::string get_type() const;

private:
    void copyUnion(const Token &t);
    void moveUnion(Token &&t) noexcept /* 移动操作不应该抛出异常 */  ;
    void freeString() { 
        if(flag == STRING) {
            sval.std::string::~string(); 
        }
    }
    void free() { freeString(); }
    

private:
    EnumType flag;  
    union {
        char cval;
        int ival;
        double dval;
        std::string sval;
    };
};


std::string Token::get_type() const { 
    static const std::unordered_map<enum_type, std::string> union_type_info = {
        {CHAR, "char"}, {INT, "int"}, {DOUBLE, "double"}, {STRING, "string"},
    };
    return "Token Type: " + union_type_info.at(static_cast<enum_type>(flag)); 
}

void Token::copyUnion(const Token &t) {
    // 自我赋值的检查应该放在 operator= 中处理
    free();
    flag = t.flag;

    switch(t.flag) {
        case CHAR:
            cval = t.cval;  break; 
        case INT:
            ival = t.ival;  break;
        case DOUBLE:
            dval = t.dval;  break;
        case STRING:
            new(&sval) std::string(t.sval);  break;
        default: ;
    }
}

void Token::moveUnion(Token &&t) noexcept  {
    // 自我赋值的检查应该放在 operator= 中处理
    free();
    flag = t.flag;

    switch(t.flag) {
        case CHAR:
            cval = t.cval;  break; 
        case INT:
            ival = t.ival;  break;
        case DOUBLE:
            dval = t.dval;  break;
        case STRING:
            new(&sval) std::string(std::move(t.sval));  break;
        default: ;
    }
}

Token& Token::operator=(char c) {
    free();
    flag = CHAR;
    cval = c;
    return *this;
}

Token& Token::operator=(int i) {
    free();
    flag = INT;
    ival = i;
    return *this;
}

Token& Token::operator=(double d) {
    free();
    flag = DOUBLE;
    dval = d;
    return *this;
}

Token& Token::operator=(const std::string &s) {
    if(flag == STRING) {
        sval = s;
    }
    else {
        free();
        flag = STRING;
        new(&sval) std::string(s);
    }
    return *this;
}

Token& Token::operator=(std::string &&_sval) noexcept  {
    if(flag == STRING) {
        sval = std::move(_sval);
    }
    else {
        free();
        flag = STRING;
        new(&sval) std::string(std::move(_sval));
    }
    return *this;
}

using namespace std;

// 测试类实现
class TestToken {
public:
    void operator()();
private:
    Token t;
};

#define println_class_size(x) \
    std::cout << "sizeof(" << #x << "): " << sizeof(x) << "B" << std::endl;

void TestToken::operator()() {
    println_class_size(string);    // sizeof(string): 32B
    println_class_size(Token);     // sizeof(Token):  40B
    cout << t.get_type() << endl;  // Token Type: int
    cout << t.ival << endl;        // 0
    t = "hello";
    cout << t.get_type() << endl;  // Token Type: string
    cout << t.sval << endl;        // hello
    t = "world";
    cout << t.get_type() << endl;  // Token Type: string
    cout << t.sval << endl;        // world
    t = 3.1415926;
    cout << t.get_type() << endl;  // Token Type: double
    cout << t.dval << endl;        // 3.14159
    t = 'A';
    cout << t.get_type() << endl;  // Token Type: char
    cout << t.cval << endl;        // A    
}

int main()
{
    TestToken{}();
    return 0;
}
```

### 7. 局部类

类可以**定义在某个函数的内部**，我们称这样的类为局部类。

* 局部类定义的类型只在定义它的作用域内可见，也即作用域仅限于定义它的函数内。
* **局部类的所有成员（包括函数在内）都必须完整定义在类的内部**。因此在局部类中也不允许声明静态数据成员，因为我们无法定义这样的成员。

局部类的这些限制使得它们在实际应用中较少使用，但在某些情况下，如果需要一个仅在特定函数内使用的辅助类，而且功能相对简单，局部类可能是一个合适的选择。这种设计有助于封装实现细节，防止类型定义污染更广泛的命名空间。

#### 7.1 局部类不能使用函数作用域中的名字

局部类对其外层作用域中名字的访问权限受到很多限制，局部类只能访问外层作用域定义的**类型名**、静态变量以及**枚举类型**。如果局部类定义在某个函数内部，则该函数的普通局部变量不能被该局部类使用。

``` c++
int a = 10, b = 20;
int val = 100;
void localfunc(int val)
{
    static int sval = 30;
    enum color {RED, BLUE};
    struct Foo {
        color c;
        int fooVal;
        void func(color lc = RED)
        {
            fooVal = val;   // 错误：val是一个局部变量
            fooVal = ::val; // 正确：使用一个全局对象
            fooVal = sval;  // 正确：使用一个静态局部变量
            lc = BLUE;      // 正确：使用一个枚举成员
        }
    };
}
```

之所以不能使用外层函数的局部临时对象，是因为我们可以通过动态内存分配，将一个局部类的对象摆脱外层作用域的限制。因此当函数执行结束，栈空间回收，临时对象销毁时，这个局部类的对象再去调用函数中的局部临时对象就是不合理的了。

#### 7.2 常规的访问保护规则对局部类同样适用

外层函数对局部类的私有成员没有任何访问特权。当然，局部类可以将外层函数声明为友元（实际测试貌似不可行？）；或者更常见的情况是局部类将其成员声明为 `public`。在程序中有权访问局部类的代码非常优先。局部类已经封装在函数作用域中，通过信息隐藏进一步封装就显得没什么必要了。

#### 7.3 名字查找

这个有点类似于嵌套类的名字查找：

1. 局部类自己的成员
2. 外层函数的静态成员、类型别名和枚举
3. 全局作用于的变量

### 8. 固有的不可移植的特性

为了支持底层编程，C++ 提供了一些固有的不可移植（***nonportable***）的特性。

* 所谓不可移植的特性是指**因机器而异的特性**。
* 当我们将含有不可移植特性的程序从一台机器转移到另一台机器上或从一个编译器转移到另一个编译器时，通常需要重新编写该程序。

**"机器"**指的是计算机系统的**硬件和软件环境的组合**，包括：

1. **硬件架构**：如 x86、ARM、RISC-V、MIPS 等不同的 CPU 架构
2. **操作系统**：如 Windows、Linux、macOS、iOS、Android 等
3. **编译器实现**：如 GCC、MSVC、Clang 等不同的编译器
4. **平台特定库**：不同平台提供的系统库和 API

当我们说某个特性"不可移植"或"因机器而异"时，意味着这个特性的行为在不同的硬件架构、操作系统或编译器环境中可能表现不同，或者在某些环境中根本不可用。

例如，在 C++ 中一些不可移植的特性包括：

- 整数类型的具体大小（除了 char）
- 浮点数的精确表示
- 字节序（大端法 vs 小端法）
- 结构体成员的内存对齐方式
- 某些编译器特定的扩展或内置函数
- 平台特定的系统调用

这就是为什么编写可移植代码通常要避免依赖这些"因机器而异的特性"，而是使用语言标准定义的、在所有符合标准的环境中行为一致的特性。

#### 8.1 算数类型

在前面我们已经提到过，算数类型的大小在不同的机器上所占用的字节数并不一致。例如，**long** 类型在 32 位机器上是 4B；但在 64 位机器上是 8B。

对于浮点类型来说，比较常用的表示形式是 IEEE754 浮点数表示法，当然还有其它表示法。

#### 8.2 位域

类可以将其（非静态）数据成员定义成**位域（bit-field）**，在一个位域中含有一定数量的二进制位。

* 当一个程序需要向其他程序或硬件设备**传递二进制数据**时，通常会用到位域。
* 位域在内存中的布局是与机器相关的。
* 位域的类型必须是**整型（或枚举类型）**。又因为带符号位域的行为（e.g. 移位操作）是由具体实现确定的，所以在通常情况下我们使用**无符号类型**保存一个位域。
* 取地址运算符不能作用域位域，因此任何指针都不能指向类的位域。

位域的声明形式是在成员名字之后紧跟一个冒号以及一个常量表达式，该表达式用于制定成员所占的二进制比特数：

``` C++
using Bit = unsigned int;

class File {
    Bit mode       : 2;
    Bit modified   : 1;
    Bit prot_owner : 3;
    Bit prot_group : 3;
    Bit prot_world : 3;
public:
    enum modes { READ = 01, WRITE = 02, EXECUTE = 03 };
    File &open(modes);
    void cloes();
    void write();
    bool isRead() const;
    void setWrite();
};
```

如果可能的话，在类的内部连续定义的位域会压缩在同一整数的相邻位，从而提供**压缩存储**。例如在上面的声明中，五个位域可能会压缩在同一个 **unsigned int** 中。这些二进制位是否能压缩到同一个整数中以及如何压缩是与机器相关的。

位域的使用方式和类的其他数据成员并无差别，只不过我们一般通过**位运算**来操纵位域：

``` c++
File& File::open(modes m) {
    mode = m;
    return *this;
}
void File::close() {
    if(modified) {
        /*. save the modified content .*/
    }
    // close 
}

void File::write() { 
    modified = 1; 
}

bool File::isRead() const {
    return mode & READ;
}

void File::setWrite() {
    mode |= WRITE;
}
```

总而言之，由于以下原因，位域是不可移植的：

1. **字节对齐和填充**：

   不同的编译器和平台可能会对位域的对齐和填充方式有所不同。例如，某些编译器可能会在位域之间插入额外的填充位，以确保数据结构对齐，这会导致结构的内存布局在不同平台上有所不同。

2. **位域的存储顺序**：

   位域的存储顺序（即从低位到高位还是从高位到低位）也可能因平台而异。有些系统可能是从低位开始填充（**little-endian**），而其他系统可能是从高位开始填充（**big-endian**），这会影响位域的解释。

3. **最大位域大小**：

   在 C++ 标准中，没有规定位域的最大大小，因此在不同的编译器中，位域的大小限制可能会有所不同。

#### 8.3 volatile限定符

##### 1. volatile 有什么作用

C/C++ 中的 **volatile** 关键字和 **const** 对应，用来修饰变量，通常用于建立语言级别的 **memory barrier（内存屏障）**。这是 Bjarne Stroustrup 在 《The C++ Programming Language》 对 **volatile** 修饰词的说明：

> A volatile specifier is a **hint** to a compiler that an object may change its value in ways not specified by the language so that aggressive optimizations must be avoided.

**volatile** 关键字是一种**类型修饰符**，用它声明的类型变量表明可能被某些编译器未知的因素更改。当对象的值可能在程序的控制或检测之外被改变时，应该将改对象声明位 `volatile`。关键字 `volatile` 告诉编译器不应该对这样的对象进行优化。

* 例如，直接处理硬件的程序常常包含这样的数据元素，它们的值由程序直接控制之外的过程控制。
* 比如，程序可能包含一个由系统时钟定时更新的变量，由于这种更新无需经过编译器，所以编译器无法察觉对该变量的更新。
* 如果此时我们将此变量第一次读取的值放入寄存器中优化对该变量的读取（从而避免从内存中读取，因为读内存相较于读寄存器很慢），那么之后从寄存器再次读取的该变量的值可能就不是最新的了。

遇到这个关键字声明的变量，编译器对访问该变量的代码就不再进行优化，从而可以提供对特殊地址的稳定访问。

* <font color=blue>当使用 **volatile** 声明的变量时，系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据。而且读取的数据立刻被保存。</font>

例如：

``` cpp
volatile int n=10;
int a = n;
...
// 其他代码，并未明确告诉编译器，对 n 进行过操作
int b = n;
```

**volatile** 指出 `n` 是随时可能发生变化的，每次使用它的时候必须从 `n` 的地址中重新读取，因而编译器生成的汇编代码会重新从 `n` 的地址读取数据放在变量 `b` 中。

而优化做法是，如果我们没有对 `n` 指定 **volatile**，那么编译器可能用一个寄存器来保存 `n` 的值，那么由于在 `a` 和 `b` 读取之间，在语言层面上我们并没有修改 `n` 的值，但此时从系统层面上值 `n` 被修改了。由于编译器看不到对 `n` 的修改，因此寄存器里的副本不会被修改，但编译器对 `a` 和 `b` 的处理都是直接从寄存器中去读取值，所以此时 `a` 和 `b` 的值不一致。

因此说，如果 `n` 是一个寄存器变量或者表示一个端口数据就容易出错，所以说 `volatile` 可以保证对特殊地址的稳定访问。

一般来说，**volatile** 用在如下的地方：

* 终端服务程序中修改的供其他程序检测的变量需要加 **volatile**。
* 多任务环境下各任务间共享的标志应该加 **volatile。**
* 存储器映射的硬件寄存器通常也需要加 **volatile** 说明，因为每次对它的读写都可能有不同意义。

##### 2. 为什么 volatile 是不可移植的

`volatile` 的确切含义与机器有关，只能通过阅读编译器文档来理解。

* 例如，不同的处理器架构对内存访问和优化的策略可能不同，某些平台可能不会按照预期行为处理 `volatile`。
* 编译器在优化代码时可能会做出不同的选择。某些编译器在遇到 `volatile` 变量时可能会禁用特定的优化，而其他编译器可能会对此变量的访问进行更激进的优化。不同的编译器和编译选项可能会影响 `volatile` 变量的行为，导致不同的执行结果。
* 虽然 `volatile` 通常用于处理多线程环境中的共享变量，但它并不能提供线程安全的保证。多个线程同时访问和修改 `volatile` 变量可能导致竞态条件和不可预测的行为。为了在多线程环境中安全地共享数据，应该使用同步原语（如互斥锁）而不是依赖于 `volatile`。
* `volatile` 仅仅表示变量可能被外部因素修改，但并没有提供关于如何正确地访问或修改这个变量的语义。因此，不同的开发者可能会对 `volatile` 的使用产生不同的理解，从而导致代码在不同环境中的行为不一致。

总言而止，想要让使用了 `volatile` 的程序在移植到新机器或新编译器后仍然有效，通常需要对该程序进行某些改变。

##### 3. const 和 volatile

用 volatile 修饰一个对象时，它的行为和 const 几乎一摸一样。

例如，和 const 一样：

* **从非 volatile 到 volatile 的转换是允许的**，因为这是一种"添加限定符"的转换，被认为是安全的。当把一个普通 int 赋值给 volatile int 时，编译器会生成必要的代码来确保访问遵循 volatile 语义（即不进行优化，每次都从内存读取）；相反的，从 volatile 到非 volatile 的转换是不允许的，因为这是一种“删除限定符”的操作，被认为是不安全的。
* 一个非 volatile 指针可以指向 volatile 对象；但一个 volatile 指针不能指向非 volatile 对象
* volatile 对于指针而言同样有顶层和底层之分。

##### 4. 多线程和 volatile

当两个线程都要用到某一个变量且该变量的值会被改变时，应该用 volatile 声明，该关键字的作用是防止优化编译器把变量从内存装入 CPU 寄存器中。如果变量被装入寄存器，那么两个线程有可能一个使用内存中的变量，一个使用寄存器中的变量，这会造成程序的错误执行。volatile 的意思是让编译器每次操作该变量时一定要从内存中真正取出，而不是使用已经存在寄存器中的值，如下：

```C++
volatile bool bStop  =  FALSE;
```

* 在一个线程中：

```C++
while(!bStop)  {  /* ... */  }  
bStop = false;  
return;    
```

* 在另外一个线程中，要终止上面的线程循环：

```C++
bStop = true;  
while(bStop);  //等待上面的线程终止，如果bStop不使用volatile申明，那么这个循环将是一个死循环，因为bStop已经读取到了寄存器中，寄存器中bStop的值永远不会变成FALSE，加上volatile，程序在执行时，每次均从内存中读出bStop的值，就不会死循环了。
```

这个关键字是用来设定某个对象的存储位置在内存中，而不是寄存器中。因为一般的对象编译器可能会将其的拷贝放在寄存器中用以加快指令的执行速度，例如下段代码中：

##### 5. 类和 volatile

就想一个类可以定义 const 成员一样，他也可以将成员定义成 volatile。

* const 对象只能调用类的 const 属性成员函数。
* volatile 对象只能调用类的 volatile 属性成员函数。

##### 6. 合成的拷贝对 volatile 对象无效

const 和 volatile 的一个重要区别是我们不能使用合成的拷贝/移动构造函数以及赋值运算符初始化 volatile 对象或从 volatile 对象赋值。

因为合成的成员默认接受的形参类型是 `const T&`，也即没有 volatile 属性，显然我们不能把一个非 volatile 的引用绑定到一个 volatile 对象上。

如果一个类希望拷贝、移动或构造它的 volatile 对象，则该类必须自定义这些操。具体的，便是将形参类型指定为 `const volatile T&`。

#### 8.4 链接指示：extern “C”

##### 1. 链接指示的作用

C++ 语言有时需要调用其他语言编写的函数：

* 像其他所有函数一样，其他语言中的函数名字也必须在 C++ 中声明（指定返回类型和形参列表）。
* 对于其他语言编写的函数来说，编译器检查函数调用的方式与普通 C++ 函数的方式相同，但是由于语言特征等方面存在较大的差异，编译器生成的代码有所区别。
  * 以 C 语言为例，C++ 由于支持函数重载，会重整函数的名字；
  * 而 C 语言不支持函数重载，它没有这样的操作。
  * 因此对于返回类型和形参完全相同的 C 和 C++ 函数来说，它们的函数名实际上是不同的。

为了解决不同语言之间编译器生成代码不同导致的名字不一致问题，对于非 C++ 语言编写的代码，需要使用链接指示来标识：

* 要注意的是，要想把 C++ 代码和其他语言编写的代码放在一起使用，我们必须有权访问该语言的编译器，并且这个编译器与当前的 C++ 编译器是兼容的。
* 为了解决与 C 代码的兼容性问题，C++ 最常见的是调用 C 语言编写的函数。而 C++ 与 C 都是使用 GCC 编译器。

##### 2. 不可移植

**编译器支持**：并不是所有编译器都完全支持 C 和 C++ 的链接特性。有些编译器可能在实现 `extern "C"` 时存在差异，导致不兼容。

**不同的 ABI（应用二进制接口）**：不同平台和编译器的 ABI 可能不同。这可能会导致使用 `extern "C"` 的代码在不同的环境中表现不同，尤其是当涉及到数据结构、调用约定和返回值处理时。例如：

1. **数据结构的布局**：
   - 不同 ABI 可能对数据结构的内存布局（如对齐方式和填充）有不同的规定。这意味着在一个平台上定义的结构体在另一个平台上可能具有不同的内存布局，从而导致数据解释错误。比如，在一个平台上，某个结构体的字段可能按照特定顺序排列，而在另一个平台上，顺序可能会不同。
2. **调用约定的差异**：
   - 不同的 ABI 可能使用不同的调用约定，这包括参数的传递方式、返回值的处理、栈的清理等。即使两个平台都支持 `extern "C"`，如果它们的调用约定不同，C++ 代码可能无法正确调用 C 代码，或者反之。（例如，假如：C++ 参数从左到右的顺序入栈，而 C 以从右到左的顺序入栈。）
3. **数据类型的大小和对齐**：
   - 不同平台对数据类型的大小（如 `int`、`long` 等）和对齐要求可能不同。例如，某些平台上 `long` 可能是 32 位，而在另一些平台上可能是 64 位。这会导致在 C/C++ 代码之间传递数据时产生错误，特别是在使用 `extern "C"` 进行函数调用时。
4. **异常处理和返回值**：
   - C++ 的异常处理机制与 C 不同。虽然 `extern "C"` 可以用于使 C++ 函数可被 C 调用，但如果 C++ 函数抛出异常并被 C 代码调用，可能会导致不可预测的行为，尤其是当 ABI 不兼容时。
5. **链接和符号解析**：
   - 不同的 ABI 可能影响链接器如何处理符号。比如，某些 ABI 可能会对符号名称进行不同的修饰或处理，这可能导致链接错误或运行时错误。
6. **平台特定的特性**：
   - 某些平台可能实现了特定的 ABI 优化或功能，这些特性在其他平台上可能不存在。这会导致基于某一平台的代码在其他平台上无法正常工作。

##### 3. 链接指示形式

链接指示必须出现在每个非 C++ 函数的声明处。

* 链接指示有两种形式：单个的或复合的。
* 链接指示不能出现在函数或类的内部。
* 链接指示可以**嵌套**，例如头文件包含带自带链接指示的函数，则该函数的链接不受影响。

``` C++
// 单个的: 在函数声明之前添加 extern "Language" 即可
extern "C" size_t strlen(const char *);
// 复合的: 用花括号括起来
extern "C" {
    int strcmp(const char*, const char *);
    char *strcat(char *, const char *);
}
```

##### 4. 链接指示与头文件

C++ 标准库中的 C 语言头文件通常使用复合链接指示方式包含，例如 C++ 的 `cstring` 头文件可能形如：

``` C++
extern "C" {
    #include <string.h>
}

// '#' not expected hereC/C++(10)
// extern "C" #include <string.h>
```

注意我们要用符合形式包含头文件，这是因为 `#include <string.h>` 是一个宏操作，它在预处理之后会展开，形如：

``` cpp
extern "C"
{
    namespace std _GLIBCXX_VISIBILITY(default)
    {
        _GLIBCXX_BEGIN_NAMESPACE_VERSION

        using ::memchr;
        using ::memcmp;
        using ::memcpy;
        using ::memmove;
        using ::memset;
        using ::strcat;
        using ::strchr;
        using ::strcmp;
        using ::strcoll;
        using ::strcpy;
        using ::strcspn;
        using ::strerror;
        using ::strlen;
        using ::strncat;
        using ::strncmp;
        using ::strncpy;
        using ::strpbrk;
        using ::strrchr;
        using ::strspn;
        using ::strstr;
        using ::strtok;
        using ::strxfrm;

#ifndef __CORRECT_ISO_CPP_STRING_H_PROTO
        inline void *
        memchr(void *__s, int __c, size_t __n)
        {
            return __builtin_memchr(__s, __c, __n);
        }

        inline char *
        strchr(char *__s, int __n)
        {
            return __builtin_strchr(__s, __n);
        }

        inline char *
        strpbrk(char *__s1, const char *__s2)
        {
            return __builtin_strpbrk(__s1, __s2);
        }

        inline char *
        strrchr(char *__s, int __n)
        {
            return __builtin_strrchr(__s, __n);
        }

        inline char *
        strstr(char *__s1, const char *__s2)
        {
            return __builtin_strstr(__s1, __s2);
        }
#endif

        _GLIBCXX_END_NAMESPACE_VERSION
    } // namespace
}
```

##### 5. 链接指示与函数指针

对于引用的其它语言编写的函数，我们可以理解为**编写该函数所用的语言也是函数类型的一部分**。这意味着：

* 对于使用链接指示的函数来说，它的每个声明都必须使用链接指示而且链接指示必须相同。
* 如果我们通过链接指示声明了一个函数指针，那么该函数指针只能指向相同链接指示的函数。
* 显然，C++ 中声明的函数指针不能指向 `extern "C"` 声明的函数。

并且，**对于函数指针来说，链接指示对整个声明都有效。也即，**当我们使用链接指示时，它不仅对函数有效，而且对**作为返回类型或形参类型的函数指针**也有效。

``` C++
// fp是一个函数指针，它的形参是一个函数指针
extern "C" void (*fp)(void (*)(int));
```

* 对于函数指针 `fp` 来说，`fp` 必须指向从 C 语言引入的函数，并且它的函数指针参数也必须是指向 C 语言引入的函数。

因为链接指示作用于声明语句的所有函数（以及函数指针），所以如果我们希望给 C++ 函数传入一个指向 C 函数的指针，则必须使用类型别名：

``` C++
extern "C" typedef void FC(int);
void f2(FC *);
```

##### 6. 链接指示与条件编译

C++ 作为 C 语言的超集，需要向后兼容 C 语言的代码。为了同时兼容 C 和 C++ 的代码，常用条件编译配合 `extern "C"`：

```cpp
#ifdef __cplusplus  // 检查是否是C++编译器
extern "C"          // 仅在C++环境中使用extern "C"
#endif
int strcmp(const char*,const char*);
```

* C++编译器会自动定义宏 `__cplusplus`，可以用它来区分编译环境

这种与条件编译配合来实现 C/C++ 兼容的处理技术在 STL 库中很常用。

##### 7. 链接指示与函数重载

链接指示和重载函数的相互作用依赖于目标语言。例如：

* 对于 C 语言来说，它不支持函数重载，那么下面的代码是错误的：

``` C++
extern "C" int cmp(int a, int b);
extern "C" int cmp(double a, double b);

// main.cpp:7:16: error: conflicting declaration of C function 'int cmp(double, double)'
//     7 | extern "C" int cmp(double a, double b);
//       |                ^~~
// main.cpp:6:16: note: previous declaration 'int cmp(int, int)'
//     6 | extern "C" int cmp(int a, int b);
```

* 而对于 C++ 来说，他支持函数重载，那么下面的代码没有问题：

``` C++
extern int cmp(int a, int b);
extern int cmp(double a, double b);
```

##### 8. 导出 C++ 函数到其他语言

链接指示按照指定语言生成代码的特性可以在 C++ 中编写的代码在其他语言编写的函数程序中可用：

``` C++
// calc 函数可以被 C 程序使用
extern "C" double calc(double dparm);
```

* 编译器将为该函数生成 C 语言的二进制数据。







