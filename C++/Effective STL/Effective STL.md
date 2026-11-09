# 容器

## item1: 仔细选择你的容器

> 你需要插入和删除的事务性语义吗？也就是说，你需要有可靠地回退插入和删除的能力吗？如果是，你就需要使用基于节点的容器。如果你需要多元素插入（比如，以范围的方式——参见条款5）的事务性语义，你就应该选择 list，因为 **list 是唯一提供多元素插入事务性语义的标准容器。**事务性语义对于有兴趣写异常安全代码的程序员来说非常重要。（事务性语义也可以在连续内存容器上实现，但会有一个性能开销，而且代码不那么直观。要了解这方面的知识，请参考 Sutter的《Exceptional C++》的条款17[8] ）

### **1. 什么是事务性语义？**

- **定义**: 事务性语义指一组操作要么**全部成功**，要么**全部失败回滚**（类似数据库事务）。在 C++ 中，这通常与**异常安全**紧密相关。
- **场景**: 当插入或删除多个元素时，若中间操作抛出异常，容器应保持操作前的状态（强异常安全保证）。

------

### **2. 基于节点的容器 vs 连续内存容器**

#### **2.1 基于节点的容器（Node-based）**

- **代表容器**: `list`、`forward_list`、关联容器（`map/set`等）。
- **事务性优势**: 
  - **插入/删除单元素**: 天然具备事务性（操作仅影响节点指针，不会破坏其他元素）。
  - **多元素插入**: `list` 是**唯一**支持多元素插入（如范围插入）事务性语义的标准容器。
- **原理**: 节点独立分配内存，操作失败时仅需释放已分配的节点，不影响现有数据。

#### **2.2 连续内存容器（Contiguous-memory）**

- **代表容器**: `vector`、`deque`、`string`。
- **劣势**: 
  - 插入/删除可能触发内存重分配（如 `vector` 扩容），若抛出异常（如内存不足），部分元素可能已移动或丢失。
  - **多元素插入**: 需手动实现回滚逻辑（复杂且性能开销大）。

### 3.《Exceptional C++》条款 17 概述

条款 17 的标题通常被译为"异常安全的事务性语义"，主要讨论了如何在连续内存容器（如 `vector`、`deque`）中实现事务性语义，即保证操作要么完全成功，要么在失败时完全回滚，不留下部分完成的状态。

#### 关键内容

1. **连续内存容器的挑战**: 连续内存容器在插入或删除元素时，可能需要移动其他元素或重新分配内存，这使得异常安全性变得复杂。

2. **Copy-and-Swap 技术: **Sutter 详细介绍了"复制并交换"技术，这是实现事务性语义的一种常用方法: 
   1. 创建原容器的一个副本
   2. 在副本上执行所需操作
   3. 如果所有操作成功，则将副本与原容器交换
   4. 如果出现异常，原容器保持不变
   
3. **强异常安全保证**: 这种方法提供了"强异常安全保证"—如果操作失败，程序状态不变。

4. **性能考虑**: 条款还讨论了这种方法的性能开销，因为创建容器副本可能代价高昂，尤其是对于大型容器。

5. **替代方案**: 条款还探讨了一些可能的替代方案和优化，以在某些特定情况下减少性能开销。

#### 实际应用

条款 17 的内容对于需要编写高可靠性代码的程序员特别重要，例如金融系统、数据库或任何不能容忍部分失败的系统。

这个条款是理解现代 C++ 异常安全编程的基础之一，也是为什么在需要事务性语义时，基于节点的容器（如 `list`）可能更受青睐的原因，尽管在其他方面（如遍历性能）它们可能不如连续内存容器。

下面我将用代码示例来具体说明《Exceptional C++》条款 17 中讨论的在连续内存容器中实现事务性语义的方法，特别是使用**"复制并交换"（Copy-and-Swap）**技术。

##### 示例1：没有事务性语义的情况

首先，让我们看一个没有事务性语义的函数，它可能在异常发生时留下容器处于部分修改状态: 

```cpp
// 不安全的实现 - 没有事务性语义
void append_elements(std::vector<MyClass>& vec, const std::vector<MyClass>& new_elements) {
    // 这种实现如果在push_back过程中抛出异常，vec将处于部分修改状态
    for (const auto& elem : new_elements) {
        vec.push_back(elem); // 如果这里抛出异常（如内存分配失败或MyClass复制构造函数抛出），
                            // 那么vec中可能已经添加了一些元素但不是全部
    }
}
```

##### 示例2：使用 Copy-and-Swap 实现事务性语义

下面是使用"复制并交换"技术实现事务性语义的版本: 

```cpp
// 安全的实现 - 使用"复制并交换"提供事务性语义
void append_elements_safe(std::vector<MyClass>& vec, const std::vector<MyClass>& new_elements) {
    // 创建原容器的副本
    std::vector<MyClass> temp(vec);
    
    try {
        // 在副本上执行操作
        for (const auto& elem : new_elements) {
            temp.push_back(elem);
        }
        
        // 操作成功，交换原容器和副本
        vec.swap(temp);
        // temp现在包含旧数据，将在函数结束时销毁
    }
    catch (...) {
        // 如果发生异常，temp会被销毁，而vec保持不变
        // 重新抛出异常以便调用者知道操作失败
        throw;
    }
}
```

##### 示例3：使用 C++11 的移动语义优化

C++11 引入的移动语义可以优化上述实现: 

```cpp
// 使用C++11特性优化的事务性实现
void append_elements_modern(std::vector<MyClass>& vec, std::vector<MyClass> new_elements) { // 这里传入 new_elements 的拷贝
    // 注意: 新元素现在是按值传递，可以直接修改
    
    // 创建原容器的副本
    std::vector<MyClass> temp(vec);
    
    // 在副本上添加所有新元素（可以使用移动而不是复制）
    temp.insert(temp.end(),
        std::make_move_iterator(new_elements.begin()), 
        std::make_move_iterator(new_elements.end())
    );
    
    // 操作完成，交换原容器和副本
    vec.swap(temp);
}
```

##### 示例4：针对多个容器的事务性操作

当操作可能影响多个容器时，事务性语义变得更加重要: 

```cpp
// 对多个容器的事务性操作
bool transfer_elements(std::vector<MyClass>& source, std::vector<MyClass>& destination, size_t count) {
    // 如果source中元素不足，返回失败
    if (source.size() < count) return false;
    
    // 创建两个容器的副本
    std::vector<MyClass> temp_source(source);
    std::vector<MyClass> temp_dest(destination);
    
    try {
        // 在副本上执行操作
        for (size_t i = 0; i < count; ++i) {
            temp_dest.push_back(temp_source.back());
            temp_source.pop_back();
        }
        
        // 操作成功，交换容器
        source.swap(temp_source);
        destination.swap(temp_dest);
        return true;
    }
    catch (...) {
        // 操作失败，原容器保持不变
        return false;
    }
}
```

##### 示例5：使用 RAII 实现事务性语义

使用 RAII（资源获取即初始化）原则可以简化错误处理: 

```cpp
// 使用RAII进行事务
template <typename T>
class Transaction {
private:
    std::vector<T>& original_; // 原始数据的引用
    std::vector<T> working_copy_; // 拷贝
    bool committed_;

public:
    explicit Transaction(std::vector<T>& original)
        : original_(original), working_copy_(original), committed_(false) {}
    
    // 获取工作副本的引用以进行修改
    std::vector<T>& get() { return working_copy_; }
    
    // 提交事务
    void commit() {
        original_.swap(working_copy_);
        committed_ = true;
    }
    
    // 析构函数 - 如果没有提交，则自动回滚（不做任何事）
    ~Transaction() {
        // 如果没有提交，working_copy_会被销毁，original_保持不变
    }
};

// 使用Transaction类
void append_elements_transaction(std::vector<MyClass>& vec, const std::vector<MyClass>& new_elements) {
    Transaction<MyClass> trans(vec);
    
    // 在事务的工作副本上进行操作
    for (const auto& elem : new_elements) {
        trans.get().push_back(elem);
    }
    
    // 提交事务（如果在这之前抛出异常，事务会自动回滚）
    trans.commit();
}
```

这些示例展示了如何在连续内存容器（如`std::vector`）中实现事务性语义，确保操作要么完全成功，要么在失败时不改变原始容器。通过"复制并交换"技术，我们可以在连续内存容器中实现与基于节点的容器（如`std::list`）类似的事务性语义，尽管有一定的性能开销。

#### 替代方案和优化

Copy-and-Swap 技术虽然强大，但有显著的性能开销: 

- **内存消耗**: 需要临时复制整个容器
- **时间消耗**: 复制全部元素产生的额外 CPU 时间
- **对象构造开销**: 每个元素都需要调用复制构造函数

对于大型容器，开销尤其明显。因此我们应考虑性能和安全性的权衡: 

- 热点路径上的代码可能需要优化
- 不频繁执行的代码可优先考虑安全性
- 可以根据容器大小选择不同策略

Sutter 讨论了多种优化和替代方案: 

##### 1. 局部 Copy-and-Swap

只复制和交换受影响的部分，而不是整个容器: 

```cpp
// 示例: 只交换受影响的范围
void insert_optimized(std::vector<MyClass>& vec, size_t pos, const MyClass& obj) {
    if (pos == vec.size()) {
        // 尾部插入优化
        std::vector<MyClass> temp(1, obj);
        vec.insert(vec.end(), temp.begin(), temp.end());
    } else {
        // 中间插入 - 只复制后半部分
        std::vector<MyClass> back_half(vec.begin() + pos, vec.end());
        vec.resize(pos);  // 保留前半部分
        vec.push_back(obj);  // 添加新元素
        vec.insert(vec.end(), back_half.begin(), back_half.end());  // 重新添加后半部分
    }
}
```

##### 2. 预分配策略

预先分配足够空间，避免重新分配: 

```cpp
void add_elements_reserve(std::vector<MyClass>& vec, const std::vector<MyClass>& new_elements) {
    // 预先分配空间
    vec.reserve(vec.size() + new_elements.size());
    
    // 现在可以安全地一个一个添加，不会触发重新分配
    for (const auto& elem : new_elements) {
        vec.push_back(elem);  // 不会导致重新分配
    }
}
```

##### 3. 使用基于节点的容器

在某些情况下，切换到基于节点的容器可能更合适: 

```cpp
// 使用list代替vector获得内置的事务性语义
void process_data(const std::vector<MyClass>& input) {
    std::list<MyClass> working_set;  // 使用list进行中间处理
    
    try {
        // list的范围插入具有事务性语义
        working_set.insert(working_set.end(), input.begin(), input.end());
        
        // 处理数据...
        for (auto& item : working_set) {
            process(item);
        }
        
        // 完成后可以转回vector
        std::vector<MyClass> result(working_set.begin(), working_set.end());
        // 使用result...
    }
    catch (...) {
        // 异常处理
    }
}
```

##### 4. 延迟提交模式

使用"意图"数据结构推迟实际修改: 

```cpp
// 延迟提交示例
class VectorEditor {
private:
    std::vector<MyClass>& target_;
    std::vector<MyClass> working_copy_;
    
public:
    explicit VectorEditor(std::vector<MyClass>& target)
        : target_(target), working_copy_(target) {}
    
    // 提供各种编辑操作
    void add(const MyClass& obj) { working_copy_.push_back(obj); }
    void remove(size_t index) { 
        working_copy_.erase(working_copy_.begin() + index); 
    }
    
    // 只有在调用commit时才应用更改
    void commit() { target_.swap(working_copy_); }
    
    // 放弃更改
    void rollback() { working_copy_ = target_; }
};
```

这些替代方案和优化提供了在异常安全性和性能之间取得平衡的不同策略，可以根据具体应用场景选择合适的方法。

## item2: 小心对“容器无关代码”的幻想

不要执着于写出“容器无关”的代码。

## item3: 使容器里对象的拷贝操作轻量而正确

如果容器元素的拷贝成为性能瓶颈，或者希望正确的保存派生类元素，又或者该元素类型禁止拷贝操作等等，可以考虑将容器元素保存为指针，如果希望避免指针导致的忘记 free，double free 等问题，可以使用智能指针。

## item4: 用 empty 来代替检查 size() 是否为 0

对于所有的标准容器，empty 是一个常数时间的操作，但对于一些 list 实现，size 花费线性时间。

之所以这样是因为 splice() 可以接受一对迭代器，如果我们要维护 O(1) 时间复杂度的 size 操作，就需要计算此时迭代器中的元素个数，而这样做会导致 splice 的时间复杂度是 O(N) 的。

不过可能因为编译器的优化，测试中发现，size() 和 empy() 的性能是差不多的。

## item5: 尽量使用区间成员函数代替它们的单元素成员函数

效率、代码可读性、可维护性。

> ``` cpp
> copy(data, data + numValues, inserter(v, v.begin()));
> ```
>
> 内联也节省不了你的第二种税——无效率地把 v 中的现有元素移动到它们最终插入后的位置的开销。每次调用 `insert` 来增加一个新元素到 `v`，插入点以上的每个元素都必须向上移动一次来为新元素腾出空间。所以在位置 `p` 的元素必须向上移动到位置 `p+1` 等。在我们的例子中，我们在 `v` 的前部插入了 `numValues` 个元素。那意味着在 `v` 中每个插入之前的元素都得向上移动一共 `numValues` 个位置。但每次 `insert` 调用时每个只能向上移动一个位置，所以每个元素一共会被移动 `numValues` 次。如果 `v` 在插入前有 `n` 个元素，则一共会发生 `n * numValues` 次移动。在这个例子里，`v` 容纳 `int`，所以每次移动可能会归结为一次 `memmove` 调用，但如果 `v` 容纳了用户自定义类型比如 `Widget`，每次移动会导致调用那个类型的赋值操作符或者拷贝构造函数。（大部分是调用赋值操作符，但每次 `vector` 的最后一个元素被移动，那个移动会通过调用元素的拷贝构造函数来完成。）于是在一般情况下，把 `numValues` 个新对象每次一个地插入容纳了 `n` 个元素的 `vector<Widget>` 的前部需要花费 `n * numValues` 次函数调用: `(n-1) * numValues` 调用 `Widget` 赋值操作符和 `numValues` 调用 `Widget` 拷贝构造函数。即使这些调用内联了，你仍然做了移动 `numValues` 次 `v` 中元素的工作。
>
> 相反的是，标准要求区间 `insert` 函数直接把现有元素移动到它们最后的位置，也就是，开销是每个元素一次移动。总共开销是 `n` 次移动，`numValues` 次容器中的对象类型的拷贝构造函数，剩下的是类型的赋值操作符。相比单元素插入策略，区间 `insert` 少执行了 `n * (numValues - 1)` 次移动。花一分钟想想。这意味着如果 `numValues` 是 100，`insert` 的区间形式会比重复调用 `insert` 的单元素形式的代码少花费 99% 的移动！
>
> 在我转向单元素成员函数和它们的区间兄弟的第三个效率开销前，我有一个小修正。我在前面写的段落都是真理，而且除了真理没别的了，但并不是真理的全部。**仅当可以不用失去两个迭代器的位置就能决定它们之间的距离时，一个区间 `insert` 函数才能在一次移动中把一个元素移动到它的最终位置。**这几乎总是可能的，因为所有前向迭代器提供了这个功能，而且前向迭代器几乎到处都是。所有用于标准容器的迭代器都提供了前向迭代器的功能。非标准的散列容器（参见条款 25）的迭代器也是。在数组中表现为迭代器的指针也提供了这样的功能。事实上，唯一不提供前向迭代器能力的标准迭代器是输入和输出迭代器。因此，除了当传给 `insert` 区间形式的迭代器是输入迭代器（比如 `istream_iterator`——参见条款 6）外，我在上面写的东西都是真的。在那个唯一的情况下，区间插入必须每次一位地把元素移动到它们的最终位置，期望中的优点就消失了。（对于输出迭代器，这个问题不会发生，因为输出迭代器不能用于为 `insert` 指定一个区间。）

