## `[*]`从汇编角度理解 C++ 对象模型

C++ 有很多很细微的概念和规则，非常微妙，容易因为忽略它们而遇到麻烦。最近写了很多代码，能感觉到有很多冗余的代码，在重构之前，想再看看基础的概念，避免犯基础错误。

### 1. 类的特殊成员函数

C++自动提供下面这些成员函数：

- 默认构造函数（如果没有定义构造函数）
- 默认析构函数（如果没有定义）
- 复制构造函数（如果没有定义）
- 赋值运算符（如果没有定义）
- 地址运算符（如果没有定义）

注意不要与涉及<u>资源管理</u>的 BIG FIVE（C++11 之前为 BIG THREE） 搞混了：

* 复制构造函数
* 析构函数 
* 赋值运算符
* 移动构造函数（C++11）
* 移动赋值运算符 （C++11）

### 2. 隐藏 `clone()` 的细节

假设有一个表示不同种类的交通工具的派生层次：

```cpp
// abstract class
struct Vehicle {
    virtual double weight() const = 0;
    virtual void start() const = 0;
    // ...
};

class RoadVehicle : public Vehicle {};
class AutoVehicle : public RoadVehicle {};
class Aircraft : public Vehicle {};
class Helicopter : public Aircraft {};
```

如果我们要跟踪处理一系列不同种类的 `Vehicle`，实际中，我们可能使用某种容器类，但是在这里，假设我们使用的是数组。我们知道，数组要求元素的类型必须是相同的，有人可能会定义如下以数组进行存储：

```cpp
Vehicle parking_lot[1000]; // 错误！
AutoVehicle x;
parking_lot[2] = x;
```

但是这种方式真的可行吗？

- `Vehicle` 为抽象基类，**抽象基类不能被实例化**，因此不会有 `Vehicle` 的对象数组；
- 当把 `AutoVehicle` 的对象赋给 `parking_lot` 的元素时，会被转成 `Vehicle` 的对象，**数据被裁剪**（对象切片）。

> 经典解决方案——指针

直接定义抽象基类对象数组的方式行不通了，有人可能会定义如下数组进行存储——**抽象基类指针数组**：

```cpp
Vehicle* parking_lot[1000];
AutoVehicle x;
parking_lot[2] = &x;
```

这样做有一个问题：如果 `AutoVehicle x;` 为局部变量，当变量 `x` 超出作用域后，我们就不知道 `parking_lot[2]` 指向的是谁。

有人可能会提议可以拷贝一份再赋值给 `parking_lot` 的元素：

```cpp
parking_lot[2] = new AutoVehicle(x);
```

这种拷贝方式带来以下问题：

- 手动管理 `new/delete`
- 必须知道对象真实类型来创建副本（深拷贝）

尽管我们可以通过 *smart pointe*r 解决手动管理内存的问题，但它只能解决内存资源管理，而不会告诉我们派生类的类型。也即，我们依然无法实现对象的深拷贝，因为对于基类指针 `Vehicle*`（静态类型），我们不知道它的动态类型（实际的派生类的类型）。

为了解决上述的拷贝方式带来的问题，可以定义一个**虚函数**，这样在不知道具体类型的情况下也能执行对这个对象的拷贝。这个时候 `Vehicle` 类可以修改为如下：

```cpp
struct Vehicle {
  virtual double weight() const = 0;
  virtual void start() const = 0;
  virtual Vehicle* clone() const = 0;
};
```

到具体的派生类就可以实现这个拷贝函数：

```cpp
Vehicle* Truck::clone() const {
  return new Truck(*this);
}
```

这个时候即使我们不知道类型也可以生成它的副本：

```cpp
Vehicle* parking_lot[1000];
AutoVehicle x;
/**
  因为中间功能的操作，我们遗失了x的类型
*/
parking_lot[2] = x.clone();
```

要清除对象，还需要为抽象基类定义一个**虚析构函数**：

```cpp
struct Vehicle {
  virtual double weight() const = 0;
  virtual void start() const = 0;
  virtual Vehicle* clone() const = 0;
  virtual ~Vehicle() {}
};
```

经过修改，利用 `clone()` 函数对对象拷贝进行封装，已经能满足我们的使用要求。但是还是把处理内存分配的实现显式地暴露了出来，怎么才能避免这个问题呢？

