# 三、基础议题

## Q1. 指针和引用的区别

关于指针和引用的区别，在 [Reddit](https://www.reddit.com/r/cpp/comments/18186it/i_am_absolutely_confused_on_the_topic_of/) 上有一个讨论，其中高赞回答说的比较清楚了，简而言之，C++  标准只是定义了引用的 **语义**，至于怎么 **实现** 这种语义，是编译器厂商自己的行为，你可以将引用在底层实现为 `const T*`，也可以将引用完全优化掉，毕竟引用只是一个别名。

[C++FAQ](https://isocpp.org/wiki/faq/references#overview-refs)：

> "Important note: Even though a reference is often implemented using an address in the underlying assembly language, please do not think of a reference as a funny looking pointer to an object.  A reference is the object, just with another name.  It is neither a pointer to the object, nor a copy of the object.  It is the object.  There is no C++ syntax that lets you operate on the reference itself separate from the object to which it refers."

在 **语义** 层面上，引用和指针的行为差别完全就是八股文了：

* 引用永远不能为空，因此创建时必须初始化，并且不能重新赋值

下面是高赞的具体回答：

> ### Pointer’s Semantics
>
> A pointer is a distinct ***value type***. It has a defined size on a given platform, which can be obtained by `sizeof(void*)`. On a platform with a true 64 bit address space, this will return `8`. All pointers to any object type will have this same size: `sizeof(T*) == sizeof(U*)`. The *value* of a pointer is an actual, real memory location. You can introspect this directly; if you print a pointer without dereferencing it, the value printed is the memory address value the pointer holds.
>
> <font color=blue>Basically, pointers are “true” value types in an of themselves and can ***exist independently*** of having an object to point to. This is why you can have a null pointer.</font> And since they are true value types, they have their own definition of the following operations, independent of the type they point to:
>
> - To compare a pointer means to compare the memory address, not the content at that memory address.
> - To copy/assign a pointer means to copy/assign the memory address, not the content at that memory address.
> - Taking the address of a pointer will produce the address of the *pointer object itself*, not the pointed to content.
> - “Dereferencing” a pointer produces a ***reference*** to the pointed to content. I know, it’s confusing. It made more sense in C when there wasn’t a distinct thing called a reference, so pointers were said to have “reference semantics” and thus “dereferencing” them produced the actual content.
>
> When we say a type is **“pointer like”**, we mean it models the properties above. This is why the smart pointers behave the way they do. It’s also why `std::span` is so confusing. `std::span` looks like a container, so you expect it to behave the way a reference to a container would: comparing it would compare the elements in the span. But you can reassign spans, and that assignment doesn’t reassign the underlying elements, it just reassigns the underlying pointer to point to new elements. So to be consistent, the standard chose the lesser of two evils and made span model a pointer, and thus has pointer semantics all around. When you compare a span, you’re asking “do these spans provide a view onto literally the same content in memory,” just like comparing a pointer. In C++20 they formalized this “container reference API but pointer semantics” notion with the view concept.
>
> ### Reference’s Semantics
>
> ***A reference is not actually a distinct type***, just like that the second quote states: there is no actual object that “represents” a reference. You cannot take the address of the reference itself. If you do `sizeof(T&)`, you will get the same answer as `sizeof(T)`. So most accurately, a reference is an ***alias*** for the actual instance of an object.
>
> Because references are ***not true value types*** in and of themselves, but instead are aliases for the instance, they have the following semantics:
>
> - They must be “bound” to an instance at initialization. A reference can never be “null.”
> - They cannot be re-assigned. Assigning to a reference after it has been “bound” to an instance is equivalent to assigning to the bound instance, not the reference.
> - Comparing references is equivalent to comparing the referenced instances.
> - Taking the address of a reference gives the address of the bound instance.
>
> ### Implementations
>
> As discussed above, a pointer is an explicitly defined value type. They take up memory on the stack/heap.
>
> A reference, on the other hand, is not an explicitly defined value type. Because a reference isn’t a “real” value type, but instead defined as an alias to the bound instance, <font color=blue>the compiler is free to implement references however it wants, as long as it ***maintains the semantics***.</font>
>
> This means a compiler is free to *optimize out a reference completely*. The compiler cannot inherently do this with a pointer: a pointer has to actually exist because you can take its address, reassign it, etc. However, the compiler is just as free to implement a reference as a pointer (in the machine sense, see below), and often will do so if the reference passes between functions that cannot be inlined.

----

不过，纸上得来终觉浅，我们可以从汇编的角度看一下，编译器到底怎么实现指针和引用的，编译器版本为：`g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`

``` cpp
// test.cpp
int main()
{
    int a = 0x1234abcd;
    int *pa = &a;
    int &ra = a;
    ++ (*pa);
    ++ ra;
}
```

对上面的程序，得到其汇编代码：`g++ -S test.cpp -o test.s -O0`

``` cpp
	.file	"test.cpp"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	; 栈帧建立
	pushq	%rbp              ; 保存旧的 %rbp（基指针）
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp        ; 设置新的栈基址 %rbp = %rsp（栈指针）
	.cfi_def_cfa_register 6
	subq	$32, %rsp         ; 分配 32 字节栈空间

	; 栈溢出保护
	movq	%fs:40, %rax      ; 从线程局部存储（TLS）加载栈保护值
	movq	%rax, -8(%rbp)    ; 将保护值存入栈（位于局部变量之后）
	xorl	%eax, %eax        ; 清零 %eax（同时避免优化干扰）

	; 变量初始化
	movl	$305441741, -28(%rbp) ; 局部变量 int a = 305441741
	leaq	-28(%rbp), %rax       ; %rax = &a 
	movq	%rax, -24(%rbp)       ; int *p1 = &a
	leaq	-28(%rbp), %rax       ; %rax = &a
	movq	%rax, -16(%rbp)       ; int *p2 = &a

	; 指针解引用与修改
	movq	-24(%rbp), %rax       ; %rax = p1
	movl	(%rax), %eax          ; %eax = *p1（即 a 的值）
	leal	1(%rax), %edx         ; %edx = *p1 + 1
	movq	-24(%rbp), %rax       ; %rax = p1
	movl	%edx, (%rax)          ; *p1 = *p1 + 1（a++）

	movq    -16(%rbp), %rax      ; %rax = p2
	movl    (%rax), %eax         ; %eax = *p2（即 a 的值）
	leal    1(%rax), %edx        ; %edx = *p2 + 1
	movq    -16(%rbp), %rax      ; %rax = p2
	movl    %edx, (%rax)         ; *p2 = *p2 + 1（a++）

	; 函数返回
	movl	$0, %eax  			 ; main 函数返回值
	movq    -8(%rbp), %rdx       ; 加载保存的栈保护值
	subq    %fs:40, %rdx         ; 与当前值比较
	je      .L3                  ; 如果一致，跳转到 .L3
	call    __stack_chk_fail@PLT ; 否则调用栈保护失败处理
.L3:
	leave						 ; 恢复 %rsp 和 %rbp
	.cfi_def_cfa 7, 8
	ret						     ; 返回 0
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0" ; 编译器标识
	.section	.note.GNU-stack,"",@progbits             ; ELF 节信息
	.section	.note.gnu.property,"a"				     ; ELF 节信息
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
```

可以看到，在机器码层面，指针和引用的效果都是一样的。

*reference*：https://zhuanlan.zhihu.com/p/89175296

## Q2. 尽量使用 C++ 风格的类型转换

C 风格类型转换和函数风格类型转换有以下问题：

1. 没有类型检查，它们允许你在任何类型之间进行转换
2. 在程序语句中难以识别，人工阅读很可能忽略这些语句，而利用类似 grep 的工具也不能从语句构成上区分出它们来

### 2.1 `static_cast`

用于执行有理智的、相关的常规类型转换。

### 2.2 `const_cast`

去掉表达式的 const 或 volatile 属性，并且被限制只能做这些事，如果你尝试用 const_cast 完成 int 到 float 的转换会编译报错。

> 去掉 volatile 属性的用途容易被忽略。

### 2.3 `dynamic_cast`

安全的沿着类的继承关系向下进行类型转换，例如将基类的指针或引用转换为派生类的指针或引用，并且附带类型检查：失败时返回 NULL。

### 2.4 `interpret_cast`

通过 interpret_cast 进行的转换结果几乎都是 ***implement-defined***。因此，使用 interpret_cast 的代码很难移植。

为什么 `reinterpret_cast` 难以移植？

1. **依赖底层实现：** `reinterpret_cast` 用来在指针类型和其它类型（指针类型、数值类型等）之间进行转换，而不需要编译器进行任何检查。它基本上是告诉编译器：“相信我，我知道我在做什么，把这些位当作另一种类型来解释。” 这意味着它直接操作内存地址和底层数据表示，而这些表示在不同的编译器、操作系统或硬件架构上可能完全不同。
2. **不进行类型检查和安全转换：** 与 `static_cast`（用于执行有理智的、相关的类型转换）或 `dynamic_cast`（用于在继承层次结构中进行安全的运行时转换）不同，`reinterpret_cast` 不会执行任何类型安全检查，也不会进行任何必要的调整（例如，在多重继承中调整指针偏移量）。它只是简单地重新解释现有位的含义。
3. **对齐和大小问题：** 不同的数据类型可能有不同的内存对齐要求和大小。例如，一个 `int*` 被 `reinterpret_cast` 转换为 `char*` 通常是安全的，但反过来，将一个任意的 `char*` 转换为 `int*` 并在其指向的地址上直接解引用，如果该地址没有正确对齐到 `int` 的边界，就可能导致未定义行为（undefined behavior），甚至程序崩溃。
4. **未定义行为的风险：** 使用 `reinterpret_cast` 很容易导致未定义行为。一旦发生未定义行为，程序的行为就不可预测，这使得调试变得极其困难，而且在不同的系统上可能表现出不同的错误，甚至看起来完全正常，但实际上已经损坏了数据。

## Q3. 不要对数组使用多态

不要对数组使用多态，本质上来说是**“不要混合使用多态和指针算法”**，因为数组使用多态出错的情况一般是我们使用了 `for` 循环（值得注意 `delete[]` 底层上也是通过循环实现的）。

`for` 循环本质上就是通过对指向对数组首指针一直 `+1` 实现的，这里的 `+1` 操作依赖于元素的大小 `size`，它是固定的，一般来说是根据数组的静态类型决定的。

而对于多态来说，在数组 `Base arr[];` 和数组 `Derived arr[];` 上执行 `for` 循环显然不能是一回事，因为 `Base` 和 `Derived` 大小不相等的情况是很常见的。如果我们的 `+1` 是依赖于基类 `Base` 的大小，那么如果我们传入一个 `Derived` 类型的数组，`for` 循环就显然不正确了。

> ### 数组的构造与析构顺序
>
> ``` cpp
> struct Node {
>     int x;
>     Node(int _x) : x(_x) { cout << "ctor:" << x << endl; }
>     ~Node() { cout << "dtor:" << x << endl; }
> };
> 
> int main()
> {
>     Node *arr = new Node[3]{1, 2, 3};
>     delete[] arr;
>     return 0;
> }
> // ctor:1
> // ctor:2
> // ctor:3
> // dtor:3
> // dtor:2
> // dtor:1
> ```
>
> 构造顺序，析构逆序。

## Q4. 避免无用的缺省构造函数

世界很大，对于有些 class 来说，它们不利用外部数据进行初始化就是不合理的，因此，请不要画蛇添足。 

不过事实来说，如果 class 没有提供缺省构造函数，它在使用上确实会遇到一些问题，一般是需要默认构造函数的地方。

不过，如果我们强行提供默认构造函数，例如为一些成员提供“无意义值”来实现初始化，可能导致程序性能的降低以及代码的膨胀，因为此时我们不能保证所有成员的值都是“有意义”的，因此我们就需要在成员函数中添加“合法性检查”相关的代码，通常是 `if` 语句，而 `if` 分支会导致分支预测等问题，降低代码性能，同时大量的“合法性检查”又会膨胀代码，并且是大量重复代码，很难说这是一种好的设计。

# 四、运算符

## Q5. 谨慎定义类型转换函数

**让编译器进行隐式类型转换造成的弊端往往大于它所带来的好处**，所以除非你确实需要，不要定义类型转换函数。

解决单参数构造函数隐式类型转换的问题：

1. explicit 关键字
2. 在不能使用 explicit 关键字的情况下，使用 nested/proxy class，其原理是 **编译器只能进行一步隐式类型转换**

解决隐式类型转换函数会隐式类型转换的问题：

1. 使用 `int toint()` 替换 `operator int()`

## Q6. 自增、自减操作符前缀形式与后缀形式的区别

``` cpp
struct X
{
    int _val;

    X(int val) : _val(val) {}

    // prefix increment
    X& operator++()
    {
        ++ _val;
        return *this; // return new value by reference
    }
 
    // postfix increment
    const X operator++(int)
    {
        X old = *this; // copy old value
        operator++();  // prefix increment
        return old;    // return old value
    }
 
    // prefix decrement
    X& operator--()
    {
        -- _val;
        return *this; // return new value by reference
    }
 
    // postfix decrement
    const X operator--(int)
    {
        X old = *this; // copy old value
        operator--();  // prefix decrement
        return old;    // return old value
    }
    X& modify(int val) {
        _val = val;
        return *this;
    }
};
```

* 在后缀自增中使用前缀自增，提高代码复用率

一个很关键的问题，为什么后缀自增返回的是 `const X` 而不是 `X` ?

1. 首先，从与内置类型行为一致的角度出发，我们应该返回 `const X`，因为对于 `int` 类型来说，`x++++` 是不合法的。因此对于我们自定义的类型，`X++++` 也应该是不合法的。
2. 两次后缀自增的结果可能并不符合我们的预期，并产生与直觉相悖的结果，在 `x++++` 中，`x` 只自增了一次，第二次递增针对的是`(x++)` 返回的临时对象。

## Q7. 不要重载 `&&`，`||` 或 `,`

`||` 和 `&&` 运算符有短路求值的特性，而我们自己重载的版本并不能保证短路求值。

`,` 表达式自左向右计算，并返回右侧表达式的值，但是我们自己重载的版本并不能保证自左向右求值。

## Q8. 理解各种不同含义的 new 和 delete

``` cpp
string *ps = new string("Memory Management");
// equals to
void *rp = operator new(sizeof(string));
rp->string("Memory Management");
string *ps = static_cast<string*>(rp);

delete ps;
// equals to
ps->~string();
operator delete(ps);
```

# 五、异常

## Q9. 使用析构函数防止资源泄露

对操纵局部资源的 raw pointer 说再见，基于 RAII 思想的 smart pointer 是更好的选择。

## Q10. 在构造函数中防止资源泄露

当构造函数抛出异常时，对象确实没有被完全构造出来，因此它的析构函数不会被调用。这意味着任何在异常抛出前已经成功分配的资源（但尚未被正确管理）将会泄露。

对此，直接使用 RAII 管理资源（如动态分配的内存、文件句柄、网络连接等），是最简单、最清晰，最容易维护的方案。

## Q11. 禁止异常信息传递到析构函数外

禁止异常传递到析构函数外有两个原因：

1. 在异常 stack-unwinding 的过程中，防止 terminate 被调用
2. 确保析构函数总能完成我们希望它做的所有事情，即析构函数的所有内容都得到执行

那么，如果析构函数可能抛出异常，该如何处理呢？

处理析构函数中可能抛出异常的情况，核心思想是**确保异常不会逸出析构函数**。最佳实践是：

1. **在析构函数内部使用 `try-catch` 块捕获并处理所有可能抛出的异常**，记录错误但**绝不重新抛出**。
2. 对于需要显式错误处理的清理操作，提供一个**独立的公共成员函数**，让用户在对象销毁前调用并处理其可能抛出的异常。析构函数作为备用，仍然调用此函数但捕获其异常。
3. 利用 C++11 的 `noexcept` 关键字，显式声明析构函数不会抛出异常，以获得更好的编译器优化和更清晰的代码意图。

### 1. 在析构函数内部捕获并处理异常

这是最常见且推荐的做法。如果析构函数中的某个操作可能抛出异常，应该使用 `try-catch` 块将其包围起来。

```cpp
#include <iostream>
#include <fstream>
#include <stdexcept>

class MyFileHandler {
private:
    std::ofstream file_;
    std::string filename_;

public:
    MyFileHandler(const std::string& filename) : filename_(filename) {
        file_.open(filename, std::ios::out);
        if (!file_.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        std::cout << "File " << filename << " opened successfully." << std::endl;
    }

    void writeData(const std::string& data) {
        if (file_.is_open()) {
            file_ << data << std::endl;
            std::cout << "Data written: " << data << std::endl;
        } else {
            throw std::runtime_error("File is not open for writing.");
        }
    }

    // 析构函数
    ~MyFileHandler() {
        std::cout << "MyFileHandler destructor called for " << filename_ << std::endl;
        try {
            if (file_.is_open()) {
                // 模拟一个可能抛出异常的关闭操作（例如，磁盘I/O错误）
                // 实际的 std::ofstream::close() 通常不会抛出异常，这里是为了演示
                // 如果是更复杂的资源（如网络连接、数据库事务），关闭时可能抛出异常
                // 例如：some_complex_resource.close_and_flush_might_throw();

                // 假设这里是文件关闭操作，我们捕获它可能抛出的任何异常
                file_.close(); // std::ofstream::close() 理论上不会抛出，但为了演示
                std::cout << "File " << filename_ << " closed successfully." << std::endl;
            }
        } catch (const std::exception& e) {
            // 捕获标准异常
            std::cerr << "ERROR: Exception caught in MyFileHandler destructor for "
                      << filename_ << ": " << e.what() << std::endl;
            // 记录日志，但不要重新抛出异常
        } catch (...) {
            // 捕获所有其他未知异常
            std::cerr << "ERROR: Unknown exception caught in MyFileHandler destructor for "
                      << filename_ << std::endl;
            // 记录日志，但不要重新抛出异常
        }
    }
};

int main() {
    try {
        MyFileHandler handler("test.txt");
        handler.writeData("Hello, C++ Destructor!");
    } catch (const std::exception& e) {
        std::cerr << "Exception in main: " << e.what() << std::endl;
    }

    // 演示析构函数中捕获异常的情况
    // 假设这里有一个场景，析构函数中的某个操作会失败
    // (在实际中，需要模拟一个会抛异常的资源关闭函数)
    // 例如：
    // class BadCloseResource {
    // public:
    //     ~BadCloseResource() {
    //         throw std::runtime_error("Error during resource cleanup!");
    //     }
    // };
    // {
    //     BadCloseResource bad_res; // 这里析构时会抛异常，导致 terminate
    // }

    std::cout << "\nProgram finished normally." << std::endl;
    return 0;
}
```

关键点：

- **捕获所有异常类型**：使用 `catch (const std::exception& e)` 捕获标准库异常，并使用 `catch (...)` 捕获所有其他未知异常。
- **记录错误信息**：在 `catch` 块中，应该将错误信息记录下来（例如，打印到 `std::cerr`、写入日志文件或发送到监控系统）。
- **不要重新抛出异常**：这是最重要的规则。一旦在析构函数中捕获了异常，就**绝对不能**再重新抛出它。如果重新抛出，将导致程序终止（`std::terminate()` 被调用），特别是在栈展开过程中已经有其他异常活跃时。

### 2. 将可能抛出异常的操作移出析构函数

如果某个清理操作确实可能失败，并且你希望调用者能够显式地处理这个失败（例如，重试、报告给用户），那么这个操作就不应该放在析构函数中。相反，提供一个公共的、可显式调用的清理方法。

```cpp
#include <iostream>
#include <stdexcept>
#include <string>

class MyNetworkConnection {
private:
    bool connected_;
    std::string server_address_;

public:
    MyNetworkConnection(const std::string& address) : server_address_(address), connected_(false) {
        // 模拟连接操作
        if (address == "bad.server.com") {
            throw std::runtime_error("Failed to connect to " + address);
        }
        connected_ = true;
        std::cout << "Connected to " << server_address_ << std::endl;
    }

    // 显式关闭连接的方法，可以抛出异常
    void disconnect() {
        if (connected_) {
            // 模拟一个可能抛出异常的断开连接操作
            // 例如：网络故障、服务器无响应等
            // if (some_network_error_occurs()) {
            //     throw std::runtime_error("Network error during disconnect!");
            // }
            std::cout << "Disconnected from " << server_address_ << std::endl;
            connected_ = false;
        } else {
            std::cout << "Already disconnected from " << server_address_ << std::endl;
        }
    }

    // 析构函数，确保资源被清理，但捕获所有异常
    ~MyNetworkConnection() {
        std::cout << "MyNetworkConnection destructor called for " << server_address_ << std::endl;
        try {
            // 在析构函数中调用 disconnect() 时，必须捕获其可能抛出的异常
            disconnect();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception caught in MyNetworkConnection destructor for "
                      << server_address_ << ": " << e.what() << std::endl;
            // 记录日志，不重新抛出
        } catch (...) {
            std::cerr << "ERROR: Unknown exception caught in MyNetworkConnection destructor for "
                      << server_address_ << std::endl;
        }
    }
};

int main() {
    try {
        MyNetworkConnection conn("good.server.com");
        // 用户可以在这里显式调用 disconnect() 并处理异常
        // conn.disconnect();
    } catch (const std::exception& e) {
        std::cerr << "Exception in main: " << e.what() << std::endl;
    }

    std::cout << "\nProgram finished normally." << std::endl;
    return 0;
}
```

关键点：

- **分离职责**：将可能失败的清理逻辑从析构函数中分离出来，放入一个公共方法。
- **用户控制**：允许用户在对象销毁前显式地调用该方法，并处理可能抛出的异常。
- **析构函数作为备用**：析构函数仍然负责清理资源，但它会**安静地**处理任何异常，作为一种“安全网”，确保即使调用者忘记调用显式清理方法，资源也能在对象销毁时得到处理。

### 3. 使用 `noexcept` 关键字（C++11 及更高版本）

从 C++11 开始，析构函数默认是 `noexcept` 的，除非它们调用的某个函数不是 `noexcept`。显式地将析构函数声明为 `noexcept` 可以向编译器和读者表明该函数不会抛出异常。

```cpp
class MyClass {
public:
    // 显式声明析构函数不会抛出异常
    ~MyClass() noexcept {
        // 内部所有操作都必须是 noexcept 或在内部捕获异常
        // 如果这里真的抛出了异常，程序会调用 std::terminate()
    }
};
```

影响：

- **编译器优化**：`noexcept` 承诺允许编译器进行某些优化，因为它知道不需要为异常处理生成额外的代码。
- **强制终止**：如果一个声明为 `noexcept` 的函数（包括析构函数）在运行时确实抛出了异常，程序会立即调用 `std::terminate()`，导致程序终止。这强化了析构函数不应抛出异常的原则。

## Q12：理解“抛出一个异常”与“传递一个参数”和“调用一个函数”间的差异

把一个对象传递给函数或一个对象调用虚拟函数与把一个对象做为异常抛 出，这之间有三个主要区别。

第一、异常对象在传递时总被进行拷贝；当通过传值方式捕获时，异常对象被拷贝了两次。对象做为参数传递给函数时不一定需要被拷贝。

第二、对象做为异常被抛出与做为参数传递给函数相比，前者类型转换比后者要少（前者只有两种转换形式）。

最后一点，catch 子句进行异常类型匹配的顺序是它们在源代码中出现的顺序，第一 个类型匹配成功的 catch 将被用来执行。当一个对象调用一个虚拟函数时，被选择的函数位 于与对象类型匹配最佳的类里，即使该类不是在源代码的最前头。

## Q13：通过引用捕获异常

**（1）catch by pointer**

好处：拷贝异常对象时只需要一个指针（8B）的开销。

坏处：大大的坏处，指针不能指向局部对象（局部对象先一步被释放），更不能指向动态分配的对象（catch 不知道是否应该 delete 掉，因为接受的异常不一定都是动态分配的内存），只能指向静态对象或全局对象（限制太大了）。

**（2）catch by value**

由于接受的是源异常对象拷贝后的对象，不会有指向被释放对象的问题。

需要拷贝源对象两次（throw 时一次，传参时一次），此外对于类对象而言，还会有 slicing problem。

**（3）catch by reference**

只需要拷贝一次，没有对象切割问题

## Q14：审慎使用 *exception specifications*

注意，**C++ 中的异常规范（exception specifications）已经被弃用**了。

* 在 **C++11** 标准中，动态异常规范（如 `throw(std::bad_alloc)`）被标记为**弃用（deprecated）**。这意味着它们不推荐使用，但在语法上仍然有效，编译器可能会发出警告。
* 到了 **C++17** 标准，除了 `throw()`（这等同于 `noexcept` 或 `noexcept(true)`）之外，所有的动态异常规范都**被彻底移除了**。

> 为什么是弃用而不是移除？
>
> 在 C++ 标准中，通常会先**弃用（deprecated）**一个特性，而不是直接移除。这给了现有代码库一个过渡期来适应新的标准。弃用意味着该特性仍然可以使用，但强烈不推荐，并且在将来的某个版本中可能会被彻底移除。
>
> 因此，虽然在 C++17 中 `throw()` 被弃用了，但你可能仍然能在一些旧代码中看到它。对于新代码，你应该始终使用 `noexcept` 来表示函数不抛出异常。

### 1. 为什么被弃用和移除？

异常规范最初的目的是为了让编译器在编译时检查函数可能抛出的异常，从而帮助开发者编写更健壮的代码。然而，它们存在一些严重的问题：

1. **运行时检查，而非编译时检查：** 尽管看起来像编译时检查，但实际上，如果一个函数抛出了异常规范中未列出的异常，程序会在运行时调用 `std::unexpected`，这通常会导致程序终止，而不是提供一个优雅的错误处理机制。这使得它更像一个运行时断言，而不是一个可靠的类型系统特性。
2. **复杂性和效率问题**：为了支持这种运行时检查，编译器需要生成额外的代码，这可能会带来不必要的开销和复杂性，尤其是在处理异常栈展开时。
3. **不实用：** 在大型项目中，维护准确的异常规范变得极其困难和不切实际。函数可能调用的其他函数可能会抛出新的异常，导致需要频繁更新异常规范。
4. **阻碍优化：** 异常规范的存在可能会阻碍编译器进行某些优化，因为编译器需要考虑异常抛出的可能性。
5. **与现代 C++ 异常安全模型不符**：现代 C++ 更强调“异常安全保证”（Exception Safety Guarantees），即强异常安全、基本异常安全和不抛出保证。`throw()` 并没有很好地融入这种模型。

### 2. 替代方案：`noexcept`

C++11 引入了 **`noexcept` 关键字**作为异常规范的更现代和有效的替代品。

- **`noexcept`**：表示函数**不会抛出任何异常**。如果一个声明为 `noexcept` 的函数在运行时抛出了异常，程序会立即调用 `std::terminate`。这允许编译器进行更积极的优化。
- **`noexcept(true)`**：与 `noexcept` 相同，表示不抛出异常。
- **`noexcept(false)`**：表示函数**可能抛出异常**。
- **`throw()`**：在 C++11 之后等同于 `noexcept` 或 `noexcept(true)`。

`noexcept` 的主要优势在于它是一个**契约保证**，而不是一个检查机制。它告诉编译器和用户，如果函数抛出异常，那么程序将终止。这使得编译器可以做出更强的优化假设，例如在栈展开时不需要捕获和传播异常。

- **编译时强制**：如果一个函数被声明为 `noexcept`，并且它内部调用的某个函数不是 `noexcept` 或抛出了异常，那么编译器可以发出警告或错误（尽管这取决于编译器实现）。更重要的是，如果一个 `noexcept` 函数在运行时真的抛出了异常，程序会直接调用 `std::terminate()`。这意味着一旦你承诺 `noexcept`，编译器就会假设并优化，如果违背承诺，程序会立即终止，而不是尝试进行复杂的运行时异常处理。
- **更好的优化**：`noexcept` 提供了更强的保证，允许编译器进行更积极的优化，因为它可以确定不需要为异常处理生成栈展开的代码。
- **明确的意图**：它清晰地表达了函数不抛出异常的意图。

## Q15：了解异常处理的系统开销

为了在运行时处理异常，程序要记录大量的信息。无论执行到什么地方，程序都必须能够识别出如果在此处抛出异常的话，将要被释放哪一个对象；程序必须知道每一个入口点， 以便从 try 块中退出；对于每一个 try 块，他们都必须跟踪与其相关的 catch 子句以及这些 catch 子句能够捕获的异常类型。这种信息的记录不是没有代价的。

虽然确保程序满足异常规格不需要运行时的比较，而且当异常被抛出时也不用额外的开销来释放相关的对象和匹配正确的 catch 字句。但是异常处理确是有代价的，即使你没有使用 try，throw 或 catch 关键字，你同样得付出一些代价。

### 不食用任何异常特性

当你不用异常处理时你不能让编译器生产商消除这方面的开销，因为程序一 般由多个独立生成的目标文件（object files）组成，只有一个目标文件不进行异常处理并不能代表其他目标文件不进行异常处理。而且即使组成可执行文件的目标文件都不进行异常处理，那么还有它们所连接的程序库呢？如果程序的任何一部分使用了异常，其它部分必须也支持异常。否则在运行时程序就不可能提供正确的异常处理。 

### 使用异常处理

使用异常处理的第二个开销来自于 try 块，无论何时使用它，也就是当你想能够捕获异常时，那你都得为此付出代价。不同的编译器实现 try 块的方法不同，所以编译器与编译器间的开销也不一样。粗略地估计，如果你使用 try 块，代码的尺寸将增加 5%－10% 并且运行速度也同比例减慢。这还是假设程序没有抛出异常，我这里讨论的只是在程序里使用 try 块的开销。为了减少开销，你应该避免使用无用的 try 块。 

编译器为异常规格生成的代码与它们为 try 块生成的代码一样多，所以一个异常规格一 般花掉与 try 块一样多的系统开销。

与一个正常的函数返回相比，通过抛出异常从函数里返回可能会慢三个数量级。

# 六、效率

## Q16：牢记 80-20 准则

80－20 准则说的是大约 20％ 的代码使用了 80％ 的程序资源；大约 20% 的代码耗用了大 约 80％ 的运行时间；大约 20％ 的代码使用了 80％ 的内存；大约 20％ 的代码执行 80％ 的磁盘访问； 80％ 的维护投入于大约  20％ 的代码上；

## Q17：考虑使用 lazy evaluation

将工作推迟到系统需要计算的结果时才去做。

不过需要注意的是，lazy evaluation 并不总是有用的，因为拖延计算实际本质上是无法避免计算的，实际上，如果你的计算都是重要的，lazy evaluation 可能会 减慢速度并增加内存的使用，因为除了进行所有的计算以外，你还必须维护数据结构让 lazy  evaluation 尽可能地在第一时间运行。在某些情况下要求软件进行原来可以避免的计算， 这时 lazy evaluation 才是有用的。

主流程序设计语言（e.g. C++）是使用 eager evaluation。在 C++ 中，我们可以通过类的封装性，实现 lazy evaluation 并隐藏其细节。

### 1. 引用计数

引用计数是一种 lazy evaluation，与之相对的是，eager evaluation，每次拷贝都创建一个新的副本。

### 2. 区别对待读取和写入

引用计数其实就是基于“独共享，写独占”的思想，因此我们在使用时应当能够区分对待读操作和写操作。一个容易模糊读写的例子是 `operator[]` 运算符：

``` cpp
String s = "hello,world";
cout << s[3] << endl; // call operator[] for reading
s[3] = 'A'; // call operator[] for writing
```

如果我们不能够区分 `operator[]` 用于读还是用于写，那么只能假设每次调用 `operator[]` 时都是用于写，也就是每次调用都需要 eager evaluation。

### 3. Lazy Fetching 

有时候我们需要从一个很大的对象上获取一些数据，例如一个存储在数据库上的对象实例，而访问数据库的开销是比较大的，特别是还从远程数据库中获取数据和通过网络发送数据。

在这种情况下，我们不需要读取所有数据，例如我们可能目前只需要访问一些个别字段，那么只需要单独读取这些字段，而不是读取整个对象。

### 4. Lazy Expression Evaluation

主要指针对矩阵运算这种时空复杂度都比较高的数学运算，我们可以推迟运算到运行时，例如：

``` cpp
Matrix<int> m1(1000, 1000);
Matrix<int> m2(1000, 1000);
//...
Matrix<int> m3 = m1 * m2;
// ... 没有使用 m3
m3 = m1 + m2;
```

对于 `m3 = m1 * m2`，我们就无需计算，虽然这看起来没什么意义，难道我们会在计算完 `m3 = m1 * m2` 之后，就立马计算 `m3 = m1 + m2`？实际上，在一个历经维护的系统上，上面的事情是有可能发生的。

为了实现 lazy evaluation，操作系统在底层需要建立一个数据结构，例如：

``` cpp
struct lazy_evaluation_t {
	matrix A
    matrix B
    enum class op {ADD, SUB, MUL, DIV};
}
```

来表示需要进行的运算。

另外，对于一个计算所得的矩阵，我们往往不会用到其所有元素，例如：

``` cpp
cout << m3[x][y];
```

我们可以只计算 `m3[x][y]` 这一个值。

## Q18：分期摊还期望的计算

在条款 17 我们介绍了 lazy evaluation：拖延计算的时机。在这个条款，我们将介绍一个截然相反的思想，那就是 over-eager evaluation：在要求你做某些事情以前就完成它们。

over-eager evaluation 基于一种 **分摊计算** 的思想，例如对于一个数值序列，我们可能需要访问其 max、min 和 avg，我们可以在程序中随时维护这三个值，这样在用户需要时可以无需计算直接返回。此时，虽然我们要计算比一次调用要多得多的计算，但我们把跟踪集合最小值、最大值和平均值的开销分摊到所有这些函数的调用上，每次函数调用所分摊的开销比 eager evaluation 或 lazy evaluation 要小。

通过分析下面的 Caching 和 Prefetching，我们其实可以发现，over-eager 其实是一种 **“以时间换空间”** 思想的体现。

另外就是要区分好 lazy 和 over-eager，它们看似互相矛盾，实际上是不同场景下的应用：

* lazy 适合于结算结果可能不被需要或部分需要
* over-eager 适用于计算结果几乎总是被需要且不只一次需要时

### 1. Caching

采用 over-eager 最简单的方法就是 caching 那些已经被计算出来而以后还有可能需要的值。

``` cpp
#include <map>    // 包含 std::map 的头文件
#include <string> // 包含 std::string 的头文件
#include <iostream> // 假设用于模拟数据库查找或其他输出

// 模拟数据库操作
// 假设这是一个外部函数或数据库连接，用于实际查找员工的隔间号码
// 在实际应用中，这里会包含数据库查询逻辑
int lookupCubicleInDatabase(const std::string& employeeName) {
    // 模拟数据库查询耗时和结果
    std::cout << "正在数据库中查找 " << employeeName << " 的隔间号码..." << std::endl;
    if (employeeName == "Alice") {
        return 101;
    } else if (employeeName == "Bob") {
        return 102;
    } else if (employeeName == "Charlie") {
        return 103;
    }
    return -1; // 未找到
}

/**
 * @brief 根据员工姓名查找隔间号码，并使用本地缓存提高效率。
 *
 * @param employeeName 员工的姓名。
 * @return int 员工的隔间号码。如果未找到，则返回 -1 (取决于 lookupCubicleInDatabase 的实现)。
 */
int findCubicleNumber(const std::string& employeeName) {
    // 定义静态map作为本地缓存，存储 (employee name, cubicle number) 对。
    // static 确保这个map只被初始化一次，并在函数调用之间保持其内容。
    static std::map<std::string, int> cubes;

    // 尝试在缓存中查找 employeeName 对应的条目。
    // 如果找到，迭代器 'it' 将指向该条目。
    std::map<std::string, int>::iterator it = cubes.find(employeeName);

    // 如果在缓存中没有找到条目 (it 等于 cubes.end())
    if (it == cubes.end()) {
        // 缓存未命中：咨询数据库以获取隔间号码。
        int cubicle = lookupCubicleInDatabase(employeeName);

        // 将 (employeeName, cubicle) 对添加到缓存中。
        cubes[employeeName] = cubicle;

        // 返回从数据库中查到的隔间号码。
        return cubicle;
    } else {
        // 缓存命中：'it' 指向正确的缓存条目，这是一个 (employee name, cubicle number) 对。
        // 我们只需要这个对的第二个组件（隔间号码）。
        return (*it).second;
    }
}

// 示例用法
int main() {
    std::cout << "第一次查找 Alice: " << findCubicleNumber("Alice") << std::endl; // 缓存未命中，查数据库
    std::cout << "第二次查找 Alice: " << findCubicleNumber("Alice") << std::endl; // 缓存命中，直接返回
    std::cout << "查找 Bob: " << findCubicleNumber("Bob") << std::endl;         // 缓存未命中，查数据库
    std::cout << "查找 Charlie: " << findCubicleNumber("Charlie") << std::endl;   // 缓存未命中，查数据库
    std::cout << "再次查找 Bob: " << findCubicleNumber("Bob") << std::endl;      // 缓存命中，直接返回
    std::cout << "查找 David (不存在): " << findCubicleNumber("David") << std::endl; // 缓存未命中，查数据库，返回-1
    return 0;
}
```

### 2. Prefetching

抛开底层的磁盘 IO Prefetching 不谈，事实上 Prefetching 的思想在高层语言层面上也有应用，例如 `vector` 的预分配，实质上就是一种对内存资源的 Prefetching。

## Q19：理解临时对象的来源

> $局部变量 \neq  临时对象$

对于临时对象，常常出现在函数调用过程中：

1. 传参时发生类型转换，产生一个临时对象
2. 返回一个临时对象

例如：

``` cpp
void func1(const string &s) {
    cout << s << endl;
}

void func2(string s) {
    cout << s << endl;
}

void func3(string &s) {
    cout << s << endl;
}

int main()
{
    char ch[] = "hello";
    func1(ch);
    func2(ch);
    func3(ch); // compile error
    return 0;
}
```

在上面的代码中，`char[]` 会先退化为一个 `const char*`（指向其首元素的指针），再通过 string 的构造函数 `std::string(const char *s)` 转化为一个 string 临时对象。

在上面的代码中，对 `func3` 的调用会编译错误，因为将一个临时对象传入 `T &` 意味着我们可以修改该临时对象，这显然不是我们想要的，我们要修改的应该是源对象，因此说编译器禁止 `non-const referernce` 接受临时对象。

同理的，`operator+` 返回的临时对象是 `const T`，加上 `const` 是因为修改返回的临时对象也是没有意义的。

## Q20：协助完成返回值优化

在代码中，我们应当尽量避免临时对象的产生，每产生一个临时对象都需要一次额外的拷贝和析构，当然编译器也会对临时对象做出一些优化。

在 Q19 中我们提到过，函数返回值往往会产生一个临时对象，而这个临时对象往往很难消除掉（妄想通过指针或引用消除临时对象的想法只会带来更大的麻烦），合理的思路应该是在必然有临时对象产生的情况下，尽可能减少这个临时对象带来的开销。

具体来说，对于 **匿名临时对象**，C++ 规则允许编译器对其进行优化，最常见的就是 **Return Value Optimization（RVO）**。

但要注意，RVO 对赋值无效，因为 RVO 的优化是通过在 return 位置调用拷贝构造函数直接完成对象的构造的，它无法执行拷贝赋值。

## Q21：通过重载避免隐式类型转换

我们既希望实现 `operator op` 的所有参数都可以进行隐式类型转换，又希望避免在 class 内部提供隐式类型转换函数，以避免隐式类型转换导致的潜在问题。一种可行的做法是对 `operator op` 函数就行重载：

``` cpp
struct Int {
    explicit Int(int _x = 0) : x(_x) {}
    int x;
};

const Int operator+(const Int &lhs, const Int &rhs) {
    return Int(lhs.x + rhs.x); 
}

const Int operator+(const Int &lhs, int rhs) {
    return Int(lhs.x + rhs); 
}

const Int operator+(int lhs, const Int &rhs) {
    return Int(lhs + rhs.x); 
}

int main()
{
    Int a{1}, b{2};
    Int c = a + 10, d = 10 + b, e = a + b;
    cout << c.x << ' ' << d.x << ' ' << e.x << endl;

    return 0;
}
```

注意，我们不能重载 `const Int operator+(int lhs, int rhs);`，因为 C++ 规定，每一个重载的 operator 必须带有一个用户自定义类型的参数，否则，这不就是意味着我们可以修改内置类型的行为？这太恐怖了。

## Q22：consider using `op=` instead of stand-alone `op`

首先我们要明白一点，我们能写出如下代码：

``` cpp
x = x + y;
x = x - y;
```

并不意味着我们就能写出如下代码：

``` cpp
x += y;
x -= y;
```

这是因为 `operator+=`、`operator+` 和 `operator=` 之间并没有任何关系。

为了确保我们能够使用 `operator+=`，一种好的设计是使用 `operator+=` 来实现 `operator+`：

``` cpp
struct Int {
    explicit Int(int _x = 0) : x(_x) {}
    Int& operator+=(const Int &rhs) {
        x += rhs.x;
        return *this;
    }
    Int& operator+=(int rhs) {
        x += rhs;
        return *this;
    }
    Int& operator=(const Int &rhs) {
        x = rhs.x;
        return *this;
    }
    Int& operator=(int rhs) {
        x = rhs;
        return *this;
    }
    int x;
};

const Int operator+(const Int &lhs, const Int &rhs) {
    return Int(lhs) += rhs;
}

const Int operator+(const Int &lhs, int rhs) {
    return Int(lhs) += rhs;
}

const Int operator+(int lhs, const Int &rhs) {
    return Int(lhs) += rhs.x;
}

int main()
{
    Int a{1}, b{2};

    Int c = a + 10, d = 10 + b, e = a + b;
    cout << c.x << ' ' << d.x << ' ' << e.x << endl;

    a += 10, b += 10, c += d;
    cout << a.x << ' ' << b.x << ' ' << c.x << endl;
    return 0;
}
```

通过这种设计，我们只需要维护 `op` 的符合版本 `op=` 即可。

另外我们看一下下面的代码：

``` cpp
Int a, b, c, d;
Int e = a + b + c + d; // 产生 3 个临时对象，维护简单，并且这部分往往属于程序的 80%，对效率的影响微乎其微
e += a, e += b, e += c, e += d; // 不产生临时对象，维护麻烦，适用于对效率特别追求的地方
```

## Q23：考虑变更程序库

程序库的设计就是一个折衷的过程。理想的程序库应该是短小的、快速的、强大的、灵活的、可扩展的、直观的、普遍适用的、具有良好的支持、没有使用约束、没有错误的。这也是不存在的。为尺寸和速度而进行优化的程序库一般不能被移植。具有大量功能的的程序库不会具有直观性。没有错误的程序库在使用范围上会有限制。真实的世界里，你不能拥有每一件东西，总得有付出。

相同功能的不同的程序库在性能上采取不同的权衡措施，所以一旦你找到软件的瓶颈（通过进行 profile 参见条款 M16），你应该知道是否可能通过替换程序库来消除瓶颈。比如如果 你的程序有 I/O 瓶颈，你可以考虑用 stdio 替代 iostream，如果程序在动态分配和释放内存上使用了大量时间，你可以想想是否有其他的 operator new 和 operator delete 的实现 可用（参见条款 M8 和 Effective C++ 条款 10）。因为不同的程序库在效率、可扩展性、移植性、类型安全和其他一些领域上蕴含着不同的设计理念，通过变换使用给予性能更多考虑的程序库，你有时可以大幅度地提高软件的效率。 

## Q24：理解虚函数、多继承、虚基类和 RTTI 所需的代价

### 1. 虚函数

1. vptr
2. vtable
3. 放弃 inline

### 2. 多继承、虚基类

1. virtual base ptr
2. virtual base table

### 3. RTTI

1. type_info

# `[*]`七、技巧

https://www.cnblogs.com/Code-For-What/p/17865791.html

https://www.cnblogs.com/shushunie/articles/17778862.html

## Q25：将构造函数和非成员函数虚拟化

### 1. 虚拟构造函数

虚拟构造函数是一种设计模式或概念，它允许你根据运行时提供的数据或条件来创建不同具体类型的对象。尽管在C++等语言中没有直接的“virtual constructor”关键字（因为构造函数不能是虚函数），但这个概念可以通过多种方式实现，例如：

- **工厂方法 (Factory Method)**：这是最常见的实现方式。你定义一个接口（例如一个基类或抽象类），然后让子类实现一个工厂方法，该方法负责创建特定类型的对象。调用方只需要知道接口，而不需要知道具体的实现类。
- **抽象工厂 (Abstract Factory)**：当需要创建一系列相关或相互依赖的对象时，抽象工厂模式会更适用。它提供一个接口来创建一系列相关对象，而不需要指定它们的具体类。
- **原型模式 (Prototype Pattern)**：通过复制现有对象（原型）来创建新对象。这在需要创建大量相似对象且创建过程复杂时非常有用。你可能有一个“虚拟克隆”操作，它返回一个指向新复制对象的指针。**<font color=blue>实际上原型模式是一种虚拟拷贝构造函数。</font>**

### 2. 非成员函数虚拟化

具有虚拟行为的非成员函数很简单。你编写一个虚拟函数来完成工作，然后再写一个非虚拟函数，它什么也不做只是调用这个虚拟函数。为了避免这个句法花招引起函数调用开销， 你当然可以内联这个非虚拟函数。

## Q26：限制某个类所能产生的对象数量

**C 语言的 `inline`（默认情况下，不带 `extern` 时）具有内部链接属性。**

**C++ 中的 `inline` 具有外部链接属性。**

# `[*]`八、杂项

## Q32：在未来时态下开发程序



## Q33：将非尾端类设计为抽象类



## Q34：如何在同一个程序中结合 C 和 C++



## Q35：让自己习惯于标准 C++ 语言













































































































































































































































































































































































































































































































































































































































































































































