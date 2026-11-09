# English

* pseudo
* overhead
* native
* modifier
* directive
* hierarchy
* handle
* encapsulation
* equality
* equivalence
* address-of operator
* translation unit：编译单元

# Preface

### 1. *object oriented* 的翻译

为什么说 object"在  Object  Oriented  技术中的真正意义是“物体、物件”而非“对象、目标”。 

这确实是一个很有意思的视角，它强调了 "object" 在面向对象技术中与现实世界概念的紧密联系。之所以说 "object" 的真正意义更接近于 "物体、物件" 而非 "对象、目标"，主要是因为它强调了 **封装的数据和行为的实体性**。

让我们展开来说说：

**1. "物体、物件" 强调的是实体和属性：**

- 当我们说 "物体" 或 "物件" 时，我们通常指的是现实世界中可以触摸、感知，或者至少可以概念化为一个独立存在的实体。比如，一辆汽车、一棵树、一本书、一个灯泡等等。
- 这些 "物体" 都具有自身的 **属性 (properties/attributes)**，例如汽车的颜色、型号、速度；树的高度、种类、叶子形状；书的页数、作者、书名；灯泡的功率、颜色、亮度等等。
- 在面向对象编程中，一个 "object" 同样拥有 **数据 (data)**，这些数据就代表了 "物体" 的属性。

**2. "物体、物件" 强调的是行为和功能：**

- 现实世界中的 "物体" 不仅仅是静态的属性集合，它们还具有 **行为 (behavior/function)**。汽车可以行驶、刹车；树可以生长、落叶；书可以被阅读；灯泡可以发光、熄灭。
- 在面向对象编程中，一个 "object" 也包含 **方法 (methods)**，这些方法定义了 "物体" 所能执行的操作或功能。

**3. "对象、目标" 更偏向于关系和指代：**

- 当我们说 "对象" 或 "目标" 时，更多的是在描述一种关系或者一个动作指向的对象。比如，"研究对象"、"攻击目标"、"关注的对象" 等等。
- 虽然在编程中我们也经常使用 "对象" 来指代实例化的类，但这个词的重心更多在于它是某个操作或交互的承受者或参与者，而较少强调其内在的实体性和自包含性。

**4. 面向对象的核心思想是模拟现实世界：**

- 面向对象编程的一个核心思想就是通过软件来模拟现实世界中的实体及其相互作用。将现实世界中的 "物体" 抽象成程序中的 "object"，能够更好地理解和组织复杂的系统。
- 通过将数据（属性）和行为（方法）封装在一个 "object" 内部，我们能够更好地实现模块化、信息隐藏和代码重用，这与我们理解现实世界中独立存在的 "物体" 是类似的。

**总结来说：**

虽然在中文语境下 "对象" 已经成为 "object" 在编程领域的常用翻译，但从面向对象技术的核心理念和 "object" 本身的含义来看，将其理解为更具实体性的 "物体" 或 "物件" 能更好地体现其 **数据和行为的封装性以及与现实世界实体的对应关系**。这种理解有助于我们更深入地把握面向对象编程的本质。

当然，语言的使用是约定俗成的，"对象" 这个翻译已经被广泛接受。我们只是从一个更细致的角度来探讨 "object" 这个词在技术语境下的深层含义。

# 一、让自己习惯 C++

## Q1. 视 C++ 为一个语言联邦

* Process-Oriented C
* Object-Oriented C++
* Template C++
* STL

C++ 高效编程守则视状况而变化，取决于你使用 C++ 的哪一部分。

例如 C 部分的内置类型 `char`、`int`、`double`、`pointer` **pass-by-value** 比 **pass-by-reference** 更高效；但在 Object-Oriented 的部分，**pass-by-const reference** 一般是更高效的方式。

## Q2. 尽量以 `const`, `enum`, `inline` 替换 `#define`

其核心思想在于，用 **编译器** 代替 **预处理器**。

* 对于单纯常量，最好以 `const` 对象或 `enums` 替换 `#defines`，宏定义的常量不好 DEBUG。
* 对于形似函数的宏 (macros) ，最好改用 `template inline` 函数替换 `#defines`，这是因为宏定义的函数，很容易出错，即使我们非常小心。

## Q3. 尽可能使用 *const*

### 1. 常量返回值

令函数返回一个常量值，往往可以降低因客户错误而造成的意外，而又不至于放弃安全性和高效性。

``` cpp
struct X { int x; };

// 返回一个 const
const X operator+(X lhs, X rhs) {
    return {lhs.x + rhs.x};
}

X x1{10}, x2{20}, x3{30};
(x1 + x2) = x3; // error
```

这么做的目的主要是为了与内置类型兼容：

``` cpp
int a, b, c;
(a + b) = c; //  error: lvalue required as left operand of assignment
```

### 2. bitwise constness & logical constness

bitwise constness，即通常意义上 const 语义，也是编译器强制实施的语义。

logical constness，在这种语义下，const 成员函数可以修改成员变量（mutable），只要客户端感知不到即可。

## Q4. 确定对象被使用前已被初始化

对于内置类型对象，确保对它们完成手工初始化，因为 C++ 不保证初始化他们。对于类类型对象特别是自定义类类型对象，要么设计为无法默认初始化，要么只要初始化，就有默认值，事实上你会发现，STL 容器就是这么做的。

注意初始化顺序与声明顺序一致。

对于那些具有多个构造函数，并且构造过程比较类似的类，可以将构造过程提取为一个 `private` 方法，然后在构造函数体内调用这个方法，从而避免代码的重复。

C++ 对定义域不同的编译单元内的 non-local static 对象的初始化相对次序并无明确定义，不过，幸运的是，一个小小的设计便可完全消除该问题。只需要将 non-local static 对象放到一个函数当中（该对象在此函数内被声明为 static），让它变成一个 local static 对象，也即所谓的 Singleton 模式。

# 二、构造/析构/赋值运算

## Q5: 了解 C++ 默认编写并调用哪些函数

* `ctor`
* `copy ctor`
* `dtor`
* `operator==`

当类中包含 `const`/`reference` 成员时，赋值运算符被删除。

> Big Three 和 Big Five 是 C++ 中管理类资源和内存时非常重要的概念。它们指的是一组编译器默认生成的特殊成员函数。
>
> ### Big Three (C++98/03)
>
> 在 C++98/03 标准中，如果一个类需要自己管理资源（例如动态分配的内存、文件句柄、网络连接等），那么为了防止资源泄露和未定义行为，开发者需要**手动**定义以下三个特殊的成员函数：
>
> 1. **析构函数 (Destructor)**
>    - **作用：** 在对象生命周期结束时自动调用，用于释放对象所持有的资源，例如用 `new` 分配的内存需要用 `delete` 释放。
>    - **例子：** `~MyClass();`
> 2. **拷贝构造函数 (Copy Constructor)**
>    - **作用：** 当一个对象以另一个同类型对象为基础进行初始化时调用。它负责**深拷贝**对象内部的资源，以避免两个对象共享同一份资源（浅拷贝）。
>    - **例子：** `MyClass(const MyClass& other);`
> 3. **拷贝赋值运算符 (Copy Assignment Operator)**
>    - **作用：** 当一个对象被赋值给另一个已经存在的同类型对象时调用。它同样需要执行深拷贝，并且通常需要处理**自赋值**的情况（`a = a`）和释放旧资源。
>    - **例子：** `MyClass& operator=(const MyClass& other);`
>
> **原则：** 如果你因为资源管理需要手动定义了其中一个函数，那么通常你需要定义全部三个。这就是所谓的“**三法则 (Rule of Three)**”。
>
> ### Big Five (C++11)
>
> 随着 C++11 引入了**右值引用 (Rvalue References)** 和**移动语义 (Move Semantics)**，为了更高效地处理临时对象和资源转移，Big Three 扩展成了 Big Five。在 Big Three 的基础上，新增了两个处理移动操作的特殊成员函数：
>
> 1. **移动构造函数 (Move Constructor)**
>    - **作用：** 用于通过**移动**（而非拷贝）一个临时对象来构造新对象。它将临时对象的资源“偷”过来，并让临时对象处于一个有效的、可销毁的状态。这个过程通常比深拷贝更高效。
>    - **例子：** `MyClass(MyClass&& other);`
> 2. **移动赋值运算符 (Move Assignment Operator)**
>    - **作用：** 用于将一个临时对象的资源**移动**给一个已经存在的对象。它需要释放当前对象的旧资源，然后接管来自临时对象的资源。
>    - **例子：** `MyClass& operator=(MyClass&& other);`
>
> **原则：** 如果你手动定义了 Big Three 中的任何一个，那么通常也应该定义移动构造函数和移动赋值运算符。这就是**“五法则 (Rule of Five)”**。
>
> ### 现代 C++ 实践
>
> 在现代 C++ 中，由于 `std::unique_ptr`、`std::shared_ptr` 等智能指针和容器（如 `std::vector`）的出现，我们**很少再需要手动实现** Big Five 函数。它们已经为你处理了所有资源管理细节。因此，更好的实践是：
>
> - **如果你的类不需要任何特殊的资源管理**，编译器会自动为你生成合适的 Big Five 函数。
> - **如果你的类需要管理资源**，优先使用智能指针和标准库容器来封装这些资源。这样，你的类就不需要定义 Big Five 函数，而是由智能指针或容器的析构函数来自动完成资源管理，从而避免了“五法则”的复杂性。
>
> 只有在少数情况下，当标准库提供的工具无法满足需求时，你才需要考虑自己实现 Big Five。

## Q6：若不想使用编译器自动生成的函数，就应该明确拒绝

### 1. private 访问权限 + 不定义

注意，仅仅将不想生成的函数设置为 `private` 并不能解决问题，因为我们可以通过成员函数和友元进行访问，因此我们还需要将其设计为未定义的函数，也即不去实现它，这样即使当它被调用时，也会被报链接错误（提示函数没有定义）。

### 2. private base class

上面的方法虽然可行，但是错误是在链接时发生的，通过这次介绍的方法，我们可以将报错提前到编译期。

我们可以利用 “当基类不能拷贝时，派生了默认删除拷贝删除” 的性质：

``` cpp
class Uncopable {
protected:
    Uncopable() {}
    ~Uncopable() {}
private:
    Uncopable(const Uncopable&); // 阻止 copy ctor
    Uncopable& operator=(const Uncopable&); // 阻止 copy
};

class Name : private Uncopable {
/*...*/
}
```

* 类 `Uncopable` 不包含任何数据，可以利用到 empty base class optimization。

### 3. `=delete`

C++11 引入。

##  Q7：为多态基类声明 *virtual* 析构函数

当 derived class 对象经由一个 base class 指针被删除，而该 base class 带着一个 non-virtual 析构函数，其结果未有定义 —— 实际执行时通常发生的是对象的 derived 成分没被销毁。

但我们不能无脑的将类的析构函数设置为 virtual，因为虚函数机制还会带来一笔较大的开销，特别是涉及零基类优化。

## Q8：别让异常逃离析构函数

C++ 语法上可以允许析构函数抛出异常，但你不应该这么做。这主要是因为 **C++ 不允许两个异常同时存在**：

> C++ 的异常处理模型基于"异常是罕见事件"的假设，设计目标是：
>
> - 正常执行路径的性能不受影响（zero-cost abstraction）
> - 异常处理应该简单、可预测
> - 避免复杂的异常组合逻辑
>
> 如果允许多个异常同时存在，就需要定义复杂的异常优先级、合并或选择规则，这与简单性原则相冲突。
>
> **栈展开的复杂性**：
>
> - 需要维护多个异常的状态
> - 需要复杂的算法来决定异常的传播顺序
> - 增加了运行时开销和内存消耗
>
> 大多数主流语言都采用类似的单异常模型：
>
> - **Java**: 同样不允许多个异常同时存在
> - **C#**: 采用单异常模型
> - **Python**: 异常链（exception chaining）但仍然是单一异常路径
>
> 当检测到潜在的双异常情况时，调用`std::terminate()`是合理的，因为：
>
> - 程序已经处于不可恢复的状态
> - 继续执行可能导致更严重的后果（如资源泄漏、数据损坏）
> - 快速失败（fail-fast）是更安全的选择
>
> 总的来说，C++ 的单异常模型是在简单性、性能和可靠性之间权衡的结果，虽然限制了某些复杂场景的表达能力，但换来了更可预测和高效的异常处理机制。

在 C++ 里，当有异常被抛出，**调用栈**(call stack)，即栈中用来储存函数调用信息的部分，会被按次序搜索，直到找到对应类型的处理程序(exception handler)。

以上这个寻找异常相应类型处理器的过程就叫做**栈展开**。同时在这一过程中，展开的栈里局部变量的资源会被清空，即调用了对象的析构函数。

现在可以回到正题了，C++ 并不阻止在类的析构函数中抛出异常，但这是一个非常不好的做法！因为栈展开的前提是已经有一个未处理的异常，并且栈展开会自动调用函数本地对象的析构函数，如果这时对象的析构函数时又抛出一个异常，现在就同时有两个异常出现，但 C++ 最多只能同时处理一个异常，因此程序这时会自动调用 `std::terminate()` 函数，导致我们所谓的闪退或者崩溃。

此外，如下的栗子也会导致程序同时出现多个异常:

```CPP
class Widget{
public:
    ...
    ~Widget(){...}        //假设此析构函数可能会抛出异常
};

void doSomething(){
  std::vector<Widget> v;
}                         //在这一行调用了v的析构函数，资源被释放
```

当 `v` 调用析构函数，它包含的所有 `Widget` 对象也都会被调用析构函数。又因为 `v` 是一个容器，如果在释放第一个元素时触发了异常，它也只能继续释放别的元素，否则会导致其它元素的资源泄露。如果在释放第二个元素的时候又触发了异常，那么程序同样会导致崩溃。

不仅仅是 `std::vector`，所有 STL 容器的类甚至包括数组也都会像这样因为析构函数抛出异常而崩溃程序，所以在 C++ 中，不要让析构函数抛出异常！

但是如果析构函数所使用的代码可能无法避免抛出异常呢？我们再来看一个栗子:

```cpp
class DBConnection{                   //某用来建立数据库连接的类
  public:
    ...
    static DBConnection create();     //建立一个连接
    void close();                     //关闭一个连接，假设可以抛出异常
};

class DBConn{                         //创建一个资源管理类来提供更好的用户接口
  public:
    ....
    ~DBConn{ db.close(); ]            //终止时自动调用关闭连接的方法
  private:
    DBConnection db;
};


...{                                 
  DBConn dbc(DBConnection::create()); //创建一个DBConn类的对象
  ...                                 //使用这个对象
}                                     //对象dbc被释放资源
                                      //但它的析构函数调用了可能会抛出异常的close()方法
```

我们通过 `DBConn` 的析构函数来释放资源并关闭连接，但析构函数所调用的 `close()` 方法可能会抛出异常，那么有什么方法来解决呢？

- **消化掉这个异常**

```cpp
DBConn::~DBConn(){
  try{ 
    db.close();
  }catch(...){
    //记录访问历史
  }
}
```

栈展开的过程终止于异常被对应类型的 `catch` 块接到，因此在这种情况下，只要 `catch` 包括了所有可能的异常，析构函数就能消化掉这个异常，防止异常从析构函数里跑出来，和别的异常产生冲突。

但这样做法的缺点是可能会给程序的稳定运行带来隐患，因为当某些比较严重或者不能处理的异常发生时，我们继续让程序运行，就可能导致程序的未知行为。当然如果能保证所有异常都能被正确处理，程序能继续稳定运行，就可以使用这个方法。

- **主动关闭程序**

```cpp
DBConn::~DBConn(){
  try{ 
    db.close();
  }catch(...){
    //记录访问历史
    std::abort();
  }
}
```

通过 `std::abort()` 函数来主动关闭程序，而不是任由程序在某个随机时刻突然崩溃，这样能减少潜在的用户风险。对于某些比较严重的异常，就可以使用这个方法。并且我们可以结合使用上面的方法，把能处理的异常消化掉。

但这些做法治标不治本，只能当做 plan B，我们再来看一个更好的方法：

- **把可能抛出异常的代码移出析构函数**

我们设计 `DBConn` 类的更安全的接口，让其他函数来承担这个风险，而且这样也可以事先在析构函数这样的紧要关头前对异常做出处理。

```cpp
class DBConn{
  public:
    ...
    ~DBConn();
    void close();        //当要关闭连接时，手动调用此函数
  private:
    ...
    closed = true;       //显示连接是否被手动关闭
};

void DBConn::close(){    //当需要关闭连接，手动调用此函数
  db.close();
  closed = true;
}

DBConn::~DBcon(){
  if(!closed)            //析构函数虽然还是要留有备用，但不用每次都承担风险了
    try{
      db.close();
    }catch(...){
      //记录访问历史
      //消化异常或者主动关闭
    }
}
```

通过以上的做法，当关闭连接时，我们先手动调用 `close()` 方法，这样就算抛出了异常，我们也可以事先处理，然后再调用析构函数。当然析构函数还是要检查是否被手动关闭并留有备用方案。如果没有被手动关闭，析构函数还是需要在消化掉异常和终止程序中做出选择。

客户端的行为可能如下所示：

``` cpp
// 客户端代码
DBconn conn;
// ...
// 客户手动处理异常
try {
    conn.close();
}
catch(...) {
    // exception_handler
}
```

在这种做法中，我们将可能抛出异常的操作拿出来作为一个接口开放给用户，实际上是给了用户一个“第一手”处理异常的机会，如果用户不 care，或者客户不觉得此时可能会抛出异常，可以忽略该函数，转而依赖析构函数释放资源，但是如果析构函数中抛出了异常，客户也没有理由抱怨，因为我们开放了这个接口，提供给用户处理异常的机会，是客户没有选择处理。

## Q9：绝不在构造和析构过程中调用 *virtual* 函数