为什么当迭代器是输入迭代器时，期望中的优点（区间 insert 不会移动元素）会失效？

### 迭代器种类

**我们首先再回顾一下 5 大迭代器: **

1. **输入迭代器 (Input Iterator):** 只能单向遍历序列，并且每个迭代器位置***只能被读取一次***。你可以递增它 (`++it`)，解引用它 (`*it`) 来获取当前元素，并进行相等或不等比较 (`it1 == it2`, `it1 != it2`)。`std::istream_iterator` 就是一个典型的输入迭代器，它从输入流中读取数据，一旦读取就不能再次读取同一个值。
2. **输出迭代器 (Output Iterator):** 只能单向遍历序列，并且每个迭代器位置***只能被写入一次***。你可以递增它 (`++it`)，并使用它来赋值 (`*it = value`). `std::ostream_iterator` 就是一个典型的输出迭代器，它将数据写入输出流。
3. **前向迭代器 (Forward Iterator):** 具有输入迭代器和输出迭代器的所有功能（read/write），并且可以多次读取同一个元素。它们可以保存其状态并多次遍历同一序列。
4. **双向迭代器 (Bidirectional Iterator):** 具有前向迭代器的所有功能，并且可以向后移动 (`--it`)。`std::list`, `std::set`, `std::map`, `std::multiset`, `std::multimap` 等容器的迭代器都是双向迭代器。
5. **随机访问迭代器 (Random Access Iterator):** 具有双向迭代器的所有功能，并且提供了随机访问的能力。这意味着你可以像操作数组指针一样直接访问任何位置的元素（例如 `it[n]`, `it + n`, `it - n`, `it1 - it2`, `it1 < it2` 等）。`std::vector`, `std::deque`, `std::array` 以及原始数组的指针都提供随机访问迭代器。

### **区间 `insert` 的高效操作原理**

对于接收两个**非输入迭代器**（如前向、双向或随机访问迭代器）的区间 `insert` 函数，`vector` 的实现通常可以执行以下高效操作: 

1. **计算区间长度: ** 由于这些迭代器允许进行比较和递增，甚至对于更强的迭代器可以进行减法运算，`vector` 可以有效地计算出要插入的元素数量（即区间的长度）。
2. **预先分配空间（如果需要）: ** 一旦知道要插入的元素数量，`vector` 可以检查当前容量是否足够。如果不够，它可以一次性重新分配足够的内存来容纳现有元素和新元素，避免多次重新分配的开销。
3. **高效移动/复制元素: ** `vector` 可以利用 `std::move`（对于 C++11 及更高版本）或高效的内存拷贝操作（如 `memmove`）将现有元素直接移动到它们最终的位置，为新插入的元素腾出空间。然后，它可以直接在空出的位置上构造或复制新元素。这个过程通常只需要对每个现有元素进行一次移动操作。

### **输入迭代器带来的问题**

**输入迭代器的关键限制在于，你只能单向遍历它们，并且一旦你递增了迭代器，就无法再回到之前的元素。更重要的是，你通常无法直接计算两个输入迭代器之间的距离。**

当区间 `insert` 函数接收到两个输入迭代器 `first` 和 `last` 时，它**无法提前知道** `first` 和 `last` 之间有多少个元素。为了插入这些元素，`vector` 唯一的选择就是: 

1. **逐个读取元素: ** 它必须从 `first` 开始，一个接一个地读取迭代器指向的元素，直到达到 `last`。
2. **像单元素 `insert` 那样插入: ** 对于每个读取到的元素，`vector` 必须调用其单元素 `insert` 操作，将该元素插入到指定的位置。正如我们之前讨论的，每次单元素 `insert` 都可能导致插入点之后的所有现有元素向后移动。

### **效率损失的原因**

由于 `vector` 无法预先知道要插入多少个元素，它不能一次性预分配足够的内存。这可能导致在插入过程中多次发生内存重新分配，每次重新分配都需要拷贝或移动现有的所有元素，这是一个非常昂贵的操作。

此外，由于每次插入新元素都像调用单元素 `insert` 一样处理，插入点之后的现有元素会重复地向后移动，为每个新插入的元素腾出空间。这与使用非输入迭代器时能够一次性将现有元素移动到最终位置形成了鲜明的对比。

## item6: 警惕C++最令人恼怒的解析

`nm` 是 **"Symbol Table Names"** 或 **"Name List"** 的缩写，它是 **Unix/Linux 系统下的二进制工具**，用于**显示目标文件（Object File）、静态库（.a）或可执行文件（Executable）中的符号表信息**（即变量、函数等符号的名称、类型和地址）。它是分析二进制文件的重要工具，常用于调试、逆向工程和链接问题排查。

### **1. 查看符号表**

`nm` 可以列出二进制文件中的**全局变量、函数、未定义符号**等，并显示它们的**地址、类型和名称**（包括 C++ 重整后的名称）。

#### **基本用法**

```bash
nm 文件名
```

**示例**（查看可执行文件 `a.out` 的符号）: 

```bash
nm a.out
```

**输出示例**: 

```bash
0000000000001120 T _Z5hellov
0000000000001135 T main
                 U printf
```

- **`T`**: 代码段（Text）中的已定义函数（如 `_Z5hellov` 是 `hello()` 的重整名称）。
- **`U`**: 未定义符号（如 `printf`，需要在链接时解析）。
- **`D`**: 已初始化的全局变量（Data 段）。
- **`B`**: 未初始化的全局变量（BSS 段）。

------

### **2. 查看 C++ 重整（Mangled）名称**

C++ 由于函数重载和命名空间，编译器会生成重整名称（如 `_Z5hellov`）。`nm` 默认显示这些名称，但可以用 `-C` 选项**反重整（Demangle）**: 

```
nm -C a.out
```

**输出**: 

```cpp
0000000000001120 T hello()
0000000000001135 T main
                 U printf
```

现在 `_Z5hellov` 被解析为 `hello()`。

------

### **3. 过滤特定符号**

#### **(1) 查找特定函数**

```bash
nm a.out | grep "main"
```

**输出**: 

```bash
0000000000001135 T main
```

#### **(2) 只查看动态链接的未定义符号**

```bash
nm -u a.out
```

**输出**: 

```bash
                 U printf
```

（`printf` 是动态链接库 `libc.so` 提供的符号，运行时才会解析。）

------

### **4. 查看静态库（.a）的符号**

```bash
nm libexample.a
```

**输出**: 

```bash
example.o:
0000000000000000 T foo
0000000000000015 T bar
```

（静态库由多个 `.o` 文件组成，`nm` 会分别列出每个目标文件的符号。）

------

### **5. 显示符号大小**

`-S` 选项可以显示符号占用的字节数: 

```bash
nm -S a.out
```

**输出**: 

```bash
0000000000001120 0000000000000015 T _Z5hellov
```

（`hello()` 函数占用了 `0x15` 字节。）

----

### 6. **`nm` 的常见符号类型**

| 符号类型 | 说明                                            |
| :------- | :---------------------------------------------- |
| **`T`**  | Text（代码段）中定义的函数（全局可见）          |
| **`t`**  | Text 中的局部函数（static 函数）                |
| **`D`**  | Data 段中的全局变量（已初始化）                 |
| **`B`**  | BSS 段中的全局变量（未初始化）                  |
| **`U`**  | Undefined（未定义，需要链接时解析）             |
| **`W`**  | Weak symbol（弱符号，可被覆盖）                 |
| **`C`**  | Common symbol（未初始化的全局变量，可能被合并） |

----

### 7. **`nm` 的常用选项**

| 选项             | 作用                                   |
| :--------------- | :------------------------------------- |
| `-a`             | 显示所有符号（包括调试符号）           |
| `-g`             | 只显示外部（全局）符号                 |
| `-u`             | 只显示未定义符号                       |
| `-C`             | 反重整 C++ 名称（Demangle）            |
| `-l`             | 显示符号所在的源代码行号（需调试信息） |
| `-S`             | 显示符号大小                           |
| `--defined-only` | 只显示已定义的符号                     |

----

### 8. **`nm` 的典型用途**

1. **查看二进制文件导出了哪些函数**（如动态库 `.so`）。
2. **检查未定义的符号**（链接错误时排查缺失的函数）。
3. **分析 C++ 重整名称**（调试时解析 `_Z3fooi` 这样的符号）。
4. **逆向工程**（查看二进制文件的函数和全局变量）。

---

### **总结**

`nm` 是 **Linux/Unix 下分析二进制文件符号表的强大工具**，常用于: 

- **调试链接错误**（如 `undefined reference`）。
- **查看 C++ 重整后的函数名**（配合 `-C`）。
- **逆向分析二进制文件**（查看导出的函数和变量）。
- **检查静态库/动态库的符号**（`.a` 或 `.so`）。

## item7: 当使用 new 的指针的容器时，记得在销毁容器前 delete 那些指针

最简单的、异常安全的写法就是使用智能指针。

## item8: 永不建立 `auto_ptr` 的容器

`auto_ptr` 已经在 **C++11** 中被弃用（deprecated），并在 **C++17** 中被正式移除。它被弃用的主要原因是其 **所有权转移语义** 容易导致潜在的问题，特别是 **隐式所有权转移** 可能引发意外的行为或难以调试的错误。

------

## **为什么 `auto_ptr` 被弃用？**

### **1. 隐式所有权转移**

`auto_ptr` 的拷贝构造函数和赋值运算符会 **转移所有权**（原指针会被置为 `nullptr`），这可能导致 **悬空指针** 或 **意外的资源释放**。例如：

```c
std::auto_ptr<int> ptr1(new int(42));
std::auto_ptr<int> ptr2 = ptr1;  // ptr1 现在变成 nullptr！
// 如果后续代码误用 ptr1，会导致未定义行为（UB）
```

### **2. 不适用于 STL 容器**

由于 `auto_ptr` 的拷贝语义不符合 STL 容器的要求（容器要求元素可复制且保持有效），尝试在 `std::vector` 或 `std::map` 中使用 `auto_ptr` 会导致编译错误或运行时问题。

### **3. 缺乏对数组的支持**

`auto_ptr` 使用 `delete` 释放资源，而不是 `delete[]`，因此它 **不能安全地管理动态数组**：

```cpp
std::auto_ptr<int> arr(new int[10]);  // 错误！应该用 delete[] 释放
```

## item9: 在删除选项中仔细选择

删除的方式: 

1. 单纯删除元素
2. 删除元素之后打印消息或做其它什么事情

`std::remove`，`container::erase`。

### 1. `std::erase_if`

C++20 引入，删除元素，改变容器大小。

``` cpp
template<class Container, class T>
constexpr auto erase(Container& c, const T& value);

template<class Container, class Predicate>
constexpr auto erase_if(Container& c, Predicate pred);
```

不过有一个缺点就是，它们只能接受一个容器，而不能接受一对迭代器。虽然看起来限制了只能接受容器本身，但实际上这是为了提供一个更高级、更安全、更易用的接口。因此，接受迭代器的成员 `erase` 则保留用于需要删除 **特定子范围**的 场景。

### 2. `std::remove_if`

将 “不被移除的元素” 移动到容器的 “前端”，不会改变容器大小。

``` cpp
template< class ForwardIt, class T = typename std::iterator_traits
                                         <ForwardIt>::value_type >
constexpr ForwardIt remove( ForwardIt first, ForwardIt last,
                            const T& value );

template< class ForwardIt, class UnaryPred >
ForwardIt remove_if( ForwardIt first, ForwardIt last, UnaryPred p );
```

### 3. `std::list::remove`

实际删除元素，按条件删除符合条件的元素。

### 4. `std::container::erase`

一般是删除一个元素或者一对迭代器指定的元素。（按迭代器删除）

### 5. `forward_list`

`forward_list` 使用 `erase_after`。

## `[*]`item10: 注意分配器的协定和约束

## `[*]`item11: 理解自定义分配器的正确用法

## item12: 对 STL 容器线程安全性的期待现实一些

STL 对线程安全的保证非常有限：

1. 多个读是安全的
2. 写不同容器是安全的

也即，STL 不对线程安全做任何保障😀。

# vector 和 string

## item13: 尽量使用 vector 和 string 代替动态分配的数组

唯一一个使用 vector/string 代替动态数组可能出现的问题，那就是 string 的引用计数在多线程环境下会导致性能下降问题，参考 [Optimizations That Aren't (In a Multithreaded World)](http://www.gotw.ca/publications/optimizations.htm)

有三种解决方案: 

1. 使用 `vector<char>` 代替 string
2. 寻找不使用引用计数的 string 实现
3. 如果可以的话，关闭 string 的引用计数，不过这会导致不可移植的问题

但在 `g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0` 中，std::string 不在使用 COW(写时拷贝/引用计数) 优化策略。这是为了遵循 C++11 及之后标准对字符串的线程安全要求。。

- **C++98/C++03 时代**: GCC 的 `std::string` 曾使用引用计数（COW）实现，即多个字符串共享同一份底层数据，直到发生修改时才复制。这种优化节省内存，但**线程不安全**。
  
  ```cpp
  std::string s1 = "hello";
  std::string s2 = s1;  // s1 和 s2 共享同一块内存（引用计数+1）
  ```
  
- **C++11 起**: C++11 标准要求 `std::string` 的 `operator[]` 和 `data()` 提供线程安全的随机访问，而 COW 实现无法满足这一要求（因为修改引用计数需要同步）。因此，GCC 从 5.0 版本开始移除了 COW 实现，改为**直接分配独立内存**。

可以通过以下代码验证: 

``` cpp
#include <iostream>
#include <string>

int main() {
    std::string s1 = "hello, world, this is a test to check is string using cow?";
    std::string s2 = s1;  // 如果是 COW，s1 和 s2 会共享内存

    // 获取底层数据地址
    // data() 是一个成员函数，主要用于获取容器或字符串中底层数据的直接指针。
    const char* p1 = s1.data();
    const char* p2 = s2.data();

    std::cout << "s1 data address: " << (void*)p1 << '\n';
    std::cout << "s2 data address: " << (void*)p2 << '\n';
    std::cout << "Are addresses equal? " << (p1 == p2 ? "Yes (COW)" : "No (No COW)") << '\n';
}
```

 **为什么放弃 COW？**

- **线程安全**：COW 需要在修改时更新引用计数，多线程环境下需要加锁，性能可能反而下降。
- **C++11 标准要求**: `data()` 必须返回连续且独立的指针，COW 难以满足。
- **移动语义的普及**：C++11 引入了移动语义，减少了拷贝开销，降低了 COW 的必要性。

**例外情况**

- **短字符串优化（SSO）**: 
  现代 `std::string` 通常对小字符串（如 `"hello"`）直接存储在对象内部（无需堆分配），此时 `data()` 返回的地址是字符串对象自身的地址，但仍独立于其他字符串。
- **特定编译选项**: 
  某些旧版本 GCC 可能通过宏（如 `_GLIBCXX_USE_COW`) 强制启用 COW，但 GCC 11.4.0 默认禁用。

## item14: 使用 reserve 来避免不必要的重新分配

使用 reserve 来避免不必要的重新分配的情形比较固定，通常是你确切/大约知道有多少元素将最后出现在容器中，然后可选的是，一旦你添加完全部数据，修正掉任何多余的容量（item17）。

* `resize(size_t n)`：调整 size 的大小（`size`）

``` cpp
vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
cout << v.size() << ' ' << v.capacity() << endl; // 10 10
for(auto &x : v)    cout << x << ' '; cout << endl; // 1 2 3 4 5 6 7 8 9 10 

v.resize(5);
cout << v.size() << ' ' << v.capacity() << endl; // 5 10
for(auto &x : v)    cout << x << ' '; cout << endl; // 1 2 3 4 5 

v.resize(15);
cout << v.size() << ' ' << v.capacity() << endl; // 15 15
for(auto &x : v)    cout << x << ' '; cout << endl; // 1 2 3 4 5 0 0 0 0 0 0 0 0 0 0
```

