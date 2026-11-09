# Reference

[python tutor code visualize](https://pythontutor.com/render.html#mode=edit)

[Python Cookbook 3rd Edition Documentation](https://python3-cookbook.readthedocs.io/zh-cn/latest/index.html)

[Python 3.14.0 documentation](https://docs.python.org/3/index.html)

[Design and History FAQ](https://docs.python.org/3/faq/design.html#id2)

[内置函数](https://docs.python.org/zh-cn/3.12/library/functions.html)

[Python 教程](https://docs.python.org/zh-cn/3/tutorial/index.html)

[Python 设计哲学](https://docs.python.org/3.12/faq/design.html)

[Python 为什么？](https://github.com/chinesehuazhou/python-whydo)

[PEP 8 – Style Guide for Python Code](https://peps.python.org/pep-0008/)



# English

* json: JavaScript Object Notion
* csv: Comma Separated Value
* pdf: Portable Document Format 
* pip: “pip” 的全称是 “Pip Installs Packages”，这是一个递归缩写（recursive acronym），类似于 GNU（GNU's Not Unix）。



* leeches: 伸手党，（吸血，蚂蝗的延伸）
* indent：缩进
* intersection：十字路口，交点，交汇
* disjoint：不连贯的；（两个集合）不相交的
* dunder：Python中双下划线的读法
* crop：剪短；剪裁（照片）
* thumbnail：缩略图
* flip：翻转
* watermark：水印
* wildcard：通配符，百搭牌
* purge：净化，清除，肃清
* descendant：后代
* axes：轴线

# Zen of Python

Python之禅：

* Beautiful is better than ugly. （优美比丑陋好）
* Explicit is better than implicit.（清晰比晦涩好）
* Simple is better than complex.（简单比复杂好）
* Complex is better than complicated.（复杂比错综复杂好）
* Flat is better than nested.（扁平比嵌套好）
* Sparse is better than dense.（稀疏比密集好）
* Readability counts.（可读性很重要）
* Special cases aren't special enough to break the rules.（特殊情况也不应该违反这些规则）
* Although practicality beats purity.（但现实往往并不那么完美）
* Errors should never pass silently.（异常不应该被静默处理）
* Unless explicitly silenced.（除非你希望如此）
* In the face of ambiguity, refuse the temptation to guess.（遇到模棱两可的地方，不要胡乱猜测）
* There should be one-- and preferably only one --obvious way to do it.（肯定有一种通常也是唯一一种最佳的解决方案）
* Although that way may not be obvious at first unless you're Dutch.（虽然这种方案并不是显而易见的，因为你不是那个荷兰人[这里指的是Python之父Guido]）
* Now is better than never.（现在开始做比不做好）
* Although never is often better than *right* now.（不做比盲目去做好[极限编程中的YAGNI原则]）
* If the implementation is hard to explain, it's a bad idea.（如果一个实现方案难于理解，它就不是一个好的方案）
* If the implementation is easy to explain, it may be a good idea.（如果一个实现方案易于理解，它很有可能是一个好的方案）
* Namespaces are one honking great idea -- let's do more of those!（命名空间非常有用，我们应当多加利用）

# FAQ

## 1. 注释

Python 中有两种形式的注释：

1. 单行注释：以 `#` 和空格开头，可以注释掉从 `#` 开始后面一整行的内容。
2. 多行注释：三个引号(`'''`/`"""`)开头，三个引号结尾，通常用于添加多行说明性内容。

但需要注意的是，python 中并没有对多行注释的内置支持，所谓通过三个引号实现多行注释的方法其实是使用了 docstrings。

当这种三引号字符串出现在特定位置时（比如模块开头、函数体第一行、类体第一行），它就会被当作 **docstring**：

```python
def add(a, b):
    """返回两个数的和。"""
    return a + b
```

这时候：

- 这个字符串会绑定到 `add.__doc__`
- `help(add)` 会显示这段文字
- 文档生成工具（如 Sphinx）也会用它

所以：

- **docstring 的定位是“对接口的说明文档”，面向使用者**
- **注释是写给开发者看的，解释实现细节、坑点，而不是给用户看的**

究其原理，所谓“三引号多行注释”其实只是下面这种代码：

```python
"""
这是一个字符串字面量
由于没有被赋值给变量
也没有作为表达式使用
所以在运行时它会被丢弃
"""
```

Python 解释器会创建这个字符串对象，然后立刻发现没人用它，之后它就会被当成“无用表达式”丢掉。

**这只是“恰巧能达到看起来像注释的效果”，并不是语法级的注释。**

因此，我们应该坚持使用 python 的单行注释，即便是应对多行，这是因为 <font color="blue">docstrings 是用来编写文档的，而不是用来注释代码的</font>。（这一点在集成环境对这两种注释的处理也可以看出，单行注释是灰色的，docstrings 注释是绿色的）

总而言之，当我们想表达注释时，应该使用 `#`，无论是单行还是多行。docstring 只应出现在函数、类和方法的开头，作为提供给使用者的帮助信息。

## 2. 缩进

不同于 C++、Java 等使用花括号 `{}` 明确界定代码块的语言，**Python 使用缩进作为语法结构的一部分来决定代码块的范围**。当某条语句以冒号 `:` 结尾时，它后的下一行必须增加缩进，而缩进层级一致的连续语句被视为同一个代码块。

缩进的宽度在语法上并没有强制规定，但 **PEP 8 推荐每一级缩进使用 4 个空格**。虽然 Tab 也是合法的，但不推荐混用空格与 Tab，否则可能触发 `IndentationError`。

此外，Python 允许使用分号 `;` 在一行中放置多条语句，例如：

```python
a = 1; b = 2; print(a + b)
```

但这种写法会降低可读性，因而在实际开发中并不推荐使用。

## 3. 基础内置类型

### 1. **数字类型（Number）**

```python
int      # 整数
float    # 浮点数
bool     # 布尔
complex  # 复数
```

- `int`: 任意精度整数
- `float`: 带小数点
- `bool`: True / False，本质是 int 的子类
- `complex`: 如 `1+2j`

**Python 的整数（`int`）**

- **任意精度**（unbounded precision）：只要内存允许，数值可以无限大，例如上千位、上万位的整数都可以。
- Python 会在需要时自动从机器整数转换为 **大整数（big integer）** 进行高精度计算。
- **支持多种进制字面量**：
  - 二进制：`0b...`
  - 八进制：`0o...`
  - 十进制：默认
  - 十六进制：`0x...`

与 C/C++ 不同，Python 不存在 `int32`、`int64` 的概念，不会溢出，也不需要手动处理。

**Python 的布尔型（`bool`）**

- 只有两个值：`True`、`False`

- 本质上是 `int` 的子类：

  ```python
  isinstance(True, int)  # True
  True == 1              # True
  False == 0             # True
  ```

> 高精度计算（大数计算）就是当整数的大小超过机器硬件能直接表示的范围后，使用<font color="blue">**软件算法**</font>来表示和运算这些超大数字。
>
> 不用固定数量的机器字节存储整数，而是用多段内存（类似一个数组）来存储数字的二进制位，然后用专门的算法做加减乘除。

### 2. **字符串（String）**

```python
str
```

- 不可变
- 用于文本：`"hello"`
- 支持切片、拼接、格式化等

计算机被设计之初是用来处理数值计算的，但发展至今，我们还需要处理文本数据，这就需要字符串了。

以三个引号开头的字符串可以拆行：

``` python
s = '''
hello,
world!
fuck,you!
'''
print(s)

# hello,
# world!
# fuck,you!

s = '''hello,
world!
fuck,you!'''
print(s)

# hello,
# world!
# fuck,you!
```

Python的字符串可以以 `R` 或 `r` 开头，这种字符串被称为原始字符串，意思是字符串中的每个字符都是它原本的含义，没有所谓的转义字符。

Python还允许在转义字符 `\` 后面跟一个八进制或者十六进制数来表示字符。例如 `\141` 和 `\x61` 都代表小写字母 `a`，前者是八进制的表示法，后者是十六进制的表示法。另外一种表示字符的方式是在 `\u` 后面跟Unicode字符编码，例如 `\u9a86\u660a` 代表的是中文“骆昊”。

字符串常用方法：

* `string.capitalize()`：字符串首字母大写
* `string.title()`：每个单词首字母大写
* `string.upper()`
* `string.lower()`
* `string.find(substring[, start, end])`：找到返回下标，找不到返回-1
* `string.index(substring[, start, end])`：找到返回下标，找不到引发异常
* `string.rfind()`
* `string.rindex()`
* `string.startwith()`
* `string.endwith()`
* `string.isdigit()`：字符串是否由数字构成
* `string.isalpha()`：字符串是否由字母构成
* `string.isalnum()`：字符串是由由字母和数字构成
* `string.center(width[, fillchar])`：居中对齐
* `string.ljust(width[, fillchar])`：居左对齐
* `string.rjust(width[, fillchar])`：居右对齐
* `string.zfill(width)`：左侧补0，直到字符串长度为width
* `string.encode(encoding=‘utf-8’,errors=‘strict’)`：将字符串编码为字节串(bytes)
* `string.decode(encoding=‘utf-8’,errors=‘strict’)`：将bytes解码为string
* `string.split(separator=‘ ’, maxsplit)`：依据separator将字符串拆分为一个list，可以规定最大拆分次数
* `string.join(iterable)`：The `join()` method takes all items in an iterable and joins them into one string.
* `string.replace(old, new, count)`
* `string.strip()`：修剪，用来去掉string左右两端的空格
* `string.lstrip()`
* `string.rstrip()`

字符串格式化：

* `%`

* `f`

* `format`

  ``` python
  a = 123
  b = 3
  c = 3.1415926
  d = "hello"
  
  print('%d * %d = %d' % (a, b, a * b))
  print('{0} * {1} = {2}'.format(a, b, a * b))
  print(f'{a} * {b} = {a * b}')
  
  print('%d, %.2f, %s' % (a, c, d))
  print('{0:d}, {1:.2f}, {2:s}'.format(a, c, d))
  print(f'{a:d}, {c:.2f}, {d:s}')
  ```

常用函数：

* `bool()`

* `int()`：将一个数值或字符串转换成整数，可以指定进制。

* `float()`：将一个字符串转换成浮点数。

* `str()`：将指定的对象转换成字符串形式，可以指定编码。

* `chr()`：将整数转换成该编码对应的字符串（一个字符）。

* `ord()`：将字符串（一个字符）转换成对应的编码（整数）。

### 3. **列表（List）**

```python
list
```

- 可变
- 有序结构，如 `[1, 2, 3]`
- 最常用的容器

链表是可变数据类型，字符串是不可变数据类型，其区别在于，当我们针对字符串增删时，是创建了一个新的字符串，原字符串没有发生变化。

注意这里说的字符串是不可变数据类型指的是不可以修改字符串的某个元素，例如 `str[0]='1'`，但是我们可以这样修改：`str='1'+str[1:]` 来实现同样的效果，其本质相当于重新创建了一个字符串，也就是说 Python 的字符串只是不支持原地修改，关于官方 Docs 的解释：

> 一个是性能：知道字符串是不可变的，意味着我们可以在创建时为它分配空间，并且存储需求是固定不变的。这也是元组和列表之间区别的原因之一。
>
> 另一个优点是，Python 中的字符串被视为与数字一样“基本”。 任何动作都不会将值 8 更改为其他值，在 Python 中，任何动作都不会将字符串 “8” 更改为其他值。

Python 中的列表底层是一个可以动态扩容的数组（类似 vector），可以随机访问，可以保存各种类型的数据

下标范围为 `[0,n-1]`，`[-n,-1]`

通过生成式创建列表，【强烈建议使用生成式语法来创建列表】，这是因为 Python 解释器的字节码指令中有专门针对生成式的指令（`LIST_APPEND` 指令）；而 for 循环是通过方法调用（`LOAD_METHOD`，`CALL_METHOD` 指令）的方式为列表添加元素，方法调用本身就是一个相对耗时的操作。

一种错误的创建的嵌套列表的方法：

``` python
matrix = [[0] * 3] * 5
print(matrix) 
# [[0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0], [0, 0, 0]]
matrix[0][0] = 3 
print(matrix)
# [[3, 0, 0], [3, 0, 0], [3, 0, 0], [3, 0, 0], [3, 0, 0]]
```

正确方法：

``` python
matrix = [[0] * 3 for _ in range(5)]
matrix[0][0] = 3
print(matrix)
```

错误的原因很简单，就是指针，先看下面例子：

``` python
list1 = [[0]]
list2 = list1 * 3
list1[0][0] = 1
print(list2)
# [[1], [1], [1]]
print(id(list1[0]))
print(id(list2[0]))
print(id(list2[1]))
print(id(list2[2]))
```

在这里我们明明修改的是 list1，怎么 list2 也变了？并且变的是所有元素

一句话解释清楚：针对 list 的 `*` 操作符在实现上是复制了值的 <font color="blue">引用</font>，而不是创建了一个新的对象，所以在上面例子中，list2 是 4 个指向同一个 list1 对象的引用

所以，`*` 操作符对于不可变对象很安全，但是对于可变对象，可能就不是你期望的结果了。

这里结合[这个可视化网站](https://pythontutor.com/visualize.html#mode=edit)会更清晰，最后，我们也可以通过 `id()` 打印对象的地址来进一步确认，毫无疑问，他们的地址是相同的。

那你可能会问，为什么下面的修改不行呢？

``` python
list1 = [0, "fuck"]
list2 = list1 * 3
list1[0] = 1
print(list2)
# [0, 'fuck', 0, 'fuck', 0, 'fuck']
```

这是因为，数字和字符串在 list 中就是以值的形式存在的，而嵌套 list 是作为指针指过去的，因此在复制嵌套 list 时，复制的是指针，在复制数字，字符串时，复制的就是值。

### 4. **元组（Tuple）**

```python
tuple
```

- 不可变的序列
- 如 `(1, 2, 3)`

tuple 不同于 list，它是 <font color="blue">不可变类型</font>，这意味着 tuple 的变量一旦定义，其中的元素不可以再添加或删除，元素的值也不可以修改。定义 tuple 通常使用 `()` 字面量语法。

`()` 表示空 tuple，但如果 tuple 中只有一个元素，需要加上一个 `,`，否则 `()` 就不是代表元组的字面量语法，而是改变算数优先级的圆括号。

tuple的应用场景：

1. 打包和解包：当我们把多个用逗号分隔的值赋给一个变量时，多个值会打包成一个元组类型；当我们把一个元组赋值给多个变量时，元组会解包成多个值然后分别赋给对应的变量。可以使用星号表达式来接受多个值，其类型为 list，不过注意其值能出现一次。打包的元素也可以是一个字符串，`range()` 等。
2. 交换多个变量的值：本质上也是通过打包操作进行的，但是对于两个和三个元素的交换，Python 有专门的字节码指令 `ROT_TWO` 和 `ROT_THREE` 进行优化。

从功能上看，tuple 就像是 list 的子集，那么 tuple 有什么 list 无法替代的好处呢？（其实主要是针对其不可变类型的特性）

1. 不可变类型更适合多线程环境，因为它降低了并发访问变量的同步化开销
2. 不可变类型在创建时间和占用空间上面都优于对应的可变类型

我们可以使用 `sys` 模块的 `getsizeof()` 来检查保存相同元素的 tuple 和 list 各自占用了多少内存空间。也可以使用 `timeit` 模块的 `timeit` 函数来检查创建保存相同元素的 tuple 和 list 各自花费的时间。

``` python
from sys import getsizeof
from timeit import timeit

n = 100000
a = list(range(n))
b = tuple(range(n))

print(getsizeof(a), getsizeof(b)) 
# 800056 800040
print(timeit('[1, 2, 3, 4, 5, 6, 7, 8, 9]')) 
# 0.034143834
print(timeit('(1, 2, 3, 4, 5, 6, 7, 8, 9)')) 
# 0.0043259999999999965
```

最后，python 中的 list 和 tuple 是可以相互转换的

### 5. **集合（Set）**

```python
set
```

- 可变集合
- 无序、元素唯一，如 `{1, 2, 3}`

注意与 C++ 的 set 进行区分，C++ 的 set 使用红黑树实现，有序，但是在 Python 中，集合元素之间是无序的，Python 的集合更像是 C++ 的 `unordered_set`，因此 Python 中的集合底层存储是哈希存储，故其取元素的时间复杂度为常数。也因此，集合中的元素必须是 `hashable` 类型（整数、浮点数、字符串、元组等），<font color=blue>通常不可变类型都是 `hashable` 类型，可变类型都不是 `hashable`类型。</font>

通过 `{}` 创建集合时，要确保集合中至少有一个元素，否则创建的是空字典而不是空集合，如果想要创建空集合，可以使用构造器 `set()`，另外也可以通过生成式创建集合对象：

``` python
s = {"hello"}  
print(s) # {'hello'}

s = set("hello") 
print(s) # {'e', 'h', 'l', 'o'}

s = set([1, 2, 2, 3, 4, 4]) 
print(s) # {1, 2, 3, 4}

s = {num for num in range(20) if num % 3 == 0 or num % 5 == 0} 
print(s) # {0, 3, 5, 6, 9, 10, 12, 15, 18}
```

集合运算：

* `set1 & set2`，`set1.intersection(set2)`：交集
* `se1 | set2`，`set1.union(set2)`：并集
* `set1 - set2`，`set1.difference(set2)`：差集
* `set1 ^ set2`，`set1.symmetric_difference(set2)`：对称差，等价于：`(set1 | set2) - (set1 & set2)`

比较运算：

* `==`，`!=`：两个集合中的元素是否完全相同
* `set1 <= set2`，`set1.issubset(set2)`：set1 是否是 set2 的子集
* `set1 < set2`：set1 是否是 set2 的真子集
* `set1 >= set2`，`set1.isupperset(set2)`：set1 是否是 set2 的超集

其他方法：

* `set.add(element)`：添加一个元素
* `set.update(set2)`：添加一个集合中的所有元素
* `set.discard(element)`：删除指定元素，不存在不会报错
* `set.remove(element)`：不存在会抛出KeyError异常
* `set.pop()`：从集合中随机删除一个元素并返回该元素
* `set.clear()`
* `set1.isdisjoint(set2)`：判断两个集合是否存在相同元素，若不存在返回True

<font color="blue">frozenset</font>：不可变 set，由于 frozenset 是不可变类型，能够计算出 hash code，因此它可以作为 set 中的元素。它跟 set 的区别就像 tuple 和 list 的区别。

### 6. **字典（Dictionary）**

```python
dict
```

- 键值对，如 `{"name": "Tom", "age": 18}`
- 键必须可哈希

如何创建字典：

``` python
# 1. 通过 {} 创建 dict
person = {
    'name': 'Fiona', 
    'age': 20,
}
print(person) # {'name': 'Fiona', 'age': 20}

# 2. 通过构造器创建 dict
person = dict(name='Steve', age=23)
print(person) # {'name': 'Steve', 'age': 23}

# 3. 通过 Python 内置函数 zip 压缩两个序列并创建 dict
items = dict(zip('ABC', '123'))
print(items) # {'A': '1', 'B': '2', 'C': '3'}


list1 = ('name', 'age')
list2 = ('Fiona', 23)
items = dict(zip(list1, list2))
print(items) # {'name': 'Fiona', 'age': 23}

# 4. 通过生成式语法创建 dict
items = {x: x ** 3 for x in range(1, 3)}
print(items) # {1: 1, 2: 8, 3: 27}
```

需要注意的是，`for-in` 循环只是获取到了字典的 key

字典的 key 必须是 **不可变类型**

字典的方法：

* `dict.get(key)`：get value，return none if not exist
* `dict.keys()`：获取所有key
* `dict.values()`：获取所有value
* `dict.items`：获取所有键值对
* `for key, value in dict.items()`
* `dict.pop(key)`：若 key 不存在返回 KeyError
* `dict.popitems()`：删除最后一个键值对并返回该值（添加顺序？）
* `dict.setdefault(key，value)`：若 key 存在，返回其值，不做修改；若 key 不存在，返回 value，并添加直 dict
* `dict.update(dict)`：有则修改，无则添加
* `del dict[key]`：key 不存在抛出 KeyError 异常

### 7. **字节序列（Bytes 类）**

```python
bytes        # 不可变
bytearray    # 可变
```

- 用于二进制数据，如文件、网络数据

### 8. **None 类型**

```python
NoneType   # 只有一个值：None
```

- 表示“空”“无”
- 唯一值 None 是一个关键字
- 函数无返回值时的默认返回

## 4. 内置运算符

``` python
# 1. 切片 sequence[start:stop:step]
# 左开右闭区间 [start,stop)
s = "01234"
print(s[1:4])     # 123
print(s[:3])      # 012
print(s[3:])      # 34
print(s[::2])     # 024
print(s[::-1])    # 43210

# 2. 指数 **
print(2 ** 10) # 1024

# 3. 整除 //
print(7 // 2) # 3
print(7 / 2)  # 3.5

# 4. 身份运算符
# 比较两个变量是否引用同一对象（同一内存地址）
x = [1, 2, 3]
y = x
z = [1, 2, 3]
print(type(x), type(y), type(z))
print(x is y) # True
print(x is z) # False
print(id(x), id(y), id(z))

# 5. 成员运算符 in, not in
print('a' in 'abc') # True 
print('z' in 'abc') # False

print(2 in [1, 2, 3])   # True
print(4 not in [1, 2, 3]) # True

dictionary = {"name": "Tom", "age": 18}
print("name" in dictionary)      # True（检查键）
print("Tom" in dictionary)       # False（值不是键）

# 6. 逻辑运算符 not, or, and
# 注意短路求值特性

# 7. 序列复制运算符 *
# 注意浅拷贝问题
print([1] * 5)         # [1, 1, 1, 1, 1]
print("ab" * 3)        # ababab
print((1, 2) * 2)      # (1, 2, 1, 2)

a = [[]] * 3
a[0].append(1)
print(a)     # [[1], [1], [1]]（三个子列表共享一个对象）
print(id(a[0]), id(a[1]), id(a[2]))

# 8. 逗号：构造元组
a = 1, 2, 3
print(type(a))
print(a)
```

## 5. with

对于正确的处理涉及到异常的资源管理时，需要使用 `try/finally` 代码结构，这样的结构一多会导致整体代码结构 **很臃肿繁琐，不易读、不美观**，因此在 *Python2.6* 版本推出 `with` 关键字。

`with-as` 语句是 `Pyhton` 提供的一种简化语法，适用于对资源进行访问的场合，确保不管使用过程中是否发生异常都会执行必要的**清理操作**（系统会自动调用`f.close()`），释放资源。

例如：

``` python
import threading

lock = threading.Lock()
counter = 0

def worker():
    global counter 
    for _ in range(10000):
        with lock:
            counter += 1

threads = [threading.Thread(target=worker) for _ in range(5)]

for t in threads:
    t.start()

for t in threads:
    t.join()

print("counter: ", counter)
```

但并不是所有的对象都可以放在 `with` 上下文语法中，只有符合**上下文管理器协议**（有 `__enter__` 和 `__exit__` 魔术方法）的对象才能使用这种语法。

- `__enter__(self)`：
  - 在进入 `with` 块之前被调用
  - `with ... as x` 中的 `x` 就是 `__enter__` 的返回值
- `__exit__(self, exc_type, exc_val, exc_tb)`：
  - 在离开 `with` 块时调用
  - 无论是正常退出还是发生异常都会被调用
  - 可以选择是否“吞掉”异常（返回 True 时异常不会继续向外抛）

执行流程类似于：

```python
cm = ContextManager()           # 创建上下文管理器对象
value = cm.__enter__()          # 进入上下文
try:
    # with 块中的代码
    ...
finally:
    cm.__exit__(exc_type, exc_val, exc_tb)  # 离开上下文，执行清理
```

据此，我们可以自定义上下文管理器：

``` python
import time 

class Timer:
    def __enter__(self):
        print('__enter__ start')
        self.start = time.time()
        print('__enter__ end')
        return self 
    def __exit__(self, exc_type, exc_val, exc_tb):
        print('__exit__ start')
        self.end = time.time()
        print(f'__exit__ end, time used: {self.end - self.start:.4f}s')

def main():
    print('main start')
    with Timer() as t:
        print('with start')
        sum(range(10_0000))
        print('with end')
    print('main end')

if __name__ == "__main__":
    main()
```

## 6. yield 与 Generator

<font color=red>1. 什么是生成器对象（Generator Object）</font>

生成器对象是由 **生成器函数** 创建的特殊对象。

当一个函数内部包含 `yield` 关键词时，它就变成一个<font color="blue">生成器函数</font>，调用该函数不会立即执行，而是返回一个<font color="blue">生成器对象</font>。

生成器对象是一种 **<font color="blue">可迭代对象（Iterable）</font>**，用于 **逐步生成值**，而不是一次性生成全部数据。

<font color=red>2. 生成器的核心特点</font>

2.1 **惰性求值（Lazy Evaluation）**

生成器不会一次性产生所有数据，而是每次被迭代时才生成下一项，能显著 **节省内存**，特别适用于大数据量。

2.2 **状态保持（Stateful）**

生成器在执行到 `yield` 时暂停，并保留：

- 当前执行位置
- 局部变量的值
- 控制流状态

下一次再调用时，从上一次暂停的地方继续执行。

2.3 **可迭代性（Iterable）**

生成器可以被：

- `next()` 访问
- `for` 循环使用
- 各种依赖可迭代对象的位置使用（如 list(...)）

当生成器执行完毕后，会抛出 `StopIteration` 异常，表示迭代结束。

<font color=red>3. `yield` 的双重功能</font>

1. **返回一个值** → 暂停函数执行
2. **接收一个值** → 当外部使用 `send()` 时，`yield` 可以接收到传入值

因此，`yield` 不仅用于生成数据，还能实现 **<font color="blue">双向通讯</font>**。

可见，yield 本质上是一个“暂停点”，既可以返回数据，也可以接收数据。

<font color=red>4. `send()` 方法</font>

生成器还能通过 `send()` 接收外界的输入。

**使用规则：**

1. **启动生成器（预激活）**
   - 必须使用 `next(gen)` 或 `gen.send(None)` 来启动生成器，才能让 `yield` 开始接收实际的值。
2. **发送数据**
   * `gen.send(value)` 会将 `value` 传给生成器，使得 `yield` 表达式接收到这个值，同时继续执行到下一个 `yield`。
3. **返回值**
   *  `send()` 返回下一个 `yield` 产出的值。

例如：

``` python
def get_average():
    total, count = 0, 0
    average = None 
    while True:
        current = yield average 
        total += current
        count += 1
        average = total / count

gen = get_average()
print('start: ', gen.send(None)) # 启动生成器
while True:
    print('avg: ', gen.send(float(input('pls input a number: '))))
```

注意 `current = yield average` 这条语句，你可能知道，它接受 `current` 并返回 `average`，但这两者之间是有顺序的，具体的：

1. 先把 `average` 作为生成器的产出返回给外部（`yield average`）
2. 当外部下次调用 `gen.send(x)` 时，`yield` 表达式的值才会变成 `x`，也就是赋给 `current`

因为生成器函数在执行到 `yield` 时会暂停，返回 `yield` 右边的值；下一次 `.send(x)` 时才会把 `x` 作为 `yield` 表达式的结果继续执行。

<font color=red>5. range 与 xrange</font>

在 python2 中

| 函数       | 返回类型     | 特点                       |
| ---------- | ------------ | -------------------------- |
| `range()`  | 列表         | 一次性生成全部数据，耗内存 |
| `xrange()` | 生成器式对象 | 惰性生成，节省内存         |

在 python3 中：

- `xrange()` 被移除
- `range()` 自动改为惰性对象（行为类似 Python2 的 `xrange()`）

<font color=red>6. 生成器的核心价值</font>

1. **节省内存（惰性生成）**
2. **保留运行状态**
3. **可暂停 + 可恢复**
4. **yield 支持双向通讯（send）**
5. 可实现复杂的迭代协议与轻量级协程行为

## 7. 函数

<font color=red>1. 一等公民</font>

在 Python 中，函数是一等公民（first-class citizen），也就是地位非常高的意思，也即它具有和其他实体（变量、对象等）同样的地位和权力。体现在：

* 函数可以作为参数传递
* 作为返回值返回
* 赋值给变量或存储在数据结构中。

这样的特性提供了大量的灵活性和动态性，特别是在更高阶的函数编程中。

<font color=red>2. 位置参数和命名关键字参数</font>

在没有特殊处理的情况下，函数的参数都是位置参数，如果我们希望调用函数时调用者必须以 `参数名=参数值` 的方式传递参数，可以使用命名关键字参数（必须以命名的形式给我）。

所谓命名关键字参数，就是写为 `*` 之后的参数，即，以 `*` 为<font color="blue">**分隔符**</font>（`*args` 也可以），左侧是位置参数，右侧是命名关键字参数：

``` python
def func(x, y, z, *, a, b, c):
    print(f'keyword-only arguments: a = {a}, b = {b}, c = {c}')
    print(f'positional arguments: x = {x}, y = {y}, z = {z}')

func(11, 22, 33, a = 1, b = 2, c = 3)
```

位置参数必须出现在关键字参数之前，通过可变参数我们也能理解这样规定的意义。

<font color=red>3. 可变参数</font>

位置参数和命名关键字参数分别有其可变参数形式，其中称 `*args` 为可变位置参数，`**ksargs` 为可变关键字参数

```python
def func(*args, **kwargs):
    print(type(args))   # <class 'tuple'>
    for arg in args:
        print(arg)
    print(type(kwargs)) # <class 'dict'>
    for key, value in kwargs.items():
        print(f'{key}:{value}')

func(1, 2, 3, a = 1, b = 2, c = 3)
```

> 上面这种写法可以接受任意多个，任意形式的参数，特别适用于在你不知道参数签名时

<font color=blue>**其中可变位置参数被处理为 tuple，可变关键字参数被处理为 `dict`。**</font>

> `*args` 用 `tuple`，不是因为“不能用 list”，而是因为：
>
> - 参数传进来后语义上是“只读的一组值”
> - CPython 内部本来就把它们打包成 tuple
> - tuple 更轻、更快、更符合“不要乱改参数”的设计习惯

注意我们上面提到的，`*` 是位置参数和关键字参数的分隔符，因此当出现`*args` 之后，它后面的参数都必须是以命名关键字参数形式传递：

``` python
def calc(*args, init_value, op, **kwargs):
    result = init_value
    for arg in args:
        if type(arg) in (int, float):
            result = op(result, arg)
    for value in kwargs.values():
        if type(value) in (int, float):
            result = op(result, value)
    return result

def add(a, b):
    return a + b

res = calc(1, 2, 3, init_value = 0, op = add, a = 1, b = 2, c = 3)
print(res)

# 如果我们写为以下形式：
res = calc(1, 2, 3, 0, add, a = 1, b = 2, c = 3)
# TypeError: calc() missing 2 required keyword-only arguments: 'init_value' and 'op'
```

位置参数必须出现在所有关键字形式参数之前；而关键字参数之间的顺序无所谓。

<font color=red>4. lambda</font>

Python 里的 `lambda` 可以理解成：**一行写完的小函数**。

 它有名字也可以没名字，但本质就是“匿名函数”。

基本语法：

```python
lambda 参数列表: 表达式
```

相当于：

```python
def 函数名(参数列表):
    return 表达式
```

但 `lambda`：

- 只能写 **一个表达式**
- 不写 `return`，表达式的值就是返回值

例如：

```python
add = lambda x, y: x + y
print(add(2, 3))  # 5
```

等价于：

```python
def add(x, y):
    return x + y
```

例1：配合 `sorted` / `list.sort` 指定排序规则

```python
students = [
    ('Alice', 20),
    ('Bob', 18),
    ('Charlie', 22),
]

# 按年龄排序
students.sort(key=lambda s: s[1])
print(students)
# [('Bob', 18), ('Alice', 20), ('Charlie', 22)]
```

`key=lambda s: s[1]` 就是“告诉排序函数：拿每个元素的第 2 项当作排序依据”。

例2：配合 `map` / `filter`

```python
nums = [1, 2, 3, 4, 5]

# map：对每个元素平方
squares = list(map(lambda x: x * x, nums))
print(squares)  # [1, 4, 9, 16, 25]

# filter：过滤出偶数
evens = list(filter(lambda x: x % 2 == 0, nums))
print(evens)  # [2, 4]
```

例3：当成简单回调函数

比如 GUI / 事件回调 / 函数型参数之类：

```python
def operate(x, y, func):
    return func(x, y)

print(operate(3, 5, lambda a, b: a + b))  # 8
print(operate(3, 5, lambda a, b: a * b))  # 15
```

<font color=red>5. 装饰器</font>

装饰器是 Python 中用一个函数装饰另外一个函数或类并为其提供额外功能的语法现象。<font color="blue">装饰器本身是一个函数，它的参数是被装饰的函数或类，它的返回值是一个带有装饰功能的函数。</font>

``` python
import random
import time

def download(filename):
    print(f'start install {filename}.')
    time.sleep(random.randint(2, 3))
    print(f'{filename} install finished.')

# 装饰器函数
def record_time(func):
    # 定义一个带装饰功能的函数
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f'{func.__name__} run：{end - start:.3f}s.')
    # 返回带装饰功能的warpper函数
    return wrapper

# 手动得到装饰后的函数
download = record_time(download)
download('C++ Prime')
```

在 Python 中，使用装饰器有更为便捷的语法糖， 可以用 `@装饰器函数` 将装饰器直接放在被装饰的函数上，效果相同。

``` python
import random
import time

# 装饰器函数
def record_time(func):
    # 定义一个带装饰功能的函数
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f'{func.__name__} run：{end - start:.3f}s.')
    # 返回带装饰功能的warpper函数
    return wrapper

@record_time # 自动装饰
def download(filename):
    print(f'start install {filename}.')
    time.sleep(random.randint(2, 3))
    print(f'{filename} install finished.')

# download = record_time(download)
download('C++ Prime')
```

如果希望取消装饰器的作用，那么在定义装饰器函数的时候，需要做一些额外的工作。Python 标准库 `functools` 模块的 `wraps` 函数也是一个装饰器，我们将它放在 `wrapper` 函数上，这个装饰器可以帮我们保留被装饰之前的函数，这样在需要取消装饰器时，可以通过被装饰函数的 `__wrapped__` 属性获得被装饰之前的函数。

``` python
import random
import time

from functools import wraps

def record_time(func):
    @wraps(func) # 该装饰器可以保留被装饰之前的函数
    def wrapper(*args, **kwargs):
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print(f'{func.__name__} run：{end - start:.3f}s.')
    return wrapper

@record_time 
def download(filename):
    print(f'start install {filename}.')
    time.sleep(random.randint(2, 3))
    print(f'{filename} install finished.')

# 自动装饰
download('C++ Prime')

# 取消装饰
download.__wrapped__('insight Java')

# 取消装饰
download = download.__wrapped__
download('Mysql')

```

最后就是，装饰器本身也可以参数化（层层装饰）。

## 8. package 和 module

<font color=red>1. 什么是 module（模块）？</font>

**模块（module）就是一个 `.py` 文件**， 每个 Python 文件都天然是一个模块。并且模块名来自文件名（去掉 `.py` 后缀），这样导入逻辑才统一、简洁、可预测。

模块的作用：

- 封装代码（函数、类、变量）
- 避免命名冲突（不同文件有不同的命名空间）
- 便于复用

你可以导入一个模块：

```python
import math
```

或者导入模块内部的属性：

```python
from math import sqrt
```

注意，如果我们从两个不同的模块中引入了同名的函数，则 <font color="blue">后引入的函数会覆盖掉先前的引入</font>，为了解决这个问题，可以使用 <font color="blue">别名</font> 进行区分。

<font color=red>2. module 的本质是什么？</font>

**模块本质上是一个 python 对象**。 Python 运行时创建一个 `module` 类型的对象，并把模块里的变量都放进去。

例如：

```python
import math
print(type(math))
```

输出：

```python
<class 'module'>
```

这意味着：

- 模块是个普通对象
- 可以动态添加属性
- 可以保存状态

比如你可以这样做：

```python
import math
math.x = 123
print(math.x)
```

模块是“运行时命名空间”，和 C++ 的 namespace 不同。

<font color=red>3. 什么是 package（包）？</font>

**包（package）是一个包含多个模块（或子包）的文件夹。**

它必须包含一个 `__init__.py` 文件，才能被 Python 识别为包。

例如目录结构：

```
mypkg/
    __init__.py
    a.py
    b.py
    subpkg/
        __init__.py
        c.py
```

`mypkg` 就是一个包，里面有：

- 模块：
   `a.py`、`b.py`
- 子包：
   `subpkg/`
- 初始化文件：
   `__init__.py`

<font color=red>4. package 的本质是什么？</font>

**包也是一个模块，只不过它的代码来自一个目录，而不是一个文件。**

当你导入包：

```python
import mypkg
```

Python 会执行：

- `mypkg/__init__.py`
- 并创建一个 module 对象 `mypkg`

里面可以写代码：

```python
# mypkg/__init__.py
print("mypkg package initialized")
```

> module 是最小的代码组织单元（一个文件）。
>
> package 是模块的集合（一个含 init 文件的文件夹）。
>
> 两者在运行时都是 module 对象，只是来源不同。

<font color=red>5. 导入模块与包的规则（import 机制详解）</font>

Python 有一套复杂但非常统一的 import 查找机制：

导入发生时，Python 会按以下顺序查找：

1. **内置模块**（比如 `sys`, `math`）
2. 当前工作目录（脚本所在目录）
3. 环境变量 `PYTHONPATH` 指定的路径
4. site-packages（第三方库）

<font color=red>6. 如何从模块和包中导入</font>

假设目录结构如下：

```
project/
    main.py
    mypkg/
        __init__.py
        a.py
        b.py
```

导入包

```python
import mypkg
```

导入模块

```python
import mypkg.a
```

导入模块中的函数

```python
from mypkg.a import foo
```

给包/模块取别名

```python
import mypkg.a as mod
from mypkg.a import foo as f
```

<font color=red>7. `__init__.py` 的作用</font>

`__init__.py` 文件让文件夹变成一个 package，并且具有以下作用：

作用 1：定义包级别的全局变量

```python
# mypkg/__init__.py
version = "1.0"
```

作用 2：导入子模块，简化包接口

例如：

```python
# __init__.py
from .a import foo
```

用户就可以直接：

```python
from mypkg import foo
```

作用 3：包初始化逻辑

```python
print("mypkg loaded")
```

<font color=red>8. 相对导入与绝对导入</font>

**绝对导入（Absolute Import）**

- 使用完整的模块路径进行导入。
- 从<font color="blue">项目的根目录</font>（即 `sys.path` 中包含的路径）开始查找。
- 更清晰、明确，推荐在大多数情况下使用。

**示例：**

假设项目结构如下：

```
myproject/
│
├── main.py
└── pkg/
    ├── __init__.py
    ├── mod1.py
    └── subpkg/
        ├── __init__.py
        └── mod2.py
```

在 `mod2.py` 中想导入 `mod1.py`，可以这样写（绝对导入）：

```python
from pkg import mod1
# 或
from pkg.mod1 import some_function
```

**相对导入（Relative Import）**

- 使用当前模块的位置作为参考点，用点号（`.`）表示层级。
- 只能在**包内部**使用（即必须在带有 `__init__.py` 的目录中的模块里）。
- 不能在直接运行的脚本中使用（会报错 `ImportError: attempted relative import with no known parent package`）。

**语法：**

- `.` 表示当前包
- `..` 表示上一级包
- `...` 表示上两级，以此类推

**示例：**

还是上面的结构，在 `subpkg/mod2.py` 中导入 `mod1.py`：

```python
from .. import mod1          # 导入同级 pkg 下的 mod1
# 或
from ..mod1 import some_function
```

**使用注意事项**

| 项目                   | 绝对导入             | 相对导入           |
| ---------------------- | -------------------- | ------------------ |
| 可读性                 | 高（路径清晰）       | 低（需理解包结构） |
| 可移植性               | 高（不依赖当前位置） | 低（依赖包层级）   |
| 是否可在顶层脚本中使用 | ✅ 是                 | ❌ 否               |
| Python 推荐方式        | ✅ 推荐               | ⚠️ 谨慎使用         |

> <font color=blue>**PEP 8 建议**：优先使用绝对导入；仅当处理复杂的包结构、避免冗长路径时才考虑相对导入。</font>

常见错误：`ImportError: attempted relative import with no known parent package`

**原因**：你直接运行了一个包含相对导入的 `.py` 文件（如 `python mod2.py`），此时 Python 不知道该文件属于哪个包。

**解决方法**：

- 通过 `-m` 参数运行模块（确保在包外执行）：

  ```python
  python -m pkg.subpkg.mod2
  ```

- 或者改用绝对导入。

**建议：**

* **不要习惯性地“进入脚本所在目录再运行”**。
* **始终从项目根目录出发，用 `python -m 包名` 的方式运行**，能避免绝大多数导入问题，使项目结构更清晰、可维护性更高。

<font color=reD>9. 常见误区</font>

<u>误区 1：认为“包就是文件夹”。</u>

不完全对：

*  **必须包含 `__init__.py` 才是传统意义上的包。**

（Python 3.3+ 引入 namespace package 后更灵活，但初学者先不用管）

<u>误区 2: think module = namespace only</u>

不对：

* module 是可执行的，可以存状态。

例如：

```python
# a.py
x = 0
def inc():
    global x
    x += 1
import a
a.inc()
print(a.x)  # 1
```

module 自己保存状态，这点和 namespace 完全不同。

<u>误区 3：认为 `import` 是“复制代码”</u>

不对：

* import 只加载一次，是缓存的（sys.modules）

## 9. 标准库

<font color=red>1. 常用标准库模块（Standard Library Modules）</font>

| 模块          | 主要用途                 | 补充说明                                                     |
| ------------- | ------------------------ | ------------------------------------------------------------ |
| `random`      | 生成伪随机数             | 常用函数：`random()`, `randint(a, b)`, `choice(seq)`, `shuffle(x)`, `sample(population, k)`, `choices(population, k=1)` |
| `time`        | 时间相关操作             | 注意：高精度计时推荐用 `time.perf_counter()`；格式化时间用 `strftime` / `strptime` |
| `math`        | 数学函数                 | 如 `sqrt`, `sin`, `log`, `ceil`, `floor` 等；不支持复数（复数用 `cmath`） |
| `functools`   | 高阶函数和可调用对象工具 | 常用：`@lru_cache`, `partial`, `reduce`（注意：`reduce` 在 Python 3 中已移入此模块） |
| `base64`      | Base64 编码/解码         | 常用于网络传输或嵌入二进制数据到文本中                       |
| `collections` | 扩展容器类型             | 常用：`defaultdict`, `Counter`, `deque`, `namedtuple`, `OrderedDict`（Python 3.7+ dict 已有序） |
| `hashlib`     | 安全哈希与消息摘要       | 支持 `md5`, `sha1`, `sha256` 等；注意 `md5/sha1` 不再安全，仅用于校验 |
| `heapq`       | 堆队列算法（最小堆）     | 实现优先队列；`heappush`, `heappop`, `heapify`               |
| `itertools`   | 迭代器工具               | 高效组合迭代：`chain`, `product`, `permutations`, `combinations`, `cycle`, `islice` |
| `uuid`        | 生成唯一标识符           | `uuid4()` 最常用（随机 UUID）；`uuid1()` 基于时间+MAC 地址   |
| `os.path`     | 路径操作（跨平台）       | 推荐改用 `pathlib.Path`（Python 3.4+ 更现代）                |

> **提示**：`os.path` 是函数式风格，而 `pathlib` 是面向对象风格，后者更简洁：
>
> ```python
> from pathlib import Path
> p = Path("data") / "file.txt"
> if p.exists(): ...
> ```

<font color=red>2. 常用内置函数（Built-in Functions）</font>

| 函数                                              | 功能                                     | 注意事项                                                     |
| ------------------------------------------------- | ---------------------------------------- | ------------------------------------------------------------ |
| `abs(x)`                                          | 返回绝对值                               | 支持 `int`, `float`, `complex`（返回模）                     |
| `bin(x)`                                          | 转为二进制字符串（`'0b1010'`）           | 仅适用于整数                                                 |
| `oct(x)`                                          | 转为八进制字符串（`'0o12'`）             | 仅适用于整数                                                 |
| `hex(x)`                                          | 转为十六进制字符串（`'0xa'`）            | 仅适用于整数                                                 |
| `chr(i)`                                          | Unicode 码位 → 字符                      | 如 `chr(97) == 'a'`                                          |
| `ord(c)`                                          | 字符 → Unicode 码位                      | 如 `ord('a') == 97`                                          |
| `input([prompt])`                                 | 读取用户输入（返回字符串）               | 在脚本中慎用；生产环境多用命令行参数或配置文件               |
| `open(file, mode='r', ...)`                       | 打开文件                                 | 推荐使用 `with open(...) as f:` 确保自动关闭                 |
| `round(number[, ndigits])`                        | 四舍五入                                 | 注意：Python 使用“银行家舍入”（四舍六入五成双）（`round(2.5) == 2`，`round(3,4) == 4`） |
| `id(obj)`                                         | 返回对象唯一标识（CPython 中是内存地址） | 用于判断是否为同一对象（`is` 的底层依据）                    |
| `map(func, *iterables)`                           | 映射函数到可迭代对象                     | 返回 **iterator**（Python 3），需 `list(map(...))` 转换      |
| `filter(func, iterable)`                          | 过滤元素                                 | 同样返回 iterator；`func` 返回 `True` 则保留                 |
| `functools.reduce(func, iterable[, initializer])` | 累积归约                                 | 需从 `functools` 导入；类似 `(((a+b)+c)+d)`                  |
| `all(iterable)`                                   | 所有元素为真则返回 `True`                | 空序列返回 `True`（逻辑上“全满足”）                          |
| `enumerate(iterable, start=0)`                    | 添加索引                                 | 常用于 `for i, x in enumerate(lst):`                         |

<font color=red>3. 关于 `random.sample` vs `random.choices` 的补充</font>

| 函数                                            | 抽样方式       | 是否允许重复 | 典型用途                     |
| ----------------------------------------------- | -------------- | ------------ | ---------------------------- |
| `random.sample(population, k)`                  | **无放回抽样** | 不重复       | 抽奖、洗牌、采样             |
| `random.choices(population, weights=None, k=1)` | **有放回抽样** | 可重复       | 模拟概率分布、生成随机字符串 |

例如：

```python
import random
letters = 'abcde'

# 无放回：3 个不重复字符
print(random.sample(letters, 3))      # ['c', 'a', 'e']

# 有放回：3 个可能重复字符
print(random.choices(letters, k=3))   # ['b', 'b', 'a']
```

> ⚠️ 注意：`sample` 要求 `k <= len(population)`，否则抛出 `ValueError`。

<font color=red>4. 建议与最佳实践</font>

1. **避免混淆 `oct` 的描述**：它是**八进制**，不是二进制。
2. **`reduce` 不再是内置函数**：Python 3 中必须 `from functools import reduce`。
3. **优先使用 `pathlib` 替代 `os.path`**：代码更简洁、可读性更强。
4. **不要滥用 `input()`**：在自动化脚本或服务中应通过参数或配置传入数据。
5. **理解 `round` 的舍入规则**：不是传统四舍五入，而是“向最近偶数舍入”。

## 10. 魔法

在 Python 中，“**魔法**”（Magic）并不是指真正的“神秘力量”，而是一种**约定俗成的术语**，用来描述那些**由 Python 解释器自动调用、具有特殊行为的内置方法或属性**。它们通常以**双下划线开头和结尾**（如 `__init__`、`__str__`），因此也被称为 **“dunder”**（double underscore 的缩写）。

> 通过魔法我们可以用于 **内省(introspect)** 和 **运行时元数据获取**。

**<font color=red>1. “魔法”的核心含义</font>**

**“魔法” = Python 提供的、用于定制对象行为的特殊接口**

当你定义一个类并实现某些“魔法方法”时，你的对象就能：

- 像内置类型一样工作（如 `len(obj)`、`obj + other`）
- 参与 Python 的核心协议（如迭代、上下文管理、布尔判断等）
- 被解释器在特定时机自动调用（无需手动调用）

这使得 Python 具有极强的**可扩展性**和**一致性**——用户自定义类型可以表现得和内置类型（如 `list`、`dict`）一样自然。

<font color=red> **2.1魔法方法（Magic Methods / Dunder Methods）**</font>

用于**定义对象的行为**，可被重写（override）。

| 魔法方法                 | 触发方式                 | 用途                 |
| ------------------------ | ------------------------ | -------------------- |
| `__init__(self, ...)`    | `MyClass()`              | 初始化实例           |
| `__str__(self)`          | `str(obj)`, `print(obj)` | 用户友好字符串表示   |
| `__repr__(self)`         | `repr(obj)`, 调试        | 开发者友好、精确表示 |
| `__len__(self)`          | `len(obj)`               | 定义“长度”           |
| `__getitem__(self, key)` | `obj[key]`               | 支持索引访问         |
| `__call__(self, ...)`    | `obj()`                  | 使对象可调用         |
| `__add__(self, other)`   | `a + b`                  | 自定义加法           |
| `__enter__` / `__exit__` | `with obj:`              | 上下文管理器         |

示例：

```python
class Vector:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y)
    def __str__(self):
        return f'Vector({self.x},{self.y})'

v1 = Vector(1, 2)
v2 = Vector(3, 4)
print(v1 + v2) # Vector(4,6)
```

<font color=red>**2.2 魔法属性（Magic Attributes / Dunder Attributes）**</font>

由解释器**自动设置**，用于存储**元信息**（metadata），通常**只读**。

| 属性         | 含义                                   |
| ------------ | -------------------------------------- |
| `__name__`   | 模块/函数/类的名称                     |
| `__file__`   | 模块文件路径                           |
| `__doc__`    | 文档字符串（docstring）                |
| `__module__` | 定义所在的模块名                       |
| `__class__`  | 对象所属的类                           |
| `__dict__`   | 对象的属性字典（除非用了 `__slots__`） |

示例：

```python
def greet():
    '''say hello'''
    pass 

print(greet.__name__)
print(greet.__doc__)
print(greet.__module__)
print(greet.__class__)
print(greet.__dict__)

# greet
# say hello
# __main__
# <class 'function'>
# {}
```

**<font color=reD>3. 为什么叫“魔法”？</font>**

- 因为这些方法/属性**不是你主动调用的**，而是 Python 在背后“悄悄”调用。
- 它们让对象具备了“神奇”的能力（比如能被 `+` 运算、能放进 `for` 循环、能用 `with` 管理）。
- 但其实**没有真正的魔法**——一切都是明确的协议和接口，只是对用户透明。

> 📌 **Python 之禅（Zen of Python）说**：
>
> *  *“Explicit is better than implicit.”*
>
>  所以这些“魔法”其实是**显式的协议**，而非隐藏机制。

**<font color=red>4. 注意事项</font>**

1. **不要自己发明 dunder 名称**
    如 `__my_custom_method__` —— 这是保留给 Python 未来的，可能冲突。
2. **魔法方法 ≠ 私有方法**
    私有方法用单下划线 `_private` 或双下划线 `__private`（名称修饰），和 dunder 无关。
3. **魔法方法应保持语义一致**
    比如 `__len__` 应返回非负整数，`__bool__` 应返回 `True/False`。

**<font color=red>5. 反射</font>**

Python 中的“魔法”（特别是魔法属性）与**反射（Reflection）**密切相关，甚至可以说：Python 的反射能力在很大程度上依赖于这些魔法属性和内置机制。

**反射** 是指程序在运行时能够：

- 检查对象的类型、属性、方法
- 动态获取或设置属性
- 调用方法（即使事先不知道名字）
- 判断对象是否具有某种能力

Python 是一门**高度动态、自省性强**的语言，其反射能力非常强大。

魔法属性如何支持反射？

以下魔法属性是反射的核心工具：

| 魔法属性          | 反射用途                                |
| ----------------- | --------------------------------------- |
| `__dict__`        | 获取/设置对象的所有实例属性（字典形式） |
| `__class__`       | 获取对象的类                            |
| `__name__`        | 获取函数、类、模块的名称                |
| `__module__`      | 获取定义所在的模块                      |
| `__bases__`       | 获取类的父类                            |
| `__mro__`         | 获取方法解析顺序（用于理解继承）        |
| `__annotations__` | 获取类型注解                            |
| `__doc__`         | 获取文档字符串                          |

反射常用内置函数（底层依赖魔法属性）

这些函数在内部大量使用魔法属性：

| 函数                            | 作用                   | 底层依赖                                   |
| ------------------------------- | ---------------------- | ------------------------------------------ |
| `getattr(obj, name[, default])` | 获取属性               | 访问 `obj.__dict__`、类属性、描述符等      |
| `setattr(obj, name, value)`     | 设置属性               | 修改 `obj.__dict__` 或触发 `__setattr__`   |
| `hasattr(obj, name)`            | 判断是否有属性         | 调用 `getattr` 并捕获异常                  |
| `dir(obj)`                      | 列出对象所有属性和方法 | 合并 `__dict__`、`__class__`、MRO 中的属性 |
| `isinstance(obj, cls)`          | 类型检查               | 使用 `__class__` 和 MRO                    |
| `issubclass(A, B)`              | 判断继承关系           | 使用 `__bases__` 和 MRO                    |

示例：用魔法属性实现反射

```python
class Person:
    species = "Homo sapiens"  # 类属性
    
    def __init__(self, name):
        self.name = name      # 实例属性
    
    def greet(self):
        return f"Hello, I'm {self.name}"

p = Person("Alice")

# 1. 获取对象的类
print(p.__class__)  # <class '__main__.Person'>
print(p.__class__.__name__)  # 'Person'

# 2. 查看实例属性 
print(type(p.__dict__))      # <class 'dict'>
print(p.__dict__)            # {'name': 'Alice'}

# 3. 查看类属性和方法
print(dir(Person))           # 包含 'greet', 'species' 等

# 4. 动态调用方法（反射）
method_name = "greet"
if hasattr(p, method_name):
    method = getattr(p, method_name)
    print(method())          # Hello, I'm Alice

# 5. 动态设置属性
setattr(p, "age", 30)
print(p.age)                 # 30
print(p.__dict__)            # {'name': 'Alice', 'age': 30}
```

魔法方法也能参与反射行为

虽然魔法方法本身通常不直接用于反射，但它们**定义了对象在反射操作中的表现**：

- `__getattr__` / `__getattribute__`：控制 `getattr()` 的行为
- `__setattr__`：控制 `setattr()` 的行为
- `__dir__`：自定义 `dir()` 的返回结果

```python
class DynamicObj:
    def __getattr__(self, name):
        return f"Attribute '{name}' not found, but I'm flexible!"

obj = DynamicObj()
print(getattr(obj, "unknown"))  # Attribute 'unknown' not found...
```

Python 的“魔法”为反射提供了元数据基础，而反射让程序具备运行时自省与动态操作的能力。

如果你正在开发需要动态处理对象的系统（比如配置加载、API 自动生成、测试框架），深入理解这两者的结合会非常有帮助！

## 11. 三目运算符

Python 是一种极简主义的编程语言，它没有引入 `?:` 这个新的运算符，而是使用已有的 `if else` 关键字来实现相同的功能。

``` python
maxn = a if a > b else b
# 等价于 
exp1 if condition else exp2
```

## 12. 面对对象编程

> 面向对象编程：把一组数据和处理数据的方法组成 **对象**，把行为相同的对象归纳为 **类**，通过 **封装** 隐藏对象的内部细节，通过 **继承** 实现类的特化和泛化，通过 **多态** 实现基于对象类型的动态分配。
>
> 类是对象的蓝图，对象是类的实例，是可以接收消息的实体，对象的属性是对象的静态特征，对象的行为是对象的动态特征。

### 1. 成员变量

在 Python 中，**类的成员变量（也称属性）不需要像 Java、C++ 那样在类体顶部“声明”类型或名称**。Python 是动态语言，成员变量通常是在运行时通过赋值动态创建的。但根据用途不同，成员变量可分为 **类变量（class variables）** 和 **实例变量（instance variables）**，它们的“声明”方式也不同。

**（1）实例变量（Instance Variables）**

- 属于**每个对象实例**，不同实例互不影响。
- **通常在 `__init__` 方法中通过 `self.xxx = value` 定义**。
- 不需要提前“声明”，首次赋值即创建。

示例：

```python
class Student:
    def __init__(self, name, age):
        self.name = name   # 实例变量
        self.age = age     # 实例变量
        self.id = name + "2024"  # 也是实例变量
```

每个 `Student` 对象都有自己的 `name`、`age`、`id`。

**（2）类变量（Class Variables）**

- 属于**类本身**，所有实例共享（除非被实例覆盖）。
- **直接在类体中定义（不在方法内）**。

示例：

```python
class Student:
    school = "MIT"  # 类变量：所有学生共享
    
    def __init__(self, name):
        self.name = name  # 实例变量
```

使用：

```python
s1 = Student("Alice")
s2 = Student("Bob")

print(s1.school)  # MIT
print(s2.school)  # MIT

# 修改类变量（影响所有实例）
Student.school = "Stanford"
print(s1.school)  # Stanford

# 但若通过实例赋值，会创建同名实例变量（遮蔽类变量）
s1.school = "Harvard"
print(s1.school)  # Harvard（实例变量）
print(s2.school)  # Stanford（仍用类变量）
```

> ⚠ 注意：**不要用可变对象（如 list、dict）作为类变量的默认值**，否则所有实例会共享同一个对象！

```python
# 危险示例
class BadClass:
    data = []  # 所有实例共享同一个 list！

# 正确做法：在 __init__ 中初始化
class GoodClass:
    def __init__(self):
        self.data = []
```

虽然 Python 不强制声明，但为了**代码可读性、IDE 提示、类型检查（如 mypy）**，推荐以下方式：

方式 1：在 `__init__` 中初始化所有实例变量

```python
class Person:
    def __init__(self, name: str = "", age: int = 0) -> None:
        self.name = name
        self.age = age
        self.email: str = ""      # 即使初始为空，也显式写出
        self.tags: list[str] = [] # 明确类型和初始值
```

方式 2：使用 `__slots__`（可选，用于限制属性 + 节省内存）

```python
class Point:
    __slots__ = ('x', 'y')  # 只允许 x 和 y 两个实例变量
    
    def __init__(self, x, y):
        self.x = x
        self.y = y
```

方式 3：使用类型注解（Python 3.6+）——**现代推荐**

```python
from typing import Optional

class Student:
    # 类变量（带类型注解）
    school: str = "Unknown"
    
    # 实例变量（在 __init__ 外用注解“声明”，但不赋值）
    name: str
    age: int
    id: str
    advisor: Optional[str]
    
    def __init__(self, name: str, age: int) -> None:
        self.name = name
        self.age = age
        self.id = f"{name}_{age}"
        self.advisor = None  # 初始化可选字段
```

> 💡 这种写法：
>
> - 让 IDE 和类型检查器（如 mypy、PyCharm）能识别属性
> - 提高代码可维护性
> - 不影响运行时行为（注解在运行时可忽略）

**总结：如何“声明”成员变量？**

| 类型         | 如何“声明”                             | 是否必须               | 推荐做法                           |
| ------------ | -------------------------------------- | ---------------------- | ---------------------------------- |
| **实例变量** | 在 `__init__` 中用 `self.attr = value` | ❌ 不必须（可动态添加） | ✅ 全部在 `__init__` 中初始化       |
| **类变量**   | 在类体中直接赋值（如 `count = 0`）     | ❌                      | ✅ 仅用于真正共享的数据             |
| **类型提示** | 使用 `attr: type` 注解                 | ❌                      | ✅ 强烈推荐（提升可读性和工具支持） |

**关键结论**

* **Python 中没有“声明变量”的语法，只有“赋值即创建”。**
* 但为了写出清晰、健壮、可维护的代码，**应在 `__init__` 中显式初始化所有实例变量，并配合类型注解**。

如果你使用现代 Python（3.7+），还可以结合 `dataclasses` 自动生成 `__init__`，进一步简化：

```python
from dataclasses import dataclass

@dataclass
class Student:
    name: str
    age: int
    school: str = "Unknown"  # 默认值（类变量语义，但实际是实例默认值）
```

这会自动生成 `__init__`、`__repr__` 等方法，且明确表达了成员变量结构

### 2. 对象方法

Python 的对象没有隐式的 this 指针，但类方法的第一个参数通常都是 `self`，相当于 this 指针。不同于 C++，在 Python 中，给对象发送消息（调用成员函数）有两种方法（这可能得益于 Python 中 this 指针 `self` 是显式指定的：

``` PYTHON
class Student:
    def study(self, course_name):
        print(f'学生正在学习{course_name}')
    def play(self):
        print(f'学生正在玩游戏')

stu1 = Student()
print(stu1)
print(type(stu1))

# （1）类.方法(对象,arguments)
Student.study(stu1, 'csapp')

# （2）对象.方法(arguments)
stu1.study('java')
```

> 注意不同于 C++，如果我们要在一个方法属性中调用另外一个方法属性 `func`，形式为 `self.func()` 而不是直接 `func()`。

### 3. 魔法方法

Python中，以两个下划线 `__` 开头和结尾的方法通常都是由特殊用途和意义的方法，我们一般称之为 **魔术方法** 或 **魔法方法**。

例如初始化对象的 `__init__`，以及更改打印对象显示的内容的 `__repr__` 方法（初始时显示对象的地址），例如：

``` PYTHON
class Student:
    # 声明了两个属性name和age
    def __init__(self, name, age):
        self.name = name
        self.age = age
    def study(self, course_name):
        print(f'学生正在学习{course_name}')
    def play(self):
        print(f'学生正在玩游戏')
    # 修改print打印的内容
    def __repr__(self):
        return f'{self.name}: {self.age}'

stu = Student('Fiona', 23)
print(stu) # Fiona: 23
```

### 4. 实例化

在 Python 中，**对象的创建（creation）和初始化（initialization）是两个分离的步骤**，分别由两个不同的魔法方法负责：

| 阶段                          | 方法                  | 职责                                                         |
| ----------------------------- | --------------------- | ------------------------------------------------------------ |
| **1. 创建对象（分配内存）**   | `__new__(cls, ...)`   | **静态方法**，负责创建并返回一个新的实例（通常是 `object.__new__(cls)`） |
| **2. 初始化对象（设置属性）** | `__init__(self, ...)` | **实例方法**，对已创建的实例进行初始化（如赋值 `self.name = name`） |

### 5. 访问可见性

在 Python 中，说明属性的可见性的方法是在属性名前面添加下划线，不加下划线表示 `public`，一个下划线表示 `protected`，两个下划线表示 `private`（魔方方法除外），<font color="blue">下划线作为属性名的一部分</font>。

``` PYTHON
class Student:
    # 声明了两个属性 name 和 age
    def __init__(self, name, _sex, __age):
        self.name = name
        self._sex = _sex
        self.__age = __age
    def study(self, course_name):
        print(f'{self.name}正在学习{course_name}')

stu = Student('Fiona', 'woman' ,23)
print(stu.name) # Fiona
print(stu._sex) # woman
print(stu.__age) # AttributeError: 'Student' object has no attribute '__age'
```

虽然 Python 提供了说明属性的可见性的方法，但这只是一个**“君子约定”**，而不是语言层面上的机制，你总是有办法绕过这些不具备强制性的约定，典型的防君子不防小人。Python 中有一句名言：“**We are all consenting adults here**”（大家都是成年人）。Python 语言的设计者认为程序员要为自己的行为负责，而不是由 Python 语言本身来严格限制访问可见性，而大多数的程序员都认为 **开放比封闭要好**，把对象的属性私有化并不是编程语言必须的东西，所以 Python 并没有从语法上做出最严格的限定。

以 protected 可见性为例，你可以在 class 外面直接访问和修改受保护的属性而不会报错。这样看来，python 的 protected 属性和方法似乎很鸡肋，但我认为，这样很符合 python 一贯坚持的风格。以单下划线做前缀的属性和方法，被视为受保护的，python 没有从解释器层面上做出强制性检查，它只是作为一个约定而存在，他们都应被视为 API 或任何 Python 代码的非公开部分，无论它是函数，方法还是数据成员。你可以选择遵守这个约定，也可以不遵守，由于不遵守此约定所造成的负面后果自然也需要你来承担。**因此，简而言之，protected 成员是受保护的，你不应该直接去修改它**。

再例如 private 方法，虽然在 class 外直接访问 private 属性会报错，但 Python 并没有从语法上严格保证私有成员的私密性，它只是给私有成员重整一个名字来阻挠对他们的访问，即：`instance._class__variable`，例如：

``` PYTHON
class Student:
    # 声明了两个属性name和age
    def __init__(self, name, _sex, __age):
        self.name = name
        self._sex = _sex
        self.__age = __age
    def study(self, course_name):
        print(f'{self.name}正在学习{course_name}')
    def __talk(self):
        print(f'{self.name} is {self.__age} years old')

stu = Student('Fiona', 'woman' ,23)
stu._Student__talk()  # 访问私有方法
print(stu._Student__age) # 访问私有成员
```

### 6. 属性装饰器

Python 中可以通过属性装饰器为“私有”属性提供读取和修改的方法，之前我们提到过，装饰器通常会放在类、函数或方法的声明之前，通过一个 `@` 符号表示将装饰器应用于类、函数或方法。通过添加属性访问器和属性修改器，我们可以像访问普通属性一样访问私有成员。

``` python
class Student:
    def __init__(self, __name):
        self.__name = __name

    # 属性访问器
    @property
    def name(self):
        return self.__name

    # 属性修改器
    @name.setter
    def name(self, name):
        self.__name = name

# 访问时无需加上下划线
stu = Student('Alice')
print(stu.name)
stu.name = 'Bob'
print(stu.name)
```

### 7. 动态属性

Python 是一门动态语言，维基百科对动态语言的解释是：“**在运行时可以改变其结构的语言**，例如新的函数、对象、甚至代码可以被引进，已有的函数可以被删除或是其他结构上的变化。动态语言非常灵活，目前流行的 Python 和 JavaScript 都是动态语言，除此之外如 PHP、Ruby 等也都属于动态语言，而C、C++ 等语言则不属于动态语言”。

在 Python 中，我们可以动态为对象添加属性，这是 Python 作为动态类型语言的一项特权，代码如下所示。需要提醒大家的是，<font color="blue">对象的方法其实本质上也是对象的属性</font>，如果给对象发送一个无法接收的消息，引发的异常仍然是`AttributeError`。

``` PYTHON
class Student:
    def __init__(self, __name):
        self.__name = __name

    def study(self, course_name):
        print(f'{self.__name} is learning {course_name}')

stu = Student('Alice')
stu.sex = 'woman'
print(stu.sex) # woman

stu2 = Student('Bob')
print(stu2.sex) # AttributeError: 'Student' object has no attribute 'sex'
```

> <font color="blue">需要注意的是，动态添加的属性仅仅属于该对象，而不是属于该 class</font>

如果不希望在使用对象时动态的为对象添加属性，可以使用 Python 的 `__slots__` 魔法。对于 `Student` 类来说，可以在类中指定 `__slots__ = ('name', 'age')`，这样 `Student` 类的对象只能有 `name` 和 `age` 属性，如果想动态添加其他属性将会引发异常，代码如下所示：

``` PYTHON
class Student:
    def __init__(self, __name):
        self.__name = __name

    def study(self, course_name):
        print(f'{self.__name} is learning {course_name}')

    __slots__ = ('__name')

stu = Student('Alice')
stu.sex = 'woman' # AttributeError: 'Student' object has no attribute 'sex'
```

### 8. 静态方法和类方法

之前我们在类中定义的方法都是**对象方法**，换句话说这些方法都是对象可以接收的消息。除了对象方法之外，类中还可以有**静态方法**和**类方法**，这两类方法是发给类的消息，二者并没有实质性的区别。在面向对象的世界里，一切皆为对象，我们定义的每一个类其实也是一个对象，而静态方法和类方法就是发送给类对象的消息。

可以直接使用 `类名.方法名` 的方式来调用静态方法和类方法，二者的区别在于，<font color="blue">类方法的第一个参数是类对象本身，而静态方法则没有这个参数</font>（这点和 C++ 一样）。

简单的总结一下，**对象方法、类方法、静态方法都可以通过`类名.方法名`的方式来调用，区别在于方法的第一个参数到底是普通对象还是类对象，还是没有接受消息的对象**。静态方法通常也可以直接写成一个独立的函数，因为它并没有跟特定的对象绑定。

``` PYTHON
class Triangle:
    def __init__(self, a, b, c):
        self.a = a
        self.b = b
        self.c = c

    def perimeter(self):
        return self.a + self.b + self.c

    def area(self):
        p = self.perimeter() / 2
        return (p * (p - self.a) * (p - self.b) * (p - self.c)) ** 0.5

    @classmethod
    def is_valid(self, a, b, c):
        return a + b > c and a + c > b and b + c > a

    @staticmethod
    def is_valid(a, b, c):
        return a + b > c and a + c > b and b + c > a

a, b, c = 1, 1, 100
if Triangle.is_valid(a, b, c) == False:
    print('Not valid triangle')
else:
    t = Triangle(a, b, c)
    print(t.area())
    print(t.perimeter())
```

### 9. 继承和多态

继承的语法是在定义类的时候，在类名后的圆括号中指定当前类的父类。如果定义一个类的时候没有指定它的父类是谁，那么默认的父类是 object 类。object 类是 Python 中的 <font color="blue">顶级类</font>，这也就意味着所有的类都是它的子类，要么直接继承它，要么间接继承它。Python 语言允许多重继承，也就是说一个类可以有一个或多个父类。在子类的初始化方法中，我们可以通过 `super().__init__()` 来调用父类初始化方法，super 函数是 Python 内置函数中专门为获取当前对象的父类对象而设计的。

子类继承父类的方法后，还可以对方法进行重写（重新实现该方法），不同的子类可以对父类的同一个方法给出不同的实现版本，这样的方法在程序运行时就会表现出多态行为（调用相同的方法，做了不同的事情）。

``` python
class Person(object):
    def __init__(self, name):
        self.name = name
    def eat(self):
        print(f'person is eating')
    def sleep(self):
        print(f'person is sleeping')

class Student(Person):
    def __init__(self, name, id):
        super().__init__(name)
        self.id = id
    def eat(self):
        print(f'student is eating')
    def sleep(self):
        print(f'student is sleeping')
    def study(self):
        print(f'student is studiing')

p = Person('John')
p.eat()
p.sleep()

s = Student('Alice', "2024")
s.eat()
s.sleep()
s.study()
```

### 10. 实例

``` PYTHON
from enum import Enum
import random

# 扑克牌花色
class Suit(Enum):
    SPADE, HEART, CLUB, DIAMOND = range(4)

class Card():
    def __init__(self, suit, face):
        self.suit = suit
        self.face = face

    def __repr__(self):
        suits = '♠♥♣♦'
        faces = ['', 'A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K']
        return f'{suits[self.suit.value]}{faces[self.face]}'

    def __lt__(self, other):
        if(self.face == other.face):
            return self.suit.value < other.suit.value
        return self.face < other.face

class Poker():
    def __init__(self):
        self.cards = [Card(suit, face) for suit in Suit for face in range(1, 14)]
        self.current = -1

    def shuffle(self):
        random.shuffle(self.cards)

    def has_next(self):
        return self.current < len(self.cards)

    def deal(self):
        if(self.has_next()):
            self.current += 1
            return self.cards[self.current]
        else:
            print('no card can be gotten!')

class Player():
    def __init__(self, name):
        self.name = name
        self.cards = []

    def get_one(self, card):
        self.cards.append(card)

    def arrange(self):
        self.cards.sort()


poker = Poker()
poker.shuffle()

players = [Player('Bob'), Player('Alice'), Player('Fiona'), Player('Steve')]

for _ in range(13):
    for player in players:
        card = poker.deal()
        print(card, poker.current)
        player.get_one(card)

for player in players:
    player.arrange()
    print(f'{player.name}', end=':')
    print(player.cards)
```

## 13. Python中变量的作用域

<font color="blue">在函数内部，无法直接对函数外部定义的变量进行修改，需要在当前函数内部再次声明：</font>

* 如果该变量是全局变量，需要加 `global`
* 如果该变量是局部变量，需要加 `nolocal`

**<font color=red>1. Local Scope</font>**

变量定义在函数内部，它们只能在该函数内部访问。这些变量称为局部变量，作用域仅限于定义它们的函数中。一旦函数执行完毕，局部变量就会被销毁。

这也解释了，为什么在 class 内部的函数中，调用成员函数需要用 `self.arg` 而不能像 C 一样直接使用 `arg`，因为直接使用 `arg` 的话，CPython 会把 `arg` 视为 Local Argument。

**<font color=red>2. Enclosing Scope</font>**

这是嵌套函数中的作用域，<font color=blue>内部函数可以访问其外部函数的变量，但不能修改它们（除非使用 `nonlocal` 关键字）。</font>外部函数的变量对内部函数是可见的，称为闭包作用域。

```python
def outer():
    x = 10
    def inner():
        print("inner:", x)
        x = 100
    inner() # UnboundLocalError: local variable 'x' referenced before assignment
    print("outer:", x)

if __name__ == '__main__':
    outer()

```

添加 nonlocal 说明：

``` python
def outer():
    x = 10
    def inner():
        nonlocal x
        print("inner:", x)
        x = 100
    inner() # UnboundLocalError: local variable 'x' referenced before assignment
    print("outer:", x)

if __name__ == '__main__':
    outer()

# inner: 10
# outer: 100
```

> 此时 `outer` 的作用域不是局部作用域了，而是闭包作用域。

**<font color=red>3. Global Scope</font>**

变量定义在函数或类之外的代码块中，它们可以在整个程序中被访问。全局变量可以在任意位置使用，但 <font color=blue>如果在函数内部修改它们，需要使用 `global` 关键字。</font>

``` python
x = 20

def main():
    x = 100 # 这里其实是在 main 内创建一个变量 x 并赋值为 100
    print("main:", x) 

if __name__ == '__main__':
    main()
    print("global:", x) 

# main: 100
# global: 20
```

添加 global 关键字：

``` python
x = 20

def main():
    global x 
    x = 100
    print("main:", x) 

if __name__ == '__main__':
    main()
    print("global:", x) 

# main: 100
# global: 10
```

**<font color=red>4. Built-in Scope</font>**

Python 有一组内置的函数和变量，它们在所有作用域中都可以直接使用，比如 `print()`、`len()` 等。这些变量存储在 `builtins` 模块中。

```python
print("Hello, World!")  # print 是一个内置函数
```

**<font color=red>5. LEGB规则</font>**

Python 的作用域查找顺序遵循 LEGB 规则：

1. **Local**：局部作用域
2. **Enclosing**：闭包函数的外层作用域
3. **Global**：全局作用域
4. **Built-in**：内置作用域

查找变量时，Python 按照这个顺序依次查找，直到找到为止。

你可以使用 `globals()` 和 `locals()` 函数来查看当前作用域内的变量。

**<font color=red>6. 非局部变量</font>**

非局部变量既不是 global 变量，也不是 local 变量。

它通常在外层函数的作用域当中被定义，但可以在内层函数中访问和修改。

下面是一个使用非局部变量的示例：

```PYTHON
def outer_function():
    x = 10  # 外层函数的变量
    
    def inner_function():
        nonlocal x  # 指定 x 是非局部变量
        x += 5  # 修改外层函数的变量
        print("Inner function:", x) 

    inner_function()  # 调用内部函数
    print("Outer function:", x)  # 输出修改后的变量

outer_function()
```

```SHELL
Inner function: 15
Outer function: 15
```

非局部变量常用于实现**闭包（closure）**，允许内部函数在其外部环境中“记住”某些状态。

## 14. I/O

**<font color=red>1. File System</font>**

**文件系统** 使用文件和树形目录的抽象逻辑概念替代了硬盘、光盘、闪存等物理设备的数据块概念。

**用户使用文件系统来保存数据时，不辞关心数据实际保存在硬盘的那个数据块上，只需要记住这个文件的路径和文件名**。再写入新数据之前，用户不必关心硬盘上的那个数据块没有被使用，硬盘上的存储空间管理（分配和释放）功能由文件系统自动完成，用户只需要记住数据被写入到那个文件中。

**<font color=red>2. IO Type</font>**

Python 的 `io` 模块定义了清晰的 I/O 抽象层次，从高层到底层依次为：

| 层级                  | 类型             | 返回数据类型 | 是否缓冲 | 典型用途                                 |
| --------------------- | ---------------- | ------------ | -------- | ---------------------------------------- |
| **1. 文本 I/O**       | `TextIOBase`     | `str`        | ✅ 是     | 处理文本文件（如 `.txt`, `.csv`, `.py`） |
| **2. 缓冲二进制 I/O** | `BufferedIOBase` | `bytes`      | ✅ 是     | 处理图片、音频、压缩包等二进制数据       |
| **3. 原始二进制 I/O** | `RawIOBase`      | `bytes`      | ❌ 否     | 底层设备操作（如串口、自定义协议）       |

> 实际开发中，**99% 的场景只需使用前两类**。

**（1）Text I/O（文本 I/O）**

- **产生/消费 `str` 对象**
- 自动处理：
  - **编码/解码**（通过 `encoding` 参数，如 `utf-8`）
  - **换行符转换**（如 Windows `\r\n` ↔ Unix `\n`）
- **必须指定文本模式**：`'r'`, `'w'`, `'a'`, `'r+'` 等（**不含 `b`**）

示例：

```python
# 读取文本文件
with open("data.txt", "r", encoding="utf-8") as f:
    content = f.read()  # type: str

# 内存中文本流
from io import StringIO
sio = StringIO("Hello\nWorld")
print(sio.readline())  # "Hello\n" (str)
```

> ⚠如果不指定 `encoding`，Python 会使用系统默认编码（可能引发跨平台问题）！

**（2）Binary I/O（缓冲二进制 I/O）**

- **产生/消费 `bytes` 对象**
- **无编码转换、无换行处理**，原始字节流
- 使用**二进制模式**：`'rb'`, `'wb'`, `'ab'` 等（**必须含 `b`**）

示例：

```python
# 读取图片
with open("image.jpg", "rb") as f:
    data = f.read()  # type: bytes

# 内存中二进制流
from io import BytesIO
bio = BytesIO(b"binary\x00data")
print(bio.read(6))  # b'binary'
```

> 推荐：处理非文本文件（图片、PDF、EXE 等）**必须用二进制模式**，避免编码错误。

**（3）Raw I/O（原始/无缓冲 I/O）**

- 直接与操作系统交互，**无缓冲**（每次 I/O 调用都触发系统调用）
- 性能低，一般**不直接使用**
- 通过 `buffering=0` 启用（仅在二进制模式下有效）

示例：

```python
# 仅用于特殊场景（如硬件通信）
f = open("device.bin", "rb", buffering=0)  # Raw I/O
```

> 注意：`buffering=0` 在文本模式下会报错！

**（4）内存中的“虚拟文件”：`StringIO` 与 `BytesIO`**

| 类型          | 类似文件模式                | 数据类型 | 用途                                       |
| ------------- | --------------------------- | -------- | ------------------------------------------ |
| `io.StringIO` | 文本模式 (`'r'`, `'w'`)     | `str`    | 模拟文本文件（如单元测试、临时日志）       |
| `io.BytesIO`  | 二进制模式 (`'rb'`, `'wb'`) | `bytes`  | 模拟二进制文件（如生成内存中的 ZIP、图像） |

完整示例：

```python
from io import StringIO, BytesIO

# 文本流
sio = StringIO()
sio.write("Line 1\n")
sio.write("Line 2")
sio.seek(0)
print(sio.readlines())  # ['Line 1\n', 'Line 2']

# 二进制流
bio = BytesIO()
bio.write(b"ABC")
bio.write(b"\x00\x01")
bio.seek(0)
print(bio.read())  # b'ABC\x00\x01'
```

> 两者都支持 `seek()`, `tell()`, `read()`, `write()` 等完整文件接口。

**最佳实践建议**

1. **明确区分文本 vs 二进制**

   - 文本：用 `'r'` + `encoding` → `str`
   - 二进制：用 `'rb'` → `bytes`

2. **永远不要混用**

   

   ```python
   # 错误：向二进制文件写字符串
   with open("test.bin", "wb") as f:
       f.write("hello")  # TypeError!
   
   # 正确
   f.write("hello".encode("utf-8"))
   ```

3. **优先使用 `with` 语句**
    自动关闭文件，避免资源泄漏。

4. **避免无缓冲 I/O（除非必要）**
    `buffering=0` 会严重降低性能。

5. **用 `pathlib.Path` 替代字符串路径**（Python 3.4+）

   ```python
   from pathlib import Path
   with open(Path("data") / "file.txt", "r") as f:
       ...
   ```

**总结**

| I/O 类型       | 打开方式                           | 数据类型 | 缓冲 | 典型类                              |
| -------------- | ---------------------------------- | -------- | ---- | ----------------------------------- |
| **Text I/O**   | `open(..., 'r', encoding='utf-8')` | `str`    | ✅    | `TextIOWrapper`                     |
| **Binary I/O** | `open(..., 'rb')`                  | `bytes`  | ✅    | `BufferedReader` / `BufferedWriter` |
| **Raw I/O**    | `open(..., 'rb', buffering=0)`     | `bytes`  | ❌    | `FileIO`                            |
| **内存文本**   | `StringIO("...")`                  | `str`    | ✅    | `StringIO`                          |
| **内存二进制** | `BytesIO(b"...")`                  | `bytes`  | ✅    | `BytesIO`                           |

**<font color=red>3. open()</font>**

``` python
fileObject = open(file, 
                  mode='r', 
                  buffering=-1, 
                  encoding=None, 
                  errors=None, 
                  newline=None, 
                  closefd=True, 
                  opener=None)
```

**mode select:**

* reading: `r`
* writing –> truncate?
  * yes: `w`
  * no: `a`
* reading and writing -> truncate?
  * yes: `w+`
  * no -> initial position?
    * begin: `r+`
    * end: `a+`

**API:**

* `close()`：该方法可以被多次调用，但只有第一次生效

* `closed`：注意这是一个属性而不是一个方法

* `fileno()`：返回流的 $fd$（正数），如果存在

* `flush()`：刷新流的写入缓冲区，这对只读和非阻塞流不起作用

* `readline(size=-1)`：包括 `\n`，可以通过 size 指定最大读取字节

* `readlines(hint=-1)`读取所有行直到 EOF，使用 `for-in` 也可以实现循环按行读取

  > If given an optional parameter sizehint, it reads that many bytes from the file and enough more to complete a line, and returns the lines from that. 
  >
  > 关于换行符：
  >
  > 对于文本文件：
  >
  > 1. 指定字符数中间包含 `\n`，则 `\n` 算做字符；
  > 2. 如果 `\n` 在指定内容末尾，则**不计入字符**，算作换行符。
  >
  > 在二进制模式下，字符数量是以 ASCII 码对应的单字节为单位来计算的，不剔除换行符，字符 `\n` 占用一个字节长度
  >
  > **并不推荐一下子读取文件的所有行（即读取整个文件），因为如果内容太大，可能会造成非常大的内存开销**

* `write(str)`

* `writelines(lines)`：将行列表写入流

* `writable()`

* `readable()`

* `seek(offset, whence=SEEK_SET)`: 将流位置修改到给定的字节 *offset*，返回新的绝对位置。

  > * `SEEK_SET` 或 `0` -- 流的开头（默认值）；*offset* 应为零或正值
  > * `SEEK_CUR` or `1` -- 当前流位置；*offset* 可以为负值
  > * `SEEK_END` or `2` -- 流的末尾；*offset* 通常为负值

* `seekable()`

* `tell()`：返回文件当前位置

* `truncate(size=none)`：将流的大小调整为给定的 *size* 个字节（如果未指定 *size* 则调整至当前位置）。当前的流位置不变。

## 15. Exception

Python中的错误至少可以被分为两类：语法错误（解析错误）和异常

### 1. 关键字

Python 中于异常有关的关键词有 5 个，分别是：

* raise：抛出异常对象，引发异常
* try：放置可能会出错的代码，异常处理程序不仅会处理在 *try 子句* 中立刻发生的异常，还会处理在 *try 子句* 中调用（包括间接调用）的函数
* except：捕获异常，基类异常对象会被其派生类异常捕获
* else：如果没有异常被捕获时执行
* finally：无论程序是否异常都会执行，甚至是调用了 `sys` 模块的 `exit` 函数终止 Python 程序，`finally` 块中的代码仍然会被执行（因为 `exit` 函数的本质是引发了 `SystemExit` 异常），因此我们把 `finally` 代码块称为“总是执行代码块”，它最适合用来做释放外部资源的操作。

### 2. 异常的继承结构

``` python
BaseException
 +-- SystemExit
 +-- KeyboardInterrupt
 +-- GeneratorExit
 +-- Exception
      +-- StopIteration
      +-- StopAsyncIteration
      +-- ArithmeticError
      |    +-- FloatingPointError
      |    +-- OverflowError
      |    +-- ZeroDivisionError
      +-- AssertionError
      +-- AttributeError
      +-- BufferError
      +-- EOFError
      +-- ImportError
      |    +-- ModuleNotFoundError
      +-- LookupError
      |    +-- IndexError
      |    +-- KeyError
      +-- MemoryError
      +-- NameError
      |    +-- UnboundLocalError
      +-- OSError
      |    +-- BlockingIOError
      |    +-- ChildProcessError
      |    +-- ConnectionError
      |    |    +-- BrokenPipeError
      |    |    +-- ConnectionAbortedError
      |    |    +-- ConnectionRefusedError
      |    |    +-- ConnectionResetError
      |    +-- FileExistsError
      |    +-- FileNotFoundError
      |    +-- InterruptedError
      |    +-- IsADirectoryError
      |    +-- NotADirectoryError
      |    +-- PermissionError
      |    +-- ProcessLookupError
      |    +-- TimeoutError
      +-- ReferenceError
      +-- RuntimeError
      |    +-- NotImplementedError
      |    +-- RecursionError
      +-- SyntaxError
      |    +-- IndentationError
      |         +-- TabError
      +-- SystemError
      +-- TypeError
      +-- ValueError
      |    +-- UnicodeError
      |         +-- UnicodeDecodeError
      |         +-- UnicodeEncodeError
      |         +-- UnicodeTranslateError
      +-- Warning
           +-- DeprecationWarning
           +-- PendingDeprecationWarning
           +-- RuntimeWarning
           +-- SyntaxWarning
           +-- UserWarning
           +-- FutureWarning
           +-- ImportWarning
           +-- UnicodeWarning
           +-- BytesWarning
           +-- ResourceWarning
```

从上面的继承结构可以看出，Python 中所有的异常都是 `BaseException` 的子类型，它有四个直接的子类，分别是：

* `SystemExit`：解释器请求退出
* `KeyboardInterrupt`：用户中断程序执行(`CTRL+C`)
* `GeneratorExit`：生成器发生异常通知退出
* `Exception`：常规异常类型，我们自定义的异常类型也因该直接或者间接继承自 `Exception` 类，并且应当以 `Error` 结尾，类似标准异常的命名。

> 异常类可以被定义成能做其他类所能做的任何事，但通常应当保持简单，它往往只提供一些属性，允许相应的异常处理程序提取有关错误的信息。

例如：

``` python
class InputError(Exception):
    '''自定义异常类型'''
    pass

def fac(num):
    if num < 0:
        raise InputError('只能计算非负整数的阶乘!')
    if num in (0, 1):
        return 1
    return num * fac(num - 1)

flag = True
while flag:
    n = int(input('n = '))
    try:
        print(f'{n}! = {fac(n)}')
    except InputError as e:
        print(str(e))
        flag = False
```

### 3. 输出异常信息

通过 `str(error)` 打印异常的错误信息：

``` python
try:
    # 可能引发异常的代码
    a = 10 / 0
except Exception as e:
    # 打印异常信息
    print("发生异常：", str(e))
```

通过 ` traceback.print_exc()` 打印完整异常信息（异常类型，错误消息，堆栈跟踪信息）：

``` python
import traceback

try:
    # 可能引发异常的代码
    a = 10 / 0
except Exception as e:
    # 打印完整的异常信息
    traceback.print_exc()
```

### 4. 异常链

如果一个未处理的异常发生在 [`except`](https://docs.python.org/zh-cn/3/reference/compound_stmts.html#except) 部分内，它会将被处理的异常附加到它上面，并包括在错误信息中:

```python
try:
	open("database.sqlite")
except OSError:
	raise RuntimeError("unable to handle error")
...
Traceback (most recent call last):
  File "<stdin>", line 2, in <module>
FileNotFoundError: [Errno 2] No such file or directory: 'database.sqlite'

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "<stdin>", line 4, in <module>
RuntimeError: unable to handle error
```

为了表明一个异常是另一个异常的直接后果， [`raise`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#raise) 语句允许一个可选的 [`from`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#raise) 子句:

```python
# exc must be exception instance or None.
raise RuntimeError from exc
```

转换异常时，这种方式很有用。例如：

```python
>>> def func():
...     raise ConnectionError
...
>>> try:
...     func()
... except ConnectionError as exc:
...     raise RuntimeError('Failed to open database') from exc
...
Traceback (most recent call last):
  File "<stdin>", line 2, in <module>
  File "<stdin>", line 2, in func
ConnectionError

The above exception was the direct cause of the following exception:

Traceback (most recent call last):
  File "<stdin>", line 4, in <module>
RuntimeError: Failed to open database
```

它还允许使用 `from None` 表达禁用自动异常链:

```python
try:
	open('database.sqlite')
except OSError:
	raise RuntimeError from None
...
Traceback (most recent call last):
  File "<stdin>", line 4, in <module>
RuntimeError
```

异常链机制详见 [内置异常](https://docs.python.org/zh-cn/3/library/exceptions.html#bltin-exceptions)。

### 5. Finally

如果存在 [`finally`](https://docs.python.org/zh-cn/3/reference/compound_stmts.html#finally) 子句，则 `finally` 子句是 [`try`](https://docs.python.org/zh-cn/3/reference/compound_stmts.html#try) 语句结束前执行的最后一项任务。不论 `try` 语句是否触发异常，都会执行 `finally` 子句。以下内容介绍了几种比较复杂的触发异常情景：

* 如果执行 `try` 子句期间触发了某个异常，则某个 [`except`](https://docs.python.org/zh-cn/3/reference/compound_stmts.html#except) 子句应处理该异常。如果该异常没有 `except` 子句处理，在 `finally` 子句执行后会被重新触发。
* `except` 或 `else` 子句执行期间也会触发异常。 同样，该异常会在 `finally` 子句执行之后被重新触发。
* 如果 `finally` 子句中包含 [`break`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#break)、[`continue`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#continue) 或 [`return`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#return) 等语句，异常将不会被重新引发。
* 如果执行 `try` 语句时遇到 [`break`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#break),、[`continue`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#continue) 或 [`return`](https://docs.python.org/zh-cn/3/reference/simple_stmts.html#return) 语句，则 `finally` 子句在执行 `break`、`continue` 或 `return` 语句之前执行。
* 如果 `finally` 子句中包含 `return` 语句，则返回值来自 `finally` 子句的某个 `return` 语句的返回值，而不是来自 `try` 子句的 `return` 语句的返回值。

### 6. 用注释细化异常情况

当一个异常被创建以引发时，它通常被初始化为描述所发生错误的信息。在有些情况下，在异常被捕获后添加信息是很有用的。为了这个目的，异常有一个 `add_note(note)` 方法接受一个字符串，并将其添加到异常的注释列表。标准的回溯在异常之后按照它们被添加的顺序呈现包括所有的注释。

```python
try:
   raise TypeError('bad type')
except Exception as e:
   e.add_note('Add some information')
   e.add_note('Add some more information')
   raise
...
Traceback (most recent call last):
  File "<stdin>", line 2, in <module>
TypeError: bad type
Add some information
Add some more information
```

### 7. 引发并处理多个不相关的异常

在有些情况下，有必要报告几个已经发生的异常。这通常是在并发框架中当几个任务并行失败时的情况，但也有其他的用例，有时需要是继续执行并收集多个错误而不是引发第一个异常。

内置的 [`ExceptionGroup`](https://docs.python.org/zh-cn/3/library/exceptions.html#ExceptionGroup) 打包了一个异常实例的列表，这样它们就可以一起被引发。它本身就是一个异常，所以它可以像其他异常一样被捕获。

> `` Exception BaseExceptionGroup(msg, excs)``

```python
def f():
...     excs = [OSError('error 1'), SystemError('error 2')]
...     raise ExceptionGroup('there were problems', excs)
...
>>> f()
  + Exception Group Traceback (most recent call last):
  |   File "<stdin>", line 1, in <module>
  |   File "<stdin>", line 3, in f
  | ExceptionGroup: there were problems
  +-+---------------- 1 ----------------
    | OSError: error 1
    +---------------- 2 ----------------
    | SystemError: error 2
    +------------------------------------
>>> try:
...     f()
... except Exception as e:
...     print(f'caught {type(e)}: e')
...
caught <class 'ExceptionGroup'>: e
```

通过使用 `except*` 代替 `except` ，我们可以有选择地只处理组中符合某种类型的异常。在下面的例子中，显示了一个嵌套的异常组，每个 `except*` 子句都从组中提取了某种类型的异常，而让所有其他的异常传播到其他子句，并最终被重新引发。

```python
def f():
...     raise ExceptionGroup(
...         "group1",
...         [
...             OSError(1),
...             SystemError(2),
...             ExceptionGroup(
...                 "group2",
...                 [
...                     OSError(3),
...                     RecursionError(4)
...                 ]
...             )
...         ]
...     )
...
>>> try:
...     f()
... except* OSError as e:
...     print("There were OSErrors")
... except* SystemError as e:
...     print("There were SystemErrors")
...
There were OSErrors
There were SystemErrors
  + Exception Group Traceback (most recent call last):
  |   File "<stdin>", line 2, in <module>
  |   File "<stdin>", line 2, in f
  | ExceptionGroup: group1
  +-+---------------- 1 ----------------
    | ExceptionGroup: group2
    +-+---------------- 1 ----------------
      | RecursionError: 4
      +------------------------------------
```

注意，嵌套在一个异常组中的异常必须是实例，而不是类型。这是因为在实践中，这些异常通常是那些已经被程序提出并捕获的异常，而不是你自己凭空raise出来的异常。

### 8. 异常使用规范

* `try`后面的`except`语句不是必须的，`finally`语句也不是必须的，但是二者必须要有一个；
* `except`语句可以有一个或多个，多个`except`会按照书写的顺序依次匹配指定的异常，如果异常已经处理就不会再进入后续的`except`语句；
* `except`语句中还可以通过元组同时指定多个异常类型进行捕获；`except`语句后面如果不指定异常类型，则默认捕获所有异常；
* 捕获异常后可以使用`raise`要再次抛出，但是不建议捕获并抛出同一个异常；
* 不建议在不清楚逻辑的情况下捕获所有异常，这可能会掩盖程序中严重的问题；
* **不要使用异常机制来处理正常业务逻辑或控制程序流程**，简单的说就是不要滥用异常机制，这是初学者常犯的错误。

## 16. [为什么Python没有自增自减运算符](https://zhuanlan.zhihu.com/p/149878104)

### 1. Python的内存管理机制

Python 的整数是不可变类型，对于变量 `i` 的内存管理在底层是通过引用计数实现的，如果我们执行了 `i=100;`，Python 不会直接修改 `100` 这个数所处内存的值，而是重新创建一个变量 `101` 并分配内存。

### 2. Python 有可迭代对象

对于`i++` 或 `++i`，其设计之初的主要目的是实现三段式的 `for` 循环，但是 Python 中没有三段式 `for` 循环的语法，取而代之的是其更为优雅的 `for-in` 循环。即使是我们想要获取迭代的下标，也可以通过 `enumrate` 实现，对于字典更是有 `keys()` 和 `values()` 这种方法。

> ### 1. `__add__()`
>
> * **定义**：`__add__(self, other)` 方法用于实现加法运算符 `+` 的行为。
> * **用法**：当你使用 `a + b` 时，Python 会自动调用 `a.__add__(b)`。
> * **返回值**：这个方法应该返回一个新的对象，而不是修改原来的对象。
>
> ``` python
> class MyNumber:
>     def __init__(self, value):
>         self.value = value
> 
>     def __add__(self, other):
>         return MyNumber(self.value + other.value)
> 
> num1 = MyNumber(10)
> num2 = MyNumber(20)
> result = num1 + num2  # 实际上调用 num1.__add__(num2)
> print(result.value)  # 输出: 30
> ```
>
> ### 2. `__iadd__()`
>
> * **定义**：`__iadd__(self, other)` 方法用于实现加法赋值运算符 `+=` 的行为。
> * **用法**：当你使用 `a += b` 时，Python 会调用 `a.__iadd__(b)`。
> * **返回值**：这个方法通常应该修改自身的状态，并返回自身，允许连锁赋值。
>
> ``` python
> class MyNumber:
>     def __init__(self, value):
>         self.value = value
> 
>     def __iadd__(self, other):
>         self.value += other.value
>         return self  # 返回自身以支持链式操作
> 
> num1 = MyNumber(10)
> num2 = MyNumber(20)
> num1 += num2  # 实际上调用 num1.__iadd__(num2)
> print(num1.value)  # 输出: 30
> ```
>
> ### 3. 主要区别
>
> * `__add__()` 通常用于返回一个新对象，而 `__iadd__()` 是用于在原地修改对象并返回自身。
> * 使用 `__add__()` 的对象通常是不可变的（immutable），而 `__iadd__()` 则通常用于可变对象（mutable）。
>
> ### 4. 注意事项
>
> * 如果你的类实现了 `__iadd__()`，通常也应该实现 `__add__()`，以确保在使用不同的加法运算时行为的一致性。
> * 你也可以在 `__add__()` 中调用 `__iadd__()`，以减少代码重复，但要确保处理不可变对象的情况。

## 17. `id()`

### 1. `id()`究竟表示什么

> Python `id()` function returns the “identity” of the object. The identity of an object is an integer, which is guaranteed to be unique and constant for this object during its lifetime. Two objects with non-overlapping lifetimes may have the same id() value. **In CPython implementation, this is the address of the object in memory.**

因此说，`id()` 在 CPython 中就表示对象object的地址。

例如 `a=24`，`b=24`，`a` 和 `b` 指向一个相同的对象 `24`，而 `a` 和 `b` 都是对 `24` 这个对象的引用，也即，`a`，`b`，`24` 的地址是一样的。

> <font color=blue>Python 中变量名都是对所指向 object 的引用。</font>

### 2. `is` 和 `==`

字符串的比较运算比较的是字符串的内容，Python 中还有一个 `is` 运算符（身份运算符），如果用 `is` 来比较两个字符串，它比较的是两个变量对应的字符串对象的内存地址，简单的说就是两个变量是否对应内存中的同一个字符串。

换言之，`a is b` 就相当于 `id(a) == id(b)`。

``` c++
s1 = 'hello world'
s2 = 'hello world'
s3 = s2

# 比较字符串的内容
print(s1 == s2, s2 == s3, s1 == s3)    # True True True

# 比较字符串的内存地址
print(s1 is s2, s2 is s3, s1 is s3)    # True True True

s2 = "Python"
# 比较字符串的内容
print(s1 == s2, s2 == s3, s1 == s3)    # False False True

# 比较字符串的内存地址
print(s1 is s2, s2 is s3, s1 is s3)    # False False True
```

 Python 中会实现创建一个**小型的整形池**，范围为 [-5,256]，为这些整形开辟好内存空间，当代码中定义该范围内的整形时，不会再重新分配内存地址。不过处于性能的考虑，但凡是<font color="blue">不可变对象</font>，在<font color="blue">同一个代码块中的对象</font>，<font color="blue">只要是值相同的对象，就不会重复创建，而是直接引用已经存在的对象。</font>

### 3. 特殊案例：切片

但是对于下面的例子，有一些特殊：

``` python
a = [1, 2, 3]
b = a[:]
c = a
print(a == b, b == c, a == c) # True True True
print(a is b, b is c, a is c) # False False True
```

这是因为在 Python 中，切片操作（如 `my_list[:]`）返回的是序列的 **浅拷贝**。这意味着返回的新对象是原始对象的一个独立副本，但对于包含可变对象的容器类型（如列表或字典）**，新对象中的元素仍然引用原始对象中的元素**。

``` C++
s1, s2, s3 = "hello", "world", "python"
a = [s1, s2, s3]
b = a[:]
print(a is b) # False
for e1, e2 in zip(a, b):
    print(e1 is e2, end=', ') # True, True, True
```

### 4. 数据类型的可变性

首先，Python3 中有六个标准的数据类型：

1. Number
2. String
3. List
4. Tuple
5. Set
6. Dictionary

其中，<font color="blue">Number，String 和 Tuple是值不可变的，List，Set 和 Dictionary 是值可变的。</font>注意这里的值的可变性，需要与内存地址联系起来，例如对于 Number 来说，我们可以这样 `a=1; a=a+2;`，看起来我们修改了 `a`，但实际上我们只是创建了一个新对象并修改 `a` 的引用罢了，本质上我们没有修改任何值。

因此说，对于**可变数据类型**的严格定义应该为：

> 当该数据类型的对应变量的值发生了改变，那么它对应的内存地址不发生改变，对于这种数据类型，就称可变数据类型。

## 18. `__str__` 和 `__repr__`

一言以蔽之，`__repr__` 适用于程序员调试，因此它的返回结果应该更准确； `__str__` 适用于最终用户，因此它的返回结果可读性应该更强。

当直接查看对象的时候调用的是 `__repr__`方法，对象需要转字符串的时候调用的是 `__str__` 方法。s

``` PYTHON
import datetime
today = datetime.datetime.today()
print(str(today))  # 2019-10-20 20:59:47.003003
print(repr(today)) # datetime.datetime(2019, 10, 20, 20, 59, 47, 3003)
```

 **每个类都最好有一个 ` __repr__ ` 方法**，**`__str__` 的默认实现就是调用`__repr__`方法。**

## 19. 缺失的 Char 类型

在 Python 中，确实没有单独的 `char` 类型，只有 `str` 类型。这是因为 Python 的设计理念追求简洁和一致性。以下是一些原因：

1. **字符串是不可变的**：在 Python 中，字符串是不可变的对象，意味着一旦创建，字符串的内容不能被更改。由于字符通常是字符串的基本构成单元，Python 选择将字符和字符串视为同一类型，即 `str`。
2. **灵活性**：`str` 类型支持任意长度的文本，因此可以用一个长度为 1 的字符串表示一个字符。这种设计让字符和字符串的处理更加一致和灵活。
3. **Unicode支持**：Python 中的字符串是 Unicode 字符串，可以表示多种语言的字符。使用 `str` 类型使得处理不同字符集变得更加简单。

总的来说，Python 通过将字符作为单字符字符串来处理，保持了语言的简洁性和一致性。

## 20. 命名规范

在 Python 中，首推<font color="blue">蛇形命名法</font>，特殊的<font color="blue">对于类名使用驼峰式命名法</font>。下面是一些对于具体情况的规范：

<left>1. 常量</left>

* 常量使用 **全大写字母**，并用 **下划线** 分隔单词。
* 常量通常放在模块的开头。

<left>2. 变量和函数</left>

* 使用 **小写字母** 和 **下划线** 分隔单词（snake_case）。
* 避免使用缩写，除非缩写是公认的或自解释的。
* 名称应当简短且有描述性，清晰传达变量或函数的用途。

<left>3. 类名</left>

* 使用 **首字母大写的驼峰式命名法（PascalCase）**。
* 类名应尽量是名词，因为类通常表示某种事物或实体。

<left>4. 模块名和包名</left>

* 使用 **全小写字母**，可以用 **下划线** 分隔单词（不过推荐用短小的单词）。

<left>5. 文档注释</left>

* 使用清晰的文档字符串 (`"""docstring"""`) 为模块、类和函数提供简要的描述，帮助解释代码的作用。
* 使用注释来解释复杂逻辑或非自明的代码段。

<left>6. 类方法</left>

* 私有成员两个下划线包围
* 受保护的成员一个下划线包围

## 21. 并发编程概述

现如今，我们使用的计算机早已是**多 CPU** 或**多核**的计算机，而我们使用的操作系统基本都支持**“多任务”**，这使得我们可以同时运行多个程序，也可以将一个程序分解为若干个相对独立的子任务，让**多个子任务**“并行”或“并发”的执行，从而缩短程序的执行时间，同时也让用户获得更好的体验。

很多时候，我们并不用严格区分并发和并行两个词，所以我们有时候也把 Python 中的多线程、多进程以及异步 I/O 都视为实现并发编程的手段，但实际上前面两者也可以实现并行编程。

### 1. daemon

Java 程序入口就是由 JVM 启动 `main` 线程，`main` 线程又可以启动其他线程。当所有线程都运行结束时，JVM 退出，进程结束。如果有一个线程没有退出，JVM 进程就不会退出。所以，必须保证所有线程都能及时结束。

但是有一种线程的目的就是无限循环，例如，一个定时触发任务的线程。如果这个线程不结束，JVM 进程就无法结束。问题是，由谁负责结束这个线程？

然而这类线程经常没有负责人来负责结束它们。但是，当其他线程结束时， JVM 进程又必须要结束，怎么办？

答案是使用守护线程（Daemon Thread）。

守护线程是指为其他线程服务的线程。在 JVM 中，所有非守护线程都执行完毕后，无论有没有守护线程，虚拟机都会自动退出。（也可以理解为守护进程此时强制结束了）最典型的守护进程有 GC（垃圾收集器）。

在守护线程中，编写代码要注意：<font color="blue">守护线程不能持有任何需要关闭的资源</font>，例如打开文件等，因为虚拟机退出时，守护线程没有任何机会来关闭文件，这会导致数据丢失。

例如：

``` Python
import threading
import time

def func():
    print("start", flush=True)
    time.sleep(4)
    print("end")


thread = threading.Thread(target=func)

thread.daemon = False # 注意这一行代码

thread.start()
thread.join(timeout=2)

print('main done')
```

> 运行顺序大致是：
>
> 1. 新线程启动，打印：`start`
> 2. 子线程 `time.sleep(4)`（总共要运行 4 秒）
> 3. 主线程调用 `join(timeout=2)`：
>    - 主线程最多等 2 秒，然后不再等，继续往下执行。
> 4. `join` 超时后，主线程打印：`finished`
> 5. 虽然主线程代码已经走完了，但 **这个线程不是 daemon**，所以 Python 解释器不会立刻退出，它会等所有 **非 daemon 线程结束** 才真正退出。
> 6. 于是再过 2 秒左右，子线程 sleep 结束，打印：`end`
>
> 所以输出一般是：
>
> ```
> start
> finished
> end
> ```
>
> 而如果我们将 `thread.deamon` 设置为 True，那么当主线程结束时，如果还有 **daemon 线程** 没跑完，解释器会 **直接杀掉整个进程**，不会等待这些 daemon 线程继续执行。
>
> 也就是说：
>
> - 子线程还在 sleep 的第 2 秒处
> - 但主线程已经结束了
> - 解释器发现只剩下 daemon 线程 → 直接退出进程
> - 子线程来不及打印 `end` 就被干掉了
>
> 所以在设置 `thread.daemon = False` 后，输出只会有：
>
> ```
> start
> finished
> ```
>
> 没有 `end`。

###  2. GIL锁

#### 2.1 GIL定义

GIL（Global Interpreter Lock）：全局解释器锁，他并不是 Python 独有的特性，他只是在实现 CPython（Python 解释器）时引入的一个概念：

> In CPython, the global interpreter lock, or GIL, is a mutex that prevents multiple native threads from executing Python bytecodes at once. This lock is necessary mainly because CPython’s memory management is not thread-safe. (However, since the GIL exists, other features have grown to depend on the guarantees that it enforces.)

由定义可知，GIL是一个互斥锁(mutex)。它阻止了多个线程同时执行 Python 字节码，毫无疑问，这降低了执行效率，使得 Python 同时最多只能有一个线程在执行。要理解 GIL 的必要性，需要先了解 CPython 对于线程安全的内存管理机制。

#### 2.2 CPython的内存管理机制

Python 使用引用计数来管理内存空间，在 Python 中创建的对象都会有引用计数，例如我们可以通过 `sys` 的 `getrefcount()` 验证：

``` python
import sys

a = [1, 2, 3, 4]
c = a
print(sys.getrefcoutn(a), getrefcoutn(c)) # 3 3
# 显示为 3 是因为 getrefcount 也引用了 [1, 2, 3, 4]
```

2.3 GIL 锁的产生和历史局限

如果多个线程同时对数据进行操作，会引发数据不一致（例如 reference counting 不一致），导致内存泄漏，我们可以对其进行加锁。但是既然有了锁，一个对象就需要一把锁，那么多个对象就会有多个锁，这又会带来新问题：

1. 死锁
2. 反复获取和释放锁导致性能降低

<font color="blue">为了确保单线程情况下 Python 的执行执行和效率，GIL 锁（单一锁）产生了</font>，它规定任何 Python 字节码的执行都需要先获取解释器锁，这样可以防止死锁，并且不会带来太多的性能开销。

通过引入 GIL 简化了在多线程情况下对 Python 内存对象的管理和实现，但因为历史原因，GIL 是在 Python 的早期版本中引入的，当时 Python 并没有专门设计为一个高性能的多线程语言。随着多核处理器的普及，多线程并行处理变的越来越重要，但 CPython 依然保持了 GIL 的存在。

#### 2.4 GIL 锁的底层原理

每一个线程在开始执行时，都会锁住 GIL，以阻止别的线程执行；同样的，每一个线程执行完一段后，会释放 GIL，以允许别的线程开始利用资源。

线程释放 GIL 锁有两种情况，一是遇到 IO 操作，二是 Time Tick 到期。

* IO 操作很好理解，比如发出一个 http 请求，等待响应。
* Time Tick 规定了线程的最长执行时间，超过时间后自动释放 GIL 锁。Python3 以后，间隔时间大致为 15 ms。

虽然都是释放 GIL 锁，但这两种情况是不一样的。比如，Thread1 遇到 IO 操作释放 GIL，由 Thread2 和 Thread3 来竞争这个 GIL 锁，Thread1 不再参与这次竞争；但如果是 Thread1 因为 Time Tick 到期释放 GIL（多数是 CPU 密集型任务），那么三个线程可以同时竞争这把 GIL 锁，可能出现 Thread1 在竞争中胜出，再次执行的情况。单核 CPU 下，这种情况不算特别糟糕。因为只有 1 个 CPU，所以 CPU 的利用率是很高的。

为了避免同一线程霸占 CPU，在 Python 3.2 版本之后，线程会自动的调整自己的优先级，使得多线程任务执行效率更高。

既然 GIL 降低了多核的效率，那保留它的目的是什么呢？这就和线程执行的安全有关。

#### 2.5 CPython GIL 不能绝对保证线程安全

``` python
def add():
    global n
    for i in range(10**1000):
        n = n +1
def sub():
    global n
    for i in range(10**1000):
        n = n - 1
n = 0
import threading
a = threading.Thread(target=add,)
b = threading.Thread(target=sub,)
a.start()
b.start()
a.join()
b.join()
print n
```

上面的程序对 n 做了同样数量的加法和减法，那么 n 理论上是 0。但运行程序，打印 n，发现它不是 0。问题出在哪里呢，问题在于<font color="blue"> python 的每行代码不是原子化的操作</font>。比如 `n = n + 1` 这步，不是一次性执行的。如果去查看 python 编译后的字节码执行过程，可以看到如下结果。

``` assembly
19 LOAD_GLOBAL              1 (n) # 加载全局变量n
22 LOAD_CONST               3 (1) # 加载常量1
25 BINARY_ADD                     # 执行二进制加法运算
26 STORE_GLOBAL             1 (n) # 将运算结果保存到n
```

从过程可以看出，`n = n + 1` 操作分成了四步完成。

根据前面的线程释放 GIL 锁原则，线程 a 执行这四步的过程中，有可能会让出 GIL。如果这样，`n = n + 1` 的运算过程就被打乱了。最后的结果中，得到一个非零的 n 也就不足为奇。

#### 2.6 解决方案

（1）**IO 密集型应用**，多线程的应用和多进程应用区别不大。即便有 GIL 存在，由于 IO 操作会导致 GIL 释放，其他线程能够获得执行权限。由于多线程的通讯成本低于多进程，因此偏向使用 <font color="blue">多线程</font>。

* 在 I/O 密集型任务中，Python 的多线程和多进程的区别不大，主要是由于任务的瓶颈不在于 CPU 的计算能力，而在于等待 I/O 操作的完成（如网络请求、文件读取等）。在这类任务中，线程和进程的切换和调度对于整体性能的影响相对较小。
* 例如多线程环境下执行 I/O 密集型任务。当一个线程因为 I/O 操作（如等待网络响应）阻塞时，GIL 会被释放，允许其他线程执行。因此，多个线程可以在 I/O 操作的等待时间内高效切换，执行其他任务，从而提升效率。

（2）**计算密集型应用**，由于 CPU 一直处于被占用状态，GIL 锁直到规定时间才会释放，然后才会切换状态，导致多线程处于绝对的劣势，此时可以采用 <font color="blue">多进程+协程。</font>

* **多进程**：使用多个进程代替线程。每个进程都有自己的 Python 解释器和 GIL，因此可以充分利用多核 CPU。

（3）**其他 Python 解释器**：一些其他的 Python 解释器，例如 Jython（Java 实现）和 IronPython（.NET 实现），没有 GIL 的限制，因为它们使用的是 Java 或 .NET 的线程管理系统。

（4）**释放 GIL**：在某些扩展模块中（如 NumPy），在执行计算密集型操作时可以临时释放 GIL，允许其他线程并行执行。

（5）**C 拓展技术：**由于 Python 是解释执行的，如果你将那些性能瓶颈代码移到一个 C 语言扩展模块中， 速度也会提升的很快。例如将计算密集型任务转移给 C，跟 Python 独立，在工作的时候在 C 代码中释放 GIL。 这可以通过在 C 代码中插入下面这样的特殊宏来完成：

```C
#include "Python.h"
...

PyObject *pyfunc(PyObject *self, PyObject *args) {
   ...
   Py_BEGIN_ALLOW_THREADS
   // Threaded C code
   ...
   Py_END_ALLOW_THREADS
   ...
}
```

如果你使用其他工具访问 C 语言，比如对于 CPython 的 ctypes 库，你不需要做任何事。 例如，ctypes 在调用 C 时会自动释放 GIL。

* C 扩展最重要的特征是它们和 Python 解释器是保持独立的。 
* 也就是说，如果你准备将 Python 中的任务分配到 C 中去执行， 你需要确保 C 代码的操作跟 Python 保持独立， 这就意味着不要使用 Python 数据结构以及不要调用 Python 的 C API。 
* 另外一个就是你要确保 C 扩展所做的工作是足够的，值得你这样做。 也就是说 C 扩展担负起了大量的计算任务，而不是少数几个计算。

## 22. 多线程编程

Python 的标准库提供了两个模块：`_thread` 和 `threading`，`_thread` 是低级模块，`threading` 是高级模块，对 `_thread` 进行了封装。绝大多数情况下，我们只需要使用 `threading` 这个高级模块。

``` python
threading.Thread(group=None, target=None, name=None, args=None, kwargs=None, deamon=None)
```

* `group`：该参数为将来扩展线程组功能而保留，当前必须始终为 `None`，否则会引发 `RuntimeError`。目前没有实际用途。
* `target`：线程启动时执行的目标函数，传递一个可调用对象（函数或方法）。当线程通过 `start()` 方法启动时，它会调用 `target` 指定的函数。如果 `target` 为 `None`，则线程不执行任何任务，除非你重载了 `Thread` 类并实现了 `run()` 方法。
* `name`：用于指定线程的名称，主要用于调试和日志记录。如果未指定，Python 将自动为线程生成一个唯一的名称，如 `"Thread-1"`、`"Thread-2"` 等。
* `args`：传递给 `target` 函数的位置参数，必须以元组形式传递。如果目标函数需要多个参数，它们可以按顺序放在 `args` 中。（注意若只有一个元素，需为`(arg1,)`而不是``(arg1)``）
* `kwargs`：传递给 `target` 函数的关键字参数，以字典形式传递。与 `args` 参数类似，允许传递命名参数给目标函数。
* `deamon`：指定线程是否为守护线程。守护线程在主线程终止后会自动结束，而非守护线程需要在程序退出前完成所有任务。默认情况下，`daemon` 属性继承自创建线程的父线程。

### 1. 自定义线程

通过继承 Thread 类并重载

``` python
import random
import time
import threading
from threading import Thread

# 重载Thread的run方法
class DownloadThread(Thread):
    def __init__(self, filename, name=None, daemon=None):
        # 1. 必须先调用 super 的 init 才能初始化当前的 init
            # 确保父类 Thread 的初始化首先完成。
            # 父类的 __init__() 方法会初始化内部的线程机制
            # 例如 self._initialized
            # 这样可以避免在子类中调用线程相关属性时出错。
        # 2. 要以关键字的形式传递参数
        super().__init__(name=name, daemon=daemon)
        self.filename = filename
        self.name = filename
    def run(self):
        print(f'[thread name]: {threading.current_thread().name}')  # 使用 current_thread
        start = time.time()
        print(f'start installing {self.filename}')
        time.sleep(random.randint(2, 4))
        print(f'{self.filename} installed successfully')
        end = time.time()
        print(f'time used: {end - start:.3f}s')


def main():
    threads = [
        DownloadThread('C++ Prime'),
        DownloadThread('Insight Java'),
        DownloadThread('Unix Kernel'),
    ]
    start = time.time()
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()

    end = time.time()
    print(f'all time used: {end - start:.3f}s')


if __name__ == '__main__':
    main()
```

### 2. 线程池

线程的创建和释放都会带来较大的开销，频繁的创建和释放线程通常都不是很好的选择。利用线程池，可以提前准备好若干个线程，在使用的过程中不需要再通过自定义的代码创建和释放线程，而是直接复用线程池中的线程。Python 内置的`concurrent.futures` 模块提供了对线程池的支持，代码如下所示。

``` python
import random
import time
from threading import Thread
from concurrent.futures import ThreadPoolExecutor

def download(*, filename):
    start = time.time()
    print(f'开始下载 {filename}.')
    time.sleep(random.randint(3, 6))
    print(f'{filename} 下载完成.')
    end = time.time()
    print(f'下载耗时: {end - start:.3f}秒.')

def main():
    start = time.time()
    with ThreadPoolExecutor(max_workers=4) as pool:
        filenames = ['C++', 'Java', 'Go']
        for filename in filenames:
            # 注意filename要以关键字形式传递
            pool.submit(download, filename=filename)
    end = time.time()
    print(f'总耗时: {end - start:.3f}秒.')


if __name__ == '__main__':
    main()
```

### 3. 死锁

CPython 自带的 GIL 并不能完全避免资源竞争，由于 python 的一个操作实际上会分解为多个原子操作，因此我们并不能保证这其中的线程安全。因此我们也需要自己手动加锁。

Python 的 threading 模块提供了 `Lock` 和 `RLock` （Reentrant Lock）类支持锁机制。

`Lock` 是 Python 中最基本的锁类型，又称为 **互斥锁（mutex）**。它有两个状态：**“锁定”**`acquire()` 和 **“解锁”**`release()`。

``` PYTHON
import threading

lock = threading.Lock()

def task():
    print(f'{threading.current_thread().name} trying to acquire lock')
    lock.acquire()
    print(f'{threading.current_thread().name} acquired lock')
    # Critical section
    lock.release()
    print(f'{threading.current_thread().name} released lock')

t1 = threading.Thread(target=task)
t2 = threading.Thread(target=task)

t1.start()
t2.start()
t1.join()
t2.join()
```

`RLock` 是 **可重入锁（reentrant lock）**，它允许**同一个线程**在不释放锁的情况下多次获得该锁。这是 `Lock` 无法做到的。

``` python
import threading
from threading import Thread
from threading import RLock

rlock = RLock()

def task():
    thread_name = threading.current_thread().name
    print(f'{thread_name} first acquire rlock...')
    rlock.acquire()
    print(f'{thread_name} second acquire rlock...')
    rlock.acquire()
    print(f'{thread_name} first release rlock')
    rlock.release()
    print(f'{thread_name} second release rlock')
    rlock.release()


t1 = Thread(target=task)
t2 = Thread(target=task)
t1.start()
t2.start()
t1.join()
t2.join()
```

在这个例子中，`RLock` 允许同一个线程多次进入临界区，并要求每次调用 `acquire()` 都对应一次 `release()`。

`RLock` 适用于递归调用或者需要多次进入临界区的场景，需要处理“递归计数”；而 `Lock` 则适用于简单的互斥场景，是一个轻量锁，不需要处理互斥场景。

> `RLock` 是 `Lock` 的超集，因此更推荐使用 `RLock`。

当然，我们也可以通过上下文语法来获取锁，从而避免了忘记释放锁以及释放锁的时机不合理的问题：

``` python
import threading
from threading import Thread
from threading import RLock
from concurrent.futures import ThreadPoolExecutor
import time

class Account:
    def __init__(self):
        self.balance = 0
        self.rlock = RLock()

    def deposit(self, money):
        with self.rlock:
            new_balance = self.balance + money
            time.sleep(0.1)
            self.balance = new_balance

def main():
    account = Account()
    with ThreadPoolExecutor(max_workers=16) as pool:
        for _ in range(100):
            pool.submit(account.deposit, 1)
    print(account.balance)


if __name__ == '__main__':
    main()
```

## 23. 多进程编程

在 Python 中可以基于 `Process` 类来创建进程，虽然进程和线程有着本质的差别，但是 `Process` 类和 `Thread` 类的用法却非常类似

``` python
class multiprocessing.Process(group=None, 
                              target=None, 
                              name=None, 
                              args=(),
                              kwargs={}, 
                              daemon=None)
```

和多线程一样，多进程的使用方法也一般为三种：

1. 直接使用 `Process` 类
2. 继承 `Process` 类
3. 使用进程池 `ProcessPoolExecutor`

下面我们以上面三种方法为例，分别比较一下多线程和多进程在执行 IO 密集型任务或者 cpu 密集型任务时的执行效率：

（1）直接使用 `Thread` 和 `Process` 类

``` python
import os
import time
import random
from threading import Thread
from multiprocessing import Process


def task(bookname, sleep_time):
    print(f'{bookname} install start')
    time.sleep(sleep_time)
    print(f'{bookname} install finished')

def test_thread():
    start = time.time()
    threads = [
        Thread(target=task, args=('Java', 1)),
        Thread(target=task, args=('CPP', 2)),
        Thread(target=task, args=('Python', 3)),
    ]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    end = time.time()
    print(f'multi thread all time uses: {end - start:.3f}')

def test_process():
    start = time.time()
    processes = [
        Process(target=task, args=('Java', 1)),
        Process(target=task, args=('CPP', 2)),
        Process(target=task, args=('Python', 3)),
    ]
    for process in processes:
        process.start()
    for process in processes:
        process.join()
    end = time.time()
    print(f'multi process all time uses: {end - start:.3f}')

def main():
    test_thread()   # 3.002
    test_process()  # 3.115


if __name__ == '__main__':
    main()
```

（2）使用 `ThreadPoolExecutor` 和 `ProcessPoolExecutor`

``` python
import os
import time
import math
import random
from threading import Thread
from multiprocessing import Process
from concurrent.futures import ThreadPoolExecutor
from concurrent.futures import ProcessPoolExecutor


def task(n):
    count = 0
    for i in range(2, n + 1):
        count += 1
        for j in range(2, int(math.sqrt(i) + 1)):
            if i % j == 0:
                count -= 1
                continue
    # print(f'from 1 to {n} have {count} prime numbers')

def test_thread(count, max_workers):
    start = time.time()
    with ThreadPoolExecutor(max_workers=max_workers) as pool:
        for _ in range(count):
            pool.submit(task, 10000)
    end = time.time()
    print(f'[multi thread] all time uses: {end - start:.3f}')

def test_process(count, max_workers):
    start = time.time()
    with ProcessPoolExecutor(max_workers=max_workers) as pool:
        for _ in range(count):
            pool.submit(task, 10000)
    end = time.time()
    print(f'[multi process] all time uses: {end - start:.3f}')


def main():
    count, max_workers = 5, 2
    test_thread(count, max_workers)   # 0.073
    test_process(count, max_workers)  # 0.165


if __name__ == '__main__':
    main()
```

## 24. 进程间通信

### 1. 引入

``` python
from multiprocessing import Process
from time import sleep
import os

counter = 0

def sub_task():
    global counter
    print('son pid: ', os.getpid(), "counter: ", counter)
    counter += 50
    print('pid: ', os.getpid(), ", counter add 50，counter: ", counter)


def main():
    global counter
    counter += 10
    print('main begin,  pid: ', os.getpid(), 'counter: ', counter) # 主进程，40
    Process(target=sub_task).start()                    # 主进程看不到子进程1对counter的修改
    print('after add 10, main, counter: ', counter)     # 40
    Process(target=sub_task).start()                    # 主进程看不到子进程2对counter的修改
    print('main done, main, counter: ', counter)     # 40


if __name__ == '__main__':
    print('global pid: ', os.getpid())      # 主进程
    print('before main, counter', counter)  # 30
    main()
    print('after main, counter', counter)   # 40
```

注意在上面的例子中，我们有三个进程，分别是主进程，和在主进程的 main 函数当中创建的两个进程，这三个进程都拥有独立的内存空间。这意味着所有在主进程中定义的变量（包括全局变量）都会被复制到子进程中。

> 也就是说，这里的 `counter=0` 会被复制到三个进程当中，他们的初始 `counter` 都为 0，并且对它的修改于其他进程不可见。

因此说，主进程对 counter 的修改对两个子进程不可见，而子进程对 counter 的修改同样对另一个子进程和主进程不可见。

> 注意这里的 main 函数只是一个普通函数，而不是像 C++ 那样作为程序的入口函数！

### 2. Queue

`multiprocessing.Queue` 提供了一个线程和进程安全的 FIFO 队列，允许进程间传递消息。

``` python
from multiprocessing import Process, Queue

def worker(q):
    q.put("Hello from worker")

if __name__ == '__main__':
    q = Queue()
    p = Process(target=worker, args=(q,))
    p.start()
    message = q.get()  # 从队列中获取消息
    p.join()
    print(message)
```

### 3. Pipe

`multiprocessing.Pipe` 提供了一个双向通信管道（任意时刻只能单向通信），允许两个进程之间发送和接收数据。

``` python
from multiprocessing import Process, Pipe

def worker(conn):
    conn.send("Hello from worker")
    conn.close()

if __name__ == '__main__':
    # Pipe()返回了包含两个连接的元组
    parent_conn, child_conn = Pipe()
    p = Process(target=worker, args=(child_conn,))
    p.start()
    message = parent_conn.recv()  # 接收消息
    p.join()
    print(message)
```

### 4. Socket

对于分布在不同主机上的进程，网络套接字 socket 也是一种有效的通信方式。

### 5. Value 和 Array

`multiprocessing.Value` 和 `multiprocessing.Array` 允许共享简单的数据结构，如整数、浮点数和数组。

``` python
from multiprocessing import Process, Value

def worker(val):
    val.value = 42

if __name__ == '__main__':
    shared_value = Value('i', 0)  # 创建共享整数
    p = Process(target=worker, args=(shared_value,))
    p.start()
    p.join()
    print(shared_value.value)  # 输出 42
```

### 6. Manager

`multiprocessing.Manager` 提供了一个可用于创建共享对象（如字典、列表等）的管理器，支持多进程之间的共享。

``` python
def worker(d):
    d['key'] = 'value'

if __name__ == '__main__':
    with Manager() as manager:
        shared_dict = manager.dict()  # 创建共享字典
        p = Process(target=worker, args=(shared_dict,))
        p.start()
        p.join()
        print(shared_dict)  # 输出 {'key': 'value'}
```

## 25. 协作式并发（异步 I/O）

爬虫属于典型的 I/O 密集型任务，其特征是程序会频繁因为网络 I/O 操作而进入阻塞状态。例如，我们使用 `requests` 来获取网页内容时，发出请求后必须等待服务器返回响应。在目标网站性能较差或网络状况不理想的情况下，这段等待时间可能会非常长，而程序在阻塞期间无法继续执行其他任务，导致大量时间被浪费。

在前面的课程中，我们已经通过多线程为爬虫提速。本质上，多线程的优势在于：**当某个线程因 I/O 阻塞时，其他线程仍然可以继续运行**，从而避免整个程序被单一阻塞点牵制。

不过，还有一种更适合 I/O 密集任务的并发方式——**异步编程**（也称作 *异步 I/O*）。与多线程或多进程不同，异步编程无需启动多个线程或进程，而是通过多个子程序（协程）之间的协作来提升 CPU 利用率。我通常称这种方式为 **“协作式并发”**。

异步 I/O 的核心思想与多线程类似：当程序因为 I/O 阻塞无法继续时，它会主动切换去执行其他任务。不过与多线程相比，异步 I/O 具备以下明显优势：

**1. 执行效率更高**：在大量 I/O 操作的场景中，异步 I/O 在遇到阻塞时只会在**同一线程内切换协程**，而多线程模型需要在**多个线程之间进行上下文切换**。
 协程切换完全发生在用户态，只需保存少量状态；线程切换则涉及内核态操作，开销更大。因此，在 I/O 密集型任务中，异步 I/O 通常能获得更高的执行效率。

**2. 不需要复杂的锁机制**：由于异步 I/O 通常运行在**单线程环境**中，各任务之间没有真正的并行运行，也就不存在传统意义上的资源竞争问题。因此，无需引入多线程编程中常见的加锁、解锁等机制，极大简化了并发逻辑。

那么，既然异步 I/O 是单线程，该如何利用多核 CPU 呢？最简单的办法就是**<font color=blue>多进程+协程</font>**，既充分利用多核，又充分发挥协程的高效率，可获得极高的性能。

### 1. 基本概念

#### 1.1 阻塞

**阻塞状态指程序未得到所需计算资源时被挂起的状态**。程序在等待某个操作完成期间，自身无法继续处理其他的事情，则称该程序在该操作上是阻塞的。阻塞随时都可能发生，最典型的就是 I/O 中断（包括网络 I/O 、磁盘 I/O 、用户输入等）、休眠操作、等待某个线程执行结束，甚至包括在 CPU 切换上下文时，程序都无法真正的执行，这就是所谓的阻塞。

#### 1.2 非阻塞

**程序在等待某操作过程中，自身不被阻塞，可以继续处理其他的事情，则称该程序在该操作上是非阻塞的。**非阻塞并不是在任何程序级别、任何情况下都可以存在的。仅当程序封装的级别可以囊括独立的子程序单元时，它才可能存在非阻塞状态。显然，某个操作的阻塞可能会导程序耗时以及效率低下，所以我们会希望把它变成非阻塞的。

#### 1.3 同步

**不同程序单元为了完成某个任务，在执行过程中需靠某种通信方式以协调一致，我们称这些程序单元是同步执行的。**例如前面讲过的给银行账户存钱的操作，我们在代码中使用了“锁”作为通信信号，让多个存钱操作强制排队顺序执行，这就是所谓的同步。

#### 1.4 异步

**不同程序单元在执行过程中无需通信协调，也能够完成一个任务，这种方式我们就称之为异步。**例如，使用爬虫下载页面时，调度程序调用下载程序后，即可调度其他任务，而无需与该下载任务保持通信以协调行为。不同网页的下载、保存等操作都是不相关的，也无需相互通知协调。很显然，异步操作的完成时刻和先后顺序并不能确定。

### 2. 生成器和协程

前面我们说过，异步编程是一种 **“协作式并发”**，即通过多个子程序相互协作的方式提升 CPU 的利用率，从而减少程序在阻塞和等待中浪费的时间，最终达到并发的效果。<font color=blue>我们可以将多个相互协作的子程序称为 **“协程”**</font>，它是实现异步编程的关键。

如何理解相互协作？看下面的例子：

``` python
def A():
    print 1
    print 2
    print 3
def B():
    print 'a'
    print 'b'
    print 'c'
A()
B()
```

调用结果毫无疑问为：

``` shell
1
2
3
a
b
c
```

但如果我们将这两个普通函数 `A` 和 `B` 修改为协程，他就有可能实现下面的输出：

``` shell
1
2
a
b
3
c
```

在执行 `A` 的过程中，随时可以中断，去执行 `B`；同理在执行 `B` 时也可以中断，再去执行 `A`；但是在 `A` 中，我们并没有调用 `B`。

----

生成器对象是由生成器函数创建的特殊对象，生成器函数就是包含 yield 的函数。我们可以通过内置函数 `next` 从生成器对象中获取值或者通过 `for-in` 循环对生成器提供的值进行遍历。

生成器经过 **预激活**，就是一个**协程**，它可以跟其他子程序合作。

``` python
def calc_average():
    total, counter = 0, 0
    avg_value = 0.0
    while True:
        current_value = yield avg_value
        counter += 1
        total += current_value
        avg_value = total / counter


def main():
    obj = calc_average() # 创建生成器对象
    print(type(obj), obj)
    obj.send(None) # 激活生成器为协程
    for _ in range(5):
        print(obj.send(int(input('pls input a number: '))))


if __name__ == '__main__':
    main()
```

上面的 `main` 函数首先通过生成器对象的 `send` 方法发送一个 `None` 值来将其激活为协程，也可以通过 `next(obj)` 达到同样的效果。接下来，协程对象会接收 `main` 函数发送的数据并产出（`yield`）数据的平均值。

> 我们可以理解为上面的例子就是 main 函数和 calc_average 函数的相互协作。

---

传统的生产者-消费者模型是一个线程写消息，一个线程取消息，通过锁机制控制队列和等待，但一不小心就可能死锁。

如果改用协程，生产者生产消息后，直接通过 `yield` 跳转到消费者开始执行，待消费者执行完毕后，切换回生产者继续生产，效率极高：

``` python
def consumer():
    r = ''
    while True:
        n = yield r
        if not n:
            return
        print('[CONSUMER] Consuming %s...' % n)
        r = '200 OK'

def produce(c):
    c.send(None)
    n = 0
    while n < 5:
        n = n + 1
        print('[PRODUCER] Producing %s...' % n)
        r = c.send(n)
        print('[PRODUCER] Consumer return: %s' % r)
    c.close()

c = consumer()
produce(c)
```

注意到 `consumer` 函数是一个 `generator`，把一个 `consumer` 传入 `produce` 后：

1. 首先调用 `c.send(None)` 启动生成器；
2. 然后，一旦生产了东西，通过 `c.send(n)` 切换到 `consumer` 执行；
3. `consumer` 通过 `yield` 拿到消息，处理，又通过 `yield` 把结果传回；
4. `produce` 拿到 `consumer` 处理的结果，继续生产下一条消息；
5. `produce` 决定不生产了，通过 `c.close()` 关闭 `consumer`，整个过程结束。

整个流程无锁，由一个线程执行，`produce` 和 `consumer`协作完成任务，所以称为“协程”，而非线程的抢占式多任务。

最后套用 Donald Knuth 的一句话总结协程的特点：

> **“子程序就是协程的一种特例。”**

### 3. 异步代码

在 Python 3.7 版本中，引入了两个关键字：

* `async`：用于定义异步函数。异步函数可能包含异步操作，异步操作的执行不会阻塞主线程。
* `await`：用于等待异步操作完成。`await` 必须在 `async` 函数内部，他会暂停 `async` 函数的执行，直到 `await` 的表达式（通常是一个协程）执行完成。

通过这两个关键字，可以简化协程代码的编写，用更简单的方式让多个子程序很好的协作起来。

看下面的例子：

``` python
import asyncio
import time

async def task(num):
    await asyncio.sleep(num)
    print(num)

async def main():
    objs = [task(i) for i in range(0, 5)]
    await asyncio.gather(*objs)


if __name__ == '__main__':
    start = time.time()
    asyncio.run(main())
    end = time.time()
    print(f'All time uses: {end - start:.3f}')
```

 ### 4. 底层原理

`async` 和 `await` 的底层实现和 **“事件循环”** 以及 **“任务队列”** 相关。可以将“事件循环”理解为一个 **“调度器”**，“任务队列” 就是被 “事件循环” 调度的数据结构。

首先我们需要注册事件循环，并添加任务。然后调度器会从任务队列中取出一个任务，如果执行到 `await`，该任务就会让出控制权，将任务挂起（从任务队列中 pop 掉），并存储其上下文，然后执行其它任务。一旦挂起的任务的事件完成，它会被重新放入任务队列中等待执行。

因此总下下来，**事件循环** 这个调度器主要做四件事：

1. 调度任务
2. 执行任务
3. 挂起任务
4. 重新激活任务

### 5. `aiohttp` 库

我们之前使用的 `requests` 三方库并不支持异步 I/O，如果希望使用异步 I/O 的方式来加速爬虫代码的执行，我们可以安装和使用名为 `aiohttp` 的三方库。

``` PYTHON
import asyncio
import re

import aiohttp
from aiohttp import ClientSession

TITLE_PATTERN = re.compile(r'<title.*?>(.*?)</title>', re.DOTALL)


async def fetch_page_title(url):
    async with aiohttp.ClientSession(headers={
        'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/95.0.4638.69 Safari/537.36',
    }) as session:  # type: ClientSession
        async with session.get(url, ssl=False) as resp:
            if resp.status == 200:
                html_code = await resp.text()
                matcher = TITLE_PATTERN.search(html_code)
                title = matcher.group(1).strip()
                print(title)


async def main():
    urls = [
        'https://www.python.org/',
        'https://www.jd.com/',
        'https://www.baidu.com/',
        'https://www.taobao.com/',
        'https://git-scm.com/',
        'https://www.sohu.com/',
        'https://gitee.com/',
        'https://www.amazon.com/',
        'https://www.usa.gov/',
        'https://www.nasa.gov/'
    ]
    objs = [fetch_page_title(url) for url in urls]
    await asyncio.gather(*objs)


if __name__ == '__main__':
    asyncio.run(main())
```

``` SHELL
淘宝
搜狐
Git
百度一下，你就知道
京东(JD.COM)-正品低价、品质保障、配送及时、轻松购物！
NASA
Welcome to Python.org
Making government services easier to find | USAGov
Amazon.com. Spend less. Smile more.
Gitee - 基于 Git 的代码托管和研发协作平台
```

从输出可以看出，网站首页标题的输出顺序跟它们的 URL 在列表中的顺序没有关系。代码的第 11 行到第 13 行创建了 `ClientSession` 对象，通过它的 `get` 方法可以向指定的 URL 发起请求，如第 14 行所示，跟 `requests` 中的 `Session` 对象并没有本质区别，唯一的区别是这里使用了异步上下文。代码第 16 行的 `await` 会让因为 I/O 操作阻塞的子程序放弃对 CPU 的占用，这使得其他的子程序可以运转起来去抓取页面。代码的第17行和第18行使用了正则表达式捕获组操作解析网页标题。 `fetch_page_title` 是一个被 `async` 关键字修饰的异步函数，调用该函数会获得协程对象，如代码第 35 行所示。

## 26. 并发编程在爬虫中的应用

单线程版本：

``` python
"""
example04.py - 单线程版本爬虫
"""
import os

import requests


def download_picture(url):
    filename = url[url.rfind('/') + 1:]
    resp = requests.get(url)
    if resp.status_code == 200:
        with open(f'images/beauty/{filename}', 'wb') as file:
            file.write(resp.content)


def main():
    if not os.path.exists('images/beauty'):
        os.makedirs('images/beauty')
    for page in range(3):
        resp = requests.get(f'https://image.so.com/zjl?ch=beauty&sn={page * 30}')
        if resp.status_code == 200:
            pic_dict_list = resp.json()['list']
            for pic_dict in pic_dict_list:
                download_picture(pic_dict['qhimg_url'])

if __name__ == '__main__':
    main()
```

多线程版本：

``` python
"""
example05.py - 多线程版本爬虫
"""
import os
from concurrent.futures import ThreadPoolExecutor

import requests


def download_picture(url):
    filename = url[url.rfind('/') + 1:]
    resp = requests.get(url)
    if resp.status_code == 200:
        with open(f'images/beauty/{filename}', 'wb') as file:
            file.write(resp.content)


def main():
    if not os.path.exists('images/beauty'):
        os.makedirs('images/beauty')
    with ThreadPoolExecutor(max_workers=16) as pool:
        for page in range(3):
            resp = requests.get(f'https://image.so.com/zjl?ch=beauty&sn={page * 30}')
            if resp.status_code == 200:
                pic_dict_list = resp.json()['list']
                for pic_dict in pic_dict_list:
                    pool.submit(download_picture, pic_dict['qhimg_url'])


if __name__ == '__main__':
    main()
```

异步IO版本：

``` python
"""
example06.py - 异步I/O版本爬虫
"""
import asyncio
import json
import os

import aiofile
import aiohttp


async def download_picture(session, url):
    filename = url[url.rfind('/') + 1:]
    async with session.get(url, ssl=False) as resp:
        if resp.status == 200:
            data = await resp.read()
            async with aiofile.async_open(f'images/beauty/{filename}', 'wb') as file:
                await file.write(data)


async def fetch_json():
    async with aiohttp.ClientSession() as session:
        for page in range(3):
            async with session.get(
                url=f'https://image.so.com/zjl?ch=beauty&sn={page * 30}',
                ssl=False
            ) as resp:
                if resp.status == 200:
                    json_str = await resp.text()
                    result = json.loads(json_str)
                    for pic_dict in result['list']:
                        await download_picture(session, pic_dict['qhimg_url'])


def main():
    if not os.path.exists('images/beauty'):
        os.makedirs('images/beauty')
    loop = asyncio.get_event_loop()
    loop.run_until_complete(fetch_json())
    loop.close()


if __name__ == '__main__':
    main()
```

## 27. 常量

在 Python 中，没有专门的关键字来定义“常量”变量（`const`）。不过，开发者通常会通过 <font color="blue">约定</font> 来标识常量。为了表明一个变量是常量，变量名通常用 <font color="blue">全大写字母</font> 书写，并且在使用时避免更改它的值。例如：

```python
PI = 3.14159
MAX_CONNECTIONS = 100
```

在上面的例子中，`PI` 和 `MAX_CONNECTIONS` 被视为常量。虽然 Python 本身不会阻止你修改这些变量的值，但按照约定，开发者不会在程序中修改它们。

如果你希望严格控制常量的行为，可以使用某些类和技术来 **模拟常量的不可变性**，例如使用 `property` 或 `namedtuple`。也可以借助第三方库，例如 `enum`。

### 方法一：enum

Python 的 `enum` 模块提供了一种创建常量集的方式。`Enum` 成员是不可变的。

```PYTHON
from enum import Enum

class Constants(Enum):
    PI = 3.14159
    MAX_CONNECTIONS = 100

print(Constants.PI.value)  # 输出 3.14159
# Constants.PI = 3.14  # 会报错：AttributeError: can't reassign members.
```

`Enum` 可以确保其成员是只读的，无法被修改。

> 这里的 `Constants` 也可以实现 C++ 的枚举类效果。

### 方法二：property

```python
class Constants:
    @property
    def PI(self):
        return 3.14159

constants = Constants()
print(constants.PI)  # 输出 3.14159
# constants.PI = 3.14  # 会报错：AttributeError: can't set attribute
```

既要保护类的封装特性，又要让开发者可以使用“对象.属性”的方式操作操作类属性，除了使用 `property()` 函数，Python 还提供了 `@property` 装饰器。通过 `@property` 装饰器，可以直接通过方法名来访问方法，不需要在方法名后添加一对小括号。

使用 `@property` 修饰了 `PI()` 方法，这样就使得该方法变成了 PI 属性的 **getter** 方法。需要注意的是，如果类中只包含该方法，那么 PI 属性将是一个**只读属性**。

在使用 `@property` 之后，如果还希望 **支持赋值（写操作）**，就需要再为同名方法加一个 `@xxx.setter` 装饰器，其中 `xxx` 必须和前面的属性名一致。

比如我们不再用常量 `PI`，而是用一个更合理的例子：圆的半径 `radius` 👇

```python
class Circle:
    def __init__(self, radius):
        # 约定：前面加下划线的是“内部使用”的属性
        self._radius = radius

    @property
    def radius(self):
        """radius 的 getter 方法"""
        return self._radius

    @radius.setter
    def radius(self, value):
        """radius 的 setter 方法"""
        if value <= 0:
            raise ValueError("半径必须为正数")
        self._radius = value


c = Circle(10)
print(c.radius)   # 调用 getter => 10

c.radius = 5      # 调用 setter，实际执行的是 radius(self, 5)
print(c.radius)   # 5

# c.radius = -1   # 会触发 ValueError: 半径必须为正数
```

这里有几个关键点：

1. `@property` 修饰的 `radius()` 是 **getter 方法**，让我们可以用 `c.radius` 来“像访问属性一样”调用方法。

2. `@radius.setter` 必须和上面的 `radius` 名字完全一致，表示这是 **同一个属性的 setter**。赋值语句：`c.radius = 5` 实际上会调用：`c.radius(5)`。

3. 如果 **只写 `@property`，不写 `@xxx.setter`**，那这个属性就是**只读属性**，对它赋值会报 `AttributeError` —— 就像你前面的 `PI` 示例那样：

   ```python
   class Constants:
       @property
       def PI(self):
           return 3.14159
   
   constants = Constants()
   print(constants.PI)   # 3.14159
   constants.PI = 3.14   # AttributeError: can't set attribute
   ```

### 方法三：fronzen

对于字典或集合类型，可以使用 `frozenset` 或者实现不可变的 `frozendict`，使其内容不可更改。

```python
# 模拟不可变集合
constants_set = frozenset(['PI', 'MAX_CONNECTIONS'])
print(constants_set)
# constants_set.add('NEW_CONST')  # 会报错：AttributeError: 'frozenset' object has no attribute 'add'

# 模拟不可变字典 (需要第三方库 frozendict)
from types import MappingProxyType

constants_dict = MappingProxyType({'PI': 3.14159, 'MAX_CONNECTIONS': 100})
print(constants_dict['PI'])  # 输出 3.14159
# constants_dict['PI'] = 3.14  # 会报错：TypeError: 'mappingproxy' object does not support item assignment
```

## `[*]`28. 为什么说 python 很慢？

https://www.reddit.com/r/Python/comments/191gmtm/why_python_is_slow_and_how_to_make_it_faster/?tl=zh-hans

https://www.kawabangga.com/posts/2979

https://www.zhihu.com/question/318030170

https://zhuanlan.zhihu.com/p/64444556