在 derived class对象的 base class 构造期间，对象的类型是 base class 而不是 derived class。不只 virtual 函数会被编译器解析至 *(resolve to)* base class, 若使用运行期类型信息 (runtime type information, 例如 dynamic_cast，也会把对象视为 base class 类型。

相同道理也适用于析构函数。一旦 derived class 析构函数开始执行，对象内的 derived class 成员变量便呈现未定义值，所以 C++ 视它们仿佛不再存在。进入 base class 析构函数后对象就成为一个 base class 对象，而 C++ 的任何部分包括 virtual 函数、 dynamic_cast 等等也就那么看待它。

例如：

``` cpp
struct Base {
    Base() { init(); }
    ~Base() { cleanup(); }
    virtual void init() { cout << "Base::init()" << endl; }
    virtual void cleanup() { cout << "Base::cleanup()" << endl; }
};

struct Derived : public Base {
    Derived() {}
    ~Derived() {}
    virtual void init() { cout << "Derived::init()" << endl; }
    virtual void cleanup() { cout << "Derived::cleanup()" << endl; }
};

int main()
{   
    Derived d;
    // Base::init()
    // Base::cleanup()
    return 0;
}
```

在上面的代码中，我们是期望通过虚函数，在构造函数中执行该类型的 `init()`，在析构函数中执行该类型的 `cleanup()`，但实际上代码并没有按照我们期望的执行。

既然自顶（Base）向下（Derived）不行，我们可以将虚函数改为非虚函数，通过参数自下朝顶传递信息：

``` cpp
struct Base {
    Base(const string &info) { init(info); }
    virtual void init(const string &info) { cout << "Base init with: " << info << endl; } 
};

struct Derived : public Base {
    Derived() : Base("Derived info") {}
};

int main()
{   
    Derived d; // Base init with: Derived info
    return 0;
}
```

不过，如果 `info` 的构造十分复杂，将这部分代码写在构造函数当中会很臃肿，而我们又不能直接将这部分逻辑拆出来作为一个成员函数，因为在执行构造函数期间，对象还没有完全构造完成，换言之，此时可能有些成员变量还没有完成初始化：

``` cpp
struct Base {
    Base(const string &info) { init(info); }
    virtual void init(const string &info) { cout << "Base init with: " << info << endl; } 
};

struct Derived : public Base {
    Derived(int data = 123456789) : Base(createInfo()), data_(data) {}
    string createInfo() {
        string s1 = "...";
        string s2 = "---";
        string s3 = "***";
        string s4 = to_string(data_);
        return s1 + s2 + s3 + s4;
    }
    int data_;
};

int main()
{   
    Derived d; // Base init with: ...---***1227357832
    return 0;
}
```

此时，虽然代码可以成功运行，但打印出来的 `data_` 的值是未定义的，因为 `Bast(createInfo())` 在初始化 `data_` 之前执行。

一个可行的解决方案是通过一个 private static 成员函数来完成，并且直接将需要的成员变量作为参数传递给它：

``` cpp
struct Base {
    Base(const string &info) { init(info); }
    virtual void init(const string &info) { cout << "Base init with: " << info << endl; } 
};

struct Derived : public Base {
    int data_;
    Derived(int data = 123456789) : Base(createInfo(data)), data_(data) {}
private:
    static string createInfo(int data) {
        string s1 = "...";
        string s2 = "---";
        string s3 = "***";
        string s4 = to_string(data);
        return s1 + s2 + s3 + s4;
    }
};

int main()
{   
    Derived d; // Base init with: ...---***123456789
    return 0;
}
```

设计为 static 的好处就是，不用担心在该函数内使用了还未初始化的成员变量，因为 static 函数只能使用 static 成员。

## Q10：令 `operator=` 返回一个 *reference to* `*this`

（1）C++ 标准库中的所有容器和类型都遵循这个约定，返回 `T&`，因此下面的表达式是合法的：

``` cpp
int x = 1, y = 2, z = 10;
(x = y) = z; // x=10
```

（2）返回引用避免了不必要的拷贝构造，否则对于链式调用 `x = y = z`，当 `y=z` 执行之后，会返回一个临时对象，就需要一次额外的构造和析构开销。

## Q11：在 `operator=` 中处理自我赋值

### 1. if 判断

最简单的处理自我赋值的代码：

``` cpp
struct Widget {
    Widget& operator=(const Widget &rhs) {
        if(&rhs == this) return *this;
        delete p;
        p = new int(*rhs.p);
        return *this;
    }
private:
    int *p;
};
```

缺点1：上面代码虽然可以处理自我赋值，但是无法处理 **异常安全**，也即，如果 `p = new int(*rhs.p)` 发生异常，`p` 就会处于指向已被删除内存的状态。

缺点2：`if(this == &rhs)` 也是需要成本的，事实上，自我赋值的概率并不高，但这条语句会给函数带来一个额外的控制流分支，从而导致一些优化比如prefetching、caching 和 pipelining 等指令的效率降低。

### 2. copy-and-swap(CAS)

``` cpp
struct Widget {
    Widget& operator=(const Widget &rhs) {
        int *pOrig = p;
        p = new int(*rhs.p);
        delete pOrig;
        return *this;
    }
private:
    int *p;
};
```

在这份代码中，通过创建一个临时对象保存旧对象 `p`，既保证了异常安全，又处理了自我赋值的问题，并且不需要引入额外的控制分支（`if`）。

> 由于自我赋值的发生概率很小，因此上面无脑 `p = new int(*rhs.p);` ，也即每次都深拷贝并不会导致效率的明显降低。

上面代码也可以简化为下面的形式，也即所谓的 `copy-and-swap`：

``` cpp
struct Widget {
    Widget& operator=(const Widget &rhs) {
        Widget tmp(rhs);  // copy
        swap(*this, tmp); // swap
        return *this;
    }
private:
    int *p;
};
```

或者直接将传引用改为传值：

``` cpp
struct Widget {
    Widget& operator=(Widget rhs) /* copy */ {
        swap(*this, rhs); // swap
        return *this;
    }
private:
    int *p;
};
```

> `std::swap` 本身是**异常安全**的，但它所调用的移动构造函数和移动赋值运算符不一定是。
>
> `std::swap` 的标准库实现通常是基于**移动语义**，大致如下：
>
> ```cpp
> template<typename T>
> void swap(T& a, T& b) {
>    	T temp = std::move(a); // 1. 移动构造
>    	a = std::move(b);      // 2. 移动赋值
>    	b = std::move(temp);   // 3. 移动赋值
> }
> ```
>
> 要让 `std::swap` 具有异常安全性，我们必须考虑这个过程中的每一步：
>
> 1. **移动构造函数 (`T temp = std::move(a);`)**： 如果 `T` 的移动构造函数抛出异常，`a` 仍然保持在有效状态，`b` 也没有被修改，`temp` 对象可能没有被完全构造。此时，`swap` 函数会立即终止，并向上层抛出异常。这是一个**基本异常安全**的保证，即程序状态没有被破坏，但操作没有完成。
> 2. **移动赋值运算符 (`a = std::move(b);` 和 `b = std::move(temp);`)**： 如果 `T` 的移动赋值运算符在执行过程中抛出异常，情况会变得复杂。例如，在第二步 `a = std::move(b)` 抛出异常，`a` 的状态可能已经改变，从而导致资源泄露或数据损坏。
>
> ### C++11 之后：`noexcept` 和异常安全
>
> 为了解决这个问题，C++11 引入了 `noexcept` 关键字，它用来声明一个函数不会抛出异常。`std::swap` 在底层实现时会利用这个特性：
>
> - **如果** `T` 的移动构造函数和移动赋值运算符都声明为 `noexcept`（即它们保证不抛出异常），那么 `std::swap` 会采用上面三步移动的方式，并且是**强异常安全**的。
> - **如果** `T` 的移动操作**没有**声明为 `noexcept`（即它们可能会抛出异常），`std::swap` 的实现会回退到更复杂、但能够提供**强异常安全**保证的逻辑。这通常涉及到在堆上进行临时内存分配，或者先进行拷贝，然后再进行赋值，以确保在任何步骤失败时都能回滚到原始状态。
>
> ### 总结
>
> - **`std::swap` 本身**是**异常安全的**。它确保在操作失败时，要么不改变任何状态（强异常安全），要么只改变部分状态但不泄露资源（基本异常安全）。
> - **`std::swap` 的异常安全性取决于 `T` 的移动构造和移动赋值**。
> - **最佳实践**：为了让你的类与 `std::swap` 高效且安全地协同工作，请**为你的移动构造函数和移动赋值运算符添加 `noexcept` 声明**。这样做不仅能提升 `std::swap` 的性能，还能明确地向使用者保证操作的可靠性。

## Q12：复制对象时勿忘其每一个成分

拷贝函数应该确保复制 **“对象内的所有成员变量”**及 **“所有 base class 成分”**。事实上，编译器生成的构造函数就是这么做的，例如我们现在有一个基类 Base：

``` cpp
struct Base {
    Base() : i(0), d(0) {}
    Base(int _i, int _d) : i(_i), d(_d) {}
    Base(const Base &rhs) : i(rhs.i), d(rhs.d) {}
    Base& operator=(const Base &rhs) { i = rhs.i, d = rhs.d; return *this; }
    int i;
    double d;
};
```

有一个派生类 Derived 继承自基类 Base，那么编译器默认生成的构造函数如下所示：

``` cpp
struct Derived : Base {
    Derived(int _i, double _d, string _s) : Base(_i, _d), s(_s) {}
    
    // 编译器自动生成的 Derived 拷贝构造函数等效于：
    Derived(const Derived &rhs) : Base(rhs), s(rhs.s) {}

    // 编译器自动生成的 Derived 赋值运算符等效于：
    Derived& operator=(const Derived &rhs) {
        Base::operator=(rhs);  // 自动调用基类赋值运算符
        s = rhs.s;             // 拷贝派生类成员
        return *this;
    }

    void print() {
        cout << i << ' ' << d << ' ' << s << endl;
    }
    string s;
};

```

对于 Derived 类，编译器会为它生成默认的拷贝构造和拷贝赋值运算符，它们会正确处理继承关系，自动调用 base class 的相应函数并拷贝所有成员。

但是，如果我们自定义了拷贝函数，并且在我们的拷贝函数中没有完成对所有成员（包括基类成员）的拷贝，编译器也不会给出任何警告：

``` cpp
struct Derived : Base {
    Derived(int _i, double _d, string _s) : Base(_i, _d), s(_s) {}

    // 自定义拷贝函数
    Derived(const Derived &rhs) : s(rhs.s) {}
    Derived& operator=(const Derived &rhs) { 
        s = rhs.s; 
        return *this; 
    }

    void print() {
        cout << i << ' ' << d << ' ' << s << endl;
    }
    string s;
};
```

在上面的 class Derived 中，拷贝函数没有完成对基类的拷贝，这是一个容易忽视的点，编译器不会对此做出警告：

``` cpp
int main()
{   
    Derived d{16, 3.14, "hello"};
    d.print(); // 16 3 hello
    Derived d2 = d;
    d2.print(); // 0 0 hello：基类部分没有拷贝
    Derived d3{1024, 1.2345, "world"};
    d3.print(); // 1024 1 world
    d3 = d;
    d3.print(); // 1024 1 hello：基类部分没有拷贝

    return 0;
}
```

正确的写法应该是：

``` cpp
struct Derived : Base {
    Derived(int _i, double _d, string _s) : Base(_i, _d), s(_s) {}

    // 自定义拷贝函数
    Derived(const Derived &rhs) : 
    	Base(rhs), s(rhs.s) {} // 手动调用积累的拷贝构造函数
    Derived& operator=(const Derived &rhs) { 
        Base::operator=(rhs); // 手动调用基类的operator=
        s = rhs.s; 
        return *this; }

    void print() {
        cout << i << ' ' << d << ' ' << s << endl;
    }
    string s;
};

int main()
{   
    Derived d{16, 3.14, "hello"};
    d.print(); // 16 3 hello
    Derived d2 = d;
    d2.print(); // 16 3 hello
    Derived d3{1024, 1.2345, "world"};
    d3.print(); // 1024 1 world
    d3 = d;
    d3.print(); // 16 3 hello

    return 0;
}
```

# 三、资源管理

## Q13：以对象管理资源

以对象管理资源的思想实际上就是 RAII。

## Q14：在资源管理类中小心 *copying* 行为

虽然基于 RAII 思想设计的智能指针很好用，但是这并不能解决一切，因为并不是所有的资源都是 heap-based，例如 socket、file description 等，我们无法用智能指针管理这些资源，因此，我们有必要家里自己的资源管理类。

但是，编写自己的资源管理类时一定会面临一个问题：当一个 RAII 对象被复制，会发生什么？

1. 禁止复制
2. 引用计数（浅拷贝）
3. 复制底部资源（深拷贝）
4. 转移底部资源的所有权

*copying* 函数（`copy ctor` 和 `operator=`）可能被编译器自动创建，因此，除非编译器所生版本做了你想要做的事情，否则，你得自己编写他们。

## Q15：在资源管理类中提供对原始资源的访问

 APIs 往往要求访问原始资源，所以每一个 RAII class 应该提供一个“取得其所管理之资源”的办法。对原始资源的访问可能经由显式转换或隐式转换。一般而言显式转换比较安全， 但隐式转换对客户比较方便。

## Q16：成对使用 *new* 和 *delete* 时要采用相同形式

简而言之：

* `new` 对应 `delete`
* `new[]` 对应 `delete[]`。

## Q17：以独立语句将 *newed* 对象置于智能指针

之所以这么做的原因是因为，**在 C++ 中，运算符优先级只决定了表达式的结合方式，但不保证子表达式的求值顺序。**例如：

``` c++
a() + b() * c();
```

在上面的表达式中，根据加法和乘法的优先级，编译器肯定先计算乘法，然后在计算加法，也即 `a() + (b() * c())`，但是对于 `a()`、`b()`、`c()` 的求值顺序，并没有做出规定。

也因此，如果我们写出如下代码：

``` cpp
processWidget(std::shared_ptr<Widget>(new Widget), process());
```

对于传入的两个表达式：`std::shared_ptr<Widget>(new Widget)` 和 `process()` 的求值顺序是未知的，或者说是不确定的、不具有移植性的。并且对于 `std::shared_ptr<Widget>(new Widget)`，它其实包含了两步：

1. `auto tmp = new Widget`
2. `std::shared_ptr<Widget>(tmp)`

也即内存分配和对象的构造是分开的，因此上面代码如果按照下面的方式执行：

1. `auto tmp = new widget`
2. `process()`
3. `std::shared_ptr<Widget>(tmp)`

如果第二步 `process()` 发生了异常，那么第一步 `new Widget` 分配的内存就泄露了，此时我们虽然使用了智能指针，却起不到任何作用。

因此，为了避免求值顺序导致的不确定性，应该以独立语句将 newed 对象置于智能指针。

``` cpp
std::shared_ptr<Widget> pw(new Widget);
processWidget(pw, process());
```

这样，即使 `process()` 发生了异常，由于 `new Widget` 已经被智能指针保管，资源也不会泄露。

不过，C++14 引入了 `std::make_shared`，可以更优雅的解决该问题：

``` cpp
processWidget(std::make_shared<Widget>(), process());
```

* `make_shared`是异常安全的，因为它在单个函数调用中完成内存分配和对象构造。

# 四、设计与声明

## Q18：让接口容易被正确使用，不易被误用

理想上，如果客户企图使用某个接口而却没有获得他所预期的行为，这个代码不该通过编译；如果代码通过了编译，它的作为就该是客户所想要的。

欲开发一个“容易被正确使用，不容易被误用”的接口，首先必须考虑客户可能做出什么样的错误。或者说，如果用户错误的使用了接口，但是代码通过了编译，你也该负一部分责任。

### 1. 许多客户端错误可以通过导入新类型而获得预防

通过引入自定义类型，我们不仅可以限制用户的取值，还可以限制用户的操作。例如：

``` cpp
class Date {
public:
    Date(int month, int day, int year);
// ...
};
```

对面上面的 Date 构造函数接口，客户端可能犯下面的错误：

``` cpp
Date d(30, 3, 2010);
```

也即，客户可能将月份和天数的次序传反了，这是很有可能的，但问题是，这份代码将会通过编译。

下面是一种可行的预防措施:

``` cpp
class Month {
public:
    static Month Jan() { return Month(1); }
    static Month Feb() { return Month(2); }
    // ...
    static Month Dec() { return Month(12); }
// private:
    explicit Month(int m) : val(m) {}; // 阻止生成新的月份
    int val;
};

struct Day {
    explicit Day(int d) : val(d) {}
    int val;
};

struct Year {
    explicit Year(int y) : val(y) {}
    int val;
};

class Date {
public:
    Date(Month _month, Day _day, Year _year)
        : month(_month), day(_day), year(_year) {}
// ...
// private:
    Month month;
    Day day;
    Year year;
};

int main()
{
    Date(Month::Jan(), Day(30), Year(2010));
    return 0;
}
```

注意在 Month 中，我们是直接提供了 12 个 static 函数来表现某个特定月份。这样做的好处就是避免客户产生不合法的月份。你可能会问，为什么不使用 `enum` 来制定月份呢？这主要是考虑到，enum 可以隐式转换为整型，可能导致奇怪的错误。

另外，我们也可以在 class 内添加一些功能性函数，达到限制客户“什么事可做，什么事不可做”的效果。

最后，除非你有完全更好 的理由，否则，令你的 type(class) 的行为与内置类型保持一致。例如，对于 int，它不允许 `a * b = c` 这种操作，因此，如果你希望重载 `operator*`，请保持这种特性。

### 2. 不要期望客户永远记得做某些事

任何接口如果要求客户必须记得做某些事情，就是有着“不正确使用”的倾向，因为客户可能会忘记做这些事。

例如，在接口中返回一个指向动态分配内存的指针，接口期望客户记得 delete 掉该指针，或者用智能指针来管理它，这显然是不合适的，因为客户是肯定会忘记释放该资源的（Muphy 定律）。因此，一个好的接口应该直接返回只能智能指针而不是裸指针。

> 在 C 语言中，一种解决方案是，该接口永远使用同一块静态内存，客户不需要释放该内存，但是当客户重新调用该函数时，新的调用结果会覆盖当前的内存，这回导致之前调用所返回的指针所指向的数据失效（被修改为最新一次调用的数据了）。
>
> 但这种方案并不是很好，除了 **多线程环境下对共享变量的竞争** 以外，它在使用上也有诸多限制，在条款 21 我们将具体说明。

### 3. 保持接口的一致性

STL 在这点上就做的很好，所有的容器都通过 `empty`、`size`、`push`、`pop` 完成相同的功能。

## Q19：设计 class 犹如设计 type

（1）新 type 的对象应该如何被创建和销毁？这会影响到你的 class 的构造函数和析构函数以及内存分配函数和释放函数的设计，当然前提是如果你打算撰写它们。

（2）对象的初始化和对象的赋值该有什么样的差别？这个答案决定你的构造函数和赋值操作符的行为，以及其间的差异。很重要的是别混淆了“初始化” 和“赋值”，因为它们对应不同的函数调用。

（3）新 type 的对象如果被 pass by value，意味着什么？记住， copy 构造函数用来定义一个 type 的如 pass by value 该如何实现。

（4）什么是新 type 的合法值？对 class 的成员变量而言，通常只有某些数值集是有效的。那些数值集决定了你的 class 必须维护的约束条件 (invariants) ，也就决定了你的成员函数（特别是构造函数、赋值操作符和所谓 “setter" 函数）必须进行的错误检查工作。它也影响函数抛出的异常、以及（极少被使用的）函数异常明细列 (exception specifications) 。

（5）你的的新 type 需要配合某个继承图系吗？如果你继承自某些既有的 classes，你就受到那些 classes 的设计的束缚，特别是受到“它们的函数是 virtual  或 non-virtual" 的影响（见条款 34 和条款 36) 。如果你允许其他 classes 继承你的 class，那会影响你所声明的函数——尤其是析构函数是否为 virtual（见条款 7)

（6）你的新 type 需要什么样的转换？你的 type 生存于其他 types 之间，因而彼此该有转换行为吗？如果你希望允许类型 T1 之物被隐式转换为类型 T2 之物，就必须在 class Tl 内写一个类型转换函数 (operator T2) 或在 class T2 内写一个 non-explicit-one-argument 的构造函数。如果你只允许 explicit 构造函数存在，就得写出专门负责执行转换的函数，且不得为类型转换操作符(type conversion operators) 或 non-explicit-one-argument 构造函数。 （条款 l5 有隐式和显式转换函数的范例。）

（7）什么样的操作符和函数对此新 type 而言是合理的？这个问题的答案决定你将为你 的 class 声明哪些函数。其中某些该是 member 函数，某些则否（见条款23,24,46) 。

（8）什么样的标准函数应该驳回？那些正是你必须声明为 private 者（见条款6) 。

（9）谁该取用新 type 的成员？这个提问可以帮助你决定哪个成员为 public, 哪个为 protected，哪个为 private。它也帮助你决定哪一个 classes 和/或 functions 应该是 friends，以及将它们嵌套于另一个之内是否合理。