> `clone()` 返回的是一个裸指针，调用者需要手动 delete 掉。谁 `clone()`，谁负责释放，这是**显式的内存管理责任**；错一个 `delete` 或漏一个 `delete` 就会出问题。
>
> 因此：`clone()` 把 **“如何拷贝对象”** 封装起来了，但没有封装 **“谁来管这块堆内存的生命周期”**。

这里我们提出**代理（surrogate）**的概念——一个行为和 `Vehicle` 对象相似、而又潜在地表示了所有继承自 `Vehicle` 类的对象的东西。

这个类封装了：

- 拷贝构造
- 赋值运算
- 析构
- 行为代理（转发调用给真实对象）

让用户完全不用关心：

- `new`
- `delete`
- 对象真实类型

<font color=blue>每一个 `Vehicle` 代理都代表某个继承自 `Vehicle` 类的对象。</font>`Vehicle` 代理的定义如下：

```cpp
struct Vehicle;

struct Surrogate {
  Surrogate();
  explicit Surrogate(const Vehicle& v);
  Surrogate(const Surrogate& su);
  Surrogate& operator=(const Surrogate& su);
  ~Surrogate();
  double weight() const;
  void start() const;

  // 每个代理有代表某个对象，利用该对象的指针
  Vehicle* vp;
};
```

下面看一下实现：

```cpp
#include "Surrogate.h"
#include "Vehicle.h"

// 默认构造函数，将指针指向空
Surrogate::Surrogate() : vp(0) {}

// 转换函数，使用explicit限制，只能显式转换
Surrogate::Surrogate(const Vehicle& v) : vp(v.copy()) {}

// 复制构造函数
Surrogate::Surrogate(const Surrogate& su) : vp(su.vp ? su.vp->copy() : 0) {}

// 重载=
Surrogate& Surrogate::operator=(const Surrogate& su) {
  if (this ！= &su) {
    delete vp;
    vp = (su.vp ? su.vp->copy() : 0);
  }
  return *this;
}

// 析构函数，用来释放指针所指向内存空间
Surrogate::~Surrogate() {
  delete vp;
}

// 和所代表的对象具有相同的行为
double Surrogate::weight() const {
  return vp->weight();
}

void Surrogate::start() const {
  vp->start();
}
```

可以发现，我们把内存管理的逻辑封装在了代理类中。

下面我们测试一下所设计的代理：

```cpp
// Truck类
#include "Vehicle.h"
struct Truck : Vehicle {
  Truck();
  virtual double weight() const;
  virtual void start() const;
  virtual Vehicle* copy() const;
  virtual ~Truck();
};
```

```cpp
// Truck的实现
#include "Truck.h"
#include <iostream>

Truck::Truck() {}

double Truck::weight() const {
  return 0;
}

void Truck::start() const {
  std::cout << "Truck start..." << std::endl;
}

Vehicle* Truck::copy() const {
  return new Truck(*this);
}

Truck::~Truck() {}
```

```cpp
// main函数
#include "Surrogate.h"
#include "Truck.h"

int main() {
  Surrogate surrs[10];
  Truck tru;
  surrs[0] = Surrogate(tru);
  surrs[0].start();

  return 0;
}
```

运行结果（本文的代码都是可以直接编译运行的）：

```text
Truck start...
```

可以发现，将 `tru` 的副本绑定到了数组 `surrs` 的第1个元素上，实际上我们是利用代理去执行真正对象的操作。

**应用场景：**

- **远程代理**：为一个对象在不同的地址空间提供局部代表，隐藏对象存在于不同地址空间的事实；
- **虚拟代理**：根据需要创建开销很大的对象，通过它来存放实例化需要很长时间的真实对象；
- **安全代理**：控制真实对象的访问权限，用于对象应有不同的访问权限时；
- **智能指引**：当调用真实的对象时，代理处理另外一些事。

> 设计一个系统实现了登录功能，在用户登录时，真正的登录类和代理登录类都实现了 `Login` 接口，不同的是 `Proxy` 类的方法中增加了用户是否合法的判断，只有合法时才去调用真正登录类的 `login` 方法。
>
> 用户访问的其实是 `Proxy` 的 `login` 方法。这都是代理模式的优点。而且采用代理模式的话，你可以随时更改代理。

### 3. 不经意间的动态绑定

多态的第二种形式就是在继承中通过虚函数实现 **Template Method**（注意：这里的 Template 并不是指模板类/模板函数）。

