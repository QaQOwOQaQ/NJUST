## 参考资料

[1] [小张的code世界](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MzI3MDQyMDE2OQ==&action=getalbum&album_id=1428452222471602177&subscene=159&subscene=178&scenenote=https%3A%2F%2Fmp.weixin.qq.com%2Fs%3F__biz%3DMzI3MDQyMDE2OQ%3D%3D%26mid%3D2247484672%26idx%3D1%26sn%3D24e65482539d85bea03079511a20d65f%26chksm%3Dead014a6dda79db0d17999009922d6c9db700f2c32bd11bbe6730200cfd023c854aa5977bd9c%26scene%3D178%26cur_album_id%3D1428452222471602177%26search_click_id%3D%23rd&nolastread=1#wechat_redirect)

[2] [侯捷 STL 源码剖析](https://www.youtube.com/watch?v=Edcwv38c7d4&list=PLTcwR9j5y6W2Bf4S-qi0HBQlHXQVFoJrP)

[3] TinySTL

* https://github.com/electronicarts/EASTL
* https://github.com/kraj/musl
* https://github.com/Alinshans/MyTinySTL

## STL 源码阅读约定

在正式开始看源码之前，需要先说明一点：STL 是一套标准接口，但源码实现不是唯一的。不同标准库实现为了 ABI、性能、调试模式、平台特性，会在内部类型、成员命名、内存布局和优化策略上存在差异。

所以我们在阅读源码时，最好先区分两件事：

1. **标准语义**：C++ 标准规定容器、算法、迭代器、复杂度、异常保证、失效规则等对外行为。
2. **实现细节**：libstdc++、libc++、MSVC STL、SGI STL、EASTL、TinySTL 等具体实现的内部结构。

本文主要以 **libstdc++/SGI STL 风格** 为主线，部分章节会借助 libc++ 或 TinySTL 的代码帮助理解。后面如果看到 `_M_xxx`、`_S_xxx` 这类命名，一般偏 libstdc++；看到 `__ptr__`、`__cntrl__` 这类命名，一般偏 libc++；看到 `__STL_BEGIN_NAMESPACE`、`stl_xxx.h` 这类代码，一般偏 SGI STL。

也就是说，源码中的内部结构可以帮助我们理解设计思想，但不能直接认为所有标准库实现都完全一致。

阅读 STL 源码时，我们可以重点抓住下面几条主线：

- **容器的核心数据结构**：连续空间、分段连续空间、链表、红黑树、哈希表。
- **内存管理**：allocator 分配原始内存，容器再用 placement new 构造对象。
- **迭代器模型**：不同容器暴露不同 category，算法通过 traits 获得能力。
- **算法分派**：根据 iterator category 和 type traits 选择更快实现。
- **异常安全**：构造一半失败时如何析构已构造对象并释放内存。
- **失效规则**：插入、删除、扩容、rehash 对 iterator/reference/pointer 的影响。

### STL 六大组件

经典 STL 可以分为六个组件：

| 组件 | 作用 | 典型例子 |
| --- | --- | --- |
| containers | 保存数据 | `vector`、`list`、`deque`、`set`、`map`、`unordered_map` |
| algorithms | 操作区间 | `sort`、`find`、`copy`、`lower_bound`、`unique` |
| iterators | 连接容器与算法 | `vector<T>::iterator`、`list<T>::iterator`、原生指针 |
| functors | 行为对象化 | `less<T>`、`greater<T>`、自定义比较器 |
| adapters | 包装已有组件形成新接口 | `stack`、`queue`、`priority_queue` |
| allocators | 内存分配与对象构造 | `std::allocator<T>`、`allocator_traits` |

从上面的关系可以发现，STL 中最核心的思想是：**算法不直接认识容器，只认识迭代器；容器不直接依赖算法，只提供迭代器区间；allocator 负责内存底层能力，traits 负责把类型能力萃取给上层使用。**

## `sort`

### 1、源码分析

STL 的 std::sort 函数是基于 Musser 在 1996 年提出的 **内省排序（Introspective sort）算法** 实现。这个算法是个缝合怪，它汲取了插入排序、堆排序以及快排的优点：

- 针对大数据量，使用快排，时间复杂度是 O(NlogN)；
- 若快排递归深度超过阈值 `__depth_limit` ，改用堆排序，防止快排递归过深，同时保持时间复杂度仍是 O(NlogN)；
- 当数据规模小于阈值 `_S_threshold` 时，改用插入排序。

STL 的 sort() 有如下两个版本，第一个版本默认按由小到大的顺序排序，第二个版本接受用户自定义一个仿函数作为排序标准。但是它们的共同点是都只接收两个 `_RandomAccessIterator`。

```cpp
template<typename _RandomAccessIterator>
inline void
sort(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    // concept requirements
    __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
    _RandomAccessIterator>)
    __glibcxx_function_requires(_LessThanComparableConcept<
    typename iterator_traits<_RandomAccessIterator>::value_type>)
    __glibcxx_requires_valid_range(__first, __last);
    __glibcxx_requires_irreflexive(__first, __last);

    std::__sort(__first, __last, __gnu_cxx::__ops::__iter_less_iter());
}

// 可以自定义排序原则
template<typename _RandomAccessIterator, typename _Compare>
inline void
sort(_RandomAccessIterator __first, _RandomAccessIterator __last,
     _Compare __comp)
{
    // concept requirements
    __glibcxx_function_requires(_Mutable_RandomAccessIteratorConcept<
    _RandomAccessIterator>)
    __glibcxx_function_requires(_BinaryPredicateConcept<_Compare,
    typename iterator_traits<_RandomAccessIterator>::value_type,
    typename iterator_traits<_RandomAccessIterator>::value_type>)
    __glibcxx_requires_valid_range(__first, __last);
    __glibcxx_requires_irreflexive_pred(__first, __last, __comp);

    std::__sort(__first, __last, __gnu_cxx::__ops::__iter_comp_iter(__comp));
}
```

底层都调用函数 `std::__sort()`，我们先整体看一下它的源代码：

```cpp
template<typename _RandomAccessIterator, typename _Compare>
inline void
__sort(_RandomAccessIterator __first, _RandomAccessIterator __last,
       _Compare __comp)
{
    if (__first != __last)
    {
        // 优先执行内省排序
        std::__introsort_loop(__first, __last,
            std::__lg(__last - __first) * 2,
            __comp);
        // 再执行插入排序
        std::__final_insertion_sort(__first, __last, __comp);
    }
}

// 对 std::__lg(__last - __first) * 2 的解读
// 这里利用 __lg 找到 2^k <= (__last - __first) 的最大值 k
// k 值用来控制分割恶化的情况，2k 为最大递归深度
```

可以看到首先进行 `std::__introsort_loop`，然后再接一个`std::__final_insertion_sort`。

`std::__introsort_loop()` 的源码分析如下，**introsort** 的意思是内省式排序，这里是递归地进行内省式排序：

```cpp
template<typename _RandomAccessIterator, typename _Size, typename _Compare>
void
__introsort_loop(_RandomAccessIterator __first,
     _RandomAccessIterator __last,
     _Size __depth_limit, _Compare __comp)
{
    // enum { _S_threshold = 16 };
    // 当元素个数少于 _S_threshold 时就进行内省排序
    while (__last - __first > int(_S_threshold))
    {
        // 当__depth_limit==0时快排递归深度达到限制
        if (__depth_limit == 0) // 分割恶化，改用堆排序
        {
            std::__partial_sort(__first, __last, __last, __comp);
            return;
        }
        -- __depth_limit;
        // 如果没有分割恶化，则直接按照快速排序法进行排序
        _RandomAccessIterator __cut =
          std::__unguarded_partition_pivot(__first, __last, __comp);
        // 对右半段递归进行内省式排序
        std::__introsort_loop(__cut, __last, __depth_limit, __comp);
        // 把__cut赋给__last，对左半段递归进行内省式排序
        __last = __cut;
    }
}
```

当元素个数少于 `_S_threshold` 时就直接调用 `std::__final_insertion_sort`：如果排序元素个数大于 16，则对两个子列分别按 `std::__insertion_sort` 和 `std::__unguarded_insertion_sort` 排序，都是插入排序；否则直接按插入排序。

```cpp
template<typename _RandomAccessIterator, typename _Compare>
void
__final_insertion_sort(_RandomAccessIterator __first,
     _RandomAccessIterator __last, _Compare __comp)
{
    if (__last - __first > int(_S_threshold))
    {
        std::__insertion_sort(__first, __first + int(_S_threshold), __comp);
        std::__unguarded_insertion_sort(__first + int(_S_threshold), __last,
            __comp);
    }
    else
        std::__insertion_sort(__first, __last, __comp);
}
```

### 2、总结

STL 的 sort 算法，数据量大时采用快速排序法分段递归；但是当分段的元素数量小于某一个门限时，为了避免快排的递归调用带来过大的额外负担，改用插入排序法。在数据量大时，如果发现递归层次过深，就会改用堆排序。

STL sort 中快排的实现如下所示：

```cpp
template<typename _RandomAccessIterator, typename _Compare>
inline _RandomAccessIterator
__unguarded_partition_pivot(_RandomAccessIterator __first,
    _RandomAccessIterator __last, _Compare __comp) // [__first,__last)
{
    // 计算中点
    _RandomAccessIterator __mid = __first + (__last - __first) / 2;
    // 从 [__first+1, __mid, __last-1] 的中间值作为主元位置并移动到 __first
    // 避免纯用第一个元素或中点作为主元时快速排序退化的情况（比如已经有序或基本有序时）
    std::__move_median_to_first(__first, __first + 1, __mid, __last - 1,
        __comp);
    // 排序 [__first+1,__last)
    return std::__unguarded_partition(__first + 1, __last, __first, __comp);
}


// __unguarded_partition
// hoare 分区法
template<typename _RandomAccessIterator, typename _Compare>
_RandomAccessIterator
__unguarded_partition(_RandomAccessIterator __first,
    _RandomAccessIterator __last,
    _RandomAccessIterator __pivot, _Compare __comp) // [__first,__last)
{
    while (true)
    {
        while (__comp(__first, __pivot)) // while(first < pivot)
            ++__first;
        --__last; // 左移 last 使其指向实际元素
        while (__comp(__pivot, __last))  // while(last > pivot)
            --__last;
        if (!(__first < __last))
            // 最后返回的 __first 是第一个 >= pivot 的位置
            return __first;
        std::iter_swap(__first, __last);
        ++__first;
    }
}
```

> `__unguarded` 这个名字字面意思就是：**“不带保护（无边界保护）的版本”**。
>
> 在 libstdc++ 里，经常会看到诸如：
>
> - `__unguarded_partition`
> - `__unguarded_linear_insert`
> - `__unguarded_insertion_sort`
>
> 这类函数名里的 `unguarded` 都有同一个含义： **内部循环不再做“是否到边界”的检查，假定调用方已经保证不会越界。**

从源码中我们发现，没有检查边界的代码：

```cpp
// 这两个迭代器的操作都没有检查边界
while (__comp(__first, __pivot)) ++__first;
while (__comp(__pivot, __last)) --__last;
```

这要从 `__unguarded_partition` 函数的入参说起：

```cpp
std::__unguarded_partition(__first + 1, __last, __first, __comp);
//入参分别为：__first+1, __last, __filst
// __first实际上就是__pivot

// 而__pivot是（__first + 1, __mid, __last - 1）三个的中值
std::__move_median_to_first(__first, __first + 1, __mid, __last - 1,
    __comp);
```

这样的话 first 一定不是最大的，一定有一个点满足 `first < pivot`。同样也会有一个点 `pivot < __last`。

---

### 3、使用陷阱

使用 std::sort 如果要自定义 `_Compare`，一定要符合严格 **弱序** 性质。否则在某些数据下会导致 core dump。因为 `__unguarded_partition` 中的 while 的停止条件是：

```cpp
 while (__comp(__first, __pivot)) ++__first;
 while (__comp(__pivot, __last)) --__last;
```

严格弱序在 `++__first` 或者 `--__last` 过程中不会越界。

如果为非严格弱序，就会越界，如下代码：

```cpp
#include <vector>
#include <algorithm>

int main(int argc, char const* argv[])
{

    std::vector<int> vec{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 
    std::sort(vec.begin(), vec.end(),
             [](const int& lhs, const int& rhs)
             {
                return lhs <= rhs;
             });
    return 0;
}
```

### 参考资料

[1] https://zhuanlan.zhihu.com/p/503795945

## 常用算法源码剖析

前面我们已经分析了 `sort`，下面再补充几个 STL 中非常常见的算法。`<algorithm>` 中的大部分算法都只接收迭代器区间，而不接收容器本身。这也是 STL 的核心设计：算法只依赖迭代器暴露出来的能力，再通过 `iterator_traits` 和 type traits 做分派。

### 0. 算法的基本约定

我们先明确一个最基本的约定：STL 算法几乎都使用半开区间：

```cpp
[first, last)
```

其含义是包含 `first` 指向的元素，不包含 `last` 指向的元素。使用半开区间有几个好处：

- 空区间可以表示为 `first == last`。
- 区间长度可以统一表示为 `last - first` 或 `distance(first, last)`。
- 多个连续区间容易拼接：`[a, b)` 与 `[b, c)`。

注意，算法通常不检查迭代器是否来自同一容器，也不检查区间是否有效。如果传入错误区间，就会导致未定义行为。

### 1. `find`

我们先看最简单的线性查找 `find`：

```cpp
template<class InputIt, class T>
InputIt find(InputIt first, InputIt last, const T& value)
{
    for (; first != last; ++first)
    {
        if (*first == value)
            return first;
    }
    return last;
}
```

从上面的实现可以发现，`find` 只要求 InputIterator，因此可以作用于 `vector`、`list`、`istream_iterator` 等。复杂度是 `O(N)`。

`find_if` 的思路也一样，只是把相等判断替换成谓词：

```cpp
template<class InputIt, class Pred>
InputIt find_if(InputIt first, InputIt last, Pred pred)
{
    for (; first != last; ++first)
    {
        if (pred(*first))
            return first;
    }
    return last;
}
```

### 2. `lower_bound` / `upper_bound` / `binary_search`

下面我们来看二分查找这一组函数。二分查找要求区间已经按同一个比较器有序。

`lower_bound` 返回第一个 `value <= *it` 不成立的位置，也就是第一个 `*it >= value` 的位置：

```cpp
template<class ForwardIt, class T, class Compare>
ForwardIt lower_bound(ForwardIt first, ForwardIt last,
                      const T& value, Compare comp)
{
    using difference_type =
        typename iterator_traits<ForwardIt>::difference_type;

    difference_type len = std::distance(first, last);

    while (len > 0)
    {
        difference_type half = len / 2;
        ForwardIt middle = first;
        std::advance(middle, half);

        if (comp(*middle, value)) // *middle < value
        {
            first = middle;
            ++first;
            len = len - half - 1;
        }
        else
        {
            len = half;
        }
    }
    return first;
}
```

注意这里模板参数是 ForwardIterator，不是 RandomAccessIterator。对 `vector` 来说 `advance` 是 `O(1)`；对 `list` 来说 `advance` 是 `O(N)`。因此虽然比较次数是 `O(logN)`，但非随机访问迭代器上的移动代价可能是 `O(N)`。

`upper_bound` 返回第一个 `value < *it` 的位置，即第一个 `*it > value`：

```cpp
if (!comp(value, *middle)) // value >= *middle
{
    first = middle;
    ++first;
    len = len - half - 1;
}
else
{
    len = half;
}
```

`binary_search` 通常基于 `lower_bound`：

```cpp
template<class ForwardIt, class T, class Compare>
bool binary_search(ForwardIt first, ForwardIt last,
                   const T& value, Compare comp)
{
    first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first);
}
```

判断相等依然不用 `==`，而是使用比较器的等价关系：

```cpp
!comp(a, b) && !comp(b, a)
```

### 3. `copy` / `copy_backward`

`copy` 把 `[first, last)` 复制到从 `result` 开始的目标区间：

```cpp
template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt result)
{
    for (; first != last; ++first, ++result)
        *result = *first;
    return result;
}
```

对于指针迭代器和 trivially copy assignable 类型，标准库通常会走 `memmove` 优化：

```cpp
if constexpr (is_pointer_v<InputIt> &&
              is_pointer_v<OutputIt> &&
              is_trivially_copy_assignable_v<T>)
{
    memmove(result, first, sizeof(T) * (last - first));
}
```

为什么是 `memmove` 而不是 `memcpy`？因为部分实现需要处理源目标区间重叠的情况，`memmove` 对重叠更安全。

`copy_backward` 从后往前复制，适合目标区间尾部与源区间重叠的场景：

```cpp
template<class BidirIt1, class BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 result)
{
    while (first != last)
        *--result = *--last;
    return result;
}
```

`vector::insert` 在已有空间中间插入元素时，经常使用 backward 方向移动尾部元素，避免覆盖还没移动的元素。

### 4. `move` / `move_backward`

`std::move` 算法与 `copy` 结构相似，只是赋值时把源元素转成右值：

```cpp
template<class InputIt, class OutputIt>
OutputIt move(InputIt first, InputIt last, OutputIt result)
{
    for (; first != last; ++first, ++result)
        *result = std::move(*first);
    return result;
}
```

`move_backward` 与 `copy_backward` 对称：

```cpp
while (first != last)
    *--result = std::move(*--last);
```

容器扩容时会选择 move 还是 copy，通常取决于元素类型是否满足 noexcept move。为了维持强异常保证，标准库常使用类似 `move_if_noexcept` 的策略：

```cpp
if constexpr (is_nothrow_move_constructible_v<T> ||
              !is_copy_constructible_v<T>)
    move elements;
else
    copy elements;
```

### 5. `fill` / `fill_n`

`fill` 把区间内每个元素赋成同一个值：

```cpp
template<class ForwardIt, class T>
void fill(ForwardIt first, ForwardIt last, const T& value)
{
    for (; first != last; ++first)
        *first = value;
}
```

`fill_n` 从 `first` 开始填充 `n` 个元素：

```cpp
template<class OutputIt, class Size, class T>
OutputIt fill_n(OutputIt first, Size n, const T& value)
{
    for (; n > 0; --n, ++first)
        *first = value;
    return first;
}
```

对 `char`、`unsigned char` 等字节类型，且填充值可直接按字节展开时，实现可能使用 `memset` 优化。

### 6. `remove` / `remove_if`

`remove` 并不会真正删除容器元素，因为算法只知道迭代器区间，不知道容器对象本身，无法调用容器的 `erase`。它做的是把“不需要删除”的元素向前搬移，并返回新的逻辑结尾。

```cpp
template<class ForwardIt, class T>
ForwardIt remove(ForwardIt first, ForwardIt last, const T& value)
{
    first = std::find(first, last, value);
    if (first == last)
        return first;

    ForwardIt result = first;
    ++first;

    for (; first != last; ++first)
    {
        if (!(*first == value))
        {
            *result = std::move(*first);
            ++result;
        }
    }
    return result;
}
```

因此要真正删除 `vector` 中的元素，需要 erase-remove idiom：

```cpp
vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
```

`remove_if` 只是把 `*first == value` 换成谓词 `pred(*first)`。

### 7. `unique`

`unique` 删除的是相邻重复元素，也只返回新的逻辑结尾，不改变容器大小。

```cpp
template<class ForwardIt, class BinaryPred>
ForwardIt unique(ForwardIt first, ForwardIt last, BinaryPred pred)
{
    if (first == last)
        return last;

    ForwardIt result = first;
    while (++first != last)
    {
        if (!pred(*result, *first))
        {
            ++result;
            if (result != first)
                *result = std::move(*first);
        }
    }
    return ++result;
}
```

常见组合：

```cpp
sort(vec.begin(), vec.end());
vec.erase(unique(vec.begin(), vec.end()), vec.end());
```

注意：`unique` 只处理相邻重复元素。如果区间没有先排序，它不会删除所有重复值。

### 8. `partition`

`partition` 根据谓词把区间分成两段：前半段满足谓词，后半段不满足谓词，但不保证相对顺序。

双向迭代器版本可以这样理解：

```cpp
template<class BidirIt, class Pred>
BidirIt partition(BidirIt first, BidirIt last, Pred pred)
{
    while (true)
    {
        while (first != last && pred(*first))
            ++first;
        if (first == last)
            return first;

        do
        {
            --last;
            if (first == last)
                return first;
        } while (!pred(*last));

        iter_swap(first, last);
        ++first;
    }
}
```

`stable_partition` 会保持相对顺序，通常需要额外缓冲区或递归旋转，代价更高。

### 9. `rotate`

`rotate(first, middle, last)` 把 `[first, middle)` 移到 `[middle, last)` 后面：

```text
[first, middle) [middle, last)
变成
[middle, last) [first, middle)
```

例如：

```cpp
vector<int> v{1, 2, 3, 4, 5};
rotate(v.begin(), v.begin() + 2, v.end());
// 3 4 5 1 2
```

不同 iterator category 下实现不同：

- ForwardIterator：通过循环交换区间。
- BidirectionalIterator：可用三次 reverse。
- RandomAccessIterator：可根据区间长度使用 juggling algorithm 优化。

三次 reverse 的思路：

```cpp
reverse(first, middle);
reverse(middle, last);
reverse(first, last);
```

### 10. `nth_element`

`nth_element(first, nth, last)` 保证：

- `nth` 位置上的元素就是完整排序后应处于该位置的元素。
- `[first, nth)` 中的元素都不大于 `*nth`。
- `[nth, last)` 中的元素都不小于 `*nth`。
- 两边区间内部不保证有序。

它通常使用 quickselect/introselect 思路，和 quicksort 类似做 partition，但每次只递归包含 `nth` 的那一边。

伪代码：

```cpp
while (last - first > threshold)
{
    cut = partition_around_pivot(first, last);
    if (cut <= nth)
        first = cut;
    else
        last = cut;
}
insertion_sort(first, last);
```

平均复杂度是 `O(N)`，非常适合求中位数、Top K 边界等问题。

### 11. `partial_sort`

`partial_sort(first, middle, last)` 只保证 `[first, middle)` 是全区间最小的 `middle-first` 个元素，并且这部分有序。

典型实现使用堆：

1. 对 `[first, middle)` 建立最大堆。
2. 扫描 `[middle, last)`。
3. 如果当前元素小于堆顶，就替换堆顶并调整堆。
4. 最后对堆执行 `sort_heap`。

伪代码：

```cpp
make_heap(first, middle, comp); // 最大堆

for (auto i = middle; i != last; ++i)
{
    if (comp(*i, *first))
        pop_heap_and_replace(first, middle, i, comp);
}

sort_heap(first, middle, comp);
```

复杂度约为 `O(N logK)`，其中 `K = middle - first`。

### 12. `stable_sort`

`stable_sort` 保证等价元素的相对顺序不变，通常基于归并排序：

1. 尝试申请临时缓冲区。
2. 有缓冲区时，使用归并排序，移动成本较低。
3. 无缓冲区时，使用原地归并，复杂度和常数都会更高。

相比 `sort`：

| 算法 | 是否稳定 | 典型实现 | 复杂度 |
| --- | --- | --- | --- |
| `sort` | 否 | introsort | `O(NlogN)` |
| `stable_sort` | 是 | merge sort | `O(NlogN)`，需要额外内存时更快 |

如果排序 key 相同的元素需要保持原始相对顺序，应使用 `stable_sort`。

### 13. 算法分派总结

STL 算法常见分派维度有两个：

1. **iterator category**
   - `advance` 对 RandomAccessIterator 走 `i += n`。
   - `distance` 对 RandomAccessIterator 走 `last - first`。
   - `rotate` 对不同迭代器类型有不同实现。

2. **type traits**
   - `copy` 对 trivial type 走 `memmove`。
   - `fill` 对字节类型可能走 `memset`。
   - `uninitialized_copy` 对 trivial type 可以退化为 `copy`。
   - 容器扩容根据 noexcept move 决定 move/copy。

这也是 STL 源码中大量 `__true_type`、`__false_type`、`iterator_category`、`is_trivial`、`is_nothrow_move_constructible` 的来源。

## `std::string`

希望读者在了解完 string 的源码实现后，能够搞清楚以下问题；

1. string 的常见的实现方式有几种？
2. string 类的内部结构是什么样子？
3. string 内部使用的内存是如何分配管理的？
4. string 是如何拷贝构造，如何析构的，有引用计数的概念吗？
5. string 都给我们提供了哪些关键的 API？
6. string 的 data() 和 c_str() 函数有什么区别？
7. std::to_string() 是如何实现的？

### 0. 字符串常量

在开始 string 的源码分析之前，我们需要明确一些和字符串相关的概念。首先就是字符常量与字符串常量。

#### 字符常量

字符常量指的是用单引号括起来的一个字符就是字符型常量。如 `'a'`, `'#'`, `'%'`, `'D'` 都是合法的字符常量，在内存中占一个字节。

* 字符常量只能包括一个字符，如 `'AB'` 是不合法的。
* 字符常量区分大小写字母，如 `'A'` 和 `'a'` 是两个不同的字符常量。
* 撇号 (`'`) 是定界符，而不属于字符常量的一部分。如 `cout << 'a';` 输出的是一个字母 "a"，而不是 3 个字符 `"'a'"`。

将一个字符常量存放到内存单元时，实际上并不是把该字符本身放到内存单元中去，而是将该字符相应的 ASCII 代码放到存储单元中。如果字符变量 `c1` 的值为 `'a'`，`c2` 的值为 `'b'`，则在变量中存放的是 `'a'` 的 ASCII 码 97，`'b'` 的 ASCII 码 98。

既然字符数据是以 ASCII 码存储的，它的存储形式就与整数的存储形式类似。这样，在 C++ 中字符型数据和整型数据之间就可以通用。一个字符数据可以赋给一个整型变量，反之，一个整型数据也可以赋给一个字符变量。也可以对字符数据进行算术运算，此时相当于对它们的 ASCII 码进行算术运算。

#### 字符串常量

字符串常量指的是用双引号括起来字符，字符串常量存储在<font color="blue">静态存储区</font>中，如 `"abc"`, `"Hello!"`, `"a+b"`, `"Li ping"` 等都是字符串常量。<font color=blue>编译系统会在字符串最后自动加一个  作为字符串结束标志。但  并不是字符串的一部分</font>，它只作为字符串的结束标志。例如，字符串常量 `"abc"` 在内存中占 4 个字节（而不是 3 个字节）。

> `\0` 只会增加字符串占用的字节数而不会增加字符串的大小，这样编译器偷偷添加的 `\0` 就用户来说就是透明的。

要区分如下两种写法。第一个是将静态存储区的 `"abc"` 的地址赋给了 `s1`，第二个是将静态存储区的 `"abc"` 赋值给了栈区的字符数组。

```cpp
char* s1 = "abc";
char s2[4] = "abc";
```

### 1. STL string 源码分析

string 的内容主要在 gcc 源码的三个文件中：`<string>`、`<basic_string.h>`、`<basic_string.tcc>`。我们首先要明白，STL 中的 string 是一个类型，我们用 string 声明的变量都是一个 string 类型的对象。因为 `<string>` 中对 string 是这样定义的：

```cpp
typedef basic_string<char>    string;

// 其中 basic_string 的定义如下
template<typename _CharT, typename _Traits = char_traits<_CharT>,
         typename _Alloc = allocator<_CharT> >
class basic_string;
```

看到了吧，我们使用的 `string` 其实是模板类 `basic_string` 的特化版本 `basic_string<char>`。对 string 的源码分析实际上就是对 `basic_string<char>` 的源代码分析。`basic_string` 源代码的简化版本如下：

```cpp
template<typename _CharT, typename _Traits, typename _Alloc>
class basic_string
{
    // 保存字符串对象的内容的地址
    _Alloc_hider  _M_dataplus;
    // 字符串对象内容的长度
    size_type    _M_string_length;
  
    // _S_local_capacity 是默认存储能力，存储数组的最大能存储字符数
    // 区别于 _M_string_length
    enum { _S_local_capacity = 15 / sizeof(_CharT) };
  
     /**
     * 这里有个小技巧，用了 union
     * 因为使用 _M_local_buf 时候不需要关注 _M_allocated_capacity
     * 使用 _M_allocated_capacity 时就不需要关注 _M_local_buf
     * 继续向下看就会明白。
     */
    union
    {
        _CharT    _M_local_buf[_S_local_capacity + 1];
        size_type _M_allocated_capacity;
    }
  
    // 其中 _Alloc_hider 的定义如下
    struct _Alloc_hider : allocator_type
    {
        _Alloc_hider(pointer __dat, const _Alloc& __a = _Alloc())
        : allocator_type(__a), _M_p(__dat) { }
  
        _Alloc_hider(pointer __dat, const _Alloc& __a)
        : allocator_type(__a), _M_p(__dat) { }
  
        // _M_p 指向实际的内容
        pointer _M_p;
    }
  
    // 几个比较重要的函数
    // 用来修改 _M_p 的指向
    void _M_data(pointer __p)
    { _M_dataplus._M_p = __p; }
    // 用来修改长度 _M_string_length
    void _M_length(size_type __length)
    { _M_string_length = __length; }
    // 用来返回 _M_p
    pointer _M_data() const
    { return _M_dataplus._M_p; }
    // 返回保存字符串内容的数组的地址
    pointer _M_local_data()
    { return pointer(_M_local_buf); }
    // 返回最大可以存储内容的长度
    void _M_capacity(size_type __capacity)
    { _M_allocated_capacity = __capacity; }
};
```

有了上面的基础后我们就可以来看构造函数了，string 有很多构造函数，也就有很多种构造方式。首先，看一下默认构造函数，是将 `_M_p` 指向了 `_M_local_buf`。

```cpp
// 默认构造函数，将开辟的数组的地址赋给 _M_p，数组的长度为 16
// 长度为 0
// string str; 这种方式就是调用这个构造函数
basic_string() ：_M_dataplus(_M_local_data())
{ _M_set_length(0); }

// _M_local_data() 函数的实现如下
const_pointer _M_local_data() const { 
    return std::pointer_traits<const_pointer>::pointer_to(*_M_local_buf); 
}
```

其他的构造函数如下，底层都是调用 `_M_construct` 函数，关于这个函数的解析请看第二节。

```cpp
// 拷贝构造函数
basic_string(const basic_string& __str)
: _M_dataplus(_M_local_data(),
    _Alloc_traits::_S_select_on_copy(__str._M_get_allocator()))
{ _M_construct(__str._M_data(), __str._M_data() + __str.length()); }

// 其他构造函数
basic_string(const basic_string& __str, size_type __pos, const _Alloc& __a = _Alloc())
: _M_dataplus(_M_local_data(), __a)
{
  const _CharT* __start = __str._M_data()
    + __str._M_check(__pos, "basic_string::basic_string");
  _M_construct(__start, __start + __str._M_limit(__pos, npos));
}

basic_string(const basic_string& __str, size_type __pos, size_type __n)
: _M_dataplus(_M_local_data())
{
  const _CharT* __start = __str._M_data()
    + __str._M_check(__pos, "basic_string::basic_string");
  _M_construct(__start, __start + __str._M_limit(__pos, __n));
}

// 使用 C 字符串进行 string 对象的构造
basic_string(const _CharT* __s, size_type __n, const _Alloc& __a = _Alloc())
: _M_dataplus(_M_local_data(), __a)
{ _M_construct(__s, __s + __n); }

basic_string(const _CharT* __s, const _Alloc& __a = _Alloc())
: _M_dataplus(_M_local_data(), __a)
{ _M_construct(__s, __s ? __s + traits_type::length(__s) : __s+npos); }

// 使用 n 个字符构造
basic_string(size_type __n, _CharT __c, const _Alloc& __a = _Alloc())
: _M_dataplus(_M_local_data(), __a)
{ _M_construct(__n, __c); }

// 使用列表初始化的方式
basic_string(initializer_list<_CharT> __l, const _Alloc& __a = _Alloc())
: _M_dataplus(_M_local_data(), __a)
{ _M_construct(__l.begin(), __l.end()); }
```

析构函数，实际就是释放 `_M_p` 所指向的内存。

```cpp
~basic_string()
{ _M_dispose(); }

void _M_dispose()
{
  /**
   * 如果当前指向的是本地内存那 15 个字节，则不需要释放
   * 如果不是，则需要使用 _M_destroy 去释放其指向的内存
   */
  if (!_M_is_local())
    _M_destroy(_M_allocated_capacity);
}

/**
 * 判断下当前内部指向的是不是本地内存
 * _M_local_data() 即返回 _M_local_buf 的地址
 */
bool _M_is_local() const { return _M_data() == _M_local_data(); }

void _M_destroy(size_type __size) throw()
{ _Alloc_traits::deallocate(_M_get_allocator(), _M_data(), __size + 1); }
```

赋值构造函数，STL 中有这样一句话：

> Propagating allocator cannot free existing storage so must deallocate it before replacing current allocator.
>
> 如果你的 allocator（分配器）会随着对象一起传播（propagating allocator），那么在把旧的 allocator 换成新的 allocator 之前，你必须把用旧 allocator 分配的内存先释放掉，因为新的 allocator 无法释放旧 allocator 分配的内存。
>
> 因为 不同 allocator 之间的内存管理不兼容。
>
> 举例：
>
> - 用 Allocator A 分配了 100 字节
> - 之后你想把容器的 allocator 换成 Allocator B
> - 但 Allocator B 不知道 Allocator A 是如何分配这块内存的
>   - 是怎么对齐的？
>   - 还有没有额外的 metadata？
>   - 是否经过池化？
> - 所以 B 无法安全 free A 的内存

```cpp
// Propagating allocator cannot free existing storage so must
// deallocate it before replacing current allocator.
basic_string& operator=(const basic_string& __str)
{
    // 底层都是调用 assign
    return this->assign(__str);
}

// assign
basic_string&
assign(const basic_string& __str)
{
   this->_M_assign(__str);
   return *this;
}

// 核心函数 _M_assign
template<typename _CharT, typename _Traits, typename _Alloc>
void basic_string<_CharT, _Traits, _Alloc>::
    _M_assign(const basic_string& __str)
{
    if (this != &__str)
    {
        const size_type __rsize = __str.length();
        const size_type __capacity = capacity();
  
        /**
         * 如果 capacity 不够用，需要进行重新分配
         */
        if (__rsize > __capacity)
        {
            size_type __new_capacity = __rsize;
            pointer __tmp = _M_create(__new_capacity, __capacity);
            _M_dispose();
            _M_data(__tmp);
            _M_capacity(__new_capacity);
        }

        /**
         * 将 __str 指向的内存拷贝到当前对象指向的内存上
         */
        if (__rsize)
            this->_S_copy(_M_data(), __str._M_data(), __rsize);

        _M_set_length(__rsize);
    }
}
```

移动构造函数的源代码分析如下：

```cpp
/**
 * 移动构造函数，其实就是把 src 指向的内存移动到了 dst 种
 */
basic_string(basic_string&& __str) noexcept : _M_dataplus(_M_local_data(), std::move(__str._M_get_allocator())) {
    if (__str._M_is_local()) {
        traits_type::copy(_M_local_buf, __str._M_local_buf, _S_local_capacity + 1);
    } else {
        _M_data(__str._M_data());
        _M_capacity(__str._M_allocated_capacity);
    }

    // Must use _M_length() here not _M_set_length() because
    // basic_stringbuf relies on writing into unallocated capacity so
    // we mess up the contents if we put a '\0' in the string.
    _M_length(__str.length());
    __str._M_data(__str._M_local_data());
    __str._M_set_length(0);
}
```

string 为我们提供的优美的 API 如下：

```cpp
basic_string&
replace(size_type __pos, size_type __n, const basic_string& __str)
{ return this->replace(__pos, __n, __str._M_data(), __str.size()); }

size_type
copy(_CharT* __s, size_type __n, size_type __pos = 0) const;

void swap(basic_string& __s) _GLIBCXX_NOEXCEPT;

const _CharT*
c_str() const _GLIBCXX_NOEXCEPT
{ return _M_data(); }

const _CharT*
data() const _GLIBCXX_NOEXCEPT
{ return _M_data(); }

size_type
find(const _CharT* __s, size_type __pos, size_type __n) const
_GLIBCXX_NOEXCEPT;

size_type
find_first_of(const basic_string& __str, size_type __pos = 0) const
_GLIBCXX_NOEXCEPT
{ return this->find_first_of(__str.data(), __pos, __str.size()); }

size_type
find_last_of(const basic_string& __str, size_type __pos = npos) const
_GLIBCXX_NOEXCEPT
{ return this->find_last_of(__str.data(), __pos, __str.size()); }

basic_string
substr(size_type __pos = 0, size_type __n = npos) const
{ return basic_string(*this,
          _M_check(__pos, "basic_string::substr"), __n);
}

int compare(const basic_string& __str) const
{
    const size_type __size = this->size();
    const size_type __osize = __str.size();
    const size_type __len = std::min(__size, __osize);

    int __r = traits_type::compare(_M_data(), __str.data(), __len);
    if (!__r)
        __r = _S_compare(__size, __osize);
    return __r;
}
```

最后看一下 `to_string` 的实现：

```cpp
inline string to_string(int __val) {
    return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, 4 * sizeof(int), "%d", __val);
}

inline string to_string(unsigned __val) {
    return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, 4 * sizeof(unsigned), "%u", __val);
}

inline string to_string(long __val) {
    return __gnu_cxx::__to_xstring<string>(&std::vsnprintf, 4 * sizeof(long), "%ld", __val);
}

template <typename _String, typename _CharT = typename _String::value_type>
_String __to_xstring(int (*__convf)(_CharT*, std::size_t, const _CharT*, __builtin_va_list), std::size_t __n,
                     const _CharT* __fmt, ...) {
    // XXX Eventually the result should be constructed in-place in
    // the __cxx11 string, likely with the help of internal hooks.
    _CharT* __s = static_cast<_CharT*>(__builtin_alloca(sizeof(_CharT) * __n));

    __builtin_va_list __args;
    __builtin_va_start(__args, __fmt);

    const int __len = __convf(__s, __n, __fmt, __args);

    __builtin_va_end(__args);

    return _String(__s, __s + __len);
}
```

### 2. string 底层函数详解

首先是 `_M_construct` 函数，一共有三个版本，我们只对第一个进行分析，后面了和第一个差不多一样。

```cpp
template<typename _CharT, typename _Traits, typename _Alloc>
template<typename _InIterator>
void
basic_string<_CharT, _Traits, _Alloc>::
  _M_construct(_InIterator __beg, _InIterator __end
              , std::input_iterator_tag)
{
    size_type __len = 0;
    size_type __capacity = size_type(_S_local_capacity);
  
    // 当长度小于默认的长度时，直接依次放进去
    while (__beg != __end && __len < __capacity)
    {
        _M_data()[__len++] = *__beg;
        ++__beg;
    }

    __try
    {
        while (__beg != __end)
        {
            if (__len == __capacity)
            {
                // Allocate more space.
                /**
                 * 就是在这里，当 string 内 capacity 不够容纳 len 个字符时，会使用 _M_create 去扩容
                 * 这里你可能会有疑惑，貌似每次 while 循环都会去重新使用 _M_create 来申请多一个字节的内存
                 * 但其实不是，_M_create 的第一个参数的传递方式是引用传递，__capacity 会在内部被修改，稍后会分析
                 */
                __capacity = __len + 1;
                pointer __another = _M_create(__capacity, __len);
                /**
                 * 把旧数据拷贝到新的内存区域，_M_data() 指向的是旧数据，__another 指向的是新申请的内存
                 */
                this->_S_copy(__another, _M_data(), __len);
                /**
                 * __M_dispose()
                 * 释放 _M_data() 指向的旧数据内存，如果是 _M_local_buf 则不需要释放，稍后分析
                 */
                _M_dispose();
                /**
                 * _M_data()
                 * 内部的指向内存的指针指向这块新申请的内存 __another，它的实现其实就是
                 * void _M_data(pointer __p) { _M_dataplus._M_p = __p; }
                 */
                _M_data(__another);
                // 更新 _S_local_capacity
                _M_capacity(__capacity);
            }
            _M_data()[__len++] = *__beg;
            ++__beg;
        }
    }
    __catch(...)
    {
        /**
         * 异常发生时，避免内存泄漏，会释放掉内部申请的内存
         */
        _M_dispose();
        __throw_exception_again;
    }
    // 最后更新 string 的长度 __len
    _M_set_length(__len);
}

template<typename _CharT, typename _Traits, typename _Alloc>
template<typename _InIterator>
void
basic_string<_CharT, _Traits, _Alloc>::
  _M_construct(_InIterator __beg, _InIterator __end
              , std::forward_iterator_tag)
{
    // NB: Not required, but considered best practice.
    if (__gnu_cxx::__is_null_pointer(__beg) && __beg != __end)
    std::__throw_logic_error(__N("basic_string::"
               "_M_construct null not valid"));

    size_type __dnew = static_cast<size_type>(std::distance(__beg, __end));

    if (__dnew > size_type(_S_local_capacity))
    {
        _M_data(_M_create(__dnew, size_type(0)));
        _M_capacity(__dnew);
    }

    // Check for out_of_range and length_error exceptions.
    __try
    {   this->_S_copy_chars(_M_data(), __beg, __end); }
    __catch(...)
    {
        _M_dispose();
        __throw_exception_again;
    }

    _M_set_length(__dnew);
}

template<typename _CharT, typename _Traits, typename _Alloc>
void
basic_string<_CharT, _Traits, _Alloc>::
  _M_construct(size_type __n, _CharT __c)
{
    if (__n > size_type(_S_local_capacity))
    {
        _M_data(_M_create(__n, size_type(0)));
        _M_capacity(__n);
    }

    if (__n)
        this->_S_assign(_M_data(), __n, __c);

    _M_set_length(__n);
}
```

`_M_create` 函数的源代码分析如下，该函数的作用是进行内部内存的配置：

```cpp
/**
 * @brief _M_create 表示申请新内存
 * @param __capacity 想要申请的内存大小，注意这里参数传递方式是引用传递，内部会改变其值
 * @param __old_capacity 以前的内存大小
 */
template <typename _CharT, typename _Traits, typename _Alloc>
typename basic_string<_CharT, _Traits, _Alloc>::pointer
basic_string<_CharT, _Traits, _Alloc>::_M_create(
    size_type& __capacity, size_type __old_capacity) {
    /**
     * max_size() 表示标准库容器规定的一次性可以分配到最大内存大小
     * 当想要申请的内存大小最大规定长度时，会抛出异常
     */
    if (__capacity > max_size()) std::__throw_length_error(__N("basic_string::_M_create"));

    /**
     * 这里就是常见的 STL 动态扩容机制，其实常见的就是申请为 __old_capacity 的 2 倍大小的内存，最大只能申请 max_size()
     * 注释只是说了常见的内存分配大小思想，不全是下面代码的意思，具体可以直接看下面这几行代码哈
     */
    if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
    {
        __capacity = 2 * __old_capacity;
        // Never allocate a string bigger than max_size.
        if (__capacity > max_size()) __capacity = max_size();
    }

    /**
     * 使用内存分配子去分配 __capacity+1 大小的内存，+1 是为了多存储个 \0
     */
    return _Alloc_traits::allocate(_M_get_allocator(), __capacity + 1);
}
```

关于 `std::string` 的分析就到这里，前面为了让您看源码看的更清晰，对代码添加了详细的注释，同时做了适当的删减，但一定是正确的源代码，大家可放心阅读。相信您看完上面的源码分析可以回答出文章开头那几个问题并有所收获。

### 3. string 的内存管理

一般情况下，我们会借助 STL string 对象去管理一个字符串字面量（例如：`string name = "XiaoZhang"`），那么 string 是怎么管理字面量 "XiaoZhang" 的呢？

#### string 类的内部结构

首先我们看一下 string 类的内部结构，通常包含四个成员：

* **_M_dataplus**：内含一个指针，指向所管理的“字符串”；
* **_M_string_length**：指示当前所管理的字符串的长度；
* **_S_local_capacity**：是一个常量，指示 string 对象能管理的“小”字符串长度——15个字符；
* **匿名 union**：为容量为 16 的 char 型数组 `_M_local_buf` 或当前能管理的字符串的最大长度 `_M_allocated_capacity`。

上面关于四个成员的描述，可能会让你感到困惑，不用去纠结，带着疑问继续往下看。

根据上述描述，在程序中声明一个 string 类型的变量 `str` 时，实际上我们在栈上开辟了如下内存：

#### 默认构造 `string str;`

如果是 `string str;` 的形式，我们调用如下构造函数：

```cpp
basic_string()
_GLIBCXX_NOEXCEPT_IF(is_nothrow_default_constructible<_Alloc>::value)
: _M_dataplus(_M_local_data())
{ _M_set_length(0); }
```

我们可以在 debug 模式下看一下 `str` 的结构，`_M_p` 指向的内存地址为 `0x72fe00`，`_M_allocated_capacity` 的值为 0。

**内存图解：** *(此处应有内存图示，显示 _M_p 指向 _M_local_data)*

如果只声明一个空的 string 对象的话，union 的对外表现就是 `_M_allocated_capacity`。

```cpp
union
{
    _CharT           _M_local_buf[_S_local_capacity + 1];
    size_type        _M_allocated_capacity;
};
```

#### 空字面量初始化 `string str = "";`

如果是 `string str = "";` 的形式，此时调用如下构造函数:

```cpp
basic_string(const _CharT* __s, const _Alloc& __a = _Alloc())
  : _M_dataplus(_M_local_data(), __a)
{ _M_construct(__s, __s ? __s + traits_type::length(__s) : __s+npos); }
```

我们可以在 debug 模式下看一下 `str` 的结构，`_M_p` 指向的内存地址为 `0x72fdf0`，和（0）中相比较少了 16 个字节，根据栈与数组的增长方式，union 的对外表现应该是 `_M_local_buf`。

union 的对外表现就是数组 `_M_local_buf[_S_local_capacity + 1]`。

#### 短字符串初始化 `string str = "abc";`

如果是 `string str = "abc";`，我们也是调用如下构造函数：

```cpp
basic_string(const _CharT* __s, const _Alloc& __a = _Alloc())
  : _M_dataplus(_M_local_data(), __a)
{ _M_construct(__s, __s ? __s + traits_type::length(__s) : __s+npos); }
```

**内存图解：** *(此处应有内存图示)*

字符串是使用栈空间管理的，将字符串拷贝到字符数组 `_M_local_buf` 中进行管理。debug 模式下 `str` 的结构如下，`_M_p` 依然指向 `_M_local_buf`，根据 union 的特性此时 `_M_allocated_capacity` 为随机值，地址依然为 `0x72fdf0`。

#### 长字符串与堆内存管理

此时，请大家想一下，如果 string `str` 管理的字符串长度大于 `_S_local_capacity` 后，怎么进行管理呢？是去扩 `_M_local_buf` 的容量吗？

如图所示（需脑补图示），`_M_p` 的指向变了，同时 `_M_allocated_capacity` 为 37，因此栈上的 `_M_local_buf` 已经被 `_M_allocated_capacity` 占用。所以当长度超过 `_S_local_capacity` 后不再使用栈来管理字符串。

实际上是使用 **堆** 来管理的，栈上的 `_M_local_buf` 退出舞台，让出位置给 `_S_local_capacity` 用例管理当前的 string 对象能管理的字符串的最大长度。

- **小串**：长度小于 `_S_local_capacity`。string 使用栈上的一个 c-string `_M_local_buf` 来管理。
- **大串**：长度大于 `_S_local_capacity`。string 通过堆上的一个 c-string 来管理，这个 c-string 的内存通过 `alloc` 来管理。

这实际上就是 C++11 标准之后使用的 **Small String Optimization (SSO)** 技术。栈上（本地）可保存 15 个（`_S_local_capacity` 常量）字节的字符，这是考虑到内存读写的效率问题。

### 4. 再谈 string 的初始化

日常使用时我们经常使用一个字符串字面量去初始化一个 string 对象，调用的是如下构造函数：

```cpp
basic_string(const _CharT* __s, const _Alloc& __a = _Alloc())
: _M_dataplus(_M_local_data(), __a)
{ _M_construct(__s, __s ? __s + traits_type::length(__s) : __s+npos); }
```

可以发现，STL 是将一个字符串字面量转成 c-string 传给 string 的构造函数。我们知道字符串字面量转成 c-string 通常会在 c-string 的末尾加一个 `\0` 作为结尾。string 实际也是用 c-string 管理字符串，那 `\0` 也会存在 string 对象内部的 c-string 中去吗？

“源码面前，了无秘密。” 我们来看一下源代码：构造函数使用 `(__s)` 和 `(__s + __s的长度)` 这个区间进行 string 对象的构造。即 `__s` 的首字符作为 begin 迭代器，`\0` 作为 end 迭代器。

### 5. 源码分析

`_M_construct` 的源码如下：

```cpp
template<typename _InIterator>
void _M_construct(_InIterator __beg, _InIterator __end)
{
    typedef typename std::__is_integer<_InIterator>::__type _Integral;
    _M_construct_aux(__beg, __end, _Integral());
}

// _M_construct_aux
template<typename _InIterator>
void _M_construct_aux(_InIterator __beg, _InIterator __end, std::__false_type)
{
    typedef typename iterator_traits<_InIterator>::iterator_category _Tag;
    _M_construct(__beg, __end, _Tag());
}

// _M_construct，iterator tag为std::forward_iterator_tag
template<typename _CharT, typename _Traits, typename _Alloc>
  template<typename _InIterator>
    void
    basic_string<_CharT, _Traits, _Alloc>::
    _M_construct(_InIterator __beg, _InIterator __end,
     std::forward_iterator_tag)
    {
// NB: Not required, but considered best practice.
if (__gnu_cxx::__is_null_pointer(__beg) && __beg != __end)
  std::__throw_logic_error(__N("basic_string::"
             "_M_construct null not valid"));

// 计算字符串长度，按照SSO进行管理
size_type __dnew = static_cast<size_type>(std::distance(__beg, __end));

if (__dnew > size_type(_S_local_capacity))
  {
    _M_data(_M_create(__dnew, size_type(0)));
    _M_capacity(__dnew);
  }

// Check for out_of_range and length_error exceptions.
// 如果为大串先分配新的内存，如果为小串直接copy到本地
__try
  { this->_S_copy_chars(_M_data(), __beg, __end); }
__catch(...)
  {
    _M_dispose();
    __throw_exception_again;
  }

_M_set_length(__dnew);
    }
```

如果为 **小串** 时直接执行 `_S_copy_chars(_M_data(), __beg, __end)`：

```cpp
template<class _Iterator>
static void _S_copy_chars(_CharT* __p, _Iterator __k1, _Iterator __k2)
{
    // 可以看出只是把'\0'之前的字符遍历赋值给了_M_data()
    for (; __k1 != __k2; ++__k1, (void)++__p)
        traits_type::assign(*__p, *__k1); // These types are off.
}
```

**结论：** 所以 `\0` 并不会被放进 string 对象。

如果为 **大串** 先分配内存再拷贝：

```cpp
// 使用堆内存
_M_data(_M_create(__dnew, size_type(0)));
// union表现为当前管理字符串的能力
_M_capacity(__dnew);
```

`_M_create` 的实现如下：

```cpp
template<typename _CharT, typename _Traits, typename _Alloc>
  typename basic_string<_CharT, _Traits, _Alloc>::pointer
  basic_string<_CharT, _Traits, _Alloc>::
  _M_create(size_type& __capacity, size_type __old_capacity)
  {
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 83.  String::npos vs. string::max_size()
    if (__capacity > max_size())
        std::__throw_length_error(__N("basic_string::_M_create"));

    // The below implements an exponential growth policy, necessary to
    // meet amortized linear time requirements of the library: see
    // [http://gcc.gnu.org/ml/libstdc++/2001-07/msg00085.html](http://gcc.gnu.org/ml/libstdc++/2001-07/msg00085.html).
    if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
    {
      __capacity = 2 * __old_capacity;
      // Never allocate a string bigger than max_size.
      if (__capacity > max_size())
        __capacity = max_size();
    }

    // NB: Need an array of char_type[__capacity], plus a terminating
    // null char_type() element.
    return _Alloc_traits::allocate(_M_get_allocator(), __capacity + 1);
  }
```

### 回答开头的问题

#### 1. **string 的常见实现方式有几种？**

主要有两种经典实现方式：

- **COW（Copy-On-Write，写时复制）**：多个 string 共享同一块内存，引用计数管理。只有当某个 string 被修改时才真正复制。
  ⚠️ **但在 C++11 后被弃用**，因为多线程下不安全（引用计数需原子操作，性能差），且 move 语义更优。
- **SSO（Small String Optimization，小字符串优化）**： 当字符串较短（如 ≤15 字节）时，直接存储在对象内部的栈缓冲区中，避免堆分配；长字符串则使用堆内存。
  ✅ **现代标准库（如 libstdc++、libc++）普遍采用 SSO**。

#### 2. **string 类的内部结构是什么样子？**

以 GCC libstdc++ 为例（C++11 后）：

```cpp
class basic_string {
    _Alloc_hider _M_dataplus;        // 包含 allocator + pointer _M_p
    size_type    _M_string_length;   // 当前字符串长度
    enum { _S_local_capacity = 15 }; // 小字符串最大长度（char）
    union {
        char     _M_local_buf[16];   // 栈上缓冲区（含 '\0'）
        size_type _M_allocated_capacity; // 堆分配时的容量
    };
};
```

- 若字符串长度 ≤15：`_M_p` 指向 `_M_local_buf`，union 表现为数组。
- 若 >15：`_M_p` 指向堆内存，union 表现为 `_M_allocated_capacity`。

#### 3. **string 内部使用的内存是如何分配管理的？**

- **小字符串（≤15 字节）**：直接拷贝到 `_M_local_buf`，**无需动态分配**，析构时不释放。
- **大字符串（>15 字节）**：
  - 使用 `allocator<char>::allocate(capacity + 1)` 在堆上分配（+1 用于末尾 `\0`）。
  - 扩容策略：通常为 **2 倍增长**（指数扩容），但不超过 `max_size()`。
  - 析构时调用 `deallocate` 释放堆内存。
- **异常安全**：所有可能抛异常的操作都包裹在 `try-catch` 中，失败时释放已分配内存。

#### 4. **string 是如何拷贝构造、析构的？有引用计数吗？**

- **拷贝构造**：
  - 深拷贝：无论大小，都重新分配内存并复制内容（**无共享**）。
  - 小串 → 小串：直接 memcpy 到本地 buffer。
  - 大串 → 大串：分配新堆内存并拷贝。
- **析构**：
  - 若指向堆内存（`!_M_is_local()`），则调用 `deallocate`。
  - 若是本地 buffer，则什么都不做。
- **引用计数？**
  ❌ **C++11 起标准禁止 string 使用 COW**，因此 **没有引用计数**。

#### 5. **string 提供了哪些关键 API？**

常用接口包括：

| 类别 | 函数                                                                 |
| ---- | -------------------------------------------------------------------- |
| 访问 | `c_str()`, `data()`, `operator[]`, `at()`                    |
| 容量 | `size()`, `length()`, `capacity()`, `empty()`                |
| 修改 | `assign()`, `append()`, `+=`, `clear()`, `resize()`        |
| 操作 | `substr()`, `replace()`, `swap()`, `copy()`                  |
| 查找 | `find()`, `rfind()`, `find_first_of()`, `find_last_not_of()` |
| 比较 | `compare()`                                                        |

> 所有接口保证：`c_str()` 和 `data()` 返回以 `\0` 结尾的 C 风格字符串（C++11 起 `data()` 也保证 `\0`）。

#### 6. **data() 和 c_str() 有什么区别？**

- **C++98/03**：
  - `c_str()`：返回以 `\0` 结尾的 const char*。
  - `data()`：返回原始数据指针，**不一定有 `\0`**。
- **C++11 及以后**：
  - **两者行为完全相同**！都返回以 `\0` 结尾的指针。
  - 标准要求 string 内部始终维护一个 `\0` 终止符（即使用户没显式添加）。

> 所以现代代码中，`data()` 和 `c_str()` 可互换使用。

#### 7. **std::to_string() 是如何实现的？**

底层调用 C 的格式化函数（如 `vsnprintf`）：

```cpp
string to_string(int val) {
    return __to_xstring<string>(&vsnprintf, 4 * sizeof(int), "%d", val);
}
```

其中 `__to_xstring`：

1. 用 `alloca` 在栈上分配临时 buffer（大小预估）。
2. 调用 `vsnprintf` 格式化到 buffer。
3. 用结果构造 string 并返回。

> 注意：**不是直接拼接字符**，而是依赖 C 标准库的数值转字符串能力。

### TinyString

```cpp
#include <iostream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <limits>
#include <string>

// ======================= String 实现 =======================

class String {
public:
    using size_type       = std::size_t;
    using value_type      = char;
    using reference       = char&;
    using const_reference = const char&;

    static constexpr size_type npos     = static_cast<size_type>(-1);
    static constexpr size_type SSO_SIZE = 15; // 15 chars + '\0'

    // 内部堆存储结构
    struct HeapStorage {
        char*     data_;
        size_type capacity_;
    };

    // 用一个带名字的 union 来存放 local / heap
    union Storage {
        char        local_[SSO_SIZE + 1];
        HeapStorage heap_;

        Storage() {}
        ~Storage() {}
    };

    // ===== 构造 / 析构 / 赋值 =====

    // 默认构造：空串，local 模式
    String() noexcept = default;

    // C 字符串构造
    String(const char* str) {
        if (!str) throw std::invalid_argument("null pointer");
        init_from_cstr(str, std::strlen(str));
    }

    // n 个字符构造
    String(size_type n, char c) {
        assign(n, c);
    }

    // 拷贝构造
    String(const String& other)
        : size_(other.size_), is_local_(other.is_local_) {
        if (other.is_local_) {
            std::memcpy(storage_.local_, other.storage_.local_, size_ + 1);
        } else {
            storage_.heap_.capacity_ = other.storage_.heap_.capacity_;
            storage_.heap_.data_     = new char[storage_.heap_.capacity_ + 1];
            std::memcpy(storage_.heap_.data_,
                        other.storage_.heap_.data_,
                        size_ + 1);
        }
    }

    // 移动构造
    String(String&& other) noexcept {
        move_from(std::move(other));
    }

    // 析构
    ~String() {
        if (!is_local_) {
            delete[] storage_.heap_.data_;
        }
    }

    // 拷贝赋值：copy-swap
    String& operator=(const String& other) {
        if (this != &other) {
            String tmp(other);
            swap(tmp);
        }
        return *this;
    }

    // 移动赋值
    String& operator=(String&& other) noexcept {
        if (this != &other) {
            if (!is_local_) {
                delete[] storage_.heap_.data_;
            }
            size_     = 0;
            is_local_ = true;
            storage_.local_[0] = '\0';

            move_from(std::move(other));
        }
        return *this;
    }

    // 赋值：C 字符串
    String& operator=(const char* str) {
        if (!str) throw std::invalid_argument("null pointer");
        return assign(str);
    }

    // 赋值：单字符
    String& operator=(char c) {
        return assign(1, c);
    }

    // ===== assign 系列 =====

    String& assign(const char* str) {
        if (!str) throw std::invalid_argument("null pointer");
        return assign(str, std::strlen(str));
    }

    String& assign(const char* str, size_type len) {
        if (!str) throw std::invalid_argument("null pointer");
        clear();
        init_from_cstr(str, len);
        return *this;
    }

    String& assign(size_type n, char c) {
        clear();
        if (n <= SSO_SIZE) {
            std::fill_n(storage_.local_, n, c);
            storage_.local_[n] = '\0';
            size_     = n;
            is_local_ = true;
        } else {
            allocate_buffer(n);
            std::fill_n(storage_.heap_.data_, n, c);
            storage_.heap_.data_[n] = '\0';
            size_          = n;
        }
        return *this;
    }

    // ===== 访问 =====

    constexpr const char* c_str() const noexcept {
        return is_local_ ? storage_.local_ : storage_.heap_.data_;
    }

    constexpr const char* data() const noexcept {
        return c_str();
    }

    char* data() noexcept {
        return is_local_ ? storage_.local_ : storage_.heap_.data_;
    }

    char& operator[](size_type pos) noexcept {
        return is_local_ ? storage_.local_[pos] : storage_.heap_.data_[pos];
    }

    const char& operator[](size_type pos) const noexcept {
        return is_local_ ? storage_.local_[pos] : storage_.heap_.data_[pos];
    }

    char& at(size_type pos) {
        if (pos >= size_) throw std::out_of_range("String::at");
        return (*this)[pos];
    }

    const char& at(size_type pos) const {
        if (pos >= size_) throw std::out_of_range("String::at");
        return (*this)[pos];
    }

    // ===== 容量 =====

    constexpr size_type size() const noexcept { return size_; }
    constexpr size_type length() const noexcept { return size_; }

    constexpr size_type capacity() const noexcept {
        return is_local_ ? SSO_SIZE : storage_.heap_.capacity_;
    }

    static constexpr size_type max_size() noexcept {
        return std::numeric_limits<size_type>::max() / 2;
    }

    constexpr bool empty() const noexcept { return size_ == 0; }

    // ===== 修改：clear / resize =====

    void clear() noexcept {
        if (!is_local_) {
            delete[] storage_.heap_.data_;
            is_local_ = true;
        }
        size_     = 0;
        storage_.local_[0] = '\0';
    }

    void resize(size_type n, char c = '\0') {
        if (n <= size_) {
            if (is_local_) storage_.local_[n] = '\0';
            else           storage_.heap_.data_[n]  = '\0';
            size_ = n;
            return;
        }

        if (n <= SSO_SIZE) {
            if (is_local_) {
                std::fill_n(storage_.local_ + size_, n - size_, c);
                storage_.local_[n] = '\0';
                size_     = n;
            } else {
                char* old = storage_.heap_.data_;
                std::memcpy(storage_.local_, storage_.heap_.data_, size_);
                std::fill_n(storage_.local_ + size_, n - size_, c);
                storage_.local_[n] = '\0';
                size_     = n;
                is_local_ = true;
                delete[] old;
            }
        } else {
            if (!is_local_ && n <= storage_.heap_.capacity_) {
                std::fill_n(storage_.heap_.data_ + size_, n - size_, c);
                storage_.heap_.data_[n] = '\0';
                size_          = n;
            } else {
                size_type old_cap = is_local_ ? SSO_SIZE : storage_.heap_.capacity_;
                size_type new_cap = grow_capacity(n, old_cap);
                char*     new_data = new char[new_cap + 1];

                char* src = is_local_ ? storage_.local_ : storage_.heap_.data_;
                std::memcpy(new_data, src, size_);
                std::fill_n(new_data + size_, n - size_, c);
                new_data[n] = '\0';

                if (!is_local_) delete[] storage_.heap_.data_;
                storage_.heap_.data_     = new_data;
                storage_.heap_.capacity_ = new_cap;
                size_           = n;
                is_local_       = false;
            }
        }
    }

    // ===== 拼接 append / operator+= =====

    String& operator+=(const String& other) {
        return append(other);
    }

    String& operator+=(const char* str) {
        return append(str);
    }

    String& operator+=(char c) {
        return append(1, c);
    }

    String& append(const String& str) {
        return append(str.data(), str.size());
    }

    String& append(const char* str) {
        if (!str) throw std::invalid_argument("null pointer");
        return append(str, std::strlen(str));
    }

    String& append(const char* str, size_type len) {
        if (!str) throw std::invalid_argument("null pointer");
        if (len == 0) return *this;

        size_type new_size = size_ + len;

        if (new_size <= SSO_SIZE && is_local_) {
            std::memcpy(storage_.local_ + size_, str, len);
            storage_.local_[new_size] = '\0';
            size_            = new_size;
            return *this;
        }

        size_type old_cap = is_local_ ? SSO_SIZE : storage_.heap_.capacity_;
        size_type new_cap = (new_size > old_cap)
                                ? grow_capacity(new_size, old_cap)
                                : old_cap;

        char* new_data = new char[new_cap + 1];
        char* src      = is_local_ ? storage_.local_ : storage_.heap_.data_;

        std::memcpy(new_data, src, size_);
        std::memcpy(new_data + size_, str, len);
        new_data[new_size] = '\0';

        if (!is_local_) delete[] storage_.heap_.data_;
        storage_.heap_.data_     = new_data;
        storage_.heap_.capacity_ = new_cap;
        size_           = new_size;
        is_local_       = false;

        return *this;
    }

    String& append(size_type n, char c) {
        if (n == 0) return *this;

        size_type new_size = size_ + n;

        if (new_size <= SSO_SIZE && is_local_) {
            std::fill_n(storage_.local_ + size_, n, c);
            storage_.local_[new_size] = '\0';
            size_            = new_size;
            return *this;
        }

        size_type old_cap = is_local_ ? SSO_SIZE : storage_.heap_.capacity_;
        size_type new_cap = (new_size > old_cap)
                                ? grow_capacity(new_size, old_cap)
                                : old_cap;

        char* new_data = new char[new_cap + 1];
        char* src      = is_local_ ? storage_.local_ : storage_.heap_.data_;

        std::memcpy(new_data, src, size_);
        std::fill_n(new_data + size_, n, c);
        new_data[new_size] = '\0';

        if (!is_local_) delete[] storage_.heap_.data_;
        storage_.heap_.data_     = new_data;
        storage_.heap_.capacity_ = new_cap;
        size_           = new_size;
        is_local_       = false;

        return *this;
    }

    // ===== 迭代器 =====

    char* begin() noexcept { return data(); }
    char* end() noexcept { return data() + size_; }

    const char* begin() const noexcept { return data(); }
    const char* end() const noexcept { return data() + size_; }

    const char* cbegin() const noexcept { return data(); }
    const char* cend() const noexcept { return data() + size_; }

    // ===== substr =====

    String substr(size_type pos = 0, size_type count = npos) const {
        if (pos > size_) {
            throw std::out_of_range("String::substr");
        }
        size_type rlen = std::min(count, size_ - pos);
        String    result;
        result.init_from_cstr(data() + pos, rlen);
        return result;
    }

    // ===== find 系列（正向）=====

    size_type find(const String& str, size_type pos = 0) const noexcept {
        return find(str.data(), pos, str.size());
    }

    size_type find(const char* s, size_type pos = 0) const {
        if (!s) throw std::invalid_argument("null pointer");
        return find(s, pos, std::strlen(s));
    }

    size_type find(const char* s, size_type pos, size_type count) const {
        if (!s && count != 0) throw std::invalid_argument("null pointer");
        if (pos > size_) return npos;
        if (count == 0) return pos;
        if (count > size_ - pos) return npos;

        const char* hay = data();
        for (size_type i = pos; i + count <= size_; ++i) {
            if (std::memcmp(hay + i, s, count) == 0) {
                return i;
            }
        }
        return npos;
    }

    size_type find(char ch, size_type pos = 0) const noexcept {
        if (pos >= size_) return npos;
        const char* hay = data();
        for (size_type i = pos; i < size_; ++i) {
            if (hay[i] == ch) return i;
        }
        return npos;
    }

    // ===== rfind 系列（反向查找）=====

    size_type rfind(const String& str, size_type pos = npos) const noexcept {
        return rfind(str.data(), pos, str.size());
    }

    size_type rfind(const char* s, size_type pos = npos) const {
        if (!s) throw std::invalid_argument("null pointer");
        return rfind(s, pos, std::strlen(s));
    }

    size_type rfind(const char* s, size_type pos, size_type count) const {
        if (!s && count != 0) throw std::invalid_argument("null pointer");
        if (count > size_) return npos;

        if (pos == npos || pos > size_ - count) {
            if (size_ < count) return npos;
            pos = size_ - count;
        }

        if (count == 0) {
            return std::min(pos, size_);
        }

        const char* hay = data();
        for (size_type i = pos + 1; i-- > 0; ) {
            if (std::memcmp(hay + i, s, count) == 0) {
                return i;
            }
            if (i == 0) break;
        }
        return npos;
    }

    size_type rfind(char ch, size_type pos = npos) const noexcept {
        if (size_ == 0) return npos;
        if (pos == npos || pos >= size_) pos = size_ - 1;

        const char* hay = data();
        for (size_type i = pos + 1; i-- > 0; ) {
            if (hay[i] == ch) return i;
            if (i == 0) break;
        }
        return npos;
    }

    // ===== find_first_of / find_last_of =====

    size_type find_first_of(const String& str, size_type pos = 0) const noexcept {
        return find_first_of(str.data(), pos, str.size());
    }

    size_type find_first_of(const char* s, size_type pos = 0) const {
        if (!s) throw std::invalid_argument("null pointer");
        return find_first_of(s, pos, std::strlen(s));
    }

    size_type find_first_of(const char* s, size_type pos, size_type count) const {
        if (!s && count != 0) throw std::invalid_argument("null pointer");
        if (pos >= size_ || count == 0) return npos;

        const char* hay = data();
        for (size_type i = pos; i < size_; ++i) {
            for (size_type j = 0; j < count; ++j) {
                if (hay[i] == s[j]) return i;
            }
        }
        return npos;
    }

    size_type find_first_of(char ch, size_type pos = 0) const noexcept {
        return find(ch, pos);
    }

    size_type find_last_of(const String& str, size_type pos = npos) const noexcept {
        return find_last_of(str.data(), pos, str.size());
    }

    size_type find_last_of(const char* s, size_type pos = npos) const {
        if (!s) throw std::invalid_argument("null pointer");
        return find_last_of(s, pos, std::strlen(s));
    }

    size_type find_last_of(const char* s, size_type pos, size_type count) const {
        if (!s && count != 0) throw std::invalid_argument("null pointer");
        if (size_ == 0 || count == 0) return npos;

        if (pos == npos || pos >= size_) pos = size_ - 1;

        const char* hay = data();
        for (size_type i = pos + 1; i-- > 0; ) {
            for (size_type j = 0; j < count; ++j) {
                if (hay[i] == s[j]) return i;
            }
            if (i == 0) break;
        }
        return npos;
    }

    size_type find_last_of(char ch, size_type pos = npos) const noexcept {
        return rfind(ch, pos);
    }

    // ===== insert 系列 =====

    String& insert(size_type pos, const String& str) {
        if (&str == this) {
            String tmp(str);
            return insert(pos, tmp.data(), tmp.size());
        }
        return insert(pos, str.data(), str.size());
    }

    String& insert(size_type pos, const char* s) {
        if (!s) throw std::invalid_argument("null pointer");
        return insert(pos, s, std::strlen(s));
    }

    String& insert(size_type pos, const char* s, size_type count) {
        if (pos > size_) throw std::out_of_range("String::insert");
        if (!s && count != 0) throw std::invalid_argument("null pointer");
        if (count == 0) return *this;

        size_type new_size = size_ + count;

        // 情况 1：SSO 下就地插入
        if (is_local_ && new_size <= SSO_SIZE) {
            std::memmove(storage_.local_ + pos + count,
                         storage_.local_ + pos,
                         size_ - pos);
            std::memcpy(storage_.local_ + pos, s, count);
            size_                  = new_size;
            storage_.local_[new_size] = '\0';
            return *this;
        }

        // 情况 2：堆上且容量足够
        if (!is_local_ && new_size <= storage_.heap_.capacity_) {
            std::memmove(storage_.heap_.data_ + pos + count,
                         storage_.heap_.data_ + pos,
                         size_ - pos);
            std::memcpy(storage_.heap_.data_ + pos, s, count);
            size_                     = new_size;
            storage_.heap_.data_[size_] = '\0';
            return *this;
        }

        // 情况 3：需要重新分配
        size_type old_cap = is_local_ ? SSO_SIZE : storage_.heap_.capacity_;
        size_type new_cap = (new_size > old_cap)
                                ? grow_capacity(new_size, old_cap)
                                : old_cap;

        char* new_data = new char[new_cap + 1];
        char* src      = is_local_ ? storage_.local_ : storage_.heap_.data_;

        std::memcpy(new_data, src, pos);
        std::memcpy(new_data + pos, s, count);
        std::memcpy(new_data + pos + count,
                    src + pos,
                    size_ - pos);

        new_data[new_size] = '\0';

        if (!is_local_) delete[] storage_.heap_.data_;
        storage_.heap_.data_     = new_data;
        storage_.heap_.capacity_ = new_cap;
        size_           = new_size;
        is_local_       = false;

        return *this;
    }

    String& insert(size_type pos, size_type count, char ch) {
        if (pos > size_) throw std::out_of_range("String::insert");
        if (count == 0) return *this;

        size_type new_size = size_ + count;

        if (is_local_ && new_size <= SSO_SIZE) {
            std::memmove(storage_.local_ + pos + count,
                         storage_.local_ + pos,
                         size_ - pos);
            std::fill_n(storage_.local_ + pos, count, ch);
            size_                  = new_size;
            storage_.local_[new_size] = '\0';
            return *this;
        }

        if (!is_local_ && new_size <= storage_.heap_.capacity_) {
            std::memmove(storage_.heap_.data_ + pos + count,
                         storage_.heap_.data_ + pos,
                         size_ - pos);
            std::fill_n(storage_.heap_.data_ + pos, count, ch);
            size_                     = new_size;
            storage_.heap_.data_[size_] = '\0';
            return *this;
        }

        size_type old_cap = is_local_ ? SSO_SIZE : storage_.heap_.capacity_;
        size_type new_cap = (new_size > old_cap)
                                ? grow_capacity(new_size, old_cap)
                                : old_cap;

        char* new_data = new char[new_cap + 1];
        char* src      = is_local_ ? storage_.local_ : storage_.heap_.data_;

        std::memcpy(new_data, src, pos);
        std::fill_n(new_data + pos, count, ch);
        std::memcpy(new_data + pos + count,
                    src + pos,
                    size_ - pos);

        new_data[new_size] = '\0';

        if (!is_local_) delete[] storage_.heap_.data_;
        storage_.heap_.data_     = new_data;
        storage_.heap_.capacity_ = new_cap;
        size_           = new_size;
        is_local_       = false;

        return *this;
    }

    String& insert(size_type pos, char ch) {
        return insert(pos, 1, ch);
    }

    // ===== compare / starts_with / ends_with =====

    int compare(const String& other) const noexcept {
        size_type minlen = size_ < other.size_ ? size_ : other.size_;
        int       res    = std::memcmp(data(), other.data(), minlen);
        if (res != 0) return res;
        if (size_ < other.size_) return -1;
        if (size_ > other.size_) return 1;
        return 0;
    }

    int compare(const char* s) const {
        if (!s) throw std::invalid_argument("null pointer");
        size_type len    = std::strlen(s);
        size_type minlen = size_ < len ? size_ : len;
        int       res    = std::memcmp(data(), s, minlen);
        if (res != 0) return res;
        if (size_ < len) return -1;
        if (size_ > len) return 1;
        return 0;
    }

    bool starts_with(const String& other) const noexcept {
        if (other.size_ > size_) return false;
        return std::memcmp(data(), other.data(), other.size_) == 0;
    }

    bool starts_with(const char* s) const {
        if (!s) throw std::invalid_argument("null pointer");
        size_type len = std::strlen(s);
        if (len > size_) return false;
        return std::memcmp(data(), s, len) == 0;
    }

    bool starts_with(char ch) const noexcept {
        return size_ > 0 && data()[0] == ch;
    }

    bool ends_with(const String& other) const noexcept {
        if (other.size_ > size_) return false;
        return std::memcmp(data() + (size_ - other.size_),
                           other.data(),
                           other.size_) == 0;
    }

    bool ends_with(const char* s) const {
        if (!s) throw std::invalid_argument("null pointer");
        size_type len = std::strlen(s);
        if (len > size_) return false;
        return std::memcmp(data() + (size_ - len), s, len) == 0;
    }

    bool ends_with(char ch) const noexcept {
        return size_ > 0 && data()[size_ - 1] == ch;
    }

    // ===== swap / 比较运算符 =====

    void swap(String& other) noexcept {
        if (this == &other) return;

        if (is_local_ && other.is_local_) {
            char tmp[SSO_SIZE + 1];
            std::memcpy(tmp, storage_.local_, SSO_SIZE + 1);
            std::memcpy(storage_.local_, other.storage_.local_, SSO_SIZE + 1);
            std::memcpy(other.storage_.local_, tmp, SSO_SIZE + 1);
        } else if (!is_local_ && !other.is_local_) {
            std::swap(storage_.heap_.data_,     other.storage_.heap_.data_);
            std::swap(storage_.heap_.capacity_, other.storage_.heap_.capacity_);
        } else {
            String tmp(std::move(*this));
            *this = std::move(other);
            other = std::move(tmp);
        }
        std::swap(size_,     other.size_);
        std::swap(is_local_, other.is_local_);
    }

    friend void swap(String& a, String& b) noexcept {
        a.swap(b);
    }

    bool operator==(const String& other) const noexcept {
        return size_ == other.size_
            && std::memcmp(c_str(), other.c_str(), size_) == 0;
    }

    bool operator!=(const String& other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const String& other) const noexcept {
        return compare(other) < 0;
    }

    bool operator>(const String& other) const noexcept {
        return compare(other) > 0;
    }

    bool operator<=(const String& other) const noexcept {
        return compare(other) <= 0;
    }

    bool operator>=(const String& other) const noexcept {
        return compare(other) >= 0;
    }

private:
    size_type size_{0};
    bool      is_local_{true}; // true 表示用 local_，false 表示用堆
    Storage   storage_;

    void allocate_buffer(size_type min_cap) {
        size_type base_cap       = SSO_SIZE;
        storage_.heap_.capacity_ = grow_capacity(min_cap, base_cap);
        storage_.heap_.data_     = new char[storage_.heap_.capacity_ + 1];
        is_local_                = false;
    }

    size_type grow_capacity(size_type min_cap, size_type old_cap) const {
        size_type new_cap = old_cap * 2;
        if (new_cap < min_cap) new_cap = min_cap;
        if (new_cap > max_size()) new_cap = max_size();
        return new_cap;
    }

    void init_from_cstr(const char* str, size_type len) {
        if (len <= SSO_SIZE) {
            std::memcpy(storage_.local_, str, len);
            storage_.local_[len] = '\0';
            size_       = len;
            is_local_   = true;
        } else {
            allocate_buffer(len);
            std::memcpy(storage_.heap_.data_, str, len);
            storage_.heap_.data_[len] = '\0';
            size_            = len;
        }
    }

    void move_from(String&& other) noexcept {
        if (other.is_local_) {
            std::memcpy(storage_.local_, other.storage_.local_, other.size_ + 1);
            size_     = other.size_;
            is_local_ = true;
        } else {
            storage_.heap_.data_     = other.storage_.heap_.data_;
            storage_.heap_.capacity_ = other.storage_.heap_.capacity_;
            size_     = other.size_;
            is_local_ = false;

            other.storage_.heap_.data_     = nullptr;
            other.storage_.heap_.capacity_ = 0;
        }
        other.size_     = 0;
        other.is_local_ = true;
        other.storage_.local_[0] = '\0';
    }
};

// ===== 全局 operator+ / 输出 =====

inline String operator+(const String& a, const String& b) {
    String res(a);
    res += b;
    return res;
}

inline String operator+(const String& a, const char* b) {
    String res(a);
    res += b;
    return res;
}

inline String operator+(const char* a, const String& b) {
    String res(a);
    res += b;
    return res;
}

inline std::ostream& operator<<(std::ostream& os, const String& s) {
    return os << s.c_str();
}

// ======================= 简单测试框架 =======================

static int g_tests_run    = 0;
static int g_tests_failed = 0;

#define ASSERT_TRUE(cond) \
    do { \
        ++g_tests_run; \
        if (!(cond)) { \
            ++g_tests_failed; \
            std::cerr << "[FAIL] " << __FUNCTION__ << " : " \
                      << "ASSERT_TRUE(" #cond ") at " \
                      << __FILE__ << ":" << __LINE__ << "\n"; \
        } \
    } while (0)

#define ASSERT_EQ(expected, actual) \
    do { \
        ++g_tests_run; \
        auto _e = (expected); \
        auto _a = (actual); \
        if (!(_e == _a)) { \
            ++g_tests_failed; \
            std::cerr << "[FAIL] " << __FUNCTION__ << " : " \
                      << "ASSERT_EQ(" #expected ", " #actual ") " \
                      << "expected=" << _e << ", actual=" << _a \
                      << " at " << __FILE__ << ":" << __LINE__ << "\n"; \
        } \
    } while (0)

#define ASSERT_THROW(stmt, ex_type) \
    do { \
        ++g_tests_run; \
        bool _caught = false; \
        try { \
            stmt; \
        } catch (const ex_type&) { \
            _caught = true; \
        } catch (...) { \
        } \
        if (!_caught) { \
            ++g_tests_failed; \
            std::cerr << "[FAIL] " << __FUNCTION__ << " : " \
                      << "ASSERT_THROW(" #stmt ", " #ex_type ") " \
                      << "did not throw expected exception at " \
                      << __FILE__ << ":" << __LINE__ << "\n"; \
        } \
    } while (0)

// ======================= 各模块测试 =======================

void test_basic_constructors() {
    // 默认构造
    String s0;
    ASSERT_EQ(0u, s0.size());
    ASSERT_TRUE(s0.empty());

    // 短串构造（SSO）
    String s1("hello");
    ASSERT_EQ(5u, s1.size());
    ASSERT_TRUE(!s1.empty());
    ASSERT_TRUE(std::string(s1.c_str()) == "hello");

    // n 个字符构造
    String s2(3, 'x');
    ASSERT_EQ(3u, s2.size());
    ASSERT_TRUE(std::string(s2.c_str()) == "xxx");

    // 长串构造（堆）
    String s3("0123456789abcdef"); // 16 字符
    ASSERT_EQ(16u, s3.size());
    ASSERT_TRUE(std::string(s3.c_str()) == "0123456789abcdef");
}

void test_copy_and_move() {
    // 拷贝构造（short）
    String a("short");
    String b(a);
    ASSERT_TRUE(a == b);
    ASSERT_EQ(a.size(), b.size());

    // 拷贝构造（long）
    String long1("this is a very very long string");
    String long2(long1);
    ASSERT_TRUE(long1 == long2);
    ASSERT_EQ(long1.size(), long2.size());

    // 拷贝赋值
    String c;
    c = a;
    ASSERT_TRUE(c == a);

    // 移动构造（short）
    String m1("move-short");
    String m2(std::move(m1));
    ASSERT_TRUE(m2 == String("move-short"));
    ASSERT_EQ(0u, m1.size());
    ASSERT_TRUE(m1.empty());

    // 移动构造（long）
    String m3("move-a-very-long-string-xxxxxxxx");
    String m4(std::move(m3));
    ASSERT_TRUE(m4 == String("move-a-very-long-string-xxxxxxxx"));
    ASSERT_EQ(0u, m3.size());
    ASSERT_TRUE(m3.empty());

    // 移动赋值（short）
    String m5("abc");
    String m6;
    m6 = std::move(m5);
    ASSERT_TRUE(m6 == String("abc"));
    ASSERT_EQ(0u, m5.size());

    // 移动赋值（long）
    String m7("0123456789abcdef0123456789abcdef");
    String m8;
    m8 = std::move(m7);
    ASSERT_TRUE(m8 == String("0123456789abcdef0123456789abcdef"));
    ASSERT_EQ(0u, m7.size());
}

void test_clear_resize() {
    String s("hello");
    s.clear();
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(0u, s.size());
    ASSERT_TRUE(std::string(s.c_str()) == "");

    // resize 扩展（stay SSO）
    s.resize(5, 'x');
    ASSERT_EQ(5u, s.size());
    ASSERT_TRUE(std::string(s.c_str()) == "xxxxx");

    // resize 扩展到堆模式
    s.resize(20, 'y');
    ASSERT_EQ(20u, s.size());
    ASSERT_TRUE(std::string(s.c_str()) == std::string(5, 'x') + std::string(15, 'y'));

    // resize 截断
    s.resize(3);
    ASSERT_EQ(3u, s.size());
    ASSERT_TRUE(std::string(s.c_str()) == "xxx");

    // resize 从 long -> SSO
    s.resize(0);
    ASSERT_EQ(0u, s.size());
    s.resize(10, 'z'); // 10 <= 15，应该走 SSO
    ASSERT_EQ(10u, s.size());
    ASSERT_TRUE(std::string(s.c_str()) == "zzzzzzzzzz");
}

void test_append_and_plus() {
    String s1("Hello");
    String s2(" World");

    s1.append(s2);
    ASSERT_TRUE(s1 == String("Hello World"));

    s1.append("!");
    ASSERT_TRUE(s1 == String("Hello World!"));

    s1.append(3, '?');
    ASSERT_TRUE(s1 == String("Hello World!???"));

    // operator+=
    String s3("A");
    s3 += "B";
    s3 += String("C");
    s3 += 'D';
    ASSERT_TRUE(s3 == String("ABCD"));

    // operator+
    String s4 = String("foo") + String("bar");
    ASSERT_TRUE(s4 == String("foobar"));

    String s5 = String("foo") + "baz";
    ASSERT_TRUE(s5 == String("foobaz"));

    String s6 = std::string("xxx").c_str() + String("yyy");
    ASSERT_TRUE(s6 == String("xxxyyy"));
}

void test_at_and_index() {
    String s("hello");
    ASSERT_EQ('h', s[0]);
    ASSERT_EQ('o', s[4]);

    s[0] = 'H';
    ASSERT_TRUE(s == String("Hello"));

    ASSERT_EQ('H', s.at(0));
    ASSERT_EQ('o', s.at(4));

    ASSERT_THROW(s.at(5), std::out_of_range);
}

void test_iterators() {
    String s("hello");
    std::string copy;

    for (char c : s) {
        copy.push_back(c);
    }
    ASSERT_TRUE(copy == "hello");

    // const 迭代器
    const String cs("world");
    std::string copy2;
    for (auto it = cs.cbegin(); it != cs.cend(); ++it) {
        copy2.push_back(*it);
    }
    ASSERT_TRUE(copy2 == "world");
}

void test_substr() {
    String s("hello world");

    String s1 = s.substr(0, 5);
    ASSERT_TRUE(s1 == String("hello"));

    String s2 = s.substr(6); // to end
    ASSERT_TRUE(s2 == String("world"));

    String s3 = s.substr(3, 100); // 超过末尾
    ASSERT_TRUE(s3 == String("lo world"));

    String s4 = s.substr(11); // pos == size -> 空串
    ASSERT_TRUE(s4.empty());

    ASSERT_THROW(s.substr(12), std::out_of_range); // pos > size_ 抛异常
}

void test_find() {
    String s("abc abc abcd");

    // find 子串
    ASSERT_EQ(0u, s.find("abc"));
    ASSERT_EQ(4u, s.find("abc", 1));
    ASSERT_EQ(8u, s.find(String("abc"), 5));
    ASSERT_EQ(String::npos, s.find("xyz"));

    // find 单字符
    ASSERT_EQ(2u, s.find('c'));
    ASSERT_EQ(6u, s.find('c', 3));
    ASSERT_EQ(String::npos, s.find('z'));

    // find 空子串
    ASSERT_EQ(0u, s.find("", 0));
    ASSERT_EQ(3u, s.find("", 3));

    // find 长度参数版本
    ASSERT_EQ(0u, s.find("abc abc", 0, 3));   // 前 3 字符 "abc"
    ASSERT_EQ(4u, s.find("abc abc", 1, 3));   // 再次 "abc"
}

void test_rfind() {
    String s("abc abc abcd");

    ASSERT_EQ(8u, s.rfind("abc"));          // 最后一个 "abc"
    ASSERT_EQ(4u, s.rfind("abc", 7));       // 限制位置
    ASSERT_EQ(0u, s.rfind("abc", 3));
    ASSERT_EQ(String::npos, s.rfind("xyz"));

    ASSERT_EQ(10u, s.rfind('d'));
    ASSERT_EQ(6u, s.rfind('c', 9));
    ASSERT_EQ(String::npos, s.rfind('z'));

    // 空子串
    ASSERT_EQ(s.size(), s.rfind("", String::npos));
    ASSERT_EQ(3u, s.rfind("", 3)); // min(pos, size)
}

void test_find_first_last_of() {
    String s("hello world");

    // find_first_of
    ASSERT_EQ(1u, s.find_first_of("eox"));         // 'e' at 1
    ASSERT_EQ(4u, s.find_first_of(String("xod"))); // 'o' at 4
    ASSERT_EQ(String::npos, s.find_first_of("xyz")); // none

    ASSERT_EQ(1u, s.find_first_of('e'));
    ASSERT_EQ(String::npos, s.find_first_of('z'));

    // find_last_of
    ASSERT_EQ(10u, s.find_last_of("abcdl"));     // last 'l' at 10
    ASSERT_EQ(7u, s.find_last_of(String("ow"))); // 'o' at 7
    ASSERT_EQ(String::npos, s.find_last_of("xyz"));

    ASSERT_EQ(10u, s.find_last_of('l'));
    ASSERT_EQ(String::npos, s.find_last_of('z'));
}

void test_insert() {
    // 在 SSO 模式下插入
    String s("hello");
    s.insert(0, "X");           // 开头插
    ASSERT_TRUE(s == String("Xhello"));
    s.insert(3, 2, 'Y');        // 中间插 2 个 'Y'
    ASSERT_TRUE(s == String("XheYYllo"));
    s.insert(s.size(), "Z");    // 末尾插
    ASSERT_TRUE(s == String("XheYYlloZ"));

    // 插入另一个 String
    String t("123");
    s.insert(1, t);
    ASSERT_TRUE(s == String("X123heYYlloZ"));

    // 自插入
    String u("abcd");
    u.insert(2, u); // "ab" + "abcd" + "cd" -> "ababcdcd"
    ASSERT_TRUE(u == String("ababcdcd"));

    // 超出 SSO -> 转堆模式再插
    String long_s("0123456789abcdef"); // 16 chars, 堆模式
    long_s.insert(0, "START-");
    ASSERT_TRUE(long_s.starts_with("START-"));
    long_s.insert(long_s.size(), "-END");
    ASSERT_TRUE(long_s.ends_with("-END"));

    // insert 单字符
    String v("hello");
    v.insert(1, 'X');
    ASSERT_TRUE(v == String("hXello"));

    // 越界 pos 抛异常
    ASSERT_THROW(v.insert(100, "oops"), std::out_of_range);
}

void test_compare_and_ordering() {
    String a("abc");
    String b("abc");
    String c("abd");
    String d("ab");

    ASSERT_EQ(0, a.compare(b));
    ASSERT_TRUE(a == b);
    ASSERT_TRUE(!(a != b));

    ASSERT_TRUE(a.compare(c) < 0);
    ASSERT_TRUE(c.compare(a) > 0);

    ASSERT_TRUE(a.compare(d) > 0);
    ASSERT_TRUE(d.compare(a) < 0);

    ASSERT_TRUE(a < c);
    ASSERT_TRUE(c > a);
    ASSERT_TRUE(d < a);
    ASSERT_TRUE(a > d);

    ASSERT_EQ(0, a.compare("abc"));
    ASSERT_TRUE(a.compare("abd") < 0);
    ASSERT_TRUE(a.compare("ab") > 0);
}

void test_starts_with_ends_with() {
    String s("hello world");

    // starts_with
    ASSERT_TRUE(s.starts_with("hello"));
    ASSERT_TRUE(s.starts_with(String("hello")));
    ASSERT_TRUE(!s.starts_with("world"));
    ASSERT_TRUE(s.starts_with('h'));
    ASSERT_TRUE(!s.starts_with('x'));

    // ends_with
    ASSERT_TRUE(s.ends_with("world"));
    ASSERT_TRUE(s.ends_with(String("world")));
    ASSERT_TRUE(!s.ends_with("hello"));
    ASSERT_TRUE(s.ends_with('d'));
    ASSERT_TRUE(!s.ends_with('x'));

    // 空串的情况
    String empty;
    ASSERT_TRUE(empty.starts_with(""));
    ASSERT_TRUE(empty.ends_with(""));
    ASSERT_TRUE(!empty.starts_with('a'));
    ASSERT_TRUE(!empty.ends_with('a'));
}

void test_exceptions_on_nullptr() {
    String s("abc");

    ASSERT_THROW(s.assign(nullptr), std::invalid_argument);
    ASSERT_THROW(s.append(nullptr), std::invalid_argument);
    ASSERT_THROW(s.insert(0, nullptr), std::invalid_argument);
    ASSERT_THROW(s.find(nullptr), std::invalid_argument);
}

// ======================= main =======================

int main() {
    std::cout << "Running tests...\n";

    test_basic_constructors();
    test_copy_and_move();
    test_clear_resize();
    test_append_and_plus();
    test_at_and_index();
    test_iterators();
    test_substr();
    test_find();
    test_rfind();
    test_find_first_last_of();
    test_insert();
    test_compare_and_ordering();
    test_starts_with_ends_with();
    test_exceptions_on_nullptr();

    std::cout << "Tests run:    " << g_tests_run << "\n";
    std::cout << "Tests failed: " << g_tests_failed << "\n";

    if (g_tests_failed == 0) {
        std::cout << "All tests PASSED.\n";
        return 0;
    } else {
        std::cout << "Some tests FAILED.\n";
        return 1;
    }
}

```

### 参考资料

[1] [STL string源码分析](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484308&idx=1&sn=3eb13f6fec136517b9f99e020ae3bc34&chksm=ead01232dda79b24cf67ca0209c2e4215993940e5abb404e3c7813df890ee3f9df90f8e30c16&scene=178&cur_album_id=1428452222471602177&search_click_id=#rd)

[2] [再谈STL string](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484492&idx=1&sn=eb62480deca24465d80696ff14b54a70&chksm=ead015eadda79cfc85bc94a1ebc42d19256e99a705c5c8fc5952734498e4a3e3a5b55bbbf340&scene=178&cur_album_id=1428452222471602177&search_click_id=#rd)

## `std::vector`

### 0. `vector` 的定义与内存管理

`vector` 的源代码在 GCC 源码的三个文件中：`<vector>`、`<stl_vector.h>`、`<vector.tcc>`。首先我们看一下 `vector` 的定义：

```cpp
template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
class vector : protected _Vector_base<_Tp, _Alloc>
{
    typedef _Vector_base<_Tp, _Alloc>         _Base;
    typedef typename _Base::_Tp_alloc_type    _Tp_alloc_type;
    typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type>  _Alloc_traits;

    // 所有的容器都一样的，基本类型和迭代器，重点关注一下几个
    typedef _Tp          value_type; // 容器元素类型
    typedef typename _Base::pointer      pointer;
    typedef typename _Alloc_traits::reference    reference;
    typedef ptrdiff_t          difference_type;
    // vector的迭代器是所有容器中最特殊的一个，实际就是_Tp*，只是用__normal_iterator封装了一下
    typedef __gnu_cxx::__normal_iterator<pointer, vector> iterator;
    typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
}
```

可以看出 `vector` 继承自 `_Vector_base`，`_Vector_base` 的定义如下：

```cpp
template<typename _Tp, typename _Alloc>
struct _Vector_base
{
    // 定义分配器与指针类型
    // 实际上就是std::allocator<_Tp>
    typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
    rebind<_Tp>::other _Tp_alloc_type;
    // 实际上就是_Tp*
    typedef typename __gnu_cxx::__alloc_traits<_Tp_alloc_type>::pointer
    pointer;
  
    // _Vector_base中只有一个数据成员_M_impl，类型为_Vector_impl
    _Vector_impl _M_impl;
    // _Vector_impl为内置类型定义如下，是一个内存管理器类，分配器为_Tp_alloc_type
    struct _Vector_impl : _Tp_alloc_type
    {
        // 持有三个指针
        pointer _M_start; // 在使用空间的起始位置
        pointer _M_finish;// 在使用空间的结束位置
        pointer _M_end_of_storage; // 可使用空间的结束位置
  
        // 构造函数
        _Vector_impl()
            : _Tp_alloc_type(), _M_start(), _M_finish(), _M_end_of_storage()
        { }
        _Vector_impl(_Tp_alloc_type const& __a) _GLIBCXX_NOEXCEPT
            : _Tp_alloc_type(__a), _M_start(), _M_finish(), _M_end_of_storage()
        { }
        // vector数据交换，只交换内存，不交换数据
        void _M_swap_data(_Vector_impl& __x) _GLIBCXX_NOEXCEPT
        {
            std::swap(_M_start, __x._M_start);
            std::swap(_M_finish, __x._M_finish);
            std::swap(_M_end_of_storage, __x._M_end_of_storage);
        }                   
    }
  
    // 构造函数
    _Vector_base() : _M_impl() { }
    _Vector_base(size_t __n) : _M_impl()
    { _M_create_storage(__n); }
    // 移动构造函数，只交换内存，不copy数据
    _Vector_base(_Vector_base&& __x) noexcept
        : _M_impl(std::move(__x._M_get_Tp_allocator()))
    { this->_M_impl._M_swap_data(__x._M_impl); }
  
    // _M_create_storage函数
    void _M_create_storage(size_t __n)
    {
      this->_M_impl._M_start = this->_M_allocate(__n);
      this->_M_impl._M_finish = this->_M_impl._M_start;
      this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
    }
    // _M_allocate函数
    pointer _M_allocate(size_t __n)
    {
      typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tr;
      return __n != 0 ? _Tr::allocate(_M_impl, __n) : pointer();
    }
  
    // 析构函数
    ~_Vector_base()
    {
        _M_deallocate(_M_impl._M_start,
          _M_impl._M_end_of_storage - _M_impl._M_start);
    }
    // _M_deallocate函数
    void _M_deallocate(pointer __p, size_t __n)
    {
       typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tr;
       if (__p)
         _Tr::deallocate(_M_impl, __p, __n);
    }
}
```

可以看出 `_Vector_base` 类主要负责 `vector` 的内存管理，内部类 `_Vector_impl` 通过继承 `_Tp_alloc_type`（也就是 `std::allocator<_Tp>`）得到内存分配和释放的功能，使用 `_M_allocate` 和 `_M_deallocate` 函数分配和释放 `vector` 所使用的内存，`vector` 只需要负责元素的构造和析构。

### 1. `vector` 的迭代器

从 `vector` 的定义中我们看到其迭代器有两个，分别为正向迭代器 `__normal_iterator` 和反向迭代器 `reverse_iterator`。在上面的分析中我们知道 `pointer` 就是普通的指针，这里使用了 traits 编程手法。

```cpp
typedef __gnu_cxx::__normal_iterator<pointer, vector> iterator;
typedef std::reverse_iterator<iterator>  reverse_iterator;
```

`__normal_iterator` 的定义如下：

```cpp
template<typename _Iterator, typename _Container>
class __normal_iterator
{
protected:
  _Iterator _M_current;

  typedef iterator_traits<_Iterator>    __traits_type;

public:
  typedef _Iterator          iterator_type;
  // 五个特征
  typedef typename __traits_type::iterator_category iterator_category;
  typedef typename __traits_type::value_type    value_type;
  typedef typename __traits_type::difference_type   difference_type;
  typedef typename __traits_type::reference   reference;
  typedef typename __traits_type::pointer     pointer;

  _GLIBCXX_CONSTEXPR __normal_iterator() _GLIBCXX_NOEXCEPT
  : _M_current(_Iterator()) { }

  explicit
  __normal_iterator(const _Iterator& __i) _GLIBCXX_NOEXCEPT
  : _M_current(__i) { }

  // Allow iterator to const_iterator conversion
  template<typename _Iter>
    __normal_iterator(const __normal_iterator<_Iter,
    typename __enable_if<
           (std::__are_same<_Iter, typename _Container::pointer>::__value),
      _Container>::__type>& __i) _GLIBCXX_NOEXCEPT
    : _M_current(__i.base()) { }

  // Forward iterator requirements
  reference
  operator*() const _GLIBCXX_NOEXCEPT
  { return *_M_current; }

  pointer
  operator->() const _GLIBCXX_NOEXCEPT
  { return _M_current; }

  __normal_iterator&
  operator++() _GLIBCXX_NOEXCEPT
  {
    ++_M_current;
    return *this;
  }

  __normal_iterator
  operator++(int) _GLIBCXX_NOEXCEPT
  { return __normal_iterator(_M_current++); }

  // Bidirectional iterator requirements
  __normal_iterator&
  operator--() _GLIBCXX_NOEXCEPT
  {
    --_M_current;
    return *this;
  }

  __normal_iterator
  operator--(int) _GLIBCXX_NOEXCEPT
  { return __normal_iterator(_M_current--); }

  // Random access iterator requirements
  reference
  operator[](difference_type __n) const _GLIBCXX_NOEXCEPT
  { return _M_current[__n]; }

  __normal_iterator&
  operator+=(difference_type __n) _GLIBCXX_NOEXCEPT
  { _M_current += __n; return *this; }

  __normal_iterator
  operator+(difference_type __n) const _GLIBCXX_NOEXCEPT
  { return __normal_iterator(_M_current + __n); }

  __normal_iterator&
  operator-=(difference_type __n) _GLIBCXX_NOEXCEPT
  { _M_current -= __n; return *this; }

  __normal_iterator
  operator-(difference_type __n) const _GLIBCXX_NOEXCEPT
  { return __normal_iterator(_M_current - __n); }

  const _Iterator&
  base() const _GLIBCXX_NOEXCEPT
  { return _M_current; }
};
```

为了看迭代器的 traits 技法的使用，我们解析一下下面这句中 `iterator_traits<_Iterator>` 的实现：

```
typedef iterator_traits<_Iterator>    __traits_type;
```

```cpp
template<typename _Iterator>
struct iterator_traits : public __iterator_traits<_Iterator> { };
// 模板类__iterator_traits的定义如下，针对任何一种迭代器类（均定义了五个特征）
template<typename _Iterator>
struct __iterator_traits<_Iterator,
       __void_t<typename _Iterator::iterator_category,
          typename _Iterator::value_type,
          typename _Iterator::difference_type,
          typename _Iterator::pointer,
          typename _Iterator::reference>>
{
  typedef typename _Iterator::iterator_category iterator_category;
  typedef typename _Iterator::value_type        value_type;
  typedef typename _Iterator::difference_type   difference_type;
  typedef typename _Iterator::pointer           pointer;
  typedef typename _Iterator::reference         reference;
};
// 由于普通指针不是类型，因此针对普通指针，__iterator_traits具有特化版本
template<typename _Tp>
struct iterator_traits<_Tp*>
{
  typedef random_access_iterator_tag iterator_category; // 随机访问迭代器
  typedef _Tp                         value_type;
  typedef ptrdiff_t                   difference_type;
  typedef _Tp*                        pointer;
  typedef _Tp&                        reference;
};
```

所以 `vector` 的迭代器类型为 **随机访问迭代器**。

### 2. `vector` 源码剖析与使用

通过对 `vector` 的定义和内存管理的学习，我们知道 `vector` 实际上就是使用三个指针管理了一片连续的可变长的内存空间。这三个指针分别是 `_M_start`、`_M_finish` 和 `_M_end_of_storage`。

![vector three pointer](https://mmbiz.qpic.cn/sz_mmbiz_jpg/7kDbTKZllib1FbibMWsRA2bjySZR1anj8MeMZrOia1fPicYfY7KbN0dMGU23bD1ibTTUswVh7tFFtaKkicBstxZhWsKA/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=0)

#### 基本接口

```cpp
iterator begin() _GLIBCXX_NOEXCEPT
{ return iterator(this->_M_impl._M_start); }

iterator end() _GLIBCXX_NOEXCEPT
{ return iterator(this->_M_impl._M_finish); }

size_type size() const _GLIBCXX_NOEXCEPT
{ return size_type(this->_M_impl._M_finish - this->_M_impl._M_start); }

// 返回当前实际能容纳的元素的量，超越这个量，vector就会重新分配内部内存（增加）
size_type capacity() const _GLIBCXX_NOEXCEPT
{ return size_type(this->_M_impl._M_end_of_storage - this->_M_impl._M_start); }

bool empty() const _GLIBCXX_NOEXCEPT
{ return begin() == end(); }
```

#### 容量控制相关函数

- **`_M_shrink_to_fit()`**：将 `_M_finish` 和 `_M_end_of_storage` 之间的内存释放掉，使分配的内存大小正好满足元素数量。
- **`reserve(n)`**：预分配**至少** `n` 个元素的空间，修改的是 `_M_end_of_storage_` 避免频繁重分配。**注意：不能用于缩减容量**。
- **`resize(n)`**：改变元素个数，修改的是 `_M_finish`。若 `n < size()` 则析构多余元素；若 `n > size()` 则追加默认构造对象。

#### 构造与析构

```cpp
// 默认构造
vector() : _Base() { } 

// 指定分配器
explicit vector(const allocator_type& __a) : _Base(__a) { }

// 初始化为 n 个值
explicit vector(size_type __n, const value_type& __value = value_type(),
             const allocator_type& __a = allocator_type()): _Base(__n, __a)
{ _M_fill_initialize(__n, __value); }

// 拷贝构造
vector(const vector& __x)
    : _Base(__x.size(),
    _Alloc_traits::_S_select_on_copy(__x._M_get_Tp_allocator()))
{ this->_M_impl._M_finish =
    std::__uninitialized_copy_a(__x.begin(), __x.end(),
                                this->_M_impl._M_start,
                                _M_get_Tp_allocator());
}

// 移动构造（C++11）
vector(vector&& _x) noexcept
    : _Base(std::move(__x)) { }

// 列表初始化（C++11）
vector(initializer_list<value_type> __l,
        const allocator_type& __a = allocator_type())
: _Base(__a)
{
    _M_range_initialize(__l.begin(), __l.end(), random_access_iterator_tag());
}

// 迭代器范围初始化
template<typename _InputIterator,
        typename = std::_RequireInputIter<_InputIterator>>
vector(_InputIterator __first, _InputIterator __last,
        const allocator_type& __a = allocator_type())
: _Base(__a)
{ _M_initialize_dispatch(__first, __last, __false_type()); }

// 析构：只析构元素，内存由 _Vector_base 释放
~vector() _GLIBCXX_NOEXCEPT
{ std::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,_M_get_Tp_allocator()); }
```

#### 插入元素与扩容机制

```cpp
typename vector<_Tp, _Alloc>::iterator
vector<_Tp, _Alloc>::insert(iterator __position, const value_type& __x)
{
    const size_type __n = __position – begin();
    if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage
        && __position == end()) // 有空余容量且是尾部插入
    {
        _Alloc_traits::construct(this->_M_impl, this->_M_impl._M_finish, __x);
        ++this->_M_impl._M_finish;
    }
    else // 不是尾部插入或没有剩余容量
    {
        _M_insert_aux(__position, __x);
    }
    return iterator(this->_M_impl._M_start + __n);
}
```

当容量不足时，调用 `_M_insert_aux`：

```cpp
template<typename _Tp, typename _Alloc>
void vector<_Tp, _Alloc>::_M_insert_aux(iterator __position, const _Tp& __x)
{
    if (this->_M_impl._M_finish != this->_M_impl._M_end_of_storage)
    {
        // 有空间：移动尾部元素，插入
        _Alloc_traits::construct(...);
        ++_M_finish;
        _GLIBCXX_MOVE_BACKWARD3(...);
        *__position = __x;
    }
    else
    {
        // 扩容
        const size_type __len = _M_check_len(size_type(1), "vector::_M_insert_aux");
        const size_type __elems_before = __position - begin();
        pointer __new_start(this->_M_allocate(__len));
        pointer __new_finish(__new_start);
        __try
        {
            // 构造新位置元素
            _Alloc_traits::construct(this->_M_impl, __new_start + __elems_before, __x);
            // 移动前后段元素
            __new_finish = std::__uninitialized_move_if_noexcept_a(
                this->_M_impl._M_start, __position.base(), __new_start, ...);
            ++__new_finish;
            __new_finish = std::__uninitialized_move_if_noexcept_a(
                __position.base(), this->_M_impl._M_finish, __new_finish, ...);
        }
        __catch(...)
        {
            // 异常安全处理
            ...
            __throw_exception_again;
        }

        // 销毁旧内存
        std::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish, ...);
        _M_deallocate(this->_M_impl._M_start, ...);

        // 更新指针
        this->_M_impl._M_start = __new_start;
        this->_M_impl._M_finish = __new_finish;
        this->_M_impl._M_end_of_storage = __new_start + __len;
    }
}
```

其中 `_M_check_len` 的策略为：

```cpp
// __n: 请求增加的元素个数
size_type _M_check_len(size_type __n, const char* __s) const
{
    if (max_size() - size() < __n)
        __throw_length_error(__N(__s));
    const size_type __len = size() + std::max(size(), __n);
    	    // 如果 __n <= size()，则新容量 = size() + size() → 翻倍扩容
			// 如果 __n > size()，则新容量 = size() + __n → 至少满足需求
    return (__len < size() || __len > max_size()) ? max_size() : __len;
}
```

> **结论**：`vector` 内存增长策略并非简单“加倍”，而是 `size + max(size, n)`。

#### 删除元素

- `erase(pos)`：删除单个元素，返回下一位置迭代器。
- `erase(first, last)`：删除区间。
- `clear()`：清空所有元素（不释放内存）。
- 结合 `remove`/`remove_if` 使用：

  ```cpp
  erase(remove(begin(), end(), val), end());
  erase(remove_if(begin(), end(), [](auto x){ return cond; }), end());
  ```

### 3. `vector` 使用陷阱

#### 陷阱 1：越界访问不抛异常（除 `at()` 外）

```cpp
std::vector<int> v;
v.push_back(3);
v.push_back(4);
// v[100]; // 未定义行为！
// 应使用 at() 或手动检查
if (!v.empty()) {
    v.front(); // 安全
}
```

#### 陷阱 2：内存只增不减

`clear()` 仅调用析构函数，**不释放已分配内存**。即使只剩一个元素，容量仍可能很大。

**解决方法**：使用 `swap` 技巧强制释放：

```cpp
vector<int> nums;
nums.push_back(1);
nums.push_back(2);
// 释放内存
vector<int>().swap(nums); // 或 nums.swap(vector<int>());
// C++11 引入 shrink_to_fit()
```

> 若需频繁缩容，考虑使用 `deque`。

#### 陷阱 3：存储指针时不会自动释放指向对象

```cpp
#include <vector>
using namespace std;

vector<void*> v;
// ... 插入 new 出的对象指针

// 必须手动释放！
for (auto it = v.begin(); it != v.end(); ++it) {
    if (*it) {
        delete *it;
        *it = nullptr;
    }
}
v.clear(); // 仅清除指针，不 delete 对象
```

## `std::deque`

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib1JDTIibJ7f6dic6CIHNwWrute8MZZ8MEzoB0yxfC6WHxupgjdLov9XvYp2lJgZjkRsWGibYSic2lrECA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=0)

### 0. deque 核心代码剖析

`deque` 这个容器类实际上很简单，核心代码就那么几行，只要抓住核心代码理解透 `deque` 的数据结构，其他的源码都不在话下。

所以我们通过源码剖析来绘制 `deque` 数据结构原理图。

`deque` 类的定义如下：

```cpp
template<typename _Tp, typename _Alloc = std::allocator<_Tp>>
class deque : protected _Deque_base<_Tp, _Alloc>
{
  // deque中的核心信息就是下面这两条
  typedef _Deque_base<_Tp, _Alloc>      _Base;
protected:
  using _Base::_M_impl; // _M_impl就是deque的核心数据结构

  // 这个静态函数是deque计算buffer大小的函数
  // buffer和这个静态函数先放到这里，我们后面详细说
  static size_t _S_buffer_size() _GLIBCXX_NOEXCEPT
  { return __deque_buf_size(sizeof(_Tp)); }
};

// __deque_buf_size的定义如下，是个内联函数，计算buffer的长度
_GLIBCXX_CONSTEXPR inline size_t __deque_buf_size(size_t __size)
{ 
  return (__size < _GLIBCXX_DEQUE_BUF_SIZE
    ? size_t(_GLIBCXX_DEQUE_BUF_SIZE / __size) : size_t(1)); 
}
```

`deque` 的核心数据结构就是 `_Deque_base<_Tp, _Alloc>::_M_impl`，接下来我们看一下 `_Deque_base` 的实现：

```cpp
template<typename _Tp, typename _Alloc>
class _Deque_base
{
protected:
  typedef typename _Alloc_traits::pointer    _Ptr; // _Tp*
public:
  typedef _Deque_iterator<_Tp, _Tp&, _Ptr>    iterator; // 迭代器
protected:
  typedef typename iterator::_Map_pointer _Map_pointer; // _Tp**，后面会可能到迭代器的详细代码
  enum { _S_initial_map_size = 8 };
  _Deque_impl _M_impl; // _M_impl是_Deque_impl类的对象
};
```

`_Deque_impl` 是 `_Deque_base` 的内部类。定义如下：

```cpp
struct _Deque_impl
{
  _Map_pointer _M_map; // _Map_pointer实际上就是_Tp**，就是一个元素类型为指针的数组
  size_t _M_map_size;  // _M_map的长度
  iterator _M_start;   // 迭代器，“指向”_M_map的第一个节点
  iterator _M_finish;  // 迭代器，“指向”_M_map的最后一个节点

  // _Deque_impl()的构造函数很普通，将长度初始化为0，指针初始化为空，迭代器也为默认值
  _Deque_impl()
  : _Tp_alloc_type(), _M_map(), _M_map_size(0),
    _M_start(), _M_finish()
  { }
};
```

由上面的代码我们大致可以看出来，`deque` 的核心数据结构是元素类型为指针的数组——`_M_map`（后面我们简称为 **Mmap**，Mmap 的每一个元素我们称之为 **node**），后面会看到 Mmap 中每个元素指向的“东西”我们称之为 **buffer**，类型为 `_Tp*`（实际上是一个数组，元素的类型为 `_Tp`），实际上 buffer 中保存的就是 `deque` 的元素，所有的 buffer 我们称之为 **缓冲区**。

有了上面的分析，我们可以大致绘制出 `deque` 数据结构的原理图。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib1JDTIibJ7f6dic6CIHNwWrutibGB8K0KSmHibLk3AZl8VF1iam9o2SGrOJlNNFTcrahYyfKdNrG3kddZg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=1)

我们暂时先不画出两个迭代器 `_M_start` 和 `_M_finish`，通过阅读源码逐步绘制出整个原理图。此时 Mmap 和 buffer 的长度还不确定。

#### Mmap 与 buffer 的长度确定

Mmap 在 `deque` 中的作用是什么呢？以及 Mmap 和 buffer 的具体长度是多少？我们需要从 `deque` 的构造函数看起：

```cpp
/**
 *  @brief  Creates a %deque with no elements.
 *  deque的构造函数
 *  typedef _Deque_base<_Tp, _Alloc>      _Base;
 */
deque() : _Base() { }

// 需要继续看_Deque_base的构造函数
_Deque_base() : _M_impl()
{ _M_initialize_map(0); } // 核心为_M_initialize_map(0)
```

`_M_initialize_map(0)` 的定义如下：

```cpp
template<typename _Tp, typename _Alloc>
// __num_elements为一个空deque对象初始可以存储元素的个数
void _Deque_base<_Tp, _Alloc>::_M_initialize_map(size_t __num_elements)
{
  // 计算node的个数，也就是Mmap的长度
  const size_t __num_nodes = (__num_elements / __deque_buf_size(sizeof(_Tp)) + 1); // +1上取整
  
  // Mmap的最终长度是_S_initial_map_size和__num_nodes + 2的最大值
  this->_M_impl._M_map_size = std::max((size_t) _S_initial_map_size, // 初始map的最小长度
         size_t(__num_nodes + 2)); // +2预留头和尾两个node
  
  // 申请连续空间作为Mmap
  this->_M_impl._M_map = _M_allocate_map(this->_M_impl._M_map_size);
  
  // 至此，“中控”Mmap已经构建出来了
```

#### Buffer 长度规则

```cpp
_GLIBCXX_CONSTEXPR inline size_t
__deque_buf_size(size_t __size)
{ 
  return (__size < _GLIBCXX_DEQUE_BUF_SIZE
    ? size_t(_GLIBCXX_DEQUE_BUF_SIZE / __size) : size_t(1)); 
}
// buffer 大小默认 _GLIBCXX_DEQUE_BUF_SIZE 为 512 B
```

- 当 `sizeof(_Tp) < 512` 时，buffer 长度为 `512 / sizeof(_Tp)`（下取整）
- 否则，buffer 长度为 `1`

#### Mmap 初始长度规则

- 若 `__num_elements == 0`，则 `_M_map_size = 8`
- 否则，`_M_map_size = __num_nodes + 2`

接着，源码引入两个临时变量 `__nstart` 和 `__nfinish`：

```cpp
_Map_pointer __nstart = (this->_M_impl._M_map
       + (this->_M_impl._M_map_size - __num_nodes) / 2); 
_Map_pointer __nfinish = __nstart + __num_nodes;
```

- `(this->_M_impl._M_map_size - __num_nodes)`：计算 **空闲槽位总数**（即 map 总长减去实际需要的节点数）。
- 除以 2：将这些空闲槽位**平均分配到前后两端**。
- `this->_M_impl._M_map + offset`：指针算术，得到中间某位置的地址。
- 所以 `__nstart` 指向的是：**map 中第一个将用于存放有效 buffer 指针的位置**，位于 map 的“中央偏左”。

目的：让初始使用的 buffer 指针位于 map 中间，**前后都留有空闲槽位**，便于后续在 deque 前端或后端插入时，直接使用相邻空槽，避免立即重分配 map。

- `__nfinish` 指向 `__nstart` 之后第 `__num_nodes` 个位置（即**最后一个有效槽位的下一个位置**）。
- 这符合 STL 的“左闭右开”区间惯例：`[__nstart, __nfinish)` 是将要初始化的 buffer 指针范围。

然后通过 `_M_create_nodes(__nstart, __nfinish)` 构造 buffer：

```cpp
template<typename _Tp, typename _Alloc>
void
_Deque_base<_Tp, _Alloc>::
_M_create_nodes(_Map_pointer __nstart, _Map_pointer __nfinish)
{
  _Map_pointer __cur;
  __try
  {
    for (__cur = __nstart; __cur < __nfinish; ++__cur)
      *__cur = this->_M_allocate_node();
  }
  __catch(...)
  {
    _M_destroy_nodes(__nstart, __cur);
    __throw_exception_again;
  }
}
```

最后设定两个迭代器 `_M_start` 和 `_M_finish`：

```cpp
this->_M_impl._M_start._M_set_node(__nstart);
this->_M_impl._M_finish._M_set_node(__nfinish - 1);
this->_M_impl._M_start._M_cur = _M_impl._M_start._M_first;
this->_M_impl._M_finish._M_cur = (this->_M_impl._M_finish._M_first
    + __num_elements % __deque_buf_size(sizeof(_Tp)));
```

* 可以发现，mmap.cur(buffer) 指向中间位置，buffer.cur 也指向中间位置，这些都是为了方便 deque 的两端插入与删除

其中 `_M_set_node` 的实现为：

```cpp
void _M_set_node(_Map_pointer __new_node) _GLIBCXX_NOEXCEPT
{
  _M_node = __new_node;
  _M_first = *__new_node;
  _M_last = _M_first + difference_type(_S_buffer_size());
}
```

至此，`deque` 数据结构的原理图已经绘制完成。

### 1. deque 迭代器源码剖析

从上一节我们知道 `deque` 数据结构是由一个 Mmap + 一系列的 node 组成，特点是**分段连续**。但是在使用中，`deque` 给人的感觉却是整体连续的——这全靠迭代器在背后处理“跳跃”。

`deque` 迭代器 `_Deque_iterator` 的定义如下：

```cpp
template<typename _Tp, typename _Ref, typename _Ptr>
struct _Deque_iterator
{
public:
  typedef __ptr_to<_Tp>    _Elt_pointer;  // 就是_Tp*
  typedef __ptr_to<_Elt_pointer>  _Map_pointer; // 就是_Tp**
  
  _Elt_pointer _M_cur;    // 当前迭代器所指向的元素位置
  _Elt_pointer _M_first;  // 指向node的头元素
  _Elt_pointer _M_last;   // 指向node最后一个元素的下一个
  _Map_pointer _M_node;   // 指向mmap中当前buffer对应的slot
};
```

#### 核心操作

- **`_M_set_node`**：设置迭代器指向新的 node
- **`operator*` / `operator->`**：直接操作 `_M_cur`

```cpp
reference operator*() const _GLIBCXX_NOEXCEPT
{ return *_M_cur; }

pointer operator->() const _GLIBCXX_NOEXCEPT
{ return _M_cur; }
```

- **`operator++` / `operator--`**：处理跨 buffer 跳跃

```cpp
_Self& operator++() _GLIBCXX_NOEXCEPT
{
  ++_M_cur;
  if (_M_cur == _M_last)
  {
    _M_set_node(_M_node + 1);
    _M_cur = _M_first;
  }
  return *this;
}

_Self& operator--() _GLIBCXX_NOEXCEPT
{
  if (_M_cur == _M_first)
  {
    _M_set_node(_M_node - 1);
    _M_cur = _M_last;
  }
  --_M_cur;
  return *this;
}
```

### 2. `push_front` 和 `push_back`

`deque` 是非连续空间，当到达 buffer 边缘时，需要“跳跃”；当 Mmap 空间不足时，还需扩充 Mmap。

#### `push_front` 源码分析

```cpp
void push_front(const value_type& __x)
{
  if (this->_M_impl._M_start._M_cur != this->_M_impl._M_start._M_first)
  { // 就地插入: 插入到当前 buffer
    _Alloc_traits::construct(this->_M_impl,
           this->_M_impl._M_start._M_cur - 1,
           __x);
    --this->_M_impl._M_start._M_cur;
  }
  else // 插入到前一个 buffer
    _M_push_front_aux(__x);
}
```

当当前 buffer 已满，调用 `_M_push_front_aux`：

```cpp
void deque<_Tp, _Alloc>::_M_push_front_aux(const value_type& __t)
{
  // 前一个 buffer 是否存在，如果不存在需要在 map 中分配内存
  _M_reserve_map_at_front();
  
  // 对于前一个 buffer，分配实际存储元素的数据块
  *(this->_M_impl._M_start._M_node - 1) = this->_M_allocate_node();
  this->_M_impl._M_start._M_set_node(this->_M_impl._M_start._M_node - 1);
  this->_M_impl._M_start._M_cur = this->_M_impl._M_start._M_last - 1;
  this->_M_impl.construct(this->_M_impl._M_start._M_cur, __t);
}
```

检查是否需扩充 Mmap：

```cpp
void _M_reserve_map_at_front(size_type __nodes_to_add = 1)
{
  if (__nodes_to_add > size_type(this->_M_impl._M_start._M_node - this->_M_impl._M_map))
      _M_reallocate_map(__nodes_to_add, true);
}
```

#### Mmap 扩充策略：`_M_reallocate_map`

```cpp
template <typename _Tp, typename _Alloc>
void deque<_Tp, _Alloc>::
_M_reallocate_map(size_type __nodes_to_add, bool __add_at_front)
{
  // 已使用节点数
  const size_type __old_num_nodes
      = this->_M_impl._M_finish._M_node - this->_M_impl._M_start._M_node + 1;
  // 拓展后所需节点数
  const size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

  _Map_pointer __new_nstart;

  // 情况1：Mmap 足够大（> 2 * 所需节点数）
  if (this->_M_impl._M_map_size > 2 * __new_num_nodes)
  {
    // 重新计算起始位置
    __new_nstart = this->_M_impl._M_map + (this->_M_impl._M_map_size - __new_num_nodes) / 2
                   + (__add_at_front ? __nodes_to_add : 0);
    // 从前往后移动，避免覆盖
    if (__new_nstart < this->_M_impl._M_start._M_node)
      std::copy(this->_M_impl._M_start._M_node,
          this->_M_impl._M_finish._M_node + 1,
          __new_nstart);
    // 从后往前移动
    else
      std::copy_backward(this->_M_impl._M_start._M_node,
             this->_M_impl._M_finish._M_node + 1,
             __new_nstart + __old_num_nodes);
  }
  // 情况2：Mmap 不够大，需重新分配
  else
  {
    // new_size = old_size + max(old_size, nodes_toadd) + 2
    // 和 vector 的思路一样，只不过还需要额外 +2 以预留头尾空间
    size_type __new_map_size = this->_M_impl._M_map_size
                               + std::max(this->_M_impl._M_map_size, __nodes_to_add) + 2;
    _Map_pointer __new_map = this->_M_allocate_map(__new_map_size);
    __new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2
                   + (__add_at_front ? __nodes_to_add : 0);
    std::copy(this->_M_impl._M_start._M_node,
        this->_M_impl._M_finish._M_node + 1,
        __new_nstart);
    _M_deallocate_map(this->_M_impl._M_map, this->_M_impl._M_map_size);
  
    this->_M_impl._M_map = __new_map;
    this->_M_impl._M_map_size = __new_map_size;
  }

  // 重新设置迭代器
  this->_M_impl._M_start._M_set_node(__new_nstart);
  this->_M_impl._M_finish._M_set_node(__new_nstart + __old_num_nodes - 1);
}
```

#### 总结：Mmap 扩充规则

- 若 `Mmap_size > 2 * 缓冲区节点数`：**原地移动** buffers 到中心
- 否则：**重新分配** 大小为 `2 * old_size + 2` 的新 Mmap，并拷贝 buffers 到中心

### 参考资料

[1] [STL源码分析之deque](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247484719&idx=1&sn=31c2b1112085db9c58178b6eb5746f7b&chksm=ead01489dda79d9f32127b3fe359d2433b4fbd6bf24f382a914a78f418dc8d3b4bc0d96cc59b&scene=178&cur_album_id=1428452222471602177&search_click_id=#rd)

## `std::hashtable`

### 两种hashtable原理图对比

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0qFBX1tGnib0iaFicsZzK6F4SYgT7HEhX2TLoyFBicsZhlvoytSFHAOTlKST9e1G6oMQPCUZuYR4mmqg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=0)

### 0. 经典hashtable源码分析

`hashtable` 是字典的一种表示方法，它用一个哈希函数把字典的数对映射到一个哈希表的具体位置。如果数对 `p` 的关键字是 `k`，哈希函数为 `f`，那么在理想的情况下，`p` 在散列表中的位置为 `f(k)`，且哈希表的每一个位置最多只能存储一个记录。这种理想的情况带来的负担是哈希表占用空间会很大。

因此通常使用哈希函数将若干个不同的关键字映射到散列表的同一个位置，这个位置就叫**桶（bucket）**，桶的个数就是散列表的长度。

上述的将若干个不同的关键字映射到散列表的同一个位置的行为叫做“**哈希碰撞**”。解决哈希碰撞的方法有：线性探索、二次探索、开链等。这些知识在任何一本数据结构的书上都有详细的讲解。我们只需要知道 **STL 中采用开链法解决哈希碰撞问题**。

所谓**开链**就是在哈希表的每个桶中维护一个 list，然后对于哈希函数计算出的相同位置的元素，在 list 上执行插入、搜索和删除等。开链法实现的 hashtable 的形式如下图所示，每一个 bucket 中都维护了一个单向链表：

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib2NVCDYcMicHz2icEwEsm11iaib9Cc1LZltOAbsN8SmbmPkPW2qqnh9tM4gwcxeia38rtKnwXAeMv6SHcw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=1)

STL 库曾经有一个版本的 hashtable 是用 `vector + 单向链表` 实现<核心1>的——就是按照上图的原理进行实现的，看这个源码可以很快速地理解哈希表的原理。

首先是 `hashtable` 类的定义：

```cpp
// _Key：键的类型
// _Val：存储在哈希表中的实际值类型（例如 pair<const Key, T> 对于 map）
// _ExtractKey：一个函数对象，用于从 _Val 中提取出 _Key。例如，在 map 中，_Val 是 pair<const Key, T>，那么 _ExtractKey 可能是返回 pair.first 的仿函数
template<class _Val, class _Key, class _HashFcn,
         class _ExtractKey, class _EqualKey, class _Alloc>
class hashtable {
public:
    typedef _Key      key_type;
    typedef _Val      value_type;
    typedef _HashFcn  hasher;
    typedef _EqualKey key_equal;

    typedef size_t            size_type;
    typedef ptrdiff_t         difference_type;
    typedef value_type*       pointer;
    typedef const value_type* const_pointer;
    typedef value_type&       reference;
    typedef const value_type& const_reference;

private:
    typedef _Hashtable_node<_Val> _Node;
    typedef typename _Alloc::template rebind<_Node>::other _Node_Alloc;
    typedef typename _Alloc::template rebind<_Node*>::other _Nodeptr_Alloc;
    typedef vector<_Node*, _Nodeptr_Alloc> _Vector_type;

private:
    // hashtable的数据成员
    hasher                _M_hash;          // hash函数
    key_equal             _M_equals;        // 判断key是否相等
    _ExtractKey           _M_get_key;       // 取出节点中的key部分
    _Vector_type          _M_buckets;       // 核心数据结构，存储节点，vector
    size_type             _M_num_elements;  // 节点数量
};
```

1. **为什么需要 `_ExtractKey`？**
   因为 `value_type` 不一定是 `key_type`。例如：
   - 在 `unordered_set<Key>` 中，`_Val == _Key`，`_ExtractKey` 可能是恒等函数。
   - 在 `unordered_map<Key, T>` 中，`_Val == pair<const Key, T>`，`_ExtractKey` 返回 `pair.first`。

从上面的源码中我们能发现，`hashtable` 类的核心数据结构就是 `_M_buckets`，这就是之前说的哈希表中的 bucket。`_M_buckets` 的类型为 `_Vector_type`：

```cpp
typedef vector<_Node*, _Nodeptr_Alloc> _Vector_type;
```

使用 `vector` 存储指向 hashtable 数据节点的指针 `_Node*`，`_Node` 的定义如下，包含需要存储的数据和指向下一个节点的指针：

```cpp
typedef _Hashtable_node<_Val> _Node;

template<class _Val>
struct _Hashtable_node {
    _Hashtable_node* _M_next;
    _Val _M_val;
};
```

根据源码，利用 `hash(key)` 函数将 key 映射到 `_M_buckets` 对应的位置上，如果不同 key 映射出的位置一样，即发生 hash 碰撞，则插入到链表中。从源码来看，是**插入到链表的头部**。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib1l3mCVibVW5hEyLTPELQObEtgh7eugxynXxPCMT2DRyHDUSNFNWaYs94X1Zy4rVxxSz5bBXibgEhZg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=2)

---

#### 构造函数与桶初始化

`hashtable` 的构造函数如下：

```cpp
hashtable(size_type __n, const _HashFcn& __hf,
          const _EqualKey& __eql, const _ExtractKey& __ext,
          const allocator_type& __a = allocator_type())
    : _M_node_allocator(__a), _M_hash(__hf), _M_equals(__eql),
      _M_get_key(_ _ext), _M_buckets(__a), _M_num_elements(0)
{
    _M_initialize_buckets(__n);
}
```

对数据成员进行列表初始化后，再初始化 buckets，初始化源码如下：

```cpp
void _M_initialize_buckets(size_type __n) {
    // 根据__n计算buckets的大小，buckets的大小不一定为__n，而是最接近__n的质数
    const size_type __n_buckets = _M_next_size(__n);
    // 使用vector的reserve方法初始化buckets的容量
    _M_buckets.reserve(__n_buckets);
    // 全部初始化为nullptr
    _M_buckets.insert(_M_buckets.end(), __n_buckets, (_Node*)0);
    _M_num_elements = 0;
}
```

用 `vector + 单向链表` 实现的 hashtable 另一个核心是 **buckets 容量的确定**<核心2>，使用 `_M_next_size(__n)`，源码如下：

```cpp
size_type _M_next_size(size_type __n) const {
    return __stl_next_prime(__n);
}

// 求解__n的下一个最接近的质数
inline unsigned long __stl_next_prime(unsigned long __n) {
    const unsigned long* __first = _Hashtable_prime_list<unsigned long>::_S_get_prime_list();
    const unsigned long* __last = __first + (int)_S_num_primes;
    // 二分查找到第一个>=__n的质数
    const unsigned long* pos = std::lower_bound(__first, __last, __n);
    return pos == __last ? *(__last - 1) : *pos;
}

// 质数表定义
enum { _S_num_primes = 29 };

template<typename _PrimeType>
struct _Hashtable_prime_list {
    static const _PrimeType __stl_prime_list[_S_num_primes];
    static const _PrimeType* _S_get_prime_list();
};

// 表中质数大致按 ×2 增长（略大于 2 倍），这样每次扩容接近翻倍，符合动态数组的摊销复杂度设计
template<typename _PrimeType>
const _PrimeType _Hashtable_prime_list<_PrimeType>::__stl_prime_list[_S_num_primes] = {
    5ul,          53ul,         97ul,         193ul,       389ul,
    769ul,        1543ul,       3079ul,       6151ul,      12289ul,
    24593ul,      49157ul,      98317ul,      196613ul,    393241ul,
    786433ul,     1572869ul,    3145739ul,    6291469ul,   12582917ul,
    25165843ul,   50331653ul,   100663319ul,  201326611ul, 402653189ul,
    805306457ul,  1610612741ul, 3221225473ul, 4294967291ul
};

template<typename _PrimeType>
inline const _PrimeType* _Hashtable_prime_list<_PrimeType>::_S_get_prime_list() {
    return __stl_prime_list;
}
```

---

#### 迭代器设计

说到容器就绕不开迭代器，我们看一下 `hashtable` 类的迭代器：

```cpp
typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey,
                            _EqualKey, _Alloc> iterator;
```

`_Hashtable_iterator` 的定义如下：

```cpp
template <class _Val, class _Key, class _HashFcn, class _ExtractKey, class _EqualKey, class _Alloc>
struct _Hashtable_iterator
{
    typedef  hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> _Hashtable;
    typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc> iterator;
    typedef _Hashtable_node<_Val>   _Node;
  
    typedef forward_iterator_tag    iterator_category;  // 单向迭代器
    typedef _Val                    value_type;
    typedef _Val&                   reference;
    typedef _Val*                   pointer;
    typedef ptrdiff_t               difference_type;
    typedef size_t                  size_type;

    _Node*      _M_cur;
    _Hashtable* _M_ht;
};
```

由于 hashtable 是由 `vector + 单向链表` 实现，所以迭代器类型为 `forward_iterator_tag`，只提供 `operator++`：

```cpp
template<class _Val, class _Key, class _HF, class _ExK, class _EqK, class _All>
_Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>&
_Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::operator++() {
    const _Node* __old = _M_cur;
    _M_cur = _M_cur->_M_next;
    // 当前桶为空
    if (!_M_cur) {
        // 遍历到下一个非空桶
        size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
        while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
            _M_cur = _M_ht->_M_buckets[__bucket];
    }
    return *this;
}
```

定位桶索引的函数：

```cpp
size_type _M_bkt_num(const value_type& __obj) const {
    // call _M_bkt_num_key(const key_type& __key)
    return _M_bkt_num_key(_M_get_key(__obj)); 
}

size_type _M_bkt_num_key(const key_type& __key) const {
    // call _M_bkt_num_key(const key_type& __key, size_t __n)
    return _M_bkt_num_key(__key, _M_buckets.size()); 
}

size_type _M_bkt_num_key(const key_type& __key, size_t __n) const {
    return _M_hash(__key) % __n;
}
```

> 注意：这种实现方式在遍历时效率较低，因为链表尾部需重新计算下一个非空桶的位置。现代实现通过全局单链优化了这一点。

---

#### 查找、插入与删除

**查找：**

```cpp
iterator find(const key_type& __key) {
    size_type __n = _M_bkt_num_key(__key);
    _Node* __first;
    for (__first = _M_buckets[__n];
         __first && !_M_equals(_M_get_key(__first->_M_val), __key);
         __first = __first->_M_next) {}
    return iterator(__first, this);
}
```

**删除：**（略去详细代码，逻辑复杂但正确处理头节点与中间节点）

**插入：**

```cpp
pair<iterator, bool> insert_unique(const value_type& __obj) {
    resize(_M_num_elements + 1);
    return insert_unique_noresize(__obj);
}
```

关键点：

- `resize(n)` 并不是“把大小设为 n”，而是 **“确保容量至少能容纳 n 个元素而不超标”**。
- 如果当前桶数足够（即 `(_M_num_elements + 1) / _M_buckets.size() ≤ max_load_factor`），则 `resize` 什么也不做；
- 否则，会分配一个更大的桶数组（通过 `_M_next_size` 找下一个质数），并将所有元素 **rehash（重新散列）** 到新桶中。

**Rehash（扩容）：**

```cpp
// 确保哈希表能容纳至少 __num_elements_hint 个元素，而不使负载因子超过阈值
// 当前哈希表负载因子为 1.0
void resize(size_type __num_elements_hint) {
    const size_type __old_n = _M_buckets.size();
	// 需要扩容
    if (__num_elements_hint > __old_n) {
        const size_type __n = _M_next_size(__num_elements_hint);
        if (__n > __old_n) {
            _Vector_type __tmp(__n, (_Node*)(0), _M_buckets.get_allocator());
            __try {
                for (size_type __bucket = 0; __bucket < __old_n; ++__bucket) {
                    _Node* __first = _M_buckets[__bucket];
                    while (__first) {
                        size_type __new_bucket = _M_bkt_num(__first->_M_val, __n);
                        _M_buckets[__bucket] = __first->_M_next;
                        __first->_M_next = __tmp[__new_bucket];
                        __tmp[__new_bucket] = __first;
                        __first = _M_buckets[__bucket];
                    }
                }
                _M_buckets.swap(__tmp);
            } __catch(...) {
                // 异常安全处理
                __throw_exception_again;
            }
        }
    }
}
```

**实际插入（无扩容）：**

```cpp
pair<iterator, bool> insert_unique_noresize(const value_type& __obj) {
    const size_type __n = _M_bkt_num(__obj);
    _Node* __first = _M_buckets[__n];
    for (_Node* __cur = __first; __cur; __cur = __cur->_M_next)
        if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj)))
            return pair<iterator, bool>(iterator(__cur, this), false);

    _Node* __tmp = _M_new_node(__obj);
    __tmp->_M_next = __first;
    _M_buckets[__n] = __tmp;
    ++_M_num_elements;
    return pair<iterator, bool>(iterator(__tmp, this), true);
}
```

---

### 1. STL hashtable 源码分析

现代 STL 中的 `_Hashtable` 作为 `unordered_map` 和 `unordered_set` 的底层实现，其设计更为复杂且高效。

类定义（简化）：

```cpp
template<typename _Key, typename _Value, typename _Alloc,
         typename _ExtractKey, typename _Equal,
         typename _H1, typename _H2, typename _Hash,
         typename _RehashPolicy, typename _Traits>
class _Hashtable
  : public __detail::_Hashtable_base<...>,
    public __detail::_Map_base<...>,
    public __detail::_Insert<...>,
    public __detail::_Rehash_base<...>,
    public __detail::_Equality<...>,
    private __detail::_Hashtable_alloc<...>
{
    // ...
};
```

#### 模板参数含义：

- `_Key`：键值类型
- `_Value`：实值类型（如 `pair<const Key, T>`）
- `_Alloc`：分配器
- `_ExtractKey`：从 value 中提取 key 的方法（如 `_Select1st`）
- `_Equal`：键相等判断函数
- `_H1`：主哈希函数（如 `hash<Key>`）
- `_H2`：范围映射函数（如 `_Mod_range_hashing`）
- `_Hash`：组合函数，`_Hash(k, N) = _H2(_H1(k), N)`
- `_RehashPolicy`：rehash 策略（如 `_Prime_rehash_policy`）
- `_Traits`：特性萃取（如是否缓存 hash code）

#### `unordered_map` 类的部分源码如下：

```cpp
template<typename _Key, typename _Tp,
   typename _Hash = hash<_Key>,
   typename _Pred = equal_to<_Key>,
   typename _Alloc = allocator<std::pair<const _Key, _Tp>>>
class unordered_map
{
    typedef __umap_hashtable<_Key, _Tp, _Hash, _Pred, _Alloc>  _Hashtable;
    _Hashtable _M_h;
};

// __umap_hashtable如下
template<bool _Cache>
  using __umap_traits = __detail::_Hashtable_traits<_Cache, false, true>;

template<typename _Key,
   typename _Tp,
   typename _Hash = hash<_Key>,
   typename _Pred = std::equal_to<_Key>,
   typename _Alloc = std::allocator<std::pair<const _Key, _Tp> >,
   typename _Tr = __umap_traits<__cache_default<_Key, _Hash>::value>>
  using __umap_hashtable = _Hashtable<_Key, std::pair<const _Key, _Tp>, _Alloc, __detail::_Select1st,
              _Pred, _Hash,
              __detail::_Mod_range_hashing,
              __detail::_Default_ranged_hash,
              __detail::_Prime_rehash_policy, _Tr>;
```

可以看出，`_H1` 就是 `_Hash = hash<_Key>`，`_H2` 就是`__detail::_Mod_range_hashing`，`_Hash` 就是 `__detail::_Default_ranged_hash`。`typename _H1, typename _H2, typename _Hash` 之间的关系如下：`_Hash(k, N) = _H2(_H1(k), N)`。

看完了这些，继续看一下 `_Hashtable` 类的数据成员：

```cpp
template<typename _Key, typename _Value, typename _Alloc,
   typename _ExtractKey, typename _Equal,
   typename _H1, typename _H2, typename _Hash,
   typename _RehashPolicy, typename _Traits>
class _Hashtable
{
    // hash表中节点类型
    using __node_type = __detail::_Hash_node<_Value, __hash_cached::value>;
  
    // __node_base的定义如下：
    using __node_base = __detail::_Hash_node_base;
  
    // __bucket_type的定义如下，实际上就是指向__node_base的指针
    using __bucket_type = __node_base*; 
  
    // buckets的定义，是一个二级指针，可以理解为类型为__node_base*的数组
    __bucket_type     _M_single_bucket   = nullptr;
    __bucket_type*    _M_buckets         = &_M_single_bucket;
    size_type         _M_bucket_count    = 1;
    size_type         _M_element_count   = 0;
    __node_base       _M_before_begin;  // 头节点
    _RehashPolicy     _M_rehash_policy;
};
```

可以看出，`_Hash_node` 和 `_Hash_node_base` 实际上才是核心数据结构。这两个类也是息息相关的：

```cpp
/*
_Hash_node_base // 仅持有 next 指针
       ↑
_Hash_node_value_base<_Value> // 添加数据存储能力
       ↑
_Hash_node<_Value, true>   // 添加哈希值缓存
*/

template<typename _Value>
struct _Hash_node<_Value, true> : _Hash_node_value_base<_Value>
{
    std::size_t  _M_hash_code;  // 缓存哈希值，避免重复计算
    _Hash_node* _M_next() const noexcept { 
        return static_cast<_Hash_node*>(this->_M_nxt); 
    }
};

template<typename _Value>
struct _Hash_node_value_base : _Hash_node_base
{
    typedef _Value value_type;
    __gnu_cxx::__aligned_buffer<_Value> _M_storage;
};

struct _Hash_node_base
{
    _Hash_node_base* _M_nxt;
    _Hash_node_base() noexcept : _M_nxt() { }
    _Hash_node_base(_Hash_node_base* __next) noexcept : _M_nxt(__next) {}
};
```

综上，同经典的 vector + 链表的 buckets 相比，STL 中 `_Hashtable` 类的 buckets 的实现为一个元素类型为的 `_Hash_node_base*` 数组。

便于大家后面理解源代码，我先将 STL 中 `_Hashtable` 的原理图展示给大家，实际上这个原理图也是我看了源代码之后才画出来的，我将这种实现称之为“单链”法。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0qFBX1tGnib0iaFicsZzK6F4S67atWlQDCdQBGicibXkVCm2c5zpfthrYjCriawSUhBwdrJriacNlBhgkSA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=3)