（10）什么是新 type 的“未声明接口” (undeclared interface) ？它对效率、异常安全性 （见条款29) 以及资源运用（例如多任务锁定和动态内存）提供何种保证？你在这些方面提供的保证将为你的 class 实现代码加上相应的约束条件。

（11）你的新 type 有多么一般化？或许你其实并非定义一个新 type，而是定义一整个 types 家族。果真如此你就不该定义一个新 class, 而是应该定义一个新的 class template。

（12）你真的需要一个新 type 吗？如果只是定义新的 derived class 以便为既有的 class 添加机能，那么说不定单纯定义一或多个 non-member 函数或 templates，更能够达到 目标。

## Q20：以 *pass-by-reference-to-const* 代替 *pass-by-value*

尽量以 pass-by-reference-to-const 替换 pass-by-value。前者通常比较高效，并可避免 **切割问题**。

以上规则并不适用 **内置类型**，以及 **STL 的迭代器和函数对象**。对它们而言， pass-by-value 往往比较适当，并且，一般情况下，也只有上面提到对象适合用 pass-by-value。

对于小型的用户自定义类型，依然是推荐使用 *pass-by-reference-to-const*，因为作为一个用户自定义类型，其大小容易有所变化。此外，某些编译器对待“内置类型”和“用户自定义类型”的态度截然不同，纵使两者拥有相同的底层表述。举个例子，某些编译器拒绝把只由一个 double 组成的对象放进缓存器内，却很乐意在一个正规基础上对光秃秃的 doubles 那么做。 当这种事发生，你更应该以 *by reference* 方式传递此等对象，因为编译器当然会将指针 (references 的实现体）放进缓存器内，绝无问题。

## Q21：必须返回对象时，别妄想返回其 *reference*

对于 stack 对象，错误是显然的。但是对于 heap 对象呢？看下面的例子：

``` cpp
const Int& operator*(const Int &a, const Int &b) {
    Int *result = new Int(a * b);
    return *result;
}
```

首先，条款 18 中提到过，返回一个指向动态分配内存的指针肯定不是一个好的接口设计。除此以外，如果我们以下面的方式调用：

``` cpp
Int a, b, c;
Int d = a * b * c;
```

应当发现，`a * b` 的结果丢失了，也即有内存泄漏了。

这时候，你可能会想起 C 语言的一种做法，就是让 reference 指向一块 **静态内存**：

``` cpp
const Int& operator*(const Int &a, const Int &b) {
    static Int result;
    result = a * b;
    return result;
}
```

但是这么做有一个显而易见的问题，那就是 `result` 作为共享资源，在多线程环境下必然面临着竞争问题，也即，此时 `operator*` 在多线程环境下不是线程安全的。

此外，如果以下面的方式调用 `if` 将永远返回 true。

``` cpp
Int a, b , c, d;

if((a * b) == (c * d)) {
    // ...
}
else {
    // ...
}
```

## Q22：将成员变量声明为 *private*

从 **封装** 的角度看 C++ 的三种访问权限，可以视为两类：

1. 提供封装：private
2. 不提供封装：public，protected

> protected 和 public 本质上都是作为接口提供给外界使用的，只不过前者是给新类的设计者，后者是给当前类的直接使用者，二者两者都是作为用户在使用当前类。

public 和 protected 权限意味着，该数据实际上是作为一个接口提供的，客户可以随意使用该接口。但是，如果我们后续修改了该接口，例如改了个名字，则会影响大量客户代码，包括客户端代码以及 Derived class 的代码（程序员难道就不算客户了吗😊），这显然算不上一种好的设计。

## Q23：以 *non-member*、*non-friend* 替换 member 函数

> 讨论 friend 函数和讨论 member 函数是一样的，因为 friend 和 member 都拥有对 private 数据的访问权限。

### 1. 封装性

从 **封装** 的角度出发，优先考虑 non-member non-friend function 而不是 member function 或 friend function，因为能够访问 class 内 private 成分的函数越少，数据的封装性就越好。当然，这是一种比较粗糙的量化手段。

### 2. 机能扩充性

第二件值得注意的事情是，只因在意封装性而让函数“成为 class 的 non-member”，并不意味它 “不可以是另一个 class 的 member" 。例如我们可以将当前 class 的一部分工具函数单独拿出来放到工具类当中作为一个 static 函数，或者在 C++ 中，一种更为自然的方法时让这个工具函数与该 class 处于同一个 namespace：

``` cpp
namespace WebBrowserStuff {
	class WebBrowser { /*...*/ }
    void clearBroswer(WebBroswer &wb); // 工具函数
};
```

有时候，考虑到不同工具函数的功能差异较大，而用户可能并不需要用到所有工具函数，我们可以分离这些工具函数：

``` cpp
// webbroswer.h
namespace WebBrowserStuff {
	class WebBrowser { /*...*/ }
    // 核心函数，例如几乎所有客户都需要的
};

// webbroswerbookarks.h
namespace WebBrowserStuff {
    // 与 bookmark 相关的工具函数
};

// webbroswercookies.h
namespace WebBrowserStuff {
    // 与 cookie 相关的工具函数
};
```

将 member 函数拿出来放到同名 namespace 中作为 non-member non-friend 函数，并且根据功能分离为多个头文件的另一个好处就是，我们可以轻松的拓展工具函数，就像我们对标准库中的 `struct hash` 所做的一样：

``` cpp
// user.h
# include<webbroswerbookarks.h>
namespace WebBrowserStuff {
    return_type user_self_define_book_mark_function(args);
    // ...
}
```

## Q24：若所有参数皆需类型转换，请为此采用 *non-member* 函数

为了支持混合式算术运算，以 `operator*`（乘法） 为例，我们一般需要做到以下两点：

1. class 支持隐式类型转换
   * 令 class 支持隐式类型转换通常是个糟糕的注意，当然这条规则也有其例外，最常见的例外就是在 **建立数值类型** 时。
2. `operator*` 是一个 non-member 函数

也即，通过上面两点，我们可以保证，`operator*` 的 **所有参数** 都可以进行 **隐式类型转换**。

例如：

``` cpp
struct INT {
    explicit INT(int x = 0) : val(x) {}
    const INT operator*(const INT &rhs) const {
        return INT{rhs.val * val};
    }
    int val;
};


int main()
{
    INT x = 10;
    INT y = x * 2;
    cout << y.val << endl; // error: no match for 'operator*' (operand types are 'INT' and 'int')
    INT z = 2 * x;
    cout << z.val << endl; //  error: no match for 'operator*' (operand types are 'int' and 'INT')

    return 0;
}
```

为了让第一个乘法合法，我们需要让 `INT` 支持从 `int` 到 `INT` 的隐式类型转换。为了让第二个乘法合法，我们需要让 `operator*` 是一个 non-member 函数。

也即：

```cpp
struct INT {
    INT(int x = 0) : val(x) {}
    int val;
};

const INT operator*(const INT &lhs, const INT &rhs) {
    return INT{rhs.val * lhs.val};
}
```

那么，当 `val` 是一个 private 成员时，是否需要将 `operator*` 设置为一个 friend 函数呢？此时我们需要考虑 `INT` 是否提供了访问 `val` 的 public 接口，例如 `getval`，如果有，使用 public 接口；如果没有，设置为 friend 函数从而可以访问 private 成员。

## Q25：考虑写出一个不抛异常的 *swap*

### 1. Copy-and-Swap Idiom

在前面我们已经提到过 swap，在处理自我赋值的同时可以保证异常安全性：

``` cpp
class Foo {
public:
    Foo(int x) : p(new int(x)) {}
    ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

    Foo(const Foo &x) : p(new int(*(x.p))) {}

    void swap(Foo &rhs) {
        std::swap(p, rhs.p);
    }

    // copy-and-swap idiom
    Foo& operator=(Foo x) { // copy
        x.swap(*this); // swap
        return *this;
    }

private:
    int *p;
};
```

### 2. swap 接口的设计

上面的例子是将 swap 作为一个工具函数用来实现 `operator=` 的，如果我们想要将 `swap` 开放为一个 public 接口，STL 是通过 <font color=blue>public `swap` 成员函数</font>（实现具体的 swap 操作）和 <font color=blue>`std::swap` 特化版本</font>（用以调用前者，因为我们在使用 swap 时一般是以 non-member 的形式调用）。

``` cpp
class Foo {
public:
    Foo(int x) : p(new int(x)) {}
    ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

    Foo(const Foo &x) : p(new int(*(x.p))) {}

    void swap(Foo &rhs) {
        cout << "Foo swap: " << *p << ' ' << *rhs.p << endl;
        swap(p, rhs.p);
    }

    // copy-and-swap idiom
    Foo& operator=(Foo x) {
        cout << "call operator=" << endl;
        x.swap(*this);
        return *this;
    }

private:
    int *p;
};

namespace std {
    template<>
    void swap<Foo>(Foo &lhs, Foo &rhs) {
        // call member function
        cout << "global swap" << endl;
        lhs.swap(rhs);
    }
}

int main()
{
    Foo x(1), y(2);
    swap(x, y); // 我们一般不以 x.swap(y) 的形式调用 swap
    return 0;
}

// global swap
// Foo swap: 1 2
// ~Foo(): 1
// ~Foo(): 2
```

如果此时，`Foo` 是一个 class template 而非 class，下面的做法可行吗？

``` cpp
template<typename T>
class Foo {
public:
    Foo(T x) : p(new T(x)) {}
    ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

    Foo(const Foo &x) : p(new T(*(x.p))) {}

    void swap(Foo &rhs) {
        cout << "Foo swap: " << *p << ' ' << *rhs.p << endl;
        swap(p, rhs.p);
    }

    // copy-and-swap idiom
    Foo& operator=(Foo x) {
        cout << "call operator=" << endl;
        x.swap(*this);
        return *this;
    }

private:
    T *p;
};

namespace std {
    template<typename T>
    void swap<Foo<T>>(Foo<T> &lhs, Foo<T> &rhs) {
        // call member function
        cout << "global swap" << endl;
        lhs.swap(rhs);
    }
}
```

代码修改如上所示，事实上会编译错误，因为我们尝试对 `std::swap(...)` 偏特化。但 **C++ 只允许对 class template 偏特化，对 function template 的偏特化是行不通的**，因为对于 function template 的偏特化，可以通过重载实现。

好了，我们似乎也知道如何解决该问题了，直接重载一个 `std::swap` 即可：

``` cpp
namespace std {
    template<typename T>
    void swap(Foo<T> &lhs, Foo<T> &rhs) {
        // call member function
        cout << "global swap" << endl;
        lhs.swap(rhs);
    }
}
```

不过，如果你对 std 命名空间有着十足的谨慎，你可能会想到：C++ 允许用户 **特化** std 内的 template，但不可以 **添加** 新的 template（或 class 或 function 或其它任何东西）到 std 命名空间。

* 这其实是设计模式中 ***open-close*** 原则的一种体现和应用

因此说，在 std 里面添加重载版本是不可行的， 即使有些编译器通过了这种做法，其行为也是未定义的。

一种可行的做法如下所示：

``` cpp
namespace FooStuff {
template<typename T>
class Foo {
public:
    Foo(T x) : p(new T(x)) {}
    ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

    Foo(const Foo &x) : p(new T(*(x.p))) {}

    void swap(Foo &rhs) {
        cout << "Foo swap: " << *p << ' ' << *rhs.p << endl;
        swap(p, rhs.p);
    }

    // copy-and-swap idiom
    Foo& operator=(Foo x) {
        cout << "call operator=" << endl;
        x.swap(*this);
        return *this;
    }

private:
    T *p;
};

template<typename T>
void swap(Foo<T> &lhs, Foo<T> &rhs) {
    // call member function
    cout << "global swap" << endl;
    lhs.swap(rhs);
}
} // namespace FooStuff

int main()
{
    using namespace FooStuff;
    Foo<int> x(1), y(2);
    swap(x, y);
    return 0;
}
```

也即，将 `Foo<T>` 和 `swap` 放在同一个 namespace 下，这样当调用 swap 时，通过 ***<font color=blue>argument-dependent lookup(Kenig lookup)</font>.***，会找到在该 namespace 下特化的版本。

* 事实上，将 `Foo<T>` 和 `swap` 放在同一个 namespace 下并不是强制的要求，只不过这样做可以带来避免命名空间污染等好处😎。

> 在 C++ 中，当调用一个函数时，编译器会按以下顺序查找该函数的声明：
>
> 1. **当前作用域：** 编译器首先在函数调用的直接作用域中查找函数。
> 2. **基类作用域：** 如果函数是成员函数，则会在基类中查找。
> 3. **命名空间作用域：** 编译器会在函数定义或调用的命名空间中查找。
>
> **ADL** 在这个基础上增加了一个规则：如果一个函数调用没有显式的命名空间限定（比如 `std::cout` 中的 `std::`），并且至少有一个参数是类类型、枚举类型或者它们的引用，那么编译器除了在常规查找路径中查找外，还会 **<font color=blue>查找这些参数所属的命名空间</font>**。

### 3. 总结

（1）对于 non-template class，我们可以在 namespace std 中为其添加特化版本，这样做最大的一个好处就是，即使用户错误的写出了如下代码：`std::swap(...)` 也可以成功调用我们自定义的 swap 函数。不过，从 C++11 开始，更推荐的做法是提供一个在自定义类型所在命名空间中的非成员 `swap` 函数（如上面 `FooStuff::swap`）。特化 `std::swap` 通常被认为是**不好的实践**，因为它违反了“不要向 `std` 命名空间添加任何东西”的通用规则（除了标准明确允许的特化）。尽管如此，它仍然是一种可行的方式。

（2）**member swap 绝不可抛出异常**。那是因为 swap 的一个最好的应用是帮助 class(和 class templates) 提供强烈的异常安全性保障。这一约束只施行于成员版！不可施行于非成员版，因为 **swap 缺省版本是以 copy 构造函数和 copy assignment 操作符为基础**，而一般情况下两者都允许抛出异常。因此当你写下一个自定版本的 swap，往往提供的不只是高效置换对象值的办法，而且不抛出异常。一般而言这两个 swap 特性是连在一起的，因为高效率的 swap 几乎总是基于对内置类型的操作（例如 pimpl 手法的底层指针），而**内置类型上的操作绝不会抛出异常**。

如果我们想实现自定义的效率更高的 swap，特别是在 pimpl 场景下。在现代 C++ 中，高效且异常安全的 `swap` 函数的最佳实践是：

1. **为你的类提供一个公有的、`noexcept` 的成员 `swap` 函数。** 这个函数应该尽可能地快，并且只交换类的内部资源（通常是指针）。
2. **在你的类所在的命名空间中，提供一个非成员的 `swap` 函数。**

在具体的使用时，推荐是先引入 `std::swap`，这样即使没有自定义 swap，也可以调用 `std::swap`：

``` cpp
template <typename T>
void generic_function(T& a, T& b) {
    using std::swap; // 引入 std::swap
    swap(a, b);      // 依赖 ADL 找到自定义的 swap，否则回退到 std::swap
}
```

#### 3.1 non-template class

``` cpp
namespace FooStuff {
    class Foo {
    public:
        Foo(int x) : p(new int(x)) {}
        ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

        Foo(const Foo &x) : p(new int(*(x.p))) {}

        // 不会抛出异常
        void swap(Foo &rhs) noexcept {
            cout << "call member swap" << endl;
            using std::swap;
            swap(p, rhs.p);
        }

        // copy-and-swap idiom
        Foo& operator=(Foo x) {
            cout << "call operator=" << endl;
            x.swap(*this);
            return *this;
        }

    private:
        int *p;
    };

    // non-member swap
    void swap(Foo &lhs, Foo &rhs) {
        cout << "Foo stuff swap" << endl;
        lhs.swap(rhs);
    }
}

int main()
{
    using namespace FooStuff;
    Foo x(1), y(2);
    swap(x, y);
    return 0;
}

// Foo stuff swap
// call member swap
// ~Foo(): 1
// ~Foo(): 2
```

#### 3.2 template class

``` cpp
namespace FooStuff {
    template<typename T>
    class Foo {
    public:
        Foo(T x) : p(new T(x)) {}
        ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

        Foo(const Foo &x) : p(new T(*(x.p))) {}

        // 不会抛出异常
        friend void swap(Foo &lhs, Foo &rhs) noexcept {
            cout << "call member swap" << endl;
            using std::swap;
            swap(lhs.p, rhs.p);
        }

        // copy-and-swap idiom
        Foo& operator=(Foo x) {
            cout << "call operator=" << endl;
            using std::swap;
            swap(*this, x);
            return *this;
        }

    private:
        T *p;
    };
}

int main()
{
    using namespace FooStuff;
    Foo x(1), y(2);
    swap(x, y);
    return 0;
}

// Foo stuff swap
// call member swap
// ~Foo(): 1
// ~Foo(): 2
```

#### 3.3 friend ADL

在之前的 ADL 中，我们是通过 non-member function 调用 member function，需要写两个函数，我们可以使用 friend 避免额外创建一个 member function：

``` cpp
namespace FooStuff {
    class Foo {
    public:
        Foo(int x) : p(new int(x)) {}
        ~ Foo() { cout << "~Foo(): " << *p << endl; delete p; }

        Foo(const Foo &x) : p(new int(*(x.p))) {}

        // 不会抛出异常
        friend void swap(Foo &lhs, Foo &rhs) noexcept {
            cout << "call member swap" << endl;
            using std::swap;
            swap(lhs.p, rhs.p);
        }

        // copy-and-swap idiom
        Foo& operator=(Foo x) {
            cout << "call operator=" << endl;
            using std::swap;
            swap(*this, x);
            return *this;
        }

    private:
        int *p;
    };
}

int main()
{
    using namespace FooStuff;
    Foo x(1), y(2);
    swap(x, y);
    return 0;
}

// Foo stuff swap
// call member swap
// ~Foo(): 1
// ~Foo(): 2
```

> 2025/9/10
>
> ``` cpp
> namespace FooStuff {
> struct Foo {
>     Foo(int x) : p(new int(x)) {}
>     Foo(const Foo &f) : p(new int(*(f.p))) {}
>     ~Foo() { delete p; }
>     Foo& operator=(Foo f) { // copy
>         using std::swap;
>         swap(p, f.p);
>         return *this;
>     }
>     void swap(Foo &rhs) noexcept {
>         using std::swap;
>         swap(p, rhs.p);
>     }
>     int *p;
> };
> 
> void swap(Foo &lhs, Foo &rhs) {
>     lhs.swap(rhs);
> }
> 
> } // namespace FooStuff
> 
> 
> int main()
> {
>     using namespace FooStuff;
>     Foo a(1), b(2);
>     cout << a.p << ' ' << b.p << endl;
>     swap(a, b);
>     cout << a.p << ' ' << b.p << endl;
>     return 0;    
> }
> ```

# 五、实现

## Q26：尽可能延后变量定义式的出现时间

你不只应该延后变量的定义，直到非得使用该变量的前一刻为止，甚至应该尝试延后这份定义直到能够给它初值实参为止。如果这样，不仅能够避免构造（和析构）非必要对象，还可以避免无意义的 default 构造行为。

## Q27：尽量少做转型动作

*old-style casts*:

* *c-style cast*: `(T)expression`
* *function-style cast*: `T(expression)`

*new-style/C++-style casts*:

* `static_cast<T>(expression)`: forced implicit conversion
* `const_cast<T>(expression)`: remove const
* `dynamic_cast<T>(expression)`: safe downcasting
* `interrupt_cast<T>(expression)`: depend complier, non-portable

许多程序员相信，转型其实什么都没做，只是告诉编译器把某种类型视为另一 种类型。这是错误的观念。任何一个类型转换（不论是通过转型操作而进行的显式 转换，或通过编译器完成的隐式转换）往往真的令编译器编译出运行期间执行的码。例如 `int` 转型为 `double`，由于这两个类型的底层表述不同，在这两个类型之间转型肯定会产生一些代码。

`dynamic_cast` 可能非常慢，在使用其进行转型时要谨慎，考虑能否通过在基类添加虚函数取代。

## Q28：避免返回 *handles* 指向对象内部成分

> 这里的 ***handles*** 指的是获取底层对象的手段，例如 *pointer*、iterator、*reference*。

所谓 **对象的内部成分**，指的是不被公开（访问权限为 private）的成员，既包括成员变量，也包括成员函数。

因此，一个显而易见的方面就是确保内部成分的封装性，如果我们返回了内部成分的引用，就是将内部成分暴露在外界，从而破坏了其封装性。

其次就是，如果我们返回了对象某个内部成员的引用 handler，我们就必须面临一个问题：handler 比所指对象更长寿，一个简单的例子就是，返回一个临时对象内部成分的引用，此时，我们恐怕很难满足该临时对象内部成分的生命周期比该临时对象更长。

## Q29：为异常安全而努力是值得的

### 1. 异常安全级别

异常安全函数（Exception-safe functions）通常提供以下三个保证之一：

1. **基本异常安全（Basic Exception Safety）**

如果操作失败（即抛出异常），*程序状态保持有效，但具体状态可能无法预测*。例如，资源不会泄漏，但数据可能会被修改，并且可能处于不确定的状态。你仍然可以使用该对象，只是它的内容可能不是你期望的。

2. **强异常安全（Strong Exception Safety / Commit or Rollback Semantics）**

如果操作成功，所有更改都将提交。如果操作失败（即抛出异常），程序状态将回滚到操作开始之前的状态，就像操作从未发生过一样。这是最理想的保证，因为它简化了错误处理，但实现起来通常也最复杂。

3. **无抛出保证（No-Throw Guarantee / Nothrow Exception Safety）**

函数保证不会抛出任何异常。如果函数内部发生了潜在的错误，它会通过其他方式处理（例如，返回错误码，或者终止程序），而不是抛出异常。这通常用于底层、关键的函数，例如交换（`swap`）操作或内存释放。在 C++ 中，可以通过 `noexcept` 关键字明确指定这种保证。

### 2. 异常安全不是不抛出异常

异常安全指的是，当异常被抛出时，带有异常安全性的代码回：

1. 不泄露任何资源：这意味着无论异常是否发生，所有已分配的资源（如内存、文件句柄、网络连接、锁等）都会被正确释放。这通常通过使用 RAII（Resource Acquisition Is Initialization）技术或 `try-finally` 块来实现。
2. 不允许数据败坏：这表示在异常发生后，程序的数据（包括对象状态和程序变量）必须保持在有效或可恢复的状态。数据要么保持在异常发生前的原始状态（**强异常安全**或**事务性异常安全**），要么处于一个确定的、但可能不完整但仍有效的新状态（**基本异常安全**）。

### 3. `noexcept`

注意对于 C++ 的 `noexcept` 函数修饰符，不是说函数不会抛出异常，而是说当函数抛出异常时，程序会直接 `terminated`。

其主要作用是允许编译器对该函数做一些针对无异常情况的优化处理，例如不需要生成用于处理栈展开的代码等。

### 4. 实例说明

初版代码：

``` cpp
class PrettyMenu {
public:
    // ...
    void chaneBackground(std::istream &imgSrc);
    // ...
private:
    Mutex mutex;
    Image* bgImage;
    int imgChangeCount;
};

void PrettyMenu::chaneBackground(std::istream &imgSrc) {
    lock(mutex);
    delete bgImage;
    bgImage = new Image(imgSrc);
    ++ imgChangeCount;
    unlock(mutex);
}
```

对函数 `chaneBackground` 的实现显然不是异常安全的，因为 `bgImage = new Image(imgSrc);` 可能抛出异常，此时，`unlock(mutex);` 得不到执行，`delete bgImage;` 也无法恢复，也就是说：

1. 锁资源泄露
2. 数据 `bgImage` 败坏

而上述两个问题我们都可以通过 RAII 的思想来解决：

``` cpp
class PrettyMenu {
public:
    // ...
    void chaneBackground(std::istream &imgSrc);
    // ...
private:
    Mutex mutex;
    std::shared_ptr<Image> bgImage;
    int imgChangeCount;
};

void PrettyMenu::chaneBackground(std::istream &imgSrc) {
    Lock lock(&mutex);
    bgImage.reset(new Image(imgSrc));
    ++ imgChangeCount;
}
```

通过 `class Lock` 管理锁防止资源泄露，通过 `shared_ptr::reset` 实现异常安全的 `delete-new`。

当 `PrettyMenu` 数据成员较多时，还有一种可行的办法是结合 ***Pimpl Idiom*** 和 `Swap-and-Copy`：

``` cpp
struct PMImpl {
    std::shared_ptr<Image> bgImage;
    int imgChangeCount;
};

class PrettyMenu {
public:
    // ...
    void chaneBackground(std::istream &imgSrc);
    // ...
private:
    Mutex mutex;
    std::shared_ptr<PMImpl> pimpl;
};

void PrettyMenu::chaneBackground(std::istream &imgSrc) {
    using std::swap;
    Lock lock(&mutex);
    std::shared_ptr<PMImpl> new_pimpl(new PMImpl(*pimpl));
    new_pimpl->bgImage.reset(new Image(imgSrc));
    ++ new_pimpl->imgChangeCount;
    swap(pimpl, new_pimpl);
}
```

> 比较原始方法和 Pimpl 方法，并没有绝对的“更好”，而是要根据具体的场景和需求来选择。它们各自有优缺点。
>
> ### 原始方法（直接成员）
>
> ```cpp
> class PrettyMenu {
> public:
>     // ...
>     void chaneBackground(std::istream &imgSrc);
>     // ...
> private:
>     Mutex mutex;
>     std::shared_ptr<Image> bgImage;
>     int imgChangeCount;
> };
> ```
>
> **优点：**
>
> - **简单直接：** 代码结构更简单，更容易理解和维护，因为所有数据都直接作为类的成员。
> - **性能略高：** 访问成员变量时没有额外的间接层（指针解引用），理论上性能开销略小。
>
> **缺点：**
>
> - **暴露实现细节：** `bgImage` 和 `imgChangeCount` 等私有成员的类型和布局暴露在类的头文件中。这意味着如果你修改了这些私有成员的类型、添加或删除了私有成员，即使对外接口没有改变，任何包含了这个头文件的客户端代码都可能需要重新编译。这被称为**编译防火墙（Compilation Firewall）**问题。
> - **难以实现强异常安全：** 直接修改成员变量很难实现强异常安全，因为一旦操作失败，对象可能会处于不一致状态。需要额外的复杂逻辑来回滚或保证原子性。
> - **代码耦合度高：** 类的实现与客户端代码紧密耦合，对内部实现的修改会影响外部编译。
>
> ### Pimpl 方法
>
> Pimpl 方法将类的实现细节封装到另一个结构体（`PMimpl`）中，主类只持有一个指向该结构体的智能指针。
>
> ```cpp
> struct PMimpl {
>     std::shared_ptr<Image> bgimage;
>     int imageChanges;
> };
> 
> class PrettyMenu {
> private:
>     Mutex mutex;
>     std::shared_ptr<PMimpl> pimpl;
> };
> ```
>
> **优点：**
>
> - **实现编译防火墙：** Pimpl 最大的优势在于它将实现细节从头文件中隐藏起来。`PrettyMenu` 的头文件只需要声明 `pimpl` 是一个智能指针即可，不需要包含 `PMimpl` 的定义。只有在 `.cpp` 源文件中才需要包含 `PMimpl` 的完整定义。这意味着你可以自由修改 `PMimpl` 内部的数据结构，而不需要重新编译所有使用 `PrettyMenu` 的客户端代码，大大减少了编译时间。
> - **降低耦合度：** 客户端代码与 `PrettyMenu` 的内部实现细节解耦。
> - **更容易实现强异常安全：** 如前所述，Pimpl 与 **copy-and-swap** 策略结合，可以优雅地实现强异常安全。在副本上操作，然后原子性地交换指针，保证了即使操作失败，原始对象也保持不变。
> - **更好的信息隐藏：** 更彻底地隐藏了类的内部实现，符合面向对象设计原则。
>
> **缺点：**
>
> - **增加复杂性：** 引入了一个额外的结构体和一层间接性（指针解引用），代码量和理解成本会略有增加。
> - **性能开销：** 每次访问内部数据都需要一次指针解引用，这会带来微小的性能损失。对于性能敏感的场景，可能需要权衡。
> - **内存开销：** 智能指针本身以及被 Pimpl 管理的动态分配的 `PMimpl` 对象会带来额外的内存开销。
>
> ### 何时选择哪种方法？
>
> - **选择原始方法（直接成员）：**
>   - 当类很简单，内部数据结构不常变化时。
>   - 当编译时间不是主要问题，或者客户端代码数量较少时。
>   - 对性能有极致要求，且可以接受较低的异常安全保证（基本异常安全或无保证，并通过其他方式处理错误）。
> - **选择 Pimpl 方法：**
>   - 当类的实现细节复杂且可能经常变化时，例如，你预计会频繁地添加、删除或修改类的私有数据成员。
>   - 当你希望减少编译依赖，缩短大型项目的编译时间时。
>   - 当你需要**强异常安全保证**，并且希望以一种优雅、惯用的方式实现它时（与 copy-and-swap 结合）。
>   - 当你需要彻底隐藏实现细节，希望提供一个更加稳定的二进制接口时（例如，开发库）。
>
> 总的来说，Pimpl 是一种**工程上的权衡**：它牺牲了一点点简单性和性能，以换取**编译时间的大幅改善、更好的模块化和更强的异常安全性**。对于大型、复杂的 C++ 项目，Pimpl 常常是一个非常有价值的设计模式。

看起来很美满了，但其实这里还有一个问题，那就是构造函数 `Image(imgSrc)` 也有可能抛出异常，此时，输入流的一部分可能已经被读走了。所以，上面程序只能提供基本的异常保证，也即没有资源泄露，但是数据的状态时不确定的。针对这个问题，可行的方案是，接受文件的路径而不是已经打开的流，或者将流的数据提前读出来放到一个缓冲区中（例如 `vector`）。

### 5. 强异常安全保证

``` cpp
void someFunc() {
    // ...
    f1(); // copy-and-swap
    f2();
    // ... 
}
```

前面提到过，通过 `copy-and-swap` 可以实现强异常安全保证，但函数提供的“异常安全保证“通常 **最高** 只等于其所调用之各个函数的“异常安全保证”中的最弱者。例如上面的 `someFunc`，如果 `f2()` 没有提供任何异常安全保证，我们很难说 `someFunc` 可以达到强异常安全保证。

另外，即使 `f1()` 和 `f2()` 都可以达到强异常安全保证，我们也不能保证说 `someFunc` 就是强异常安全保证的了，因为不同函数之前可能存在 **“连带影响”（side effects）**。当 `f1()` 圆满结束时，程序状态在任何方面都可能有所改变，因此如果 `f2()` 随后抛出异常，程序状态和 `someFunc` 被调用前并不相同，甚至当 `f2` 没有改变任何东西时也是如此。

最后就是，如果我们使用 `copy-and-swap` 保证强异常安全，一个不容忽视的点就是拷贝的效率问题，特别是当对象很大时，拷贝带来的时间和空间开销都是值得考虑的。

总而言之，当 “强异常安全保证” 不切实际时，你就必须提供 “基本保证”。现实中你或许会发 现，你可以为某些函数提供强烈保证，但效率和复杂度带来的成本会使它对许多人而言摇摇欲坠。只要你曾经付出适当的心力试图提供强烈保证，万一实际不可行， 使你退而求其次地只提供基本保证，任何人都不该因此责难你。对许多函数而言， “异常安全性之基本保证”是一个绝对通情达理的选择。

## Q30：透彻了解 inlining 的里里外外

（1）我们知道 inline 函数可以免除函数调用成本，除此以外，编译器最优化机制通常被设计用来浓缩那些“不含函数调用”的代码，所以当你 inline 某个函数，或许编译器就因此有能力对它（函数本体）执行语境相关最优化。大部分编译器绝不会对着一个 “outlined 函数调用“动作执行如此之最优化。

（2）但是，在一台内存有限的机器上，过度热衷 inlining 会造成程序体积太大（对可用空间而言） 即使拥有虚内存， inline 造成的代码膨胀亦会导致额外的换页行为 (paging) ，降低指令高速缓存装置的击中率，以及伴随这些而来的效率损失。换个角度说，**如果 inline 函数的本体很小，编译器针对“函数本体”所产出的码可能比针对“函数调用”所产出的码更小。**果真如此，将函数 inlining 确实可能导致较小的目标码和较高的指令高速缓存装置击中率！

（3）**inline 只是对编译器的一个申请，不是强制命令。**这项申请可以隐喻提出，也可以明确提出。隐喻方式是将函数定义于 class 定义式内。需要注意的是，**friend 函数** 也可以定义在 class 内部，如果真是那样，它们也是被隐喻声明为 inline。

（4）**inline 函数通常－定被置于头文件内，**因为大多数建置环境在编译过程中进行 inlining，而为了将一个 “函数调用” 替换为 “被调用函数的本体”，编译器必须知道那个函数长什么样子。

（5）所有对 virtual 函数的调用（除非是最平淡无奇的）也都会使 inlining 落空。这不该令你惊讶，因为 virtual 意味 “等待，直到运行期才确定调用哪个函数”，而 inline 意味 “执行前，先将调用动作替换为被调用函数的本体”。如果编译器不知道该调用哪个函数， 你就很难责备它们拒绝将函数本体 inlining。

（6）有时候虽然编译器有意愿 inlining 某个函数，还是可能为该函数生成一个函数本体。举个例子，如果程序要取某个 inline 函数的地址，编译器通常必须为此函数生成一个 outlined 函数本体。毕竟编译器哪有能力提出一个指针指向并不存在的函数呢？与此并提的是，**编译器通常不对 “通过函数指针而进行的调用” 实施inlining。**

（7）实际上构造函数和析构函数往往是 inlining 的糟糕候选人，即使是一个类似于 `Derived() {}` 这样的，参数列表和函数体完全为空的构造函数，编译器在背后所做的事情可能超乎你的想象

1. 派生类需要调用基类的默认构造函数
2. 如果构造函数抛出异常，该对象已经构造好的那一部分会被自动销毁（调用析构函数）
3. 等等

（8）程序库设计者必须评估 “将函数声明为 inline" 的冲击： inline 函数无法随着程序库的升级而升级。换句话说如果 f 是程序库内的一个 inline 函数，客户将  “f 函数本体” 编进其程序中，一旦程序库设计者决定改变 f, 所有用到 f 的客户端程序都必须重新编译。这往往是大家不愿意见到的。然而如果 f 是non-inline 函数，一旦它有任何修改，客户端只需重新连接就好，远比重新编译的负担少很多。如果程序库采取动态连接，升级版函数甚至可以不知不觉地被应用程序吸纳。

程序开发而言，将上述所有考虑牢记在心很是重要，但若从纯粹实用观点出 发，有一个事实比其他因素更重要：大部分调试器面对 inline 函数都束手无策。这对你应该不是太大的意外，毕竟你如何在一个并不存在的函数内设立断点呢？虽然某些建置环境勉力支持对 inlined 函数的调试，其他许多建置环境仅仅只能“在调试版程序中禁止发生 inlining" 。

（summary）这使我们在决定哪些函数该被声明为 inline 而哪些函数不该时，掌握一个合乎逻辑的策略。一开始先不要将任何函数声明为 inline，或至少将inlining 施行范围局限在那些 “一定成为 inline" （见条款 46) 或 “十分平淡无奇”  的函数身上。慎重使用 inline 便是对日后使用调试器带来帮助。

不过这么一来也等于把自己推向手工最优化之路。不要忘记 80-20 经验法则：平均而言一个程序往往将 80% 的执行时间花费在 20% 的代码上头。这是一个重要的法则， 因为它提醒你，作为一个软件开发者，你的目标是找出这可以有效增进程序整体效率的 20% 代码，然后将它 inline 或竭尽所能地将它瘦身。但除非你选对目标，否则 一切都是虚功。

## Q31：将文件之间的编译依存关系降至最低

### 1. Compilation Firewall 的缺失

**C++ 没有把 “将接口从实现中分离” 这事做得很好**，这会导致，有时候我们只对某个 class 的实现文件做了轻微修改（注意我们没有修改 class 接口），甚至我们修改的还是 private 成分，但是当我们重新 make 时，你会发现，整个世界都被重新编译和链接了。😖

例如：

``` cpp
// person.h
class Person {
public:
    Person(const std::string &name, const Date &birthday, const Address &address);
    std::string name() const;
    std::string birthday() const;
    std::string address() const;
    // ...
private:
    std::string _name;
    Date _date;
    Address _address;
};
```

上面的代码要想通过编译，就必须包含以下头文件：

``` cpp
#include <string>
#include "date.h"
#include "address.h"
```

**因为编译器必须在编译期间知道对象的大小**，而这就意味着编译器必能看到对象的“定义式”。

> 问题的核心在于：
>
> 1. **对象大小的确定性：** 编译器在编译时必须知道一个类的大小（`sizeof(Person)`），才能正确地分配内存、在栈上创建对象、计算成员变量的偏移量等。
> 2. **内联函数展开：** 如果类中有内联函数（包括在类定义内部实现的成员函数），编译器需要看到这些函数的完整定义，以便在调用点进行展开。
> 3. **模板实例化：** 如果类是模板的一部分，或者使用了模板成员，编译器在实例化时也需要完整的定义信息。
>
> 为了满足这些要求，C++ 标准强制要求类的**完整定义**必须在编译单元中可见。这意味着即使是 `private` 成员，它们的类型和声明也必须出现在头文件中。因此，`Person` 类的头文件不得不 `#include <string>`、`"date.h"` 和 `"address.h"`，从而建立了**编译时依赖关系**。

这种依赖关系会导致恶性循环：

- 如果 `string`、`Date` 或 `Address` 的**任何内部实现细节发生改变**（例如 `Date` 类添加了一个新的私有成员，`"date.h"` 会被更新）。
- 因为 `"date.h"` 改变了，所有包含 `Person.h` 的文件（直接或间接）都必须重新编译。
- 所有使用 `Person` 对象的源文件，如果它们包含了 `Person.h`，也必须重新编译。
- 最终，所有的对象文件（`.o`）都需要重新链接，即使仅仅是 `Date` 类的一个私有成员发生了微小变化。

这种现象被称为**编译防火墙（Compilation Firewall）**的缺失，它使得在大型 C++ 项目中，*即使是局部的小改动，也可能导致漫长的全项目编译时间*。这大大降低了开发效率，是 C++ 开发人员经常抱怨的问题之一。

为了解决该问题，常用设计策略有：

1. 如果可以使用 object reference 和 object pointer，优先使用它们
2. 如果能够，尽量以 class 声明式代替 class 定义式
3. 为声明式和定义式提供不同的头文件
   * 例如 C++ 中的 `iosfwd.h` 内含  iostream 各组件的声明式。

这些策略的本质就在于，<font color=blue>**以 “声明的依存性” 替换 “定义的依存性”**</font>，那正是编译依存性最小化的本质：现实中让头文件尽可能自我满足，万一做不到，则计它与其他文件内的声明式（而非定义式）相依。其他每件事都源自于这个简单的设计策略。

### 2. Pimpl Idiom

为了缓解这个问题，C++ 社区发展出了各种设计模式和技术，其中最主要和最常用的就是我们之前讨论的 **Pimpl Idiom（Pointer to Implementation）**。

Pimpl 的核心思想就是将类的私有成员（那些会引起编译依赖的细节）移动到一个单独的辅助类或结构体中，然后主类只持有一个指向这个辅助类的**不完整类型指针**（或者智能指针）。这样，在主类的头文件中，你就不需要包含那些复杂的实现细节头文件了。

例如，`Person` 类使用 Pimpl 后可能会变成这样：

```cpp
// Person.h
#include <memory> // For std::unique_ptr or std::shared_ptr

// 声明一个不完整的类型，编译器此时不需要知道其定义
// "声明"即可，无需包含 PersonImpl.h
class PersonImpl; 

class Person {
public:
    Person(const std::string &name, const Date &birthday, const Address &address);
    // ... 其他接口保持不变 ...

private:
    std::unique_ptr<PersonImpl> pImpl; // 使用智能指针管理实现
};

// Person.cpp
#include "Person.h"
#include <string> // 现在只在 Person.cpp 中需要
#include "date.h"   // 现在只在 Person.cpp 中需要
#include "address.h"// 现在只在 Person.cpp 中需要

// PersonImpl 的完整定义只在 .cpp 文件中可见
class PersonImpl {
public:
    std::string _name;
    Date _date;
    Address _address;

    PersonImpl(const std::string &name, const Date &birthday, const Address &address)
        : _name(name), _date(birthday), _address(address) {}
    // ... 还可以把 Person 的所有实现逻辑都放在这里
};

// Person 构造函数的实现
Person::Person(const std::string &name, const Date &birthday, const Address &address)
    : pImpl(std::make_unique<PersonImpl>(name, birthday, address)) {
}

// ... Person 的其他成员函数，通过 pImpl 访问 PersonImpl 的成员 ...
```

使用 Pimpl 后：

- `Person.h` 不再需要包含 `<string>`、`"date.h"` 和 `"address.h"`。
- 如果 `Date` 或 `Address` 的内部实现改变了，只有 `Person.cpp` 需要重新编译，而其他包含了 `Person.h` 的文件则不受影响。
- 这就大大提高了编译效率，特别是在大型项目中。

当然，使用 Pimpl Idiom 也是有代价的。成员函数必须通过 implementation pointer 取得对象数据。那会为每一次访问增加一层 **间接性**。而每一个对象消耗的内存数景必须增加 implementation pointer 的大小。最后， implementation pointer 必须初始化，指向一个动态分配得来的 implementation object, 所以你将蒙受因动态内存分配（及其后的释放动作）而来的额外开销，以及遭遇 bad_alloc 异常（内存不足）的可能性。

### 3.  Abstract Base Class

抽象基类（Abstract Base Class，ABC）在解决 C++ 编译依赖问题上，其作用与 Pimpl Idiom 有所不同，它主要通过**定义接口而非具体实现来切断部分编译依赖**。

让我们用你提供的 `Person` 类例子来理解这一点，并假设我们想让 `Person` 成为一个抽象概念，可能有多种具体实现（例如 `Employee`、`Customer` 等）：

```cpp
// person_interface.h (这是一个抽象基类头文件)
#include <string> // string 通常是基本类型，或者本身就是 Pimpl 化的，依赖较小

// Date 和 Address 在这里不需要完整定义，只需要声明即可
// 假设 Date 和 Address 也有相应的接口类或 Pimpl 实现，这里只做前置声明
class IDate;    // 接口或Pimplized Date
class IAddress; // 接口或Pimplized Address

class IPerson { // 使用 'I' 前缀表示接口 (Interface)
public:
    virtual ~IPerson() = default; // 虚析构函数，保证多态删除的正确性

    // 纯虚函数，定义了 Person 接口必须提供的行为
    virtual std::string name() const = 0;
    virtual std::string birthday() const = 0; // 返回的可能是一个格式化字符串
    virtual std::string address() const = 0;  // 返回的可能是一个格式化字符串

    // 工厂方法：负责创建具体实现类的实例，客户端无需知道具体实现类
    static std::unique_ptr<IPerson> create(const std::string& name, 
                                             const IDate& birthday, 
                                             const IAddress& address);
};
```

```cpp
// employee.h (一个具体的实现类)
#include "person_interface.h" // 包含抽象基类
// ... 如果 Employee 有自己的私有数据和依赖，它也可以选择 Pimpl
class Employee : public IPerson {
public:
    Employee(const std::string& name, const IDate& birthday, const IAddress& address);
    
    // 实现 IPerson 的纯虚函数
    std::string name() const override { return _name; }
    std::string birthday() const override; // 具体实现可能需要 Date 的完整定义
    std::string address() const override;  // 具体实现可能需要 Address 的完整定义

private:
    // Employee 自己的私有数据，这里仍然直接包含在头文件中，这依然是编译依赖点
    // 如果这些成员复杂或频繁变化，Employee 自身也应该考虑 Pimpl
    std::string _name; 
    // 为了简化，这里仍然使用原始 Date 和 Address，但实际中会是 IDate/IAddress 的实现
    // 或者 Employee 自身对 Date 和 Address 的 Pimpl 化
    Date _date;    
    Address _address;
    int employeeId; // Employee 特有的数据
};
```

#### 抽象基类如何解决编译依赖问题？

抽象基类通过以下方式缓解编译依赖：

1. **定义了稳定的接口：** 在 `person_interface.h` 中，`IPerson` 类定义了一组纯虚函数，这些函数构成了 `Person` 对象的**外部可见行为**。只要这些纯虚函数的签名（名称、参数、返回类型）不变，`IPerson` 的接口就是稳定的。

2. **客户端只依赖接口：** 使用 `IPerson` 接口的客户端代码（例如，一个处理 `IPerson` 对象列表的函数）只需要包含 `person_interface.h`。它们可以这样操作：

   ```cpp
   // client_code.cpp
   #include "person_interface.h" // 只包含接口头文件
   
   void processPeople(const std::vector<std::unique_ptr<IPerson>>& people) {
       for (const auto& person : people) {
           std::cout << "Name: " << person->name() << std::endl;
           // ... 多态调用
       }
   }
   
   // 在其他地方可能通过工厂方法创建具体对象，然后传递给 processPeople
   // IPerson::create() 的实现将决定创建 Employee 还是 Customer
   ```

   在这种情况下，`client_code.cpp` **无需知道 `Employee` 或 `Customer` 的具体实现细节**（它们的私有数据成员是什么，它们的 `.cpp` 文件依赖了哪些头文件）。它只关心 `IPerson` 提供的接口。

3. **将实现细节推迟到派生类和源文件：** `Employee` 或 `Customer` 这些具体实现类才包含它们自己的私有数据成员，并实现 `IPerson` 接口定义的纯虚函数。这些具体实现类的头文件（例如 `employee.h`）通常会包含它们的私有数据成员所需的头文件（例如 `Date.h` 和 `Address.h`），但这些头文件只会在**需要创建或直接操作具体类型的地方**被包含。

#### 限制与 Pimpl Idiom 的对比

尽管抽象基类提供了接口与实现分离，但它在解决**编译依赖**方面的能力是**有限的**：

- **编译依赖仍存在于派生类：** 如果你修改了 `Employee` 类中的私有数据成员（例如，改变 `_name` 的类型，或者添加/删除一个私有成员），那么 `employee.h` 必须改变。任何直接包含 `employee.h` 并创建 `Employee` 对象的源文件仍然需要重新编译。**抽象基类本身并不能完全隐藏其派生类的具体实现细节。**
- **无法隐藏基类本身的私有数据：** 如果抽象基类本身有数据成员（虽然不常见，但合法），那么这些数据成员的类型和布局仍然会暴露在基类头文件中，导致编译依赖。
- **不适用于单一同构类：** 如果你只有一个 `Person` 类，并且它没有继承层次结构，那么抽象基类的方法就不适用，因为它需要多态和继承的概念。Pimpl Idiom 更适合这种情况。

------

**总结来说：**

- **抽象基类**通过**定义一套稳定的接口**来分离“**行为是什么**”和“**行为如何实现**”，从而让客户端代码可以只依赖接口而不依赖具体的实现类。这在处理多态性和可扩展性方面非常强大。它解决的是**逻辑层面的解耦**。
- 然而，**抽象基类并不能完全解决所有编译依赖问题**。它不会在编译层面为**具体派生类的私有数据成员**筑起“编译防火墙”。如果派生类的私有实现细节发生变化，那些直接依赖派生类的代码仍然需要重新编译。

所以，Pimpl Idiom 是在 **C++ 语言层面（尤其是编译层面）** 解决编译依赖和信息隐藏的强大工具，而抽象基类是在**面向对象设计层面**解决接口与实现分离、多态和扩展性的工具。两者可以协同使用，例如一个 Pimpl 化的类也可以实现一个抽象接口。

----

和 Pimpl Idiom 一样， 借由 Interface class 完成任务也会带来一些代价。由千每个函数都是 virtual, 所以你必须为每次函数调用付出一个间接寻址的成本。此外 Interface class 派生的对象必须内含一 个 vptr ，这个指针可能会增加存放对象所需的内存数量 —— 实际取决于这个对象除了 Interface class 之外是否 还有其他virtual 函数来源。

# 六、继承与面向对象设计

## Q32：确定你的 *public* 继承塑模出 *is-a* 关系

is-a 是 public 继承的基石，但是。。。

作者举了两个很经典的例子：

1. penguin is-a bird？
2. square is a rectangle？

在现实世界，这两个问题的答案是毫无疑问的，但是在计算机面向对象编程的世界中，可能未必。

出现这种差异的原因在于，现实世界的特殊性和一般性，抽象和具体的其界限并没有那么清晰。例如，我们说正方形是一个矩形，这没错，正方形体现了特殊性，而矩形体现了一般性，但是，我们可以让矩形的长度不变，宽度变为原来两倍，而正方形就不可以。再例如，企鹅确实是鸟，但是企鹅并不会飞。

但是我们希望在计算机面向对象的世界当中，只要 D is-a B，那么 B 可以干的事，D 就一定可以干，这种抽象和具体、特殊和一般的关系界限就太清晰了，因此，如果照搬现实世界的逻辑，很可能在面向对象编程中产生一些反直觉的错误。就比如，企鹅可以派生自鸟，但你不能给鸟定义一个 `virtual fly()`，因为企鹅不会飞。

但无论怎样，有一点是确定的，**在面向对象的世界中，public 继承意味着 is-a**。因此，适用于 base class 身上的每一件事情也一定使用在 derived class 身上，因为 *every derived class object is-a base class object*。

## Q33：避免 *hide* 继承而来的名称

派生类中的名称（变量/函数/类型定义）会 hide 基类 **所有** 的同名名称，这在 public 继承下是绝对不允许的，我们可以通过 using 声明式或 forwarding function 手动引入基类被 hide 的名称。

## Q34：区分接口继承和实现继承

讨论在 public 继承下，对于一个函数，有三种继承语义：

* pure virtual：只继承接口
* impure virtual：继承接口和缺省实现
* non-virtual：只继承实现

对于 pure virtual 和 non-virtual 继承，其含义很明确，前者要求派生类继承该接口后必须给出实现，否则无法实例化；而后者要求派生类必须继承该实现，并且不能修改。

对于 impure virtual 继承，值得我们小心，因为对于该继承语义来说，实际上是有些模糊，或者说容易出错的。在基类中，对于 impure virtual 函数，我们需要给出其实现，也称之为默认（缺省）实现，对于派生类，如果不想自己实现，可以继承该派生实现。

但这里有一个问题，如果该默认实现无法适用于所有派生类，也就是说，有些派生类可以使用该默认实现，而有些派生类无法使用该默认实现，这是很可能的。因为随着软件的拓展维护，后续的派生类和之前的派生类差异较大的情况是很可能发生的，此时，新添加的派生类无法像原本的派生类一样使用基类的缺省实现是完全可能的。

如果发生了这种情况，并且派生类没有提供其特殊化实现的话，错误就发生了。当然，这其实是客户端（派生类）自己的错误，是客户端没有提供其特殊化实现导致的错误。但其实，追根溯源，这或许更应该是因为基类提供了不正确的接口，如果基类不提供缺省实现，那么派生类就必须提供其特殊化实现，也就不会出现这种错误了。

所以，为了处理这种情况：基类希望为 virtual 函数提供其缺省实现，但有些派生类可能不能使用该缺省的行为，也即必须提供其特殊化实现，一种解决方案如下：

``` cpp
class Base {
public:
    virtual void func() = 0;
};

// 可以在类定义外部给纯虚函数提供实现
void Base::func() {
    cout << "Base" << endl;
}

class Derived1 : public Base {
public:
    void func() override {
        cout << "Derived" << endl;
    }
};

class Derived2 : public Base {
public:
    void func() override {
        Base::func();
    }
};

int main()
{   
    Derived1 d1;
    d1.func(); // Derived
    Derived2 d2;
    d2.func(); // Base
    return 0;
}
```

将基类的 virtual 函数声明为 pure virtual，然后为其提供缺省的行为，是的，我们可以为 pure virtual 函数提供缺省实现，但要注意，此时该 class 仍然是 abstract class。由于基类声明的是 pure virtual，派生类就必须实现该 pure virtual 函数：

1. 调用 `Base::func()`，继承基类实现
2. 特殊化实现

## Q35：考虑 *virtual* 函数以外的其它选择

下面是一个使用 virtual 函数实现多态的例子：

``` cpp
class GameCharacter {
public:
    // 返回人物的健康指数
    virtual int heathValue() const ;
};
```

### 1. ***NVI*** 手法实现 ***Template Method***

``` cpp
class GameCharacter {
public:
    int heathValue() const {
        // 事前工作
        int retVal = doHeathValue();
        // 事后工作
        return retVal;
    }

private:
    virtual int doHeathValue() const ;
};
```

核心思想就是，通过 public non-virtual 成员函数间接调用 private virtual 函数，称之为 ***non-virtual interface(NVI)*** 手法。

这个 public 函数就类似于 virtual 函数的 wrapper。因此，NVI 手法的一个优点隐身在上述代码注释“做一些事前工作”和“做一些事后 工作”之中（通过 wrapper 实现）。如果你让客户直接调用 virtual 函数，就没有任何好办法可以做这些事。

不过需要注意的是，在 NVI 手法下其实没有必要让 virtual 函数一定得是 private。这个要具体情况具体分析，比如 Derived class 在实现 virtual 函数时需要调用其 base class 的 virtual 实现，那么该 virtual 函数的访问级别就至少是 protected。

### 2. 函数指针实现 Strategy 模式

``` cpp
class GameCharacter;

// 计算健康指数的缺省算法
int defaultHealthCalc(const GameCharacter &gc);
int HealthCalcByA(const GameCharacter &gc);
int HealthCalcByB(const GameCharacter &gc);
int HealthCalcByC(const GameCharacter &gc);


class GameCharacter {
public:
    typedef int(*HealthCalc)(const GameCharacter&);
    GameCharacter(HealthCalc _healthCalcPtr = defaultHealthCalc) : healthCalcPtr(_healthCalcPtr) {}
    int healthCalc(const GameCharacter &gc) const { return healthCalcPtr(gc); } 
private:
    // 函数指针对象，指向需要使用的算法，可以动态修改
    HealthCalc healthCalcPtr;
};
```

NVI 手法最大的作用就是实现 ***Wrapper*** 的效果，但它本质上还是依赖于 virtual 函数。

通过函数指针实现 Strategy 模式最大的好处就是可以在运行时更换 Strategy，因为我们是将计算函数封装为一个函数指针对象于 class 内部，通过外部的 non-member non-friend 函数实现不同的策略。

但是 Strategy 模式也有一个显而易见的问题，那就是我们的外部策略函数是 non-member non-friend 的，因此这些函数只能访问 `GameCharacter` 的 public 成员。如果我们必须要访问 `GameCharacter` 的 private 成员，可能需要将这些函数设置为 friend，或者放在一个 `class CharacterHealthCalcStrategy` 中，然后将 `CharacterHealthCalcStrategy` 设置为 `GameCharacter` 的友元，这就是一个标准的 Strategy 模式结构了。

> friend 在一定程序上降低了 `GameCharacter` 的封装性，我们需要考虑，non-virtual 设计带来的好处是否可以弥补封装性。

另外，函数指针实现 Strategy 模式有一定的局限，例如我们只能传入指针对象，至于 C++ 的仿函数、Lambda 表达式就不行了，不过幸好，我们有 `std::function`，我们可以用 `std::function` 取代函数指针，以实现更好的兼容性。

不过，除了兼容性外，`std::function` 一个更大的好处就是，只要目标 callable object 的函数签名 **兼容** 于 `std::function` 指定的签名，就可以接受该 callable object。

所谓兼容，就是说 callable object 的签名（参数类型和返回类型）可以隐式转换为目标签名。具体的：

``` cpp
class GameCharacter;

int HealthCalcByInt(const GameCharacter &gc) {
    puts("HealthCalcByInt");
    return 0;
}

float HealthCalcByFloat(const GameCharacter &g, int level) {
    puts("HealthCalcByFloat");
    return level;
}

struct HealthCalcByChar {
    char operator()(const GameCharacter &gc) {
        puts("HealthCalcByChar");
        return 0;
    }
};

class GameCharacter {
public:
    typedef std::function<int(const GameCharacter&)> HealthCalc;
    GameCharacter(HealthCalc _healthCalcPtr = HealthCalcByInt) : healthCalcPtr(_healthCalcPtr) {}
    int healthCalc() const { return healthCalcPtr(*this); } 
protected:
    // 函数指针对象，指向需要使用的算法，可以动态修改
    HealthCalc healthCalcPtr;
};

class Alice : public GameCharacter {
public:
    Alice(HealthCalc _healthCalcPtr = HealthCalcByInt) : GameCharacter(_healthCalcPtr) {}
    float healthCalc() const { return healthCalcPtr(*this); } 
};

class Bob : public GameCharacter {
public:
    Bob(HealthCalc _healthCalcPtr = HealthCalcByInt) : GameCharacter(_healthCalcPtr) {}
    char healthCalc() const { return healthCalcPtr(*this); } 
};

int main() 
{
    int level = 1;
    GameCharacter{HealthCalcByInt}.healthCalc(); // HealthCalcByInt
    Alice{bind(HealthCalcByFloat, _1, level)}.healthCalc(); // HealthCalcByFloat
    Bob{HealthCalcByChar{}}.healthCalc(); // HealthCalcByChar
    return 0;
}
```

在函数 `HealthCalcByFloat` 中，你甚至可以通过 `bind` 实现多参数。

## Q36：绝不重定义继承而来的 *non-virtual* 函数

non-virtual 函数都是 **statically bound** 的，也即所谓调用函数之版本由其静态类型决定。

从务实的角度来说，如果我们在派生类中修改了基类的 non-virtual 函数，那么一个问题就是：对于同一个 Derived 对象， 通过基类指针和派生类指针调用会产生不同的结果。例如下面的例子：

``` cpp
class Base {
public:
    void func() {
        cout << "Base" << endl;
    }
};

class Derived : public Base {
public:
    void func() {
        cout << "Derived" << endl;
    }
};

int main()
{   
    Derived d;
    Base *pb = &d;
    Derived *pd = &d;
    pb->func(); // Base
    pd->func(); // Derived
    return 0;
}
```

另外就是，如果实在 public 继承之下，重定义继承而来 non-virtual 函数就违背了 public 继承 is-a 的语义，因为 Derived class 对象此时显然不是一个 Base 对象（无法使用基类的同名 non-virtual 函数）。

## 37：绝不重新定义继承而来的 *default parameter value*

virtual 函数是 dynamic bound，但缺省参数值确实 statically bound，我们也可以将其理解为：**函数声明是 *statically bound*，函数实现是 *dynamic bound***。

之所以不将缺省参数值设计为 dynamic bound，是考虑到 **运行期效率**，将缺省参数的确定从编译器转移到运行期，不仅需要花费更多的运行期时间，设计上也更为复杂。

另外，不要画蛇添足，看下面的代码：

``` cpp
class Base {
public:
    virtual void func(string s = "Hello") {
        cout << "Base: " << s << endl;
    }
};

class Derived : public Base {
public:
    void func(string s = "Hello") override {
        cout << "Derived: " << s << endl;
    }
};

int main()
{   
    Derived d;
    Base *pb = &d;
    Derived *pd = &d;
    pb->func(); // Derived: Hello
    pd->func(); // Derived: Hello
    return 0;
}
```

我们在 derived class 中的 virtual 函数 `func` 中添加了缺省参数 `s="hello"`，这起来没什么问题，因为这个缺省值和 base class 一致，但这么做有一个显而易见的缺陷，那就是此时 derived class 的 `func` 函数 与 base class 的 `func` 函数建立了高度的依赖关系（derived 依赖于 base），也即，如果我们要修改 base `func`，就需要修改 derived `func`，这显然会带来维护问题。

但如果我们删去派生类中的缺省参数定义，又会导致一个新问题，那就是 **派生类不会继承基类 virtual 函数的缺省参数**：

``` cpp
class Base {
public:
    virtual void func(string s = "Hello") {
        cout << "Base: " << s << endl;
    }
    void print(string s = "Hello") {
        cout << "Base: " << s << endl;
    }
};

class Derived : public Base {
public:
    void func(string s) override {
        cout << "Derived: " << s << endl;
    }
};

int main()
{   
    Derived d;
    Base *pb = &d;
    Derived *pd = &d;
    pb->func(); // Derived: Hello
    // pd->func(); // error: no matching function for call to ‘Derived::func()’
    return 0;
}
```

因为当静态类型为 derived class 时，是使用 derived `func` 的函数声明，而 derived `func` 的函数声明中是没有缺省参数的。

总而言之，就是很奇怪，对于基类定义的缺省参数，我们不能修改，也没有继承。

或许，你可以不使用虚函数，例如条款 35 介绍了一些取代 virtual 函数的方法，下面我们可以使用 **NVI（Non-Virtual Interface）Method**来修改代码：

``` cpp
class Base {
public:
    // Non-Virtual Interface
    void func(string s = "hello") { 
        doFunc(s);
    }
private:
    virtual void doFunc(string s) {
        cout << "Base: " << s << endl;
    }
};

class Derived : public Base {
private:
    virtual void doFunc(string s) {
        cout << "Derived: " << s << endl;
    }
};

int main()
{   
    Derived d;
    Base *pb = &d;
    Derived *pd = &d;
    pb->func(); // Derived: Hello
    pd->func(); // Derived: hello
    return 0;
}
```

在上面的代码中，将 virtual 函数放到 private，通过一个 non-virtual interface `func` 来调用 private virtual function `doFunc`，由于 `func` 是 non-virtual 函数，派生类会相当于调用基类的函数，从而可以使用基类的缺省参数。

## Q38：通过 *composition* 塑模出 *has-a* 或 *is-implement-in-terms-of* 

在程序员之间，复合 (composition) 这个术语有许多同义词，包括 layering （分层）， containment （内含）， aggregation （聚合）和 embedding （内嵌）。

“public 继承” 带有 is-a （是一种）的意义。复合也有它自己的意义。实际上它有两个意义。复合意味 ***has-a***（有一个）或 ***is-implemented-in-terms-of*** （根据某物实现出）。

当复合发生在 **应用域** 内的对象之间，表现出 ***has-a*** 的关系；当它发生于实现域内则是表现 ***is-implemented-in-terms-of*** 的关系。	

## Q39：明智而审慎的使用 *private inheritance* 

private 继承意味着 ***implement-in-terms-of*** 的关系。如果你让 class D 以 private 形式继承 class B，你的用意是为 了采用 class B 内已经备妥的某些特性，不是因为 B 对象和 D 对象存在有任何观念上的关系。 private 继承纯粹只是一种 **实现技术**（这就是为什么继承自一个 private base  class 的每样东西在你的 class 内都是 private：因为它们都只是实现枝节而已）。

注意区分 private 继承和 public 继承：

* 如果 classes 之间的继承关系是 private，编译器不会自动将一个 derived class 对象转换为 base class 对象
* 由 private base class 继承而来的所有成员，在 derived class 中都会变成 private 属性，纵使它们在 base class 中原本是 protected 或 public 属性。

private 继承意味 is-implemented-in-terms-of （根据某物实现出），这个事实有点令人不安，因为条款 38 才刚指出复合 (composition) 的意义也是这样。你如何在两者之间取舍？

答案很简单：***尽可能使用复合，必要时才使用 private 继承。***何时才是必要？主要是当 protected 成员和/或 virtual 函数牵扯进来的时候。其实还有一种激进情况，那是当空间方面的利害关系有足够的理由让我们选择 private 继承（空基类优化）。

## Q40：明智而审慎的使用 *multiple inheritance*

### 1. 多基类重名问题

当 multiple inheritance 进入设计景框，程序有可能从一个以上的 base classes 继承相同名称（如函数、 typedef 等等）。那会导致较多的歧义 (ambiguity) 机会。例如：

``` cpp
struct Base1 {
    void print() {puts("Base1");}
};

struct Base2 {
private:
    void print() {puts("Base2");}
};

struct Derived : Base1, Base2 {};

int main()
{   
    Derived d;
    d.print(); //  error: request for member ‘print’ is ambiguous
    return 0;
}
```

即使 Base2 `print` 是 private 访问权限，在这里也会产生歧义，这是因为在 C++ 中，**<font color=blue>名字查找先于访问权限检查</font>**。

为了解决该问题，你需要显式指定需要调用哪一个 Base class 内的函数：

``` cpp
d.Base1::print();
```

### 2. 菱形继承

菱形继承会导致派生类中有多份基类成员，除了导致派生类空间浪费之外，还会导致基类成员重名问题：

``` cpp
struct Base { int val; };
struct Middle1 : Base {};
struct Middle2 : Base {};
struct Derived : Middle1, Middle2 {};

int main()
{   
    cout << sizeof(Derived) << endl; // 8
    Derived d;
    cout << d.val << endl; //  error: request for member ‘val’ is ambiguous
    return 0;
}
```

为了解决重名的问题，需要将 Base class 修改为 **virtual Base class**：

``` cpp
struct Base { int val; };
struct Middle1 : virtual Base {};
struct Middle2 : virtual Base {};
struct Derived : Middle1, Middle2 {};

int main()
{   
    cout << sizeof(Base) << endl; // 4
    cout << sizeof(Middle1) << endl; // 16
    cout << sizeof(Middle2) << endl; // 16
    cout << sizeof(Derived) << endl; // 24
    Derived d;
    cout << d.val << endl; // valid
    return 0;
}
```

但这么做会进一步恶化派生类的空间使用，我们可以发现，在使用 virtual base class 之后，派生类空间达到了 24 字节，而我们仅仅使用了 4 字节（一个 int 类型数据）。

这主要是因为在使用 virtual base class 之后，class middle1 和 class middle2 分别包含了一个指向 Base 的虚拟表指针 vptr，这个 vptr 与使用 virtual function 引入的 vptr 并不相同：

* virtual function 的 vptr 指向 vtable，用于动态分发，查找转发函数
* virtual base class 的 vptr 指向虚基类偏移表，用于定位虚基类，查找对象中基类部分的偏移量

因为虚基类子对象在最终派生类中的位置是**不确定的**，取决于具体的继承层次结构，具体来说，**它被"推迟"到最终派生类的末尾**，位置取决于整个继承层次结构。编译器无法在编译时确定偏移量，所以需要通过 vptr 和虚基类表在运行时动态计算。

除了派生类体积增大之外，virtual inheritance 还会带来效率问题，因为对于虚基类对象，我们需要先访问 vptr，

virtual 继承的成本还包括其他方面。支配 “virtual base classes 初始化”的规则比起 non-virtual bases 的情况远为复杂且不直观。**virtual base 的初始化责任是由继承体系中的最低层 (most derived) class 负责**，这暗示 ：

1. classes 若派生自 virtual bases  而需要初始化，必须认知其 virtual bases 不论那些 bases 距离多远
2. 当一个 新的 derived class 加入继承体系中，它必须承担其 virtual bases （不论直接或间接） 的初始化责任。

> ## vptr 的真正机制
>
> ### 1. vptr 指向虚基类表（Virtual Base Table）
>
> ```cpp
>class Base { int base_data; };
> class Derived1 : virtual public Base { int d1_data; };
>class Derived2 : virtual public Base { int d2_data; };
> class Final : public Derived1, public Derived2 { int final_data; };
>```
> 
>每个包含虚基类的类都有一个**虚基类表**：
> 
>```
> Derived1 在 Final 中的虚基类表：
> [偏移量信息]
>      - Base 相对于 Derived1 的偏移量: +24 (举例)
> 
> Derived2 在 Final 中的虚基类表：  
> [偏移量信息]
>   - Base 相对于 Derived2 的偏移量: +20 (举例)
> ```
> 
>### 2. 不同"最终派生类"有不同的表
> 
>关键是：**同一个类（如 Derived1）在不同的最终派生类中会有不同的虚基类表**。
> 
>```cpp
> // 情况1：只有 Derived1
>class Final1 : public Derived1 {
>     int final1_data;
> };
>
> // 情况2：Derived1 + Derived2
>class Final2 : public Derived1, public Derived2 {
>     int final2_data;
>};
> ```
>
> 编译器会生成：
> 
> ```
> Derived1_in_Final1 的虚基类表：
>   - Base 偏移量: +12
> 
> Derived1_in_Final2 的虚基类表：
>   - Base 偏移量: +24
> ```
> 
> ### 3. 访问过程
> 
> 当通过 `Derived1` 访问虚基类 `Base` 时：
> 
> ```cpp
> Final2 obj;
> Derived1* ptr = &obj;
>ptr->访问_Base_成员();  // 这里发生了什么？
> ```
>
> 步骤：
>
> 1. 读取 `Derived1` 部分的 vptr
> 2. vptr 指向 `Derived1_in_Final2` 的虚基类表
> 3. 从表中读取 Base 的偏移量
> 4. 计算实际地址：`Derived1地址 + 偏移量`
> 
> ## 为什么不能直接在 vptr 里存偏移量？
> 
> 因为一个类可能有**多个虚基类**：
> 
> ```cpp
> class Base1 { int b1; };
> class Base2 { int b2; };
> class Derived : virtual public Base1, virtual public Base2 {
>    int d;
> };
>```
> 
>`Derived` 的虚基类表需要记录：
> 
> ```
> Derived 的虚基类表：
>  - Base1 的偏移量: +8
>   - Base2 的偏移量: +12
>```
> 
>一个指针无法存储多个偏移量，所以需要指向一个表。
> 
>## 总结
> 
>- vptr **不是**直接记录位置
> - vptr **指向**一个表，表中记录了所有虚基类的位置信息
>- 同一个类在不同最终派生类中有不同的虚基类表
> - 这样设计支持多虚基类的情况
>
> 所以更准确的说法是：vptr 指向的表记录了当前继承层次中所有虚基类相对于当前子对象的偏移量。

----

我对virtual base classes （亦相当千对 virtual 继承）的忠告很简单。第一，非必 要不使用virtual bases。平常请使用 non-virtual 继承。第二，如果你必须使用 virtual base classes, 尽可能避免在其中放置数据。这么一来你就不需担心这些classes 身上 的初始化（和赋值）所带来的诡异事情了。 

### 3. 一个多重继承的实例

多重继承的确有正当用途。其中一个情节涉及 “public 继承某 Interface class"  和 “private 继承某个协助实现的 class" 的两相组合。

* public 继承自某接口
* private 继承自某实现

# 七、模板与泛型编程

## Q41：了解 *implicit interface* 和 *compile-time polymorphism*

>  classes 和 templates 都支持 interfaces 和 polymorphism。
>
> * 对 classes 而言接口是 explicit，以函数签名为中心。多态则是通过 virtual 函数发生于运行期。
> * 对 template 参数而言，接口是 implicit，奠基于有效表达式。多态则是通过 template 具现化和 function overloading resolution 发生于编译期。

通常 explicit interface 由函数签名（返回类型、函数名称、参数类型、常量性、左值/右值属性）构成，而 implicit interface 则完全不同，它并不基于函数签名，而是由有效表达式组成，例如：

``` cpp
template<typename T>
void processing(T &w) {
    if(w.size() > 10 && w != womeNastyData) {
        // ...
    }
    else {
        // ...
	}
}
```

类型 T 的隐式接口 `processing` 有这些约束：

* 必须提供一个名为 `size` 的成员函数
* 支持 `operator!=`

## Q42：了解 typename 的双重含义

typename 的第一重含义就是在模板元编程中指定类型参数，此时，typename 和 class 可以达到相同的效果。

typename 的第二重含义就是验明嵌套从属类型名称，也即，**如果我们在 template 中涉及一个嵌套从属类型名称时，需要在它的前面添加 typename，表示此东西是一个类型而不是一个（静态）变量。**不过，这一规则有一个例外：typename 不可以出现在 base classes list 内的嵌套从属类型名称之前，也不可以在 member initialization list 中作为 base class 修饰符。

> ## 使用规则
>
> ### 1. 必须使用typename的情况
>
> 当访问模板参数的嵌套类型时：
>
> ```cpp
> template<typename T>
> void func() {
>     typename T::iterator it;  // 必须使用typename
>     typename T::value_type val;  // 必须使用typename
> }
> ```
>
> ### 2. 不得使用typename的情况
>
> #### 在基类列表(base class lists)中：
>
> ```cpp
> template<typename T>
> class Derived : public T::BaseClass {  // 不能写成 typename T::BaseClass
>     // ...
> };
> ```
>
> #### 在成员初始化列表(member initialization list)中：
>
> ```cpp
> template<typename T>
> class MyClass {
>     T::value_type member;
>     
> public:
>     MyClass() : member(T::SomeClass()) {  // 不能写成 typename T::SomeClass
>         // ...
>     }
> };
> ```
>
> ## 原因解释
>
> - **基类列表**：编译器知道这个位置只能是类型，所以不需要 typename
> - **成员初始化列表**：编译器能够从上下文推断出这是构造函数调用，不需要 typename
> - **其他位置**：编译器无法确定是类型还是静态成员，所以需要 typename 明确指出

## Q43：学习处理 *templatized base class* 内的名称

### **1. 模板的两阶段编译（Two-Phase Lookup）**

C++ 模板编译分为两个阶段：

1. **模板定义阶段**（Template Definition Time）
   - 检查所有 **不依赖模板参数** 的语法（如普通函数调用、静态成员访问等）。
   - 这些调用必须在模板定义时就可见，否则报错。
2. **模板实例化阶段**（Template Instantiation Time）
   - 检查所有 **依赖模板参数** 的调用（如 `T::foo()` 或 `func(t)`，其中 `t` 是 `T` 类型）。
   - 这些调用可以等到模板实例化时再检查。

### **2. 模板类继承中的名称查找问题**

先来看下面的例子：

``` cpp
class CompanyA {
public:
    void sendClearText(const string &msg) {
        cout << "Company A Clear Text: " << msg << endl;
    }
    void sendEncrypted(const string &msg) {
        cout << "Company A Encrypted Text: " << msg << endl;
    }
};

class CompanyB {
public:
    void sendClearText(const string &msg) {
        cout << "Company B Clear Text: " << msg << endl;
    }
    void sendEncrypted(const string &msg) {
        cout << "Company B Encrypted Text: " << msg << endl;
    }
};

struct MsgInfo { 
    string msg;
    // other info ...
};

template<typename Company>
class MsgSender {
public:
    void sendClear(const MsgInfo &info) {
        Company c;
        c.sendClearText(info.msg);
    }
    void sendSecret(const MsgInfo &info) {
        Company c;
        c.sendEncrypted(info.msg);
    }
};

template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    void logSendClear(const MsgInfo &info) {
        // log message before send
        sendClear(info);
        // log message after send
    }

    void logSendSecret(const MsgInfo &info) {
        // log message before send
        sendSecret(info);
        // log message after send
    }
};
```

上面的代码存在一个问题，编译代码之后，在类 `LoggingMsgSender` 中，编译器会报错如下：

``` cpp
g++ test.cpp -Wall -Wextra -std=c++23  -I/opt/ -o ./bin/test  
test.cpp: In member function ‘void LoggingMsgSender<Company>::logSendClear(const MsgInfo&)’:
test.cpp:67:9: error: there are no arguments to ‘sendClear’ that depend on a template parameter, so a declaration of ‘sendClear’ must be available [-fpermissive]
   67 |         sendClear(info);
      |         ^~~~~~~~~
test.cpp:67:9: note: (if you use ‘-fpermissive’, G++ will accept your code, but allowing the use of an undeclared name is deprecated)
test.cpp: In member function ‘void LoggingMsgSender<Company>::logSendSecret(const MsgInfo&)’:
test.cpp:73:9: error: there are no arguments to ‘SendSecret’ that depend on a template parameter, so a declaration of ‘SendSecret’ must be available [-fpermissive]
   73 |         SendSecret(info);
      |         ^~~~~~~~~~
```

可以发现，编译器分别对 `sendClear(info)` 和 `sendSecret(info)` 报错，并且理由都是一样的：*error: there are no arguments to ‘sendClear’ that depend on a template parameter, so a declaration of ‘sendClear’ must be available [-fpermissive]*

我们以针对 `sendClear(info)` 的报错为例，意思就是说，对于 `sendClear` 这个函数，它的参数不依赖于模板参数 `Company`（显然的，该函数没有使用该模板参数），因此编译器必须在模板定义时（也即模板的第一阶段检查）就确定它的合法性（看到它的声明），而不能等到模板实例化（模板的第二阶段检查）时才检查。

因此说，`sendClear(info)` 需要在模板定义阶段进行检查，而由于编译器此时没有看到该函数的声明，于是报错了。

这种情况在 **模板派生类继承自一个模板基类** 时尤为常见，由于当前派生类是一个类模板，所以它会触发两阶段编译，由于继承自一个模板基类，而 **在编译阶段模板基类的类型没有确定（还没有指定基类的模板参数）**，所以派生类模板看不到模板基类中成员的定义。

针对该问题，有三种解决方案：

### 3. 解决方案

#### 3.1 使用 `this->` 前缀

``` cpp
template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    void logSendClear(const MsgInfo &info) {
        // log message before send
        this->sendClear(info);
        // log message after send
    }

    void logSendSecret(const MsgInfo &info) {
        // log message before send
        this->sendSecret(info);
        // log message after send
    }
};
```

**原理：**`this` 指针明确表明我们要在当前对象中查找方法，这使得编译器将名称查找推迟到第二阶段。

**为什么有效：**

- `this` 的类型依赖于模板参数（`T* const`），所以 `this->sendClear` 被视为依赖名称
- 编译器会在实例化时进行查找，此时基类 `MsgSender<Company>` 已经确定，因此可以看到函数的定义

#### 3.2 使用 using 声明

``` cpp
template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
    using MsgSender<Company>::sendClear;
    using MsgSender<Company>::sendSecret;
public:
    void logSendClear(const MsgInfo &info) {
        // log message before send
        sendClear(info);
        // log message after send
    }

    void logSendSecret(const MsgInfo &info) {
        // log message before send
        sendSecret(info);
        // log message after send
    }
};
```

**原理**：将基类的名称引入到派生类的作用域中。

**为什么有效**：

- `using` 声明将基类的 `sendClear` 引入到派生类的作用域
- 这样 `sendClear` 就成为了派生类作用域中的名称
- 编译器可以在派生类作用域中找到这个名称

#### 3.3 使用基类限定符

``` cpp
template<typename Company>
class LoggingMsgSender : public MsgSender<Company> {
public:
    void logSendClear(const MsgInfo &info) {
        // log message before send
        MsgSender<Company>::sendClear(info);
        // log message after send
    }

    void logSendSecret(const MsgInfo &info) {
        // log message before send
        MsgSender<Company>::sendSecret(info);
        // log message after send
    }
};
```

**原理**：明确指定要在哪个作用域中查找方法。

**为什么有效**：

- 显式指定了查找的作用域
- 编译器知道要在 `MsgSender<Company>` 中查找 `sendClear`
- 虽然 `MsgSender<Company>` 依赖于模板参数，但限定名称查找会在实例化时进行

不过，该方法有一个问题就是，如果我们调用的是 virtual 函数，这种调用方式会关闭 virtual 动态绑定，因为我们显式制定了调用方（`MsgSender<Company>`）。

## Q44：将与参数无关的代码抽离 *templates*

设计不好的模板会导致 **代码膨胀（code bloat）**，特别是在关系到 *non-type template parameter* 时。

因 *non-type template parameters* 而造成的代码膨胀，往往可<u>消除</u>，做法是以函数参数或 class 成员变最替换 template 参数。

因 *type parameters* 而造成的代码膨胀，往往可<u>降低</u>，做法是让带有完全相同二进制表述 (binary representations) 的具现类型 (instantiation types)  共享实现码。例如：

1. 在某些平台下，`int` 和 `long` 有着相同的二进制表述，此时如果针对 `int` 和 `long` 生成两份模板实例，就会造成代码膨胀
2. 类似的，在大多数平台上，所有指针类型都有着相同的二进制表述，因此凡 templates 持有指针者（例如 `list<int*>`，`list<const int*>`，`list<double*>` 等等）往往应该对每一个成员函数使用唯一一份底层实现。例如都通过调用 `list<void*>` 完成工作？

 ## Q45: 运用 *member function template* 接受所有兼容类型

### 1. generalized copy ctor

需要注意的是，成员函数模板也可以适用于构造函数，例如：

``` cpp
template<typename T>
class smart_ptr {
public:
    // generalized copy ctor
    // one-argument non-explicit ctor
	template<typename U>
    smart_ptr(const smart_ptr<U> &other) : ptr(other.ptr) {}
private:
	T *ptr;
}
```

此时，我们可以将任意 `smart_ptr<U>` 隐式转换为 `smart_ptr<T>`，当然，前提是 `ptr(other.ptr)` 是合法的。

### 2. 是否还需要 non-templated member copy-ctor/assignment?

我们知道，编译器会默认为我们生成：

1. ctor
2. copy ctor
3. operator =
4. dtor

如果我们声明了 member template 用于泛化 copy ctor 和 assignment，那么我们是否还需要声明正常的（non-templated） copy ctor 和 copy assignment？毕竟当 `U == T` 时，泛化的 copy ctor 和 assignment 与 non-templated member 所达到的效果是相同的。

我们可以测试一下：

``` cpp
template<typename T>
struct Foo {
    Foo() = default;

    template<typename U>
    Foo(const Foo<U> &f) { puts("templated member copy ctor"); }

    template<typename U>
    Foo<T>& operator=(const Foo<U> &f) { puts("templated member assignment"); return *this; }
};

int main()
{   
    Foo<int> fi;
    Foo<double> fd;

    // copy ctor
    cout << "----(1)" << endl;
    Foo<int> fi1(fi);
    cout << "----(2)" << endl;
    Foo<int> fi2(fd); // templated member copy ctor

    // assignment
    cout << "----(3)" << endl; 
    fi1 = fd; // templated member assignment
    cout << "----(4)" << endl;
    fi1 = fi;

    return 0;
}
```

经过测试可以发现，即使我们声明了 member template copy-ctor/assignment，编译器仍然会生成默认的 copy-ctor 和 assignment。

这所以这样，是因为，在 C++ 中，**模板函数永远不会被编译器视为特殊成员函数**，因为特殊成员函数必须有固定的签名，而模板函数的签名是可变的。

* 特殊成员函数：编译器在特定条件下会自动生成的成员函数

## Q46：需要类型转换时请为模板定义非成员函数

在条款 24 中我们介绍过，如果所有参数都需要类型转换，请采用 non-member 函数。在这里我们有一个 Rational 模板类，我们希望为其提供一个 `operator*`：

``` cpp
template<typename T>
class Rational {
public:
    Rational(const T& numerator = 0, const T& denominator = 1) : numerator_(numerator), denominator_(denominator) {}
    const T numerator() const { return numerator_; }
    const T denominator() const { return denominator_; }

private:
    T numerator_;
    T denominator_;
};

template<typename T>
const Rational<T> operator*(const Rational<T> &lhs, const Rational<T> &rhs) {
    return Rational<T>{lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator()};
}
```

像条款 24 一样，我们预期下面的代码通过编译：

``` cpp
Rational<int> a{1, 2};
Rational<int> b{3, 4};
Rational<int> c = a * b; // valid
Rational<int> d = a * 2; // compile error
```

报错信息如下：

``` cpp
test.cpp: In function ‘int main()’:
test.cpp:40:25: error: no match for ‘operator*’ (operand types are ‘Rational<int>’ and ‘int’)
   40 |     Rational<int> d = a * 2;
      |                       ~ ^ ~
      |                       |   |
      |                       |   int
      |                       Rational<int>
test.cpp:31:19: note: candidate: ‘template<class T> const Rational<T> operator*(const Rational<T>&, const Rational<T>&)’
   31 | const Rational<T> operator*(const Rational<T> &lhs, const Rational<T> &rhs) {
      |                   ^~~~~~~~
test.cpp:31:19: note:   template argument deduction/substitution failed:
test.cpp:40:27: note:   mismatched types ‘const Rational<T>’ and ‘int’
   40 |     Rational<int> d = a * 2;
      |                           ^
```

意思就是说，我们不能将 `Rational<int>` 和 `int` 作为两个参数进行 `operator*` 运算，说明问题不是出现在 `operator*`，而是出现在类型转换上，更具体来说，这里的 `2` 应该被模板推导为 `Rational<int>` 而不是 `int`。

之所以这样，是因为 <font color=blue>**在 C++ 模板实参推导的过程中，编译器不会考虑隐式类型转换函数（例如 one-argument non-explicit constructor）**</font>，这么做旨在避免推导过程变得过于复杂和模糊，从而引入不确定性。推导过程只基于实参的**精确类型匹配**和一些限定的转换（如**资格转换**——添加`const`、`volatile`，以及数组到指针的转换，函数到函数指针的转换）。

**解决方案：利用 `friend` 函数声明一个非模板函数**

``` cpp
template<typename T>
class Rational {
public:
    Rational(const T& numerator = 0, const T& denominator = 1) : numerator_(numerator), denominator_(denominator) {}
    const T numerator() const { return numerator_; }
    const T denominator() const { return denominator_; }
    friend const Rational operator*(const Rational &lhs, const Rational &rhs) {
        return Rational{lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator()};
    }
private:
    T numerator_;
    T denominator_;
};

int main()
{   
    Rational<int> a{1, 2};
    Rational<int> b{3, 4};
    Rational<int> c = a * b;
    Rational<int> d = a * 2;
    Rational<int> e = 2 * a;

    cout << c.numerator() << ' ' << c.denominator() << endl;// 3 8
    cout << d.numerator() << ' ' << d.denominator() << endl;// 2 2
    cout << e.numerator() << ' ' << e.denominator() << endl;// 2 2 
    return 0;
}
```

**工作原理：**

1. **类模板实例化：** 当您声明 `Rational<int> oneHalf;` 时，`Rational<int>` 类模板会被具现化（实例化）。在这个过程中，编译器已经明确知道 `T` 就是 `int`。
2. **`friend` 函数的声明：** 作为 `Rational<int>` 实例化的一部分，其内部声明的 `friend` 函数 `operator*` 也被具现化并声明出来。此时，这个 `operator*` 是一个**具体的、非模板的函数**，它的签名是 `const Rational<int> operator*(const Rational<int>&, const Rational<int>&)`。
3. **非模板函数调用时的隐式转换：** 由于现在 `operator*` 是一个普通的非模板函数，当您调用 `oneHalf * 2` 时，编译器会像处理任何普通函数调用一样，**考虑隐式类型转换**。因此，`int` 类型的 `2` 可以通过 `Rational<int>` 的 `non-explicit` 构造函数隐式转换为 `Rational<int>` 对象，从而成功匹配并调用这个 `friend operator*` 函数。

好了，结合该条款和条款 24，我们可以做出一个总结：<font color=blue>**如果所有参数皆需要类型转换（包括隐式类型转换），一个万能的做法是 `friend` 函数。**</font>

### 1. 如何设计 traits

traits 并不是 C++ 关键字或一个预先定义好的构件；它们是一种 **技术**，也是一个 C++ 程序员共同遵守的 **协议**。这个技术的要求之一是，<font color=blue>**它对内置 (built-in) 类型和用户自定义(user-defined)类型的表现必须一样好。**</font>

**"traits 必须能够施行于内置类型” **意味“类型内的嵌套信息 (nesting information) "这种东西出局了，因为我们无法将信息嵌套于原始指针内。

因此 **类型的 traits 信息必须位于类型自身之外**。标准技术是把它放进一个 template 及其一或多个特化版本中。这样的 templates 在标准程序库中有若干个，其中针对迭代器者被命名为 iterator_traits：

``` cpp
template<typename IterT>
struct iterator_traits;
```

iterator_traits 的运作方式是，针对每一个类型 IterT, 在 `struct iterator_traits<IterT>` 内一定声明某个 typedef 名为 iterator_category。这个 typedef 用来确认 IterT 的迭代器分类。

iterator_traits 以两个部分实现上述所言。首先它要求 **每一个“用户自定义的迭代器类型”必须嵌套一个 typedef，名为 iterator_category，用来确认适当的 tag struct。**例如：

``` cpp
template< ... >
class deque {
public:
    class iterator {
    public:
        typedef random_access_iterator_tag iterator_category;
        // ...
    }
    // ...
}

template< ... >
class list {
public:
    class iterator {
    public:
        typedef bidirectional_iterator_tag iterator_category;
        // ...
    }
    // ...
}
```

至千 iterator_traits，只是鹦鹉学舌般地响应 iterator class 的嵌套式 typedef：

``` cpp
// 类型 IterT 的 iterator_category 其实就是用来表现 “IterT 说它自己是什么”。
template<typenme IterT>
struct iterator_traits {
    typedef typename IterT::iterator_category 	iterator_category;
    // ...
}
```

为了支持指针迭代器， iterator_traits 特别针对指针类型提供一个偏特化版本：

``` cpp
template<typename IterT>
struct iterator_traits<IterT*>
{
    typedef random_access_iterator_tag iterator_category;
}
```

好了，我们现在应该知道如何设计一个 traits template class 了：

1. 确定我们想要的类型相关信息，例如迭代器的种类
2. 为该信息选择一个名字，例如 iterator_traits
3. 提供一个 template 和一组特化版本（针对内置类型）

### 2. `advance(iter,dist)` 的设计

我们现在想要实现类似于 C++ `advance` 函数的设计，代码如下：

``` cpp
template<typename IterT, typename Dist>
void Advance(IterT &it, Dist dist) {
    if(typeid(typename std::iterator_traits<IterT>::iterator_category) == typeid(random_access_iterator_tag)) {
        it += dist;
    }
    else {
        // 假设不存在 forward_iterator 但 dist < 0 的情况
        if(dist > 0) {
            while(dist -- ) ++ it;
        }
        else {
            while(dist ++ ) -- it;
        }
    }
}
```

上面的代码存在两个主要的问题：

（1）`it += dist;` 可能导致编译错误，因为 `typeid()` 是一个运行时行为，即它的值要等到运行时才能确定，这意味着编译器无法根据 `typeid` 的结果在编译时选择是编译 `it += dist` 还是 `while` 循环。所有分支的代码都会被编译。如果此时传入的迭代器类型不是随机访问迭代器，编译器依然会编译 `if(true)` 这份代码，也就包含了 `it += dist`，由于此时迭代器种类不支持 `+=`，就会报错：`error: no match for ‘operator+=’`。

> 在编译时，`typeid(typename std::iterator_traits<IterT>::iterator_category)` 表达式的类型是已知的，但其**值**（即 `std::type_info` 对象）要等到运行时才能确定。

（2）除了编译错误，`typeid()` 在运行时才能计算会导致代码运行时间增长和文件膨胀（`if(true)` 和 `if(false)` 都要进行编译），	

为了解决上面的问题，我们就不能使用 `typeid(id)`，而是使用一种可以在编译时确定的方法，在 C++17 以前（C++17 引入了 `if constexpr`），**Tag Dispatching** 是一种常用的方案，所谓标签分发（Tag Dispatching），就是说通过重载函数模板，并利用迭代器类别标签（如 `std::random_access_iterator_tag`）作为 **额外的函数参数（Tag）**，让编译器在编译时根据迭代器类别选择最匹配的重载函数。

``` cpp
template<typename IterT, typename Dist>
void doAdvance(IterT &it, Dist dist, std::random_access_iterator_tag) {
    it += dist;
}

template<typename IterT, typename Dist>
void doAdvance(IterT &it, Dist dist, std::bidirectional_iterator_tag) {
    if(dist > 0) {while(dist -- ) ++ it;}
    else {while(dist ++ ) -- it;}
}

template<typename IterT, typename Dist>
void doAdvance(IterT &it, Dist dist, std::input_iterator_tag) {
    if(dist < 0) throw std::out_of_range("Negative distance");
    while(dist -- ) ++ it;
}


template<typename IterT, typename Dist>
void Advance(IterT &it, Dist dist) {
    doAdvance(it, dist, typename std::iterator_traits<IterT>::iterator_category{});
}
```

## Q48：认识 *template metaprogramming*

Template MetaProgramming（TMP，模板元编程）是编写 template-based C++ 程序并执行于编译期的过程。

TMP 主要是个**“函数式语言” (functional language)** ，这是因为在 TMP 中，并没有在真正的循环构建，而是通过递归实现循环。

利用模板元编程的相关构建，我们可以改善条款 47 最后得到的代码，以一种更优雅的方式：

``` cpp
template<typename Iter, typename Dist>
void Advance(Iter &it, Dist dist) {
    if constexpr(std::is_same_v<typename std::iterator_traits<Iter>::iterator_category, std::random_access_iterator_tag>) {
        it += dist;
    }
    else {
        if(dist > 0) while(dist -- ) ++ it;
        else while(++ dist) -- it;
    }
}

int main()
{
{
    vector<int> v{1, 2, 3, 4, 5};
    auto it = v.begin();
    cout << *it << ' '; // 1
    Advance(it, 3);
    cout << *it << ' '; // 4
    Advance(it, -2); 
    cout << *it << endl; // 2
}
{
    int *p = new int[]{1, 2, 3, 4, 5};
    auto it = p;
    cout << *it << ' '; // 1
    Advance(it, 3);
    cout << *it << ' '; // 4
    Advance(it, -2); 
    cout << *it << endl; // 2
}
{
    list<int> s{1, 2, 3, 4, 5};
    auto it = s.begin();
    cout << *it << ' '; // 1
    Advance(it, 3);
    cout << *it << ' '; // 4
    Advance(it, -2); 
    cout << *it << endl; // 2
}
    return 0;
}
```

其中，`if constexpr `在 C++17 中引入，极大地简化了编写编译时多态代码（如根据类型特性选择不同实现）的方式，让代码更加清晰、高效且易于维护。

# 八、定制 new 和 delete

## Q49：了解 *new-handler* 的行为

``` cpp
namespace std {
    typedef void (*new_handler)();
    // set new new_handler, return old new_handler
    new_handler set_new_handler(new_henalder p) throw();
}

void out_of_memory()
{
    cerr << "Out of memory" << endl;
    // std::abort();
}

int main()
{   
    set_new_handler(out_of_memory);
    int *big_data = new int[100000000000000000L];
    return 0;
}
```

new-handler 是一个 typedef，它指向 `operator new` 无法分配足够内存时该被调用的函数。当 `operator new` 无法满足内存申请时，它会 **不断调用** new-handler，直到找到足够内存或程序结束。

默认的 new-handler 行为是抛出 `bad_alloc` 异常。

一个设计良好的 new_handler 必须做以下事情：

* **让更多内存可被使用：**这便造成 `operator new` 内的下一次内存分配动作可能成功。实现此策略的一个做法是，程序一开始执行就分配一大块内存，而后当 new-handler 第一次被调用，将它们释还给程序使用。
* **安装另一个 new_handler：**如果目前这个 new-handler 无法取得更多可用内存， 或许它知道另外哪个 new-handler 有此能力。果真如此，目前这个 new-handler 就可以安装另外那个new-handler 以替换自己（只要调用 `set_new_handler`) 。 下次当 `operator new` 调用  new-handler, 调用的将是最新安装的那个。（这个旋律的变奏之一是让 new-handler 修改自己的行为，于是当它下次被调用，就会做某些不同的事。为达此目的，做法之一是令 new-handler 修改“会影响 new-handler 行为＂的 static 数据、 namespace 数据或 global 数据。）
* **卸载 new_handler：**也就是将 null 指针传给 `set_new_handler`。一旦没有安装任何 new-handler，``operator new`` 会在内存分配不成功时抛出异常。
* **抛出 bad_alloc：**这样的异常不会被 `operator new` 捕捉，因此会被传播到内存申请处。
* **不返回：**通常调用 abort 或 exit。