```cpp
#include <iostream>
using namespace std;

struct Base {
  virtual void loggggg() {
    printf("calling base loggggg...\n");
  }
  void log() {
    loggggg();
    printf("calling base log...\n");
  }
  void call() {
    log();
    printf("calling base call...\n");
  }
  virtual ~Base() {}
};

struct Driver : Base {
  virtual void loggggg() {
    printf("calling Driver loggggg...\n");
  }
  virtual ~Driver() {}
};

int main() {
  Driver dr;
  dr.call();
  return 0;
}
```

输出结果：

```cpp
calling Driver loggggg...
calling base log...
calling base call...
```

可见，`loggggg()` 函数调用的是派生类中的。

如果去掉 `virtual` 关键字，调用的就是基类的 `loggggg()`。

这是为什么呢？`dr` 明明非指针或引用，为什么产生动态绑定，调用了派生类的 `loggggg()`？

答案其实藏在 `call()` 隐含的 `this` 指针里面。

由 `this` 指针的特性我们知道，`dr.call()` 会被转化为 `Base::call(Base* this)`，这里的 `this` 指针会动态绑定到派生类对象 `dr`，由于 `this` 是一个指针，而 `loggggg()` 又是一个虚函数，所以这里触发动态绑定，对 `loggggg()` 的调用将被转换为 `Derived::loggggg()` 的形式进行。

### 4. 从汇编角度理解函数调用

``` cpp
int func() 
{
    int a = 1; 
    int b = 2; 

    return 0;
}
```

[Compiler Exployer](https://godbolt.org/)

``` assembly
func():
        push    rbp
        mov     rbp, rsp
        mov     DWORD PTR [rbp-4]，1
        mov     DWORD PTR [rbp-8]，2
        mov     eax, 0
        pop     rbp
        ret
```

源码和汇编代码对应关系为：

``` cpp
int func()
{               //         push    rbp
                // mov     rbp, rsp
    
    int a = 1;	// mov     DWORD PTR [rbp-4]，1
    int b = 2;	// mov     DWORD PTR [rbp-8]，2
    return 0;	// mov     eax, 0
    
}				// pop     rbp
	   			// ret
```

栈帧结构大致为：

！[call_frame_struct](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763349385381.8vncd3e2x6.webp)

编译器基础知识：

！[编译器基础知识](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763349407650.39llz8d3vd.webp)

### 5. 从汇编角度理解类

学习 C++ 对象模型，需要关注 *Nostatic data members*、*static data members*、*Nostatic function members* 以及 *static function members* 在对象模型空间中的位置，同时关注 *virtual function* 在对象模型中怎么模塑。

首先，我们通过汇编探究一下简单类的 C++ 对象模型：

``` cpp
struct Point
{
    Point(int xval) : x(xval) {}
    virtual ~Point() = default;
    int getX() const;
    virtual int printPoint() const {
        return x;
    }
private:
    int x;
};

int main()
{
    Point point(1) ;
    return 0;
}

```

`main` 对应的汇编如下所示，将其分为六部分，具体见注释：

``` cpp
main:
        ; 第一部分寄存器保存与抬栈
        push    rbp 
        mov     rbp, rsp 
        push    rbx           ; 保存rbx，内存地址为[rbp-8]
        sub     rsp, 24       ; 抬栈
        
        ; 调用Point的构造函数
        lea     rax, [rbp-32] ; 将内存地址[rbp-32]放入rax寄存器
        mov     esi, 1        ; 将xval放入esi寄存器，
                              ; esi为Point(int)的第二个参数
        mov     rdi, rax      ; 将rax的值放入寄存器rdi寄存器,
                              ; rdi为Point(int)的第一个参数this指针
        call    Point::Point(int) [complete object constructor]
                              ; 调用构造函数Point(int)
        
        ; 将0赋值给ebx，最终作为返回值
        mov     ebx, 0 
        
        ; 调用Point的析构函数
        lea     rax, [rbp-32] 
        mov     rdi, rax 
        call    Point::~Point() [complete object destructor]
        
        ; 返回值放入eax，返回值为0
        mov     eax, ebx 
        
        ; 将rbx寄存器的值恢复
        mov     rbx, QWORD PTR [rbp-8] 
        leave 
        ret 
```

通过对 `main` 的分析，`point` 对象的 `this` 指针指向内存地址 `[rbp-32]` 处，并将其放入寄存器 `rax` 中，通过 `rdi` 寄存器传递给构造函数。

现在我们确定了 `this` 指针的指向，接下来我们需要确定 data member x 的空间位置。需要在构造函数中查看。

`Point` 的构造函数如下：

``` cpp
Point::Point(int) [base object constructor]:
        ; 保存寄存器
        push    rbp 
        mov     rbp, rsp 
        
        ; 将构造函数的入参压栈，this指针[rbp-8]，将xval放入[rbp-12]
        mov     QWORD PTR [rbp-8], rdi 
        mov     DWORD PTR [rbp-12], esi 
        
        ; 构造函数执行
        ; 处理虚表指针
        mov     edx, OFFSET FLAT:vtable for Point+16
                                        ; 将虚表指针放入寄存器edx
        mov     rax, QWORD PTR [rbp-8]  ; 将this指针指向的内存地址放入rax寄存器
        mov     QWORD PTR [rax], rdx    ; 将虚表指针放入this指针指向为内存中
        
        ; 处理 Nostatic data member
        mov     rax, QWORD PTR [rbp-8]  ; this放入rax
        mov     edx, DWORD PTR [rbp-12] ; 将xval放入寄存器edx
        mov     DWORD PTR [rax+8], edx  ; rax为main中的rbp-32，
                                        ; [rax+8]为main中的rbp-24
                                        ; 将xval放入main中的rbp-24中
        
        ; 返回main函数
        nop 
        pop     rbp 
        ret 
```

通过对 `main` 和构造函数 `Point::Point(int)` 的分析，从 `main` 函数栈的角度我们可以看出，`this` 指针指向内存地址 `[rbp-32]` 处，虚表指针 `vptr` 被放入 `[rbp-32]` 处， Nostatic data member 位于 `[rbp-24]`。

接下来看一下 Point point 的虚表是什么样的：

``` cpp
vtable for Point:
        .quad   0
        .quad   typeinfo for Point
        .quad   Point::~Point() [complete object destructor]
        .quad   Point::~Point() [deleting destructor]
        .quad   Point::printPoint() const
```

据此我们可以将 Point point 的 C++ 对象模型画出来：

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0hOe8VUZsrQia3q8okG6rCyTOeLYDOtbLmuMqKnQlsrr8LfSll2D1nhWr1oXm3ylib7TG0xicBiccOpg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=1)

