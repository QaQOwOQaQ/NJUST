# [Google Test](https://google.github.io/googletest/)

**Google Test (通常简称为 gtest)** 是 Google 开发的一个开源、跨平台的 C++ 单元测试框架。<font color=blue>它基于 xUnit 架构</font>，是 C++ 生态系统中最流行和广泛使用的测试框架之一。它不仅适用于简单的单元测试，也支持复杂的集成测试。

Google Test 的设计目标是让编写测试变得**简单、可靠且独立**。

- **独立性与隔离性：** 每个测试用例都应该独立运行，不应该依赖于其他测试用例的执行结果。gtest 会为每个测试实例化一个新的对象，确保状态隔离。
- **可移植性：** 支持 Linux、Windows、macOS 等多种操作系统，且可以与多种构建工具（如 CMake、Bazel）无缝集成。
- **丰富的断言机制：** 提供了非常直观的断言宏来判断代码行为是否符合预期。

## xUnit 架构

**<font color=blue>xUnit 架构</font>** 是所有现代单元测试框架的通用架构模式统称，最早由 Kent Beck 为 Smalltalk 设计（SUnit），后来被移植到 Java（JUnit）、C#（NUnit）、Python（unittest/pytest）等几乎所有主流语言中。

它的核心机制可以拆解为 **<font color=ee11ff>4 个核心概念</font>** 和 **<font color=ee11ff>1 个执行生命周期</font>**：

**核心组件**

- **<font color=24b3aa>Test Suite（测试套件）</font>**：用例的集合，支持将多个测试用例或嵌套套件打包批量运行。
- **<font color=24b3aa>Test Case（测试用例）</font>**：最小的测试单元，包含具体的测试逻辑与断言（Assert）。
- **<font color=24b3aa>Test Fixture（测试上下文/夹具）</font>**：为用例准备的前置环境（Setup）和运行后的清理工作（Teardown），确保测试隔离、互不干扰。
- **<font color=24b3aa>Test Runner / Result（运行器与结果收集）</font>**：负责调度用例执行、捕获异常，并输出格式化结果（Pass/Fail 状态与耗时）。

**标准的 4 阶段执行生命周期：**

1. Setup（准备环境） 
2. Exercise（执行被测代码） 
3. Verify（断言校验） 
4. Teardown（清理现场）

这种模式规范了“环境准备—执行—校验—销毁”的流水线，使得不同语言的自动化测试代码具备了统一的组织结构与高复用性。

## GTest

### 1. 基本概念：Test Suite、Test Case

**Test (测试 / Test Case)**

- **是什么**：<font color=blur>它是最小的独立测试单元。</font>它的任务是验证你代码中的某一个非常具体的行为、分支或功能是否按预期工作。
- **代码体现**：在 gtest 中，它包含了具体的断言（如 `EXPECT_EQ`, `ASSERT_TRUE`）。如果一个 Test 内的任何断言失败，或者发生崩溃，这个 Test 就会被标记为失败。
- **在宏里的位置**：它是 `TEST()` 或 `TEST_F()` 宏的**第二个参数**。

**Test Suite (测试套件)**

- **是什么**：<font color=blur>它是一组**逻辑上相关的 Test 的集合**。</font>将相关的测试放在同一个 Test Suite 中，可以让测试报告结构更清晰，并且方便复用测试环境。
- **共享环境 (Fixtures)**：同一个 Test Suite 下的所有测试，通常会共享相同的数据配置（Setup）和清理逻辑（Teardown），这通常通过继承 `::testing::Test` 来实现（即 Test Fixture）。
- **在宏里的位置**：它是 `TEST()` 或 `TEST_F()` 宏的**第一个参数**。

看看这两个概念在代码中是如何体现的：

```cpp
#include <gtest/gtest.h>

// 这里的 "StringUtilsTest" 是 Test Suite (测试套件) 的名称
// 这里的 "IsEmpty" 和 "IsNotEmpty" 是具体的 Test (测试/测试用例) 名称

TEST(StringUtilsTest, IsEmpty) {
    EXPECT_TRUE(MyString("").empty());
}

TEST(StringUtilsTest, IsNotEmpty) {
    EXPECT_FALSE(MyString("Hello").empty());
}
```

在这个例子中，当你运行测试时，gtest 会报告 `StringUtilsTest` 这个 Test Suite 下跑了 2 个 Test，并分别告诉你它们的成功或失败状态。

```
Running main() from gmock_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from StringUtilsTest
[ RUN      ] StringUtilsTest.IsEmpty
[       OK ] StringUtilsTest.IsEmpty (0 ms)
[ RUN      ] StringUtilsTest.IsNotEmpty
[       OK ] StringUtilsTest.IsNotEmpty (0 ms)
[----------] 2 tests from StringUtilsTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

### 2. 断言：EXPECT 与 ASSERT 宏

Google Test 使用断言来检查代码的行为是否符合预期。断言是类似于函数调用的宏。gtest 提供了两种主要类型的断言：

- **`ASSERT_*` (致命断言):** <font color=blur>如果断言失败，它会**立即终止**当前所在的测试函数。</font>通常用于那些如果失败了，后续代码继续执行也没有意义（或会导致崩溃）的检查。
- **`EXPECT_*` (非致命断言):** 如果断言失败，它会报告失败，但**允许当前测试函数继续执行**。这有助于在一个测试中发现并报告多个相关的错误。在大多数情况下，推荐使用 `EXPECT_*`。

下面罗列一些最常用的 EXPECT 宏，把前缀换成 ASSERT 也可以使用。完整列表见 [文档](http://google.github.io/googletest/reference/assertions.html)。

#### 2.1 一元 / 二元比较

- `EXPECT_TRUE(foo)`、`EXPECT_FALSE(foo)`：判断一个变量是否是 true 或 false。
- 二元比较：
  - `EXPECT_EQ(foo, bar)`：判断两个变量是否相等。
    - 只要重载了 `==` 运算符就可以，所以也可以判断两个 vector 是否相等。
  - `EXPECT_NE(foo, bar)`：判断 foo != bar。
  - `EXPECT_LT(foo, bar)`：foo < bar，less than。
  - `EXPECT_LE(foo, bar)`: foo ≤ bar，less or equal。
  - `EXPECT_GT(foo, bar)`：foo > bar，greater than。
  - `EXPECT_GE(foo, bar)`: foo ≥ bar，greater or equal。

#### 2.2 浮点数比较

浮点数（`float` / `double`）由于 IEEE 754 标准的二进制精度限制，运算时会产生微小的舍入误差，直接使用 `EXPECT_EQ`（底层为精确的 `==` 运算符）极易因精度偏差导致非预期的断言失败。

GoogleTest 针对浮点数提供了专门的断言宏：

- **`EXPECT_DOUBLE_EQ(val1, val2)` / `EXPECT_FLOAT_EQ(val1, val2)`** 基于 **4 个 ULP（Unit in the Last Place）** 范围进行相对误差校验，用于判断两个浮点数在精度极限内“几乎相等”。

- **`EXPECT_NEAR(val1, val2, abs_error)`** 判断两个数字的绝对值相差是否小于等于 abs_error。

  ```cpp
  double pi = 3.141592653589793238;
  double approx_pi = 3.14;
  EXPECT_NEAR(pi, approx_pi, 0.01);  // 检测两个 π 值，允许误差在 0.01 以内
  ```

#### 2.3 字符串比较

- `EXPECT_STREQ(foo, "bar")`：判断两个字符串是否相等。<font color=blur>这里比较的是 C 风格的字符串，即 `char*`。</font>如果某个对象是 `std::string`，需要调用其 `c_str()` 方法。如果两个对象都是 `std::string`，可以使用 `EXPECT_EQ`。

  ```C++
  std::string str = "hello";
  EXPECT_STREQ(str.c_str(), "hello");
  ```

- `EXPECT_STRNE`：不相等。

- `EXPECT_STRCASEEQ`：忽略大小写，是否相等。

- `EXPECT_STRCASENE`：忽略大小写，是否相等。

#### 2.4 其他

- `EXPECT_THROW`/`EXPECT_NO_THROW`：处理异常，不要自行 `try-catch`。

- `EXPECT_THAT`：这实际上是 GMock 提供的宏，需要和 [匹配器 Matcher](https://imageslr.com/2023/gtest.html#matcher) 配合使用，详见下文。这是**写出优雅单测的必备技能**。

- `EXPECT_CALL`：同样是 GMock 提供的宏，判断函数被调用的次数，详见下文。

- `EXPECT_PRED(func, arg1, arg2, ...)`：自定义一个返回 bool 的谓词，传给该谓词一系列参数，判断是否返回 true。如果失败，会依次打印传入的参数值。

  ```C++
  std::vector<int> vec = {1, 2, 3};
  EXPECT_PRED([](const std::vector<int>& v) { return v.size() == 3; }, vec);
  ```

#### 2.5 断言失败时输出自定义信息

默认当 EXPECT 或 ASSERT 失败时，GTest 会打印预期值和实际值：

```cpp
#include <gtest/gtest.h>
#include "string"

TEST(UtilsTest, IsNotEmpty) {
    EXPECT_TRUE(!std::string("Hello").empty());
}

TEST(UtilsTest, IsNotEqual) {
    EXPECT_NE(3, 3);
}


```

```C++
Running main() from gmock_main.cc
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from UtilsTest
[ RUN      ] UtilsTest.IsNotEmpty
[       OK ] UtilsTest.IsNotEmpty (0 ms)
[ RUN      ] UtilsTest.IsNotEqual
test.cpp:9: Failure
Expected: (3) != (3), actual: 3 vs 3

