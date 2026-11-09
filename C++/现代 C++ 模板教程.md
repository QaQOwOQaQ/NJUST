# Reference

https://mq-b.github.io/Modern-Cpp-templates-tutorial/

# 函数模板

## 1. 引用折叠不是 1+1=2，2+1=1

引用折叠不是“数量相加”，不要误认为两个引用参与折叠就是 `& + & => &&` 右值引用。

引用折叠是有明确规定的，而且左值引用 `&` 的优先级更高。

在模板/类型推导里出现 `T&`、`T&&` 叠在一起时会折叠成：

- `&  +  &  =>  &`
- `&  +  && =>  &`
- `&& +  &  =>  &`
- `&& +  && => &&` 

只有引用折叠两方都是右值引用时，结果才是右值引用。只要一方是左值引用，结果就一定是左值引用。

## 2. (demo)`std::forward`

注意下面对 `std::forward` 的复现是不正确的，或者说是不标准的。

``` cpp
template<typename T>
constexpr T&& forward(T&& t) noexcept {
    return static_cast<T&&>(t);
}
```

它的主要问题是：**它允许把左值转换成右值，从而让人误用 `forward`**，把本来应该保留为左值的对象错误地 move 掉（或者触发只接受右值的重载）。

例如下面的调用：

``` cpp
int x = 0;
// r 的类型是 int&&
auto&& r = std::forward<int>(x);
```

产生误用问题的原因在于，我们在 `forward` 的入口也做了“转发引用”（`T&&` 形参），从而：

* 任何东西都能传进来（左值/右值）
* 并且只要调用者显式指定一个非引用的 `T`，就可能把左值强行 `cast` 成 `T&&`。

这在标准 `std:forward<int>(x)` 下是不允许通过的：因为标准实现的第一个重载需要的是 `remove_reference_t<int>&`（也就是 `int&`）形参没问题，但返回 `int&&` 会导致“把左值当右值转发”的意图被第二重载的约束/使用方式卡住（更准确地说：标准实现通过重载集合与 static_assert 防止从右值重载路径把左值 forward 成右值）。

简而言之：标准库的 `std::forward` 不是一个单一实现，它用**“两个重载 + 对右值形参的静态约束”**来“卡死”这种误用。

标准库实现思路大致是这样（忽略细节类型工具）：

``` cpp
// 只接受左值的版本
template<typename T>
constexpr T&& forward(std::remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t); // 万能转发
}

// 只接受右值的版本
template<typename T>
constexpr T&& forward(std::remove_reference_t<T>&& t) noexcept {
    static_assert(!std::is_lvalue_reference_v<T>, "bad forward of lvalue as rvalue");
    return static_cast<T&&>(t); // 万能转发
}
```

## 3. (demo)`std::move`

```cpp
template<typename T>
constexpr remove_reference_t<T>&& move(T&& t) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}
```

## 4. 调用 `std::move` 并不会发生“移动”

调用 `std::move` 并不会产生资源的移动，它只是进行一个类型转换，将目标对象的类型转换为右值引用，然后调用相应的接受右值引用的函数，在这个函数当中可能发生底层资源的“移动”。

## 5. (demo)编译期计算数组大小

``` cpp
// 由于我们不会使用数组，因此参数不需要声明数组名，表示我们不会使用该参数
// 以避免编译器警告
template<typename T, std::size_t N>
std::size_t get_array_size(T (&)[N]) {
    return N;
}
```

## 6. 推导两个类型的公共返回类型

### 方法1：借用三目运算符的类型规则

下面是推导对两个不同类型参数取 max 时的返回类型。其中三目表达式 `true ? T1{} : T2{}` 的用法很有意思：

``` cpp
template<typename T1, typename T2, 
	typename RT = decltype(true ? T1{} : T2{})>
RT max(const T1 &a, const T2 &b)
{
    return a > b ? a : b;
}
```

因为你想要的是：给定 `T1` 和 `T2`，得到一个“能同时容纳两者”的类型 `RT`。

条件运算符 `cond ? expr1 : expr2` 的类型规则（简化版）正好满足这个需求：

- 它会尝试在 `expr1` 和 `expr2` 之间做隐式转换；
- 如果两者能转换到某个共同类型，它就产生那个共同类型（更准确地说是一套很复杂的规则，例如 `int` 和 `double` 的公共返回类型是 `double`）。

此外，由于 `decltype` 是不求值语境，这种不求值语境里，不会真的运行分支，编译器只需要进行**类型检查与可行性分析**。因此对于三目运算（条件运算）以及临时变量 `T1{}` 和 `T2{}` 的构造不会实际发生。所以 `decltype(true ? T1{} : T2{})` 就相当于：“让我看看 `T1` 和 `T2` 在条件运算符下会合成什么类型”。

>  `T1{}` / `T2{}` 需要默认构造可行，否则 `decltype(...)` 直接不成立。

### 方法二：`std::common_type`

上面用三目表达式求公共类型的方法还是太简易了，它首先要求类型必须可以默认构造，其次还有奇怪的隐式转换/比较规则，就可能出现：

* `RT` 推导方式和函数体内 `?:` 的实际结果类型不一致
* 或者 `a>b` 能编译但 `?:` 在返回处发生不期望的转换/切片

事实上，C++11 引入了 `std::common_type`，它就是为“求公共类型”准备的，避免你手写 `?:` 黑魔法，也避免默认构造要求。

``` cpp
template<typename T1, typename T2>
std::common_type_t<T1, T2> max(const T1 &a, const T2 &b) {
    return a > b ? a : b;
}

// 也可以写为后置返回类型的形式
template<class T1, class T2>
auto max(const T1& a, const T2& b) -> std::common_type_t<T1, T2>
{
    return (a > b) ? a : b;
}
```

### 方法三：利用后置返回类型保留 CV 和引用信息

如果我们想得到 **表达式的精确类型（CV 和引用信息）**，我们可以利用后置返回类型：

```cpp
template<class T1, class T2>
auto max(const T1& a, const T2& b) -> decltype((a > b) ? a : b)
{
    return (a > b) ? a : b;
}
```

* 注意，当两边能形成同一类型的引用时，返回类型可能是引用（语义上更接近 `std::max`）

这里“使用后置返回类型”与“使用模板推导得到公共返回类型”并不等价，根源在于两者走的是不同的机制：

- **后置返回类型**并不是通过模板实参推导（template argument deduction）得到的。
  它由我们写出的**类型表达式**（例如 `T&`、`decltype(expr)`、`std::common_type_t<...>` 等）直接确定。编译器需要做的只是**实例化并计算/确定这个类型表达式的结果类型**。
  因此它可以精确保留类型的 **cv/ref** 信息（具体保留哪些，取决于该类型表达式本身的规则）。
- **模板实参推导**是否会“丢弃 cv/ref”，取决于形参形式：
  - 按值形参（例如 `T`）会丢弃顶层 `const/volatile`，并发生退引用/退化等效果；
  - 引用形参（例如 `T&`、`const T&`）以及万能引用（`T&&`）则不一定，会以规则允许的方式保留/编码这些信息。

可以把后置返回类型理解为：我们显式写出了一个**类型明确的表达式**作为返回类型，编译器只是去“算出它最终是什么类型”，而不是像模板实参推导那样对类型进行“去引用/去 cv”等剥离处理。

## 7. 模板不能分文件

关键点在一句话：

> **模板只有在“被用到/被实例化”时才会生成真实代码。**

- 如果你把模板定义写在 `test.cpp`，而 `main.cpp` 只 include 了声明（甚至只有模板声明），那么：
  - `main.cpp` 编译时需要实例化模板（比如 `f<int>()`），但它**看不到模板定义**，无法生成对应的实例化代码；
  - `test.cpp` 即使有模板定义，也**不一定会自动生成任何实例**（因为它可能根本没有在 `test.cpp` 内使用这些具体类型）。
- 结果就是：要么编译期直接报错（看不到定义无法实例化），要么链接期缺符号（没生成对应实例，例如 extern）。

所以模板通常组织文件的常见做法：

1. **模板声明 + 模板定义都放在头文件**（`.h/.hpp`），使用者 include 后在自己的翻译单元内完成实例化。
2. 或者用 `.hpp` / `.tpp` / `.inl` 这种文件放定义，然后在 `.h` 末尾 `#include "xxx.tpp"` ——本质仍然是“让定义在 include 后可见”。

> 你提到的 “`.hpp` 代表这里主要放模板” 也是一种约定俗成。

## 8. 不要使用 C 语言的变长参数

当你的函数需要接受任意数量的参数时，要使用变参模板而不是 `va_arg` 参数。

变参函数（variadic function）是像 `std::printf` 这样的函数，可以接受任意数量的参数。问题是，必须假设传递的类型总是正确的。当然，这种假设非常容易出错，其正确性依赖于程序员的素养。

> `std::printf` 不是类型安全的函数，很多东西理论上根本无法检测，但是因为用的太多了，大部分编译器都给它开洞检查类型之类的玩意了。

为了理解变参函数的隐含危险，请看下面的小例子。

``` cpp
#include <iostream>
#include <cstdarg>

int sum(int num, ...){
    int sum = 0;
    va_list argPointer;
    va_start(argPointer, num);
    for(int i = 0;i<num;++i)
        sum += va_arg(argPointer, int);
    va_end(argPointer);

    return sum;
}

int main(){
    std::cout << "sum(1, 5): " << sum(1,5) << '\n';
    std::cout << "sum(3, 1, 2, 3): " << sum(3, 1, 2, 3) << '\n';
    std::cout << "sum(3, 1, 2, 3, 4): " << sum(3, 1, 2, 3, 4) << '\n';    //（1）
    std::cout << "sum(3, 1, 2, 3.5): " << sum(3, 1, 2, 3.5) << '\n';      //（2）
}
```

`sum` 是一个变参数函数。它的第一个参数是需要被求和的参数个数。以下是关于 `va_arg` 宏的背景信息，有助于理解该代码。

- **`va_list`:** 保存下列宏的必要信息。
- **`va_start`:** 启用对变参函数参数的访问。
- **`va_arg`:** 访问下一个变参函数的参数。
- **`va_end`:** 结束对变参函数参数的访问。