>  C++ 的 ABI（尤其是 Itanium C++ ABI）中，**一个类的析构函数在编译后通常会生成三个版本**。你看到的 **“两个析构函数”** 是极其正常的现象，甚至还有第三个，只是没在 vtable 全显示出来。
>
>  你看到的是：
>
>  ```cpp
>  .quad Point::~Point() [complete object destructor]
>  .quad Point::~Point() [deleting destructor]
>  ```
>
>  背后的原因是：**不同场景下需要不同的析构行为**，编译器会生成多个“拆分职责”的析构函数版本。
>
>  # C++ 中析构函数的三个版本
>
>  按照 Itanium ABI（GCC/Clang 通用）：
>
>  | 名称                                     | 作用                                                         | 典型场景                 |
>  | ---------------------------------------- | ------------------------------------------------------------ | ------------------------ |
>  | **1. Base destructor（基础析构）**       | 只析构本类，不析构基类（用于子类调用父类的析构逻辑）         | 派生类析构时调用父类析构 |
>  | **2. Complete destructor（完整析构）**   | 析构本类 + 析构基类 + 析构成员                               | 直接析构对象（自动变量） |
>  | **3. Deleting destructor（删除版析构）** | 调用 complete destructor 后，还会调用 `operator delete(this)` | 使用 `delete ptr` 时     |
>
>  当我们使用 `delete ptr` 时，编译器会先析构 `ptr`，然后释放 `ptr` 占用的内存。
>
>  * 析构 `ptr` 时调用的是 `Complete destructor`
>  * 释放内存时调用的是 `operator delete(this)`。
>
>  而 vtable 通常只有下面两个条目：
>
>  ```cpp
>  Point::~Point() [complete object destructor]
>  Point::~Point() [deleting destructor]
>  ```
>
>  因为：
>
>  - **virtual 删除动态存储区对象时使用 deleting destructor**
>  - **virtual 析构静态存储区和自动存储区对象使用 complete destructor**
>
>  这两者恰好覆盖了所有对象的情况。
>
>  举个例子就很明白了，例如：
>
>  ```cpp
>  struct Base {
>     virtual ~Base() {}
>  };
>  
>  int main() {
>     Base* p = new Base;
>     delete p; // 调用 deleting destructor   
>     Base b;
>  } // 调用 complete destructor
>  ```