可以看出，STL 中也是采用开链法解决 hash 冲突，但是并不是一个 slot 一个链表，而是整个 hash 表是一个链表，<font color="blue">“前一个” slot 的尾节点指向“下一个”非空 slot 的头节点，slot 中存储的是头节点的 prev 节点的地址。</font>这里要着重强调 **链表并不是按照 slot 数组数组即桶的前后顺序串联起来的，而是根据节点的插入时间串联起来的**。

> 单链法相较于每个 slot 对应一个单独的链表，一个显而易见的好处就是遍历 hashtable 中所有元素的速度更快了，因为我们此时遍历的是一个单链表。

可能很多人看到这里就蒙了，下面我们详细说明一下。实际上想要明白这些，只需要看一下 `_Hashtable` 的插入节点的方法就行：

```cpp
// 插入函数的底层均调用 _M_insert_bucket_begin
// __bkt 为 slot 的索引，__node 为需要插入的节点，使用 key 与 value 构造出来
// _M_before_begin 是虚拟头节点
// _M_buckets 桶数组
// bucket包含的元素规则是: 桶指向前驱
template<typename _Key, typename _Value,
   typename _Alloc, typename _ExtractKey, typename _Equal,
   typename _H1, typename _H2, typename _Hash, typename _RehashPolicy,
   typename _Traits>
void _Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _H1, _H2, 
    _Hash, _RehashPolicy, _Traits>::_M_insert_bucket_begin
    (size_type __bkt, __node_type* __node)
{
    // 如果所要插入的 slot 已经有节点，直接插入头部
    if (_M_buckets[__bkt])
    {
        // Bucket is not empty, we just need to insert the new node
        // after the bucket before begin.
        __node->_M_nxt = _M_buckets[__bkt]->_M_nxt;
        _M_buckets[__bkt]->_M_nxt = __node;
    }
    // 如果所要插入的 slot 还没插入过节点
    else
    {
        // The bucket is empty, the new node is inserted at the
        // beginning of the singly-linked list and the bucket will
        // contain _M_before_begin pointer.
        __node->_M_nxt = _M_before_begin._M_nxt;
        _M_before_begin._M_nxt = __node;

        // 如果新节点后面还有节点，那么原来第一个节点所在的桶，其桶指针必须从指向 _M_before_begin 改为指向 __node
        if (__node->_M_nxt)
            // We must update former begin bucket that is pointing to
            // _M_before_begin.
            _M_buckets[_M_bucket_index(__node->_M_next())] = __node;
  
        // 将当前要插入的桶 __bkt 设置为指向 _M_before_begin
        _M_buckets[__bkt] = &_M_before_begin;
    }
}
```