下面这个代码实现了一个 **per-class new-handler** 机制，让每个类都能有自己的内存分配失败处理函数，而不是只能使用全局的 `new-handler`。

``` cpp
// RAII 管理 new-handler 资源
// 用来自动恢复 global new-handler
class NewHandlerHolder {	
public:
    explicit NewHandlerHolder(std::new_handler nh) : handler(nh)  {}
    ~NewHandlerHolder() { std::set_new_handler(handler); } 
    
    NewHandlerHolder(const NewHandlerHolder&) = delete;
    NewHandlerHolder& operator=(const NewHandlerHolder&) = delete;
private:
    std::new_handler handler;    
};

// 将 set-new-handler 行为作为一个 template base class
template<typename T>
class NewHandlerSupport {
public:
    static std::new_handler set_new_handler(std::new_handler p) noexcept;
    static void* operator new(std::size_t);
    // ...
private:
    static std::new_handler currentHandler;
};

template<typename T>
std::new_handler NewHandlerSupport<T>::set_new_handler(std::new_handler p) noexcept {
    std::new_handler oldHandler = currentHandler;
    currentHandler = p;
    return oldHandler;
}

template<typename T>
void* NewHandlerSupport<T>::operator new(std::size_t size) {
    NewHandlerHolder oldHandler(std::set_new_handler(currentHandler)); // set new-handler
    return ::operator new(size);
} // 恢复 global new-handler

template<typename T>
std::new_handler NewHandlerSupport<T>::currentHandler = NULL;


// Curiously Recurring Template Pattern
// Do it For Me
class Widget : public NewHandlerSupport<Widget> {
// ...
};
```