接下来我们来看一下使用 `point` 对象进行 *Nostatic function members* 和 virtual function members 调用时的汇编。

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0hOe8VUZsrQia3q8okG6rCy204HCs4aEWINWGI47vq87B98p6aK6e9cPOXAtkCicEceojxiaI5r5nRw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=2)

可以看出，**直接通过对象进行调用时，普通成员函数和虚成员函数都是直接进行函数调用，虚函数并没有通过虚表指针进行调用**。

再通过下面两个函数对比一下虚函数调用，`func1(Point point)` 使用对象调用虚函数，`func2(const Point& point)` 使用引用调用虚函数：

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0hOe8VUZsrQia3q8okG6rCyxGXDUbDUfVnyMzttCUu6E5Ioq1ZLcvj3mklibbfG28w57UZL1t8mMqg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=3)

可以看出，**使用引用调用虚函数时，是通过虚表指针进行调用的**。接下来，对 `func2` 的汇编进行解析：

``` cpp
func2(Point const&):
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        
        mov     QWORD PTR [rbp-8], rdi  ; rdi寄存器中存储的是point的
                                        ; 首地址，即this指向的地址
        mov     rax, QWORD PTR [rbp-8]  ; rax寄存器保存的也是这个地址
        
        mov     rax, QWORD PTR [rax]    ; 这个rax保存的地址指向的内存
                                        ; 内容为虚表的地址
                                        ; 现在rax寄存器的内容为虚表的地址
        
        add     rax, 16                 ; 虚表地址偏移16为保存
                                        ; printPoint地址的那一格
        mov     rdx, QWORD PTR [rax]    ; 将printPoint的地址放入rdx
        
        ; rdi中放入this指针指向的地址
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        
        ; 调用printPoint
        call    rdx
        
        leave
        ret
```

可以看出，通过**虚表指针调用虚函数千真万确**。

**指针和引用调用虚函数**

我们再看一下通过指针和引用调用虚函数有什么区别：

``` cpp
int func1(Point* point)
{
    return point->printPoint();
}

int func2(const Point& point)
{
    return point.printPoint();
}

int main()
{
    Point point(1) ;

    func1(&point);
    func2(point);

    return 0;
}
```

首先，我们看一下函数调用方 `main()` 的汇编：

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0hOe8VUZsrQia3q8okG6rCyxKtUHPW2W7lVUcBBQt6R6gsaRibS4ZXicg3PAk4klGsP2kNWVDzJA2dA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=4)

可以看出，**不管是指针还是引用，都对象的首地址**。

然后，我们看一下 `func1` 和 `func2` 的汇编，完全是一样的，都是通过虚表指针进行调用。

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0hOe8VUZsrQia3q8okG6rCyhxM7cUiaKFicyUOQ35rwNWzPUaVmMeWfF0JlbTWP2kwHuEQiaXs507g0w/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=5)

所以，**对于引用和指针来说，二者在编译器看来没有任何区别，都是对象的首地址**。

好了，一个不在继承层次中的类对象的 C++ 对象模型就学习到这了。后面还会给大家分享加上继承的 C++ 对象模型，以及多重继承和虚继承的 C++ 对象模型，尽情期待。

### 6. 单一继承派生类的对象模型

### 7. 多重继承派生类的对象模型

### 8. 菱形多重继承模型

### 9. 构造函数语义学

我们可能经常会有这样一个误区：当不给我们设计的类提供显式构造函数的时候，编译器会给我们隐式的合成一个默认构造函数出来，并将该类的 members 进行初始化。

实际上，<font color=blue>编译器不一定总是会给没有显式构造函数的类合成一个隐式默认构造函数。</font>仅仅会在需要的时候合成一个隐式默认构造函数，即使合成一个隐式默认构造函数也仅仅是为了满足编译器的需要，而非程序员需要——初始化类的 members。 C++ standard 是这么描述的：