接下来我们用图逐步解析一下这个过程：

第一步：先在 slot49 中依次插入三个元素，由于是在头部插入，因此示意图如下：

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib20BiaRTlZKbMV6rgicnTbN87ibmiaLdmOGRO3MYRXl5iaJEmhzpMp5oia4qvpkUqS2sKBOYV31TZN5PZLA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=4)

第二步：在 slot46 中插入一个元素 4，示意图如下：

2.1先将新节点 4 指向 `_M_before_begin` 的 `next`;

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib20BiaRTlZKbMV6rgicnTbN87gtcSn6q276wQUCFribicXiaMJOAPqHHKkQhHXRhYZAU4ZCOYMPNiaF8GWQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=5)

2.2 再将 `_M_before_begin` 的 next 指向新节点4：

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib20BiaRTlZKbMV6rgicnTbN87rK3PDVz3l7f0H02M9n0aF5qe7Oa9z0wVlPgic0LcicmvECpgn8l9GCOw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=6)

2.3 根据 `_M_buckets[_M_bucket_index(__node->_M_next())] = __node;`，将 Slot49 修改为新节点 4 的地址：

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib20BiaRTlZKbMV6rgicnTbN87F0mt5Kq27h1kaWLS44AWJeTlxE9l9CVOwmDqqUdVLk6hzLdvPGJF2Q/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=7)