在上面的代码中，我们让 `Widget` 继承自 `NewHandlerSupport<Widget>`，看起来好像有一些奇怪，并且我们还可以发现，在 `NewHandlerSupport<T>` 中，没有任何用到模板参数 `T` 的行为。

事实上，这里将 `NewHandlerSupport` 设计为模板类是为了对于每一个类型参数 `T`，`NewHandlerSupport` 都会具现化一份独立的 `currentHandler`，否则，所有类型共用一份 `currentHandler`，显然是不合适的。另外，`Widget` 继承自 `NewHandlerSupport<Widget>` 其实是一种 *Curious Recurring Template Pattern*（CRTP，奇异递归递归模板模式），通过 CRTP，每个继承的类 `T` 都会获得自己独立的 `currentHandler` 静态成员，实现了类型安全的 per-class 存储。

最后，使用 Widget 的客户代码如下：

``` cpp
// 指定 Widget 的 new-handler
void out_of_memory() {
    cerr << "out of memory" << endl;
    abort();
}

void process
{   
    // 设置 Widget 的 new-handler
    Widget::set_new_handler(out_of_memory);
    
    // 使用 Widget::new-handler
    Widget *pw1 = new Widget;
	
    // 使用 global new-handler
    std::string *ps = new std::string;
}
```