> 对于 class X，如果没有任何 user-declared constructor，那么就会有一个 default construct 被隐式（implicitly）声明出来... ...一个被隐式声明出来的 default constructor 将是一个 trivial constructor... ...
>
> C++ standard[ISO-C++95]

如下所示，编译器不会合成隐式默认构造函数，尽管这个默认构造函数是程序需要的：

``` cpp
struct Foo {
    int value;
};
```

只有以下四种情况，编译器才会合成一个 **non trivial 隐式默认构造函数**，并且也仅仅是满足编译器的需要。那么，又满足了编译器的什么需要呢？

**1、“带有显式默认构造函数” 的 Member Class Object**

如果一个类没有任何显式的构造函数，但是它内含一个 member object，而该  member object 有显式的默认构造函数，那么编译器就会为这个类合成一个默认构造函数。

- **这个编译器合成的构造函数的职责是为了调用 member object 的构造函数，而且仅完成这件事。**
- **这个合成动作只有在这个构造函数真正需要被调用的时候才会发生。**

接下来我们用代码举例，如下代码，Bar 中含有一个 Foo 类型的成员，但是 Foo 没有任何一个显式构造函数。

```cpp
struct Foo
{
    int value;
};

struct Bar
{
    int type;
    Foo foo;
};
```

按照对象模型，Bar 类的内存布局如下所示：

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib2Qwz5Wb5NsWXHX3yibdOJb51vgZXeWw4ruibSYsOnbuoHv5a8wZmibibibPg70tTTL5KyKrMhjticPMprw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=0)



此时，当实例化一个的对象对象时：

```cpp
void foo()
{    
	Bar bar;
}
```

通过 foo() 函数对应的汇编，可以发现，编译器并没有生成并调用任何默认构造函数，而是 Bar bar 的 member 为当前的内存值：

```
foo():
        push    rbp
        mov     rbp, rsp
        nop
        pop     rbp
        ret
```

但是当为类声明并定一个显式的构造函数后，通过 foo() 函数对应的汇编，可以发现，编译器为 Bar 类合成了一个默认构造函数 `Bar::Bar()`，并调用之。

```cpp
struct Foo
{
    Foo() {}
    explicit Foo(int value) : value(value) {}
    int value;
};

struct Bar
{
    int type;
    Foo foo;
};

void foo()
{
    Bar bar;
}
```

``` cpp
Foo::Foo() [base object constructor]:
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        nop
        pop     rbp
        ret
        .set    Foo::Foo() [complete object constructor],Foo::Foo() [base object constructor]
            
Bar::Bar() [base object constructor]:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        add     rax, 4
        mov     rdi, rax
        call    Foo::Foo() [complete object constructor]
        nop
        leave
        ret
        .set    Bar::Bar() [complete object constructor],Bar::Bar() [base object constructor]
            
foo():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        lea     rax, [rbp-8]
        mov     rdi, rax
        call    Bar::Bar() [complete object constructor]
        nop
        leave
        ret
```

通过 `Bar::Bar()` 的汇编可以发现，编译器合成的 `Bar::Bar()` 中仅仅完成了对 `Foo::Foo()` 的调用，并没有初始化其他 member。

```cpp
Bar::Bar() [base object constructor]:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        add     rax, 4
        mov     rdi, rax
        ; 仅仅调用了Foo::Foo()，并没有为我们初始化其他的member
        call    Foo::Foo() [complete object constructor]
        nop
        leave
        ret
```

由于合成动作只会在正在被调用的时候发生，那么就有可能在各个不同的编译模块中合成多个默认构造函数。我们知道一个 inline 函数有静态连接性，因此合成的默认构造函数是以 inline 方式完成的。可以通过 C++Insights 这个软件进行查看：

！[图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib2Qwz5Wb5NsWXHX3yibdOJb5gn2wBvu78awqCHjictaiaem8pCibS0dNJdQHof1j1TXJ3Hv5LMo3yYbtg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=1)

编译器为 Bar 类合成的构造函数就好像这样：

```cpp
inline constexpr Bar() noexcept(false)
{
    foo.Foo::Foo();
}
```

当我们显式的为 Bar 类定义一个构造函数时，编译器就没办法再合成第二个隐式的构造函数。那么编译器还会采取行动吗？