请阅读 cppreference 中关于[变参数函数](https://zh.cppreference.com/w/cpp/language/variadic_arguments)的部分来获取进一步的信息。

代码行（1）和（2）中出了些状况。（1）中参数 num 的数量是错的；（2）中我提供了一个 double 而不是一个 int。[输出结果](https://godbolt.org/z/nch8fjdf5)显示了这两个问题。（1）中的最后一个元素丢失了，而 double 被解释为 int（2）。

```cpp
# x86-64 gcc 13.2 -std=c++11
sum(1, 5): 5
sum(3, 1, 2, 3): 6
sum(3, 1, 2, 3, 4): 6
sum(3, 1, 2, 3.5): 3075
```

> 注意，最后一个输出结果并不一定，我们只是按照书上的结果写了。经过实测的话，msvc 和 mingw clang 都是 **6**，gcc 是 **3075**。
>
> “类型不匹配读取 `va_arg` 属于 UB，任何输出都可能出现。”

这些问题可以通过 C++17 的折叠表达式轻松解决。跟 `va_arg` 相比，折叠表达式会自动推导出其参数的数量和类型。

```cpp
#include <iostream>

template<class...Args>
auto sum(Args...args) {
    return (... + args);
}

int main() {
    std::cout << "sum(5): " << sum(5) << '\n';
    std::cout << "sum(1, 2, 3): " << sum(1, 2, 3) << '\n';
    std::cout << "sum(1, 2, 3, 4): " << sum(1, 2, 3, 4) << '\n';
    std::cout << "sum(1, 2, 3.5): " << sum(1, 2, 3.5) << '\n';
}
```

函数 sum 可能看起来挺可怕，它需要至少一个参数，并使用 C++11 的变参模板。变参模板可以接受任意数量的参数。这些任意数量的参数由所谓的参数包持有，用省略号（`...`）表示。此外，在 C++17 中，可以用二元运算符直接对参数包进行归约。这一针对变参模板的增强被称为折叠表达式。在 sum 函数的例子中，应用了二元的 + 运算符（`... + args`）。想要了解 C++17 折叠表达式的更多信息可参阅 https://zh.cppreference.com/w/cpp/language/fold 了解更多细节。

程序的输出正如预期：

```cpp
sum(5): 5
sum(1, 2, 3): 6
sum(1, 2, 3, 4): 10
sum(1, 2, 3.5): 6.5
```

## 9. ADL

**Argument-dependent lookup（ADL）**，也叫 *Koenig lookup*，是 C++ 的名字查找规则之一：当你写一个**不带限定名**的函数调用（比如 `f(x)` 而不是 `ns::f(x)`），编译器除了在当前作用域找 `f`，还会去“参数相关的命名空间/类作用域”里找 `f`。

### ADL 解决了什么问题？

典型用途：让“类型和它的相关自由函数”可以写得很自然。

```cpp
#include <iostream>

namespace lib {
  struct X {};
  void print(X) { std::cout << "lib::print\n"; }
}

int main() {
  lib::X x;
  print(x);       // ✅ 找得到：ADL 会去 namespace lib 里找 print
  // lib::print(x); // 当然也可以显式写
}
```

如果没有 ADL，你每次都得写 `lib::print(x)`，对泛型代码很不友好。

### ADL 的“相关作用域”包括哪些？

对函数调用 `f(arg1, arg2, ...)`，ADL 会把每个实参类型的“关联实体”收集起来，常见的是：

- 实参是某个命名空间里的类型 → 关联命名空间包含那个命名空间
- 实参是类/结构体类型 → 关联类本身、它的基类、以及它所在命名空间
- 实参是 `std::pair<lib::X, int>` 这种模板实例 → 关联命名空间也会包含模板实参相关的命名空间（因此 ADL 可能“传播”得更广）

注意：**只对不带限定名的函数调用**触发，比如 `f(x)`；写成 `ns::f(x)` 就不会再额外用 ADL 去补找别的 `f` 了（已经限定死了）。

### 最常见的 ADL 场景：`swap`

为什么推荐写成下面这样：

```cpp
using std::swap;
swap(a, b);  // 让 ADL 有机会找到自定义 swap
```

因为如果 `a`/`b` 的类型在 `mylib` 里，并且 `mylib::swap` 提供了更高效的交换，ADL 能选到它；否则就回退到 `std::swap`。

### ADL 的坑：会“意外找到”不该找到的函数

ADL 有时会把你不想引入的东西也引进来，导致二义性或调用到意料之外的重载。

```cpp
namespace A {
  struct X {};
  void f(X) {}
}

namespace B {
  void f(A::X) {}
}

int main() {
  A::X x;
  f(x); // ❗可能二义性：ADL 会考虑 A::f，同时普通查找可能找到 B::f（取决于当前作用域可见性）
}
```

### 另一个坑：在泛型代码里“屏蔽 ADL”会破坏可扩展性

比如你在模板里写死 `std::swap(a,b)`，就让用户自定义 `swap` 没机会被 ADL 找到，性能/语义都可能受影响。

### 小结

- ADL = 不限定名函数调用时，额外去“参数相关命名空间/类作用域”找候选函数
- 主要用于：让自由函数（如 `swap`、`begin/end`、`operator<<` 等）对自定义类型更自然、更可扩展
- 主要风险：意外引入候选导致二义性/意外重载被选中

如果你告诉我你想讨论 ADL 的哪个角度（比如和 `using std::swap;`、`std::begin`、隐藏友元 hidden friend、或 `ranges` 的 CPO 的关系），我可以给你更针对性的例子。

# 类模板

## 1. 类模板实参推导 CTAD（C++17）

``` cpp
template<typename T>
struct Test {
    T t;
};


int main()
{
    Test<int> t1;
    Test t2{1}; // C++17 OK
    return 0;
}
```

C++17 增加了类模板实参推导（Class Template Argument Deduction, CTAD），也就是说类模板也可以像函数模板一样被推导，而不需要显式的写明模板类型参数了，这里的 `Test` 被推导为 `Test<int>`。

不单单是聚合体，当然，写构造函数也可以：

```cpp
template<typename T>
struct Test{
    Test(T v) :t{ v } {}
private:
    T t;
};
```

## 2. 用户定义的推导指引（C++17）

前面介绍过，C++17 首次引入了 **类模板实参推导（CTAD）**，允许在构造对象时省略模板实参，例如：

```cpp
std::pair p(1, 2);   // C++17 起合法，推导为 std::pair<int, int>
```

在此基础上，C++17 还允许**用户为自己的类模板编写推导指引**，用来控制或补充编译器的推导行为。

推导指引的语法还是简单的，如果只是涉及具体类型，那么只需要：

``` cpp
template <可选的模板参数...>
类模板名(形参列表) -> 类模板名<要推导出的模板实参...>;
```

* **本质上就是：写一个“看起来像构造函数声明”的东西 + `->` 指定要推导成的类模板实例类型**。

如果涉及的是一类类型，那么就需要加上 `template`，然后使用它的模板形参。

---

例如，我要让一个类模板，如果推导为 int，就让它实际成为 size_t：

```cpp
template<typename T>
struct Test{
    Test(T v) :t{ v } {}
private:
    T t;
};

Test(int) -> Test<std::size_t>;

Test t(1);      // t 是 Test<size_t>
```

如果要类模板 `Test` 推导为指针类型，就变成数组呢？

```cpp
template<typename T>
Test(T*) -> Test<T[]>;

char* p = nullptr;

Test t(p);      // t 是 Test<char[]>
```

我们提一个稍微有点难度的需求：

```cpp
template<class Ty, std::size_t size>
struct Array {
    Ty arr[size];
};

Array arr{1, 2, 3, 4, 5};     // Error!
```

类模板 Array 同时使用了类型模板形参与非类型模板形参，保有了一个成员是数组。

它无法被我们直接推导出类型，此时就需要我们自己**定义推导指引**。

这会用到我们之前在函数模板里学习到的形参包。

```cpp
template<typename T, typename... Args>
Array(T, Args...) -> Array<T, sizeof...(Args) + 1>;
```

原理很简单，我们要给出 array 的模板类型，那么就让模板形参单独写一个 T 占位，放到形参列表中，并且写一个模板类型形参包用来处理任意个参数；获取 array 的 size 也很简单，直接使用 sizeof... 获取形参包的元素个数，然后再 +1 ，因为先前我们用了一个模板形参占位。

标准库的 [`std::array`](https://zh.cppreference.com/w/cpp/container/array/deduction_guides) 的推导指引，原理和这个一样。

## 3. 类模板可以用默认模板参数

和函数模板一样，**类模板形参也能写默认实参**：

```cpp
template<typename T = int>
struct X{};
```

含义：如果用户没显式提供 `T`，就用 `int`。

<font color=blue>但是，在“对象定义处”能不能省略 `<>`：取决于是否有 C++17 的 CTAD：</font>

- **C++17 起**引入了 CTAD（Class Template Argument Deduction）
- 因此在**全局 / 函数作用域**写：

```cpp
X x;     // C++17 起 OK：推导为 X<int>
X<> x2;  // 一直 OK：显式写出空 <>，使用默认参数 int
```

<font color=blue>但在类里声明数据成员时：不能省略 `<>`（不管有没有 C++17）</font>

在类中声明成员（静态/非静态都一样），标准要求上 **`X x;` 不行**：

```cpp
struct Test{
    X x;                // Error（不能省略 <>）
    X<> x2;             // OK
    static inline X x3; // Error（同理）
};
```

> 不过，gcc 13.2 在 `-std=c++17` 下，**类内定义的 `static inline X x3;`** 可能通过，但 MinGW clang 16 / MSVC 仍然报错。这属于实现差异，可移植代码里应当坚持：**类内成员声明不要省略 `<>`**。

## 4. 模板模板参数

**模板模板形参（template template parameter）**： 类模板（或函数模板）的模板形参中，有一种形参可以**接收“一个类模板/别名模板”作为参数**。

典型用途：把“容器模板/策略模板/包装模板”当作参数传进去，在外层模板里再用它去实例化：

```cpp
template<typename T> struct X {};
template<template<typename T> typename C> struct Test {};
Test<X> t;   // C 接收的是模板 X
```

#### 核心语法：怎么读 `template<typename T> typename C`

把外层 `template<...>` 拆开看：

- `template<typename T>`：描述**被接收的那个模板**的形参列表（要能匹配）
- `typename C` / `class C`：声明一个名字 `C`，它代表“一个模板”

因此：

```cpp
<template<template<typename> typename C>
```

意思是：`C` 必须是“接收一个类型参数”的模板。

#### 常见形式总结

模板模板形参支持：

1. **普通形式**（可选名字）

```
template <形参列表> class/typename 名字;
```

1. **带默认值**

```
template <形参列表> class/typename 名字 = default;
```

1. **模板模板形参包（C++11 起）**

```
template <形参列表> class/typename ... 名字;
```

补充：`typename` 用于模板模板形参是 **C++17 起**允许（以前主要写 `class`）。

------

## 4) 用法：把模板当“成员的外壳”用

你给的例子本质是：外层模板参数决定“内部成员用哪种模板包装”。

```
template<typename T>
struct my_array { T arr[10]; };

template<typename Ty, template<typename> typename C>
struct Array { C<Ty> array; };

Array<int, my_array> a;  // 成员类型是 my_array<int>，里面有 int arr[10]
```

------

## 5) 默认模板 + 模板模板形参也能给默认值

```
template<typename Ty, template<typename> typename C = my_array>
struct Array { C<Ty> array; };

Array<int> a;  // 等价于 Array<int, my_array>
```

结论：✅ 模板模板形参也支持默认实参。

------

## 6) 模板模板形参包：能接收“任意个同形模板”

```
template<typename T> struct X{};
template<typename T> struct X2{};

template<template<typename> typename... Ts>
struct Test{};

Test<X, X2, X, X> t;
```

结论：✅ 它就是一种形参包，只是“元素类型”是模板。

------

## 7) 能接收非类型模板形参的模板

模板模板形参也可以描述“被接收模板”的形参是**非类型模板参数**：

```
template<std::size_t N> struct X {};

template<template<std::size_t> typename C>
struct Test {};

Test<X> t;
```

并且像你说的，`template<std::size_t>` 里参数名 `N` 可省略。

------

## 8) 变参模板也同理：被接收模板可以是 `template<typename...>`

```
template<typename... T>
struct my_array { int arr[sizeof...(T)]; };

template<typename Ty, template<typename...> typename C = my_array>
struct Array { C<Ty> array; };

Array<int> a; // 这里 C<Ty> => my_array<int>
```

------

## 总结一句话

模板模板形参让你把“类模板本身”当作参数传入；它要求你在形参处写出**可匹配的模板形参列表**，并且同样支持**默认值**与**参数包**；类型形参/非类型形参/变参模板都能用同样的思路描述和组合。

如果你愿意，我也可以补一个常见坑点总结：比如“形参列表必须匹配到什么程度”“带默认参数时为什么看起来不一样也能匹配”“别名模板能不能传”等。

# 变量模板（C++14）

变量模板不是变量，只有实例化的变量模板，编译器才会生成实际的变量。

变量模板实例化后简单的说**就是一个全局变量**，所以也不用担心生存期的问题。

另外，函数模板中，我们强调了“**同一个函数模板生成的不同类型的函数，彼此之间没有任何关系**”，这句话放在类模板、变量模板中，也同样适用。

``` cpp
// 1. 定义（全局）变量模板
template<typename T> T v;
v<int>;     // 相当于 constexpr int v = 0;


// 2. 有默认实参的模板形参
template<typename T = int> constexpr T v{};
int b = v<>;        // v 就是 v<int> 也就是 const int v = 0

// 3. 非类型模板形参
template<std::size_t N> constexpr int v = N;
std::cout << v<10> << '\n'; // 等价 constexpr int v = 10;

// 4. 可变参数变量模板
template<std::size_t...values>
constexpr std::size_t array[]{ values... };

int main() {
    for (const auto& i : array<1, 2, 3, 4, 5>) {
        std::cout << i << ' ';
    }
}

// 5. 成员变量模板
struct limits{
    template<typename T>
    static constexpr T min; // 静态数据成员模板的声明
};
 
template<typename T>
constexpr T limits::min = {}; // 静态数据成员模板的定义
```

变量模板其实很常见，在 C++17，所有元编程库的[类型特征](https://zh.cppreference.com/w/cpp/meta)均添加了 `_v` 的版本，就是使用的变量模板，比如 `std::is_same_v` 、`std::is_pointer_v` 等；我们在后面会详细讲解。

练习：

```cpp
#include <ios>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <type_traits>

using namespace std;

// 全局模板变量
template<typename T>
T v1{};

// 有默认实参的模板变量
template<typename T = int>
T v2;

// 非类型模板变量
template<size_t N = 1000>
size_t n = N;

// 可变参数模板变量
template<size_t... Args>
size_t arr[]{Args...};

// 成员模板变量
struct Limit {
    template<typename T>
    constexpr static T min_limit = T{};
};

int main()
{
    // 全局模板变量
    int *x1 = &v2<int>;
    int *x2 = &v2<>;
    double *y1 = &v2<double>;
    cout << x1 << endl; // 0x56a7dd68b194
    cout << x2 << endl; // 0x56a7dd68b194
    cout << y1 << endl; // 0x56a7dd68b198

    // 非类型模板变量
    auto N = n<1000>;
    cout << N << endl; // 1000

    // 可变参数模板
    for(auto &x : arr<1,2,3,4,5>)
        cout << x << ' '; // 1 2 3 4 5 
    cout << endl;
    cout << boolalpha << "is_same_type: " << is_same_v<decltype(arr<1,2,3,4,5>), size_t[5]> << endl; // true

    // 成员模板变量
    const int *limit1 = &Limit::min_limit<int>;
    const int *limit2 = &Limit::min_limit<int>;
    const double *limit3 = &Limit::min_limit<double>;
    cout << limit1 << endl; // 0x5e795d10a004
    cout << limit2 << endl; // 0x5e795d10a004
    cout << limit3 << endl; // 0x5e795d10a008

    return 0;
}
```

# 模板全特化

写一个类的全特化，就相当于写一个新的类一样，你可以自己定义任何东西，不管是函数、数据成员、静态数据成员，等等；根据自己的需求。

## 1. `is_void_v`

利用模板全特化和变量模板实现 `is_void` 和 `is_void_v`：

``` cpp
template<typename T>
struct is_void {
    static constexpr bool value = false;
};

template<> // T = void
struct is_void<void> {
    static constexpr bool value = true;
};

// C++17 变量模板
template<typename T>
constexpr bool is_void_v = is_void<T>::value;
```

``` cpp
template<typename T>
constexpr bool is_void_v = false;

template<>
constexpr bool is_void_v<void> = true;
```

# 模板偏特化

模板偏特化使我们可以对具有相同的一类特征的类模板、变量模板进行定制行为。

比如：指针类型，这是一类类型，有 `int*`、`double*`、`char*`，以及自定义类型的指针等等，它们都属于指针这一类类型；可以使用偏特化对指针这一类类型进行定制。

例如：

``` cpp
#include <ios>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <type_traits>

using namespace std;

template<typename T>
const char* s = "?";            // 主模板

template<typename T>
const char* s<T*> = "pointer";  // 偏特化，对指针这一类类型

template<typename T>
const char* s<T[]> = "array";   // 偏特化，但是只是对 T[] 这一类类型，而不是数组类型，因为 int[] 和 int[N] 不是一个类型


int main()
{
    std::cout << s<int> << '\n';            // ?
    std::cout << s<int*> << '\n';           // pointer
    std::cout << s<std::string*> << '\n';   // pointer
    std::cout << s<int[]> << '\n';          // array
    std::cout << s<double[]> << '\n';       // array
    std::cout << s<int[1]> << '\n';         // ?
    return 0;
}
```

> <font color=blue>注意对 `T[]` 和 `T[N]` 的特化并不属于一类。</font>

## 1. `is_same_v`

下面我们利用模板的偏特化，实现 `std::is_same_v`：

``` cpp
template<typename T1, typename T2>
inline constexpr bool is_same_v = false;

template<typename T>
inline constexpr bool is_same_v<T,T> = true;
```

* inline 声明为内联变量，可以允许多次定义

## 2. 函数模板没有偏特化

函数模板没有偏特化，只有类模板和变量模板可以。

这是因为 函数模板本身就支持重载，而函数模板的「偏特化」在语义上会与重载 + 模板实参推导产生歧义和冲突；

因此，C++ 标准刻意不提供函数模板的偏特化机制。

而类模板没有重载，只能通过偏特化来区分不同情况。

# 模板显式实例化解决模板分文件问题

将模板声明放在 `.h` 文件、模板定义放在 `.cpp` 文件的问题在于：模板定义所在的 `.cpp` 文件中并没有使用该模板，因此不会发生模板实例化；而在其他 `.cpp` 文件中使用该模板时，又由于看不到模板定义，无法进行实例化，最终在链接阶段找不到对应的符号而报错。

既然模板只有实例化，编译期才能生成实际的代码，我们才能看到模板的定义，那么我们可以通过显式实例化模板来解决模板分文件导致链接错误的问题。

## 1. 函数模板显式实例化

``` cpp
template RT FuncName < 实参列表 > ( 形参列表 ) ;          (1)
template RT FuncName ( 形参列表 ) ;                      (2)
extern template RT FuncName < 实参列表 > ( 形参列表 ) ;   (3) (C++11 起)
extern template RT FuncName ( 形参列表 ) ;               (4) (C++11 起)
```

1. 显式实例化定义（显式指定所有无默认值模板形参时不会推导模板实参）
2. 显式实例化定义，对所有形参进行模板实参推导
3. 显式实例化声明（显式指定所有无默认值模板形参时不会推导模板实参）
4. 显式实例化声明，对所有形参进行模板实参推导

**在模板分文件问题中，几乎不会使用到显式实例化声明。**因为我们引用 `.h` 文件本身就有声明，除非你准备直接两个 `.cpp`。

## 2. 类模板显式实例化

``` cpp
template class CName < 实参列表 > ;	        (1)	
extern template class CName < 实参列表 > ;	    (2)	(C++11 起)
```

class 也可以是 struct 或 union

1. 显式实例化定义
2. 显式实例化声明

## 3. 模板全特化

模板全特化本质上是：

- **一个普通的、非模板的定义**
- 编译器会直接生成实体代码
- 不依赖模板实例化机制

因此：

- 函数模板的**全特化**
- 类模板成员函数的**全特化**

都可以在 `.cpp` 中生成实际符号，从而**“顺带”解决链接问题**。

另外需要注意的是，**类模板的完整定义并不要求成员函数必须同时给出定义**；只要类的**数据成员、基类、成员声明**等信息是完整的，类模板本身就被认为是完整类型，成员函数的定义可以放在类外、甚至放在其他翻译单元中。

这是因为：

- **类的完整定义 ≠ 成员函数都有定义**
- 成员函数在语义上是**独立的实体**
- 是否需要成员函数的定义，取决于：
  - 是否对该成员函数进行了 **ODR 使用**
  - 是否发生了模板实例化

# 显式实例化解决模板导出静态库动态库

# 折叠表达式（C++17）

**语法：**

```cpp
( pack op ... )             (1) 
( ... op pack )             (2)
( pack op ... op init )     (3)
( init op ... op pack )     (4)
```

1. 一元右折叠
2. 一元左折叠
3. 二元右折叠
4. 二元左折叠

> 折叠表达式是左折叠还是右折叠，取决于 `...` 是在“形参包”的左边还是右边。

折叠表达式的实例化按以下方式展开成表达式 e：

1. 一元右折叠 `(E 运算符 ...)` 成为 `(E1 运算符 (... 运算符 (EN-1 运算符 EN)))`
2. 一元左折叠 `(... 运算符 E)` 成为 `(((E1 运算符 E2) 运算符 ...) 运算符 EN)`
3. 二元右折叠 `(E 运算符 ... 运算符 I)` 成为 `(E1 运算符 (... 运算符 (EN−1 运算符 (EN 运算符 I))))`
4. 二元左折叠 `(I 运算符 ... 运算符 E)` 成为 `((((I 运算符 E1) 运算符 E2) 运算符 ...) 运算符 EN)` （其中 N 是包展开中的元素数量）

需要注意的是，**<font color=blue>折叠表达式在 C++ 语法中必须写成括包包围的整体。</font>**

**例1：实现一个 `print` 函数：**

``` cpp
#include <iostream>

using namespace std;

template<typename... Args>
void print(const Args&... args) {
    // expression: (std::cout << args << ' ')
    // op: ,
    ((std::cout << args << ' '), ...); // 一元右折叠
    // 对于逗号表达式，左折叠和右折叠没有区别，或者说效果是一样的
    std::cout << endl;

    (std::cout << ... << args) << std::endl; // 二元左折叠
}

int main()
{
    print(1, 3.14, 'A', "hello", "\n");
    return 0;
}
```

**例2：sub**

``` cpp
#include <iostream>

using namespace std;

template<int... I>
constexpr int sub_left = (I - ...); // 一元右折叠

template<int... I>
constexpr int sub_right = (... - I); // 一元左折叠

int main()
{
    cout << sub_left<1, 2, 3, 4> << endl;  // (1 - (2 - (3 - 4)))
    cout << sub_right<1, 2, 3, 4> << endl; // (((1 - 2) - 3) - 4)
    
    cout << (1 - (2 - (3 - 4))) << endl;
    cout << (((1 - 2) - 3) - 4) << endl;
    return 0;
}
```

**例3：reverse**

``` cpp
#include <bits/utility.h>
#include <tuple>
#include <vector>
#include <type_traits>
#include <utility>
#include <iostream>

template<typename... Args>
auto Reverse(Args&&... args) {
    using C = std::common_type_t<std::decay_t<Args>...>;
    constexpr std::size_t N = sizeof...(Args);

    std::vector<C> res;
    res.reserve(N);

    auto tup = std::forward_as_tuple(std::forward<Args>(args)...);

    [&]<std::size_t... I>(std::index_sequence<I...>) {
        (res.push_back(static_cast<C>(std::get<N - 1 - I>(tup))), ...);
    }(std::make_index_sequence<N>{});

    return res;
}

int main()
{
    auto res =Reverse(3.14, 2, 'A');
    for(auto &x : res) std::cout << x<< std::endl;
    return 0;
}
```

# 待决名（Dependent Name）

在模板定义中，其含义<font color="blue">依赖于模板参数</font>、且在<font color="blue">模板定义阶段无法确定</font>的名字，叫<font color="blue">待决名</font>。

换句话说：

- 在**模板定义阶段**
  编译器 **不知道它到底是什么**
- 只能等到**模板实例化阶段**
  用具体模板参数才能决定

根本原因就是， **C++ 模板是“两阶段编译”** 的：

模板定义阶段（Definition phase）

- 不知道模板参数
- **只能解析非待决名**
- 不能假设任何依赖于模板参数的东西

模板实例化阶段（Instantiation phase）

- 模板参数已知
- 才能解析待决名

## 1. 常见的“依赖来源”

抓好待决名的定义，只有当名字依赖于模板参数时才会产生待决名。

| 依赖对象       | 示例           |
| -------------- | -------------- |
| 类型模板参数   | `T::type`      |
| 非类型模板参数 | `N + 1`        |
| 依赖基类       | `Base<T>::f()` |
| `this->成员`   | `this->f()`    |

前三条都好理解，因为我们用到了模板参数，因此名字查找只能等到实例化阶段。那为什么 `this->f()` 也是待决的呢？其实我们将 `this` 转换为其原本的含义——指向类的指针，就明白了，例如：

``` cpp
template<class T>
struct Foo : Base<T> {
  void g() { this->f(); }
};
```

在上面，`this` 的类型就是 `Foo<T>*`，类型里面有模板参数 `T`，因此 `this` 也是一个待决名。

## 2. `typename`：消除“类型 / 非类型”歧义

在模板中：不是当前实例化成员，并且依赖模板参数，默认 **不会被当作类型** 而是当作一个静态成员变量。必须用 `typename` 明确告诉编译器：**“这是一个类型”**

经典错误示例

```cpp
template<typename T>
const T::type& f(const T&) { return 0; }
```

错误原因：

- `T::type` 依赖 `T`
- 编译器不知道 `type` 是类型还是成员变量

正确写法

```cpp
template<typename T>
const typename T::type& f(const T&) { return 0; }
```

* `typename` 明确：`T::type` 是类型

什么时候可以不写 `typename`？

以下情况 **不需要**：

1. **当前实例化的成员**（明确知道名字的含义）
2. 已经显式通过 `typedef / using` 声明为类型
3. 用作 **基类名**

```cpp
typedef typename T::type iter_t; // 以后 iter_t 不再需要 typename
```

一个极易踩坑的例子

```cpp
std::vector<T>::const_iterator* p;
```

如果不加 `typename`，会被解析为：

```cpp
(std::vector<T>::const_iterator) * p;
```

**乘法表达式**，不是声明

`typename` 的合法位置

- 只能用于 **限定名之前**

```cpp
typename T::x;
typename std::vector<int>::value_type v;
```

即使不是待决名，也**允许**写（只是没意义）

## 3. `template`：消除“模板 / 非模板”歧义

问题来源：

在模板定义中出现如下代码：

```cpp
s.foo<T>();
```

编译器在**第一阶段解析**时看到的是：

```
foo < T
```

在语法层面，这更像是一个 **小于号运算表达式**，而不是模板实参列表。

规则核心：**在模板定义中，对于依赖模板参数的名字，编译器默认不会将其视为模板。**

因此，当成员名是否为模板无法在定义阶段确定时， `<` **不会被当作模板实参列表的起始符号**。

正确写法

```cpp
s.template foo<T>();
```

明确告诉编译器：`foo` 是模板

`template` 的使用限制：`template` 关键字 **只能用于可能产生“成员模板歧义”的位置**， 即以下三种成员访问形式之后：

| 运算符 | 示例                       |
| ------ | -------------------------- |
| `::`   | `T::template foo<int>()`   |
| `.`    | `s.template foo<int>()`    |
| `->`   | `ptr->template foo<int>()` |

组合用法（高阶但常见）

```cpp
typename T::template iterator<int>::value_type v;
```

* `template`：消除 `iterator<int>` 是否为模板的歧义
* `typename`：消除 `value_type` 是否为类型的歧义

下面是一个完整的例子：

```cpp
struct A {
    template<typename T>
    void foo(T v) {
        std::cout << "A::foo: " << v << '\n';
    }
};

template<typename T>
struct Wrapper {
    T obj;

    void test() {
        // ❌ 错误：有歧义
        obj.foo<int>(42);

        // ✅ 正确：明确 foo 是模板
        // obj.template foo<int>(42);
    }
};

int main() {
    Wrapper<A> w;
    w.test();
}
```

歧义产生的原因

- `Wrapper<T>` 是模板，`obj` 的类型依赖于模板参数 `T`
- 因此 `obj.foo` 是 **依赖名（dependent name）**
- 在模板定义阶段，编译器：
  - 不进行依赖名的语义查找
  - 不假设依赖名是模板

结果是：

- `<` **不会被视为模板实参列表的起始符号**
- 而是被解析为 **小于号运算符**

因此表达式：

```cpp
obj.foo<int>(42)
```

在语法层面 **可以被解析为**：

```cpp
(obj.foo < int) > (42)
```

从而导致语法歧义或编译错误。

## 4. 名字查找与绑定规则

关键结论：

* 非待决名：**在模板定义点查找并绑定**，实例化时 **不会再变**
* 待决名：**推迟到模板实例化时查找**

经典反直觉示例

```cpp
void g(double);

template<class T>
struct S {
    void f() const {
        g(1);
    }
};

void g(int);


S<int> s;
s.f();   // 调用 g(double)
```

为什么不是 `g(int)`？

因为：

- `g` 是 **非待决名**
- 在模板定义阶段就已经绑定到 `g(double)`
- 后面新增的 `g(int)` **不影响**

## 5. 继承 + 名字查找

```cpp
template<class T>
struct X {
    void f() const {
        std::cout << "X\n";
    }
};

void f() {
    std::cout << "全局\n";
}

template<class T>
struct Y : X<T> {
    void t() const {
        this->f(); // X
    }

    void t2() const {
        f();       // 全局
    }
};
```

`this->f()`

- `this` 依赖 `T`
- 👉 **待决名**
- 实例化时才查找
- 能找到基类 `X<T>::f`

`f()`

- 非限定名
- 👉 非待决名
- 在模板定义阶段查找
- **不会查找依赖基类**
- 只能找到全局 `f`

规则铁律

> **依赖模板参数的基类**，**不会参与无限定名字查找**

所以你必须写：

```cpp
this->f();
X<T>::f();
using X<T>::f;
```

## 6. 非标准行为

- MSVC（尤其旧版本）
  - 不完全支持 **Two-phase name lookup**
  - 可能：
    - 不写 `typename` 也能过
    - 不写 `template` 也能过
    - `f()` 找到基类函数

⚠️ **这些都是非标准行为**

**不要以 MSVC 是否报错作为模板正确性的依据**

## 7. 总结

看到下面情况，立刻反应：

| 场景                 | 该做什么                   |
| -------------------- | -------------------------- |
| `T::type`            | 加 `typename`              |
| `obj.foo<T>()`       | 加 `template`              |
| 依赖基类成员         | 用 `this->` 或 `Base<T>::` |
| 模板里函数调用走错   | 检查是否非待决名           |
| MSVC 能过但 GCC 不行 | 你代码大概率不标准         |

一句话总总结

> **待决名不是“语法细节”，而是模板“两阶段编译”的直接产物。**
>
> `typename`、`template`、`this->` 本质都是在模板定义阶段帮编译器消除不确定性。

# SFINAE

“代换失败不是错误” (Substitution Failure Is Not An Error)

在**函数模板的重载决议**[[1\]](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fn1)中会应用此规则：当模板形参在替换成显式指定的类型或推导出的类型失败时，<font color="blue">从重载集中丢弃这个特化，而非导致编译失败</font>。

此特性被用于模板元编程。

> 注意：**本节非常非常的重要，是模板基础中的基础，最为基本的特性和概念**。

## 解释

对函数模板形参进行两次代换（由模板实参所替代）：

- 在模板实参推导前，对显式指定的模板实参进行代换
- 在模板实参推导后，对推导出的实参和从默认项获得的实参进行替换

代换的实参写出时非良构[[2\]](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fn2)（并带有必要的诊断）的任何场合，都是*代换失败*。

> ”对显式指定的模板实参进行代换“这里的显式指定，就比如 `f<int>()` 就是显式指明了。我知道你肯定有疑问：我都显式指明了，那下面还推导啥？对，如果模板函数 `f` 只有一个模板形参，而你显式指明了，的确第二次代换没用，因为根本没啥好推导的。

> 两次代换都有作用，是在于有多个模板形参，显式指定一些，又根据传入参数推导一些。

## 代换失败与硬错误

只有在函数类型或其模板形参类型或其 explicit 说明符 (C++20 起)的 <font color=blue>立即语境</font> 中的类型与表达式中的失败，才是 <font color=blue>SFINAE 错误</font>。如果对代换后的类型表达式的<font color=blue>求值导致副作用</font>，例如实例化某模板特化、生成某隐式定义的成员函数等，那么这些副作用中的错误都被当做 <font color=blue>硬错误</font>。

> SFINAE 只对“替换当下正在看的那一小段声明本身”生效；只要你为了判断对不对而“顺带做了别的事”，那些事一旦出错，就不是 SFINAE，而是硬错误。
>
> 立即语境 = 编译器在做“模板实参替换”时，为了形成该声明本身而必须检查的那一小块代码。
>
> “副作用”指的是为了判断一个类型/表达式是否合法，**不得不额外实例化别的模板或生成函数定义**。一旦发生这种事，**你已经离开了“立即语境”**。

**例1：典型示例**

``` cpp
template<typename T>
auto f(T) -> decltype(T::value) {
    return {};
}
```

* 如果 T 是 `struct A {};`：
  * `A::value` 不存在：
  * 👉 `f<A>` 被移除
  * 👉 SFINAE 生效

``` cpp
template<typename T>
auto f(T) -> typename T::type::value {
    return {};
}
```

* 假设：

  ``` cpp
  struct A {
      using type = int;
  };
  ```

  这里为了判断 `T::type::value`

  编译器必须：

  - 实例化 `T::type`
  - 然后去看 `int::value`（非法）

  👉 这一步 **触发了对 `int` 的成员查找（副作用）**

  ❌ 不是 SFINAE，是硬错误

**例2：下面的函数，它永远不是 SFINAE**

``` cpp
template<typename T>
auto f(T t) -> decltype(t.foo()) {
    t.bar();  // ❌ 即使 bar 不存在，也不是 SFINAE
}
```

* 原因：

  - 函数体 **不属于立即语境**
  - 错误发生时，模板已被选中

  👉 直接硬错误

**例3：C++20 的 `explicit(...)`**

```CPP
template<typename T>
explicit(!std::is_integral_v<T>)
Foo(T);
```

* `explicit(...)` 是 **立即语境**

  替换失败 → SFINAE

  但如果为了求值触发别的实例化 → 硬错误

## 基础使用示例

*请在完全读懂上一节内容再阅读本节内容*。

C++ 的模板，很多时候就像拼图一样，我们带入进去想，很多问题即使没有阅读规则，也可以无师自通，猜出来。

------

> ***我需要写一个函数模板 `add`，想要要求传入的对象必须是支持 `operator+` 的，应该怎么写？***

利用 SFINAE 我们能轻松完成这个需求。

cpp

```cpp
template<typename T>
auto add(const T& t1, const T& t2) -> decltype(t1 + t2){   // C++11 后置返回类型，在返回类型中运用 SFINAE
    std::puts("SFINAE +");
    return t1 + t2;
}
```

我知道你一定会有疑问

> 1. 这样有啥好处吗？使用了 SFINAE 看起来还变复杂了。我就算不用这写法，如果对象没有 `operator+` 不是一样会编译错误吗？
> 2. 虽然前面说了 SFINAE 可以影响重载决议，我知道这个很有用，但是我这个函数根本没有别的重载，这样写还是有必要的吗？

这两个问题其实是一个问题，本质上就是还是不够懂 SFINAE 或者说模板：

- 如果就是简单写一个 `add` 函数模板不使用 SFINAE，那么编译器在编译的时候，会尝试模板实例化，生成函数定义，发现你这类型根本没有 `operator+`，于是实例化模板错误。
- 如果按照我们上面的写法使用 SFINAE，根据“*代换失败不是错误*”的规则，从重载集中丢弃这个特化 `add`，然而又没有其他的 `add` 重载，所以这里的错误是“**未找到匹配的重载函数**”。

这里的重点是什么？**是模板实例化，能不要实例化就不要实例化**，我们当前的示例只是因为 `add` 函数模板非常的简单，即使实例化错误，编译器依然可以很轻松的报错告诉你，是因为没有 `operator+`。但是很多模板是非常复杂的，编译器实例化模板经常会产生一些完全不可读的报错；如果我们使用 SFINAE，编译器就是直接告诉我：“未找到匹配的重载函数”，我们自然知道就是传入的参数没有满足要求。而且实例化模板也是有开销的，很多时候甚至很大。

总而言之： **即使不为了处理重载，使用 SFINAE 约束函数模板的传入类型，也是有很大好处的：报错、编译速度**。

但是令人诟病的是 SFINAE 的写法在很多时候非常麻烦，目前各位可能还是没有感觉，后面的需求，写出的示例，慢慢的你就会感觉到了。这些问题会在下一章的[约束与概念](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/11约束与概念)解决。

## 标准库支持

标准库提供了一些设施帮助我们更好的使用 SFINAE。

### `std::enable_if`

```cpp
template<bool B, class T = void>
struct enable_if {};
 
template<class T> // 类模板偏特化
struct enable_if<true, T> { typedef T type; };     // 只有 B 为 true，才有 type，即 ::type 才合法

template< bool B, class T = void >
using enable_if_t = typename enable_if<B,T>::type; // C++14 引入
```

这是一个模板类，在 C++11 引入，它的用法很简单

* 第一个模板参数为 true，此模板类就有 `type`，替换成功
* 否则，第一个模板参数为 false，没有 `type`，以此进行 SFINAE。

例如，我们希望参与 `add` 运算的都是整数：

```cpp
#include <iostream>
#include <type_traits>

template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T add(T a, T b) {
    return a + b;
}

int main() {
    int x = 1, y = 2;
    std::cout << add(x, y) << std::endl;
    std::string s1 = "hello", s2 = "world";
    // std::cout << add(s1, s2) << std::endl; // no matching function for call to ‘add(std::string&, std::string&)’
    double d = 3.14;
    // std::cout << add(x, d) << std::endl; // error: no matching function for call to ‘add(int&, double&)’
}
```

------

再谈，`std::enable_if` 的默认模板实参是 **`void`**，如果我们不在乎 `std::enable_if` 得到的类型，就让它默认就行，比如我们先前的示例 `add` 根本不在乎第二个模板形参 `SFINAE` 是啥类型。

此外，`std::enable_if` 还有一种常见的用法，即利用其第二个模板参数而不声明额外的模板类型参数。

例如，下面是仿 [libstdc++](https://github.com/gcc-mirror/gcc/blob/7a01cc711f33530436712a5bfd18f8457a68ea1f/libstdc%2B%2B-v3/include/std/array#L292-L295)  `std::array` 的声明：

```cpp
template<typename Type, typename... Args>
array(Type, Args...) -> array<std::enable_if_t<(std::is_same_v<Type, Args> && ...), Type>, sizeof...(Args) + 1>;

```

它利用了 C++17 引入的折叠表达式和 C++17 引入的用户指定推导指引。

这样做有很多好处，老式写法存在很多问题：

```cpp
template<class Ty, std::size_t size>
struct array {
    Ty arr[size];
};

template<typename T, typename ...Args>
array(T t, Args...) -> array<T, sizeof...(Args) + 1>;

::array arr{1.4, 2, 3, 4, 5};        // 被推导为 array<double,5>
::array arr2{1, 2.3, 3.4, 4.5, 5.6}; // 被推导为 array<int,5>    有数据截断
```

如果不使用 SFINAE 约束，那么 array 的类型完全取决于第一个参数的类型，很容易导致其他问题。

### `std::void_t`

```cpp
template< class... >
using void_t = void;
```

如你所见，它的实现非常非常的简单，就是一个别名，**接受任意个数的类型参数，但自身始终是 `void` 类型。**

- 将任意类型的序列映射到类型 void 的工具元函数。
- 模板元编程中，用此元函数检测 SFINAE 语境中的非良构类型[[2:2\]](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fn2)。

------

> *我要写一个函数模板 `add`，我要求传入的对象需要支持 `+` 以及它需要有别名 `type` ，成员 `value`、`f`*。

```cpp
#include <iostream>
#include <type_traits>

template<typename T,
    typename SFINAE = std::void_t<
    decltype(T{} + T{}), typename T::type, decltype(&T::value), decltype(&T::f) >>
auto add(const T& t1, const T& t2) {
    std::puts("SFINAE + | typename T::type | T::value");
    return t1 + t2;
}

struct Test {
    int operator+(const Test& t)const {
        return this->value + t.value;
    }
    void f()const{}
    using type = void;
    int value;
};

int main() {
    Test t{ 1 }, t2{ 2 };
    add(t, t2);  // OK
    //add(1, 2); // 未找到匹配的重载函数
}
```

- `decltype(T{} + T{})` 用 decltype 套起来只是为了获得类型符合语法罢了，std::void_t 只接受类型参数。如果类型没有 `operator+`，自然是*代换失败*。
- `typename T::type` 使用 `typename` 是因为[待决名](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/09待决名)；type 本身是类型，不需要 decltype。如果 `add` 推导的类型没有 `type` 别名，自然是*代换失败*。
- `decltype(&T::value)` 用 decltype 套就不用说了，`&T::value` 是[成员指针](https://zh.cppreference.com/w/cpp/language/pointer#.E6.88.90.E5.91.98.E6.8C.87.E9.92.88)的语法，不区分是数据成员还是成员函数，如果有这个成员 `value`，`&类名::成员名字` 自然合法，要是没有，就是*代换失败*。
- `decltype(&T::f)` ，其实前面已经说了，成员函数是没区别的，没有成员 `f` 就是 *代换失败*。

总而言之，这是为了使用 SFINAE。

> 那么这里 `std::void_t` 的作用是？

其实倒也没啥，无非就是给了个好的语境，让我们能这样写，最终 `typename SFINAE = std::void_t` 这里的 `SFINAE` 的类型就是 `void`；当然了，这不重要，重要的是创造这样写的语境，能够方便我们进行 **`SFINAE`**。

仅此一个示例，我相信就足够展示 `std::void_t` 的使用了。

> *那么如果在 C++17 标准之前，没有 std::void_t ，我该如何要求类型有某些成员呢？*

其实形式和原理都是一样的。

```cpp
template<typename T,typename SFINAE = decltype(&T::f)>
void f(T){}

struct Test {
    void f()const{}
};

Test t;
f(t);  // OK
f(1);  // 未找到匹配的重载函数
```

C++11 可用。

### `std::declval`

```cpp
template<class T>
typename std::add_rvalue_reference<T>::type declval() noexcept;
```

`std::declval<T>()` 返回一个类型为 `T&&` 的表达式（而不是一个对象）。

将任意类型 T 转换成引用类型，*使得在 decltype 说明符的操作数中<font color="blue">不必经过构造函数</font>就能使用成员函数*。

- [std::declval](https://zh.cppreference.com/w/cpp/utility/declval) 只能用于 **[不求值语境](https://zh.cppreference.com/w/cpp/language/expressions#.E6.BD.9C.E5.9C.A8.E6.B1.82.E5.80.BC.E8.A1.A8.E8.BE.BE.E5.BC.8F)**，且 **不要求有定义**。
- **它不能被实际调用，因此不会返回值，返回类型是 `T&&`**。

它常用于模板元编程 SFINAE，我们用一个示例展现它的必要性：

```cpp
template<typename T, typename SFINAE = std::void_t<decltype(T{} + T{})>>
auto add(const T& t1, const T& t2) {
    std::puts("SFINAE +");
    return t1 + t2;
}

struct X{
    int operator+(const X&)const{
        return 0;
    }
};

struct X2 {
    X2(int){}   // 有参构造，没有默认构造函数
    int operator+(const X2&)const {
        return 0;
    }
};

int main(){
    X x1, x2;
    add(x1, x2);          // OK

    X2 x3{ 0 }, x4{ 0 };
    add(x3,x4);           // 未找到匹配的重载函数
}
```

错误的原因很简单，`decltype(T{} + T{})` 这个表达式中，同时**要求了 `T` 类型支持默认构造**（虽然这不是我们的本意），然而我们的 `X2` 类型没有默认构造，自然而然 `T{}` 不是合法表达式，*代换失败*。其实我们之前也有类似的写法，我们在本节进行纠正，使用 `std::declval`：

```cpp
template<typename T, typename SFINAE = std::void_t<decltype(std::declval<T>() + std::declval<T>())> >
auto add(const T& t1, const T& t2) {
    std::puts("SFINAE +");
    return t1 + t2;
}
```

把 `T{}` 改成 `std::declval<T>()` 即可，decltype 是不求值语境，没有问题。

------

还不止如此，使用它得以让我们先前的 `SFINAE` 检查类型是否有某些成员的形式得以改进，而不是像之前一样的 `decltype(&T::value), decltype(&T::f)` 的利用成员指针的形式。

```cpp
#include <iostream>
#include <type_traits>

template<typename T,typename SFINAE = decltype(std::declval<T>().f(1))>
void f(int) { std::puts("f int"); }

template<typename T, typename SFINAE = decltype(std::declval<T>().f())>
void f(double) { std::puts("f double"); }

struct X{
    void f()const{}
};
struct Y{
    void f(int)const{}
};

int main(){
    f<X>(1);   // f double 
    f<Y>(1.1); // f int
}
```

显而易见，虽然我们的 `f<X>(1)` 传递的参数是 int 类型，但是却打印了 `f double`，也就是代表实际匹配到了参数为 `f(double)` 的版本，这是因为我们的 `f(int)` 版本的 `SFINAE` 约束要求了类型必须是支持 `f(1)` 这种形式，`X` 的成员函数 `f` 是空参的，自然不满足。

**使用此种方式得以更加明确的约束，因为不管成员函数 f 的形参是什么情况，其成员指针表示形式都是：`&类名::f`。**

数据成员同样可以使用 `declval` 进行约束：

```cpp
template<typename T, typename SFINAE = decltype(std::declval<T>().value)>
void f(int) { std::puts("f value"); }

template<typename T>
void f(double) { std::puts("f"); }

struct X {
    int value{};
};
struct Y {};

int main() {
    f<X>(1); // f value
    f<Y>(1); // f
}
```

`f<Y>(1)` 虽然传递的参数是 int 类型，但是因为 `Y` 不满足 `SFINAE` 的约束，即没有成员 `value`，所以只能选择到 `f(double)` 的版本。

## 部分（偏）特化中的 SFINAE

在确定一个类或变量 (C++14 起)模板的特化是由部分特化还是主模板生成的时候也会出现推导与替换。在这种确定期间，**部分特化的替换失败不会被当作硬错误，而是像函数模板一样 SFINAE，只是忽略这个部分特化**。

```cpp
#include <iostream>

template<typename T,typename T2 = void>
struct X{
    static void f() { std::puts("主模板"); }
};

template<typename T>
struct X<T, std::void_t<typename T::type>>{
    using type = typename T::type;
    static void f() { std::puts("偏特化 T::type"); }
};

struct Test1 { using type = int; };
struct Test2 { };

int main(){
    X<Test1>::f();      // 偏特化 T::type
    X<Test2>::f();      // 主模板
}
```

## 总结

到此，其实就足够了，SFINAE 的原理、使用、标准库支持（std::enable_if、std::void_t、std::declval）。

虽然称不上全部，但如果你能完全理解明白本节的所有内容，那你一定超越了至少 95% C++ 开发者。其他的各种形式无非都是这样类似的，因为我们已经为你讲清楚了 ***原理***。

- ***代换失败不是错误***。

------

1. 注：“[重载决议](https://zh.cppreference.com/w/cpp/language/overload_resolution)”，简单来说，一个函数被重载，编译器必须决定要调用哪个重载，我们决定调用的是各形参与各实参之间的匹配最紧密的重载。 [↩︎](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fnref1)
2. 注：[非良构（ill-formed）](https://zh.cppreference.com/w/cpp/language/ub)——程序拥有语法错误或可诊断的语义错误。遵从标准的 C++ 编译器必须为此给出诊断。 [↩︎](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fnref2) [↩︎](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fnref2:1) [↩︎](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/10了解与利用SFINAE#fnref2:2)

# 约束和概念

类模板，函数模板，以及非模板函数（通常是类模板的成员），可以与一项约束（constraint）相关联，它指定了对模板实参的一些要求，这些要求可以被用于选择最恰当的函数重载和模板特化。

这种要求的具名集合被称为概念（concept）。每个概念都是一个谓词，它在编译时求值，并在将之用作约束时成为模板接口的一部分。

## 前言

在 C++20 引入了约束与概念，这一核心语言特性是所有使用模板的 C++ 开发者都期待的。

有了它，我们的模板可以有更多的静态检查，语法更加美观，写法更加容易，而不再需要利用古老的 **SFINAE**。

请务必学习完了上一章节内容；本节会一边为你教学约束与概念的语法，一边用 SFINAE 对比，让你意识到：***这是多么先进、简单的核心语言特性***。

## 定义*概念*（concept）与使用

cpp

```
template < 模板形参列表 >
concept 概念名 属性 (可选) = 约束表达式;
```

定义概念所需要的 *约束表达式*，只需要是可以在编译期产生 `bool` 值的表达式即可。

> - 你可以先不看基本概念，关注我们的示例和下面的讲解。

------

> ***我需要写一个函数模板 `add`，想要要求传入的对象必须是支持 `operator+` 的，应该怎么写？***

此需求就是 `SFINAE` 中提到的，我们使用*概念*（concept）来完成。

cpp

```
template<typename T>
concept Add = requires(T a) {
    a + a; // "需要表达式 a+a 是可以通过编译的有效表达式"
};

template<Add T>
auto add(const T& t1, const T& t2){
    std::puts("concept +");
    return t1 + t2;
}
```

我们使用关键字 `concept` 定义了一个*概念*（concept），命名为 `Add`，它的*约束*是 `requires(T a) { a + a; }` 即要求 `f(T a)`、`a + a` 是合法表达式。

cpp

```
template<Add T> // T 被 Add 约束
```

语法上就是把原本的 `typename` 、`class` 换成了我们定义的 `Add` *概念*（concept），语义和作用也非常的明确：

- **就是让这个概念约束模板类型形参 `T`，即要求 `T` 必须满足\*约束表达式\*的\*要求序列\* `T a` `a + a`**。如果不满足，则不会选择这个模板。

> "满足"：要求带入后必须是合法表达式；

最开始的概念已经说了：

> *概念*（concept）可以与一项约束（constraint）相关联，它指定了对模板实参的一些要求，这些要求可以被用于选择最恰当的函数重载和模板特化。

另外最开始的概念中还说过：

> 每个概念都是一个**谓词**，它在**编译时求值**，并在将之用作约束时成为模板接口的一部分。

也就是说我们其实可以这样：

cpp

```
std::cout << std::boolalpha << Add<int> << '\n';        // true
std::cout << std::boolalpha << Add<char[10]> << '\n';   // false
constexpr bool r = Add<int>;                            // true
```

我相信这非常的好理解，这些语法形式，合理且简单。

*记得我们在第一章节[函数模板](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/01函数模板)中提到的：“C++20 简写函数模板”吗？*

cpp

```
decltype(auto) max(const auto& a, const auto& b)  { // const T&
    return a > b ? a : b;
}
```

这段代码来自函数模板那一章节。

> **我想要约束：传入的对象 a b 必须都是整数类型，应该怎么写？**。

cpp

```
#include <concepts> // C++20 概念库标头

decltype(auto) max(const std::integral auto& a, const std::integral auto& b) {
    return a > b ? a : b;
}

max(1, 2);     // OK
max('1', '2'); // OK
max(1u, 2u);   // OK
max(1l, 2l);   // OK
max(1.0, 2);   // Error! 未满足关联约束
```

如你所见，我们没有自己定义 *概念*（concept），而是使用了标准库的 [`std::integral`](https://zh.cppreference.com/w/cpp/concepts/integral)，它的实现非常简单：

cpp

```
template< class T >
concept integral = std::is_integral_v<T>;
```

这也告诉各位我们一件事情：**定义\*概念\*（concept）** 时声明的约束表达式，只需要是编译期可得到 `bool` 类型的表达式即可。

> 我相信你这里一定有疑问：“那么我们之前写的 requires 表达式呢？它会返回 `bool` 值吗？” 对，简单的说，把模板参数带入到 `requires` 表达式中，是否符合语法，符合就返回 `true`，不符合就返回 `false`。在 [`requires` 表达式](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/11约束与概念#requires-表达式) 一节中会详细讲解。

它的实现是直接使用了标准库的 `std::is_integral_v<T>`，非常简单。

再谈*概念*（concept）在简写函数模板中的写法 `const std::integral auto& a`，*概念*（concept）只需要写在 `auto` 之前即可，表示此概念约束 `auto` 推导的类型必须为整数类型，语义十分明确，像是 cv 限定、引用等，不需要在乎，或许我们可以先写的简单一点先去掉那些：

cpp

```
decltype(auto) max(std::integral auto a, std::integral auto b) {
    return a > b ? a : b;
}
```

这是否直观了很多？并且概念不单单是可以用作简写函数模板中的 `auto`，还有几乎一切语境，比如：

cpp

```
int f() { return 0; }

std::integral auto result = f();
```

还是那句话，语义很明确：

- ***概念\*（concept）约束了 `auto` ，它必须被推导为整数类型；如果函数 `f()` 返回类型是 `double` `auto` 无法推导为整数类型，那么编译器会报错：“\*未满足关联约束\*”**。

------

类模板同理，如：

cpp

```
template<typename T>
concept add = requires(T t){  // 定义概念，通常推荐首字母小写
    t + t;
};

template<add T>
struct X{
    T t;
};
```

变量模板也同理

cpp

```
template<typename T>
concept add = requires(T t){
    t + t;
};

template<add T>
T t;

t<int>;     // OK
t<char[1]>; // “t”未满足关联约束
```

将模板中的 `typename` 、`class` 换成 *概念*（concept）即可，表示约束此模板类型形参 `T`。

## `requires` 子句

关键词 requires 用来引入 requires 子句，它指定对各模板实参，或对函数声明的约束。

也就是说我们多了一种使用*概念*（concept）或者说约束的写法。

cpp

```
template<typename T>
concept add = requires(T t) {
    t + t;
};

template<typename T>
    requires std::is_same_v<T, int>
void f(T){}

template<typename T> requires add<T>
void f2(T) {}

template<typename T>
void f3(T)requires requires(T t) { t + t; }
{}
```

> `requires` 子句期待一个能够编译期产生 `bool` 值的表达式。

以上示例展示了 `requires` 子句的用法，我们一个个解释

1. `f` 的 `requires` 子句写在 `template` 之后，并空四格，这是我个人推荐的写法；它的约束是：`std::is_same_v<T, int>`，意思很明确，约束 `T` 必须是 int 类型，就这么简单。
2. `f2` 的 `requires` 子句写法和 `f` 其实是一样的，只是没换行和空格；它使用了我们自定义的*概念*（concept）`add`，约束 `T` 必须满足 `add`。
3. `f3` 的 `requires` 子句在函数声明符的末尾元素出现；这里我们连用了两个 `requires`，为什么？其实很简单，我们要区分，第一个 `requires` 是 *`requires` 子句*，第二个 `requires` 是*约束表达式*，它会产生一个编译期的 `bool` 值，没有问题。（如果 `T` 类型带入*约束表达式*是良构，那么就返回 `true`、否则就返回 `false`）。

> 类模板、变量模板等也都同理

requires 子句中，**关键词 requires 必须后随某个常量表达式**。

cpp

```
template<typename T>
    requires true
void f(T){}
```

完全可行，各位其实可以直接带入，说白了 `requires` 子句引入的约束表，必须是可以编译期返回 `bool` 类型的值的表达式，我们前面的三个例子：`std::is_same_v`、`add`、`requires 表达式` 都如此。

## 约束

前面我们讲的都是非常基础的*概念*（concept）使用，它们的约束也都十分简单，本节我们详细讲一下。

约束是逻辑操作和操作数的序列，它指定了对模板实参的要求。它们可以在 requires 表达式（见下文）中出现，也可以直接作为概念的主体。

有三种类型的约束：

1. 合取（conjunction）
2. 析取（disjunction）

### 合取

两个约束的合取是通过在约束表达式中使用 && 运算符来构成的：

cpp

```
template<class T>
concept Integral = std::is_integral_v<T>;
template<class T>
concept SignedIntegral = Integral<T> && std::is_signed_v<T>;
template<class T>
concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;
```

很合理，**约束表达式**可以使用 `&&` 运算符连接两个约束，只有在两个约束都被满足时才会得到满足

我们先定义了一个 *概念*（concept）Integral，此概念要求整形；又定义了*概念*（concept）SignedIntegral，它的约束表达式用到了先前定义的*概念*（concept）Integral，然后又加上了一个 **`&&`** 还需要满足 std::is_signed_v。

*概念*（concept）`SignedIntegral` 是要求有符号整数类型，它的*约束表达式*是：**`Integral<T> && std::is_signed_v<T>`**，就是这个表达式要返回 `true` 才成立，就这么简单。

cpp

```
void s_f(const SignedIntegral auto&){}
void u_f(const UnsignedIntegral auto&){}

s_f(1);   // OK
s_f(1u);  // 未找到匹配的重载函数
u_f(1);   // 未找到匹配的重载函数
u_f(1u);  // OK
```

> ***两个约束的合取只有在两个约束都被满足时才会得到满足**。合取从左到右短路求值（如果不满足左侧的约束，那么就不会尝试对右侧的约束进行模板实参替换：这样就会防止出现立即语境外的替换所导致的失败）*。

cpp

```
struct X{
    int c{}; // 无意义，为了扩大 X
    static constexpr bool value = true;
};

template<typename T>
constexpr bool get_value() { return T::value; }

template<typename T>
    requires (sizeof(T) > 1 && get_value<T>())
void f(T){}

X x;
f(x); // OK
```

### 析取

两个约束的析取，是通过在约束表达式中使用 || 运算符来构成的：

cpp

```
template<typename T>
concept number = std::integral<T> || std::floating_point<T>;
```

和 `||` 运算符本来的意思一样， `std::integral<T>`、`std::floating_point` 满足任意一个，那么整个约束表达式就都得到满足。

cpp

```
void f(const number auto&){}

f(1);      // OK 
f(1u);     // OK 
f(1.2);    // OK 
f(1.2f);   // OK 
f("1");    // 未找到匹配的重载函数
```

> *如果其中一个约束得到满足，那么两个约束的析取的到满足。析取从左到右短路求值（如果满足左侧约束，**那么就不会尝试对右侧约束进行模板实参替换**）*。

cpp

```
struct X{
    int c{}; // 无意义，为了扩大 X
    //static constexpr bool value = true;
};

template<typename T>
constexpr bool get_value() { return T::value; }

template<typename T>
    requires (sizeof(T) > 1 || get_value<T>())
void f(T){}

X x;
f(x);  // OK 即使 X 根本没有静态 value 成员。
```

如你所见，即使我们的 X 根本不满足右侧约束 `get_value<T>()` 的要求，没有静态 `value` 成员，不过一样可以通过编译。

## `requires` 表达式

> **产生描述约束的 bool 类型的纯右值表达式**。

虽然前面聊*概念*（concept）的时候，用到了 `requires` 表达式（定义 concept 的时候），但是没有详细说明，本节我们详细展开说明。

> *注意，`requires` 表达式 和 `requires` 子句，没关系*。

txt

```
requires { 要求序列 }
requires ( 形参列表 (可选) ) { 要求序列 }
```

要求序列，是以下形式之一：

- 简单要求
- 类型要求
- 复合要求
- 嵌套要求

### 解释

要求可以援引处于作用域内的模板形参，形参列表中引入的局部形参，以及在上下文中可见的任何其他声明。

- 将模板参数**代换**到模板化实体的声明中所使用的 requires 表达式中，*可能会导致在其要求中形成无效的类型或表达式，或者违反这些要求的语义*。在这种情况下，requires 表达式的值为 **`false`** 而不会导致程序非良构。
- 按照词法顺序进行**代换**和语义约束检查，当遇到决定 requires 表达式结果的条件时就停止。*如果代换（若存在）和语义约束检查成功*，则 requires 表达式的结果为 **`true`**。

> **简单的说，把模板参数带入到 requires 表达式中，是否符合语法，符合就返回 `true`，不符合就返回 `false`**。

cpp

```
#include <iostream>

template<typename T>
void f(T) {
    constexpr bool v = requires{ T::type; }; // 此处可不使用 typename
    std::cout << std::boolalpha << v << '\n';
}

struct X { using type = void; };
struct Y { static constexpr int type = 0; };

int main() {
    f(1);   // false 因为 int::type 不是合法表达式
    f(X{}); // false 因为 X::type   在待决名中不被认为是类型，需要添加 typename
    f(Y{}); // true  因为 Y::type   是合法表达式
}
```

> 三端[测试](https://godbolt.org/z/sxf4PnsfG)。

### 简单要求

简单要求是任何不以关键词 requires 开始的表达式语句。它断言该表达式是有效的。表达式是不求值的操作数；只检查语言的正确性。

cpp

```
template<typename T>
concept Addable = requires (T a, T b) {
    a + b; // "需要表达式 a+b 是可以通过编译的有效表达式"
};

template<class T, class U = T>
concept Swappable = requires(T && t, U && u) {
    swap(std::forward<T>(t), std::forward<U>(u));
    swap(std::forward<U>(u), std::forward<T>(t));
};

template<typename T>
    requires (Addable<T> && Swappable<T, T>)
struct Test{};

namespace loser{
    struct X{
        X operator+(const X&)const{
            return *this;
        }
    };
    void swap(const X&,const X&){}
}

int main() {
    using loser::X;

    Test<X> t2; // OK
    std::cout << std::boolalpha << Addable<X> << '\n';     // true
    std::cout << std::boolalpha << Swappable<X,X> << '\n'; // true
}
```

> 以上代码利用了实参依赖查找（`ADL`），即 `swap(X{})` 是合法表达式，而不需要增加命名空间限定。

以关键词 requires 开始的要求总是被解释为[嵌套要求](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/11约束与概念#嵌套要求)。因此简单要求**不能以没有括号的 requires 表达式开始**。

### 类型要求

类型要求是关键词 **`typename`** 后面接一个可以被限定的**类型名称**。该要求是，所指名的类型是有效的。

可以用来验证：

1. 某个指名的嵌套类型是否存在
2. 某个类模板特化是否指名了某个类型
3. 某个别名模板特化是否指名了某个类型。

cpp

```
struct Test{
    struct X{};
    using type = int;
};

template<typename T>
struct S{};

template<typename T>
using Ref = T&;

template<typename T>
concept C = requires{
    typename T::X;      // 需要嵌套类型
    typename T::type;   // 需要嵌套类型
    typename S<T>;      // 需要类模板特化
    typename Ref<T>;    // 需要别名模板代换
};

std::cout << std::boolalpha << C<Test> << '\n'; // true
```

稍微解释一下，类 `Test` 有一个嵌套类 `X`，一个别名 `type`，所以 `typename T::X`、`typename T::type` 类型是有效的。

`typename S<T>` 因为有类模板 `S`，且它接受类型模板参数，所以 `typename S<T>` 类型是有效的。假设模板类 `S` 的模板是接受非类型模板参数的，比如 `template<std::size_t> S` ，那么 `typename S<T>` 类型自然不是有效的。

`typename Ref<T>` 因为有别名模板 `Ref`，自然没问题，类型自然是有效的。

> 其实说来说去也很简单，你就直接**带入**，把*概念*（concept）的模板实参（比如 Test）直接带入进去 `requires` 表达式，想想它是不是合法的表达式就可以了。

### 复合要求

复合要求具有如下形式

txt

```
{ 表达式 } noexcept(可选) 返回类型要求 (可选) ;
```

> 返回类型要求：-> 类型约束（*概念* concept）

并断言所指名表达式的属性。替换和语义约束检查按以下顺序进行：

1. 模板实参 (若存在) 被替换到 表达式 中；

2. 如果使用了`noexcept`，表达式 一定不能潜在抛出；

3. 如果

   返回类型要求

    

   存在，则：

   - 模板实参被替换到*返回类型要求* 中；
   - `decltype((表达式))` 必须满足*类型约束* 蕴含的约束。否则，被包含的 requires 表达式是 **`false`**。

cpp

```
template<typename T>
concept C2 = requires(T x){
    // 表达式 *x 必须合法
    // 并且 类型 T::inner 必须存在
    // 并且 *x 的结果必须可以转换为 T::inner
    {*x} -> std::convertible_to<typename T::inner>;

    // 表达式 x + 1 必须合法
    // 并且 std::same_as<decltype((x + 1)), int> 必须满足
    // 即, (x + 1) 必须为 int 类型的纯右值
    {x + 1} -> std::same_as<int>;

    // 表达式 x * 1 必须合法
    // 并且 它的结果必须可以转换为 T
    {x * 1} -> std::convertible_to<T>;

    // 复合："x.~T()" 是不会抛出异常的合法表达式
    { x.~T() } noexcept;
};
```

我们可以写一个满足*概念*（concept）`C2` 的类型：

cpp

```
struct X{
    int operator*()const { return 0; }
    int operator+(int)const { return 0; }
    X operator*(int)const { return *this; }
    using inner = int;
};
```

cpp

```
std::cout << std::boolalpha << C2<X> << '\n'; // true
```

[测试](https://godbolt.org/z/vWhcPjbe7)。

**析构函数比较特殊**，不需要我们显式声明它为 `noexcept` 的，它默认就是 `noexcept` 的。

不管编译器为我们生成的 `X` 析构函数，还是我们用户显式定义的 `X` 析构函数，默认都是有 `noexcept` 的[[1\]](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/11约束与概念#fn1)。只有我们用户定义析构函数的时候把它声明为了 `noexcept(false)` 这个析构函数才不是 `noexcept` 的，才会不满足 *概念*（concept）`C2` 的要求。

### 嵌套要求

嵌套要求具有如下形式

txt

```
requires 约束表达式 ;
```

它可用于根据本地形参指定其他约束。*约束表达式* 必须由被替换的模板实参（若存在）满足。将模板实参替换到嵌套要求中会导致替换到 *约束表达式* 中，但仅限于确定是否满足 *约束表达式* 所需的程度。

cpp

```
template<typename T>
concept C3 = requires(T a, std::size_t n) {
    requires std::is_same_v<T*, decltype(&a)>;     // 要求 is_same_v          求值为 true
    requires std::same_as<T*, decltype(new T[n])>; // 要求 same_as            求值为 true
    requires requires{ a + a; };                   // 要求 requires{ a + a; } 求值为 true
    requires sizeof(a) > 4;                        // 要求 sizeof(a) > 4      求值为 true
};
std::cout << std::boolalpha << C3<int> << '\n';    // false
std::cout << std::boolalpha << C3<double> << '\n'; // true
```

> 嵌套要求的 *约束表达式*，只要能编译期产生 `bool` 值的表达式即可，*概念*（concept）、[类型特征](https://zh.cppreference.com/w/cpp/meta#.E7.B1.BB.E5.9E.8B.E7.89.B9.E5.BE.81)的库、`requires` 表达式，等都一样。

这里用 `std::is_same_v` 和 `std::same_as` 其实毫无区别，因为它们都是编译时求值，返回 `bool` 值的表达式。

在上面示例中 `requires requires{ a + a; }` 其实是更加麻烦的写法，目的只是为了展示 `requires` 表达式是编译期产生 `bool` 值的表达式，所以有可能会有**两个 `requires`连用的情况**；我们完全可以直接改成 `a + a`，效果完全一样。

## 部分（偏）特化中使用*概念*

我们在讲 SFINAE 的时候[提到](https://github.com/Mq-b/Modern-Cpp-templates-tutorial/blob/main/md/第一部分-基础知识/10了解与利用SFINAE.md#部分偏特化中的-sfinae)了，它可以用作模板偏特化，帮助我们选择特化版本；本节的约束与概念当然也可以做到，并且写法**更加简单直观优美**：

cpp

```
#include <iostream>

template<typename T>
concept have_type = requires{
    typename T::type;
};

template<typename T>
struct X {
    static void f() { std::puts("主模板"); }
};

template<have_type T>
struct X<T> {
    using type = typename T::type;
    static void f() { std::puts("偏特化 T::type"); }
};

struct Test { using type = int; };
struct Test2 { };

int main() {
    X<Test>::f();       // 偏特化 T::type
    X<Test2>::f();      // 主模板
}
```

这个示例完全是从 SFINAE 的写法改进而来，我们不需要再写第二个模板类型参数，我们直接写作 `template<have_type T>` 就完成了，概念约束了模板类型参数 `T`。

- **只有概念被满足的时候，才会选择到这个偏特化**。

一些实际的用途，比如我以前的 [`C++20 STL Cookbook`](https://github.com/Mq-b/Cpp20-STL-Cookbook-src#76使用格式库格式化文本) 中对 [`std::formatter`](https://zh.cppreference.com/w/cpp/utility/format/formatter) 进行偏特化，也是使用的概念，[`std::ranges::range`](https://zh.cppreference.com/w/cpp/ranges/range)。

## 总结

我们先讲述了 *概念*（concept）的定义和使用，其中使用到了 `requires` 表达式，但是我们留到了后面详细讲述。

其实本章内容可以划分为两个部分

- 约束与概念
- `requires` 表达式

如果你耐心看完，我相信也能意识到它们是互相掺杂，一起使用的。语法上虽然感觉有些多，但是也都很合理，我们只需要 ***带入***，按照基本的常识判断这是不是符合语法，基本上就可以了。

`requires` 关键字的用法很多，但是划分的话其实就两类

- `requires` 子句
- `requires` 表达式

`requires` 子句和 `requires` 表达式可以连用，组成 `requires requires` 的形式。我们在 [`requires` 子句](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/11约束与概念#requires-子句)讲过。

还有在 `requires` 表达式中的嵌套要求，也会有 `requires requires` 的形式。

如果看懂了，这些看似奇怪的 `requires` 关键字复用，其实也都很合理，只需要记住最重要的一句话：

> 可以连用 `requires requires` 的情况，都是因为第一个 `requires` 期待一个可以编译期产生 `bool` 值的表达式；而 **`requires` 表达式就是产生描述约束的 bool 类型的纯右值表达式**。

------

1. 参见[文档](https://zh.cppreference.com/w/cpp/language/destructor#:~:text=如果没有显,C%2B%2B11 起)。 [↩︎](https://mq-b.github.io/Modern-Cpp-templates-tutorial/md/第一部分-基础知识/11约束与概念#fnref1)

# CRTP

它的范式基本上都是：父类是模板，再定义一个类类型继承它。因为类模板不是类，只有实例化后的类模板才是实际的类类型，所以我们需要实例化它，**显式指明模板类型参数，而这个类型参数就是我们定义的类型，也就是子类了**。

```cpp
template <class Dervied>
class Base {};

class X : public Base<X> {};
```

## 编译期多态

CRTP 可用于在父类暴露接口，而子类实现该接口，以此实现“***编译期多态***”，或称“***静态多态***”。 示例如下：

```cpp
#include <iostream>

using namespace std;

template<typename Derived>
class Base {
public:
    void add_water(int amount) {
        static_cast<Derived*>(this)->impl(amount);
    }
};

class Derived1 : public Base<Derived1> {
private:
    friend Base<Derived1>;
    void impl(int amount) {
        std::cout << "Derived::add water.\n";
    }
};

class Derived2 : public Base<Derived2> {
private:
    friend Base<Derived2>;
    void impl(int amount) {
        std::cout << "Derived::add water.\n";
    }
};

template<typename T>
void process_add_water(Base<T>& r, int amount) {
    if(amount > 0) {
        r.add_water(amount);
    }
    else {
        std::cerr << "not valid amount: " << amount << "\n";
    }
}

int main()
{
    Derived1 d1;
    Derived2 d2;
    process_add_water(d1, 10);
    process_add_water(d2, -1);
    return 0;
}
```

那么好，问题来了，**为什么呢？** `static_cast<Dervied*>(this)` 是做了什么，它为什么可以这样？

- 很显然 `static_cast<Dervied*>(this)` 是进行了一个类型转换，将 `this` 指针（也就是**父类的指针**），转换为通过模板参数传递的类型，也就是**子类的指针**。
- **这个转换是安全合法的**。因为 this 指针实际上指向一个 X 类型的对象，X 类型对象继承了 `Base<X>` 的部分，X 对象也就包含了 `Base<X>` 的部分，所以这个转换在编译期是有效的，并且是合法的。
- 当你调用 `x.addWater()` 时，实际上是 X 对象调用了父类 `Base<X>` 的成员函数。这个成员函数内部使用 `static_cast<X*>(this)`，将 this 从 `Base<X>*` 转换为 `X*`，然后调用 X 中的 impl() 函数。这种转换是合法且安全的，且 X 确实实现了 impl() 函数。

## 显式对象形参（C++23）

C++23 引入了**显式对象形参**，让我们的 `CRTP` 的形式也出现了变化：

> [显式对象形参](https://zh.cppreference.com/w/cpp/language/member_functions#.E6.98.BE.E5.BC.8F.E5.AF.B9.E8.B1.A1.E6.88.90.E5.91.98.E5.87.BD.E6.95.B0)，顾名思义，就是将 C++23 之前，隐式的，由编译器自动将 `this` 指针传递给成员函数使用的，改成**允许用户显式写明**了，也就是：
>
> ```cpp
> struct X{
> void f(this const X& self){}
> };
> ```
>
> 它也支持模板（可以直接 `auto` 而无需再 `template<typename>`），也支持各种修饰，如：`this X self`、`this X& self`、`this const X& self`、`this X&& self`、`this auto&& self`、`const auto& self` ... 等等。

```cpp
struct Base { void name(this auto&& self) { self.impl(); } };
struct D1 : Base { void impl() { std::puts("D1::impl()"); } };
struct D2 : Base { void impl() { std::puts("D2::impl()"); } };
```

不再需要使用 `static_cast` 进行转换，直接调用即可。且如你所见，我们的显式对象形参也可以写成模板的形式：`this auto&& self`。

使用上也与之前并无区别，创建子类对象，调用接口即可。

```cpp
D1 d;
d.name();
D2 d2;
d2.name();
```

`d.name` 也就是把 `d` 传入给父类模板成员函数 `name`，`auto&&` 被推导为 `D1&`，顾名思义”***显式***“对象形参，非常的简单直观。

> [运行](https://godbolt.org/z/WW59PqEd3)测试。

## CRTP 的好处

上一节我们详细的介绍和解释了 CRTP 的编写范式和原理。现在我们来稍微介绍一下 CRTP 的众多好处。

1. **静态多态**

   CRTP 实现静态多态，无需使用虚函数，静态绑定，无运行时开销。

2. **类型安全**

   CRTP 提供了类型安全的多态性。通过模板参数传递具体的子类类型，编译器能够确保类型匹配，避免了传统向下转换可能引发的类型错误。

3. **灵活的接口设计**

   CRTP 允许父类定义公共接口，并要求子类实现具体的操作。这使得基类能够提供通用的接口，而具体的实现细节留给派生类。其实也就是说多态了。

## 总结

事实上笔者所见过的 `CRTP` 的用法也还不止如此，还有许多更加复杂，有趣的做法，不过就不想再探讨了，以上的内容已然足够。其它的做法也无非是基于以上了。

各位可以尝试尽可能的将使用虚函数的代码改成 `CRTP` ，这其实在大多数时候并不构成难度，绝大多数的多态类型都能被很轻松的改成 `CRTP` 的形式。