## Q50：了解 new 和 delete 的合理替换时机

在什么情况下，我们会想要替换编译器提供的 `operator new` 和 `operator delete` 呢？

### 1. 检测运用上的错误

#### 内存管理的常见问题

**Memory Leaks**

- 当 `new` 分配的内存没有被正确 `delete` 时发生
- 导致内存无法回收，程序占用的内存越来越多

**Double Delete**

- 对同一块内存多次调用 `delete` 会导致未定义行为
- 可能导致程序崩溃或数据损坏

**数据越界写入**

- **Overruns**: 写入数据超出分配区块的末端
- **Underruns**: 写入数据在分配区块的起始点之前

#### 自定义内存管理器的解决方案

文中提到的解决方案是通过自定义 `operator new` 和 `operator delete` 来检测这些问题：

1. **地址追踪**: `operator new` 维护一个已分配地址的列表，`operator delete` 从中移除地址，这样可以检测到重复删除和内存泄漏
2. **边界检查:**
   - 在分配内存时额外分配一些空间
   - 在用户数据区块前后放置特定的字节模式（签名）
   - `operator delete` 时检查这些签名是否被破坏
   - 如果签名被修改，说明发生了越界写入
3. **日志记录**: 当检测到问题时，记录相关信息和有问题的指针

### 2. 为了强化效能