2.4 将 Slot46 指向 `_M_before_begin`

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib20BiaRTlZKbMV6rgicnTbN87fFdavribD5d3hoJjWje4x4MkIbLKy0ugaagNHTVFFw5wco46fa2XmKQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=8)

第三步：在 slot3 中插入一个元素 5，示意图如下：

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib20BiaRTlZKbMV6rgicnTbN871tFNg3Xnw4bqmHpyUEuSScNCA3G9jPiauLBsX5loABvgFFOKegz3ooA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=9)

最后依次再插入更多的元素：

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0qFBX1tGnib0iaFicsZzK6F4S67atWlQDCdQBGicibXkVCm2c5zpfthrYjCriawSUhBwdrJriacNlBhgkSA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=10)

关于 `_Hashtable`，只要明白了这些原理，我相信每个人都能够得心应手的使用 unordered_map 和 unordered_set。

这次没有涉及到的问题就是 hash 表的 rehash 过程，有了原理的加持，这些应该都不是问题，希望大家能够自己完成这部分的剖析，一起交流~

#### 总结

C++11 标准中的 `unordered_map` 并未规定底层哈希表的具体实现方式，但 **GCC 的 libstdc++ 实现采用了一种不同于传统“动态数组 + 多个独立链表”的设计**。

##### 传统哈希表的问题

在传统实现中：

- 哈希表由一个动态数组（buckets）构成；
- 每个 bucket 指向一个独立的单链表（或为空）；
- **遍历时需逐个检查每个 bucket 是否为空**，若为空则跳过，直到找到下一个非空 bucket；
- 当负载因子较低（即空桶较多）时，这种跳转会带来显著的遍历开销，实际复杂度接近 O(n + bucket_count)，而非理想的 O(n)。

##### libstdc++ 的优化：全局单链表 + 前驱指针

为解决上述问题，GCC 的 `_Hashtable` 实现采用了以下策略：

- 所有元素节点被组织成**一个全局的单向链表**，按插入顺序串联；
- 动态数组（buckets）中的每个元素**不再直接指向链表头**，而是存储**该桶第一个节点的前驱节点指针**；
- 特别地，若某桶的第一个节点是整个链表的首节点，则其 bucket 指向一个虚拟头节点 `_M_before_begin`；
- 因此，桶 i 中的第一个元素可通过 `_M_buckets[i]->_M_nxt` 访问。

##### 优势

- **遍历高效**：迭代器只需沿全局单链表顺序访问，无需跳过空桶，时间复杂度严格为 O(n)；
- **内存紧凑**：无需为每个 bucket 维护独立的链表头对象；
- **插入仍为 O(1)**：通过哈希定位桶后，利用前驱指针可在桶内头部快速插入。

📝 注：此设计是 **GCC libstdc++ 的实现细节**，并非 C++11 标准强制要求。其他标准库（如 Clang 的 libc++ 或 MSVC STL）可能采用不同策略。

## `priority_queue`

binary-heap 本质上是一种完全二叉树，有最大堆和最小堆两种类型。binary-heap 的重要应用是用来实现  priority queue。STL 也提供了一种叫做 priority_queue 的<font color="blue">容器适配器</font>，它的底层就是用<font color="blue">最大堆</font>来实现。

今天我们就来学习一下 STL 中的 priority_queue 和 max-heap。

### 0. binary_heap 的特点

要学习二叉堆，我们需要知道它的特点。二叉堆分为大顶堆和小顶堆：大顶堆指的是任何一个父节点的值都大于或等于它左右子节点的值；小顶堆指的是任何一个父节点的值都小于或等于它左右子节点的值，注意这两个过程都是<font color="blue">递归定义</font>的。

二叉堆虽然是一个完全二叉树，但是它的存储方式并不是链式存储，而是<font color="blue">顺序存储</font>的。也就是说，二叉堆的元素都存在于数组中，由于数组的大小是固定的，因此我们一般使用 vector 来实现 heap。

当数组下标从 $0$ 开始时，我们可以发现，当某个节点位于数组的 $i$ 处时：

* 左子节点必位于数组的 $2i+1$处
* 右子节点必位于数组的 $2i+2$ 处
* 父节点必位于 $(i-1)/2$ 处。