很显然，编译器为我们扩张了显式构造函数，安插了调用每一个 member 所关联的默认构造函数。可以猜测，当我们**显式声明定义**多个构造函数时，每一个构造函数均会被编译器安插这些程序代码，而且是以 member 声明顺序调用 member 所关联的默认构造函数。这些代码将被安插在 explicit user code 之前。

**2、“带有显式默认构造函数”的 Base Class**

类似的道理，如果一个没有任何显式构造函数的类派生自一个带有显式构造函数的 base class，那么这个派生类会被编译器隐式合成一个构造函数，用于**按照继承顺序来调用 base class 的构造函数**。

```cpp
struct Foo
{
    Foo() {}
    explicit Foo(int value) : value(value) {}
    int value;
};

struct Bar : Foo
{
    int type;
};

void foo()
{
    Bar bar;
}
```

对应的汇编代码如下所示：

```
Bar::Bar() [base object constructor]:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        mov     rdi, rax
        ; 仅仅调用了Foo::Foo()，并没有为我们初始化其他的member
        call    Foo::Foo() [base object constructor]
        nop
        leave
        ret
foo():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        lea     rax, [rbp-8]
        mov     rdi, rax
        call    Bar::Bar() [complete object constructor]
        nop
        leave
        ret
```

**3、“带有一个虚函数”的 Class**

类中声明或者继承了一个虚函数，也会合成默认构造函数。以下面的代码片段为例：

```cpp
struct Foo
{
    int value;
    virtual void doSomething() {}
};

void foo()
{
    Foo foo;
}
```

对应的汇编代码如下所示：

```cpp
Foo::Foo() [base object constructor]:
        push    rbp
        mov     rbp, rsp
        mov     QWORD PTR [rbp-8], rdi
        ; 该构造函数是为了给虚表指针vptr设定初值，指向类的虚表vtbl 
        mov     edx, OFFSET FLAT:vtable for Foo+16
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax], rdx
        nop
        pop     rbp
        ret
foo():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        lea     rax, [rbp-16]
        mov     rdi, rax
        call    Foo::Foo() [complete object constructor]
        nop
        leave
        ret
vtable for Foo:
        .quad   0
        .quad   typeinfo for Foo
        .quad   Foo::doSomething()
typeinfo for Foo:
        .quad   vtable for __cxxabiv1::__class_type_info+16
        .quad   typeinfo name for Foo
typeinfo name for Foo:
        .string "3Foo"
```

当一个类声明或者继承了一个虚函数时，在编译期编译器首先会为这个类完成如下行为：

- 一个 virtual function table 也就是上述汇编中的 vtable for Foo 会被编译器产生，内部放置虚函数的地址；
- 在每一个类对象中，一个虚表指针（vptr）会被编译器合成出来，这个指针将来会被设定为虚表 vtable for Foo 的地址。

因此，编译器就需要为这个类合成一个默认构造函数，来完成虚表指针指向虚表的设置。

**4、“带有一个虚基类”的 Class**

当一个类派生自一个继承链，其中有一个或者多个虚基类时，也会为这个派生类合成一个默认构造函数。**即使虚基类中并没有虚函数**。以下面的代码片段为例：

```cpp
struct Foo{ int value;};
struct FooX : virtual Foo{ int valueX;};
struct FooY : virtual Foo{ int valueY;};
struct Bar : FooX, FooY{ int type;};
void foo(){ Bar bar;}
```

看一下函数的汇编代码：

```cpp
Bar::Bar() [complete object constructor]:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     QWORD PTR [rbp-8], rdi
        mov     rax, QWORD PTR [rbp-8]
        mov     edx, OFFSET FLAT:VTT for Bar+8
        mov     rsi, rdx
        mov     rdi, rax
        ; 为FooX也生成了默认构造函数FooX::FooX()
        call    FooX::FooX() [base object constructor]
        mov     rax, QWORD PTR [rbp-8]
        add     rax, 16
        mov     edx, OFFSET FLAT:VTT for Bar+16
        mov     rsi, rdx
        mov     rdi, rax
        ; 为FooY也生成了默认构造函数FooY::FooY()
        call    FooY::FooY() [base object constructor]
        mov     edx, OFFSET FLAT:vtable for Bar+24
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax], rdx
        mov     edx, OFFSET FLAT:vtable for Bar+48
        mov     rax, QWORD PTR [rbp-8]
        mov     QWORD PTR [rax+16], rdx
        nop
        leave
        ret
foo():
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        lea     rax, [rbp-48]
        mov     rdi, rax
        call    Bar::Bar() [complete object constructor]
        nop
        leave
        ret
```