标准内存管理器一般是针对各种各样的场景所涉及的同样的内存管理器，而 **通用解决方案往往是各种需求的折中方案**。因此，虽然标准内存管理器对大多数情况都"适度地好"，但针对特定场景的定制化解决方案往往能带来显著的性能提升。这也解释了为什么在高性能计算、游戏开发、实时系统等对性能要求严格的领域，**自定义内存管理器** 如此重要。

例如 Folly 库就设计了针对它们所使用场景的内存管理器（jmalloc）。

通过设计针对我们所应用场景设计的内存管理器（例如倾向于分配大内存/小内存），可以 **优化内存分配和回收的效率**，**降低缺省内存管理器带来的空间额外开销**。泛用型内存管理器往往（虽然并非总是）不只比定制型慢，它们往往还使用更多内存，那是因为它们祁常在 每一个分配区块身上招引某些额外开销。针对小型对象而开发的分配器（例如 Boost 的 Pool 程序库）本质上消除了这样的额外开销。

**相关对象成簇集中。**如果你知道特定之某个数据结构往往被一起使用， 而你又希望在处理这些数据时将 page faults 的频率降全最低， 那么为此数据结构创建另一个 heap 就有意义，这么一来它们就可以被成簇集中在尽可能少的 pages 上。 new 和 delete 的 placement 版本有可能完成这样的集簇行为。