#### 1. 插入元素（Push）

**步骤**：

1. 将新元素插入到数组末尾（即树的最底层最左侧空位）；
2. 执行 “上溯”（sift-up / percolate-up）：
   - 比较该节点与其父节点；
   - 若违反堆序（如在最大堆中比父大），则交换；
   - 重复直到满足堆序或到达根节点。

**时间复杂度**：*O*(log*n*)

#### 2. 删除堆顶元素（Pop）

**只能高效删除堆顶（根节点）**，这是堆的设计目的。

**步骤**：

1. 将最后一个叶子节点移到堆顶（覆盖原根）；
2. 执行 “下溯”（sift-down / heapify-down）：
   - 比较当前节点与其**较大（最大堆）或较小（最小堆）**的子节点；
   - 若违反堆序，则与该子节点交换；
   - 重复直到满足堆序或成为叶子。

**时间复杂度**：*O*(log*n*)

#### 3. 构建二叉堆（Heapify）

给定一个无序数组，如何快速构建堆？

- **方法**：自底向上对所有**非叶子节点**执行 `sift-down`。
- **起始位置**：最后一个非叶子节点索引为 ⌊2*n*⌋−1（0-based）。
- **为什么有效**？因为叶子节点天然满足堆性质，无需处理。
- **时间复杂度**：***O\*(\*n\*)**

虽然单次 `sift-down` 最坏 *O*(log*n*)，但由于底层节点多、高度低，总代价线性。

#### 4. 堆排序（Heap Sort）

利用最大堆实现**升序排序**：

步骤：

1. **建堆**：将数组构造成最大堆（*O*(*n*)）；
2. 重复提取最大值

   （共 *n*−1 次）：

   - 交换堆顶（最大值）与堆尾元素；
   - 缩小堆的大小（排除已排序部分）；
   - 对新堆顶执行 `sift-down` 恢复堆性质。

复杂度：

- 时间：*O*(*n*log*n*)
- 空间：*O*(1)（原地排序）
- **不稳定排序**

### 1. `std::priority_queue`

STL 中并没有一个叫做 heap 的类，而是在 `<stl_heap.h>` 中提供了一系列的算法，这些算法包括插入元素时执行的“上溯”和删除元素时执行的“下溯”，堆排序和构建堆，STL 中堆的底层是使用 vector 来管理元素。这些算法都接受两个 `_RandomAccessIterator`。接下来我们来分析一下源码。

> 父子索引关系（0-based）：
>
> - 父节点：`(i - 1) / 2`
> - 左子：`2i + 1`
> - 右子：`2i + 2`

#### 1. `push_heap`

```cpp
template<typename _RandomAccessIterator>
inline void
push_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    typedef typename iterator_traits<_RandomAccessIterator>::value_type  
        _ValueType;
    typedef typename iterator_traits<_RandomAccessIterator>::difference_type 
        _DistanceType;
    __gnu_cxx::__ops::_Iter_less_val __comp;
    // __value为新插入的元素
    _ValueType __value = _GLIBCXX_MOVE(*(__last - 1));
    // __value的位置为(__last - __first) - 1
    std::__push_heap(__first, _DistanceType((__last - __first) - 1),
         _DistanceType(0), _GLIBCXX_MOVE(__value), __comp);
}


template<typename _RandomAccessIterator, typename _Distance, 
    typename _Tp, typename _Compare>
void
__push_heap(_RandomAccessIterator __first, // 堆的起始位置
            _Distance __holeIndex,         // 新插入所在索引
            _Distance __topIndex,          // 堆顶索引
            _Tp __value,                   // 要插入的元素值
            _Compare& __comp)              // _Iter_less_val 
{
    // 首先计算出父节点的位置，根据(i-1)/2
    _Distance __parent = (__holeIndex - 1) / 2;
    // 只要待上溯（新插入）的节点位置不是根节点，且父节点小于新插入的节点，就执行上溯
    while (__holeIndex > __topIndex && __comp(__first + __parent, __value))
    {
        // 父节点下移
        *(__first + __holeIndex) = _GLIBCXX_MOVE(*(__first + __parent));
        // 待上溯的节点位置变成了父节点的位置
        __holeIndex = __parent;
        // 计算到达新位置后其父节点的位置
        __parent = (__holeIndex - 1) / 2;
    }
    *(__first + __holeIndex) = _GLIBCXX_MOVE(__value);
}
```

#### 2. `pop_heap`

```cpp
// pop_back 用于调整堆时将堆顶元素放到堆尾，而不是真的删除元素
// 因此当堆内元素 <= 1 时，无需调整堆
template<typename _RandomAccessIterator>
inline void
pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    if (__last - __first > 1)
    {
        -- __last;   // 此时 last 指向最后一个元素
        __gnu_cxx::__ops::_Iter_less_iter __comp; // __comp 默认为 _Iter_less_iter
        std::__pop_heap(__first, __last, __last, __comp);
    }
}


// __first：堆起始
// __last：新堆的结束
// __result：用于存放被弹出的堆顶
// _comp：比较器，这里是 _Iter_less_iter，等价于 *a < *b
template<typename _RandomAccessIterator, typename _Compare>
inline void
__pop_heap(_RandomAccessIterator __first, 
            _RandomAccessIterator __last,
            _RandomAccessIterator __result, 
            _Compare& __comp)
{
    typedef typename iterator_traits<_RandomAccessIterator>::value_type
        _ValueType;
    typedef typename iterator_traits<_RandomAccessIterator>::difference_type
        _DistanceType;

    // 交换堆顶与最后一个元素的值
    _ValueType __value = _GLIBCXX_MOVE(*__result);
    *__result = _GLIBCXX_MOVE(*__first);

    // 执行__adjust_heap调整堆，待调整的元素为堆顶元素_DistanceType(0)
    std::__adjust_heap(__first, 
                       _DistanceType(0),
                       _DistanceType(__last - __first),
                       _GLIBCXX_MOVE(__value), 
                       __comp);
}


// shift-down
template<typename _RandomAccessIterator, typename _Distance, typename _Tp, typename _Compare>
void __adjust_heap(
    _RandomAccessIterator __first,   // 堆的起始位置（数组首地址）
    _Distance __holeIndex,           // 当前“空洞”位置（通常是 0，堆顶）
    _Distance __len,                 // 堆的长度（即 [__first, __first + __len) 是待调整的堆）
    _Tp __value,                     // 要插入到堆中的值（来自原堆末尾）
    _Compare __comp                  // 比较器（默认是 <，用于最大堆）
){
    const _Distance __topIndex = __holeIndex; 
    _Distance __secondChild = __holeIndex;

    // 找到“空洞”的最终位置
    while (__secondChild < (__len - 1) / 2) // (__len - 1) / 2 为最后一个根节点的位置
    {
        // 计算右子节点的位置
        __secondChild = 2 * (__secondChild + 1);

        // 找出左右子节点的较大者
        if (__comp(__first + __secondChild, __first+ (__secondChild - 1)))
            __secondChild -- ;

        // 将较大者上浮，对换与栈顶的位置
        *(__first + __holeIndex) = _GLIBCXX_MOVE(*(__first + __secondChild));

        // 待下沉的新节点
        __holeIndex = __secondChild;
    }

    // 处理只有左子节点的情况
    if ((__len & 1) == 0 && __secondChild == (__len - 2) / 2)
    {
        // 交换当前节点与它的左子节点
        __secondChild = 2 * (__secondChild + 1);
        *(__first + __holeIndex) = _GLIBCXX_MOVE(*(__first + (__secondChild - 1)));
        __holeIndex = __secondChild - 1;
    }

    __decltype(__gnu_cxx::__ops::__iter_comp_val(_GLIBCXX_MOVE(__comp)))
               __cmp(_GLIBCXX_MOVE(__comp));

    // 把 __value 插入当前 hole，并可能上推
    std::__push_heap(__first, __holeIndex, __topIndex, _GLIBCXX_MOVE(__value), __cmp);
}
```

传统教科书的 sift-down 是这样写的：

```cpp
while (有子节点) {
    找较大子节点；
    if (__value < 较大子) {
        把较大子上移；
        hole 下移；
    } else break;
}
把 __value 放入 hole；
```

但 libstdc++ 的实现更高效：**它假设 `__value` 很小，会一直沉到底部，所以先不管它，一路把路径上的较大子节点往上提，直到叶子层，最后再把 `__value` 插入，并检查是否需要“微调上浮”**。

> 这样做的好处：**减少比较次数**（主循环中只比较子节点之间，不和 `__value` 比），对现代 CPU 更友好。

#### 3. `make_heap`

```cpp
template<typename _RandomAccessIterator>
inline void
make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __gnu_cxx::__ops::_Iter_less_iter __comp;
    std::__make_heap(__first, __last, __comp);
}


template<typename _RandomAccessIterator, typename _Compare>
void __make_heap(_RandomAccessIterator __first, _RandomAccessIterator
             __last,
            _Compare& __comp)
{
    typedef typename iterator_traits<_RandomAccessIterator>::value_type
        _ValueType;
    typedef typename iterator_traits<_RandomAccessIterator>::difference_type
        _DistanceType;

    // 小于两个元素时无需调整
    if (__last - __first < 2)
        return;

    const _DistanceType __len = __last - __first;
    // 最后一个元素的父节点为(__len - 2) / 2
    _DistanceType __parent = (__len - 2) / 2;
    while (true)
    {
        // 从最后一个非叶子节点开始调整堆
        _ValueType __value = _GLIBCXX_MOVE(*(__first + __parent));
        std::__adjust_heap(__first, __parent, __len, _GLIBCXX_MOVE(__value),
                           __comp);
        if (__parent == 0)
            return;
        __parent--;
    }
}
```

#### 4. `sort_heap`

通过 `pop_heap` 的源码解读我们发现每次 `pop_heap` 就会将堆顶（最大值）放到堆尾，因此当对堆中的每个元素执行一遍 `pop_heap` 操作后就会得到一个递增序列：

```cpp
template<typename _RandomAccessIterator>
inline void
sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last)
{
    __gnu_cxx::__ops::_Iter_less_iter __comp;
    std::__sort_heap(__first, __last, __comp);
}


template<typename _RandomAccessIterator, typename _Compare>
void
__sort_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
            _Compare& __comp)
{
    // 依次将堆顶元素（最大值）放到堆的末尾
    // 最终就能得到一个递增序列
    while (__last - __first > 1)
    {
        --__last;
        std::__pop_heap(__first, __last, __last, __comp);
    }
}
```

#### 5. `priority_queue` 的源码分析

缺省情况下 priority_queue 的底部容器为 vector，再加上 heap 的处理规则。如下所示为其源代码：

```cpp
template<typename _Tp, typename _Sequence = vector<_Tp>,
   typename _Compare  = less<typename _Sequence::value_type>>
class priority_queue
{
public:
    typedef typename  _Sequence::value_type          value_type;
    typedef typename  _Sequence::reference           reference;
    typedef typename  _Sequence::const_reference     const_reference;
    typedef typename  _Sequence::size_type           size_type;
    typedef    _Sequence          container_type;
    typedef    _Compare           value_compare;

protected:
    _Sequence  c;
    _Compare   comp;

    // 构造函数
    priority_queue() : c(), comp() { }
  
    explicit
    priority_queue(const _Compare& __x, const _Sequence& __s)
        : c(__s), comp(__x)
        { std::make_heap(c.begin(), c.end(), comp); }

    explicit
    priority_queue(const _Compare& __x, _Sequence&& __s = _Sequence())
        : c(std::move(__s)), comp(__x)
        { std::make_heap(c.begin(), c.end(), comp); }

    template<typename _InputIterator>
    priority_queue(_InputIterator __first, _InputIterator __last,
             const _Compare& __x,
             const _Sequence& __s)
    : c(__s), comp(__x)
    {
        __glibcxx_requires_valid_range(__first, __last);
        c.insert(c.end(), __first, __last);
        std::make_heap(c.begin(), c.end(), comp);
    }

    // 插入元素
    void push(const value_type& __x)
    {
        c.push_back(__x);
        std::push_heap(c.begin(), c.end(), comp);
    }

    // 删除元素
    void pop()
    {
        __glibcxx_requires_nonempty();
        std::pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }
};
```

## `std::list`

### 0. list 的结构

list 是一个双向链表，而且是一个<font color="blue">环状双向链表</font>。如图所示，为 list 的示意图。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_jpg/7kDbTKZllib0biaGv0KgdYgibNHu2rdWwfDiaiaMbmLCNDiczrpbHbA1GBfYUPAE3fz6Pk8B0v88XTxrTpUNDQeCnmTw/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=0)

可以看出 list 是由节点组成的，每个节点有三部分组成，指向前一个节点的指针，指向后一个节点的指针和数据。

另外，为了符合 STL 规范之<font color="blue">“前闭后开”</font>区间，在环状链表的尾端加上一个空白节点即可，这个节点就是我们的 end()。

### 1. list 的节点

因为 list 是由每个节点组成的，所以我们先看节点结构的源码。list 源码的节点结构分为 `_List_node_base` 和 `_List_node`，后者继承前者。

`_List_node_base` 的源码如下。可以看出，`_List_node_base` 用来存放这个节点指向前一个节点的指针 `_M_prev` 和指向后一个节点的指针 `_M_next`。

```cpp
namespace __detail
{
    _GLIBCXX_BEGIN_NAMESPACE_VERSION
    /// Common part of a node in the %list. 
    struct _List_node_base
    {
      // 指针
      _List_node_base* _M_next;
      _List_node_base* _M_prev;

      // 交换两个节点在链表中的位置
      static void
      swap(_List_node_base& __x, _List_node_base& __y) _GLIBCXX_USE_NOEXCEPT;
  
      // 将 [__first, __last) 这个左闭右开区间内的所有节点移动到当前节点之前
      void
      _M_transfer(_List_node_base* const __first,
          _List_node_base* const __last) _GLIBCXX_USE_NOEXCEPT;

      // 将以当前节点为头（或作为链表一部分）的整个链表反转
      void
      _M_reverse() _GLIBCXX_USE_NOEXCEPT;
  
      // 将当前节点插入到 __position 所指向节点的前面
      void
      _M_hook(_List_node_base* const __position) _GLIBCXX_USE_NOEXCEPT;

      // 将当前节点从链表中移除（断开前后连接）
          // 注意：不释放内存，只是修改指针，将节点从链表中摘除
      void
      _M_unhook() _GLIBCXX_USE_NOEXCEPT;
    };

    _GLIBCXX_END_NAMESPACE_VERSION
} // namespace detail
```

`_List_node` 的源码如下，`_List_node` 继承 `_List_node_base`，保存节点数据。

```cpp
/// An actual node in the %list.
template<typename _Tp>
struct _List_node : public __detail::_List_node_base
{
  // 数据
  _Tp _M_data;

#if __cplusplus >= 201103L
  template<typename... _Args>
    _List_node(_Args&&... __args)
: __detail::_List_node_base(), _M_data(std::forward<_Args>(__args)...) 
    { }
#endif
};
```

### 2. list

`list` 的类型定义如下，可以发现 `list` 继承了一个 `_List_base` 基类。

```cpp
template <typename _Tp, typename _Alloc = std::allocator<_Tp>>
class list : protected _List_base<_Tp, _Alloc> {
    // 创建一个新节点
    // _M_get_node() 是从 _List_base 继承来的函数，调用 allocator 分配一个 _Node 类型的原始内存（类型通常是 _List_node<_Tp>）
    // 然后在此内存上 placement-new 构造 _Tp 对象
    template <typename... _Args>
    _Node* _M_create_node(_Args&&... __args) {
        auto __p = this->_M_get_node();
        _Node* __p = this->_M_get_node();
        try {
            ::new ((void*)__p) _Tp(std::forward<_Args>(__args)...);
        }
        catch (...) {
            this->_M_put_node(__p);  // 异常安全：释放内存
            throw;
        }
        return __p;
    }

    // O(n)
    // std::list 的迭代器是 双向迭代器（BidirectionalIterator），不支持随机访问，所以 distance 必须遍历
    static size_t _S_distance(const_iterator __first, const_iterator __last) {
        return std::distance(__first, __last);
    }

    // _M_get_size() 来自 _List_base，返回缓存的元素个数
    // 若定义了 _GLIBCXX_USE_CXX11_ABI（默认开启）
    // 则 list 内部维护一个 size_t 成员。
    // 否则，size() 会遍历链表（但现代 GCC 基本都启用缓存）
    size_t _M_node_count() const {
        return this->_M_get_size();
    }

    // 构造函数（后续详述）
    // ...
};
```

`_List_base` 基类的定义如下：

```cpp
class _List_base {
protected:
    typedef typename _Alloc::template rebind<_List_node<_Tp>>::other _Node_alloc_type;
    typedef typename _Alloc::template rebind<_Tp>::other _Tp_alloc_type;

    // 分配一个节点
    typename _Node_alloc_traits::pointer _M_get_node() {
        return _Node_alloc_traits::allocate(_M_impl, 1);
    }

    // 释放一个节点
    void _M_put_node(typename _Node_alloc_traits::pointer __p) _GLIBCXX_NOEXCEPT {
        _Node_alloc_traits::deallocate(_M_impl, __p, 1);
    }

    struct _List_impl : public _Node_alloc_type {
        __detail::_List_node_base _M_node;

        _List_impl() : _Node_alloc_type(), _M_node() {}

        _List_impl(const _Node_alloc_type& __a) _GLIBCXX_NOEXCEPT : _Node_alloc_type(__a), _M_node() {}

#if __cplusplus >= 201103L
        _List_impl(_Node_alloc_type&& __a) _GLIBCXX_NOEXCEPT : _Node_alloc_type(std::move(__a)), _M_node() {}
#endif
    };

    // 空白头节点（即 end() 所指向的哨兵节点）
    _List_impl _M_impl;
};
```

> **说明**：`_M_impl._M_node` 是一个不存储有效数据的“空白节点”，作为双向循环链表的头节点（哨兵），其 `end()` 迭代器即指向此节点。

#### 构造函数

```cpp
// 默认构造函数
explicit
list(const allocator_type &__a) _GLIBCXX_NOEXCEPT: _Base(_Node_alloc_type(__a)) {}

// 用 n 个相同的值初始化
explicit list(size_type __n, const value_type &__value = value_type(),const allocator_type &__a = allocator_type()) : _Base(_Node_alloc_type(__a)) 
{ _M_fill_initialize(__n, __value); }

// 用一个连续的数据初始化
template<typename _InputIterator>
list(_InputIterator __first, _InputIterator __last,
     const allocator_type &__a = allocator_type())
        : _Base(_Node_alloc_type(__a)) {
    // Check whether it's an integral type.  If so, it's not an iterator.
    typedef typename std::__is_integer<_InputIterator>::__type _Integral;
    _M_initialize_dispatch(__first, __last, _Integral());
}
```

在构造函数中调用了`_M_fill_initialize` 函数和 `_M_initialize_dispatch` 函数。

`_M_fill_initialize` 的源码如下，可以看出 `_M_fill_initialize` 就是在尾部插入，调用了 `push_back` 函数：

```cpp
void _M_fill_initialize(size_type __n, const value_type &__x) {
    for (; __n; --__n)
        push_back(__x);
}
```

`push_back` 的源码如下，底层是 `_M_insert`：

```cpp
void push_back(const value_type &__x) { 
    this->_M_insert(end(), __x); 
}

// push_front 底层也是调用 _M_insert 函数
void push_front(const value_type &__x) { 
    this->_M_insert(begin(), __x); 
}
```

那么 `_M_insert` 的底层又是什么呢？通过看源码，我们发现，首先构造一个节点，然后将其插入到指定位置。调用的是 `_List_node_base` 的 `_M_hook` 函数，表示在某个位置插入值为 `__x` 节点。

```cpp
void _M_insert(iterator __position, const value_type &__x) {
    _Node *__tmp = _M_create_node(__x);
    // 将 __tmp 插入到 __position._M_node 的全面
    __tmp->_M_hook(__position._M_node);
}

// __tmp 转化为 this
void_List_node_base::_M_hook(_List_node_base* const __position) _GLIBCXX_USE_NOEXCEPT
{
  this->_M_next = __position;
  this->_M_prev = __position->_M_prev;
  __position->_M_prev->_M_next = this;
  __position->_M_prev = this;
}
```

`_M_initialize_dispatch` 的源码如下，可以看出底层仍然调用的是 `push_back` 函数

```cpp
void _M_initialize_dispatch(_InputIterator __first, _InputIterator __last) {
    for (; __first != __last; ++__first)
#if __cplusplus >= 201103L
        empalce_back(*__first);
#else
        push_back(*__first);
}
```

#### 插入元素

```cpp
// 在指定位置前插入指定元素的节点
template<typename _Tp, typename _Alloc>
typename list<_Tp, _Alloc>::iterator
list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
insert(const_iterator __position, const value_type& __x)
#else
insert(iterator __position, const value_type& __x)
#endif
{
  _Node* __tmp = _M_create_node(__x);
  __tmp->_M_hook(__position._M_const_cast()._M_node);
  return iterator(__tmp);
}

// 在指定位置前插入 n 个指定值的节点
void insert(iterator __position, size_type __n, const value_type &__x) {
    list __tmp(__n, __x, get_allocator());
    splice(__position, __tmp);
}

// 将一个区间插入到指定位置前
template<typename _InputIterator>
void insert(iterator __position, _InputIterator __first, _InputIterator __last) {
    list __tmp(__first, __last, get_allocator());
    splice(__position, __tmp);
}
```

看一下 `splice` 的源码，是将一个 `list` 插入到指定位置，底层调用的则是 `_M_transfer`

```cpp
void splice(iterator __position, list &__x)
{
    if (!__x.empty()) {
        _M_check_equal_allocators(__x);

        this->_M_transfer(__position._M_const_cast(),
                          __x.begin(), __x.end());
    }
}
```

`_M_transfer` 的源码如下，调用的是 `_List_node_base` 的 `_M_transfer` 函数

```cpp
void _M_transfer(iterator __position, iterator __first, iterator __last) {
    __position._M_node->_M_transfer(__first._M_node, __last._M_node);
}
```

`_List_node_base` 的 `_M_transfer` 函数的源码如下

```cpp
// 将区间 [__first, __last)（左闭右开）从原链表中剪切，并插入到 this 节点之前
void _List_node_base::
_M_transfer(_List_node_base * const __first,
    _List_node_base * const __last) _GLIBCXX_USE_NOEXCEPT
{
  if (this != __last)
  {
      __last->_M_prev->_M_next  = this;    // (1)
      __first->_M_prev->_M_next = __last;  // (2)
      this->_M_prev->_M_next    = __first; // (3)

      _List_node_base* const __tmp = this->_M_prev;
      this->_M_prev                = __last->_M_prev; // (4)
      __last->_M_prev              = __first->_M_prev;// (5)
      __first->_M_prev             = __tmp;		      // (6)
  }
}
```

![list::splice](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763796112889.9rjtzxmq5y.webp)

#### 删除元素

之前从 list 的构造函数一直剖析到 insert 函数。接下来，我们直接剖析 list 的删除函数。

```cpp
// 哨兵节点(空节点,end节点)：_M_impl._M_node

// 移除头节点
void pop_front() _GLIBCXX_NOEXCEPT { 
    this->_M_erase(begin()); // begin() = _M_impl._M_node._M_next（哨兵后第一个节点）
}

// 移除尾节点点
void pop_back() _GLIBCXX_NOEXCEPT { 
    this->_M_erase(iterator(this->_M_impl._M_node._M_prev)); 
}


// 删除迭代器所指节点，返回下一个有效迭代器
iterator erase(iterator __position)
{
  iterator __ret = iterator(__position._M_node->_M_next);
  _M_erase(__position._M_const_cast());
  return __ret;
}

// 删除一个区间内的元素，是逐个调用erase(iterator __position)进行删除
// 返回 last
iterator
#if __cplusplus >= 201103L
erase(const_iterator __first, const_iterator __last) noexcept
#else
erase(iterator __first, iterator __last)
#endif
{
    while (__first != __last)
        __first = erase(__first);
    return __last._M_const_cast();
}

// 将数值为 value 的节点移除
// 当 __value 本身是链表中的某个元素的引用时，不能在遍历过程中直接删除它，否则会导致悬空引用（dangling reference），从而引发未定义行为（UB）
template<typename _Tp, typename _Alloc>
void list<_Tp, _Alloc>::remove(const value_type& __value)
{
  iterator __first = begin();
  iterator __last  = end();
  iterator __extra = __last; // 记录自引用情况下的待删节点，初始时初始化为 end() 表示暂无特殊待删节点
  while (__first != __last)
  {
      iterator __next = __first;
      ++ __next;
      if (*__first == __value)
        {
          // 这是一个保护操作，避免删除“自引用”节点
          if (std::__addressof(*__first) != std::__addressof(__value))
        _M_erase(__first);
          else
        __extra = __first;
        }
      __first = __next;
  }
  // 之前为了保护迭代器存起来，现在删除
  if (__extra != __last)
    _M_erase(__extra);
}


template<typename _Tp, typename _Alloc>
template <typename _Predicate>
void list<_Tp, _Alloc>::remove_if(_Predicate __pred)
{
    iterator __first = begin();
    iterator __last  = end();
    while (__first != __last)
    {
        iterator __next = __first;
        ++__next;
        if (__pred(*__first))
          _M_erase(__first);
        __first = __next;
    }
}
```

* 注意 `std::list::remove(const T& value)` 的实现，我们需要对<font color="red">“自引用”</font>的情况做特殊处理。

  考虑以下代码：

  ```cpp
  std::list<int> lst = {1, 2, 3, 2, 4};
  lst.remove(lst.front());   // 删除所有值等于 lst[0]（即 1）的元素
  ```

  或者更危险的例子：

  ```cpp
  auto it = lst.begin();
  ++it; // 指向 2
  lst.remove(*it);  // 删除所有值为 2 的元素，而 *it 就是 lst 中的一个元素！
  ```

  此时，`__value` 是 `lst` 中某个节点的**引用**（比如 `*it`）。

  如果我们**在遍历到该节点之前就把它删了**，那么 `__value` 就变成了**悬空引用**——后续再用 `__value`（比如和其他元素比较）就是未定义行为！

  **解决方案：延迟删除**

  - 使用 `std::__addressof(x)` 获取对象的真实地址（比 `&x` 更安全，可绕过重载的 `operator&`）。
  - 比较：
    - `&(*__first)`：当前元素在链表中的地址
    - `&__value`：传入参数的地址

  **如果两个地址不同**：

  - 说明 `__value` 是一个“外部值”（比如字面量、局部变量、其他容器的元素等）
  - 可以安全删除 `__first` → 立即调用 `_M_erase(__first)`

  **如果两个地址相同**：

  - 说明 `__value` 就是 `*__first` 本身（即引用的是链表中的这个元素！）
  - 此时**不能立即删除**，否则 `__value` 失效
  - 所以先记住这个位置：`__extra = __first`
  - 继续遍历，**等所有其他元素都处理完后再删它**

上面所有删除操作都围绕一个核心底层操作：`_M_erase(iterator)`。源码如下：

```cpp
_M_erase(iterator __position)
_GLIBCXX_NOEXCEPT
{
    // 调用_M_unhook
    __position._M_node->_M_unhook();
    _Node *__n = static_cast<_Node *>(__position._M_node);
    // 后面的操作都是释放内存用的
#if __cplusplus >= 201103L
    _M_get_Node_allocator().destroy(__n);
#else
    _M_get_Tp_allocator().destroy(std::__addressof(__n->_M_data));
#endif
    _M_put_node(__n);
}
```

而 `_M_erase` 函数的底层则调用，节点类的 `_M_unhook` 函数

```cpp
void _List_node_base::_M_unhook() _GLIBCXX_USE_NOEXCEPT
{
  // 把当前节点的后驱节点保存起来
  _List_node_base* const __next_node = this->_M_next;
  // 把当前节点的前驱节点保存起来
  _List_node_base* const __prev_node = this->_M_prev;
  // 前驱节点的next指向后继节点
  __prev_node->_M_next = __next_node;
  // 后继节点的prev指向前驱节点
  __next_node->_M_prev = __prev_node;
}
```

list 中和删除元素相关的函数还有一个 `clear` 函数，调用 `_M_clear`

```cpp
void clear()
_GLIBCXX_NOEXCEPT
{
  _Base::_M_clear();
  _Base::_M_init();
}


_List_base<_Tp, _Alloc>::
_M_clear() _GLIBCXX_NOEXCEPT
{
  typedef _List_node<_Tp>  _Node;
  // 从第一个元素开始
  _Node* __cur = static_cast<_Node*>(_M_impl._M_node._M_next);
  // 逐个释放内存
  while (__cur != &_M_impl._M_node)
    {
      _Node* __tmp = __cur;
      __cur = static_cast<_Node*>(__cur->_M_next);
    #if __cplusplus >= 201103L
      _M_get_Node_allocator().destroy(__tmp);
    #else
      _M_get_Tp_allocator().destroy(std::__addressof(__tmp->_M_data));
    #endif
      _M_put_node(__tmp);
    }
}

// _M_impl._M_node 是哨兵节点 end()
void _M_init()
_GLIBCXX_NOEXCEPT
{
    // 自环
    this->_M_impl._M_node._M_next = &this->_M_impl._M_node;
    this->_M_impl._M_node._M_prev = &this->_M_impl._M_node;
}
```

#### 元素访问

list 中还有一些专门返回 list 元素的函数，如 begin、end、front、back：

```cpp
// _M_impl._M_node 是哨兵节点 end()
iterator begin() _GLIBCXX_NOEXCEPT
{ return iterator(this->_M_impl._M_node._M_next); }

iterator end() _GLIBCXX_NOEXCEPT
{ return iterator(&this->_M_impl._M_node); }

// 返回首个元素
reference front() _GLIBCXX_NOEXCEPT
{ return *begin(); }

const_reference front() const _GLIBCXX_NOEXCEPT
{ return *begin(); }

// 返回的是最后一个有效节点，而非空白节点
reference back() _GLIBCXX_NOEXCEPT
{
    iterator __tmp = end();
    --__tmp;
    return *__tmp;
}

const_reference back() const _GLIBCXX_NOEXCEPT
{
    const_iterator __tmp = end();
    --__tmp;
    return *__tmp;
}
```

#### 运算符重载

```cpp
reference operator*() const _GLIBCXX_NOEXCEPT
{ 
    return static_cast<_Node*>(_M_node)->_M_data; 
}

pointer operator->() const _GLIBCXX_NOEXCEPT
{ 
    return std::__addressof(static_cast<_Node*>(_M_node)->_M_data); 
}

_Self& operator++() _GLIBCXX_NOEXCEPT   
{ 
    _M_node = _M_node->_M_next;
    return *this;
}

_Self operator++(int) _GLIBCXX_NOEXCEPT
{
    _Self __tmp = *this;
    _M_node = _M_node->_M_next;
    return __tmp;
}

_Self& operator--() _GLIBCXX_NOEXCEPT
{
    _M_node = _M_node->_M_prev;
    return *this;
}

_Self operator--(int) _GLIBCXX_NOEXCEPT
{
    _Self __tmp = *this;
    _M_node = _M_node->_M_prev;
    return __tmp;
}

bool operator==(const _Self& __x) const _GLIBCXX_NOEXCEPT
{ 
    return _M_node == __x._M_node; 
}

bool operator!=(const _Self& __x) const _GLIBCXX_NOEXCEPT
{ 
    return _M_node != __x._M_node; 
}
```

### 3. list 的迭代器

list 的节点在空间上并不一定是连续的，list 的迭代器必须指向节点，同时具有递增、递减、取值、成员存取等操作。

由上面的分析，可以构造一个类型，这个类型包含一个指针，这个指针的类型即为 `_List_node_base`

list 迭代器的源代码如下，包含一个 `__detail::_List_node_base* _M_node `

```cpp
emplate<typename _Tp>
struct _List_iterator
{
  typedef _List_iterator<_Tp>                _Self;
  typedef _List_node<_Tp>                    _Node;

  typedef ptrdiff_t                          difference_type;
  typedef std::bidirectional_iterator_tag    iterator_category;
  typedef _Tp                                value_type;
  typedef _Tp*                               pointer;
  typedef _Tp&                               reference;

   // The only member points to the %list element.
   __detail::_List_node_base* _M_node;   

  _List_iterator() _GLIBCXX_NOEXCEPT
  : _M_node() { }

  explicit
  _List_iterator(__detail::_List_node_base* __x) _GLIBCXX_NOEXCEPT
  : _M_node(__x) { }

  _Self
  _M_const_cast() const _GLIBCXX_NOEXCEPT
  { return *this; }
};
```

### 4. list 的算法

#### `unique()`

`unique()` 函数是<font color="blue">移除数值相同的连续元素</font>。意思是将几个连续的相同元素移除到剩余一个。它通常和 `sort()` 搭配使用去除 list 中的重复元素。

```cpp
template <typename _Tp, typename _Alloc>
typename list<_Tp, _Alloc>::__remove_return_type list<_Tp, _Alloc>::unique() {
    iterator __first = begin();
    iterator __last = end();
    // 空链表，什么都不做
    if (__first == __last) return _GLIBCXX20_ONLY(0);
  
    size_type __removed __attribute__((__unused__)) = 0;
    // 从第二个元素开始，如果后一个和前一个相同，移除后一个
    iterator __next = __first;
    while (++ __next != __last) {
        if (*__first == *__next) {
            _M_erase(__next);
            _GLIBCXX20_ONLY(__removed++);
        }
        else
            __first = __next;
        __next = __first;
    }
    return _GLIBCXX20_ONLY(__removed);
}
```

#### `merge()`

`merge` 函数是将 `list __x` 合并到 `*this` 上。源码如下，通过对源码分析，可以发现底层调用 `_M_transfer`。`merge` 函数一般用在两个<font color="blue">有序</font> list 合并的场景。

过程是遍历原 list 和 `list __x`，将 `list __x` 中小于原 list 的元素，查到原 list 元素之前，直到原 list 遍历结束，再将 `list __x` 一并插入的原 list 的尾部。

```cpp
// void _M_transfer(iterator pos, iterator first, iterator last);
// pos：目标位置（插入点）
// [first, last)：要移动的节点区间（左闭右开）

template <typename _Tp, typename _Alloc>
void list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
    merge(list&& __x)
#else
    merge(list& __x)
#endif
{
    // 自合并检查
    if (this != std::__addressof(__x)) {
        _M_check_equal_allocators(__x);
            // 确保 *this 和 __x 使用相同的 allocator
            // 因为 _M_transfer 直接转移节点内存，若 allocators 不同，可能导致释放时使用错误的 deallocate

        iterator __first1 = begin();
        iterator __last1  = end();
        iterator __first2 = __x.begin();
        iterator __last2  = __x.end();
        const size_t __orig_size = __x.size();
        __try {
            // 将 __x 合并到 this
            while (__first1 != __last1 && __first2 != __last2)
                // 如果__x的元素小于原list的元素，执行插入
                if (*__first2 < *__first1)
                {
                    iterator __next = __first2;
                    // 将 __first2 插入到 __first1 之前
                    _M_transfer(__first1, __first2, ++ __next);
                    __first2 = __next;
                }
                // 否则原list额迭代器++
                else
                    ++__first1;
            // 原list遍历结束，__x的剩余一并插入道尾部
            if (__first2 != __last2) _M_transfer(__last1, __first2, __last2);

            this->_M_inc_size(__x._M_get_size());
            __x._M_set_size(0);
        }
        __catch(...) { // 保证 basic exception safety
                       // 程序状态有效（size 正确，链表结构完整）
                       // 但可能部分元素已转移（无法完全回滚）
            const size_t __dist = std::distance(__first2, __last2);
            this->_M_inc_size(__orig_size - __dist);
            __x._M_set_size(__dist);
            __throw_exception_again;
        }
    }
}
```

另外，`merge` 还有一个可以指定排序顺序的版本，大家可以自行分析一下，可以用在当元素的类型为复杂类型的时候。

```cpp
template<typename _Tp, typename _Alloc>
    template <typename _StrictWeakOrdering>
      void
      list<_Tp, _Alloc>::
#if __cplusplus >= 201103L
      merge(list&& __x, _StrictWeakOrdering __comp)
#else
      merge(list& __x, _StrictWeakOrdering __comp)
#endif
      {
         // _GLIBCXX_RESOLVE_LIB_DEFECTS
         // 300. list::merge() specification incomplete
         if (this != std::__addressof(__x))
         {
           _M_check_equal_allocators(__x);
   
           iterator __first1 = begin();
           iterator __last1 = end();
           iterator __first2 = __x.begin();
           iterator __last2 = __x.end();
           const size_t __orig_size = __x.size();
           __try
           {
             while (__first1 != __last1 && __first2 != __last2)
               if (__comp(*__first2, *__first1))
               {
                 iterator __next = __first2;
                 _M_transfer(__first1, __first2, ++__next);
                 __first2 = __next;
               }
               else
                 ++__first1;
             if (__first2 != __last2)
               _M_transfer(__last1, __first2, __last2);
   
             this->_M_inc_size(__x._M_get_size());
             __x._M_set_size(0);
          }
          __catch(...)
          {
            const size_t __dist = std::distance(__first2, __last2);
            this->_M_inc_size(__orig_size - __dist);
            __x._M_set_size(__dist);
            __throw_exception_again;
          }
        }
      }
```

#### `splice()`

`list` 提供了 `splice` 作为公开接口，用于将一个链表的部分或全部节点“剪切-粘贴”到另一个位置。其底层调用的是 `_M_transfer`。

* <font color="blue"></font>

```cpp
void
#if __cplusplus >= 201103L
splice(const_iterator __position, list&& __x) noexcept
#else
splice(iterator __position, list& __x)
#endif
{
  if (!__x.empty())
  {
    // 分配器一致性检查
    _M_check_equal_allocators(__x);
  
    // 调用 _M_transfer 函数
    this->_M_transfer(__position._M_const_cast(),
        __x.begin(), __x.end());

    this->_M_inc_size(__x._M_get_size());
    __x._M_set_size(0);
  }
}

// 当用户传入一个左值时，自动将其转换为右值引用，调用移动版本
#if __cplusplus >= 201103L
void
splice(const_iterator __position, list& __x) noexcept
{ splice(__position, std::move(__x)); }
#endif
```