* `reserve(size_t n)`：n 是希望预分配的 **最小** 容量（`capacity`）

``` cpp
vector<int> v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
cout << v.size() << ' ' << v.capacity() << endl; // 10 10
for(auto &x : v)    cout << x << ' '; cout << endl; // 1 2 3 4 5 6 7 8 9 10 

v.reserve(5);
cout << v.size() << ' ' << v.capacity() << endl; // 10 10
for(auto &x : v)    cout << x << ' '; cout << endl; // 1 2 3 4 5 6 7 8 9 10 

v.reserve(15);
cout << v.size() << ' ' << v.capacity() << endl; // 10 15
for(auto &x : v)    cout << x << ' '; cout << endl; // 1 2 3 4 5 6 7 8 9 10 
```

## item15: 小心 string 实现的多样性

不同库对 string 的实现细节很可能是不一样的，这取决于其具体使用场景。

## item16: 如何将 vector 和 string 的数据传给遗留的 API

vector 可以直接返回元素的指针，string 需要返回 `c_str()`。但一个容易忽视的点就是，**string 不在意结束符（`\0`），但是 C 风格字符串介意**：

``` cpp
string s = "hello,";
s += '\0';
s += "world!";
cout << s << endl; // hello,world!
cout << s.c_str() << endl; // hello,
```

## item17: 使用“交换技巧”来修正过剩容量

vector 的 swap 会交换所有内容，包括 capacity。

``` cpp
vector<int> v1{1, 2, 3};
vector<int> v2{1, 2, 3, 4, 5};
cout << v1.size() << ' ' << v1.capacity() << endl; // 3 3
cout << v2.size() << ' ' << v2.capacity() << endl; // 5 5

swap(v1, v2);
cout << v1.size() << ' ' << v1.capacity() << endl; // 5 5
cout << v2.size() << ' ' << v2.capacity() << endl; // 3 3
```

一个 `std::vector` 通常包含三个核心部分：

1. **指向动态分配数组的指针 (pointer to the underlying array):** 这是存储 `vector` 元素的地方。
2. **大小 (size):** 当前 `vector` 中元素的个数。
3. **容量 (capacity):** `vector` 在不必重新分配内存的情况下可以容纳的元素个数。容量通常大于或等于大小。

**`swap()` 的行为：**

当你调用 `v1.swap(v2)` 时，实际上发生的是 `v1` 和 `v2` 内部的这三个部分的值会进行交换。这意味着：

- `v1` 会获得 `v2` 的元素、大小和容量。
- `v2` 会获得 `v1` 原来的元素、大小和容量。

----

不过，从 C++11 标准开始，`std::vector` 引入了 `shrink_to_fit()` 成员函数。

`shrink_to_fit()` 是一个 **请求性操作**，它试图减小 `vector` 的 capacity 以适应当前的 size。

调用 `shrink_to_fit()` 可能会导致 `vector` 重新分配内存，元素被移动到新的内存位置（如果发生重新分配），之前占用的内存被释放。

**与 `swap()` 技巧的区别：**

虽然 `swap()` 技巧（`std::vector<T>(v).swap(v)`) 也能达到缩小容量的目的，但 `shrink_to_fit()` 提供了一个更直接和语义化的方式来请求这个操作。

**`shrink_to_fit()` 的特点和注意事项：**

- **这是一个请求：** 标准并没有保证 `shrink_to_fit()` 一定会减小容量。具体的行为取决于标准库的实现。有些实现可能会选择不进行任何操作，或者只在某些特定条件下才缩小容量。

- **可能导致元素移动：** 如果 `shrink_to_fit()` 导致了内存的重新分配，那么 `vector` 中的元素会被移动到新的内存位置。这可能会使之前获取的指向元素的指针、引用或迭代器失效。

  > **为什么不能直接“缩小”已分配的内存块？** 
  >
  > 操作系统提供的内存管理机制通常不允许直接缩小一块已经分配的内存区域。一旦一块内存被分配，它的大小在原地是无法直接改变的。要改变内存区域的大小，通常需要分配一块新的内存，将数据复制过去，然后释放旧的内存。

- **时间复杂度：** 如果发生了重新分配，`shrink_to_fit()` 的时间复杂度通常是线性的，即 O(size())，因为需要移动所有的元素。

- **不需要创建临时对象：** 与 `swap()` 技巧相比，`shrink_to_fit()` 不需要创建额外的临时 `vector` 对象。

## item18: 避免使用 `vector<bool>`

作为一个 STL 容器，`vector<bool>` 确实只有两个问题：

1. 它不是一个 STL 容器
2. 它并不容纳 bool

除此以外，就没有什么要反对的了。

之所以 `vector<bool>` 不是一个 STL 容器，是因为它不满足容器的一些基本条件。其中一条就是，如果你使用 `operator[]` 来得到 `Container<T>` 中的一个 T 对象，你可以通过取它的地址来获得指向那个对象的指针。也即：
``` cpp
vector<bool> v;
bool *pb = &v[0]; // NOT VALID!
```

而对于 `vector<bool>` 来说，这显然是不合法的，因为 `&v[0]` 返回的对象并不是 bool 类型。

# 关联容器

## item19: 了解相等和等价的区别

操作上来说，**相等（equality）**的概念是基于 `operator==` 的，如果表达式 `x == y` 返回 `true`，则我们说 `x` 和 `y` 这两个对象相等。但是 `x` 和 `y` 具有相等性，并不意味着所有成员都相等，也即 **$相等性 \ne 全等性$**，例如：

``` cpp
struct Node {
    int val;
    Node *next;
    bool operator==(const Node &rhs) const {
        // 在 operator== 中并没有涉及到 next 成员
        return val == rhs.val;
    }
};
```

**等价（equivalence）**是基于在一个 **有序区间** 中对象值的 **相对位置**。等价一般在每种标准关联容器（比如 `set`、`multiset`、`map` 和 `multimap`）的一部分 ---- 排序顺序方面有意义。两个对象 `x` 和 `y` 如果在关联容器 `c` 的排序顺序中没有 **哪个排在另一个之前**，那么它们关于 `c` 使用的排序顺序有 **等价** 的值。例如，我们有类 `Node` 的两个变量 `w1` 和 `w2`，如果 `!(w1 < w2) && !(w2 < w1)`，我们说 `w1` 和 `w2` 是 **等价** 的。

| 特性     | 等价 (Equivalence)     | 相等 (Equality)           |
| :------- | :--------------------- | :------------------------ |
| 定义依据 | 比较函数 (`operator<`) | 相等运算符 (`operator==`) |
| 数学基础 | 偏序关系               | 等价关系                  |
| 典型应用 | 有序关联容器           | 无序容器、通用比较        |
| 默认实现 | `!(a<b) && !(b<a)`     | `a == b`                  |

----

可以发现，**等价（equivalence）**的概念主要用在 **有序关联容器** 当中，这是因为有序容器需要一个比较函数（默认为 `less<T>`）来比较元素，从而确定元素的相对位置。并且使用**等价关系** `!(w1 < w2) && !(w2 < w1)` 来实现对 **相等** 的判断。

> 从直观上看，`!(w1 < w2) && !(w2 < w1)` 应该等价于 `w1 == w2`，但由于运算符重载，实际可能并非如此。

但如果，我们仅仅使用比较函数确定元素的相对位置，额外使用一个 `operator==` 来判断容器中的两个元素是否相等，就会导致一个很奇怪的问题。例如，对于一个 `set<string>` 容器，我们自定义的比较函数：

``` cpp
// 忽略大小写
bool set_less(const string &lhs, const string &rhs) {
    for(int i = 0; i < min(lhs.size(), rhs.size())) {
        if(tolower(lhs[i]) != tolower(rhs[i])) {
            return tolower(lhs[i]) < tolower(rhs[i]);
        }
    }
    return lhs.size() < rhs.size();
}
```

但我们对相等函数的定义是这样的：

``` cpp
bool operaotr==(const string &lhs, const string &rhs) {
    return lhs == rhs;
}
```

这就很奇怪，因为此时 `!(w1 < w2) && !(w2 < w1)` 的结果不等于 `w1 == w2`（例如，`hello` 和 `HELLO` ），从而产生一个严重的歧义。这种不一致性会导致关联容器的行为变得难以预测和理解，例如在查找、插入和删除元素时可能会出现逻辑错误。

---

另外就是，如果我们分别定义了比较函数和 `operator==`，`find` 成员函数和非成员 `find` 可能返回不同结果：

* **成员 `find` (`container.find(key)`):** 成员 `find` 使用容器自身的比较函数（用于排序和等价性判断）来查找与 `key` 等价的元素。
* **非成员 `find` (`std::find(begin, end, value)`):** 非成员 `find` 算法通常使用 `operator==` 来判断范围内的元素是否与 `value` 相等。

## item20: 为指针的关联容器指定比较类型

### 1. C++ 中函数不是一个类型

**在 C++ 中，函数本身（不包括函数指针或函数对象）并不是一个独立的“第一类 (first-class)”类型。** 这意味着你不能像操作基本数据类型（如 `int`, `float`）或对象那样直接操作函数本身。

让我们更详细地解释一下：

**C++ 中类型的概念：**

在 C++ 中，类型定义了变量可以存储的数据种类以及可以对这些数据执行的操作。类型是编译器进行类型检查、内存布局和代码生成的基础。

**函数的特殊性：**

- **代码块：** 函数本质上是一段可执行的代码块，它有自己的作用域、参数列表和返回类型。
- **没有直接的“值”：** 与变量存储特定的值不同，函数本身代表一段行为或逻辑。你不能说一个变量“存储”了一个函数。
- **不能直接作为变量存储：** 你不能声明一个类型为“函数”的变量来直接存储一个函数。例如，类似 `function my_func;` 的声明在 C++ 中是不合法的（这里的 `function` 只是一个示意性的名称）。

**如何“间接”地操作函数：**

虽然函数本身不是类型，但 C++ 提供了几种机制来间接地操作和传递函数：

1. **函数指针 (Function Pointers):**

   - 函数指针是一种特殊的指针类型，它可以存储函数的内存地址。
   - 你可以声明函数指针变量，将函数的地址赋给它们，并通过函数指针来调用函数。
   - 函数指针有其特定的类型签名，包括函数的返回类型和参数列表。

   ```cpp
   int add(int a, int b)      { return a + b; }
   int subtract(int a, int b) { return a - b; }
   
   int (*operation)(int, int); // 声明一个函数指针类型
   
   operation = add;
   int sum = operation(5, 3); // 通过函数指针调用 add
   
   operation = subtract;
   int difference = operation(5, 3); // 通过函数指针调用 subtract
   ```

   在这个例子中，

   ```cpp
   int (*operation)(int, int)
   ```

    是一个函数指针类型，可以指向接受两个 `int` 参数并返回 `int` 的函数。

2. **函数对象 (Function Objects) 或仿函数 (Functors):**

   - 函数对象是重载了函数调用运算符 `operator()` 的类的对象。
   - 它们可以像函数一样被调用，并且可以携带状态。
   - 函数对象是类型，因此你可以创建函数对象类型的变量，并将它们作为参数传递等。

   ```cpp
   struct Adder {
       int operator()(int a, int b) const {
           return a + b;
       }
   };
   
   Adder my_adder;
   int result = my_adder(10, 5); // 像调用函数一样调用函数对象
   ```

3. **`std::function` (C++11 及更高版本):**

   - `std::function` 是一个通用的 **函数包装器**，可以存储任何**可调用对象**，包括函数指针、函数对象、lambda 表达式和成员函数指针。

     ``` cpp
     // 绑定成员函数
     struct Foo {
         void print(int x) {
             cout << "Int: " << x << endl;
         }
     
         static void name() { puts("Foo"); }
     };
     
     void (Foo::*fptr)(int) = &Foo::print;
     void (*fptr2)() = &Foo::name;
     
     int main() 
     {
         Foo f;
         (f.*fptr)(10); // Int: 10
         fptr2(); // Foo
         return 0;
     }
     ```

   - `std::function` 本身是一个模板类，你需要指定它所包装的可调用对象的类型签名。

   - `std::function` 提供了类型擦除的能力，使得可以以统一的方式处理不同类型的可调用对象。

   ```cpp
   #include <functional>
   
   int multiply(int a, int b) { return a * b; }
   
   std::function<int(int, int)> func;
   
   func = multiply;
   int product = func(7, 2);
   
   auto lambda = [](int a, int b) { return a / b; };
   func = lambda;
   int quotient = func(10, 2);
   ```

4. **Lambda 表达式 (C++11 及更高版本):**

   - Lambda 表达式是一种创建**匿名函数对象（闭包）**的简洁方式。
   - **Lambda 表达式本身并不直接是类型**，但编译器会为每个 lambda 表达式生成一个唯一的匿名函数对象类。
   - 你通常使用 `auto` 来捕获 lambda 表达式，或者将它们赋值给 `std::function` 类型的变量。

   ```cpp
   auto add_lambda = [](int a, int b) { return a + b; };
   int sum_lambda = add_lambda(20, 5);
   ```

### 2. 书中给出的例子

首先，使用算法代替 `for` 循环，这是一个好的编程习惯！😀

``` cpp
struct DeReferenceLessThan {
    // 使用模板成员函数
    template<typename PtrType>
    bool operator()(const PtrType &lhs, const PtrType &rhs) const {
        return *lhs < *rhs;
    }
};

void test_raw_pointer() 
{
    set<string*, DeReferenceLessThan> s;
    s.insert(new string("hello"));
    s.insert(new string("world"));
    s.insert(new string("vscode"));
    s.insert(new string("pycharm"));
    // 使用算法而不是原生 for 循环
    for_each(s.begin(), s.end(), [](const auto &ptr){std::cout << *ptr << std::endl;});
}

// 更推荐使用智能指针
void test_smart_pointer() 
{
    std::set<std::unique_ptr<string>, DeReferenceLessThan> s;
    s.insert(std::make_unique<string>("hello"));
    s.insert(std::make_unique<string>("world"));
    s.insert(std::make_unique<string>("vscode"));
    s.insert(std::make_unique<string>("pycharm"));
    for_each(s.begin(), s.end(), [](const auto &ptr){std::cout << *ptr << std::endl;});
}

int main()
{
    test_raw_pointer();
    test_smart_pointer();
    return 0;
}
```

## item21: 永远让比较函数对相等的值返回 false

这主要是因为在关联容器中，**相等** 的定义是 **等价**。如果我们令比较函数在相等时返回 `true`，那么下面对 **相等** 的判断就会出问题：

``` cpp
!(a <= b) && !(b <= a)
```

当 `a==b` 时，`a` 和 `b` 直觉上应该是相等的，但是基于等价的判断是 `!true && !true` 也即 `false && false`（`false`），也就是说，此时基于比较函数的等价判断，`a` 和 `b` 是不等价的，也即 `a` 和 `b` 是不相等的。

如果这种情况出现在 `set` 这种容器当中，可能会导致 `set` 中出现重复的值，那此时 `set` 还是 `set` 吗？并且此后使用`set` 的行为是未定义的，遗患无穷。

另一个问题是，`set` 不允许出现重复值，那 `multiset` 呢？

我们考虑函数 `equal_range`，我们 `multiset` 中有两个 `a`，那么我们期望 `equal_range` 返回包含这两个拷贝的范围迭代器，但实际上并不如此，因为 `equal_range` 是依赖于 **等价** 进行相等判断的，而我们前面提到过，`a` 和 `a` 并不等价。

``` cpp
multiset<int, less_equal<int>> s;
s.insert(8);
s.insert(9);
s.insert(10);
s.insert(10);
s.insert(11);
{
    // multiset::equal_range 使用 less_equal
    auto [s_beg, s_end] = s.equal_range(10);
    for(auto &it = s_beg; it != s_end; ++ it) 
        cout << *it << ' ';
    cout << endl;
    // 在测试中, 什么也不会输出
}

{
    // std::equal_range 使用 operator==
    auto [s_beg, s_end] = equal_range(s.begin(), s.end(), 10);
    for(auto &it = s_beg; it != s_end; ++ it) 
        cout << *it << ' ';
    cout << endl;
    // 10, 10
}
```