通过 [C++底层原理探究（C++对象模型三）](http://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484854&idx=1&sn=342760686c0c071c8ee6c62eb75842a8&chksm=ead01410dda79d06b0aa5aca32eac0daa85e61cc338fc8829da55b2e747ab928a4e41229d621&scene=21#wechat_redirect) 文章的大致分析，当使用 `Foo` 类型的指针获取 `Foo::value` 时，由于编译器在编译期无法固定住 `Foo::value` 在派生类中的实际偏移位置，只能延迟到执行期再来决定。因此就借助虚表来查找派生类中的实际偏移。

**总结**

只有上述四种情况编译器才会合成一个 implicit non trivial default constructors，在合成的 default constructors 中，只有 base class sub object 和member class object 会被初始化，其他所有的 non static data member（如整数、指针、数组等）均不会被初始化。

以下为两个常见的误解：

- 任何 class 如果没有定义default constructor，就会被合成一个出来；
- 编译器合成的 default constructor 会显示设定“class 内每一个 data  member 的默认值”。

### 参考资料

[1] [C++之类的那些事儿（一）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483782&idx=1&sn=3c4fb74a8c77511dfec202017dcfeacc&chksm=ead01020dda799364a206c51f1c1600f568d1d01743b1462d4dab3d6eb79d77ddded967acccf&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[2] [C++之类的那些事儿（二）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483809&idx=1&sn=144348310332b6a5de32155a7b4318a6&chksm=ead01007dda79911218cc31d467f41635e28f909e22778e83b490cc842a476151ceeb6713c95&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[3] [C++之类的那些事儿（三）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483815&idx=1&sn=5caf0d4e74d480549d942bb853d7a373&chksm=ead01001dda799172b440c81e00337e779cea703a6b77c57d22e858a6f62a3d0aca9847ae499&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[4] [类的那些事之动态绑定的本质](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484106&idx=1&sn=ff95f0fb225e4e21c98937317fde59b6&chksm=ead0136cdda79a7abf0435db14d773a837505de05b9387f0a6aedd58bec7b5d875b6d2dd25d4&cur_album_id=1428444313188663297&scene=190#rd)

[5] [手撕pubsub机制](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483752&idx=1&sn=ee0cab1b81a3fe9132ae4bd53a08acc0&chksm=ead010cedda799d8a50d656b848e1e6cd0881ab09bd96df7666c7b7dc02c5e6951bef85c7b45&scene=21#wechat_redirect)

[6] [Protocol Buffer之解读](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483707&idx=1&sn=4a41073039fe8b042c6e121f2e5dc761&chksm=ead0109ddda7998b455c5ef7b79f1851322f532fa19f16625679db38d8ea76ef83438b1c9e05&cur_album_id=1428455568251125761&scene=189#wechat_redirect)

[7] [C++底层原理探究（工具篇）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484775&idx=1&sn=76334db946af962098a8f6812e15b30c&chksm=ead014c1dda79dd78db370b370678c0c33ebfe61b5da7b3c9be4ea6117e8d2ba8c6dcf9fac49&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[8] [C++底层原理探究（C++对象模型一）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484793&idx=1&sn=2426cf693ca1b00ace99888e08939b7e&chksm=ead014dfdda79dc9a800e1de2404d94914b6f11d14739877c0a6c3197a16a0b196817b69e6a5&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[9] [C++底层原理探究（C++对象模型二）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484831&idx=1&sn=e6390784fa4aa2e6c0fb82063d8c6536&chksm=ead01439dda79d2feeee684026bb773c68460d74b3f447a0b13289f1066b5c69cfb4479a2ae7&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[10] [C++底层原理探究（C++对象模型三）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484854&idx=1&sn=342760686c0c071c8ee6c62eb75842a8&chksm=ead01410dda79d06b0aa5aca32eac0daa85e61cc338fc8829da55b2e747ab928a4e41229d621&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

[11] [C++底层原理探究（构造函数语义学）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484908&idx=1&sn=daecfdb953e3024ee9390c1876952d9d&chksm=ead0144adda79d5c5200fbcaba90b6da39fdf0ac9ce71ea035d371f6bfb73aa7332926f7751f&scene=178&cur_album_id=1428444313188663297&search_click_id=#rd)

## 