* <font color="blue"></font>

```cpp
// 将另一个 list __x 中的单个元素 _i 移动到当前 list 的指定位置 __position
void splice(const_iterator __position, list& __x, const_iterator __i) noexcept
{
    // 调用右值引用版本
    // 注意 std::move(__x) 只是将 __x 转换为左值
    // 它并不会破坏 __x 本身
    splice(__position, std::move(__x), __i);
}

void splice(iterator __position, list& __x, iterator __i)
{
    // 构造区间 [__i, __j) 以复用 _M_transfer(pos, first, last)
    iterator __j = __i._M_const_cast();
    ++ __j;
    // 自移动检查（无效移动）
    if (__position == __i || __position == __j) return;

 	// 分配器一致性检查
    if (this != std::__addressof(__x)) _M_check_equal_allocators(__x);

    this->_M_transfer(__position._M_const_cast(), __i._M_const_cast(), __j);

    this->_M_inc_size(1);
    __x._M_dec_size(1);
}
```

`splice` 是 **O(1)** 操作（对单个节点或整个链表），不涉及元素拷贝或构造/析构，仅通过指针重连完成。

#### `swap()`

`swap` 用于交换两个节点所代表的子链表（常用于内部排序逻辑）。其实现基于全局 `std::swap`，但针对链表结构做了特殊处理：

```cpp
// swap 不是交换两个元素的值，而是交换两个链表节点在链表结构中的“位置”
// _M_impl._M_node: 哨兵节点，end() 节点
// 交换两个链表（由哨兵节点 __x 和 __y 表示）的所有节点，O(1) 时间完成
void _List_node_base::swap(_List_node_base& __x, _List_node_base& __y) _GLIBCXX_USE_NOEXCEPT
{
    if (__x._M_next != &__x) 
    {
        if (__y._M_next != &__y) 
        {
            // __x 和 __y 所在的 list 都不为空
                // 交换两个哨兵的 _M_next 和 _M_prev
            std::swap(__x._M_next, __y._M_next);
            std::swap(__x._M_prev, __y._M_prev);
                // 修复新首节点的 _M_prev 指向新的哨兵 __x
                // 修复新尾节点的 _M_next 指向新的哨兵 __x
            __x._M_next->_M_prev = __x._M_prev->_M_next = &__x;
                // 同理修复 __y 链表的首尾连接
            __y._M_next->_M_prev = __y._M_prev->_M_next = &__y;
        }
        else
        {
            // __x 非空，__y 为空
            __y._M_next = __x._M_next;
            __y._M_prev = __x._M_prev;
            __y._M_next->_M_prev = __y._M_prev->_M_next = &__y;
            __x._M_next = __x._M_prev = &__x;
        }
    }
    else if (__y._M_next != &__y)
    {
        // __y 非空，__x 为空
        __x._M_next = __y._M_next;
        __x._M_prev = __y._M_prev;
        __x._M_next->_M_prev = __x._M_prev->_M_next = &__x;
        __y._M_next = __y._M_prev = &__y;
    }
}
```

这个函数看似复杂，但核心思想是：**交换两个节点在链表中的“连接关系”**，从而实现子链表的交换。

#### `sort()`

由于 STL 本身的排序算法 `sort` 接受的输入迭代器是**随机访问迭代器（RandomAccessIterator）**，但是双向链表容器 `std::list` 的访问方式是**双向迭代器（BidirectionalIterator）**，因此**不能直接使用 STL 的 `std::sort`**，必须自己定义适用于自身访问模式的排序算法。

在深入 `list::sort()` 实现前，我们需要先了解 `list` 提供的一些关键公共接口，尤其是 **`splice`** 和 **`swap`**，它们是实现高效排序的基础。

如果对上述函数理解不够深入，直接看 `sort` 源码会比较吃力。我们先看一个**简化但完整的版本**：

* **<font color="blue">版本一：清晰注释版</font>**

```cpp
// 按升序进行排序，list 链表的迭代器是双向迭代器
template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::sort()
{
    // Do nothing if the list has length 0 or 1.
    if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node)
    {
        list<_Tp, _Alloc> __carry;  // 存放需要移动的元素
        list<_Tp, _Alloc> __counter[64]; // 存放已排序元素的桶，最多可以存放 2^0 + 2^1 + ... + 2^64 个元素
        int __fill = 0; // 当前使用到的最高桶的“上界”，也就是“已经用到了多少位”

        // 若不是空链表
        while (!empty())
        {
            // 第一步：把当前链表的第一个节点放到 carry 链表头
            __carry.splice(__carry.begin(), *this, begin());

            int __i = 0; // 当前遍历到的桶
            // 第二步：只要当前桶不为空，就一直合并
            while (__i < __fill && !__counter[__i].empty())
            {
                __counter[__i].merge(__carry);
                __carry.swap(__counter[__i ++ ]);
            }

            // 第三步：交换 carry 和 counter[i] 内容，将 carry 中的内容放入桶
            __carry.swap(__counter[__i]);

            // 第四步：若进位到新高位，更新 fill
            if (__i == __fill) ++__fill;
        }

        // 第六步：把低位未进位的数据依次合并到高位
        for (int __i = 1; __i < __fill; ++__i) __counter[__i].merge(__counter[__i - 1]);

        // 第七步：将最终有序链表交换回原链表
        swap(__counter[__fill - 1]);
    }
}
```

`std::list` 是一个**双向链表**，不能像数组那样随机访问元素，所以不能用快排这类依赖“下标”的算法。

为了解决这个问题，C++ 标准库采用了一种巧妙的 **“<font color="blue">自底向上的归并排序</font>”**，并且借鉴了 **<font color="blue">二进制加法中“进位”</font>** 的思想：

> 每次从原链表中取出一个元素，把它当作一个长度为 1 的有序小链表；
>
> 然后尝试把它和已有的“同长度”的有序链表合并；（因为 $1+1=2$，$2+2=4$，$4+4=8$，所以说是一种二进制合并）如果合并后长度翻倍，就继续尝试向上“进位”合并，直到无法再合并为止。
>
> 最后，把所有不同长度的有序段从低到高依次合并，得到完整有序链表。

时间复杂度 O(n log n)，空间复杂度 O(log n)（仅用固定数量的辅助链表）。

关键点：

- **不依赖随机访问**（适合双向迭代器）
- **非递归（自底向上迭代）**
- **原地操作**（只重排节点，不拷贝元素）
- **稳定排序**（相等元素相对顺序不变）

例如：对链表 `[9, 3, 7, 1, 8, 2, 6, 4, 5, 10, 11, 0, 12]` 排序

> 共 13 个元素。我们希望最终得到 `[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]`。
>
> 我们将模拟 `list::sort()` 的内部流程，使用“盒子”（counter）模型：
>
> - **盒0**：最多存长度 1（2⁰）的有序链
> - **盒1**：最多存长度 2（2¹）
> - **盒2**：最多存长度 4（2²）
> - **盒3**：最多存长度 8（2³）
> - **盒4**：最多存长度 16（2⁴）——13 个元素用不到这么高，但可能临时用到
>
>> 注意：每个盒子**要么空，要么恰好装满 $2^i$ 个元素且有序**。
>>
>
> 我们还有一个 **“搬运工”（carry）**，每次拿一个新元素，然后尝试往盒子里“进位”。
>
> ---
>
> ### 🔁 开始处理：逐个取出元素
>
> 我们按顺序从原链表取：9 → 3 → 7 → 1 → 8 → 2 → 6 → 4 → 5 → 10 → 11 → 0 → 12
>
> #### 第1个：9
>
> - carry = [9]
> - 盒0 空 → 放入盒0
>   ✅ 盒0: [9]
>
> #### 第2个：3
>
> - carry = [3]
> - 盒0 非空（[9]）→ 合并：[3] + [9] → [3,9]（升序）
> - carry = [3,9]，盒0 清空
> - 盒1 空 → 放入盒1
>   ✅ 盒1: [3,9]
>
> #### 第3个：7
>
> - carry = [7]
> - 盒0 空 → 放入盒0
>   ✅ 盒0: [7]，盒1: [3,9]
>
> #### 第4个：1
>
> - carry = [1]
> - 盒0 非空（[7]）→ 合并 → [1,7]
> - carry = [1,7]，盒0 清空
> - 盒1 非空（[3,9]）→ 合并 [1,7] + [3,9] → [1,3,7,9]
> - carry = [1,3,7,9]，盒1 清空
> - 盒2 空 → 放入盒2
>   ✅ 盒2: [1,3,7,9]
>
> #### 第5个：8
>
> - carry = [8]
> - 盒0 空 → 放入盒0
>   ✅ 盒0: [8]，盒2: [1,3,7,9]
>
> #### 第6个：2
>
> - carry = [2]
> - 盒0 非空（[8]）→ 合并 → [2,8]
> - carry = [2,8]，盒0 清空
> - 盒1 空 → 放入盒1
>   ✅ 盒1: [2,8]，盒2: [1,3,7,9]
>
> #### 第7个：6
>
> - carry = [6]
> - 盒0 空 → 放入盒0
>   ✅ 盒0: [6]，盒1: [2,8]，盒2: [1,3,7,9]
>
> #### 第8个：4
>
> - carry = [4]
> - 盒0 非空（[6]）→ 合并 → [4,6]
> - carry = [4,6]，盒0 清空
> - 盒1 非空（[2,8]）→ 合并 [4,6] + [2,8] → [2,4,6,8]
> - carry = [2,4,6,8]，盒1 清空
> - 盒2 非空（[1,3,7,9]）→ 合并 [2,4,6,8] + [1,3,7,9] → [1,2,3,4,6,7,8,9]
> - carry = [1,2,3,4,6,7,8,9]，盒2 清空
> - 盒3 空 → 放入盒3
>   ✅ 盒3: [1,2,3,4,6,7,8,9]
>
>> 💥 注意：这里连续进位了三次（0→1→2→3），就像二进制 111 + 1 = 1000！
>>
>
> 当前已处理 8 个元素，剩下：5, 10, 11, 0, 12
>
> #### 第9个：5
>
> - carry = [5]
> - 盒0 空 → 放入盒0
>   ✅ 盒0: [5]，盒3: [1..9 除5]
>
> #### 第10个：10
>
> - carry = [10]
> - 盒0 非空（[5]）→ 合并 → [5,10]
> - carry = [5,10]，盒0 清空
> - 盒1 空 → 放入盒1
>   ✅ 盒1: [5,10]，盒3: [1,2,3,4,6,7,8,9]
>
> #### 第11个：11
>
> - carry = [11]
> - 盒0 空 → 放入盒0
>   ✅ 盒0: [11]，盒1: [5,10]，盒3: [大段]
>
> #### 第12个：0
>
> - carry = [0]
> - 盒0 非空（[11]）→ 合并 → [0,11]
> - carry = [0,11]，盒0 清空
> - 盒1 非空（[5,10]）→ 合并 [0,11] + [5,10] → [0,5,10,11]
> - carry = [0,5,10,11]，盒1 清空
> - 盒2 空 → 放入盒2
>   ✅ 盒2: [0,5,10,11]，盒3: [1,2,3,4,6,7,8,9]
>
> #### 第13个：12
>
> - carry = [12]
> - 盒0 空 → 放入盒0
>   ✅ 最终各盒状态：
> - 盒0: [12]
> - 盒1: 空
> - 盒2: [0,5,10,11]
> - 盒3: [1,2,3,4,6,7,8,9]
>
> ---
>
> ### 🔚 第二阶段：从低到高合并所有盒子
>
> 现在我们有多个非空盒子：0、2、3
>
> 按照算法，执行：
>
> ```cpp
> for i = 1 to fill-1:
>     counter[i].merge(counter[i-1])
> ```
>
> 即依次合并：
>
> 1. **盒1 = 盒1 + 盒0**
>    - 盒1 原为空，盒0 = [12] → 盒1 = [12]
> 2. **盒2 = 盒2 + 盒1**
>    - 盒2 = [0,5,10,11]，盒1 = [12]
>    - 合并 → [0,5,10,11,12]
> 3. **盒3 = 盒3 + 盒2**
>    - 盒3 = [1,2,3,4,6,7,8,9]
>    - 盒2 = [0,5,10,11,12]
>    - 合并两个有序链表 →
>      ✅ 最终结果：
>      `[0,1,2,3,4,5,6,7,8,9,10,11,12]`
>
> 完美有序！
>
> ![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763817972151.4clbhv7z1q.webp)

* **<font color="blue">版本二：带异常安全的工业级实现</font>**

```cpp
template <typename _Tp, typename _Alloc>
void list<_Tp, _Alloc>::sort()
{
    // Do nothing if the list has length 0 or 1.
    if (this->_M_impl._M_node._M_next != &this->_M_impl._M_node && this->_M_impl._M_node._M_next->_M_next != &this->_M_impl._M_node)
    {
        // 中介数据存放区
        list __carry;
        list __tmp[64];
        list* __fill = __tmp;
        list* __counter;

        __try
        {
            do {
                // 利用 splice 将原 list 的第一个节点移到 carry
                __carry.splice(__carry.begin(), *this, begin());

                for (__counter = __tmp; __counter != __fill && !__counter->empty(); ++__counter)
                {
                    __counter->merge(__carry);
                    __carry.swap(*__counter);
                }
                __carry.swap(*__counter);
                if (__counter == __fill) ++__fill;
            } while (!empty());

            // 合并所有 counter 链表
            for (__counter = __tmp + 1; __counter != __fill; ++__counter) __counter->merge(*(__counter - 1));

            // 将结果换回原链表
            swap(*(__fill - 1));
        }
        __catch(...)
        {
            // 异常安全：将所有临时链表拼回原链表
            this->splice(this->end(), __carry);
            for (int __i = 0; __i < sizeof(__tmp) / sizeof(__tmp[0]); ++__i) this->splice(this->end(), __tmp[__i]);
            __throw_exception_again;
        }
    }
}
```

算法本质**：这是一种**非递归的归并排序（Iterative Merge Sort）**，利用 `64` 个“桶”模拟二进制进位，每个 `counter[i]` 最多保存 `2^(i+1)` 个已排序节点。

* **综合应用示例**

```cpp
#include <iostream>
#include <list>
using namespace std;

int main()
{
    // 创建三个 list
    list<int> list1 = {8, 5, 3, 8, 2, 2, 5};
    list<int> list2 = {2, 8, 9, 6, 4, 7, 4, 3};
    list<int> list3 = list2;
  
    // 对 list1 和 list2 进行排序
    list1.sort();
    for (auto it : list1)
        cout << it << " ";
    cout << endl;
  
    list2.sort();
    for (auto it : list2)
        cout << it << " ";
    cout << endl;
  
    // 对 list1 和 list2 使用 unique 剔除重复元素
    list1.unique();
    for (auto it : list1)
        cout << it << " ";
    cout << endl;
  
    list2.unique();
    for (auto it : list2)
        cout << it << " ";
    cout << endl;
  
    // 将排序后的 list1 合并到 list2
    list2.merge(list1);
    for (auto it : list2)
        cout << it << " ";
    cout << endl;
  
    // list3 没有排序，将排序后的 list1 合并到 list3
    list3.merge(list1);
    for (auto it : list3)
        cout << it << " ";
    cout << endl;
  
    return 0;
}
```

运行结果：

```
2 2 3 5 5 8 8
2 3 4 4 6 7 8 9
2 3 5 8
2 3 4 6 7 8 9
2 2 3 3 4 5 6 7 8 8 9
2 8 9 6 4 7 4 3
```

> 注意：`merge()` 要求两个链表都已排序，否则行为未定义（如最后一行输出所示）。

### 参考资料

[1] [STL之list源码剖析（一）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483738&idx=1&sn=6d20a7e643e43221decba1a617c03a0f&chksm=ead010fcdda799eaf959161e2ab00c339f9c0e4b607613e11af055adf4f89e3ac0a2bb35e853&scene=178&cur_album_id=1428452222471602177&search_click_id=#rd)

[2] [STL之list源码剖析（二）](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483742&idx=1&sn=292c425eb57173a425e3e53f9cb99280&chksm=ead010f8dda799ee6a35114f3b2efb5f9647383bb7be963e2602f5df407c4560ecd4098b2bda&scene=178&cur_album_id=1428452222471602177&search_click_id=&poc_token=HD9JHWmjkhAPyGz3riJsY8Fg91HDqBcZPXGBXZ0l)

[3] [STL之list排序算法图解](https://mp.weixin.qq.com/s?__biz=MzI3MDQyMDE2OQ==&mid=2247483997&idx=1&sn=120573cf1ca259fdd96acb3ef17c4b13&chksm=ead013fbdda79aed6515f8928a9b2f73217ccc5735af75565e5b01cd84b3ea17fbc8d4e5bdfc&scene=178&cur_album_id=1428452222471602177&search_click_id=#rd)

## iterators 与 traits 编程技法

不论是泛型思维或 STL 的实际应用，迭代器（iterators）都扮演着重要的角色。STL 的中心思想在于：将数据容器（container）和算法（algorithms）分开，彼此独立设计，最后再以一帖胶着剂将他们撮合在一起。这个扮演胶合角色的就是<font color="blue">迭代器</font>，每一种 STL 容器都提供专属迭代器。迭代器的设计思维是 STL 的关键所在。

由于每个容器都提供自己的专属迭代器，算法在使用时需要知道这些迭代器的特性，这时候需要在迭代器和算法中间再增加一个特性“萃取”的东西，而扮演这个角色的就是 <font color="blue">traits（“特性萃取机”）</font>。

这些都是 STL 的组件，STL 还有其他一共六大组件，之间的关系如下图所示。今天我们着重讨论迭代器和萃取机，明白这两个后 STL 的源代码的大部分东西就透彻了。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_jpg/7kDbTKZllib0gNT4nNe0pUGrhtm1kXjXH4KOdmibvEe2S0AlRxup4D0pQ3OoKUjkRMjhD4nMsqP5KHtqcElwoUoQ/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=0)

### 0. 迭代器设计思维

上面说了迭代器在容器和算法之间扮演的角色，下面就先展示一下它们的合作是什么样的，以大家比较熟悉的算法 `find()` 为例，它接收两个迭代器和一个“搜寻目标”：

```cpp
template<typename InputIterator, typename T>
InputIterator find(InputIterator first, InputIterator last, const T& value)
{
    while(first != last && *first != value)
        ++first;
    return first;
}
```

只要给予不同的迭代器，`find()` 便能够对不同容器进行查找操作：

```cpp
#include <vector>
#include <list>
using namespace std;

int main()
{
    int ia[7] = {0,1,2,3,4,5,6};
    vector<int> ivect(ia, ia+7);
    list<int> ilist(ia, ia+7);
    // 使用find
    vector<int>::iterator it1 = find(ivect.begin(), ivect.end(), 4);
    list<int>::iterator it2 = find(ilist.begin(), ilist.end(), 4);
  
    return 0;
}
```

迭代器是一种行为类似指针的对象，指向序列元素，通过迭代器可以访问序列中的某个元素、改变序列中的某个元素的值，是迭代器向前或向后移动等。

为了让大家有一个直白的感受我们看一下迭代器的定义，如下所示。可以看出迭代器中定义了五种型别，这就是迭代器的特性。

```cpp
template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
typename _Pointer = _Tp*, typename _Reference = _Tp&>
    struct _GLIBCXX17_DEPRECATED iterator
    {
        /// One of the @link iterator_tags tag types@endlink.
        typedef _Category  iterator_category;
        /// The type "pointed to" by the iterator.
        typedef _Tp        value_type;
        /// Distance between iterators is represented as this type.
        typedef _Distance  difference_type;
        /// This type represents a pointer-to-value_type.
        typedef _Pointer   pointer;
        /// This type represents a reference-to-value_type.
        typedef _Reference reference;
    };
```

1. iterator_category 指的即是迭代器的类别。
2. value_type 就是指迭代器所指对象的型别。任何打算与 STL 有完美搭配的 class，都应该定义自己的 value type 内嵌型别。
3. difference_type 用来表示两个迭代器之间的距离，对于连续空间的容器而言，两个迭代器之间的距离就是两个迭代器只差。
4. pointer 指的即是迭代器所指之物的型别。
5. reference 指的即是 `*p` 的型别。

### 1. iterator_category

根据移动特性与施行操作，迭代器被分为五类

* 单向移动只读迭代器 Input Iterator
* 单向移动只写迭代器 Output Iterator
* 单向移动读写迭代器 Forward Iterator
* 双向移动读写迭代器 Bidrectional Iterator
* 随机移动读写迭代器 Random Access Iterator

这些迭代器的分类与从属关系如下图所示，这个图的意思是一个 Random Access Iterator也是Input Iterator。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0gNT4nNe0pUGrhtm1kXjXHNrT639qRrA4qEct4FIr2j0tZ4pZP7TxibWiaNjG3U9DnpHckKZyCI40A/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=1)

有的人可能会问指定迭代器的类型有什么作用呢？我们以 `advance()` 算法为例针对不同的迭代类型写不同的版本。

```cpp
template<typename InputIterator, typename Distance>
void __advance_II(InputIterator& i, Distance n)
{
    while(n--)
        ++i;
}

template<typename RandomAccessIterator, typename Distance>
void __advance_RAI(RandomAccessIterator& i, Distance n)
{
    i += n;
}

template<typename InputIterator, typename Distance>
void advance(InputIterator& i, Distance n)
{
    if(is_random_access_iterator(i))
        advance_RAI(i, n);
    else
        advance_II(i, n);
}
```

当程序调用 `advance()` 时，会根据迭代器类型调用对应的函数，是因为不这么做的话，对于 Random Access Iterator 迭代器如果按照 advance_II 版本的算法进行的话会将时间复杂度从 O(1) 变为 O(n)，效率大大降低。

但是这样的策略方式只有在代码的执行期才能决定使用哪个版本，如何在编译器就能知道选择哪个版本呢？我们可以在策略函数中将迭代器类型作为参数传进去，按迭代器类型对 `__advance()` 函数进行重载，再在 `advance()` 函数中使用函数模板就可以实现。

幸运的是标准库已经为我们定义好了五种迭代器的类型（tag），我们在可以直接在迭代器中使用。

```cpp
///  Marking input iterators.
struct input_iterator_tag { };

///  Marking output iterators.
struct output_iterator_tag { };

/// Forward iterators support a superset of input iterator operations.
struct forward_iterator_tag : public input_iterator_tag { };

/// Bidirectional iterators support a superset of forward iterator
/// operations.
struct bidirectional_iterator_tag : public forward_iterator_tag { };

/// Random-access iterators support a superset of bidirectional
/// iterator operations.
struct random_access_iterator_tag : public bidirectional_iterator_tag { };
```

`advance()` 算法实现如下：

```cpp
template<typename InputIterator, typename Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    while(n--)
        ++i;
}

template<typename RandomAccessIterator, typename Distance>
void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

template<typename InputIterator, typename Distance>
void advance(InputIterator& i, Distance n)
{
    __advance(i, n, iterator_category);
}
```

如上述代码所示，我们在 `advance()` 函数中传入 `iterator_category` 参数时会遇到问题，不同的容器都具有不同的迭代器，我们怎么获得他们的类型呢。如果容器的迭代器都像 `list` 的迭代器一样是一个 class，那可以直接使用 `::` 来获取。但是 `vector` 的迭代器就是一个普通的指针 `T*`（`T` 为元素类型），又怎么知道它的迭代器类型呢？

这个时候就需要一个中间层来将迭代器的特性“萃取”出来，这个时候就需要用到我们之前谈到的 traits 了。如下图所示。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_png/7kDbTKZllib0gNT4nNe0pUGrhtm1kXjXH5pu5cqZZRfy6guR3JvxPKDvJYous4h11K1b3Jbf6NIueGNoEb8zMxw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=2)

### 2. traits 编程技法

从第一节中我们已经大概知道 traits 对 iterators 的意义——就是将 iterators 的五个型别“萃取”出来，在算法需要知道的时候告诉算法。traits 的机制如下图所示。