最后就是依然要注意，**全局的 `equal_range` 并不使用我们指定的 `less_equal` 比较函数，它使用默认的比较函数（`operator<`）。**

这也是比较典型的，全局函数和类成员函数执行结果不一致的例子。

## item22: 避免原地修改 set 和 multiset 的值

最安全的，也是最简单的方法就是，删除旧元素，插入新元素。😀

## item23: 考虑用有序 vector 代替关联容器

书中最后 `vector` 模拟 `map` 的例子：

``` cpp
struct DataCompare {
    bool operator()(const Data &lhs, const Data &rhs) const {
        return less_compare(lhs.first, rhs.first);
    }

    bool operator()(const Data &lhs, const Data::first_type &x) const {
        return less_compare(lhs.first, x);
    }
    
    bool operator()(const Data::first_type &x, const Data &rhs) const {
        return less_compare(x, rhs.first);
    }

    bool less_compare(const Data::first_type &lhs, const Data::first_type &rhs) const {
        return lhs < rhs;
    }
};

int main()
{
    vector<Data> v;

    // 插入阶段
    v.push_back({"cplusplus", 0});
    v.push_back({"world", 1});
    v.push_back({"python", 2});
    v.push_back({"rust", 3});
    v.push_back({"java", 4});
    v.push_back({"hello", 5});
    v.push_back({"world", 6});
    v.push_back({"lua", 7});
    v.push_back({"mysql", 8});
    v.push_back({"redis", 9});

    // 排序
    sort(v.begin(), v.end(), DataCompare{});

    // 查找阶段
    string key = "hello";
    if(!binary_search(v.begin(), v.end(), key, DataCompare{})) {
        cout << "Not Found: " << key << endl;
    }
    else {
        cout << "Found it: " << key << endl;
    }

    key = "mysql";
    auto it = lower_bound(v.begin(), v.end(), key, DataCompare{});
        // lowerbound 返回第一个 >=key 的元素
        // 因此，比较  !DataCompare{}(*it, key) 是避免返回的 it 是 key 应该插入的位置但 key 本身并不存在
        // 另外就是要注意，在这里，vector 模拟的是关联容器，因此我们需要以 等价 去思考 相等
        // 所以这里不要误写为 it != v.end() && DataCompare{}(*it, key)
    if(it != v.end() && !DataCompare{}(*it, key)) {
        cout << "Fount it: " << it->first << ' ' << it->second << endl;
    }
    else {
        cout << "Noe Found: " << key << endl;
    }

    key = "redis";
    auto [key_beg, key_end] = equal_range(v.begin(), v.end(), key, DataCompare{});
    if(key_beg != key_end) {
        cout << "Fount it(" << distance(key_beg, key_end) << " items):" << endl;
        for(auto it = key_beg; it != key_end; ++ it) {
            cout << it->first << ' ' << it->second << endl; 
        }
    }
    else {
        cout << "Not Found: " << key << endl;
    }

    return 0;
}
```

## item24: 当关乎效率时应该在 `map::operator[]` 和 `map::insert` 之间仔细选择

### 1. operator[]

`map::operator[]` 是一个经过特殊处理的功能，为了简化**“添加和更新”**功能。当我们调用 `m[k]=v` 时，实际上包含了两个步骤：

1. 检查键 `k` 是否已经在 map 里了。
2. 如果不存在，就添加键 `k`（此时 `v` 的值默认构造），然后修改其值为 `v`；否则，直接修改 `m[k]` 为 `v`。

### 2. 选择规则

* **insert:** `insert()`，如果我们使用 `operator[]` 实现 insert 操作，会额外的默认构造一个临时对象（一次额外的构造和析构），然后还会重新对插入的对象赋值（一次额外的赋值）。
* **update:** `operator[]`，如果我们使用 `insert` 完成 update 操作，insert 会插入一个对象（这个对象需要一次构造和析构）。

## item25: 警惕全局函数和同名成员函数行为不同的情况

参见条款 20 中 `find` 和条款 21 中 `equal_range` 的例子。

# 迭代器

## item26: 尽量用 iterator 代替 const_iterator, reverse_iterator 和 const_reverse_iterator

> `const_iterator` 等价于 `T const*`。

更直接来说，尽量用 `iterator` 代替 `const_iterator`，这是因为 `reverse_iterator` 的使用场景更显而易见，一个从头到尾遍历，一个从尾到头遍历，而且 `reverse_iterator` 可以通过 `base()` 轻松转换回 `iterator`。

（1）某些成员函数只能接收 `iterator`：没有办法从一个 `const_iterator` 转换得到一个 `iterator`，也无 法从`const_reverse_iterator` 得到 `reverse_iterator`。而 `insert` 和 `erase` 的一些版本要求 `iterator`。如果你需要调用这些函数， 你就必须产生 `iterator`，而不能用 `const` 或 `reverse iterators`。

（2）混用 `iterator` 和 `const_iterator`：按理来说，在一些不涉及对象的修改的操作中，我们应该是可以混用 `iterator` 和 `const_iterator` 的，毕竟后者只是多了一层常量行，但由于某些容器对 `const_iterator` 的特殊实现，体现为 `iterator` 和 `const_iterator` 是两个不同的 class，混用它们可能导致编译问题。

## item27: 用 distance 和 advance 把 const_iterator 转化成 iterator

对于一些容器类（deque、list、关联容器、散列容器）来说，`iterator` 和 `const_iterator` 是两个完全不同的 class，它们之间并不是 `const T*` 和 `T*` 的关系。

对于所有容器来说，`reverse_iterator` 和 `const_reverse_ierator` 是两个不同的类。

我们可以看一下标准库是怎么实现的：`g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`。

* `iterator` 和 `const_iterator`

``` cpp
vector<int>::iterator;       // typedef __gnu_cxx::__normal_iterator<int *, std::vector<int>> std::vector<int>::iterator
vector<int>::const_iterator; // typedef __gnu_cxx::__normal_iterator<const int *, std::vector<int>> std::vector<int>::const_iterator

string::iterator;       // typedef __gnu_cxx::__normal_iterator<char *, std::string> std::string::iterator
string::const_iterator; // typedef __gnu_cxx::__normal_iterator<const char *, std::string> std::string::const_iterator

map<int,int>::iterator;       // typedef std::_Rb_tree_iterator<std::pair<const int, int>> std::map<int, int>::iterator
map<int,int>::const_iterator; // typedef std::_Rb_tree_const_iterator<std::pair<const int, int>> std::map<int, int>::const_iterator

list<int>::iterator;       // typedef std::_List_iterator<int> std::__cxx11::list<int>::iterator
list<int>::const_iterator; // typedef std::_List_const_iterator<int> std::__cxx11::list<int>::const_iterator

deque<int>::iterator;       // typedef std::_Deque_iterator<int, int &, int *> std::deque<int>::iterator
deque<int>::const_iterator; // typedef std::_Deque_iterator<int, const int &, const int *> std::deque<int>::const_iterator

set<int>::iterator;         // typedef std::_Rb_tree_const_iterator<int> std::set<int>::iterator
set<int>::const_iterator;   // typedef std::_Rb_tree_const_iterator<int> std::set<int>::const_iterator
```

其中，`__gnu_cxx::__normal_iterator` 是 **GNU C++ 标准库 (libstdc++)** 中用于实现某些容器（特别是那些在内存中连续存储元素的容器）的迭代器的一种常见内部类型。你可以把它理解为对原生 C++ 指针的一层封装或抽象。它“表现得像”一个普通的指针，允许你进行递增、递减、解引用等操作来访问容器中的元素。我们可以看一下它的源代码：

``` cpp
template<typename _Iterator, typename _Container>
class __normal_iterator
{
    protected:
    _Iterator _M_current;

    typedef iterator_traits<_Iterator>		__traits_type;
    // ...
}
```

通过源代码我们可以很清晰的得知：

* __`gnu_cxx::__normal_iterator<int *, std::vector<int>> std::vector<int>::iterator` 就是对 `int *` 的封装
* `typedef __gnu_cxx::__normal_iterator<const int *, std::vector<int>> std::vector<int>::const_iterator` 就是对 `const int*` 的封装

因此说，对于 string 和 vector 这两个容器，理论上 `iterator` 和 `const_iterator` 是可以进行类型转换的。

但是对于 map、list、deque 这三个容器，它们的 `iterator` 和 `const_iterator` 就完全是不同的类了。

不过有趣的是，set 的 `iterator` 和 `const_iterator` 都是对类型 `*std::_Rb_tree_const_iterator<int>*` 的 typedef。

**核心原因：`set` 的元素本质上是只读的**

* `std::set` 是一种关联容器，它存储**唯一**且**已排序**的元素。为了维护这种排序和唯一性，当你将一个元素插入到 `set` 中后，**你就不应该再直接修改这个元素的值**。如果你允许通过 `iterator` 修改元素的值，可能会破坏 `set` 内部的排序规则，导致容器状态混乱，甚至违反其基本特性（唯一性）。
* **`set` 的迭代器的设计哲学是：通过迭代器，你只能访问（读取）`set` 中的元素，而不能修改它们的值。**

* `reverse_iterator` 和 `const_reverse_iterator`

``` cpp
vector<int>::reverse_iterator;        // typedef std::reverse_iterator<std::vector<int>::iterator> std::vector<int>::reverse_iterator
vector<int>::const_reverse_iterator;  // typedef std::reverse_iterator<std::vector<int>::const_iterator> std::vector<int>::const_reverse_iterator

string::reverse_iterator;       // typedef std::reverse_iterator<std::string::iterator> std::string::reverse_iterator
string::const_reverse_iterator; // typedef std::reverse_iterator<std::string::const_iterator> std::string::const_reverse_iterator

map<int,int>::reverse_iterator;       // typedef std::reverse_iterator<std::conditional<false, std::map<int, int>::const_iterator, std::_Rb_tree_iterator<std::pair<const int, int>>>::type> std::map<int, int>::reverse_iterator
map<int,int>::const_reverse_iterator; // typedef std::reverse_iterator<std::map<int, int>::const_iterator> std::map<int, int>::const_reverse_iterator
```

我们可以清晰地发现，对于 `reverse_iterator` 和 `const_reverse_iterator`，所有容器都是对模板类 `std::reverse_iterator<T>` 的实例化，因此显然是完全不同的类型。

## item28: 了解如何通过 `reverse_iterator` 的 base 得到 `iterator`

在通过 `reverse_iterator` 得到 `iterator` 时，我们需要考虑，对于当前的操作 `op`，`op(reverse_iterator)` 和 `op(reverse_iterator::base())` 是否等价，这是一个很核心的问题。如果这两者不等价，那么看起来 `base()` 就是无效的操作。

1.  插入：`insert(ri)` 等价于 `insert(ri.base())`
2. 删除：`erase(ri)` 不等价于 `erase(ri.base())`
   * `erase(--ri.base())`     ❌
     * 对于 `vector` 和 `string`，这种写法可能无法通过编译：`ri.base()` 返回的是临时迭代器对象，C++ 禁止修改临时对象（`--` 操作需要左值）
   *  `erase((++ri).base())` ✔️

> 注意 `base()` 只能用于反向迭代器，对正向迭代器使用 `base()` 是无效的。
>
> `ri.base()` 指向的是 `ri` 所指向元素的下一个位置。因此，**不要直接解引用 `base()`** 返回的迭代器，因为它通常指向当前元素的下一个位置（可能为 end）。

## item29: 需要一个一个字符输入时考虑使用 `istreambuf_iterator`

为什么不使用 `istream_iterator`？`istream` 默认使用 `operator>>` 函数读入数据，这导致：

1. 默认情况下 `operator>>` 会忽略空格，这有时并不是我们想要的
2. `operator>>` 进行的是格式化输入，这意味着每次读入都需要处理一些格式化信息（例如是否需要跳过空格的 `skipws` 标记）、进行全面的读取错误检查、读取遇到问题时时决定是否抛出异常

而 `istreambuf_iterator` 直接从流的缓冲区读取一个字符，不需要关心格式化的问题。

相应的，当我们需要一个一个字符输出的时候，考虑使用 `ostreambuf_iterator`。

| 特性         | `istream_iterator<char>`     | `istreambuf_iterator<char>` |
| :----------- | :--------------------------- | :-------------------------- |
| **读取方式** | 格式化输入（`operator>>`）   | 直接缓冲区访问              |
| **跳过空格** | 默认跳过（受 `skipws` 影响） | 永不跳过                    |
| **性能开销** | 高（有格式检查/转换逻辑）    | 极低（直接内存访问）        |
| **异常处理** | 可能抛出异常                 | 无额外异常检查              |
| **典型用途** | 读取已知格式的数字/字符串    | 原始字节流/字符级操作       |

在使用时，通常令 `istreambuf_iterator<T>(cin/file)` 作为 `begin()`，`istream_iterator<T>()` 作为 `end()`，构成一对迭代器。

> 默认构造表示 `end()`。

``` cpp
ifstream in("./data/data.txt", ios::binary);
ofstream out("./data/copy.txt", ios::binary);
copy(istreambuf_iterator<char>(in), istreambuf_iterator<char>(), ostreambuf_iterator<char>(out));

vector<int> numbers {
    istream_iterator<int>(cin), 
    istream_iterator<int>()
};

for(auto &x : numbers) cout << x << endl;
```

# 算法

## item30: 确保目标区间足够大

无论何时你使用一个要求 **指定目的区间** 的算法，确保目的区间已经 **足够大** 或者在算法执行时可以 **增加大小**。如果你选择增加大小，就使用插入迭代器，比如 `ostream_iterator` 或从 `back_inserter`、`front_inserter` 或 `inserter` 返回的迭代器。

一个容易犯的错误：

当使用 reserve 来提高一连串插入的效率时，总是应该记住 **reserve 只增加容器的容量：容器的大小仍然没有改变。**即使调用完 reserve，当你想要让容器把新元素加入到 vector 或 string 时，你也必须对算法使用插入迭代器（比如，从 back_inserter、front_inserter 或 inserter 返回的迭代器之一）。

``` cpp
vector<int> v;
v.reserve(16);
cout << v.size() << ' ' << v.capacity() << endl; // 0 16
for(int i = 0; i < 16; i ++ ) {
    v[i] = i; // 未定义行为
}
```

请注意，有可能在你执行上面的代码后，对 `v` 进行输出，得到了正确的结果，但这并没有什么用，从 `v.size()` 也可以看出。而且，未定义的行为，往往在你测试时正确，测试人员测试时也正确，但在用户哪里出错了！

正确的写法应该是：

``` cpp
vector<int> v;
v.reserve(16);
vector<int> tmp{1, 2, 3, 4, 5};
// copy(tmp.begin(), tmp.end(), back_inserter(v));
copy(tmp.begin(), tmp.end(), back_insert_iterator<vector<int>>(v));
```

## item31: 了解你的排序选择

``` cpp
/* 1. sort */
template< class RandomAccessIterator >
void sort( RandomAccessIterator first, 
          RandomAccessIterator last);

template< class RandomAccessIterator, class Compare >
void sort( RandomAccessIterator first, 
          RandomAccessIterator last, 
          Compare comp);


/* 2. partial_sort */
// [first,middle) is sorted
template< class RandomAccessIterator >
void partial_sort( RandomAccessIterator first,
                   RandomAccessIterator middle,
                   RandomAccessIterator last);

template< class RandomAccessIterator, class Compare >
void partial_sort( RandomAccessIterator first,
                   RandomAccessIterator middle,
                   RandomAccessIterator last,
                   Compare comp);


/* 3. nth_element */
template< class RandomAccessIterator >
void nth_element( RandomAccessIterator first,
                  RandomAccessIterator nth,
                  RandomAccessIterator last);

template< class RandomAccessIterator, class Compare >
void nth_element( RandomAccessIterator first,
                  RandomAccessIterator nth,
                  RandomAccessIterator last,
                  Compare comp);

/* 4. partition */
template< class BidirectionalIterator, class UnaryPredicate >
BidirectionalIterator partition( BidirectionalIterator first, // attention:
                                 BidirectionalIterator last,  // bidirectionalIterator!
                                 UnaryPredicate p);

/* 5. stable_sort */

/* 6. stable_partition */
```