[  FAILED  ] UtilsTest.IsNotEqual (0 ms)
[----------] 2 tests from UtilsTest (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 1 test.
[  FAILED  ] 1 test, listed below:
[  FAILED  ] UtilsTest.IsNotEqual

 1 FAILED TEST
```

但有时候，这些信息不够定位具体的失败原因。可以像这样**输出自定义日志**，这些日志仅在 EXPECT 失败时才打印：

```C++
for (int i = 0; i < x.size(); i++) {
  EXPECT_EQ(x[i], y[i]) << "x and y differ at index " << i;
}
```

还可以**<font color=blur>在 TestFixture 中封装 debug 函数</font>**，输出更详细的信息。比如，被测对象中包含了一些位图 std::bitset。在 EXPECT 失败时打印位图信息，有助于排查单测失败的原因：

```C++
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <gtest/gtest.h>

bool validate(int ad, int pos) {
    return ad == pos; 
}

class BaseTest : public ::testing::Test {
protected:
    std::unordered_map<std::string, std::bitset<8>> bitset_maps;

    void SetUp() override {
        bitset_maps["feature_flags"] = std::bitset<8>("10101010");
        bitset_maps["user_permissions"] = std::bitset<8>("00001111");
    }
};

class BitsetTest : public BaseTest {
public:
    std::string debug_message() const {
        std::stringstream ss;
        ss << "\n--- Debug Context ---\n";
        for (const auto& [name, value] : bitset_maps) {
            ss << "bitset: name=" << name << " value=" << value << '\n';
        }
        return ss.str();
    }
};

TEST_F(BitsetTest, validate) {
    int ad = 1;
    int pos = 2; // 故意使校验不相等以触发失败

    // 当断言失败时，<< 后的 debug_message 会被打印出来
    EXPECT_TRUE(validate(ad, pos)) << debug_message();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

### 3. 基本测试宏：`TEST()`

对于不依赖任何共享状态或外部资源的简单测试，你可以使用 `TEST()` 宏。

**语法：**

```cpp
TEST(TestSuiteName, TestName) {
  // 测试体：包含 C++ 代码和 gtest 断言
}
```

- `TestSuiteName` (测试套件名称): 通常是你正在测试的类名或模块名。
- `TestName` (测试名称): 具体测试的场景或功能点。

**代码示例：**

```cpp
#include <gtest/gtest.h>

// 假设这是我们要测试的业务代码
int Multiply(int a, int b) { return a * b; }

// 编写测试
TEST(MathOperations, MultiplyPositiveNumbers) {
    EXPECT_EQ(Multiply(3, 4), 12);
    EXPECT_EQ(Multiply(2, 5), 10);
}

TEST(MathOperations, MultiplyByZero) {
    EXPECT_EQ(Multiply(5, 0), 0);
    EXPECT_EQ(Multiply(0, 0), 0);
}
```

### 3. 测试夹具 (Test Fixtures)：`TEST_F()`

当你有多个测试需要操作相同的数据、对象或需要执行相同的初始化/清理步骤时，你应该使用测试夹具（Test Fixtures）。这可以 **<font color=blue>避免代码重复</font>**。

**使用步骤：**

1. 创建一个继承自 `::testing::Test` 的类。
2. 在类中声明你想在测试中使用的对象。
3. （可选）重写 `SetUp()` 方法：在每个测试**开始前**执行（初始化资源）。
4. （可选）重写 `TearDown()` 方法：在每个测试**结束后**执行（清理资源）。
5. 使用 `TEST_F()` 宏代替 `TEST()`，第一个参数必须是你的**夹具类名**。

**代码示例：**

```cpp
#include <gtest/gtest.h>
#include <vector>

// 1. 定义夹具类
class VectorTest : public ::testing::Test {
protected:
    // 3. 初始化操作
    void SetUp() override {
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
    }

    // 4. 清理操作（这里不需要特殊清理，vector 会自动销毁，但演示用法）
    void TearDown() override {
        vec.clear();
    }

    // 2. 共享的数据
    std::vector<int> vec;
};

// 5. 使用 TEST_F 宏，第一个参数是夹具类名 VectorTest
TEST_F(VectorTest, SizeIsThreeInitially) {
    EXPECT_EQ(vec.size(), 3);
}

TEST_F(VectorTest, PopBackReducesSize) {
    vec.pop_back();
    EXPECT_EQ(vec.size(), 2);
    EXPECT_EQ(vec.back(), 2);
}
```

*<font color=blur>注意：对于 `TEST_F` 中的每一个测试，gtest 都会在运行时创建一个全新的 `VectorTest` 对象，调用 `SetUp()`，运行测试体，然后调用 `TearDown()` 并销毁该对象。测试之间是相互独立的。</font>*

对每个测试用例，执行流程严格为： `构造函数` → `SetUp()` → `测试用例主体` → `TearDown()` → `析构函数`。

### 4. 运行测试

通常，你需要一个包含 `main` 函数的入口点来运行所有的测试。<font color=blur>gtest 提供了一个默认的 `main` 函数实现，你可以直接链接它</font>，<font color=blur>或者你自己编写：</font>

```cpp
#include <gtest/gtest.h>

int main(int argc, char **argv) {
    // 初始化 gtest，解析命令行参数
    ::testing::InitGoogleTest(&argc, argv);
    // 运行所有注册的测试，并返回结果
    return RUN_ALL_TESTS();
}
```

<font color=blur>如果你觉得太麻烦，可以链接 `gtest_main` 库，这是 GTest 自带的一个预编译好的 `main` 函数库。你只需要在编译链接时多加一个参数（` -l gtest_main`），就可以彻底删掉代码里的 `main` 函数。</font>

### 5. 值参数化测试 

**解决什么问题：** <font color=blur>当您需要用多组不同的输入数据来测试同一个函数逻辑时（例如：测试一个判断素数的函数，需要传入正数、负数、边界值等），</font>传统的做法是写很多个 `TEST` 或 `TEST_F`，这会导致大量代码重复。值参数化测试（Value-Parameterized Tests）允许您 **<font color=blur>将数据和测试逻辑分离</font>**，即 <font color=blur>“数据驱动测试”</font>。

**如何使用：**

1. **定义夹具类：** 继承自 `::testing::TestWithParam<T>`（`T` 是参数的类型）。
2. **获取参数：** 在测试逻辑中使用 `GetParam()` 获取当前传入的参数。
3. **编写测试：** 使用 <font color=blue>`TEST_P` 宏</font>（P 代表 Parameterized）。
4. **注入数据：** 使用 `INSTANTIATE_TEST_SUITE_P` 宏将数据集与测试套件绑定。

**代码示例：**

```cpp
#include <gtest/gtest.h>

// 被测函数
bool IsEven(int n) { return n % 2 == 0; }

// 1. 继承 TestWithParam，指定参数类型为 int
class IsEvenTest : public ::testing::TestWithParam<int> {};

// 2 & 3. 使用 TEST_P 编写测试逻辑
TEST_P(IsEvenTest, HandlesEvenNumbers) {
    int n = GetParam(); // 获取当前参数
    EXPECT_TRUE(IsEven(n));
}

// 4. 实例化测试套件，传入数据集：2, 4, 6, 8
INSTANTIATE_TEST_SUITE_P(
    TrueReturnCases, // 实例化前缀名，随意取
    IsEvenTest,      // 测试套件类名
    ::testing::Values(2, 4, 6, 8) // 数据生成器
);
```

*运行时，gtest 会自动将上述代码展开为 4 个独立的测试用例。*

> 疑问：为什么实例化测试套件时，不把数据传给测试名称 HandlesEvenNumbers，而是传给 IsEvenTest。
>
> **在 Google Test 的设计中，<font color=blue>数据是绑定给“测试夹具类”（也就是 `IsEvenTest`）的</font>，而不是绑定给某个具体的“测试逻辑”（如 `HandlesEvenNumbers`）的。**
>
> 之所以这样设计，主要是出于**数据集复用**和**面向对象架构**的考量。以下为您详细拆解其中的原因：
>
> ### 1. 核心原因：为了“一对多”的数据复用
>
> 假设 gtest 允许你把数据直接传给 `HandlesEvenNumbers`，那么这批数据（2, 4, 6, 8）就只能给这一个测试逻辑使用了。
>
> 但真实场景中，我们往往需要**用同一批数据，去跑好几个不同的测试逻辑**。把数据绑定在类（`IsEvenTest`）上，就意味着**所有挂载在这个类名下的 `TEST_P`，都会自动遍历这批数据。**
>
> **我们来看一个扩充后的例子：**
>
> ```cpp
> // 依然是那个参数化的夹具类
> class IsEvenTest : public ::testing::TestWithParam<int> {};
> 
> // 测试逻辑 1：测试它们是不是偶数 (你写的那个)
> TEST_P(IsEvenTest, HandlesEvenNumbers) {
> int n = GetParam(); 
> EXPECT_TRUE(IsEven(n));
> }
> 
> // 测试逻辑 2：测试它们是不是都大于 0 (新增的测试逻辑)
> TEST_P(IsEvenTest, IsPositive) {
> int n = GetParam(); 
> EXPECT_GT(n, 0); // 断言 n > 0
> }
> 
> // 注入数据：依然只需要写这一句！
> INSTANTIATE_TEST_SUITE_P(
> TrueReturnCases, 
> IsEvenTest,      // 数据交给了类
> ::testing::Values(2, 4, 6, 8) 
> );
> ```
>
> **发生了什么？** 因为数据是传给 `IsEvenTest` 类的，所以 gtest 会自动拿着 `(2, 4, 6, 8)` 这四个数，分别去跑 `HandlesEvenNumbers` 和 `IsPositive`。 框架会自动为你生成并执行 **8 个**测试用例（2 个逻辑 × 4 个数据）。如果你把数据传给具体的函数，那你每次写一个新的测试逻辑，都要重新写一遍 `INSTANTIATE_TEST_SUITE_P`，代码会非常冗余。
>
> ------
>
> ### 2. 底层机制：`GetParam()` 到底是谁的方法？
>
> 回看我们定义的类：
>
> ```cpp
> class IsEvenTest : public ::testing::TestWithParam<int> {};
> ```
>
> 注意，它继承自 `::testing::TestWithParam<int>`。
>
> <font color=blur>在 gtest 的底层源码中，`GetParam()` **并不是一个全局函数，它是 `TestWithParam` 这个父类里的一个成员方法。**</font>
>
> <font color=blur>当你写下 `TEST_P(IsEvenTest, HandlesEvenNumbers)` 时，gtest 在后台实际上是动态生成了一个继承自 `IsEvenTest` 的子类。</font> 因此，`HandlesEvenNumbers` 内部之所以能调用 `GetParam()`，是因为它处在这个类的作用域内。既然拿参数的方法是属于类的，那么参数自然也要传给类。
>
> ### 总结打个比方
>
> - **`IsEvenTest` (测试套件/类)** 就像是一条**流水线**。
> - **`INSTANTIATE_TEST_SUITE_P`** 就像是流水线的**进料口**，你把原材料 `(2, 4, 6, 8)` 倒进了 `IsEvenTest` 这条流水线。
> - **`TEST_P(IsEvenTest, ...)`** 就像是流水线上的一个个**质检工人**（比如 `HandlesEvenNumbers` 工人负责查偶数，`IsPositive` 工人负责查正负）。原材料顺着流水线往下走，每个工人都会对每一件原材料检查一遍。
>
> 这就是为什么数据必须传给流水线（`IsEvenTest`），而不是单独传给某个工人（`HandlesEvenNumbers`）。
>
> 理解了这个机制后，您是否想看看如果此时我还想测试一组**奇数**（让 `IsEven` 返回 false），应该如何优雅地组织代码，是新建一个类，还是复用当前的类？

### 6. 类型参数化测试 

**解决什么问题：** 与“值参数化”不同，<font color=blue>“类型参数化(Typed Tests)”用于测试**相同的逻辑在不同数据类型下的表现**。</font>这在测试 C++ 模板类或泛型接口时极其有用（例如：测试一个自定义的 `Stack` 容器，要确保它在 `int`、`double` 和 `std::string` 下都能正常工作）。

**如何使用：**

1. 定义一个模板夹具类。
2. 使用 `using MyTypes = ::testing::Types<Type1, Type2, ...>;` 定义需要测试的类型列表。
3. 使用 `TYPED_TEST_SUITE` 将类型列表绑定到模板夹具。
4. 使用 `TYPED_TEST` 编写测试。

```cpp
#include <iostream>
#include <type_traits>
#include <gtest/gtest.h>

// 1. 被测业务代码：通用的区间限制函数
template <typename T>
T clamp(T val, T low, T high) {
    if (val < low) return low;
    if (val > high) return high;
    return val;
}

// 2. 定义模板夹具类
template <typename T>
class MathUtilTest : public ::testing::Test {
protected:
    void SetUp() override {
        default_val = static_cast<T>(0);
    }

    T default_val; // 夹具内部成员变量
};

// 3. 注册要测试的类型列表
using MyTypes = ::testing::Types<int, float, double>;
TYPED_TEST_SUITE(MathUtilTest, MyTypes);

// 4. 用例 1：测试默认值初始化与 TypeParam 使用
TYPED_TEST(MathUtilTest, DefaultValueInitialization) {
    // TypeParam 即为当前特化的类型（int / float / double）
    TypeParam zero = 0;

    // 重点：访问模板夹具的成员变量必须加 this->
    EXPECT_EQ(this->default_val, zero);
}

// 5. 用例 2：测试通用逻辑（结合类型特征处理浮点/整型比较）
TYPED_TEST(MathUtilTest, ClampFunctionality) {
    TypeParam low = static_cast<TypeParam>(10);
    TypeParam high = static_cast<TypeParam>(20);

    TypeParam result1 = clamp(static_cast<TypeParam>(5), low, high);
    TypeParam result2 = clamp(static_cast<TypeParam>(15), low, high);
    TypeParam result3 = clamp(static_cast<TypeParam>(25), low, high);

    // 针对整型和浮点型做精确/容差断言
    if constexpr (std::is_floating_point_v<TypeParam>) {
        EXPECT_DOUBLE_EQ(result1, 10.0);
        EXPECT_DOUBLE_EQ(result2, 15.0);
        EXPECT_DOUBLE_EQ(result3, 20.0);
    } else {
        EXPECT_EQ(result1, 10);
        EXPECT_EQ(result2, 15);
        EXPECT_EQ(result3, 20);
    }
}

// 6. 入口函数
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

------

### 7. 死亡测试 (Death Tests)

**解决什么问题：** 常规的测试用于验证程序走在“正确”的道路上。但有时我们需要<font color=blue>验证程序**在遇到致命错误时，是否以预期的方式崩溃或退出**。</font>例如：非法内存访问、`assert` 触发、或者显式调用 `abort()`。

**工作原理：** gtest 会 `fork()` 出一个子进程来执行死亡测试的代码。主进程会监控子进程的退出状态和标准错误输出 (stderr)，以此判断其是否按照预期“死亡”。

**如何使用：** <font color=blur>主要使用 `ASSERT_DEATH(statement, regex)` 宏。它检查 `statement` 是否会导致进程崩溃，并且崩溃前输出的错误信息是否匹配正则表达式 `regex`。</font>

**代码示例：**

```cpp
void Divide(int a, int b) {
    // 如果 b 为 0，程序直接终止并输出错误信息
    assert(b != 0 && "Division by zero is not allowed");
}

// 死亡测试的套件名称习惯上以 DeathTest 结尾
TEST(MathDeathTest, DivideByZeroCrashes) {
    // 测试：当传入 0 时，期望程序崩溃，并输出包含 "Division by zero" 的信息
    ASSERT_DEATH(Divide(10, 0), "Division by zero");
}
```

## GMock

GMock 是 **Google Mock**，它是 GoogleTest（GTest）配套的 **C++ Mock 框架**。

简单说：

> **GTest 负责“断言结果对不对”，GMock 负责“伪造依赖，并检查调用行为对不对”。**

比如被测代码依赖 Redis、HTTP 服务、数据库、文件系统，或者另一个复杂函数。单测不应该真的去访问这些外部依赖，这时就可以用 GMock 把它们替换成“假的实现”。

例如原代码依赖一个接口：

```cpp
class UserService {
public:
    virtual ~UserService() = default;
    virtual int get_user_level(int user_id) = 0;
};
```

业务类：

```cpp
class PriceCalculator {
public:
    explicit PriceCalculator(UserService* user_service)
        : user_service_(user_service) {}

    int calculate(int user_id, int price) {
        int level = user_service_->get_user_level(user_id);

        if (level >= 10) {
            return price * 80 / 100;
        }

        return price;
    }

private:
    UserService* user_service_;
};
```

如果不用 Mock，你为了测试 VIP 分支，可能要真的构造用户数据，甚至访问数据库：

```text
PriceCalculator
      ↓
UserService
      ↓
Redis / DB / RPC
```

这就会让单测依赖外部环境。

GMock 的做法是：直接造一个假的 `UserService`：

```cpp
#include <gmock/gmock.h>

class MockUserService : public UserService {
public:
    MOCK_METHOD(int, get_user_level, (int user_id), (override));
};
```

然后在单测里规定：

```cpp
TEST(PriceCalculatorTest, CalculateForVipUser) {
    MockUserService user_service;

    EXPECT_CALL(user_service, get_user_level(123))
        .WillOnce(::testing::Return(10));

    PriceCalculator calculator(&user_service);

    EXPECT_EQ(calculator.calculate(123, 100), 80);
}
```

这里 GMock 做了两件事。

第一件事，是**控制外部函数返回什么**：

```cpp
.WillOnce(::testing::Return(10));
```

意思是：

```cpp
get_user_level(123)
```

不要真的查询任何东西，我规定它返回：

```cpp
10
```

于是测试可以精准控制被测代码走：

```cpp
if (level >= 10)
```

这个分支。

第二件事，是**检查函数有没有按照预期被调用**：

```cpp
EXPECT_CALL(user_service, get_user_level(123))
```

这意味着不仅要求最终结果正确，还要求：

> `get_user_level()` 必须以参数 `123` 被调用。

如果业务代码不小心变成：

```cpp
get_user_level(456);
```

即使最后计算结果碰巧还是 80，GMock 也能发现调用行为不符合预期。

所以 GMock 特别适合验证这类逻辑：

```text
是否调用了某函数？
调用了几次？
传了什么参数？
调用顺序是否正确？
外部函数返回不同结果时，被测函数怎么处理？
```

常见语法有：

```cpp
EXPECT_CALL(mock, foo())
    .WillOnce(::testing::Return(1));
```

表示期望调用一次，并返回 1。

指定调用次数：

```cpp
EXPECT_CALL(mock, foo())
    .Times(2);
```

完全不能调用：

```cpp
EXPECT_CALL(mock, foo())
    .Times(0);
```

匹配任意参数：

```cpp
using ::testing::_;

EXPECT_CALL(mock, foo(_))
    .WillOnce(::testing::Return(1));
```

连续返回不同结果：

```cpp
EXPECT_CALL(mock, foo())
    .WillOnce(::testing::Return(1))
    .WillOnce(::testing::Return(2));
```

比如第一次调用返回 1，第二次返回 2。

还有一个经常和 `EXPECT_CALL` 混淆的东西：

```cpp
ON_CALL(mock, foo())
    .WillByDefault(::testing::Return(1));
```

它更偏向于：

> “如果调用这个函数，默认返回 1。”

而：

```cpp
EXPECT_CALL(...)
```

不仅定义行为，还明确表达：

> “我期望它被这样调用。”

所以在单测里，如果“这个调用本身就是业务逻辑的一部分”，通常应该优先用 `EXPECT_CALL`。

### 1. 原理与示例

GMock 是 Google Test 提供的一个 C++ mocking 框架，可以用于创建虚拟的对象和方法。<font color=blur>**GMock 的原理是利用 C++ 的多态特性，覆盖 virtual 函数，将函数调用转发到相应的 mock 函数中。**</font>

<font color=24b3aa>**解决什么问题：** 在进行真正的“单元”测试时，被测函数往往依赖于外部组件（如数据库连接、网络请求、文件系统）。为了保证测试的独立性和速度，我们不能在测试中连接真实的数据库。**Mock 对象（模拟对象）**可以用来替代这些外部依赖，伪造它们的行为。</font>

<font color=24b3aa>**工作原理与使用：** gmock 允许你定义一个接口的“假”实现，并**设定预期 (Expectations)**：比如“我预期这个假对象的 `FetchData` 方法会被调用 1 次，并且当它被调用时，请返回预设的固定字符串”。</font>

**<font color=blur>GMock 基本使用流程如下：</font>**

1. 继承被 mock 的类，定义一个新的 Mock 类
2. 使用 GMock 提供的 mock 宏，用于实现 Mock 类的方法
3. 通过上面的 Mock 类，创建一个模拟对象
4. 通过 EXPECT_CALL 宏，控制模拟方法的返回值

下面是一个非常经典的 gmock 使用场景：假设我们正在开发一个**购物车 (ShoppingCart)** 系统，在结账时，它需要依赖一个**支付网关 (PaymentGateway)** 来扣款。

在单元测试中，我们不想（也不能）真的去调用真实的支付接口（比如支付宝、微信的 API），因此我们需要用 gmock 来“模拟（Mock）”这个支付网关。

以下是完整的 C++ 示例代码：

```C++
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// ==========================================
// 1. 定义依赖的接口 (Interface)
// ==========================================
class PaymentGateway {
public:
    virtual ~PaymentGateway() = default;
    // 纯虚函数，真实的支付类会实现它
    virtual bool charge(int amount) = 0; 
};

// ==========================================
// 2. 待测试的类 (Class Under Test)
// ==========================================
class ShoppingCart {
private:
    PaymentGateway* gateway_; // 依赖支付网关
public:
    // 通过构造函数注入依赖 (Dependency Injection)
    ShoppingCart(PaymentGateway* gw) : gateway_(gw) {}
    
    // 结账功能
    bool checkout(int total_amount) {
        if (total_amount <= 0) return false;
        // 调用外部依赖
        return gateway_->charge(total_amount);
    }
};

// ==========================================
// 3. 使用 gmock 创建 Mock 类
// ==========================================
class MockPaymentGateway : public PaymentGateway {
public:
    // 使用宏 MOCK_METHOD 定义要模拟的函数
    // 语法: MOCK_METHOD(返回类型, 方法名, (参数类型列表), (修饰符));
    MOCK_METHOD(bool, charge, (int amount), (override));
};

// ==========================================
// 4. 编写测试用例
// ==========================================
using ::testing::Return;
using ::testing::_; // 通配符匹配

// 测试场景 1：当支付网关扣款成功时，结账应该成功
TEST(ShoppingCartTest, CheckoutSuccessWhenChargeSucceeds) {
    // a. 创建 Mock 对象
    MockPaymentGateway mock_gateway;
    
    // b. 设置期望 (Expectations)
    // 我们期望 mock_gateway 的 charge 方法被调用 1 次，且传入参数必须是 100
    // 并且我们让它这次调用返回 true
    EXPECT_CALL(mock_gateway, charge(100))
        .Times(1)
        .WillOnce(Return(true));
        
    // c. 实例化待测对象，注入 Mock
    ShoppingCart cart(&mock_gateway);
    
    // d. 执行测试并断言
    EXPECT_TRUE(cart.checkout(100));
}

// 测试场景 2：当支付网关扣款失败时，结账应该失败
TEST(ShoppingCartTest, CheckoutFailsWhenChargeFails) {
    MockPaymentGateway mock_gateway;
    
    // 设置期望：不管 charge 传入的参数是多少（使用下划线 _ 表示任意参数）
    // 都让它一直返回 false
    EXPECT_CALL(mock_gateway, charge(_))
        .WillRepeatedly(Return(false));
        
    ShoppingCart cart(&mock_gateway);
    
    // 尝试结账 50 元，预期应该返回 false
    EXPECT_FALSE(cart.checkout(50));
}
```

代码核心点解析

- **`MOCK_METHOD` 宏**：这是生成 mock 类的核心。在较新的 gmock 版本（1.10 及以后）中，你只需要写 `MOCK_METHOD(返回类型, 函数名, (参数...), (override))` 即可，不需要再像老版本那样区分 `MOCK_METHOD0`、`MOCK_METHOD1` 等等。
- **`EXPECT_CALL` 宏**：用于设置“你预期这个 mock 对象的哪个函数会被怎么调用”。这是 gmock 的灵魂。如果实际运行过程中，函数的调用次数或参数与你在 `EXPECT_CALL` 中设置的不符，测试就会直接判定为失败 (Fail)。
- **匹配器 (Matchers)**：比如代码中的 `100` 表示精确匹配参数 100；`_` 表示匹配任意参数。
- **动作 (Actions)**：比如 `.WillOnce(Return(true))` 告诉 mock 对象：“当这个函数被调用时，请返回 `true`”。

使用 GMock 有**两个前提**：(1) 被 Mock 的方法必须是虚函数；(2) 必须替换掉被 mock 的对象，将其赋值为 mock 对象。其**不足之处**：(1) 使用 GMock 时必须定义一个 Mock class；(2) 如果想 mock 非虚函数，需要变更函数签名，这可能不太安全；(3) 对于函数内部的局部变量，无法赋值，也就无法 mock。

### 2. `EXPCET_CALL`

语法：

```C++
EXPECT_CALL(mock_object, method(matchers))
    .Times(cardinality)
    .WillOnce(action)
    .WillRepeatedly(action);
```

比如下面代码的含义是：调用 `turtle` 对象的 `GetX(string)` 方法 5 次，每次传入的参数都是”hello”，第一次返回 100，第二次返回 150，之后几次返回 200：

```C++
using ::testing::Return;
...
EXPECT_CALL(turtle, GetX("hello"))
    .Times(5)
    .WillOnce(Return(100))
    .WillOnce(Return(150))
    .WillRepeatedly(Return(200));
```

#### 基数：判断函数调用次数

- `Times(n)`：调用 n 次
- `Times(0)`：不被调用
- `Times(AtLeast(n))`：至少被调用 n 次
- `WillOnce(action)`：被调用 1 次，执行自定义行为
- `WillRepeatedly(action)`：被调用任意次，执行自定义行为

#### Action：控制被调用时的行为

`Will` 开头的接口可以传入一个 Action 参数，设置 mock 函数被调用时的行为。常用的：

- `Return`：返回指定值。比如 `WillOnce(Return(100))`。

- `ReturnRef`、`ByRef`：`Return` 不支持返回引用类型的变量，需要用这两个宏。

- `SetArgReferee<n>(value)`：修改传入的第 n 个引用类型的参数的值，下标 n 从 0 开始。

  ```C++
  class MockGetter : public Getter {
      public:
      MOCK_METHOD(int, get, (const string&, string&));
  };
    
  TEST(MockGetter, SetArgRefereeTest) {
      const std::string key = "foo_key";
      std::string value;
    
      MockGetter getter;
      EXPECT_CALL(getter, get(key, _))
          .WillOnce(SetArgReferee<1>("bar_value"));
    
      getter.get(key ,value);
      EXPECT_EQ(sum, "bar_value");
  }
  ```

- `DoAll(action1, action2, ...)`：执行多个 Action，比如修改参数的值 + 设定返回值：

  ```C++
  EXPECT_CALL(calc, Add(_, _, _))
    .WillOnce(DoAll(SetArgReferee<2>(8), Return(true)));
  ```

- 直接传入一个 lambda 函数，或者 `Invoke(function)`：执行自定义的函数，比如：

  ```C++
  // 传入 lambda 函数
  EXPECT_CALL(calc, Add).WillOnce([](int a, int b)) {
      return a + b + 1;
  });
    
  // 传入函数指针
  int AddFunc(int a, int b) {
      return a + b + 1;
  }
  EXPECT_CALL(calc, Add(_, _)).WillOnce(Invoke(AddFunc));
    
  // 传入类方法
  class AddHelper {
      public:
      int Add(int a, int b) {
          return a + b + 1;
      }
  };
  AddHelper helper;
  EXPECT_CALL(calc, Add(_, _)).WillOnce(Invoke(&helper, &AddHelper::Add));
  ```

  - Lambda 函数的签名必须和被 Mock 的函数一致。
  - Invoke 函数可接受任何可调用对象作为参数，包括函数指针、函数对象、Lambda 表达式等

#### Matcher：匹配传给函数的参数

Matcher 能够实现在复杂场景下进行断言，可以让测试用例更加灵活和可读，是写出优雅单测的必备工具。

Matcher 提供了一系列常用的比较函数，例如 Eq、Ne、Lt、Gt、Le、Ge 等，可以满足不同类型变量的比较。

Matcher 有两个使用场景：

1. 和 EXPECT_CALL 配合使用，用于检查传递给函数的参数值是否符合预期

   ```C++
   // 期望第一个参数大于 2，第二个参数小于 6
   EXPECT_CALL(calc, Add(Gt(2), Le(6)));
   calc.Add(3, 5);  // 可以通过检测
   calc.Add(2, 7);  // 不能通过检测
   ```

2. 和 EXPECT_THAT 配合使用，用于检查某个变量的值是否符合预期

   ```C++
   // int_foo > 6
   EXPECT_THAT(int_foo, Gt(6));
      
   // 判断一个 vector 的元素值
   std::vector<int> result = {1, 2, 5};
   EXPECT_THAT(result, ElementsAre(1, 2, Gt(3)));
      
   // 判断一个 unordered_map 的元素值
   std::unordered_map<string, int> result = {{"idt_a", 1}, {"idt_b"， 2}};
   EXPECT_THAT(result, UnorderedElementsAre(Pair("idt_a", 1), Pair("idt_b", 2)));
      
   // 期望 foo 包含子串 "hello"
   EXPECT_THAT(foo, HasSubStr("hello"));  
   ```

##### 通配符：`_`，`A<type>`

`_` 可以匹配任意类型的任意变量。它位于 `::testing` 命名空间下。示例：

```C++
using namespace testing;
EXPECT_CALL(calc, Add(_, _)).Times(1);
EXPECT_CALL(calc, Add).Times(1);  // 省略参数列表，和上面等价
```

`A<type>()` 或者 `An<type>()` 匹配类型是 `type` 的任意变量。其应用场景主要是匹配重载函数。示例：

```C++
class Foo {
    void DoSomething(int a, int b);
    void DoSomething(int a, string b);
}

EXPECT_CALL(foo, DoSomething(_, A<int>()));  // 预期调用第一个函数
```

##### 常用匹配器

完整列表见 http://google.github.io/googletest/reference/matchers.html，下面罗列常用的匹配器：

- 一般比较

  - `value`：写出字面量的值，就是精确匹配，等价于 `Eq(value)`。

    ```C++
    EXPECT_CALL(foo, method(100)).Times(1); 
    EXPECT_CALL(foo, method(Eq(100))).Times(1);  // 和上面等价
    ```

  - `Ge(value)`、`Gt`、`Le`、`Lt`：>= (greater or equal)、> (greater)、<= (less or equal)、< (less)。

    ```C++
    EXPECT_THAT(int_foo, Gt(100));  // int_foo > 100
    EXPECT_THAT(int_foo, Le(200));  // int_foo <= 200
    ```

  - `Ne(value)`：不等于，not equal。

  - `IsFalse()`、`IsTrue()`：转成 bool 值后是 false 或 true。非 0 值、非空指针等都可以视为 true。

  - `IsNull()`、`NotNull()`：指针是否为空。

- 浮点数比较

  - `DoubleEq(a_double)`、`FloatEq(a_float)`：浮点数相等。

    ```C++
    double foo = 0.01 + 0.02;
    EXPECT_THAT(foo, Eq(0.03));  // 会失败
    EXPECT_THAT(foo, DoubleEq(0.03));  // 会成功
    ```

  - `DoubleNear(a_double, max_abs_error)`：浮点数近似，差值的绝对值小于给定的 `abs_error`。

    ```C++
    double foo = 0.03 + 0.001;
    EXPECT_THAT(foo, DoubleNear(0.01));   // 会失败
    EXPECT_THAT(foo, DoubleNear(0.001));  // 会成功
    ```

  - `FloatNear(a_float, max_abs_error)`：同上。

- 字符串比较

  - `StartsWith(prefix)`：指定前缀

  - `EndsWith(suffix)`：指定后缀

  - `HasSubstr(string)`：包含子串

    ```C++
    std::string str = "hello, world";
    EXPECT_THAT(str, StartsWith("hello"));
    EXPECT_THAT(str, EndsWith("world"));
    EXPECT_THAT(str, HasSubstr("llo"));
    ```

  - `IsEmpty()`：字符串为空

  - `StrEq(string)`、`StrNe(string)`：字符串相等或不等

    ```C++
    EXPECT_CALL(m, foo(StrEq("hello, world")).Times(1);
    m.foo("hello, world");  // 符合预期
    ```

  - `StrCaseEq(string)`、`StrCaseNe(string)`：忽略大小写，字符串相等或不等

    ```C++
    EXPECT_CALL(m, foo(StrCaseEq("hello, world")).Times(1);
    m.foo("HELLO, WORLD");  // 符合预期
    ```

  - `ContainsRegex(string)`：正则表达式匹配

- 容器比较

  - `ElementsAre(e0, e1, ..., en)`：每个元素依次是什么，用于 vector、map、set 等。

    ```C++
    std:vector<int> v = {1, 2, 4};
    EXPECT_THAT(v, ElementsAre(1, 2, 4));  // 值是 {1, 2, 4}
    EXPECT_THAT(v, ElementsAre(1, 2, Gt(3)));  // 值是 {1, 2, 大于 3 的任意值}
        
    // 下面这样也可以，但不如上面只写一行优雅，不推荐
    vector<int> expect_vector = {1, 2, 4};
    EXPECT_EQ(v, expected_vector);
    ```

  - `UnorderedElementsAre(e0, e1, ..., en)`：同上，用于 unordered_set、unordered_map 等。

    ```C++
    std:set<int> v = {1, 2, 4};
    EXPECT_THAT(v, UnorderedElementsAre(1, 2, 4));  // 值包含 {1, 2, 4}
    EXPECT_THAT(v, UnorderedElementsAre(1, 2, Gt(3)));  // 值包含 {1, 2, 大于 3 的任意值}
    ```

  - `ContainerEq(container)`：效果同上，但会打印出哪些元素不一致。

  - `Contains(e)`：包含一个元素和 `e` 匹配，这里 `e` 可以是一个精确值，也可以是一个匹配器。

  - `Contains(e).Times(n)`：检测 `e` 指定的元素出现 n 次。`Times(0)` 表示不能包含这样的元素。

    ```C++
    EXPECT_THAT(v, Contains(5));  // 需要包含一个值为 5 的元素
    EXPECT_THAT(v, Contains(Lt(5)));  // 需要包含一个值 <5 的元素
    EXPECT_THAT(v, Contains(Lt(5)).Times(3));  // 需要包含 3 个值 <5 的元素
    ```

  - `Each(e)`：每个元素都要匹配 `e`。

    ```C++
    EXPECT_THAT(v, Each(Lt(5)));  // 每个元素都需要 <5
    EXPECT_THAT(v, Each(AllOf(Lt(5), Gt(3)));  // 每个元素都需要 >3 且 <5
    ```

  - `IsSubsetOf(array)`、`IsSubsetOf(begin, end)`：参数是指定数组的子集，顺序可以不一致。

- 成员匹配

  - `Field(&class::field, m)`：匹配字段值，用于结构体检测，比如：

    ```C++
    struct MyStruct {
        int value = 42;
        std::string greeting = "aloha";
    };
    MyStruct s;
    EXPECT_THAT(s, FieldsAre(42, "aloha"));
    ```

  - `Pair(m1, m2)`：匹配一个 `std::pair`，经常和 `ElementsAre` 配合使用，匹配一个 map：

    ```C++
    std::map m = {
        {"hello", 1},
        {"world", 2},
    };
    EXPECT_THAT(m, ElementsAre(Pair("hello", 1), Pair("world", 2)));
    ```

- 指针匹配

  - `Pointee`：匹配一个指针或 shared_ptr，常和 `Field` 一起检测某个指针的字段值：

    ```C++
    struct Item { int id; };
    std::shared_ptr<Item> obj = std::make_shared<Item>();
    obj->id = 1;
    EXPECT_THAT(obj, Pointee(Field(&Item::id, 1));
    ```

- 复合匹配

  - `AllOf(m1, m2, ..., mn)`：匹配所有给定的匹配器。

    ```C++
    std:vector<int> v = {4, 5};
    EXPECT_THAT(v, Each(AllOf(Le(5), Gt(3)));  // 每个元素都需要 >3 且 <=5
    ```

  - `AnyOf(m1, m2, ..., mn)`：匹配任何一个给定的匹配器。

    ```C++
    std:vector<int> v = {1, 2, 6, 7};
    EXPECT_THAT(v, Each(AnyOf(Lt(3), Gt(5)));  // 每个元素要么 <3，要么 >5
    ```

  - `Not(m)`：不匹配给定的匹配器，可以和 `AllOf`、`AnyOf` 配合使用。

    ```C++
    EXPECT_THAT(v, Each(AllOf(Gt(3), Lt(5)));  // 3 < each_item < 5
    EXPECT_THAT(v, Each(Not(AllOf(Gt(3), Lt(5))));  // each_item <= 3 || each_item >=5
    ```

  - `Conditional(cond, m1, m2)`：cond 为 true 时匹配 `m1`，否则匹配 `m2`。

    ```C++
    EXPECT_THAT(v, Conditional(is_ad, Gt(5), Lt(3));  // v = is_ad ? v > 5 : v < 3
    ```

##### 匹配器的优先级

在使用 GMock 的 EXPECT_CALL 宏进行 mock 函数参数匹配时，一次函数调用可能命中多个匹配器：

```C++
EXPECT_CALL(calc, add).Times(1);                // 任意参数
EXPECT_CALL(calc, add(_, _)).Times(1);          // 和上面等价
EXPECT_CALL(calc, add(3, 5)).Times(1);          // 字面量，精确匹配
EXPECT_CALL(calc, add(Gt(2), Lt(6))).Times(1);  // 比较，模糊匹配

calc.add(3, 5);  // 这一行理论上可以匹配上面每一个 EXPECT_CALL
```

匹配的优先级如下：**模糊匹配器 > 精确匹配器 >** **通配符**

- 模糊匹配器：Lt (小于)、Gt (大于) 等
- 精确匹配器：字面量、Eq (相等) 等
- 通配符：_ 等
- 当优先级相同时，越近的声明优先级越高。

这引入了一些使用技巧：

1. 只设置必要的匹配器。如果对某个参数的值不感兴趣，请写 `_` 作为参数，这意味着“一切皆有可能”。

   ```C++
   EXPECT_CALL(calc, add(5, _).Times(1);  // 如果只关心第一个参数的值，第二个参数就写成 _
   EXPECT_CALL(calc, add(5, 3).Times(1);  // 如果这样写，之后代码变动，单测可能就不通过了
   ```

2. 如果对所有参数的值都不感兴趣，可以**省略参数列表**，这和把每个参数都写成 `_` 是一致的。好处是后续改了函数签名后，比如新增了一个参数，单测是不需要改动的。

   ```C++
   EXPECT_CALL(calc, add).Times(1);       // 任意参数
   EXPECT_CALL(calc, add(_, _)).Times(1); // 和上面等价
   ```

3. 利用匹配器的优先级，可以细粒度地**控制函数在不同参数下的返回值**。比如 mock 一个 getter，我们希望在 key == `foo` 时返回 `bar`、key == `hello` 时返回 `world`，其他 key 通通返回空字符串，那么可以这样写：

   ```C++
   EXPECT_CALL(getter, get).WillRepeatedly(Return(""));
   EXPECT_CALL(getter, get("foo")).WillRepeatedly(Return("bar"));
   EXPECT_CALL(getter, get("hello")).WillRepeatedly(Return("world"));
      
   EXPECT_STREQ(getter.get("foo"), "bar");
   EXPECT_STREQ(getter.get("hello"), "world");
   EXPECT_STREQ(getter.get("aaa"), "");
   EXPECT_STREQ(getter.get("bbb"), "");
   ```

#### Uninteresting call：处理非预期调用

`非预期调用`是指未被 `EXPECT_CALL` 匹配的调用。当有非预期调用时，会有 warning 日志输出：

```plaintext
Uninteresting mock function call - returning default value.
    Function call: foo(42)
          Returns: 0
```

有两种处理方式。

##### NiceMock：不要输出 warning 信息

GMock 有三种级别：Nice Mock、Naggy Mock、Strict Mock。

默认是 Naggy Mock，当有非预期调用时，输出 warning 日志。

```plaintext
Uninteresting mock function call - returning default value.
    Function call: foo(42)
          Returns: 0
```

如果我们希望非预期调用不要有 warning，可以用 `NiceMock`。`NiceMock` 是一个模板类：

```C++
class MyMockClass : public MyClass {
    MOCK_METHOD(...)
};
MyMockClass mock;  // 这非预期调用会有 warning 日志
NiceMock<MyMockClass> mock; // 改成这样就不会有 warning 日志了
```

也可以在 Mock Class 定义的时候，直接继承 `NiceMock`：

```C++
class MyMockClass : public NiceMock<MyClass> {
    MOCK_METHOD(...)
};
MyMockClass mock;  // 这里非预期调用会返回默认值，不会有 warning 日志
```

Strict Mock 在有非预期调用时会直接 fail。也是一个模板类，使用方法和 `NiceMock` 类似。

##### 打印调用栈：检查非预期调用来自哪里

当有非预期调用时，如果我们希望检查非预期调用来自哪里，可以打印调用栈。有两种方式。

- 一种是通过 EXPECT_CALL 打印调用栈：

  ```C++
  #include <boost/stacktrace.hpp>
    
  void print_stack_trace() {
    std::cout << "call stack:" << std::endl;
    const auto frames = boost::stacktrace::stacktrace();
    for (const auto& frame : frames) {
      std::cout << "  " << frame << std::endl;
    }
  }
    
  EXPECT_CALL(...).WillRepeatedly([](){
    print_stack_trace();
    return xxx;  // 返回默认值
  });
  ```

- 另一种方式是使用 GTest 提供的选项 `--gmock_verbose=info`，该选项会打印每次 Mock Method 被调用时的参数和调用栈。需要在单测 main 函数执行 `::testing::InitGoogleMock(&`*`argc`*`, `*`argv)`**。*

### 3. `ON_CALL`

ON_CALL 可以和 EXPECT_CALL 配合使用。ON_CALL 设置函数的默认行为，EXPECT_CALL 临时修改其行为。

💡 ON_CALL 和 EXPECT_CALL 的语法很像，但提供了不同的语义。EXPECT_CALL 目的在于定义一个预期，即我们期望被测试函数在某些特定条件下应该调用哪些函数，如果没有满足预期的调用，则认为是一次失败。<font color=blue>ON_CALL 只是为了指定被测试函数的默认行为。</font>

ON_CALL 通常用在 **Mock 类的构造函数**、或者 TestFixture 的 **`SetUp`** **函数**里：

1. <font color=blur>令 mock 函数始终返回某个自定义的值</font>

2. <font color=blur>将 mock 函数的默认操作委托给基类或其他实例进行。</font>一个具体使用场景：希望 Mock 某个函数，默认还是执行原有操作，但当有需要的时候，可以临时更改其行为。这时就可以在 ON_CALL 里把默认操作委托给基类，后续再在 EXPECT_CALL 里临时控制其返回值。

   ```C++
   class MockFoo : public Foo {
   public:
       // Normal mock method definitions using gMock.
       MOCK_METHOD(char, DoThis, (int n), (override));
       MOCK_METHOD(void, DoThat, (const char* s, int* p), (override));
   
       // 构造函数里，委托 Mock 接口的操作给其他类
       MockFoo() {
           // 委托给基类
           ON_CALL(*this, DoThat).WillByDefault([this](const char* s, int* p) { Foo::DoThat(s, p); });
           // 委托给另一个对象
           ON_CALL(*this, DoThis).WillByDefault([this](int n) { return fake_.DoThis(n); });
       }
   
   private:
       FakeFoo fake_;  // Keeps an instance of the fake in the mock.
   };
   ```

## 单测编写规范

### 1. 命名规范

#### 1.1 目录结构 `[强制]`

假设源码目录是：

```
src/
  common/
    item_data.cpp
  frame/
    request_context.cpp
```

<font color=blue>那么对应的**单测目录**应该和源码目录保持相同层级：</font>

```
src/
  common/
    item_data.cpp
  frame/
    request_context.cpp
unittest/
  common/
    item_data_test.cpp
  frame/
    request_context_test.cpp
```

不推荐把所有单测直接平铺在 `unittest/` 下：

```
// bad
src/
  common/
    item_data.cpp
  frame/
    request_context.cpp
unittest/
  item_data_test.cpp
  request_context_test.cpp
```

原因很直接：项目规模小时可能没感觉，但源码一旦有几十个甚至几百个目录，`unittest/` 平铺就会越来越混乱。比如你看到：

```
unittest/
  config_test.cpp
  context_test.cpp
  parser_test.cpp
  item_test.cpp
```

你很难立刻判断这些文件分别对应源码中的：

```
src/common/parser.cpp
src/request/parser.cpp
src/config/parser.cpp
```

中的哪一个。

而如果单测结构和源码一一对应：

```
src/request/parser.cpp
unittest/request/parser_test.cpp
```

几乎不需要搜索就能定位。

例如：

```
src/auction/validator/frame/validator.cpp
```

对应：

```
unittest/auction/validator/frame/validator_test.cpp
```

这种方式还有一个很重要的好处：**避免重复写单测**。

开发者修改：

```
src/common/item_data.cpp
```

只要去看：

```
unittest/common/item_data_test.cpp
```

就能快速确认：

- 这个文件有没有单测；
- 已有单测覆盖了哪些函数；
- 新增测试应该放在哪里；
- 有没有别人已经写过类似 case。

而不是全仓库搜索一遍 `ItemData`。

#### 1.2 单测文件名 `[强制]`

<font color=blue>**单测文件名** = 源码文件名 + `_test`。</font>

文件名应该明确映射源码。

例如：

```
item_data.cpp
```

对应：

```
item_data_test.cpp
```

如果项目中头文件和实现文件是一组：

```
item_data.h
item_data.cpp
```

一般也仍然使用：

```
item_data_test.cpp
```

而不是：

```
test_item_data.cpp
item_data_unittest.cpp
item_data_ut.cpp
```

除非整个项目已经统一采用其他约定。

这里最重要的是**确定性**。看到源码文件名，开发者应该不需要猜，就能知道测试文件叫什么。

推荐形成固定映射：

```
foo.cpp      -> foo_test.cpp
foo.h        -> foo_test.cpp
bar.cpp      -> bar_test.cpp
```

这样搜索、构建规则、脚本处理都更简单。

#### 1.3 TestSuite、TestCase 命名规范 `[建议]`

<font color=blue>**TestSuite** 建议命名为被测试的类名加上 `Test` 后缀：</font>

```C++
// bad
TEST(MyTest, foo) {...}

// good
TEST(RequestContextTest, foo) {...}
```

好处很明显，就是去掉 Test 后缀，TestSuite 名和测试的类名一一对应。

<font color=blue>**TestCase** 建议命名为被测试的函数名，不要随意起名，也不需要增加不必要的前缀：</font>

```JSON
// bad
TEST(RequestContextTest, test_uav) {
    ASSERT_EQ(ctx->init_uav_to_group_bid(), 1);
}

// good
TEST(RequestContextTest, init_uav_to_group_bid) { // 不需要加 test_ 前缀
    ASSERT_EQ(ctx->init_uav_to_group_bid(), 1);
}
```

<font color=blue>TestCase 建议使用函数名并不是说任何情况下都必须只有函数名。</font>

<font color=blue>如果一个函数非常复杂：</font>

```cpp
int foo(Ad ad) {
    if (!ad) {
        return -1;
    }
    switch (ad.pricing) {
        case CPT:
            ...
        case GD:
            ...
    }
}
```

如果全部塞进：

```cpp
TEST(FooTest, foo) {
    // 200 行
}
```

可读性反而会很差。

这时可以把主要分支拆成独立 TestCase：

```cpp
TEST(FooTest, foo_when_ad_is_null) {
	// ...
}

TEST(FooTest, foo_when_pricing_is_cpt) {
	// ...
}

TEST(FooTest, foo_when_pricing_is_gd) {
	// ...
}
```

或者项目如果更倾向简洁风格：

```cpp
TEST(FooTest, foo_is_null) {
	// ...
}

TEST(FooTest, foo_is_cpt) {
	// ...
}

TEST(FooTest, foo_is_gd) {
	// ...
}
```

重点是 TestCase 名应该同时回答两个问题：

* 测哪个函数？
* 测这个函数的什么场景？

不要出现这种名字：

```cpp
TEST(FooTest, case1)
TEST(FooTest, case2)
TEST(FooTest, special)
TEST(FooTest, normal)
TEST(FooTest, test_new_logic)
```

因为几个月后几乎没人知道 `case2` 到底是什么。

### 2. 写有用的单测，而不只是通过单测覆盖率卡点

#### 2.1 禁止写无用单测 `[强制]`

经典问题：<font color=blue>“**假单测**”</font>。为了通过单测覆盖率卡点、便只是在单测里执行了一下新增函数，但**不检测其返回值，没有任何断言逻辑**。之前遇到过有同学写了几百行单测，reviewer 从头看到尾，居然一行 EXPECT 都没有，（╯‵□′）╯。

还有一种场景是<font color=blue>“**蹭单测**”</font>：新增了一个分支逻辑，引入了一坨逻辑，但只是在某个已有单测里，把这分支的控制参数打开了，完全没有自己构造输入去覆盖新增逻辑。这样即使覆盖率也能达标，也属于无用单测。

* **“蹭单测”**（可以理解为“搭便车”式的单元测试）是指：开发人员在代码中新增了一段逻辑（比如一个新的 `if` 分支），为了满足项目的**单测覆盖率（Coverage）要求**，不去专门为这个新逻辑编写独立的测试用例，而是直接跑到**已有的**某个老测试用例里，改一下输入参数（把新分支的开关打开），让代码在运行老单测时，**顺便（蹭）**跑一遍新写的代码。

  简单来说，就是**“只求代码被执行到，不求逻辑被真正校验”**。

* 为什么说这属于“无用单测”？

  这种做法虽然能骗过覆盖率统计工具，让覆盖率达标，但它存在几个致命问题：

  * **没有针对性的断言（Assertion）：** 原有的单测是为老逻辑写的，它的 `EXPECT` 或 `ASSERT` 也是为了校验老逻辑的结果。你只是把新分支的控制参数打开了，但并没有在单测结尾补充对新逻辑预期输出的校验。这相当于代码在“裸奔”，哪怕新逻辑里算错了值，只要没导致程序崩溃，单测依然会通过。
  * **没有覆盖边界情况：** 新增的“一坨逻辑”通常包含多种输入情况（合法、非法、边界值等）。“蹭”老单测通常只会提供一种极其普通的默认输入，完全无法测试新逻辑的鲁棒性。
  * **破坏了原有单测的纯粹性：** 原本的单测可能只是为了测试 A 场景，现在强行塞入了 B 场景的变量，会导致这个测试用例变得臃肿、意图不明。后续其他人维护时，会搞不清这个单测到底想校验什么。

* 举个直观的例子：

  假设你新增了一个**“VIP 打折”**的逻辑：

  **源码逻辑：**

  ```cpp
  int calculate_price(Order order) {
      int price = order.base_price;
  
      // 你新增的逻辑
      if (order.is_vip) { 
          price = price * 0.8; 
          // 假设这里还有一坨非常复杂的满减计算逻辑...
      }
  
      return price;
  }
  ```

  **“蹭单测”的做法（反面教材）：**

  ```cpp
  TEST(OrderTest, NormalPrice) {
      Order order;
      order.base_price = 100;
  
      // 你为了让覆盖率达标，强行在这里加了一行：
      order.is_vip = true; // 把新分支的开关打开了，蹭一下覆盖率
  
      // 但你没有修改或新增任何 EXPECT，只是确保程序跑完没报错
      // 甚至原来的 EXPECT 还是错的，或者只是个随意的检查
      EXPECT_TRUE(calculate_price(order) > 0); 
  }
  ```

  在这个例子中，VIP 打折的代码确实被执行了，覆盖率提升了，但由于没有断言去校验 `price` 是否真的变成了 80，这段新逻辑里哪怕写成了 `price = price * 0.9`，单测也能通过。这就叫“蹭单测”。

  **正确的做法：** 应该新开一个独立的 Test Case，专门自己构造针对这坨新逻辑的输入，并给出明确的断言：

  ```cpp
  TEST(OrderTest, VipDiscountCalculatedCorrectly) {
      Order order;
      order.base_price = 100;
      order.is_vip = true; // 专门为这个 case 构造输入
  
      // 明确校验新逻辑的预期结果
      EXPECT_EQ(calculate_price(order), 80); 
  }
  ```

#### 2.2 测试不符合预期的边界情况，而不是只测试符合预期的情况 `[建议]`

单测的目的之一在于测试程序的鲁棒性，即当输入不符合预期时，是否能正确处理。比如一个 `stoi` 函数 —— 将字符串转成整数。在构造输入时，最基本的是 `123` 这种合法字符串，此外还应当构造 `0.9999` (小数)、`123abc` (含非法字符) 等非法输入，以及 `1781234123412341234` 这种合法但越界的输入。

### 3. 代码风格与注释：写优雅的、可理解的、易于维护的单测

#### 3.1 不要用 `std::cout` 输出变量值，改为用 `ASSERT` / `EXPECT` 检查 `[强制]`

能用 EXPECT 就不要写 std::cout：

- 如果 cout 的日志是确定性的，那么应该写成断言。
- 如果是 debug 用的，那么在写完单测后应该删除。
- 如果期望单测失败时打印，那么应该放在 `EXPECT_CALL()... << ...` 后面，而不是直接输出。
- 除此之外，**这些日志没有任何意义**，只会刷屏，没有保留的必要。

```C++
// bad 
std::cout << "ads_size = " << rsp.ads.size() << std::endl; // 这一行多此一举
EXPECT_EQ(rsp.ads.size(), 1);

// good
EXPECT_EQ(rsp.size(), 1); // 这一行在检测失败时，会打印 rsp.size() 的值
EXPECT_EQ(rsp.size(), 1) << rsp.ads.debug_string() << std::endl;  // 可以在检测失败时，打印更多 debug 日志
```

#### 4.2 不要直接写数值，要写清楚这个数字是怎么算的 `[建议]`

直接写一个数字 `2965`，其他人并不知道这个数字是怎么算出来的，后续有问题也不好排查。

写出这个数字的计算过程，**映射到代码分支上**，其他人好看懂。这也是白盒化单测的表现之一。

```C++
// bad
params.alpha = 2;
params.beta = 2.5;
ASSERT_EQ(params.get_score(), 2965); // 这 2965 咋算的？

// good
params.alpha = 2;
params.beta = 2.5;
ASSERT_EQ(params.get_score(), 2 * 2.5 * 593); // alpha * beta * ctx.bid

// good: 把变量名直接注释在字面量后面
ASSERT_EQ(params.get_score(), 2 /* alpha */ * 2.5 /* beta */ * 593 /* ctx.bid */);
```

#### 4.3 使用大括号分隔、缩进不同的 Test Case `[建议]`

一个 `TEST(Foo, Bar){...}` 就是一个 Test Case。考虑到构造输入有成本，通常一个 `TEST(Foo, Bar)` 里会反复修改输入，构造多个 case，测试不同的执行流程。这里建议**用大括号分隔不同的 case**，整体更条理。另一个好处在于：**每个变量的生命周期仅限于大括号内**。这样就可以反复使用相同的变量名，而不用给变量名编号。

```C++
// bad
TEST(Foo, bar) {
    Context ctx1;
    params.enable_refresh = true;
    ASSERT_EQ(ctx1->is_enable_fresh(), true);
    
    Context ctx2;
    params.enable_refresh = false;
    ASSERT_EQ(ctx2->is_enable_fresh(), false);
}

// good
TEST(Foo, bar) {
    // case 1: enable = true
    {
        Context ctx;
        params.enable_refresh = true;
        ASSERT_EQ(ctx->is_enable_fresh(), true);
    }
    
    // case 2: enable = false
    {
        Context ctx;
        params.enable_refresh = false;
        ASSERT_EQ(ctx->is_enable_fresh(), false);
    }
}
```

此外，如果待测函数十分复杂，建议拆分多个 `TEST(Foo, Bar){...}`，避免 Test Case 代码膨胀。比如：

```C++
// 待测函数
int foo(Ad ad) {
    if (!ad)
        return -1;
    switch(ad.pricing) {
        case CPT:
            ...
        case GD:
            ...
    }
}
// 输入为空
TEST(Foo, IsNil) {
    ...
}

// 输入是 CPT 广告
TEST(Foo, IsCpt) {
    ...
}

// 输入是 GD 广告
TEST(Foo, IsGd) {
    ...
}
```

#### 4.4 正确使用 `ASSERT` 和 `EXPECT` 前缀 `[建议]`

- 前者在校验失败时会直接终止，后者则会继续运行。

- 如果某个判断不通过时会影响后续步骤 ，需要使用 `ASSERT`。常见的是空指针，或者数组访问越界。

- 如果某个 EXPECT 失败会导致后续一连串 EXPECT 失败，那么第一个 EXPECT 应该换成 ASSERT。这就像编译时的报错信息，往往只有第一个是有用的，其他错误都只是刷屏。

- 其他情况，可以使用 `EXPECT`，尽可能多测试几个用例。

- 此外，如果修改了某个字段的**目的是影响某个函数的返回值，那么最好补一行** **`ASSERT`**。好处显而易见：代码即注释；且在查单测 bug 的时候，这些断言能够预先排除一些问题。

  ```C++
  // bad
  req.type = Type::foo;  // 其他人看不懂这一行的目的是什么
  EXPECT_EQ(req.get_value(), 1);
    
  // good
  req.type = Type::foo;
  ASSERT_TRUE(context.is_foo());  // 这里表明，上一行是为了影响代码里这个判断函数的结果
  EXPECT_EQ(req.get_value(), 1);
  ```

#### 4.5 解除对外部逻辑的依赖 / 耦合 `[建议]`

- 如果被测代码里用到了某个全局变量：
  - Bad：从请求入口开始执行全部代码、间接构造该变量。这样太黑盒了。
  - Good：直接就地构造变量，然后赋值到全局字段上。
- 如果被测代码里调用了某个函数：
  - Bad：想办法构造外部函数的输入，以此来影响其返回结果。这样会导致被测函数与外部函数耦合 —— 需要看外部函数的实现逻辑，且如果后续外部函数改动了，当前函数的单测可能会不通过。
  - Good：使用 GMock 劫持该函数，在单测里控制其返回结果。完全不需要关心外部函数的实现。

#### 4.6 为单测补充详细的注释 `[建议]`

单测写出来必须的白盒的、可理解的、可维护的。如果不补充注释，其他人根本看不懂这些单测在测试什么逻辑，也无法确保其有效，后续修单测也很痛苦。

为单测补充注释时，重点要说明「这些赋值对应了哪个分支条件」，目标是让其他人扫一眼源码就能知道这些单测在测试哪些逻辑。

```C++
// bad
req.type = Type::foo;
req.from = "localhost";
EXPECT_EQ(ctx.get_value(), 5);

// good：补充注释
req.type = Type::foo;  // is_foo()
req.from = "localhost";  // is_local_req()
EXPECT_EQ(ctx.get_value(), 5);  // 本地请求，默认值是 5

// best：代码即注释
req.type = Type::foo;
ASSERT_TRUE(ctx->is_foo());
req.from = "localhost";
ASSERT_TRUE(ctx->is_local_req());
EXPECT_EQ(ctx.get_value(), 5);  // 本地请求，默认值是 5
```

### 4. 写稳定的单测

#### 4.1 Mock 所有 IO，不要依赖外部数据 `[强制]`

单测里**禁止访问外部服务**，最好是整个单测能够断网。

之前遇到的实际 case：

- 单测依赖线上服务，导致必须在一台线上环境的容器里才能启动单测。
- 单测依赖了线上 redis 里的测试数据，过了半年后数据过期了，线上单测突然挂了。

## TIPS

### 1. 访问私有变量

错误的做法：`#define private public`，或者定义 getter 函数。前者可能导致编译报错，后者需要修改代码。

<font color=blue>正确的做法：**`-fno-access-control`**，放在单测的 optimize 参数里。</font>

在 C++ 开发中，`-fno-access-control` 是一个由 GCC 和 Clang 编译器提供的“黑科技”参数。<font color=24b3aa>它的核心作用是：**在编译阶段禁用 C++ 的访问控制检查（即忽略 `public`、`protected` 和 `private` 关键字的限制）**。</font>

这个参数在编写单元测试时非常强大，因为它允许测试代码直接访问和修改被测类的私有成员，而无需修改源码。

通常情况下，C++ 编译器会严格执行封装性。如果你在类外尝试访问私有变量，编译器会报错： `error: 'int MyClass::m_private_var' is private within this context`

当你在编译命令中加入 `-fno-access-control` 时，编译器会“睁一只眼闭一只眼”。<font color=24b3aa>它依然知道哪些是私有的，但在生成目标文件时，它**不再拦截**对这些成员的非法访问。</font>

假设你有如下源码 `item_data.h`：

```cpp
class ItemData {
private:
    int _score = 0;
    void _update_score(int s) { _score = s; }
};
```

在单测文件 `item_data_test.cpp` 中，你可以直接操作：

```cpp
TEST(ItemDataTest, private_access) {
    ItemData item;
    item._score = 100;         // 直接访问私有变量
    item._update_score(200);   // 直接调用私有函数
    EXPECT_EQ(item._score, 200);
}
```

**编译命令：**

```bash
g++ -fno-access-control \
	item_data_test.cpp -lgtest -lpthread -o unit_test
```

### 2. 修改 Const 字段

 `const_cast<Type&>` 修改常量类型。

### 3. 优化级别改为 O0

好处：单测覆盖率报告更准。

### 4. 输出日志

`std::cout` 输出的日志会直接展示在终端。

建议：**能用 EXPECT 就不要写 `std::cout`**

- 如果 cout 的日志是确定性的，那么应该写成断言。
- 如果是 debug 用的，那么在写完单测后应该删除。
- <font color=24b3aa>如果期望单测失败时打印，那么应该放在 `EXPECT_CALL()... << ...` 后面，而不是直接输出。</font>
- <font color=24b3aa>除此之外，这些日志没有任何意义，只会刷屏，没有保留的必要。</font>

### 5. 使用 GDB 运行和调试程序

GDB 也是研发基本功之一。<font color=blue>使用 GDB 断点调试的效率远高于 `加日志+重新编译单测`，</font>但大部分人依然使用后面这种调试方式，原因可能是认为 GDB 的上手成本太高。但实际上，GDB 入门只需要 3 分钟。这里罗列 GDB 的基本使用姿势，足够覆盖大部分单测场景。上面高亮块里也提供了一个速查手册。

1. 进入 GDB，同时加载单测程序：`gdb ./path/to/unit_test`

2. 加载动态链接库：`set env LD_LIBRARY_PATH=...`

3. 运行单测：`r`。如果要运行指定单测，加 `--gtest_filter` 参数：`  r --gtest_filter=FooTest.bar_method`

4. 打断点：`b`。比如：

   ```Shell
   b 文件名:行号
   b prime/src/auction/validator/frame/validator.cpp:52
   ```

5. 从断点处继续运行：`c`

6. 逐行执行：`n`

7. 打印变量：`p 变量名`

8. 查看 core 栈：`bt`

### 6. 运行单测

> 注意这些参数是在运行可执行文件时添加的而不是编译时添加的。

#### 6.1 运行特定单测：`--gtest_filter`

什么时候需要运行特定单测：

- 运行所有单测，发现某个单测失败了。但这个时候单测日志已经刷屏，看不到这个单测的具体失败原因了。
- 修复单测 bug 后重新编译，只希望运行上次失败的那个单测。

语法：`--gtest_filter=TestSuite.TestCase`。**支持通配符 `\*` 和排除符 `-`**。

- `--gtest_filter=FooTest.Bar`，只运行 `FooTest.Bar`。
- `--gtest_filter=*FooTest*`，运行所有名称里包含 `FooTest` 的单测。
- `--gtest_filter=FooTest.*:BarTest.*`，运行 FooTest 和 BarTest 两个 suites 下的所有单测。
- `--gtest_filter=FooTest.*-FooTest.Bar`，运行 FooTest 下的所有单测，但不运行 FooTest.Bar。
- `--gtest_filter=FooTest.*:BarTest.*-FooTest.Bar:BarTest.Foo`，运行 FooTest 和 BarTest 下的所有单测，但不运行 FooTest.Bar 和 BarTest.Foo。
- 详细的匹配规则见[文档](https://github.com/google/googletest/blob/main/docs/advanced.md#running-a-subset-of-the-tests)。

#### 6.2 重复运行单测多次：`--gtest_repeat`、`--gtest_break_on_failure`

有些单元测试涉及到多线程，可能会偶发性的不通过。

可以使用 `--gtest_repeat=1`、`--gtest_break_on_failure`运行多次来复现。

#### 6.3 临时禁用某个单测：`DISABLED_`

可以使用 `DISABLED_` 前缀来跳过某项测试：

```C++
TEST_F(DISABLED_BarTest, DoesXyz) { ... }
TEST_F(BarTest, DISABLED_DoesXyz) { ... }
```

DISABLED 之后，单测日志会输出 DISABLED 的单测数量：

![img](https://imageslr.com/media/gtest/disable.png)

之后在修理单测过程中，可以使用 `--gtest_also_run_disabled_tests` 或者 `--gtest_filter` 来执行被 DISABLED 的单测。

相比于把整段单测代码全部注释掉，加一个 DISABLED_ 前缀的 diff 更少，而且后续可以直接运行。