### 3. 为了收集使用上的统计数据

在一头栽进定制型 news 和定制型 deletes 之前，理当先收集你的软件如何使用其动态内存。分配区块的大小分布如何？寿命分 布如何？它们倾向于以 FIFO 次序或 LIFO次序或随机次序来分配和归还？它们的运用型态是否随时间改变，也就是说你的软件在不同的执行阶段有不同的分配/归还形态吗？任何时刻所使用的最大动态分配量 是多少？自行定义 operator new 和 operator delete 使我们得以轻松收集到这些信息。

### 但是，真的需要自定义内存管理器吗？

在 [1. 用来检测运用上的错误] 我们提到，可以在分配内存时额外分配一些空间用来放置特定的字节模式（签名），调用 `operator delete` 时检查这些签名是否被破坏，如果签名被修改，说明发生了越界写入。

那么，我们看一下下面的实现代码：

``` cpp
void* operator new(size_t size) {
    size_t realSize = size + 2 * sizeof(uint32_t);

    void *mem = malloc(realSize);
    if(!mem)    throw bad_alloc();

    // 在所分配内存前后各添加一个 signatrue(32b)
    *(static_cast<uint32_t*>(mem)) = signature;
    *reinterpret_cast<uint32_t*>((static_cast<Byte*>(mem) + realSize - sizeof(uint32_t))) = signature;
    return static_cast<Byte*>(mem) + sizeof(uint32_t);
}


int main()
{   
    int *p = new int(0xabcd1234);
    Byte *buf = reinterpret_cast<Byte*>(p);
    cout << "0x: " << hex;
    for(int i = 0 - 4; i < 4 + 4; i ++ ) {
        cout << (int)*(buf + i);    // 0x: f1f1f1f13412cdabf1f1f1f1
    }
    long long *p2 = new long long(0x1a2b3c4d5e6f7a8b);
    cout << "\n0x: " << hex << *p2 << endl; // 0x: 1a2b3c4d5e6f7a8b
    return 0;
}
```

看起来似乎没什么问题，但这其实就是区分普通程序员和高级成员员，普通内存管理器（能跑就行）和高级内存管理器的区别之一：alignment。

在我们的内存管理器中，我们是以 `sizeof(obj) - 4` 对齐的，这显然不符合计算机体系结构的对其要求（按照对象大小对其）。我们知道如果数据不进行对齐，会造成访问效率上的降低，此外，对于某些硬件来说，如果你没有进行对齐，将会直接导致硬件异常。

除此以外，一个优秀的内存管理器还需要考虑可移植性、线程安全性等复杂的细节，因此编写一个好的内存管理器是非常困难的！

况且，市面上已经有许多优秀的内存管理器了，你可以在 boost 上（例如开源的 Pool）或者寻找一些商用的内存管理器。

因此，可能在绝大多数情况下，并没有必要编写自己的内存管理器。

## Q51：编写 *new* 和 *delete* 时需固守常规

### operator new

当内存分配失败时，调用 new-handler，并且只有当 new-handler 为 NULL 时，才可以抛出 `bad_alloc` 异常，否则 `operator new` 无限调用 new-handler。

C++ 规定，即使客户要求 0 bytes，`operator new` 也得返回一个合法指针。

``` cpp
void *operator new(std::size_t size) {
    using namespace std;
    if(size == 0)
        size = 1;
    while(true) {
        if(分配成功)
            return (一个指针，指向分配得到的内存)
        
        new_handler global_handler = set_new_handler(NULL); // 获取 new-handler
        set_new_handler(global_handler); // 恢复 new-handler
        if(global_handler) 
            global_handler();
        else 
            throw std::bad_alloc();
    }
}
```

有些时候，考虑到我们的定制内存管理器可能是针对某特定 class 的对象分配提供最优化，却不是为了该 class 的任何 derived classes，也就是说，针对 class X 而设计的 `operator new`，其行为很典型地只为大小刚好为 `sizeof(X)` 的对象而设计。 然而一旦被继承下去，有可能 base class 的 `operator new` 被调用用以分配  derived class 对象。

处理此问题的最佳做法是，将“内存申请量错误”的调用行为改采标准 `operator new`，也即：

``` cpp
void* Base::operator new(std::size_t size) {
    using namespace std;
    if(size != sizeof(Base))
		return ::operator new(size);
    while(true) {
        if(分配成功)
            return (一个指针，指向分配得到的内存)
        
        new_handler global_handler = set_new_handler(NULL);
        set_new_handler(global_handler); // 恢复 new-handler
        if(global_handler) 
            global_handler();
        else 
            throw std::bad_alloc();
    }
}
```

### operator new[]

如果你决定写个 `operator new[]`，记住，**唯一需要做的一件事就是分配一块未加工内存 (raw memory)**，因为你无法对 array 之内迄今尚未存在的元素对象做任何事情。实际上你甚至无法计算这个 array 将含多少个元素对象。首先你不知道每个对象多大，毕竟 base class 的 `operator new[]`  有可能经由继承被调用，将内存分配给“元素为 derived class 对象”的 array 使用， 而你当然知道， derived class 对象通常比其 base class 对象大。

因此，你不能在 `Base::operator new[]` 内假设 array 的每个元素对象的大小是  `sizeof(Base)` ，这也就意味你不能假设 array 的元素对象个数是 `size/sizeof(Base)`。此外，传递给 `operator new[]` 的 size 参数，其值有可能比 “将被填以对象” 的内存数量更多，因为条款 16 说过，动态分配的 arrays 可能包含额外空间来来存放元素个数。

### operator delete

 operator delete 情况更简单，你需要记住的唯一事情就是 **C+＋保证“删除 null 指针永远安全”**，所以你必须兑现这项保证。

``` cpp
void operator delete(void *rawMemory) noexcept {
    if(rawMemory == 0)	return ;
    // 归还 rawMemory
}
```

对于 class 版本，只需要添加一个检查大小的操作，万一你的 class 专属的 `operator new` 将大小有误的分配行为转交 `::operator new` 执行， 你也必须将大小有误的删除行为转交 `::operator delete` 执行：

``` cpp
void Base::operator delete(void *rawMemory) noexcept {
    if(rawMemory == 0)	return ;
    if(rawMemory != sizeof(Base)) {
        ::operator delete(rawMemory);
        return ;
    }
    // 归还 rawMemory
}
```

有趣的是，如果即将被删除的对象派生自某个 base class 而后者欠缺 virtual 析 构函数，那么 C++ 传给 `operator delete` 的 size_t 数值可能不正确。此时正确大小应该是 `sizeof(Derived)`，但实际传入 `operator delete` 为 `sizeof(Base)`，因为析构函数不是虚函数，没有触发动态绑定。

## Q52：写了 *placement new* 也要写 *placement delete*

如果 operator new 接受的参数除了一定会有的那个 size_t 之外还有其他，这 便是个所谓的 placement new。

在众多 placement new 版本中特别有用的一个是“接受一个指针指向对象该被构造之处”：

``` cpp
// Default placement versions of operator new.
_GLIBCXX_NODISCARD inline void* operator new(std::size_t, void* __p) _GLIBCXX_USE_NOEXCEPT
{ return __p; }

_GLIBCXX_NODISCARD inline void* operator new[](std::size_t, void* __p) _GLIBCXX_USE_NOEXCEPT
{ return __p; }
```

这个版本的 new 已被纳入 C++ 标准库，在头文件 `new` 中。一般来说，当我们谈到 placement new 时，大多数时候指的是此特化版本，少数时候才是指接受任意额外参数的 operator new。

如果一个带额外参数的 operator new 没有 “带相同额外参数” 的对应版 operator delete，那么**当 new 的内存分配动作需要取消并恢复旧观时就没有任何 operator delete 会被调用**，从而导致内存泄漏。

----

下面举一个例子，具体的探讨 placement new 和 placement delete。

``` cpp
class Widget {
public:
    static void* operator new(size_t size, int val);
    // class 自定义 operator delete 一般有一个 size_t 参数
    // 通过该参数可以获取对象的大小，通过与 sizeof(Base) 进行比较
    // 可以判断传进来的是基类对象，还是派生类对象
    static void operator delete(void *mem, size_t size) noexcept;
};

void* Widget::operator new(size_t size, int val) {
    cout << "Widget::placement new: " << val << endl;
    return ::operator new(size);
}

void Widget::operator delete(void *mem, size_t size) noexcept {
    cout << "Widget::operator delete" << endl;
    return ::operator delete(mem);
}

int main()
{   
    Widget *w = new(10) Widget; // Widget::placement new: 10
    delete w; // Widget::operator delete

    return 0;
}
```

上面的代码运行没有任何问题，但有意思的一点是，我们可以发现，虽然我们通过 placement new 来进行分配内存，但是调用 delete 时，仍然可以通过 operator delete 销毁内存，那为什么还需要 placement delete呢？

我们稍微修改一下代码，假设 Widget 在构造函数中抛出异常：

``` cpp
class Widget {
public:
    Widget() {
        throw bad_cast();
    }
    static void* operator new(size_t size, int val);
    static void operator delete(void *mem, size_t size) noexcept;
};
```

输出结果为：

``` 
g++ test.cpp -Wall -Wextra -std=c++23  -I/opt/ -o ./bin/test           
./bin/test 
Widget::placement new
terminate called after throwing an instance of 'std::bad_cast'
  what():  std::bad_cast
make: *** [makefile:11: test] Aborted (core dumped)
```

这里需要注意一点，那就是**当内存分配成功时，而构造函数抛出异常，运行期系统有责任取消 operator new 的分配并恢复旧照**，也即此时系统应该自动调用 operator delete。

**事实上，这里也是 placement delete 唯一发挥作用的地方，那就是销毁对应 placement new 分配的内存**。由于在这里 placement new 没有相对应的 placement delete，运行期系统就不知道如何恢复原先对 placement new 的调用，于是什么也不做，从而导致内存泄漏。

因此，正确的做法应该是添加对应的 placement delete：

``` cpp
class Widget {
public:
    Widget() {
        throw bad_cast();
    }
    static void* operator new(size_t size, int val);
    static void operator delete(void *mem, int val) noexcept;
    static void operator delete(void *mem, size_t size) noexcept;
};

void* Widget::operator new(size_t size, int val) {
    cout << "Widget::placement new: " << val << endl;
    return ::operator new(size);
}

void Widget::operator delete(void *mem, int val) noexcept {
    cout << "Widget::placement delete: " << val << endl;
    return ::operator delete(mem);
}

void Widget::operator delete(void *mem, size_t size) noexcept {
    cout << "Widget::operator delete" << endl;
    return ::operator delete(mem);
}

int main()
{   
    Widget *w;
    try {
        w = new(10) Widget; 
    }
    catch(...) {
        w = NULL;
    }
    //delete w;

    return 0;
}
```

输出为：

``` cpp
Widget::placement new: 10
Widget::placement delete: 10
```

但这里又有一个有意思的点，那就是如果我们将 main 函数修改为：

``` cpp
int main()
{   
    Widget *w = new(10) Widget; 
    delete w;
    return 0;
}
```

输出为：

``` cpp
Widget::placement new: 10
terminate called after throwing an instance of 'std::bad_cast'
  what():  std::bad_cast
make: *** [makefile:11: test] Aborted (core dumped)
```

也即，如果不处理构造函数抛出的异常，任何一个 operator delete 都不会被调用，程序直接抛出异常终止掉了，我看起来似乎是合理的，因为 operator delete 只是处理程序泄露的内存，而不是处理异常。由于程序终止时，所有资源都自动归还了，再调用 operator delete 又有什么意义呢？

总而言之，一个好的设计是，如果我们声明了 placement new，就要声明相应的 placement delete（用于异常处理时运行时系统自动调用）和 operator delete（用户手动调用）。

另外就是，如果我们在 class 中声明了 class 专属的 operator new，就无法使用 global operator new，因为 class 内的名字 hide 了全局的名字：

``` cpp
class Widget {
public:
    static void* operator new(size_t size, int val);
    static void operator delete(void *mem, int val) noexcept;
    static void operator delete(void *mem, size_t size) noexcept;
};

void* Widget::operator new(size_t size, int val) {
    cout << "Widget::placement new: " << val << endl;
    return ::operator new(size);
}

void Widget::operator delete(void *mem, int val) noexcept {
    cout << "Widget::placement delete: " << val << endl;
    return ::operator delete(mem);
}

void Widget::operator delete(void *mem, size_t size) noexcept {
    cout << "Widget::operator delete" << endl;
    return ::operator delete(mem);
}

int main()
{   
    Widget *w = new Widget; // error: no matching function for call to ‘Widget::operator new(sizetype)’
    delete w;
    return 0;
}
```

针对此问题，我们可以建立一个 base class，用来防止标准的 new/delete，然后让自定义 class 继承自该 base class：

``` cpp
class StandardNewDeleteForms {
public:
    // normal new/delete
    static void* operator new(size_t size) { return ::operator new(size); }
    static void operator delete(void* mem) noexcept { ::operator delete(mem); }
    // placement new/delete
    static void* operator new(size_t size, void* ptr) noexcept { return ::operator new(size, ptr); }
    static void operator delete(void* mem, void* ptr) noexcept { ::operator delete(mem, ptr); }
    // nothrow new/delete
    static void* operator new(size_t size, const std::nothrow_t& nt) noexcept { return ::operator new(size, nt); }
    static void operator delete(void* mem, const std::nothrow_t& nt) noexcept { ::operator delete(mem); }
};

class Widget : public StandardNewDeleteForms {
public:
    using StandardNewDeleteForms::operator new;     // 引入基类的普通 new
    using StandardNewDeleteForms::operator delete;  // 引入基类的普通 delete

    static void* operator new(size_t size, int val);
    static void operator delete(void* mem, int val) noexcept;
    static void operator delete(void* mem, size_t size) noexcept;
};

void* Widget::operator new(size_t size, int val) {
    cout << "Widget::placement new: " << val << endl;
    return ::operator new(size);
}

void Widget::operator delete(void* mem, int val) noexcept {
    cout << "Widget::placement delete: " << val << endl;
    ::operator delete(mem);
}

void Widget::operator delete(void* mem, size_t size) noexcept {
    cout << "Widget::operator delete (size=" << size << ")" << endl;
    ::operator delete(mem);
}

int main() {
    Widget* w1 = new Widget;          // 调用基类的普通 new
    delete w1;                        // 调用 Widget::operator delete(void*, size_t)

    Widget* w2 = new (42) Widget;     // 调用 Widget::operator new(size_t, int)
    delete w2;                        // 调用 Widget::operator delete(void*, size_t)

    return 0;
}
```

# 九、杂项讨论

## Q53：不要轻易忽略编译器的警告

严肃对待编译器发出的警告信息。努力在你的编译器的最高（最严苛）警告级 别下争取“无任何警告”的荣誉。 

不要过度倚赖编译器的报警能力，因为不同的编译器对待事情的态度并不相同。 一旦移植到另一个编译器上，你原本倚赖的警告信息有可能消失。