`sort`，`stable_sort`，`nth_element`，`partial_sort` 需要随机访问迭代器，使用随机访问迭代器的容器：

1. `vector`
2. `string`
3. `deque`
4. `array`

`partition` 和 `stable_partition` 需要双向迭代器，因此它们可以应用于任何容器。

除了算法，容器或许也是你可以考虑的选择：

1. 关联容器
2. `priority_queue`，注意 `priority_queue` 不是标准的 STL 容器，因为它首先就没有迭代器
3. `std::list`：有专门的成员函数 `sort` 用来排序，而且是稳定排序，它的时间复杂度介于 `std::sort` 和将 `list` 中元素复制到 `vector` 再排序之间，也是 O(nlogn) 级别的。

## item32: erase-remove 惯用法

通过本条款，你需要明白：

1. `remove` 和 `erase` 提供的功能有何区别？
2. `std::remove` 到底干了什么？
3. 为什么 `remove` 是非成员函数，`erase` 是成员函数？
4. 为什么 `list` 和 `forward_list` 有成员函数版本的 `remove`？

### 1. remove 函数

`remove` 是按 **<font color=blue>值</font>** 删除的 STL 算法。在 C++ 的 STL 容器中，除了 `list` 和 `forward_list`，其它容器都没有提供名为 `remove` 的成员函数。

这是因为对于像 `std::vector` 和 `std::deque` 这样的基于数组的容器，直接在成员函数中实现一个与 `std::list::remove` 行为完全相同的 `remove`（即查找并删除所有匹配项）效率会很低。因为每次删除一个元素后，其后的所有元素都需要向前移动以填补空位，如果有很多匹配的元素，这将导致多次的元素移动操作。

而 `erase` 就没有这个问题，因为 `erase` 是按 **<font color=blue>位置</font>** 删除元素，它接受迭代器，通过迭代器删除元素是很方便的。

----

全局的 `std::remove` 函数（以及 `std::remove_if`）不实际从容器中删除元素，这主要是基于 C++ STL (标准模板库) 的核心设计原则，特别是**算法与容器的分离**以及**迭代器的通用性**。

以下是几个关键原因：

1. **算法的通用性 (基于迭代器操作)**：
   - STL 算法（如 `std::remove`）被设计为通用的，它们通过**迭代器 (iterators)** 对序列进行操作，而不是直接操作容器本身。这意味着同一个 `std::remove` 算法可以用于各种不同的序列，包括 `std::vector`、`std::list`、`std::deque`、C 风格数组，甚至是自定义的、符合迭代器要求的序列。
   - 如果 `std::remove` 尝试实际删除元素，它就需要知道具体容器的内部实现细节（例如，如何管理内存、如何调整大小等）。这将破坏其通用性，使得算法与特定容器类型紧密耦合。
2. **容器的职责 (管理自身元素和内存)**：
   - 在 STL 中，容器负责管理其自身的元素，包括内存分配、元素数量的跟踪以及元素的物理删除。
   - 直接修改容器大小（例如删除元素）是容器自身的成员函数（如 `erase()`）的职责。这些成员函数了解容器的内部结构，可以高效且安全地执行这些操作。
3. **效率考量 (避免多次低效操作)**：
   - 对于像 `std::vector` 这样的连续存储容器，每次删除一个元素都可能需要移动其后的大量元素来填补空缺。如果 `std::remove` 每找到一个匹配项就执行一次删除，对于有多个匹配项的情况，这将导致多次、低效的元素移动。
   - `std::remove` 的工作方式（将所有不匹配的元素移到序列的前面，返回一个指向逻辑新尾部的迭代器）允许它在一次遍历中完成所有元素的“逻辑移除”。然后，容器的 `erase` 成员函数可以根据这个新的逻辑尾部，一次性地、高效地移除所有不再需要的元素。这就是所谓的 "erase-remove idiom" (擦除-移除惯用法)。
4. **保持接口简洁和一致性**：
   - STL 算法通过迭代器提供了一个统一的接口。算法只关心如何根据迭代器访问和（可能）修改元素的值，而不关心这些元素是如何存储或管理的。
   - 如果算法也负责删除元素，那么算法的接口会变得更加复杂，并且可能需要为不同类型的容器提供不同的重载或特化版本。
5. **对非容器序列的支持**：
   - `std::remove` 也可以作用于普通的 C 风格数组。数组的大小是固定的，无法像容器那样动态调整。`std::remove` 在这种情况下依然可以工作，它会将不匹配的元素移到数组的前部，并返回一个指向逻辑新尾部的指针。用户之后可以根据这个指针来决定如何处理数组的有效部分。

**总结来说：**

`std::remove` 的设计哲学是“做一件事并把它做好”。它的任务是**重新排列序列中的元素**，将所有满足移除条件的元素逻辑上分离出来，并标识出有效元素的范围。实际的**物理删除**和**容器大小调整**则交由容器自身的成员函数 `erase` 来完成，因为只有容器才知道如何高效和安全地管理自己的内存和元素。这种关注点分离的设计使得 STL 更加灵活、通用和高效。

> 1. `remove` 和 `erase` 提供的功能有何区别？
> 2. `std::remove` 到底干了什么？
>
> remove 按值将要删除的元素放在容器的最后，erase 按迭代器位置实际删除元素；remove 一般是非成员函数（list 和 forward_list 有成员函数版本的 remove），erase 一般是成员函数（也有非成员函数的版本）。
>
> 1. 为什么 `remove` 是非成员函数，`erase` 是成员函数？
>
> 因为 remove 的功能是按值删除，由于一个元素接一个元素的删除可能导致效率的低下，特别是对于 vector 这种连续容器，因此 remove 实际上是将元素移动到容器的末尾。也因此它不需要实际删除元素，也就不需要了解容器的实现细节，也就无需设计为成员函数。
>
> 1. 为什么 `list` 和 `forward_list` 有成员函数版本的 `remove`？
>
> 因此它们一个接一个元素的删除很高效。

### 2. STL 算法设计原则

C++ STL (标准模板库) 中的算法（定义在 `<algorithm>` 头文件中的那些，例如 `std::sort`, `std::find`, `std::copy`, `std::transform`, `std::remove` 等）其核心设计原则之一就是**通过迭代器操作序列，而不直接修改容器的结构属性，比如大小 (size)**。

以下是对此的详细解释：

1. **算法的通用性与迭代器**：
   - STL 算法被设计为高度通用的，它们通过迭代器 (iterators) 来访问和操作元素序列。迭代器提供了一种统一的访问方式，使得同一个算法可以应用于多种不同的容器（如 `std::vector`, `std::list`, `std::deque`, 甚至 C 风格数组）以及其他非标准序列。
   - 算法本身并不知道它正在操作的是哪种具体类型的容器，它只关心迭代器提供的接口（如解引用 `*it`，前进 `++it` 等）。
2. **容器负责自身管理**：
   - 容器（如 `std::vector`）负责管理其内部存储的元素，包括内存分配、元素数量的跟踪以及容器大小的调整。
   - 改变容器大小的操作（如添加元素导致容量不足而重新分配内存，或删除元素导致大小减小）是容器自身的成员函数（如 `push_back()`, `pop_back()`, `insert()`, `erase()`, `resize()`, `clear()` 等）的职责。这些成员函数了解容器的内部实现，可以安全有效地执行这些操作。
3. **`std::remove` 的例子**：
   - 你可能会想到 `std::remove` 这样的算法。但正如我们之前讨论的，`std::remove` **并不真正从容器中删除元素，也不改变容器的大小**。
   - 它的工作方式是将所有 *不等于* 指定值的元素向前移动，覆盖掉那些 *等于* 指定值的元素。它返回一个迭代器，指向逻辑上新的序列末尾。原始容器的大小在 `std::remove` 调用后保持不变。
   - 要真正从容器中移除元素并改变其大小，你需要结合使用容器的 `erase()` 成员函数，即所谓的 "erase-remove idiom"：`container.erase(std::remove(container.begin(), container.end(), value), container.end());`。在这个组合中，是 `container.erase()` 改变了容器的大小，而不是 `std::remove`。
4. **少数例外或特殊情况**：
   - 有一些算法，比如 `std::copy_if` 或 `std::transform` 配合 `std::back_inserter` 或 `std::inserter` 使用时，看起来像是算法在“添加”元素到另一个容器中。但严格来说，是插入迭代器（`std::back_inserter` 返回的对象）利用了目标容器的 `push_back()` 或 `insert()` 成员函数来实际添加元素并改变其大小。算法本身只是通过迭代器写入值。
   - 类似地，`std::generate_n` 配合插入迭代器也能向容器中填充新生成的元素。

**总结：**

核心思想是**关注点分离 (Separation of Concerns)**。

- **STL 算法** 专注于对已有元素序列进行操作（排序、查找、变换、移动等），它们通过迭代器抽象了具体的数据源。
- **STL 容器** 专注于数据的存储和管理，包括控制容器的大小、内存分配和元素的生命周期。

因此，当你使用一个标准的 STL 算法时，除非它配合了特殊的插入迭代器或者你后续调用了容器的成员函数，否则算法本身不会改变源容器的大小。这种设计使得 STL 既强大又灵活。

## item33: 提防在指针的容器上使用类似 remove 的算法

首先，直接 `erase` 包含指针的容器就是一个很危险的行为，因为这些指针指向的内存可能是动态分配的。

不过，即使我们还没有执行 `erase`，当我们执行了 `remove` 之后，就已经产生了十分危险的后果：这是因为，对于 `remove`、`unique`、`remove_if` 这些算法，它们的实现是基于 **覆盖** 的，即后面的元素会覆盖前面的元素。而覆盖会导致前面被覆盖的元素 **丢失**。如果被覆盖的元素是一个指向动态分配内存的指针，就会导致该动态分配的内存无法释放，也即导致内存泄露的问题。

对此，我们有以下解决方案：

1. 最简单的方法就是避免使用裸指针，如果我们使用智能指针管理动态分配的内存，就不会有上面的问题。
2. 如果我们对剩余元素没有顺序需求的话，`partition` 也是一个好的替代。
3. 在调用这些算法之前，提前手动将指针 delete 掉。

## item34: 注意那些算法需要有序区间

### 1. 算法介绍

只能操作有序区间的算法：

* 通过 **等价** 来判断两个值是否 **相同**

``` cpp
/*=====================================*/
/*              二分搜索操作            */
/*=====================================*/
bool binary_search(ForwardIt first, 
              ForwardIt last, 
              const T& value, 
              Compare comp = std::less<>{})
    
ForwardIt lower_bound(ForwardIt first, 
                      ForwardIt last, 
                      const T& value, 
                      Compare comp = std::less<>{})
    
ForwardIt upper_bound(ForwardIt first, 
                      ForwardIt last, 
                      const T& value, 
                      Compare comp = std::less<>{})
    
std::pair<ForwardIt, ForwardIt> 
equal_range(ForwardIt first, 
            ForwardIt last, 
            const T& value, 
            Compare comp = std::less<>{})
  
/*=====================================*/
/*              集合操作                */
/*=====================================*/

// A U B
OutputIt set_union(InputIt1 first1, 
                   InputIt1 last1, 
                   InputIt2 first2, 
                   InputIt2 last2, 
                   OutputIt d_first, 
                   Compare comp = std::less<>{})
// 说明: 如果一个元素在第一个集合中出现 m 次，在第二个集合中出现 n 次，那么它在交集中会出现 max(m, n) 次。

// A ∩ B
OutputIt set_intersection(InputIt1 first1, 
                          InputIt1 last1, 
                          InputIt2 first2, 
                          InputIt2 last2, 
                          OutputIt d_first, 
                          Compare comp = std::less<>{})
// 说明: 如果一个元素在第一个集合中出现 m 次，在第二个集合中出现 n 次，那么它在交集中会出现 min(m, n) 次。
    
// A - B
OutputIt set_difference(InputIt1 first1, 
                        InputIt1 last1, 
                        InputIt2 first2, 
                        InputIt2 last2, OutputIt d_first,
                        Compare comp = std::less<>{})
// 用途: 计算两个已排序范围的差集，即存在于第一个范围 [first1, last1) 但不存在于第二个范围 [first2, last2) 的元素
// 说明: 如果一个元素在第一个集合中出现 m 次，在第二个集合中出现 n 次，那么它在差集中会出现 max(0, m - n) 次。

// (A U B) - (A ∩ B)。 
OutputIt 
set_symmetric_difference(InputIt1 first1, 
                         InputIt1 last1, 
                         InputIt2 first2, 
                         InputIt2 last2, 
                         OutputIt d_first, 
                         Compare comp = std::less<>{})
// 用途: 计算两个已排序范围的对称差集，即存在于第一个范围但不存在于第二个范围的元素，以及存在于第二个范围但不存在于第一个范围的元素。并将结果存储到从 d_first 开始的输出范围中。
// 说明: 如果一个元素在第一个集合中出现 m 次，在第二个集合中出现 n 次，那么它在对称差集中会出现 abs(m - n) 次。可以看作是 (A U B) - (A ∩ B)。 

/*==============================================*/
/*                   其它操作                    */
/*==============================================*/
  
OutputIt merge(InputIt1 first1, 
               InputIt1 last1, 
               InputIt2 first2, 
               InputIt2 last2, 
               OutputIt d_first, 
               Compare comp = std::less<>{})
// 说明: merge 是稳定的，即如果两个范围中有相等的元素，它们在输出范围中的相对顺序会保持它们在原输入范围中的相对顺序（来自第一个范围的元素会先于来自第二个范围的相等元素）。你需要确保输出范围 d_first 有足够的空间容纳两个输入范围的所有元素。
    
bool includes(InputIt1 first1, 
              InputIt1 last1, 
              InputIt2 first2, 
              InputIt2 last2, 
              Compare comp = std::less<>{})
// 用途: 检查一个已排序的范围 [first2, last2) 是否是另一个已排序范围 [first1, last1) 的子序列。也就是说，第二个范围中的所有元素是否都以相同的相对顺序出现在第一个范围中。

void inplace_merge(BidirectionalIt first, 
                   BidirectionalIt middle, 
                   BidirectionalIt last, 
                   Compare comp = std::less<>{})
// 用途: 合并一个范围 [first, last) 内的两个连续的、已排序的子序列 [first, middle) 和 [middle, last)，使得整个范围 [first, last) 变成一个单一的有序序列。合并是就地的（in-place），但可能会使用临时内存。
// 说明: inplace_merge 也是稳定的。虽然名为 "in-place"，但在无法分配额外内存的情况下，其时间复杂度可能较高 (O(N log N))；如果可以分配额外内存，则通常为 O(N)。常用于归并排序的实现。
```

一般用于有序区间，虽然它们不要求：

* 通过 **相等** 来判断两个值是否 **相同**

``` cpp
ForwardIt std::unique(ForwardIt first, 
                      ForwardIt last, 
                      BinaryPredicate p = std::equal_to<>{})
// 用途 (Purpose): std::unique 用于从一个已排序（或者至少是重复元素都相邻排列）的范围 [first, last) 中移除连续的重复元素。它通过将不重复的元素前移来覆盖后续的重复元素。
// 返回值：返回一个指向新的逻辑末尾的迭代器。也就是说，范围 [first, returned_iterator) 内包含了所有唯一的元素（根据谓词 p 判断的连续唯一性）。从 returned_iterator到 last 之间的元素处于有效但未指定的状态。
// 稳定性：std::unique 是稳定的，这意味着它保留了非重复元素之间的原始相对顺序。
    
OutputIt unique_copy(InputIt first, 
                     InputIt last, 
                     OutputIt d_first, 
                     BinaryPredicate p = std::equal_to<>{})
// 返回值: 返回一个指向复制到目标范围中的最后一个元素的下一个位置的迭代器。
```