![图片](https://mmbiz.qpic.cn/sz_mmbiz_jpg/7kDbTKZllib0gNT4nNe0pUGrhtm1kXjXH392Hgz8GsMmr0IUUCI9cWXuicGLwhlEmqrVUFAtibVt594rNcThOARaA/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1#imgIndex=3)

可以看出不仅对 class-type iterators（定义了自己的型别）有能力，也对原生指针（例如 `int*`，没有定义自己的型别）也有能力。

我们先看一下标准库中对于 class-type iterators，traits 的源码是什么样的。 class-type iterators 已经定义了自己的五大型别是什么，所以直接通过 traits 去获取就可以了。

```cpp
template<typename I>
struct iterator_traits
{
    typedef typename I::iterator_category iterator_category;
    typedef typename I::value_type        value_type;
    typedef typename I::difference_type   difference_type;
    typedef typename I::pointer           pointer;
    typedef typename I::reference         reference;
};
```

原生指针 pointer 以及 pointer-to-const，不像 class-type iterators，没有定义自己的五大型别，这个时候我们可以通过偏特化来实现，当传入的类型为原生指针就会自动调用偏特化版本的 traits。

```cpp
// pointer的偏特化traits
template<typename T>
struct iterator_traits<T*>
{
    typename random_access_iterator_tag iterator_category;
    typename T                          value_type;
    typename ptrdiff_t                  difference_type;
    typename T*                         pointer;
    typename T&                         reference;
};

// pointer-to-const的偏特化traits
template<typename T>
struct iterator_traits<const T*>
{
    typename random_access_iterator_tag iterator_category;
    typename T                          value_type;
    typename ptrdiff_t                  difference_type;
    typename const T*                   pointer;
    typename const T&                   reference;
};
```

### 3. iterators + traits 实现 advance

现在有了 traits 我们就可以萃取出 iterators 的迭代器类型传入 advance 算法中了。

```cpp
template<typename InputIterator, typename Distance>
void __advance(InputIterator& i, Distance n, input_iterator_tag)
{
    while(n--)
        ++i;
}

template<typename RandomAccessIterator, typename Distance>
void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag)
{
    i += n;
}

template<typename InputIterator, typename Distance>
void advance(InputIterator& i, Distance n)
{
    __advance(i, n, iterator_traits<InputIterator>::iterator_category());
}

// advance的另一个版本
template<typename InputIterator, typename Distance>
void advance(InputIterator& i, Distance n)
{
    typename iterator_traits<InputIterator>::iterator_category category;
    __advance(i, n, category());
}
```

我每次看到 STL 的源码中的编程思想和技法都会想是否能应用到我自己的日常编码中去。要能应用上述的技法需要具备以下能力：

- 模板的参数推导
- 模板的偏特化
- 声明内嵌型别

熟悉这些后，遇到不能解决的问题时，我们只需要增加中间层，利用中间层将上层的特性萃取出来然后传递给下层就行。遇到原生类型采用模板的偏特化。

**这样增加一个“软的”中间层去适配下层功能。但是上层必须按规定提供中间层必须的型别。**

学习完 iterators 和 traits 我们就可以回过头来将各个容器再看一遍，重点关注容器的 iterators 实现，容器的底层机制决定了它的 iterators 应该是什么样的。

## 迭代器失效规则与异常安全

看完容器的数据结构后，还必须关注两个对日常使用影响很大的问题：

1. 容器修改后，原来的 iterator/reference/pointer 是否还能用。
2. 操作中途抛异常时，容器能保证什么状态。

### 0. 为什么会发生迭代器失效

我们先想一下，为什么会发生迭代器失效。迭代器本质上保存了某种“定位元素”的信息。不同容器的定位方式不同：

- `vector` 迭代器通常就是原生指针或包装指针。
- `deque` 迭代器保存当前元素指针、buffer 边界、map 节点指针。
- `list` 迭代器保存链表节点指针。
- `set/map` 迭代器保存红黑树节点指针。
- `unordered_map` 迭代器保存哈希表节点位置。

当容器操作导致这些定位信息不再有效时，迭代器就失效。典型原因如下：

- 连续内存扩容后，元素整体搬迁。
- 删除节点后，节点内存被释放。
- `deque` 的 map 重新分配。
- 哈希表 rehash 后，bucket 结构重建。

### 1. 顺序容器失效规则

| 容器 | 操作 | iterator/reference/pointer 失效规则 |
| --- | --- | --- |
| `vector` | `push_back/emplace_back` 未扩容 | `end()` 失效，其他通常不失效 |
| `vector` | 触发扩容 | 全部 iterator/reference/pointer 失效 |
| `vector` | 中间 `insert` 未扩容 | 插入点及其后的 iterator/reference/pointer 失效 |
| `vector` | `erase` | 删除点及其后的 iterator/reference/pointer 失效 |
| `deque` | 首尾插入 | iterator 通常失效，reference/pointer 通常不失效 |
| `deque` | 中间插入/删除 | 全部 iterator/reference/pointer 可能失效 |
| `list` | 插入 | 已有 iterator/reference/pointer 不失效 |
| `list` | 删除 | 只有被删元素失效 |
| `array` | 无扩容/删除 | iterator 稳定，容器生命周期结束后失效 |

这里尤其需要注意 `vector`：只要扩容，原来的元素地址全部变了。

```cpp
vector<int> v;
v.push_back(1);
int* p = &v[0];

v.push_back(2); // 可能扩容
*p = 3;         // 如果扩容，p 已悬空
```

### 2. 关联容器失效规则

`set/map/multiset/multimap` 基于节点式红黑树：

| 操作 | 失效规则 |
| --- | --- |
| 插入 | 已有 iterator/reference/pointer 不失效 |
| 删除 | 只有被删元素失效 |
| 旋转/rebalance | 不使元素地址变化 |
| `clear` | 全部失效 |

原因是红黑树插入和旋转只修改节点之间的指针关系，不会搬迁已有节点对象。

### 3. 无序关联容器失效规则

`unordered_set/unordered_map` 基于哈希表：

| 操作 | 失效规则 |
| --- | --- |
| 插入未触发 rehash | 已有 iterator 通常不失效 |
| 插入触发 rehash | 全部 iterator 失效 |
| 删除 | 只有被删元素失效 |
| `rehash/reserve` | 全部 iterator 失效 |
| `clear` | 全部失效 |

注意，rehash 会重建 bucket 数组，iterator 会失效；但节点式实现中，元素对象本身通常不移动，所以 reference/pointer 在未删除对应元素时往往仍有效。实际使用时仍应尽量避免依赖 rehash 前后的 iterator。

### 4. 边遍历边删除的正确写法

`vector`：

```cpp
for (auto it = v.begin(); it != v.end(); )
{
    if (need_remove(*it))
        it = v.erase(it);
    else
        ++it;
}
```

`list` / `map` / `set`：

```cpp
for (auto it = c.begin(); it != c.end(); )
{
    if (need_remove(*it))
        it = c.erase(it);
    else
        ++it;
}
```

C++11 起，标准容器的 `erase(iterator)` 通常返回下一个有效迭代器。旧代码中也常见：

```cpp
c.erase(it++);
```

但这种写法更容易出错，优先使用返回值形式。

### 5. 异常安全等级

STL 容器和算法常见异常安全分为三类：

| 等级 | 含义 |
| --- | --- |
| 基本保证 | 抛异常后对象仍处于有效状态，但内容可能改变 |
| 强保证 | 抛异常后对象回到操作前状态 |
| 不抛保证 | 操作承诺不抛异常 |

例如：

- `vector::push_back` 如果无需扩容，构造新元素失败时通常能保持原状态。
- `vector::push_back` 扩容时，如果元素 move 可能抛且不可 copy，强保证可能无法维持。
- `list::splice` 只改指针，通常不抛。
- `swap` 是否不抛，受 allocator propagation 和元素类型影响。

### 6. vector 扩容的异常回滚

`vector` 扩容时要先分配新内存，再构造/搬移元素。如果中途抛异常，必须清理新内存并保持旧 vector 可用。

```cpp
pointer new_start = allocate(new_cap);
pointer new_finish = new_start;

try
{
    new_finish = uninitialized_move_or_copy(begin, pos, new_start);
    construct(new_finish, value);
    ++new_finish;
    new_finish = uninitialized_move_or_copy(pos, end, new_finish);
}
catch (...)
{
    destroy(new_start, new_finish);
    deallocate(new_start, new_cap);
    throw;
}

destroy(old_begin, old_end);
deallocate(old_begin, old_cap);
commit_new_buffer();
```

关键是最后一步 `commit_new_buffer()` 必须在所有可能抛异常的构造动作完成后再执行。否则一旦提前修改 `_M_start/_M_finish`，回滚会非常困难。

### 7. noexcept move 的影响

为了维持强异常保证，容器扩容时会谨慎选择 move 或 copy：

```cpp
if constexpr (is_nothrow_move_constructible_v<T> ||
              !is_copy_constructible_v<T>)
    move old elements;
else
    copy old elements;
```

如果移动构造可能抛异常，并且已经把旧元素移动走了，那么新内存构造失败时，旧容器也未必能恢复原值。因此当 copy 可用且更安全时，标准库可能选择 copy。

这也是自定义类型最好把移动构造/移动赋值声明为 `noexcept` 的原因之一：

```cpp
class Buffer
{
public:
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
};
```

这样 `vector<Buffer>` 扩容时更可能走 move，提高性能。

### 8. 容器失效规则速查

| 容器 | 插入是否导致已有 iterator 失效 | 删除是否导致其他 iterator 失效 | 备注 |
| --- | --- | --- | --- |
| `vector` | 可能，扩容则全部失效 | 删除点及之后失效 | reference/pointer 同理 |
| `deque` | 规则复杂，首尾和中间不同 | 规则复杂 | 不要长期保存 deque iterator |
| `list` | 不失效 | 只影响被删元素 | 节点式容器 |
| `set/map` | 不失效 | 只影响被删元素 | 红黑树节点式容器 |
| `unordered_set/map` | rehash 则 iterator 全失效 | 只影响被删元素 | reserve 可减少 rehash |
| `string` | 类似 vector | 类似 vector | SSO 与实现有关 |

## `std::pair`

```cpp
#ifndef __SGI_STL_INTERNAL_PAIR_H
#define __SGI_STL_INTERNAL_PAIR_H

__STL_BEGIN_NAMESPACE

// pair是一个struct，拥有两个成员变量first和second
template <class _T1, class _T2>
struct pair
{
  typedef _T1 first_type;
  typedef _T2 second_type;

  _T1 first;
  _T2 second;
// pair的默认构造函数和显式构造函数
  pair() : first(_T1()), second(_T2()) {}
// 显式构造函数使用的是引用传递，这个很重要
// 可以说明在构造pair对象的时候只会进行一次T1或T2拷贝构造函数的调用
  pair(const _T1& __a, const _T2& __b) : first(__a), second(__b) {}

#ifdef __STL_MEMBER_TEMPLATES
// 拷贝构造函数，pair不能使用赋值，因为没有重载赋值运算符
  template <class _U1, class _U2>
  pair(const pair<_U1, _U2>& __p) : first(__p.first), second(__p.second) {}
#endif
};

// pair提供的内联函数，==，<，!=，>
// 只有 first 和 second 都对应相等时才相等
template <class _T1, class _T2>
inline bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{ 
  return __x.first == __y.first && __x.second == __y.second; 
}
// 优先比较first的大小
template <class _T1, class _T2>
inline bool operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{ 
  return __x.first < __y.first || (!(__y.first < __x.first) && __x.second < __y.second); 
}

#ifdef __STL_FUNCTION_TMPL_PARTIAL_ORDER
template <class _T1, class _T2>
inline bool operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
  return !(__x == __y);
}

template <class _T1, class _T2>
inline bool operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
  return __y < __x;
}

template <class _T1, class _T2>
inline bool operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
  return !(__y < __x);
}

template <class _T1, class _T2>
inline bool operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
{
  return !(__x < __y);
}
#endif /* __STL_FUNCTION_TMPL_PARTIAL_ORDER */

// 内联函数make_pair，调用pair的显式构造函数，构造一个pair对象
template <class _T1, class _T2>
inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2& __y)
{
  return pair<_T1, _T2>(__x, __y);
}

__STL_END_NAMESPACE
#endif /* __SGI_STL_INTERNAL_PAIR_H */
```

注意，上面版本是 C++11 之前的实现。

## `set`

### 0. set 的定义

set 的源码是非常简单的，包含在 `stl_set.h` 文件中，我们把 set 的源码部分分解来看，并总结出它的特性。

```cpp
// 模板包含三个参数
/**
 * @tparam _Key  Type of key objects.
 * @tparam _Compare  Comparison function object type, defaults to less<_Key>
 * @tparam _Alloc  type, defaults to allocator<_Key>
*/
template<typename _Key, typename _Compare = std::less<_Key>,
  typename _Alloc = std::allocator<_Key> >
class set
{
  typedef typename _Alloc::value_type    _Alloc_value_type;
  
public:
  // key和value是同一个
  typedef _Key     key_type;
  typedef _Key     value_type;
  typedef _Compare key_compare;
  typedef _Compare value_compare;
  typedef _Alloc   allocator_type;

private:
  // 底层的结构是红黑树  
  typedef _Rb_tree<key_type, value_type, _Identity<value_type>,
           key_compare, _Key_alloc_type> _Rep_type;
  _Rep_type _M_t;  // Red-black tree representing set.
};
```

从上述代码中我们可以看出：

- set 的底层结构是红黑树 `_Rep_type _M_t`；
- 因为是红黑树，所以元素自动排序，排序函数为 `_Compare`，默认按照标准函数 `std::less<_Key>` 进行排序的，`std::less<_Key>` 的源码如下：

```cpp
template<typename _Tp>
struct less : public binary_function<_Tp, _Tp, bool>
{
  bool
  operator()(const _Tp& __x, const _Tp& __y) const
  { return __x < __y; }
};
```

当然也可以传入自定义的排序函数。但是，如果要使用默认的函数，对于自定义类型就必须重载“<”。

<font color="blue">set 中的元素实际上也是一个 key-value 对，只是 key 和 value 是一样的。</font>

### 1. set 的迭代器

```cpp
// set的迭代器都是const“型”的

// _GLIBCXX_RESOLVE_LIB_DEFECTS
// DR 103. set::iterator is required to be modifiable,
// but this allows modification of keys.
typedef typename _Rep_type::const_iterator   iterator;
typedef typename _Rep_type::const_iterator   const_iterator;
typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
```

从上述代码可以看出，<font color="blue">set 的迭代器都是 const 的</font>，因此无法通过迭代器修改 set 元素。

### 2. set 的构造函数

```cpp
set() : _M_t() { }

set(const _Compare& __comp,
    const allocator_type& __a = allocator_type())
: _M_t(__comp, _Key_alloc_type(__a)) { }

template<typename _InputIterator>
set(_InputIterator __first, _InputIterator __last)
: _M_t()
{ _M_t._M_insert_unique(__first, __last); }

set(const set& __x)
: _M_t(__x._M_t) { }

set(initializer_list<value_type> __l,
    const _Compare& __comp = _Compare(),
    const allocator_type& __a = allocator_type())
: _M_t(__comp, _Key_alloc_type(__a))
{ _M_t._M_insert_unique(__l.begin(), __l.end()); }
```

可以使用一个 `[__first, __last)` 区间来构造，也可以利用列表初始化的形式来构造 `initializer_list`。

另外，我们可以注意到底层都调用了红黑树的 `_M_insert_unique` 函数，源码如下，可以发现底层又调用了 `_M_get_insert_unique_pos` 函数，该函数返回的是一个 pair 类型，如果 pair 的 second 为 0 则插入失败。

```cpp
template <typename _Key, typename _Val, typename _KeyOfValue, typename _Compare, typename _Alloc>
#if __cplusplus >= 201103L
template <typename _Arg>
#endif
pair<typename _Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::iterator, bool>
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::_M_insert_unique(_Arg&& __v)
{
    typedef pair<iterator, bool> _Res;
    pair<_Base_ptr, _Base_ptr> __res = _M_get_insert_unique_pos(_KeyOfValue()(__v));

    if (__res.second) return (_Res(_M_insert_(__res.first, __res.second, _GLIBCXX_FORWARD(_Arg, __v)), true));
    return (_Res(iterator(static_cast<_Link_type>(__res.first)), false));
}
```

> ### 为什么需要 `_KeyOfValue`？
>
> 在 STL 的关联容器中：
>
> - `std::set<T>` 存储的是 **键本身**（即 `_Key == _Val == T`）；
> - `std::map<K, V>` 存储的是 **键值对**（即 `_Val = std::pair<const K, V>`），但排序只依据 **键 `K`**。
>
> 因此，红黑树底层需要一种统一的方式：**无论容器是 set 还是 map，都能从 `_Val` 中拿到用于比较的 `_Key`**。
>
> ### 返回值的含义
>
> | 成员       | 类型         | 含义                                                                                                            |
> | ---------- | ------------ | --------------------------------------------------------------------------------------------------------------- |
> | `first`  | `iterator` | 指向**红黑树中键等于待插入值的元素**的迭代器。                                                            |
> | `second` | `bool`     | 表示**是否真正执行了插入操作**： - `true`：成功插入（键原先不存在）； - `false`：未插入（键已存在）。 |

而 `_M_get_insert_unique_pos` 函数的源码如下，我们看看在什么时候 second 为 0：

```cpp
template<typename _Key, typename _Val, typename _KeyOfValue,
         typename _Compare, typename _Alloc>
pair<typename _Rb_tree<_Key, _Val, _KeyOfValue,
       _Compare, _Alloc>::_Base_ptr,
     typename _Rb_tree<_Key, _Val, _KeyOfValue,
       _Compare, _Alloc>::_Base_ptr>
_Rb_tree<_Key, _Val, _KeyOfValue, _Compare, _Alloc>::
_M_get_insert_unique_pos(const key_type& __k)
{
    // typedef pair
    typedef pair<_Base_ptr, _Base_ptr> _Res;
    // _x表示当前节点,_y表示_x的父节点
    _Link_type __x = _M_begin();
    _Link_type __y = _M_end();
    bool __comp = true;
  
    // 寻找插入点
    while (__x != 0)
    {
        __y = __x;
        // __k<__x是否为true
        __comp = _M_impl._M_key_compare(__k, _S_key(__x));
        // __k<__x就往左孩子查找，否则右孩子查找
        __x = __comp ? _S_left(__x) : _S_right(__x);
    }
    iterator __j = iterator(__y);
    // __k<__y，往__y的左孩子插入节点即可，不是做插入，是找到位置即可。
    if (__comp)
    {
        // 特殊位置
        if (__j == begin())
            return _Res(__x, __y);
        else
            --__j;  // 左孩子 这里调用了--操作符
    }
    // 否则直接比较__j和__k的大小，此时的__j就是y
    // __j<__k，返回当前节(__x=0)点与父节点
    if (_M_impl._M_key_compare(_S_key(__j._M_node), __k))
        return _Res(__x, __y);
    // _j>=__k,插入失败
    return _Res(__j._M_node, 0);
}
```

> ### 返回值含义
>
> | 成员       | 含义                                                                                                                                                                                                                                                |
> | ---------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
> | `first`  | 指向**父节点（parent）**，即新节点应当作为其子节点插入； 如果树为空，则为 `_M_end()`（即 header 节点）。                                                                                                                                    |
> | `second` | 表示**插入方向和是否存在冲突**： - **非空（!= nullptr）**：表示可以插入，通常指向“应插入位置”的占位（实际实现中常直接用作标志）； - **为空（== nullptr）**：表示**键已存在**，此时 `first` 指向**已存在的节点**。 |

通过分析上述源代码，可以发现满足当 `k < y` 为 false 同时 `y < k` 为 false 时，即 `k == y`，second 为 0。

所以，经过这么长的分析，我们可以得出这样的结论：set 中的元素都是唯一的。

### 3. set 常用方法总结

如果读懂了上面的源码分析，学习起 set 的方法来就简单多了。

`begin()` 方法返回 set 的首元素，调用的是红黑树的 `begin()` 方法，实际上这个红黑树的 `begin()` 方法返回的并不是整棵树的根节点，而是整个树的最左节点。因为 set 的元素是按顺序排列的，最左节点也是最小节点。

```cpp
iterator
begin() const _GLIBCXX_NOEXCEPT
{ return _M_t.begin(); }
```

同理 `end()` 方法则返回的是最右节点，最小的值。

```cpp
iterator
end() const _GLIBCXX_NOEXCEPT
{ return _M_t.end(); }
```

`empty()` 方法判断是否为空：

```cpp
bool
empty() const _GLIBCXX_NOEXCEPT
{ return _M_t.empty(); }
```

`insert` 方法有很多种，但是底层都调用的是红黑树的 `_M_insert_unique` 方法：

```cpp
std::pair<iterator, bool>
insert(const value_type& __x)
{
  std::pair<typename _Rep_type::iterator, bool> __p =
  _M_t._M_insert_unique(__x);
  return std::pair<iterator, bool>(__p.first, __p.second);
}
```

其它的不做赘述了：

```cpp
size_type
erase(const key_type& __x)
{ return _M_t.erase(__x); }

void
clear() _GLIBCXX_NOEXCEPT
{ _M_t.clear(); }

size_type
count(const key_type& __x) const
{ return _M_t.find(__x) == _M_t.end() ? 0 : 1; }

iterator
find(const key_type& __x)
{ return _M_t.find(__x); 
```

## `std::unique_ptr`

`std::unique_ptr` 是小巧、高速的、具备只移型别的智能指针，对托管资源实施专属所有权语义。默认的，资源析构采用 delete 运算符来实现，但可以指定自定义删除器。<font color="blue">有状态的删除器和采用函数指针实现的删除器会增加 std::unique_ptr 型别的对象尺寸。</font>

`unique_ptr` 的重要使用场景是 <font color="blue">工厂函数</font>，以及用作实现 <font color="blue">Pimpl Idiom</font> 的机制。

`unique_ptr` 的源代码分析如下：

```cpp
// _Tp 为管理对象的类型，_Dp 为析构器的类型
// default_delete 是默认析构器，默认析构器中使用 delete 运算符实现对象的析构
template <typename _Tp, typename _Dp = default_delete<_Tp>>
class unique_ptr
{
    // 使用 __uniq_ptr_impl 管理要管理的 heap 对象
    __uniq_ptr_impl<_Tp, _Dp> _M_t;
public:
    using pointer       = typename __uniq_ptr_impl<_Tp, _Dp>::pointer;
    using element_type  = _Tp;
    using deleter_type  = _Dp;
};
```

在看 `unique_ptr` 的构造函数之前，我们先看一下 `__uniq_ptr_impl` 的源代码：

```cpp
// _Tp 为管理对象的类型，_Dp 为析构器的类型
template <typename _Tp, typename _Dp>
class __uniq_ptr_impl
{
    template <typename _Up, typename _Ep, typename = void>
    struct _Ptr
    {
        using type = _Up*;
    };
    // pointer实际上就是 _Tp*
    using pointer = typename _Ptr<_Tp, _Dp>::type;
private:
    // 使用 tuple 管理指针和析构器，通过 get<0> 获取 _Tp*，get<1> 获取析构器
    tuple<pointer, _Dp> _M_t;
public:
    __uniq_ptr_impl() = default;
    // 先通过 _M_t() 获取指针，再赋值
    __uniq_ptr_impl(pointer __p) : _M_t() { _M_ptr() = __p; }
    // 自定义析构器
    template<typename _Del>
    __uniq_ptr_impl(pointer __p, _Del&& __d) : _M_t(__p, std::forward<_Del>(__d)) { }
    // 获取所管理的对象
    pointer&   _M_ptr() { return std::get<0>(_M_t); }
    // 获取析构器
    _Dp&       _M_deleter() { return std::get<1>(_M_t); }
};
```

* 可以发现，在 `unique_ptr` 内部是通过一个 tuple 管理指针和删除器的。

接着，我们来分析 `unique_ptr` 的构造函数和析构函数：

```cpp
// 默认构造函数，显式创建一个空的 unique_ptr 对象
template<typename _Up = _Dp, typename = _DeleterConstraint<_Up>>
constexpr unique_ptr() noexcept : _M_t() {}

// 使用指针 __p 构造一个 unique_ptr 对象
template<typename _Up = _Dp, typename = _DeleterConstraint<_Up>>
explicit unique_ptr(pointer __p) noexcept : _M_t(__p) {}

// 使用指针 __p 和自定义析构器 __d 构造一个 unique_ptr 对象
unique_ptr(pointer __p,
    typename conditional<is_reference<deleter_type>::value,
      deleter_type, const deleter_type&>::type __d) noexcept
: _M_t(__p, __d) { }

// 不允许复制，体现专属所有权语义
unique_ptr(const unique_ptr&) = delete;
unique_ptr& operator=(const unique_ptr&) = delete;

// 允许移动，体现专属所有权语义和只移型别
unique_ptr(unique_ptr&& __u) noexcept 
    : _M_t(__u.release(), std::forward<deleter_type>(__u.get_deleter())) { }
unique_ptr& operator=(unique_ptr&& __u) noexcept
{
    // __u.release() 释放并返回源 unique_ptr 对象管理的资源
    // reset是将__u.release()返回的资源赋给目标（当前）unique_ptr对象
    reset(__u.release());
    get_deleter() = std::forward<deleter_type>(__u.get_deleter());
    return *this;
}

// 这个也是移动拷贝构造函数
// 只是使用的类型是可以隐式转换的其他unique_ptr对象
template<typename _Up, typename _Ep, typename = _Require<
         __safe_conversion_up<_Up, _Ep>,
   typename conditional<is_reference<_Dp>::value,
      is_same<_Ep, _Dp>,
      is_convertible<_Ep, _Dp>>::type>>
unique_ptr(unique_ptr<_Up, _Ep>&& __u) noexcept
: _M_t(__u.release(), std::forward<_Ep>(__u.get_deleter()))
{ }


// 析构函数，调用析构器析构掉管理的资源，并将 __ptr 指向 nullptr
~unique_ptr()
{
    auto& __ptr = _M_t._M_ptr();
    if (__ptr != nullptr)
        get_deleter()(__ptr);
    __ptr = pointer();
}
```

* `get_deleter()(__ptr);` 返回的是析构器，默认的析构器为 `struct default_delete<_Tp>`

  `struct default_delete<_Tp>` 有一个 `operator()` 操作符

  ```cpp
  void operator()(_Tp* __ptr) const {
      delete __ptr;
  }
  ```

  所以`get_deleter()(__ptr);`实际上就是 `delete __ptr`;

`unique_ptr` 的重要成员方法解读：

```cpp
// 可以像raw pointer一样，解引用
typename add_lvalue_reference<element_type>::type operator*() const
{
    __glibcxx_assert(get() != pointer());
    return *get();
}

// 像raw pointer一样获取保存的指针，调用get方法
pointer operator->() const noexcept
{
    _GLIBCXX_DEBUG_PEDASSERT(get() != pointer());
    return get();
}

pointer get() const noexcept
{ return _M_t._M_ptr(); }

// 释放对所管理资源的所有权
pointer release() noexcept
{
    pointer __p = get();
    _M_t._M_ptr() = pointer();
    return __p;
}

// 重置所管理的资源
void reset(pointer __p = pointer()) noexcept
{
    using std::swap;
    swap(_M_t._M_ptr(), __p);
    if (__p != pointer()) get_deleter()(__p);
}
```

接下来我们看一下 `unique_ptr` 的使用，我们看一下怎么为一个 `unique_ptr` 对象指定析构器：

```cpp
struct Investment {}; // 待管理对象
// 默认的析构器struct default_delete<_Tp>实际上是一个仿函数
// 我们就可以定义一个 lambda 作为析构器
auto delInvmt = [](Investment* pInvestment)
                {
                    makeLogEntry(pInvestment); // 做一些删除前的工作
                    delete pInvestment;
                };
// 使用 decltype 推导出自定义析构器的类型
unique_ptr<Investment, decltype(delInvmt)> pInv((new Investment), delInvmt);
```

值得注意的是，如果要自定义析构器，就必须使用构造函数，而无法通过 C++14 提供的 `make_unique` 函数。我们看一下上面的运行结果：

```cpp
struct Investment
{
  ~Investment()
  {
    cout << "called ~Investment()..." << endl;
  }
};

int main()
{
  auto ivmt = [](Investment* pInvestment)
        {
          cout << "user-defined delete..." << endl;
          delete pInvestment;
        };
  // 包在括号中，方便观察结果
  {
    unique_ptr<Investment, decltype(ivmt)> pInvestment((new Investment), ivmt);
  }

  return 0;
}

// 运行结果
user-defined delete...
called ~Investment()...
```

另外 `unique_ptr` 不允许以赋值语法将一个 raw pointer 当作初值：

```cpp
unique_ptr<int> pInt1 = new int(2); // No viable conversion from 'int *' to 'unique_ptr<int>'
unique_ptr<int> pInt2(new int(2)); 
```

不能使用普通的拷贝或者赋值：

```cpp
unique_ptr<int> pInt1(new int(2)); // pInt1现在为左值
unique_ptr<int> pInt2(pInt1);      // error

// 需要使用移动语义，使用move将左值转为右值
unique_ptr<int> pInt2(std::move(pInt1));

unique_ptr<int> pInt2;
pInt2 = pInt1; // error
pInt2 = std::move(pInt1);
```

## `std::shared_ptr`

### 0. `shared_ptr` 类图与内部结构

在看 `shared_ptr` 的源代码之前，先看一下 `shared_ptr` 的类图：

![shared_ptr 类图](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763893302048.wizr0tc5k.webp)

从类图可以看出：`shared_ptr` 内部主要包含两部分

1. **被管理对象的裸指针** `_Tp*`
2. 一个引用计数相关的对象 `__shared_count`

而 `__shared_count` 内部又维护一个指向“管理对象（control block/manager object）”的指针。该管理对象保存：

- 原子化的 `use_count`（强引用计数）
- 原子化的 `weak_count`（弱引用计数）
- 指向被管理对象 `T` 的指针
- deleter（用于销毁被管理对象）

> 约定术语：
>
> - 用 `new` 创建并交给智能指针托管的对象称为**被管理对象**（managed object）。
> - 智能指针内部用于维护生命周期和计数的控制块称为**管理对象/控制块**（manager object / control block）。

![img](https://img2020.cnblogs.com/blog/1449936/202112/1449936-20211201093930856-124882834.png)

`weak_ptr` 的内部结构与 `shared_ptr` 很像：也有 `T*` 指针 + 一个计数对象 `__weak_count`。

![weak_ptr](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763893316722.32iecskzws.webp)

对比可知：

- `shared_ptr` 与 `weak_ptr` 的差异主要体现在 `__shared_ptr` vs `__weak_ptr`
- 而这二者的核心差异是 `__shared_count` vs `__weak_count`

即：一个影响 `use_count`，另一个仅影响 `weak_count`。

### 1. `shared_ptr` 的构造：控制块是如何创建的

构造 `shared_ptr` 时，需要拿到一个 `new` 出来的对象指针；`shared_ptr` 会在内部构造 `_shared_count`，而 `_shared_count` 会进一步**动态分配一个控制块** `_Sp_counted_base_impl`。

```cpp
template<typename _Tp1> explicit __shared_ptr(_Tp1* __p)
: _M_ptr(__p), _M_refcount(__p) {
    ...
    __enable_shared_from_this_helper(_M_refcount, __p, __p);
}

template<typename _Ptr>
__shared_count(_Ptr __p) : _M_pi(0)
{
    __try
    {
        ...
        _M_pi = new _Sp_counted_base_impl<...>(__p, _Sp_deleter<_Tp>());
    }
    __catch(...)
    {
        delete __p;
        __throw_exception_again;
    }
}
```

解释：

- `__shared_ptr(_Tp1* __p)` 是 `shared_ptr` 的底层实现构造函数。
  - `:_M_ptr(__p)` 把**被管理对象的裸指针**存进 `shared_ptr` 自己的成员 `_M_ptr`，用于之后 `->`/`*` 访问。
  - `,_M_refcount(__p)` 会调用 `__shared_count` 的构造函数，从而**创建控制块（管理对象）**。
- `__shared_count(_Ptr __p)` 的作用就是“为这个裸指针配一个控制块”。
  - `_M_pi` 是指向控制块基类的指针（control block pointer），初始化为 `0`。
  - `_M_pi = new _Sp_counted_base_impl(...)`：这里动态分配了一个控制块对象。
    控制块里会保存：
    1. `use_count / weak_count`
    2. 被管理对象指针 `__p`
    3. deleter（这里是默认 deleter）
  - `__catch` 分支：如果控制块分配失败，会先 `delete __p` 释放裸指针，避免泄漏，然后把异常继续抛出。

### 2. 多个 `shared_ptr` 如何共享同一个控制块

当 `sp2` 由 `sp1` 拷贝构造或赋值得到时：

- `sp1` 与 `sp2` 会**共享同一个控制块**
- 控制块的 `use_count` 会增加

对应源码：

```cpp
template<typename _Tp1>
__shared_ptr(const __shared_ptr<_Tp1, _Lp>& __r)
: _M_ptr(__r._M_ptr), _M_refcount(__r._M_refcount) // never throws
{...}

__shared_count&
operator=(const __shared_count& __r) // nothrow
{
    _Sp_counted_base<_Lp>* __tmp = __r._M_pi;
    if (__tmp != _M_pi)
    {
        if (__tmp != 0)
            __tmp->_M_add_ref_copy();
        if (_M_pi != 0)
            _M_pi->_M_release();

        _M_pi = __tmp;
    }
}
```

解释：

- `__shared_ptr` 的拷贝构造：
  - `_M_ptr(__r._M_ptr)`：新指针拿到**同样的对象地址**。
  - `_M_refcount(__r._M_refcount)`：新指针拿到**同一个控制块**。
  - 所以拷贝后两个 `shared_ptr` 指向同一对象、共享同一控制块。
- `__shared_count::operator=` 是控制块指针的赋值逻辑：
  - `__tmp = __r._M_pi`：对方控制块指针。
  - `if (__tmp != _M_pi)`：避免自赋值。
  - 若对方控制块非空：`__tmp->_M_add_ref_copy()`
    → **use_count +1**（因为多了一个 shared_ptr）
  - 若当前控制块非空：`_M_pi->_M_release()`
    → **释放当前管理的对象/控制块（可能会触发销毁）**
  - 最后 `_M_pi = __tmp` 真正指向新的控制块。

所以：当多个 `shared_ptr` 指向同一对象时，它们的 `_M_pi` 都指向同一个控制块。

接下来我们看一下引用计数的增加和减少：

```cpp
void _M_add_ref_copy()
{ __gnu_cxx::__atomic_add_dispatch(&_M_use_count, 1); }

void _M_release() // nothrow
{
    if (__gnu_cxx::__exchange_and_add_dispatch(&_M_use_count, -1) == 1)
    {
        _M_dispose(); // 销毁被管理对象
        if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
        {
            _M_destroy(); // 销毁控制块
        }
    }
}
```

其逻辑可以更直白地理解为：

- `use_count` 从 1 减到 0：
  → **对象没人再用**，销毁对象 `_M_dispose()`
- 且此时 `weak_count` 也从 1 减到 0：
  → **连弱引用都没了**，销毁控制块 `_M_destroy()`

### 3. 为什么有两份 `_M_ptr`？是不是冗余？

你会注意到：

- `shared_ptr` 内部有 `_M_ptr`
- 控制块 `_Sp_counted_base_impl` 内部也有 `_M_ptr`

看起来像重复，但它们职责不同：

1. `shared_ptr` 自己保存的 `_M_ptr`
   - 用于实现 `->`、`*` 等“像普通指针一样”的访问
2. 控制块里保存的 `_M_ptr`
   - 用于在计数归零时正确销毁对象
   - 即“生命周期管理的依据”

### 4. `weak_ptr`

`weak_ptr` 的拷贝/赋值/析构，只影响 `weak_count`，**不影响 `use_count`**：

```cpp
void _M_weak_add_ref()
{ __gnu_cxx::__atomic_add_dispatch(&_M_weak_count, 1); }

void _M_weak_release()
{
    if (__gnu_cxx::__exchange_and_add_dispatch(&_M_weak_count, -1) == 1)
        _M_destroy(); // weak_count 为 0 时销毁控制块
}
```

因此：

- `weak_ptr` **不延长对象寿命**
- `weak_ptr` **不能直接 `*` / `->` 访问对象**
- 但它共享同一个控制块
  → 能判断对象是否还活着，并安全获取 `shared_ptr`

`weak_ptr` 的关键接口：

- `expired()`：对象是否已被释放
- `use_count()`：当前有多少个强引用
- `lock()`：若对象仍存在，则返回对应的 `shared_ptr`

```cpp
long use_count() const
{ return _M_refcount._M_get_use_count(); }

bool expired() const
{ return _M_refcount._M_get_use_count() == 0; }

__shared_ptr<_Tp, _Lp> lock() const
{
    if (expired())
        return __shared_ptr<element_type, _Lp>();
    try {
        return __shared_ptr<element_type, _Lp>(*this);
    } catch(...) {
        return __shared_ptr<element_type, _Lp>();
    }
}
```

### 5. `shared_ptr` 的大坑：重复控制块导致重复释放

`shared_ptr` 管理对象的前提是：**所有 `shared_ptr` 必须共享同一个控制块**。

如果你用同一个裸指针构造出多个独立的 `shared_ptr`：

- 每个 `shared_ptr` 都会创建自己的控制块
- 每个控制块都会在 `use_count` 归零时释放对象
- 于是对象被释放多次 → **twice delete**

#### 错误的 `shared_ptr` 版本

```cpp
#include <iostream>
#include <memory>

using namespace std;

class Thing {
public:
    void foo() {
        shared_ptr<Thing> sp_for_this(this); // 控制块 B（危险）
	    // do something...
    }
};

int main()
{
    shared_ptr<Thing> t1(new Thing); // 控制块 A
    t1->foo();
}
```

`t1` 与 `sp_for_this` 用了**不同控制块管理同一对象**，最终触发重复释放。

#### 解决方案：`enable_shared_from_this`

问题本质是：

> **对象内部想拿到一个“指向自己”的 `shared_ptr`，但又不能新建控制块。**

C++ 提供 `enable_shared_from_this`：

- 让对象内部持有一个 `weak_ptr`（`_M_weak_this`）
- 第一次为对象创建 `shared_ptr` 时，自动把控制块绑定给 `_M_weak_this`
- 之后对象内部通过 `shared_from_this()` 获取“共享同一控制块”的 `shared_ptr`

关键代码：

```cpp
template<typename _Tp>
class enable_shared_from_this
{
public:
    shared_ptr<_Tp>
    shared_from_this()
    { return shared_ptr<_Tp>(this->_M_weak_this); }

private:
    mutable weak_ptr<_Tp>  _M_weak_this;
};
```

正确写法

```cpp
#include <iostream>
#include <memory>

using namespace std;

class Thing : public enable_shared_from_this<Thing> {
public:
    void foo() {
        shared_ptr<Thing> sp_this = shared_from_this(); // 控制块 B（危险）
	    // do something...
    }
};

int main()
{
    shared_ptr<Thing> t1(new Thing); // 控制块 A
    t1->foo();
}
```

这样得到的 `sp_this` 与外部 `t1` **共享同一控制块**，不会重复释放。

### 6. `shared_ptr` 的性能问题

```cpp
shared_ptr<M> p(new M);
```

创建时发生两次动态分配：

1. 分配被管理对象 `M`
2. 分配控制块

而分配操作通常较贵。

#### 优化：`make_shared`

```cpp
shared_ptr<M> p(make_shared<M>());
```

实现：

- 一次分配一大块连续内存
- 同时存放对象 + 控制块
- 降低分配次数，也避免裸 `new`（符合 “no naked new!”）

但它的代价是：

- 对象析构后，若还有 `weak_ptr` 指向控制块
- 控制块所占内存不会释放
- 由于两者在同一内存块中
  → 整块内存要等所有 `weak_ptr` 释放后才能回收

### 参考资料

[1] [C++ 智能指针（shared_ptr/weak_ptr）原理分析](https://www.cnblogs.com/cnhk19/p/15628579.html)

## `std::weak_ptr`

### 0. 功能和作用

今天，我们从源码实现的角度来进行 `weak_ptr` 的解析，首先我们先看一下 *cppreference* 中的解析，先了解一下该智能指针的作用。

> Defined in header `<memory>`
>
> ```cpp
> template<class T>
> class weak_ptr; // (since C++11)
> ```
>
> `std::weak_ptr` is a smart pointer that holds a ***non-owning ("weak") reference*** to an object that is managed by [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr.html). It must be converted to [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr.html) in order to access the referenced object.
>
> `std::weak_ptr` models temporary ownership: when an object needs to be accessed only if it exists, and it may be deleted at any time by someone else, `std::weak_ptr` is used to track the object, and it is converted to [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr.html) to acquire temporary ownership. If the original [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr.html) is destroyed at this time, the object's lifetime is extended until the temporary [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr.html) is destroyed as well.
>
> Another use for `std::weak_ptr` is to break ***reference cycle***s formed by objects managed by [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr.html). If such cycle is orphaned (i.e., there are no outside shared pointers into the cycle), the `shared_ptr` reference counts cannot reach zero and the memory is leaked. To prevent this, one of the pointers in the cycle [can be made weak](https://en.cppreference.com/w/cpp/memory/weak_ptr/~weak_ptr.html#Example).

对应的释义如下：

1. `weak_ptr` 是一种以 ***non-owning reference*** 的方式来持有一个被 `std::shared_ptr` 指针所管理的对象的一种智能指针
2. `weak_ptr` 构建临时所有权：当一个存在的对象被访问的时候（这个对象可能在任何时候被删除），`weak_ptr` 被用来追踪这个对象，并且可以被转换成 `shared_ptr`，从而达到获取**临时所有权**的目的。如果这个对象对应的原始 `shared_ptr` 在这个时候被销毁，这个对象的生命周期也不会立刻结束，其生命周期会扩展到这个从 `weak_ptr` 转换来的 `shared_ptr` 被销毁的时候
3. `weak_ptr` 的另外一个用途是解决对象的 ***reference cycle*** 问题（这里面的循环引用关系是通过 `shared_ptr` 形成的）。如果循环引用形成，这些引用计数由于循环指向，永远不会等于 0，则会造成内存泄漏。为了解决这种问题，可以将引用环中某一个或多个引用替换成 `weak_ptr` 类型的引用。

下面我们分别对上面一些概念进行补充：

1. `“Non-owning”` 引用：`shared_ptr` 和 `unique_ptr` 这两种指针强调的是所有权，前者表示该对象被多个对象所持有，后者表示被唯一的对象所持有。这里面的持有状态对应的是对象的生命周期控制，当所有的持有者释放（销毁）对应的 `shared_ptr` 或者 `unique_ptr` 对象的时候，该对象才不被持有，才会结束生命周期。这里面的 `owning` 对应的是控制该对象的生命周期，前面的 `non` 代表不对对象的生命周期进行持有（管理和控制），也就是说被 `weak_ptr` 所指向的对象的生命周期是不受影响的
2. 临时所有权：这里面的临时所有权，指的是可以通过 `weak_ptr` 获得一个 `shared_ptr` 来对对象进行访问，当这个 `shared_ptr` 生命周期结束的时候，对应代码对这个对象的所有权会进行释放。这种随用随取、用完释放的思想对应这里的临时所有权

### 1. 数据成员以及所有权

通过上面的描述，我们对于 `weak` 指针的功能和用途有了了解，下面我们从 `STL` 源码实现的角度来看看，标准库是怎么实现的

我们首先先看一下数据成员，截图如下；其中:

1. 对应的是指向被持有对象的指针，通过这个指针来进行对象的访问；
2. 对应的是维护一个引用计数的指针，用来追踪记录对象的生命周期

```cpp
template<class _Tp>
class _LIBCPP_TEMPLATE_VIS weak_ptr {
public:
    typedef _Tp element_type;
private:
    element_type*        __ptr__;    // 1
    __shared_weak_count* __cntrl__;  // 2
};
```

从上面的成员中，我们可以看到，比较核心的点在第 2 个数据成员 `__cntrl_` 上了。这个成员对应的类型是 `__shared_weak_count` 的指针类型，用来指向 `shared_ptr` 对象的内部的引用计数相关的成员变量的。通过对这个数据进行访问和操作，可以实现获取临时访问权和追踪生命周期的目的。

从 `__shared_weak_count* __cntrl__;` 中的 `shared` 关键字我们可以推断出，该智能指针是需要配合 `shared pointer` 来使用的。上面的文章中我们提到，`weak_ptr` 是用来获取对象的临时所有权（`temporary ownership`）的，这个对象的临时所有权是从 `shared_ptr` 中获取到的。也就是说这里的临时所有权对应的是 `shared_ptr` 的共享所有权（`shared ownership`）

### **2. 构造**

下面，我们看一下 `weak_ptr` 的相关拷贝构造函数和赋值函数，看看其是如何构造的

#### **构造函数**

在 cppreference 官网中的构造函数的情况截图如下：

![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763896886754.1ovv8te8xx.webp)

上面的构造函数，可以分为 3 类，分别是默认构造函数、从 `shared_ptr` 构造、从其他 `weak_ptr` 构建

- 默认构造函数：构造一个空的 `weak_ptr`，该指针对象没有指向，效果类似于空指针
- 从 `shared_ptr` 构造：如上面所说的 `weak_ptr` 的临时所有权是从 `shared_ptr` 指针获取的，这里正好印证了上面的讨论
- 从其他 `weak_ptr` 构造：`weak_ptr` 对象有两个成员变量（`__cntrl_` 和 `__ptr_`），前者是用来保存引用计数数据块地址的，后者是指针指向的数据地址。当从其他 `weak_ptr` 构造时，构造函数执行的主要工作就是复制这两个指针，其效果就相当于从其他 `weak_ptr` 对应的 `shared_ptr` 指针构造

#### **赋值函数**

在 cppreference 官网中的赋值构造函数的情况截图如下：

![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763897010856.szdtd77nv.webp)

从上面图片中我们可以看出，和构造函数基本上是一致的，唯一的差别就是没有默认构造函数。主要分为两种：从 `shared_ptr` 中赋值、从其他 `weak_ptr` 赋值。从获取所有权的角度来讲，作用同构造函数，这里不再赘述

#### **数据成员指向图例**

下图展示了数据成员的指向，从下图中可以看出。`shared_ptr` 和 `weak_ptr` 中都有两个指针，它们分别指向用来保存引用信息的控制块数据（`_CntrlBlk`，该数据块由第一个持有数据对象的 `shared_ptr` 对象实例化的时候进行创建，由最后一个相关弱引用释放后销毁）以及被持有对象

![img](https://pic1.zhimg.com/v2-268a67650913ce176aa31da6f1da7a78_1440w.jpg)

从数据成员的指向上来看，二者基本上是一致的。但是这两种智能指针的对象在构造的时候是有区别的，`shared_ptr` 在构造的时候，会将控制对象生命周期的强引用计数增加 1；而 `weak_ptr` 不会增加强引用计数，只会增加弱引用计数。控制生命周期对应的强引用计数不增加，对应的就是一种弱引用的概念。

下面的截图是对应上面讨论的当数据块由第一个 `shared_ptr` 持有的时候，会创建一个保存引用计数信息的对应构造函数

![img](https://pic3.zhimg.com/v2-d603e85dcb35b3073fd8f91f95df65cc_1440w.jpg)

#### **左值构建和右值构建**

在上面的构造函数和赋值构造函数中我们可以看出，其中从其他 `weak_ptr` 构造的时候分为两个版本，从左值构造和从右值构造。从 `shared_ptr` 构造则没有右值重载，这是因为 `weak_ptr` 本身获取的是临时所有权，是从 `shared_ptr` 获取的临时所有权，右值强调的是一个移动的概念，如果从右值构造意味着将剥夺 `shared_ptr` 的所有权，显然这从语义上来讲是不合理的。下面我们先看一下从其他 `weak_ptr` 构造的构造函数源码，源码截图如下

![img](https://pica.zhimg.com/v2-2c26827d3513a968476a99db5e9e7634_1440w.jpg)

这里的这个弱引用计数不是用来控制被持有变量的生命周期的，在控制块中有另外一个引用计数来控制数据的生命周期。弱引用计数的作用是保留控制块，使得 weak_ptr 可以访问相关联的 shared_ptr 是否析构掉了。

上面看完了构造函数的实现，下面我们看一下赋值构造函数的实现，代码截图如下

![img](https://pic1.zhimg.com/v2-11aee797f9c809bcbcf34917ee837476_1440w.jpg)

上图中，上半部分就是从左值赋值函数，下半部分就是右值赋值函数，两个函数的思路都是在内部构建一个 `weak_ptr` 临时对象，然后调用 `swap` 函数交换对象内部的成员变量（swap 函数的具体实现如下图所示）。差别在于左值赋值函数中的临时对象采用左值构造函数，右值赋值函数中的临时对象采用右值构造函数。也就是说赋值函数是通过在函数内部调用不同的构造函数来实现的

![img](https://pic2.zhimg.com/v2-c677eaaaf352f89a7c1dfa0bcc2141fd_1440w.jpg)

`weak_ptr` 对应的成员函数 `swap` 执行的动作是将内部的两个成员变量进行数据交换

### **3. 其他成员函数以及相关用途**

如下图所示，我们将逐一探讨下图中的成员函数的实现以及功能

![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1763897611116.73udr98wpx.webp)

#### **reset**

该函数用来释放指针对被持有对象的所有权，函数的实现如下图所示。下图中函数内部先构造了一个`weak_ptr` 类型的临时变量，这个临时变量的构造方式是通过默认构造函数进行构造的（`__cntrl_` 和 `__ptr_` 这两个指针都初始化为空），然后通过 `swap` 函数将当前对象和这个临时对象进行这两个数据成员进行值交换。函数退出的时候，这个临时对象会进行析构操作

![img](https://pic3.zhimg.com/v2-93c41389104df0a563809453c3ac5c48_1440w.jpg)

这个临时对象进行析构的时候，所执行的操作如下面代码截图所示。析构函数中先判断该指针是否有指向，如果有指向执行 `__release_weak` 函数，这个函数的实现我没有找到，我猜测是用来将弱引用计数减1的（如果哪位大佬知道，欢迎评论区不吝赐教）

![img](https://picx.zhimg.com/v2-e26e49cc06cfbff30b1a295e2c3bd167_1440w.jpg)

上面的操作效果就相当于断掉当前 `weak_ptr` 对于两个指针成员变量的指向信息，然后减少控制块中的弱引用计数

#### **use_count**

该函数用来查看当前 `weak_ptr` 所指向的对象被多少个 `shared_ptr` 所管理，即返回该对象对应的强引用计数（不同于弱引用计数，这里面的引用计数是用来控制被管理对象生命周期的）。下面我们来看一下具体的代码实现，截图如下

![img](https://pic2.zhimg.com/v2-f4a95513ad09bd217006b114483600c1_1440w.jpg)

如上截图所示，该函数首先判断指向控制块的成员变量 `__cntrl_` 指针是否为空，如果为空返回 0，代表该指针没有指向。如果指针不为空，返回该控制块对象的 `use_count` 成员函数的执行结果。该成员函数会对控制被管理对象生命周期的引用计数进行原子读取（线程安全），代表当前对象被多少个 `shared_ptr` 实例所持有

#### **expired**

用来检查指针所管理的对象是否被删除（或者指针是否没有指向，即指针为空），具体的代码实现如下

![img](https://pica.zhimg.com/v2-52c4b5f3be1a009f8cf4949cf7fc1702_1440w.png)

#### **lock**

是用来创建一个 `shared_ptr` 指针，该指针指向被持有的对象。通常我们在访问 `weak_ptr` 指针所管理的对象的时候，应该通过这个方法获取到指向被持有对象的 `shared_ptr` 指针，然后通过这个指针进行对象的访问

![img](https://pic1.zhimg.com/v2-4431c27a05d3c4125613a632537d0512_1440w.jpg)

上面代码首先构建了一个 `shared_ptr` 指针，该指针没有指向。然后判断 `weak_ptr` 内部的 `__cntrl_` 指针是否指向有效的控制块（如果为空，说明没有指向任何控制块，说明当前的 `weak_ptr` 没有指向），然后调用该控制块的 `lock` 函数，该函数会先判断被管理的对象是否存在。如果存在构建一个指向该对象的 `shared_ptr` 的智能指针对应的控制块，如果不存在返回空指针

在后边的`if`判断语句中，会判断是否有有效的指向控制块的指针返回。如果有效，则将指向由 `weak_ptr` 持有的对象的指针赋值给刚刚创建的 `shared_ptr` 对象的 `__ptr_` 指针。最后将这个构建的 `shared_ptr` 指针返回

下面，我们来说一下通过该方法进行对象的访问

```cpp
//假设有一个类A
std::shared_ptr<A> sp = std::make_shared<A>();
std::weak_ptr<A> wp(sp);

//访问
if(auto sp1 = wp.lock())
{
    sp1->DoSomething();
}
```

另外，还有一个细节来说一下，上边有一个 `expired` 函数，有些人可能用如下代码的方式进行对象的访问

```cpp
if(!wp.expired())
{
    wp.lock()->DoSomething();
}
```

上面这种方法，在多线程程序中，是有程序崩溃的可能的。比如在程序运行到第 1 行，判断了当前 `weak_ptr` 指向的对象有效（指向的对象没有被释放），这个时候程序流走到第三行，在还未执行 `lock` 函数的时候，在另外的线程中，所有指向被管理对象的 `shared_ptr` 指针对象都被释放了，此时被这些智能指针所管理的对象生命周期截止，对象被释放。此时上面代码中通过 `lock` 函数得到的 `shared_ptr` 是一个没有指向的空 `shared_ptr`，这个时候直接通过 `->` 操作符访问对象，就相当于用空指针访问对象，程序就会出现异常。

> `std::weak_ptr<T>::lock()` **本身在“控制块/引用计数层面”是线程安全的**：
>
> 多个线程同时对**各自的** `weak_ptr`（哪怕它们指向同一个控制块）调用 `lock()`，不会破坏控制块，也不会得到悬空的 `shared_ptr`。标准语义等价于“原子地”执行 `expired() ? shared_ptr{} : shared_ptr{*this}`，要么拿到一个有效的 `shared_ptr` 并把强引用计数 +1，要么得到空指针

### 参考资料

[1] [从源码了解Weak Point](https://zhuanlan.zhihu.com/p/617034450)

[2] [std::weak_ptr](https://en.cppreference.com/w/cpp/memory/weak_ptr.html)

## alloc traits

我们在看 STL 源码时会碰到各种各样的 traits，iterator traits 用来萃取迭代器的特性，type traits 用来萃取型别的特性......当然，对于空间配置器也有相应的 traits——alloc traits。

alloc 通常为类模板，当我们要为具体类型进行空间配置的话，就需要特化为具体类型的版本，这时候就需要用 alloc traits 完成这个工作，并将具体对象的型别提取出来。

另外，alloc 会定义提供空间的配置与释放函数、对象的构造与析构函数，这个时候需要 alloc traits 提供一个 API 去调用 alloc 提供的相关函数。

例如，rb tree 源码中的如下代码，就是将 `_Alloc<void>` 转成 `_Alloc<_Rb_tree_node<_Val>>`

```cpp
typedef typename __gnu_cxx::__alloc_traits<_Alloc>::templaterebind<_Rb_tree_node<_Val> >::other _Node_allocator;
```

然后使用具体的 alloc 形成具体的 traits，去使用具体的函数

```cpp
typedef __gnu_cxx::__alloc_traits<_Node_allocator> _Alloc_traits;
```

今天我们就剖析一下 alloc traits 的机制，解决掉阅读源码时的这个拦路虎，并学习一下这种编程思想。

### 0. allocator 解决的核心问题

在看 allocator traits 之前，我们需要先搞清楚 allocator 到底解决什么问题。STL 容器的内存管理可以拆成两层：

1. **分配原始内存**：只获得一段未初始化的字节空间，不调用对象构造函数。
2. **在原始内存上构造对象**：使用 placement new，把对象构造在指定地址上。

对应到代码就是下面这种形式：

```cpp
T* p = alloc.allocate(n);                 // 分配 n 个 T 的原始空间
std::allocator_traits<Alloc>::construct(  // 在 p+i 处构造对象
    alloc, p + i, value);
std::allocator_traits<Alloc>::destroy(    // 析构 p+i 处对象
    alloc, p + i);
alloc.deallocate(p, n);                   // 归还原始空间
```

那么为什么不直接 `new T[n]` 呢？原因是容器需要把“容量”和“已构造对象数量”分开。例如 `vector` 的 `capacity()` 可能是 100，但 `size()` 只有 20。剩下 80 个位置只是原始内存，不能被析构，也不能当作已经存在的对象。

### 1. `std::allocator<T>` 的基本接口

现代 C++ 中，`std::allocator<T>` 可以理解为最普通的分配器。我们先看一下它的核心接口：

```cpp
template<class T>
class allocator
{
public:
    using value_type = T;

    allocator() noexcept;

    template<class U>
    allocator(const allocator<U>&) noexcept;

    T* allocate(size_t n);
    void deallocate(T* p, size_t n);
};
```

早期 STL 的 allocator 还会直接提供 `construct`、`destroy`、`address`、`max_size`、`rebind` 等成员。C++11 之后这些能力更多通过 `allocator_traits` 统一适配。

### 2. 为什么需要 `allocator_traits`

那为什么还需要 `allocator_traits` 呢？如果容器直接调用 allocator 的成员函数，会遇到两个问题：

- 不同 allocator 提供的成员不一定完整。
- 容器需要处理 allocator 的传播规则、rebind、构造/析构回退逻辑。

于是标准库引入 `allocator_traits<Alloc>` 作为统一入口。也就是说，容器通常不直接写：

```cpp
alloc.construct(p, value);
```

而是写：

```cpp
std::allocator_traits<Alloc>::construct(alloc, p, value);
```

如果 `Alloc` 自己提供了 `construct`，traits 就转发给它；如果没有，traits 使用 placement new 作为默认实现。

我们写一个简化版来理解这个机制：

```cpp
template<class Alloc>
struct allocator_traits
{
    using allocator_type = Alloc;
    using value_type = typename Alloc::value_type;
    using pointer = value_type*;
    using size_type = size_t;

    template<class T>
    using rebind_alloc = typename Alloc::template rebind<T>::other;

    static pointer allocate(Alloc& a, size_type n)
    {
        return a.allocate(n);
    }

    static void deallocate(Alloc& a, pointer p, size_type n)
    {
        a.deallocate(p, n);
    }

    template<class T, class... Args>
    static void construct(Alloc& a, T* p, Args&&... args)
    {
        ::new ((void*)p) T(std::forward<Args>(args)...);
    }

    template<class T>
    static void destroy(Alloc& a, T* p)
    {
        p->~T();
    }
};
```

真实实现会更复杂，会使用 SFINAE 检测 allocator 是否有对应成员，并处理 fancy pointer、传播规则和异常规格。但核心思想就是：容器只和 traits 打交道，traits 再去适配具体 allocator。

### 3. `rebind`：给节点类型重新绑定分配器

继续往下看会遇到一个问题：容器的模板参数通常是 `allocator<value_type>`，但容器内部要分配的未必就是 `value_type`。

例如 `list<T>` 对外保存 `T`，内部实际分配的是链表节点：

```cpp
template<class T>
struct __list_node
{
    __list_node* prev;
    __list_node* next;
    T value;
};
```

如果用户传入的是 `allocator<T>`，容器需要得到一个能分配 `__list_node<T>` 的 allocator。这就是 `rebind` 的用途：

```cpp
using node_allocator =
    typename allocator_traits<Alloc>::template rebind_alloc<__list_node<T>>;
```

红黑树也一样：`set<Key>` 对外的 `value_type` 是 `Key`，但底层要分配 `_Rb_tree_node<Key>`；`map<Key, T>` 对外的 `value_type` 是 `pair<const Key, T>`，底层要分配 `_Rb_tree_node<pair<const Key, T>>`。

### 4. placement new 与对象生命周期

allocator 的 `allocate` 只负责分配原始内存，不会调用构造函数。真正构造对象要依赖 placement new：

```cpp
void* raw = ::operator new(sizeof(T));
T* p = static_cast<T*>(raw);

::new ((void*)p) T(1, 2, 3);  // 在 p 指向的内存上构造 T
p->~T();                      // 显式析构对象
::operator delete(raw);        // 释放原始内存
```

这也解释了为什么容器析构时不能简单 `delete[]`。容器必须知道哪些位置已经构造了对象，只析构 `[begin, end)` 这段有效元素，然后释放整段原始内存。

### 5. `uninitialized_copy` / `uninitialized_fill` / `uninitialized_move`

STL 中还有一组算法专门操作“未初始化内存”：

- `uninitialized_copy(first, last, result)`
- `uninitialized_fill(first, last, value)`
- `uninitialized_fill_n(first, n, value)`
- `uninitialized_move(first, last, result)`

它们和普通 `copy/fill/move` 的区别如下：

- 普通算法假设目标位置已经存在对象，执行赋值。
- uninitialized 系列假设目标位置只有原始内存，执行构造。

我们看一下简化版 `uninitialized_copy`：

```cpp
template<class InputIt, class ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt result)
{
    ForwardIt cur = result;
    try
    {
        for (; first != last; ++first, ++cur)
        {
            using Value =
                typename iterator_traits<ForwardIt>::value_type;
            ::new ((void*)std::addressof(*cur))
                Value(*first);
        }
        return cur;
    }
    catch (...)
    {
        using Value =
            typename iterator_traits<ForwardIt>::value_type;
        for (; result != cur; ++result)
            std::addressof(*result)->~Value();
        throw;
    }
}
```

这里最核心的点是异常回滚：如果构造第 k 个元素时抛异常，前面 `[result, cur)` 已构造的对象必须逐个析构，否则资源泄漏。

### 6. type traits 优化

对于 `int`、`char`、POD/trivially copyable 类型，逐个 placement new 可能没有必要。标准库会通过 type traits 判断是否可以走更快路径：

```cpp
if constexpr (is_trivially_copy_constructible_v<T>)
    memmove(dst, src, n * sizeof(T));
else
    for (...) ::new ((void*)dst) T(*src);
```

这也是 STL 中 traits 技法的另一个典型应用：接口保持泛型，但实现根据类型特性做编译期分派。

### 7. allocator propagation

容器拷贝、移动、swap 时，还有一个问题：allocator 是否跟着传播？这个行为由 traits 中的几个类型控制：

```cpp
allocator_traits<Alloc>::propagate_on_container_copy_assignment
allocator_traits<Alloc>::propagate_on_container_move_assignment
allocator_traits<Alloc>::propagate_on_container_swap
allocator_traits<Alloc>::is_always_equal
```

这些规则会影响容器赋值和移动时能不能直接“偷取”对方内存。

例如 `vector` 移动赋值时：

- 如果 allocator 可以传播，目标容器可以释放自己的内存，然后直接接管源容器的 buffer。
- 如果 allocator 不传播但两个 allocator 相等，也可以接管。
- 如果 allocator 不传播且不相等，目标容器不能用自己的 allocator 释放源容器的内存，只能逐个 move 构造元素。

这也是前面 `string` 赋值章节里那句注释的背景：

> Propagating allocator cannot free existing storage so must deallocate it before replacing current allocator.

### 8. vector 扩容时 allocator 的完整路径

最后，我们以 `vector::push_back` 触发扩容为例，把 allocator 的完整路径串起来：

1. 计算新容量。
2. 用 allocator 分配一段更大的原始内存。
3. 在新内存中构造新元素。
4. 把旧元素 move/copy 到新内存。
5. 如果中途抛异常，析构新内存中已构造对象并释放新内存。
6. 成功后析构旧元素。
7. 释放旧内存。
8. 更新 `_M_start`、`_M_finish`、`_M_end_of_storage`。

伪代码如下：

```cpp
pointer new_start = alloc_traits::allocate(alloc, new_cap);
pointer new_finish = new_start;

try
{
    new_finish = uninitialized_move(old_start, pos, new_start);
    alloc_traits::construct(alloc, new_finish, value);
    ++new_finish;
    new_finish = uninitialized_move(pos, old_finish, new_finish);
}
catch (...)
{
    destroy(new_start, new_finish);
    alloc_traits::deallocate(alloc, new_start, new_cap);
    throw;
}

destroy(old_start, old_finish);
alloc_traits::deallocate(alloc, old_start, old_cap);

_M_start = new_start;
_M_finish = new_finish;
_M_end_of_storage = new_start + new_cap;
```

所以，allocator 不是孤立组件，它贯穿所有容器的构造、析构、扩容、插入、删除、移动和异常安全。

## `_Rb_tree_`

前面分析 `set` 的时候已经提到过，`set` 的底层结构是红黑树。实际上，`map`、`multiset`、`multimap` 的底层也都是红黑树。理解 `_Rb_tree` 后，关联容器的大部分源码都只是对红黑树接口的薄封装。

### 0. 红黑树的性质

我们先回顾一下红黑树的基本性质。红黑树是一棵近似平衡的二叉搜索树，它通过给节点染色并在插入/删除后旋转调整，保证树高为 `O(logN)`。经典红黑树满足以下性质：

1. 每个节点要么是红色，要么是黑色。
2. 根节点是黑色。
3. 所有叶子空节点视为黑色。
4. 红色节点的孩子必须是黑色，即不能出现连续红节点。
5. 从任一节点到其所有后代空节点的路径上，黑色节点数量相同。

性质 4 限制局部连续倾斜，性质 5 限制任意路径的黑色高度。两者共同保证最长路径不会超过最短路径的两倍，因此查找、插入、删除都是 `O(logN)`。

### 1. 节点结构

下面我们先看节点结构。SGI/libstdc++ 风格的红黑树通常把节点拆成 base 节点和值节点：

```cpp
enum _Rb_tree_color { _S_red = false, _S_black = true };

struct _Rb_tree_node_base
{
    typedef _Rb_tree_node_base* _Base_ptr;

    _Rb_tree_color _M_color;
    _Base_ptr      _M_parent;
    _Base_ptr      _M_left;
    _Base_ptr      _M_right;

    static _Base_ptr _S_minimum(_Base_ptr __x)
    {
        while (__x->_M_left != 0)
            __x = __x->_M_left;
        return __x;
    }

    static _Base_ptr _S_maximum(_Base_ptr __x)
    {
        while (__x->_M_right != 0)
            __x = __x->_M_right;
        return __x;
    }
};

template<typename _Val>
struct _Rb_tree_node : public _Rb_tree_node_base
{
    _Val _M_value_field;
};
```

base 节点只保存树结构和值无关的信息；值节点继承 base，再额外保存 `value_type`。这样迭代器可以只拿一个 `_Rb_tree_node_base*` 完成前进/后退，不必关心具体 `value_type`。

现代 libstdc++ 为了配合 allocator 和对象生命周期，会把 `_M_value_field` 改成 aligned storage：

```cpp
__gnu_cxx::__aligned_membuf<_Val> _M_storage;

_Val* _M_valptr()
{ return _M_storage._M_ptr(); }
```

这样节点内存分配完成后，`value_type` 可以延迟到插入时再 placement new 构造。

### 2. header 节点

`_Rb_tree` 通常不只保存根节点，还会额外保存一个 header 节点：

```cpp
struct _Rb_tree_header
{
    _Rb_tree_node_base _M_header;
    size_t             _M_node_count;
};
```

header 的几个指针有特殊含义：

```cpp
_M_header._M_parent  // root
_M_header._M_left    // leftmost，最小节点
_M_header._M_right   // rightmost，最大节点
```

这带来几个好处：

- `begin()` 可以 `O(1)` 返回最小节点。
- `end()` 可以直接返回 header。
- `rbegin()` 可以从 rightmost 开始。
- 空树时 root/leftmost/rightmost 都能统一挂到 header，减少边界判断。

常见接口可以理解为：

```cpp
_Base_ptr& _M_root()     { return _M_header._M_parent; }
_Base_ptr& _M_leftmost() { return _M_header._M_left; }
_Base_ptr& _M_rightmost(){ return _M_header._M_right; }

iterator begin()
{ return iterator(_M_header._M_left); }

iterator end()
{ return iterator(&_M_header); }
```

### 3. 迭代器为什么能按顺序移动

红黑树迭代器本质上保存一个节点指针：

```cpp
struct _Rb_tree_iterator
{
    typedef _Rb_tree_node_base::_Base_ptr _Base_ptr;
    _Base_ptr _M_node;

    reference operator*() const;
    _Self& operator++();
    _Self& operator--();
};
```

`operator++` 返回中序遍历的下一个节点，逻辑如下：

```cpp
void _Rb_tree_increment(_Base_ptr& x)
{
    if (x->_M_right != 0)
    {
        x = x->_M_right;
        while (x->_M_left != 0)
            x = x->_M_left;
    }
    else
    {
        _Base_ptr y = x->_M_parent;
        while (x == y->_M_right)
        {
            x = y;
            y = y->_M_parent;
        }
        if (x->_M_right != y)
            x = y;
    }
}
```

含义：

- 如果当前节点有右子树，下一个节点就是右子树的最左节点。
- 如果没有右子树，就不断向上找第一个“当前节点位于其左子树”的祖先。

`operator--` 对称：

- 如果当前节点有左子树，前一个节点就是左子树的最右节点。
- 如果没有左子树，就向上找第一个“当前节点位于其右子树”的祖先。

由于红黑树节点不会像 `vector` 一样整体搬迁，插入不会导致已有节点地址变化，所以关联容器插入通常不使已有 iterator/reference 失效。

### 4. 查找

红黑树查找就是二叉搜索树查找：

```cpp
iterator find(const key_type& k)
{
    link_type x = _M_begin(); // root
    link_type y = _M_end();   // last not less than k

    while (x != 0)
    {
        if (!_M_key_compare(_S_key(x), k))
        {
            y = x;
            x = _S_left(x);
        }
        else
        {
            x = _S_right(x);
        }
    }

    iterator j = iterator(y);
    return (j == end() || _M_key_compare(k, _S_key(j._M_node)))
        ? end()
        : j;
}
```

这里的写法看起来不像简单的 `if (k < key) left; else if (key < k) right; else found;`，但本质是在寻找第一个 `!(key < k)` 的节点，也就是 `lower_bound(k)`。最后再检查 `k < key` 是否成立，如果成立说明没有相等元素。

### 5. `lower_bound` / `upper_bound`

`lower_bound(k)` 返回第一个 `key >= k` 的节点：

```cpp
iterator lower_bound(const key_type& k)
{
    link_type x = _M_begin();
    link_type y = _M_end();

    while (x != 0)
    {
        if (!_M_key_compare(_S_key(x), k)) // x->key >= k
        {
            y = x;
            x = _S_left(x);
        }
        else
        {
            x = _S_right(x);
        }
    }
    return iterator(y);
}
```

`upper_bound(k)` 返回第一个 `key > k` 的节点：

```cpp
iterator upper_bound(const key_type& k)
{
    link_type x = _M_begin();
    link_type y = _M_end();

    while (x != 0)
    {
        if (_M_key_compare(k, _S_key(x))) // k < x->key
        {
            y = x;
            x = _S_left(x);
        }
        else
        {
            x = _S_right(x);
        }
    }
    return iterator(y);
}
```

`equal_range(k)` 就是 `{lower_bound(k), upper_bound(k)}`。

### 6. 插入位置：unique 与 equal

`set/map` 不允许重复 key，插入使用 `_M_insert_unique`。核心是先找插入位置：

```cpp
pair<_Base_ptr, _Base_ptr>
_M_get_insert_unique_pos(const key_type& k);
```

返回值可以理解为：

- `second != 0`：可以插入，`second` 是父节点，`first` 通常是空子树位置标记。
- `second == 0`：不能插入，`first` 指向已存在的等价 key 节点。

判断两个 key 等价不是用 `==`，而是用比较器：

```cpp
!comp(a, b) && !comp(b, a)
```

这也说明自定义比较器必须满足严格弱序。否则红黑树无法稳定维护“等价”和“有序”的含义。

`multiset/multimap` 允许重复 key，插入使用 `_M_insert_equal`。它只需要找到一个合法位置，不需要拒绝等价 key：

```cpp
iterator _M_insert_equal(const value_type& v)
{
    link_type x = _M_begin();
    link_type y = _M_end();

    while (x != 0)
    {
        y = x;
        x = _M_key_compare(_KeyOfValue()(v), _S_key(x))
            ? _S_left(x)
            : _S_right(x);
    }
    return _M_insert_(x, y, v);
}
```

等价元素通常会落在已有等价区间附近，但标准只保证容器有序，不保证相同 key 的相对插入顺序在所有实现中完全一致。

### 7. 旋转

红黑树通过旋转调整局部结构，同时保持中序顺序不变。

左旋：

```cpp
void rotate_left(node* x, node*& root)
{
    node* y = x->right;
    x->right = y->left;
    if (y->left)
        y->left->parent = x;

    y->parent = x->parent;
    if (x == root)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}
```

右旋与左旋对称。旋转只改变少量父子指针，不移动节点对象本身，因此不会让指向节点值的 iterator/reference 失效。

### 8. 插入后的 rebalance

红黑树插入新节点时，通常先把新节点染成红色。原因是：

- 插入红节点不会破坏黑高性质。
- 可能破坏的是“红节点不能连续”。

修复时关注新节点 `x`、父节点 `p`、祖父节点 `g`、叔父节点 `u`：

1. 父节点是黑色：无需调整。
2. 父节点是红色，叔父也是红色：父叔染黑，祖父染红，然后把祖父当作新节点继续向上修复。
3. 父节点是红色，叔父是黑色或空，且形成内侧结构：先对父节点旋转，转成外侧结构。
4. 父节点是红色，叔父是黑色或空，且形成外侧结构：对祖父旋转，父节点染黑，祖父染红。

最后强制根节点为黑色。

可以把四种方向压缩理解为：

```text
LL: right rotate(g), p black, g red
RR: left rotate(g),  p black, g red
LR: left rotate(p)  -> LL
RL: right rotate(p) -> RR
```

### 9. 删除后的 rebalance

删除比插入复杂，因为删除黑节点会破坏黑高。标准库实现一般先把删除问题转换成“真正从树中摘掉一个至多只有一个非空孩子的节点”：

1. 如果目标节点有两个孩子，找后继节点替换目标位置。
2. 实际摘掉的是后继节点或目标节点本身，该节点最多只有一个孩子。
3. 如果被摘掉的是红节点，不影响黑高。
4. 如果被摘掉的是黑节点，需要 rebalance。

删除修复的核心是处理“某条路径少了一个黑色”的问题。设当前需要修复的位置为 `x`，兄弟为 `w`，常见情况：

- 兄弟是红色：兄弟染黑，父节点染红，对父节点旋转，转换成兄弟为黑的情况。
- 兄弟是黑色，且兄弟两个孩子都是黑色：兄弟染红，缺失的黑色向父节点上传。
- 兄弟是黑色，远侄子是黑色，近侄子是红色：对兄弟旋转，转换成远侄子为红。
- 兄弟是黑色，远侄子是红色：兄弟继承父节点颜色，父节点和远侄子染黑，对父节点旋转，修复结束。

实际源码会区分 `x` 是左孩子还是右孩子，两边逻辑完全对称。

### 10. `_KeyOfValue`：set/map 复用同一棵树的关键

`_Rb_tree` 模板参数通常类似：

```cpp
template<class _Key, class _Val, class _KeyOfValue,
         class _Compare, class _Alloc>
class _Rb_tree;
```

其中：

- `_Key` 是排序用的 key。
- `_Val` 是节点中真正保存的 value。
- `_KeyOfValue` 负责从 value 中取出 key。

`set<Key>` 中：

```cpp
_Key = Key
_Val = Key
_KeyOfValue = _Identity<Key>
```

`map<Key, T>` 中：

```cpp
_Key = Key
_Val = pair<const Key, T>
_KeyOfValue = _Select1st<pair<const Key, T>>
```

这样红黑树只需要通过 `_KeyOfValue()(value)` 获得 key，就能同时服务 `set` 和 `map`。

### 11. 复杂度与失效规则

红黑树关联容器的复杂度：

| 操作 | 复杂度 |
| --- | --- |
| `find` | `O(logN)` |
| `lower_bound` / `upper_bound` | `O(logN)` |
| `insert` | `O(logN)` |
| 带正确 hint 的 `insert` | 均摊 `O(1)` |
| `erase(iterator)` | 摊还 `O(1)`，因为不需要再查找 key |
| `erase(key)` | `O(logN + count(key))` |

失效规则：

- 插入不会使已有 iterator/reference/pointer 失效。
- 删除只使指向被删除元素的 iterator/reference/pointer 失效。
- 旋转不会移动节点对象，只改指针关系，因此不会额外造成失效。

## set、unordered_set、multiset

前面我们单独分析过 `set`，这里再把集合类容器放在一起看一下：`set`、`multiset` 基于红黑树；`unordered_set` 基于哈希表。它们的共同点是只保存 key，没有额外的 mapped value。

### 0. `set`

前面已经分析过 `set` 的定义，这里再把核心结构拎出来看一下：

```cpp
template<typename _Key,
         typename _Compare = std::less<_Key>,
         typename _Alloc = std::allocator<_Key>>
class set
{
    typedef _Rb_tree<key_type, value_type,
                     _Identity<value_type>,
                     key_compare, _Key_alloc_type> _Rep_type;
    _Rep_type _M_t;
};
```

从上述结构中可以发现，`set` 有如下几个特点：

- key 和 value 是同一个对象。
- 底层是红黑树，元素按比较器自动有序。
- key 唯一，插入走 `_M_insert_unique`。
- iterator 通常是 const iterator，不能通过迭代器修改元素值。

为什么不能修改元素值呢？原因很关键：元素值本身就是排序 key。如果允许原地修改，可能破坏红黑树顺序不变量。正确做法是先 erase，再 insert 新值。

### 1. `multiset`

`multiset` 与 `set` 的底层结构几乎一样，我们看一下它的结构：

```cpp
template<typename _Key,
         typename _Compare = std::less<_Key>,
         typename _Alloc = std::allocator<_Key>>
class multiset
{
    typedef _Rb_tree<key_type, value_type,
                     _Identity<value_type>,
                     key_compare, _Key_alloc_type> _Rep_type;
    _Rep_type _M_t;
};
```

二者的区别主要在于插入策略：

```cpp
iterator insert(const value_type& x)
{
    return _M_t._M_insert_equal(x);
}
```

因此可以得到如下结论：

- `set::count(k)` 只能返回 0 或 1。
- `multiset::count(k)` 可以返回多个。
- `set::insert` 返回 `pair<iterator, bool>`，bool 表示是否插入成功。
- `multiset::insert` 通常返回 iterator，因为不会因 key 已存在而失败。

### 2. `unordered_set`

`unordered_set` 的底层是哈希表，而不是红黑树。它的特点如下：

- 元素不按大小排序。
- 平均查找/插入/删除复杂度是 `O(1)`。
- 最坏情况下可能退化到 `O(N)`。
- key 唯一。
- 插入可能触发 rehash，导致 iterator 失效。

源码大致封装如下：

```cpp
template<class _Value,
         class _Hash = hash<_Value>,
         class _Pred = equal_to<_Value>,
         class _Alloc = allocator<_Value>>
class unordered_set
{
    typedef _Hashtable<_Value, _Value, _Alloc,
                       _Identity, _Pred, _Hash> _Hashtable;
    _Hashtable _M_h;
};
```

其中 `_Identity` 表示 value 本身就是 key。

### 3. `set` 与 `unordered_set` 对比

| 容器 | 底层结构 | 是否有序 | key 是否唯一 | 平均查找复杂度 | iterator 稳定性 |
| --- | --- | --- | --- | --- | --- |
| `set` | 红黑树 | 是 | 是 | `O(logN)` | 插入不失效，删除只影响被删元素 |
| `multiset` | 红黑树 | 是 | 否 | `O(logN)` | 插入不失效，删除只影响被删元素 |
| `unordered_set` | 哈希表 | 否 | 是 | `O(1)` | rehash 会使 iterator 失效 |

总结一下它们的使用场景：

- 需要有序遍历、范围查询、`lower_bound`：选 `set/multiset`。
- 只关心快速查找，不关心顺序：选 `unordered_set`。
- 自定义类型放进 `set` 要提供比较器；放进 `unordered_set` 要提供 hash 和相等判断。

### 4. 自定义比较器与严格弱序

这里再强调一下比较器的问题。`set` 的比较器不是“判断相等”，而是“判断严格小于”。两个元素等价的定义是：

```cpp
!comp(a, b) && !comp(b, a)
```

下面是一个错误示例：

```cpp
struct BadComp
{
    bool operator()(int a, int b) const
    {
        return a <= b; // 错误：comp(a, a) 为 true
    }
};
```

这会破坏红黑树对“等价 key”的判断，严重时会造成查找异常或内部循环无法收敛。

### 5. `unordered_set` 的 hash 与相等关系

`unordered_set` 需要满足一条重要约束：

```cpp
if (eq(a, b) == true)
    hash(a) == hash(b);
```

也就是说，相等对象必须有相同哈希值。反过来不要求成立，因为哈希冲突是允许的。

下面看一个自定义类型的例子：

```cpp
struct Point
{
    int x;
    int y;
};

struct PointHash
{
    size_t operator()(const Point& p) const
    {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};

struct PointEq
{
    bool operator()(const Point& a, const Point& b) const
    {
        return a.x == b.x && a.y == b.y;
    }
};

unordered_set<Point, PointHash, PointEq> s;
```

## map、unordered_map、multimap

`map`、`multimap` 基于红黑树；`unordered_map` 基于哈希表。它们与 set 系列最大的区别是：节点保存的是键值对 `pair<const Key, T>`，但是排序或哈希只看 key。

### 0. `map`

我们先看 `map` 的典型定义：

```cpp
template<typename _Key, typename _Tp,
         typename _Compare = std::less<_Key>,
         typename _Alloc = std::allocator<std::pair<const _Key, _Tp>>>
class map
{
public:
    typedef _Key                  key_type;
    typedef _Tp                   mapped_type;
    typedef pair<const _Key, _Tp> value_type;

private:
    typedef _Rb_tree<key_type, value_type,
                     _Select1st<value_type>,
                     key_compare, _Pair_alloc_type> _Rep_type;
    _Rep_type _M_t;
};
```

注意，`value_type` 是：

```cpp
pair<const Key, T>
```

`first` 是 `const Key`，意味着不能通过 iterator 修改 key：

```cpp
it->first = new_key; // error
it->second = value;  // ok
```

原因和 `set` 一样：key 决定节点在红黑树中的位置，原地修改会破坏有序性。

### 1. `map::operator[]`

`operator[]` 是 `map` 最容易被误用的接口。我们先看一下它的语义：

```cpp
mapped_type& operator[](const key_type& k)
{
    iterator i = lower_bound(k);

    if (i == end() || key_comp()(k, (*i).first))
        i = insert(i, value_type(k, mapped_type()));

    return (*i).second;
}
```

也就是说：

- key 存在：返回对应的 mapped value。
- key 不存在：插入 `{k, T()}`，再返回新 value。

因此下面代码会改变容器：

```cpp
map<string, int> m;
cout << m["missing"]; // 插入 "missing" -> 0
```

如果只想查找，不想插入，应该使用：

```cpp
auto it = m.find(k);
if (it != m.end())
    use(it->second);
```

或 C++20 的：

```cpp
if (m.contains(k))
    ...
```

### 2. `insert` / `emplace` / `try_emplace`

`map::insert` 插入完整的 `value_type`：

```cpp
m.insert(pair<const string, int>("a", 1));
```

`emplace` 则是在节点内直接构造对象，减少临时对象：

```cpp
m.emplace("a", 1);
```

但是 `emplace` 在 key 已存在时，参数对象是否已经构造、移动，取决于实现路径。C++17 引入 `try_emplace`，更适合表达“key 不存在时才构造 mapped value”：

```cpp
map<string, unique_ptr<int>> m;
m.try_emplace("a", make_unique<int>(1));
```

`insert_or_assign` 则表达“存在就赋值，不存在就插入”：

```cpp
m.insert_or_assign("a", 42);
```

### 3. `multimap`

`multimap` 与 `map` 的底层结构相同，也是：

```cpp
pair<const Key, T>
```

区别是允许重复 key，插入走 `_M_insert_equal`：

```cpp
iterator insert(const value_type& x)
{
    return _M_t._M_insert_equal(x);
}
```

因此 `multimap` 中一个 key 可能对应多个 value。访问某个 key 的所有值通常使用 `equal_range`：

```cpp
auto range = mm.equal_range(k);
for (auto it = range.first; it != range.second; ++it)
    use(it->second);
```

`multimap` 没有 `operator[]`，因为一个 key 可能对应多个 mapped value，`[]` 无法表达应该返回哪一个。

### 4. `unordered_map`

`unordered_map` 的底层是哈希表，value_type 同样是：

```cpp
pair<const Key, T>
```

源码大致封装如下：

```cpp
template<class _Key, class _Tp,
         class _Hash = hash<_Key>,
         class _Pred = equal_to<_Key>,
         class _Alloc = allocator<pair<const _Key, _Tp>>>
class unordered_map
{
    typedef _Hashtable<_Key, pair<const _Key, _Tp>, _Alloc,
                       _Select1st, _Pred, _Hash> _Hashtable;
    _Hashtable _M_h;
};
```

从上面的结构可以看出，它的特点如下：

- 平均 `O(1)` 查找、插入、删除。
- 不保证遍历顺序。
- 插入可能触发 rehash。
- rehash 会使所有 iterator 失效，但 reference/pointer 通常仍指向节点元素，除非元素被删除。

### 5. `unordered_map::operator[]`

`unordered_map::operator[]` 与 `map::operator[]` 一样：key 不存在时会插入默认值。

```cpp
unordered_map<string, vector<int>> groups;
groups["cpp"].push_back(1); // 如果 "cpp" 不存在，先插入空 vector
```

这个行为很方便，但也可能导致无意插入。只读查询时不要使用 `operator[]`。

### 6. map 系列对比

| 容器 | 底层结构 | 是否有序 | key 是否唯一 | 查询复杂度 | 典型用途 |
| --- | --- | --- | --- | --- | --- |
| `map` | 红黑树 | 是 | 是 | `O(logN)` | 有序字典、范围查询 |
| `multimap` | 红黑树 | 是 | 否 | `O(logN + count)` | 一个 key 多个 value |
| `unordered_map` | 哈希表 | 否 | 是 | 平均 `O(1)` | 快速字典 |

### 7. 关联容器常见陷阱

1. `map::operator[]` 会插入默认值，不适合只读查询。
2. `set/map` 的比较器必须是严格弱序。
3. `unordered_map` 自定义 key 时，hash 与 equality 必须一致。
4. `unordered_map` rehash 后 iterator 失效，不要在遍历时无保护地插入大量元素。
5. 不要修改 `map` 的 key 或 `set` 的元素；要修改 key，先删除再插入。
6. 对 `unordered_map` 性能敏感时，可以提前 `reserve(n)`，减少 rehash 次数。

## STL 源码阅读总结

最后，我们把前面分析过的内容串起来，总结一下 STL 源码的阅读顺序。这样后面继续补充其他组件时，也可以按照这个思路往下看。

### 0. 泛型基础设施

首先要看的就是泛型基础设施：

- `iterator_traits`
- `type_traits`
- `allocator_traits`
- `uninitialized_xxx`
- functor 与比较器

这部分解决的是两个问题：算法如何知道迭代器和类型有什么能力，以及容器如何分配内存并构造对象。

### 1. 顺序容器

然后再看顺序容器：

- `vector`
- `string`
- `deque`
- `list`

这一部分重点看：

- 内存布局
- 扩容策略
- 插入删除路径
- iterator 结构
- 异常回滚
- 失效规则

其中 `vector` 是连续空间代表，`deque` 是分段连续空间代表，`list` 是节点式容器代表。

### 2. 算法

接着回过头来看算法：

- `sort` / `stable_sort`
- heap 系列算法
- `lower_bound` / `upper_bound`
- `copy` / `move` / `fill`
- `remove` / `unique`
- `partition` / `rotate`
- `nth_element` / `partial_sort`

这一部分重点理解算法对 iterator category 的要求。例如 `sort` 必须是 RandomAccessIterator，所以 `list` 不能直接使用 `std::sort`，而是提供自己的 `list::sort`。

### 3. 关联容器

再往后看关联容器：

- `_Rb_tree`
- `set` / `map`
- `multiset` / `multimap`
- `hashtable`
- `unordered_set` / `unordered_map`

这一部分重点看：

- `Key` 与 `Value` 的区别
- `_KeyOfValue`
- 比较器的严格弱序
- hash 与 equality 的一致性
- rehash 策略
- 节点式容器的 iterator 稳定性

### 4. 资源管理组件

最后再看资源管理组件：

- `unique_ptr`
- `shared_ptr`
- `weak_ptr`
- `enable_shared_from_this`

这一部分重点看：

- 所有权语义
- 控制块
- deleter
- 强弱引用计数
- `make_shared` / `allocate_shared`
- 循环引用与重复控制块问题

### 5. 总结

每读一个 STL 组件，都可以回过头来问自己下面这些问题：

1. 它对外提供什么语义？
2. 它底层核心数据结构是什么？
3. 它的 iterator 保存了什么状态？
4. 插入、删除、查找的关键路径是什么？
5. 内存由谁分配，对象由谁构造？
6. 中途抛异常时如何回滚？
7. 哪些操作会导致 iterator/reference/pointer 失效？
8. 复杂度为什么能达到标准要求？
9. 哪些行为依赖比较器、hash、allocator？
10. 哪些是标准保证，哪些只是某个实现的内部细节？