### 2. 确保算法和容器的排序行为一致

算法无法得到容器的排序行为，例如 `binary_search` 默认容器元素是升序的，但如果我们的容器是按照降序排列的，那么对该容器执行 `binary_search` 的结果就不正确了。

## `[*]`item35: 通过 mismatch 或 lexicographical 比较实现简单的忽略大小写字符串比较

忽略大小写字符串比较要么真的简单要么真的困难，依赖于你要多一般地解决这个问题。如果你忽略国际化问题而且只关注于设计成字符串 `strcmp` 那样的类型，这个任务很简单。如果你要有 `strcmp` 不具有的按语言处理字符串中的字符的能力（也就是，容纳文本的字符串是除了英语以外的语言）或程序使用了区域设置而不是默认的，这个任务很困难。

## item36: 了解 copy_if 的正确实现

现在，你无需实现自己的 `copy_if` 了，C++11 引入了 `std::copy_if`。

``` cpp
template<class InputIt, class OutputIt, class UnaryPredicate>
OutputIt copy_if(InputIt first, InputIt last,
                 OutputIt d_first,
                 UnaryPredicate pred);

// possible implement of std::copy
template<class Input, class Output>
Output copy(Input first, Input last, Output d_first) {
	for(; first != last; ++ first, ++ d_first) {
        *d_first = *first;
    }
    return d_first;
}

template<class Input, class Output, class UnaryPred>
Output copy_if(Input first, Input last, 
            Output d_first, UnaryPred pred) {
	for(; first != last; ++ first) {
        if(pred(*first)) {
            *d_first ++ = *first;
        }
    }
    return d_first;
}
```

**Notes**

* In practice, implementations of `std::copy` avoid multiple assignments and use bulk copy functions such as [std::memmove](https://en.cppreference.com/w/cpp/string/byte/memmove) if the value type is [*TriviallyCopyable*](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable) and the iterator types satisfy [*LegacyContiguousIterator*](https://en.cppreference.com/w/cpp/named_req/ContiguousIterator).

* ⭐When copying overlapping ranges, `std::copy` is appropriate when copying to the left (beginning of the destination range is outside the source range) while `std::copy_backward` is appropriate when copying to the right (end of the destination range is outside the source range).

## item37: 用 accumulate 或 for_each 来统计区间

### 1. accumulate

#### 函数原型

``` cpp
template< class InputIt, class T >
T accumulate( InputIt first, InputIt last, T init );

template< class InputIt, class T, class BinaryOp >
T accumulate( InputIt first, InputIt last, T init, BinaryOp op );
```

Computes the sum of the given value `init` and the elements in the range `[first, last)`

1) initializes the accumulator `acc` (of type `T`) with the initial value `init` and then modifies it with `acc = acc + *i(until C++20)acc = std::move(acc) + *i(since C++20)` for every iterator `i` in the range `[first, last)` in order.
2) Initializes the accumulator `acc` (of type `T`) with the initial value `init` and then modifies it with `acc = op(acc, *i)(until C++20)acc = op(std::move(acc), *i)(since C++20)` for every iterator `i` in the range `[first, last)` in order.

``` cpp
template <class InputIterator, class T, class BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op) {
    T result = init;
    for (; first != last; ++first) {
        result = binary_op(result, *first);
    }
    return result;
}
```

#### 接收输入迭代器

`accumulate` 只需要 <font color=blue>输入迭代器</font>，所以你甚至可以使用 `istream_iterator` 和 `istreambuf_iterator`：

``` cpp
cout << "The sum of the ints on the standard input is: " 
    << accumulate(istream_iterator<int>(cin), istream_iterator<int>(), 0) 
    << endl;
```

#### 自定义谓词

如果我们要自定义 `op`，其函数形式为：

``` cpp
T op(T current_accumulate_value, *InputIt element)
```

例如：

``` cpp
// 统计 vector<string> 中所有字符串的长度之和
string::size_type stringLengthSum(string::size_type sumSoFar, const string &s) {
    return sumSoFar + s.length();
}

int main()
{
    vector<string> v{"hello", "world", "java", "python"};
    string::size_type len = accumulate(v.begin(), v.end(), 0, stringLengthSum); // 20
    return 0;
}
```

### 2. for_each

#### 函数原型

``` cpp
template< class InputIt, class UnaryFunc >
UnaryFunc for_each(InputIt first, 
                   InputIt last, 
                   UnaryFunc f );


// possible implement
template<class InputIt, class UnaryFunc>
constexpr UnaryFunc for_each(InputIt first, 
                             InputIt last, 
                             UnaryFunc f)
{
    for (; first != last; ++first)
        f(*first);
 
    return f; // implicit move since C++11
}
```

值得注意的是，`for_each` 返回类型是 `UnaryFunc`。

#### 为什么引入 for_each

``` cpp
struct Point{
    Point(double initx, double inity) :x(initx), y(inity){};
    double x;
    double y;
};

class PointAverage{
public:
    PointAverage() :xSum(0), ySum(0), numPoints(0){};
    
    const Point operator()(const Point& avgSoFar, const Point& p)
    {
        ++ numPoints;
        xSum += p.x;
        ySum += p.y;
        return Point(xSum / numPoints, ySum / numPoints);
    }

private:
    int numPoints;
    double xSum;
    double ySum;
};

int main()
{
    list<Point> lp = {
        {2, 6}, {1, 4}, {3, 5}
    };
    Point avg = accumulate(lp.begin(), lp.end(), Point(0, 0), PointAverage());
    cout << avg.x << ' ' << avg.y << endl;
    return 0;
}
```

关于这里的 `accumulate`，作者这样提到：

> 不过，`PointAverage`和标准的第 26.4.1 节第 2 段冲突，我知道你想 起来了，那就是禁止传给 `accumulate` 的函数中有 **<font color=blue>副作用</font>**。成员变量 `numPoints`、`xSum` 和 `ySum` 的修改造成了一个副作用，所以，技术上 讲，我刚展示的代码会导致结果未定义。实际上，很难想象它无法工 作，但当我这么写时我被险恶的语言律师包围着，所以我别无选择， 只能在这个问题上写出难懂的条文。

至于为什么 `accumulate` 不允许产生副作用，作者是这样解释的：

> 你可能想知道为什么 `for_each` 的函数参数允许有副作用，而`accumulate` 不允许。这是一个刺向 STL 心脏的探针问题。唉，尊敬的读者，有一 些秘密总是在我们的知识范围之外。为什么 `accumulate` 和`for_each` 之间有差别？我尚待听到一个令人信服的解释。

what can I say?😓

总而言之，如果 `accumulate` 的使用涉及副作用的话，用 `for_each` 代替就好了，`for_each` 听起来好像你只是要对区间的每个元素进行一些操作，而且应当是那个算法的主要应用。

``` cpp
struct Point{
    Point(double initx, double inity) :x(initx), y(inity){};
    double x;
    double y;
};

class PointAverage{
public:
    PointAverage() :xSum(0), ySum(0), numPoints(0){};

    void operator()(const Point& p)
    {
        ++numPoints;
        xSum += p.x;
        ySum += p.y;
    }

    Point result() const {
        return Point(xSum / numPoints, ySum / numPoints);
    }

private:
    int numPoints;
    double xSum;
    double ySum;
};

int main()
{
    list<Point> lp = {
        {2, 6}, {1, 4}, {3, 5}
    };
    
    // for_each 返回
    Point avg = for_each(lp.begin(), lp.end(), PointAverage()).result();
    cout << avg.x << ' ' << avg.y << endl;
    return 0;
}
```

当然这里对 `for_each` 的调用和对 `PointAverage` 的设计也是很巧妙地。我们将一个函数对象 `PointAverage()`（或者写为 `PointAverage{}` 更清晰一些） 传递给 `for_each`，`for_each` 会对每个 `[lp.begin(), lp.end())` 之间的元素调用该函数对象，因此元素信息就保存在了该临时函数对象当中。最后我们利用返回的函数对象得到我们想要的结果。

# 仿函数、仿函数类、函数等

## item38: 把仿函数类设计为用于值传递

C 语言中函数指针始终 **按值传递**，STL 保持这一传统，确保从 C 到 C++ 的平滑过渡。仿函数被设计为 "智能函数"，其传递方式应与基本类型（普通指针）一致，避免引入特殊规则。

因为函数对象以值传递和返回，你的任务就是确保当那么传递（也就 是拷贝）时你的函数对象行为良好。这暗示了两个东西：

1. 你的函数对象应该很小，否则它们的拷贝会很昂贵。
2. 你的函数对象必须单态——非多态，不能用虚函数。那是因为派生类对象以值传递代入基类类型的参数会造成切割问题。

而我们往往很难舍弃掉这两方面，函数对象比真的函数优越的的原因之一是仿函数可以包含你需要的所有状态。仿函数类如果缺少继承就像 C++ 缺少 “++”。

为了解决该问题，一个有效的解决方案是 `Pimpl` 惯用法。

## item39:  用纯函数做判断式

### 1. 什么是纯函数

纯函数是函数式编程范式中的一个核心概念，它指的是满足以下两个主要条件的函数：

1. **相同的输入，永远会得到相同的输出 (Same input, same output / Deterministic):**
   - 无论你调用一个纯函数多少次，只要传递给它的参数是相同的，那么它返回的结果也永远是相同的。
   - 这意味着函数的输出完全由其输入参数决定，不依赖于任何外部状态、全局变量、随机数生成器或者任何在函数执行期间可能发生变化的东西。
2. **没有可观察的副作用 (No observable side effects):**
   - 副作用是指函数在执行过程中，除了返回一个值之外，还对函数 <u>外部环境</u> 产生的 **任何可观察** 的交互或状态改变。
   - 纯函数在执行时不会修改任何外部状态，包括：
     - 修改全局变量或静态变量。
     - 修改传入的参数（如果参数是引用类型，比如对象或数组，纯函数不应修改其内容）。
     - 进行任何 I/O 操作，例如：
       - 打印到控制台 (console.log)。
       - 读写文件。
       - 进行网络请求 (HTTP calls)。
       - 查询或修改数据库。
     - 调用其他不纯的函数（如果一个函数调用了不纯的函数，它本身也很可能变得不纯）。
     - 抛出依赖于外部状态或非确定性因素的异常。
     - 获取当前时间或日期，或生成随机数（因为这些会使得相同输入产生不同输出）。

**纯函数的特性与优点：**

1. **可预测性 (Predictability):**
   - 由于输出仅由输入决定且没有副作用，纯函数的行为非常容易预测和理解。你知道只要输入不变，结果就不会变。
2. **可测试性 (Testability):**
   - 纯函数非常容易进行单元测试。你不需要搭建复杂的环境或模拟外部依赖，只需提供输入并断言输出是否符合预期即可。测试结果也是确定和可重复的。
3. **引用透明性 (Referential Transparency):**
   - 一个纯函数的调用可以用其返回值来替换，而不会改变程序的整体行为。这个特性使得代码更容易推理和分析。
   - 例如，如果 `add(2, 3)` 是一个纯函数并且返回 `5`，那么在任何表达式中出现的 `add(2, 3)` 都可以被 `5` 替换，而程序的逻辑不会改变。
4. **可缓存性/记忆化 (Memoization):**
   - 因为纯函数对于相同的输入总是产生相同的输出，所以它们的计算结果可以被缓存起来。如果函数再次以相同的参数被调用，可以直接返回缓存的结果，避免重复计算，从而提高性能。这种技术称为记忆化。
5. **并行性与并发性 (Concurrency and Parallelism):**
   - 纯函数天然适合并行计算。因为它们不依赖于共享状态，也不会修改外部状态，所以多个纯函数可以同时在不同的处理器核心上安全地执行，而不用担心竞态条件 (race conditions) 或死锁 (deadlocks) 等并发问题。
6. **可组合性 (Composability):**
   - 纯函数更容易组合起来构建更复杂的功能。由于它们是独立的、没有副作用的单元，你可以像搭积木一样将它们组合在一起，而不必担心一个函数的副作用会意外地影响另一个函数。
7. **代码可维护性和可读性 (Maintainability and Readability):**
   - 纯函数的逻辑通常更简单直接，因为它们只关注从输入到输出的转换。这使得代码更容易阅读、理解和维护。当出现 bug 时，也更容易定位问题，因为问题的范围被限制在函数内部。

**不纯函数的例子 (以及为什么不纯)：**

- 修改全局变量：

  ```javaScript
  let counter = 0;
  function incrementCounter() {
    counter++; // 副作用：修改了全局变量 counter
    return counter;
  }
  ```

- 进行 I/O 操作 (如打印到控制台)：

  ```javaScript
  function greet(name) {
    console.log("Hello, " + name); // 副作用：向控制台输出
    return "Hello, " + name;       // 即使返回值是确定的，但有副作用
  }
  ```

- 修改传入的对象参数：

  ```javaScript
  function addProperty(obj, key, value) {
    obj[key] = value; // 副作用：修改了传入的对象 obj
    return obj;
  }
  ```

- 依赖当前时间：

  ```javaScript
  function getCurrentTime() {
    return new Date(); // 每次调用返回值都可能不同，依赖外部状态（系统时间）
  }
  ```

**纯函数与函数式编程：**

纯函数是函数式编程 (Functional Programming, FP) 的基石。FP 强调使用纯函数、避免共享状态和可变数据。通过组合纯函数来构建应用程序，可以带来上述提到的诸多好处。

**实践中的考量：**

- **完全纯粹的程序是不现实的：** 任何有用的程序最终都需要与外部世界交互（例如，读取用户输入、显示输出、访问数据库等），这些操作本质上都是有副作用的。
- **策略是将副作用隔离：** 在实践中，函数式编程的策略通常是将大部分核心逻辑用纯函数来实现，并将副作用操作推到程序的边缘或特定的模块中进行管理。这样可以最大限度地保留纯函数带来的好处。
- **性能与纯度：** 有时，为了性能考虑，可能会选择性地引入一些受控的副作用或可变性（例如，在实现某些算法时，原地修改数据结构可能比每次都创建新数据结构更高效）。关键在于理解其中的权衡。

总而言之，纯函数是一种强大的编程概念，它通过限制函数的行为（只依赖输入，没有副作用）来换取代码的可预测性、可测试性和可维护性。虽然在实际应用中不可能所有函数都是纯函数，但尽可能地使用和隔离纯函数是提升软件质量的有效手段。

### 2. 书中的例子

``` cpp
class BadPredicate {
public:
    BadPredicate(): timesCalled(0) {} 

    bool operator()(const Widget&) {
        return ++ timesCalled == 3;
    }

private:
    size_t timesCalled;
}; 
```

注意这里的 `operator()`，它是一个 **非纯函数**，因为它修改了一个不属于 `operator()` 的变量 `timesCalled`。

不要误以为，`operator()` 和 `timesCalled` 同属于类 `BadPredicate` 的成员，`operator()` 就可以随便修改 `timesCalled` 了。

试想 `BadPredicate` 的 *lambda* 形式：

``` cpp
auto BadPredicate = [timesCalled = 0]() mutable -> bool {
    return ++ timesCalled == 3;
};
```

在 *lambda* 中，我们通过  **C++14 引入的初始化捕获 (init-capture)** 定义了一个变量 `timesCalled`。

显然的，这对于函数体来说属于 **外部状态**，它不是我们通过函数参数传入的变量。

## `[*]`item40: 使仿函数类可适配

## item41: 了解使用 `ptr_fun`, `mem_fun` 和 `mem_fun_ref` 的原因

> 在这里有必要统一说明一下条款 40 和 41，由于这本书写于 C++ 11 之前，而截至目前 $2025/5/16$，C++ 版本已经更新到 C++23 了，因此之前的很多特性都不在使用甚至被弃用或完全移除了，而 `ptr_fun`，`mem_fun`，`mem_fun_ref` 就是很好的例子。

`ptr_fun`、`mem_fun` 和 `mem_fun_ref` 是 C++ 标准库中较早版本（C++98/C++03）提供的函数适配器，主要<u>用于将 **普通函数指针** 或 **成员函数指针** 包装成 **函数对象**</u>，以便与 STL 算法等接受函数对象的组件协同工作。

然而，需要强调的是，**这些适配器在 C++11 中已被弃用，并在 C++17 中被完全移除**。现代 C++ 更推荐使用 <font color=blue>`std::function`、lambda 表达式以及 `std::bind`（尽管 `std::bind` 的使用也逐渐被 lambda 表达式取代，因为后者通常更简洁易懂）</font>。

尽管如此，了解它们有助于理解 C++ 函数对象和适配器的演进。下面分别介绍这三个适配器：

**1. `ptr_fun`**

- **用途**：`ptr_fun` 用于将一个普通的（非成员）函数指针包装成一个函数对象（仿函数）。<u>STL 算法通常期望接收函数对象作为参数，而直接传递函数指针在某些情况下可能无法满足模板推导或类型匹配的要求。</u>`ptr_fun` 就是为了解决这个问题。

- **工作原理**：它接受一个函数指针作为参数，并返回一个特定类型的函数对象（通常是 `std::pointer_to_unary_function` 或 `std::pointer_to_binary_function` 的实例，取决于原函数是一元还是二元函数）。这个返回的函数对象重载了 `operator()`，其内部会调用原始的函数指针。

- 示例

  ```cpp
  #include <iostream>
  #include <vector>
  #include <algorithm>
  #include <functional> // 需要包含 <functional>
  
  // 一个普通函数
  void print_int(int n) {
      std::cout << n << " ";
  }
  
  bool is_odd(int n) {
      return n % 2 != 0;
  }
  
  int main() {
      std::vector<int> v = {1, 2, 3, 4, 5};
  
      // 使用 ptr_fun 包装 print_int
      // 在 C++98/03 中，可以直接传递函数指针，但 ptr_fun 提供了显式的适配
      // std::for_each(v.begin(), v.end(), std::ptr_fun(print_int));
      // std::cout << std::endl;
  
      // 更常见的场景是用于需要特定函数对象类型的算法
      // 比如，remove_if 的第三个参数期望一个返回 bool 的一元谓词
      // auto it = std::remove_if(v.begin(), v.end(), std::ptr_fun(is_odd));
      // v.erase(it, v.end());
  
      // 现代 C++ 的做法 (无需 ptr_fun):
      std::for_each(v.begin(), v.end(), print_int); // 直接传递函数指针
      std::cout << std::endl;
  
      auto it = std::remove_if(v.begin(), v.end(), is_odd); // 直接传递函数指针
      // 或者使用 lambda 表达式
      // auto it = std::remove_if(v.begin(), v.end(), [](int n){ return n % 2 != 0; });
      v.erase(it, v.end());
  
      for (int x : v) {
          std::cout << x << " "; // 输出: 2 4
      }
      std::cout << std::endl;
  
      return 0;
  }
  ```

  <font color=blue>注意：可以发现，在现代 C++ 中，很多情况下可以直接传递函数指针给 STL 算法，因为模板推导通常能正确处理。</font>`ptr_fun` 的主要价值在于显式转换和处理更复杂的函数签名（尽管其能力有限）。

**2. `mem_fun`**

- **用途**：`mem_fun` 用于将一个**成员函数指针**包装成一个 **函数对象**。这个函数对象在被调用时，需要接收一个 **指向对象的指针** 作为其第一个参数，然后是成员函数本来的参数。

- **工作原理**：它接受一个成员函数指针作为参数，并返回一个特定类型的函数对象（例如 `std::mem_fun_t` 或 `std::const_mem_fun_t` 等的实例）。当这个函数对象被调用时，它会通过传入的对象指针来调用相应的成员函数。

- **适用场景**：当你有一个容器，其中存储的是**对象的指针**，并且你想对这些指针所指向的对象调用某个成员函数时。

- 示例

  ```cpp
  #include <iostream>
  #include <vector>
  #include <algorithm>
  #include <functional> // 需要包含 <functional>
  #include <string>
  
  class MyClass {
  public:
      MyClass(const std::string& s) : data_(s) {}
      void print()    const { std::cout << data_ << " "; }
      bool is_empty() const { return data_.empty(); }
  private:
      std::string data_;
  }; 
  
  int main() {
      std::vector<MyClass*> ptr_vec;
      
      ptr_vec.push_back(new MyClass("Hello"));
      ptr_vec.push_back(new MyClass(""));
      ptr_vec.push_back(new MyClass("World"));
  
      // 使用 mem_fun 适配 MyClass::print 成员函数
      // for_each 会将 ptr_vec 中的每个 MyClass* 指针传递给 mem_fun 返回的函数对象
      // 该函数对象内部会执行 ptr->print()
      std::for_each(ptr_vec.begin(), ptr_vec.end(), std::mem_fun(&MyClass::print));
      // std::cout << std::endl;
  
      // 使用 mem_fun 适配 MyClass::is_empty 成员函数
      // auto it = std::remove_if(ptr_vec.begin(), ptr_vec.end(), std::mem_fun(&MyClass::is_empty));
      // 从这里开始清理被 remove_if 标记的元素 (这里只是示意，实际清理需要 delete 对象)
      // for (auto p = it; p != ptr_vec.end(); ++p) {
      //     delete *p;
      // }
      // ptr_vec.erase(it, ptr_vec.end());
  
  
      // 现代 C++ 的做法 (使用 lambda 表达式):
      std::cout << "Modern C++ (pointers):" << std::endl;
      std::for_each(ptr_vec.begin(), ptr_vec.end(), [](MyClass* p){ p->print(); });
      std::cout << std::endl;
  
      auto it_modern = std::remove_if(ptr_vec.begin(), ptr_vec.end(), [](MyClass* p){ return p->is_empty(); });
      for (auto p_iter = it_modern; p_iter != ptr_vec.end(); ++p_iter) {
           delete *p_iter; // 清理被标记移除的对象
      }
      ptr_vec.erase(it_modern, ptr_vec.end());
  
      std::for_each(ptr_vec.begin(), ptr_vec.end(), [](MyClass* p){ p->print(); }); // 输出: Hello World
      std::cout << std::endl;
  
      // 清理剩余的对象
      for (MyClass* p : ptr_vec) {
          delete p;
      }
      ptr_vec.clear();
  
      return 0;
  }
  ```

**3. `mem_fun_ref`**

- **用途**：`mem_fun_ref` 与 `mem_fun` 非常相似，也是用于将成员函数指针包装成函数对象。不同之处在于，`mem_fun_ref` 返回的函数对象在被调用时，期望接收一个指向对象的**引用**（而不是指针）作为其第一个参数。

- **工作原理**：它接受一个成员函数指针作为参数，并返回一个特定类型的函数对象（例如 `std::mem_fun_ref_t` 或 `std::const_mem_fun_ref_t` 等的实例）。当这个函数对象被调用时，它会通过传入的对象引用来调用相应的成员函数。

- **适用场景**：当你有一个容器，其中存储的是**对象本身**（或者对象的引用包装器，如 `std::reference_wrapper`），并且你想对这些对象调用某个成员函数时。

- 示例

  ```cpp
  #include <iostream>
  #include <vector>
  #include <algorithm>
  #include <functional> // 需要包含 <functional>
  #include <string>
  
  class MyClass {
  public:
      MyClass(const std::string& s) : data_(s) {}
      void print() const {
          std::cout << data_ << " ";
      }
      bool is_empty() const {
          return data_.empty();
      }
  private:
      string data_;
  };
  
  int main() {
      std::vector<MyClass> obj_vec;
      obj_vec.push_back(MyClass("Apple"));
      obj_vec.push_back(MyClass(""));
      obj_vec.push_back(MyClass("Banana"));
  
      // 使用 mem_fun_ref 适配 MyClass::print 成员函数
      // for_each 会将 obj_vec 中的每个 MyClass 对象 (的引用) 传递给 mem_fun_ref 返回的函数对象
      // 该函数对象内部会执行 obj.print()
      std::for_each(obj_vec.begin(), obj_vec.end(), std::mem_fun_ref(&MyClass::print));
      // std::cout << std::endl;
  
      // 使用 mem_fun_ref 适配 MyClass::is_empty 成员函数
      // auto it = std::remove_if(obj_vec.begin(), obj_vec.end(), std::mem_fun_ref(&MyClass::is_empty));
      // obj_vec.erase(it, obj_vec.end());
  
      // 现代 C++ 的做法 (使用 lambda 表达式):
      std::cout << "Modern C++ (objects/references):" << std::endl;
      std::for_each(obj_vec.begin(), obj_vec.end(), [](const MyClass& obj){ obj.print(); });
      std::cout << std::endl;
  
      auto it_modern = std::remove_if(obj_vec.begin(), obj_vec.end(), [](const MyClass& obj){ return obj.is_empty(); });
      obj_vec.erase(it_modern, obj_vec.end());
  
      std::for_each(obj_vec.begin(), obj_vec.end(), [](const MyClass& obj){ obj.print(); }); // 输出: Apple Banana
      std::cout << std::endl;
  
      return 0;
  }
  ```

**总结与对比**

| 特性         | `ptr_fun`                               | `mem_fun`                                  | `mem_fun_ref`                                             |
| ------------ | --------------------------------------- | ------------------------------------------ | --------------------------------------------------------- |
| **适配对象** | 普通函数指针                            | 成员函数指针                               | 成员函数指针                                              |
| **调用方式** | `fun_obj(arg)` 或 `fun_obj(arg1, arg2)` | `fun_obj(p_obj, arg1, ...)` (p_obj 是指针) | `fun_obj(obj_ref, arg1, ...)` (obj_ref 是引用)            |
| **容器元素** | 无特定要求（取决于函数本身）            | 通常是对象指针 (`T*`)                      | 通常是对象本身 (`T`) 或引用 (`std::reference_wrapper<T>`) |
| **C++ 版本** | C++98/03 (C++11 弃用, C++17 移除)       | C++98/03 (C++11 弃用, C++17 移除)          | C++98/03 (C++11 弃用, C++17 移除)                         |

**现代 C++ 的替代方案**

如前所述，这些适配器在现代 C++ 中已不再推荐使用。主要的替代方案是：

1. **Lambda 表达式 (C++11 及更高版本)**：

   - 非常灵活和简洁，可以直接在需要函数对象的地方定义。

   - 可以捕获上下文变量。

   - 对于成员函数调用：

     ```cpp
     // 类似于 mem_fun
     std::for_each(ptr_vec.begin(), ptr_vec.end(), [](MyClass* p){ p->member_function(); });
     // 类似于 mem_fun_ref
     std::for_each(obj_vec.begin(), obj_vec.end(), [](const MyClass& obj){ obj.member_function(); });
     ```

   - 对于普通函数：

     ```cpp
     std::for_each(vec.begin(), vec.end(), [](int x){ global_function(x); });
     // 或者直接传递函数指针，如果签名匹配
     std::for_each(vec.begin(), vec.end(), global_function);
     ```

2. **`std::function` (C++11 及更高版本)**：

   - 一个通用的、多态的函数包装器。可以存储、复制和调用任何可调用目标（函数指针、lambda 表达式、`std::bind` 表达式、函数对象）。
   - 当你需要存储不同类型的可调用对象，或者需要一个统一的接口来处理它们时非常有用。

3. **`std::bind` (C++11 及更高版本)**：

   - 可以用于绑定参数到可调用对象，包括成员函数。

   - 对于成员函数，`std::bind`  的第一个参数是成员函数指针，第二个参数是对象（或对象指针/引用）。

     ```cpp
     // 类似于 mem_fun
     // std::for_each(ptr_vec.begin(), ptr_vec.end(), std::bind(&MyClass::print, std::placeholders::_1));
     // 类似于 mem_fun_ref
     // std::for_each(obj_vec.begin(), obj_vec.end(), std::bind(&MyClass::print, std::placeholders::_1));
     ```

   - `std::bind` 曾经很流行，但现在很多情况下 lambda 表达式更易读和维护。

总而言之，虽然 `ptr_fun`、`mem_fun` 和 `mem_fun_ref` 是 C++ 历史上重要的组成部分，帮助弥合了函数指针/成员函数指针与 STL 算法之间的差距，但在现代 C++ 编程中，你应该优先考虑使用 lambda 表达式和 `std::function`。

## item42: 确定 `less<T>` 表示 `operator<`

主要论点和解释如下：

1. **背景设定**：

   - 有一个 `Widget` 类，它有 `weight()` 和 `maxSpeed()` 成员函数。
   - `Widget` 的 `operator<` 通常被定义为按 `weight()` 排序，这是其“自然”排序方式。
   - `std::multiset` (以及其他有序关联容器) 默认使用 `std::less` 作为比较函数，而 `std::less` 默认情况下会调用相应类型的 `operator<`。

2. **一个“坏主意”**：

   - 当需要按 `maxSpeed()` 对 `Widget` 排序时，有人可能会想到去特化 `std::less<Widget>`，使其内部比较 `maxSpeed()` 而不是调用 `lhs.weight() < rhs.weight()`。
   - 文字中给出了这样一个特化 `std::less<Widget>` 的例子，并明确指出这是“非常坏的主意”。

3. **为什么特化 `std::less` 在此场景下是坏主意**：

   - **违反“最小惊讶原则” (Principle of Least Astonishment)**：C++程序员普遍期望 `std::less<T>` 的行为与 `T` 的 `operator<` 行为完全一致。如果特化 `std::less<Widget>` 使其行为不同于 `Widget::operator<`，那么 `std::multiset<Widget> widgets;` 这样的代码就会具有误导性——它看起来是按默认方式（即 `operator<`，按重量）排序，但实际上却是按最高速度排序。
   - **破坏了既有约定和预期**：就像程序员期望拷贝构造函数执行拷贝、`operator+` 执行加法一样，他们也期望 `std::less` 等同于 `operator<`。改变这种基本约定是不友好且低劣的做法。
   - **即使技术上允许，也不代表是好实践**：虽然C++允许用户为自定义类型特化 `std` 命名空间中的模板（例如为智能指针特化 `std::less` 以使其行为类似内建指针，如文中的 `boost::shared_ptr` 例子），但这通常是为了让自定义类型的行为更符合其“模仿对象”的自然行为，或者与内建类型的行为保持一致，而不是为了引入与基本操作符相悖的行为。`Widget` 的 `std::less` 特化用于 `maxSpeed` 并不属于这种情况，因为它改变了 `Widget` 类型固有比较方式的“公共认知”。

4. **正确的做法**：

   - 如果需要按非默认标准（如 `maxSpeed`）排序，应该创建一个**新的、名称清晰的仿函数类**（例如 `MaxSpeedCompare`）。

   - 然后在创建 `std::multiset` 时，将这个自定义的比较仿函数作为模板参数显式传入：

     ```cpp
     struct MaxSpeedCompare :
         public std::binary_function<Widget, Widget, bool> { // 这是C++03及更早版本的写法
         bool operator()(const Widget& lhs, const Widget& rhs) const {
             return lhs.maxSpeed() < rhs.maxSpeed();
         }
     };
     
     std::multiset<Widget, MaxSpeedCompare> widgets;
     ```

     (在现代C++中，通常不再需要继承 `std::binary_function`，可以直接定义仿函数或使用lambda表达式。)

   - 这样的代码清晰地表达了其意图，即创建一个按 `MaxSpeedCompare` 规则排序的 `Widget` 多重集合。

5. **结论**：

   - 不要通过修改 `std::less` 的特化版本来“戏弄”它，使其行为与对应类型的 `operator<` 不一致，这样做会误导其他程序员。
   - 如果使用 `std::less`（无论是显式还是隐式地作为默认比较器），请确保其行为与 `operator<` 一致。
   - 如果需要其他排序标准，就应该创建一个专门的、名称不为 `less` 的仿函数类，并显式使用它。

简单来说，这段文字强调的是代码的**清晰性、可维护性以及遵守普遍接受的编程约定**，以避免混淆和意外行为。程序员应该努力编写“符合直觉”的代码，而不是利用语言特性来创建令人困惑的捷径。

# 使用 STL 编程

## item43: 尽量用算法代替手写循环

### 1. 效率: 算法通常比程序员产生的循环更高效

#### (1) 避免重复计算

``` cpp
// 手写循环
for (list::iterator i = lw.begin(); i != lw.end(); /*每次循环都调用 lw.end()*/ ++i) {
    i->redraw();
}

// 算法
for_each(lw.begin(), lw.end(),          // lw.end() 只在这里被调用和求值一次
         mem_fun_ref(&Widget::redraw));
```

当然，你可能会说，STL 可能将 `begin()`、`end()` 这种函数 inline，并且编译器在编译时可能将重复计算提到循环外面等等，因此上面两种写法是等价的。然而，没有人保证一定会这么做，我们不应该依赖于外部的优化。

不过，我们稍微修改一下手写循环就可以避免 `lw.end()` 重复调用的问题：

``` cpp
for (list::iterator i = lw.begin(), j = lw.end();
     i != j; // 每次循环都调用 lw.end()
     ++i) {
    i->redraw();
}
```

因此说，这只是一个影响效率的次要方面。

#### (2) 基于底层特定实现的优化

一个影响效率的主要因素是库的实现者可以利用他们知道容器的具体实现的优势，用库的使用者无法采用的方式来优化遍历。比如，在 `deque` 中的对象通常存储在（内部的）一个或多个固定大小的数组上。基于指针的遍历比基于迭代器的遍历更快， 但只有库的实现者可以使用基于指针的遍历，因为只有他们知道内部数组的大小以及如何从一个数组移向下一个。一些 STL 容器和算法的实现版本将它们的 `deque` 的内部数据结构引入了 `account`，而且已经知道，这样的实现比“通常”的实现快 20%。 这点不是说 STL 实现这为 `deque`（或者其他特定的容器类型）做了优化，而是实现者对于他们的实现比你知道得更多，而且他们可以把这 些知识用在算法实现上。如果你避开了算法调用，而只喜欢自己写循 环，你就相当于放弃了得到任何他们所提供的特定实现优点的机会。

> deque 的迭代器遍历每次都需要判断是否到达了当前内存块的终点，而显然的，直接使用底层指针则不用。下面是 deque 迭代器的结构：
>
> ``` 
> +-----+-------+------+------+
> | cur | first | last | node |
> +-----+-------+------+------+
> ```
>
> 显然的，迭代器由一个内存块转向另一个内存块的逻辑比指针也复杂的多。

#### (3) 复杂度和精妙程度

除了最简单的几种情况，标准库（STL）算法所采用的计算机科学原理和具体实现，通常比一般 C++ 程序员自己编写的算法更为复杂和高效——有时甚至复杂得多。

* STL 算法的设计者和实现者通常是算法领域的专家。他们会选用或设计出在特定问题上表现最优或接近最优的算法。
* 这些算法可能融合了多种基础算法的优点，或者针对特定场景进行了深度优化，其复杂度和精妙程度往往超出了普通程序员在日常开发中从头设计的水平。

例如，`sort` 排序算法，其一般工作原理如下：

1. **快速排序 (QuickSort)**：算法首先尝试使用快速排序。快速排序在平均情况下具有 O(N log N) 的优秀性能，最好情况下有 O(N) 的性能，并且常数因子较小。
2. **堆排序 (HeapSort)**：为了防止快速排序在最坏情况下（例如，对于已经排序或反向排序的数据，或者不良的基准元选择）性能退化到 O(N²)，Introsort 会监控快速排序的递归深度。如果递归深度超过某个阈值（通常与 log N 相关），算法会切换到堆排序。堆排序保证了 O(N log N) 的最坏情况时间复杂度。
3. **插入排序 (Insertion Sort)**：当快速排序处理的子数组元素数量变得非常少（例如，小于16或32个元素）时，Introsort 会切换到插入排序。对于小规模或者基本有序的数据，插入排序非常高效，其常数因子小，且代码简单。

通过这种混合策略，Introsort 结合了这三种算法的优点：快速排序的平均情况高性能，堆排序的最坏情况性能保证，以及插入排序在小数组上的高效性。

### 2. 正确性: 写循环时比调用算法更容易产生错误 

STL 算法凝聚了计算机科学领域在算法设计上的智慧和最佳实践。它们由专家实现并经过严格测试和优化，所以出错的概率可以视为 0。你可以理解为，STL 永远不会出错。（应该😀

最经典的，在涉及 STL 时，指针、迭代器、引用失效的情况非常常见，你不一定总是能正确处理所有这些东西失效的情况，但 STL 算法可以。

例如，我们实现一个将数组 `nums` 中的所有元素的平方插入 `deque` 的头部的功能：

``` cpp
const int N = 10;
int nums[N] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
deque<int> q{1, 2, 3};

// 手写循环
auto beg = q.begin();
for(int i = 0; i < N; i ++ ) {
    beg = q.insert(q.begin(), nums[i] * nums[i]);
    ++ beg;
}

// 调用算法
transform(nums, nums + N, inserter(q, q.begin()), [](int x){return x * x;});
```

### 3. 可维护性: 算法通常使代码比相应的显式循环更干净、更直观

以上面的代码为例，对于手写循环和调用算法两个版本，何种方式更能直观的体现代码的功能？显然，是调用算法的版本。

## item44: 尽量用成员函数代替同名的算法

1. 效率
2. 对相同的定义：相等 还是 等价
3. 对于 map：
   1. 成员函数按 `key` 查找
   2. 算法按 `key,value` 查找

特殊的，对于 list，它的同名成员函数和算法所做的事情甚至经常不相同，例如：

* `std::remove` 不会实际的删除元素；`list::remove` 会删除元素

  ``` cpp
  list<int> l{1, 2, 3, 4, 5, 6, 7, 8, 9};
  
  remove_if(l.begin(), l.end(), [](int x){return x & 1;}); // 2 4 6 8 5 6 7 8 9 
  l.remove_if([](int x){return x & 1;}); // 2 4 6 8
  ```

* `std::merge` 不会修改源对象；`list::merge` 会修改源对象

  ``` cpp
  list<int> l1{2, 4, 6, 8};
  list<int> l2{1, 3, 5, 7};
  
  // 注意 merge 要求源对象有序
  
  l1.merge(l2);
  for(auto &x : l1) cout << x << ' '; cout << endl; // 1 2 3 4 5 6 7 8
  for(auto &x : l2) cout << x << ' '; cout << endl; // 
  
  list<int> l3;
  // std::back_inserter 是一个工厂函数，它为你制造并返回一个 std::back_insert_iterator 类的实例
  merge(l1.begin(), l1.end(), l2.begin(), l2.end(), back_inserter(l3)); 
  for(auto &x : l1) cout << x << ' '; cout << endl; // 2 4 6 8 
  for(auto &x : l2) cout << x << ' '; cout << endl; // 1 3 5 7
  for(auto &x : l3) cout << x << ' '; cout << endl; // 1 2 3 4 5 6 7 8
  ```

## item45: 注意 `count`、`find`、`binary_search`、`lower_bound`、`upper_bound` 和 `equal_range` 的区别

### 1. 如何定义相同？

一个非常重要又容易忽视的细节是，`count` 和 `find` 算法都用 **相等（equality）** 来搜索，而 `binary_search`、`lower_bound`、 `upper_bound` 和 `equal_range` 则用 **等价（equivalence）**。

> `count` 和 `find` 可以用于无序序列，而 `binary_search`、`lower_bound`、 `upper_bound` 和 `equal_range` 只能用于有序序列。

### 2. lower_bound/upper_bound vs. equal_range

`lower_bound` 返回一个迭代器，这个迭代器指向这个值的第一个拷贝（如果找到的话）或者可以插入这个值的位置（如果没找到）。因此，如果我们使用 `lower_bound` 判断是否查找到某个元素时，可能会写出下面的代码：

``` cpp
vector<int> v{/*...*/};
vector<int>::iterator it = lower_bound(v.begin(), v.end(), val);
if(it != v.end() && (*it == val)) { // found it
    // ...
}
else { // not found
    // ...
}
```

这是一个经典的错误，我们忽略了我们刚刚提到的内容，那就是 `lower_bound` 实际上是使用 **等价（`less<T>`）** 作为相同的判断依据的，但是在这里，对于 `*it` 和 `val` 的判断，我们使用了 **相等（`operator==`）**。

虽然在大多数情况下，相等和等价返回的结果是一致的，也即 `!less<T>(a,b) && !less<T>(b,a)` 等价于 `operator==(a,b)`，但我们无法保证这一点。因此我们就需要修改代码如下：

``` cpp
vector<int> v{/*...*/};
vector<int>::iterator it = lower_bound(v.begin(), v.end(), val, vectorComp{});
// 假设我们使用仿函数 vectorComp 作为 lower_bound 的排序规则
if(it != v.end() && vectorComp(*it, val)) { // found it
    // ...
}
else { // not found
    // ...
}
```

但是，这又引入了一个新问题，上面的代码并不是一个可维护性比较好的代码，试想一下，如果我们修改了 `lower_bound` 的排序规则为 `newVectorComp`，那么我们就需要同时修改 `if` 中对 `*it` 和 `val` 的等价判断，也即代码之间有耦合。这显然是一个很容易出错的地方，我们恐怕很容易就忘记修改 `if` 中的内容。

一个好的解决方案是使用 `equal_range` 代替 `lower_bound`：

``` cpp
vector<int> v{/*...*/};
auto [v_beg, v_end] = equal_range(v.begin(), v.end(), val, vectorComp{});
if(v_beg != v_end) { // found it
    // ...
}
else { // not found
    // ...
}
```

并且，`equal_range` 在效率上与 `equal_range` 相差不大（都是对数时间复杂度）的情况下，还可以通过 `advance(v_beg, v_end)` 为我们提供值为 `val` 的元素个数总数的功能。

### 3. upper_bound 结合 insert

在 STL 算法和成员函数中，`[c.]insert(iterator,val)` 一般都是在 `iterator` <u>之前</u>插入元素 `val`，也就与 `upper_bound` 非常适配，因为 `upper_bound` 返回的是第一个大于 `val` 的元素的位置 `p`，我们只需要在 `p` 之前插入 `val` 即可。

### 4. 在关联容器中查找元素第一次出现的位置

对于 `multi` 容器，如果不只有一个值存在，`find` 并不保证能识别出容器里的等于给定值的第一个元素；它只识别这些元素中的一个。如果你真的需要找到等于给定值的第一个元素，你应该使用 `lower_bound`，而且你必须手动的对第二部分做等价检测。

### 总结

| 查询操作                      | 无序区间                 | 有序区间                                | `std::set` 或 `std::map`         | `std::multiset` 或 `std::multimap`              |
| :---------------------------- | ------------------------ | --------------------------------------- | -------------------------------- | ----------------------------------------------- |
| 1. **值是否存在？**           | `std::find`              | `std::binary_search`/`std::equal_range` | `container.count()`              | `container.find()` 或 `container.count()`       |
| 2. **首个等于值的对象？**     | `std::find`              | `std::find`                             | `container.find()`               | `container.find()` 或 `container.lower_bound()` |
| 3. **首个 `>=` 值的对象？**   | `std::find_if`           | `std::lower_bound`                      | `container.lower_bound()`        | `container.lower_bound()`                       |
| 4. **首个 `>` 值的对象？**    | `std::find_if`           | `std::upper_bound`                      | `container.upper_bound()`        | `container.upper_bound()`                       |
| 5. **等于值的对象数量？**     | `std::count`             | `distance` 结合 `equal_range`           | `container.count()` (结果为0或1) | `container.count()`                             |
| 6. **所有等于值的对象范围？** | `std::find` (需迭代调用) | `std::equal_range`                      | `container.equal_range()`        | `container.equal_range()`                       |

可以发现，对于关联容器，我们要优先使用容器自带的函数。

## item46: 考虑使用函数对象代替函数做算法的参数

## item47: 避免产生只写代码

只写代码：很容易写，但很难读和理解。

该条款建议我们不要写出可读性太差的代码，因为可读性差往往意味着难以维护。而在软件的生命周期中，维护比开发花费多得多的时间。

## item48: 总是 `#include` 适当的头文件

对于头文件，你不能依赖于以下几点：

1. 依赖于 `<father>` 包含 `<son>`
2. 依赖于平台 A 默认 `#include <header1>`

因为对于以上两点，可能在平台 A 是行得通的，但是在平台 B 可能就不行了。因为在 B 平台下，`<father>` 可能不包含 `son`，也不默认 `#include <header1>`。

简而言之，就是我们的这种依赖不具有可移植性。而根据 Murphy 定律（坏事情可能发生时，一定发生），你的代码一定是不可移植的。

因此，用到了哪些标准库的组件，就要 `include` 相应的头文件。

## item49: 学习破解有关 STL 的编译器诊断信息

### 技巧1: 替换内容以简化文本

例如在涉及 `string` 的报错信息当中，`string` 会以 `std::__cxx11::basic_string<_CharT, _Traits, _Alloc>` 的形式出现，哇，这太长了，而且很多信息对我们来说可能并没有什么用，例如 `_Traits`、`_Alloc`。因此我们可以将 `std::__cxx11::basic_string<_CharT, _Traits, _Alloc>` 替换为 `string`，这样报错信息就清晰多了。

当然，如果可以的话，假设你是个高手，肉眼替换也是可行的😀。

### 技巧2: AI

🤔......

## item50: 让你自己熟悉有关 STL 的网站

### 1. SGI STL

"SGI STL" 指的是 **Silicon Graphics, Inc. (SGI) 公司开发和维护的 C++ 标准模板库（Standard Template Library, STL）的一个特定实现版本**。

SGI 的 STL 网站名列前茅，而且有很好的理由。它对 STL 的每个组件提供了全面的文档。对于很多程序员，不管他们使用的是哪个STL 平台，这个网站都是他们的在线参考手册。

SGI 网站为 STL 程序员提供了其它东西：一个可以自由下载的 STL 实现。这个实现只被移植到少数编译器，但广泛移植的 STLport 分发也是 基于 SGI 分发的，我马上要写更多关于 STLport 的东西。此外，STL 的 SGI 实现提供了可以让STL 编程更强大、更灵活而且更有趣的许多非标 准组件。

### 2. [STLport](http://www.stlport.org/)

STLport 的主要卖点是，它提供一个可以移植到超过 20 个编译器的 SGI STL 实现的*修改版本*。和 SGI 的库一样，STLport 可以免费下载。如果你写的代码必须在多个平台工作，你可以通过以 STLport 实现为标准并让你所有的编译器都使用它的方法来给自己节省一堆麻烦。 大多数 STLport 对 SGI 代码基础的修改都专注于改进移植性上，但 STLport 的 STL 也是我知道的唯一一个提供“调试模式”来帮助侦测 STL 的不正确用法的实现——可以编译但导致未定义的运行期行为的用法。

### 3. [Boost](https://www.boost.org/)

在 1997 年，当关闭通向 C++ 国际标准的道路的钟声响起时，一些人对他们提倡的库特性没有入选而感到失望。这些人中的一部分本身就是委员会的成员，所以他们开始在第二轮标准化期间为标准库的扩充打下基础。结果就是 Boost，一个任务是“提供免费、同行评议的 C++ 库。重点在于可以和 C++ 标准库配合良好的可移植库”的网站。在 任务后面是一个动机：一个库变成“现有实践”的程度，某人把它提交给未来标准的可能 性就增加了。提交一个库给 Boost.org 是建立现有实践的一种方法…… 换句话说，当一个库可能增加到标准 C++ 库时，Boost 把自己作为帮助区分好坏的鉴别机制。











































































































