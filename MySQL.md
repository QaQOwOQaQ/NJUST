# 参考资料

[图解 Mysql](https://www.xiaolincoding.com/mysql/)

[MySql 查询性能优化](https://segmentfault.com/a/1190000011330649)

[分库分表的几种常见形式以及可能遇到的难点](https://www.infoq.cn/article/key-steps-and-likely-problems-of-split-table)

[水平分库分表的关键步骤以及可能遇到的问题](https://www.infoq.cn/article/key-steps-and-likely-problems-of-horizontal-split-table)

[分库分表需要考虑的问题及思路](https://juejin.cn/post/6844903478171533320)

[大众点评订单系统分库分表实践](https://tech.meituan.com/2016/11/18/dianping-order-db-sharding.html)

# [[基础]元组, 码, 候选码, 主码, 外码, 主属性, 非主属性](https://javaguide.cn/database/basis.html#什么是元组-码-候选码-主码-外码-主属性-非主属性)

在关系型数据库理论中，理解元组、码、候选码、主码、外码、主属性和非主属性这些核心概念，对于数据库设计和规范化至关重要。这些概念构成了关系数据库的理论基础。

### 基础概念

- **元组（Tuple）：** 元组是关系数据库中的基本单位，在二维表中对应一行记录。每个元组包含了一个实体的完整信息。例如，在学生表中，每个学生的完整信息（学号、姓名、年龄等）构成一个元组。
- **码（Key）：** 码是能够唯一标识关系中元组的一个或多个属性的集合。码的主要作用是保证数据的唯一性和完整性。

### 码的分类

- **候选码（Candidate Key）：** 候选码是能够唯一标识元组的最小属性集合，其任何真子集都不能唯一标识元组。一个关系可能有多个候选码。例如，在学生表中，如果"学号"能唯一标识学生，同时"身份证号"也能唯一标识学生，那么{学号}和{身份证号}都是候选码。
- **主码/主键（Primary Key）：** 主码是从候选码中选择的一个，用于唯一标识关系中的元组。每个关系只能有一个主码，但可以有多个候选码。选择主码时通常考虑：简单性、稳定性、无业务含义等因素。
- **外码/外键（Foreign Key）：** 外码是一个关系中的属性或属性组，它对应另一个关系的主码。外码用于建立和维护两个关系之间的联系，是实现参照完整性的重要机制。例如，在选课表中的"学号"如果引用学生表的主码"学号"，则选课表中的"学号"就是外码。

### 属性分类

- **主属性（Prime Attribute）：** 主属性是包含在任何一个候选码中的属性。如果一个关系有多个候选码，那么这些候选码中出现的所有属性都是主属性。例如，工人关系（工号，身份证号，姓名，性别，部门）中，如果{工号}和{身份证号}都是候选码，那么"工号"和"身份证号"都是主属性。
- **非主属性（Non-prime Attribute）：** 非主属性是不包含在任何候选码中的属性。这些属性完全依赖于候选码来确定其值。在上述工人关系中，"姓名"、"性别"、"部门"都是非主属性。

# [基础]DBMS

## #DBMS 是什么

DBMS 全称是**数据库管理系统**（Database Management System）。<font color=blue>简单来说，它是介于你的应用程序和底层物理数据文件之间的一个“大管家”软件。</font>

它的核心价值可以概括为以下三点：

- **提供标准操作接口**：你的代码不需要自己去造轮子读写硬盘上的文件，而是直接发送指令（比如 SQL）给 DBMS，由它去高效完成数据的增删改查。
- **兜底数据安全与一致性**：当面临高并发访问，或者系统突然宕机时，DBMS 会通过锁机制和事务（ACID 特性）来保证数据不乱、不丢。
- **底层的性能优化**：它内部自带查询优化器、缓存和索引机制，能自动规划出最快的数据提取路径。

市面上大家常用的 MySQL、PostgreSQL（关系型），以及 MongoDB、Redis（非关系型），本质上都是不同侧重点的 DBMS。

## #DBMS 提供的功能

DBMS 通常提供四大核心功能：

1. **<font color=blue>数据定义：</font>** 这是 DBMS 的基础。它提供了一套数据定义语言（Data Definition Language - DDL），让我们能够创建、修改和删除数据库中的各种对象。这不仅仅是定义表的结构（比如字段名、数据类型），还包括定义视图、索引、触发器、存储过程等。
2. **<font color=blue>数据操作：</font>** 这是我们作为开发者日常使用最多的功能。它提供了一套数据操作语言（Data Manipulation Language - DML），核心就是我们熟悉的增、删、改、查（CRUD）操作。它让我们能够方便地对数据库中的数据进行操作和检索。
3. **<font color=blue>数据控制：</font>** 这是保证数据正确、安全、可靠的关键。通常包含并发控制、事务管理、完整性约束、权限控制、安全性限制等功能。
4. **<font color=blue>数据库维护：</font>** 这部分功能是为了保障数据库系统的长期稳定运行。它包括了数据的导入导出、数据库的备份与恢复、性能监控与分析、以及系统日志管理等。

## #[你知道哪些类型的 DBMS？](https://javaguide.cn/database/basis.html#你知道哪些类型的-dbms)

### 关系型数据库

除了我们最常用的关系型数据库（RDBMS），比如 MySQL（开源首选）、PostgreSQL（功能最全）、Oracle（企业级），它们基于严格的表结构和 SQL，非常适合结构化数据和需要事务保证的场景，例如银行交易、订单系统。

近年来，为了应对互联网应用带来的海量数据、高并发和多样化数据结构的需求，涌现出了一大批 NoSQL 和 NewSQL 数据库。

### NoSQL 数据库

它们的共同特点是为了极致的性能和水平扩展能力，在某些方面（通常是事务）做了妥协。

**1. 键值数据库，代表是 Redis。**

- **特点：** 数据模型极其简单，就是一个巨大的 Map，通过 Key 来存取 Value。内存操作，性能极高。
- **适用场景：** 非常适合做缓存、会话存储、计数器等对读写性能要求极高的场景。

**2. 文档数据库，代表是 MongoDB。**

- **特点：** 它存储的是半结构化的文档（比如 JSON/BSON），结构灵活，不需要预先定义表结构。
- **适用场景：** 特别适合那些数据结构多变、快速迭代的业务，比如用户画像、内容管理系统、日志存储等。

**3. 列式数据库，代表是 HBase, Cassandra。**

- **特点：** 数据是按列族而不是按行来存储的。这使得它在对大量行进行少量列的读取时，性能极高。
- **适用场景：** 专为海量数据存储和分析设计，非常适合做大数据分析、监控数据存储、推荐系统等需要高吞吐量写入和范围扫描的场景。

**4. 图形数据库，代表是 Neo4j。**

- **特点：** 数据模型是节点（Nodes）和边（Edges），专门用来存储和查询实体之间的复杂关系。
- **适用场景：** 在社交网络（好友关系）、推荐引擎（用户-商品关系）、知识图谱、欺诈检测（资金流动关系）等场景下，表现远超关系型数据库。

### NewSQL 数据库

由于 NoSQL 通常不支持事务，很多对于数据安全要求非常高的系统（比如财务系统、订单系统、交易系统）就不太适合使用了。不过，这类系统往往有存储大量数据的需求。

这些系统往往只能通过购买性能更强大的计算机，或者通过数据库中间件来提高存储能力。不过，前者的金钱成本太高，后者的开发成本太高。

于是，**NewSQL** 就来了！

简单来说，<font color=blue>NewSQL 就是：**分布式存储+SQL+事务** 。</font>NewSQL 不仅具有 NoSQL 对海量数据的存储管理能力，还保持了传统数据库支持 ACID 和 SQL 等特性。因此，NewSQL 也可以称为 **分布式关系型数据库**。

NewSQL 数据库设计的一些目标：

1. 横向扩展（Scale Out） ： 通过增加机器的方式来提高系统的负载能力。与之类似的是 Scale Up(纵向扩展)，升级硬件设备的方式来提高系统的负载能力。
2. 强一致性（Strict Consistency）：在任意时刻，所有节点中的数据是一样的。
3. 高可用（High Availability）：系统几乎可以一直提供服务。
4. 支持标准 SQL（Structured Query Language） ：PostgreSQL、MySQL、Oracle 等关系型数据库都支持 SQL 。
5. 事务（ACID） ： 原子性（Atomicity）、一致性（Consistency）、 隔离性（Isolation）; 持久性（Durability）。
6. 兼容主流关系型数据库 ： 兼容 MySQL、Oracle、PostgreSQL 等常用关系型数据库。
7. 云原生 （Cloud Native）：可在公有云、私有云、混合云中实现部署工具化、自动化。
8. HTAP（Hybrid Transactional/Analytical Processing） ：支持 OLTP 和 OLAP 混合处理。

NewSQL 数据库代表：Google 的 F1/Spanner、阿里的 [OceanBase](https://open.oceanbase.com/)、PingCAP 的 [TiDB](https://pingcap.com/zh/product-community/) 。

# [基础]主键和外键有什么区别?

从定义和属性上看，它们的区别是：

- **主键 (Primary Key):** 它的核心作用是唯一标识表中的每一行数据。因此，<font color=24b3aa>主键列的值必须是唯一的 (Unique) 且不能为空 (Not Null)。</font>一张表只能有一个主键。主键保证了实体完整性。
- **外键 (Foreign Key):** 它的核心作用是建立并强制两张表之间的关联关系。一张表中的外键列，其值必须对应另一张表中某行的候选键值（通常是主键，也可以是唯一键），或者是一个 NULL 值。因此，<font color=24b3aa>外键的值可以重复，也可以为空。一张表可以有多个外键，分别关联到不同的表。</font>外键保证了引用完整性。

用一个简单的电商例子来说明：假设我们有两张表：`users` (用户表) 和 `orders` (订单表)。

- 在 `users` 表中，`user_id` 列是**主键**。每个用户的 `user_id` 都是独一无二的，我们用它来区分张三和李四。
- 在 `orders` 表中，`order_id` 是它自己的**主键**。同时，它会有一个 `user_id` 列，这个列就是一个**外键**，它引用了 `users` 表的 `user_id` 主键。

这个外键约束就保证了：

1. 你不能创建一个不属于任何已知用户的订单（ `user_id` 在 `users` 表中不存在）。
2. 你不能删除一个已经下了订单的用户（除非设置了级联删除等特殊规则）。

# [基础]自增主键一定是连续的吗

> https://javaguide.cn/database/mysql/mysql-auto-increment-primary-key-continuous.html

众所周知，自增主键可以让聚集索引尽量地保持递增顺序插入，避免了随机查询，从而提高了查询效率。

<font color=blue>但实际上，MySQL 的自增主键并不能保证一定是连续递增的。</font>

下面举个例子来看下，如下所示创建一张表：

![img](https://oss.javaguide.cn/p3-juejin/3e6b80ba50cb425386b80924e3da0d23~tplv-k3u1fbpfcp-zoom-1.png)

## 自增值保存在哪里？

使用 `insert into test_pk values(null, 1, 1)` 插入一行数据，再执行 `show create table` 命令来看一下表的结构定义：

![img](https://oss.javaguide.cn/p3-juejin/c17e46230bd34150966f0d86b2ad5e91~tplv-k3u1fbpfcp-zoom-1.png)

上述表的结构定义存放在后缀名为 `.frm` 的本地文件中，在 MySQL 安装目录下的 data 文件夹下可以找到这个 `.frm` 文件：

![img](https://oss.javaguide.cn/p3-juejin/3ec0514dd7be423d80b9e7f2d52f5902~tplv-k3u1fbpfcp-zoom-1.png)

从上述表结构可以看到，表定义里面出现了一个 `AUTO_INCREMENT=2`，表示下一次插入数据时，如果需要自动生成自增值，会生成 id = 2。

但需要注意的是，自增值并不会保存在这个表结构也就是 `.frm` 文件中，不同的引擎对于自增值的保存策略不同：

1）MyISAM 引擎的自增值保存在数据文件中

2）InnoDB 引擎的自增值，其实是保存在了内存里，并没有持久化。第一次打开表的时候，都会去找自增值的最大值 `max(id)`，然后将 `max(id)+1` 作为这个表当前的自增值。

举个例子：我们现在表里当前数据行里最大的 id 是 1，AUTO_INCREMENT=2，对吧。这时候，我们删除 id=1 的行，AUTO_INCREMENT 还是 2。

![img](https://oss.javaguide.cn/p3-juejin/61b8dc9155624044a86d91c368b20059~tplv-k3u1fbpfcp-zoom-1.png)

但如果马上重启 MySQL 实例，重启后这个表的 AUTO_INCREMENT 就会变成 1。﻿ 也就是说，<font color=blue>MySQL 重启可能会修改一个表的 AUTO_INCREMENT 的值。</font>

![img](https://oss.javaguide.cn/p3-juejin/27fdb15375664249a31f88b64e6e5e66~tplv-k3u1fbpfcp-zoom-1.png)

![img](https://oss.javaguide.cn/p3-juejin/dee15f93e65d44d384345a03404f3481~tplv-k3u1fbpfcp-zoom-1.png)

以上，是在我本地 MySQL 5.x 版本的实验，<font color=blue>实际上，**到了 MySQL 8.0 版本后，自增值的变更记录被放在了 redo log 中，提供了自增值持久化的能力** ，也就是实现了“如果发生重启，表的自增值可以根据 redo log 恢复为 MySQL 重启前的值”</font>

也就是说对于上面这个例子来说，重启实例后这个表的 AUTO_INCREMENT 仍然是 2。

理解了 MySQL 自增值到底保存在哪里以后，我们再来看看自增值的修改机制，并以此引出第一种自增值不连续的场景。

## 自增值不连续的场景

### 自增值不连续场景 1

在 MySQL 里面，如果字段 id 被定义为 AUTO_INCREMENT，在插入一行数据的时候，自增值的行为如下：

- 如果插入数据时 id 字段指定为 0、null 或未指定值，那么就把这个表当前的 AUTO_INCREMENT 值填到自增字段；
- 如果插入数据时 id 字段指定了具体的值，就直接使用语句里指定的值。

根据要插入的值和当前自增值的大小关系，自增值的变更结果也会有所不同。假设某次要插入的值是 `insert_num`，当前的自增值是 `autoIncrement_num`：

- 如果 `insert_num < autoIncrement_num`，那么这个表的自增值不变
- 如果 `insert_num >= autoIncrement_num`，就需要把当前自增值修改为新的自增值

也就是说，如果插入的 id 是 100，当前的自增值是 90，`insert_num >= autoIncrement_num`，那么自增值就会被修改为新的自增值即 101

一定是这样吗？

非也~

了解过分布式 id 的小伙伴一定知道，为了避免两个库生成的主键发生冲突，我们可以让一个库的自增 id 都是奇数，另一个库的自增 id 都是偶数

这个奇数偶数其实是通过 `auto_increment_offset` 和 `auto_increment_increment` 这两个参数来决定的，这俩分别用来表示自增的初始值和步长，默认值都是 1。

所以，上面的例子中生成新的自增值的步骤实际是这样的：从 `auto_increment_offset` 开始，以 `auto_increment_increment` 为步长，持续叠加，直到找到第一个大于 100 的值，作为新的自增值。

所以，这种情况下，自增值可能会是 102，103 等等之类的，就会导致不连续的主键 id。

更遗憾的是，即使在自增初始值和步长这两个参数都设置为 1 的时候，自增主键 id 也不一定能保证主键是连续的

### 自增值不连续场景 2

举个例子，我们现在往表里插入一条 (null,1,1) 的记录，生成的主键是 1，AUTO_INCREMENT= 2，对吧

![img](https://oss.javaguide.cn/p3-juejin/c22c4f2cea234c7ea496025eb826c3bc~tplv-k3u1fbpfcp-zoom-1.png)

这时我再执行一条插入 `(null,1,1)` 的命令，很显然会报错 `Duplicate entry`，因为我们设置了一个唯一索引字段 `a`：

![img](https://oss.javaguide.cn/p3-juejin/c0325e31398d4fa6bb1cbe08ef797b7f~tplv-k3u1fbpfcp-zoom-1.png)

但是，你会惊奇的发现，虽然插入失败了，但自增值仍然从 2 增加到了 3！

这是为啥？

我们来分析下这个 insert 语句的执行流程：

1. 执行器调用 InnoDB 引擎接口准备插入一行记录 (null,1,1);
2. InnoDB 发现用户没有指定自增 id 的值，则获取表 `test_pk` 当前的自增值 2；
3. 将传入的记录改成 (2,1,1);
4. 将表的自增值改成 3；
5. 继续执行插入数据操作，由于已经存在 a=1 的记录，所以报 Duplicate key error，语句返回

可以看到，自增值修改的这个操作，是在真正执行插入数据的操作之前。

这个语句真正执行的时候，因为碰到唯一键 a 冲突，所以 id = 2 这一行并没有插入成功，但也没有将自增值再改回去。所以，在这之后，再插入新的数据行时，拿到的自增 id 就是 3。也就是说，出现了自增主键不连续的情况。

至此，我们已经罗列了两种自增主键不连续的情况：

1. 自增初始值和自增步长设置不为 1
2. 唯一键冲突

除此之外，事务回滚也会导致这种情况

### 自增值不连续场景 3

我们现在表里有一行 `(1,1,1)` 的记录，AUTO_INCREMENT = 3：

![img](https://oss.javaguide.cn/p3-juejin/6220fcf7dac54299863e43b6fb97de3e~tplv-k3u1fbpfcp-zoom-1.png)

我们先插入一行数据 `(null, 2, 2)`，也就是 (3, 2, 2) 嘛，并且 AUTO_INCREMENT 变为 4：

![img](https://oss.javaguide.cn/p3-juejin/3f02d46437d643c3b3d9f44a004ab269~tplv-k3u1fbpfcp-zoom-1.png)

再去执行这样一段 SQL：

![img](https://oss.javaguide.cn/p3-juejin/faf5ce4a2920469cae697f845be717f5~tplv-k3u1fbpfcp-zoom-1.png)

虽然我们插入了一条 (null, 3, 3) 记录，但是使用 rollback 进行回滚了，所以数据库中是没有这条记录的：

![img](https://oss.javaguide.cn/p3-juejin/6cb4c02722674dd399939d3d03a431c1~tplv-k3u1fbpfcp-zoom-1.png)

在这种事务回滚的情况下，自增值并没有同样发生回滚！如下图所示，自增值仍然固执地从 4 增加到了 5：

![img](https://oss.javaguide.cn/p3-juejin/e6eea1c927424ac7bda34a511ca521ae~tplv-k3u1fbpfcp-zoom-1.png)

所以这时候我们再去插入一条数据（null, 3, 3）的时候，主键 id 就会被自动赋为 `5` 了：

![img](https://oss.javaguide.cn/p3-juejin/80da69dd13b543c4a32d6ed832a3c568~tplv-k3u1fbpfcp-zoom-1.png)

那么，为什么在出现唯一键冲突或者回滚的时候，MySQL 没有把表的自增值改回去呢？回退回去的话不就不会发生自增 id 不连续了吗？

事实上，这么做的主要原因是为了提高性能。

我们直接用反证法来验证：假设 MySQL 在事务回滚的时候会把自增值改回去，会发生什么？

现在有两个并行执行的事务 A 和 B，在申请自增值的时候，为了避免两个事务申请到相同的自增 id，肯定要加锁，然后顺序申请，对吧。

1. 假设事务 A 申请到了 id = 1， 事务 B 申请到 id=2，那么这时候表 t 的自增值是 3，之后继续执行。
2. 事务 B 正确提交了，但事务 A 出现了唯一键冲突，也就是 id = 1 的那行记录插入失败了，那如果允许事务 A 把自增 id 回退，也就是把表的当前自增值改回 1，那么就会出现这样的情况：表里面已经有 id = 2 的行，而当前的自增 id 值是 1。
3. 接下来，继续执行的其他事务就会申请到 id=2。这时，就会出现插入语句报错“主键冲突”。

![img](https://oss.javaguide.cn/p3-juejin/5f26f02e60f643c9a7cab88a9f1bdce9~tplv-k3u1fbpfcp-zoom-1.png)

而为了解决这个主键冲突，有两种方法：

1. 每次申请 id 之前，先判断表里面是否已经存在这个 id，如果存在，就跳过这个 id
2. 把自增 id 的锁范围扩大，必须等到一个事务执行完成并提交，下一个事务才能再申请自增 id

很显然，上述两个方法的成本都比较高，会导致性能问题。而究其原因呢，是我们假设的这个 “允许自增 id 回退”。

因此，InnoDB 放弃了这个设计，语句执行失败也不回退自增 id。也正是因为这样，所以才只保证了自增 id 是递增的，但不保证是连续的。

综上，已经分析了三种自增值不连续的场景，还有第四种场景：批量插入数据。

### 自增值不连续场景 4

对于批量插入数据的语句，MySQL 有一个批量申请自增 id 的策略：

1. 语句执行过程中，第一次申请自增 id，会分配 1 个；
2. 1 个用完以后，这个语句第二次申请自增 id，会分配 2 个；
3. 2 个用完以后，还是这个语句，第三次申请自增 id，会分配 4 个；
4. 依此类推，同一个语句去申请自增 id，每次申请到的自增 id 个数都是上一次的两倍。

注意，这里说的批量插入数据，不是在普通的 insert 语句里面包含多个 value 值！！！，因为这类语句在申请自增 id 的时候，是可以精确计算出需要多少个 id 的，然后一次性申请，申请完成后锁就可以释放了。

而对于 `insert … select`、replace …… select 和 load data 这种类型的语句来说，MySQL 并不知道到底需要申请多少 id，所以就采用了这种批量申请的策略，毕竟一个一个申请的话实在太慢了。

举个例子，假设我们现在这个表有下面这些数据：

![img](https://oss.javaguide.cn/p3-juejin/6453cfc107f94e3bb86c95072d443472~tplv-k3u1fbpfcp-zoom-1.png)

我们创建一个和当前表 `test_pk` 有相同结构定义的表 `test_pk2`：

![img](https://oss.javaguide.cn/p3-juejin/45248a6dc34f431bba14d434bee2c79e~tplv-k3u1fbpfcp-zoom-1.png)

然后使用 `insert...select` 往 `teset_pk2` 表中批量插入数据：

![img](https://oss.javaguide.cn/p3-juejin/c1b061e86bae484694d15ceb703b10ca~tplv-k3u1fbpfcp-zoom-1.png)

可以看到，成功导入了数据。

再来看下 `test_pk2` 的自增值是多少：

![img](https://oss.javaguide.cn/p3-juejin/0ff9039366154c738331d64ebaf88d3b~tplv-k3u1fbpfcp-zoom-1.png)

如上分析，是 8 而不是 6

具体来说，insert……select 实际上往表中插入了 5 行数据 （1 1）（2 2）（3 3）（4 4）（5 5）。但是，这五行数据是分三次申请的自增 id，结合批量申请策略，每次申请到的自增 id 个数都是上一次的两倍，所以：

- 第一次申请到了一个 id：id=1
- 第二次被分配了两个 id：id=2 和 id=3
- 第三次被分配到了 4 个 id：id=4、id = 5、id = 6、id=7

由于这条语句实际只用上了 5 个 id，所以 id=6 和 id=7 就被浪费掉了。之后，再执行 `insert into test_pk2 values(null,6,6)`，实际上插入的数据就是（8,6,6)：

![img](https://oss.javaguide.cn/p3-juejin/51612fbac3804cff8c5157df21d6e355~tplv-k3u1fbpfcp-zoom-1.png)

## [小结](https://javaguide.cn/database/mysql/mysql-auto-increment-primary-key-continuous.html#小结)

本文总结下自增值不连续的 4 个场景：

1. 自增初始值和自增步长设置不为 1
2. 唯一键冲突
3. 事务回滚
4. 批量插入（如 `insert...select` 语句）

# [基础]内连接 `INNER JOIN` 内部是怎么实现的？

在数据库底层，`INNER JOIN` 并不是一种单一的固定操作。数据库的**查询优化器**（Optimizer）会像一个聪明的导航软件，根据你的表大小、有没有建索引、内存够不够等情况，自动从底层算法中挑选最快的一条路。

一般来说，数据库内部实现 `INNER JOIN` 核心靠以下 **3 种算法**：

**<font color=blue>1. 嵌套循环连接（Nested Loop Join）：最简单粗暴</font>** 你可以把它理解为代码里的 `for` 循环嵌套。数据库会先选一张表作为“外表”，然后逐行遍历；每扫到一行，就去另一张“内表”里把所有数据扫一遍，看看有没有匹配的。

- **适用场景：** 一张表极小，另一张表很大且连接字段上有**索引**（Index Nested Loop）。这样内表就不需要全表扫描，而是直接通过索引快速定位，效率会飙升。

**<font color=blue>2. 哈希连接（Hash Join）：大数据量的杀手锏</font>** 当两张表都比较大，且没有合适的索引时，数据库会祭出大招。它分为两步：首先，它会把比较小的那张表读进内存，根据连接字段算出一个“哈希表”（类似字典）。然后，它再去扫描大表，把大表的连接字段也算一下哈希值，直接去内存的“字典”里查。

- **适用场景：** 数据量大、没有索引的等值连接（即使用 `=` 连接）。它的速度极快，代价是比较消耗 CPU 和内存。

**<font color=blue>3. 排序合并连接（Sort-Merge Join）：井然有序</font>** 如果两张表的数据刚好都已经按连接字段**排好序**了，数据库就会用两根“指针”分别指着两张表的第一行，往下比对。谁小，谁的指针就往下走一步；如果相等，就匹配成功并一起往下走。

- **适用场景：** 数据本身已经有序（比如连接字段本来就是主键或有索引），或者两张大表需要做非等值连接（如 `<` 或 `>`）时非常高效。

----

假设我们有以下两张表：

- **用户表 (Users):** 有 3 条记录（ID：1张三，2李四，3王五）。
- **订单表 (Orders):** 有 3 条记录（UserID：2买手机，1买电脑，2买耳机）。

我们要执行的 SQL 是：`把两张表按 ID 连起来，找出谁买了什么。`

以下是 3 种底层算法的实际执行动作：

**1. 嵌套循环连接（最原始的“双重 for 循环”）**

数据库会死磕到底，拿左边的人去右边挨个问。

- 拿“1号张三”，去订单表从头问到尾：2号的手机（不是他）、1号的电脑（**匹配成功！**）、2号的耳机（不是他）。
- 拿“2号李四”，去订单表从头问到尾：2号的手机（**匹配成功！**）、1号的电脑（不是他）、2号的耳机（**匹配成功！**）。
- 拿“3号王五”，去订单表扫一圈：发现什么都没买。 **代价：** 3个人 × 3个订单 = 扫描比对 **9 次**。如果两张表都有 1 万行，就要比对 1 亿次，非常慢。

**2. 哈希连接（建“字典”快速查）**

数据库嫌每次扫表太慢，决定先花点内存整理一下。

- **第一步（建字典）：** 把较小的“用户表”放进内存，按 ID 做成一个速查字典：`{1:张三, 2:李四, 3:王五}`。
- **第二步（查字典）：** 开始扫描“订单表”。
  - 看到“2号买手机”，直接查内存字典，2号是李四，**瞬间匹配！**
  - 看到“1号买电脑”，直接查字典，1号是张三，**瞬间匹配！**
  - 看到“2号买耳机”，再查字典，2号是李四，**瞬间匹配！** **代价：** 用户表扫 3 次建字典，订单表扫 3 次查字典，一共只扫描 **6 次**。数据量越大，它的速度优势就越碾压。

**3. 排序合并连接（像“拉链”一样咬合）**

当发现数据太多内存装不下字典时，数据库会要求双方先排好队。

- **第一步（各自排队）：** 用户表已经是 1、2、3 顺序了；数据库把订单表也强行按 UserID 排序，变成：`1号电脑、2号手机、2号耳机`。
- **第二步（拉链咬合）：** 双方的排头兵开始比对。
  - 1号张三 对 1号电脑（**匹配！**）。
  - 接下来，张三遇到 2号手机，发现 1<2，说明后面没有自己的订单了，张三退场，换 2号李四。
  - 2号李四 对上 2号手机（**匹配！**），接着往下走遇到 2号耳机（**匹配！**）。 **代价：** 提前排好序后，两边只需顺着往下走一遍，谁也不走回头路。非常适合本身就建了索引（自带顺序）的大表。

# [[基础] ER 图](https://javaguide.cn/database/basis.html#什么是-er-图)

我们做一个项目的时候一定要试着画 ER 图来捋清数据库设计，这个也是面试官问你项目的时候经常会被问到的。

<font color=blue>**ER 图** 全称是 Entity Relationship Diagram（实体联系图）</font>，提供了表示实体类型、属性和联系的方法。

ER 图由下面 3 个要素组成：

- **<font color=blue>实体</font>**：通常是现实世界的业务对象，当然使用一些逻辑对象也可以。比如对于一个校园管理系统，会涉及学生、教师、课程、班级等等实体。在 ER 图中，实体使用矩形框表示。
- **<font color=blue>属性</font>**：即某个实体拥有的属性，属性用来描述组成实体的要素，对于产品设计来说可以理解为字段。在 ER 图中，属性使用椭圆形表示。
- **<font color=blue>联系</font>**：即实体与实体之间的关系，在 ER 图中用菱形表示，这个关系不仅有业务关联关系，还能通过数字表示实体之间的数量对照关系。例如，一个班级会有多个学生就是一种实体间的联系。

![ER Graph](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.2vfgngo2fg.png)

# [基础]想查看一张表的所有表结构，用什么 SQL 语句？

查看表结构，最常用的是以下两种方法（以最常见的 MySQL 为例）：

**<font color=red>1. 最直观的“骨架”：`DESC`</font>**

如果你只想快速看一眼表里有哪些字段、数据类型、是否允许为空、是不是主键，用这个最省事。

```sql
DESC TableName;
-- 或者写全：DESCRIBE TableName;
```

**<font color=red>2. 最底层的“源码”：`SHOW CREATE TABLE`</font>**

如果你不仅想看字段，还想查看详细的**索引定义**、表备注、字符集和存储引擎等完整的底层建表 DDL 语句，用这个最准。

```sql
SHOW CREATE TABLE TableName;
```

# [基础]字符集详解

> https://javaguide.cn/database/character-set.html

MySQL 字符编码集中有两套 UTF-8 编码实现：**`utf8`** 和 **`utf8mb4`**。

如果使用 **`utf8`** 的话，存储 emoji 符号和一些比较复杂的汉字、繁体字就会出错。

为什么会这样呢？这篇文章可以从源头给你解答。

## 字符集是什么？

字符是各种文字和符号的统称，包括各个国家文字、标点符号、表情、数字等等。 **字符集** 就是一系列字符的集合。字符集的种类较多，每个字符集可以表示的字符范围通常不同，就比如说有些字符集是无法表示汉字的。

**计算机只能存储二进制的数据，那英文、汉字、表情等字符应该如何存储呢？**

我们要将这些字符和二进制的数据一一对应起来，比如说字符“a”对应“01100001”，反之，“01100001”对应 “a”。<font color=blue>我们将字符对应二进制数据的过程称为"**字符编码**"，反之，二进制数据解析成字符的过程称为“**字符解码**”。</font>

## 字符编码是什么？

字符编码是一种将字符集中的字符与计算机中的二进制数据相互转换的方法，可以看作是一种映射规则。也就是说，字符编码的目的是为了让计算机能够存储和传输各种文字信息。

每种字符集都有自己的字符编码规则，常用的字符集编码规则有 ASCII 编码、 GB2312 编码、GBK 编码、GB18030 编码、Big5 编码、UTF-8 编码、UTF-16 编码等。

## 有哪些常见的字符集？

常见的字符集有：ASCII、GB2312、GB18030、GBK、Unicode……。

<font color=blue>不同的字符集的主要区别在于：</font>

- <font color=blue>可以表示的字符范围</font>
- <font color=blue>编码方式</font>

### ASCII

**ASCII** (**A**merican **S**tandard **C**ode for **I**nformation **I**nterchange，美国信息交换标准代码) 是一套主要用于现代美国英语的字符集（这也是 ASCII 字符集的局限性所在）。

**为什么 ASCII 字符集没有考虑到中文等其他字符呢？** 因为计算机是美国人发明的，当时，计算机的发展还处于比较雏形的时代，还未在其他国家大规模使用。因此，美国发布 ASCII 字符集的时候没有考虑兼容其他国家的语言。

ASCII 字符集至今为止共定义了 128 个字符，其中有 33 个控制字符（比如回车、删除）无法显示。

一个 ASCII 码长度是一个字节也就是 8 个 bit，比如“a”对应的 ASCII 码是“01100001”。不过，最高位是 0 仅仅作为校验位，其余 7 位使用 0 和 1 进行组合，所以，ASCII 字符集可以定义 128（2^7）个字符。

由于，ASCII 码可以表示的字符实在是太少了。后来，人们对其进行了扩展得到了 **ASCII 扩展字符集** 。ASCII 扩展字符集使用 8 位（bits）表示一个字符，所以，ASCII 扩展字符集可以定义 256（2^8）个字符。

![ASCII字符编码](https://oss.javaguide.cn/github/javaguide/csdn/c1c6375d08ca268690cef2b13591a5b4.png)

### GB2312

我们上面说了，ASCII 字符集是一种现代美国英语适用的字符集。因此，很多国家都捣鼓了一个适合自己国家语言的字符集。

GB2312 字符集是一种对汉字比较友好的字符集，共收录 6700 多个汉字，基本涵盖了绝大部分常用汉字。不过，GB2312 字符集不支持绝大部分的生僻字和繁体字。

对于英语字符，GB2312 编码和 ASCII 码是相同的，1 字节编码即可。对于非英字符，需要 2 字节编码。

### GBK

GBK 字符集可以看作是 GB2312 字符集的扩展，兼容 GB2312 字符集，共收录了 20000 多个汉字。

GBK 中 K 是汉语拼音 Kuo Zhan（扩展）中的“Kuo”的首字母。

### GB18030

GB18030 完全兼容 GB2312 和 GBK 字符集，纳入中国国内少数民族的文字，且收录了日韩汉字，是目前为止最全面的汉字字符集，共收录汉字 70000 多个。

### BIG5

BIG5 主要针对的是繁体中文，收录了 13000 多个汉字。

### Unicode & UTF-8

为了更加适合本国语言，诞生了很多种字符集。

我们上面也说了不同的字符集可以表示的字符范围以及编码规则存在差异。这就导致了一个非常严重的问题：**使用错误的编码方式查看一个包含字符的文件就会产生乱码现象。**

就比如说你使用 UTF-8 编码方式打开 GB2312 编码格式的文件就会出现乱码。示例：“牛”这个汉字 GB2312 编码后的十六进制数值为 “C5A3”，而 “C5A3” 用 UTF-8 解码之后得到的却是 “ţ”。

你可以通过这个网站在线进行编码和解码：https://www.haomeili.net/HanZi/ZiFuBianMaZhuanHuan

![img](https://oss.javaguide.cn/github/javaguide/csdn/836c49b117ee4408871b0020b74c991d.png)

这样我们就搞懂了乱码的本质：**编码和解码时用了不同或者不兼容的字符集** 。

![img](https://oss.javaguide.cn/javaguide/a8808cbabeea49caa3af27d314fa3c02-1.jpg)

为了解决这个问题，人们就想：“如果我们能够有一种字符集将世界上所有的字符都纳入其中就好了！”。

然后，**Unicode** 带着这个使命诞生了。

<font color=blue>Unicode 字符集中包含了世界上几乎所有已知的字符。不过，Unicode 字符集并没有规定如何存储这些字符（也就是如何使用二进制数据表示这些字符）。</font>

<font color=blue>然后，就有了 **UTF-8**（**8**-bit **U**nicode **T**ransformation **F**ormat）。类似的还有 UTF-16、 UTF-32。</font>

<font color=24b3aa>UTF-8 使用 1 到 4 个字节为每个字符编码， UTF-16 使用 2 或 4 个字节为每个字符编码，UTF-32 固定位 4 个字节为每个字符编码。</font>

UTF-8 可以根据不同的符号自动选择编码的长短，像英文字符只需要 1 个字节就够了，这一点 ASCII 字符集一样 。因此，对于英语字符，UTF-8 编码和 ASCII 码是相同的。

UTF-32 的规则最简单，不过缺陷也比较明显，对于英文字母这类字符消耗的空间是 UTF-8 的 4 倍之多。

**UTF-8** 是目前使用最广的一种字符编码。

![img](https://oss.javaguide.cn/javaguide/1280px-Utf8webgrowth.svg.png)

## MySQL 字符集

MySQL 支持很多种字符集的方式，比如 GB2312、GBK、BIG5、多种 Unicode 字符集（UTF-8 编码、UTF-16 编码、UCS-2 编码、UTF-32 编码等等）。

### 查看支持的字符集

<font color=24b3aa>你可以通过 `SHOW CHARSET` 命令来查看，支持 like 和 where 子句。</font>

![img](https://oss.javaguide.cn/javaguide/image-20211008164229671.png)

### 默认字符集

在 MySQL5.7 中，默认字符集是 `latin1` ；在 MySQL8.0 中，默认字符集是 `utf8mb4`

### 字符集的层次级别

<font color=ee11ff>MySQL 中的字符集有以下的层次级别：</font>

- <font color=ee11ff>`server`（MySQL 实例级别）</font>
- <font color=ee11ff>`database`（库级别）</font>
- <font color=ee11ff>`table`（表级别）</font>
- <font color=ee11ff>`column`（字段级别）</font>

它们的优先级可以简单的认为是从上往下依次增大，也即 `column` 的优先级会大于 `table` 等其余层次的。如指定 MySQL 实例级别字符集是 `utf8mb4`，指定某个表字符集是 `latin1`，那么这个表的所有字段如果不指定的话，编码就是 `latin1`。

#### server

不同版本的 MySQL 其 `server` 级别的字符集默认值不同，在 MySQL5.7 中，其默认值是 `latin1` ；在 MySQL8.0 中，其默认值是 `utf8mb4` 。

当然也可以通过在启动 `mysqld` 时指定 `--character-set-server` 来设置 `server` 级别的字符集。

```bash
mysqld
mysqld --character-set-server=utf8mb4
mysqld --character-set-server=utf8mb4 \
  --collation-server=utf8mb4_0900_ai_ci
```

或者如果你是通过源码构建的方式启动的 MySQL，你可以在 `cmake` 命令中指定选项：

```sh
cmake . -DDEFAULT_CHARSET=latin1
或者
cmake . -DDEFAULT_CHARSET=latin1 \
  -DDEFAULT_COLLATION=latin1_german1_ci
```

此外，你也可以在运行时改变 `character_set_server` 的值，从而达到修改 `server` 级别的字符集的目的。

`server` 级别的字符集是 MySQL 服务器的全局设置，它不仅会作为创建或修改数据库时的默认字符集（如果没有指定其他字符集），还会影响到客户端和服务器之间的连接字符集，具体可以查看 [MySQL Connector/J 8.0 - 6.7 Using Character Sets and Unicode](https://dev.mysql.com/doc/connector-j/8.0/en/connector-j-reference-charsets.html)。

#### database

`database` 级别的字符集是我们在创建数据库和修改数据库时指定的：

```sql
CREATE DATABASE db_name
    [[DEFAULT] CHARACTER SET charset_name]
    [[DEFAULT] COLLATE collation_name]

ALTER DATABASE db_name
    [[DEFAULT] CHARACTER SET charset_name]
    [[DEFAULT] COLLATE collation_name]
```

如前面所说，如果在执行上述语句时未指定字符集，那么 MySQL 将会使用 `server` 级别的字符集。

可以通过下面的方式查看某个数据库的字符集：

```sql
USE db_name;
SELECT @@character_set_database, @@collation_database;
```

```sql
SELECT DEFAULT_CHARACTER_SET_NAME, DEFAULT_COLLATION_NAME
FROM INFORMATION_SCHEMA.SCHEMATA WHERE SCHEMA_NAME = 'db_name';
```

#### table

`table` 级别的字符集是在创建表和修改表时指定的：

```sql
CREATE TABLE tbl_name (column_list)
    [[DEFAULT] CHARACTER SET charset_name]
    [COLLATE collation_name]]

ALTER TABLE tbl_name
    [[DEFAULT] CHARACTER SET charset_name]
    [COLLATE collation_name]
```

如果在创建表和修改表时未指定字符集，那么将会使用 `database` 级别的字符集。

#### column

`column` 级别的字符集同样是在创建表和修改表时指定的，只不过它是定义在列中。下面是个例子：

```sql
CREATE TABLE t1
(
    col1 VARCHAR(5)
      CHARACTER SET latin1
      COLLATE latin1_german1_ci
);
```

如果未指定列级别的字符集，那么将会使用表级别的字符集。

### 连接字符集

前面说到了字符集的层次级别，它们是和存储相关的。而连接字符集涉及的是和 MySQL 服务器的通信。

连接字符集与下面这几个变量息息相关：

- `character_set_client` ：描述了客户端发送给服务器的 SQL 语句使用的是什么字符集。
- `character_set_connection` ：描述了服务器接收到 SQL 语句时使用什么字符集进行翻译。
- `character_set_results` ：描述了服务器返回给客户端的结果使用的是什么字符集。

它们的值可以通过下面的 SQL 语句查询：

```sql
SELECT * FROM performance_schema.session_variables
WHERE VARIABLE_NAME IN (
'character_set_client', 'character_set_connection',
'character_set_results', 'collation_connection'
) ORDER BY VARIABLE_NAME;
```

```sql
SHOW SESSION VARIABLES LIKE 'character\_set\_%';
```

如果要想修改前面提到的几个变量的值，有以下方式：

1、修改配置文件

```properties
[mysql]
# 只针对MySQL客户端程序
default-character-set=utf8mb4
```

2、使用 SQL 语句

```sql
set names utf8mb4
# 或者一个个进行修改
# SET character_set_client = utf8mb4;
# SET character_set_results = utf8mb4;
# SET collation_connection = utf8mb4;
```

### JDBC 对连接字符集的影响

不知道你们有没有碰到过存储 emoji 表情正常，但是使用类似 Navicat 之类的软件的进行查询的时候，发现 emoji 表情变成了问号的情况。这个问题很有可能就是 JDBC 驱动引起的。

根据前面的内容，我们知道连接字符集也是会影响我们存储的数据的，而 JDBC 驱动会影响连接字符集。

`mysql-connector-java` （JDBC 驱动）主要通过这几个属性影响连接字符集：

- `characterEncoding`
- `characterSetResults`

以 `DataGrip 2023.1.2` 来说，在它配置数据源的高级对话框中，可以看到 `characterSetResults` 的默认值是 `utf8` ，在使用 `mysql-connector-java 8.0.25` 时，连接字符集最后会被设置成 `utf8mb3` 。那么这种情况下 emoji 表情就会被显示为问号，并且当前版本驱动还不支持把 `characterSetResults` 设置为 `utf8mb4` ，不过换成 `mysql-connector-java driver 8.0.29` 却是允许的。

具体可以看一下 StackOverflow 的 [DataGrip MySQL stores emojis correctly but displays them as?](https://stackoverflow.com/questions/54815419/datagrip-mysql-stores-emojis-correctly-but-displays-them-as)这个回答。

### UTF-8 使用

通常情况下，我们建议使用 UTF-8 作为默认的字符编码方式。

不过，这里有一个小坑。

MySQL 字符编码集中有两套 UTF-8 编码实现：

- **`utf8`**：`utf8`编码只支持`1-3`个字节 。 在 `utf8` 编码中，中文是占 3 个字节，其他数字、英文、符号占一个字节。但 emoji 符号占 4 个字节，一些较复杂的文字、繁体字也是 4 个字节。
- **`utf8mb4`**：UTF-8 的完整实现，正版！最多支持使用 4 个字节表示字符，因此，可以用来存储 emoji 符号。

**为什么有两套 UTF-8 编码实现呢？** 原因如下：

![img](https://oss.javaguide.cn/javaguide/image-20211008164542347.png)

因此，如果你需要存储`emoji`类型的数据或者一些比较复杂的文字、繁体字到 MySQL 数据库的话，数据库的编码一定要指定为`utf8mb4` 而不是`utf8` ，要不然存储的时候就会报错了。

演示一下吧！（环境：MySQL 5.7+）

建表语句如下，我们指定数据库 CHARSET 为 `utf8` 。

```sql
CREATE TABLE `user` (
  `id` varchar(66) CHARACTER SET utf8mb3 NOT NULL,
  `name` varchar(33) CHARACTER SET utf8mb3 NOT NULL,
  `phone` varchar(33) CHARACTER SET utf8mb3 DEFAULT NULL,
  `password` varchar(100) CHARACTER SET utf8mb3 DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

当我们执行下面的 insert 语句插入数据到数据库时，果然报错！

```sql
INSERT INTO `user` (`id`, `name`, `phone`, `password`)
VALUES
 ('A00003', 'guide哥😘😘😘', '181631312312', '123456');
```

报错信息如下：

```plain
Incorrect string value: '\xF0\x9F\x98\x98\xF0\x9F...' for column 'name' at row 1
```

## 参考

- 字符集和字符编码（Charset & Encoding）：https://www.cnblogs.com/skynet/archive/2011/05/03/2035105.html
- 十分钟搞清字符集和字符编码：http://cenalulu.github.io/linux/character-encoding/
- Unicode-维基百科：https://zh.wikipedia.org/wiki/Unicode
- GB2312-维基百科：https://zh.wikipedia.org/wiki/GB_2312
- UTF-8-维基百科：https://zh.wikipedia.org/wiki/UTF-8
- GB18030-维基百科: https://zh.wikipedia.org/wiki/GB_18030
- MySQL8 文档：https://dev.mysql.com/doc/refman/8.0/en/charset.html
- MySQL5.7 文档：https://dev.mysql.com/doc/refman/5.7/en/charset.html
- MySQL Connector/J 文档：https://dev.mysql.com/doc/connector-j/8.0/en/connector-j-reference-charsets.html

# [基础]`BEETWEEN...AND` 和 `IN` 表示的区间

**1. `BETWEEN ... AND ...` (连续范围)**

- **数学含义**：<font color=blue>闭区间 **[a, b]**</font>。
- **底层逻辑**：大于等于 a，**并且**小于等于 b（`>= a AND <= b`）。
- **应用场景**：适合筛选**连续**的数据，比如一段日期、一段价格区间。
- **举个例子**：`price BETWEEN 100 AND 500`。
  - 只要价格落在这个段落里，哪怕是 `100.5`、`250`、`499.9`，统统会被抓出来。

**2. `IN (...)` (精确点名)**

- **数学含义**：特定元素的集合 **{a, b, c}**。
- **底层逻辑**：等于 a，**或者**等于 b，**或者**等于 c（`= a OR = b OR = c`）。
- **应用场景**：适合筛选**毫无连续规律**的特定值，比如几个特定的商品 ID、特定的几个订单状态。
- **举个例子**：`price IN (100, 500)`。
  - 数据库只会死板地去找价格刚好是 `100` 或者刚好是 `500` 的记录。如果价格是 `101` 或者 `499`，它完全不理会。

> 不要误认为 `IN (100,300)` 表示一个区间 `[100,300]` 或者 `(100,300)`

# [基础]DML 语句和 DDL 语句区别是？

- **DML（Data Manipulation Language）** 是数据库操作语言的缩写，是指对数据库中表记录的操作，主要包括表记录的插入、更新、删除和查询，是开发人员日常使用最频繁的操作。
- **DDL （Data Definition Language）**是数据定义语言的缩写，简单来说，就是对数据库内部的对象进行创建、删除、修改的操作语言。它和 DML 语言的最大区别是 DML 只是对表内部数据的操作，而不涉及到表的定义、结构的修改，更不会涉及到其他对象。DDL 语句更多的被数据库管理员（DBA）所使用，一般的开发人员很少使用。

另外，由于 `SELECT` 不会对表进行破坏，所以有的地方也会把 `SELECT` 单独区分开叫做数据库查询语言 **DQL（Data Query Language）**。

# [基础]`DROP`、`DELETE`、`TRUNCATE` 有什么区别？

在数据库操作中，`DROP`、`DELETE` 和 `TRUNCATE` 是三个常用的数据删除命令，它们在功能、性能和使用场景上存在显著差异。

**DROP命令：**

- 语法：`DROP TABLE 表名`
- 作用：<font color=blue>完全删除整个表</font>，包括表结构、数据、索引、触发器、约束等所有相关对象
- 使用场景：当表不再需要时使用

**TRUNCATE命令：**

- 语法：`TRUNCATE TABLE 表名`
- 作用：<font color=blue>清空表中所有数据，但保留表结构</font>
- 特点：自增长字段（AUTO_INCREMENT）会重置为初始值（通常为1）
- 使用场景：需要快速清空表数据但保留表结构时使用

**DELETE命令：**

- 语法：`DELETE FROM 表名 WHERE 条件`
- 作用：<font color=blue>删除满足条件的数据行，不带WHERE子句时删除所有数据</font>
- 特点：自增长字段不会重置，继续从之前的值递增
- 使用场景：需要有选择地删除部分数据时使用

`TRUNCATE` 和不带 `WHERE`子句的 `DELETE`、以及 `DROP` 都会删除表内的数据，但是 **`TRUNCATE` 和 `DELETE` 只删除数据不删除表的结构(定义)，执行 `DROP` 语句，此表的结构也会删除，也就是执行`DROP` 之后对应的表不复存在。**

### 对表结构的影响

- `DROP`：删除表结构和所有数据，表将不复存在
- `TRUNCATE`：仅删除数据，保留表结构和定义
- `DELETE`：仅删除数据，保留表结构和定义

### 触发器

- `DELETE` 操作会触发相关的 DELETE 触发器
- `TRUNCATE` 和 `DROP` 不会触发 DELETE 触发器

### 事务和回滚

* <font color=24b3aa>`DROP` 和 `TRUNCATE` 属于 DDL 操作，执行后立即生效，不能回滚</font>
* <font color=24b3aa>`DELETE` 属于 DML 操作，可以回滚（在事务中）</font>

### 执行速度

一般来说：`DROP` > `TRUNCATE` > `DELETE`（这个我没有实际测试过）。

- `DELETE` 命令执行的时候会产生数据库的 `binlog` 日志，而日志记录是需要消耗时间的，但是也有个好处方便数据回滚恢复。
- `TRUNCATE` 命令执行的时候不会产生数据库日志，因此比 `DELETE` 要快。除此之外，<font color=blue>还会把表的自增值重置和索引恢复到初始大小等</font>。
- `DROP` 命令会把表占用的空间全部释放掉。

Tips：你应该更多地关注在使用场景上，而不是执行效率。

# [基础]什么是存储过程?

存储过程是数据库中预编译的SQL语句集合，它将多条SQL语句和程序逻辑控制语句（如IF-ELSE、WHILE循环等）封装在一起，形成一个可重复调用的数据库对象。

**存储过程的优势：**

在传统企业级应用中，存储过程具有一定的实用价值。当业务逻辑复杂时，需要执行大量 SQL 语句才能完成一个业务操作，此时可以将这些语句封装成存储过程，简化调用过程。由于存储过程在创建时就已经编译并存储在数据库中，执行时无需重新编译，因此相比动态 SQL 语句具有更好的执行性能。同时，一旦存储过程调试完成，其运行相对稳定可靠。

**存储过程的局限性：**

然而，在现代互联网架构中，存储过程的使用越来越少。主要原因包括：调试困难，缺乏成熟的调试工具；扩展性差，修改业务逻辑需要直接修改数据库对象；移植性差，不同数据库系统的存储过程语法差异较大；占用数据库资源，增加数据库服务器负担；版本管理困难，不便于进行代码版本控制。

**行业规范：**

基于以上原因，许多互联网公司的开发规范中明确限制或禁止使用存储过程。例如，<font color=blue>《阿里巴巴 Java 开发手册》中明确规定“禁止使用存储过程，存储过程难以调试和拓展，更没有可移植性。”，推荐将业务逻辑放在应用层实现，保持数据库的简单和高效。</font>

# [基础]数据库三范式

为了建立冗余较小、结构合理的数据库，设计数据库时必须遵循一定的规则。在关系型数据库中这种规则就称为范式。范式是符合某一种设计要求的总结。要想设计一个结构合理的关系型数据库，必须满足一定的范式。

在实际开发中最为常见的设计范式有三个：

**<font color=red>1. 第一范式(确保每列保持原子性)</font>**

<font color=24b3aa>第一范式是最基本的范式。如果数据库表中的所有字段值都是不可分解的原子值，就说明该数据库表满足了第一范式。</font>

第一范式的合理遵循需要根据系统的实际需求来定。比如某些数据库系统中需要用到“地址”这个属性，本来直接将“地址”属性设计成一个数据库表的字段就行。但是如果系统经常会访问“地址”属性中的“城市”部分，那么就非要将“地址”这个属性重新拆分为省份、城市、详细地址等多个部分进行存储，这样在对地址中某一部分操作的时候将非常方便。这样设计才算满足了数据库的第一范式，如下表所示。

![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.8z74l0j73w.webp)

上表所示的用户信息遵循了第一范式的要求，这样在对用户使用城市进行分类的时候就非常方便，也提高了数据库的性能。

<font color=red>**2. 第二范式(确保表中的每列都和主键相关)**</font>

第二范式在第一范式的基础之上更进一层。<font color=24b3aa>第二范式需要确保数据库表中的每一列都和主键相关，而**不能只与主键的某一部分相关（主要针对联合主键而言）**。也就是说在一个数据库表中，一个表中只能保存一种数据，不可以把多种数据保存在同一张数据库表中。</font>

比如要设计一个订单信息表，因为订单中可能会有多种商品，所以要将订单编号和商品编号作为数据库表的联合主键，如下表所示。

 **订单信息表**

![img](https://pic002.cnblogs.com/images/2012/270324/2012040114063976.png)

这样就产生一个问题：这个表中是以订单编号和商品编号作为联合主键。这样在该表中商品名称、单位、商品价格等信息不与该表的主键相关，而仅仅是与商品编号相关。所以在这里违反了第二范式的设计原则。

而如果把这个订单信息表进行拆分，把商品信息分离到另一个表中，把订单项目表也分离到另一个表中，就非常完美了。如下所示。

![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.60uuhiic4f.webp)

这样设计，在很大程度上减小了数据库的冗余。如果要获取订单的商品信息，使用商品编号到商品信息表中查询即可。

<font color=red>**3. 第三范式(确保每列都和主键列直接相关,而不是间接相关)**</font>

第三范式需要确保数据表中的每一列数据都和主键直接相关，而不能间接相关。

比如在设计一个订单数据表的时候，可以将客户编号作为一个外键和订单表建立相应的关系。而不可以在订单表中添加关于客户其它信息（比如姓名、所属公司等）的字段。如下面这两个表所示的设计就是一个满足第三范式的数据库表。

![img](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.1lcfc96g7f.webp)

这样在查询订单信息的时候，就可以使用客户编号来引用客户信息表中的记录，也不必在订单信息表中多次输入客户信息的内容，减小了数据冗余。

# [基础]数据表「学号、姓名、分数、课程名、学分」如何优化

这个表最大的问题在于**数据冗余**和**牵一发而动全身**（也就是不符合数据库范式）。

如果在同一个表里塞满这些字段，当一个学生选了10门课，他的“姓名”就要存10遍；如果一门课有100人选，这门课的“学分”和“课程名”就要存100遍。万一哪天学校要把某门课的学分从 2 改成 3，你得在数据库里改100行记录，极易出错。

最标准的优化方法是将其拆分为**三个表**，构建经典的多对多关系：

**1. 学生表（Student）**

- **字段**：学号（主键）、姓名
- **作用**：专门用来管学生信息。每个学生在这个表里只有一行记录，无论他选多少门课，名字都只存一次。

**2. 课程表（Course）**

- **字段**：课程编号（主键）、课程名、学分
- **作用**：专门用来管课程信息。给每门课加一个独立的“课程编号”，以后无论改课名还是改学分，都只修改这一行数据即可。

**3. 成绩表（Score / 桥梁表）**

- **字段**：记录编号（主键）、学号（外键）、课程编号（外键）、分数
- **作用**：只负责记录“谁、选了什么课、考了多少分”。通过学号和课程编号，去前两个表里关联具体的姓名和学分。

**优化后的好处：** 结构清爽了。不仅大幅节省了存储空间，而且再也不会出现“删除了某个学生的最后一条选课记录，结果把这门课本身的学分信息也跟着弄丢了”的尴尬情况。

# [SQL基础]秒杀问题

应对百万并发的商品库存扣减，核心思路就是“排队、缓冲、把压力挡在门外”，绝对不能让请求直接砸向数据库。你可以通过以下四个核心点来解决：

1. **<font color=blue>顶层无情限流</font>** 一百万人抢少量商品，绝大部分注定失败。在请求刚进来的网关层，就要通过黑名单、验证码机制或者直接按比例丢弃（比如令牌桶算法），拦截掉 99% 的无效请求，只放行系统能承受的流量进入核心业务线。
2. **<font color=blue>Redis 内存预扣减（低时延防超卖）</font>** 把库存数量提前存放到 Redis 中。利用 Redis 的单线程特性和 Lua 脚本，把“查库存是否大于0”和“扣减库存”打包成一个绝对不可分割的原子操作。因为是全内存操作，响应极其迅速，同时也从根源上杜绝了并发超卖。
3. **<font color=blue>消息队列异步接盘（削峰填谷）</font>** Redis 扣减成功后，千万不要立刻去写 MySQL。而是把这个下单请求丢进消息队列（比如 RocketMQ 或 Kafka），然后立刻给用户返回“抢购成功，正在为您生成订单”。这样用户的体感等待时间极短。
4. **<font color=blue>数据库平滑落底</font>** 后台的消费服务根据 MySQL 数据库能健康承受的写入速率，从消息队列里按照自己的节奏慢慢拉取数据，稳妥地执行最终的数据库扣库存和订单落表操作。

简单来说就是：**网关挡掉大头流量，Redis 扛住并发快速扣减，MQ 缓冲异步排队，MySQL 在最后面稳稳兜底。**

# [基础]SQL 注入攻击

SQL注入（SQL Injection）就像是坏人利用了填表游戏里的漏洞。本来系统只是让你填个名字，结果你填了一段“暗号”，系统没防备，把暗号当成了指令去执行了。

核心逻辑主要有这三点：

- **根本原因**：程序把“用户输入的数据”直接拼接到“数据库命令”里了，没有做严格的区分。数据库分不清哪部分是数据，哪部分是代码。
- **攻击手段**：攻击者在输入框里填入特殊的符号（比如单引号 `'` 或者注释符 `--`），强行改变原本 SQL 语句的语法结构。比如本来是查密码，结果变成了“密码无论填啥都算对”（例如输入 `' OR '1'='1`）。
- **防御方法**：最靠谱的方法是使用**参数化查询（预编译）**。这相当于给用户输入的内容套上了一个“安全罩”，数据库只把它当成纯文本数据，绝不当成可执行的命令。

----

我们拿最常见的“账号登录”来举个具体的例子。

假设系统后台用来验证登录的 SQL 语句模板是这样的：

```sql
SELECT * FROM users WHERE username = '你输入的账号' AND password = '你输入的密码'
```

**1. 正常人的操作** 你输入账号 `admin`，密码 `123`。 后台拼出来的语句是：`SELECT * FROM users WHERE username = 'admin' AND password = '123'` **结果**：系统乖乖去数据库里核对，账号和密码同时匹配，才让你登录。

**2. 坏人的操作（注释截断法）** 坏人在账号框里输入了 `admin' -- `（注意最后有个空格），密码随便填。 后台拼出来的语句变成了：`SELECT * FROM users WHERE username = 'admin' -- ' AND password = '随便填'` **结果**：在 SQL 语言里，`-- ` 代表“注释”，意思是“后面的代码全是废话，不用执行”。所以系统实际执行的只有 `...WHERE username = 'admin'`。系统一看名为 admin 的账号确实存在，直接放行，密码检查被完美绕过！

**3. 更狠的“万能密码”（逻辑强暴法）** 坏人在账号框里直接输入 `' OR '1'='1`。 后台拼出来的语句变成了：`SELECT * FROM users WHERE username = '' OR '1'='1' AND password = '...'` **结果**：`OR` 代表“或者”，`1=1` 是个永远成立的绝对真理。这就好比系统问：“你是某个特定账号吗？或者 1等于1 吗？” 系统一想，1确实等于1啊，条件成立！于是直接给坏人开了门，通常还会登录成数据库里的第一个用户（往往是超级管理员）。

----

防范 SQL 注入的核心思想就一条：**绝对不要相信用户的任何输入，把“代码”和“数据”严格分开。**

具体落实到日常开发中，做好这三点就足够把门焊死了：

**<font color=ee11ff>1. 必须使用预编译（参数化查询）</font>** 这是最靠谱、最根本的一招。简单来说，就是先让数据库把 SQL 的“骨架”解析好，留下数据空位（比如用 `?` 占位）。用户输入的内容，只能老老实实当成“纯文本”填进空位里，数据库绝不会把它当成指令去执行。

**<font color=ee11ff>2. 做好输入检查（白名单机制）</font>** 把好第一道关卡，不符合格式的东西直接拦下。比如要求填手机号的框，就只允许提交数字；需要选日期的，就只认日期格式。对于带有特殊符号（像单引号、分号）的异常输入，直接拒绝或进行转义处理。

**<font color=ee11ff>3. 限制数据库权限（最小权限原则）</font>** 别用超级管理员（Root）账号去跑日常的业务代码。给每个应用分配一个“刚好够用”的普通账号，比如只能查改某几张表，严禁赋予删除表或修改数据库结构的权限。这样就算真的被钻了空子，损失也被限制在最小范围内。

# [基础]如果一个表上索引了还是发生慢查询，可能是什么原因导致的

<font color=blue>建了索引依然慢，无非是**没用上**、**用错了**或者**被外部拖累**。</font>核心原因主要有这 4 个：

**<font color=ee11ff>1. 索引压根没生效（最常见）</font>** 你虽然建了，但 SQL 写法让 MySQL 无法使用它。最经典的踩坑场景包括：在索引字段上做计算或用函数、发生了隐式类型转换（比如字符串和数字混用）、用了 `LIKE '%abc'` （左边带百分号），或者不满足联合索引的“最左匹配原则”。这些都会导致 MySQL 放弃索引，直接全表扫描。

**<font color=ee11ff>2. “回表”操作太疯狂</font>** 虽然走索引查到了目标 ID，但你的语句是 `SELECT *` 或者查了超多非索引字段。如果符合条件的数据有几十万条，MySQL 就得拿着这些 ID 疯狂跑回主表去捞完整数据（回表）。海量的随机磁盘 I/O 分分钟把查询拖慢。

**<font color=ee11ff>3. 索引的“区分度”太低</font>** 比如你给“性别”（只有男、女）或“订单状态”建普通索引。MySQL 的优化器很聪明，它一看走索引捞出来的数据占了整张表的一大半，算下来走索引+回表的开销比直接全表扫描还大，就会强行无视你的索引。

**<font color=ee11ff>4. 被别的操作“堵”住了</font>** 有时候慢不是 SQL 的锅。可能是你的查询刚好撞上了别人在修改这行数据，不得不排队等“行锁”释放；或者是数据库整体负载太高（CPU 打满、磁盘 I/O 飙升），导致你的查询只能在队列里干等。

# [基础]数据库转账怎么保证不出错？ 

保证数据库转账不出错，核心靠的是**<font color=blue>数据库事务（Transaction）</font>**和**<font color=blue>并发控制</font>**。用大白话来说，主要通过以下 4 个机制来兜底：

1. **<font color=ee11ff>事务“捆绑执行”（原子性）</font>** 转账包含“A扣钱”和“B加钱”两条独立语句。数据库把它们绑进同一个事务中，规则是**要么全成功，要么全失败**（回滚 Rollback）。如果钱从 A 账上扣了，但 B 的账户因为挂断等原因没加上，系统会自动把 A 的钱退回去，绝不会出现钱凭空消失的中间态。
2. **<font color=ee11ff>行锁排队处理（隔离性）</font>** 如果有两个人同时给 A 转账，同时读取了余额，很容易把账算乱（并发覆盖）。数据库会利用**行级锁**（比如 `FOR UPDATE` 排他锁），把同时发生的操作强制变成“排队处理”。前一个人的转账事务没提交，后一个人只能等着，确保余额计算绝对精准。
3. **<font color=ee11ff>唯一流水号防重（幂等性）</font>** 为了防止网络卡顿导致请求重发（比如用户狂点转账按钮），表结构里通常会要求设置“唯一流水号”（唯一索引）。转账前先查单号，或者依靠数据库的唯一键冲突报错，确保一笔订单不管请求多少次，系统只扣一次钱。
4. **<font color=ee11ff>日志随时记录（持久性）</font>** 就算转账进行到一半服务器突然断电宕机，也不用怕。数据库底层实时记着 Undo/Redo 日志（相当于操作流水账），重启后，它会对照日志，把没做完的半截操作撤销，或者把已经成功的操作恢复写入磁盘，保证数据稳如泰山。

# [基础]Mysql char 和 varchar 的区别

MySQL 中 `char` 和 `varchar` 最核心的区别就在于“定长”**和**“变长”。简单来说，可以归纳为以下 3 个关键点：

**1. 存储方式：**

- **char 定长：** `char(10)` 固定占用 10 字符的空间，哪怕你只存入 "abc"（3 个字符），它也会在后面用空格给你补齐 10 个位置。缺点是费空间，优点是长度绝对统一。
- **varchar 变长：**`varchar(10)` 表示最多可以存储 10 个字符，存放 "abc" 时只会占用 3 个字符的空间，不过实际存储时还会额外加 1 到 2 个字节用来记录“这串字符到底有多长”。

**2. 性能表现：**

- **char 性能更高：** 因为长度永远是固定的，数据库在找数据或者更新数据时，直接按固定大小去对应位置拿就行。它非常适合频繁修改，不容易产生碎片。
- **varchar 稍微慢一点：** 每次操作都要先计算长度。如果修改后的数据变长了，原先的位置放不下，数据库还得费劲去重新分配空间，这就会稍微拖慢速度。

**3. 对尾部空格的处理**

- **char 会偷偷删空格：** 当你用 Select 查数据时，`char` 会自动把你存进去的尾部空格（包括它自己补齐的空格）统统删掉再给你。
- **varchar 原汁原味：** 你存进去的数据尾巴上带了多少个空格，它就老老实实保留多少个。

# [基础]跨库查询

遇到两个表在不同机器上的情况，本质上是**<font color=blue>跨节点/跨库查询</font>**。通常有以下 4 个核心解决思路，按落地成本从低到高排列：

**<font color=ee11ff>1. 数据库层的“跨库直连”（最快捷）</font>** 你可以让主数据库“兼职”做客户端，直接去连另一台机器的数据库。比如 MySQL 的 Federated 引擎，或者 PostgreSQL 的 FDW（外部数据包装器）。这种方式写 SQL 时就像查本地表一样，但如果数据量大，跨网络传输会成为性能瓶颈。

**<font color=ee11ff>2. 业务代码层“手动拼接”（最常见）</font>** 在微服务架构里最常用的土办法。放弃在数据库里做复杂的跨机 JOIN，而是用代码分两步走：先去机器 A 查出第一批数据，拿着结果里的关键 ID，再去机器 B 查关联数据。最后在应用内存里把两份数据组合起来。优点是灵活解耦，缺点是比较吃服务器内存。

**<font color=ee11ff>3. 提前做“数据同步”（最稳妥）</font>** 如果这个跨机器查询非常高频，且对响应速度要求高，最好的办法是消灭跨机器关联。可以通过 Canal 等工具，把机器 B 上的表实时同步到机器 A 里。这样在机器 A 上直接做本地 JOIN 即可，用空间换时间。

**<font color=ee11ff>4. 引入“数据库中间件”（最彻底）</font>** 如果是系统做分库分表导致的数据分散，可以在应用和底层数据库之间加一层中间件（比如 ShardingSphere、MyCat）。应用只需向中间件发一条普通 SQL，中间件会自动帮你把请求拆开派发给两台机器，最后汇总结果返回给你，对业务代码完全透明。

# [基础]跨库查询不一致问题

> 跨库查询也会遇到一致性问题啊，比如我们要访问 A 库和 B 库的数据，访问 A 库的时候，B 库的数据被修改了，该怎么办？

这在分布式系统里是非常经典的“跨库读取不一致”问题。大白话讲，解决这个问题有 3 种常见的应对套路：

**<font color=blue>1. 业务妥协：接受微小的时间差（最常见）</font>** 如果只是普通的页面展示（比如查用户的基本信息，顺带去另一个库查他的积分），大多数公司选择**不管它**。A 和 B 差了几十毫秒的更新，对用户体验影响极小，下一次刷新可能就一致了。没必要为了这种展示类查询去折腾复杂的架构。 *大白话：只要不是核心财务算账，随它去，保住系统性能最重要。*

**<font color=blue>2. 提前搬家：同步到同一个“查询专用库”</font>** 如果这是重要的数据报表，一点都不能错，那就不要在用户请求时去实时跨库查。通过数据同步工具（如 Canal），把 A 库和 B 库的数据实时抽取、拼装好，放到一个统一的查询库（比如 Elasticsearch 或者专用的 MySQL 从库）。在这个单库里查，就能保证快照是一致的。 *大白话：别跨网现查，提前把两边的数据抄送到同一个地方再查。*

**<font color=blue>3. 死磕到底：全局加锁（牺牲性能）</font>** 如果业务极其核心（比如转账前必须严格确认双方状态），那就只能上强制手段了。在查询动作发生时，通过 Redis 加一把分布式锁，或者用 Seata 开启分布式事务。你查 A 和 B 的这几百毫秒内，谁也别想改 B 的数据，必须乖乖在门外排队，等你查完了才能放行。 *大白话：想要数据绝对准确，就只能牺牲性能，让别人改数据的操作排队等着。*

# [基础]Mysql 执行计划分析

> https://javaguide.cn/database/mysql/mysql-query-execution-plan.html

优化 SQL 的第一步应该是读懂 SQL 的执行计划。本篇文章，我们一起来学习下 MySQL `EXPLAIN` 执行计划相关知识。

> **版本说明**：本文内容基于 MySQL 5.7+ 和 8.0+ 版本。`filtered` 和 `partitions` 列在 MySQL 5.7+ 可用，`EXPLAIN ANALYZE` 和 Hash Join 特性需要 MySQL 8.0.18+ 和 8.0.20+。

## 什么是执行计划？

<font color=blue>**执行计划** 是指一条 SQL 语句在经过 **MySQL 查询优化器** 的优化后，具体的执行方式。</font>

执行计划通常用于 SQL 性能分析、优化等场景。<font color=24b3aa>通过 `EXPLAIN` 的结果，可以了解到如数据表的查询顺序、数据查询操作的操作类型、哪些索引可以被命中、哪些索引实际会命中、每个数据表有多少行记录被查询等信息。</font>

## 如何获取执行计划？

MySQL 为我们提供了 `EXPLAIN` 命令，来获取执行计划的相关信息。

需要注意的是，标准 `EXPLAIN` 语句并不会真的去执行相关的语句，而是通过查询优化器对语句进行分析，找出最优的查询方案，并显示对应的信息。

MySQL 8.0.18 引入了 <font color=red>`EXPLAIN ANALYZE`</font>，它会**真正执行**查询并输出每个步骤的实际耗时与行数，比标准 `EXPLAIN` 的估算数据更可靠，适合在测试环境深度排查慢查询：

```sql
mysql> EXPLAIN ANALYZE SELECT * FROM users WHERE age = 25\G
*************************** 1. row ***************************
EXPLAIN: -> Covering index lookup on users using idx_age_score_name (age=25)
(cost=1.52 rows=12) (actual time=0.0272..0.0344 rows=12 loops=1)
```

此外，`EXPLAIN FORMAT=JSON` 可以输出优化器的成本模型数据（`query_cost`），比表格形式更能反映各步骤的实际代价，在多表 JOIN 或子查询调优时尤为有用：

```sql
mysql> EXPLAIN FORMAT=JSON SELECT * FROM users WHERE age = 25\G
*************************** 1. row ***************************
EXPLAIN: {
  "query_block": {
    "select_id": 1,
    "cost_info": {
      "query_cost": "1.52"
    },
    "table": {
      "table_name": "users",
      "access_type": "ref",
      "key": "idx_age_score_name",
      "rows_examined_per_scan": 12,
      "filtered": "100.00",
      "using_index": true
    }
  }
}
```

`EXPLAIN` 执行计划支持 `SELECT`、`DELETE`、`INSERT`、`REPLACE` 以及 `UPDATE` 语句。我们一般多用于分析 `SELECT` 查询语句，使用起来非常简单，语法如下：

```sql
EXPLAIN SELECT 查询语句；
```

我们简单来看下一条查询语句的执行计划：

**示例 1：单表查询（使用索引）**

```sql
-- 表结构：users(id, age, score, name, address)，联合索引 idx_age_score_name(age, score, name)
mysql> EXPLAIN SELECT * FROM users WHERE age = 25;
+----+-------------+-------+------------+------+---------------------+---------------------+---------+-------+------+----------+-------------+
| id | select_type | table | partitions | type | possible_keys       | key                 | key_len | ref   | rows | filtered | Extra       |
+----+-------------+-------+------------+------+---------------------+---------------------+---------+-------+------+----------+-------------+
|  1 | SIMPLE      | users | NULL       | ref  | idx_age_score_name  | idx_age_score_name  | 5       | const |   12 |   100.00 | Using index |
+----+-------------+-------+------------+------+---------------------+---------------------+---------+-------+------+----------+-------------+
```

**示例 2：UNION 查询（id 为 NULL 的场景）**

```sql
mysql> EXPLAIN SELECT * FROM users WHERE id = 1 UNION SELECT * FROM users WHERE id = 2;
+----+--------------+------------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
| id | select_type  | table      | partitions | type  | possible_keys | key     | key_len | ref   | rows | filtered | Extra |
+----+--------------+------------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
|  1 | PRIMARY      | users      | NULL       | const | PRIMARY       | PRIMARY | 4       | const |    1 |   100.00 | NULL  |
|  2 | UNION        | users      | NULL       | const | PRIMARY       | PRIMARY | 4       | const |    1 |   100.00 | NULL  |
|  3 | UNION RESULT | <union1,2> | NULL       | ALL   | NULL          | NULL    | NULL    | NULL  | NULL |     NULL | Using temporary |
+----+--------------+------------+------------+-------+---------------+---------+---------+-------+------+----------+-------+
```

可以看到，执行计划结果中共有 12 列，各列代表的含义总结如下表：

| **列名**      | **含义**                                                     |
| ------------- | ------------------------------------------------------------ |
| id            | SELECT 查询的序列标识符                                      |
| select_type   | SELECT 关键字对应的查询类型                                  |
| table         | 用到的表名                                                   |
| partitions    | 匹配的分区，对于未分区的表，值为 NULL                        |
| type          | 表的访问方法                                                 |
| possible_keys | 可能用到的索引                                               |
| key           | 实际用到的索引                                               |
| key_len       | 所选索引的长度                                               |
| ref           | **<font color=blue>当使用索引等值查询时，与索引作比较的列或常量</font>** |
| rows          | 预计要读取的行数                                             |
| filtered      | 按表条件过滤后，留存的记录数的百分比                         |
| Extra         | 附加信息                                                     |

## 如何分析 EXPLAIN 结果？

为了分析 `EXPLAIN` 语句的执行结果，我们需要搞懂执行计划中的重要字段。

### id

`SELECT` 标识符，用于标识每个 `SELECT` 语句的执行顺序。

`id` 列的解读规则：

- **id 相同**：从上往下依次执行（通常出现在多表 JOIN 场景）
- **id 不同**：id 值越大，执行优先级越高（子查询先于外层查询执行）
- **id 为 NULL**：表示这是 UNION RESULT 或 DERIVED 表的结果集，不需要单独执行查询

**示例**：

```sql
mysql> EXPLAIN 
    -> SELECT * FROM users WHERE id = 1
    -> UNION
    -> SELECT * FROM users WHERE id = 2\G
*************************** 1. row ***************************
           id: 1
  select_type: PRIMARY
        table: users
         type: const
*************************** 2. row ***************************
           id: 2
  select_type: UNION
        table: users
         type: const
*************************** 3. row ***************************
           id: NULL
  select_type: UNION RESULT
        table: <union1,2>
         type: ALL
        Extra: Using temporary
```

第三行的 `id = NULL`，table = `<union1,2>`，表示这是前两个查询结果的合并。

### select_type

查询的类型，主要用于区分普通查询、联合查询、子查询等复杂的查询，常见的值有：

- **SIMPLE**：简单查询，不包含 UNION 或者子查询。
- **PRIMARY**：查询中如果包含子查询或其他部分，外层的 SELECT 将被标记为 PRIMARY。
- **SUBQUERY**：子查询中的第一个 SELECT。
- **UNION**：在 UNION 语句中，UNION 之后出现的 SELECT。
- **DERIVED**：在 FROM 中出现的子查询将被标记为 DERIVED。
- **UNION RESULT**：UNION 查询的结果。

### table

查询用到的表名，每行都有对应的表名，表名除了正常的表之外，也可能是以下列出的值：

- **`<unionM,N>`** : 本行引用了 id 为 M 和 N 的行的 UNION 结果；
- **`<derivedN>`** : 本行引用了 id 为 N 的表所产生的派生表结果。派生表有可能产生自 FROM 语句中的子查询。
- **`<subqueryN>`** : 本行引用了 id 为 N 的表所产生的物化子查询结果。

### type（重要）

查询执行的类型，描述了查询是如何执行的。**从最优到最差的排序为**：

```
system > const > eq_ref > ref > fulltext > ref_or_null > index_merge > unique_subquery > index_subquery > range > index > ALL
```

**性能判断经验法则**：

- **优秀**（至少达到）：`system`、`const`、`eq_ref`、`ref`、`range`
- **需关注**：`index_merge`、`index`（全索引扫描，大数据量下仍有性能风险）
- **需优化**：`ALL`（全表扫描）

<font color=24b3aa>**注意**：此排序反映的是**单表访问效率**，不代表整体查询性能。例如 `type=ref` 配合大量回表，可能比 `type=index` 的覆盖索引更慢。</font>

常见的几种类型具体含义如下：

- **system**：表中只有一行记录（或者是空表），且存储引擎能够精确统计行数。适用于 MyISAM、Memory、InnoDB（当表只有 1 行时，InnoDB 会优化为 const）等引擎。是 const 访问类型的特例。

- **const**：表中最多只有一行匹配的记录，一次查询就可以找到，常用于使用主键或唯一索引的所有字段作为查询条件。

- **eq_ref**：当连表查询时，前一张表的行在当前这张表中只有一行与之对应。是除了 system 与 const 之外最好的 join 方式，常用于使用主键或唯一非空索引的所有字段作为连表条件（严格保证一对一匹配）。

- **ref**：使用普通索引作为查询条件，查询结果可能找到多个符合条件的行（与 eq_ref 的区别：一个驱动行可能匹配多个被驱动行）。

- **index_merge**：当 WHERE 子句包含多个范围条件，且每个条件可以使用不同索引时，MySQL 会合并多个索引的扫描结果。key 列列出使用的索引，Extra 列显示合并算法：

  - `Using union(...)`：对多个索引结果取并集（OR 条件）
  - `Using sort_union(...)`：先对索引结果排序再取并集（OR 条件，索引列非有序）
  - `Using intersection(...)`：对多个索引结果取交集（AND 条件）

  **示例**：

  ```sql
  -- OR 条件触发 index merge union
  EXPLAIN SELECT * FROM employees WHERE emp_no = 10001 OR dept_no = 'd001';
  -- Extra: Using union(PRIMARY,dept_no_index)
  ```

- **range**：对索引列进行范围查询，执行计划中的 key 列表示哪个索引被使用了。

- **index**：Full Index Scan，查询遍历了整棵索引树。与 ALL（全表扫描）类似，但通常开销更低：索引记录的体积远小于完整行数据，读取相同行数所需的 I/O 页数更少；若同时满足覆盖索引条件，还可避免回表。但在超大表（亿级以上）上，全索引扫描同样可能产生大量 I/O，不可因 type 级别高于 ALL 就忽视其代价。

- **ALL**：全表扫描。

### possible_keys

possible_keys 列表示 MySQL 执行查询时可能用到的索引。如果这一列为 NULL ，则表示没有可能用到的索引；这种情况下，需要检查 WHERE 语句中所使用的列，看是否可以通过给这些列中某个或多个添加索引的方法来提高查询性能。

### key（重要）

key 列表示 MySQL 实际使用到的索引。如果为 NULL，则表示未用到索引。

### key_len

key_len 列表示 MySQL 实际使用的索引的最大长度；当使用到联合索引时，有可能是多个列的长度和。在满足需求的前提下越短越好。如果 key 列显示 NULL ，则 key_len 列也显示 NULL 。

### rows

rows 列表示根据表统计信息及索引选用情况，**估算**出找到所需记录需要读取的行数，数值越小越好。

需要注意的是，该值是估算值而非精确值。<font color=24b3aa>InnoDB 的统计信息基于对索引页的随机采样：</font>

- 采样页数由 `innodb_stats_persistent_sample_pages` 控制（默认 20 页）
- 在表数据频繁变动或批量导入后，估算值与真实行数的偏差可能达到 10%～50% 甚至更大
- **小表陷阱**：当表行数极少（如 < 100 行）时，优化器可能忽略索引而选择全表扫描，因为全表扫描的成本估算更低

**验证方法**：

```sql
-- 执行计划估算行数
mysql> EXPLAIN SELECT * FROM users WHERE age = 25\G
rows: 12

-- 实际行数（注意：在大表上慎用 COUNT(*)）
mysql> SELECT COUNT(*) FROM users WHERE age = 25;
+----------+
| COUNT(*) |
+----------+
|       12 |
+----------+
```

遇到执行计划与实际性能不符时，可以执行 `ANALYZE TABLE` 重新采样，再观察执行计划的变化。

### filtered

filtered 列表示存储引擎返回的数据在 Server 层经 WHERE 条件过滤后，**估算**留存的记录占比（百分比，0～100）。计算公式为：`filtered = (条件过滤后的行数 / 存储引擎返回的行数) × 100`。

**解读规则**：

- 当 `filtered = 100`：存储引擎返回的所有行都满足 WHERE 条件（理想情况）
- 当 `filtered < 100`：部分行被 Server 层过滤掉，说明索引未能覆盖所有查询条件
- **JOIN 场景**：优化器用 `rows × (filtered / 100)` 估算当前表传递给下一张表的行数（扇出）

该字段在多表 JOIN 场景中尤为重要：扇出越大，驱动表需要匹配的被驱动表行数就越多。因此<font color=24b3aa>当 `filtered` 值很低时，说明过滤效率较好；</font>而当 `rows` 很大且 `filtered` 又不高时，则是潜在性能瓶颈的信号，应优先考虑通过索引下推（ICP）或更合适的索引来减少扇出。

### Extra（重要）

这列包含了 MySQL 解析查询的额外信息，通过这些信息，可以更准确的理解 MySQL 到底是如何执行查询的。常见的值如下：

- **Using filesort**：MySQL 无法利用索引完成 ORDER BY 或 GROUP BY 的排序要求，需要在返回结果集后额外执行一次排序操作。当结果集大小在 `sort_buffer_size` 以内时，排序在内存中完成；超出则借助临时磁盘文件。"filesort" 是历史遗留名称，并不代表一定产生磁盘 I/O。

- **Using temporary**：MySQL 需要创建临时表来存储查询的结果，常见于 ORDER BY 和 GROUP BY。

- **Using index**：表明查询使用了覆盖索引，不用回表，查询效率非常高。

- **Using index condition**：表示查询优化器选择使用了索引条件下推这个特性。

- **Using where**：MySQL Server 层对存储引擎返回的行应用了额外的 WHERE 条件过滤。即使已命中索引（如 `type=ref`），若索引只能满足部分查询条件，剩余条件仍需在 Server 层过滤，此时同样会出现 `Using where`。

- **Using join buffer (Block Nested Loop)**：连表查询时，被驱动表未使用索引，MySQL 会先将驱动表数据读入 join buffer，再遍历被驱动表进行匹配（复杂度 O(N×M)）。

- **Using join buffer (hash join)**：MySQL 8.0.18 引入了 Hash Join 算法，**仅用于等值 JOIN**（如 `t1.id = t2.id`），8.0.20 起默认替代 BNL。Hash Join 复杂度为构建阶段 O(N) + 探测阶段 O(M)，比 BNL 的 O(N×M) 更高效。

  **例外场景**（仍会退回 BNL）：

  - 非等值 JOIN（如 `t1.id > t2.id`）
  - JOIN 条件包含函数或表达式
  - 被驱动表上有索引可用时（此时会使用 Index Nested Loop）

注意区别 Using index condition 和 Using index：

* `Using index` 是“完全不需要回表”；
* `Using index condition` 是“需要回表，但在回表前先用索引过滤，尽量少回表”。

这里提醒下，当 Extra 列包含 Using filesort 或 Using temporary 时，MySQL 的性能可能会存在问题，需要尽可能避免。

## 参考

- https://dev.mysql.com/doc/refman/8.0/en/explain-output.html
- https://dev.mysql.com/doc/refman/8.0/en/explain.html
- https://juejin.cn/post/6953444668973514789

# [基础]Mysql 日期类型选择建议

> https://javaguide.cn/database/mysql/some-thoughts-on-database-storage-time.html

在日常的软件开发工作中，存储时间是一项基础且常见的需求。无论是记录数据的操作时间、金融交易的发生时间，还是行程的出发时间、用户的下单时间等等，时间信息与我们的业务逻辑和系统功能紧密相关。因此，正确选择和使用 MySQL 的日期时间类型至关重要，其恰当与否甚至可能对业务的准确性和系统的稳定性产生显著影响。

本文旨在帮助开发者重新审视并深入理解 MySQL 中不同的时间存储方式，以便做出更合适项目业务场景的选择。

## 不要用字符串存储日期

和许多数据库初学者一样，笔者在早期学习阶段也曾尝试使用字符串（如 VARCHAR）类型来存储日期和时间，甚至一度认为这是一种简单直观的方法。毕竟，'YYYY-MM-DD HH:MM:SS' 这样的格式看起来清晰易懂。

但是，这是不正确的做法，主要会有下面两个问题：

1. **空间效率**：与 MySQL 内建的日期时间类型相比，字符串通常需要占用更多的存储空间来表示相同的时间信息。

2. **查询与计算效率低下：**
   - **比较操作复杂且低效**：基于字符串的日期比较需要按照字典序逐字符进行，这不仅不直观（例如，'2024-05-01' 会小于 '2024-1-10'），而且效率远低于使用原生日期时间类型进行的数值或时间点比较。
   - **计算功能受限**：无法直接利用数据库提供的丰富日期时间函数进行运算（例如，计算两个日期之间的间隔、对日期进行加减操作等），需要先转换格式，增加了复杂性。
   - **索引性能不佳**：基于字符串的索引在处理范围查询（如查找特定时间段内的数据）时，其效率和灵活性通常不如原生日期时间类型的索引。

## DATETIME 和 TIMESTAMP 选择

`DATETIME` 和 `TIMESTAMP` 是 MySQL 中两种非常常用的、用于存储包含日期和时间信息的数据类型。它们都可以存储精确到秒（MySQL 5.6.4+ 支持更高精度的小数秒）的时间值。那么，在实际应用中，我们应该如何在这两者之间做出选择呢？

下面我们从几个关键维度对它们进行对比：

### 时区信息

<font color=blue>`DATETIME` 类型存储的是**字面量的日期和时间值**，它本身**不包含任何时区信息**。</font>当你插入一个 `DATETIME` 值时，MySQL 存储的就是你提供的那个确切的时间，不会进行任何时区转换。

**这样就会有什么问题呢？** 如果你的应用需要支持多个时区，或者服务器、客户端的时区可能发生变化，那么使用 `DATETIME` 时，应用程序需要自行处理时区的转换和解释。如果处理不当（例如，假设所有存储的时间都属于同一个时区，但实际环境变化了），可能会导致时间显示或计算上的混乱。

<font color=blue>**`TIMESTAMP` 和时区有关**。</font>存储时，MySQL 会将当前会话时区下的时间值转换成 UTC（协调世界时）进行内部存储。当查询 `TIMESTAMP` 字段时，MySQL 又会将存储的 UTC 时间转换回当前会话所设置的时区来显示。

这意味着，对于同一条记录的 `TIMESTAMP` 字段，在不同的会话时区设置下查询，可能会看到不同的本地时间表示，但它们都对应着同一个绝对时间点（UTC 时间）。这对于需要全球化、多时区支持的应用来说非常有用。

下面实际演示一下！

建表 SQL 语句：

```sql
CREATE TABLE `time_zone_test` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `date_time` datetime DEFAULT NULL,
  `time_stamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```

插入一条数据（假设当前会话时区为系统默认，例如 UTC+0）:：

```sql
INSERT INTO time_zone_test(date_time,time_stamp) VALUES(NOW(),NOW());
```

查询数据（在同一时区会话下）：

```sql
SELECT date_time, time_stamp FROM time_zone_test;
```

结果：

```plain
+---------------------+---------------------+
| date_time           | time_stamp          |
+---------------------+---------------------+
| 2020-01-11 09:53:32 | 2020-01-11 09:53:32 |
+---------------------+---------------------+
```

现在，修改当前会话的时区为东八区 (UTC+8):

```sql
SET time_zone = '+8:00';
```

再次查询数据：

```bash
# TIMESTAMP 的值自动转换为 UTC+8 时间
+---------------------+---------------------+
| date_time           | time_stamp          |
+---------------------+---------------------+
| 2020-01-11 09:53:32 | 2020-01-11 17:53:32 |
+---------------------+---------------------+
```

**扩展：MySQL 时区设置常用 SQL 命令**

```sql
# 查看当前会话时区
SELECT @@session.time_zone;
# 设置当前会话时区
SET time_zone = 'Europe/Helsinki';
SET time_zone = "+00:00";
# 数据库全局时区设置
SELECT @@global.time_zone;
# 设置全局时区
SET GLOBAL time_zone = '+8:00';
SET GLOBAL time_zone = 'Europe/Helsinki';
```

### 占用空间

下图是 MySQL 日期类型所占的存储空间（官方文档传送门：https://dev.mysql.com/doc/refman/8.0/en/storage-requirements.html）：

![img](https://oss.javaguide.cn/github/javaguide/FhRGUVHFK0ujRPNA75f6CuOXQHTE.jpeg)

<font color=24b3aa>在 MySQL 5.6.4 之前，DateTime 和 TIMESTAMP 的存储空间是固定的，分别为 8 字节和 4 字节。但是从 MySQL 5.6.4 开始，它们的存储空间会根据毫秒精度的不同而变化，DateTime 的范围是 5~8 字节，TIMESTAMP 的范围是 4~7 字节。</font>

### 表示范围

`TIMESTAMP` 表示的时间范围更小，只能到 2038 年：

- `DATETIME`：'1000-01-01 00:00:00.000000' 到 '9999-12-31 23:59:59.999999'
- `TIMESTAMP`：'1970-01-01 00:00:01.000000' UTC 到 '2038-01-19 03:14:07.999999' UTC

### 性能

由于 `TIMESTAMP` 在存储和检索时需要进行 UTC 与当前会话时区的转换，这个过程可能涉及到额外的计算开销，尤其是在需要调用操作系统底层接口获取或处理时区信息时。虽然现代数据库和操作系统对此进行了优化，但在某些极端高并发或对延迟极其敏感的场景下，`DATETIME` 因其不涉及时区转换，处理逻辑相对更简单直接，可能会表现出微弱的性能优势。

为了获得可预测的行为并可能减少 `TIMESTAMP` 的转换开销，推荐的做法是在应用程序层面统一管理时区，或者在数据库连接/会话级别显式设置 `time_zone` 参数，而不是依赖服务器的默认或操作系统时区。

## 数值时间戳是更好的选择吗？

除了上述两种类型，实践中也常用整数类型（`INT` 或 `BIGINT`）来存储所谓的“Unix 时间戳”（即从 1970 年 1 月 1 日 00:00:00 UTC 起至目标时间的总秒数，或毫秒数）。

<font color=blue>这种存储方式的具有 `TIMESTAMP` 类型的所具有一些优点，并且使用它的进行日期排序以及对比等操作的效率会更高，跨系统也很方便，毕竟只是存放的数值。缺点也很明显，就是数据的可读性太差了，你无法直观的看到具体时间。</font>

时间戳的定义如下：

> 时间戳的定义是从一个基准时间开始算起，这个基准时间是「1970-1-1 00:00:00 +0:00」，从这个时间开始，用整数表示，以秒计时，随着时间的流逝这个时间整数不断增加。这样一来，我只需要一个数值，就可以完美地表示时间了，而且这个数值是一个绝对数值，即无论的身处地球的任何角落，这个表示时间的时间戳，都是一样的，生成的数值都是一样的，并且没有时区的概念，所以在系统的中时间的传输中，都不需要进行额外的转换了，只有在显示给用户的时候，才转换为字符串格式的本地时间。

数据库中实际操作：

```sql
-- 将日期时间字符串转换为 Unix 时间戳 (秒)
mysql> SELECT UNIX_TIMESTAMP('2020-01-11 09:53:32');
+---------------------------------------+
| UNIX_TIMESTAMP('2020-01-11 09:53:32') |
+---------------------------------------+
|                            1578707612 |
+---------------------------------------+
1 row in set (0.00 sec)

-- 将 Unix 时间戳 (秒) 转换为日期时间格式
mysql> SELECT FROM_UNIXTIME(1578707612);
+---------------------------+
| FROM_UNIXTIME(1578707612) |
+---------------------------+
| 2020-01-11 09:53:32       |
+---------------------------+
1 row in set (0.01 sec)
```

## PostgreSQL 中没有 DATETIME

由于有读者提到 PostgreSQL（PG） 的时间类型，因此这里拓展补充一下。PG 官方文档对时间类型的描述地址：https://www.postgresql.org/docs/current/datatype-datetime.html。

![PostgreSQL 时间类型总结](https://oss.javaguide.cn/github/javaguide/mysql/pg-datetime-types.png)

可以看到，PG 没有名为 `DATETIME` 的类型：

- PG 的 `TIMESTAMP WITHOUT TIME ZONE`在功能上最接近 MySQL 的 `DATETIME`。它存储日期和时间，但不包含任何时区信息，存储的是字面值。
- PG 的`TIMESTAMP WITH TIME ZONE` (或 `TIMESTAMPTZ`) 相当于 MySQL 的 `TIMESTAMP`。它在存储时会将输入值转换为 UTC，并在检索时根据当前会话的时区进行转换显示。

对于绝大多数需要记录精确发生时间点的应用场景，`TIMESTAMPTZ`是 PostgreSQL 中最推荐、最健壮的选择，因为它能最好地处理时区复杂性。

## 总结

MySQL 中时间到底怎么存储才好？`DATETIME`？`TIMESTAMP`？还是数值时间戳？

并没有一个银弹，很多程序员会觉得数值型时间戳是真的好，效率又高还各种兼容，但是很多人又觉得它表现的不够直观。

《高性能 MySQL 》这本神书的作者就是推荐 TIMESTAMP，原因是数值表示时间不够直观。下面是原文：

![img](https://oss.javaguide.cn/github/javaguide/%E9%AB%98%E6%80%A7%E8%83%BDmysql-%E4%B8%8D%E6%8E%A8%E8%8D%90%E7%94%A8%E6%95%B0%E5%80%BC%E6%97%B6%E9%97%B4%E6%88%B3.jpg)

每种方式都有各自的优势，根据实际场景选择最合适的才是王道。下面再对这三种方式做一个简单的对比，以供大家实际开发中选择正确的存放时间的数据类型：

| 类型         | 存储空间 | 日期格式                       | 日期范围                                                     | 是否带时区信息 |
| ------------ | -------- | ------------------------------ | ------------------------------------------------------------ | -------------- |
| DATETIME     | 5~8 字节 | YYYY-MM-DD hh:mm:ss[.fraction] | 1000-01-01 00:00:00[.000000] ～ 9999-12-31 23:59:59[.999999] | 否             |
| TIMESTAMP    | 4~7 字节 | YYYY-MM-DD hh:mm:ss[.fraction] | 1970-01-01 00:00:01[.000000] ～ 2038-01-19 03:14:07[.999999] | 是             |
| 数值型时间戳 | 4 字节   | 全数字如 1578707612            | 1970-01-01 00:00:01 之后的时间                               | 否             |

**选择建议小结：**

- `TIMESTAMP` 的核心优势在于其内建的时区处理能力。数据库负责 UTC 存储和基于会话时区的自动转换，简化了需要处理多时区应用的开发。如果应用需要处理多时区，或者希望数据库能自动管理时区转换，`TIMESTAMP` 是自然的选择（注意其时间范围限制，也就是 2038 年问题）。
- 如果应用场景不涉及时区转换，或者希望应用程序完全控制时区逻辑，并且需要表示 2038 年之后的时间，`DATETIME` 是更稳妥的选择。
- 如果极度关注比较性能，或者需要频繁跨系统传递时间数据，并且可以接受可读性的牺牲（或总是在应用层转换），数值时间戳是一个强大的选项。

# [存储引擎]执行一条 SELECT 语句，期间发生了什么？

大家好，我是小林。

学习 SQL 的时候，大家肯定第一个先学到的就是 SELECT 查询语句了，比如下面这句查询语句：

```sql
// 在 product 表中，查询 id = 1 的记录
SELECT * FROM product WHERE id = 1;
```

但是有没有想过，**MySQL 执行一条 SELECT 查询语句，在 MySQL 中期间发生了什么？**

带着这个问题，我们可以很好的了解 MySQL 内部的架构，所以这次小林就带大家拆解一下 MySQL 内部的结构，看看内部里的每一个“零件”具体是负责做什么的。

## [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#mysql-执行流程是怎样的)MySQL 执行流程是怎样的？

先来一个上帝视角图，下面就是 MySQL 执行一条 SQL 查询语句的流程，也从图中可以看到 MySQL 内部架构里的各个功能模块。

![查询语句执行流程](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/mysql%E6%9F%A5%E8%AF%A2%E6%B5%81%E7%A8%8B.png)

可以看到， MySQL 的架构共分为两层：**Server 层和存储引擎层**，

- **<font color=24b3aa>Server 层负责建立连接、分析和执行 SQL</font>**。MySQL 大多数的核心功能模块都在这实现，主要包括<font color=blue>连接器，查询缓存、解析器、预处理器、优化器、执行器</font>等。另外，所有的<font color=blue>内置函数</font>（如日期、时间、数学和加密函数等）和所有<font color=blue>跨存储引擎</font>的功能（如存储过程、触发器、视图等。）都在 Server 层实现。
- **<font color=24b3aa>存储引擎层负责数据的存储和提取</font>**。支持 InnoDB、MyISAM、Memory 等多个存储引擎，不同的存储引擎共用一个 Server 层。现在最常用的存储引擎是 InnoDB，从 MySQL 5.5 版本开始， InnoDB 成为了 MySQL 的默认存储引擎。我们常说的索引数据结构，就是由存储引擎层实现的，不同的存储引擎支持的索引类型也不相同，比如 InnoDB 支持索引类型是 B+树 ，且是默认使用，也就是说在数据表中创建的主键索引和二级索引默认使用的是 B+ 树索引。

好了，现在我们对 Server 层和存储引擎层有了一个简单认识，接下来，就详细说一条 SQL 查询语句的执行流程，依次看看每一个功能模块的作用。

## [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#第一步-连接器)第一步：连接器

如果你在 Linux 操作系统里要使用 MySQL，那你第一步肯定是要先连接 MySQL 服务，然后才能执行 SQL 语句，普遍我们都是使用下面这条命令进行连接：

```shell
# -h 指定 MySQL 服务得 IP 地址，如果是连接本地的 MySQL服务，可以不用这个参数；
# -u 指定用户名，管理员角色名为 root；
# -p 指定密码，如果命令行中不填写密码（为了密码安全，建议不要在命令行写密码），就需要在交互对话里面输入密码
mysql -h$ip -u$user -p
```

<font color=24b3aa>连接的过程需要先经过 TCP 三次握手，因为 MySQL 是基于 **TCP 协议** 进行传输的</font>，如果 MySQL 服务并没有启动，则会收到如下的报错：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/mysql%E8%BF%9E%E6%8E%A5%E9%94%99%E8%AF%AF.png)

如果 MySQL 服务正常运行，完成 TCP 连接的建立后，连接器就要开始验证你的用户名和密码，如果用户名或密码不对，就收到一个"Access denied for user"的错误，然后客户端程序结束执行。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E5%AF%86%E7%A0%81%E9%94%99%E8%AF%AF.png)

如果用户密码都没有问题，连接器就会获取该用户的权限，然后保存起来，<font color=24b3aa>后续该用户在此连接里的任何操作，都会基于连接开始时读到的权限进行权限逻辑的判断。</font>

<font color=24b3aa>所以，如果一个用户已经建立了连接，即使管理员中途修改了该用户的权限，也不会影响已经存在连接的权限。修改完成后，只有再新建的连接才会使用新的权限设置。</font>

> 如何查看 MySQL 服务被多少个客户端连接了？

如果你想知道当前 MySQL 服务被多少个客户端连接了，你可以执行 `show processlist` 命令进行查看。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E6%9F%A5%E7%9C%8B%E8%BF%9E%E6%8E%A5.png)

比如上图的显示结果，共有两个用户名为 root 的用户连接了 MySQL 服务，其中 id 为 6 的用户的 Command 列的状态为 `Sleep` ，这意味着该用户连接完 MySQL 服务就没有再执行过任何命令，也就是说这是一个空闲的连接，并且空闲的时长是 736 秒（ Time 列）。

> 空闲连接会一直占用着吗？

当然不是了，MySQL 定义了空闲连接的最大空闲时长，由 `wait_timeout` 参数控制的，默认值是 8 小时（28880秒），如果空闲连接超过了这个时间，连接器就会自动将它断开。

```sql
mysql> show variables like 'wait_timeout';
+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| wait_timeout  | 28800 |
+---------------+-------+
1 row in set (0.00 sec)
```

当然，我们自己也可以手动断开空闲的连接，使用的是 kill connection + id 的命令。

```sql
mysql> kill connection +6;
Query OK, 0 rows affected (0.00 sec)
```

一个处于空闲状态的连接被服务端主动断开后，这个客户端并不会马上知道，等到客户端在发起下一个请求的时候，才会收到这样的报错“ERROR 2013 (HY000): Lost connection to MySQL server during query”。

> MySQL 的连接数有限制吗？

MySQL 服务支持的最大连接数由 max_connections 参数控制，比如我的 MySQL 服务默认是 151 个，超过这个值，系统就会拒绝接下来的连接请求，并报错提示“Too many connections”。

```sql
mysql> show variables like 'max_connections';
+-----------------+-------+
| Variable_name   | Value |
+-----------------+-------+
| max_connections | 151   |
+-----------------+-------+
1 row in set (0.00 sec)
```

<font color=24b3aa>MySQL 的连接也跟 HTTP 一样，有短连接和长连接的概念</font>，它们的区别如下：

```c
// 短连接
连接 mysql 服务（TCP 三次握手）
执行sql
断开 mysql 服务（TCP 四次挥手）

// 长连接
连接 mysql 服务（TCP 三次握手）
执行sql
执行sql
执行sql
....
断开 mysql 服务（TCP 四次挥手）
```

可以看到，使用长连接的好处就是可以减少建立连接和断开连接的过程，所以一般是推荐使用长连接。

但是，使用长连接后可能会占用内存增多，因为 MySQL 在执行查询过程中临时使用内存管理连接对象，这些连接对象资源只有在连接断开时才会释放。如果长连接累计很多，将导致 MySQL 服务占用内存太大，有可能会被系统强制杀掉，这样会发生 MySQL 服务异常重启的现象。

> 怎么解决长连接占用内存的问题？

有两种解决方式。

第一种，**定期断开长连接**。既然断开连接后就会释放连接占用的内存资源，那么我们可以定期断开长连接。

第二种，**客户端主动重置连接**。MySQL 5.7 版本实现了 `mysql_reset_connection()` 函数的接口，注意这是接口函数不是命令，那么当客户端执行了一个很大的操作后，在代码里调用 mysql_reset_connection 函数来重置连接，达到释放内存的效果。这个过程不需要重连和重新做权限验证，但是会将连接恢复到刚刚创建完时的状态。

至此，连接器的工作做完了，简单总结一下：

- 与客户端进行 TCP 三次握手建立连接；
- 校验客户端的用户名和密码，如果用户名或密码不对，则会报错；
- 如果用户名和密码都对了，会读取该用户的权限，然后后面的权限逻辑判断都基于此时读取到的权限；

## [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#第二步-查询缓存)第二步：查询缓存

连接器得工作完成后，客户端就可以向 MySQL 服务发送 SQL 语句了，MySQL 服务收到 SQL 语句后，就会解析出 SQL 语句的第一个字段，看看是什么类型的语句。

如果 SQL 是查询语句（SELECT 语句），MySQL 就会先去查询缓存（ Query Cache ）里查找缓存数据，看看之前有没有执行过这一条命令，这个查询缓存是以 key-value 形式保存在内存中的，key 为 SQL 查询语句，value 为 SQL 语句查询的结果。

如果查询的语句命中查询缓存，那么就会直接返回 value 给客户端。如果查询的语句没有命中查询缓存中，那么就要往下继续执行，等执行完后，查询的结果就会被存入查询缓存中。

这么看，查询缓存还挺有用，但是其实 **查询缓存挺鸡肋** 的。

<font color=24b3aa>对于更新比较频繁的表，查询缓存的命中率很低的，因为只要一个表有更新操作，那么这个表的查询缓存就会被清空。如果刚缓存了一个查询结果很大的数据，还没被使用的时候，刚好这个表有更新操作，查询缓冲就被清空了，相当于缓存了个寂寞。</font>

所以，<font color=24b3aa>MySQL 8.0 版本直接将查询缓存删掉了，也就是说 MySQL 8.0 开始，执行一条 SQL 查询语句，不会再走到查询缓存这个阶段了。</font>

对于 MySQL 8.0 之前的版本，如果想关闭查询缓存，我们可以通过将参数 query_cache_type 设置成 DEMAND。

TIP

这里说的查询缓存是 server 层的，也就是 MySQL 8.0 版本移除的是 server 层的查询缓存，并不是 Innodb 存储引擎中的 buffer pool。

## [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#第三步-解析-sql)第三步：解析 SQL

在正式执行 SQL 查询语句之前， MySQL 会先对 SQL 语句做解析，这个工作交由「解析器」来完成。

### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#解析器)解析器

解析器会做如下两件事情。

第一件事情，**词法分析**。MySQL 会根据你输入的字符串识别出关键字出来，例如，SQL语句 SELECT username FROM userinfo，在分析之后，会得到 4 个Token，其中有 2 个 Keyword，分别为 SELECT 和 FROM：

| 关键字 | 非关键字 | 关键字 | 非关键字 |
| ------ | -------- | ------ | -------- |
| SELECT | username | FROM   | userinfo |

第二件事情，**语法分析**。根据词法分析的结果，语法解析器会根据语法规则，判断你输入的这个 SQL 语句是否满足 MySQL 语法，如果没问题就会构建出 SQL 语法树，这样方便后面模块获取 SQL 类型、表名、字段名、 WHERE 条件等等。

![img](https://cdn.xiaolincoding.com//picgo/db-mysql-sql-parser-2.png)

如果我们输入的 SQL 语句语法不对，就会在解析器这个阶段报错。比如，我下面这条查询语句，把 FROM 写成了 form，这时 MySQL 解析器就会给报错。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E8%AF%AD%E6%B3%95%E9%94%99%E8%AF%AF.png)

但是注意，<font color=24b3aa>**表不存在或者字段不存在**，并不是在解析器里做的，《MySQL 45 讲》说是在解析器做的，但是经过我和朋友看 MySQL 源码（5.7和8.0）得出结论是解析器只负责检查语法和构建语法树，但是不会去查表或者字段存不存在。</font>

那到底谁来做检测表和字段是否存在的工作呢？别急，接下来就是了。

## [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#第四步-执行-sql)第四步：执行 SQL

经过解析器后，接着就要进入执行 SQL 查询语句的流程了，每条 `SELECT` 查询语句流程主要可以分为下面这三个阶段：

- prepare 阶段，也就是预处理阶段；
- optimize 阶段，也就是优化阶段；
- execute 阶段，也就是执行阶段；

### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#预处理器)预处理器

我们先来说说预处理阶段做了什么事情。

- <font color=24b3aa>检查 SQL 查询语句中的表或者字段是否存在；</font>
- <font color=24b3aa>将 `SELECT *` 中的 `*` 符号，扩展为表上的所有列；</font>

我下面这条查询语句，test 这张表是不存在的，这时 MySQL 就会在执行 SQL 查询语句的 prepare 阶段中报错。

```sql
mysql> SELECT * FROM test;
ERROR 1146 (42S02): Table 'mysql.test' doesn't exist
```

这里贴个 MySQL 8.0 源码来证明表或字段是否存在的判断，不是在解析器里做的，而是在 prepare 阶段。（*PS：下图是公众号「一树一溪」老哥帮我分析的，这位老哥专门写 MySQL 源码文章，感兴趣的朋友，可以微信搜索关注*）

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E8%A1%A8%E4%B8%8D%E5%AD%98%E5%9C%A8.jpeg)

上面的中间部分是 MySQL 报错表不存在时的函数调用栈，可以看到表不存在的错误是在 get_table_share() 函数里报错的，而这个函数是在 prepare 阶段调用的。

不过，对于 MySQL 5.7 判断表或字段是否存在的工作，是在词法分析&语法分析之后，prepare 阶段之前做的。结论都一样，不是在解析器里做的。代码我就不放了，正因为 MySQL 5.7 代码结构不好，所以 MySQL 8.0 代码结构变化很大，后来判断表或字段是否存在的工作就被放入到 prepare 阶段做了。

### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#优化器)优化器

经过预处理阶段后，还需要为 SQL 查询语句先制定一个执行计划，这个工作交由「优化器」来完成的。

**<font color=24b3aa>优化器主要负责将 SQL 查询语句的执行方案确定下来</font>**，比如在表里面有多个索引的时候，优化器会基于查询成本的考虑，来决定选择使用哪个索引。

当然，我们本次的查询语句（SELECT * FROM product WHERE id = 1）很简单，就是选择使用主键索引。

要想知道优化器选择了哪个索引，我们可以在查询语句最前面加个 `explain` 命令，这样就会输出这条 SQL 语句的执行计划，然后执行计划中的 key 就表示执行过程中使用了哪个索引，比如下图的 key 为 `PRIMARY` 就是使用了主键索引。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E6%89%A7%E8%A1%8C%E8%AE%A1%E5%88%92.png)

如果查询语句的执行计划里的 key 为 null 说明没有使用索引，那就会全表扫描（type = ALL），这种查询扫描的方式是效率最低档次的，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E5%85%A8%E8%A1%A8%E6%89%AB%E6%8F%8F.png)

这张 product 表只有一个索引就是主键，现在我在表中将 name 设置为普通索引（二级索引）。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E4%BA%A7%E5%93%81%E8%A1%A8.png)

这时 product 表就有主键索引（id）和普通索引（name）。假设执行了这条查询语句：

```sql
SELECT id FROM product WHERE id > 1  and name like 'i%';
```

这条查询语句的结果既可以使用主键索引，也可以使用普通索引，但是执行的效率会不同。这时，就需要优化器来决定使用哪个索引了。

很显然这条查询语句是**覆盖索引**，直接在二级索引就能查找到结果（因为二级索引的 B+ 树的叶子节点的数据存储的是主键值），就没必要在主键索引查找了，因为查询主键索引的 B+ 树的成本会比查询二级索引的 B+ 的成本大，优化器基于查询成本的考虑，会选择查询代价小的普通索引。

在下图中执行计划，我们可以看到，执行过程中使用了普通索引（name），Exta 为 Using index，这就是表明使用了覆盖索引优化。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E9%80%89%E6%8B%A9%E7%B4%A2%E5%BC%95.png)

### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#执行器)执行器

经历完优化器后，就确定了执行方案，接下来 MySQL 就真正开始执行语句了，这个工作是由「执行器」完成的。**<font color=24b3aa>在执行的过程中，执行器就会和存储引擎交互了，交互是以记录为单位的</font>。**

接下来，用三种方式执行过程，跟大家说一下执行器和存储引擎的交互过程（PS ：为了写好这一部分，特地去看 MySQL 源码，也是第一次看哈哈）。

- 主键索引查询
- 全表扫描
- 索引下推

#### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#主键索引查询)主键索引查询

以本文开头查询语句为例，看看执行器是怎么工作的。

```sql
SELECT * FROM product WHERE id = 1;
```

这条查询语句的查询条件用到了主键索引，而且是等值查询，同时主键 id 是唯一，不会有 id 相同的记录，所以优化器决定选用访问类型为 const 进行查询，也就是使用主键索引查询一条记录，那么执行器与存储引擎的执行流程是这样的：

- 执行器第一次查询，会调用 read_first_record 函数指针指向的函数，因为优化器选择的访问类型为 const，这个函数指针被指向为 InnoDB 引擎索引查询的接口，把条件 `id = 1` 交给存储引擎，**让存储引擎定位符合条件的第一条记录**。
- 存储引擎通过主键索引的 B+ 树结构定位到 id = 1的第一条记录，如果记录是不存在的，就会向执行器上报记录找不到的错误，然后查询结束。如果记录是存在的，就会将记录返回给执行器；
- 执行器从存储引擎读到记录后，接着判断记录是否符合查询条件，如果符合则发送给客户端，如果不符合则跳过该记录。
- 执行器查询的过程是一个 while 循环，所以还会再查一次，但是这次因为不是第一次查询了，所以会调用 read_record 函数指针指向的函数，因为优化器选择的访问类型为 const，这个函数指针被指向为一个永远返回 - 1 的函数，所以当调用该函数的时候，执行器就退出循环，也就是结束查询了。

至此，这个语句就执行完成了。

#### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#全表扫描)全表扫描

举个全表扫描的例子：

```sql
SELECT * FROM product WHERE name = 'iphone';
```

这条查询语句的查询条件没有用到索引，所以优化器决定选用访问类型为 ALL 进行查询，也就是全表扫描的方式查询，那么这时执行器与存储引擎的执行流程是这样的：

- 执行器第一次查询，会调用 read_first_record 函数指针指向的函数，因为优化器选择的访问类型为 all，这个函数指针被指向为 InnoDB 引擎全扫描的接口，**让存储引擎读取表中的第一条记录**；
- 执行器会判断读到的这条记录的 name 是不是 iphone，如果不是则跳过；如果是则将记录发给客户的（是的没错，Server 层每从存储引擎读到一条记录就会发送给客户端，之所以客户端显示的时候是直接显示所有记录的，是因为客户端是等查询语句查询完成后，才会显示出所有的记录）。
- 执行器查询的过程是一个 while 循环，所以还会再查一次，会调用 read_record 函数指针指向的函数，因为优化器选择的访问类型为 all，read_record 函数指针指向的还是 InnoDB 引擎全扫描的接口，所以接着向存储引擎层要求继续读刚才那条记录的下一条记录，存储引擎把下一条记录取出后就将其返回给执行器（Server层），执行器继续判断条件，不符合查询条件即跳过该记录，否则发送到客户端；
- 一直重复上述过程，直到存储引擎把表中的所有记录读完，然后向执行器（Server层） 返回了读取完毕的信息；
- 执行器收到存储引擎报告的查询完毕的信息，退出循环，停止查询。

至此，这个语句就执行完成了。

#### [#](https://www.xiaolincoding.com/mysql/base/how_SELECT.html#索引下推)<font color=blue>索引下推</font>

在这部分非常适合讲索引下推（MySQL 5.6 推出的查询优化策略），这样大家能清楚的知道，「下推」这个动作，下推到了哪里。

索引下推能够减少**二级索引**在查询时的回表操作，提高查询的效率，因为它将 Server 层部分负责的事情，交给存储引擎层去处理了。

举一个具体的例子，方便大家理解，这里一张用户表如下，我对 age 和 reward 字段建立了联合索引（age，reward）：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E8%B7%AF%E9%A3%9E%E8%A1%A8.png)

现在有下面这条查询语句：

```sql
SELECT * FROM t_user  WHERE age > 20 and reward = 100000;
```

联合索引当遇到范围查询 (>、<) 就会停止匹配，也就是 **age 字段能用到联合索引，但是 reward 字段则无法利用到索引**。具体原因这里可以看这篇：[索引常见面试题(opens new window)](https://xiaolincoding.com/mysql/index/index_interview.html#按字段个数分类)

那么，不使用索引下推（MySQL 5.6 之前的版本）时，执行器与存储引擎的执行流程是这样的：

- Server 层首先调用存储引擎的接口定位到满足查询条件的第一条二级索引记录，也就是定位到 age > 20 的第一条记录；
- 存储引擎根据二级索引的 B+ 树快速定位到这条记录后，获取主键值，然后**进行回表操作**，将完整的记录返回给 Server 层；
- Server 层在判断该记录的 reward 是否等于 100000，如果成立则将其发送给客户端；否则跳过该记录；
- 接着，继续向存储引擎索要下一条记录，存储引擎在二级索引定位到记录后，获取主键值，然后回表操作，将完整的记录返回给 Server 层；
- 如此往复，直到存储引擎把表中的所有记录读完。

可以看到，没有索引下推的时候，每查询到一条二级索引记录，都要进行回表操作，然后将记录返回给 Server，接着 Server 再判断该记录的 reward 是否等于 100000。

而使用索引下推后，判断记录的 reward 是否等于 100000 的工作交给了存储引擎层，过程如下 ：

- Server 层首先调用存储引擎的接口定位到满足查询条件的第一条二级索引记录，也就是定位到 age > 20 的第一条记录；
- 存储引擎定位到二级索引后，**先不执行回表**操作，而是先判断一下该索引中包含的列（reward列）的条件（reward 是否等于 100000）是否成立。如果**条件不成立**，则直接**跳过该二级索引**。如果**成立**，则**执行回表**操作，将完成记录返回给 Server 层。
- Server 层在判断其他的查询条件（本次查询没有其他条件）是否成立，如果成立则将其发送给客户端；否则跳过该记录，然后向存储引擎索要下一条记录。
- 如此往复，直到存储引擎把表中的所有记录读完。

可以看到，使用了索引下推后，虽然 reward 列无法使用到联合索引，但是因为它包含在联合索引（age，reward）里，所以直接在存储引擎过滤出满足 reward = 100000 的记录后，才去执行回表操作获取整个记录。相比于没有使用索引下推，节省了很多回表操作。

当你发现执行计划里的 Extr 部分显示了 <font color=blue>“Using index condition”</font>，说明使用了索引下推。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/%E7%B4%A2%E5%BC%95%E4%B8%8B%E6%8E%A8%E6%89%A7%E8%A1%8C%E8%AE%A1%E5%88%92.png)

------

总结

![img](https://cdn.xiaolincoding.com//mysql/other/1623727651911_20170928110355446.png)

<font color=24b3aa>执行一条 SQL 查询语句，期间发生了什么？</font>

- 连接器：建立连接，管理连接、校验用户身份；
- 查询缓存：查询语句如果命中查询缓存则直接返回，否则继续往下执行。MySQL 8.0 已删除该模块；
- 解析 SQL，通过解析器对 SQL 查询语句进行词法分析、语法分析，然后构建语法树，方便后续模块读取表名、字段、语句类型；
- 执行 SQL：执行 SQL 共有三个阶段：
  - 预处理阶段：检查表或字段是否存在；将 `SELECT *` 中的 `*` 符号扩展为表上的所有列。
  - 优化阶段：基于查询成本的考虑， 选择查询成本最小的执行计划；
  - 执行阶段：根据执行计划执行 SQL 查询语句，从存储引擎读取记录，返回给客户端；

怎么样？现在再看这张图，是不是很清晰了。

![查询语句执行流程](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/mysql%E6%9F%A5%E8%AF%A2%E6%B5%81%E7%A8%8B.png)

------

参考资料：

- 《MySQL 45 讲》
- 《MySQL是怎样运行的：从根儿上理解MySQL》
- https://gohalo.me/post/mysql-executor.html
- http://www.iskm.org/mysql56/sql__executor_8cc_source.html
- https://tangocc.github.io/2018/10/11/mysql-sourcecode/

# [存储引擎]MySQL 一行记录是怎么存储的？

你能回答以下问题吗？

- MySQL 的 NULL 值是怎么存放的？
- MySQL 的 NULL 值会占用空间吗？
- MySQL 怎么知道 varchar(n) 实际占用数据的大小？
- varchar(n) 中 n 最大取值为多少？
- 行溢出后，MySQL 是怎么处理的？

这些问题看似毫不相干，其实都是在围绕「 MySQL 一行记录的存储结构」这一个知识点，所以攻破了这个知识点后，这些问题就引刃而解了。

好了，话不多说，发车！

## [#](https://www.xiaolincoding.com/mysql/base/row_format.html#mysql-的数据存放在哪个文件)MySQL 的数据存放在哪个文件？

大家都知道 MySQL 的数据都是保存在磁盘的，那具体是保存在哪个文件呢？

<font color=24b3aa>MySQL 存储的行为是由存储引擎实现的</font>，MySQL 支持多种存储引擎，不同的存储引擎保存的文件自然也不同。

InnoDB 是我们常用的存储引擎，也是 MySQL 默认的存储引擎。所以，本文主要以 InnoDB 存储引擎展开讨论。

先来看看 MySQL 数据库的文件存放在哪个目录？

```sql
mysql> SHOW VARIABLES LIKE 'datadir';
+---------------+-----------------+
| Variable_name | Value           |
+---------------+-----------------+
| datadir       | /var/lib/mysql/ |
+---------------+-----------------+
1 row in set (0.00 sec)
```

我们每创建一个 database（数据库） 都会在 /var/lib/mysql/ 目录里面创建一个以 database 为名的目录，然后保存表结构和表数据的文件都会存放在这个目录里。

比如，我这里有一个名为 my_test 的 database，该 database 里有一张名为 t_order 数据库表。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/database.png)

然后，我们进入 /var/lib/mysql/my_test 目录，看看里面有什么文件？

```shell
[root@xiaolin ~]#ls /var/lib/mysql/my_test
db.opt  
t_order.frm  
t_order.ibd
```

可以看到，共有三个文件，这三个文件分别代表着：

- db.opt，用来存储当前数据库的默认字符集和字符校验规则。
- t_order.frm ，t_order 的 **表结构** 会保存在这个文件。在 MySQL 中建立一张表都会生成一个.frm 文件，该文件是用来保存每个表的元数据信息的，主要包含表结构定义。
- t_order.ibd，t_order 的 **表数据** 会保存在这个文件。表数据既可以存在共享表空间文件（文件名：ibdata1）里，也可以存放在独占表空间文件（文件名：表名字.ibd）。这个行为是由参数 innodb_file_per_table 控制的，若设置了参数 innodb_file_per_table 为 1，则会将存储的数据、索引等信息单独存储在一个独占表空间，从 MySQL 5.6.6 版本开始，它的默认值就是 1 了，因此从这个版本之后， MySQL 中每一张表的数据都存放在一个独立的 .ibd 文件。

好了，现在我们知道了一张数据库表的数据是保存在「 表名字.ibd 」的文件里的，这个文件也称为独占表空间文件。

### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#表空间文件的结构是怎么样的)表空间文件的结构是怎么样的？

**表空间由段（segment）、区（extent）、页（page）、行（row）组成**， InnoDB 存储引擎的逻辑存储结构大致如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E8%A1%A8%E7%A9%BA%E9%97%B4%E7%BB%93%E6%9E%84.drawio.png)

下面我们从下往上一个个看看。

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_1、行-row)1、行（row）

<font color=24b3aa>数据库表中的记录都是按行（row）进行存放的，每行记录根据不同的行格式，有不同的存储结构。</font>

后面我们详细介绍 InnoDB 存储引擎的行格式，也是本文重点介绍的内容。

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_2、页-page)2、页（page）

记录是按照行来存储的，但是数据库的读取并不以「行」为单位，否则一次读取（也就是一次 I/O 操作）只能处理一行数据，效率会非常低。

因此，**<font color=24b3aa>InnoDB 的数据是按「页」为单位来读写的</font>**，也就是说，当需要读一条记录的时候，并不是将这个行记录从磁盘读出来，而是以页为单位，将其整体读入内存。

**<font color=24b3aa>默认每个页的大小为 16KB</font>**，也就是最多能保证 16KB 的连续存储空间。

<font color=24b3aa>页是 InnoDB 存储引擎磁盘管理的最小单元</font>，意味着数据库每次读写都是以 16KB 为单位的，一次最少从磁盘中读取 16K 的内容到内存中，一次最少把内存中的 16K 内容刷新到磁盘中。

页的类型有很多，常见的有数据页、undo 日志页、溢出页等等。数据表中的行记录是用「数据页」来管理的，数据页的结构这里我就不讲细说了，之前文章有说过，感兴趣的可以去看这篇文章：[换一个角度看 B+ 树(opens new window)](https://xiaolincoding.com/mysql/index/page.html)

总之知道表中的记录存储在「数据页」里面就行。

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_3、区-extent)3、区（extent）

我们知道 InnoDB 存储引擎是用 B+ 树来组织数据的。

B+ 树中每一层都是通过双向链表连接起来的，如果是以页为单位来分配存储空间，那么链表中相邻的两个页之间的物理位置并不是连续的，可能离得非常远，那么磁盘查询时就会有大量的随机I/O，随机 I/O 是非常慢的。

解决这个问题也很简单，就是让链表中相邻的页的物理位置也相邻，这样就可以使用 <font color=24b3aa>顺序 I/O</font> 了，那么在范围查询（扫描叶子节点）的时候性能就会很高。

那具体怎么解决呢？

**<font color=24b3aa>在表中数据量大的时候，为某个索引分配空间的时候就不再按照页为单位分配了，而是按照区（extent）为单位分配。每个区的大小为 1MB，对于 16KB 的页来说，连续的 64 个页会被划为一个区，这样就使得链表中相邻的页的物理位置也相邻，就能使用顺序 I/O 了</font>**。

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_4、段-segment)4、段（segment）

<font color=24b3aa>表空间是由各个段（segment）组成的，段是由多个区（extent）组成的。段一般分为数据段、索引段和回滚段等。</font>

- 索引段：存放 B + 树的非叶子节点的区的集合；
- 数据段：存放 B + 树的叶子节点的区的集合；
- 回滚段：存放的是回滚数据的区的集合，之前讲 [事务隔离 (opens new window)](https://xiaolincoding.com/mysql/transaction/mvcc.html) 的时候就介绍到了 MVCC 利用了回滚段实现了多版本查询数据。

好了，终于说完表空间的结构了。接下来，就具体讲一下 InnoDB 的行格式了。

之所以要绕一大圈才讲行记录的格式，主要是想让大家知道行记录是存储在哪个文件，以及行记录在这个表空间文件中的哪个区域，有一个从上往下切入的视角，这样理解起来不会觉得很抽象。

## [#](https://www.xiaolincoding.com/mysql/base/row_format.html#innodb-行格式有哪些)InnoDB 行格式有哪些？

行格式（row_format），就是一条记录的存储结构。

InnoDB 提供了 4 种行格式，分别是 Redundant、Compact、Dynamic和 Compressed 行格式。

- Redundant 是很古老的行格式了， MySQL 5.0 版本之前用的行格式，现在基本没人用了。
- 由于 Redundant 不是一种紧凑的行格式，所以 MySQL 5.0 之后引入了 Compact 行记录存储方式，Compact 是一种紧凑的行格式，设计的初衷就是为了让一个数据页中可以存放更多的行记录，从 MySQL 5.1 版本之后，行格式默认设置成 Compact。
- Dynamic 和 Compressed 两个都是紧凑的行格式，它们的行格式都和 Compact 差不多，因为都是基于 Compact 改进一点东西。从 MySQL5.7 版本之后，默认使用 Dynamic 行格式。

Redundant 行格式我这里就不讲了，因为现在基本没人用了，这次重点介绍 Compact 行格式，因为 Dynamic 和 Compressed 这两个行格式跟 Compact 非常像。

所以，弄懂了 Compact 行格式，之后你们在去了解其他行格式，很快也能看懂。

## [#](https://www.xiaolincoding.com/mysql/base/row_format.html#compact-行格式长什么样)COMPACT 行格式长什么样？

先跟 Compact 行格式混个脸熟，它长这样：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/COMPACT.drawio.png)

可以看到，<font color=24b3aa>一条完整的记录分为「记录的额外信息」和「记录的真实数据」两个部分。</font>

接下里，分别详细说下。

### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#记录的额外信息)记录的额外信息

记录的额外信息包含 3 个部分：变长字段长度列表、NULL 值列表、记录头信息。

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_1-变长字段长度列表)1. 变长字段长度列表

varchar(n) 和 char(n) 的区别是什么，相信大家都非常清楚，char 是定长的，varchar 是变长的，`varchar(N)` 中，`N` 指最大允许长度；`char(N)` 中，`N` 指固定存储长度。

所以，在存储数据的时候，也要把数据占用的大小存起来，存到「变长字段长度列表」里面，读取数据的时候才能根据这个「变长字段长度列表」去读取对应长度的数据。其他 TEXT、BLOB 等变长字段也是这么实现的。

为了展示「变长字段长度列表」具体是怎么保存「变长字段的真实数据占用的字节数」，我们先创建这样一张表，字符集是 ascii（所以每一个字符占用的 1 字节），行格式是 Compact，t_user 表中 name 和 phone 字段都是变长字段：

```sql
CREATE TABLE `t_user` (
  `id` int(11) NOT NULL,
  `name` VARCHAR(20) DEFAULT NULL,
  `phone` VARCHAR(20) DEFAULT NULL,
  `age` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB DEFAULT CHARACTER SET = ascii ROW_FORMAT = COMPACT;
```

现在 t_user 表里有这三条记录：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/t_test.png)

接下来，我们看看看看这三条记录的行格式中的 「变长字段长度列表」是怎样存储的。

先来看第一条记录：

- name 列的值为 a，真实数据占用的字节数是 1 字节，十六进制 0x01；
- phone 列的值为 123，真实数据占用的字节数是 3 字节，十六进制 0x03；
- age 列和 id 列不是变长字段，所以这里不用管。

这些变长字段的真实数据占用的字节数会按照列的顺序 **逆序存放**（等下会说为什么要这么设计），所以「变长字段长度列表」里的内容是「 03 01」，而不是 「01 03」。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E5%8F%98%E9%95%BF%E5%AD%97%E6%AE%B5%E9%95%BF%E5%BA%A6%E5%88%97%E8%A1%A81.png)

同样的道理，我们也可以得出**第二条记录**的行格式中，「变长字段长度列表」里的内容是「 04 02」，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E5%8F%98%E9%95%BF%E5%AD%97%E6%AE%B5%E9%95%BF%E5%BA%A6%E5%88%97%E8%A1%A82.png)

**第三条记录**中 phone 列的值是 NULL，**NULL 是不会存放在行格式中记录的真实数据部分里的**，所以「变长字段长度列表」里不需要保存值为 NULL 的变长字段的长度。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E5%8F%98%E9%95%BF%E5%AD%97%E6%AE%B5%E9%95%BF%E5%BA%A6%E5%88%97%E8%A1%A83.png)

> 为什么「变长字段长度列表」的信息要按照逆序存放？

这个设计是有想法的，主要是因为「记录头信息」中指向下一个记录的指针，指向的是下一条记录的「记录头信息」和「真实数据」之间的位置，这样的好处是向左读就是记录头信息，向右读就是真实数据，比较方便。

<font color=24b3aa>「变长字段长度列表」中的信息之所以要 **逆序存放**，是因为这样可以**使得位置靠前的记录的真实数据和数据对应的字段长度信息可以同时在一个 CPU Cache Line 中，这样就可以提高 CPU Cache 的命中率**。</font>

<font color=24b3aa>同样的道理， NULL 值列表的信息也需要逆序存放。</font>

如果你不知道什么是 CPU Cache，可以看[这篇文章 (opens new window)](https://xiaolincoding.com/os/1_hardware/how_to_make_cpu_run_faster.html)，这属于计算机组成的知识。

> 每个数据库表的行格式都有「变长字段字节数列表」吗？

其实变长字段字节数列表不是必须的。

**当数据表没有变长字段的时候，比如全部都是 int 类型的字段，这时候表里的行格式就不会有「变长字段长度列表」了**，因为没必要，不如去掉以节省空间。

所以<font color=24b3aa>「变长字段长度列表」只出现在数据表有变长字段的时候。</font>

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_2-null-值列表)2. NULL 值列表

表中的某些列可能会存储 NULL 值，如果把这些 NULL 值都放到记录的真实数据中会比较浪费空间，所以 Compact 行格式把这些值为 NULL 的列存储到 NULL值列表中。

如果存在允许 NULL 值的列，则每个列对应一个二进制位（bit），二进制位按照列的顺序逆序排列。

- 二进制位的值为 1 时，代表该列的值为 NULL。
- 二进制位的值为 0 时，代表该列的值不为 NULL。

另外，NULL 值列表必须用整数个字节的位表示（1 字节 8 位），如果使用的二进制位个数不足整数个字节，则在字节的高位补 0。

还是以 t_user 表的这三条记录作为例子：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/t_test.png)

接下来，我们看看看看这三条记录的行格式中的 NULL 值列表是怎样存储的。

先来看 **第一条记录**，第一条记录所有列都有值，不存在 NULL 值，所以用二进制来表示是酱紫的：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/null%E5%80%BC%E5%88%97%E8%A1%A81.png)

但是 InnoDB 是用整数字节的二进制位来表示 NULL 值列表的，现在不足 8 位，所以要在高位补 0，最终用二进制来表示是酱紫的：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/null%E5%80%BC%E5%88%97%E8%A1%A82.png)

所以，对于第一条数据，NULL 值列表用十六进制表示是 0x00。

接下来看 **第二条记录**，第二条记录 age 列是 NULL 值，所以，对于第二条数据，NULL值列表用十六进制表示是 0x04。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/null%E5%80%BC%E5%88%97%E8%A1%A83.png)

最后 **第三条记录**，第三条记录 phone 列 和 age 列是 NULL 值，所以，对于第三条数据，NULL 值列表用十六进制表示是 0x06。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/null%E5%80%BC%E5%88%97%E8%A1%A84.png)

我们把三条记录的 NULL 值列表都填充完毕后，它们的行格式是这样的：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/null%E5%80%BC%E5%88%97%E8%A1%A85.png)

> 每个数据库表的行格式都有「NULL 值列表」吗？

NULL 值列表也不是必须的。

<font color=24b3aa>**当数据表的字段都定义成 NOT NULL 的时候，这时候表里的行格式就不会有 NULL 值列表了**。</font>

所以在设计数据库表的时候，通常都是建议将字段设置为 NOT NULL，这样可以至少节省 1 字节的空间（NULL 值列表至少占用 1 字节空间）。

#### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#_3-记录头信息)3. 记录头信息

记录头信息中包含的内容很多，我就不一一列举了，这里说几个比较重要的：

- DELETE_mask ：标识此条数据是否被删除。从这里可以知道，<font color=24b3aa>我们执行 detele 删除记录的时候，并不会真正的删除记录，只是将这个记录的 DELETE_mask 标记为 1。</font>
- next_record：下一条记录的位置。从这里可以知道，<font color=24b3aa>记录与记录之间是通过链表组织的。</font>在前面我也提到了，指向的是下一条记录的「记录头信息」和「真实数据」之间的位置，这样的好处是向左读就是记录头信息，向右读就是真实数据，比较方便。
- record_type：表示当前记录的类型，0表示普通记录，1 表示 B+ 树非叶子节点记录，2 表示最小记录，3 表示最大记录

### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#记录的真实数据)记录的真实数据

记录真实数据部分除了我们定义的字段，还有三个隐藏字段，分别为：row_id、trx_id、roll_pointer，我们来看下这三个字段是什么。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E8%AE%B0%E5%BD%95%E7%9A%84%E7%9C%9F%E5%AE%9E%E6%95%B0%E6%8D%AE.png)

- row_id

如果我们建表的时候指定了主键或者唯一约束列，那么就没有 row_id 隐藏字段了。<font color=24b3aa>如果既没有指定主键，又没有唯一约束，那么 InnoDB 就会为记录添加 row_id 隐藏字段。</font>row_id 不是必需的，占用 6 个字节。

- trx_id

事务 id，表示这个数据是由哪个事务生成的。 trx_id 是必需的，占用 6 个字节。

- roll_pointer

这条记录上一个版本的指针。roll_pointer 是必需的，占用 7 个字节。

如果你熟悉 MVCC 机制，你应该就清楚 trx_id 和 roll_pointer 的作用了，如果你还不知道 MVCC 机制，可以看完[这篇文章 (opens new window)](https://xiaolincoding.com/mysql/transaction/mvcc.html)，一定要掌握，面试也很经常问 MVCC 是怎么实现的。

## [#](https://www.xiaolincoding.com/mysql/base/row_format.html#varchar-n-中-n-最大取值为多少)varchar(n) 中 n 最大取值为多少？

我们要清楚一点，**<font color=24b3aa>MySQL 规定除了 TEXT、BLOBs 这种大对象类型之外，其他所有的列（不包括隐藏列和记录头信息）占用的字节长度加起来不能超过 65535 个字节</font>**。

也就是说，一行记录除了 TEXT、BLOBs 类型的列，限制最大为 65535 字节，注意是一行的总长度，不是一列。

知道了这个前提之后，我们再来看看这个问题：「varchar(n) 中 n 最大取值为多少？」

**<font color=24b3aa>varchar(n) 字段类型的 n 代表的是最多存储的字符数量，并不是字节大小哦。</font>**

要算 varchar(n) 最大能允许存储的字节数，还要看数据库表的字符集，因为字符集代表着，1个字符要占用多少字节，比如 ascii 字符集， 1 个字符占用 1 字节，那么 varchar(100) 意味着最大能允许存储 100 字节的数据。

### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#单字段的情况)单字段的情况

前面我们知道了，一行记录最大只能存储 65535 字节的数据。

那假设数据库表只有一个 varchar(n) 类型的列且字符集是 ascii，在这种情况下， varchar(n) 中 n 最大取值是 65535 吗？

不着急说结论，我们先来做个实验验证一下。

我们定义一个 varchar(65535) 类型的字段，字符集为 ascii 的数据库表。

```sql
CREATE TABLE test ( 
`name` VARCHAR(65535)  NULL
) ENGINE = InnoDB DEFAULT CHARACTER SET = ascii ROW_FORMAT = COMPACT;
```

看能不能成功创建一张表：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/error.png)

可以看到，创建失败了。

从报错信息就可以知道**一行数据的最大字节数是 65535（不包含 TEXT、BLOBs 这种大对象类型），其中包含了 storage overhead**。

问题来了，这个 storage overhead 是什么呢？其实就是「变长字段长度列表」和 「NULL 值列表」，也就是说**<font color=24b3aa>一行数据的最大字节数 65535，其实是包含「变长字段长度列表」和 「NULL 值列表」所占用的字节数的</font>**。所以， 我们在算 varchar(n) 中 n 最大值时，需要减去 storage overhead 占用的字节数。

这是因为我们存储字段类型为 varchar(n) 的数据时，其实分成了三个部分来存储：

- 真实数据
- 真实数据占用的字节数
- NULL 标识，如果不允许为 NULL，这部分不需要

> 本次案例中，「NULL 值列表」所占用的字节数是多少？

前面我创建表的时候，字段是允许为 NULL 的，所以**会用 1 字节来表示「NULL 值列表」**。

> 本次案例中，「变长字段长度列表」所占用的字节数是多少？

「变长字段长度列表」所占用的字节数 = 所有「变长字段长度」占用的字节数之和。

所以，我们要先知道每个变长字段的「变长字段长度」需要用多少字节表示？具体情况分为：

- 条件一：如果变长字段允许存储的最大字节数小于等于 255（$2^8-1$） 字节，就会用 1 字节表示「变长字段长度」；
- 条件二：如果变长字段允许存储的最大字节数大于 255 字节，就会用 2 字节表示「变长字段长度」；

我们这里字段类型是 varchar(65535) ，字符集是 ascii，所以代表着变长字段允许存储的最大字节数是 65535，符合条件二，所以会用 2 字节来表示「变长字段长度」。

**因为我们这个案例是只有 1 个变长字段，所以「变长字段长度列表」= 1 个「变长字段长度」占用的字节数，也就是 2 字节**。

因为我们在算 varchar(n) 中 n 最大值时，需要减去 「变长字段长度列表」和 「NULL 值列表」所占用的字节数的。所以，**在数据库表只有一个 varchar(n) 字段且字符集是 ascii 的情况下，varchar(n) 中 n 最大值 = 65535 - 2 - 1 = 65532**。

我们先来测试看看 varchar(65533) 是否可行？

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/65533.png)

可以看到，还是不行，接下来看看 varchar(65532) 是否可行？

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/65532.png)

可以看到，创建成功了。说明我们的推论是正确的，在算 varchar(n) 中 n 最大值时，需要减去 「变长字段长度列表」和 「NULL 值列表」所占用的字节数的。

当然，我上面这个例子是针对字符集为 ascii 情况，如果采用的是 UTF-8，varchar(n) 最多能存储的数据计算方式就不一样了：

- 在 UTF-8 字符集下，一个字符最多需要三个字节，varchar(n) 的 n 最大取值就是 65532/3 = 21844。

上面所说的只是针对于一个字段的计算方式。

### [#](https://www.xiaolincoding.com/mysql/base/row_format.html#多字段的情况)多字段的情况

**如果有多个字段的话，要保证所有字段的长度 + 变长字段字节数列表所占用的字节数 + NULL值列表所占用的字节数 <= 65535**。

## [#](https://www.xiaolincoding.com/mysql/base/row_format.html#行溢出后-mysql-是怎么处理的)行溢出后，MySQL 是怎么处理的？

<font color=24b3aa>MySQL 中磁盘和内存交互的基本单位是 **页**，一个页的大小一般是 16KB，也就是 16384B，而一个 varchar(n) 类型的列最多可以存储 65532B，一些大对象如 TEXT、BLOB 可能存储更多的数据，这时一个页可能就存不了一条记录。这个时候就会 **发生行溢出，多的数据就会存到另外的「溢出页」中**。</font>

如果一个数据页存不了一条记录，InnoDB 存储引擎会自动将溢出的数据存放到「溢出页」中。在一般情况下，InnoDB 的数据都是存放在 「数据页」中。但是当发生行溢出时，溢出的数据会存放到「溢出页」中。

<font color=24b3aa>当发生行溢出时，在记录的真实数据处只会保存该列的一部分数据，而把剩余的数据放在「溢出页」中，然后 **真实数据处用 20 字节存储指向溢出页的地址，从而可以找到剩余数据所在的页。**</font>大致如下图所示。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E8%A1%8C%E6%BA%A2%E5%87%BA.png)

上面这个是 Compact 行格式在发生行溢出后的处理。

Compressed 和 Dynamic 这两个行格式和 Compact 非常类似，主要的区别在于处理行溢出数据时有些区别。

这两种格式采用完全的行溢出方式，记录的真实数据处不会存储该列的一部分数据，只存储 20 个字节的指针来指向溢出页。而实际的数据都存储在溢出页中，看起来就像下面这样：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/row_format/%E8%A1%8C%E6%BA%A2%E5%87%BA2.png)

## [#](https://www.xiaolincoding.com/mysql/base/row_format.html#总结)总结

> MySQL 的 NULL 值是怎么存放的？

MySQL 的 Compact 行格式中会用「NULL 值列表」来标记值为 NULL 的列，NULL 值并不会存储在行格式中的真实数据部分。

NULL 值列表会占用 1 字节空间，当表中所有字段都定义成 NOT NULL，行格式中就不会有 NULL 值列表，这样可节省 1 字节的空间。

> MySQL 怎么知道 varchar(n) 实际占用数据的大小？

MySQL 的 Compact 行格式中会用「变长字段长度列表」存储变长字段实际占用的数据大小。

> varchar(n) 中 n 最大取值为多少？

一行记录最大能存储 65535 字节的数据，但是这个是包含「变长字段字节数列表所占用的字节数」和「NULL值列表所占用的字节数」。所以， 我们在算 varchar(n) 中 n 最大值时，需要减去这两个列表所占用的字节数。

如果一张表只有一个 varchar(n) 字段，且允许为 NULL，字符集为 ascii。varchar(n) 中 n 最大取值为 65532。

计算公式：65535 - 变长字段字节数列表所占用的字节数 - NULL值列表所占用的字节数 = 65535 - 2 - 1 = 65532。

如果有多个字段的话，要保证所有字段的长度 + 变长字段字节数列表所占用的字节数 + NULL值列表所占用的字节数 <= 65535。

> 行溢出后，MySQL 是怎么处理的？

如果一个数据页存不了一条记录，InnoDB 存储引擎会自动将溢出的数据存放到「溢出页」中。

Compact 行格式针对行溢出的处理是这样的：当发生行溢出时，在记录的真实数据处只会保存该列的一部分数据，而把剩余的数据放在「溢出页」中，然后真实数据处用 20 字节存储指向溢出页的地址，从而可以找到剩余数据所在的页。

Compressed 和 Dynamic 这两种格式采用完全的行溢出方式，记录的真实数据处不会存储该列的一部分数据，只存储 20 个字节的指针来指向溢出页。而实际的数据都存储在溢出页中。

参考资料：

- 《MySQL 是怎样运行的》
- 《MySQL技术内幕 InnoDB存储引擎》

- 
- **数据库管理员 (Database Administrator - DBA ):** 他就是图书馆的馆长，负责整个数据库系统正常运行。他的职责非常广泛，包括数据库的设计、安装、监控、性能调优、备份与恢复、安全管理等等，确保整个系统的稳定、高效和安全。

DB 和 DBMS 我们通常会搞混，这里再简单提一下：**通常我们说“用 MySQL 数据库”，其实是用 MySQL（DBMS）来管理一个或多个数据库（DB）。**


# [存储引擎]说说多线程情境下 B+ 树插入一个数的过程

多线程下往 B+ 树里插数据，是个经典的并发难题。难点不在于“插”本身，而是因为一旦节点满了，会引发**节点分裂（Split）**，并一直向上波及父节点甚至重构根节点。如果不加控制，其他并发读写的线程会顺着旧指针读到错乱的脏数据。

为了避免用全局锁卡死整个系统，工业界通常采用以下 3 种精妙的细粒度并发策略：

**1. 经典走法：螃蟹步加锁（Crabbing / Lock Coupling）** 这是最基础且常用的解法，画面感就像螃蟹下楼梯。

- 线程从根节点往下搜索，先对父节点加锁，再对子节点加锁。
- **核心动作**：判断子节点是否“安全”（即有没有满）。如果子节点空间足够，意味着接下来无论怎么插都不会引发分裂，此时就可以**果断释放父节点的锁**，然后继续向下。
- 这样层层递进，直到叶子节点完成插入。它确保了只有在真正发生连环分裂时，才会锁住一长串路径。

**2. 数据库实战：B-link 树的右指针** PostgreSQL 等主流数据库倾向于用这种变体来进一步降低锁的开销。

- 它的核心思想是：哪怕节点正在分裂，也不让其他读线程阻塞等待。
- 发生分裂时，系统会在原节点和新切出来的节点之间，临时拉一条**右兄弟指针（Right-link）**。如果别的线程按照旧状态找过来，发现节点满了或者找不到目标值，它不会报错，而是顺着右指针去新节点里捞数据。

**3. 现代极致性能：乐观读写（Optimistic Lock Coupling）** 现代内存数据库（比如追求极限速度的场景）连“螃蟹步”频繁加解锁的系统调用开销都嫌弃。

- **无锁下探**：每个节点维护一个版本号（Version）。往下搜索时完全不加锁，只是默默把沿途节点的版本号记下来。
- **触底反弹**：找到叶子节点后，一把挂上写锁，然后**回头核对**刚才记录的整条链路的版本号。
- 如果版本号全对上了，说明刚才下楼的过程中没人动过树的结构，直接写数据收工；如果对不上（比如别人刚好触发了分裂），就解开锁从头重试。因为实际业务中“触发分裂”是极低概率事件，这种“赌一把”的乐观思路整体性能极高。

# [存储引擎]什么是 MySQL 中的执行计划

MySQL 中的执行计划（Execution Plan），简单来说就是 **MySQL 在真正执行你的 SQL 语句之前，内部生成的一份“导航路线图”**。

在去硬盘里捞数据之前，MySQL 的优化器（Optimizer）会先在脑子里盘算一下：怎么查最快？是把整张表从头到尾翻一遍（全表扫描），还是顺着目录直接找（走索引）？它经过评估选定的最终方案，就是执行计划。

理解和使用执行计划，你只需要掌握以下 3 个核心点：

**1. 怎么看执行计划？** 操作极其简单，只需要在你的普通 SQL 查询语句最前面，加上 `EXPLAIN` 关键字即可。 例如：`EXPLAIN SELECT * FROM user WHERE name = '张三';` 执行后，MySQL 不会给你返回真实的张三的数据，而是返回一行或多行表格，里面写满了它打算怎么执行这条 SQL 的细节。

**2. 核心作用：慢查询的“照妖镜”** 当你的接口变卡、SQL 跑得很慢时，看执行计划是排查问题的第一步，也是最重要的一步。它能直接告诉你：**你的索引到底有没有生效。** 很多时候你以为建了索引就能快，但因为 SQL 写法问题导致索引失效，执行计划能一眼把它“照”出来。

**3. 实战中死磕这 4 个核心字段** 执行计划会返回十几个字段，但日常排查问题，你只需要盯紧这 4 个：

- **`type`（访问类型）：最核心的指标！** 它代表了 MySQL 找数据的方式。性能从烂到好常见的有：
  - `ALL`：全表扫描（最拉垮，数据量大时必须优化）。
  - `index`：全索引树扫描（比 ALL 稍微好一点点）。
  - `range`：索引范围扫描（比如用了 `BETWEEN`、`>`、`<`）。
  - `ref`：非唯一性索引的等值查询（比较理想的状态）。
  - `const` / `eq_ref`：主键或唯一索引查询（一步到位，速度最快）。
- **`key`（实际使用的索引）**：它展示了 MySQL 最终决定用哪个索引。如果是 `NULL`，说明没走任何索引，这是个极其危险的信号。
- **`rows`（预估扫描行数）**：MySQL **估算**它大概要翻看多少行数据才能找到结果。这个数值**越小越好**。
- **`Extra`（附加信息）**：包含了很多关键的动作提示。
  - 看到 `Using index`：恭喜，触发了**覆盖索引**，性能极佳。
  - 看到 `Using filesort`（文件排序）或 `Using temporary`（临时表）：警告信号，说明 MySQL 在内存或磁盘里做了额外的耗时动作，强烈建议优化。

# [存储引擎]一条 `update` 语句到达 MySQL 之后，内部是怎么执行的

**<font color=red>1. 客户端把更新 SQL 发给 MySQL Server</font>**

比如：

```sql
update account 
set balance = balance - 100 
where id = 1;
```

MySQL Server 先做这些事：

- 建立连接，校验权限
- 解析 SQL，生成语法树
- 优化器选择执行计划
- 调用存储引擎接口，真正进入 InnoDB 执行更新

MySQL 的执行层负责 SQL 解析、优化和调用存储引擎；像 InnoDB 这样的存储引擎负责事务、锁、redo/undo 等底层数据修改。

**<font color=red>2. 先定位要更新的记录</font>**

InnoDB 会根据条件走索引找数据：

- 如果 `where` 条件命中索引，就走索引查找
- 如果没命中合适索引，可能扫描更多记录
- 找到目标行后，检查是否需要加锁

在 InnoDB 里，不同隔离级别使用不同的锁策略；默认隔离级别是 **REPEATABLE READ**。

**<font color=red>3. 对目标记录加锁</font>**

更新是写操作，所以会对命中的记录加 **排他锁（X 锁）**。如果是范围更新，还可能涉及 **间隙锁 / Next-Key Lock**，这是为了防止并发事务带来幻读问题。InnoDB 在不同隔离级别下会采用不同加锁策略。

你可以简单理解成：

- **先锁住要改的行**
- 别的事务不能同时改这行
- 某些场景下，连这行附近的索引区间也要保护起来

**<font color=red>4. 先写 Undo Log</font>**

真正改数据前，InnoDB 会先记录一份 **undo log**。

作用有两个：

- 事务回滚时，可以把数据恢复回去
- MVCC 读旧版本数据时，也会用到 undo 信息

也就是说，更新不是直接覆盖完就算了，而是先保留“修改前”的版本，给回滚和一致性读留后路。MySQL 官方文档也说明 undo log 可以单独存放在 undo tablespace 中。

**<font color=red>5. 修改 Buffer Pool 中的数据页</font>**

InnoDB 真正改的通常不是磁盘上的页，而是先改 **内存中的数据页**。

也就是：

- 先把目标页读到 Buffer Pool
- 在内存页上完成更新
- 这个页变成 **脏页（dirty page）**

所以一条 `update` 执行成功，不代表数据页立刻写盘。后续会由后台线程再把脏页刷新到磁盘。MySQL 官方文档在 InnoDB 磁盘 I/O 章节也区分了数据文件、doublewrite 文件、redo log 等不同 I/O 路径。[MySQL开发者专区](https://dev.mysql.com/doc/refman/8.3/en/optimizing-innodb-diskio.html?utm_source=chatgpt.com)

**<font color=red>6. 写 Redo Log</font>**

为了防止“内存改了但还没刷盘就宕机”，InnoDB 会记录 **redo log**。

你可以把 redo log 理解成：

- 记录“这个页做了什么修改”
- 即使数据页还没真正写回磁盘
- 宕机恢复时也可以靠 redo 重放，把已提交的数据补回来

这就是 **WAL（Write-Ahead Logging）** 思想：**先写日志，再考虑刷数据页。**

官方文档把 redo log 归为顺序 I/O 的日志文件之一，并且说明 InnoDB 在崩溃恢复时需要扫描 redo log。

**<font color=red>7. 写 Binlog</font>**

如果 MySQL 开启了 binlog，那么执行层还会记录 **binary log**。

它和 redo log 的区别要分清：

- **redo log**：InnoDB 引擎层日志，保证崩溃恢复
- **binlog**：MySQL Server 层日志，主要用于主从复制和按时间点恢复

MySQL 官方文档对 binary log 的定义就是：记录修改数据的语句或行变更，可用于复制和恢复。

**<font color=red>8. 提交时做“两阶段提交”</font>**

这一步是面试高频。

因为同时有：

- InnoDB 的 **redo log**
- Server 层的 **binlog**

为了保证这两个日志一致，MySQL 使用 **两阶段提交（2PC）**。官方文档明确说明，InnoDB 对 XA 两阶段提交的支持用于保证 binary log 和 InnoDB 数据文件同步；在 MySQL 8.4 中，这种支持始终开启。

```
┌─────────────────────────────────────────────────────────┐
│                    事务执行阶段                          │
│  1. Server 层生成 SQL 的 Binlog 事件（暂存线程缓存）      │
│  2. InnoDB 执行更新，修改 Buffer Pool，生成 Redo Log     │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Prepare 阶段（InnoDB 主导）                  │
│  3. InnoDB 将 Redo Log 写入磁盘，标记为 [prepare]        │
│  4. InnoDB 调用 fsync() 确保 Redo Log 持久化             │
│  5. InnoDB 通知 Server 层："我已 prepare"                 │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Binlog 写入阶段（Server 主导）               │
│  6. Server 将 Binlog 从线程缓存写入 Binlog 文件          │
│  7. Server 调用 fsync() 确保 Binlog 持久化               │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Commit 阶段（InnoDB 主导）                   │
│  8. InnoDB 将 Redo Log 标记从 [prepare] 改为 [commit]    │
│  9. （可选）fsync Redo Log 的 commit 标记                │
│  10. 事务对客户端返回成功                                 │
└─────────────────────────────────────────────────────────┘
```

# [执行引擎]SQL SELECT 语句的执行顺序

**1.FORM**：选择 from 后面跟的表，产生虚拟表 1。

**2.ON**：ON 是 JOIN 的连接条件，符合连接条件的行会被记录在虚拟表 2 中。

**3.JOIN**：如果指定了 LEFT JOIN，那么保留表中未匹配的行就会作为外部行添加到虚拟表 2 中，产生虚拟表 3。如果有多个 JOIN 链接，会重复执行步骤 1~3，直到处理完所有表。

**4.WHER**E：对虚拟表 3 进行 WHERE 条件过滤，符合条件的记录会被插入到虚拟表 4 中。

**5.GROUP BY**：根据 GROUP BY 子句中的列，对虚拟表 2 中的记录进行分组操作，产生虚拟表 5。

**6.HAVING**：对虚拟表5进行 HAVING 过滤，符合条件的记录会被插入到虚拟表6中。

**7.SELECT**：SELECT 到一步才执行，选择指定的列，插入到虚拟表 7 中。

**8.UNION**：UNION 连接的两个 SELECT 查询语句，会重复执行步骤 1~7，产生两个虚拟表7，UNION 会将这些记录合并到虚拟表 8 中。

**9.ORDER BY**: 将虚拟表8中的记录进行排序，虚拟表 9。

**10.LIMIT**：取出指定行的记录，返回结果集。

# [缓存]内存揭开 Buffer Pool 的面纱

![img](https://cdn.xiaolincoding.com//mysql/other/e5a23e5c53ef471b947b5007866229fe.png)

## [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#为什么要有-buffer-pool)为什么要有 Buffer Pool？

虽然说 MySQL 的数据是存储在磁盘里的，但是也不能每次都从磁盘里面读取数据，这样性能是极差的。

要想提升查询性能，加个缓存就行了嘛。所以，当数据从磁盘中取出后，缓存内存中，下次查询同样的数据的时候，直接从内存中读取。

为此，Innodb 存储引擎设计了一个**<font color=blue>缓冲池（Buffer Pool）</font>**，来提高数据库的读写性能。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/%E7%BC%93%E5%86%B2%E6%B1%A0.drawio.png)

有了缓冲池后：

- 当读取数据时，如果数据存在于 Buffer Pool 中，客户端就会直接读取 Buffer Pool 中的数据，否则再去磁盘中读取。
- 当修改数据时，首先是修改 Buffer Pool 中数据所在的页，然后将其页设置为脏页，最后由后台线程将脏页写入到磁盘。

### [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#buffer-pool-有多大)Buffer Pool 有多大？

Buffer Pool 是在 MySQL 启动的时候，向操作系统申请的一片连续的内存空间，<font color=24b3aa>默认配置下 Buffer Pool 只有 128MB。</font>

可以通过调整 `innodb_buffer_pool_size` 参数来设置 Buffer Pool 的大小，一般建议设置成可用物理内存的 60%~80%。

### [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#buffer-pool-缓存什么)Buffer Pool 缓存什么？

<font color=24b3aa>InnoDB 会把存储的数据划分为若干个**「页」**，以页作为磁盘和内存交互的基本单位，一个页的默认大小为 16KB。因此，Buffer Pool 同样需要按「页」来划分。</font>

在 MySQL 启动的时候，**InnoDB 会为 Buffer Pool 申请一片连续的内存空间，然后按照默认的 16KB 的大小划分出一个个的页， Buffer Pool 中的页就叫做 <font color=blue>缓存页</font>**。此时这些缓存页都是空闲的，之后随着程序的运行，才会有磁盘上的页被缓存到 Buffer Pool 中。

<font color=24b3aa>所以，MySQL 刚启动的时候，你会观察到使用的虚拟内存空间很大，而使用到的物理内存空间却很小，</font>这是因为只有这些虚拟内存被访问后，操作系统才会触发缺页中断，接着将虚拟地址和物理地址建立映射关系。

Buffer Pool 除了缓存「索引页」和「数据页」，还包括了 undo 页，插入缓存、自适应哈希索引、锁信息等等。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/bufferpool%E5%86%85%E5%AE%B9.drawio.png)

<font color=24b3aa>为了更好的管理这些在 Buffer Pool 中的缓存页，InnoDB 为每一个缓存页都创建了一个 **控制块**，控制块信息包括「缓存页的表空间、页号、缓存页地址、链表节点」等等。</font>

控制块也是占有内存空间的，它是放在 Buffer Pool 的最前面，接着才是缓存页，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/%E7%BC%93%E5%AD%98%E9%A1%B5.drawio.png)

上图中控制块和缓存页之间灰色部分称为碎片空间。

> 为什么会有碎片空间呢？

你想想啊，每一个控制块都对应一个缓存页，那在分配足够多的控制块和缓存页后，可能剩余的那点儿空间不够一对控制块和缓存页的大小，自然就用不到喽，这个用不到的那点儿内存空间就被称为碎片了。

## [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#如何管理-buffer-pool)如何管理 Buffer Pool？

### [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#如何管理空闲页)如何管理空闲页？

Buffer Pool 是一片连续的内存空间，当 MySQL 运行一段时间后，这片连续的内存空间中的缓存页既有空闲的，也有被使用的。

那当我们从磁盘读取数据的时候，总不能通过遍历这一片连续的内存空间来找到空闲的缓存页吧，这样效率太低了。

所以，为了能够快速找到空闲的缓存页，可以使用链表结构，将空闲缓存页的「控制块」作为链表的节点，这个链表称为 **<font color=blue>Free 链表</font>**（空闲链表）。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/freelist.drawio.png)

Free 链表上除了有控制块，还有一个 <font color=24b3aa>**头节点**</font>，该头节点包含链表的头节点地址，尾节点地址，以及当前链表中节点的数量等信息。

Free 链表节点是一个一个的控制块，而每个控制块包含着对应缓存页的地址，所以相当于 Free 链表节点都对应一个空闲的缓存页。

有了 Free 链表后，每当需要从磁盘中加载一个页到 Buffer Pool 中时，就从 Free链表中取一个空闲的缓存页，并且把该缓存页对应的控制块的信息填上，然后把该缓存页对应的控制块从 Free 链表中移除。

### [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#如何管理脏页)如何管理脏页？

设计 Buffer Pool 除了能提高读性能，还能提高写性能，也就是更新数据的时候，不需要每次都要写入磁盘，而是将 Buffer Pool 对应的缓存页标记为 **脏页**，然后再由后台线程将脏页写入到磁盘。

那为了能快速知道哪些缓存页是脏的，于是就设计出 **<font color=blue>Flush 链表</font>**，它跟 Free 链表类似的，链表的节点也是控制块，区别在于 Flush 链表的元素都是脏页。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/Flush.drawio.png)

有了 Flush 链表后，后台线程就可以遍历 Flush 链表，将脏页写入到磁盘。

### [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#如何提高缓存命中率)如何提高缓存命中率？

Buffer Pool 的大小是有限的，对于一些频繁访问的数据我们希望可以一直留在 Buffer Pool 中，而一些很少访问的数据希望可以在某些时机可以淘汰掉，从而保证 Buffer Pool 不会因为满了而导致无法再缓存新的数据，同时还能保证常用数据留在 Buffer Pool 中。

要实现这个，最容易想到的就是 LRU（Least recently used）算法。

该算法的思路是，链表头部的节点是最近使用的，而链表末尾的节点是最久没被使用的。那么，当空间不够了，就淘汰最久没被使用的节点，从而腾出空间。

简单的 LRU 算法的实现思路是这样的：

- 当访问的页在 Buffer Pool 里，就直接把该页对应的 LRU 链表节点移动到链表的头部。
- 当访问的页不在 Buffer Pool 里，除了要把页放入到 LRU 链表的头部，还要淘汰 LRU 链表末尾的节点。

比如下图，假设 LRU 链表长度为 5，LRU 链表从左到右有 1，2，3，4，5 的页。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lru.png)

如果访问了 3 号的页，因为 3 号页在 Buffer Pool 里，所以把 3 号页移动到头部即可。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lru2.png)

而如果接下来，访问了 8 号页，因为 8 号页不在 Buffer Pool 里，所以需要先淘汰末尾的 5 号页，然后再将 8 号页加入到头部。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lru3.png)

到这里我们可以知道，Buffer Pool 里有三种页和链表来管理数据。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/bufferpoll_page.png)

图中：

- Free Page（空闲页），表示此页未被使用，位于 Free 链表；
- Clean Page（干净页），表示此页已被使用，但是页面未发生修改，位于LRU 链表。
- Dirty Page（脏页），表示此页「已被使用」且「已经被修改」，其数据和磁盘上的数据已经不一致。当脏页上的数据写入磁盘后，内存数据和磁盘数据一致，那么该页就变成了干净页。脏页同时存在于 LRU 链表和 Flush 链表。

<font color=24b3aa>简单的 LRU 算法并没有被 MySQL 使用，因为简单的 LRU 算法无法避免下面这两个问题：</font>

- <font color=24b3aa>预读失效；</font>
- <font color=24b3aa>Buffer Pool 污染；</font>

> 什么是预读失效？

先来说说 MySQL 的预读机制。程序是有空间局部性的，靠近当前被访问数据的数据，在未来很大概率会被访问到。

所以，MySQL 在加载数据页时，会提前把它相邻的数据页一并加载进来，目的是为了减少磁盘 IO。

<font color=blue>但是可能这些 **被提前加载进来的数据页，并没有被访问**，相当于这个预读是白做了，这个就是 **预读失效**。</font>

如果使用简单的 LRU 算法，就会把预读页放到 LRU 链表头部，而当 Buffer Pool空间不够的时候，还需要把末尾的页淘汰掉。

如果这些预读页如果一直不会被访问到，就会出现一个很奇怪的问题，不会被访问的预读页却占用了 LRU 链表前排的位置，而末尾淘汰的页，可能是频繁访问的页，这样就大大降低了缓存命中率。

> 怎么解决预读失效而导致缓存命中率降低的问题？

我们不能因为害怕预读失效，而将预读机制去掉，大部分情况下，局部性原理还是成立的。

要避免预读失效带来影响，最好就是**让预读的页停留在 Buffer Pool 里的时间要尽可能的短，让真正被访问的页才移动到 LRU 链表的头部，从而保证真正被读取的热数据留在 Buffer Pool 里的时间尽可能长**。

那到底怎么才能避免呢？

MySQL 是这样做的，它改进了 LRU 算法，将 LRU 划分了 2 个区域：**<font color=24b3aa>old 区域 和 young 区域</font>**。

young 区域在 LRU 链表的前半部分，old 区域则是在后半部分，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/young%2Bold.png)

old 区域占整个 LRU 链表长度的比例可以通过 `innodb_old_blocks_pct` 参数来设置，默认是 37，代表整个 LRU 链表中 young 区域与 old 区域比例是 63:37。

**<font color=24b3aa>划分这两个区域后，预读的页就只需要加入到 old 区域的头部，当页被真正访问的时候，才将页插入 young 区域的头部</font>**。如果预读的页一直没有被访问，就会从 old 区域移除，这样就不会影响 young 区域中的热点数据。

接下来，给大家举个例子。

假设有一个长度为 10 的 LRU 链表，其中 young 区域占比 70 %，old 区域占比 30 %。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lrutwo.drawio.png)

现在有个编号为 20 的页被预读了，这个页只会被插入到 old 区域头部，而 old 区域末尾的页（10号）会被淘汰掉。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lrutwo2.png)

如果 20 号页一直不会被访问，它也没有占用到 young 区域的位置，而且还会比 young 区域的数据更早被淘汰出去。

如果 20 号页被预读后，立刻被访问了，那么就会将它插入到 young 区域的头部，young 区域末尾的页（7号），会被挤到 old 区域，作为 old 区域的头部，这个过程并不会有页被淘汰。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lrutwo3.png)

虽然通过划分 old 区域 和 young 区域避免了预读失效带来的影响，但是还有个问题无法解决，那就是 Buffer Pool 污染的问题。

> 什么是 Buffer Pool 污染？

<font color=blue>当某一个 SQL 语句 **扫描了大量的数据**时</font>，在 Buffer Pool 空间比较有限的情况下，可能会将 **Buffer Pool 里的所有页都替换出去，导致大量热数据被淘汰了**，等这些热数据又被再次访问的时候，由于缓存未命中，就会产生大量的磁盘 IO，MySQL 性能就会急剧下降，这个过程被称为 **Buffer Pool 污染**。

<font color=ff44ff>注意， Buffer Pool 污染并不只是查询语句查询出了大量的数据才出现的问题，即使查询出来的结果集很小，也会造成 Buffer Pool 污染。</font>

比如，在一个数据量非常大的表，执行了这条语句：

```sql
SELECT * FROM t_user WHERE name like "%xiaolin%";
```

可能这个查询出来的结果就几条记录，但是由于这条语句会发生索引失效，所以这个查询过程是全表扫描的，接着会发生如下的过程：

- 从磁盘读到的页加入到 LRU 链表的 old 区域头部；
- 当从页里读取行记录时，也就是页被访问的时候，就要将该页放到 young 区域头部；
- 接下来拿行记录的 name 字段和字符串 xiaolin 进行模糊匹配，如果符合条件，就加入到结果集里；
- 如此往复，直到扫描完表中的所有记录。

经过这一番折腾，原本 young 区域的热点数据都会被替换掉。

举个例子，假设需要批量扫描：21，22，23，24，25 这五个页，这些页都会被逐一访问（读取页里的记录）。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lruthree.drawio.png)

在批量访问这些数据的时候，会被逐一插入到 young 区域头部。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/innodb/lruthree1.png)

可以看到，原本在 young 区域的热点数据 6 和 7 号页都被淘汰了，这就是 Buffer Pool 污染的问题。

> 怎么解决出现 Buffer Pool 污染而导致缓存命中率下降的问题？

像前面这种全表扫描的查询，很多缓冲页其实只会被访问一次，但是它却只因为被访问了一次而进入到 young 区域，从而导致热点数据被替换了。

LRU 链表中 young 区域就是热点数据，只要我们提高进入到 young 区域的门槛，就能有效地保证 young 区域里的热点数据不会被替换掉。

MySQL 是这样做的，<font color=24b3aa>进入到 young 区域条件增加了一个 **停留在 old 区域的时间判断**。</font>

具体是这样做的，在对某个处在 old 区域的缓存页进行第一次访问时，就在它对应的控制块中记录下来这个访问时间：

- 如果后续的访问时间与第一次访问的时间 **在某个时间间隔内**，那么**该缓存页就不会被从 old 区域移动到 young 区域的头部**；
- 如果后续的访问时间与第一次访问的时间 **不在某个时间间隔内**，那么**该缓存页移动到 young 区域的头部**；

这个间隔时间是由 `innodb_old_blocks_time` 控制的，默认是 1000 ms。

也就说，**只有同时满足「被访问」与「在 old 区域停留时间超过 1 秒」两个条件，才会被插入到 young 区域头部**，这样就解决了 Buffer Pool 污染的问题 。

另外，MySQL 针对 young 区域其实做了一个优化，<font color=24b3aa>为了防止 young 区域节点频繁移动到头部。young 区域前面 1/4 被访问不会移动到链表头部，只有后面的 3/4被访问了才会。</font>

> 划分为 young 和 old 区域
>
> * 新来的数据会进入 old 区域，old 满后不会加入 young 而是在 old 上进行 LRU
> * 只有当再次访问并且访问时间超过某个时间间隔后，才能从 old 区域移动到 young 区域
> * young 区域的元素被访问，只有后面 3/4 的数据才会移动到头部
>
> 核心目的：避免一次性大扫描读入很多数据后，把热点页全挤出去了

### [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#脏页什么时候会被刷入磁盘)脏页什么时候会被刷入磁盘？

引入了 Buffer Pool 后，当修改数据时，首先是修改 Buffer Pool 中数据所在的页，然后将其页设置为脏页，但是磁盘中还是原数据。

因此，脏页需要被刷入磁盘，保证缓存和磁盘数据一致，但是若每次修改数据都刷入磁盘，则性能会很差，因此一般都会在一定时机进行批量刷盘。

<font color=24b3aa>可能大家担心，如果在脏页还没有来得及刷入到磁盘时，MySQL 宕机了，不就丢失数据了吗？</font>

<font color=24b3aa>这个不用担心，InnoDB 的更新操作采用的是 **Write Ahead Log** 策略，即先写日志，再写入磁盘，通过 redo log 日志让 MySQL 拥有了崩溃恢复能力。</font>

<font color=blue>下面几种情况会触发脏页的刷新：</font>

- Buffer Pool 空间不足时，需要将一部分数据页淘汰掉，如果淘汰的是脏页，需要先将脏页同步到磁盘；
- MySQL 正常关闭之前，会把所有的脏页刷入到磁盘；
- 当 redo log 日志满了的情况下，会主动触发脏页刷新到磁盘；
- MySQL 认为空闲时，后台线程会定期将适量的脏页刷入到磁盘；

在我们开启了慢 SQL 监控后，如果你发现**「偶尔」会出现一些用时稍长的 SQL**，这可能是因为脏页在刷新到磁盘时可能会给数据库带来性能开销，导致数据库操作抖动。

如果间断出现这种现象，就需要调大 Buffer Pool 空间或 redo log 日志的大小。

## [#](https://www.xiaolincoding.com/mysql/buffer_pool/buffer_pool.html#总结)总结

Innodb 存储引擎设计了一个**缓冲池（Buffer Pool）**，来提高数据库的读写性能。

Buffer Pool 以页为单位缓冲数据，可以通过 `innodb_buffer_pool_size` 参数调整缓冲池的大小，默认是 128 M。

Innodb 通过三种链表来管理缓页：

- Free List （空闲页链表），管理空闲页；
- Flush List （脏页链表），管理脏页；
- LRU List，管理脏页+干净页，将最近且经常查询的数据缓存在其中，而不常查询的数据就淘汰出去。；

InnoDB 对 LRU 做了一些优化，我们熟悉的 LRU 算法通常是将最近查询的数据放到 LRU 链表的头部，而 InnoDB 做 2 点优化：

- 将 LRU 链表 分为 **young 和 old 两个区域**，加入缓冲池的页，优先插入 old 区域；页被访问时，才进入 young 区域，目的是为了解决预读失效的问题。
- 当**「页被访问」且「 old 区域停留时间超过 `innodb_old_blocks_time` 阈值（默认为1秒）」**时，才会将页插入到 young 区域，否则还是插入到 old 区域，目的是为了解决批量数据访问，大量热数据淘汰的问题。

可以通过调整 `innodb_old_blocks_pct` 参数，设置 young 区域和 old 区域比例。

在开启了慢 SQL 监控后，如果你发现「偶尔」会出现一些用时稍长的 SQL，这可因为脏页在刷新到磁盘时导致数据库性能抖动。如果在很短的时间出现这种现象，就需要调大 Buffer Pool 空间或 redo log 日志的大小。

# [事务]Mysql 是如何开启事务的？

在 MySQL 中，开启事务主要有以下几种方式，具体取决于你使用的存储引擎（必须是 **InnoDB** 等支持事务的引擎）以及操作场景：

### **1. 显式开启事务（最常用）**

使用 `START TRANSACTION` 或 `BEGIN` 语句手动开启一个事务块。

```sql
-- 方式一（推荐，SQL标准兼容性好）
START TRANSACTION;

-- 方式二（MySQL特有别名）
BEGIN;

-- 执行你的 SQL 操作...
UPDATE accounts SET balance = balance - 100 WHERE id = 1;
UPDATE accounts SET balance = balance + 100 WHERE id = 2;

-- 提交或回滚
COMMIT;   -- 提交事务
-- ROLLBACK; -- 或者回滚事务
```

> ⚠️ **注意**：`START TRANSACTION` 和 `BEGIN` 都会**隐式提交**当前正在进行的事务（如果有的话），然后开启新事务。

------

### **2. 自动关闭 autocommit（会话级/全局级）**

MySQL 默认 `autocommit = ON`，即每条 DML 语句自动作为一个独立事务提交。你可以关闭它来让后续语句都处于同一个事务中：

```sql
-- 仅对当前会话生效
SET SESSION autocommit = 0;

-- 之后所有语句都在同一事务中，直到手动 COMMIT/ROLLBACK
UPDATE t SET col = val WHERE id = 1;
UPDATE t SET col = val WHERE id = 2;
COMMIT; -- 必须显式提交

-- 恢复自动提交
SET SESSION autocommit = 1;
```

> 💡 这种方式适合需要连续执行多条语句且都属于同一事务的场景，但**容易忘记 COMMIT 导致长事务**，生产环境需谨慎使用。

------

### **3. 隐式事务（某些语句自动触发）**

以下情况会**自动开启并立即提交**一个事务（本质上仍是 autocommit 行为）：

- 单条 DML 语句（当 `autocommit=ON` 时）
- DDL 语句（如 `CREATE TABLE`, `ALTER TABLE`）—— **DDL 会先隐式提交当前事务，再执行自身，然后再隐式提交**
- 部分管理语句（如 `SET PASSWORD`, `GRANT` 等）

------

### **关键注意事项**

表格

| 要点           | 说明                                                         |
| :------------- | :----------------------------------------------------------- |
| **存储引擎**   | MyISAM **不支持**事务，只有 InnoDB、NDB 等支持               |
| **隔离级别**   | 可通过 `SET TRANSACTION ISOLATION LEVEL ...` 设置（READ UNCOMMITTED / READ COMMITTED / REPEATABLE READ / SERIALIZABLE） |
| **SAVEPOINT**  | 支持保存点：`SAVEPOINT sp1; ROLLBACK TO SAVEPOINT sp1;`      |
| **客户端/API** | 在代码中通常通过连接对象的方法开启，如 Python 的 `conn.begin()`、Java JDBC 的 `conn.setAutoCommit(false)`、Go 的 `db.Begin()` 等 |
| **XA 事务**    | 分布式事务使用 `XA START xid` / `XA END` / `XA PREPARE` / `XA COMMIT` |

# [事务]事务与 MVCC 机制

这是我的钱包，共有 100 万元。

![图片](https://cdn.xiaolincoding.com//mysql/other/bc8fe6de0e9e466671cd7d335c116de3.png)

今天我心情好，我决定给你转账 100 万，最后的结果肯定是我的余额变为 0 元，你的余额多了 100 万元，是不是想到就很开心？

转账这一动作在程序里会涉及到一系列的操作，假设我向你转账 100 万的过程是有下面这几个步骤组成的：

![图片](https://cdn.xiaolincoding.com//mysql/other/40b48e0a7c9a57ae6b78b62c3fad4048.png)

可以看到这个转账的过程涉及到了两次修改数据库的操作。

假设在执行第三步骤之后，服务器忽然掉电了，就会发生一个蛋疼的事情，我的账户扣了 100 万，但是钱并没有到你的账户上，也就是说**这 100 万消失了！**

要解决这个问题，就要保证转账业务里的所有数据库的操作是不可分割的，要么全部执行成功 ，要么全部失败，不允许出现中间状态的数据。

数据库中的「**事务（Transaction）**」就能达到这样的效果。

我们在转账操作前先开启事务，等所有数据库操作执行完成后，才提交事务，对于已经提交的事务来说，该事务对数据库所做的修改将永久生效，如果中途发生发生中断或错误，那么该事务期间对数据库所做的修改将会被回滚到没执行该事务之前的状态。

没错，今天就来图解 MySQL 事务啦，开车！

![img](https://cdn.xiaolincoding.com//mysql/other/eb15d4b6a9d543c1be4f7090479d969c.png)

------

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#事务有哪些特性)事务有哪些特性？

事务是由 MySQL 的引擎来实现的，我们常见的 InnoDB 引擎它是支持事务的。

不过并不是所有的引擎都能支持事务，比如 MySQL 原生的 MyISAM 引擎就不支持事务，也正是这样，所以大多数 MySQL 的引擎都是用 InnoDB。

事务看起来感觉简单，但是要实现事务必须要遵守 4 个特性，分别如下：

- **原子性（Atomicity）**：一个事务中的所有操作，要么全部完成，要么全部不完成，不会结束在中间某个环节，而且事务在执行过程中发生错误，会被回滚到事务开始前的状态，就像这个事务从来没有执行过一样，就好比买一件商品，购买成功时，则给商家付了钱，商品到手；购买失败时，则商品在商家手中，消费者的钱也没花出去。
- **一致性（Consistency）**：<font color=24b3aa>是指事务操作前和操作后，数据满足完整性约束，数据库保持一致性状态。</font>比如，用户 A 和用户 B 在银行分别有 800 元和 600 元，总共 1400 元，用户 A 给用户 B 转账 200 元，分为两个步骤，从 A 的账户扣除 200 元和对 B 的账户增加 200 元。一致性就是要求上述步骤操作后，最后的结果是用户 A 还有 600 元，用户 B 有 800 元，总共 1400 元，而不会出现用户 A 扣除了 200 元，但用户 B 未增加的情况（该情况，用户 A 和 B 均为 600 元，总共 1200 元）。<font color=24b3aa>数据库总是从一个一致性状态到另一个一致性状态。</font>
- **隔离性（Isolation）**：<font color=24b3aa>数据库允许多个并发事务同时对其数据进行读写和修改的能力，隔离性可以防止多个事务并发执行时由于交叉执行而导致数据的不一致</font>，因为多个事务同时使用相同的数据时，不会相互干扰，每个事务都有一个完整的数据空间，对其他并发事务是隔离的。也就是说，消费者购买商品这个事务，是不影响其他消费者购买的。
- **持久性（Durability）**：事务处理结束后，对数据的修改就是永久的，即便系统故障也不会丢失。

<font color=24b3aa>InnoDB 引擎通过什么技术来保证事务的这四个特性的呢？</font>

- <font color=24b3aa>原子性是通过 undo log（回滚日志） 来保证的；</font>
- <font color=24b3aa>一致性则是通过持久性+原子性+隔离性来保证；</font>
- <font color=24b3aa>隔离性是通过 MVCC（多版本并发控制） 或锁机制来保证的；</font>
- <font color=24b3aa>持久性是通过 redo log （重做日志）来保证的；</font>

这次将**重点介绍事务的隔离性**，这也是面试时最常问的知识的点。

为什么事务要有隔离性，我们就要知道并发事务时会引发什么问题。

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#并行事务会引发什么问题)并行事务会引发什么问题？

MySQL 服务端是允许多个客户端连接的，这意味着 MySQL 会出现同时处理多个事务的情况。

那么 **在同时处理多个事务的时候，就可能出现脏读（dirty read）、不可重复读（non-repeatable read）、幻读（phantom read）的问题**。

接下来，通过举例子给大家说明，这些问题是如何发生的。

### [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#脏读)脏读

**如果一个事务「读到」了另一个「未提交事务修改过的数据」，就意味着发生了「脏读」现象。**

举个栗子。

假设有 A 和 B 这两个事务同时在处理，事务 A 先开始从数据库中读取小林的余额数据，然后再执行更新操作，如果此时事务 A 还没有提交事务，而此时正好事务 B 也从数据库中读取小林的余额数据，那么事务 B 读取到的余额数据是刚才事务 A 更新后的数据，即使没有提交事务。

![图片](https://cdn.xiaolincoding.com//mysql/other/10b513008ea35ee880c592a88adcb12f.png)

因为事务 A 是还没提交事务的，也就是它随时可能发生回滚操作，**如果在上面这种情况事务 A 发生了回滚，那么事务 B 刚才得到的数据就是过期的数据，这种现象就被称为脏读。**

### [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#不可重复读)不可重复读

**在一个事务内多次读取同一个数据，如果出现前后两次读到的数据不一样的情况，就意味着发生了「不可重复读」现象。**

举个栗子。

假设有 A 和 B 这两个事务同时在处理，事务 A 先开始从数据库中读取小林的余额数据，然后继续执行代码逻辑处理，**在这过程中如果事务 B 更新了这条数据，并提交了事务，那么当事务 A 再次读取该数据时，就会发现前后两次读到的数据是不一致的，这种现象就被称为不可重复读。**

![图片](https://cdn.xiaolincoding.com//mysql/other/f5b4f8f0c0adcf044b34c1f300a95abf.png)

### [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#幻读)幻读

**在一个事务内多次查询某个符合查询条件的「记录数量」，如果出现前后两次查询到的记录数量不一样的情况，就意味着发生了「幻读」现象。**

举个栗子。

假设有 A 和 B 这两个事务同时在处理，事务 A 先开始从数据库查询账户余额大于 100 万的记录，发现共有 5 条，然后事务 B 也按相同的搜索条件也是查询出了 5 条记录。

![图片](https://cdn.xiaolincoding.com//mysql/other/d19a1019dc35dfe8cfe7fbff8cd97e31.png)

接下来，事务 A 插入了一条余额超过 100 万的账号，并提交了事务，此时数据库超过 100 万余额的账号个数就变为 6。

然后事务 B 再次查询账户余额大于 100 万的记录，此时查询到的记录数量有 6 条，**发现和前一次读到的记录数量不一样了，就感觉发生了幻觉一样，这种现象就被称为幻读。**

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#事务的隔离级别有哪些)事务的隔离级别有哪些？

前面我们提到，当多个事务并发执行时可能会遇到「脏读、不可重复读、幻读」的现象，这些现象会对事务的一致性产生不同程序的影响。

- 脏读：读到其他事务未提交的数据；
- 不可重复读：前后读取的数据不一致；
- 幻读：前后读取的记录数量不一致。

这三个现象的严重性排序如下：

![图片](https://cdn.xiaolincoding.com//mysql/other/d37bfa1678eb71ae7e33dc8f211d1ec1.png)

SQL 标准提出了四种隔离级别来规避这些现象，隔离级别越高，性能效率就越低，这四个隔离级别如下：

- **读未提交（read uncommitted）**，指一个事务还没提交时，它做的变更就能被其他事务看到；
- **读提交（read committed）**，指一个事务提交之后，它做的变更才能被其他事务看到；
- **可重复读（repeatable read）**，<font color=blue>指一个事务执行过程中看到的数据，一直跟这个事务启动时看到的数据是一致的，</font>**<font color=red>MySQL InnoDB 引擎的默认隔离级别</font>**；
- **串行化（serializable ）**；会对记录加上读写锁，在多个事务对这条记录进行读写操作时，如果发生了读写冲突的时候，后访问的事务必须等前一个事务执行完成，才能继续执行；

按隔离水平高低排序如下：

![图片](https://cdn.xiaolincoding.com//mysql/other/cce766a69dea725cd8f19b90db2d0430.png)

针对不同的隔离级别，并发事务时可能发生的现象也会不同。

![图片](https://cdn.xiaolincoding.com//mysql/other/4e98ea2e60923b969790898565b4d643.png)

所以，要解决脏读现象，就要升级到「读提交」以上的隔离级别；要解决不可重复读现象，就要升级到「可重复读」的隔离级别，要解决幻读现象不建议将隔离级别升级到「串行化」。

不同的数据库厂商对 SQL 标准中规定的 4 种隔离级别的支持不一样，有的数据库只实现了其中几种隔离级别，**我们讨论的 MySQL 虽然支持 4 种隔离级别，但是与 SQL 标准中规定的各级隔离级别允许发生的现象却有些出入**。

<font color=24b3aa>MySQL 在「可重复读」隔离级别下，可以很大程度上避免幻读现象的发生（注意是很大程度避免，并不是彻底避免😓），所以 MySQL 并不会使用「串行化」隔离级别来避免幻读现象的发生，因为使用「串行化」隔离级别会影响性能。</font>

**MySQL InnoDB 引擎的默认隔离级别虽然是「可重复读」，但是它很大程度上避免幻读现象（并不是完全解决了，详见这篇[文章 (opens new window)](https://xiaolincoding.com/mysql/transaction/phantom.html)）**，解决的方案有两种：

- 针对 <font color=blue>**快照读**（普通 SELECT 语句）</font>，是 **通过 <font color=blue>MVCC</font> 方式解决了幻读**，因为可重复读隔离级别下，事务执行过程中看到的数据，一直跟这个事务启动时看到的数据是一致的，即使中途有其他事务插入了一条数据，是查询不出来这条数据的，所以就很好了避免幻读问题。</font>
- 针对 <font color=blue>**当前读**（SELECT ... FOR UPDATE 等语句）</font>，是**通过 <font color=blue>next-key lock（记录锁+间隙锁）</font>方式解决了幻读**，因为当执行 SELECT ... FOR UPDATE 语句的时候，会加上 next-key lock，如果有其他事务在 next-key lock 锁范围内插入了一条记录，那么这个插入语句就会被阻塞，无法成功插入，所以就很好了避免幻读问题。

> ## 快照读（Snapshot Read）
>
> **读的是某个时间点的“一致性视图”数据**，不会被正在进行的更新阻塞（通常也不去加锁），看起来像“读历史版本”。
>
> - **实现**：MVCC（多版本并发控制），读取 undo log 里的历史版本来拼出一致性结果
>
> - **特点**：
>
>   - 不加锁（一般情况下）
>   - <font color=24b3aa>可能读到“旧值”（相对当前最新提交）</font>
>   - 更适合高并发查询
>
> - **常见场景**：普通 `SELECT`（不带锁的那种）
>
>   ```sql
>   SELECT * FROM t WHERE id = 1;
>   ```
>
> > 在 InnoDB 的可重复读（RR）下，同一个事务里多次快照读，看到的可能一直是同一个版本（除非用特殊语句）。
>
> ## 当前读（Current Read）
>
> **读的是“最新已提交/最新可见”的记录，并且通常会加锁**，为了后续更新/删除/一致性判断。
>
> - **实现**：直接读当前记录（可能需要等待别的事务释放锁），并对读到的记录加锁（行锁/间隙锁等，取决于条件和隔离级别）
>
> - **特点**：
>
>   - <font color=24b3aa>读最新版本</font>
>   - 通常加锁、可能阻塞
>   - 常用于“读后要改”的场景，避免并发下的数据错乱
>
> - **常见场景**：
>
>   - `SELECT ... FOR UPDATE`
>   - `SELECT ... LOCK IN SHARE MODE`（MySQL 8.0 也可用 `FOR SHARE`）
>   - `UPDATE/DELETE/INSERT`（它们内部也会做当前读来定位并加锁）
>
>   ```sql
>   SELECT * FROM t WHERE id = 1 FOR UPDATE;
>   UPDATE t SET c = c + 1 WHERE id = 1;
>   ```

这四种隔离级别具体是如何实现的呢？

- 对于「读未提交」隔离级别的事务来说，因为可以读到未提交事务修改的数据，所以直接读取最新的数据就好了；
- 对于「串行化」隔离级别的事务来说，通过加读写锁的方式来避免并行访问；
- <font color=24b3aa>对于「读提交」和「可重复读」隔离级别的事务来说，它们是通过  **Read View** 来实现的，它们的区别在于创建 Read View 的时机不同，大家可以把 Read View 理解成一个数据快照，就像相机拍照那样，定格某一时刻的风景。</font><font color=ee11ff>「读提交」隔离级别是在「每个语句执行前」都会重新生成一个 Read View，而「可重复读」隔离级别是「启动事务时」生成一个 Read View，然后整个事务期间都在用这个 Read View。</font>

<font color=24b3aa>注意，执行「开始事务」命令，并不意味着启动了事务。</font>在 MySQL 有两种开启事务的命令，分别是：

- 第一种：begin/start transaction 命令；
- 第二种：start transaction with consistent snapshot 命令；

这两种开启事务的命令，事务的启动时机是不同的：

- 执行了 begin/start transaction 命令后，并不代表事务启动了。只有在执行这个命令后，执行了第一条 SELECT 语句，才是事务真正启动的时机；
- 执行了 start transaction with consistent snapshot 命令，就会马上启动事务。

> 这段话想表达的核心是：**MySQL 的“事务开始”在语义层面和 InnoDB 内部真正“创建事务/生成一致性视图（Read View）”并不是同一件事**。所以看起来像是“执行开始事务命令，并不意味着（立刻）启动了事务”。
>
> 更准确地拆开讲是这样：
>
> ## 1）`BEGIN` / `START TRANSACTION` 到底做了什么？
>
> 它做的是**开启一个“显式事务”的边界**（把后续语句归到同一个事务里），并且在 `autocommit=1` 时会让你进入“需要 COMMIT/ROLLBACK 才结束”的模式。
>
> <font color=24b3aa>但在 InnoDB 里，为了减少开销，**事务对象（trx）、事务 ID、以及一致性视图 Read View 可能会延迟到“第一条真正需要它的语句”才创建**：</font>
>
> - 如果你后面一直没执行任何会访问 InnoDB 表的语句（或只是一些不涉及事务的操作），那 InnoDB 可能根本不需要分配 trx。
> - 即便你马上执行 `SELECT`，在 **RR（可重复读）** 下，**Read View 通常也是在第一次“快照读”时才创建**，不是在 `BEGIN` 的瞬间就创建。
>
> <font color=24b3aa>所以很多资料说的“并不意味着启动事务”，更接近于： **并不意味着 InnoDB 立刻生成事务 ID / 立刻生成一致性快照**。</font>
>
> > <font color=24b3aa>语义上你已经进入显式事务了；只是内部关键结构可能懒创建。</font>
>
> ------
>
> ## 2）`START TRANSACTION WITH CONSISTENT SNAPSHOT` 有什么不同？
>
> 这条命令的区别是：**强制在事务开始时就创建一致性视图（Read View）**。
>
> - 主要在 **RR（可重复读）** 下有意义：它保证你事务里的快照从“事务开始那一刻”就固定下来。
> - 目的：避免“你事务开始后很久才第一次 SELECT，快照却取的是那次 SELECT 时刻”的情况。
>
> 换句话说：
>
> - `BEGIN/START TRANSACTION`：**事务边界先立起来，Read View 可能等第一次快照读再生成**
> - `... WITH CONSISTENT SNAPSHOT`：**事务一开始就把 Read View 固定住**
>
> ------
>
> ## 3）一个直观例子（RR 下）
>
> 假设会话 A：
>
> ```sql
> START TRANSACTION;   -- 还不一定立刻生成 Read View
> -- 停顿一会儿...
> SELECT * FROM t WHERE id=1;  -- 第一次快照读，此刻才生成 Read View（常见情况）
> ```
>
> 如果你想让快照以事务开始那一刻为准：
>
> ```sql
> START TRANSACTION WITH CONSISTENT SNAPSHOT; -- 立刻生成 Read View
> -- 停顿一会儿...
> SELECT * FROM t WHERE id=1;  -- 用的是刚才开始事务时的快照
> ```
>
> 这显然是合理的，因为我们肯定希望事务看到的数据和第一条事务语句的执行时间对应而不是和 `START TRANSACTION` 的执行时间对应。

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#read-view-在-mvcc-里如何工作的)Read View 在 MVCC 里如何工作的？

我们需要了解两个知识：

- <font color=blue>Read View 中四个字段作用；</font>
- <font color=blue>聚簇索引记录中两个跟事务有关的隐藏列；</font>

那 Read View 到底是个什么东西？

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/%E4%BA%8B%E5%8A%A1%E9%9A%94%E7%A6%BB/readview%E7%BB%93%E6%9E%84.drawio.png)

Read View 有四个重要的字段：

- <font color=blue>creator_trx_id ：</font>指的是 **创建该 Read View 的事务的事务 id**。
- <font color=blue>m_ids ：</font>指的是在创建 Read View 时，当前数据库中「活跃事务」的**事务 id 列表**，注意是一个 **<font color=ff00ff>列表</font>**，**“活跃事务”指的就是，启动了但还没提交的事务**。<font color=24b3aa>**`ids`** 就是 **IDs 的复数**，全称是 **Identifiers**，意思是“标识符/编号”。</font>
- <font color=blue>min_trx_id ：</font>指的是在创建 Read View 时，当前数据库中「活跃事务」中事务 **id 最小的事务**，也就是 m_ids 的最小值（事务 ID 小于该值的事务都是已提交的）。
- <font color=blue>max_trx_id ：</font>这个并不是 m_ids 的最大值，而是**创建 Read View 时当前数据库中应该给下一个事务的 id 值**，也就是<font color=24b3aa>全局事务中最大的事务 id 值 + 1；</font>

>  `m_ids` 只记录“当时还活着(未提交)的事务”，**不会包含已提交的事务**；但 `max_trx_id`（`up_limit_id`） 需要把“当时已经出现过的 trx_id 范围上界”固定住，用来快速判定“更晚开启的事务（未来事务）一定不可见”。
>
>  也就是说：
>
>  * 凡是 `< min_trx_id` 的事务都是已提交的，可见的
>  * 凡是 `>= max_trx_id` 的事务都是不可见的
>  * `<= min_trx_id && < max_trx_id` 的事务不一定提交，也就不一定可见， 因此需要通过 `m_ids` 判断

知道了 Read View 的字段，我们还需要了解聚簇索引记录中的两个隐藏列。

假设在账户余额表插入一条小林余额为 100 万的记录，然后我把这两个隐藏列也画出来，该记录的整个示意图如下：

![图片](https://cdn.xiaolincoding.com//mysql/other/f595d13450878acd04affa82731f76c5.png)

对于使用 InnoDB 存储引擎的数据库表，它的聚簇索引记录中都包含下面两个隐藏列：

- <font color=blue>trx_id</font>，当一个事务对某条聚簇索引记录进行改动时，就会**把该事务的事务 id 记录在 trx_id 隐藏列里**；
- <font color=blue>roll_pointer</font>，每次对某条聚簇索引记录进行改动时，都会把旧版本的记录写入到 undo 日志中，然后**这个隐藏列是个指针，<font color=24b3aa>指向每一个旧版本记录</font>**，于是就可以通过它找到修改前的记录。 

在创建 Read View 后，我们可以将记录中的 trx_id 划分这三种情况：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/%E4%BA%8B%E5%8A%A1%E9%9A%94%E7%A6%BB/ReadView.drawio.png)

一个事务去访问记录的时候，除了自己的更新记录总是可见之外，还有这几种情况：

- 如果记录的 trx_id 值小于 Read View 中的 `min_trx_id` 值，表示这个版本的记录是在创建 Read View **前**已经提交的事务生成的，所以该版本的记录对当前事务 **<font color=24b3aa>可见</font>**。
- 如果记录的 trx_id 值大于等于 Read View 中的 `max_trx_id` 值，表示这个版本的记录是在创建 Read View **后**才启动的事务生成的，所以该版本的记录对当前事务 **<font color=24b3aa>不可见</font>**。
- 如果记录的 trx_id 值在 Read View 的 `min_trx_id` 和 `max_trx_id`之间，需要判断 trx_id 是否在 m_ids 列表中：

  - 如果记录的 trx_id **在** `m_ids` 列表中，表示生成该版本记录的活跃事务依然活跃着（还没提交事务），所以该版本的记录对当前事务 **<font color=24b3aa>不可见</font>**。
  - 如果记录的 trx_id **不在** `m_ids`列表中，表示生成该版本记录的活跃事务已经被提交，所以该版本的记录对当前事务 **<font color=24b3aa>可见</font>**。

**这种通过「版本链」来控制并发事务访问同一个记录时的行为就叫 MVCC（Multi Version Concurrency Control, 多版本并发控制）。**

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#可重复读是如何工作的)可重复读是如何工作的？

**可重复读隔离级别是启动事务时生成一个 Read View，然后整个事务期间都在用这个 Read View**。

假设事务 A （事务 id 为51）启动后，紧接着事务 B （事务 id 为52）也启动了，那这两个事务创建的 Read View 如下：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/%E4%BA%8B%E5%8A%A1%E9%9A%94%E7%A6%BB/%E4%BA%8B%E5%8A%A1ab%E7%9A%84%E8%A7%86%E5%9B%BE-new.png)

事务 A 和 事务 B 的 Read View 具体内容如下：

- 在事务 A 的 Read View 中，它的事务 id 是 51，由于它是第一个启动的事务，所以此时活跃事务的事务 id 列表就只有 51，活跃事务的事务 id 列表中最小的事务 id 是事务 A 本身，下一个事务 id 则是 52。
- 在事务 B 的 Read View 中，它的事务 id 是 52，由于事务 A 是活跃的，所以此时活跃事务的事务 id 列表是 51 和 52，**活跃的事务 id 中最小的事务 id 是事务 A**，下一个事务 id 应该是 53。

接着，在可重复读隔离级别下，事务 A 和事务 B 按顺序执行了以下操作：

- 事务 B 读取小林的账户余额记录，读到余额是 100 万；
- 事务 A 将小林的账户余额记录修改成 200 万，并没有提交事务；
- 事务 B 读取小林的账户余额记录，读到余额还是 100 万；
- 事务 A 提交事务；
- 事务 B 读取小林的账户余额记录，读到余额依然还是 100 万；

接下来，跟大家具体分析下。

事务 B 第一次读小林的账户余额记录，在找到记录后，它会先看这条记录的 trx_id，此时**发现 trx_id 为 50，比事务 B 的 Read View 中的 min_trx_id 值（51）还小，这意味着修改这条记录的事务早就在事务 B 启动前提交过了，所以该版本的记录对事务 B 可见的**，也就是事务 B 可以获取到这条记录。

接着，事务 A 通过 UPDATE 语句将这条记录修改了（还未提交事务），将小林的余额改成 200 万，这时 MySQL 会记录相应的 undo log，并以链表的方式串联起来，形成**版本链**，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/%E4%BA%8B%E5%8A%A1%E9%9A%94%E7%A6%BB/%E4%BA%8B%E5%8A%A1ab%E7%9A%84%E8%A7%86%E5%9B%BE2.png)

你可以在上图的「记录的字段」看到，由于事务 A 修改了该记录，以前的记录就变成旧版本记录了，于是最新记录和旧版本记录通过链表的方式串起来，而且最新记录的 trx_id 是事务 A 的事务 id（trx_id = 51）。

然后事务 B 第二次去读取该记录，**发现这条记录的 trx_id 值为 51，在事务 B 的 Read View 的 min_trx_id 和 max_trx_id 之间，则需要判断 trx_id 值是否在 m_ids 范围内，判断的结果是在的，那么说明这条记录是被还未提交的事务修改的，这时事务 B 并不会读取这个版本的记录。而是沿着 undo log 链条往下找旧版本的记录，直到找到 trx_id 「小于」事务 B 的 Read View 中的 min_trx_id 值的第一条记录**，所以事务 B 能读取到的是 trx_id 为 50 的记录，也就是小林余额是 100 万的这条记录。

最后，当事物 A 提交事务后，**由于隔离级别时「可重复读」，所以事务 B 再次读取记录时，还是基于启动事务时创建的 Read View 来判断当前版本的记录是否可见。所以，即使事物 A 将小林余额修改为 200 万并提交了事务， 事务 B 第三次读取记录时，读到的记录都是小林余额是 100 万的这条记录**。

就是通过这样的方式实现了，「可重复读」隔离级别下在事务期间读到的记录都是事务启动前的记录。

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#读提交是如何工作的)读提交是如何工作的？

**读提交隔离级别是在每次读取数据时，都会生成一个新的 Read View**。

也意味着，事务期间的多次读取同一条数据，前后两次读的数据可能会出现不一致，因为可能这期间另外一个事务修改了该记录，并提交了事务。

那读提交隔离级别是怎么工作呢？我们还是以前面的例子来聊聊。

假设事务 A （事务 id 为51）启动后，紧接着事务 B （事务 id 为52）也启动了，接着按顺序执行了以下操作：

- 事务 B 读取数据（创建 Read View），小林的账户余额为 100 万；
- 事务 A 修改数据（还没提交事务），将小林的账户余额从 100 万修改成了 200 万；
- 事务 B 读取数据（创建 Read View），小林的账户余额为 100 万；
- 事务 A 提交事务；
- 事务 B 读取数据（创建 Read View），小林的账户余额为 200 万；

那具体怎么做到的呢？我们重点看事务 B 每次读取数据时创建的 Read View。前两次 事务 B 读取数据时创建的 Read View 如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/%E4%BA%8B%E5%8A%A1%E9%9A%94%E7%A6%BB/%E8%AF%BB%E6%8F%90%E4%BA%A4%E4%BA%8B%E5%8A%A1.png)

我们来分析下为什么事务 B 第二次读数据时，读不到事务 A （还未提交事务）修改的数据？

事务 B 在找到小林这条记录时，会看这条记录的 trx_id 是 51，在事务 B 的 Read View 的 min_trx_id 和 max_trx_id 之间，接下来需要判断 trx_id 值是否在 m_ids 范围内，判断的结果是在的，那么说明**这条记录是被还未提交的事务修改的，这时事务 B 并不会读取这个版本的记录**。而是，沿着 undo log 链条往下找旧版本的记录，直到找到 trx_id 「小于」事务 B 的 Read View 中的 min_trx_id 值的第一条记录，所以事务 B 能读取到的是 trx_id 为 50 的记录，也就是小林余额是 100 万的这条记录。

我们来分析下为什么事务 A 提交后，事务 B 就可以读到事务 A 修改的数据？

在事务 A 提交后，**由于隔离级别是「读提交」，所以事务 B 在每次读数据的时候，会重新创建 Read View**，此时事务 B 第三次读取数据时创建的 Read View 如下：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/ImageHost4@main/mysql/%E4%BA%8B%E5%8A%A1%E9%9A%94%E7%A6%BB/%E8%AF%BB%E6%8F%90%E4%BA%A4%E4%BA%8B%E5%8A%A12.drawio.png)

事务 B 在找到小林这条记录时，**会发现这条记录的 trx_id 是 51，比事务 B 的 Read View 中的 min_trx_id 值（52）还小，这意味着修改这条记录的事务早就在创建 Read View 前提交过了，所以该版本的记录对事务 B 是可见的**。

正是因为在读提交隔离级别下，事务每次读数据时都重新创建 Read View，那么在事务期间的多次读取同一条数据，前后两次读的数据可能会出现不一致，因为可能这期间另外一个事务修改了该记录，并提交了事务。

## [#](https://www.xiaolincoding.com/mysql/transaction/mvcc.html#总结)总结

<font color=blue>事务是在 MySQL 引擎层实现的</font>，我们常见的 InnoDB 引擎是支持事务的，事务的四大特性是原子性、一致性、隔离性、持久性，我们这次主要讲的是隔离性。

当多个事务并发执行的时候，会引发脏读、不可重复读、幻读这些问题，那为了避免这些问题，SQL 提出了四种隔离级别，分别是读未提交、读已提交、可重复读、串行化，从左往右隔离级别顺序递增，隔离级别越高，意味着性能越差，InnoDB 引擎的默认隔离级别是可重复读。

要解决脏读现象，就要将隔离级别升级到读已提交以上的隔离级别，要解决不可重复读现象，就要将隔离级别升级到可重复读以上的隔离级别。

<font color=24b3aa>而对于幻读现象，不建议将隔离级别升级为串行化，因为这会导致数据库并发时性能很差。MySQL InnoDB 引擎的默认隔离级别虽然是「可重复读」，但是它很大程度上避免幻读现象（并不是完全解决了，详见这篇[文章 (opens new window)](https://xiaolincoding.com/mysql/transaction/phantom.html)），解决的方案有两种：</font>

- 针对**快照读**（普通 SELECT 语句），是**通过 MVCC 方式解决了幻读**，因为可重复读隔离级别下，事务执行过程中看到的数据，一直跟这个事务启动时看到的数据是一致的，即使中途有其他事务插入了一条数据，是查询不出来这条数据的，所以就很好了避免幻读问题。
- 针对**当前读**（SELECT ... FOR UPDATE 等语句），是**通过 next-key lock（记录锁+间隙锁）方式解决了幻读**，因为当执行 SELECT ... FOR UPDATE 语句的时候，会加上 next-key lock，如果有其他事务在 next-key lock 锁范围内插入了一条记录，那么这个插入语句就会被阻塞，无法成功插入，所以就很好了避免幻读问题。

对于「读提交」和「可重复读」隔离级别的事务来说，它们是通过 Read View 来实现的，它们的区别在于创建 Read View 的时机不同：

- 「读提交」隔离级别是在每个 SELECT 都会生成一个新的 Read View，也意味着，事务期间的多次读取同一条数据，前后两次读的数据可能会出现不一致，因为可能这期间另外一个事务修改了该记录，并提交了事务。
- 「可重复读」隔离级别是启动事务时生成一个 Read View，然后整个事务期间都在用这个 Read View，这样就保证了在事务期间读到的数据都是事务启动前的记录。

这两个隔离级别实现是通过「事务的 Read View 里的字段」和「记录中的两个隐藏列」的比对，来控制并发事务访问同一个记录时的行为，这就叫 MVCC（多版本并发控制）。

在可重复读隔离级别中，普通的 SELECT 语句就是基于 MVCC 实现的快照读，也就是不会加锁的。而 SELECT .. FOR UPDATE 语句就不是快照读了，而是当前读了，也就是每次读都是拿到最新版本的数据，但是它会对读到的记录加上 next-key lock 锁。

# [事务]MySQL 可重复读隔离级别，完全解决幻读了吗？

大家好，我是小林。

我在[上一篇文章 (opens new window)](https://xiaolincoding.com/mysql/transaction/mvcc.html)提到，MySQL InnoDB 引擎的默认隔离级别虽然是「可重复读」，但是它很大程度上避免幻读现象（并不是完全解决了），解决的方案有两种：

- 针对  **<font color=blue>快照读</font>**（普通 SELECT 语句），是 **通过 <font color=ee11ff>MVCC</font> 方式解决了幻读**，因为可重复读隔离级别下，事务执行过程中看到的数据，一直跟这个事务启动时看到的数据是一致的，即使中途有其他事务插入了一条数据，是查询不出来这条数据的，所以就很好了避免幻读问题。
- 针对 **<font color=blue>当前读</font>**（SELECT ... FOR UPDATE 等语句），是 **通过 <font color=ee11ff>next-key lock（记录锁+间隙锁）</font>方式解决了幻读**，因为当执行 SELECT ... FOR UPDATE 语句的时候，会加上 next-key lock，如果有其他事务在 next-key lock 锁范围内插入了一条记录，那么这个插入语句就会被阻塞，无法成功插入，所以就很好了避免幻读问题。

这两个解决方案是很大程度上解决了幻读现象，但是还是有个别的情况造成的幻读现象是无法解决的。

这次，就跟大家好好聊这个问题。

## [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#什么是幻读)什么是幻读？

首先来看看 MySQL 文档是怎么定义幻读（Phantom Read）的:

***The so-called <font color=24b3aa>phantom problem</font> occurs within a transaction when <font color=blue>the same query produces different sets of rows at different times.</font> For example, if a SELECT is executed twice, but returns a row the second time that was not returned the first time, the row is a “phantom” row.***

举个例子，假设一个事务在 T1 时刻和 T2 时刻分别执行了下面查询语句，途中没有执行其他任何语句：

```sql
SELECT * FROM t_test WHERE id > 100;
```

只要 T1 和 T2 时刻执行产生的结果集是不相同的，那就发生了幻读的问题，比如：

- T1 时间执行的结果是有 5 条行记录，而 T2 时间执行的结果是有 6 条行记录，那就发生了幻读的问题。
- T1 时间执行的结果是有 5 条行记录，而 T2 时间执行的结果是有 4 条行记录，也是发生了幻读的问题。

## [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#快照读是如何避免幻读的)快照读是如何避免幻读的？

可重复读隔离级是由 MVCC（多版本并发控制）实现的，实现的方式是开始事务后（执行 begin 语句后），在执行第一个查询语句后，会创建一个 Read View，**后续的查询语句利用这个 Read View，通过这个 Read View 就可以在 undo log 版本链找到事务开始时的数据，所以事务过程中每次查询的数据都是一样的**，即使中途有其他事务插入了新纪录，是查询不出来这条数据的，所以就很好了避免幻读问题。

做个实验，数据库表 t_stu 如下，其中 id 为主键。

![img](https://cdn.xiaolincoding.com//mysql/other/7f9df142b3594daeaaca495abb7133f5.png)

然后在可重复读隔离级别下，有两个事务的执行顺序如下：

![在这里插入图片描述](https://cdn.xiaolincoding.com//mysql/other/e576e047dccc47d5a59636ea342750b8.png)

从这个实验结果可以看到，即使事务 B 中途插入了一条记录，事务 A 前后两次查询的结果集都是一样的，并没有出现所谓的幻读现象。

## [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#当前读是如何避免幻读的)当前读是如何避免幻读的？

<font color=ee11ff>MySQL 里除了普通查询是快照读，其他都是 **当前读**，比如 UPDATE、insert、DELETE，这些语句执行前都会查询 **最新版本** 的数据，然后再做进一步的操作。</font>

这很好理解，假设你要 UPDATE 一个记录，另一个事务已经 DELETE 这条记录并且提交事务了，这样不是会产生冲突吗，所以 UPDATE 的时候肯定要知道最新的数据。

另外，需要注意 `SELECT ... FOR UPDATE` 这种查询语句也是当前读，每次执行的时候都是读取最新的数据。

接下来，我们假设 `SELECT ... FOR UPDATE` 当前读是不会加锁的（实际上是会加锁的），在做一遍实验。

![img](https://cdn.xiaolincoding.com//mysql/other/1f872ff92b644b5f81cee2dd9188b199.png)

这时候，事务 B 插入的记录，就会被事务 A 的第二条查询语句查询到（因为是当前读），这样就会出现前后两次查询的结果集合不一样，这就出现了幻读。

所以，**<font color=24b3aa>Innodb 引擎为了解决「可重复读」隔离级别使用「当前读」而造成的幻读问题，就引出了间隙锁</font>**。

假设，表中有一个范围 id 为 `(3,5)` 间隙锁，那么其他事务就无法插入 id = 4 这条记录了，这样就有效的防止幻读现象的发生。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/gap%E9%94%81.drawio.png)

举个具体例子，场景如下：

![img](https://cdn.xiaolincoding.com//mysql/other/3af285a8e70f4d4198318057eb955520.png)

事务 A 执行了这面这条锁定读语句后，就在对表中的记录加上 id 范围为 `(2, +∞]` 的 next-key lock（next-key lock 是间隙锁+记录锁的组合）。

然后，事务 B 在执行插入语句的时候，判断到插入的位置被事务 A 加了 next-key lock，于是事物 B 会生成一个插入意向锁，同时进入 <font color=24b3aa>等待状态</font>，直到事务 A 提交了事务。这就避免了由于事务 B 插入新记录而导致事务 A 发生幻读的现象。

## [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#幻读被完全解决了吗)<font color=red>幻读被完全解决了吗？</font>

<font color=blue>**可重复读隔离级别下虽然很大程度上避免了幻读，但是还是没有能完全解决幻读😓**。</font>

我举例一个可重复读隔离级别发生幻读现象的场景。

### [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#第一个发生幻读现象的场景)第一个发生幻读现象的场景

还是以这张表作为例子：

![img](https://cdn.xiaolincoding.com//mysql/other/7f9df142b3594daeaaca495abb7133f5-20230309222119359.png)

事务 A 执行查询 id = 5 的记录，此时表中是没有该记录的，所以查询不出来。

```sql
# 事务 A
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM t_stu WHERE id = 5;
Empty set (0.01 sec)
```

然后事务 B 插入一条 id = 5 的记录，并且提交了事务。

```sql
# 事务 B
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> insert into t_stu values(5, '小美', 18);
Query OK, 1 row affected (0.00 sec)

mysql> commit;
Query OK, 0 rows affected (0.00 sec)
```

此时，<font color=e133>事务 A 更新 id = 5 这条记录，对没错，事务 A 看不到 id = 5 这条记录，但是他去更新了这条记录，这场景确实很违和，然后再次查询 id = 5 的记录，事务 A 就能看到事务 B 插入的纪录了，幻读就是发生在这种违和的场景</font>。

```sql
# 事务 A
mysql> UPDATE t_stu set name = '小林coding' WHERE id = 5;
Query OK, 1 row affected (0.01 sec)
Rows matched: 1  Changed: 1  Warnings: 0

mysql> SELECT * FROM t_stu WHERE id = 5;
+----+--------------+------+
| id | name         | age  |
+----+--------------+------+
|  5 | 小林coding   |   18 |
+----+--------------+------+
1 row in set (0.00 sec)
```

整个发生幻读的时序图如下：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E5%B9%BB%E8%AF%BB%E5%8F%91%E7%94%9F.drawio.png)

<font color=24b3aa>在可重复读隔离级别下，事务 A 第一次执行普通的 SELECT 语句时生成了一个 ReadView，之后事务 B 向表中新插入了一条 id = 5 的记录并提交。接着，事务 A 对 id = 5 这条记录进行了更新操作，在这个时刻，这条新记录的 trx_id 隐藏列的值就变成了事务 A 的事务 id，之后事务 A 再使用普通 SELECT 语句去查询这条记录时就可以看到这条记录了，于是就发生了幻读。</font>

因为这种特殊现象的存在，所以我们认为 **MySQL Innodb 中的 MVCC 并不能完全避免幻读现象**。

> 这段现象，核心点其实就两句话：
>
> 1. **事务 A 第一次 `SELECT` 是快照读（MVCC），不加锁**，所以挡不住 B 插入 `id=5`。
> 2. **事务 A 的 `UPDATE` 是当前读**，它会“看最新已提交版本”去定位并更新那行；一旦更新成功，这行就产生了一个 **trx_id = A** 的新版本，所以 A 后面再做普通 `SELECT` 时**能看到自己写出来的版本**（read-your-writes），看起来就像“凭空多出来一行”。

### [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#第二个发生幻读现象的场景)第二个发生幻读现象的场景

除了上面这一种场景会发生幻读现象之外，还有下面这个场景也会发生幻读现象。

- T1 时刻：事务 A 先执行<font color=blue>「快照读语句」</font>：SELECT * FROM t_test WHERE id > 100 得到了 3 条记录。
- T2 时刻：事务 B 往插入一个 id= 200 的记录并提交；
- T3 时刻：事务 A 再执行<font color=blue>「当前读语句」</font> SELECT * FROM t_test WHERE id > 100 FOR UPDATE 就会得到 4 条记录，此时也发生了幻读现象。

> 事务 A 第二次读取时要使用 `SELECT ... FOR UPDATE` （当前读）直接读取已提交的最新数据才会产生幻读。

<font color=e133>**要避免这类特殊场景下发生幻读的现象的话，就是尽量在开启事务之后，马上执行 SELECT ... FOR UPDATE 这类当前读的语句**，因为它会对记录加 next-key lock，从而避免其他事务插入一条新记录。</font>

## [#](https://www.xiaolincoding.com/mysql/transaction/phantom.html#总结)总结

MySQL InnoDB 引擎的可重复读隔离级别（默认隔离级），根据不同的查询方式，分别提出了避免幻读的方案：

- 针对 **<font color=blue>快照读</font>**（普通 SELECT 语句），是通过 MVCC 方式解决了幻读。
- 针对 **<font color=blue>当前读</font>**（SELECT ... FOR UPDATE 等语句），是通过 next-key lock（记录锁+间隙锁）方式解决了幻读。

我举例了两个发生幻读场景的例子。

* 第一个例子：对于快照读， MVCC 并不能完全避免幻读现象。因为当事务 A 更新了一条事务 B 插入的记录，那么事务 A 前后两次查询的记录条目就不一样了，所以就发生幻读。
* 第二个例子：对于当前读，如果事务开启后，并没有执行当前读，而是先快照读，然后这期间如果其他事务插入了一条记录，那么事务后续使用当前读进行查询的时候，就会发现两次查询的记录条目就不一样了，所以就发生幻读。

所以，**MySQL 可重复读隔离级别并没有彻底解决幻读，只是很大程度上避免了幻读现象的发生。**

要避免这类特殊场景下发生幻读的现象的话，就是尽量在开启事务之后，马上执行 SELECT ... FOR UPDATE 这类当前读的语句，因为它会对记录加 next-key lock，从而避免其他事务插入一条新记录。

> <font color=blue>归根结底，Mysql InnoDB 可重复读隔离级别下，依然会出现幻读的问题，并且发生幻读的原因其本质都是相同的：</font>
>
> * 当前事务首先进行一次快照读（不加锁，可能读到旧数据）
> * 此时另一个事务插入了一条消息
> * 当前事务进行一次当前读，读取到了其它事务插入的消息

# [事务]MySQL 两阶段提交

> ref：https://learnku.com/docs/daily-qa/2021-07-16-why-do-redo-logs-need-to-be-submitted-in-two-phases/11200

## #MySQL 中的日志

为了保证事务的 ACID 特性，MySQL 需要记录一些日志来辅助事务的执行和恢复。MySQL 中主要有两种日志：redo log 和 binlog。

redo log 是 InnoDB 存储引擎特有的日志，用于记录数据页的物理修改，保证事务的持久性和原子性。redo log 是循环写入的，由两部分组成：一块固定大小的内存区域（redo log buffer）和一组固定大小的磁盘文件（redo log file）。当事务对数据进行修改时，会先将修改记录到 redo log buffer 中，然后在适当的时机将其刷新到 redo log file 中。这样即使数据库发生异常重启，也可以根据 redo log 恢复数据。

binlog 是 MySQL Server 层的日志，用于记录 SQL 语句的逻辑修改，保证事务的一致性。binlog 是追加写入的，由一个 binlog 文件序列和一个索引文件组成。当事务提交时，会将 SQL 语句记录到 binlog 中。binlog 主要用于数据备份、恢复和主从复制。

## #<font color=blue>为什么需要两阶段提交</font>

如果只有 redo log 或者只有 binlog，那么事务就不需要两阶段提交。但是如果同时使用了 redo log 和 binlog，那么就需要保证这两种日志之间的一致性。否则，在数据库发生异常重启或者主从切换时，可能会出现数据不一致的情况。

例如，假设我们有一个事务 T，它修改了两行数据 A 和 B，并且同时开启了 redo log 和 binlog。如果我们先写 redo log 再写 binlog，并且在写完 redo log 后数据库发生了宕机，那么在重启后，根据 redo log 我们可以恢复 A 和 B 的修改，但是 binlog 中没有记录 T 的信息，导致备份或者从库中没有 T 的修改。反之，如果我们先写 binlog 再写 redo log，并且在写完 binlog 后数据库发生了宕机，那么在重启后，根据 redo log 我们无法恢复 A 和 B 的修改，但是 binlog 中有记录 T 的信息，导致备份或者从库中有 T 的修改。

为了避免这种情况，MySQL 引入了两阶段提交的机制。两阶段提交就是将一个事务分成两个阶段来提交：prepare 阶段和 commit 阶段。在 prepare 阶段，事务会先写 redo log 并将其标记为 prepare 状态，然后写 binlog；在 commit 阶段，事务会将 redo log 标记为 commit 状态，并将 binlog 落盘。这样，无论数据库在哪个时刻发生宕机，都可以根据 redo log 和 binlog 的状态来判断事务是否提交，并保证数据的一致性。

## #什么是两阶段提交

<font color=24b3aa>MySQL 的两阶段提交（2PC）主要是为了保证 **InnoDB 存储引擎的 Redo Log** 和 **Server 层的 Binlog** 这两份日志的数据一致性，防止数据库宕机重启后导致主从库数据对不上。</font>

![img](https://pica.zhimg.com/v2-fd517a8d56ef67dad9f8d384d1a23fd4_1440w.jpg)

1. 在准备阶段，MySQL 先将数据修改写入 redo log，并将其标记为 prepare 状态，表示事务还未提交。然后将对应的 SQL 语句写入 bin log。
2. 在提交阶段，MySQL 将 redo log 标记为 commit 状态，表示事务已经提交。然后根据 sync_binlog 参数的设置，决定是否将 bin log 刷入磁盘。

**为什么要分两步走？** 万一在执行过程中突然断电，MySQL 重启时会这么处理：

- **如果刚写完 Prepare，Binlog 还没写就崩了：** 既然官方档案没记录，那重启后直接**回滚**这笔事务。
- **如果 Binlog 写完了，但最后一步还没改 Commit 就崩了：** MySQL 发现 Binlog 里有完整记录，为了保证以后用 Binlog 恢复的数据和现在一致，重启后会顺手把这笔事务给**提交**掉。

## #如何判断 binlog 和 redolog 是否达成了一致

当 MySQL 写完 redolog 并将它标记为 prepare 状态时，会在 redolog 中记录一个 XID，它全局唯一的标识着这个事务。而当你设置 `sync_binlog=1` 时，做完了上面第一阶段写 redolog 后，mysql 就会将对应 binlog 直接刷新到磁盘中。

下图就是磁盘上的 row 格式的 binlog记录。binlog 结束的位置上也有一个 XID。

只要这个  XID 和 redolog 中记录的 XID 是一致的，MySQL 就会认为 binlog 和 redolog 逻辑上一致。就上面的场景来说就会 commit，而如果仅仅是 redolog 中记录了 XID，binlog 中没有，MySQL 就会 RollBack

![img](https://pic2.zhimg.com/v2-217bcf3a4d081e4734b12c32b5a05d55_1440w.jpg)

# [日志]串行化隔离级别是如何实现的？

MySQL 的 **SERIALIZABLE（串行化）** 隔离级别是通过 **MVCC + 自动加锁** 的组合机制实现的，<font color=blue>核心思想是：**将非锁定读强制转换为锁定读**，从而消除所有并发异常。</font>

具体实现方式如下：

### **1. 对 SELECT 语句的处理**

在 SERIALIZABLE 级别下，普通的 `SELECT` 不再走 MVCC 快照读，而是**自动等价于 `SELECT ... LOCK IN SHARE MODE`**：

- 读取时会对涉及的每一行加上 **共享锁（S锁）**
- 如果其他事务持有这些行的排他锁（X锁），当前 SELECT 会**阻塞等待**
- 这彻底消除了脏读、不可重复读和幻读

> ⚠️ 注意：只有**非锁定读**（plain SELECT）会被转换。显式使用 `SELECT ... FOR UPDATE` / `LOCK IN SHARE MODE` 的行为不变。

### **2. 对 DML 语句的处理**

`INSERT`、`UPDATE`、`DELETE` 的行为与 REPEATABLE READ 相同：

- 对修改的行加 **排他锁（X锁）**
- 使用 **Next-Key Lock**（记录锁 + 间隙锁）防止幻读
- 遵循两阶段锁协议（2PL），锁直到事务提交/回滚才释放

### **3. 与 REPEATABLE READ 的关键区别**

| 特性        | REPEATABLE READ       | SERIALIZABLE                |
| :---------- | :-------------------- | :-------------------------- |
| 普通 SELECT | MVCC 快照读（不加锁） | 自动加共享锁（锁定读）      |
| 并发性能    | 读写不冲突，高并发    | 读写互斥，**严重降低并发**  |
| 幻读防护    | Next-Key Lock + MVCC  | Next-Key Lock + SELECT 加锁 |
| 实际使用    | InnoDB 默认级别，推荐 | 极少使用                    |

### **4. 本质原理**

SERIALIZABLE 的实现本质是**悲观并发控制**：

- 放弃 MVCC 带来的读写并发优势
- 通过强制加锁使事务的执行效果等同于**某种串行调度**
- 以牺牲并发性能为代价，换取最强的数据一致性保证

### **实践建议**

- **几乎不需要手动设置 SERIALIZABLE**。InnoDB 的 REPEATABLE READ 配合 Next-Key Lock 已经能防止幻读，且并发性能远优于 SERIALIZABLE。
- 仅在以下极端场景考虑：应用逻辑要求严格的串行语义，且无法通过应用层或其他手段保证。
- 设置方式：`SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE;`

**一句话总结：** SERIALIZABLE 通过将普通 SELECT 自动转为共享锁读 + DML 保持 Next-Key Lock，把所有操作都变成锁定操作，从而实现真正的串行化执行，代价是并发性能急剧下降。

# [日志]三大日志详解

大家好，我是小林。

从这篇「[执行一条 SQL 查询语句，期间发生了什么？ (opens new window)](https://xiaolincoding.com/mysql/base/how_SELECT.html)」中，我们知道了一条查询语句经历的过程，这属于「读」一条记录的过程，如下图：

![查询语句执行流程](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/sql%E6%89%A7%E8%A1%8C%E8%BF%87%E7%A8%8B/mysql%E6%9F%A5%E8%AF%A2%E6%B5%81%E7%A8%8B.png)

那么，**<font color=red>执行一条 UPDATE 语句，期间发生了什么？</font>**，比如这一条 UPDATE 语句：

```sql
UPDATE t_user SET name = 'xiaolin' WHERE id = 1;
```

查询语句的那一套流程，更新语句也是同样会走一遍：

- 客户端先通过连接器建立连接，连接器自会判断用户身份；
- 因为这是一条 UPDATE 语句，所以不需要经过查询缓存，但是 <font color=24b3aa>表上有更新语句，是会把整个表的查询缓存清空的</font>，所以说查询缓存很鸡肋，在 MySQL 8.0 就被移除这个功能了；
- 解析器会通过词法分析识别出关键字 UPDATE，表名等等，构建出语法树，接着还会做语法分析，判断输入的语句是否符合 MySQL 语法；
- 预处理器会判断表和字段是否存在；
- 优化器确定执行计划，因为 WHERE 条件中的 id 是主键索引，所以决定要使用 id 这个索引；
- 执行器负责具体执行，找到这一行，然后更新。

不过，更新语句的流程会涉及到 undo log（回滚日志）、redo log（重做日志） 、binlog （归档日志）这三种日志：

- **<font color=blue>undo log（回滚日志）</font>**：是 Innodb 存储引擎层生成的日志，实现了事务中的 **原子性**，主要 **用于事务回滚和 MVCC**。
- **<font color=blue>redo log（重做日志）</font>**：是 Innodb 存储引擎层生成的日志，实现了事务中的 **持久性**，主要 **用于掉电等故障恢复**；
- **<font color=blue>binlog （归档日志）</font>**：是 Server 层生成的日志，主要 **用于数据备份和主从复制**；

所以这次就带着这个问题，看看这三种日志是怎么工作的。

## [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#为什么需要-undo-log)为什么需要 undo log？

我们在执行执行一条“增删改”语句的时候，虽然没有输入 begin 开启事务和 commit 提交事务，但是 **MySQL 会 <font color=24b3aa>隐式开启事务</font> 来执行“增删改”语句** 的，执行完就自动提交事务的，这样就保证了执行完“增删改”语句后，我们可以及时在数据库表看到“增删改”的结果了。

* <font color=ff44ff>执行一条语句是否自动提交事务，是由 `autocommit` 参数决定的，默认是开启。所以，执行一条 UPDATE 语句也是会使用事务的。</font>

那么，考虑一个问题。一个事务在执行过程中，在还没有提交事务之前，如果 MySQL 发生了崩溃，要怎么回滚到事务之前的数据呢？

如果我们每次在事务执行过程中，都记录下回滚时需要的信息到一个日志里，那么在事务执行中途发生了 MySQL 崩溃后，就不用担心无法回滚到事务之前的数据，我们可以通过这个日志回滚到事务之前的数据。

实现这一机制就是 **undo log（回滚日志），它保证了事务的 ACID 特性中的原子性（Atomicity）**。

undo log 是一种用于撤销回退的日志。<font color=blue>在事务没提交之前，MySQL 会先记录更新前的数据到 undo log 日志文件里面</font>，当事务回滚时，可以利用 undo log 来进行回滚。

每当 InnoDB 引擎对一条记录进行操作（修改、删除、新增）时，要把回滚时需要的信息都记录到 undo log 里，比如：

- <font color=24b3aa>在 **插入** 一条记录时，要把这条记录的主键值记下来，这样之后回滚时只需要把这个主键值对应的记录 **删掉** 就好了；</font>
- <font color=24b3aa>在 **删除** 一条记录时，要把这条记录中的内容都记下来，这样之后回滚时再把由这些内容组成的记录 **插入** 到表中就好了；</font>
- <font color=24b3aa>在 **更新** 一条记录时，要把被更新的列的旧值记下来，这样之后回滚时再把这些列**更新为旧值**就好了。</font>

> 在 MySQL (InnoDB引擎) 中，系统能够精准定位并回滚特定的事务，核心依赖于 **事务 ID (TRX_ID)** 的绑定机制以及一套严密的状态追踪系统。
>
> 简单来说，MySQL 区分需要回滚哪些事务，主要通过以下 3 个核心机制：
>
> **1. 事务 ID 与 Undo Log 的深度绑定** 每当一个事务开始修改数据时，InnoDB 会为它分配一个全局递增且唯一的 **事务 ID (TRX_ID)**。同时，系统会为这个事务分配专属的 Undo Log 存储空间（Undo Segment）。 在这块空间的头部（Undo Header）中，会清晰地记录下当前占用它的 `TRX_ID`。这就相当于给撤销日志贴上了对应事务的“身份证”，两者牢牢绑定。此外，每行被修改的数据页里也有一个隐藏的指针字段（`DB_ROLL_PTR`），直接指向这行数据对应的 Undo Log 物理位置。
>
> **2. 正常运行时的回滚（语句报错或执行 ROLLBACK）** 当数据库正常运行，只是某个事务因为死锁、报错或用户主动取消需要回滚时，定位非常简单。 此时事务还在内存中活跃，InnoDB 的内存结构中维护着一个活跃事务链表（`trx_sys`）。执行该事务的线程在自己的内存上下文里，不仅知道自己的 `TRX_ID`，还直接握着指向自己那串 Undo Log 的指针。系统只需顺着内存指针找到属于自己的 Undo Log 链表，从后往前反向执行一遍（比如插入变删除，修改变改回原值）即可清理现场。
>
> **3. 宕机重启后的异常回滚（Crash Recovery）** 如果数据库突然断电或崩溃，内存里的活跃链表全丢了，MySQL 在重启时依然能精准抓出未完成的事务。
>
> - 数据库重启时，首先会通过 Redo Log 将数据恢复到崩溃那一刻的模样（前滚）。
> - 随后，InnoDB 会去扫描磁盘上所有的 Undo Log 空间。它会挨个检查 Undo Header 里的状态标记。
> - 如果发现某个 Undo 段的状态是 **`TRX_UNDO_ACTIVE`（活跃状态）**，就说明记录在案的这个 `TRX_ID` 在崩溃前还没来得及提交（Commit）。
> - 系统会把这些没提交的 `TRX_ID` 重新拎出来，组装成一个“需要回滚的黑名单”，然后顺藤摸瓜利用它们的 Undo Log 执行回滚，从而确保数据恢复到一致的状态。
>
> ----
>
> Undo Header 中的状态标记（`TRX_UNDO_STATE`）反映了该 Undo Log 段当前所处的生命周期。它的修改主要发生在以下四个关键时间点：
>
> **1. 事务启动并分配 Undo 段时** 当事务执行第一条修改语句（Insert/Update/Delete）时，InnoDB 会为其分配一个 Undo Log Segment。此时，系统会将 Undo Header 中的状态设置为 **`TRX_UNDO_ACTIVE`**。这标志着该段正被一个活跃事务占用，如果此时数据库崩溃，重启后系统看到这个状态就知道必须回滚。
>
> **2. 两阶段提交的 Prepare 阶段** 在开启了 Binlog 或分布式事务的情况下，事务进入 Prepare 阶段。此时，InnoDB 会将状态修改为 **`TRX_UNDO_PREPARED`**。这个状态非常关键，它告诉系统：该事务已经完成操作并等待最终提交。如果此时宕机，重启后系统会根据 Binlog 的完整性来决定是继续提交还是回滚。
>
> **3. 事务 Commit（提交）瞬间** 这是状态变化最复杂的时候，系统会根据 Undo Log 的类型做出不同选择：
>
> - **Insert Undo Log**：由于插入操作仅对当前事务可见，提交后即可释放。状态通常会被改为 **`TRX_UNDO_CACHED`**（进入缓存链表待重用）或直接设为可回收。
> - **Update Undo Log**：由于 MVCC（多版本并发控制）可能还需要读这些旧版本数据，它不能立即删除。状态会被改为 **`TRX_UNDO_TO_PURGE`**，随后该 Undo Log 会被加入到 History List 中，等待 Purge 线程处理。
>
> **4. Purge 线程清理完成后** 当系统中不再有任何快照读需要引用这些旧版本数据时，Purge 线程会异步清理这些 Undo Log。清理完毕后，该 Undo 段的状态会再次修改为 **`TRX_UNDO_CACHED`**（归还给缓存池）或者 **`TRX_UNDO_TO_FREE`**，以便未来给新事务使用。

<font color=blue>在发生回滚时，就读取 undo log 里的数据，然后做原先相反操作。比如当 DELETE 一条记录时，undo log 中会把记录中的内容都记下来，然后执行回滚操作的时候，就读取 undo log 里的数据，然后进行 insert 操作。</font>

<font color=ff44ff>针对 DELETE 操作和 UPDATE 操作会有一些特殊的处理：</font>

- <font color=24b3aa>DELETE 操作实际上不会立即直接删除，而是将 DELETE 对象打上 DELETE flag，标记为删除，最终的删除操作是 purge 线程完成的。</font>
- <font color=24b3aa>UPDATE 分为两种情况：UPDATE 的列是否是主键列。</font>
  - <font color=24b3aa>如果不是主键列，在 undo log 中直接反向记录是如何 UPDATE 的。即 UPDATE 是直接进行的。</font>
  - <font color=24b3aa>如果是主键列，UPDATE 分两部执行：先删除该行，再插入一行目标行。</font>

> <font color=red>Mysql 可以修改主键的值，但**强烈建议永远不要去改它**。</font>
>
> 在 MySQL (InnoDB引擎) 中修改主键，虽然语法上完全支持，但在底层系统却要为此付出极大的代价。核心原因有以下三点：
>
> **1. 底层其实是“删了重写”** InnoDB 的表结构本质上是一棵以主键为核心的树（聚簇索引），整行数据都紧紧绑在主键上。如果你直接 `UPDATE` 某个主键的值，MySQL 无法在原地进行修改。它必须先把原来的整行数据**彻底删除**，然后再带着新的主键值重新**插入**到树的另一个位置。这个过程极耗性能，还容易把数据页撑爆，引发磁盘碎片。
>
> **2. 会拖累所有的普通索引** 除了主键树，表里的其他普通索引（二级索引），它们的底部记录的都是**对应的主键值**。这就意味着，只要你改动了一个主键，表里所有的普通索引都必须跟着全部更新一遍。表里的索引越多，修改主键的开销就成倍放大。
>
> **3. 极易引发关联灾难** 如果其他表通过外键依赖了这个主键，修改时可能会直接报错或者引发级联锁。即便没有外键，下游的业务系统、Redis 缓存、或者数据同步链路（如 Canal、Binlog）往往都认准了这个主键，一旦改动，很容易导致数据对不上、缓存击穿等连锁反应。
>
> <font color=blue>**如何规避这种问题？** 如果是因为业务需求变更导致你想改主键，说明表设计本身出了问题。</font>
>
> 最标准的最佳实践是：**<font color=blue>永远不要把带有实际业务含义的字段（比如手机号、身份证、订单号）当做主键</font>**。<font color=24b3aa>主键应该只用完全无意义的、全局唯一的标识（比如自增 ID 或雪花算法 ID）</font>。只要主键跟业务彻底解耦，写进去之后就再也不需要、也没有理由去修改它了。
>
> ----
>
> 对于 `UPDATE`，要看 **更新的列是否影响聚簇索引记录的位置**。在 InnoDB 里，主键就是聚簇索引，所以常简化说成“是否更新主键列”。
>
> - **如果更新的不是主键列**
>   记录在聚簇索引中的物理位置通常不用变，InnoDB 可以直接在原记录上修改对应字段。
>   这时 undo log 会记录修改前的信息，用于事务回滚和 MVCC。
>
> - **如果更新的是主键列**
>   因为主键决定了聚簇索引中记录的存储位置，主键值一变，原记录就不能继续待在原位置。
>   所以 InnoDB 通常会把它当成：
>
>   1. 删除旧记录
>   2. 插入新记录
>
>   对外看是一次 `UPDATE`，但底层更接近 `DELETE + INSERT`。

不同的操作，需要记录的内容也是不同的，所以不同类型的操作（修改、删除、新增）产生的 undo log 的格式也是不同的，具体的每一个操作的 undo log 的格式我就不详细介绍了，感兴趣的可以自己去查查。

一条记录的每一次更新操作产生的 undo log 格式都有一个 roll_pointer 指针和一个 trx_id 事务id：

- 通过 trx_id 可以知道该记录是被哪个事务修改的；
- 通过 roll_pointer 指针可以将这些 undo log 串成一个链表，这个链表就被称为版本链；

另外，**<font color=blue>undo log 还有一个作用，通过 ReadView + undo log 实现 MVCC（多版本并发控制）</font>**。

对于「读提交」和「可重复读」隔离级别的事务来说，它们的快照读（普通 SELECT 语句）是通过 Read View + undo log 来实现的，它们的区别在于创建 Read View 的时机不同：

- 「读提交」隔离级别是在每个 SELECT 都会生成一个新的 Read View，也意味着，事务期间的多次读取同一条数据，前后两次读的数据可能会出现不一致，因为可能这期间另外一个事务修改了该记录，并提交了事务。
- 「可重复读」隔离级别是启动事务时生成一个 Read View，然后整个事务期间都在用这个 Read View，这样就保证了在事务期间读到的数据都是事务启动前的记录。

<font color=24b3aa>这两个隔离级别实现是通过「事务的 Read View 里的字段」和「记录中的两个隐藏列（trx_id 和 roll_pointer）」的比对，如果不满足可见行，就会顺着 undo log 版本链里找到满足其可见性的记录</font>，从而控制并发事务访问同一个记录时的行为，这就叫 MVCC（多版本并发控制）。具体的实现可以看我这篇文章：[事务隔离级别是怎么实现的？(opens new window)](https://xiaolincoding.com/mysql/transaction/mvcc.html#事务的隔离级别有哪些)

因此，<font color=blue>**undo log 两大作用：**</font>

- **实现事务回滚，保障事务的原子性**。事务处理过程中，如果出现了错误或者用户执 行了 ROLLBACK 语句，MySQL 可以利用 undo log 中的历史数据将数据恢复到事务开始之前的状态。
- **实现 MVCC（多版本并发控制）关键因素之一**。MVCC 是通过 ReadView + undo log 实现的。undo log 为每条记录保存多份历史数据，MySQL 在执行快照读（普通 SELECT 语句）的时候，会根据事务的 Read View 里的信息，顺着 undo log 的版本链找到满足其可见性的记录。

TIP

很多人疑问 undo log 是如何刷盘（持久化到磁盘）的？

<font color=24b3aa>undo log 和数据页的刷盘策略是一样的，都需要通过 redo log 保证持久化。buffer pool 中有 undo 页，对 undo 页的修改也都会记录到 redo log。redo log 会每秒刷盘，提交事务时也会刷盘，数据页和 undo 页都是靠这个机制保证持久化的。</font>

## [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#为什么需要-redo-log)为什么需要 redo log ？

Buffer Pool 是提高了读写效率没错，但是问题来了，Buffer Pool 是基于内存的，而内存总是不可靠，万一断电重启，还没来得及落盘的脏页数据就会丢失。

<font color=24b3aa>为了防止断电导致数据丢失的问题，当有一条记录需要更新的时候，InnoDB 引擎就会先更新内存里的 Buffer Pool（同时标记为脏页），然后 **将本次对这个页的修改以 redo log 的形式记录下来**，这个时候**更新事务的持久化承诺已经达成**了。</font>

> 为什么 Buffer Pool 不直接刷盘？
>
> - **随机 IO vs 顺序 IO**：Buffer Pool 中的脏页在内存中是随机修改的，如果每次提交都直接将脏页写入磁盘数据文件，会产生大量的**随机 IO**，性能极差。而 Redo Log 是追加写入的，属于**顺序 IO**，即使在高并发下也能保持极高的吞吐性能。
> - **解耦“逻辑完成”与“物理落盘”**：通过 Redo Log，InnoDB 将“事务提交的响应时间”与“脏页刷盘的耗时”彻底解耦。只要 Redo Log 写成功并 fsync，事务就可以返回成功，脏页可以在后台慢慢异步刷入数据文件。

后续，InnoDB 引擎会在适当的时候，由后台线程将缓存在 Buffer Pool 的脏页刷新到磁盘里，这就是 **WAL （Write-Ahead Logging）技术**。

**<font color=ff44ff>WAL 技术指的是， MySQL 的写操作并不是立刻写到磁盘上，而是先写日志（redolog），然后在合适的时间再写到磁盘上</font>**。

> **1. Undo Log：本质上就是一种“特殊的数据”** 不要被“Log”这个词骗了，InnoDB 在底层是把 Undo Log 当作**普通表数据**一样来对待的。
>
> - **在内存中**：当你需要修改数据，产生 Undo 记录时，它会先写在内存的 **Buffer Pool（缓冲池）** 里的 Undo 数据页中。
> - **在磁盘中**：就像普通数据页会被后台线程刷入磁盘一样，Undo 页最终也会被刷入磁盘上的 **Undo Tablespace（撤销表空间文件）** 中长久保存。
>
> **2. Redo Log：先写内存，再刷磁盘** Redo Log 的使命是防断电，为了保证极高的写入速度，它采用了分步走的策略：
>
> - **在内存中**：事务执行时，记录会先被疯狂地写入内存中一块专属的区域，叫 **Redo Log Buffer（日志缓冲区）**。
> - **在磁盘中**：<font color=24b3aa>当事务提交时（或者后台线程每秒轮询时），系统会把 Buffer 里的日志顺序追加到磁盘上的 **ib_logfile（重做日志文件）** 中。</font>这就是前面提到的 WAL 机制落盘的那一步。
>
> **3. 最核心的套娃关系：Redo 保护 Undo** 既然 Undo Log 本质上就像普通数据一样存在 Buffer Pool 里，那如果突然断电，内存里的 Undo Log 丢了怎么办？ 答案是：**修改 Undo Log 的操作本身，也会生成对应的 Redo Log！** 如果宕机，MySQL 重启时会先通过 Redo Log 把所有数据（包括 Undo Log）恢复到崩溃前的状态，然后再利用完整的 Undo Log 把未提交的脏事务回滚掉。

需要注意的是，虽然机制上 WAL 保证了安全性，但实际行为取决于这个参数：``innodb_flush_log_at_trx_commit``。因为我们写 redolog 时，是写入到内存中的 redolog buffer，此时 redolog 并没有 fsync 到磁盘。

| 值       | 行为                                                    | 安全性                 | 性能     |
| -------- | ------------------------------------------------------- | ---------------------- | -------- |
| 1 (默认) | 事务提交时，Redo Log 直接 `fsync` 到磁盘                | 最高，断电不丢数据     | 相对最低 |
| 2        | 事务提交时，Redo Log 只写到 OS Page Cache，每秒 `fsync` | OS 崩溃可能丢失1秒数据 | 较高     |
| 0        | 事务提交时不写 Redo Log，每秒批量写入并 `fsync`         | 可能丢失1秒数据        | 最高     |

对值为 2 的情况，将 Redolog 先 fsync 到操作系统的缓存 Page Cache，这样即使 Mysql 崩溃宕机了，但只要操作系统正常运行，redolog 就还存在于内存之中。但如果操作系统也崩溃了，那就找不回来了。

> 什么是 redo log？

<font color=red>redo log 是 **物理日志**，记录了某个数据页做了什么修改，比如 **对 XXX 表空间中的 YYY 数据页 ZZZ 偏移量的地方做了AAA 更新**</font>，每当执行一个事务就会产生这样的一条或者多条物理日志。

* 这很好理解，毕竟 redolog 记录的是对 MySQL Buffer Pool 的修改，直接对应数据页。

在事务提交时，只要先将 redo log 持久化到磁盘即可，可以不需要等到将缓存在 Buffer Pool 里的脏页数据持久化到磁盘。

当系统崩溃时，虽然脏页数据没有持久化，但是 redo log 已经持久化，接着 MySQL 重启后，可以根据 redo log 的内容，将所有数据恢复到最新的状态。

> 修改 Undo 页面，需要记录对应 redo log 吗？

需要的。

开启事务后，InnoDB 层更新记录前，首先要记录相应的 undo log，如果是更新操作，需要把被更新的列的旧值记下来，也就是要生成一条 undo log，undo log 会写入 Buffer Pool 中的 Undo 页面。

不过，**在内存修改该 Undo 页面后，也是需要记录对应的 redo log，因为 undo log 也要实现持久性的保护**。

> redo log 和 undo log 区别在哪？

这两种日志是属于 InnoDB 存储引擎的日志，它们的区别在于：

- undo log 记录了此次事务「**修改前**」的数据状态，记录的是更新 **之前** 的值，**主要用于事务回滚，保证事务的原子性**。
- redo log 记录了此次事务「**修改后**」的数据状态，记录的是更新 **之后** 的值，**主要用于事务崩溃恢复，保证事务的持久性**。

事务提交之前发生了崩溃，重启后会通过 undo log 回滚事务。

* 这里的崩溃不是宕机崩溃，而是事务执行错误，mysql 还是正常运行的。
* 如果是宕机崩溃的话，其实就不需要通过 undo log 回滚了，因为事务没有提交，事务的数据并不会持久化，还是在内存中，宕机崩溃了数据就丢失了，反正事务都没有提交成功，所以数据本身就无意义的，丢失了就丢失了

事务提交之后发生了崩溃，重启后会通过 redo log 恢复事务。

* 这里的崩溃是宕机崩溃

所以有了 redo log，再通过 WAL 技术，InnoDB 就可以保证即使数据库发生异常重启，之前已提交的记录都不会丢失，这个能力称为 **crash-safe**（崩溃恢复）。可以看出来， **redo log 保证了事务四大特性中的持久性**。

> redo log 要写到磁盘，数据也要写磁盘，为什么要多此一举？

写入 redo log 的方式使用了追加操作， 所以磁盘操作是 **顺序写**，而写入数据需要先找到写入位置，然后才写到磁盘，所以磁盘操作是 **随机写**。

磁盘的「顺序写 」比「随机写」 高效的多，因此 redo log 写入磁盘的开销更小。

可以说这是 WAL 技术的另外一个优点：**<font color=24b3aa>MySQL 的写操作从磁盘的「随机写」变成了「顺序写」</font>**，提升语句的执行性能。这是因为 MySQL 的写操作并不是立刻更新到磁盘上，而是先记录在日志上，然后在合适的时间再更新到磁盘上 。

至此， 针对为什么需要 redo log 这个问题我们有两个答案：

- **实现事务的持久性，让 MySQL 有 crash-safe 的能力**，能够保证 MySQL 在任何时间段突然崩溃，重启后之前已提交的记录都不会丢失；
- **将写操作从「随机写」变成了「顺序写」**，提升 MySQL 写入磁盘的性能。

> 产生的 redo log 是直接写入磁盘的吗？

不是的。

实际上， 执行一个事务的过程中，产生的 redo log 也不是直接写入磁盘的，因为这样会产生大量的 I/O 操作，而且磁盘的运行速度远慢于内存。

所以，redo log 也有自己的缓存—— **<font color=24b3aa>redo log buffer</font>**，每当产生一条 redo log 时，会先写入到 redo log buffer，后续在持久化到磁盘如下图：

redo log buffer 默认大小 16 MB，可以通过 `innodb_log_Buffer_size` 参数动态的调整大小，增大它的大小可以让 MySQL 处理「大事务」是不必写入磁盘，进而提升写 IO 性能。

> 如果 Redo Log 在写入缓冲区后，没有刷盘电脑就死机了，数据不就丢失了？
>
> 完全正确。如果 Redo Log 还在内存的 Log Buffer 里，还没来得及刷入物理磁盘，电脑就突然死机或断电，**这部分日志确实会灰飞烟灭**。
>
> 但是，这会不会导致你的“业务数据丢失”（即你以为提交成功了，结果数据没存上），完全取决于 MySQL 的一个核心配置参数：**`innodb_flush_log_at_trx_commit`**。
>
> MySQL 把决定权交给了你，提供了 3 种刷盘策略：
>
> **1. 策略 1：绝对安全（默认选项）** 当你执行 `COMMIT` 提交事务时，MySQL **死磕到底**，必须亲眼看着这个事务的 Redo Log 真正写入了物理磁盘，才会对客户端返回“提交成功”。
>
> - **结果**：只要系统提示你成功了，哪怕下一秒立刻拔电源，数据也**绝对不会丢**。这是用一点性能换取 100% 的安全。
>
> **2. 策略 0：性能狂飙（最危险）** 当你提交事务时，MySQL 把日志扔在内存（Log Buffer）里就不管了，直接给你返回“成功”。后台会有一个线程，默默地**每秒钟**把内存里的日志往磁盘上刷一次。
>
> - **结果**：写数据的速度快到飞起。但如果突然断电，你**最多会丢失最近 1 秒钟内所有已经提交的数据**。这正是你担忧的那种情况。
>
> **3. 策略 2：折中方案（防软件崩溃，防不住断电）** 当你提交事务时，MySQL 会把日志写进操作系统的内存缓存（OS Cache）中，然后立刻返回“成功”。同样是后台线程每秒钟触发一次将 OS Cache 刷入物理磁盘。
>
> - **结果**：如果只是 MySQL 这个软件崩了，操作系统还在，数据不会丢；但如果是**整机断电**，操作系统也挂了，依然**会丢失最多 1 秒钟的数据**。

### #redo log 不是越大越好

redo log 增大，虽然能让 **刷脏页压力减轻**，但会带来几个问题。

***（1）崩溃恢复时间会变长***

数据库宕机后，重启时要根据 redo log 做恢复。如果 redo log 很大，需要扫描和重放的日志就可能更多，恢复时间会变长。

也就是：

- redo log 小：checkpoint 更频繁，恢复范围小
- redo log 大：checkpoint 压力小，但 crash recovery 可能更慢

***（2）checkpoint 推迟太久，不一定总是好事***

redo log 空间是循环使用的。旧日志要想被覆盖，前提是对应的脏页已经刷盘，也就是 checkpoint 要往前推进。

如果 redo log 很大：

- 短期看，数据库可以积攒更多脏页，写入更“从容”
- 但脏页积累过多，也会让后续刷盘压力变得集中
- 一旦触发强制刷盘，可能出现性能抖动

所以它只是把压力往后推，不是消灭压力。

***（3）占用更多磁盘空间***

这是最直接的问题。 虽然日志文件通常不算特别夸张，但做得过大，本身就是资源浪费。

不过，太小也不行。如果 redo log 太小：

- 很快就写满
- 为了继续写，就必须推动 checkpoint
- checkpoint 又要求把相关脏页刷盘
- 刷盘过于频繁，会导致用户线程被连带影响，吞吐下降

所以：

- **太小**：写入容易被刷盘拖累
- **太大**：恢复时间变长，脏页管理可能更粗放

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#redo-log-什么时候刷盘)redo log 什么时候刷盘？

缓存在 redo log buffer 里的 redo log 还是在内存中，它什么时候刷新到磁盘？

主要有下面几个时机：

- MySQL 正常关闭时；
- 当 redo log buffer 中记录的写入量大于 redo log buffer 内存空间的一半时，会触发落盘；
- innodb_flush_log_at_trx_commit 参数控制的刷盘策略
  * InnoDB 的后台线程每隔 1 秒，将 redo log buffer 持久化到磁盘 
  * 事务提交时，Redo Log 只写到 OS Page Cache，每秒 `fsync` 
  * 每次事务提交时都将缓存在 redo log buffer 里的 redo log 直接持久化到磁盘

单独执行一个更新语句的时候，InnoDB 引擎会自己启动一个事务，在执行更新语句的过程中，生成的 redo log 先写入到 redo log buffer 中，然后等事务提交的时候，再将缓存在 redo log buffer 中的 redo log 按组的方式「顺序写」到磁盘。

上面这种 redo log 刷盘时机是在事务提交的时候，这个默认的行为。

除此之外，InnoDB 还提供了另外两种策略，由参数 `innodb_flush_log_at_trx_commit` 参数控制，可取的值有：0、1、2，默认值为 1，这三个值分别代表的策略如下：

- 当设置该**参数为 1 时**，表示每次事务提交时，都**将缓存在 redo log buffer 里的 redo log 直接持久化到磁盘**，这样可以保证 MySQL 异常重启之后数据不会丢失。
- <font color=blue>当设置该**参数为 2 时**，表示每次事务提交时，都只是缓存在 redo log buffer 里的 redo log **写到 redo log 文件，注意写入到「 redo log 文件」并不意味着写入到了磁盘**</font>，因为操作系统的文件系统中有个 Page Cache（如果你想了解 Page Cache，可以看[这篇 (opens new window)](https://xiaolincoding.com/os/6_file_system/pagecache.html)），Page Cache 是专门用来缓存文件数据的，所以写入「 redo log文件」意味着写入到了操作系统的文件缓存。
- 当设置该**参数为 0 时**，表示每次事务提交时 ，还是**将 redo log 留在 redo log buffer 中** ，该模式下在事务提交时不会主动触发写入磁盘的操作。

> 把 redo 写入 redo log 文件，在操作系统层面通常意味着：数据从 MySQL 进程内存 **copy 到内核页缓存（page cache）**，此时：
>
> - ✅ **能抗“mysqld 进程崩溃”**：因为数据已经不在进程私有内存里了，进程挂了内核还在。
> - ❌ **不一定能抗“机器断电/内核崩溃/硬件故障”**：页缓存还没真正落到稳定介质（盘/有掉电保护的缓存）就可能丢。

> innodb_flush_log_at_trx_commit 为 0 和 2 的时候，什么时候才将 redo log 写入磁盘？

InnoDB 的后台线程每隔 1 秒：

- 针对参数 0 ：会把缓存在 redo log buffer 中的 redo log ，通过调用 `write()` 写到操作系统的 Page Cache，然后调用 `fsync()` 持久化到磁盘。**所以参数为 0 的策略，MySQL 进程的崩溃会导致上一秒钟所有事务数据的丢失**;
- 针对参数 2 ：调用 fsync，将缓存在操作系统中 Page Cache 里的 redo log 持久化到磁盘。**所以参数为 2 的策略，较取值为 0 情况下更安全，因为 MySQL 进程的崩溃并不会丢失数据，只有在操作系统崩溃或者系统断电的情况下，上一秒钟所有事务数据才可能丢失**。

> 这三个参数的应用场景是什么？

这三个参数的数据安全性和写入性能的比较如下：

- 数据安全性：参数 1 > 参数 2 > 参数 0
- 写入性能：参数 0 > 参数 2> 参数 1

所以，数据安全性和写入性能是熊掌不可得兼的，**要不追求数据安全性，牺牲性能；要不追求性能，牺牲数据安全性**。

- 在一些对数据安全性要求比较高的场景中，显然 `innodb_flush_log_at_trx_commit` 参数需要设置为 1。
- 在一些可以容忍数据库崩溃时丢失 1s 数据的场景中，我们可以将该值设置为 0，这样可以明显地减少日志同步到磁盘的 I/O 操作。
- 安全性和性能折中的方案就是参数 2，虽然参数 2 没有参数 0 的性能高，但是数据安全性方面比参数 0 强，因为参数 2 只要操作系统不宕机，即使数据库崩溃了，也不会丢失数据，同时性能方便比参数 1 高。

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#redo-log-文件写满了怎么办)redo log 文件写满了怎么办？

默认情况下， InnoDB 存储引擎有 1 个重做日志文件组( redo log Group），「重做日志文件组」由有 2 个 redo log 文件组成，这两个 redo 日志的文件名叫 ：`ib_logfile0` 和 `ib_logfile1` 。

在重做日志组中，每个 redo log File 的大小是固定且一致的，假设每个 redo log File 设置的上限是 1 GB，那么总共就可以记录 2GB 的操作。

重做日志文件组是以 **<font color=red>循环写</font>** 的方式工作的，从头开始写，写到末尾就又回到开头，相当于一个环形。

所以 InnoDB 存储引擎会先写 ib_logfile0 文件，当 ib_logfile0 文件被写满的时候，会切换至 ib_logfile1 文件，当 ib_logfile1 文件也被写满时，会切换回 ib_logfile0 文件。

我们知道 redo log 是为了防止 Buffer Pool 中的脏页丢失而设计的，那么如果随着系统运行，Buffer Pool 的脏页刷新到了磁盘中，那么 redo log 对应的记录也就没用了，这时候我们擦除这些旧记录，以腾出空间记录新的更新操作。

redo log 是循环写的方式，相当于一个环形，InnoDB 用 write pos 表示 redo log 当前记录写到的位置，用 checkpoint 表示当前要擦除的位置，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/checkpoint.png)

图中的：

- write pos 和 checkpoint 的移动都是顺时针方向；
- write pos ～ checkpoint 之间的部分（图中的红色部分），用来记录新的更新操作；
- check point ～ write pos 之间的部分（图中蓝色部分）：待落盘的脏数据页记录；

如果 write pos 追上了 checkpoint，就意味着 **redo log 文件满了，这时 MySQL 不能再执行新的更新操作，也就是说 MySQL 会被阻塞**（*因此所以针对并发量大的系统，适当设置 redo log 的文件大小非常重要*），此时**会停下来将 Buffer Pool 中的脏页刷新到磁盘中，然后标记 redo log 哪些记录可以被擦除，接着对旧的 redo log 记录进行擦除，等擦除完旧记录腾出了空间，checkpoint 就会往后移动（图中顺时针）**，然后 MySQL 恢复正常运行，继续执行新的更新操作。

<font color=24b3aa>所以，一次 checkpoint 的过程就是脏页刷新到磁盘中变成干净页，然后标记 redo log 哪些记录可以被覆盖的过程。</font>

## [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#为什么需要-binlog)为什么需要 binlog ？

前面介绍的 <font color=24b3aa>undo log</font> 和 <font color=24b3aa>redo log</font> 这两个日志都是 <font color=24b3aa>Innodb 存储引擎</font> 生成的。

MySQL 在完成一条更新操作后，<font color=24b3aa>**Server 层**</font> 还会生成一条 <font color=24b3aa>binlog</font>，<font color=ee11ff>等之后事务提交的时候，会将该事物执行过程中产生的所有 binlog 统一写入 binlog 文件。</font>

<font color=blue>binlog 文件是记录了所有 **数据库表结构变更** 和 **表数据修改** 的日志，不会记录查询类的操作，比如 SELECT 和 SHOW 操作。</font>

> <font color=ff44ff>为什么有了 redolog， 还要有 binlog？</font>

这个问题跟 MySQL 的时间线有关系。

* 最开始 MySQL 里并没有 InnoDB 引擎，MySQL 自带的引擎是 MyISAM，但是 MyISAM 没有 crash-safe 的能力，binlog 日志只能用于归档。
* 而 InnoDB 是另一个公司以插件形式引入 MySQL 的，**既然只依靠 binlog 是没有 crash-safe 能力的，所以 InnoDB 使用 redo log 来实现 crash-safe 能力。**

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#redo-log-和-binlog-有什么区别)redo log 和 binlog 有什么区别？

这两个日志有四个区别。

**<font color=24b3aa>*1、适用对象不同：*</font>**

- binlog 是 MySQL 的 Server 层实现的日志，所有存储引擎都可以使用；
- redo log 是 Innodb 存储引擎实现的日志；

**<font color=24b3aa>*2、文件格式不同：*</font>**

- binlog 有 3 种格式类型，分别是 STATEMENT（默认格式）、ROW、 MIXED，区别如下：
  - STATEMENT：每一条修改数据的 SQL 都会被记录到 binlog 中（相当于记录了逻辑操作，所以针对这种格式， binlog 可以称为逻辑日志），主从复制中 slave 端再根据 SQL 语句重现。但 STATEMENT 有动态函数的问题，比如你用了 uuid 或者 now 这些函数，你在主库上执行的结果并不是你在从库执行的结果，这种随时在变的函数会导致复制的数据不一致；
  - ROW：记录行数据最终被修改成什么样了（这种格式的日志，就不能称为逻辑日志了），不会出现 STATEMENT 下动态函数的问题。但 ROW 的缺点是每行数据的变化结果都会被记录，比如执行批量 UPDATE 语句，更新多少行数据就会产生多少条记录，使 binlog 文件过大，而在 STATEMENT 格式下只会记录一个 UPDATE 语句而已；
  - MIXED：包含了 STATEMENT 和 ROW 模式，它会根据不同的情况自动使用 ROW 模式和 STATEMENT 模式；
- redo log 是物理日志，记录的是在某个数据页做了什么修改，比如对 XXX 表空间中的 YYY 数据页 ZZZ 偏移量的地方做了AAA 更新；

**<font color=24b3aa>*3、写入方式不同：*</font>**

- binlog 是 **追加写**，<font color=ff44ff>写满一个文件，就创建一个新的文件继续写，不会覆盖以前的日志，保存的是全量的日志。</font>
- redo log 是 **循环写**，日志空间大小是固定，全部写满就从头开始，保存未被刷入磁盘的脏页日志。

**<font color=24b3aa>*4、用途不同：*</font>**

- <font color=blue>binlog 用于备份恢复、主从复制；</font>
- <font color=blue>redo log 用于掉电等故障恢复。</font>

> 如果不小心整个数据库的数据被删除了，能使用 redo log 文件恢复数据吗？

不可以使用 redo log 文件恢复，只能使用 binlog 文件恢复。

**因为 redo log 文件是循环写，是会边写边擦除日志的，只记录未被刷入磁盘的数据的物理日志，已经刷入磁盘的数据都会从 redo log 文件里擦除。**

binlog 文件保存的是全量的日志，也就是保存了所有数据变更的情况，理论上只要记录在 binlog 上的数据，都可以恢复，所以如果不小心整个数据库的数据被删除了，得用 binlog 文件恢复数据。

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#主从复制是怎么实现)主从复制是怎么实现？

<font color=24b3aa>MySQL 的主从复制依赖于 binlog ，也就是记录 MySQL 上的所有变化并以二进制形式保存在磁盘上。复制的过程就是将 binlog 中的数据从主库传输到从库上。</font>

这个过程一般是 **异步** 的，也就是主库上执行事务操作的线程不会等待复制 binlog 的线程同步完成。

![MySQL 主从复制过程](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E4%B8%BB%E4%BB%8E%E5%A4%8D%E5%88%B6%E8%BF%87%E7%A8%8B.drawio.png?image_process=watermark,text_5YWs5LyX5Y-377ya5bCP5p6XY29kaW5n,type_ZnpsdHpoaw,x_10,y_10,g_se,size_20,color_0000CD,t_70,fill_0)

MySQL 集群的主从复制过程梳理成 3 个阶段：

- **写入 Binlog**：主库写 binlog 日志，提交事务，并更新本地存储数据。
- **同步 Binlog**：把 binlog 复制到所有从库上，每个从库把 binlog 写到暂存日志中。
- **回放 Binlog**：回放 binlog，并更新存储引擎中的数据。

具体详细过程如下：

- MySQL 主库在收到客户端提交事务的请求之后，会先写入 binlog，再提交事务，更新存储引擎中的数据，事务提交完成后，返回给客户端“操作成功”的响应。
- 从库会创建一个专门的 I/O 线程，连接主库的 log dump 线程，来接收主库的 binlog 日志，再把 binlog 信息写入 relay log 的中继日志里，再返回给主库“复制成功”的响应。
- 从库会创建一个用于回放 binlog 的线程，去读 relay log 中继日志，然后回放 binlog 更新存储引擎中的数据，最终实现主从的数据一致性。

在完成主从复制之后，你就可以在写数据时只写主库，在读数据时只读从库，这样即使写请求会锁表或者锁记录，也不会影响读请求的执行。

![MySQL 主从架构](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E4%B8%BB%E4%BB%8E%E6%9E%B6%E6%9E%84.drawio.png?image_process=watermark,text_5YWs5LyX5Y-377ya5bCP5p6XY29kaW5n,type_ZnpsdHpoaw,x_10,y_10,g_se,size_20,color_0000CD,t_70,fill_0)

> 从库是不是越多越好？

不是的。

因为从库数量增加，从库连接上来的 I/O 线程也比较多，**主库也要创建同样多的 log dump 线程来处理复制的请求，对主库资源消耗比较高，同时还受限于主库的网络带宽**。

所以在实际使用中，一个主库一般跟 2～3 个从库（1 套数据库，1 主 2 从 1 备主），这就是一主多从的 MySQL 集群结构。

> MySQL 主从复制还有哪些模型？

主要有三种：

- **同步复制**：MySQL 主库提交事务的线程要等待所有从库的复制成功响应，才返回客户端结果。这种方式在实际项目中，基本上没法用，原因有两个：一是性能很差，因为要复制到所有节点才返回响应；二是可用性也很差，主库和所有从库任何一个数据库出问题，都会影响业务。
- **异步复制**（默认模型）：MySQL 主库提交事务的线程并不会等待 binlog 同步到各从库，就返回客户端结果。这种模式一旦主库宕机，数据就会发生丢失。
- **半同步复制**：MySQL 5.7 版本之后增加的一种复制方式，介于两者之间，事务线程不用等待所有的从库复制成功响应，只要一部分复制成功响应回来就行，比如一主二从的集群，只要数据成功复制到任意一个从库上，主库的事务线程就可以返回给客户端。这种**半同步复制的方式，兼顾了异步复制和同步复制的优点，即使出现主库宕机，至少还有一个从库有最新的数据，不存在数据丢失的风险**。

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#binlog-什么时候刷盘)binlog 什么时候刷盘？

事务执行过程中，先把日志写到 binlog cache（Server 层的 cache），事务提交的时候，再把 binlog cache 写到 binlog 文件中。

一个事务的 binlog 是不能被拆开的，因此无论这个事务有多大（比如有很多条语句），也要保证一次性写入。这是因为有一个线程只能同时有一个事务在执行的设定，所以每当执行一个 begin/start transaction 的时候，就会默认提交上一个事务，这样如果一个事务的 binlog 被拆开的时候，在备库执行就会被当做多个事务分段自行，这样破坏了原子性，是有问题的。

<font color=ff44ff>MySQL 给每个线程分配了一片内存用于缓冲 binlog ，该内存叫 **binlog cache**，参数 binlog_cache_size 用于控制单个线程内 binlog cache 所占内存的大小。如果超过了这个参数规定的大小，就要暂存到磁盘。</font>

> 什么时候 binlog cache 会写到 binlog 文件？

<font color=blue>在 **事务提交的时候**，执行器把 binlog cache 里的完整事务写入到 binlog 文件中，并清空 binlog cache。</font>如下图：

![binlog cach](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/binlogcache.drawio.png)

虽然每个线程有自己 binlog cache，但是最终都写到同一个 binlog 文件：

- 图中的 write，指的就是指把日志写入到 binlog 文件，但是并没有把数据持久化到磁盘，因为数据还缓存在文件系统的 page cache 里，write 的写入速度还是比较快的，因为不涉及磁盘 I/O。
- 图中的 fsync，才是将数据持久化到磁盘的操作，这里就会涉及磁盘 I/O，所以频繁的 fsync 会导致磁盘的 I/O 升高。

MySQL 提供一个 sync_binlog 参数来控制数据库的 binlog 刷到磁盘上的频率：

- sync_binlog = 0 的时候，表示每次提交事务都只 write，不 fsync，后续交由操作系统决定何时将数据持久化到磁盘；
- sync_binlog = 1 的时候，表示每次提交事务都会 write，然后马上执行 fsync；
- sync_binlog =N(N>1) 的时候，表示每次提交事务都 write，但累积 N 个事务后才 fsync。

在 MySQL 中系统默认的设置是 sync_binlog = 0，也就是不做任何强制性的磁盘刷新指令，这时候的性能是最好的，但是风险也是最大的。因为一旦主机发生异常重启，还没持久化到磁盘的数据就会丢失。

而当 sync_binlog 设置为 1 的时候，是最安全但是性能损耗最大的设置。因为当设置为 1 的时候，即使主机发生异常重启，最多丢失一个事务的 binlog，而已经持久化到磁盘的数据就不会有影响，不过就是对写入性能影响太大。

如果能容少量事务的 binlog 日志丢失的风险，为了提高写入的性能，一般会 sync_binlog 设置为 100~1000 中的某个数值。

# [日志]两阶段提交

## #一条 UPDATE 语句的执行过程

当优化器分析出成本最小的执行计划后，执行器就按照执行计划开始进行更新操作。

具体更新一条记录 `UPDATE t_user SET name = 'xiaolin' WHERE id = 1;` 的流程如下:

1. [Server]执行器负责具体执行，会调用存储引擎的接口，通过主键索引树搜索获取 id = 1 这一行记录：

   - 如果 id=1 这一行所在的数据页本来就在 buffer pool 中，就直接返回给执行器更新；
   - 如果记录不在 buffer pool，将数据页从磁盘读入到 buffer pool，返回记录给执行器。

2. [Server]执行器得到聚簇索引记录后，会看一下更新前的记录和更新后的记录是否一样：

   - 如果一样的话就不进行后续更新流程；
   - 如果不一样的话就把更新前的记录和更新后的记录都当作参数传给 InnoDB 层，让 InnoDB 真正的执行更新记录的操作；

3. <font color=24b3aa>[InnodDB]开启事务， InnoDB 层更新记录前，首先要记录相应的 undo log，因为这是更新操作，需要把被更新的列的旧值记下来，也就是要生成一条 undo log，undo log 会写入 Buffer Pool 中的 Undo 页面，不过在内存修改该 Undo 页面后，需要记录对应的 redo log。</font>

   > redo log 记录的是：**“对某个 InnoDB 页做了哪些物理层面的修改”**（page-level change），用来在崩溃恢复时把页重做到正确状态。
   > 所以你在内存里改了 **Undo 页**，redo 里就会记录“Undo 页被改成什么样”，以便宕机后能把 Undo 页恢复到一致状态。

4. [InnoDB]InnoDB 层开始更新记录，会先更新内存（同时标记为脏页），然后将记录写到 redo log 里面，这个时候更新就算完成了。为了减少磁盘I/O，不会立即将脏页写入磁盘，后续由后台线程选择一个合适的时机将脏页写入到磁盘。这就是 **WAL 技术**，MySQL 的写操作并不是立刻写到磁盘上，而是先写 redo 日志，然后在合适的时间再将修改的行数据写到磁盘上。（<font color=24b3aa>写 undolog -> 写 redolog -> 更新 buffer pool -> 写 redo log</font>）

   至此，一条记录更新完了。

5. [Server]在一条更新语句执行完成后，然后开始记录该语句对应的 binlog，此时记录的 binlog 会被保存到 binlog cache，并没有刷新到硬盘上的 binlog 文件，在事务提交时才会统一将该事务运行过程中的所有 binlog 刷新到硬盘。

> 具体更新一条记录 `UPDATE t_user SET name = 'xiaolin' WHERE id = 1;` 的流程如下:
>
> 1. 执行器负责具体执行，会调用存储引擎的接口，通过主键索引树搜索获取 id = 1 这一行记录：
>    - 如果 id=1 这一行所在的数据页本来就在 buffer pool 中，就直接返回给执行器更新；
>    - 如果记录不在 buffer pool，将数据页从磁盘读入到 buffer pool，返回记录给执行器。
> 2. 执行器得到聚簇索引记录后，会看一下更新前的记录和更新后的记录是否一样：
>    - 如果一样的话就不进行后续更新流程；
>    - 如果不一样的话就把更新前的记录和更新后的记录都当作参数传给 InnoDB 层，让 InnoDB 真正的执行更新记录的操作；
> 3. 开启事务， InnoDB 层更新记录前，首先要记录相应的 undo log，因为这是更新操作，需要把被更新的列的旧值记下来，也就是要生成一条 undo log，undo log 会写入 Buffer Pool 中的 Undo 页面，不过在内存修改该 Undo 页面后，需要记录对应的 redo log。
> 4. InnoDB 层开始更新记录，会先更新内存（同时标记为脏页），然后将记录写到 redo log 里面，这个时候更新就算完成了。为了减少磁盘I/O，不会立即将脏页写入磁盘，后续由后台线程选择一个合适的时机将脏页写入到磁盘。这就是 **WAL 技术**，MySQL 的写操作并不是立刻写到磁盘上，而是先写 redo 日志，然后在合适的时间再将修改的行数据写到磁盘上。
> 5. 至此，一条记录更新完了。
> 6. 在一条更新语句执行完成后，然后开始记录该语句对应的 binlog，此时记录的 binlog 会被保存到 binlog cache，并没有刷新到硬盘上的 binlog 文件，在事务提交时才会统一将该事务运行过程中的所有 binlog 刷新到硬盘。
> 7. 事务提交（为了方便说明，这里不说组提交的过程，只说两阶段提交）：
>    - **prepare 阶段**：将 redo log 对应的事务状态设置为 prepare，然后将 redo log 刷新到硬盘；
>    - **commit 阶段**：将 binlog 刷新到磁盘，接着调用引擎的提交事务接口，将 redo log 状态设置为 commit（将事务设置为 commit 状态后，刷入到磁盘 redo log 文件）；
> 8. 至此，一条更新语句执行完成。

事务提交，剩下的就是「两阶段提交」的事情了，接下来就讲这个。

## [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#为什么需要两阶段提交)为什么需要两阶段提交？

事务提交后，redo log 和 binlog 都要持久化到磁盘，但是这两个是独立的逻辑，可能出现半成功的状态，这样就造成两份日志之间的逻辑不一致。

举个例子，假设 id = 1 这行数据的字段 name 的值原本是 'jay'，然后执行 `UPDATE t_user SET name = 'xiaolin' WHERE id = 1;` 如果在持久化 redo log 和 binlog 两个日志的过程中，出现了半成功状态，那么就有两种情况：

- **如果在将 redo log 刷入到磁盘之后， MySQL 突然宕机了，而 binlog 还没有来得及写入**。MySQL 重启后，通过 redo log 能将 Buffer Pool 中 id = 1 这行数据的 name 字段恢复到新值 xiaolin，但是 binlog 里面没有记录这条更新语句，在主从架构中，binlog 会被复制到从库，由于 binlog 丢失了这条更新语句，从库的这一行 name 字段是旧值 jay，与主库的值不一致性；
- **如果在将 binlog 刷入到磁盘之后， MySQL 突然宕机了，而 redo log 还没有来得及写入**。由于 redo log 还没写，崩溃恢复以后这个事务无效，所以 id = 1 这行数据的 name 字段还是旧值 jay，而 binlog 里面记录了这条更新语句，在主从架构中，binlog 会被复制到从库，从库执行了这条更新语句，那么这一行 name 字段是新值 xiaolin，与主库的值不一致性；

可以看到，在持久化 redo log 和 binlog 这两份日志的时候，如果出现半成功的状态，就会造成主从环境的数据不一致性。这是因为 <font color=blue>redo log 影响主库的数据，binlog 影响从库的数据</font>，所以 redo log 和 binlog 必须保持一致才能保证主从数据一致。

**MySQL 为了避免出现两份日志之间的逻辑不一致的问题，使用了「两阶段提交」来解决**，两阶段提交其实是 <font color=blue>分布式事务一致性协议</font>，它可以保证多个逻辑操作要不全部成功，要不全部失败，不会出现半成功的状态。

**<font color=24b3aa>两阶段提交把单个事务的提交拆分成了 2 个阶段，分别是「准备（Prepare）阶段」和「提交（Commit）阶段」</font>**，每个阶段都由协调者（Coordinator）和参与者（Participant）共同完成。注意，不要把提交（Commit）阶段和 commit 语句混淆了，commit 语句执行的时候，会包含提交（Commit）阶段。

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#两阶段提交的过程是怎样的)两阶段提交的过程是怎样的？

```
客户端: BEGIN; UPDATE t SET v=1 WHERE id=1; COMMIT;

┌─────────────────────────────────────────────────────────┐
│                    事务执行阶段                          │
│  1. Server 层生成 SQL 的 Binlog 事件（暂存线程缓存）      │
│  2. InnoDB 执行更新，修改 Buffer Pool，生成 Redo Log     │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Prepare 阶段（InnoDB 主导）                  │
│  3. InnoDB 将 Redo Log 写入磁盘，标记为 [prepare]        │
│  4. InnoDB 调用 fsync() 确保 Redo Log 持久化             │
│  5. InnoDB 通知 Server 层："我已 prepare"                 │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Binlog 写入阶段（Server 主导）               │
│  6. Server 将 Binlog 从线程缓存写入 Binlog 文件          │
│  7. Server 调用 fsync() 确保 Binlog 持久化               │
└─────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────┐
│              Commit 阶段（InnoDB 主导）                   │
│  8. InnoDB 将 Redo Log 标记从 [prepare] 改为 [commit]    │
│  9. （可选）fsync Redo Log 的 commit 标记                │
│  10. 事务对客户端返回成功                                 │
└─────────────────────────────────────────────────────────┘
```

MySQL 崩溃重启后，扫描 Redo Log 中处于 `[prepare]` 状态的事务，按以下规则裁决：

| Redo Log 状态 | Binlog 状态            | 决策       | 理由                                                        |
| :------------ | :--------------------- | :--------- | :---------------------------------------------------------- |
| `[commit]`    | 不检查                 | ✅ **提交** | Redo Log 已确认提交，Binlog 一定已写完（否则到不了 commit） |
| `[prepare]`   | Binlog **完整**        | ✅ **提交** | Binlog 已 fsync，说明事务逻辑上已完成，补上 commit 即可     |
| `[prepare]`   | Binlog **缺失/不完整** | ❌ **回滚** | Binlog 未落盘，事务不应被承认，回滚保持主从一致             |
| 无记录        | —                      | ❌ **回滚** | Prepare 都没完成，直接丢弃                                  |

**“Binlog 完整”的判断标准**：Binlog 事件头中有长度字段，且末尾有校验码（checksum）。两者都匹配才算完整。

> 💡 **精髓**：2PC 把 **Binlog 的完整性当作事务提交的最终仲裁者**。只要 Binlog 完整落盘，事务就“事实上”应该提交；Redo Log 的 prepare 只是一个“待确认”的临时状态。

**🔹 Q: 2PC 的性能代价是什么？**

每个事务提交至少 **2 次 fsync**（Redo Prepare + Binlog）。这是 MySQL 单机写入吞吐的主要瓶颈。优化手段包括：

- **组提交（Group Commit）**：多个事务的 Binlog fsync 合并为一次系统调用
- **并行复制**：从库并发应用 Binlog
- **SSD / NVMe**：降低 fsync 延迟

**🔹 Q: 为什么不用分布式事务的 2PC 协议（XA）？**

MySQL 内部的 2PC 是**单进程内的轻量级协调**，没有网络开销、没有 Coordinator 节点、没有阻塞等待。它只是借用“Prepare + Commit”的状态机思想，与跨数据库的 XA 2PC 完全不同。

### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#两阶段提交有什么问题)两阶段提交有什么问题？

两阶段提交虽然保证了两个日志文件的数据一致性，但是性能很差，主要有两个方面的影响：

- **<font color=blue>磁盘 I/O 次数高</font>**：对于“双1”配置，每个事务提交都会进行两次 fsync（刷盘），一次是 redo log 刷盘，另一次是 binlog 刷盘。

- **<font color=blue>锁竞争激烈</font>**：两阶段提交虽然能够保证「单事务」两个日志的内容一致，但在「多事务」的情况下，却不能保证两者的提交顺序一致，因此，在两阶段提交的流程基础上，还需要加一个锁来保证提交的原子性，从而保证多事务的情况下，两个日志的提交顺序一致。

  > ### **为什么单事务 2PC 不够？—— 乱序问题复现**
  >
  > 假设两个并发事务 T1、T2 各自独立执行 2PC：
  >
  > 表格
  >
  > | 时间线 | T1                     | T2                     |
  > | :----- | :--------------------- | :--------------------- |
  > | t1     | Redo Prepare ✅         |                        |
  > | t2     |                        | Redo Prepare ✅         |
  > | t3     | Binlog Write + fsync ✅ |                        |
  > | t4     |                        | Binlog Write + fsync ✅ |
  > | t5     | Redo Commit ✅          |                        |
  > | t6     |                        | Redo Commit ✅          |
  >
  > 此时：
  >
  > - **Redo Log 顺序**：T1(prepare) → T2(prepare) → T1(commit) → T2(commit)
  > - **Binlog 顺序**：T1 → T2
  >
  > 看起来一致？**不一定**。如果 t3 和 t4 交换：
  >
  > 表格
  >
  > | 时间线 | T1                     | T2                         |
  > | :----- | :--------------------- | :------------------------- |
  > | t1     | Redo Prepare ✅         |                            |
  > | t2     |                        | Redo Prepare ✅             |
  > | t3     |                        | **Binlog Write + fsync ✅** |
  > | t4     | Binlog Write + fsync ✅ |                            |
  > | t5     | Redo Commit ✅          |                            |
  > | t6     |                        | Redo Commit ✅              |
  >
  > 现在：
  >
  > - **Binlog 顺序**：**T2 → T1**
  > - **Redo Commit 顺序**：T1 → T2
  >
  > > 💥 **灾难后果**：从库按 Binlog 顺序回放，先应用 T2 再应用 T1；但主库崩溃恢复时按 Redo Log commit 顺序，先恢复 T1 再恢复 T2。**主从数据不一致！**

> 为什么两阶段提交的磁盘 I/O 次数会很高？

binlog 和 redo log 在内存中都对应的缓存空间，binlog 会缓存在 binlog cache，redo log 会缓存在 redo log buffer，它们持久化到磁盘的时机分别由下面这两个参数控制。一般我们为了避免日志丢失的风险，会将这两个参数设置为 1：

- 当 sync_binlog = 1 的时候，表示每次提交事务都会将 binlog cache 里的 binlog 直接持久到磁盘；
- 当 innodb_flush_log_at_trx_commit = 1 时，表示每次事务提交时，都将缓存在 redo log buffer 里的 redo log 直接持久化到磁盘；

可以看到，如果 sync_binlog 和 当 innodb_flush_log_at_trx_commit 都设置为 1，那么在每个事务提交过程中， 都会**至少调用 2 次刷盘操作**，一次是 redo log 刷盘，一次是 binlog 落盘，所以这会成为性能瓶颈。

> 为什么锁竞争激烈？

为了保证 **Binlog 写入顺序 = Redo Log Commit 顺序**，MySQL 必须将“写 Binlog”和“标记 Redo Commit”这两个动作**绑定为一个不可分割的临界区**。

**🔒 核心锁：**`LOCK_log`**（Binlog 互斥锁）**

在 Server 层，所有事务在进入 Binlog 写入阶段前，必须先获取 `LOCK_log`。这保证了：

- Binlog 文件的写入是**严格串行**的
- 谁先拿到锁，谁的 Binlog 就先写入，commit 也就先完成

通过加锁虽然完美地解决了顺序一致性的问题，但在并发量较大的时候，就会导致对锁的争用，性能不佳。

#### [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#组提交)组提交

**MySQL 引入了 binlog 组提交（group commit）机制，当有多个事务提交的时候，会将多个 binlog 刷盘操作合并成一个，从而减少磁盘 I/O 的次数**，如果说 10 个事务依次排队刷盘的时间成本是 10，那么将这 10 个事务一次性一起刷盘的时间成本则近似于 1。

<font color=blue>引入了组提交机制后，prepare 阶段不变，只针对 commit 阶段，将 commit 阶段拆分为三个过程：</font>

- **<font color=24b3aa>flush 阶段</font>**：多个事务按进入顺序，将其**线程私有 Binlog Cache** 中的内容写入 Binlog 文件的 **OS Page Cache**；（`write()`）
- **<font color=24b3aa>sync 阶段</font>**：对 binlog 文件做 fsync 操作（多个事务的 binlog 合并一次刷盘）；（`fsync()`）
- **<font color=24b3aa>commit 阶段</font>**：各个事务按顺序做 InnoDB commit 操作；

> **MySQL 标准提交流程中，`write()` = 内存到 Page Cache，`fsync()` = Page Cache 到磁盘。两者职责严格分离。**

> #### **三阶段流水线锁**
>
> ```
> 时间轴 →
> Flush:  [T1,T2,T3 write]        [T4,T5 write]
> Sync:          [T1,T2,T3 fsync]        [T4,T5 fsync]
> Commit:               [T1,T2,T3 commit]       [T4,T5 commit]
>    ↑ LOCK_log    ↑ LOCK_sync     ↑ LOCK_commit
> ```
>
> - **三把锁各自保护一个阶段**，阶段间可流水线重叠
> - **T4,T5 的 Flush 可与 T1,T2,T3 的 Sync/Commit 并行**
> - **吞吐量瓶颈 = max(单阶段延迟) 而非总和**

上面的**每个阶段都有一个队列**，每个阶段有锁进行保护，因此保证了事务写入的顺序，第一个进入队列的事务会成为 leader，leader 领导所在队列的所有事务，全权负责整队的操作，完成后通知队内其他事务操作结束。

![每个阶段都有一个队列](http://keithlan.github.io/image/mysql_innodb_arch/commit_4.png)

对每个阶段引入了队列后，锁就只针对每个队列进行保护，不再锁住提交事务的整个过程，可以看的出来，**锁粒度减小了，这样就使得多个阶段可以并发执行，从而提升效率**。

> 有 binlog 组提交，那有 redo log 组提交吗？

这个要看 MySQL 版本，MySQL 5.6 没有 redo log 组提交，MySQL 5.7 有 redo log 组提交。

在 MySQL 5.6 的组提交逻辑中，每个事务各自执行 prepare 阶段，也就是各自将 redo log 刷盘，这样就没办法对 redo log 进行组提交。

所以在 MySQL 5.7 版本中，做了个改进，在 prepare 阶段不再让事务各自执行 redo log 刷盘操作，而是推迟到组提交的 flush 阶段，也就是说 prepare 阶段融合在了 flush 阶段。

这个优化是将 redo log 的刷盘延迟到了 flush 阶段之中，sync 阶段之前。通过延迟写 redo log 的方式，为 redolog 做了一次组写入，这样 binlog 和 redo log 都进行了优化。

接下来介绍每个阶段的过程，注意下面的过程针对的是“双 1” 配置（sync_binlog 和 innodb_flush_log_at_trx_commit 都配置为 1）。

> flush 阶段

第一个事务会成为 flush 阶段的 Leader，此时后面到来的事务都是 Follower ：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E7%BB%84%E6%8F%90%E4%BA%A41.png)

接着，获取队列中的事务组，由绿色事务组的 Leader 对 redo log 做一次 write + fsync，即一次将同组事务的 redolog 刷盘：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E7%BB%84%E6%8F%90%E4%BA%A42.png)

完成了 prepare 阶段后，将绿色这一组事务执行过程中产生的 binlog 写入 binlog 文件（调用 write，不会调用 fsync，所以不会刷盘，binlog 缓存在操作系统的文件系统中）。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/write_binlog.png)

从上面这个过程，可以知道 flush 阶段队列的作用是**用于支撑 redo log 的组提交**。

如果在这一步完成后数据库崩溃，由于 binlog 中没有该组事务的记录，所以 MySQL 会在重启后回滚该组事务。

> sync 阶段

绿色这一组事务的 binlog 写入到 binlog 文件后，并不会马上执行刷盘的操作，而是**会等待一段时间**，这个等待的时长由 `Binlog_group_commit_sync_delay` 参数控制，**目的是为了组合更多事务的 binlog，然后再一起刷盘**，如下过程：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E7%BB%84%E6%8F%90%E4%BA%A44.png)

不过，在等待的过程中，如果事务的数量提前达到了 `Binlog_group_commit_sync_no_delay_count` 参数设置的值，就不用继续等待了，就马上将 binlog 刷盘，如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E7%BB%84%E6%8F%90%E4%BA%A45.png)

从上面的过程，可以知道 sync 阶段队列的作用是**用于支持 binlog 的组提交**。

如果想提升 binlog 组提交的效果，可以通过设置下面这两个参数来实现：

- `binlog_group_commit_sync_delay= N`，表示在等待 N 微妙后，直接调用 fsync，将处于文件系统中 page cache 中的 binlog 刷盘，也就是将「 binlog 文件」持久化到磁盘。
- `binlog_group_commit_sync_no_delay_count = N`，表示如果队列中的事务数达到 N 个，就忽视binlog_group_commit_sync_delay 的设置，直接调用 fsync，将处于文件系统中 page cache 中的 binlog 刷盘。

如果在这一步完成后数据库崩溃，由于 binlog 中已经有了事务记录，MySQL会在重启后通过 redo log 刷盘的数据继续进行事务的提交。

> commit 阶段

最后进入 commit 阶段，调用引擎的提交事务接口，将 redo log 状态设置为 commit。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/how_UPDATE/%E7%BB%84%E6%8F%90%E4%BA%A46.png)

commit 阶段队列的作用是承接 sync 阶段的事务，完成最后的引擎提交，使得 sync 可以尽早的处理下一组事务，最大化组提交的效率。

# [日志]MySQL 磁盘 I/O 很高，有什么优化的方法？

现在我们知道事务在提交的时候，需要将 binlog 和 redo log 持久化到磁盘，那么如果出现 MySQL 磁盘 I/O 很高的现象，我们可以通过控制以下参数，来 “延迟” binlog 和 redo log 刷盘的时机，从而降低磁盘 I/O 的频率：

- 设置组提交的两个参数： binlog_group_commit_sync_delay 和 binlog_group_commit_sync_no_delay_count 参数，延迟 binlog 刷盘的时机，从而减少 binlog 的刷盘次数。这个方法是基于“额外的故意等待”来实现的，因此可能会增加语句的响应时间，但即使 MySQL 进程中途挂了，也没有丢失数据的风险，因为 binlog 早被写入到 page cache 了，只要系统没有宕机，缓存在 page cache 里的 binlog 就会被持久化到磁盘。
- 将 sync_binlog 设置为大于 1 的值（比较常见是 100~1000），表示每次提交事务都 write，但累积 N 个事务后才 fsync，相当于延迟了 binlog 刷盘的时机。但是这样做的风险是，主机掉电时会丢 N 个事务的 binlog 日志。
- 将 innodb_flush_log_at_trx_commit 设置为 2。表示每次事务提交时，都只是缓存在 redo log buffer 里的 redo log 写到 redo log 文件，注意写入到「 redo log 文件」并不意味着写入到了磁盘，因为操作系统的文件系统中有个 Page Cache，专门用来缓存文件数据的，所以写入「 redo log文件」意味着写入到了操作系统的文件缓存，然后交由操作系统控制持久化到磁盘的时机。但是这样做的风险是，主机掉电的时候会丢数据。

## [#](https://www.xiaolincoding.com/mysql/log/how_UPDATE.html#总结)总结

1. 

------

参考资料：

- 《MySQL 45 讲》
- 《MySQL 是怎样运行的？》
- https://developer.aliyun.com/article/617776
- http://mysql.taobao.org/monthly/2021/10/01/
- https://www.cnblogs.com/Neeo/articles/13883976.html
- https://www.cnblogs.com/mengxinJ/p/14211427.html

# [日志]double write 了解吗？

我们常见的服务器一般都是 Linux 操作系统，Linux 文件系统页（OS Page）的大小默认是 4KB。而 MySQL 的页（Page）大小默认是 16KB。

MySQL 程序是跑在 Linux 操作系统上的，需要跟操作系统交互，所以 MySQL 中一页数据刷到磁盘，要写 4 个文件系统里的页。

![img](https://cdn.xiaolincoding.com//picgo/1737301125998-6ecd9068-6603-4932-81c4-b54cf2984fb6.png)

需要注意的是，这个操作并非原子操作，比如我操作系统写到第二个页的时候，Linux 机器断电了，这时候就会出现问题了。造成“页数据损坏”，也就是 **<font color=blue>“部分页写入”（Partial Page Write / Torn Page）</font>** 问题。<font color=red>并且这种”页数据损坏“靠 redo 日志是无法修复的。</font>

> 机器重启 → Buffer Pool 被清空（内存是易失的！）
>
> 此时，磁盘上的页 = 前 4KB 新 + 后 12KB 旧 → 结构损坏   
>
> 但内存里的 Buffer Pool 里面的脏页被清空，没有完整副本可供“再刷”
>
> ----
>
> 你可能会想：“Redo Log 里有这个页的修改记录，重放一下不就行了？”
>
> 问题在于 **Redo Log 是物理-逻辑混合日志**，它的记录格式类似：
>
> * "在 page_id=12345 的 offset=0x1A2C 处，将 4 字节从 old_val 改为 new_val"
>
> 要应用这条记录，InnoDB 必须：
>
> 1. 从磁盘读取 page_id=12345 的页到 Buffer Pool
> 2. **验证该页的 checksum 和 LSN 是否合法**
> 3. 在指定 offset 上应用修改
>
> > 💥 **如果页是 Torn Page，步骤 2 就会失败**：
> >
> > - Checksum 校验不通过（因为一半新一半旧，哈希必然错）
> > - 页头部的 LSN 字段可能也被撕裂，无法判断该页属于哪个版本
> > - InnoDB **拒绝在一个结构损坏的页上应用任何 Redo 记录**，因为这会导致更严重的数据错乱

Doublewrite Buffer 的出现就是为了解决上面的这种情况，虽然名字带了 Buffer，但实际上 Doublewrite Buffer 是内存+磁盘的结构。

![img](https://cdn.xiaolincoding.com//picgo/1737302055987-847a5e01-f883-4b5c-8c36-26ffad1f6f69.png)

<font color=24b3aa>Doublewrite Buffer 作用是，在把页写到数据文件之前，InnoDB 先把它们写到一个叫 doublewrite buffer（双写缓冲区）的共享表空间内，在写 doublewrite buffer 完成后，InnoDB 才会把页写到数据文件的适当的位置。如果在写页的过程中发生意外崩溃，InnoDB 在稍后的恢复过程中在 doublewrite buffer 中找到完好的 page 副本用于恢复，所以本质上是一个最近写回的页面的备份拷贝。</font>

![img](https://cdn.xiaolincoding.com//picgo/1737301211946-81988282-fb5d-44f9-b8d8-94f7396db723.png)

如上图所示，当 InnoDB 需要将脏页刷入磁盘时，执行以下步骤：

1. **内存聚合**：通过 `memcpy` 将多个脏页顺序拷贝至**<font color=blue>内存中的 Doublewrite Buffer</font>**（约 2MB）。该缓冲分为两个 1MB 的槽（slot），交替使用以实现批量聚合。
2. **DWB 持久化**：当当前 slot 写满或触发刷新条件时，执行一次 `fsync()`，将该 slot 内容**顺序写入<font color=blue>磁盘上的 Doublewrite 存储区</font>**（MySQL ≤5.7 位于系统表空间 `ibdata` 中；≥8.0.20 为独立文件 `*.dblwr`）。此步骤确保完整页副本安全落盘。
3. **数据文件写入**：**确认 DWB 落盘成功后**，再将同一批脏页以随机 IO 方式写入各自的表空间文件（`.ibd`）。

根据崩溃发生的时机，分为以下四种情况：

| 场景 | 崩溃时机                 | 内存状态             | 磁盘 DWB 状态  | 数据文件状态          | 恢复策略                                             | 结果                   |
| ---- | ------------------------ | -------------------- | -------------- | --------------------- | ---------------------------------------------------- | ---------------------- |
| ①    | 步骤 1 之前              | 脏页仍在 Buffer Pool | 未写入         | 旧版本                | Redo Log 重放                                        | ✅ 安全                 |
| ②    | 步骤 1 之后、步骤 2 之前 | 已拷贝至内存 DWB     | 未落盘         | 旧版本                | 等同场景①，Redo Log 重放                             | ✅ 安全                 |
| ③    | 步骤 2 之后、步骤 3 之前 | 已丢失               | 完整副本已落盘 | 未写入/旧版本         | 无需修复（数据文件未被破坏），后续正常 Redo 重放即可 | ✅ 安全                 |
| ④ ⭐  | 步骤 3 执行过程中        | 已丢失               | 完整副本已落盘 | Torn Page（部分写入） | 从磁盘 DWB 读取完整页覆盖损坏页 → 再应用 Redo Log    | ✅ 安全（DWB 核心价值） |

由此我们可以得出结论，double write buffer 是针对实际的 buffer 数据页的原子性保证，就是避免 MySQL 异常崩溃时，写的那几个 data page不会出错，要么都写了，要么什么都没有做。

# [日志]常见问题总结

## #Undo Log 日志的作用是什么？

undo log 是一种用于撤销回退的日志，**它保证了事务的** **ACID 特性中的原子性**（Atomicity）。

在事务没提交之前，MySQL 会先记录更新前的数据到 undo log 日志文件里面，当事务回滚时，可以利用 undo log 来进行回滚。如下图：

![img](https://cdn.xiaolincoding.com//picgo/1717920811388-2146eb90-98bd-4b2d-b6a8-9c207fbdacc4.png)

每当 InnoDB 引擎对一条记录进行操作（修改、删除、新增）时，要把回滚时需要的信息都记录到 undo log 里，比如：

- 在**插入**一条记录时，要把这条记录的主键值记下来，这样之后回滚时只需要把这个主键值对应的记录**删掉**就好了；
- 在**删除**一条记录时，要把这条记录中的内容都记下来，这样之后回滚时再把由这些内容组成的记录**插入**到表中就好了；
- 在**更新**一条记录时，要把被更新的列的旧值记下来，这样之后回滚时再把这些列**更新为旧值**就好了。

<font color=blue>在发生回滚时，就读取 undo log 里的数据，然后做原先相反操作。</font>比如当 DELETE 一条记录时，undo log 中会把记录中的内容都记下来，然后执行回滚操作的时候，就读取 undo log 里的数据，然后进行 insert 操作。

## #Redo Log 日志的作用是什么？

Redo log 是 MySQL 中用于保证持久性的重要机制之一。它通过以下方式来保证持久性：

1. **<font color=blue>Write-ahead logging（WAL）：</font>**<font color=24b3aa>在事务提交之前，将事务所做的修改操作记录到 redo log 中</font>，然后再将数据写入磁盘。这样即使在数据写入磁盘之前发生了宕机，系统可以通过 redo log 中的记录来恢复数据。
2. **<font color=blue>Redo log 的顺序写入：</font>**redo log 采用追加写入的方式，将 redo 日志记录追加到文件末尾，而不是随机写入。这样可以减少磁盘的随机 I/O 操作，提高写入性能。
3. **<font color=blue>Checkpoint 机制：</font>**MySQL 会定期将内存中的数据刷新到磁盘，同时将最新的 LSN（Log Sequence Number）记录到磁盘中，这个 LSN 可以确保 redo log 中的操作是按顺序执行的。在恢复数据时，系统会根据 LSN 来确定从哪个位置开始应用 redo log。

## #bin log 的作用是什么？

MySQL 在完成一条更新操作后，Server 层还会生成一条 binlog，等之后事务提交的时候，会将该事物执行过程中产生的所有 binlog 统一写 入 binlog 文件，binlog 是 MySQL 的 Server 层实现的日志，所有存储引擎都可以使用。

binlog 是<font color=blue>**追加写**</font>，写满一个文件，就创建一个新的文件继续写，不会覆盖以前的日志，保存的是全量的日志，用于备份恢复、主从复制；

binlog 文件是记录了所有数据库表结构变更和表数据修改的日志，不会记录查询类的操作，比如 SELECT 和 SHOW 操作。

<font color=blue>binlog 有 3 种格式类型，分别是 STATEMENT（默认格式）、ROW、 MIXED</font>，区别如下：

- STATEMENT：每一条修改数据的 SQL 都会被记录到 binlog 中（相当于记录了逻辑操作，所以针对这种格式， binlog 可以称为逻辑日志），主从复制中 slave 端再根据 SQL 语句重现。但 STATEMENT 有动态函数的问题，比如你用了 uuid 或者 now 这些函数，你在主库上执行的结果并不是你在从库执行的结果，这种随时在变的函数会导致复制的数据不一致；
- ROW：记录行数据最终被修改成什么样了（这种格式的日志，就不能称为逻辑日志了），不会出现 STATEMENT 下动态函数的问题。但 ROW 的缺点是每行数据的变化结果都会被记录，比如执行批量 UPDATE 语句，更新多少行数据就会产生多少条记录，使 binlog 文件过大，而在 STATEMENT 格式下只会记录一个 UPDATE 语句而已；
- MIXED：包含了 STATEMENT 和 ROW 模式，它会根据不同的情况自动使用 ROW 模式和 STATEMENT 模式；

## #有了undolog 为啥还需要 redolog 呢？

Buffer Pool 是提高了读写效率没错，但是问题来了，Buffer Pool 是基于内存的，而内存总是不可靠，万一断电重启，还没来得及落盘的脏页数据就会丢失。

为了防止断电导致数据丢失的问题，当有一条记录需要更新的时候，InnoDB 引擎就会先更新内存（同时标记为脏页），然后将本次对这个页的修改以 redo log 的形式记录下来，**这个时候更新就算完成了**。

后续，InnoDB 引擎会在适当的时候，由后台线程将缓存在 Buffer Pool 的脏页刷新到磁盘里，这就是 **WAL （Write-Ahead Logging）技术**。

**WAL 技术指的是， MySQL 的写操作并不是立刻写到磁盘上，而是先写日志，然后在合适的时间再写到磁盘上**。

过程如下图：

![img](https://cdn.xiaolincoding.com//picgo/1717920899043-30125c0d-bd83-4ca7-9784-07b70c362168.png)

redo log 是物理日志，记录了某个数据页做了什么修改，比如**对 XXX 表空间中的 YYY 数据页 ZZZ 偏移量的地方做了AAA 更新**，每当执行一个事务就会产生这样的一条或者多条物理日志。

在事务提交时，只要先将 redo log 持久化到磁盘即可，可以不需要等到将缓存在 Buffer Pool 里的脏页数据持久化到磁盘。

当系统崩溃时，虽然脏页数据没有持久化，但是 redo log 已经持久化，接着 MySQL 重启后，可以根据 redo log 的内容，将所有数据恢复到最新的状态。

redo log 和 undo log 这两种日志是属于 InnoDB 存储引擎的日志，它们的区别在于：

- redo log 记录了此次事务「**完成后**」的数据状态，记录的是更新**之后**的值；
- undo log 记录了此次事务「**开始前**」的数据状态，记录的是更新**之前**的值；

事务提交之前发生了崩溃，重启后会通过 undo log 回滚事务，事务提交之后发生了崩溃，重启后会通过 redo log 恢复事务，如下图：

![img](https://cdn.xiaolincoding.com//picgo/1717920935536-45ceca35-c79c-48eb-a240-96d580e399b5.png)

所以有了 redo log，再通过 WAL 技术，InnoDB 就可以保证即使数据库发生异常重启，之前已提交的记录都不会丢失，这个能力称为 **crash-safe**（崩溃恢复）。可以看出来， **<font color=24b3>redo log 保证了事务四大特性中的持久性</font>**。

写入 redo log 的方式使用了追加操作， 所以磁盘操作是**顺序写**，而写入数据需要先找到写入位置，然后才写到磁盘，所以磁盘操作是**随机写**。

磁盘的「顺序写 」比「随机写」 高效的多，因此 redo log 写入磁盘的开销更小。

<font color=24b3>可以说这是 WAL 技术的另外一个优点：**MySQL 的写操作从磁盘的「随机写」变成了「顺序写」**，提升语句的执行性能。这是因为 MySQL 的写操作并不是立刻更新到磁盘上，而是先记录在日志上，然后在合适的时间再更新到磁盘上 。</font>

至此， 针对为什么需要 redo log 这个问题我们有两个答案：

- <font color=blue>**实现事务的持久性，让 MySQL 有 crash-safe 的能力**</font>，能够保证 MySQL 在任何时间段突然崩溃，重启后之前已提交的记录都不会丢失；
- <font color=blue>**将写操作从「随机写」变成了「顺序写」**</font>，提升 MySQL 写入磁盘的性能。

## #能不能只用 binlog 不用 relolog？

不行，binlog 是 server 层的日志，没办法记录哪些脏页还没有刷盘，redolog 是存储引擎层的日志，可以记录哪些脏页还没有刷盘，这样崩溃恢复的时候，就能恢复那些还没有被刷盘的脏页数据。

## #为什么要写 RedoLog，而不是直接写到 B+ 树里面？

因为 redolog 写入磁盘是顺序写，而 b+ 树里数据页写入磁盘是随机写，顺序写的性能会比随机写好，这样可以提升事务提交的效率。

最重要的是 redolog 具备故障恢复的能力，Redo Log 记录的是物理级别的修改，包括页的修改，如插入、更新、删除操作在磁盘上的物理位置和修改内容。例如，当执行一个更新操作时，Redo Log 会记录修改的数据页的地址和更新后的数据，而不是 SQL 语句本身。

在数据页实际更新之前，先将修改操作写入 Redo Log。当数据库重启时，会进行恢复操作。首先，根据 Redo Log 检查哪些事务已经提交但数据页尚未完全写入磁盘。然后，使用 Redo Log 中的记录对这些事务进行重做（Redo）操作，将未完成的数据页修改完成，确保事务的修改生效。

## #RedoLog 是在内存里吗？

事务执行过程中，生成的 redolog 会在 redolog buffer 中，也就是在内存中，等事务提交的时候，会把 redolog 写入磁盘。

## #redo log、undo log、bin log 文件格式的区别？

**1. Redo Log（重做日志）：底层物理格式，循环写入**

- **记录格式：** 它是“物理日志”（InnoDB 引擎专属）。它根本不关心你写了什么花里胡哨的 SQL，它只记录极其底层的物理改动。比如：“在第 X 号表空间的第 Y 个数据页的第 Z 个字节处，把值改成了 1”。
- **文件特征：** 大小固定，**循环写入**。你可以把它想象成一个环形的跑道，写满一圈就会覆盖最老的数据。
- **核心使命：** 崩溃恢复。突然断电了？没关系，重启后按着它重新写一遍，数据绝对丢不了。

**2. Undo Log（回滚日志）：反向逻辑格式，段式管理**

- **记录格式：** 它是“逻辑日志”**（InnoDB 引擎专属）。它记录的是你当前操作的**“反向动作”。你执行 `INSERT`，它就悄悄记一条 `DELETE`；你把名字从 A 改成了 B，它就记一条把 B 改回 A 的 `UPDATE`。
- **文件特征：** 并不像普通文件那样按行追加，而是以数据页的“段（Segment）”形式存放在独立的 Undo 表空间里。事务一旦提交，且没有其他查询需要用到它（比如 MVCC 的快照读），这些空间就会被系统回收清理。
- **核心使命：** 事务回滚（吃后悔药）和 MVCC（给别人看老版本数据）。

**3. Binlog（归档日志）：正向逻辑格式，无限追加**

- **记录格式：** 它也是“逻辑日志”（属于 MySQL Server 层的统管日志，什么引擎都有）。它记录的是引起数据变动的“正向操作”。实战中最常用的 `ROW` 格式，会事无巨细地记录每一行数据修改前和修改后的完整内容。
- **文件特征：** 它是**追加写入**的。写满一个文件（比如 1G），就自动开辟下一个新文件，只要你不手动清理，它绝不会覆盖历史数据。
- **核心使命：** 主从同步（从库拿它重新播放一遍）和 数据找回（比如手滑删库了，可以用它把数据一盘盘录像带放回来）。

## #mysql 崩溃后的一致性

MySQL 崩溃后还能保证数据一致性（特别是 InnoDB 引擎），全靠底层设计的“三板斧”。用大白话来说，就是“防丢”、“防错”和“防写破”。

**<font color=blue>1. Redo Log（重做日志）：保证已提交的数据不丢</font>** MySQL 修改数据时，为了快，是先在内存（Buffer Pool）里改，后台再慢慢刷到磁盘。为了防止中途断电导致内存数据丢失，它采用了 WAL (Write-Ahead Logging) 机制。 简单说，就是真正改磁盘数据前，先飞速地把这次**修改操作**顺序记录到磁盘的 Redo Log 里。只要 Redo Log 成功落盘，就算 MySQL 秒崩，重启后也能照着日志把操作“重做”一遍，保证提交成功的事务绝不丢失。

**<font color=blue>2. Undo Log（回滚日志）：保证未提交的半成品不留</font>** 如果一个事务执行到一半，还没来得及 `commit` 系统就崩了，这些修改了一半的数据怎么办？ 这时候 Undo Log 就上场了。它每次都会记录数据的“老版本”（比如你把 A 改成 B，它就记下 A）。崩溃重启后，MySQL 一查发现这个事务没提交，就会掏出 Undo Log 把数据“撤销”回原来的样子，保证要么全做，要么全不做。

**<font color=blue>3. Doublewrite Buffer（双写缓冲）：防止底层数据页写坏</font>** 这是极其关键的兜底机制。MySQL 往磁盘写数据是以 16KB（一个数据页）为单位的，但操作系统写磁盘往往是 4KB 一批。如果 MySQL 刚好写了 8KB 的时候突然断电，这个数据页就坏了（这叫“撕裂页”）。数据页一旦物理损坏，连 Redo Log 都救不回来。 所以，MySQL 每次写数据页之前，会先把这 16KB 完整地写到磁盘上的一个连续安全区（Doublewrite Buffer）。如果写数据页中途崩了，重启后直接从这个安全区把完好的副本拷回来恢复。

> **1. 为什么“连 Redo Log 都救不回来”？** Redo Log（重做日志）记录的是“物理修改”，比如它的指令是：“*在第 10 号数据页的偏移量 100 处，把值 A 改成 B*”。 当数据库重启准备通过 Redo Log 恢复时，它必须要先去读那个第 10 号数据页的原文件。如果这个页发生了“撕裂（只写了 8KB）”，它的文件头部信息或者 Checksum（校验和）就已经完全错乱了。MySQL 读到这个页时，会认为这是一个根本不合法的“坏页”，直接拒绝读取。皮之不存，毛将焉附？基础的数据页都无法识别，Redo Log 里的修改指令自然也就无从下手了。
>
> **2. 既然写两遍，性能不会大打折扣吗？** 这也是很多人刚接触双写时的疑问。既然怕丢，全写到安全区不就行了？ 实际上，写 Doublewrite Buffer 是**顺序写（Sequential I/O）**，速度极快；而把数据真正写回到对应的 ibd 数据文件里，是**随机写（Random I/O）**，速度较慢。 操作系统非常擅长处理顺序写，所以多写这一遍连续的 16KB，对整体性能的损耗通常只有百分之几（大约 5% - 10%），完全在可接受范围内。用这点微小的性能损耗换取“数据绝对不坏”的底线，是一笔极其划算的买卖。
>
> ----
>
> 哪怕是在写 Doublewrite Buffer（双写缓冲）的半路上服务器直接物理断电，数据依然能完美恢复。
>
> 要理解这套“防弹衣”是怎么起作用的，得先弄清楚写数据的动作其实分了**两步**。
>
> 每次 MySQL 要把内存里的“脏页”（修改过的数据）刷到磁盘时，它不会直接去写原本的数据文件，而是：
>
> 1. **第一步**：先把这批数据**顺序写入**磁盘上的 Doublewrite 文件（系统表空间）。
> 2. **第二步**：等第一步彻底写完并落盘后，再把这批数据**随机写入**磁盘上真正的数据页（Data Page）位置。
>
> 既然分了两步，我们来看看如果在这两步中发生崩溃，MySQL 是怎么自救的：
>
> ### 崩溃场景 1：在“第一步”写 Doublewrite 磁盘文件时断电了
>
> 这时候，磁盘上的 Doublewrite 数据确实坏了（发生了撕裂，只写了一半）。
>
> - **底层真相**：因为还在执行第一步，**真正的数据页压根还没被动过**，它依然是完好无损的“旧数据”。
> - **如何恢复**：MySQL 重启时，发现 Doublewrite 里的页校验不通过（坏了），它会直接丢弃这个半成品。然后去读取那个完好无损的旧数据页，接着掏出 **Redo Log**，把崩溃前的操作重新执行一遍。数据完美恢复！
>
> ### 崩溃场景 2：在“第二步”写真正的数据页时断电了
>
> 这就是最经典的“数据撕裂”场景。真正的数据页写了一半，坏掉了。
>
> - **底层真相**：因为能进入第二步，说明**第一步已经成功了**！也就是说，Doublewrite 文件里已经静静躺着一份完整无缺的、最新的数据页副本。
> - **如何恢复**：MySQL 重启时，发现真正的数据页损坏了，它直接从 Doublewrite 里把那份完整的副本“拷贝”过来，强行覆盖掉损坏的数据页。把物理坑填平后，再走一遍 Redo Log 和 Undo Log。数据再次完美恢复！

<font color=24b3aa>总而言之，MySQL 崩溃重启时，先用 **Doublewrite Buffer** 修复物理损坏的数据页，接着用 **Redo Log** 把崩溃前所有的操作统统恢复到内存里，最后再用 **Undo Log** 把那些没提交的半拉子事务全部回滚掉。这样账本就彻底平了。</font>

# [索引]索引分类

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E7%B4%A2%E5%BC%95%E6%8F%90%E7%BA%B2.png)

## [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#什么是索引)什么是索引？

当你想查阅书中某个知识的内容，你会选择一页一页的找呢？还是在书的目录去找呢？

傻瓜都知道时间是宝贵的，当然是选择在书的目录去找，找到后再翻到对应的页。书中的**目录**，就是充当 **索引** 的角色，方便我们快速查找书中的内容，所以索引是以空间换时间的设计思想。

那换到数据库中，索引的定义就是帮助存储引擎快速获取数据的一种数据结构，形象的说就是 **索引是数据的目录**。

<font color=24b3aa>所谓的存储引擎，说白了就是如何存储数据、如何为存储的数据建立索引和如何更新、查询数据等技术的实现方法。</font>MySQL 存储引擎有 MyISAM 、InnoDB、Memory，其中 InnoDB 是在 MySQL 5.5 之后成为默认的存储引擎。

下图是 MySQL 的结构图，索引和数据就是位于存储引擎中：

![img](https://cdn.xiaolincoding.com//mysql/other/1623727651911_20170928110355446.png)

## [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#索引的分类)索引的分类

你知道索引有哪些吗？大家肯定都能霹雳啪啦地说出聚簇索引、主键索引、二级索引、普通索引、唯一索引、hash索引、B+ 树索引等等。

然后再问你，你能将这些索引分一下类吗？可能大家就有点模糊了。其实，要对这些索引进行分类，要清楚这些索引的使用和实现方式，然后再针对有相同特点的索引归为一类。

我们可以按照四个角度来分类索引。

- <font color=24b3aa>按**「数据结构」**分类：**B+ tree索引、Hash 索引、Full-text 索引**。</font>
- <font color=24b3aa>按**「物理存储」**分类：**聚簇索引（主键索引）、二级索引（辅助索引）**。</font>
- <font color=24b3aa>按**「字段特性」**分类：**主键索引、唯一索引、普通索引、前缀索引**。</font>
- <font color=24b3aa>按**「字段个数」**分类：**单列索引、联合索引（复合索引）**。</font>

接下来，按照这些角度来说说各类索引的特点。

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#按数据结构分类)按数据结构分类

从数据结构的角度来看，MySQL 常见索引有 B+Tree 索引、HASH 索引、Full-Text 索引。

每一种存储引擎支持的索引类型不一定相同，我在表中总结了 MySQL 常见的存储引擎 InnoDB、MyISAM 和 Memory 分别支持的索引类型。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E7%B4%A2%E5%BC%95%E5%88%86%E7%B1%BB.drawio.png)

InnoDB 是在 MySQL 5.5 之后成为默认的 MySQL 存储引擎，B+Tree 索引类型也是 MySQL 存储引擎采用最多的索引类型。

<font color=blue>在创建表时，InnoDB 存储引擎会根据不同的场景选择不同的列作为聚簇（主键）索引索引的索引键：</font>

- 如果有主键，默认会使用主键作为聚簇索引的索引键（key）；
- 如果没有主键，就选择第一个不包含 NULL 值的唯一列作为聚簇索引的索引键（key）；
- 在上面两个都没有的情况下，InnoDB 将自动生成一个隐式自增 id（row_id）列作为聚簇索引的索引键（key）；

<font color=blue>其它索引都属于辅助索引（Secondary Index），也被称为二级索引或非聚簇索引。</font>**<font color=24b3aa>创建的主键索引和二级索引默认使用的是 B+ Tree 索引</font>**。

为了让大家理解 B+Tree 索引的存储和查询的过程，接下来我通过一个简单例子，说明一下 B+Tree 索引在存储数据中的具体实现。

先创建一张商品表，id 为主键，如下：

```sql
CREATE TABLE `product`  (
  `id` int(11) NOT NULL,
  `product_no` varchar(20)  DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `price` decimal(10, 2) DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Dynamic;
```

商品表里，有这些行数据：

![img](https://cdn.xiaolincoding.com//mysql/other/824c43b801c64e81acb0a9b042d50311.png)

这些行数据，存储在 B+Tree 索引时是长什么样子的？

<font color=24b3aa>B+Tree 是一种多叉树，叶子节点才存放数据，非叶子节点只存放索引，而且每个节点里的数据是 **按主键顺序存放** 的。每一层父节点的索引值都会出现在下层子节点的索引值中，因此在叶子节点中，包括了所有的索引值信息，并且每一个叶子节点都有两个指针，分别指向下一个叶子节点和上一个叶子节点，形成一个双向链表。</font>

主键索引的 B+Tree 如图所示（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行）：

![主键索引 B+Tree](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/btree.drawio.png)

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#通过主键查询商品数据的过程)通过主键查询商品数据的过程

比如，我们执行了下面这条查询语句：

```sql
SELECT * FROM product WHERE id= 5;
```

这条语句使用了主键索引查询 id 号为 5 的商品。查询过程是这样的，B+Tree 会自顶向下逐层进行查找：

- 将 5 与根节点的索引数据 (1，10，20) 比较，5 在 1 和 10 之间，所以根据 B+Tree 的搜索逻辑，找到第二层的索引数据 (1，4，7)；
- 在第二层的索引数据 (1，4，7)中进行查找，因为 5 在 4 和 7 之间，所以找到第三层的索引数据（4，5，6）；
- 在叶子节点的索引数据（4，5，6）中进行查找，然后我们找到了索引值为 5 的行数据。

数据库的索引和数据都是存储在硬盘的，我们可以把读取一个节点当作一次磁盘 I/O 操作。那么上面的整个查询过程一共经历了 3 个节点，也就是进行了 3 次 I/O 操作。

B+Tree 存储千万级的数据只需要 3-4 层高度就可以满足（但节点存储 1000 个数据），这意味着从千万级的表查询目标数据最多需要 3-4 次磁盘 I/O，所以 **B+Tree 相比于 B 树和二叉树来说，最大的优势在于查询效率很高，因为即使在数据量很大的情况，查询一个数据的磁盘 I/O 依然维持在 3-4次。**

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#通过二级索引查询商品数据的过程)通过二级索引查询商品数据的过程

主键索引的 B+Tree 和二级索引的 B+Tree 区别如下：

- <font color=24b3aa>主键索引的 B+Tree 的叶子节点存放的是 **实际数据**，所有完整的用户记录都存放在主键索引的 B+Tree 的叶子节点里；</font>
- <font color=24b3aa>二级索引的 B+Tree 的叶子节点存放的是 **主键值**，而不是实际数据。</font>

我这里将前面的商品表中的 product_no （商品编码）字段设置为二级索引，那么二级索引的 B+Tree 如下图（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行）。

![二级索引 B+Tree](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E4%BA%8C%E7%BA%A7%E7%B4%A2%E5%BC%95btree.drawio.png)

其中非叶子的 key 值是 product_no（图中橙色部分），叶子节点存储的数据是主键值（图中绿色部分）。

如果我用 product_no 二级索引查询商品，如下查询语句：

```sql
SELECT * FROM product WHERE product_no = '0002';
```

会先检二级索引中的 B+Tree 的索引值（商品编码，product_no），找到对应的叶子节点，然后获取主键值，然后再通过主键索引中的 B+Tree 树查询到对应的叶子节点，然后获取整行数据。**这个过程叫<font color=red>「回表」</font>，也就是说要查两个 B+Tree 才能查到数据**。如下图（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行）：

![回表](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E5%9B%9E%E8%A1%A8.drawio.png)

<font color=24b3aa>不过，当查询的数据是能在二级索引的 B+Tree 的叶子节点里查询到，这时就不用再查主键索引查，比如下面这条查询语句：</font>

```sql
SELECT id FROM product WHERE product_no = '0002';
```

**这种在二级索引的 B+Tree 就能查询到结果的过程就叫作<font color=red>「覆盖索引」</font>，也就是只需要查一个 B+ Tree 就能找到数据**。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#为什么-mysql-innodb-选择-b-tree-作为索引的数据结构)为什么 MySQL InnoDB 选择 B+tree 作为索引的数据结构？

前面已经讲了 B+Tree 的索引原理，现在就来回答一下 B+Tree 相比于 B 树、二叉树或 Hash 索引结构的优势在哪儿？

之前我也专门写过一篇文章，想详细了解的可以看这篇：「[女朋友问我：为什么 MySQL 喜欢 B+ 树？我笑着画了 20 张图 (opens new window)](https://mp.weixin.qq.com/s/w1ZFOug8-Sa7ThtMnlaUtQ)」，这里就简单做个比对。

***1、B+Tree vs B Tree***

<u>B+Tree 只在叶子节点存储数据</u>，而 B 树的非叶子节点也要存储数据，所以 B+Tree 的单个节点的数据量更小，在相同的磁盘 I/O 次数下，就能查询更多的节点。

另外，<u>B+Tree 叶子节点采用的是双链表连接</u>，适合 MySQL 中常见的基于范围的顺序查找，而 B 树无法做到这一点。

***2、B+Tree vs 二叉树***

对于有 N 个叶子节点的 B+Tree，其搜索复杂度为 $O(log_d^N)$，其中 d 表示节点允许的最大子节点个数为 d 个。

<u>在实际的应用当中， d 值是大于100的，这样就保证了，即使数据达到千万级别时，B+Tree 的高度依然维持在 3~4 层左右</u>，也就是说一次数据查询操作只需要做 3~4 次的磁盘 I/O 操作就能查询到目标数据。

而二叉树的每个父节点的儿子节点个数只能是 2 个，意味着其搜索复杂度为 $O(log_2^N)$，这已经比 B+Tree 高出不少，因此二叉树检索到目标数据所经历的磁盘 I/O 次数要更多。

***3、B+Tree vs Hash***

Hash 在做等值查询的时候效率贼快，搜索复杂度为 O(1)。

但是 <u>Hash 表 不适合做范围查询</u>，它更适合做等值的查询，这也是 B+Tree 索引要比 Hash 表索引有着更广泛的适用场景的原因。

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#按物理存储分类)按物理存储分类

从物理存储的角度来看，索引分为聚簇索引（主键索引）、二级索引（辅助索引）。

在 InnoDB 里：

- 有 `PRIMARY KEY` 时，主键就是聚簇索引。
- **没有 `PRIMARY KEY`** 时，`InnoDB` 会选择**第一个**“所有列都为 `NOT NULL` 的 `UNIQUE` 索引”作为聚簇索引。
- 如果连这样的 `UNIQUE NOT NULL` 索引也没有，`InnoDB` 会自己生成隐藏聚簇索引 `GEN_CLUST_INDEX`（实际就是 `row_id`）。

这两个区别在前面也提到了：

- 主键索引的 B+Tree 的叶子节点存放的是实际数据，所有完整的用户记录都存放在主键索引的 B+Tree 的叶子节点里；
- 二级索引的 B+Tree 的叶子节点存放的是主键值，而不是实际数据。

所以，在查询时使用了二级索引，如果查询的数据能在二级索引里查询的到，那么就不需要回表，这个过程就是 **<font color=blue>覆盖索引</font>**。如果查询的数据不在二级索引里，就会先检索二级索引，找到对应的叶子节点，获取到主键值后，然后再检索主键索引，就能查询到数据了，这个过程就是 **<font color=blue>回表</font>**。

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#按字段特性分类)按字段特性分类

从字段特性的角度来看，索引分为 <font color=red>主键索引、唯一索引、普通索引、前缀索引</font>。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#主键索引)主键索引

主键索引就是建立在主键字段上的索引，通常在创建表的时候一起创建，一张表最多只有一个主键索引，索引列的值不允许有空值。

在创建表时，创建主键索引的方式如下：

```sql
CREATE TABLE table_name  (
  ....
  PRIMARY KEY (index_column_1) USING BTREE
);
```

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#唯一索引)唯一索引

<font color=24b3aa>唯一索引建立在 UNIQUE 字段上的索引，一张表可以有多个唯一索引</font>，<font color=blue>**索引列的值必须唯一，但是允许有空值**。</font>

> **允许空值：** 这是唯一索引与主键索引最大的区别。在大多数主流数据库（如 MySQL、PostgreSQL、SQL Server）中，唯一索引列允许包含 `NULL` 值。
>
> <font color=red>在关系型数据库理论中，`NULL` 代表 **“未知”** 或 **“缺失”** 的值，它不是一个具体的数值。</font>
>
> - **比较运算：** `NULL = NULL` 的结果既不是 `TRUE` 也不是 `FALSE`，而是 **`UNKNOWN`**。
> - **唯一性判断：** 唯一约束检查的是“是否存在两行数据使得 `A = A` 为 TRUE”。既然 `NULL = NULL` 永远不为 TRUE，数据库就无法断定两个 `NULL` 是“相同”的，因此不会将其视为重复。
>
> *注意：* 对于 `NULL` 的唯一性判断，不同数据库行为略有差异。例如在 MySQL 和 PostgreSQL 中，多个 `NULL` 值被视为“不相等”，因此可以存在多行 `NULL`；而在 SQL Server 的某些旧版本或特定配置下，可能只允许一个 `NULL`。

在创建表时，创建唯一索引的方式如下：

```sql
CREATE TABLE table_name  (
  ....
  UNIQUE KEY(index_column_1,index_column_2,...) 
);
```

建表后，如果要创建唯一索引，可以使用这面这条命令：

```sql
CREATE UNIQUE INDEX index_name
ON table_name(index_column_1,index_column_2,...); 
```

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#普通索引)普通索引

普通索引就是建立在普通字段上的索引，既不要求字段为主键，也不要求字段为 UNIQUE。

在创建表时，创建普通索引的方式如下：

```sql
CREATE TABLE table_name  (
  ....
  INDEX(index_column_1,index_column_2,...) 
);
```

建表后，如果要创建普通索引，可以使用这面这条命令：

```sql
CREATE INDEX index_name
ON table_name(index_column_1,index_column_2,...); 
```

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#前缀索引)前缀索引

前缀索引是指对字符类型字段的前几个字符建立的索引，而不是在整个字段上建立的索引，<font color=blue>前缀索引可以建立在字段类型为 char、 varchar、binary、varbinary 的列上。</font>

**<font color=24b3aa>使用前缀索引的目的是为了减少索引占用的存储空间，提升查询效率。</font>**也就是在索引效果差不多的前提下，减少索引的体积。

在创建表时，创建前缀索引的方式如下：

```sql
CREATE TABLE table_name(
    column_list,
    INDEX(column_name(length))
); 
```

建表后，如果要创建前缀索引，可以使用这面这条命令：

```sql
CREATE INDEX index_name
ON table_name(column_name(length)); 
```

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#按字段个数分类)按字段个数分类

从字段个数的角度来看，索引分为单列索引、联合索引（复合索引）。

- 建立在单列上的索引称为单列索引，比如主键索引；
- 建立在多列上的索引称为联合索引；

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#联合索引)联合索引

通过将多个字段组合成一个索引，该索引就被称为联合索引。

比如，将商品表中的 product_no 和 name 字段组合成联合索引`(product_no, name)`，创建联合索引的方式如下：

```sql
CREATE INDEX index_product_no_name ON product(product_no, name);
```

联合索引 `(product_no, name)` 的 B+Tree 示意图如下（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行）。

![联合索引](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E8%81%94%E5%90%88%E7%B4%A2%E5%BC%95.drawio.png)

可以看到，联合索引的非叶子节点用两个字段的值作为 B+Tree 的 key 值。<font color=24b3aa>当在联合索引查询数据时，key 是按字段在索引中出现的顺序联合排序的，也就是**字典序**。</font>在这里，先按 product_no 字段比较，在 product_no 相同的情况下再按 name 字段比较。

因此，使用联合索引时，存在 **<font color=blue>最左匹配原则</font>**，也就是按照最左优先的方式进行索引的匹配。在使用联合索引进行查询的时候，如果不遵循「最左匹配原则」，联合索引会失效，这样就无法利用到索引快速查询的特性了。

<font color=24b3aa>使用联合索引查询时，不需要指定联合索引中的每一个字段，但必须满足“最左原则”。</font>

比如，如果创建了一个 `(a, b, c)` 联合索引，如果查询条件是以下这几种，就可以匹配上联合索引：

- WHERE a=1；
- WHERE a=1 and b=2 and c=3；
- WHERE a=1 and b=2；

<font color=24b3aa>需要注意的是，因为有查询优化器，所以 a 字段在 WHERE 子句的顺序并不重要。</font>

但是，如果查询条件是以下这几种，因为不符合最左匹配原则，所以就无法匹配上联合索引，联合索引就会失效:

- WHERE b=2；
- WHERE c=3；
- WHERE b=2 and c=3；

上面这些查询条件之所以会失效，是因为`(a, b, c)` 联合索引，是先按 a 排序，在 a 相同的情况再按 b 排序，在 b 相同的情况再按 c 排序。所以，**<font color=24b3aa>b 和 c 是全局无序，局部相对有序的</font>**，这样在没有遵循最左匹配原则的情况下，是无法利用到索引的。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#联合索引范围查询)联合索引范围查询

<font color=24b3aa>联合索引有一些特殊情况，**并不是查询过程使用了联合索引查询，就代表联合索引中的所有字段都用到了联合索引进行索引查询**，也就是可能存在部分字段用到联合索引的 B+Tree，部分字段没有用到联合索引的 B+Tree 的情况。</font>

例如：<font color=blue>范围条件会“截断”后面的列。</font>

这种特殊情况就发生在范围查询。联合索引的最左匹配原则会一直向右匹配直到遇到「范围查询」就会停止匹配。**也就是范围查询的字段可以用到联合索引，但是在范围查询字段的后面的字段无法用到联合索引**。

范围查询有很多种，那到底是哪些范围查询会导致联合索引的最左匹配原则会停止匹配呢？

接下来，举例几个范围查例子。

> Q1: `SELECT * FROM t_table WHERE a > 1 and b = 2`，联合索引（a, b）哪一个字段用到了联合索引的 B+Tree？

由于联合索引（二级索引）是先按照 a 字段的值排序的，所以符合 a > 1 条件的二级索引记录肯定是相邻，于是在进行索引扫描的时候，可以定位到符合 a > 1 条件的第一条记录，然后沿着记录所在的链表向后扫描，直到某条记录不符合 a > 1 条件位置。所以 a 字段可以在联合索引的 B+Tree 中进行索引查询。

**但是在符合 a > 1 条件的二级索引记录的范围里，b 字段的值是无序的**。比如前面图的联合索引的 B+ Tree 里，下面这三条记录的 a 字段的值都符合 a > 1 查询条件，而 b 字段的值是无序的：

- a 字段值为 5 的记录，该记录的 b 字段值为 8；
- a 字段值为 6 的记录，该记录的 b 字段值为 10；
- a 字段值为 7 的记录，该记录的 b 字段值为 5；

因此，我们不能根据查询条件 b = 2 来进一步减少需要扫描的记录数量（b 字段无法利用联合索引进行索引查询的意思）。

所以在执行 Q1 这条查询语句的时候，对应的扫描区间是 (2, + ∞)，形成该扫描区间的边界条件是 a > 1，与 b = 2 无关。

因此，**Q1 这条查询语句只有 a 字段用到了联合索引进行索引查询，而 b 字段并没有使用到联合索引**。

我们也可以在执行计划中的 key_len 知道这一点，在使用联合索引进行查询的时候，通过 key_len 我们可以知道优化器具体使用了多少个字段的搜索条件来形成扫描区间的边界条件。

举例个例子 ，a 和 b 都是 int 类型且不为 NULL 的字段，那么 Q1 这条查询语句执行计划如下，可以看到 key_len 为 4 字节（如果字段允许为 NULL，就在字段类型占用的字节数上加 1，也就是 5 字节），说明只有 a 字段用到了联合索引进行索引查询，而且可以看到，即使 b 字段没用到联合索引，key 为 idx_a_b，说明 Q1 查询语句使用了 idx_a_b 联合索引。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/q1.png)

通过 Q1 查询语句我们可以知道，a 字段使用了 > 进行范围查询，联合索引的最左匹配原则在遇到 a 字段的范围查询（ >）后就停止匹配了，因此 b 字段并没有使用到联合索引。

> Q2: `SELECT * FROM t_table WHERE a >= 1 and b = 2`，联合索引（a, b）哪一个字段用到了联合索引的 B+Tree？

Q2 和 Q1 的查询语句很像，唯一的区别就是 a 字段的查询条件「大于等于」。

由于联合索引（二级索引）是先按照 a 字段的值排序的，所以符合 >= 1 条件的二级索引记录肯定是相邻，于是在进行索引扫描的时候，可以定位到符合 >= 1 条件的第一条记录，然后沿着记录所在的链表向后扫描，直到某条记录不符合 a>= 1 条件位置。所以 a 字段可以在联合索引的 B+Tree 中进行索引查询。

虽然在符合 a>= 1 条件的二级索引记录的范围里，b 字段的值是「无序」的，**但是对于符合 a = 1 的二级索引记录的范围里，b 字段的值是「有序」的**（因为对于联合索引，是先按照 a 字段的值排序，然后在 a 字段的值相同的情况下，再按照 b 字段的值进行排序）。

于是，在确定需要扫描的二级索引的范围时，当二级索引记录的 a 字段值为 1 时，可以通过 b = 2 条件减少需要扫描的二级索引记录范围（b 字段可以利用联合索引进行索引查询的意思）。也就是说，从符合 a = 1 and b = 2 条件的第一条记录开始扫描，而不需要从第一个 a 字段值为 1 的记录开始扫描。

所以，**Q2 这条查询语句 a 和 b 字段都用到了联合索引进行索引查询**。

我们也可以在执行计划中的 key_len 知道这一点。执行计划如下，可以看到 key_len 为 8 字节，说明优化器使用了 2 个字段的查询条件来形成扫描区间的边界条件，也就是 a 和 b 字段都用到了联合索引进行索引查询。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/q2.png)

通过 Q2 查询语句我们可以知道，虽然 a 字段使用了 >= 进行范围查询，但是联合索引的最左匹配原则并没有在遇到 a 字段的范围查询（ >=）后就停止匹配了，b 字段还是可以用到了联合索引的。

> Q3: `SELECT * FROM t_table WHERE a BETWEEN 2 AND 8 AND b = 2`，联合索引（a, b）哪一个字段用到了联合索引的 B+Tree？

Q3 查询条件中 `a BETWEEN 2 AND 8` 的意思是查询 a 字段的值在 2 和 8 之间的记录。<font color=ee11ff>不同的数据库对 BETWEEN ... AND 处理方式是有差异的。在 MySQL 中，BETWEEN 包含了 value1 和 value2 边界值，类似于 >= and =<。</font>而有的数据库则不包含 value1 和 value2 边界值（类似于 > and <）。

这里我们只讨论 MySQL。由于 MySQL 的 BETWEEN 包含 value1 和 value2 边界值，所以类似于 Q2 查询语句，因此 **Q3 这条查询语句 a 和 b 字段都用到了联合索引进行索引查询**。

我们也可以在执行计划中的 key_len 知道这一点。执行计划如下，可以看到 key_len 为 8 字节，说明优化器使用了 2 个字段的查询条件来形成扫描区间的边界条件，也就是 a 和 b 字段都用到了联合索引进行索引查询。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/q3.png)

通过 Q3 查询语句我们可以知道，虽然 a 字段使用了 BETWEEN 进行范围查询，但是联合索引的最左匹配原则并没有在遇到 a 字段的范围查询（ BETWEEN）后就停止匹配了，b 字段还是可以用到了联合索引的。

> Q4: `SELECT * FROM t_user WHERE name like 'j%' and age = 22`，联合索引（name, age）哪一个字段用到了联合索引的 B+Tree？

由于联合索引（二级索引）是先按照 name 字段的值排序的，所以前缀为 ‘j’ 的 name 字段的二级索引记录都是相邻的， 于是在进行索引扫描的时候，可以定位到符合前缀为 ‘j’ 的 name 字段的第一条记录，然后沿着记录所在的链表向后扫描，直到某条记录的 name 前缀不为 ‘j’ 为止。

所以 a 字段可以在联合索引的 B+Tree 中进行索引查询，形成的扫描区间是['j','k')。注意， j 是闭区间。如下图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/q4-1.drawio.png)

虽然在符合前缀为 ‘j’ 的 name 字段的二级索引记录的范围里，age 字段的值是「无序」的，**但是对于符合 name = j 的二级索引记录的范围里，age字段的值是「有序」的**（因为对于联合索引，是先按照 name 字段的值排序，然后在 name 字段的值相同的情况下，再按照 age 字段的值进行排序）。

于是，在确定需要扫描的二级索引的范围时，当二级索引记录的 name 字段值为 ‘j’ 时，可以通过 age = 22 条件减少需要扫描的二级索引记录范围（age 字段可以利用联合索引进行索引查询的意思）。也就是说，从符合 `name = 'j' and age = 22` 条件的第一条记录时开始扫描，而不需要从第一个 name 为 j 的记录开始扫描 。如下图的右边：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/q4-2.drawio.png)

所以，**Q4 这条查询语句 a 和 b 字段都用到了联合索引进行索引查询**。

我们也可以在执行计划中的 key_len 知道这一点。本次例子中：

- name 字段的类型是 varchar(30) 且不为 NULL，数据库表使用了 utf8mb4 字符集，一个字符集为 utf8mb4 的字符是 4 个字节，因此 name 字段的实际数据最多占用的存储空间长度是 120 字节（30 x 4），然后因为 name 是变长类型的字段，需要再加 2 字节（用于存储该字段实际数据的长度值），也就是 name 的 key_len 为 122。
- age 字段的类型是 int 且不为 NULL，key_len 为 4。

TIP

可能有的同学对于「因为 name 是变长类型的字段，需要再加 2 字节」这句话有疑问。之前这篇[文章 (opens new window)](https://xiaolincoding.com/mysql/base/row_format.html)说「如果变长字段允许存储的最大字节数小于等于 255 字节，就会用 1 字节表示变长字段的长度」，而这里为什么是 2 字节？

<font color=24b3aa>key_len 的显示比较特殊，行格式是由 innodb 存储引擎实现的，而执行计划是在 server 层生成的，所以它不会去问 innodb 存储引擎可变字段的长度占用多少字节，而是不管三七二十一都使用 2 字节表示可变字段的长度。</font>

毕竟 key_len 的目的只是为了告诉你索引查询中用了哪些索引字段，而不是为了准确告诉这个字段占用多少字节空间。

Q4 查询语句的执行计划如下，可以看到 key_len 为 126 字节，name 的 key_len 为 122，age 的 key_len 为 4，说明优化器使用了 2 个字段的查询条件来形成扫描区间的边界条件，也就是 name 和 age 字段都用到了联合索引进行索引查询。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/q4-%E6%89%A7%E8%A1%8C%E8%AE%A1%E5%88%92.png)

通过 Q4 查询语句我们可以知道，虽然 name 字段使用了 like 前缀匹配进行范围查询，但是联合索引的最左匹配原则并没有在遇到 name 字段的范围查询（ like 'j%'）后就停止匹配了，age 字段还是可以用到了联合索引的。

综上所示，**<font color=24b3aa>联合索引的最左匹配原则，在遇到范围查询（如 >、<）的时候，就会停止匹配，也就是范围查询的字段可以用到联合索引，但是在范围查询字段的后面的字段无法用到联合索引。注意，对于 >=、<=、BETWEEN、like 前缀匹配的范围查询，并不会停止匹配，前面我也用了四个例子说明了</font>**。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#索引下推)索引下推

现在我们知道，对于联合索引（a, b），在执行 `SELECT * FROM table WHERE a > 1 and b = 2` 语句的时候，只有 a 字段能用到索引，那在联合索引的 B+Tree 找到第一个满足条件的主键值（ID 为 2）后，还需要判断其他条件是否满足（看 b 是否等于 2），那是在联合索引里判断？还是回主键索引去判断呢？

- 在 MySQL 5.6 之前，只能从 ID2 （主键值）开始一个个回表，到「主键索引」上找出数据行，再对比 b 字段值。
- 而 MySQL 5.6 引入的 <font color=red>**索引下推优化**（index condition pushdown)</font>， <font color=24b3aa>**可以在联合索引遍历过程中，对联合索引中包含的字段先做判断，直接过滤掉不满足条件的记录，减少回表次数**。</font>

当你的查询语句的执行计划里，出现了 Extra 为 `Using index condition`，那么说明使用了索引下推的优化。

“索引下推优化”（Index Condition Pushdown，常简称 **ICP**）是一种数据库查询优化手段：**把原本需要到表里再判断的部分 `WHERE` 条件，尽可能“下推”到索引扫描阶段就先过滤掉**，从而减少回表（读整行数据）次数，提高查询性能。

* 索引下推（ICP）的前提是：`WHERE` 条件里有一部分，能够只依赖“索引记录中的列值”在索引扫描阶段完成判断。

很多时候索引只能用于定位“范围”，但 `WHERE` 里还有一些条件不能直接用于定位范围。

例如有复合索引 `(a, b)`：

```sql
SELECT *  FROM t
WHERE a BETWEEN 10 AND 20
AND b LIKE 'xyz%';
```

- **没有 ICP**：先用索引 `(a,b)` 做范围扫描找到 `a` 在 10..20 的索引项 → **每条都回表取整行** → 再判断 `b LIKE 'xyz%'` 是否满足。
- **有 ICP**：仍然做 `a` 的范围扫描，但在读取索引项时，**直接用索引里就能拿到的 `b` 值先判断 `b LIKE 'xyz%'`**，满足的才回表取整行。⇒ 回表次数更少，IO 更少。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#索引区分度)索引区分度

另外，建立联合索引时的字段顺序，对索引效率也有很大影响。越靠前的字段被用于索引过滤的概率越高，实际开发工作中 **<font color=24b3aa>建立联合索引时，要把区分度大的字段排在前面，这样区分度大的字段越有可能被更多的 SQL 使用到</font>**。

区分度就是某个字段 column 不同值的个数「除以」表的总行数，计算公式如下：

![区分度计算公式](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E5%8C%BA%E5%88%86%E5%BA%A6.png)

比如，性别的区分度就很小，不适合建立索引或不适合排在联合索引列的靠前的位置，而 UUID 这类字段就比较适合做索引或排在联合索引列的靠前的位置。

<font color=24b3aa>因为如果索引的区分度很小，假设字段的值分布均匀，那么无论搜索哪个值都可能得到一半的数据。在这些情况下，还不如不要索引，因为 MySQL 还有一个查询优化器，查询优化器发现某个值出现在表的数据行中的百分比（惯用的百分比界线是"30%"）很高的时候，它一般会忽略索引，进行全表扫描。</font>

- **回表成本**：MySQL 的二级索引（非聚簇索引）叶子节点存储的是主键值。当通过二级索引找到记录后，如果需要查询的列不在索引中，就必须拿着主键去聚簇索引中再查一次完整行，这叫**回表**。
- **随机 I/O vs 顺序 I/O**：
  - **全表扫描**是顺序 I/O，磁盘预读机制使其效率较高。
  - **索引回表**通常是随机 I/O。如果匹配的数据量很大（如你所说的“一半数据”），大量的随机 I/O 累积起来的耗时，往往远超一次顺序的全表扫描。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#联合索引进行排序)联合索引进行排序

这里出一个题目，针对针对下面这条 SQL，你怎么通过索引来提高查询效率呢？

```sql
SELECT * 
FROM order 
WHERE status = 1 
ORDER BY create_time ASC
```

有的同学会认为，单独给 status 建立一个索引就可以了。

但是更好的方式给 status 和 create_time 列建立一个联合索引，因为这样可以避免 MySQL 数据库发生文件排序。

因为在查询时，如果只用到 status 的索引，但是这条语句还要对 create_time 排序，这时就要用文件排序 filesort，也就是在 SQL 执行计划中，Extra 列会出现 Using filesort。

所以，要利用索引的有序性，在 status 和 create_time 列建立联合索引，这样根据 status 筛选后的数据就是按照 create_time 排好序的，避免在文件排序，提高了查询效率。

# [索引]索引优化

## [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#什么时候需要-不需要创建索引)什么时候需要 / 不需要创建索引？

索引最大的好处是提高查询速度，但是索引也是有缺点的，比如：

- <font color=blue>需要占用物理空间</font>，数量越大，占用空间越大；
- <font color=blue>创建索引和维护索引要耗费时间，这种时间随着数据量的增加而增大；</font>对索引的维护会降低表的增删改的效率，因为每次增删改索引，B+ 树为了维护索引有序性，都需要进行动态维护。

所以，索引不是万能钥匙，它也是根据场景来使用的。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#什么时候适用索引)什么时候适用索引？

- <font color=blue>字段有唯一性限制的</font>，比如商品编码；（区分度大）
- <font color=blue>经常用于 `WHERE` 查询条件的字段</font>，这样能够提高整个表的查询速度，如果查询条件不是一个字段，可以建立联合索引。
- <font color=blue>经常用于 `GROUP BY` 和 `ORDER BY` 的字段</font>，这样在查询的时候就不需要再去做一次排序了，因为我们都已经知道了建立索引之后在 B+Tree 中的记录都是排序好的。

#### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#什么时候不需要创建索引)什么时候不需要创建索引？

- <font color=blue>字段中存在大量重复数据</font>，不需要创建索引，比如性别字段，只有男女，如果数据库表中，男女的记录分布均匀，那么无论搜索哪个值都可能得到一半的数据。在这些情况下，还不如不要索引，因为 MySQL 还有一个查询优化器，查询优化器发现某个值出现在表的数据行中的百分比很高的时候，它一般会忽略索引，进行全表扫描。
- <font color=blue>`WHERE` 条件，`GROUP BY`，`ORDER BY` 里用不到的字段</font>，索引的价值是快速定位，如果起不到定位的字段通常是不需要创建索引的，因为索引是会占用物理空间的。
- <font color=blue>表数据太少的时候</font>，不需要创建索引；
- <font color=blue>经常更新的字段不用创建索引</font>，比如不要对电商项目的用户余额建立索引，因为索引字段频繁修改，由于要维护 B+Tree的有序性，那么就需要频繁的重建索引，这个过程是会影响数据库性能的。

## [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#有什么优化索引的方法)有什么优化索引的方法？

这里说一下几种常见优化索引的方法：

- 前缀索引优化；
- 覆盖索引优化；
- 主键索引最好是自增的；
- 索引最好设置为 NOT NULL
- 防止索引失效；

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#前缀索引优化)前缀索引优化

前缀索引顾名思义就是使用某个字段中字符串的前几个字符建立索引，那我们为什么需要使用前缀来建立索引呢？

- **<font color=blue>节省空间</font>**：只索引字符串的前 N 个字符，显著减少索引文件大小。
- **<font color=blue>提升 I/O 效率</font>**：单个索引页能容纳更多条目，减少 B+ 树高度和磁盘读取次数。
- **<font color=blue>加速等值/范围查询</font>**：在 `WHERE col LIKE 'abc%'` 或 `WHERE col = '...'` 场景下依然有效。

<font color=ee11ff>不过，前缀索引有一定的局限性，例如：</font>

1. **<font color=blue>ORDER BY 无法使用前缀索引</font>**
   - **原因**：排序需要完整的列值来保证顺序的正确性。前缀相同但后缀不同的记录（如 `"apple"` vs `"application"`），在前缀索引中可能被视为相同或顺序不确定，导致排序结果错误。
   - **后果**：数据库必须回表获取完整数据进行文件排序（filesort），丧失索引排序优势。
2. **<font color=blue>无法用作覆盖索引（Covering Index）</font>**
   - **原因**：覆盖索引要求查询所需的所有列都包含在索引中。前缀索引仅存储部分数据，当 SELECT 列表中包含该字段时，引擎无法仅从索引中还原完整值。
   - **后果**：即使 WHERE 条件命中了前缀索引，仍需回表读取完整行数据，无法实现“索引即数据”的零回表查询。
3. **<font color=blue>其他常见限制</font>**
   - **区分度（Selectivity）下降风险**：如果前缀长度选择不当，可能导致大量重复值，使索引退化为全表扫描。建议通过 `COUNT(DISTINCT LEFT(col, N)) / COUNT(*)` 评估不同前缀长度的区分度。
   - **不支持某些操作**：如 `GROUP BY`、`DISTINCT`、以及以通配符开头的 `LIKE '%abc'` 查询均无法利用前缀索引。
   - **字符集敏感**：对于多字节字符集（如 UTF8MB4），前缀长度按字节计算时需特别注意截断到合法字符边界，否则可能引发错误或乱码。

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#覆盖索引优化)覆盖索引优化

<font color=24b3aa>覆盖索引是指 SQL 中 query 的所有字段，在索引 B+Tree 的叶子节点上都能找得到的那些索引，从而不需要通过主键回表到聚簇索引查询。</font>

InnoDB 中，任何索引都要保存主键值，主键索引自然没问题，辅助索引保存主键值是为了回表查询完整数据：

* **聚簇索引（主键索引）**：叶子节点 = 完整数据行（自然包含主键）。
* **辅助索引（二级索引）**：叶子节点 = `索引列值 + 主键值`。

假设我们只需要查询商品的名称、价格，有什么方式可以避免回表呢？

我们可以建立一个联合索引，即「商品ID、名称、价格」作为一个联合索引。如果索引中存在这些数据，查询将不会再次检索主键索引，从而避免回表。

所以，<font color=24b3aa>使用覆盖索引的好处就是，不需要查询出包含整行记录的所有信息，也就减少了大量的 I/O 操作。</font>

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#主键索引最好是自增的)主键索引最好是自增的

我们在建表的时候，都会默认将主键索引设置为自增的，具体为什么要这样做呢？又什么好处？

<font color=24b3aa>InnoDB 创建主键索引默认为聚簇索引，数据被存放在了 B+Tree 的叶子节点上。</font>也就是说，同一个叶子节点内的各个数据是按主键顺序存放的，因此，每当有一条新的数据插入时，数据库会根据主键将其插入到对应的叶子节点中。

<font color=24b3aa>**如果我们使用自增主键**，那么每次插入的新数据就会按顺序添加到当前索引节点的位置，不需要移动已有的数据，当页面写满，就会自动开辟一个新页面。因为每次**插入一条新记录，都是追加操作，不需要重新移动数据**，因此这种插入数据的方法效率非常高。</font>

**如果我们使用非自增主键**，由于每次插入主键的索引值都是随机的，因此每次插入新的数据时，就可能会插入到现有数据页中间的某个位置，这将不得不移动其它数据来满足新数据的插入，甚至需要从一个页面复制数据到另外一个页面，我们通常将这种情况称为 **<font color=blue>页分裂</font>**。**页分裂还有可能会造成大量的内存碎片，导致索引结构不紧凑，从而影响查询效率**。

举个例子，假设某个数据页中的数据是1、3、5、9，且数据页满了，现在准备插入一个数据7，则需要把数据页分割为两个数据页：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E9%A1%B5%E5%88%86%E8%A3%82.png)

<font color=blue>出现页分裂时，需要将一个页的记录移动到另外一个页，性能会受到影响，同时页空间的利用率下降，造成存储空间的浪费。</font>

而如果记录是顺序插入的，例如插入数据11，则只需开辟新的数据页，也就不会发生页分裂：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E7%B4%A2%E5%BC%95/%E5%BC%80%E8%BE%9F%E6%96%B0%E9%A1%B5.png)

因此，在使用 InnoDB 存储引擎时，如果没有特别的业务需求，建议使用自增字段作为主键。

另外，<font color=ee11ff>主键字段的长度不要太大，因为**主键字段长度越小，意味着二级索引的叶子节点越小（二级索引的叶子节点存放的数据是主键值），这样二级索引占用的空间也就越小**。</font>

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#索引最好设置为-not-null)索引最好设置为 NOT NULL

为了更好的利用索引，索引列要设置为 NOT NULL 约束。

**1.** ***NULL 值破坏索引的连续性与可比性***

- 在 B+ 树索引中，`NULL` 值的排序位置是不确定的（MySQL InnoDB 中 `NULL` 排在最前，但不同引擎/数据库行为可能不同）。
- 涉及 `NULL` 的比较（如 `=`, `<`, `>`）无法直接使用索引的有序性进行高效范围扫描或等值查找，优化器可能放弃使用索引。
- `IS NULL` / `IS NOT NULL` 虽然可以用索引，但效率通常低于等值查询，且无法与其他条件组合形成高效的索引范围扫描。

**2.** ***NULL 增加存储开销与复杂度***

- InnoDB 中，允许为 NULL 的列需要额外的 **NULL 标志位（null bitmap）** 来记录每行该列是否为 NULL。
- 对于联合索引，若包含可空列，索引结构会更复杂，可能导致索引条目变长、页分裂更频繁。
- `NOT NULL` 列无需此标志位，索引更紧凑，单页可容纳更多条目 → 减少 B+ 树高度 → 降低 IO 次数。

**3.** ***优化器统计信息更准确***

- 可空列的基数（Cardinality）估算需考虑 NULL 的比例，统计信息容易失真。
- `NOT NULL` 列的值域更明确，优化器对选择率（Selectivity）的判断更精准，更容易选出最优执行计划。

**4.** ***避免隐式转换与意外结果***

- `WHERE col = NULL` 永远返回 FALSE（应写为 `IS NULL`），这是常见 bug 来源。
- 聚合函数如 `COUNT(col)` 会忽略 NULL，而 `COUNT(*)` 不会，语义混淆易导致业务错误。
- `NOT NULL` 从 schema 层面杜绝此类问题，使索引行为更可预测。

### [#](https://www.xiaolincoding.com/mysql/index/index_interview.html#防止索引失效)防止索引失效

用上了索引并不意味着查询的时候会使用到索引，所以我们心里要清楚有哪些情况会导致索引失效，从而避免写出索引失效的查询语句，否则这样的查询效率是很低的。

我之前写过索引失效的文章，想详细了解的可以去看这篇文章：[谁还没碰过索引失效呢?(opens new window)](https://mp.weixin.qq.com/s/lEx6iRRP3MbwJ82Xwp675w)

这里简单说一下，发生索引失效的情况：

- 当我们使用左或者左右模糊匹配的时候，也就是 `like %xx` 或者 `like %xx%` 这两种方式都会造成索引失效；联合索引要能正确使用需要遵循最左匹配原则，也就是按照最左优先的方式进行索引的匹配，否则就会导致索引失效。<font color=24b3aa>最左原则</font>
- 当我们在查询条件中对索引列做了<font color=24b3aa>计算、函数、类型转换操作</font>，这些情况下都会造成索引失效；
- <font color=24b3aa>在 WHERE 子句中，如果在 OR 前的条件列是索引列，而在 OR 后的条件列不是索引列，那么索引会失效。</font>

我上面说的是常见的索引失效场景，实际过程中，可能会出现其他的索引失效场景，这时我们就需要查看执行计划，通过执行计划显示的数据判断查询语句是否使用了索引。

如下图，就是一个没有使用索引，并且是一个全表扫描的查询语句。

![img](https://cdn.xiaolincoding.com//mysql/other/798ab1331d1d6dff026e262e788f1a28.png)

对于执行计划，参数有：

- possible_keys 字段表示可能用到的索引；
- key 字段表示实际用的索引，如果这一项为 NULL，说明没有使用索引；
- key_len 表示索引的长度；
- rows 表示扫描的数据行数。
- type 表示数据扫描类型，我们需要重点看这个。

type 字段就是描述了找到所需数据时使用的扫描方式是什么，常见扫描类型的**执行效率从低到高的顺序为**：

- <font color=24b3aa>All（全表扫描）；</font>
- <font color=24b3aa>index（全索引扫描）；</font>
- <font color=24b3aa>range（索引范围扫描）；</font>
- <font color=24b3aa>ref（非唯一索引扫描）；</font>
- <font color=24b3aa>eq_ref（唯一索引扫描）；</font>
- <font color=24b3aa>const（结果只有一条的主键或唯一索引扫描）。</font>

在这些情况里，all 是最坏的情况，因为采用了全表扫描的方式。index 和 all 差不多，只不过 index 对索引表进行全扫描，这样做的好处是不再需要对数据进行排序，但是开销依然很大。所以，要尽量避免全表扫描和全索引扫描。

range 表示采用了索引范围扫描，一般在 WHERE 子句中使用 < 、>、in、between 等关键词，只检索给定范围的行，属于范围查找。**从这一级别开始，索引的作用会越来越明显，因此我们需要尽量让 SQL 查询可以使用到 range 这一级别及以上的 type 访问方式**。

ref 类型表示采用了非唯一索引，或者是唯一索引的非唯一性前缀，返回数据返回可能是多条。因为虽然使用了索引，但该索引列的值并不唯一，有重复。这样即使使用索引快速查找到了第一条数据，仍然不能停止，要进行目标值附近的小范围扫描。但它的好处是它并不需要扫全表，因为索引是有序的，即便有重复值，也是在一个非常小的范围内扫描。

eq_ref 类型是使用主键或唯一索引时产生的访问方式，通常使用在多表联查中。比如，对两张表进行联查，关联条件是两张表的 user_id 相等，且 user_id 是唯一索引，那么使用 EXPLAIN 进行执行计划查看的时候，type 就会显示 eq_ref。

const 类型表示使用了主键或者唯一索引与常量值进行比较，比如 SELECT name FROM product WHERE id=1。

需要说明的是 const 类型和 eq_ref 都使用了主键或唯一索引，不过这两个类型有所区别，**const 是与常量进行比较，查询效率会更快，而 eq_ref 通常用于多表联查中**。

> 除了关注 type，我们也要关注 extra 显示的结果。

这里说几个重要的参考指标：

- **Using filesort ：**当查询语句中包含 group by 操作，而且无法利用索引完成排序操作的时候， 这时不得不选择相应的排序算法进行，甚至可能会通过文件排序，效率是很低的，所以要避免这种问题的出现。
- **Using temporary：**使了用临时表保存中间结果，MySQL 在对查询结果排序时使用临时表，常见于排序 ORDER BY 和分组查询 group by。效率低，要避免这种问题的出现。
- **Using index：**所需数据只需在索引即可全部获得，不须要再到表中取数据，也就是使用了覆盖索引，避免了回表操作，效率不错。

# [索引]为什么 MySQL 采用 B+ 树作为索引？

## 前言

在 MySQL 中，无论是 Innodb 还是 MyIsam，都使用了 B+ 树作索引结构（这里不考虑 hash 等其他索引）。本文将从最普通的二叉查找树开始，逐步说明各种树解决的问题以及面临的新问题，从而说明 MySQL 为什么选择 B+ 树作为索引结构。

## 一、二叉查找树(BST)：不平衡

二叉查找树（BST，Binary Search Tree），也叫二叉排序树，在二叉树的基础上需要满足：任意节点的左子树上所有节点值不大于根节点的值，任意节点的右子树上所有节点值不小于根节点的值。如下是一颗BST。

![img](https://img2018.cnblogs.com/blog/1174710/201909/1174710-20190925074809219-1463846213.png)

当需要快速查找时，将数据存储在 BST 是一种常见的选择，因为此时查询时间取决于树高，平均时间复杂度是 O(lgn)。然而，**BST ** **可能长歪而变得不平衡**，如下图所示，此时 BST 退化为链表，时间复杂度退化为 O(n)。

为了解决这个问题，引入了平衡二叉树。

![img](https://img2018.cnblogs.com/blog/1174710/201909/1174710-20190925074822219-428720198.png)

## 二、平衡二叉树(AVL)：旋转耗时

AVL 树是严格的平衡二叉树，所有节点的左右子树高度差不能超过 1；AVL 树查找、插入和删除在平均和最坏情况下都是 O(lgn)。

AVL 实现平衡的关键在于旋转操作：插入和删除可能破坏二叉树的平衡，此时需要通过一次或多次树旋转来重新平衡这个树。

* 当<font color=blue>插入</font>数据时，最多只需要 <font color=blue>1 次旋转(单旋转或双旋转)；</font>
* 但是当<font color=blue>删除</font>数据时，会导致树失衡，AVL 需要维护从被删除节点到根节点这条路径上所有节点的平衡，旋转的量级为 <font color=24b3aa>$O(log^n)$</font>。

<font color=24b3aa>**由于旋转的耗时，AVL 树在删除数据时效率很低**；在删除操作较多时，维护平衡所需的代价可能高于其带来的好处，因此 AVL 实际使用并不广泛。</font>

## 三、红黑树：树太高

与 AVL 树相比，红黑树并不追求严格的平衡，而是大致的平衡：只是确保从根到叶子的最长的可能路径不多于最短的可能路径的两倍长。从实现来看，红黑树最大的特点是每个节点都属于两种颜色（红色或黑色）之一，且节点颜色的划分需要满足特定的规则（具体规则略）。红黑树示例如下：

![img](https://img2018.cnblogs.com/blog/1174710/201909/1174710-20190925074849294-796902926.png) 

与 AVL 树相比，红黑树的查询效率会有所下降，这是因为树的平衡性变差，高度更高。但红黑树的删除效率大大提高了，<font color=blue>因为红黑树同时引入了颜色，当插入或删除数据时，只需要进行 **O(1)** 次数的旋转以及变色就能保证基本的平衡</font>，不需要像 AVL 树进行 O(lgn) 次数的旋转。总的来说，红黑树的统计性能高于 AVL。

因此，在实际应用中，AVL 树的使用相对较少，而红黑树的使用非常广泛。例如，Java 中的 TreeMap 使用红黑树存储排序键值对；Java8 中的 HashMap 使用链表+红黑树解决哈希冲突问题（当冲突节点较少时，使用链表，当冲突节点较多时，使用红黑树）。

<font color=24b3aa>对于数据在内存中的情况（如上述的 TreeMap 和 HashMap），红黑树的表现是非常优异的。但是 **对于数据在磁盘等辅助存储设备中的情况（如  MySQL 等数据库），红黑树并不擅长，因为红黑树长得还是太高了。**</font>当数据在磁盘中时，磁盘 IO 会成为最大的性能瓶颈，设计的目标应该是尽量减少 IO 次数；而树的高度越高，增删改查所需要的 IO 次数也越多，会严重影响性能。

## 四、B树：为磁盘而生

<font color=blue>B 树（B-Tree，注意 - 表示连字符而不是减号）</font>，是为磁盘等辅存设备设计的多路平衡查找树，与二叉树相比，B 树的每个非叶节点可以有多个子树。因此，当总节点数量相同时，B 树的高度远远小于 AVL 树和红黑树（B 树是一颗“矮胖子”），磁盘 IO 次数大大减少。

定义 B 树最重要的概念是 <font color=ee11ff>阶数（Order）</font>，对于一颗 m 阶 B 树，需要满足以下条件：

- 每个节点最多包含 m 个子节点。
- 如果根节点包含子节点，则至少包含 2 个子节点；除根节点外，每个非叶节点至少包含 m/2 个子节点。
- 拥有 k 个子节点的非叶节点将包含 k - 1 条记录。
- 所有叶节点都在同一层中。

可以看出，B 树的定义，主要是对非叶结点的子节点数量和记录数量的限制。

下图是一个 3 阶 B 树的例子：

 ![img](https://img2018.cnblogs.com/blog/1174710/201909/1174710-20190925074904497-1088606861.png)

B 树的优势除了 **<font color=red>树高小</font>**，还有对 **<font color=red>访问局部性原理</font>** 的利用。所谓局部性原理，是指当一个数据被使用时，其附近的数据有较大概率在短时间内被使用。B 树将键相近的数据存储在同一个节点，当访问其中某个数据时，数据库会将该整个节点读到缓存中；当它临近的数据紧接着被访问时，可以直接在缓存中读取，无需进行磁盘 IO；换句话说，B 树的缓存命中率更高。

B 树在数据库中有一些应用，如 mongodb 的索引使用了 B 树结构。但是在很多数据库应用中，使用了是 B 树的变种 B+ 树。

## 五、B+ 树

B+ 树也是多路平衡查找树，其与 B 树的区别主要在于：

1. B 树中每个节点（包括叶节点和非叶节点）都存储真实的数据，B+ 树中只有叶子节点存储真实的数据，非叶节点只存储键。在 MySQL 中，这里所说的真实数据：
   * 可能是行的全部数据（如 Innodb 的聚簇索引）
   * 也可能只是行的主键（如 Innodb 的辅助索引）
   * 或者是行所在的地址（如 MyIsam 的非聚簇索引）
2. B 树中一条记录（键 + 数据）只会出现一次，不会重复出现，而 B+ 树中同一个键值会作为**路由索引**出现在内部节点，同时作为**数据载体**出现在叶子节点
3. B+ 树的叶节点之间通过双向链表链接。
4. B 树中的非叶节点，记录数比子节点个数少 1；而 B+ 树中记录数与子节点个数相同。

由此，B+ 树与 B 树相比，有以下优势：

- **<font color=blue>更少的 IO 次数：</font>**B+ 树的非叶节点只包含键，而不包含真实数据，因此每个节点存储的记录个数比 B 数多很多（即阶 m 更大），因此 B+ 树的高度更低，访问时所需要的 IO 次数更少。此外，由于每个节点存储的记录数更多，所以对<font color=blue>访问局部性原理的利用更好</font>，缓存命中率更高。
- **<font color=blue>更适于范围查询：</font>**在 B 树中进行范围查询时，首先找到要查找的下限，然后对 B 树进行中序遍历，直到找到查找的上限；而 B+ 树的范围查询，只需要对链表进行遍历即可。
- **<font color=blue>更稳定的查询效率：</font>**B 树的查询时间复杂度在 1 到树高之间（分别对应记录在根节点和叶节点），而 B+ 树的查询复杂度则稳定为树高，因为所有数据都在叶节点。

B+ 树也存在劣势：由于键会重复出现，因此会占用更多的空间。但是与带来的性能优势相比，空间劣势往往可以接受，因此 B+ 树的在数据库中的使用比 B 树更加广泛。

## 六、感受 B+ 树的威力

前面说到，B树/B+树与红黑树等二叉树相比，最大的优势在于树高更小。实际上，<font color=blue>对于 Innodb 的 B+ 索引来说，树的高度一般在 2-4 层。</font>下面来进行一些具体的估算。

树的高度是由阶数决定的，阶数越大树越矮；而阶数的大小又取决于每个节点可以存储多少条记录。 Innodb 中每个节点使用一个页（page），页的大小为 16KB，其中元数据只占大约 128 字节左右(（包括文件管理头信息、页面头信息等等），大多数空间都用来存储数据。

- <font color=24b3aa>对于非叶节点，记录只包含索引的键和指向下一层节点的指针。</font>假设每个非叶节点页面存储 1000 条记录，则每条记录大约占用 16 字节；当索引是整型或较短的字符串时，这个假设是合理的。延伸一下，我们经常听到建议说索引列长度不应过大，原因就在这里：索引列太长，每个节点包含的记录数太少，会导致树太高，索引的效果会大打折扣，而且索引还会浪费更多的空间。
- 对于叶节点，记录包含了索引的键和值（值可能是行的主键、一行完整数据等，具体见前文），数据量更大。这里假设每个叶节点页面存储 100 条记录（实际上，当索引为聚簇索引时，这个数字可能不足 100；当索引为辅助索引时，这个数字可能远大于 100；可以根据实际情况进行估算）。

对于一颗 3 层 B+ 树，第一层（根节点）有 1 个页面，可以存储 1000 条记录；第二层有 1000 个页面，可以存储 $1000\times1000$条记录；第三层（叶节点）有 $1000\times1000$ 个页面，每个页面可以存储 100 条记录，因此可以存储 $1000\times1000\times100$ 条记录，即 1 亿条。而对于二叉树，存储 1 亿条记录则需要 26 层左右。

## 七、总结

最后，总结一下各种树解决的问题以及面临的新问题：

1. 二叉查找树（BST）：解决了排序的基本问题，但是由于无法保证平衡，可能退化为链表；
2. 平衡二叉树（AVL）：通过旋转解决了平衡的问题，但是旋转操作效率太低；
3. 红黑树：通过舍弃严格的平衡和引入红黑节点，解决了 AVL 旋转效率过低的问题，但是在磁盘等场景下，树仍然太高，IO 次数太多；
4. B 树：通过将二叉树改为多路平衡查找树，解决了树过高的问题；
5. B+ 树：在 B 树的基础上，将非叶节点改造为不存储数据的纯索引节点，进一步降低了树的高度；此外将叶节点使用指针连接成链表，范围查询更加高效。

## 参考文献

《MySQL技术内幕：InnoDB存储引擎》

《MySQL运维内参》

https://zhuanlan.zhihu.com/p/54102723

https://cloud.tencent.com/developer/article/1425604

https://blog.csdn.net/whoamiyang/article/details/51926985

https://www.jianshu.com/p/37436ed14cc6

https://blog.csdn.net/CrankZ/article/details/83301702

https://www.cnblogs.com/gaochundong/p/btree_and_bplustree.html

# [索引]从数据页的角度看 B+ 树

![图片](https://cdn.xiaolincoding.com//mysql/other/f7696506a3c1c94621fcbad10341f1a8.png)

## [#](https://www.xiaolincoding.com/mysql/index/page.html#innodb-是如何存储数据的)InnoDB 是如何存储数据的？

MySQL 支持多种存储引擎，不同的存储引擎，存储数据的方式也是不同的，我们最常使用的是 InnoDB 存储引擎，所以就跟大家图解下InnoDB 是如何存储数据的。

记录是按照行来存储的，但是数据库的读取并不以「行」为单位，否则一次读取（也就是一次 I/O 操作）只能处理一行数据，效率会非常低。

因此，**<font color=blue>InnoDB 的数据是按「数据页」为单位来读写的</font>**，也就是说，当需要读一条记录的时候，并不是将这个记录本身从磁盘读出来，而是以页为单位，将其整体读入内存。

数据库的 I/O 操作的最小单位是页，**InnoDB 数据页的默认大小是 16KB**，意味着数据库每次读写都是以 16KB 为单位的，一次最少从磁盘中读取 16K 的内容到内存中，一次最少把内存中的 16K 内容刷新到磁盘中。

数据页包括七个部分，结构如下图：

![图片](https://cdn.xiaolincoding.com//mysql/other/243b1466779a9e107ae3ef0155604a17.png)

这 7 个部分的作用如下图：

![图片](https://cdn.xiaolincoding.com//mysql/other/fabd6dadd61a0aa342d7107213955a72.png)

在 File Header 中有两个指针，分别指向上一个数据页和下一个数据页，连接起来的页相当于一个双向的链表，如下图所示：

![图片](https://cdn.xiaolincoding.com//mysql/other/557d17e05ce90f18591c2305871af665.png)

采用链表的结构是让数据页之间不需要是物理上的连续的，而是逻辑上的连续。

数据页的主要作用是存储记录，也就是数据库的数据，所以重点说一下数据页中的 User Records 是怎么组织数据的。

<font color=24b3aa>**数据页中的记录按照「主键」顺序组成单向链表**，单向链表的特点就是插入、删除非常方便，但是检索效率不高，最差的情况下需要遍历链表上的所有节点才能完成检索。</font>

<font color=24b3aa>因此，数据页中有一个**页目录**，起到记录的索引作用</font>，就像我们书那样，针对书中内容的每个章节设立了一个目录，想看某个章节的时候，可以查看目录，快速找到对应的章节的页数，而数据页中的页目录就是为了能快速找到记录。

那 InnoDB 是如何给记录创建页目录的呢？页目录与记录的关系如下图：

![图片](https://cdn.xiaolincoding.com//mysql/other/261011d237bec993821aa198b97ae8ce.png)

页目录创建的过程如下：

1. 将所有的记录划分成几个组，这些记录包括最小记录和最大记录，但不包括标记为“已删除”的记录；
2. 每个记录组的最后一条记录就是组内最大的那条记录，<font color=blue>并且最后一条记录的头信息中会存储该组一共有多少条记录，作为 n_owned 字段（上图中粉红色字段）</font>
3. 页目录用来存储每组最后一条记录的地址偏移量，这些地址偏移量会按照先后顺序存储起来，<font color=24b3aa>每组的地址偏移量也被称之为槽（slot），**每个槽相当于指针指向了不同组的最后一个记录**。</font>

从图可以看到，<font color=24b3aa>**页目录就是由多个槽组成的，槽相当于分组记录的索引**。然后，因为记录是按照「主键值」从小到大排序的，所以**我们通过槽查找记录时，可以使用二分法快速定位要查询的记录在哪个槽（哪个记录分组），定位到槽后，再遍历槽内的所有记录，找到对应的记录**，无需从最小记录开始遍历整个页中的记录链表。</font>

以上面那张图举个例子，5 个槽的编号分别为 0，1，2，3，4，我想查找主键为 11 的用户记录：

- 先二分得出槽中间位是 (0+4)/2=2 ，2 号槽里最大的记录为 8。因为 11 > 8，所以需要从 2 号槽后继续搜索记录；
- 再使用二分搜索出 2 号和 4 槽的中间位是 (2+4)/2= 3，3 号槽里最大的记录为 12。因为 11 < 12，所以主键为 11 的记录在 3 号槽里；
- 这里有个问题，**「槽对应的值都是这个组的主键最大的记录，如何找到组里最小的记录」**？比如槽 3 对应最大主键是 12 的记录，那如何找到最小记录 9。解决办法是：通过槽 3 找到 槽 2 对应的记录，也就是主键为 8 的记录。主键为 8 的记录的下一条记录就是槽 3 当中主键最小的 9 记录，然后开始向下搜索 2 次，定位到主键为 11 的记录，取出该条记录的信息即为我们想要查找的内容。

看到第三步的时候，可能有的同学会疑问，如果某个槽内的记录很多，然后因为记录都是单向链表串起来的，那这样在槽内查找某个记录的时间复杂度不就是 O(n) 了吗？

这点不用担心，InnoDB 对每个分组中的记录条数都是有规定的，槽内的记录就只有几条：

- 第一个分组中的记录只能有 1 条记录；
- 最后一个分组中的记录条数范围只能在 1-8 条之间；
- 剩下的分组中记录条数范围只能在 4-8 条之间。

> 本质上就是以空间换时间，slot 就是 block array。block 内部的 record 没有逻辑关联，仅仅是为了提高查询速度。
>
> 不过需要注意的是，slot 只能用于 single page 索引。

## [#](https://www.xiaolincoding.com/mysql/index/page.html#总结)总结

InnoDB 的数据是按「数据页」为单位来读写的，默认数据页大小为 16 KB。每个数据页之间通过双向链表的形式组织起来，物理上不连续，但是逻辑上连续。

数据页内包含用户记录，每个记录之间用单向链表的方式组织起来，为了加快在数据页内高效查询记录，设计了一个页目录，页目录存储各个槽（slot，分组），且主键值是有序的，于是可以通过二分查找法的方式进行检索从而提高效率。

为了高效查询记录所在的数据页，InnoDB 采用 B+ 树作为索引，每个节点都是一个数据页。

如果叶子节点存储的是实际数据的就是聚簇索引，一个表只能有一个聚簇索引；如果叶子节点存储的不是实际数据，而是主键值则就是二级索引，一个表中可以有多个二级索引。

在使用二级索引进行查找数据时，如果查询的数据能在二级索引找到，那么就是「索引覆盖」操作，如果查询的数据不在二级索引里，就需要先在二级索引找到主键值，需要去聚簇索引中获得数据行，这个过程就叫作「回表」。

关于索引的内容还有很多，比如索引失效、索引优化等等，这些内容我下次在讲啦！

# [索引]MySQL 单表不要超过 2000W 行，靠谱吗？

作为在后端圈开车的多年老司机，是不是经常听到过：

- “MySQL 单表最好不要超过 2000W”
- “单表超过 2000W 就要考虑数据迁移了”
- “你这个表数据都马上要到 2000W 了，难怪查询速度慢”

这些名言民语就和 “群里只讨论技术，不开车，开车速度不要超过 120 码，否则自动踢群”，只听过，没试过，哈哈。

下面我们就把车速踩到底，干到 180 码试试…….

> 原文链接：https://my.oschina.net/u/4090830/blog/5559454

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#实验)实验

实验一把看看… 建一张表

```sql
CREATE TABLE person(
    id int NOT NULL AUTO_INCREMENT PRIMARY KEY comment '主键',
    person_id tinyint not null comment '用户id',
    person_name VARCHAR(200) comment '用户名称',
    gmt_create datetime comment '创建时间',
    gmt_modified datetime comment '修改时间'
) comment '人员信息表';
```

插入一条数据

```sql
insert into person values(1, 1,'user_1', NOW(), now());
```

利用 MySQL 伪列 rownum 设置伪列起始点为 1

```sql
SELECT (@i:=@i+1) as rownum, person_name FROM person, (SELECT @i:=100) as init; 
set @i=1;
```

运行下面的 sql，连续执行 20 次，就是 2 的 20 次方约等于 100w 的数据；执行 23 次就是 2 的 23 次方约等于 800w , 如此下去即可实现千万测试数据的插入。

如果不想翻倍翻倍的增加数据，而是想少量，少量的增加，有个技巧，就是在 SQL 的后面增加 WHERE 条件，如 id > 某一个值去控制增加的数据量即可。

```sql
insert into person(id, person_id, person_name, gmt_create, gmt_modified)
SELECT @i:=@i+1,
left(rand()*10,10) as person_id,
concat('user_',@i%2048),
date_add(gmt_create,interval + @i*cast(rand()*100 as signed) SECOND),
date_add(date_add(gmt_modified,interval +@i*cast(rand()*100 as signed) SECOND), interval + cast(rand()*1000000 as signed) SECOND)
FROM person;
```

此处需要注意的是，也许你在执行到近 800w 或者 1000w 数据的时候，会报错：The total number of locks exceeds the lock table size。

这是由于你的临时表内存设置的不够大，只需要扩大一下设置参数即可。

```sql
SET GLOBAL tmp_table_size =512*1024*1024; （512M）
SET global innodb_buffer_pool_size= 1*1024*1024*1024 (1G);
```

先来看一组测试数据，这组数据是在 MySQL 8.0 的版本，并且是在我本机上，由于本机还跑着 idea , 浏览器等各种工具，所以并不是机器配置就是用于数据库配置，所以测试数据只限于参考。

![图片](https://cdn.xiaolincoding.com//mysql/other/db5d7c556b3e8e31b3b90d4ace54fe7e.png)

看到这组数据似乎好像真的和标题对应，当数据达到 2000W 以后，查询时长急剧上升，难道这就是铁律吗？

那下面我们就来看看这个建议值 2000W 是怎么来的？

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#单表数量限制)单表数量限制

首先我们先想想数据库单表行数最大多大？

```sql
CREATE TABLE person(
    id int(10) NOT NULL AUTO_INCREMENT PRIMARY KEY comment '主键',
    person_id tinyint not null comment '用户id',
    person_name VARCHAR(200) comment '用户名称',
    gmt_create datetime comment '创建时间',
    gmt_modified datetime comment '修改时间'
) comment '人员信息表';
```

看看上面的建表 sql。id 是主键，本身就是唯一的，也就是说主键的大小可以限制表的上限：

- 如果主键声明 `int` 类型，也就是 32 位，那么支持 2^32-1 ~~21 亿；
- 如果主键声明 `bigint` 类型，那就是 2^62-1 （36893488147419103232），难以想象这个的多大了，一般还没有到这个限制之前，可能数据库已经爆满了！！

有人统计过，如果建表的时候，自增字段选择无符号的 bigint , 那么自增长最大值是 18446744073709551615，按照一秒新增一条记录的速度，大约什么时候能用完？

![图片](https://cdn.xiaolincoding.com//mysql/other/87031fea63547be0f8ea692781d5b068.png)

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#表空间)表空间

下面我们再来看看索引的结构，我们下面讲内容都是基于 Innodb 引擎的，大家都知道 Innodb 的索引内部用的是 B+ 树。

![图片](https://cdn.xiaolincoding.com//mysql/other/cb6f4fddc960cde55575a1b80a563b9a.png)

这张表数据，在硬盘上存储也是类似如此的，它实际是放在一个叫 person.ibd （innodb data）的文件中，也叫做表空间；虽然数据表中，他们看起来是一条连着一条，但是实际上在文件中它被分成很多小份的数据页，而且每一份都是 16K。

大概就像下面这样，当然这只是我们抽象出来的，在表空间中还有段、区、组等很多概念，但是我们需要跳出来看。

![图片](https://cdn.xiaolincoding.com//mysql/other/ee29f7137057b226658627ff61a6a514.png)

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#页的数据结构)页的数据结构

实际页的内部结构像是下面这样的：

![图片](https://cdn.xiaolincoding.com//mysql/other/c34b589e12e5bc0855c9bdeab0c63a88.png)

从图中可以看出，一个 InnoDB 数据页的存储空间大致被划分成了 7 个部分，有的部分占用的字节数是确定的，有的部分占用的字节数是不确定的。

在页的 7 个组成部分中，我们自己存储的记录会按照我们指定的行格式存储到 `User Records` 部分。

但是在一开始生成页的时候，其实并没有 User Records 这个部分，每当我们插入一条记录，都会从 Free Space 部分，也就是尚未使用的存储空间中申请一个记录大小的空间划分到 User Records 部分。

当 Free Space 部分的空间全部被 User Records 部分替代掉之后，也就意味着这个页使用完了，如果还有新的记录插入的话，就需要去申请新的页了。

这个过程的图示如下：

![图片](https://cdn.xiaolincoding.com//mysql/other/ea5cc8c67b7656d3f2a11e42293a0244.png)

刚刚上面说到了数据的新增的过程。

那下面就来说说，数据的查找过程，假如我们需要查找一条记录，我们可以把表空间中的每一页都加载到内存中，然后对记录挨个判断是不是我们想要的。

在数据量小的时候，没啥问题，内存也可以撑。但是现实就是这么残酷，不会给你这个局面。

为了解决这问题，MySQL 中就有了索引的概念，大家都知道索引能够加快数据的查询，那到底是怎么个回事呢？下面我就来看看。

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#索引的数据结构)索引的数据结构

在 MySQL 中索引的数据结构和刚刚描述的页几乎是一模一样的，而且大小也是 16K,。

但是在索引页中记录的是页 (数据页，索引页) 的最小主键 id 和页号，以及在索引页中增加了层级的信息，从 0 开始往上算，所以页与页之间就有了上下层级的概念。

![图片](https://cdn.xiaolincoding.com//mysql/other/6374409c6c404d446855dc6a694b6d26.png)

看到这个图之后，是不是有点似曾相似的感觉，是不是像一棵二叉树啊，对，没错！它就是一棵树。

只不过我们在这里只是简单画了三个节点，2 层结构的而已，如果数据多了，可能就会扩展到 3 层的树，这个就是我们常说的 B+ 树，最下面那一层的 page level =0, 也就是叶子节点，其余都是非叶子节点。

![图片](https://cdn.xiaolincoding.com//mysql/other/baf6644df710e1639e8e956b1f67d502.png)

看上图中，我们是单拿一个节点来看，首先它是一个非叶子节点（索引页），在它的内容区中有 id 和 页号地址两部分：

- id ：对应页中记录的最小记录 id 值；
- 页号：地址是指向对应页的指针；

而数据页与此几乎大同小异，区别在于数据页记录的是真实的行数据而不是页地址，而且 id 的也是顺序的。

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#单表建议值)单表建议值

下面我们就以 3 层，2 分叉（实际中是 M 分叉）的图例来说明一下查找一个行数据的过程。

![图片](https://cdn.xiaolincoding.com//mysql/other/585429e5078566bda9b2fa18f85215af.png)

比如说我们需要查找一个 id=6 的行数据：

- 因为在非叶子节点中存放的是页号和该页最小的 id，所以我们从顶层开始对比，首先看页号 10 中的目录，有 [id=1, 页号 = 20],[id=5, 页号 = 30], 说明左侧节点最小 id 为 1，右侧节点最小 id 是 5。6>5, 那按照二分法查找的规则，肯定就往右侧节点继续查找；
- 找到页号 30 的节点后，发现这个节点还有子节点（非叶子节点），那就继续比对，同理，6>5 && 6<7, 所以找到了页号 60；
- 找到页号 60 之后，发现此节点为叶子节点（数据节点），于是将此页数据加载至内存进行一一对比，结果找到了 id=6 的数据行。

从上述的过程中发现，我们为了查找 id=6 的数据，总共查询了三个页，如果三个页都在磁盘中（未提前加载至内存），那么最多需要经历三次的磁盘 IO。

需要注意的是，图中的页号只是个示例，实际情况下并不是连续的，在磁盘中存储也不一定是顺序的。

至此，我们大概已经了解了表的数据是怎么个结构了，也大概知道查询数据是个怎么的过程了，这样我们也就能大概估算这样的结构能存放多少数据了。

从上面的图解我们知道 B+ 数的叶子节点才是存在数据的，而非叶子节点是用来存放索引数据的。

所以，同样一个 16K 的页，非叶子节点里的每条数据都指向新的页，而新的页有两种可能

- 如果是叶子节点，那么里面就是一行行的数据
- 如果是非叶子节点的话，那么就会继续指向新的页

<font color=24b3aa>假设</font>

- <font color=24b3aa>非叶子节点内指向其他页的数量为 x</font>
- <font color=24b3aa>叶子节点内能容纳的数据行数为 y</font>
- <font color=24b3aa>B+ 数的层数为 z</font>

<font color=24b3aa>如下图中所示，总数据行数 **$Total =x^{z-1} \times y$ **</font>

![图片](https://cdn.xiaolincoding.com//mysql/other/e741373dcb282fce80d1522d33c6b53b.png)

> **<font color=24b3aa>$X$ =？</font>**

<font color=24b3aa>在文章的开头已经介绍了页的结构，索引也也不例外，都会有 File Header (38 byte)、Page Header (56 Byte)、Infimum + Supermum（26 byte）、File Trailer（8byte），再加上页目录，大概 1k 左右。</font>

<font color=24b3aa>我们就当做它就是 1K，那整个页的大小是 16K，剩下 15k 用于存数据，在索引页中主要记录的是主键与页号，主键我们假设是 Bigint (8 byte), 而页号也是固定的（4Byte）, 那么索引页中的一条数据也就是 12byte。</font>

<font color=24b3aa>所以 $X=\frac{15 \times 1024 }{12}≈1280$ 行。</font>

> **<font color=24b3aa>$Y$=？</font>**

<font color=24b3aa>叶子节点和非叶子节点的结构是一样的，同理，能放数据的空间也是 15k。</font>

<font color=24b3aa>但是叶子节点中存放的是真正的行数据，这个影响的因素就会多很多，比如，字段的类型，字段的数量。每行数据占用空间越大，页中所放的行数量就会越少。</font>

<font color=24b3aa>这边我们暂时按一条行数据 1k 来算，那一页就能存下 15 条，$Y = 15*1024/1000 ≈15$。</font>

<font color=24b3aa>算到这边了，是不是心里已经有谱了啊。</font>

<font color=24b3aa>根据上述的公式，$Total =x^{z-1} \times y$，已知 $x=1280，y=15$：</font>

- <font color=24b3aa>假设 B+ 树是两层，那就是 $z=2$， $Total=(1280 ^1)*15=19200$</font>
- <font color=24b3aa>假设 B+ 树是三层，那就是 $z=3$， $Total=(1280 ^2) \times 15 = 24576000$ （约 $2.45$KW）</font>

<font color=24b3aa>哎呀，妈呀！这不是正好就是文章开头说的最大行数建议值 2000W 嘛！对的，一般 B+ 数的层级最多也就是 3 层。</font>

<font color=24b3aa>你试想一下，如果是 4 层，除了查询的时候磁盘 IO 次数会增加，而且这个 Total 值会是多少，大概应该是 3 百多亿吧，也不太合理，所以，3 层应该是比较合理的一个值。</font>

> **<font color=24b3aa>到这里难道就完了？</font>**

<font color=24b3aa>不。</font>

<font color=24b3aa>我们刚刚在说 Y 的值时候假设的是 1K ，那比如我实际当行的数据占用空间不是 1K , 而是 5K, 那么单个数据页最多只能放下 3 条数据。</font>

<font color=24b3aa>同样，还是按照 z = 3 的值来计算，那 $Total=(1280 ^2) \times 3 = 4915200$ （近 500w）</font>

<font color=24b3aa>所以，在保持相同的层级（相似查询性能）的情况下，在行数据大小不同的情况下，其实这个最大建议值也是不同的，而且影响查询性能的还有很多其他因素，比如，数据库版本，服务器配置，sql 的编写等等。</font>

<font color=24b3aa>MySQL 为了提高性能，会将表的索引装载到内存中，在 InnoDB buffer size 足够的情况下，其能完成全加载进内存，查询不会有问题。</font>

<font color=24b3aa>但是，当单表数据库到达某个量级的上限时，导致内存无法存储其索引，使得之后的 SQL 查询会产生磁盘 IO，从而导致性能下降，所以增加硬件配置（比如把内存当磁盘使），可能会带来立竿见影的性能提升哈。</font>

## [#](https://www.xiaolincoding.com/mysql/index/2000w.html#总结)总结

- MySQL 的表数据是以页的形式存放的，页在磁盘中不一定是连续的。
- 页的空间是 16K, 并不是所有的空间都是用来存放数据的，会有一些固定的信息，如，页头，页尾，页码，校验码等等。
- 在 B+ 树中，叶子节点和非叶子节点的数据结构是一样的，区别在于，叶子节点存放的是实际的行数据，而非叶子节点存放的是主键和页号。
- 索引结构不会影响单表最大行数，2000W 也只是推荐值，超过了这个值可能会导致 B + 树层级更高，影响查询性能。

> “单表不要超过 2000 万行”其实是个**经验阈值**，不是数据库的硬性上限。它想表达的是：当一张表增长到“千万级”，在很多常见业务形态里，**性能、运维和风险会开始明显变差**，这时候通常该考虑分区/分表/归档等治理手段了。原因主要在这几类：
>
> ## 1) 索引树变大：随机 I/O、缓存命中率下降
>
> 大多数 OLTP 表会建 B+Tree 索引。行数越多：
>
> - 索引层级更深、叶子页更多，查一次要访问更多页
> - 数据/索引页更难被 buffer pool 全部“热缓存”住
> - 范围查询、排序、回表（secondary index → primary key → row）成本更高
>   结果就是：**同样的 SQL，数据量上去后延迟更抖、P99 变差更明显**。
>
> ## 2) 写入/更新成本上升：页分裂、二级索引维护、锁竞争
>
> 行数大通常意味着：
>
> - 插入导致更多 **页分裂/页合并**（索引维护更频繁）
> - 更新/删除要维护更多索引，产生更多 undo/redo、MVCC 版本链变长
> - 热点写入（按某个业务键集中）更容易出现锁竞争/自旋/等待
>   所以表越大，**写性能和并发稳定性**越容易成为瓶颈。
>
> ## 3) 大表的“运维动作”更痛：DDL、备份恢复、回滚都变慢
>
> 大表最难的是“出事或变更时”：
>
> - 加字段/改类型/加索引（即使在线 DDL），耗时也会很长，并且带来较大 IO 压力
> - 备份、恢复、主从追延迟、数据校验都更重
> - 误操作（DELETE 没带条件）回滚/修复代价巨大
>   因此经验上会给个阈值：到了千万级，**风险成本**开始指数化。
>
> ## 4) 统计信息/优化器误判概率上升，慢 SQL 更难控
>
> 数据分布一复杂（倾斜、冷热、时间序列），大表里更容易出现：
>
> - 统计信息过期或不准 → 选错索引/Join 顺序
> - 某些查询从“走索引”退化成“扫大量数据”
>   小表时可能感知不明显，大表时就是事故级。
>
> ## 5) “2000 万”是怎么来的：跟行大小、硬件、查询形态强相关
>
> 这个数在不同场景差异很大：
>
> - 行很窄（几十字节）+ 全部命中缓存 + 查询都走覆盖索引：**几亿也能跑得很好**
> - 行很宽（上 KB）+ 大量范围扫描/排序/回表：**几百万就开始吃力**
> - NVMe、超大内存、分区裁剪等都能把“阈值”推高
>   所以它更像一句提醒：**到了这个量级要开始系统性治理，而不是继续靠“加机器/调参数”硬扛。**

# [索引]索引失效有哪些？

大家好，我是小林。

在工作中，如果我们想提高一条语句查询速度，通常都会想对字段建立索引。

但是索引并不是万能的。<font color=blue>建立了索引，并不意味着任何查询语句都能走索引扫描。</font>

稍不注意，可能你写的查询语句是会导致索引失效，从而走了全表扫描，虽然查询的结果没问题，但是查询的性能大大降低。

今天就来跟大家盘一盘，常见的 6 种会发生索引失效的场景。

**不仅会用实验案例给大家说明，也会清楚每个索引失效的原因**。

![图片](https://cdn.xiaolincoding.com//mysql/other/a9e6a9708a6dbbcc65906d1338d2ae70.png)

## [#](https://www.xiaolincoding.com/mysql/index/index_lose.html#索引存储结构长什么样)索引存储结构长什么样？

我们先来看看索引存储结构长什么样？因为只有知道索引的存储结构，才能更好的理解索引失效的问题。

索引的存储结构跟 MySQL 使用哪种存储引擎有关，因为存储引擎就是负责将数据持久化在磁盘中，而不同的存储引擎采用的索引数据结构也会不相同。

MySQL 默认的存储引擎是 InnoDB，它采用 B+Tree 作为索引的数据结构。

<font color=e111>在创建表时，InnoDB 存储引擎默认会创建一个 **主键索引**，也就是 **聚簇索引**，其它索引都属于 **二级索引**。</font>

* 聚簇索引的叶子节点存放的是实际数据，所有完整的用户数据都存放在聚簇索引的叶子节点
* 二级索引的叶子节点存放的是主键值，而不是实际数据

MySQL 的 MyISAM 存储引擎支持多种索引数据结构，比如 B+ 树索引、B 树索引、Full-Text 索引。MyISAM 存储引擎在创建表时，创建的主键索引默认使用的是 B+ 树索引。

虽然，InnoDB 和 MyISAM 都支持 B+ 树索引，但是它们数据的存储结构实现方式不同。不同之处在于：

- InnoDB 存储引擎：B+ 树索引的叶子节点保存数据本身；
- MyISAM 存储引擎：B+ 树索引的叶子节点保存数据的物理地址；

接下来，我举个例子，给大家展示下这两种存储引擎的索引存储结构的区别。

这里有一张 t_user 表，其中 id 字段为主键索引，其他都是普通字段。

![图片](https://cdn.xiaolincoding.com//mysql/other/00730eac527c21a4034f0f9ba0218aba.png)

如果使用的是 MyISAM 存储引擎，B+ 树索引的叶子节点保存数据的物理地址，即用户数据的指针，如下图：

![图片](https://cdn.xiaolincoding.com//mysql/other/f287701eba9bf6f32a2d09b013bb451b.png)

如果使用的是 InnoDB 存储引擎， B+ 树索引的叶子节点保存数据本身，如下图所示（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行）。

![图片](https://cdn.xiaolincoding.com//mysql/other/ef726d7287b854ea4862f7442d8012ec.png)

如果将 name 字段设置为普通索引，那么这个二级索引长下图这样（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行），**叶子节点仅存放主键值**。

![图片](https://cdn.xiaolincoding.com//mysql/other/7dd91be9fe584d94a4d71572ae7214d0.png)

知道了 InnoDB 存储引擎的聚簇索引和二级索引的存储结构后，接下来举几个查询语句，说下查询过程是怎么选择用哪个索引类型的。

在我们使用「主键索引」字段作为条件查询的时候，如果要查询的数据都在「聚簇索引」的叶子节点里，那么就会在「聚簇索引」中的 B+ 树检索到对应的叶子节点，然后直接读取要查询的数据。如下面这条语句：

```sql
// id 字段为主键索引
SELECT * FROM t_user WHERE id=1;
```

在我们使用「二级索引」字段作为条件查询的时候，如果要查询的数据都在「聚簇索引」的叶子节点里，那么需要检索两棵 B+ 树：

- 先在「二级索引」的 B+ 树找到对应的叶子节点，获取主键值；
- 然后用上一步获取的主键值，在「聚簇索引」中的 B+ 树检索到对应的叶子节点，然后获取要查询的数据。

上面这个过程叫做 **<font color=blue>回表</font>**，如下面这条语句：

```sql
// name 字段为二级索引
SELECT * FROM t_user WHERE name="林某";
```

在我们使用「二级索引」字段作为条件查询的时候，如果要查询的数据在「二级索引」的叶子节点，那么只需要在「二级索引」的 B+ 树找到对应的叶子节点，然后读取要查询的数据，这个过程叫做 **<font color=blue>覆盖索引</font>**。如下面这条语句：

```sql
// name 字段为二级索引
SELECT id FROM t_user WHERE name="林某";
```

上面这些查询语句的条件都用到了索引列，所以在查询过程都用上了索引。

但是并不意味着，查询条件用上了索引列，就查询过程就一定都用上索引，接下来我们再一起看看哪些情况会导致索引失效，而发生全表扫描。

首先说明下，下面的实验案例，我使用的 MySQL 版本为 `8.0.26`。

## 1. 对索引使用左或者左右模糊匹配

当我们使用左或者左右模糊匹配的时候，也就是 `like %xx` 或者 `like %xx%` 这两种方式都会造成索引失效。

比如下面的 like 语句，查询 name 后缀为「林」的用户，执行计划中的 type=ALL 就代表了全表扫描，而没有走索引。

```sql
// name 字段为二级索引
SELECT * FROM t_user WHERE name like '%林';
```

![图片](https://cdn.xiaolincoding.com//mysql/other/3175fdd8a7d00aa7edeb18bc90e63333.png)

如果是查询 name 前缀为林的用户，那么就会走索引扫描，执行计划中的 type=range 表示走索引扫描，key=index_name 看到实际走了 index_name 索引：

```sql
// name 字段为二级索引
SELECT * FROM t_user WHERE name like '林%';
```

![图片](https://cdn.xiaolincoding.com//mysql/other/c5c1506ac9fe3fdb98085e7e09c48394.png)

> 为什么 like 关键字左或者左右模糊匹配无法走索引呢？

<font color=ef11>因为索引 B+ 树是按照「索引值」有序排列存储的，只能根据前缀进行比较。</font>

举个例子，下面这张二级索引图（图中叶子节点之间我画了单向链表，但是实际上是双向链表，原图我找不到了，修改不了，偷个懒我不重画了，大家脑补成双向链表就行），是以 name 字段有序排列存储的。

![图片](https://cdn.xiaolincoding.com//mysql/other/b161c1d88f978e42077f8c14e10972a7.png)

假设我们要查询 name 字段前缀为「林」的数据，也就是 `name like '林%'`，扫描索引的过程：

- 首节点查询比较：林这个字的拼音大小比首节点的第一个索引值中的陈字大，但是比首节点的第二个索引值中的周字小，所以选择去节点 2 继续查询；
- 节点 2 查询比较：节点2的第一个索引值中的陈字的拼音大小比林字小，所以继续看下一个索引值，发现节点2有与林字前缀匹配的索引值，于是就往叶子节点查询，即叶子节点4；
- 节点 4 查询比较：节点4的第一个索引值的前缀符合林字，于是就读取该行数据，接着继续往右匹配，直到匹配不到前缀为林的索引值。

如果使用 `name like '%林'` 方式来查询，因为查询的结果可能是「陈林、张林、周林」等之类的，所以不知道从哪个索引值开始比较，于是就只能通过全表扫描的方式来查询。

## 2. 对索引使用函数/表达式

有时候我们会用一些 MySQL 自带的函数来得到我们想要的结果，这时候要注意了，如果查询条件中对索引字段使用函数，就会导致索引失效。

比如下面这条语句查询条件中对 name 字段使用了 LENGTH 函数，执行计划中的 type=ALL，代表了全表扫描：

```sql
// name 为二级索引
SELECT * FROM t_user WHERE length(name)=6;
```

![图片](https://cdn.xiaolincoding.com//mysql/other/2525f7b7190eeb70323579e8d665eb94.png)

> 为什么对索引使用函数，就无法走索引了呢？

<font color=ef11>因为索引保存的是索引字段的原始值，而不是经过函数计算后的值，自然就没办法走索引了。</font>

不过，从 MySQL 8.0 开始，索引特性增加了函数索引，即可以针对函数计算后的值建立一个索引，也就是说该索引的值是函数计算后的值，所以就可以通过扫描索引来查询数据。

举个例子，我通过下面这条语句，对 length(name) 的计算结果建立一个名为 idx_name_length 的索引。

```sql
alter table t_user add key idx_name_length ((length(name)));
```

然后我再用下面这条查询语句，这时候就会走索引了。

![图片](https://cdn.xiaolincoding.com//mysql/other/56fc9a5a9b35d933f810e5213b3c8acb.png)

<font color=ef11>另外需要说明的是，在查询条件中对索引进行 **表达式计算**，也是无法走索引的。</font>

比如，下面这条查询语句，执行计划中 type = ALL，说明是通过全表扫描的方式查询数据的：

```sql
explain SELECT * FROM t_user WHERE id + 1 = 10;
```

![图片](https://cdn.xiaolincoding.com//mysql/other/798ab1331d1d6dff026e262e788f1a28-20230309223731964.png)

但是，如果把查询语句的条件改成 WHERE id = 10 - 1，这样就不是在索引字段进行表达式计算了，于是就可以走索引查询了。

![图片](https://cdn.xiaolincoding.com//mysql/other/96c3132ade7b161a5c2a3c051b337402.png)

> 为什么对索引进行表达式计算，就无法走索引了呢？

<font color=ef11>原因跟对索引使用函数差不多。因为索引保存的是索引字段的原始值，而不是 id + 1 表达式计算后的值，所以无法走索引，只能通过把索引字段的取值都取出来，然后依次进行表达式的计算来进行条件判断，因此采用的就是全表扫描的方式。</font>

<font color=ee11ff>有的同学可能会说，这种对索引进行简单的表达式计算，在代码特殊处理下，应该是可以做到索引扫描的，比方将 id + 1 = 10 变成 id = 10 - 1。</font>

<font color=ee11ff>是的，是能够实现，但是 MySQL 还是偷了这个懒，没有实现。</font>

我的想法是，可能也是因为，表达式计算的情况多种多样，每种都要考虑的话，代码可能会很臃肿，所以干脆将这种索引失效的场景告诉程序员，让程序员自己保证在查询条件中不要对索引进行表达式计算。

## 3. 对索引隐式类型转换

如果索引字段是字符串类型，但是在条件查询中，输入的参数是整型的话，你会在执行计划的结果发现这条语句会走全表扫描。

我在原本的 t_user 表增加了 phone 字段，是二级索引且类型是 varchar。

![图片](https://cdn.xiaolincoding.com//mysql/other/e7aa91ce5e699ff0c6c357f6b1e70597.png)

然后我在条件查询中，用整型作为输入参数，此时执行计划中 type = ALL，所以是通过全表扫描来查询数据的。

```sql
SELECT * FROM t_user WHERE phone = 1300000001;
```

![图片](https://cdn.xiaolincoding.com//mysql/other/7be4dcbe00f15c8c583f20ffd67b772a.png)

但是如果索引字段是整型类型，查询条件中的输入参数即使字符串，是不会导致索引失效，还是可以走索引扫描。

我们再看第二个例子，id 是整型，但是下面这条语句还是走了索引扫描的。

```sql
 explain SELECT * FROM t_user WHERE id = '1';
```

![图片](https://cdn.xiaolincoding.com//mysql/other/028df5ce4a42a6cf33524b2e1102c15d.png)

> 为什么第一个例子会导致索引失效，而第二例子不会呢？

<font color=blue>要明白这个原因，首先我们要知道 MySQL 的数据类型转换规则是什么？就是看 **MySQL 是会将字符串转成数字处理，还是将数字转换成字符串处理？**</font>

我在看《mysql45讲的时候》看到一个简单的测试方式，就是通过 <font color=red>`SELECT "10" > 9` 的结果来知道 MySQL 的数据类型转换规则是什么：</font>

- <font color=red>如果规则是 MySQL 会将自动「字符串」转换成「数字」，就相当于 `SELECT 10 > 9`，这个就是数字比较，所以结果应该是 1；</font>
- <font color=red>如果规则是 MySQL 会将自动「数字」转换成「字符串」，就相当于 SELECT "10" > "9"，这个是字符串比较，字符串比较大小是逐位从高位到低位逐个比较（按 ASCII 码） ，那么"10"字符串相当于 “1”和“0”字符的组合，所以先是拿 “1” 字符和 “9” 字符比较，因为 “1” 字符比 “9” 字符小，所以结果应该是 0。</font>

在 MySQL 中，执行的结果如下图：

![图片](https://cdn.xiaolincoding.com//mysql/other/25316fd06cadff4434f02391ef0b5c55.png)

上面的结果为 1，说明 **<font color=24b3aa>MySQL 在遇到字符串和数字比较的时候，会自动把 <u>字符串转为数字</u>，然后再进行比较</font>**。

前面的例子一中的查询语句，我也跟大家说了是会走全表扫描：

```sql
//例子一的查询语句
SELECT * FROM t_user WHERE phone = 1300000001;
```

这是因为 phone 字段为字符串，所以 MySQL 要会自动把字符串转为数字，所以这条语句相当于：

```sql
SELECT * FROM t_user WHERE CAST(phone AS signed int) = 1300000001;
```

可以看到，**CAST 函数是作用在了 phone 字段，而 phone 字段是索引，也就是对索引使用了函数！而前面我们也说了，对索引使用函数是会导致索引失效的**。

例子二中的查询语句，我跟大家说了是会走索引扫描：

```sql
//例子二的查询语句
SELECT * FROM t_user WHERE id = "1";
```

这时因为字符串部分是输入参数，也就需要将字符串转为数字，所以这条语句相当于：

```sql
SELECT * FROM t_user WHERE id = CAST("1" AS signed int);
```

可以看到，索引字段并没有用任何函数，CAST 函数是用在了输入参数，因此是可以走索引扫描的。

## 4. 联合索引非最左匹配

对主键字段建立的索引叫做聚簇索引，对普通字段建立的索引叫做二级索引。

那么 **<font color=blue>多个普通字段组合在一起创建的索引就叫做 <u>联合索引</u>，也叫 <u>组合索引</u>。</font>**

创建联合索引时，我们需要注意创建时的顺序问题，因为联合索引 (a, b, c) 和 (c, b, a) 在使用的时候会存在差别。

联合索引要能正确使用需要遵循 **<font color=ef11>最左匹配原则</font>**，也就是按照最左优先的方式进行索引的匹配。

比如，如果创建了一个 (a,b,c) 联合索引，如果查询条件是以下这几种，就可以匹配上联合索引：

- WHERE a=1；
- WHERE a=1 and b=2 and c=3；
- WHERE a=1 and b=2；

**需要注意的是，因为有查询优化器，所以 a 字段在 WHERE 子句的顺序并不重要。**

但是，如果查询条件是以下这几种，因为不符合最左匹配原则，所以就无法匹配上联合索引，联合索引就会失效:

- WHERE b=2；
- WHERE c=3；
- WHERE b=2 and c=3；

有一个比较特殊的查询条件：WHERE a = 1 and c = 3 ，符合最左匹配吗？

这种其实严格意义上来说是属于 <font color=red>**索引截断**</font>，不同版本处理方式也不一样。

* MySQL 5.5 的话，前面 a 会走索引，在联合索引找到主键值后，开始回表，到主键索引读取数据行，Server 层从存储引擎层获取到数据行后，然后在 Server 层再比对 c 字段的值。
* 从 MySQL 5.6 之后，有一个 **<font color=red>索引下推功能</font>**，可以在存储引擎层进行索引遍历过程中，对索引中包含的字段先做判断，直接过滤掉不满足条件的记录，再返还给 Server 层，从而减少回表次数。

<font color=ef11>索引下推的大概原理是：截断的字段不会在 Server 层进行条件判断，而是会被下推到「存储引擎层」进行条件判断（因为 c 字段的值是在 `(a, b, c)` 联合索引里的），然后过滤出符合条件的数据后再返回给 Server 层。由于在引擎层就过滤掉大量的数据，无需再回表读取数据来进行判断，减少回表次数，从而提升了性能。</font>

比如下面这条 WHERE a = 1 and c = 0 语句，我们可以从执行计划中的 Extra=Using index condition 使用了索引下推功能。

![图片](https://cdn.xiaolincoding.com//mysql/other/28f252422b6c5e49cc81c448d08547dc.png)

> 为什么联合索引不遵循最左匹配原则就会失效？

原因是，在联合索引的情况下，数据是按照索引第一列排序，第一列数据相同时才会按照第二列排序。

也就是说，如果我们想使用联合索引中尽可能多的列，查询条件中的各个列必须是联合索引中从最左边开始连续的列。如果我们仅仅按照第二列搜索，肯定无法走索引。

## 5. WHERE 子句中的 OR

在 WHERE 子句中，如果在 OR 前的条件列是索引列，而在 OR 后的条件列不是索引列，那么索引会失效。

举个例子，比如下面的查询语句，id 是主键，age 是普通列，从执行计划的结果看，是走了全表扫描。

```sql
SELECT * FROM t_user WHERE id = 1 or age = 18;
```

![图片](https://cdn.xiaolincoding.com//mysql/other/550dc58c222cc8d2740985e8d6497821.png)

这是因为 <font color=blue>OR 的含义就是两个只要满足一个即可，因此只有一个条件列是索引列是没有意义的，只要有条件列不是索引列，就会进行全表扫描。</font>

要解决办法很简单，将 age 字段设置为索引即可。

![图片](https://cdn.xiaolincoding.com//mysql/other/7f72c4ef72b7b979c844798d7be06916.png)

可以看到 type=index merge， index merge 的意思就是对 id 和 age 分别进行了扫描，然后将这两个结果集进行了合并，这样做的好处就是避免了全表扫描。

## [#](https://www.xiaolincoding.com/mysql/index/index_lose.html#总结)总结

今天给大家介绍了 5 种会发生索引失效的情况：

1. 当我们使用左或者左右模糊匹配的时候，也就是 `like %xx` 或者 `like %xx%`这两种方式都会造成索引失效；
2. 当我们在查询条件中对索引列使用函数/表达式计算，就会导致索引失效。
3. MySQL 在遇到字符串和数字比较的时候，会自动 **<font color=red>把字符串转为数字</font>**，然后再进行比较。如果字符串是索引列，而条件语句中的输入参数是数字的话，那么索引列会发生隐式类型转换，由于隐式类型转换是通过 CAST 函数实现的，等同于对索引列使用了函数，所以就会导致索引失效。
4. 联合索引要能正确使用需要遵循最左匹配原则，也就是按照最左优先的方式进行索引的匹配，否则就会导致索引失效。
5. 在 WHERE 子句中，如果在 OR 前的条件列是索引列，而在 OR 后的条件列不是索引列，那么索引会失效。

总结一下，索引失效笼统上来说可以分为两类：

1. <font color=ee11ff>违背最左匹配</font>
   * 模糊查询不是最左匹配
   * 聚合索引不是最左匹配
   * where 字句中 or 包含非索引列
2. <font color=ee11ff>查询的不是原始索引值</font>
   * 索引值上有隐式类型转换
   * 索引值上有显示的函数/表达式计算

另外还有一种我们自己使用没问题，但<font color=ee11ff>优化器主动选择全盘扫描</font>的情况：例如返回数据量过大，优化器判断全表扫描成本更低，主动选择全盘扫描。（索引主要是可能有很大的回表成本）

# [索引]常见问题

## #mysql的explain有什么作用？

explain 是查看 sql 的执行计划，主要用来分析 sql 语句的执行过程，比如有没有走索引，有没有外部排序，有没有索引覆盖等等。

如下图，就是一个没有使用索引，并且是一个全表扫描的查询语句。

![img](https://cdn.xiaolincoding.com//picgo/1720420604941-9fafd933-6a90-4f02-a23c-0e577790f040.webp)

对于执行计划，参数有：

- possible_keys 字段表示可能用到的索引；
- key 字段表示实际用的索引，如果这一项为 NULL，说明没有使用索引；
- key_len 表示索引的长度；
- rows 表示扫描的数据行数。
- type 表示数据扫描类型，我们需要重点看这个。

type 字段就是描述了找到所需数据时使用的扫描方式是什么，常见扫描类型的**执行效率从低到高的顺序为**：

- All（全表扫描）：在这些情况里，all 是最坏的情况，因为采用了全表扫描的方式。
- index（全索引扫描）：index 和 all 差不多，只不过 index 对索引表进行全扫描，这样做的好处是不再需要对数据进行排序，但是开销依然很大。所以，要尽量避免全表扫描和全索引扫描。
- range（索引范围扫描）：range 表示采用了索引范围扫描，一般在 WHERE 子句中使用 < 、>、in、between 等关键词，只检索给定范围的行，属于范围查找。**从这一级别开始，索引的作用会越来越明显，因此我们需要尽量让 SQL 查询可以使用到 range 这一级别及以上的 type 访问方式**。
- ref（非唯一索引扫描）：ref 类型表示采用了非唯一索引，或者是唯一索引的非唯一性前缀，返回数据返回可能是多条。因为虽然使用了索引，但该索引列的值并不唯一，有重复。这样即使使用索引快速查找到了第一条数据，仍然不能停止，要进行目标值附近的小范围扫描。但它的好处是它并不需要扫全表，因为索引是有序的，即便有重复值，也是在一个非常小的范围内扫描。
- eq_ref（唯一索引扫描）：eq_ref 类型是使用主键或唯一索引时产生的访问方式，通常使用在多表联查中。比如，对两张表进行联查，关联条件是两张表的 user_id 相等，且 user_id 是唯一索引，那么使用 EXPLAIN 进行执行计划查看的时候，type 就会显示 eq_ref。
- const（结果只有一条的主键或唯一索引扫描）：const 类型表示使用了主键或者唯一索引与常量值进行比较，比如 SELECT name FROM product WHERE id=1。需要说明的是 const 类型和 eq_ref 都使用了主键或唯一索引，不过这两个类型有所区别，**const 是与常量进行比较，查询效率会更快，而 eq_ref 通常用于多表联查中**。

extra 显示的结果，这里说几个重要的参考指标：

- Using filesort ：当查询语句中包含 group by 操作，而且无法利用索引完成排序操作的时候， 这时不得不选择相应的排序算法进行，甚至可能会通过文件排序，效率是很低的，所以要避免这种问题的出现。
- Using temporary：使了用临时表保存中间结果，MySQL 在对查询结果排序时使用临时表，常见于排序 ORDER BY 和分组查询 group by。效率低，要避免这种问题的出现。
- Using index：所需数据只需在索引即可全部获得，不须要再到表中取数据，也就是使用了覆盖索引，避免了回表操作，效率不错。

## #聚簇索引和非聚簇索引的区别

| 特性         | 聚簇索引           | 非聚簇索引（二级索引）       |
| ------------ | ------------------ | ---------------------------- |
| 叶子节点内容 | 完整的行数据       | 索引列值 + 主键值            |
| 每表数量     | 有且仅有 1 个      | 可以有多个                   |
| 物理存储     | 数据按索引顺序存储 | 与数据物理顺序无关           |
| 查询特点     | 直接获取数据       | 通常需要回表（除非覆盖索引） |

“聚簇”不仅仅是一种索引结构，它决定了**数据在磁盘上的物理存储顺序**。

- **聚簇索引：** 数据的物理存储顺序与索引顺序一致。因此，一张表**只能有一个**聚簇索引。
- **非聚簇索引：** 索引顺序与数据物理存储顺序无关。一张表可以有**多个**非聚簇索引。

## #什么字段适合当做主键？

- 字段具有 <font color=blue>唯一性</font>，且 <font color=blue>不能为空</font> 的特性
- 字段最好的是有 <font color=blue>递增的趋势</font> 的，如果字段的值是随机无序的，可能会引发页分裂的问题，造型性能影响。
- <font color=blue>不建议用业务数据作为主键</font>，比如会员卡号、订单号、学生号之类的，因为我们无法预测未来会不会因为业务需要，而出现业务字段重复或者重用的情况。
- <font color=blue>通常情况下会用自增字段来做主键</font>，对于单机系统来说是没问题的。但是，如果有多台服务器，各自都可以录入数据，那就不一定适用了。因为如果每台机器各自产生的数据需要合并，就可能会出现主键重复的问题，这时候就需要考虑 <font color=blue>分布式 id</font> 的方案了。

## #B+树的叶子节点链表是单向还是双向？

<font color=blue>双向的，为了实现倒序遍历或者排序。</font>

![image-20240725232027951](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.45idqygqf4.png)

## #如果一个列即使单列索引，又是联合索引，单独查它的话先走哪个？

mysql 优化器会分析每个索引的查询成本，然后选择成本最低的方案来执行 sql。

如果单列索引是 a，联合索引是（a ，b），那么针对下面这个查询：

```sql
SELECT a, b FROM table WHERE a = x and b = y
```

优化器会选择联合索引，因为查询成本更低，查询也不需要回表，直接索引覆盖了。

## #什么自增ID更快一些，UUID不快吗，它在 B+ 树里面存储是有序的吗?

自增的主键的值是顺序的，所以 Innodb 把每一条记录都存储在一条记录的后面，所以自增 id 更快的原因：

- 下一条记录就会写入新的页中，一旦数据按照这种顺序的方式加载，主键页就会近乎于顺序的记录填满，提升了页面的最大填充率，不会有页的浪费
- 新插入的行一定会在原有的最大数据行下一行，mysql 定位和寻址很快，不会为计算新行的位置而做出额外的消耗
- 减少了页分裂和碎片的产生

但是 UUID 不是递增的，MySQL 中索引的数据结构是 B+Tree，这种数据结构的特点是索引树上的节点的数据是有序的，而如果使用 UUID 作为主键，那么每次插入数据时，因为无法保证每次产生的 UUID 有序，所以就会出现新的 UUID 需要插入到索引树的中间去，这样可能会频繁地导致页分裂，使性能下降。

> UUID（Universally Unique Identifier，通用唯一识别码）是一种由 **128 位二进制数**组成的标识符标准，用于在分布式系统中无需中央协调即可生成全局唯一的 ID。
>
> - **长度固定：** 128 bit，通常以 32 个十六进制字符 + 4 个连字符表示，共 36 个字符。
>   例如：`550e8400-e29b-41d4-a716-446655440000`
> - **全局唯一：** 不依赖数据库自增、不依赖中心化服务，任何节点独立生成的 UUID 碰撞概率极低（理论上可忽略）。
> - **无业务含义：** 纯粹的技术标识，不包含时间、机器、序列等业务信息（部分版本除外）。
>
> UUID 是一种**无需中心协调、全局唯一**的 128 位标识符标准，最适合分布式环境；但在 MySQL 中用作主键时需注意**有序性问题**，优先考虑 v7 或雪花算法。

而且，UUID 太占用内存。每个 UUID 由 36 个字符组成，在字符串进行比较时，需要从前往后比较，字符串越长，性能越差。另外字符串越长，占用的内存越大，由于页的大小是固定的，这样一个页上能存放的关键字数量就会越少，这样最终就会导致索引树的高度越大，在索引搜索的时候，发生的磁盘 IO 次数越多，性能越差。

## #**怎么查看表的索引？**

最直接的是 `SHOW INDEX` 命令，语法简单，能清晰列出表的所有索引信息，包括索引名、索引类型、对应字段、是否主键等。比如查看 user 表的索引，执行：

```sql
SHOW INDEX FROM user;
```

执行后会返回多列结果，关键信息很直观：Key_name 是索引名称（主键索引默认叫 PRIMARY），Column_name 是索引对应的字段，Index_type 是索引类型（比如 BTREE、HASH），Non_unique 字段为 0 表示唯一索引（主键索引也是唯一索引），为 1 表示普通非唯一索引。

如果想更简洁地查看索引名称和对应字段，也可以用 `SHOW KEYS`，它和 `SHOW INDEX` 功能完全一致，只是写法不同：

```sql
SHOW KEYS FROM user;
```

另外，MySQL 的信息 _schema 数据库中存储了所有表的元数据，通过查询 `STATISTICS` 表能获取更详细的索引信息，适合需要筛选或批量查询的场景。比如查询 user 表的索引，执行：

```sql
SELECT INDEX_NAME, COLUMN_NAME, INDEX_TYPE, NON_UNIQUE 
FROM information_schema.STATISTICS 
WHERE TABLE_SCHEMA = '你的数据库名' AND TABLE_NAME = 'user';
```

这里需要替换成实际的数据库名，结果会和`SHOW INDEX`一致，但支持更灵活的条件查询，比如只查唯一索引（加`AND NON_UNIQUE = 0`）。

## #如果Explain用到的索引不正确的话，有什么办法干预吗？

可以使用 force index，强制走索引。例如：

```sql
EXPLAIN 
SELECT productName, buyPrice
FROM products 
FORCE INDEX (idx_buyprice)
WHERE buyPrice BETWEEN 10 AND 80
ORDER BY buyPrice; 
```

输出：

![img](https://cdn.xiaolincoding.com//picgo/1715425169012-fcd6a89d-c073-4f3c-a395-c70d8045eec7.png)

# [锁]MySQL 有哪些锁？

在 MySQL 里，根据加锁的范围，可以分为**全局锁、表级锁和行锁**三类。

![img](https://cdn.xiaolincoding.com//mysql/other/1e37f6994ef44714aba03b8046b1ace2.png)

## [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#全局锁)全局锁

> <font color=24b3aa>全局锁是怎么用的？</font>

要使用全局锁，则要执行这条命令：

```sql
flush tables with read lock
```

`FLUSH TABLES WITH READ LOCK` 简写为 **FTWRL**，是 MySQL 提供的一种**全局读锁**（Global Read Lock）命令。顾名思义，它的核心作用是强制刷新所有打开的表，并对整个数据库实例加上全局的只读锁。

这是一种非常“重”的锁机制。下面为你详细拆解 FTWRL 的工作原理、使用场景、潜在风险以及现代替代方案。

> **1. FTWRL 的执行过程与底层逻辑**

当你执行 `FLUSH TABLES WITH READ LOCK;` 时，MySQL 实际上在后台悄悄做了两件核心的事情：

- **Flush Tables（刷新表）：** MySQL 会等待所有当前正在执行的语句结束，然后关闭所有当前打开的表，清空表缓存。这一步会将内存中未落盘的脏数据（如果有必要的话）刷新到磁盘上。
- **With Read Lock（加读锁）：** 对整个数据库实例（包含所有库、所有表）加上全局共享读锁。

**被加锁后的数据库状态：**

- **允许的操作：** 所有客户端（包括当前执行 FTWRL 的客户端）只能执行读操作（`SELECT`）。
- **被阻塞的操作：**
  * 所有的 **DML**（数据操纵语言）：`INSERT`、`UPDATE`、`DELETE`。
  * 所有的 **DDL**（数据定义语言）：`ALTER TABLE`、`DROP TABLE`、`CREATE TABLE` 等。
  * 更新类命令：例如 `GRANT` 等修改系统表的命令。

释放该锁的唯一正常途径是执行：

```sql
UNLOCK TABLES;
```

*(注：如果执行 FTWRL 的客户端连接断开，MySQL 也会自动释放这个全局锁。)*

> **2. 为什么需要 FTWRL？（核心使用场景）**

FTWRL 最经典、也是几乎唯一的使用场景就是：**全库数据备份，以获取一致性视图。**

在早期的 MySQL 中，或者在混合使用了 InnoDB 和 MyISAM 等不同存储引擎的场景下，为了保证备份出来的跨表、跨库数据在时间点上是绝对一致的（比如订单表和库存表的数据必须匹配），DBA 必须让整个数据库“静止”下来。

FTWRL 就是让数据库瞬间静止的“暂停键”。备份工具（如早期的 `mysqldump` 或物理备份工具）会在备份前自动执行此命令，备份完成后再解锁。

> **3. FTWRL 的致命风险与副作用**

由于 FTWRL 会让整个数据库变成只读状态，在生产环境中使用它具有极高的风险：

- **业务停顿：** 锁表期间，所有的写请求都会被阻塞，导致前端业务出现卡顿、超时或报错。
- **主从延迟激增：** 如果在从库（Slave）上执行 FTWRL 进行备份，从库的 SQL 线程会被阻塞，无法回放主库同步过来的 Binlog，导致主从延迟（Seconds_Behind_Master）迅速拉大。
- **长查询引发的“灾难”：** 这是最容易踩坑的地方。如果执行 FTWRL 时，数据库中正好有一个**长查询（Long-running Query）**正在执行，FTWRL 必须等待这个长查询执行完毕才能获取到锁。更糟糕的是，由于 FTWRL 的锁优先级很高，**它在等待的过程中，会阻塞其后发起的所有读写请求**。这可能导致数据库连接数瞬间打满，整个数据库呈现假死状态。

> **警示：** 永远不要在高峰期的主库上手动执行 `FLUSH TABLES WITH READ LOCK`。

> **4. 现代 MySQL 中的替代方案**

随着 InnoDB 成为 MySQL 的绝对主流引擎，DBA 们已经很少直接面对 FTWRL 的痛苦了，因为有了更好的替代方案：

- **逻辑备份的替代方案：MVCC** 如果你的数据库全部是 InnoDB 引擎，由于 InnoDB 支持多版本并发控制（MVCC）和事务，你可以使用 `mysqldump` 的 `--single-transaction` 参数。它会在备份开始前开启一个可重复读（Repeatable Read）的事务，依靠事务隔离级别来获取一致性视图，**完全不需要锁表**，不影响业务的正常读写。

- **物理备份的替代方案：Redo Log** 像 Percona XtraBackup 这样的物理热备工具，在备份 InnoDB 表时，是通过拷贝数据文件并同时持续监控记录 Redo Log 来实现的，不需要阻塞 DML 操作。（不过要注意，XtraBackup 在备份最后的非 InnoDB 元数据阶段，早期版本依然会短暂触发表锁或 FTWRL）。

- **MySQL 8.0+ 的备份锁（Backup Lock）** MySQL 8.0 引入了更细粒度的备份锁：

  ```sql
  LOCK INSTANCE FOR BACKUP;
  ```

  这个命令比 FTWRL 优雅得多。它**允许 DML**（你可以继续 INSERT/UPDATE/DELETE），**仅阻塞 DDL**（防止在备份期间表结构被修改或文件被删除）。这极大地减轻了备份对在线业务的影响。

总结来说，FTWRL 是一把历史悠久的“重锤”，为了保证数据的一致性它选择牺牲可用性。在纯 InnoDB 和 MySQL 8.0 环境下，我们应尽量利用事务特性和实例级备份锁来替代它。

## [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#表级锁)表级锁

> MySQL 表级锁有哪些？具体怎么用的。

MySQL 里面表级别的锁有这几种：

- 表锁；
- 元数据锁（MDL）;
- 意向锁；
- AUTO-INC 锁；

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#表锁)表锁

先来说说 **表锁**。

如果我们想对学生表（t_student）加表锁，可以使用下面的命令：

```sql
# 表级别的共享锁，也就是读锁；
# 允许当前会话读取被锁定的表，但阻止其他会话对这些表进行写操作。
lock tables t_student read;

# 表级别的独占锁，也就是写锁；
# 允许当前会话对表进行读写操作，但阻止其他会话对这些表进行任何操作（读或写）。
lock tables t_student write;
```

<font color=24b3aa>需要注意的是，表锁除了会限制别的线程的读写外，**也会限制本线程接下来的读写操作**。</font>

举个例子， 如果在某个线程 A 中执行 `lock tables t1 read, t2 write` 这个语句，则其他线程写 t1、读写 t2 的语句都会被阻塞。同时，线程 A 在执行 unlock tables 之前，也只能执行读 t1、读写 t2 的操作，而不能写 t1。

> 很多人会误以为“我加了锁，我就是老大，我想干嘛就干嘛”，但实际上 `LOCK TABLES` 对加锁线程自身的限制也非常严格：
>
> - **<font color=blue>不能越权操作：</font>** 虽然是你加的锁，但既然声明了是 `read`，线程 A 自身也绝对**不允许写 t1**。
> - **<font color=blue>视野被禁锢（不能访问未加锁的表）：</font>** 这一点尤为重要。一旦线程 A 执行了 `LOCK TABLES`，它的“视野”就被限制在了锁定的这几张表里。如果此时线程 A 尝试去 `SELECT t3`，MySQL 会直接报错，而不会去隐式加锁。
>
> 为什么 MySQL 要这样设计？
>
> - **防止死锁与逻辑混乱：** 如果允许加锁线程“越权”或“隐式加锁”，在复杂的业务逻辑中极易产生不可预知的锁依赖链，导致死锁检测变得极其困难。
> - **强制显式声明：** `LOCK TABLES` 的设计哲学是 **“所见即所得”**。它要求开发者在执行前就必须想清楚需要哪些表、需要什么级别的锁。这种“笨拙”恰恰是一种安全机制，迫使开发者审视 SQL 逻辑。
> - **性能考量：** 隐式加锁意味着 MySQL 需要在运行时动态解析 SQL 并申请锁资源，这会带来额外的开销和不确定性。直接报错比运行时猜测意图更高效、更安全。

需要注意的是，线程 A 在对表 t1不和 t2 加锁后，就不能访问其他表，也就是上面提到的 <font color=24b3aa>**“不能访问未加锁的表”**</font>。

这是为了**<font color=blue>防止死锁</font>**。如果允许线程 A 在持有 t1、t2 锁的情况下再去访问 t3，而此时另一个线程 B 持有 t3 的锁并等待 t1，就会发生死锁。因此，MySQL 强制要求：**<font color=ee11ff>你在执行 `LOCK TABLES` 时，必须一次性把你后续操作需要用到的所有表全都锁上</font>。**

要释放表锁，可以使用下面这条命令，会释放当前会话的所有表锁：

```sql
unlock tables
```

另外，<font color=24b3aa>当会话退出后，也会释放所有表锁。</font>

> <FONT COLOR=BLUE>`UNLOCK TABLES` 的作用是释放当前会话（Session）持有的**所有**显式表级锁。由于 <u>FTWRL 在底层被视为一种特殊的全局表级锁</u>，所以它也遵循这个规则。</FONT>

在还没有出现更细粒度的锁的时候，表锁是最常用的处理并发的方式，不过尽量避免在使用 InnoDB 引擎的表使用表锁，因为表锁的颗粒度太大，会影响并发性能，**InnoDB 牛逼的地方在于实现了颗粒度更细的行级锁**。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#元数据锁)元数据锁

<font color=24b3aa>MDL（元数据锁） 是为了保证当用户对表执行 CRUD 操作时，防止其他线程对这个 **表结构** 做了变更。</font>

MDL 锁的“元数据”是什么？

不是行数据，而是这些东西：

- 表/库的定义：列、索引、约束、分区、字符集、表名…
- 以及一些需要保证结构不变的访问（比如执行查询时依赖表结构）

<font color=blue>我们不需要显式的使用 MDL，因为当我们对数据库表进行操作时，会 **自动** 给这个表加上 MDL：</font>

- <font color=24b3aa>对一张表进行 CRUD 操作时，加的是 **MDL 读锁**；</font>
- <font color=24b3aa>对一张表做结构变更操作的时候，加的是 **MDL 写锁**；</font>

当有线程在执行 SELECT 语句（ 加 MDL 读锁）的期间，如果有其他线程要更改该表的结构（ 申请 MDL 写锁），那么将会被阻塞，直到执行完 SELECT 语句（ 释放 MDL 读锁）。

反之，当有线程对表结构进行变更（ 加 MDL 写锁）的期间，如果有其他线程执行了 CRUD 操作（ 申请 MDL 读锁），那么就会被阻塞，直到表结构变更完成（ 释放 MDL 写锁）。

> MDL 不需要显式调用，那它是在什么时候释放的?

MDL 是在事务提交后才会释放，这意味着**事务执行期间，MDL 是一直持有的**。

那如果数据库有一个长事务（所谓的长事务，就是开启了事务，但是一直还没提交），那在对表结构做变更操作的时候，可能会发生意想不到的事情，比如下面这个顺序的场景：

1. 首先，线程 A 先启用了事务（但是一直不提交），然后执行一条 SELECT 语句，此时就先对该表加上 MDL 读锁；
2. 然后，线程 B 也执行了同样的 SELECT 语句，此时并不会阻塞，因为「读读」并不冲突；
3. 接着，线程 C 修改了表字段，此时由于线程 A 的事务并没有提交，也就是 MDL 读锁还在占用着，这时线程 C 就无法申请到 MDL 写锁，就会被阻塞，

那么在线程 C 阻塞后，后续有对该表的 SELECT 语句，就都会被阻塞，如果此时有大量该表的 SELECT 语句的请求到来，就会有大量的线程被阻塞住，这时数据库的线程很快就会爆满了。

> 为什么线程 C 因为申请不到 MDL 写锁，而导致后续的申请读锁的查询操作也会被阻塞？

这是因为申请 MDL 锁的操作会形成一个队列，队列中 **<font color=blue>写锁获取优先级高于读锁</font>**，<font color=24b3aa>一旦出现 MDL 写锁等待，会阻塞后续该表的所有 CRUD 操作。</font>

所以为了能安全的对表结构进行变更，在对表结构变更前，先要看看数据库中的长事务，是否有事务已经对表加上了 MDL 读锁，如果可以考虑 kill 掉这个长事务，然后再做表结构的变更。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#意向锁)意向锁

接着，说说 **意向锁**。

- 在使用 InnoDB 引擎的表里对某些 <font color=blue>记录</font> 加上「共享锁」之前，需要先在 <font color=blue>表级别</font> 加上一个「意向共享锁」；
- 在使用 InnoDB 引擎的表里对某些 <font color=blue>记录</font> 加上「独占锁」之前，需要先在 <font color=blue>表级别</font> 加上一个「意向独占锁」；

也就是，当执行 <font color=blue>插入、更新、删除</font> 操作，需要先对表加上「意向独占锁」，然后对该记录加独占锁。

而 <font color=blue>普通的 SELECT</font> 是不会加行级锁的，普通的 SELECT 语句是利用 MVCC 实现一致性读，是无锁的。不过，SELECT 也是可以对记录加共享锁和独占锁的，具体方式如下：

```sql
# 先在表上加上意向共享锁，然后对读取的记录加共享锁
SELECT ... lock in share mode;

# 先表上加上意向独占锁，然后对读取的记录加独占锁
SELECT ... FOR UPDATE;
```

<font color=ee11ff>意向共享锁和意向独占锁是表级锁，不会和行级的共享锁和独占锁发生冲突，而且意向锁之间也不会发生冲突，只会和共享表锁（`lock tables ... read`）和独占表锁（`lock tables ... write`）发生冲突。</font>

<font color=24b3aa>意向锁的目的是为了 **<font color=red>加速表级锁和行级锁的冲突判断</font>**。这首先基于一个事实：表锁和行锁是满足读读共享、读写互斥、写写互斥的。</font>

<font color=24b3aa>如果没有「意向锁」，那么加「独占表锁」时，就需要遍历表里所有记录，查看是否有记录存在独占锁，显而易见这样效率非常低。</font>

<font color=24b3aa>那么有了「意向锁」，**<font color=red>由于在对记录加独占锁前，先会加上表级别的意向独占锁</font>**，那么在加「独占表锁」时，直接查该表是否有意向独占锁，如果有就意味着表里已经有记录被加了独占锁，这样就不用去遍历表里的记录。</font>

所以，**<font color=blue>意向锁的目的是为了快速判断表里是否有记录被加锁</font>**。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#auto-inc-锁)AUTO-INC 锁

表里的主键通常都会设置成自增的，这是通过对主键字段声明 `AUTO_INCREMENT` 属性实现的。

之后可以在插入数据时，可以不指定主键的值，数据库会自动给主键赋值递增的值，这主要是通过 **AUTO-INC 锁**实现的。

<font color=red>AUTO-INC 锁是特殊的表锁机制，锁 **不是再一个事务提交后才释放，而是再执行完插入语句后就会立即释放**。</font>

**在插入数据时，会加一个表级别的 AUTO-INC 锁**，然后为被 `AUTO_INCREMENT` 修饰的字段赋值递增的值，等插入语句执行完成后，才会把 AUTO-INC 锁释放掉。

那么，一个事务在持有 AUTO-INC 锁的过程中，其他事务的如果要向该表插入语句都会被阻塞，从而保证插入数据时，被 `AUTO_INCREMENT` 修饰的字段的值是连续递增的。

但是， AUTO-INC 锁再对大量数据进行插入的时候，会影响插入性能，因为另一个事务中的插入会被阻塞。

因此， <font color=24b3aa>在 MySQL 5.1.22 版本开始，InnoDB 存储引擎提供了一种 **轻量级的锁** 来实现自增。</font>

<font color=24b3aa>一样也是在插入数据的时候，会为被 `AUTO_INCREMENT` 修饰的字段加上轻量级锁，**然后给该字段赋值一个自增的值，就把这个轻量级锁释放了，而不需要等待整个插入语句执行完后才释放锁**。</font>

InnoDB 存储引擎提供了个 innodb_autoinc_lock_mode 的系统变量，是用来控制选择用 AUTO-INC 锁，还是轻量级的锁。

- 当 innodb_autoinc_lock_mode = 0，就采用 AUTO-INC 锁，语句执行结束后才释放锁；
- 当 innodb_autoinc_lock_mode = 2，就采用轻量级锁，申请自增主键后就释放锁，并不需要等语句执行后才释放。
- 当 innodb_autoinc_lock_mode = 1：
  - 普通 insert 语句，自增锁在申请之后就马上释放；
  - 类似 insert … SELECT 这样的批量插入数据的语句，自增锁还是要等语句结束后才被释放；

<font color=24b3aa>当 innodb_autoinc_lock_mode = 2 是性能最高的方式，但是当搭配 binlog 的日志格式是 statement 一起使用的时候，在「主从复制的场景」中会发生 **数据不一致的问题**（这时需要修改日志记录格式为 row，但这样日志体积会变大）。</font>

举个例子，考虑下面场景：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/innodb_autoinc_lock_mode=2.png)

session A 往表 t 中插入了 4 行数据，然后创建了一个相同结构的表 t2，然后 **两个 session 同时执行向表 t2 中插入数据**。

如果 innodb_autoinc_lock_mode = 2，意味着「申请自增主键后就释放锁，不必等插入语句执行完」。那么就可能出现这样的情况：

- session B 先插入了两个记录，(1,1,1)、(2,2,2)；
- 然后，session A 来申请自增 id 得到 id=3，插入了（3,5,5)；
- 之后，session B 继续执行，插入两条记录 (4,3,3)、 (5,4,4)。

可以看到，**session B 的 insert 语句，生成的 id 不连续**。

当「主库」发生了这种情况，binlog 面对 t2 表的更新只会记录这两个 session 的 insert 语句，如果 binlog_format=statement，记录的语句就是原始语句。记录的顺序要么先记 session A 的 insert 语句，要么先记 session B 的 insert 语句。

但不论是哪一种，这个 binlog 拿去「从库」执行，这时从库是按「顺序」执行语句的，只有当执行完一条 SQL 语句后，才会执行下一条 SQL。因此，在**从库上「不会」发生像主库那样两个 session 「同时」执行向表 t2 中插入数据的场景。所以，在备库上执行了 session B 的 insert 语句，生成的结果里面，id 都是连续的。这时，主从库就发生了数据不一致**。

要解决这问题，binlog 日志格式要设置为 row，这样在 binlog 里面记录的是主库分配的自增值，到备库执行的时候，主库的自增值是什么，从库的自增值就是什么。

所以，**当 innodb_autoinc_lock_mode = 2 时，并且 binlog_format = row，既能提升并发性，又不会出现数据一致性问题**。

> <FONT COLOR=BLUE>AUTO_INC 锁的好处就是没有主从不一致的问题，因为插入数据的过程中不会并发执行到另一个线程进行插入数据，但是锁的粒度比较大，影响并发性能。</FONT>
>
> <FONT COLOR=BLUE>轻量级锁不会锁住插入过程，它只负责递增，但是会导致插入过程会被打断，如果此时日志记录格式恰好为 statement，还会导致主从不一致。</FONT>

## [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#行级锁)行级锁

<font color=24b3aa>InnoDB 引擎是支持行级锁的，而 MyISAM 引擎并不支持行级锁。</font>

<font color=blue>前面也提到，普通的 SELECT 语句是不会对记录加锁的，因为它属于 **快照读**。如果要在查询时对记录加行锁，可以使用下面这两个方式，这种查询会加锁的语句称为 **锁定读**。</font>

```sql
# 对读取的记录加共享锁
SELECT ... lock in share mode;

# 对读取的记录加独占锁
SELECT ... FOR UPDATE;
```

上面这两条语句必须在一个事务中，**因为当事务提交了，锁就会被释放**，所以在使用这两条语句的时候，要加上 begin、start transaction 或者 set autocommit = 0。

行级锁的类型主要有三类：

- Record Lock，记录锁，也就是仅仅把一条记录锁上；
- Gap Lock，间隙锁，锁定一个范围，但是不包含记录本身；
- Next-Key Lock：Record Lock + Gap Lock 的组合，锁定一个范围，并且锁定记录本身。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#record-lock)Record Lock

Record Lock 称为记录锁，锁住的是一条记录。而且 <font color=blue>记录锁是有 S 锁和 X 锁之分的。</font>

举个例子，当一个事务执行了下面这条语句：

```sql
begin;
SELECT * FROM t_test WHERE id = 1 FOR UPDATE;
```

就是对 t_test 表中主键 id 为 1 的这条记录加上 X 型的记录锁，这样其他事务就无法对这条记录进行修改了。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E8%AE%B0%E5%BD%95%E9%94%81.drawio.png)

当事务执行 commit 后，事务过程中生成的锁都会被释放。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#gap-lock)Gap Lock

**Gap Lock（间隙锁）**：锁的是 **<u>相邻</u> 索引记录之间的 <u>空隙</u>**，以及<u>边界外</u>的<u>空隙</u>（比如 `(-∞, 5)`、`(5, 10)` 这种“没有具体行”的区间）。它主要 **<font color=24b3aa>阻止插入</font>**，<font color=blue>对已有行的更新/删除不直接由 gap lock 管（那是 record lock 管）。</font>

Gap Lock 存在于 **可重复读隔离级别** 和 **串行化隔离级别**，目的是为了 **解决可重复读隔离级别下幻读的现象**。

假设，表中有一个范围 id 为`(3,5)`间隙锁，那么其他事务就无法插入 id = 4 这条记录了，这样就有效的防止幻读现象的发生。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/gap%E9%94%81.drawio.png)

<font color=24b3aa>间隙锁虽然存在 X 型间隙锁和 S 型间隙锁，但是并没有什么区别</font>，**<font color=blue>间隙锁之间是兼容的</font>，<font color=24b3aa>即两个事务可以同时持有包含共同间隙范围的间隙锁，并不存在互斥关系</font>，因为间隙锁的目的是防止插入幻影记录而提出的**。

注意不要把 Gap Lock 理解为 Range Lock，假设我们有记录如下（只考虑主键）：

```
1, 3, 6, 7, 8
```

Gap Lock 只能锁住记录之间的间隙以及记录与边界的间隙：`(-∞,1), (1,3), (3,6), (6,7), (7,8), (8,+∞) `。

不能锁住已存在的记录例如 `[1,3]`，更不能任意锁一个范围，例如 `(1,8)`，因为 `[1,3]` 是一个闭区间，`(1,8)` 不属于两个相邻记录之间。

也因此说，<font color=blue>间隙锁只能解决“插入”导致的幻读，它无法解决“删除”导致的幻读（无法锁住已存在的记录）。</font>

例如下面的语句：

```sql
SELECT * FROM t 
WHERE id BETWEEN 1 AND 8 
FOR UPDATE;
```

它可能会锁住：

- 记录锁：`1,3,6,7,8`
- 间隙锁：`(1,3),(3,6),(6,7),(7,8)`

从最终的结果上来看，我们似乎是锁住了 `[1,8]`，但这是 Record Lock 和 Gap Lock 共同作用的结果。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#next-key-lock)Next-Key Lock

<font color=blue>Next-Key Lock 称为临键锁（或下一键锁），是 Record Lock + Gap Lock 的组合，锁定一个范围，并且锁定记录本身。</font>

在 InnoDB 里，**Next-Key Lock 就是用来解决（锁定读场景下的）幻读的主要手段**。

之所以叫 **Next-Key Lock（下一键锁）**，是因为它锁的对象可以理解为：**“当前索引记录（key）以及到下一条索引记录之间的区间”**（也就是“这条 key + 它后面的 gap”）。在 B+Tree 索引里记录是按 key 排序的，所以“下一条 key（next key）”非常自然。

假设，表中有一个范围 id 为 `(3,5]` 的 next-key lock，那么其他事务即不能插入 id = 4 记录，也不能修改 id = 5 这条记录。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E4%B8%B4%E9%94%AE%E9%94%81.drawio.png)

所以，next-key lock 即能保护该记录，又能阻止其他事务将新纪录插入到被保护记录前面的间隙中。

<font color=24b3aa>**next-key lock 是包含间隙锁+记录锁的，如果一个事务获取了 X 型的 next-key lock，那么另外一个事务在获取相同范围的 X 型的 next-key lock 时，是会被阻塞的**。</font>

比如，一个事务持有了范围为 `(1, 10]` 的 X 型的 next-key lock，那么另外一个事务在获取相同范围的 X 型的 next-key lock 时，就会被阻塞。

这是因为虽然相同范围的间隙锁是多个事务相互兼容的，但对于记录锁，我们是要考虑 X 型与 S 型关系，X 型的记录锁与 X 型的记录锁是冲突的。

> 例如主键序列：
>
> ```
> 1, 3, 6, 7, 8
> ```
>
> 如果对 key=3 加一个 next-key lock，它通常覆盖的是：
>
> - **记录锁**：锁住记录 `3`
> - **间隙锁**：锁住区间 `(3, 6)`（直到“下一条 key=6”）
>
> 所以你会看到它的范围像这样：
>
> - next-key(1) 覆盖：`[1,3)`（锁 1 这条记录 + 1 到下一条 3 的间隙）
> - next-key(3) 覆盖：`[3,6)`
> - next-key(6) 覆盖：`[6,7)`
> - next-key(7) 覆盖：`[7,8)`
> - next-key(8) 覆盖：`[8,+∞)`（到“正无穷”的间隙）
>
> 常见教材里会写成 **左闭右开** `[key, next_key)`：左边包含当前记录（record lock），右边是到下一条记录之前的 gap（gap lock）。
>
> 但通常按 InnoDB 的描述，**next-key lock 是左开右闭**，也就是：**`(a, b]`**
>
> 因为 next-key lock 本质上是：
>
> - 前面的 **gap**：`(a, b)`
> - 加上记录 **`b`** 本身
>
> 为什么这名字贴切？因为它强调的是 **按索引顺序** 的锁粒度：每一把锁都跟“下一条索引键”绑定，而不是你随意指定一个数学意义上的区间。

### [#](https://www.xiaolincoding.com/mysql/lock/mysql_lock.html#插入意向锁)插入意向锁

**插入意向锁（Insert Intention Lock）** 是 MySQL InnoDB 在执行 `INSERT` 操作时产生的一种 **<font color=blue>特殊的间隙锁（Gap Lock）</font>**。

它的核心设计目的就一个：**<font color=ee11ff>提高多事务并发插入的性能</font>**。

**<font color=24b3aa>1. 意向锁之间“互不排斥”</font>** 如果多个事务想在同一个区间（Gap）内插入数据，只要**插入的具体主键/唯一索引值不冲突**，它们获取的插入意向锁就不会互相阻塞，可以同时并发插入。

**<font color=24b3aa>2. 与普通间隙锁“完全互斥”</font>** 插入意向锁与普通的 **Gap Lock** 或 **Next-Key Lock** 是互斥的。

- 如果事务 A 已经拿到了区间 `(10, 20)` 的间隙锁（例如执行了 `SELECT ... FOR UPDATE`）；
- 事务 B 尝试在该区间插入数据（申请插入意向锁）时，就会被**阻塞等待**，直到事务 A 释放锁。

**<font color=24b3aa>3. 它是一种“意向”表示</font>** 它并不是真正去锁住具体的某行数据，而是在告诉存储引擎：“我准备在这个区间里插一行新数据了”。只有当没有其他事务在这个区间设置排他的 Gap Lock 时，当前插入才能顺利进行。

例如，假设表中现有的主键为 `10` 和 `20`：

1. **事务 A** 执行：`SELECT * FROM t WHERE id BETWEEN 11 AND 19 FOR UPDATE;`
   - 事务 A 获得了区间 `(10, 20)` 的间隙锁（Gap Lock），目的是防止其他事务在该区间插入。
2. **事务 B** 执行：`INSERT INTO t(id) VALUES(15);`
   - 事务 B 尝试获取区间 `(10, 20)` 的**插入意向锁**。
3. **结果**：由于 Gap Lock 与插入意向锁冲突，**事务 B 被阻塞**，保证了事务 A 读取区间数据时的一致性（避免幻读）。

------

参考资料：

- 《MySQL技术内幕：innodb》
- 《MySQL实战45讲》
- 《从根儿上理解MySQL》

# [锁]MySQL 是怎么加锁的？

大家好，我是小林。

是不是很多人都对 MySQL 加行级锁的规则搞的迷迷糊糊，对记录一会加的是 next-key 锁，一会加是间隙锁，一会又是记录锁。

坦白说，确实还挺复杂的，但是好在我找点了点规律，也知道如何如何用命令分析加了什么类型的行级锁。

之前我写过一篇关于「MySQL 是怎么加行级锁的？」的文章，随着我写 MySQL 锁相关的文章越来越多时，后来发现当时的文章写的不够详细。

为了让大家很清楚的知道 MySQL 是怎么加行级锁的，以及如何用命令分析加了什么行级锁，再加上为了解释清楚为什么 MySQL 要这么加行级锁，所以**我重写了这篇文章**。

这一重写，就多增加了 1W 多字 + 30 张图，所以全完算是新的文章了。

文章内容比较长，大家可以耐心看下去，看完之后你会有一种突然被顿悟的感觉，因为我自己写完这篇文章后，自己也被自己顿悟了。

## [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#什么-sql-语句会加行级锁)什么 SQL 语句会加行级锁？ 

**InnoDB 引擎是支持行级锁的**，而 **MyISAM 引擎并不支持行级锁**，所以后面的内容都是基于 InnoDB 引擎 的。

所以，在说 MySQL 是怎么加行级锁的时候，其实是在说 InnoDB 引擎是怎么加行级锁的。

***1.SELECT***

<font color=24b3aa>普通的 SELECT 语句是不会对记录加锁的（除了串行化隔离级别），因为它属于快照读，是通过 MVCC（多版本并发控制）实现的。</font>

如果要在查询时对记录加行级锁，可以使用下面这两个方式，这两种查询会加锁的语句称为 **<font color=blue>锁定读</font>**。

```sql
//对读取的记录加共享锁(S型锁)
SELECT ... lock in share mode;

//对读取的记录加独占锁(X型锁)
SELECT ... FOR UPDATE;
```

上面这两条语句必须在一个事务中，**因为当事务提交了，锁就会被释放**，所以在使用这两条语句的时候，要加上 begin 或者 start transaction 开启事务的语句。

***2.UPDAYE/DELETE***

**<font color=24b3aa>除了上面这两条锁定读语句会加行级锁之外，UPDATE 和 DELETE 操作都会加行级锁，且锁的类型都是独占锁（X型锁）</font>**。

```sql
//对操作的记录加独占锁(X型锁)
UPDATE table .... WHERE id = 1;

//对操作的记录加独占锁(X型锁)
DELETE FROM table WHERE id = 1;
```

***3.INSERT***

**<font color=24b3aa>INSERT 不会像 UPDATE/DELETE 那样去“锁住已有记录再修改”，它主要是为了保证并发插入的正确性，会在索引上做必要的加锁。</font>**

在 InnoDB 里，INSERT 涉及的锁大致分成三类：**记录锁（新插入的记录）、间隙相关锁（插入意向锁/间隙锁）、唯一性检查相关锁（唯一索引冲突检测）**。

>  3.1 普通 INSERT 会加什么锁？

```sql
insert into table(id, c) values (1, 'a');
```

- **在“要插入的位置”加插入意向锁（Insert Intention Lock）**（重点）
  这是 InnoDB 在索引间隙上加的一种“意向”标记，用来支持并发插入。
  **多个事务往同一个间隙插入不同的值，一般不会互相阻塞**（所以你常感觉 INSERT “不怎么加锁”）。
- **对新插入的记录加独占锁（X 锁）**
  插入成功后，这条新记录在事务提交前对其它事务来说是“不可见/不可改”的（其它事务想改它会被阻塞）。

> 3.2 为什么 INSERT 有时会被卡住？——Gap/Next-Key 锁导致“插不进去”

**INSERT 最常见的等待场景，不是 INSERT 自己锁范围，而是别的事务先把范围/间隙锁住了。**

在默认隔离级别 **RR（REPEATABLE READ）** 下，下面这些“锁定读/当前读”可能会产生 **next-key lock（记录锁 + 间隙锁）**：

- `SELECT ... FOR UPDATE`
- `SELECT ... lock in share mode`
- `UPDATE ... WHERE ...`
- `DELETE ... WHERE ...`

例子：

```sql
-- T1
begin;
SELECT * FROM t WHERE k between 10 and 20 FOR UPDATE;  -- 可能锁住范围及间隙

-- T2
begin;
insert into t(k, ...) values (15, ...);  -- 可能被阻塞（15 落在被锁住的间隙/范围里）
```

现象：你会在 `SHOW ENGINE INNODB STATUS` 里看到类似 **lock wait**，常见关键词是 gap、next-key、insert intention 等。

> 3.3 有唯一索引/主键时：INSERT 还会做“唯一性检查”的锁

如果 `id` 是 **PRIMARY KEY** 或者有 **UNIQUE**：

```sql
insert into t(id, ...) values (1, ...);
```

- InnoDB 必须保证“同一个唯一键”不会被两个事务同时插入成功。
- 因此在并发场景下，会对 **唯一索引相关记录/位置** 加锁来做冲突检测。
- 如果另一个事务已经插入了同样的唯一键但未提交，你的 INSERT 可能会 **等待**（等对方提交/回滚后才能确定是否冲突）。

> 这也是“并发插入同一个 key，会互相等/报重复键”的根源。

> 3.4 `INSERT ... ON DUPLICATE KEY UPDATE / REPLACE / INSERT IGNORE` 的锁更“像写操作”

这些语句本质上不是“单纯插入”，而是**可能转成更新/删除+插入**，因此锁行为更重：

- `INSERT ... ON DUPLICATE KEY UPDATE`
  命中冲突时会走 UPDATE 路径，通常会对命中的记录加 **X 锁**（跟 UPDATE 类似）。
- `REPLACE INTO`
  语义是“存在则删除再插入”，会涉及 **DELETE + INSERT**，锁冲突风险更高（也更容易触发外键/级联问题）。
- `INSERT IGNORE`
  仍要做唯一性检查；如果冲突被忽略，不一定会像普通 INSERT 那样简单结束，但总体也会有索引检查相关锁参与。

> 3.5 自增列（AUTO_INCREMENT）会有额外的并发控制

如果表里有自增主键：

```
insert into t(c) values ('a');  -- id 自增
```

InnoDB 分配自增值会用到 **AUTO-INC 相关的内部锁/机制**（实现和参数有关），高并发批量插入时可能成为热点之一。（一般日常不明显，但压测/高并发写入时会更突出。）

## [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#行级锁有哪些种类)行级锁有哪些种类？

不同隔离级别下，行级锁的种类是不同的。

<font color=24b3aa>在 **读已提交** 隔离级别下，行级锁的种类只有 **记录锁**，也就是仅仅把一条记录锁上。</font>

<font color=ff44ff>在 **可重复读** 隔离级别下，行级锁的种类除了有 **记录锁**，还有 **间隙锁**（目的是为了避免幻读），所以行级锁的种类主要有三类：</font>

- Record Lock，记录锁，也就是仅仅把一条记录锁上；
- Gap Lock，间隙锁，锁定一个范围，但是不包含记录本身；
- Next-Key Lock：Record Lock + Gap Lock 的组合，锁定一个范围，并且锁定记录本身。

接下来，分别介绍这三种行级锁。

> Gap Lock 主要用于解决幻读问题。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#record-lock)Record Lock

Record Lock 称为记录锁，锁住的是一条记录。而且记录锁是有 S 锁和 X 锁之分的：

举个例子，当一个事务执行了下面这条语句：

```sql
mysql > begin;
mysql > SELECT * FROM t_test WHERE id = 1 FOR UPDATE;
```

事务会对表中主键 id = 1 的这条记录加上 X 型的记录锁，如果这时候其他事务对这条记录进行删除或者更新操作，那么这些操作都会被阻塞。<font color=22aa22>注意，其他事务插入一条 id = 1 的新记录并不会被阻塞，而是会报主键冲突的错误，这是因为主键有唯一性的约束。</font>

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E8%AE%B0%E5%BD%95%E9%94%81.drawio.png)

当事务执行 commit 后，事务过程中生成的锁都会被释放。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#gap-lock)Gap Lock

Gap Lock 称为间隙锁，存在于可重复读隔离级别和串行化隔离级别，**<font color=24b3aa>目的是为了解决可重复读隔离级别下幻读的现象。</font>**

假设，表中有一个范围 id 为（3，5）间隙锁，那么其他事务就无法插入 id = 4 这条记录了，这样就有效的防止幻读现象的发生。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/gap%E9%94%81.drawio.png)

间隙锁虽然存在 X 型间隙锁和 S 型间隙锁，但是并没有什么区别，**<font color=24b3aa>间隙锁之间是兼容的</font>，即两个事务可以同时持有包含共同间隙范围的间隙锁，并不存在互斥关系，因为间隙锁的目的是防止插入幻影记录而提出的**。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#next-key-lock)Next-Key Lock

<font color=24b3aa>Next-Key Lock 称为临键锁，是 Record Lock + Gap Lock 的组合，锁定一个范围，并且锁定记录本身。</font>

Next-Key Lock 是 <font color=blue>**前开后闭**</font> 区间。

假设，表中有一个范围 id 为 <font color=blue>**(3,5]**</font> 的 next-key lock，那么其他事务即不能插入 id = 4 记录，也不能修改和删除 id = 5 这条记录。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E4%B8%B4%E9%94%AE%E9%94%81.drawio.png)

所以，next-key lock 即能保护该记录，又能阻止其他事务将新记录插入到被保护记录前面的间隙中。

next-key lock 包含记录锁，这意味着它是不兼容的。

如果一个事务获取了 X 型的 next-key lock，那么另外一个事务在获取相同范围的 X 型的 next-key lock 时，是会被阻塞的。

比如，一个事务持有了范围为 (1, 10] 的 X 型的 next-key lock，那么另外一个事务在获取相同范围的 X 型的 next-key lock 时，就会被阻塞。

虽然相同范围的间隙锁是多个事务相互兼容的，但对于记录锁，我们是要考虑 X 型与 S 型关系，X 型的记录锁与 X 型的记录锁是冲突的。

## [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#mysql-是怎么加行级锁的)MySQL 是怎么加行级锁的？

行级锁加锁规则比较复杂，不同的场景，加锁的形式是不同的。

> Mysql、InnoDB、Repeatable Read

**加锁的对象是 <font color=blue>索引</font>，加锁的基本单位是 <font color=blue>next-key lock</font>**，它是由记录锁和间隙锁组合而成的，**next-key lock 是 <font color=blue>前开后闭</font> 区间，而间隙锁是前开后开区间**。

<font color=ff44ff>但是，**next-key lock 在一些场景下会退化成记录锁或间隙锁**。</font>

那到底是什么场景呢？总结一句，**<font color=ff44ff>在能使用记录锁或者间隙锁就能避免幻读现象的场景下， next-key lock 就会退化成记录锁或间隙锁</font>**。

这次会以下面这个表结构来进行实验说明：

```sql
CREATE TABLE `user` (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `name` varchar(30) COLLATE utf8mb4_unicode_ci NOT NULL,
  `age` int NOT NULL,
  PRIMARY KEY (`id`),
  KEY `index_age` (`age`) USING BTREE
) ENGINE=InnoDB  DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
```

其中，id 是主键索引（唯一索引），age 是普通索引（非唯一索引），name 是普通的列。

表中的有这些行记录：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/user.png)

这次实验环境的 **MySQL 版本是 8.0.26，隔离级别是「可重复读」**。

不同版本的加锁规则可能是不同的，但是大体上是相同的。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#唯一索引-主键索引-等值查询)唯一索引（主键索引）等值查询

当我们用唯一索引进行等值查询的时候，查询的记录存不存在，加锁的规则也会不同：

- 当查询的记录是「存在」的，在索引树上定位到这一条记录后，将该记录的索引中的 next-key lock 会**退化成「记录锁」**。
- 当查询的记录是「不存在」的，在索引树找到第一条大于该查询记录的记录后，将该记录的索引中的 next-key lock 会**退化成「间隙锁」**。

TIP

我本篇文章的「唯一索引」是用「主键索引」作为案例说明的，加锁只加在主键索引项上。

然后，很多同学误以为如果是二级索引的「唯一索引」，加锁也是只加在二级索引项上。

其实这是不对的，所以这里特此说明下，如果是用二级索引（不管是不是非唯一索引，还是唯一索引）进行锁定读查询的时候，除了会对二级索引项加行级锁（如果是唯一索引的二级索引，加锁规则和主键索引的案例相同），而且还会对查询到的记录的主键索引项上加「记录锁」。

在文章的「非唯一索引」的案例中，我就是用二级索引作为例子，在后面的章节我有说明，<font color=blue>对二级索引进行锁定读查询的时候，因为存在两个索引（二级索引和主键索引），所以两个索引都会加锁。</font>

接下里用两个案例来说明。

#### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#_1、记录存在的情况)1、记录存在的情况

假设事务 A 执行了这条等值查询语句，查询的记录是「存在」于表中的。

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE id = 1 FOR UPDATE;
+----+--------+-----+
| id | name   | age |
+----+--------+-----+
|  1 | 路飞   |  19 |
+----+--------+-----+
1 row in set (0.02 sec)
```

那么，事务 A 会为 id 为 1 的这条记录就会加上 **X 型的记录锁**。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%AE%B0%E5%BD%95%E9%94%81.drawio.png)

接下来，如果有其他事务，对 id 为 1 的记录进行更新或者删除操作的话，这些操作都会被阻塞，因为更新或者删除操作也会对记录加 X 型的记录锁，而 X 锁和 X 锁之间是互斥关系。

比如，下面这个例子：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2%EF%BC%88%E5%AD%98%E5%9C%A8%EF%BC%89.drawio.png)

因为事务 A 对 id = 1的记录加了 **X 型的记录锁**，所以事务 B 在修改 id=1 的记录时会被阻塞，事务 C 在删除 id=1 的记录时也会被阻塞。

> 有什么命令可以分析加了什么锁？

我们可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务执行 SQL 过程中加了什么锁。

我们以前面的事务 A 作为例子，分析下下它加了什么锁。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E4%BA%8B%E5%8A%A1a%E5%8A%A0%E9%94%81%E5%88%86%E6%9E%90.png)

从上图可以看到，共加了两个锁，分别是：

- 表锁：X 类型的意向锁；
- 行锁：X 类型的记录锁；

这里我们重点关注行级锁，图中 LOCK_TYPE 中的 RECORD 表示行级锁，而不是记录锁的意思。

通过 LOCK_MODE 可以确认是 next-key 锁，还是间隙锁，还是记录锁：

- 如果 LOCK_MODE 为 `X`，说明是 next-key 锁；
- 如果 LOCK_MODE 为 `X, REC_NOT_GAP`，说明是记录锁；
- 如果 LOCK_MODE 为 `X, GAP`，说明是间隙锁；

因此，**此时事务 A 在 id = 1 记录的主键索引上加的是记录锁，锁住的范围是 id 为 1 的这条记录。**这样其他事务就无法对 id 为 1 的这条记录进行更新和删除操作了。

从这里我们也可以得知，**加锁的对象是针对索引**，因为这里查询语句扫描的 B+ 树是聚簇索引树，即主键索引树，所以是对主键索引加锁。将对应记录的主键索引加记录锁后，就意味着其他事务无法对该记录进行更新和删除操作了。

> 为什么唯一索引等值查询并且查询记录存在的场景下，该记录的索引中的 next-key lock 会退化成记录锁？

原因就是在 唯一索引等值查询并且查询记录存在的场景下，仅靠记录锁也能避免幻读的问题。

幻读的定义就是，当一个事务前后两次查询的结果集，不相同时，就认为发生幻读。所以，要避免幻读就是避免结果集某一条记录被其他事务删除，或者有其他事务插入了一条新记录，这样前后两次查询的结果集就不会出现不相同的情况。

- 由于主键具有唯一性，所以 **其他事务插入 id = 1 的时候，会因为主键冲突，导致无法插入 id = 1 的新记录**。这样事务 A 在多次查询 id = 1 的记录的时候，不会出现前后两次查询的结果集不同，也就避免了幻读的问题。
- 由于对 id = 1 加了记录锁，**其他事务无法删除该记录**，这样事务 A 在多次查询 id = 1 的记录的时候，不会出现前后两次查询的结果集不同，也就避免了幻读的问题。

#### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#_2、记录不存在的情况)2、记录不存在的情况

假设事务 A 执行了这条等值查询语句，查询的记录是「不存在」于表中的。

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE id = 2 FOR UPDATE;
Empty set (0.03 sec)
```

接下来，通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务执行 SQL 过程中加了什么锁。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E4%BA%8B%E5%8A%A1a%E5%88%86%E6%9E%901.png)

从上图可以看到，共加了两个锁，分别是：

- 表锁：X 类型的意向锁；
- 行锁：X 类型的间隙锁；

因此，**此时事务 A 在 id = 5 记录的主键索引上加的是间隙锁，锁住的范围是 (1, 5)。**

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E9%97%B4%E9%9A%99%E9%94%81.drawio.png)

接下来，如果有其他事务插入 id 值为 2、3、4 这一些记录的话，这些插入语句都会发生阻塞。

注意，如果其他事务插入的 id = 1 或者 id = 5 的记录话，并不会发生阻塞，而是报主键冲突的错误，因为表中已经存在 id = 1 和 id = 5 的记录了。

比如，下面这个例子：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2%EF%BC%88%E4%B8%8D%E5%AD%98%E5%9C%A8%EF%BC%89.drawio.png)

因为事务 A 在 id = 5 记录的主键索引上加了范围为 (1, 5) 的 X 型间隙锁，所以事务 B 在插入一条 id 为 3 的记录时会被阻塞住，即无法插入 id = 3 的记录。

> 间隙锁的范围 `(1,5)` ，是怎么确定的？

<font color=bb33>根据我的经验，如果 LOCK_MODE 是 next-key 锁或者间隙锁，那么 LOCK_DATA 就表示锁的范围「右边界」，此次的事务 A 的 LOCK_DATA 是 5。</font>

然后锁范围的「左边界」是表中 id 为 5 的上一条记录的 id 值，即 1。

因此，间隙锁的范围 `(1,5)`。

> 为什么唯一索引等值查询并且查询记录「不存在」的场景下，在索引树找到第一条大于该查询记录的记录后，要将该记录的索引中的 next-key lock 会退化成「间隙锁」？

原因就是在唯一索引等值查询并且查询记录不存在的场景下，仅靠间隙锁就能避免幻读的问题。

- 为什么 id = 5 记录上的主键索引的锁不可以是 next-key lock？如果是 next-key lock，就意味着其他事务无法删除 id = 5 这条记录，但是这次的案例是查询 id = 2 的记录，只要保证前后两次查询 id = 2 的结果集相同，就能避免幻读的问题了，所以即使 id =5 被删除，也不会有什么影响，那就没必须加 next-key lock，因此只需要在 id = 5 加间隙锁，避免其他事务插入 id = 2 的新记录就行了。<font color=e1344>当记录不存在时，如果我们加 Next-Key Lock，反而会降低程序的并发性能，因为 Next-Key Lock 锁的粒度太大了。</font>
- 为什么不可以针对不存在的记录加记录锁？锁是加在索引上的，而这个场景下查询的记录是不存在的，自然就没办法锁住这条不存在的记录。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#唯一索引-主键索引-范围查询)唯一索引（主键索引）范围查询

范围查询和等值查询的加锁规则是不同的。

在 RR（Repeatable Read）下做范围查询：

- **扫描过程里每遇到一条索引记录**，默认先加 **next-key 锁**（记录锁 + 间隙锁）。
- 但在“范围边界”的那条记录上，会根据操作符和“边界值是否存在”发生**退化**：
  - 退化为 **记录锁（Record Lock）**：只锁该行，不锁前后间隙
  - 退化为 **间隙锁（Gap Lock）**：不锁该行，只锁间隙（防止插入）

<font color=blue>***1.`>= x`***</font>

- 因为包含了“等值”（可能命中 x 这一行），所以：
  - **如果 x 这条记录存在**：x 这一条上的 next-key 锁 **退化为记录锁**
  - **如果 x 不存在**：通常边界落在“第一个大于 x 的记录”上，那个位置更偏“间隙控制”，不会出现“命中等值行退化为记录锁”的情况（本质就是：没有等值行可锁）

<font color=blue>***2.`<` / `<=`***</font>

这两个的关键差异就在：**边界值存在时，`<=` 会把边界那行也算进范围，而 `<` 不算。**

1. 边界值 **不存在**

   例如 `< x` 或 `<= x`，但表里没有 x 这条记录：

   - 扫描到“终止位置”（即**第一条大于 x 的记录**）时：
     - 该记录上的 next-key 锁 **退化为间隙锁**（因为我们只需要防止有人插入落进范围的值）
   - 其他被扫描到的记录：都加 **next-key 锁**

2. 边界值 **存在**

   例如表里有 x：

   - `< x`：
     - x **不在范围内**，所以扫描到 x 作为终止记录时，
     - x 上的 next-key 锁 **退化为间隙锁**
     - 其他扫描到的记录：next-key 锁
   - `<= x`：
     - x **在范围内**，所以扫描到 x 时它是“要被包含的记录”，
     - x 上的 **next-key 锁不退化**（仍是 next-key）
     - 其他扫描到的记录：next-key 锁

<font color=blue>***3.`> x`***</font>

- x **不在范围内**，所以如果表里存在 x：
  - x 这一行不需要被锁成“记录锁”，关键是防止插入落进范围的值
  - 通常边界附近会表现为“间隙控制”（也就是更偏 gap 的效果）
- 扫描到的“真正属于范围的第一条记录”（第一条 `> x` 的记录）及之后扫描到的记录：仍然是 **next-key 锁**（扫到哪锁到哪）

接下来，通过几个实验，才验证我上面说的结论。

#### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#_1、针对「大于或者大于等于」的范围查询)1、针对「大于或者大于等于」的范围查询

> 实验一：针对「大于」的范围查询的情况。

假设事务 A 执行了这条范围查询语句：

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE id > 15 FOR UPDATE;
+----+-----------+-----+
| id | name      | age |
+----+-----------+-----+
| 20 | 香克斯    |  39 |
+----+-----------+-----+
1 row in set (0.01 sec)
```

事务 A 加锁变化过程如下：

1. 最开始要找的第一行是 id = 20，由于查询该记录不是一个等值查询（不是大于等于条件查询），所以对该主键索引加的是范围为 (15, 20] 的 next-key 锁；
2. 由于是范围查找，就会继续往后找存在的记录，虽然我们看见表中最后一条记录是 id = 20 的记录，但是实际在 Innodb 存储引擎中，会用一个特殊的记录来标识最后一条记录，该特殊的记录的名字叫 supremum pseudo-record ，所以扫描第二行的时候，也就扫描到了这个特殊记录的时候，会对该主键索引加的是范围为 (20, +∞] 的 next-key 锁。
3. 停止扫描。

可以得知，事务 A 在主键索引上加了两个 X 型 的 next-key 锁：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%A4%A7%E4%BA%8E15.drawio.png)

- 在 id = 20 这条记录的主键索引上，加了范围为 (15, 20] 的 next-key 锁，意味着其他事务即无法更新或者删除 id = 20 的记录，同时无法插入 id 值为 16、17、18、19 的这一些新记录。
- 在特殊记录（ supremum pseudo-record）的主键索引上，加了范围为 (20, +∞] 的 next-key 锁，意味着其他事务无法插入 id 值大于 20 的这一些新记录。

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%A4%A7%E4%BA%8E.png)

从上图中的分析中，也可以得到**事务 A 在主键索引上加了两个 X 型 的next-key 锁：**

- 在 id = 20 这条记录的主键索引上，加了范围为 (15, 20] 的 next-key 锁，意味着其他事务即无法更新或者删除 id = 20 的记录，同时无法插入 id 值为 16、17、18、19 的这一些新记录。
- 在特殊记录（ supremum pseudo-record）的主键索引上，加了范围为 (20, +∞] 的 next-key 锁，意味着其他事务无法插入 id 值大于 20 的这一些新记录。

> 实验二：针对「大于等于」的范围查询的情况。

假设事务 A 执行了这条范围查询语句：

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE id >= 15 FOR UPDATE;
+----+-----------+-----+
| id | name      | age |
+----+-----------+-----+
| 15 | 乌索普    |  20 |
| 20 | 香克斯    |  39 |
+----+-----------+-----+
2 rows in set (0.00 sec)
```

事务 A 加锁变化过程如下：

1. 最开始要找的第一行是 id = 15，由于查询该记录是一个等值查询（等于 15），所以该主键索引的 next-key 锁会**退化成记录锁**，也就是仅锁住 id = 15 这一行记录。
2. 由于是范围查找，就会继续往后找存在的记录，扫描到的第二行是 id = 20，于是对该主键索引加的是范围为 (15, 20] 的 next-key 锁；
3. 接着扫描到第三行的时候，扫描到了特殊记录（ supremum pseudo-record），于是对该主键索引加的是范围为 (20, +∞] 的 next-key 锁。
4. 停止扫描。

可以得知，事务 A 在主键索引上加了三个 X 型 的锁，分别是：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%A4%A7%E4%BA%8E%E7%AD%89%E4%BA%8E15.drawio.png)

- 在 id = 15 这条记录的主键索引上，加了记录锁，范围是 id = 15 这一行记录；意味着其他事务无法更新或者删除 id = 15 的这一条记录；
- 在 id = 20 这条记录的主键索引上，加了 next-key 锁，范围是 (15, 20] 。意味着其他事务即无法更新或者删除 id = 20 的记录，同时无法插入 id 值为 16、17、18、19 的这一些新记录。
- 在特殊记录（ supremum pseudo-record）的主键索引上，加了 next-key 锁，范围是 (20, +∞] 。意味着其他事务无法插入 id 值大于 20 的这一些新记录。

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%A4%A7%E4%BA%8E%E7%AD%89%E4%BA%8E.png)

通过前面这个实验，我们证明了：

- 针对「大于等于」条件的唯一索引范围查询的情况下， 如果条件值的记录存在于表中，那么由于查询该条件值的记录是包含一个等值查询的操作，所以该记录的索引中的 next-key 锁会**退化成记录锁**。

#### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#_2、针对「小于或者小于等于」的范围查询)2、针对「小于或者小于等于」的范围查询

> 实验一：针对「小于」的范围查询时，查询条件值的记录「不存在」表中的情况。

假设事务 A 执行了这条范围查询语句，注意查询条件值的记录（id 为 6）并不存在于表中。

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE id < 6 FOR UPDATE;
+----+--------+-----+
| id | name   | age |
+----+--------+-----+
|  1 | 路飞   |  19 |
|  5 | 索隆   |  21 |
+----+--------+-----+
3 rows in set (0.00 sec)
```

事务 A 加锁变化过程如下：

1. 最开始要找的第一行是 id = 1，于是对该主键索引加的是范围为 (-∞, 1] 的 next-key 锁；
2. 由于是范围查找，就会继续往后找存在的记录，扫描到的第二行是 id = 5，所以对该主键索引加的是范围为 (1, 5] 的 next-key 锁；
3. 由于扫描到的第二行记录（id = 5），满足 id < 6 条件，而且也没有达到终止扫描的条件，接着会继续扫描。
4. 扫描到的第三行是 id = 10，该记录不满足 id < 6 条件的记录，所以 id = 10 这一行记录的锁会**退化成间隙锁**，于是对该主键索引加的是范围为 (5, 10) 的间隙锁。
5. 由于扫描到的第三行记录（id = 10），不满足 id < 6 条件，达到了终止扫描的条件，于是停止扫描。

从上面的分析中，可以得知事务 A 在主键索引上加了三个 X 型的锁：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%B0%8F%E4%BA%8E%E7%AD%89%E4%BA%8E6.drawio.png)

- 在 id = 1 这条记录的主键索引上，加了范围为 (-∞, 1] 的 next-key 锁，意味着其他事务即无法更新或者删除 id = 1 的这一条记录，同时也无法插入 id 小于 1 的这一些新记录。
- 在 id = 5 这条记录的主键索引上，加了范围为 (1, 5] 的 next-key 锁，意味着其他事务即无法更新或者删除 id = 5 的这一条记录，同时也无法插入 id 值为 2、3、4 的这一些新记录。
- 在 id = 10 这条记录的主键索引上，加了范围为 (5, 10) 的间隙锁，意味着其他事务无法插入 id 值为 6、7、8、9 的这一些新记录。

TIP

读者提问：小林哥 ，请教一个问题 在看mysql加行级锁中 SELECT *FROM user WHERE id<6 FOR UPDATE; 为什么id(5,10)要加间隙锁呢？ 不加 也不会发生幻读吧 ？

小林回答：第三个间隙锁是加在id=10 索引上的，这个例子不太好说明，如果是id<7，id=10索引不加间隙锁的话，中途插入一个 6，就发生幻读了

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%B0%8F%E4%BA%8E6.png)

从上图中的分析中，也可以得知事务 A 在主键索引加的三个锁，就是我们前面分析出那三个锁。

虽然这次范围查询的条件是「小于」，但是查询条件值的记录不存在于表中（ id 为 6 的记录不在表中），所以如果事务 A 的范围查询的条件改成 <= 6 的话，加的锁还是和范围查询条件为 < 6 是一样的。 大家自己也验证下这个结论。

因此，**针对「小于或者小于等于」的唯一索引范围查询，如果条件值的记录不在表中，那么不管是「小于」还是「小于等于」的范围查询，扫描到终止范围查询的记录时，该记录中索引的 next-key 锁会退化成间隙锁，其他扫描的记录，则是在这些记录的索引上加 next-key 锁**。

> 实验二：针对「小于等于」的范围查询时，查询条件值的记录「存在」表中的情况。

假设事务 A 执行了这条范围查询语句，注意查询条件值的记录（id 为 5）存在于表中。

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE id <= 5 FOR UPDATE;
+----+--------+-----+
| id | name   | age |
+----+--------+-----+
|  1 | 路飞   |  19 |
|  5 | 索隆   |  21 |
+----+--------+-----+
2 rows in set (0.00 sec)
```

事务 A 加锁变化过程如下：

1. 最开始要找的第一行是 id = 1，于是对该记录加的是范围为 (-∞, 1] 的 next-key 锁；
2. 由于是范围查找，就会继续往后找存在的记录，扫描到的第二行是 id = 5，于是对该记录加的是范围为 (1, 5] 的 next-key 锁。
3. 由于主键索引具有唯一性，不会存在两个 id = 5 的记录，所以不会再继续扫描，于是停止扫描。

从上面的分析中，可以得到**事务 A 在主键索引上加了 2 个 X 型的锁**：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%B0%8F%E4%BA%8E%E7%AD%89%E4%BA%8E5.drawio.png)

- 在 id = 1 这条记录的主键索引上，加了范围为 (-∞, 1] 的 next-key 锁。意味着其他事务即无法更新或者删除 id = 1 的这一条记录，同时也无法插入 id 小于 1 的这一些新记录。
- 在 id = 5 这条记录的主键索引上，加了范围为 (1, 5] 的 next-key 锁。意味着其他事务即无法更新或者删除 id = 5 的这一条记录，同时也无法插入 id 值为 2、3、4 的这一些新记录。

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%B0%8F%E4%BA%8E%E7%AD%89%E4%BA%8E.png)

从上图中的分析中，可以得到事务 A 在主键索引上加了两个 X 型 next-key 锁，分别是：

- 在 id = 1 这条记录的主键索引上，加了范围为 (-∞, 1] 的 next-key 锁；
- 在 id = 5 这条记录的主键索引上，加了范围为(1, 5 ] 的 next-key 锁。

> 实验三：再来看针对「小于」的范围查询时，查询条件值的记录「存在」表中的情况。

如果事务 A 的查询语句是小于的范围查询，且查询条件值的记录（id 为 5）存在于表中。

```sql
SELECT * FROM user WHERE id < 5 FOR UPDATE;
```

事务 A 加锁变化过程如下：

1. 最开始要找的第一行是 id = 1，于是对该记录加的是范围为 (-∞, 1] 的 next-key 锁；
2. 由于是范围查找，就会继续往后找存在的记录，扫描到的第二行是 id = 5，该记录是第一条不满足 id < 5 条件的记录，于是**该记录的锁会退化为间隙锁，锁范围是 (1,5)**。
3. 由于找到了第一条不满足 id < 5 条件的记录，于是停止扫描。

可以得知，此时**事务 A 在主键索引上加了两种 X 型锁：**

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%B0%8F%E4%BA%8E5.drawio.png)

- 在 id = 1 这条记录的主键索引上，加了范围为 (-∞, 1] 的 next-key 锁，意味着其他事务即无法更新或者删除 id = 1 的这一条记录，同时也无法插入 id 小于 1 的这一些新记录。
- 在 id = 5 这条记录的主键索引上，加了范围为 (1,5) 的间隙锁，意味着其他事务无法插入 id 值为 2、3、4 的这一些新记录。

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2%E5%B0%8F%E4%BA%8E.png)

从上图中的分析中，可以得到事务 A 在主键索引上加了 **X 型的范围为 (-∞, 1] 的 next-key 锁，和 X 型的范围为 (1, 5) 的间隙锁**。

因此，通过前面这三个实验，可以得知。

在针对「小于或者小于等于」的唯一索引（主键索引）范围查询时，存在这两种情况会将索引的 next-key 锁会退化成间隙锁的：

- 当条件值的记录「不在」表中时，那么不管是「小于」还是「小于等于」条件的范围查询，扫描到终止范围查询的记录时，该记录的主键索引中的 next-key 锁会**退化成间隙锁**，其他扫描到的记录，都是在这些记录的主键索引上加 next-key 锁。
- 当条件值的记录「在」表中时：
  - 如果是「小于」条件的范围查询，扫描到终止范围查询的记录时，该记录的主键索引中的 next-key 锁会**退化成间隙锁**，其他扫描到的记录，都是在这些记录的主键索引上，加 next-key 锁。
  - 如果是「小于等于」条件的范围查询，扫描到终止范围查询的记录时，该记录的主键索引中的 next-key 锁「不会」退化成间隙锁，其他扫描到的记录，都是在这些记录的主键索引上加 next-key 锁。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#非唯一索引等值查询)非唯一索引等值查询

当我们用非唯一索引进行等值查询的时候，**<font color=24b3aa>因为存在两个索引，一个是主键索引，一个是非唯一索引（二级索引），所以在加锁时，同时会对这两个索引都加锁，但是对主键索引加锁的时候，只有满足查询条件的记录才会对它们的主键索引加锁</font>**。

针对非唯一索引等值查询时，查询的记录存不存在，加锁的规则也会不同：

- 当查询的记录「存在」时，由于不是唯一索引，所以肯定存在索引值相同的记录，于是 **非唯一索引等值查询的过程是一个扫描的过程，直到扫描到第一个不符合条件的二级索引记录就停止扫描，然后在扫描的过程中，对扫描到的二级索引记录加的是 next-key 锁，而对于第一个不符合条件的二级索引记录，该二级索引的 next-key 锁会退化成间隙锁。同时，在符合查询条件的记录的主键索引上加记录锁**。
- 当查询的记录「不存在」时，**扫描到第一条不符合条件的二级索引记录，该二级索引的 next-key 锁会退化成间隙锁。因为不存在满足查询条件的记录，所以不会对主键索引加锁**。

接下里用两个实验来说明。

#### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#_1、记录不存在的情况)1、记录不存在的情况

> 实验一：针对非唯一索引等值查询时，查询的值不存在的情况。

先来说说非唯一索引等值查询时，查询的记录不存在的情况，因为这个比较简单。

假设事务 A 对非唯一索引（age）进行了等值查询，且表中不存在 age = 25 的记录。

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE age = 25 FOR UPDATE;
Empty set (0.00 sec)
```

事务 A 加锁变化过程如下：

- 定位到第一条不符合查询条件的二级索引记录，即扫描到 age = 39，于是**该二级索引的 next-key 锁会退化成间隙锁，范围是 (22, 39)**。
- 停止查询

事务 A 在 age = 39 记录的二级索引上，加了 X 型的间隙锁，范围是 (22, 39)。意味着其他事务无法插入 age 值为 23、24、25、26、....、38 这些新记录。不过对于插入 age = 22 和 age = 39 记录的语句，在一些情况是可以成功插入的，而一些情况则无法成功插入，具体哪些情况，会在后面说。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2age=25.drawio.png)

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2age=25.png)

从上图的分析，可以看到，事务 A 在 age = 39 记录的二级索引上（INDEX_NAME: index_age ），加了范围为 (22, 39) 的 X 型间隙锁。

此时，如果有其他事务插入了 age 值为 23、24、25、26、....、38 这些新记录，那么这些插入语句都会发生阻塞。不过对于插入 age = 39 记录的语句，在一些情况是可以成功插入的，而一些情况则无法成功插入，具体哪些情况，接下来我们就说！

> 当有一个事务持有二级索引的间隙锁 (22, 39) 时，什么情况下，可以让其他事务的插入 age = 22 或者 age = 39 记录的语句成功？又是什么情况下，插入 age = 22 或者 age = 39 记录时的语句会被阻塞？

我们先要清楚，什么情况下插入语句会发生阻塞。

**插入语句在插入一条记录之前，需要先定位到该记录在 B+树 的位置，如果插入的位置的下一条记录的索引上有间隙锁，才会发生阻塞**。

在分析二级索引的间隙锁是否可以成功插入记录时，我们要先要知道二级索引树是如何存放记录的？

二级索引树是按照二级索引值（age列）按顺序存放的，在相同的二级索引值情况下， 再按主键 id 的顺序存放。知道了这个前提，我们才能知道执行插入语句的时候，插入的位置的下一条记录是谁。

基于前面的实验，事务 A 是在 age = 39 记录的二级索引上，加了 X 型的间隙锁，范围是 (22, 39)。

插入 age = 22 记录的成功和失败的情况分别如下：

- 当其他事务插入一条 age = 22，id = 3 的记录的时候，在二级索引树上定位到插入的位置，而**该位置的下一条是 id = 10、age = 22 的记录，该记录的二级索引上没有间隙锁，所以这条插入语句可以执行成功**。
- 当其他事务插入一条 age = 22，id = 12 的记录的时候，在二级索引树上定位到插入的位置，而**该位置的下一条是 id = 20、age = 39 的记录，正好该记录的二级索引上有间隙锁，所以这条插入语句会被阻塞，无法插入成功**。

插入 age = 39 记录的成功和失败的情况分别如下：

- 当其他事务插入一条 age = 39，id = 3 的记录的时候，在二级索引树上定位到插入的位置，而**该位置的下一条是 id = 20、age = 39 的记录，正好该记录的二级索引上有间隙锁，所以这条插入语句会被阻塞，无法插入成功**。
- 当其他事务插入一条 age = 39，id = 21 的记录的时候，在二级索引树上定位到插入的位置，而**该位置的下一条记录不存在，也就没有间隙锁了，所以这条插入语句可以插入成功**。

所以，**当有一个事务持有二级索引的间隙锁 (22, 39) 时，插入 age = 22 或者 age = 39 记录的语句是否可以执行成功，关键还要考虑插入记录的主键值，因为「二级索引值（age列）+主键值（id列）」才可以确定插入的位置，确定了插入位置后，就要看插入的位置的下一条记录是否有间隙锁，如果有间隙锁，就会发生阻塞，如果没有间隙锁，则可以插入成功**。

知道了这个结论之后，我们再回过头看，非唯一索引等值查询时，查询的记录不存在时，执行`SELECT * FROM performance_schema.data_locks\G;` 输出的结果。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2age=25_1.png)

在前面分析输出结果的时候，我说的结论是：「*事务 A 在 age = 39 记录的二级索引上（INDEX_NAME: index_age ），加了范围为 (22, 39) 的 X 型间隙锁*」。这个结论其实还不够准确，因为只考虑了 LOCK_DATA 第一个数值（39），没有考虑 LOCK_DATA 第二个数值（20）。

那 `LOCK_DATA：39，20` 是什么意思？

- LOCK_DATA 第一个数值，也就是 39， 它代表的是 age 值。从前面我们也知道了，LOCK_DATA 第一个数值是 next-key 锁和间隙锁**锁住的范围的右边界值**。
- LOCK_DATA 第二个数值，也就是 20， 它代表的是 id 值。

之所以 LOCK_DATA 要多显示一个数值（ID值），是因为针对「当某个事务持有非唯一索引的 (22, 39) 间隙锁的时候，其他事务是否可以插入 age = 39 新记录」的问题，还需要考虑插入记录的 id 值。而 **LOCK_DATA 的第二个数值，就是说明在插入 age = 39 新记录时，哪些范围的 id 值是不可以插入的**。

因此， `LOCK_DATA：39，20` + `LOCK_MODE : X, GAP` 的意思是，事务 A 在 age = 39 记录的二级索引上（INDEX_NAME: index_age ），加了 age 值范围为 (22, 39) 的 X 型间隙锁，**同时针对其他事务插入 age 值为 39 的新记录时，不允许插入的新记录的 id 值小于 20 **。如果插入的新记录的 id 值大于 20，则可以插入成功。

但是我们无法从`SELECT * FROM performance_schema.data_locks\G;` 输出的结果分析出「在插入 age =22 新记录时，哪些范围的 id 值是可以插入成功的」，这时候就**得自己画出二级索引的 B+ 树的结构，然后确定插入位置后，看下该位置的下一条记录是否存在间隙锁，如果存在间隙锁，则无法插入成功，如果不存在间隙锁，则可以插入成功**。

#### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#_2、记录存在的情况)2、记录存在的情况

> 实验二：针对非唯一索引等值查询时，查询的值存在的情况。

假设事务 A 对非唯一索引（age）进行了等值查询，且表中存在 age = 22 的记录。

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE age = 22 FOR UPDATE;
+----+--------+-----+
| id | name   | age |
+----+--------+-----+
| 10 | 山治   |  22 |
+----+--------+-----+
1 row in set (0.00 sec)
```

事务 A 加锁变化过程如下：

- 由于不是唯一索引，所以肯定存在值相同的记录，于是非唯一索引等值查询的过程是一个扫描的过程，最开始要找的第一行是 age = 22，于是对该二级索引记录加上范围为 (21, 22] 的 next-key 锁。同时，因为 age = 22 符合查询条件，于是对 age = 22 的记录的主键索引加上记录锁，即对 id = 10 这一行加记录锁。
- 接着继续扫描，扫描到的第二行是 age = 39，该记录是第一个不符合条件的二级索引记录，所以该二级索引的 next-key 锁会**退化成间隙锁**，范围是 (22, 39)。
- 停止查询。

可以看到，事务 A 对主键索引和二级索引都加了 X 型的锁：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2%E5%AD%98%E5%9C%A8.drawio.png)

- 主键索引：
  - 在 id = 10 这条记录的主键索引上，加了记录锁，意味着其他事务无法更新或者删除 id = 10 的这一行记录。
- 二级索引（非唯一索引）：
  - 在 age = 22 这条记录的二级索引上，加了范围为 (21, 22] 的 next-key 锁，意味着其他事务无法更新或者删除 age = 22 的这一些新记录，不过对于插入 age = 21 和 age = 22 新记录的语句，在一些情况是可以成功插入的，而一些情况则无法成功插入，具体哪些情况，会在后面说。
  - 在 age = 39 这条记录的二级索引上，加了范围 (22, 39) 的间隙锁。意味着其他事务无法插入 age 值为 23、24、..... 、38 的这一些新记录。不过对于插入 age = 22 和 age = 39 记录的语句，在一些情况是可以成功插入的，而一些情况则无法成功插入，具体哪些情况，会在后面说。

我们也可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句来看看事务 A 加了什么锁。

输出结果如下，我这里只截取了行级锁的内容。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E7%AD%89%E5%80%BC%E6%9F%A5%E8%AF%A2%EF%BC%88%E5%AD%98%E5%9C%A8%EF%BC%89.png)

从上图的分析，可以看到，事务 A 对二级索引（INDEX_NAME: index_age ）加了两个 X 型锁，分别是：

- 在 age = 22 这条记录的二级索引上，加了范围为 (21, 22] 的 next-key 锁，意味着其他事务无法更新或者删除 age = 22 的这一些新记录，针对是否可以插入 age = 21 和 age = 22 的新记录，分析如下：
  - 是否可以插入 age = 21 的新记录，还要看插入的新记录的 id 值，**如果插入 age = 21 新记录的 id 值小于 5，那么就可以插入成功**，因为此时插入的位置的下一条记录是 id = 5，age = 21 的记录，该记录的二级索引上没有间隙锁。**如果插入 age = 21 新记录的 id 值大于 5，那么就无法插入成功**，因为此时插入的位置的下一条记录是 id = 10，age = 22 的记录，该记录的二级索引上有间隙锁。
  - 是否可以插入 age = 22 的新记录，还要看插入的新记录的 id 值，从 `LOCK_DATA : 22, 10` 可以得知，其他事务插入 age 值为 22 的新记录时，**如果插入的新记录的 id 值小于 10，那么插入语句会发生阻塞；如果插入的新记录的 id 大于 10，还要看该新记录插入的位置的下一条记录是否有间隙锁，如果没有间隙锁则可以插入成功，如果有间隙锁，则无法插入成功**。
- 在 age = 39 这条记录的二级索引上，加了范围 (22, 39) 的间隙锁。意味着其他事务无法插入 age 值为 23、24、..... 、38 的这一些新记录，针对是否可以插入 age = 22 和 age = 39 的新记录，分析如下：
  - 是否可以插入 age = 22 的新记录，还要看插入的新记录的 id 值，**如果插入 age = 22 新记录的 id 值小于 10，那么插入语句会被阻塞，无法插入**，因为此时插入的位置的下一条记录是 id = 10，age = 22 的记录，该记录的二级索引上有间隙锁（ age = 22 这条记录的二级索引上有 next-key 锁）。**如果插入 age = 22 新记录的 id 值大于 10，也无法插入**，因为此时插入的位置的下一条记录是 id = 20，age = 39 的记录，该记录的二级索引上有间隙锁。
  - 是否可以插入 age = 39 的新记录，还要看插入的新记录的 id 值，从 `LOCK_DATA : 39, 20` 可以得知，其他事务插入 age 值为 39 的新记录时，**如果插入的新记录的 id 值小于 20，那么插入语句会发生阻塞，如果插入的新记录的 id 大于 20，则可以插入成功**。

同时，事务 A 还对主键索引（INDEX_NAME: PRIMARY ）加了 X 型的记录锁：

- 在 id = 10 这条记录的主键索引上，加了记录锁，意味着其他事务无法更新或者删除 id = 10 的这一行记录。

> 为什么这个实验案例中，需要在二级索引索引上加范围 (22, 39) 的间隙锁？

要找到这个问题的答案，我们要明白 MySQL 在可重复读的隔离级别场景下，为什么要引入间隙锁？其实**是为了避免幻读现象的发生**。

如果这个实验案例中：

```sql
SELECT * FROM user WHERE age = 22 FOR UPDATE;
```

如果事务 A 不在二级索引索引上加范围 (22, 39) 的间隙锁，只在二级索引索引上加范围为 (21, 22] 的 next-key 锁的话，那么就会有幻读的问题。

前面我也说过，在非唯一索引上加了范围为 (21, 22] 的 next-key 锁，是无法完全锁住 age = 22 新记录的插入，因为对于是否可以插入 age = 22 的新记录，还要看插入的新记录的 id 值，从 `LOCK_DATA : 22, 10` 可以得知，其他事务插入 age 值为 22 的新记录时，如果插入的新记录的 id 值小于 10，那么插入语句会发生阻塞，**如果插入的新记录的 id 值大于 10，则可以插入成功**。

也就是说，只在二级索引索引（非唯一索引）上加范围为 (21, 22] 的 next-key 锁，其他事务是有可能插入 age 值为 22 的新记录的（比如插入一个 age = 22，id = 12 的新记录），那么如果事务 A 再一次查询 age = 22 的记录的时候，前后两次查询 age = 22 的结果集就不一样了，这时就发生了幻读的现象。

**那么当在 age = 39 这条记录的二级索引索引上加了范围为 (22, 39) 的间隙锁后，其他事务是无法插入一个 age = 22，id = 12 的新记录，因为当其他事务插入一条 age = 22，id = 12 的新记录的时候，在二级索引树上定位到插入的位置，而该位置的下一条是 id = 20、age = 39 的记录，正好该记录的二级索引上有间隙锁，所以这条插入语句会被阻塞，无法插入成功，这样就避免幻读现象的发生**。

所以，为了避免幻读现象的发生，就需要在二级索引索引上加范围 (22, 39) 的间隙锁。

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#非唯一索引范围查询)非唯一索引范围查询

<font color=24b3aa>非唯一索引和主键索引的范围查询的加锁也有所不同，不同之处在于**非唯一索引范围查询，索引的 next-key lock 不会有退化为间隙锁和记录锁的情况**，也就是非唯一索引进行范围查询时，对二级索引记录加锁都是加 next-key 锁。</font>

就带大家简单分析一下，事务 A 的这条范围查询语句：

```sql
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT * FROM user WHERE age >= 22 FOR UPDATE;
+----+-----------+-----+
| id | name      | age |
+----+-----------+-----+
| 10 | 山治      |  22 |
| 20 | 香克斯    |  39 |
+----+-----------+-----+
2 rows in set (0.01 sec)
```

事务 A 的加锁变化：

- 最开始要找的第一行是 age = 22，虽然范围查询语句包含等值查询，但是这里不是唯一索引范围查询，所以是不会发生退化锁的现象，因此对该二级索引记录加 next-key 锁，范围是 **`(21, 22]`**。同时，对 age = 22 这条记录的主键索引加记录锁，即对 id = 10 这一行记录的主键索引加记录锁。
- 由于是范围查询，接着继续扫描已经存在的二级索引记录。扫面的第二行是 age = 39 的二级索引记录，于是对该二级索引记录加 next-key 锁，范围是 **`(22, 39]`**，同时，对 age = 39 这条记录的主键索引加记录锁，即对 id = 20 这一行记录的主键索引加记录锁。
- 虽然我们看见表中最后一条二级索引记录是 age = 39 的记录，但是实际在 Innodb 存储引擎中，会用一个特殊的记录来标识最后一条记录，该特殊的记录的名字叫 supremum pseudo-record ，所以扫描第二行的时候，也就扫描到了这个特殊记录的时候，会对该二级索引记录加的是范围为 **`(39, +∞]`** 的 next-key 锁。
- 停止查询

可以看到，事务 A 对主键索引和二级索引都加了 X 型的锁：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E8%8C%83%E5%9B%B4%E6%9F%A5%E8%AF%A2age%E5%A4%A7%E4%BA%8E%E7%AD%89%E4%BA%8E22.drawio.png)

- 主键索引（id 列）：
  - 在 id = 10 这条记录的主键索引上，加了记录锁，意味着其他事务无法更新或者删除 id = 10 的这一行记录。
  - 在 id = 20 这条记录的主键索引上，加了记录锁，意味着其他事务无法更新或者删除 id = 20 的这一行记录。
- 二级索引（age 列）：
  - 在 age = 22 这条记录的二级索引上，加了范围为 (21, 22] 的 next-key 锁，意味着其他事务无法更新或者删除 age = 22 的这一些新记录，不过对于是否可以插入 age = 21 和 age = 22 的新记录，还需要看新记录的 id 值，有些情况是可以成功插入的，而一些情况则无法插入，具体哪些情况，我们前面也讲了。
  - 在 age = 39 这条记录的二级索引上，加了范围为 (22, 39] 的 next-key 锁，意味着其他事务无法更新或者删除 age = 39 的这一些记录，也无法插入 age 值为 23、24、25、...、38 的这一些新记录。不过对于是否可以插入 age = 21 ，还需要看新记录的 id 值，有些情况是可以成功插入的，而一些情况则无法插入，比如新记录（ age=21，id 小 5）都是可以正常插入，而新记录（ age=21，id 大于 5）则是无法正常插入，因为插入位置的下一条记录上是有间隙锁。对于是否可以插入 age =22，不管新记录 age=22 的 id 是什么值都是无法插入的，因为插入位置的下一条记录上都是有间隙锁。
  - 在特殊的记录（supremum pseudo-record）的二级索引上，加了范围为 (39, +∞] 的 next-key 锁，意味着其他事务无法插入 age 值大于 39 的这些新记录。

> 在 age >= 22 的范围查询中，明明查询 age = 22 的记录存在并且属于等值查询，为什么不会像唯一索引那样，将 age = 22 记录的二级索引上的 next-key 锁退化为记录锁？

因为 age 字段是非唯一索引，不具有唯一性，所以如果只加记录锁（记录锁无法防止插入，只能防止删除或者修改），就会导致其他事务插入一条 age = 22 的记录，这样前后两次查询的结果集就不相同了，出现了幻读现象。<font color=blue>简而言之，现在有两条记录 `[1,22,Alice], [2,22,Candy]`，中间还可能插入一条记录 `[3,22,Bob]`，因此通过记录锁锁住这两条 age=22 的记录是不行的。 </font>

### [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#没有加索引的查询)没有加索引的查询

前面的案例，我们的查询语句都有使用索引查询，也就是查询记录的时候，是通过索引扫描的方式查询的，然后对扫描出来的记录进行加锁。

**<font color=24b3aa>如果锁定读查询语句，没有使用索引列作为查询条件，或者查询语句没有走索引查询，导致扫描是全表扫描。那么，每一条记录的索引上都会加 next-key 锁，这样就相当于锁住的全表，这时如果其他事务对该表进行增、删、改操作的时候，都会被阻塞。</font>**

不只是锁定读查询语句不加索引才会导致这种情况，UPDATE 和 DELETE 语句如果查询条件不加索引，那么由于扫描的方式是全表扫描，于是就会对每一条记录的索引上都会加 next-key 锁，这样就相当于锁住的全表。

因此，<font color=24b3aa>**在线上在执行 UPDATE、DELETE、SELECT ... FOR UPDATE 等具有加锁性质的语句，一定要检查语句是否走了索引，如果是全表扫描的话，会对每一个索引加 next-key 锁，相当于把整个表锁住了**，这是挺严重的问题。</font>

## #谁负责加锁

在 MySQL 里，**加锁的直接执行者通常是 <font color=blue>存储引擎</font>**，不是 SQL 层本身。

更准确地说：

- **MySQL Server 层** 负责解析 SQL、生成执行计划、调用存储引擎接口
- **存储引擎层** 负责真正的数据读写和锁管理
- 对于最常见的 **InnoDB**，就是 **InnoDB 自己负责行锁、间隙锁、意向锁、MVCC 相关控制**

例如：

- `SELECT ... FOR UPDATE`
- `UPDATE ...`
- `DELETE ...`

这些语句是由 **Server 层识别出需要加锁**，但**真正对记录、索引项、区间加锁的是 InnoDB**

可以简单记成：

**<font color=blue>“Server 层决定要不要调用，InnoDB 决定具体怎么锁。”</font>**

补一句区分：

- **表锁**：有些是 Server 层参与管理的，比如 `LOCK TABLES`
- **行锁**：基本是 **InnoDB 层** 管的，因为行数据和索引都在引擎里

## [#](https://www.xiaolincoding.com/mysql/lock/how_to_lock.html#总结)总结

这次我以 **MySQL 8.0.26** 版本，在可重复读隔离级别之下，做了几个实验，让大家了解了唯一索引和非唯一索引的行级锁的加锁规则。

我这里总结下， MySQL 行级锁的加锁规则。

唯一索引等值查询：

- 当查询的记录是「存在」的，在索引树上定位到这一条记录后，将该记录的索引中的 next-key lock 会**退化成「记录锁」**。
- 当查询的记录是「不存在」的，在索引树找到第一条大于该查询记录的记录后，将该记录的索引中的 next-key lock 会**退化成「间隙锁」**。

非唯一索引等值查询：

- 当查询的记录「存在」时，由于不是唯一索引，所以肯定存在索引值相同的记录，于是非唯一索引等值查询的过程是一个扫描的过程，直到扫描到第一个不符合条件的二级索引记录就停止扫描，然后**在扫描的过程中，对扫描到的二级索引记录加的是 next-key 锁，而对于第一个不符合条件的二级索引记录，该二级索引的 next-key 锁会退化成间隙锁。同时，在符合查询条件的记录的主键索引上加记录锁**。
- 当查询的记录「不存在」时，**扫描到第一条不符合条件的二级索引记录，该二级索引的 next-key 锁会退化成间隙锁。因为不存在满足查询条件的记录，所以不会对主键索引加锁**。

非唯一索引和主键索引的范围查询的加锁规则不同之处在于：

- 唯一索引在满足一些条件的时候，索引的 next-key lock 退化为间隙锁或者记录锁。
- 非唯一索引范围查询，索引的 next-key lock 不会退化为间隙锁和记录锁。

其实理解 MySQL 为什么要这样加锁，主要要以避免幻读角度去分析，这样就很容易理解这些加锁的规则了。

还有一件很重要的事情，在线上在执行 UPDATE、DELETE、SELECT ... FOR UPDATE 等具有加锁性质的语句，一定要检查语句是否走了索引，**如果是全表扫描的话，会对每一个索引加 next-key 锁，相当于把整个表锁住了**，这是挺严重的问题。

最后附上「@一只小铭」同学总结的流程图。

唯一索引（主键索引）加锁的流程图如下。（*注意这个流程图是针对「主键索引」的，如果是二级索引的唯一索引，除了流程图中对二级索引的加锁规则之外，还会对查询到的记录的主键索引项加「记录锁」，流程图没有提示这一个点，所以在这里用文字补充说明下*）

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E5%8A%A0%E9%94%81%E6%B5%81%E7%A8%8B.jpeg)

非唯一索引加锁的流程图：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E8%A1%8C%E7%BA%A7%E9%94%81/%E9%9D%9E%E5%94%AF%E4%B8%80%E7%B4%A2%E5%BC%95%E5%8A%A0%E9%94%81%E6%B5%81%E7%A8%8B.jpeg)

就说到这啦， 我们下次见啦！

# [锁]MySQL 加锁，锁加到哪里去了？

这是一个非常深刻且直指 MySQL 内核实现的问题。很多开发者只知道“加锁”这个动作，但不清楚锁的**物理载体**和**内存结构**。

简单来说，<font color=blue>MySQL 的锁并不是一个独立存在的“实体”，而是**依附于具体对象上的内存标记**。</font>根据锁的类型不同，它们被“加”在完全不同的地方：

### **1. InnoDB 行锁 / 间隙锁：加在「索引记录」上**

这是最容易产生误解的地方。<font color=blue>**InnoDB 的行锁本质上是索引锁**。</font>

- **锁的位置：** 锁信息存储在 **Buffer Pool 中的索引页（Index Page）** 里，或者更准确地说，是关联到索引记录（Record）或索引记录之间的间隙（Gap）上的内存结构。
- 关键机制：
  - 如果 SQL 走了索引，锁就加在对应的**索引 B+ 树节点**上。
  - 如果 SQL **没有走索引**（全表扫描），InnoDB 会退化为对**聚簇索引的所有记录**加锁（相当于锁全表）。
  - 锁不是加在“数据行”本身，而是加在“索引条目”上。这就是为什么更新非索引字段也可能导致大量锁冲突的原因。
- **内存结构：** 由 `lock_t` 结构体表示，通过哈希表（`lock_sys->rec_hash`）将 `(space_id, page_no, heap_no)` 映射到具体的锁对象。

### **2. InnoDB 表级意向锁 / AUTO-INC 锁：加在「内存字典对象」上**

- **锁的位置：** 存储在 InnoDB 内部的**数据字典缓存（Data Dictionary Cache）**中，具体是 `dict_table_t` 结构体内。
- **作用：** 意向锁（IS/IX）不阻塞任何行锁，它只是一个“标志位”，告诉其他事务“有人正在对这个表的某些行加锁”，避免表级 DDL 与行级 DML 冲突时做全表扫描检查。

### **3. MDL 锁（Metadata Lock）：加在「表定义缓存」上**

从 MySQL 5.5 开始引入，解决 Server 层与 Engine 层的元数据一致性问题。

- **锁的位置：** 存储在 Server 层的 **Table Definition Cache (TDC)** 中，具体是 `TABLE_SHARE` 结构体里的 `MDL_lock` 对象。
- **特点：** 它是**语句级别**自动加的（不需要手动 LOCK TABLES）。只要你执行 `SELECT * FROM t`，就会在 `t` 的 `TABLE_SHARE` 上加一个 MDL 读锁。这也是为什么长事务未提交会导致后续 DDL 被阻塞的根本原因——DDL 需要 MDL 写锁，而被前面的 MDL 读锁挡住了。

### **4. LOCK TABLES 显式表锁：加在「线程专属锁列表」+ 「全局表锁管理器」**

回到你之前提到的 `LOCK TABLES`：

- 锁的位置：
  1. **全局层面：** 在 Server 层的 `Table_lock` 结构中登记，维护每张表的读写锁计数器。
  2. **线程层面：** 在当前线程的 `THD::locked_tables_list` 中记录该线程持有哪些表锁、什么类型。
- **这就是“视野禁锢”的实现原理：** 当线程执行 SQL 时，Server 层会先检查 `THD::locked_tables_list`。如果目标表不在列表中，直接报错 `Table 'xxx' was not locked with LOCK TABLES`，根本不会走到存储引擎层。

### **一张图总结锁的物理位置**

| 锁类型             | 加在哪里（物理载体）                    | 可见性                                                       | 生命周期                    |
| :----------------- | :-------------------------------------- | :----------------------------------------------------------- | :-------------------------- |
| InnoDB 行锁/间隙锁 | Buffer Pool 中的**索引页** + 内存哈希表 | `INFORMATION_SCHEMA.INNODB_LOCKS` (8.0前) / `performance_schema.data_locks` (8.0+) | 事务提交/回滚时释放         |
| InnoDB 意向锁      | 内存中的 `dict_table_t`                 | 不可直接查询，隐含在行锁中                                   | 事务结束时释放              |
| MDL 锁             | Server 层 `TABLE_SHARE`                 | `performance_schema.metadata_locks`                          | 语句结束或事务结束时释放    |
| LOCK TABLES        | `THD` 线程对象 + 全局表锁管理器         | `SHOW OPEN TABLES` / `processlist`                           | 显式 `UNLOCK TABLES` 时释放 |

### **💡 核心认知升级**

> **锁不是一个独立的"东西"，它是依附于数据结构的一种状态。**
>
> - 行锁依附于**索引记录** → 所以没有索引就没有精准的行锁
> - MDL 锁依附于**表定义缓存** → 所以长事务会阻塞 DDL
> - 显式表锁依附于**线程对象** → 所以换连接就丢失，且不能访问未声明的表

理解了“锁加在哪里”，就能从根本上理解为什么某些 SQL 会导致意外的锁竞争，以及为什么 `EXPLAIN` 的执行计划对锁行为有决定性影响。

# [锁]字节面试：加了什么锁，导致死锁的？

> ## 题目描述
>
> ### 表字段（students）
>
> ```
> (id, no, name, age, score)
> ```
>
> ### 初始数据
>
> | id   | no    | name  | age  | score |
> | ---- | ----- | ----- | ---- | ----- |
> | 15   | S0001 | Bob   | 25   | 34    |
> | 18   | S0002 | Alice | 24   | 77    |
> | 20   | S0003 | Jim   | 24   | 5     |
> | 30   | S0004 | Eric  | 23   | 91    |
> | 37   | S0005 | Tom   | 22   | 22    |
> | 49   | S0006 | Tom   | 25   | 83    |
> | 50   | S0007 | Rose  | 23   | 89    |
>
> ------
>
> ## 并发事务执行顺序
>
> ### 事务 A
>
> - **time1**
>
> ```sql
> UPDATE students set score = 100 WHERE id = 25;
> ```
>
> - **time3**
>
> ```sql
> insert into students(id, no, name, age, score)
> value(25, 's0025', 'sony', 28, 90);
> ```
>
> ### 事务 B
>
> - **time2**
>
> ```sql
> UPDATE students set score = 100 WHERE id = 26;
> ```
>
> - **time4**
>
> ```sql
> insert into students(id, no, name, age, score)
> value(26, 's0026', 'ace', 28, 90);
> ```

在 <font color=blue>**可重复读**</font> 隔离级别下，这个场景会发生什么？

如果对 MySQL 加锁机制比较熟悉的同学，应该一眼就能看出会发生 **<font color=blue>死锁</font>**，但是具体加了什么锁而导致死锁，是需要我们具体分析的。

接下来，就跟聊聊上面两个事务执行 SQL 语句的过程中，加了什么锁，从而导致死锁的。

> 有个业务主要逻辑就是新增订单、修改订单、查询订单等操作。然后因为订单是不能重复的，所以当时在新增订单的时候做了幂等性校验，做法就是在新增订单记录之前，先通过 `SELECT ... FOR UPDATE` 语句查询订单是否存在，如果不存在才插入订单记录。
>
> 而正是因为这样的操作，当业务量很大的时候，就可能会出现死锁。

## [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#准备工作)准备工作

先创建一张 t_student 表，假设除了 id 字段，其他字段都是普通字段。

```sql
CREATE TABLE `t_student` (
  `id` int NOT NULL,
  `no` varchar(255) DEFAULT NULL,
  `name` varchar(255) DEFAULT NULL,
  `age` int DEFAULT NULL,
  `score` int DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

然后，插入相关的数据后，t_student 表中的记录如下：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/t_student.png)

## [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#开始实验)开始实验

在实验开始前，先说明下实验环境：

- MySQL 版本：8.0.26
- 隔离级别：可重复读（RR）

启动两个事务，按照题目的 SQL 执行顺序，过程如下表格：

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/ab%E4%BA%8B%E5%8A%A1%E6%AD%BB%E9%94%81.drawio.png)

可以看到，事务 A 和 事务 B 都在执行 insert 语句后，都陷入了等待状态（前提没有打开死锁检测），也就是发生了死锁，因为都在相互等待对方释放锁。

## [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#为什么会发生死锁)为什么会发生死锁？

我们可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务执行 SQL 过程中加了什么锁。

接下来，针对每一条 SQL 语句分析具体加了什么锁。

### [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#time-1-阶段加锁分析)Time 1 阶段加锁分析

Time 1 阶段，事务 A 执行以下语句：

```sql
# 事务 A
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> UPDATE t_student set score = 100 WHERE id = 25;
Query OK, 0 rows affected (0.01 sec)
Rows matched: 0  Changed: 0  Warnings: 0
```

然后执行 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务 A 此时加了什么锁。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E4%BA%8B%E5%8A%A1a%E7%9A%84%E9%94%81.png)

从上图可以看到，共加了两个锁，分别是：

- 表锁：X 类型的意向锁；
- 行锁：X 类型的间隙锁；

这里我们重点关注行锁，图中 LOCK_TYPE 中的 RECORD 表示行级锁，而不是记录锁的意思，通过 LOCK_MODE 可以确认是 next-key 锁，还是间隙锁，还是记录锁：

- 如果 LOCK_MODE 为 `X`，说明是 next-key 锁；
- 如果 LOCK_MODE 为 `X, REC_NOT_GAP`，说明是记录锁；
- 如果 LOCK_MODE 为 `X, GAP`，说明是间隙锁；

**因此，此时事务 A 在主键索引（INDEX_NAME : PRIMARY）上加的是间隙锁，锁范围是`(20, 30)`**。

> 间隙锁的范围 `(20, 30)` ，是怎么确定的？

根据我的经验，如果 LOCK_MODE 是 next-key 锁或者间隙锁，那么 LOCK_DATA 就表示锁的范围最右值，此次的事务 A 的 LOCK_DATA 是 30。

然后锁范围的最左值是 t_student 表中 id 为 30 的上一条记录的 id 值，即 20。

![在这里插入图片描述](https://cdn.xiaolincoding.com//mysql/other/403f9c1012e84a4c83bfb2fc3990f177.png)

因此，间隙锁的范围 `(20, 30)`。

### [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#time-2-阶段加锁分析)Time 2 阶段加锁分析

Time 2 阶段，事务 B 执行以下语句：

```sql
# 事务 B
mysql> begin;
Query OK, 0 rows affected (0.00 sec)

mysql> UPDATE t_student set score = 100 WHERE id = 26;
Query OK, 0 rows affected (0.01 sec)
Rows matched: 0  Changed: 0  Warnings: 0
```

然后执行 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务 B 此时加了什么锁。

![在这里插入图片描述](https://cdn.xiaolincoding.com//mysql/other/44277cfefbd6446db861bfb81a1e4a59.png)

从上图可以看到，行锁是 X 类型的间隙锁，间隙锁的范围是 `(20, 30)`。

> 事务 A 和 事务 B 的间隙锁范围都是一样的，为什么不会冲突？

两个事务的间隙锁之间是相互兼容的，不会产生冲突。

在 MySQL 官网上还有一段非常关键的描述：

*Gap locks in InnoDB are “purely inhibitive”, which means that their only purpose is to prevent other transactions FROM Inserting to the gap. Gap locks can co-exist. A gap lock taken by one transaction does not prevent another transaction FROM taking a gap lock on the same gap. There is no difference between shared and exclusive gap locks. They do not conflict with each other, and they perform the same function.*

<font color=243baaa>**间隙锁的意义只在于阻止区间被插入**，因此是可以共存的。**一个事务获取的间隙锁不会阻止另一个事务获取同一个间隙范围的间隙锁**，共享（S 型）和排他（X 型）的间隙锁是没有区别的，他们相互不冲突，且功能相同。</font>

### [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#time-3-阶段加锁分析)Time 3 阶段加锁分析

Time 3，事务 A 插入了一条记录：

```sql
# Time 3 阶段，事务 A 插入了一条记录
mysql> insert into t_student(id, no, name, age,score) value (25, 'S0025', 'sony', 28, 90);
    /// 阻塞等待......
```

此时，事务 A 就陷入了等待状态。

然后执行 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务 A 在获取什么锁而导致被阻塞。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E4%BA%8B%E5%8A%A1a%E7%AD%89%E5%BE%85%E4%B8%AD.png)

可以看到，事务 A 的状态为等待状态（LOCK_STATUS: WAITING），因为向事务 B 生成的间隙锁（范围 `(20, 30)`）中插入了一条记录，所以事务 A 的插入操作生成了一个插入意向锁（`LOCK_MODE:INSERT_INTENTION`）。

> 插入意向锁是什么？

注意！插入意向锁名字里虽然有意向锁这三个字，但是它并不是意向锁，它属于行级锁，是一种特殊的间隙锁。

在 MySQL 的官方文档中有以下重要描述：

*An Insert intention lock is a type of gap lock set by Insert operations prior to row Insertion. This lock signals the intent to Insert in such a way that multiple transactions Inserting into the same index gap need not wait for each other if they are not Inserting at the same position within the gap. Suppose that there are index records with values of 4 and 7. Separate transactions that attempt to Insert values of 5 and 6, respectively, each lock the gap between 4 and 7 with Insert intention locks prior to obtaining the exclusive lock on the Inserted row, but do not block each other because the rows are nonconflicting.*

<font color=24b3aa>这段话表明尽管 **插入意向锁是一种特殊的间隙锁，但不同于间隙锁的是，该锁只用于并发插入操作**。</font>

<font color=24b3aa>如果说间隙锁锁住的是一个区间，那么「插入意向锁」锁住的就是一个点。因而从这个角度来说，插入意向锁确实是一种特殊的间隙锁。</font>

<font color=24b3aa>插入意向锁与间隙锁的另一个非常重要的差别是：**尽管「插入意向锁」也属于间隙锁，但两个事务却不能在同一时间内，一个拥有间隙锁，另一个拥有该间隙区间内的插入意向锁（当然，插入意向锁如果不在间隙锁区间内则是可以的）。所以，插入意向锁和间隙锁之间是冲突的**。</font>

另外，我补充一点，插入意向锁的生成时机：

- 每插入一条新记录，都需要看一下待插入记录的下一条记录上是否已经被加了间隙锁，如果已加间隙锁，此时会生成一个插入意向锁，然后锁的状态设置为等待状态（*PS：MySQL 加锁时，是先生成锁结构，然后设置锁的状态，如果锁状态是等待状态，并不是意味着事务成功获取到了锁，只有当锁状态为正常状态时，才代表事务成功获取到了锁*），现象就是 Insert 语句会被阻塞。

### [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#time-4-阶段加锁分析)Time 4 阶段加锁分析

Time 4，事务 B 插入了一条记录：

```sql
# Time 4 阶段，事务 B 插入了一条记录
mysql> insert into t_student(id, no, name, age,score) value (26, 'S0026', 'ace', 28, 90);
    /// 阻塞等待......
```

此时，事务 B 就陷入了等待状态。

然后执行 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务 B 在获取什么锁而导致被阻塞。

![img](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E4%BA%8B%E5%8A%A1b%E7%AD%89%E5%BE%85%E4%B8%AD.png)

可以看到，事务 B 在生成插入意向锁时而导致被阻塞，这是因为事务 B 向事务 A 生成的范围为 `(20, 30)` 的间隙锁插入了一条记录，而插入意向锁和间隙锁是冲突的，所以事务 B 在获取插入意向锁时就陷入了等待状态。

> 最后回答，为什么会发生死锁？

本次案例中，事务 A 和事务 B 在执行完后 UPDATE 语句后都持有范围为 `(20, 30)` 的间隙锁，而接下来的插入操作为了获取到插入意向锁，都在等待对方事务的间隙锁释放，于是就造成了循环等待，满足了死锁的四个条件：**互斥、占有且等待、不可强占用、循环等待**，因此发生了死锁。

## [#](https://www.xiaolincoding.com/mysql/lock/deadlock.html#如何避免死锁)如何避免死锁？

死锁的四个必要条件：**互斥、占有且等待、不可强占用、循环等待**。只要系统发生死锁，这些条件必然成立，但是只要破坏任意一个条件就死锁就不会成立。

在数据库层面，有两种策略通过「打破循环等待条件」来解除死锁状态：

- **设置事务等待锁的超时时间**。当一个事务的等待时间超过该值后，就对这个事务进行回滚，于是锁就释放了，另一个事务就可以继续执行了。在 InnoDB 中，参数 `innodb_lock_wait_timeout` 是用来设置超时时间的，默认值时 50 秒。

  当发生超时后，就出现下面这个提示：

![图片](https://cdn.xiaolincoding.com//mysql/other/c296c1889f0101d335699311b4ef20a8.png)

- **开启主动死锁检测**。主动死锁检测在发现死锁后，主动回滚死锁链条中的某一个事务，让其他事务得以继续执行。将参数 `innodb_deadlock_detect` 设置为 on，表示开启这个逻辑，默认就开启。

  当检测到死锁后，就会出现下面这个提示：

![图片](https://cdn.xiaolincoding.com//mysql/other/f380ef357d065498d8d54ad07f145e09.png)

上面这个两种策略是「当有死锁发生时」的避免方式。

我们可以回归业务的角度来预防死锁，对订单做幂等性校验的目的是为了保证不会出现重复的订单，那我们可以直接将 order_no 字段设置为唯一索引列，利用它的唯一性来保证订单表不会出现重复的订单，不过有一点不好的地方就是在我们插入一个已经存在的订单记录时就会抛出异常。

## [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#总结)总结

两个事务即使生成的间隙锁的范围是一样的，也不会发生冲突，因为间隙锁目的是为了防止其他事务插入数据，因此间隙锁与间隙锁之间是相互兼容的。

在执行插入语句时，如果插入的记录在其他事务持有间隙锁范围内，插入语句就会被阻塞，因为插入语句在碰到间隙锁时，会生成一个插入意向锁，然后插入意向锁和间隙锁之间是互斥的关系。

如果两个事务分别向对方持有的间隙锁范围内插入一条记录，而插入操作为了获取到插入意向锁，都在等待对方事务的间隙锁释放，于是就造成了循环等待，满足了死锁的四个条件：**互斥、占有且等待、不可强占用、循环等待**，因此发生了死锁。

## [#](https://www.xiaolincoding.com/mysql/lock/show_lock.html#读者问答)读者问答

![在这里插入图片描述](https://cdn.xiaolincoding.com//mysql/other/f4d4d7fdb9074b098b1077acff698aea.png)

# [锁]UPDATE 没 MySQL 加锁，锁加到哪里去了？加索引会锁全表？

大家好，我是小林。

昨晚在群划水的时候，看到有位读者说了这么一件事。

大概就是，在线上执行一条 UPDATE 语句修改数据库数据的时候，WHERE 条件没有带上索引，导致业务直接崩了，被老板教训了一波

这次我们就来看看：

- 为什么会发生这种的事故？
- 又该如何避免这种事故的发生？

说个前提，接下来说的案例都是基于 <u>InnoDB 存储引擎</u>，且事务的隔离级别是 <u>可重复读</u>。

## [#](https://www.xiaolincoding.com/mysql/lock/UPDATE_index.html#为什么会发生这种的事故)为什么会发生这种的事故？

InnoDB 存储引擎的默认事务隔离级别是「可重复读」，但是在这个隔离级别下，在多个事务并发的时候，会出现幻读的问题，所谓的幻读是指在同一事务下，连续执行两次同样的查询语句，第二次的查询语句可能会返回之前不存在的行。

因此 InnoDB 存储引擎自己实现了行锁，通过 next-key 锁（记录锁和间隙锁的组合）来锁住记录本身和记录之间的“间隙”，防止其他事务在这个记录之间插入新的记录，从而避免了幻读现象。

当我们执行 UPDATE 语句时，实际上是会对记录加独占锁（X 锁）的，如果其他事务对持有独占锁的记录进行修改时是会被阻塞的。另外，这个锁并不是执行完 UPDATE 语句就会释放的，而是会等事务结束时才会释放。

在 InnoDB 事务中，对记录加锁的基本单位是 next-key 锁，但是会因为一些条件会退化成间隙锁，或者记录锁。<font color=blue>**加锁的位置准确的说，锁是加在索引上的而非行上。**</font>

<font color=ff44ff>比如，在 UPDATE 语句的 WHERE 条件使用了唯一索引，那么 next-key 锁会退化成记录锁，也就是只会给一行记录加锁。</font>

这里举个例子，这里有一张数据库表，其中 id 为主键索引。

![img](https://cdn.xiaolincoding.com//mysql/other/3c3af16e7a948833ccb6409e8b51daf8.png)

假设有两个事务的执行顺序如下：

![在这里插入图片描述](https://cdn.xiaolincoding.com//mysql/other/d2326f98cbb34fc09ca4013703251501.png)

可以看到，事务 A 的 UPDATE 语句中 WHERE 是等值查询，并且 id 是唯一索引，所以只会对 id = 1 这条记录加锁，因此，事务 B 的更新操作并不会阻塞。

但是，**<font color=24b344>在 UPDATE 语句的 WHERE 条件没有使用索引，就会全表扫描，于是就会对所有记录加上 next-key 锁（记录锁 + 间隙锁），相当于把整个表锁住了。</font>**

假设有两个事务的执行顺序如下：

![img](https://cdn.xiaolincoding.com//mysql/other/1aa886fe95e7bc791c296e2d342fa435.png)

可以看到，这次事务 B 的 UPDATE 语句被阻塞了。

这是因为事务 A的 UPDATE 语句中 WHERE 条件没有索引列，触发了全表扫描，在扫描过程中会对索引加锁，所以全表扫描的场景下，所有记录都会被加锁，也就是这条 UPDATE 语句产生了 4 个记录锁和 5 个间隙锁，相当于锁住了全表。

![img](https://cdn.xiaolincoding.com//mysql/other/63e055617720853f5b64c99576227c09.png)

<font color=24b3aa>因此，当在数据量非常大的数据库表执行 UPDATE 语句时，如果没有使用索引，就会给全表的加上 next-key 锁， 那么锁就会持续很长一段时间，直到事务结束，而这期间除了 `SELECT ... FROM` 语句，其他语句都会被锁住不能执行，业务会因此停滞，接下来等着你的，就是老板的挨骂。</font>

<font color=ff44ff>那 UPDATE 语句的 WHERE 带上索引就能避免全表记录加锁了吗？</font>

<font color=ff44ff>并不是。</font>

<font color=ff44ff>**关键还得看这条语句在执行过程种，优化器最终选择的是索引扫描，还是全表扫描，如果走了全表扫描，就会对全表的记录加锁了**。</font>

TIP

<font color=24b3aa>网上很多资料说，UPDATE 没加锁索引会加表锁，这是不对的。</font>

<font color=24b3aa>Innodb 源码里面在扫描记录的时候，都是针对索引项这个单位去加锁的， UPDATE 不带索引就是全表扫扫描，也就是表里的索引项都加锁，相当于锁了整张表，所以大家误以为加了表锁。</font>

## [#](https://www.xiaolincoding.com/mysql/lock/UPDATE_index.html#如何避免这种事故的发生)如何避免这种事故的发生？

我们可以将 MySQL 里的 `sql_safe_UPDATEs` 参数设置为 1，开启安全更新模式。

> 官方的解释： If set to 1, MySQL aborts UPDATE or DELETE statements that do not use a key in the WHERE clause or a LIMIT clause. (Specifically, UPDATE statements must have a WHERE clause that uses a key or a LIMIT clause, or both. DELETE statements must have both.) This makes it possible to catch UPDATE or DELETE statements WHERE keys are not used properly and that would probably change or DELETE a large number of rows. The default value is 0.

大致的意思是，<font color=24b3aa>当 sql_safe_UPDATEs 设置为 1 时。</font>

<font color=24b3aa>UPDATE 语句必须满足如下条件之一才能执行成功：</font>

- <font color=24b3aa>使用 WHERE，并且 WHERE 条件中必须有索引列；</font>
- <font color=24b3aa>使用 limit；</font>
- <font color=24b3aa>同时使用 WHERE 和 limit，此时 WHERE 条件中可以没有索引列；</font>

<font color=24b3aa>DELETE 语句必须满足以下条件能执行成功：</font>

- <font color=24b3aa>同时使用 WHERE 和 limit，此时 WHERE 条件中可以没有索引列；</font>

如果 WHERE 条件带上了索引列，但是优化器最终扫描选择的是全表，而不是索引的话，我们可以使用 `force index([index_name])` 可以告诉优化器使用哪个索引，以此避免有几率锁全表带来的隐患。

## [#](https://www.xiaolincoding.com/mysql/lock/UPDATE_index.html#总结)总结

不要小看一条 UPDATE 语句，在生产机上使用不当可能会导致业务停滞，甚至崩溃。

当我们要执行 UPDATE 语句的时候，确保 WHERE 条件中带上了索引列，并且在测试机确认该语句是否走的是索引扫描，防止因为扫描全表，而对表中的所有记录加上锁。

我们可以打开 MySQL sql_safe_UPDATEs 参数，这样可以预防 UPDATE 操作时 WHERE 条件没有带上索引列。

如果发现即使在 WHERE 条件中带上了列索引列，优化器走的还是全标扫描，这时我们就要使用 `force index([index_name])` 可以告诉优化器使用哪个索引。

这次就说到这啦，下次要小心点，别再被老板挨骂啦。

# [锁]MySQL 记录锁+间隙锁可以防止删除操作而导致的幻读吗？

答案是可以的。

接下来，通过几个小实验来证明这个结论吧，顺便再帮大家复习一下记录锁+间隙锁。

## [#](https://www.xiaolincoding.com/mysql/lock/lock_phantom.html#什么是幻读)什么是幻读？

首先来看看 MySQL 文档是怎么定义幻读（Phantom Read）的:

***The so-called phantom problem occurs within a transaction when the same query produces different sets of rows at different times. For example, if a SELECT is executed twice, but returns a row the second time that was not returned the first time, the row is a “phantom” row.***

翻译：当同一个查询在不同的时间产生不同的结果集时，事务中就会出现所谓的幻象问题。例如，如果 SELECT 执行了两次，但第二次返回了第一次没有返回的行，则该行是“幻像”行。

举个例子，假设一个事务在 T1 时刻和 T2 时刻分别执行了下面查询语句，途中没有执行其他任何语句：

```sql
SELECT * FROM t_test WHERE id > 100;
```

只要 T1 和 T2 时刻执行产生的结果集是不相同的，那就发生了幻读的问题，比如：

- T1 时间执行的结果是有 5 条行记录，而 T2 时间执行的结果是有 6 条行记录，那就发生了幻读的问题。
- T1 时间执行的结果是有 5 条行记录，而 T2 时间执行的结果是有 4 条行记录，也是发生了幻读的问题。

> MySQL 是怎么解决幻读的？

<font color=24b4aa>MySQL InnoDB 引擎的默认隔离级别虽然是「可重复读」，但是它 **很大程度上避免** 幻读现象</font>（<font color=ff44ff>并不是完全解决了</font>，详见这篇[文章 (opens new window)](https://xiaolincoding.com/mysql/transaction/phantom.html)），解决的方案有两种：

- 针对 **快照读**（普通 SELECT 语句），是 **通过 MVCC 方式解决了幻读**，因为可重复读隔离级别下，事务执行过程中看到的数据，一直跟这个事务启动时看到的数据是一致的，即使中途有其他事务插入了一条数据，是查询不出来这条数据的，所以就很好了避免幻读问题。
- 针对 **当前读**（SELECT ... FOR UPDATE 等语句），是 **通过 next-key lock（记录锁+间隙锁）方式解决了幻读**，因为当执行 SELECT ... FOR UPDATE 语句的时候，会加上 next-key lock，如果有其他事务在 next-key lock 锁范围内插入了一条记录，那么这个插入语句就会被阻塞，无法成功插入，所以就很好了避免幻读问题。

## [#](https://www.xiaolincoding.com/mysql/lock/lock_phantom.html#实验验证)实验验证

接下来，来验证「 MySQL 记录锁+间隙锁 **可以防止** 删除操作而导致的幻读问题」的结论。

实验环境：MySQL 8.0 版本，可重复读隔离级。

现在有一张用户表（t_user），表里 **只有一个主键索引**，表里有以下行数据：

![在这里插入图片描述](https://cdn.xiaolincoding.com//mysql/other/75c5c503d7df4ad091bfc35708dce6c4.png)

现在有一个 A 事务执行了一条查询语句，查询到年龄大于 20 岁的用户共有 6 条行记录。

![img](https://cdn.xiaolincoding.com//mysql/other/68dd89fc95aa42cf9b0c4251d4e9226c.png)

然后， B 事务执行了一条删除 id = 2 的语句：

![img](https://cdn.xiaolincoding.com//mysql/other/2332fad58bc548ec917ba7ea44d09d30.png)

此时，B 事务的删除语句就陷入了**等待状态**，说明是无法进行删除的。

因此，<font color=blue>MySQL 记录锁+间隙锁 **可以防止** 删除操作而导致的幻读问题。</font>

## [#](https://www.xiaolincoding.com/mysql/lock/lock_phantom.html#加锁分析)加锁分析

问题来了，A 事务在执行 SELECT ... FOR UPDATE 语句时，具体加了什么锁呢？

我们可以通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务执行 SQL 过程中加了什么锁。

输出的内容很多，共有 11 行信息，我删减了一些不重要的信息：

![请添加图片描述](https://cdn.xiaolincoding.com//mysql/other/90e68bf52b2c4e8a9127cfcbb0f0a322.png)

从上面输出的信息可以看到，共加了两种不同粒度的锁，分别是：

- 表锁（`LOCK_TYPE: TABLE`）：X 类型的意向锁；
- 行锁（`LOCK_TYPE: RECORD`）：X 类型的 next-key 锁；

这里我们重点关注「行锁」，图中 `LOCK_TYPE` 中的 `RECORD` 表示行级锁，而不是记录锁的意思：

- 如果 LOCK_MODE 为 `X`，说明是 next-key 锁；
- 如果 LOCK_MODE 为 `X, REC_NOT_GAP`，说明是记录锁；
- 如果 LOCK_MODE 为 `X, GAP`，说明是间隙锁；

然后通过 `LOCK_DATA` 信息，可以确认 next-key 锁的范围，具体怎么确定呢？

- 根据我的经验，如果 LOCK_MODE 是 next-key 锁或者间隙锁，那么 **LOCK_DATA 就表示锁的范围最右值**，而锁范围的最左值为 LOCK_DATA 的上一条记录的值。

因此，此时事务 A 在主键索引（`INDEX_NAME : PRIMARY`）上加了 10 个 next-key 锁，如下：

- X 型的 next-key 锁，范围：(-∞, 1]
- X 型的 next-key 锁，范围：(1, 2]
- X 型的 next-key 锁，范围：(2, 3]
- X 型的 next-key 锁，范围：(3, 4]
- X 型的 next-key 锁，范围：(4, 5]
- X 型的 next-key 锁，范围：(5, 6]
- X 型的 next-key 锁，范围：(6, 7]
- X 型的 next-key 锁，范围：(7, 8]
- X 型的 next-key 锁，范围：(8, 9]
- X 型的 next-key 锁，范围：(9, +∞]

**这相当于把整个表给锁住了，其他事务在对该表进行增、删、改操作的时候都会被阻塞**。

只有在事务 A 提交了事务，事务 A 执行过程中产生的锁才会被释放。

> 为什么只是查询年龄 20 岁以上行记录，而把整个表给锁住了呢？

这是因为事务 A 的这条查询语句是 **全表扫描，<font color=24b3aa>锁是在遍历索引的时候加上的，并不是针对输出的结果加锁</font>**。

![img](https://cdn.xiaolincoding.com//mysql/other/e0b2a18daa864306a84ec51c0866d170.png)

<font color=ff44ff>因此，**在线上在执行 UPDATE、DELETE、SELECT ... FOR UPDATE 等具有加锁性质的语句，一定要检查语句是否走了索引，如果是全表扫描的话，会对每一个索引加 next-key 锁，相当于把整个表锁住了**，这是挺严重的问题。</font>

> 如果对 age 建立索引，事务 A 这条查询会加什么锁呢？

接下来，我**对 age 字段建立索引**，然后再执行这条查询语句：

![img](https://cdn.xiaolincoding.com//mysql/other/68dd89fc95aa42cf9b0c4251d4e9226c-20230309222654910.png)

接下来，继续通过 `SELECT * FROM performance_schema.data_locks\G;` 这条语句，查看事务执行 SQL 过程中加了什么锁。

具体的信息，我就不打印了，我直接说结论吧。

**因为表中有两个索引，分别是主键索引和 age 索引，所以会分别对这两个索引加锁。**

主键索引会加如下的锁：

- X 型的记录锁，锁住 id = 2 的记录；
- X 型的记录锁，锁住 id = 3 的记录；
- X 型的记录锁，锁住 id = 5 的记录；
- X 型的记录锁，锁住 id = 6 的记录；
- X 型的记录锁，锁住 id = 7 的记录；
- X 型的记录锁，锁住 id = 8 的记录；

分析 age 索引加锁的范围时，要先对 age 字段进行排序。

![请添加图片描述](https://cdn.xiaolincoding.com//mysql/other/b93b31af4eec416e9f00c2adc1f7d0c1.png)

age 索引加的锁：

- X 型的 next-key lock，锁住 age 范围 (19, 21] 的记录；
- X 型的 next-key lock，锁住 age 范围 (21, 21] 的记录；
- X 型的 next-key lock，锁住 age 范围 (21, 23] 的记录；
- X 型的 next-key lock，锁住 age 范围 (23, 23] 的记录；
- X 型的 next-key lock，锁住 age 范围 (23, 39] 的记录；
- X 型的 next-key lock，锁住 age 范围 (39, 43] 的记录；
- X 型的 next-key lock，锁住 age 范围 (43, +∞] 的记录；

化简一下，**age 索引 next-key 锁的范围是 (19, +∞]。**

可以看到，对 age 字段建立了索引后，查询语句是索引查询，并不会全表扫描，因此 **不会把整张表给锁住**。

![img](https://cdn.xiaolincoding.com//mysql/other/2920c60d5a9b42f2a65933fa14761c20.png)

总结一下，在对 age 字段建立索引后，事务 A 在执行下面这条查询语句后，主键索引和 age 索引会加下图中的锁。

![请添加图片描述](https://cdn.xiaolincoding.com//mysql/other/5b9a2d7a2cd240fea47b938364f0b76a.png)

事务 A 加上锁后，事务 B、C、D、E 在执行以下语句都会被阻塞。

![请添加图片描述](https://cdn.xiaolincoding.com/gh/xiaolincoder/mysql/%E9%94%81/%E9%98%BB%E5%A1%9E%E6%97%B6%E5%BA%8F%E5%9B%BE.png)

## [#](https://www.xiaolincoding.com/mysql/lock/lock_phantom.html#总结)总结

<font color=blue>在 MySQL 的可重复读隔离级别下，针对当前读的语句会对 **索引** 加记录锁+间隙锁，这样可以避免其他事务执行增、删、改时导致幻读的问题。</font>

<font color=blue>有一点要注意的是，在执行 UPDATE、DELETE、SELECT ... FOR UPDATE 等具有加锁性质的语句，一定要检查语句是否走了索引，如果是全表扫描的话，会对每一个索引加 next-key 锁，相当于把整个表锁住了，这是挺严重的问题！</font>

# [锁]普通读、快照读

“快照读”和“当前读”是数据库（特别是 MySQL 的 InnoDB 引擎）在处理多个人同时操作数据时，为了保证不出错而采用的两种读取策略。

**1. 快照读 (Snapshot Read)：看历史** 这就好比给数据拍了一张照片。当你去查数据时，数据库会根据你的事务开启时间，给你看一个当时“安全”的旧版本。

- **怎么触发：** 平时最普通的 `SELECT` 查询都是快照读。
- **核心特点：** **不加锁**。别人在改数据，你依然可以读你的快照，大家互不干扰，性能极高。缺点是你读到的可能不是此时此刻绝对最新的数据。

**2. 当前读 (Current Read)：看最新** 顾名思义，强制要求读取数据库里此时此刻**最新已提交**的数据。并且为了防止你刚读完别人就把数据改了，它必须对数据上锁。

- **怎么触发：** 所有的写操作（`UPDATE`、`DELETE`、`INSERT`），以及手动加锁的查询（`SELECT ... FOR UPDATE` 或 `SELECT ... LOCK IN SHARE MODE`）。
- **核心特点：** **要加锁**。读的时候不允许别人改，别人想改就得排队等你的事务结束，保证了数据绝对的一致性。

# [锁]InnoDB 如何解决幻读？

> InnoDB 通过 MVCC 解决快照读（普通 SELECT），通过 Next-key Lock 解决当前读（UPDATE、INSERT、DELETE、加锁 SELECT）。

InnoDB 在默认的 **RR（可重复读）隔离级别**下，其实并没有用一刀切的办法，而是把查询分成了两类情况，打出了一套“组合拳”来对付幻读。

深入它的底层逻辑，核心点就 3 个：

**1. 普通查询：靠 MVCC 快照读** 当你执行普通的 `SELECT` 时，InnoDB 使用的是多版本并发控制（MVCC）。 在事务开启并执行第一次查询的那一刻，内核会给你生成一个“视图快照”（Read View）。在这个快照建立之后，哪怕别的事务疯狂往表里 `INSERT` 新数据并且提交了，你的快照里也根本看不见这些新加入的行。 因为底层每条记录都带着 **版本号**，只要你不去主动加锁，你永远只能看到事务开启时那个版本的数据，自然就不会发生幻读。

**2. 加锁更新/查询：靠 Next-Key Lock 当前读** 如果你执行的是 `UPDATE`、`DELETE` 或者是 `SELECT ... FOR UPDATE`，这在底层叫“当前读”，系统必须去读取绝对最新的数据，MVCC 的快照术就失效了。 这时候 InnoDB 的硬核绝招上场了：**Next-Key Lock（临键锁）**。它等于 **记录锁（Record Lock）+ 间隙锁（Gap Lock）**。 举个例子，表里有 id 为 5 和 10 的两条数据，你想查 `id > 5 FOR UPDATE`。InnoDB 不仅会锁住 10 这条现有的记录，还会把 (5, 10) 以及 (10, +∞) 这些不存在记录的**空隙**也给焊死。别的事务只要想在这个缝隙里 `INSERT` 任何新数据，都会被立刻阻塞。既然连插都插不进来，你再去读，自然就没有“幻影行”了。

**<font color=blue>3. 唯一可能翻车的破绽：两种读法混用</font>** 严格意义上，InnoDB 并没有 100% 杜绝幻读，它留了一个极其经典的“漏网之鱼”。 如果在同一个事务里，你先用了“快照读”，然后又用了“当前读”，幻读就会出现。 比如：你先 `SELECT` 发现没有 id=8 的数据；此时别的事务 `INSERT` 了 id=8 并提交；接着你手欠去执行了一个 `UPDATE ... WHERE id=8`（触发了当前读，不仅修改了数据，还把这行数据的所属权强行拉入了自己的快照里）；当你最后再次 `SELECT` 时，原来不存在的 id=8 就会像幽灵一样浮现出来。

- 解决方案的核心思想只有一个：**让事务内所有相关操作使用同一套时间基准**。
- 要么全走 MVCC（只读事务），要么全走加锁（读写事务）。

# [优化]SQL 查询性能优化

## 避免向数据库请求不需要的数据

在访问数据库时，应该只请求需要的行和列。请求多余的行和列会消耗 MySQL 服务器的 CPU 和内存资源，并增加网络开销。

例如在处理分页时，应该使用 LIMIT 限制 MySQL 只返回一页的数据，而不是向应用程序返回全部数据后，再由应用程序过滤不需要的行。

当一行数据被多次使用时可以考虑将数据行缓存起来，避免每次使用都要到 MySQL 查询。

尤其避免使用 `SELECT *` 这种方式进行查询，应该只返回需要的列。

- `SELECT *` 会消耗更多的 CPU。
- `SELECT *` 无用字段增加网络带宽资源消耗，增加数据传输时间，尤其是大字段（如 varchar、blob、text）。
- `SELECT *` 无法使用 MySQL 优化器覆盖索引的优化（基于 MySQL 优化器的“覆盖索引”策略又是速度极快，效率极高，业界极为推荐的查询优化方式）
- `SELECT <字段列表>` 可减少表结构变更带来的影响。

## 干掉低效的“相关子查询”

根据 <font color=ee11ff>子查询在执行时，是否需要依赖外层查询的每一行数据</font>，可以将子查询分为相关子查询和非相关子查询。

相关子查询内部**引用了外层表的字段**，导致子查询无法独立执行，必须**对外层每一行都执行一次**。

例如：

```sql
-- ❌ 相关子查询：子查询里用了 e.department_id（外层的字段）
SELECT e.name FROM employees e
WHERE '研发部' = (
    SELECT d.department_name 
    FROM departments d
    WHERE d.id = e.department_id   -- 👈 依赖外层的 e
);
```

在这个相关子查询的执行过程中，对于外层的每一个 `e.department_id`，需要遍历内层表 d，找到满足 `e.department_id == d.id` 的记录。**外层有多少行，子查询就执行多少次。** 数据量大时性能急剧下降。

解决方案很简单，就是把相关子查询优化为非相关子查询：

```sql
-- ✅ 非相关子查询：子查询里没有引用外层的任何字段
SELECT e.name FROM employees e
WHERE e.department_id = (
    SELECT d.id 
    FROM departments d
    WHERE d.department_name = '研发部'   -- 👈 与外层无关
);
```

**执行过程：**

```
dept_id = SELECT id FROM departments 
          WHERE department_name = '研发部'  -- 只执行1次！

for 每一个员工 e in employees:
    if e.department_id == dept_id:       -- 直接比较缓存值
        输出 e.name
```

## 分解大的查询

可以将一个大查询切分成多个小查询执行，每个小查询只完成整个查询任务的一小部分，每次只返回一小部分结果

删除旧的数据是一个很好的例子。<font color=24b3aa>如果只用一条语句一次性执行一个大的删除操作，则可能需要一次锁住很多数据，占满整个事务日志，耗尽系统资源、阻塞很多小的但重要的查询。</font>将一个大的删除操作分解成多个较小的删除操作可以将服务器上原本一次性的压力分散到多次操作上，尽可能小地影响 MySQL 性能，减少删除时锁的等待时间。同时也减少了 MySQL 主从复制的延迟。

另一个例子是分解关联查询，即对每个要关联的表进行单表查询，然后将结果在应用程序中进行关联。下面的这个查询：

```sql
SELECT * FROM tag
    JOIN tag_post ON tag_post.tag_id=tag.id
    JOIN post ON tag_post.post_id=post.id
WHERE tag.tag = 'MySQL';
```

可以分解成下面这些查询来代替：

```sql
SELECT * FROM tag WHERE tag = 'MySQL';
SELECT * FROM tag_post WHERE tag_id = 1234;
SELECT * FROM post WHERE post.id in (123,456,567,9098,8904);
```

将一个关联查询拆解成多个单表查询有如下有点：

1. **让缓存的效率更高。**如果缓存的是关联查询的结果，那么其中的一个表发生变化，整个缓存就失效了。而拆分后，如果只是某个表很少的改动，并不会破坏所有的缓存。
2. 可以减少锁的竞争
3. 更容易对数据库进行拆分，更容易做到高性能和可扩展。
4. 查询本身的效率也有可能会有所提升。例如上面用 `IN()` 代替关联查询比随机的关联更加高效。

## 优化 `MIN()` 和 `MAX()`

<font color=blue>添加索引可以优化 `MIN()` 和 `MAX()` 表达式。</font>例如，要找到某一列的最小值，只需要查询对应 B-Tree 索引的最左端的记录即可。类似的，如果要查询列中的最大值，也只需要读取 B-Tree 索引的最后一条记录。对于这种查询，EXPLAIN 中可以看到 `"Select tables optimized away"`，表示优化器已经从执行计划中移除了该表，并以一个常数取而代之。

## 用 `IN()` 取代 `OR`

在 MySQL 中，当 `IN()` 列表由**常量值**组成时，优化器会在解析阶段自动对其进行排序和去重，然后通过二分查找匹配，时间复杂度为 O(log n)。而多个 `OR` 条件只能逐一比较，时间复杂度为 O(n)。因此，当匹配值较多时，用 `IN()` 替换 `OR` 会显著更快。

除了性能，`IN()` 在可维护性上也远优于多个 `OR`，尤其是值列表来自配置或参数化传入时。

例如：

```sql
SELECT name FROM employee 
WHERE employee.department_id IN (3, 1, 2);
```

1. **排序 + 去重**：`(3, 1, 2)` → 内部变为 `(1, 2, 3)`
2. **二分查找**：对每一行的 `department_id`，在有序列表 `(1, 2, 3)` 中做二分匹配，O(log 3) ≈ 2 次比较
3. **等价改写**：优化器还可能将其转化为**范围扫描或索引合并**（如果 `department_id` 上有索引）

## 坚决戒掉代码循环

在应用层用 `for/while` 循环逐条执行 SQL，是拖垮系统性能的头号杀手。正确做法是用**单条批量语句**替代循环，并利用 `WHERE` 条件精准过滤，避免对无需变动的数据做无效更新。

**反面示例 vs 正面示例**

```sql
# ❌ N+1 循环：1000次网络往返
for order in orders:
    db.execute("UPDATE orders SET status='shipped' WHERE id=%s", (order.id,))

# ✅ 批量更新：1次网络往返
ids = [o.id for o in orders]
db.execute("UPDATE orders SET status='shipped' WHERE id IN %s", (ids,))
```

假设单次 SQL 耗时 1ms + 网络往返 0.5ms，循环 1000 次 = **1.5 秒**；批量操作 = **几毫秒**。差距可达 **数百倍**。

但需要注意，批量语句并非越大越好。`IN()` 列表过长（如 > 1000）可能导致 SQL 解析开销剧增甚至超出 `max_allowed_packet`。建议**分批处理**（如每批 200~500 条）。

## 优先使用批量操作

对于数据库中的数据更新，如果能使用批量操作就要尽量使用，减少请求数据库的次数，提高性能。

```sql
# 反例
INSERT INTO `cus_order` (`id`, `score`, `name`) VALUES (1, 426547, 'user1');
INSERT INTO `cus_order` (`id`, `score`, `name`) VALUES (1, 33, 'user2');
INSERT INTO `cus_order` (`id`, `score`, `name`) VALUES (1, 293854, 'user3');

# 正例
INSERT into `cus_order` (`id`, `score`, `name`) values(1, 426547, 'user1'),(1, 33, 'user2'),(1, 293854, 'user3');
```

## 排序优化

<font color=blue>应该尽量让 MySQL 使用索引进行排序。</font>当不能使用索引生成排序结果的时候， MySQL 需要自己进行排序。如果数据量小于“排序缓冲区”的大小，则 MySQL 使用内存进行“快速排序”操作。如果数据量太大超过“排序缓冲区”的大小，那么 MySQL 只能采用文件排序，而文件排序的算法非常复杂，会消耗很多资源。

无论如何排序都是一个成本很高的操作，所以从性能角度考虑，应尽可能避免排序。所以让 MySQL 根据索引构造排序结果非常的重要。

## 优化关联查询

优化关联查询，根本原因是**不优化的代价太高，极易引发线上业务雪崩**。核心原因就这 3 点：

**1. 防止性能呈“指数级”恶化** MySQL 处理 JOIN 的本质是两层 for 循环。如果两张 10 万行的表没走索引直接硬连，那就是 10万 × 10万 = 100 亿次比对。优化查询，就是利用索引和选择合适的小表把这个恐怖的“乘法”降维，让查询耗时从几分钟甚至卡死，缩短到几毫秒。

**2. 避免拖垮整个数据库实例** 一个糟糕的 JOIN 会把大量无用数据塞进内存，一旦超出系统设定的内存缓冲区（Join Buffer）大小，就会疯狂读写磁盘产生临时表。这会瞬间榨干数据库服务器的 CPU 和 I/O 资源，导致同库里的其他无关查询（比如简单的单表更新）也被卡死，造成全站变慢。

**3. 减少锁占用，为架构拆分留后路** 查询越慢，占用的数据库连接和事务锁就越久，高并发下极易导致应用层的数据库连接池被打爆。此外，业务后期一旦需要分库分表或微服务拆分，复杂的跨表 JOIN 根本无法跨物理节点执行。提早优化，甚至将复杂 JOIN 拆分到业务代码层组装，能让系统的并发能力和可扩展性强得多。

优化 MySQL 关联查询（JOIN），核心逻辑就是**减少嵌套循环的次数**和**降低每次循环的内存和磁盘读取量**。实战中主要抓住以下 3 个关键点：

**1. 让“小表”驱动“大表”** MySQL 的 JOIN 底层是嵌套循环。必须确保用数据量少、或者 `WHERE` 过滤后结果集更小的表作为驱动表，去关联大表。 通常优化器会自动选择驱动表，但如果遇到复杂语句选错了，你可以用 `STRAIGHT_JOIN` 强制指定顺序。核心思路是：**先过滤，再关联**，尽量把大表变成小表。

> 表面上看，无论谁驱动谁，逻辑上的比对次数确实都是 x×y，但在数据库底层，**利用索引的成本和读取磁盘的次数是天差地别的**。
>
> 区别主要体现在以下 2 个核心场景：
>
> **1. 有索引时：减少查 B+ 树的次数** 执行关联查询时，驱动表通常是“全表扫描”（逐行拿数据），被驱动表是“走树查索引”（找匹配）。 假设小表有 100 行，大表有 1 万行，且关联字段都有索引：
>
> - **小驱动大**：扫 100 次小表，拿着数据去大表的索引树里查 **100 次**。
> - **大驱动小**：扫 1 万次大表，拿着数据去小表的索引树里查 **1 万次**。 查 100 次树和查 1 万次树，消耗的 CPU 和时间显然不在一个量级。
>
> **2. 无索引时：减少磁盘的重复读取批次** 如果关联字段没走索引，MySQL 会把驱动表的数据塞进一块叫 Join Buffer 的内存里，然后读取被驱动表的数据进内存比对。
>
> - **小驱动大**：如果小表的数据能一次性全塞进这块内存，大表只需从磁盘读取 **1 次**，就能完成所有比对。
> - **大驱动小**：大表太大，内存一次塞不下，只能“分段”塞。假设大表分了 10 批才读完，那小表就得被迫从磁盘里重新读取 **10 次**来配合它。磁盘 I/O 成本直接爆炸。
>
> **一句话总结：** 小表驱动大表，并不是改变了 x×y 的总计算量，而是为了**让“循环的外部圈数”尽可能少**，从而大幅减少查索引的次数和磁盘的重复扫描。

**2. 关联字段必须建索引** 被驱动表（被 join 的大表）的关联字段上一定要有索引。这样 MySQL 就能走高效的 `Index Nested-Loop Join`（直接走 B+ 树精准匹配），而不是把数据塞进内存里做缓慢的暴力比对（`Block Nested-Loop Join`）。 *注意：两张表的关联字段类型、字符集（Charset）和排序规则（Collation）必须完全一致，否则会导致隐式转换，索引直接失效。*

**3. 别用 SELECT，按需查字段** 关联查询时千万别偷懒查全字段。MySQL 的 Join Buffer（关联内存缓冲区）大小有限，只 `SELECT` 业务真正需要的列，能让 Buffer 单次存下更多行数据，大幅减少读取被驱动表的批次。如果关联的表超过 3 张，建议直接在业务代码（Java/Go 等）里拆分成多次单表查询，不仅能在应用层组装，后期也更好上 Redis 缓存和分库分表。

## 尽量避免多表做 join

阿里巴巴《Java 开发手册》中有这样一段描述：

> 【强制】超过三个表禁止 join。需要 join 的字段，数据类型保持绝对一致;多表关联查询时，保证被关联 的字段需要有索引。

join 的效率比较低，主要原因是因为其使用嵌套循环（Nested Loop）来实现关联查询，以前常见的实现效率都不是很高：

- **Simple Nested-Loop Join** ：直接使用笛卡尔积实现 join，逐行遍历/全表扫描，效率最低。
- **Block Nested-Loop Join (BNL)** ：利用 JOIN BUFFER 进行优化。**注意：在 MySQL 8.0.20 及更高版本中，BNL 已被 Hash Join 取代**，Hash Join 通常能将非索引列关联的复杂度从 O(M*N) 降低到接近 O(M+N)。
- **Index Nested-Loop Join** ：在必要的字段上增加索引，性能得到进一步提升。

实际业务场景避免多表 join 常见的做法有两种：

1. **单表查询后在内存中自己做关联** ：对数据库做单表查询，再根据查询结果进行二次查询，以此类推，最后再进行关联。
2. **数据冗余**，把一些重要的数据在表中做冗余，尽可能地避免关联查询。很笨的一种做法，表结构比较稳定的情况下才会考虑这种做法。进行冗余设计之前，思考一下自己的表结构设计的是否有问题。

更加推荐第一种，这种在实际项目中的使用率比较高，除了性能不错之外，还有如下优势：

1. **拆分后的单表查询代码可复用性更高** ：join 联表 SQL 基本不太可能被复用。
2. **单表查询更利于后续的维护** ：不论是后续修改表结构还是进行分库分表，单表查询维护起来都更容易。

不过，如果系统要求的并发量不大的话，我觉得多表 join 也是没问题的。很多公司内部复杂的系统，要求的并发量不高，很多数据必须 join 5 张以上的表才能查出来。

## 优化 UNION 查询

`UNION` 是 SQL 中的一个操作符，简单来说，它的作用就是**把多个 SELECT 查询的结果“纵向”拼接到一起**，组合成一个新的结果集。

打个比方，如果说关联查询（`JOIN`）是把两张表在“横向”上拼接（增加列数），那么 `UNION` 就是在“纵向”上叠加（增加行数）。

使用 `UNION` 时，主要得遵守这 3 个硬性规则：

1. **查询的列数必须一样**：你参与拼接的各个 `SELECT` 语句，查出来的列的数量必须完全一致。
2. **数据类型必须兼容**：对应位置上的列，数据类型得一样，或者至少能互相兼容转换，并且排列顺序要对齐。
3. **默认自动去重**：合并结果时，`UNION` 默认会把完全重复的行剔除掉。

<font color=blue>除非确实需要服务器消除重复的行，否则一定要使用 UNION ALL。如果没有 ALL 关键字，MySQL 会给临时表加上 DISTINCT 选项，这会导致对整个临时表的数据做唯一性检查。这样做的代价非常高。</font>

`UNION` 为了实现去重，必须先建一个内部临时表，把数据全塞进去，再进行排序或哈希操作来检查唯一性。如果结果集比较大，超出了内存限制，这个临时表就会被强制刷到磁盘上。这时候不仅狂吃 CPU，还会引发严重的磁盘 I/O 瓶颈，导致查询耗时断崖式下跌。

很多时候，我们要查的两部分数据在业务逻辑上天然就是互斥的（比如“历史订单归档表”和“当前活跃订单表”），这种情况下根本不可能有重复数据，闭着眼睛用 `UNION ALL` 就行。就算真的有少量重复，把数据拉到应用层（比如 Java/Go 代码里丢进 Set 集合）去做去重，通常也比让宝贵的数据库去扛计算压力要划算得多。

## 不要使用外键

> https://juejin.cn/post/7338778403552690210

阿里巴巴《Java 开发手册》中有这样一段描述：<font color=blue>不得使用外键与级联，一切外键概念必须在应用层解决。</font>

<font color=24b3aa>定义外键之后，数据库的每次操作都需要去检查外键约束。对于插入来说，影响了插入速度；对于更新来说，级联更新是强阻塞，存在数据库更新风暴（Database Update Storm）的风险。</font>

所谓 Database Update Storm，指的是在高并发环境下，多个客户端同时对数据库进行大量的更新操作，存在锁竞争问题甚至死锁，从而导致数据库性能急剧下降或完全崩溃。

<font color=24b3aa>另外，当数据量非常大的时候，常见手段是分库分表，但外键通常难以跨越不同数据库来建立联系，数据的一致性更难维护。</font>

因此，外键与级联并不适合分布式、高并发集群，但单机低并发业务可以考虑使用外键保证一致性和完整性。

# [优化]正确使用索引

## 删除长期未使用的索引

删除长期未使用的索引，不用的索引的存在会造成不必要的性能损耗 MySQL 5.7 可以通过查询 sys 库的 schema_unused_indexes 视图来查询哪些索引从未被使用。

## 选择合适的字段创建索引

- **不为 NULL 的字段** ：索引字段的数据应该尽量不为 NULL，因为对于数据为 NULL 的字段，数据库较难优化。如果字段频繁被查询，但又避免不了为 NULL，建议使用 0,1,true,false 这样语义较为清晰的短值或短字符作为替代。
- **被频繁查询的字段** ：我们创建索引的字段应该是查询操作非常频繁的字段。
- **被作为条件查询的字段** ：被作为 WHERE 条件查询的字段，应该被考虑建立索引。
- **频繁需要排序的字段** ：索引已经排序，这样查询可以利用索引的排序，加快排序查询时间。
- **被经常频繁用于连接的字段** ：经常用于连接的字段可能是一些外键列，对于外键列并不一定要建立外键，只是说该列涉及到表与表的关系。对于频繁被连接查询的字段，可以考虑建立索引，提高多表连接查询的效率。

## 避免索引失效

索引失效也是慢查询的主要原因之一，常见的导致索引失效的情况有下面这两类：

**1. SQL 写法与底层逻辑冲突（破坏 B+Tree 有序性）**

此类问题最为常见，本质是查询条件让底层的 B+Tree 失去了“二分查找”的快速定位能力。

- **违背最左前缀原则**：跳过联合索引前导列，或遇到范围查询（如 `>`、`<`、`BETWEEN`、`LIKE "abc%"`）导致后续列中断精确定位，降级为范围扫描加过滤。
- **对索引列进行加工**：在 `WHERE` 左侧对索引列进行数学计算或应用函数，导致原始数据发生逻辑改变，在索引树中呈现无序状态。
- **隐式类型转换（隐蔽且致命）**：当“字符串类型的列”去比较“数字类型的值”时，MySQL 会默认在列上套用转换函数，直接破坏树的有序性。
- **LIKE 模糊查询前置通配符**：如 `LIKE "%abc"`，前缀字符的不确定性使得优化器无法锁定扫描区间的起始点。
- **ORDER BY 排序陷阱**：排序列未命中索引、排序方向与索引结构不一致等触发额外的内存或磁盘排序（`Using filesort`）。

**2. 优化器的成本决策（基于 I/O 成本妥协）**

此类问题并非索引本身不可用，而是 MySQL 优化器经过计算后，认为“不走普通索引”整体开销反而更小。

- **无脑 `SELECT *` 导致回表成本超载**：查询大量非索引覆盖列时，若命中数据量较大（通常超 20%~30%），优化器会判定全表扫描的顺序 I/O 优于频繁回表的随机 I/O，从而主动放弃索引。
- **`OR` 条件导致全表扫描**：只要 `OR` 连接的任意一侧条件没有对应索引，就会触发全表扫描。即使两侧都有索引，若 Index Merge（索引合并）的预期成本过高，依然会被放弃。
- **`IN` 列表过长引发估算失真**：当 `IN` 列表长度超过系统阈值（默认 200）时，优化器会从精准的深入探测（Index Dive）切换为粗略的统计估算，极易因统计信息陈旧而产生执行成本的误判。

详细介绍：[MySQL索引失效场景总结](https://javaguide.cn/database/mySQL/mySQL-index-invalidation.html)。

## 被频繁更新的字段应该慎重建立索引

虽然索引能带来查询上的效率，但是维护索引的成本也是不小的，B+ 树需要频繁改动，磁盘 IO 很大。 如果一个字段不被经常查询，反而被经常修改，那么就更不应该在这种字段上建立索引了。

## 尽可能的考虑建立联合索引而不是单列索引

因为索引是需要占用磁盘空间的，可以简单理解为每个索引都对应着一颗 B+树。如果一个表的字段过多，索引过多，那么当这个表的数据达到一个体量后，索引占用的空间也是很多的，且修改索引时，耗费的时间也是较多的。如果是联合索引，多个字段在一个索引上，那么将会节约很大磁盘空间，且修改数据的操作效率也会提升。

## 注意避免冗余索引

冗余索引指的是索引的功能相同，能够命中索引 (a, b) 就肯定能命中索引 (a) ，那么索引 (a) 就是冗余索引。能够命中前者的查询肯定是能够命中后者的，在大多数情况下，都应该尽量扩展已有的索引而不是创建新索引。

## 考虑在字符串类型的字段上使用前缀索引代替普通索引

前缀索引仅限于字符串类型，较普通索引会占用更小的空间，所以可以考虑使用前缀索引带替普通索引。

前缀索引，简单来说就是**只提取字符串的前几个字符来建立索引**，而不是把整个长字符串都塞进索引树里。

它的核心逻辑可以归纳为以下 3 点：

- **核心原理（截取前段）：** 比如给长邮箱字段建索引，可以只取前 10 个字符。这就像查字典时不看整个单词，只按前两三个字母去快速定位一样。
- **最大优势（省空间提速）：** 完整存储长文本（如长 URL、详细地址）的索引非常臃肿。只存前缀能大幅缩减索引体积，让数据库的内存能缓存更多索引页，从而提升整体检索效率。
- **局限与代价（区分度下降）：** 如果截取得太短，极易发生“撞车”（比如很多数据的前缀一模一样），导致数据库最终还是得回表去核对完整数据。此外，因为前缀索引里的信息是不完整的，所以它**绝对不能**用来做 `ORDER BY` 排序，也无法实现“覆盖索引”。

# [优化]`COUNT()` 性能分析 

大家好，我是小林。

当我们对一张数据表中的记录进行统计的时候，习惯都会使用 `COUNT` 函数来统计，但是 `COUNT` 函数传入的参数有很多种，比如 `COUNT(1)`、`COUNT(*)`、`COUNT(字段)` 等。

<font color=red>到底哪种效率是最好的呢？</font>

我曾经以为 `COUNT(*)` 是效率最差的，因为认知上 `SELECT * FROM t` 会读取所有表中的字段，所以凡事带有 `*` 字符的就觉得会读取表中所有的字段，当时网上有很多博客也这么说。

但是，当我深入 `COUNT` 函数的原理后，被啪啪啪的打脸了！

![图片](https://cdn.xiaolincoding.com//mySQL/other/d9b9817e92f805e9a16faf31a2c10d9a.png)

## [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#哪种-COUNT-性能最好)哪种 `COUNT` 性能最好？

我先直接说结论：按照性能排序：

* **<font color=red>`COUNT(*) = COUNT(1) == COUNT(主键索引字段) >= COUNT(非主键索引字段) > COUNT(普通字段)`</font>**

要弄明白这个，我们得要深入 `COUNT` 的原理，以下内容基于常用的 innodb 存储引擎来说明。

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#COUNT-是什么)`COUNT()` 是什么？

`COUNT()` 是一个聚合函数，函数的参数不仅可以是字段名，也可以是其他任意表达式，该函数作用是 **<font color=red>统计符合查询条件的记录中，<u>函数指定的参数不为 NULL 的记录</u> 有多少个</font>**。

假设 `COUNT()` 函数的参数是字段名，如下：

```sql
SELECT COUNT(name) FROM t_order;
```

这条语句是统计「 t_order 表中，name 字段不为 NULL 的记录」有多少个。也就是说，如果某一条记录中的 name 字段的值为 NULL，则就不会被统计进去。

再来假设 `COUNT()` 函数的参数是数字 1 这个表达式，如下：

```sql
SELECT COUNT(1) FROM t_order;
```

这条语句是统计「 t_order 表中，1 这个表达式不为 NULL 的记录」有多少个。

<font color=24b3aa>1 这个表达式就是单纯数字，它永远都不是 NULL，所以上面这条语句，其实是在统计 t_order 表中有多少个记录。</font>

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#COUNT-主键字段-执行过程是怎样的)`COUNT(索引字段)` 执行过程是怎样的？

<font color=24b3aa>在通过 `COUNT` 函数统计有多少个记录时，MySQL 的 server 层会维护一个名叫 **count** 的变量。</font>

server 层会循环向 InnoDB 读取一条记录，如果 `COUNT` 函数指定的参数不为 NULL，那么就会将变量 count 加 1，直到符合查询的全部记录被读完，就退出循环。最后将 count 变量的值发送给客户端。

InnoDB 是通过 B+ 树来保存记录的，根据索引的类型又分为聚簇索引和二级索引，它们区别在于，聚簇索引的叶子节点存放的是实际数据，而二级索引的叶子节点存放的是主键值，而不是实际数据。

用下面这条语句作为例子：

```sql
//id 为主键值
SELECT COUNT(id) FROM t_order;
```

<font color=ef11>如果表里只有主键索引，没有二级索引时，那么，InnoDB 循环遍历聚簇索引，将读取到的记录返回给 server 层，然后读取记录中的 id 值，就会 id 值判断是否为 NULL，如果不为 NULL，就将 count 变量加 1。</font>

![图片](https://cdn.xiaolincoding.com//mySQL/other/9bb4f32ac843467684a2664d4db61ae3.png)

<font color=ef11>但是，如果表里有二级索引时，InnoDB 循环遍历的对象就不是聚簇索引，而是二级索引。</font>

![图片](https://cdn.xiaolincoding.com//mySQL/other/aac550602ef1022e0b45020dbe0f716a.png)

<font color=blue>这是因为相同数量的二级索引记录可以比聚簇索引记录占用更少的存储空间，所以二级索引树比聚簇索引树小，这样遍历二级索引的 I/O 成本比遍历聚簇索引的 I/O 成本小，因此「优化器」优先选择的是二级索引。</font>

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#COUNT-1-执行过程是怎样的)`COUNT(1)` 执行过程是怎样的？

用下面这条语句作为例子：

```sql
SELECT COUNT(1) FROM t_order;
```

如果表里只有主键索引，没有二级索引时。

![图片](https://cdn.xiaolincoding.com//mySQL/other/e630fdc5897b5c5dbc332e8838afa1fc.png)

那么，InnoDB 循环遍历聚簇索引（主键索引），将读取到的记录返回给 server 层，**但是 <font color=red>不会读取记录中的任何字段的值</font>**，因为 COUNT 函数的参数是 1，不是字段，所以不需要读取记录中的字段值。参数 1 很明显并不是 NULL，因此 server 层每从 InnoDB 读取到一条记录，就将 COUNT 变量加 1。

可以看到，<font color=blue>`COUNT(1)` 相比 `COUNT(索引字段)` 少一个步骤，就是不需要读取记录中的字段值，所以通常会说 `COUNT(1)` 执行效率会比 `COUNT(索引字段)` 高一点。</font>

但是，如果表里有二级索引时，InnoDB 循环遍历的对象就二级索引了。

![图片](https://cdn.xiaolincoding.com//mySQL/other/01e83441a7721f0864deb1ac14ad8ea6.png)

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#COUNT-执行过程是怎样的)`COUNT(*)` 执行过程是怎样的？

看到 `*` 这个字符的时候，是不是大家觉得是读取记录中的所有字段值？

对于 `SELECT *` 这条语句来说是这个意思，但是在 `COUNT(*)` 中并不是这个意思。

<font color=24b3aa>**`count(*)` 其实等于 `COUNT(0)`**，也就是说，当你使用 `COUNT(*)` 时，MySQL 会将 `*` 参数转化为参数 0 来处理。</font>

![图片](https://cdn.xiaolincoding.com//mySQL/other/27b229f049b27898f3a86c7da7e26114.png)

<font color=BLUE>所以，**`COUNT(*)` 执行过程跟 `COUNT(1)` 执行过程基本一样的**，性能没有什么差异。</font>

在 MySQL 5.7 的官方手册中有这么一句话：

* *InnoDB handles `SELECT COUNT(*)` and `SELECT COUNT(1)` operations in the same way. There is no performance difference.*

而且 MySQL 会对 `COUNT(*)` 和 `COUNT(1)` 有个优化，<font color=24b3aa>如果有多个二级索引的时候，优化器会使用 key_len 最小的二级索引进行扫描。</font>

只有当没有二级索引的时候，才会采用主键索引来进行统计。

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#COUNT-字段-执行过程是怎样的)COUNT(普通字段) 执行过程是怎样的？

`COUNT(普通字段)` 的执行效率相比前面的 `COUNT(1)`、 `COUNT(*)`、 `COUNT(索引字段)` 执行效率是最差的。

用下面这条语句作为例子：

```sql
// name 不是索引，普通字段
SELECT COUNT(name) FROM t_order;
```

**由于没有能覆盖 `name` 字段的二级索引可用**，最终退化为**<font color=red>聚簇索引全扫描（ALL）</font>**。在 InnoDB 引擎中，我们常说的 <font color=blue>“全表扫描”（Full Table Scan）</font>，其底层实现就是**遍历聚簇索引（Clustered Index）的 B+ 树的所有叶子节点**。

![图片](https://cdn.xiaolincoding.com//mySQL/other/f24dfeb85e2cfce0e4dc3a17b893b3f5.png)

- **`index`**：扫描的是**二级索引树**（数据量小，每行只含索引列+主键）
- **`ALL`**：扫描的是**聚簇索引树**（数据量大，每行是完整记录）

<font color=24b3aa>因此，`COUNT(普通字段)` 的执行效率通常确实是最差的，但全盘扫描只是表象，根本原因在于，`count(普通字段)` 无法利用覆盖索引以及需要逐行判断是否为 NULL。</font>

**1. 无法利用覆盖索引（I/O 瓶颈）—— 主因**

- **`COUNT(\*)` / `COUNT(1)` / `COUNT(索引字段)`**：优化器可以选择任意一个**最小的二级索引**进行全扫描（`type: index`）。二级索引的叶子节点只包含「索引列 + 主键」，单页能容纳的记录数远多于聚簇索引页，因此**总页数少、I/O 量小**。
- **`COUNT(普通字段)`**：由于该字段不在任何二级索引中，**必须回到聚簇索引读取完整行数据**（`type: ALL`）。聚簇索引叶子节点存储的是完整行记录，单页容纳行数少，**总页数可能是二级索引的数倍甚至数十倍**。

**2. 逐行非空判断（CPU 瓶颈）—— 次因**

即使假设 I/O 相同（比如表只有一个聚簇索引），`COUNT(name)` 仍然比 `COUNT(*)` 慢：

- **`COUNT(*)` / `COUNT(1)`**：MySQL 内部做了专门优化，**不读取任何字段值**，直接按行数累加计数器。
- **`COUNT(name)`**：必须从每一行中**提取 `name` 字段的值**，判断是否为 `NULL`，非 NULL 才计数。这涉及字段偏移计算、变长字段解析、NULL 标志位检查等 CPU 操作。

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#小结)小结

在 MySQL 中进行行数统计时，优化器遵循“最小 I/O 代价”原则。由于二级索引通常比聚簇主键索引更窄、页更紧凑，因此在执行 `COUNT(*)`、`COUNT(1)` 或 `COUNT(主键)` 时，若表上存在二级索引，优化器会自动优先选择扫描该二级索引以减少磁盘 I/O、提升效率。这意味着即使查询不涉及特定字段，为表设计一个合适的窄二级索引也是加速全表计数的有效手段。

在具体写法上，`COUNT(*)` 和 `COUNT(1)` 因无需读取具体列值且不用判空，性能最优；`COUNT(主键字段)` 由于主键天然非 NULL，同样无需判空逻辑，效率与前两者等同。相比之下，`COUNT(普通字段)` 不仅语义上仅代表“该字段非 NULL 的行数”而非总行数，而且在性能上需要逐行判空，若该字段未被索引覆盖还会引发回表甚至全表扫描，开销显著更大。

因此，实践中应严格避免使用 `COUNT(普通字段)` 来统计总行数，这既会导致语义错误也会带来性能隐患。统计总行数应统一使用 `COUNT(*)` 或 `COUNT(1)` 并依赖二级索引加速；若确实需要统计某字段的非 NULL 记录数，则必须确保该字段被二级索引（单列或联合索引）覆盖，以便通过扫描更窄的索引高效完成统计，避免不必要的回表开销。

## [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#为什么要通过遍历的方式来计数)为什么要通过遍历的方式来计数？

你可能会好奇，为什么 COUNT 函数需要通过遍历的方式来统计记录个数？

我前面将的案例都是基于 Innodb 存储引擎来说明的，但是在 MyISAM 存储引擎里，执行 COUNT 函数的方式是不一样的，通常在没有任何查询条件下的 `COUNT(*)`，MyISAM 的查询速度要明显快于 InnoDB。

使用 MyISAM 引擎时，执行 COUNT 函数只需要 O(1) 复杂度，这是因为每张 MyISAM 的数据表都有一个 meta 信息有存储了row_count值，由表级锁保证一致性，所以直接读取 row_count 值就是 COUNT 函数的执行结果。

<font color=blue>那为什么 InnoDB 存储引擎内部不维护一个元素据（row_count），这样我们查询数据行数时就可以在 O(1) 时间内得到结果？</font>

<font color=blue>原因在于， InnoDB 存储引擎是支持事务的，同一个时刻的多个查询，由于多版本并发控制（MVCC）的原因，InnoDB 表“应该返回多少行”也是不确定的，所以无法像 MyISAM一样，只维护一个 row_count 变量。</font>

举个例子，假设表 t_order 有 100 条记录，现在有两个会话并行以下语句：

![图片](https://cdn.xiaolincoding.com//mySQL/other/04d714293f5c687810562e984b67d2e7.png)

在会话 A 和会话 B的最后一个时刻，同时查表 t_order 的记录总个数，可以发现，显示的结果是不一样的。所以，在使用 InnoDB 存储引擎时，就需要扫描表来统计具体的记录。

而当带上 WHERE 条件语句之后，MyISAM 跟 InnoDB 就没有区别了，它们都需要扫描表来进行记录个数的统计。

## [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#如何优化-COUNT)如何优化 `COUNT(*)`？

如果对一张大表经常用 `COUNT(*)` 来做统计，其实是很不好的。

比如下面我这个案例，表 t_order 共有 1200 多万条记录，我也创建了二级索引，但是执行一次 `SELECT COUNT(*) FROM t_order` 要花费差不多 5 秒！

![图片](https://cdn.xiaolincoding.com//mySQL/other/74a4359b58dc6ed41a241e425f43764d.png)

面对大表的记录统计，我们有没有什么其他更好的办法呢？

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#第一种-近似值)第一种，近似值

如果你的业务对于统计个数不需要很精确，比如搜索引擎在搜索关键词的时候，给出的搜索结果条数是一个大概值。

![图片](https://cdn.xiaolincoding.com//mySQL/other/cd18879de0c0b37660f53a5f1af3d172.png)

这时，我们就可以使用 show table status 或者 explain 命令来表进行估算。

<font color=blue>执行 explain 命令效率是很高的，因为它并不会真正的去查询</font>，下图中的 rows 字段值就是 explain 命令对表 t_order 记录的估算值。

![图片](https://cdn.xiaolincoding.com//mySQL/other/7590623443e8f225e5652109e6d9e3d2.png)

> **`EXPLAIN` 里看到的 `rows` 通常是估计值，不是实际扫描行数。**
>
> 它表示的是：**优化器认为**执行这一步时，大概要读取多少行。
>
> 几个关键点：
>
> 1. **`rows` 是基于统计信息估算出来的**
>    MySQL 会根据：
>    - 索引基数（cardinality）
>    - 数据分布
>    - 谓词选择性
>    - 表统计信息
>      来推测要扫多少行。
> 2. **所以它可能不准**
>    尤其在这些情况下容易偏差很大：
>    - 统计信息过旧
>    - 数据分布严重倾斜
>    - 有范围查询
>    - 多列条件相关性很强，但优化器按独立性估算
>    - 使用了不够合适的索引
> 3. **`EXPLAIN` 看的是“计划预估”**
>    它不是执行后的真实结果。
>
> 如果你想看更接近真实执行的数据，应该看：
>
> - `EXPLAIN ANALYZE`
>   这个会真正执行 SQL，并给出 **actual rows**、实际耗时等信息。
>   这比普通 `EXPLAIN` 更能看出估算是否失真。
>
> 例如可以这样理解：
>
> - `EXPLAIN`：我**预计**扫 1000 行
> - `EXPLAIN ANALYZE`：我**实际**扫了 8 万行
>
> 补充一点：
>
> - `filtered` 也是估计值
> - 一般可以粗略看 `rows × filtered` 来理解经过这一层后预计剩多少行
> - 但这也仍然是优化器估算，不是实测值
>
> 一句话总结：
>
> **普通 `EXPLAIN` 里的 `rows` 是优化器估计值；想看真实执行行数，用 `EXPLAIN ANALYZE`。**
>
> 你要的话，我可以顺手把 `EXPLAIN` 里 `rows`、`filtered`、`type`、`key_len` 这几个最容易混淆的字段一起讲明白。

### [#](https://www.xiaolincoding.com/mySQL/index/COUNT.html#第二种-额外表保存计数值)第二种，额外表保存计数值

如果是想精确的获取表的记录总数，我们可以将这个计数值保存到单独的一张计数表中。

当我们在数据表插入一条记录的同时，将计数表中的计数字段 +1。也就是说，在新增和删除操作时，我们需要额外维护这个计数表。

# [优化]系统配置的优化

**1. 核心内存池 (`innodb_buffer_pool_size`)** 这是 MySQL 性能调优的“一哥”。一般建议把这台服务器 **50% ~ 80%** 的物理内存都分给它。它专门用来缓存表数据和索引，值越大，磁盘 IO 就越少。

**2. 刷盘策略（双 1 配置折中）** 影响写入性能最大的参数是 `innodb_flush_log_at_trx_commit` 和 `sync_binlog`。默认都是 1（最安全，断电不丢数据，但最慢）。

- 如果是**非核心业务**（比如日志、点击流），追求极致写入速度，可以把 `innodb_flush...` 改成 2，`sync_binlog` 改成 0，性能能直接翻倍。

**3. 连接数与系统句柄**

- 把 MySQL 的 `max_connections` 调高（比如 1000-2000），应对高并发。
- **系统联动**：一定要把 Linux 系统的最大文件打开数（`ulimit -n`）调高到 65535 甚至更高，否则并发一来直接报错 "Too many open files"。

**4. 系统层面：按死 Swap（虚拟内存）** 内存不够时，Linux 会把硬盘当内存用（Swap）。这对数据库是灾难，会导致性能断崖式下跌。建议把操作系统的 `vm.swappiness` 参数设为 1（尽量不用 Swap），甚至直接关闭 Swap 交换区。

# [优化]容灾

数据库容灾，说白了就是“主数据库机房突然遭遇停电、火灾、光缆被挖断等物理毁灭时，怎么保证数据不丢、业务不停”。

根据你愿意花多少钱，容灾方式主要分为以下三个核心层级：

- **<font color=blue>定期冷备（最省钱，但最痛）</font>**： 就像每天下班前把文件拷到 U 盘带回家。通常是每天半夜把数据库全量打包，存到另一个机房或者云对象存储里。
  - **代价**：一旦白天主库炸了，你要花几个小时把数据重新导出来恢复（业务长时间瘫痪），而且昨晚到今天宕机期间产生的所有新数据，彻底丢失。
- **<font color=blue>主从热备 / 异地容灾（性价比最高，主流标配）</font>**： 搞一个主库接客，同时在另一个城市的机房养一个“备胎”从库。主库写一条数据，立刻通过网络给从库发一条。
  - **代价**：当主库被一锅端时，系统可以自动或手动将“备胎”扶正接客。业务只会中断几十秒到几分钟，最多丢失最后没来得及发给备胎的那几毫秒的数据。
- **<font color=blue>异地多活（终极防线，极度烧钱）</font>**： 大型互联网公司（如微信、支付宝）的玩法。在不同城市建好几个机房，**大家都是主库，都在同时接客**，底层数据通过复杂的分布式共识算法（如 Paxos/Raft）保持强一致。
  - **代价**：技术难度极大，建设成本极高。但效果也是神级的——哪怕某一个城市的机房被陨石砸了，流量瞬间自动切到其他城市，用户连卡顿都感觉不到（真正的数据零丢失，业务零中断）。

# [优化]分库分表/分片/sharding

> shard：碎片、分片

物理服务机的 CPU、内存、存储设备、连接数等资源有限，某个时段大量连接同时执行操作，会导致数据库在处理上遇到性能瓶颈。为了解决这个问题，行业先驱门充分发扬了分而治之的思想，对大库表进行分割，

然后实施更好的控制和管理，同时使用多台机器的 CPU、内存、存储，提供更好的性能。而分治有两种实现方式：垂直拆分和水平拆分。

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.3uvk0d2jdl.png)

**<FONT COLOR=BLUE>1. 垂直拆分（按业务或字段切）</FONT>**

- **垂直分库**：把一个大杂烩数据库按业务模块拆开，比如把“电商库”拆成独立的“用户库”、“订单库”和“商品库”。这能做到业务解耦，并分摊单台服务器的连接数和 CPU 压力。
- **垂直分表**：针对字段极多的“宽表”，把高频访问的核心字段放一张表，低频访问的长文本或大字段拿出来单建一张扩展表，从而提升查询时的内存利用率。

**<FONT COLOR=BLUE>2. 水平拆分（按数据行打散）</FONT>**

当单表数据量达到千万级别（比如 MySQL），导致查询极其缓慢且无法通过索引优化时，就需要水平拆分。表结构完全不变，而是把海量的数据行按照规则（<u>比如按用户 ID 哈希取模，或者按时间范围</u>）分散到多个库或表中。这是彻底解决单机存储和 I/O 瓶颈的终极方案。

**<FONT COLOR=BLUE>3. 伴随而来的技术代价</FONT>**

架构界有句老话：“能不拆就不拆”。分库分表虽然解决了容量和并发问题，但会带来系统复杂度的剧增。拆分后，你必须解决 **<u>跨库跨表 Join 查询、分布式事务保证、全局唯一 ID 生成</u>** 等一系列复杂的分布式难题。

下边我们以经典的订单业务举例，看看如何拆分。

- **<font color=blue>垂直分库</font>**：一般来说 <font color=24b3aa>按照业务和功能的维度进行拆分</font>，将不同业务数据分别放到不同的数据库中，核心理念 <font color=24b3aa>专库专用</font>。按业务类型对数据分离，剥离为多个数据库，像订单、支付、会员、积分相关等表放在对应的订单库、支付库、会员库、积分库。垂直分库把一个库的压力分摊到多个库，提升了一些数据库性能，但<font color=24b3aa>并没有解决由于单表数据量过大导致的性能问题</font>，所以就需要配合后边的分表来解决。
- **<font color=blue>垂直分表</font>**：针对业务上字段比较多的大表进行的，<font color=24b3aa>一般是把业务宽表中比较独立的字段，或者不常用的字段拆分到单独的数据表中</font>，是一种大表拆小表的模式。数据库它是以行为单位将数据加载到内存中，这样拆分以后核心表大多是访问频率较高的字段，而且字段长度也都较短，因而<font color=24b3aa>可以加载更多数据到内存中，减少磁盘IO，增加索引查询的命中率，进一步提升数据库性能。</font>
- **<font color=blue>水平分库（分库分表）</font>**：是把同一个表按一定规则拆分到不同的数据库中，每个库可以位于不同的服务器上，以此实现水平扩展，是一种常见的提升数据库性能的方式。这种方案往往能解决单库存储量及性能瓶颈问题，但由于同一个表被分配在不同的数据库中，数据的访问需要额外的路由工作，因此系统的复杂度也被提升了。
- **<font color=blue>水平分表（库内分表）</font>**：是在**同一个数据库内**，把一张大数据量的表按一定规则，切分成多个结构完全相同表，而每个表只存原表的一部分数据。水平分表尽管拆分了表，但子表都还是在同一个数据库实例中，只是解决了单一表数据量过大的问题，并没有将拆分后的表分散到不同的机器上，还在竞争同一个物理机的CPU、内存、网络IO等。要想进一步提升性能，就需要将拆分后的表分散到不同的数据库中，达到分布式的效果。

## 1. 垂直拆分

总体来说：垂直拆分有以下优点：

- <font color=24b3aa>跟随业务进行分割，类似微服务的分治理念，方便解耦之后的管理及扩展。</font>
- 高并发的场景下，垂直拆分使用多台服务器的 CPU、I/O、内存能提升性能，同时对单机数据库连接数、一些资源限制也得到了提升，能实现<font color=24b3aa>冷热数据的分离</font>。

垂直拆分的缺点：

- 部分业务表无法 join，应用层需要很大的改造，只能通过聚合的方式来实现。增加了开发的难度。
- 单表数据量膨胀的问题依然没有得到有效的解决。分布式事务也是一个难题。

### 1.1 垂直分库

先说说垂直分库。垂直分库其实是一种简单逻辑分割。比如我们的数据库中有商品表 Products、还有对订单表 Orders，还有积分表 Scores。接下来我们就可以创建三个数据库，一个数据库存放商品，一个数据库存放订单，一个数据库存放积分。

垂直分库有一个优点，他能够根据业务场景进行孵化，比如某一单一场景只用到某 2-3 张表，基本上应用和数据库可以拆分出来做成相应的服务。

### 1.2 垂直分表

垂直分表在日常开发和设计中比较常见，通俗的说法叫做“大表拆小表”，拆分是基于关系型数据库中的“列”（字段）进行的。通常情况，某个表中的字段比较多，可以新建立一张<font color=blue>“扩展表”</font>，将不经常使用或者长度较大的字段拆分出去放到“扩展表”中。

除了这种 <font color=blue>访问频率的冷热拆分</font> 之外，还可以 <font color=blue>按照字段类型结构来拆分</font>，比如大文本字段单独放在一个表中，与基础字段隔离，提高基础字段的访问效率。

也可以<font color=blue>将字段按照功能用途来拆分</font>，比如采购的物料表可以按照基本属性、销售属性、采购属性、生产制造属性、财务会计属性等用途垂直拆分。

在字段很多的情况下，拆分开确实更便于开发和维护（笔者曾见过某个遗留系统中，一个大表中包含 100 多列的）。<font color=blue>某种意义上也能避免“跨页”的问题</font>（MySQL、MSSQL 底层都是通过“数据页”来存储的，“跨页”问题可能会造成额外的性能开销，这里不展开，感兴趣的朋友可以自行查阅相关资料进行研究）。

拆分字段的操作建议在数据库设计阶段就做好。如果是在发展过程中拆分，则需要改写以前的查询语句，会额外带来一定的成本和风险，建议谨慎。

## 2. 水平拆分

### 2.1 水平分表（库内分表）

假设当我们的 Orders 表达到了 5000 万行记录的时候，非常影响数据库的读写效率，怎么办呢？

我们可以考虑按照订单编号的 order_id 进行 rang 分区，就是把订单编号在 1-1000 万的放在 order1 表中，将编号在 1000万 - 2000 万的放在 order2 中，以此类推，每个表中存放 1000 万数据。

关于水平分表的时机，业内的标准不是很统一，阿里的 Java 开发手册的标准是当单表行数超过 500 万行或者单表容量超过 2GB 时，才推荐进行分库分表。百度的则是 1000W 行的进行分表，这个是百度的 DBA 经过测试推算出的结果。

但是这边忽略了单表的字段数和字段类型，如果字段数很多，超过 50 列，对性能影响也是不小的，我们曾经有个业务，表字段是随着业务的增长而自动扩增的，到了后期，字段越来越多，查询性能也越来越慢。

所以个人觉得不必拘泥于 500W 还是 1000W，开发人员在使用过程中，如果压测发现因为数据基数变大而导致执行效率慢下来，就可以开始考虑分表了。

水平分表，能够降低单表的数据量，一定程度上可以缓解查询性能瓶颈。<font color=24b3aa>但本质上这些表还保存在同一个库中，所以库级别还是会有 IO 瓶颈。所以，一般不建议采用这种做法。</font>

目前在 MySQL 中支持四种表分区的方式，分别为 HASH、RANGE、LIST 及 KEY，当然在其它的类型数据库中，分区的实现方式略有不同，但是分区的思想原理是相同，具体如下：

#### 2.1.1 HASH

HASH 分区主要用来确保数据在预先确定数目的分区中平均分布，而在 RANGE 和 LIST 分区中，必须明确指定一个给定的列值或列值集合应该保存在哪个分区中，而在 HASH 分区中，MySQL 自动完成这些工作，

你所要做的只是基于将要被哈希的列值指定一个列值或表达式，以及指定被分区的表将要被分割成的分区数量。 示例如下：

```sql
 1 drop table if EXISTS  `t_userinfo`; 
 2 CREATE TABLE `t_userinfo` (
 3 `id` int(10) unsigned NOT NULL,
 4 `personcode` varchar(20) DEFAULT NULL,
 5 `personname` varchar(100) DEFAULT NULL,
 6 `depcode` varchar(100) DEFAULT NULL,
 7 `depname` varchar(500) DEFAULT NULL,
 8 `gwcode` int(11) DEFAULT NULL,
 9 `gwname` varchar(200) DEFAULT NULL,
10 `gravalue` varchar(20) DEFAULT NULL,
11 `createtime` DateTime NOT NULL
12 ) ENGINE=InnoDB DEFAULT CHARSET=utf8
13 PARTITION BY HASH(YEAR(createtime))
14 PARTITIONS 10; 
```

上面的例子，使用 HASH 函数对 createtime 日期进行 HASH 运算，并根据这个日期来分区数据，这里共分为 10 个分区。 

建表语句上添加一个“PARTITION BY HASH (expr)”子句，其中“expr”是一个返回整数的表达式，它可以是字段类型为 MySQL  整型的一列的名字，也可以是返回非负数的表达式。

另外，可能需要在后面再添加一个“PARTITIONS num”子句，其中 num 是一个非负的整数，它表示表将要被分割成分区的数量。 

#### 2.1.2 RANGE

基于属于一个给定连续区间的列值，把多行分配给同一个分区，这些区间要连续且不能相互重叠，使用 VALUES LESS THAN 操作符来进行定义。示例如下：

```sql
 1 drop table if EXISTS  `t_userinfo`; 
 2 CREATE TABLE `t_userinfo` (
 3 `id` int(10) unsigned NOT NULL,
 4 `personcode` varchar(20) DEFAULT NULL,
 5 `personname` varchar(100) DEFAULT NULL,
 6 `depcode` varchar(100) DEFAULT NULL,
 7 `depname` varchar(500) DEFAULT NULL,
 8 `gwcode` int(11) DEFAULT NULL,
 9 `gwname` varchar(200) DEFAULT NULL,
10 `gravalue` varchar(20) DEFAULT NULL,
11 `createtime` DateTime NOT NULL
12 ) ENGINE=InnoDB DEFAULT CHARSET=utf8
13 PARTITION BY RANGE(gwcode) (
14 PARTITION P0 VALUES LESS THAN(101) ,
15 PARTITION P1 VALUES LESS THAN(201) ,
16 PARTITION P2 VALUES LESS THAN(301) ,
17 PARTITION P3 VALUES LESS THAN MAXVALUE
18 );
```

上面的示例，使用了范围 RANGE 函数对岗位编号进行分区，共分为 4 个分区，

岗位编号为 1~100 的对应在分区 P0 中，101~200 的编号在分区 P1 中，依次类推即可。那么类别编号大于 300，可以使用 MAXVALUE 来将大于 300 的数据统一存放在分区 P3 中即可。 

#### 2.1.3 LIST

类似于按 RANGE 分区，区别在于 LIST 分区是基于列值匹配一个<font color=blue>**离散值集合**</font>中的某个值来进行选择分区的。LIST 分区通过使用“PARTITION BY LIST(expr)”来实现，其中“expr” 是某列值或一个基于某个列值、并返回一个整数值的表达式，

然后通过“VALUES IN (value_list)”的方式来定义每个分区，其中“value_list”是一个通过逗号分隔的整数列表。 示例如下：

```sql
 1 drop table if EXISTS  `t_userinfo`; 
 2 CREATE TABLE `t_userinfo` (
 3 `id` int(10) unsigned NOT NULL,
 4 `personcode` varchar(20) DEFAULT NULL,
 5 `personname` varchar(100) DEFAULT NULL,
 6 `depcode` varchar(100) DEFAULT NULL,
 7 `depname` varchar(500) DEFAULT NULL,
 8 `gwcode` int(11) DEFAULT NULL,
 9 `gwname` varchar(200) DEFAULT NULL,
10 `gravalue` varchar(20) DEFAULT NULL,
11 `createtime` DateTime NOT NULL
12 ) ENGINE=InnoDB DEFAULT CHARSET=utf8
13 PARTITION BY LIST(`gwcode`) (
14 PARTITION P0 VALUES IN (46,77,89) ,
15 PARTITION P1 VALUES IN (106,125,177) ,
16 PARTITION P2 VALUES IN (205,219,289) ,
17 PARTITION P3 VALUES IN (302,317,458,509,610) 
18 );
```

上面的例子，使用了列表匹配 LIST 函数对员工岗位编号进行分区，共分为 4 个分区，编号为 46,77,89 的对应在分区 P0 中，106,125,177 类别在分区 P1 中，依次类推即可。

<font color=24b3aa>不同于 RANGE 的是，LIST 分区的数据必须匹配列表中的岗位编号才能进行分区，所以这种方式只是适合比较区间值确定并少量的情况。 </font>

#### 2.1.4 Composite

Composite，顾名思义，是上面几种模式的组合使用，比如你在 Range 的基础上，再进行 Hash 哈希分区。

### 2.2 水平分库（分库分表）

<font color=24b3aa>库内分表解决了单表数据量过大的瓶颈问题，但使用还是同一主机的 CPU、IO、内存，另外单库的连接数也有限制，并不能完全的降低系统的压力。</font>

此时，我们就要考虑另外一种技术叫分库分表。<font color=ee11ff>分库分表在库内分表的基础上，将分的表挪动到不同的主机和数据库上。可以充分的使用其他主机的 CPU、内存和 IO 资源。</font> 拆分方式进一步演进到下面：

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.67y6hl93wd.png)

> <font color=24b3aa>注意不要把这里的分库分表和笼统意义上的分库分表混淆了。</font>
>
> * <font color=24b3aa>我们这里所说的分库分表是狭义上的水平分库，把表分到不同的数据库上。</font>
> * <font color=24b3aa>而笼统意义上的分库分表，包含了水平分库、水平分表，垂直分库、垂直分表</font>

## 3. 分库分表带来的问题

### 3.1 事务问题

在执行分库分表之后，由于数据存储到了不同的库上，数据库事务管理出现了困难。<font color=24b3aa>如果依赖数据库本身的分布式事务管理功能去执行事务，将付出高昂的性能代价；如果由应用程序去协助控制，形成程序逻辑上的事务，又会造成编程方面的负担。</font>

- 方案一：使用分布式事务                            
  - 优点：交由数据库管理，简单有效
  - 缺点：性能代价高，特别是 shared 越来越多时
- 方案二：由应用程序和数据库共同控制                            
  - 原理：将一个跨多个数据库的分布式事务分拆成多个仅处 于单个数据库上面的小事务，并通过应用程序来总控 各个小事务。 
  - 优点：性能上有优势 
  - 缺点：需要应用程序在事务控制上做灵活设计。如果使用 了 [spring](https://link.juejin.cn?target=http%3A%2F%2Flib.csdn.net%2Fbase%2Fjavaee) 的事务管理，改动起来会面临一定的困难。

### 3.2 跨库跨表的 join 问题

在执行了分库分表之后，难以避免会将原本逻辑关联性很强的数据划分到不同的表、不同的库上，这时，表的关联操作将受到限制，<font color=24b3aa>我们无法 join 位于不同分库的表，也无法 join 分表粒度不同的表，结果原本一次查询能够完成的业务，可能需要多次查询才能完成。 </font>

#### 全局表

所谓全局表，就是有可能系统中所有模块都可能会依赖到的一些表。比较类似我们理解的“数据字典”。<font color=24b3aa>为了避免跨库 join 查询，我们可以将这类表在其他每个数据库中均保存一份。同时，这类数据通常也很少发生修改（甚至几乎不会），所以也不用太担心“一致性”问题。</font>

#### 字段冗余

这是一种典型的反范式设计，在互联网行业中比较常见，通常是为了性能来避免 join 查询。

举个电商业务中很简单的场景：

<font color=24b3aa>“订单表”中保存“卖家 Id”的同时，将卖家的“Name”字段也冗余，这样查询订单详情的时候就不需要再去查询“卖家用户表”。</font>

字段冗余能带来便利，是一种“空间换时间”的体现。但其适用场景也比较有限，比较适合依赖字段较少的情况。最复杂的还是<font color=24b3aa>数据一致性问题</font>，这点很难保证，可以借助数据库中的触发器或者在业务代码层面去保证。当然，也需要结合实际业务场景来看一致性的要求。就像上面例子，如果卖家修改了 Name 之后，是否需要在订单信息中同步更新呢？

* A 库中的 tab_a 表和 B 库中 tbl_b 有关联，可以定时将指定的表做同步。当然，同步本来会对数据库带来一定的影响，需要性能影响和数据时效性中取得一个平衡。这样来避免复杂的跨库查询。笔者曾经在项目中是通过 ETL 工具来实施的。

### 3.3 额外的数据管理负担和数据运算压力

额外的数据管理负担，最显而易见的就是<font color=blue>数据的定位问题和数据的增删改查的重复执行问题，这些都可以通过应用程序解决，但必然引起额外的逻辑运算</font>，例如，对于一个记录用户成绩的用户数据表 userTable，业务要求查出成绩最好的 100 位，在进行分表之前，

只需一个 order by 语句就可以搞定，但是在进行分表之后，将需要 n 个 order by 语句，分别查出每一个分表的前 100 名用户数据，然后再对这些数据进行合并计算，才能得出结果。

### 3.4 分布式全局唯一 ID

在很多中小项目中，我们往往直接使用数据库自增特性来生成主键 ID，这样确实比较简单。而在分库分表的环境中，数据分布在不同的分片上，不能再借助数据库自增长特性直接生成，否则会造成不同分片上的数据表主键会重复。简单介绍下使用和了解过的几种 ID 生成算法。

1. Twitter 的 Snowflake（又名“雪花算法”）
2. UUID/GUID（一般应用程序和数据库均支持）
3. MongoDB ObjectID（类似 UUID 的方式）
4. Ticket Server（数据库生存方式，Flickr 采用的就是这种方式）

其中，Twitter 的 Snowflake 算法是笔者近几年在分布式系统项目中使用最多的，未发现重复或并发的问题。该算法生成的是 64 位唯一 Id（由 41 位的 timestamp+ 10 位自定义的机器码 + 13 位累加计数器组成）。这里不做过多介绍，感兴趣的读者可自行查阅相关资料。

### 3.5 分片字段该如何选择

在开始分片之前，我们首先要确定分片字段（也可称为“片键”）。很多常见的例子和场景中是采用 ID 或者时间字段进行拆分。这也并不绝对的，我的建议是结合实际业务，通过对系统中执行的 SQL 语句进行统计分析，选择出需要分片的那个表中最频繁被使用，或者最重要的字段来作为分片字段。

### 3.6 跨节点的 `count`、``order`、`group by` 以及聚合函数问题

这些是一类问题，因为它们都需要 <font color=24b3aa>基于全部数据集合进行计算</font>。多数的代理都不会自动处理合并工作。

解决方案：<font color=24b3aa>与解决跨节点 join 问题的类似，分别在各个节点上得到结果后在应用程序端进行合并。</font>和 join不同的是每个结点的查询可以并行执行，因此很多时候它的速度要比单一大表快很多。但如果结果集很大，对应用程序内存的消耗是一个问题。

### 3.7 跨分片的排序分页

一般来讲，分页时需要按照指定字段进行排序。当排序字段就是分片字段的时候，我们通过分片规则可以比较容易定位到指定的分片，而当排序字段非分片字段的时候，情况就会变得比较复杂了。为了最终结果的准确性，我们需要在不同的分片节点中将数据进行排序并返回，并将不同分片返回的结果集进行汇总和再次排序，最后再返回给用户。

> **1. 浅分页的假象（第 1 页）** 当你只要第 1 页的 10 条数据时（`LIMIT 0, 10`），3 个分片各自拿出自己的前 10 条。中间件（网关）总共收到 30 条数据，在内存里排个序，抽出前 10 条给你。一切看起来轻松无压力。
>
> **2. 深分页的灾难（第 10 页）** 当你想要第 10 页数据时（跳过前 90 条，取 10 条，即 `LIMIT 90, 10`），问题来了：中间件根本不知道全局排名前 90 的数据到底分布在哪个分片里。为了保证结果绝对准确，**每个分片都必须把自己的前 100 条数据全传过来**。 这时，中间件收到了 300 条数据，在内存里满头大汗地重排，最后无情地丢掉前 290 条，只截取最后的 10 条给你。页数越深，内存和 CPU 的负担呈指数级爆炸。

那如何解决分库情况下的分页问题呢？有以下几种办法：

* <font color=24b3aa>如果是在前台应用提供分页，则**限定用户只能看前面 n 页**，这个限制在业务上也是合理的，一般看后面的分页意义不大（如果一定要看，可以要求用户缩小范围重新查询）。</font>
* <font color=24b3aa>如果是后台批处理任务要求分批获取数据，则可以**加大 page size**，比如每次获取 5000 条记录，有效减少分页数（当然离线访问一般走备库，避免冲击主库）。</font>
* <font color=24b3aa>开发中最常用的一招其实是**游标分页（Cursor Pagination）**。 它抛弃了传统的 `LIMIT offset, size`，改用上一页最后一条记录的标识来查：`WHERE 排序字段 > 上次最大值 LIMIT size`。这样每次都精准定位，完美避开无效数据的传输和全量排序，多深的分页都不卡。</font>

分库设计时，一般还有配套大数据平台汇总所有分库的记录，有些分页查询可以考虑走大数据平台。

# [优化]数据冷热分离

## 什么是数据冷热分离？

数据冷热分离是指根据数据的**<font color=red>访问频率</font>**和**<font color=red>业务重要性</font>**，将数据划分为冷数据和热数据，并<font color=blue>分别存储在不同性能和成本的存储介质中</font>的架构策略。

这种架构的核心目标有三个：

1. **<font color=ee11ff>提升查询性能</font>**：热数据存储在高性能介质（如 SSD、内存）中，保障核心业务的响应速度。
2. **<font color=ee11ff>降低存储成本</font>**：冷数据迁移至低成本介质（如 HDD、对象存储），大幅削减存储开支。
3. **<font color=ee11ff>满足合规要求</font>**：部分行业（如金融、医疗）要求数据长期归档，冷热分离可兼顾合规与成本。

### 冷数据和热数据

**<FONT COLOR=BLUE>热数据</FONT>** 是指被频繁访问和修改、且需要快速响应的数据；**<FONT COLOR=BLUE>冷数据</FONT>** 是指访问频率极低、对当前业务价值较小、但需要长期保留的数据。

冷热数据的区分方法主要有两种：

1. **<font color=24b3aa>时间维度区分</font>**：按照数据的创建时间、更新时间或过期时间划分。例如，订单系统将一段时间前（如 90 天或 1 年）的订单数据标记为冷数据。该方法适用于**数据访问频率与时间强相关**的场景，实现简单、成本低。
2. **<font color=24b3aa>访问频率区分</font>**：将高频访问的数据视为热数据，低频访问的数据视为冷数据。例如，内容系统将**浏览量低于阈值**的文章标记为冷数据。该方法需要额外记录访问频率，适用于**访问频率与数据本身特性强相关**的场景。

**如何选择区分策略？**

- 若业务数据天然具有时效性（如订单、日志、账单），优先选择**时间维度**，实现成本最低。
- 若数据价值与时间无关（如文章、商品、用户画像），需结合**访问频率**进行判定。
- 实际项目中，可将两者结合使用：以时间维度为主、访问频率为辅，覆盖更多业务场景。

### 冷热分离的多级分层策略

实际落地时，"冷"与"热"往往不是非此即彼的二分法，而是**渐进式多级分层**：

| 层级         | 数据特性             | 判定规则示例                | 存储策略               |
| ------------ | -------------------- | --------------------------- | ---------------------- |
| **热数据**   | 高频访问、实时响应   | 最近 30 天 + 所有未完成订单 | MySQL 热库（SSD）      |
| **温数据**   | 中频访问、可能被查询 | 30~90 天前的订单            | MySQL 温库（HDD）      |
| **冷数据**   | 低频访问、偶发查询   | 90 天~3 年的历史订单        | 独立冷库或对象存储     |
| **归档数据** | 极少访问、仅合规留存 | 超过 3 年的订单             | 对象存储（仅保留汇总） |

**实践建议**：判定规则应通过**配置中心**动态管理，避免因业务变化导致频繁修改代码。

### 冷数据被访问后如何处理？

如果冷数据突然被访问（如用户查询 3 年前的订单），是否需要"热升级"？

| 策略         | 适用场景               | 优点                 | 缺点                         |
| ------------ | ---------------------- | -------------------- | ---------------------------- |
| **不回迁**   | 偶发查询、查询频率极低 | 实现简单             | 查询速度慢                   |
| **缓存层**   | 中等频率查询           | 加速查询、不改变存储 | 需要额外缓存组件             |
| **异步回迁** | 高频查询、需要持续访问 | 彻底解决性能问题     | 实现复杂、可能产生一致性问题 |

**推荐做法**：绝大多数场景采用"**<font color=blue>不回迁 + 缓存层</font>**"的组合方案。<font color=24b3aa>冷数据查询时，先查缓存，命中则直接返回；未命中则查冷库并将结果写入缓存（针对偶发查询，设置 5~15 分钟的短暂 TTL 即可）。</font>

**⚠️注意**：为防止恶意攻击者利用随机参数频繁查询不存在的数据导致冷库被击穿，可以在缓存层前置 **<font color=ee11ff>布隆过滤器（Bloom Filter）</font>**或在缓存中设置**<font color=ee11ff>空值占位符</font>**，避免恶意请求穿透到冷库。详细介绍参考 [Redis 常见面试题总结(下)](https://javaguide.cn/database/redis/redis-questions-02.html)（Redis 事务、性能优化、生产问题、集群、使用规范等）。

### 冷热分离的思想

冷热分离的核心思想是**分层存储（Tiered Storage）**，根据数据的访问特性将其分配到不同层级的存储介质中。在企业级存储架构中，通常划分为以下层级：

| 层级                  | 数据特性           | 典型存储介质         | 访问延迟    |
| --------------------- | ------------------ | -------------------- | ----------- |
| **Hot（热层）**       | 高频访问、实时响应 | NVMe SSD、内存       | 毫秒级      |
| **Warm（温层）**      | 中频访问、近期数据 | SATA SSD、高速 HDD   | 百毫秒级    |
| **Cold（冷层）**      | 低频访问、历史数据 | 大容量 HDD、对象存储 | 秒级        |
| **Archive（归档层）** | 极少访问、合规留存 | 磁带库、冰川存储     | 分钟~小时级 |

这种分层思想在 IT 基础设施中被广泛应用，不仅限于数据库，还包括文件系统、对象存储、CDN 缓存等场景。

### 数据冷热分离的优缺点

**优点：**

- **<font color=ee11ff>热数据查询性能优化</font>**：热数据集中在高性能存储上，表数据量大幅减少，索引效率显著提升，用户的绝大部分操作体验会更好。
- **<font color=ee11ff>存储成本大幅降低</font>**：冷数据可迁移至 HDD 或对象存储，**SSD 与 HDD 的单位成本差距可达 5~10 倍**，对于海量数据场景节省效果显著。
- **<font color=ee11ff>系统可维护性增强</font>**：热库数据量可控，备份恢复速度更快，DDL 操作（如加索引）耗时更短。

**缺点：**

- **<font color=ee11ff>系统复杂性增加</font>**：需要额外的迁移组件、路由逻辑和监控体系，数据一致性风险增加。
- **<font color=ee11ff>跨库查询效率低</font>**：若业务需要同时查询冷热数据（如年度统计报表），需进行跨库关联或数据聚合，查询性能和开发成本均会上升。
- **<font color=ee11ff>迁移策略维护成本</font>**：冷热数据的判定规则需要持续调优，避免误判导致热数据被错误迁移。

## 冷数据迁移

### 冷数据如何迁移？

| 方案                | 实现原理                                                     | 优点                   | 缺点                                         | 适用场景                                             |
| ------------------- | ------------------------------------------------------------ | ---------------------- | -------------------------------------------- | ---------------------------------------------------- |
| **业务层代码实现**  | 写操作时判断冷热，直接路由到对应库                           | 实时性高               | 侵入业务代码、判定逻辑复杂                   | 几乎不使用                                           |
| **任务调度迁移**    | <font color=blue>定时任务扫描热库，批量迁移符合条件的数据</font> | 实现简单               | 存在迁移延迟、扫表可能污染 Buffer Pool       | **<font color=blue>时间维度区分场景</font>**         |
| **Binlog 监听迁移** | <font color=blue>监听数据库变更日志，实时或准实时迁移</font> | 实时性好、对业务无侵入 | 需要额外组件（如 Canal）、不适合时间维度判定 | **<font color=blue>访问频率区分场景（推荐）</font>** |

**任务调度迁移**是最常用的方案，可借助 XXL-Job、Elastic-Job 等分布式任务调度平台实现。关于任务调度的方案，我也写过文章详细介绍，可以查看这篇文章：[Java 定时任务详解](https://javaguide.cn/system-design/schedule-task.html) 。

> ⚠️ **风险提示**：<font color=red>任务调度迁移在大数据量下存在性能隐患。大范围的扫表操作（如 `SELECT * FROM orders WHERE create_time < 'xxx' LIMIT 10000`）会严重污染 InnoDB Buffer Pool，将真正的业务热数据挤出内存。</font>**生产环境建议**：
>
> - 使用**基于主键的范围查询**，避免全表扫描；
> - 控制**单次迁移批量大小**，分批执行；
> - 在**业务低峰期**执行迁移任务；
> - 对于海量数据，优先考虑 **Binlog 监听**方案，将对热库的冲击降到最低。

典型流程如下：

![冷热分离 - 冷数据迁移](https://oss.javaguide.cn/github/javaguide/high-performance/data-cold-hot-separation.png)

**实践建议**：若公司有 DBA 支持，可先进行一次**存量冷数据的人工迁移**，将历史数据批量导入冷库；后续再通过任务调度实现**增量迁移**的自动化。

### 迁移过程中如何保证数据一致性？

数据迁移过程中，最棘手的问题是：**如果数据在迁移过程中被更新，如何处理？**

#### [常见解决方案](https://javaguide.cn/high-performance/data-cold-hot-separation.html#常见解决方案)

| 方案                                        | 实现方式                               | 优点             | 缺点                                 |
| ------------------------------------------- | -------------------------------------- | ---------------- | ------------------------------------ |
| **迁移前锁定**                              | 迁移前对记录加写锁，迁移完成后释放     | 一致性强         | 影响业务写入、吞吐量下降             |
| **<font color=blue>版本号乐观锁</font>**    | 迁移时记录版本，删除前校验版本是否变化 | 无锁、性能好     | 需要业务表增加版本字段、冲突时需重试 |
| **<font color=blue>状态标记 + 幂等</font>** | 热库增加迁移状态字段，先标记再迁移     | 可追溯、支持回滚 | 需要改造业务表                       |

> **注意**：冷热库通常是**不同的数据库实例**，`INSERT`（冷库）和 `DELETE`（热库）无法放在同一个本地事务中，需要特殊处理跨库原子性问题。

#### [推荐方案：状态标记 + 幂等迁移](https://javaguide.cn/high-performance/data-cold-hot-separation.html#推荐方案-状态标记-幂等迁移)

在热库表中增加 `migrate_status` 字段，通过状态机保证迁移的原子性和可追溯性：

```sql
-- 1. 热库表增加迁移状态字段
ALTER TABLE orders ADD COLUMN migrate_status TINYINT DEFAULT 0
    COMMENT '0-未迁移 1-迁移中 2-已迁移';
```

```java
// 2. 迁移流程（伪代码，独立冷库场景需在应用层分步执行）

// Step 1: 标记为迁移中（热库事务）
hotDb.execute("UPDATE orders SET migrate_status = 1 WHERE id = ? AND migrate_status = 0", id);

// Step 2: 读取热库数据并写入冷库（需切换数据库连接）
Order order = hotDb.query("SELECT * FROM orders WHERE id = ?", id);
coldDb.execute("INSERT IGNORE INTO orders_cold VALUES (?, ?, ...)", order.id, order.data...);

// Step 3: 标记为已迁移（热库事务）
hotDb.execute("UPDATE orders SET migrate_status = 2 WHERE id = ? AND migrate_status = 1", id);

// Step 4: 延迟删除热库数据（可选，确认冷库数据无误后执行）
hotDb.execute("DELETE FROM orders WHERE id = ? AND migrate_status = 2", id);
```

> **注意**：独立冷库场景下，标准 MySQL 无法直接执行跨库 `INSERT ... SELECT`，必须在应用层拆分为"读取热库 → 写入冷库"两步。

**方案优势**：

- **幂等性**：`INSERT IGNORE` 保证冷库写入幂等，`migrate_status` 状态流转保证热库更新幂等。
- **可追溯**：通过状态字段可以查询迁移进度，异常时可以人工介入。
- **可回滚**：迁移失败时可以将状态重置为 0，重新迁移。
- **渐进式删除**：不立即删除热库数据，确认冷库无误后再清理，降低风险。

> **空间回收**：InnoDB 执行 `DELETE` 后仅将数据页标记为删除，物理空间不会立即释放给操作系统。需在**业务低峰期**执行 `OPTIMIZE TABLE` 或 `ALTER TABLE ENGINE=InnoDB` 重建表，才能真正回收磁盘空间。

**兜底机制**：

- **定时对账**：定期扫描 `migrate_status = 1` 超过阈值的记录，自动重置或告警。**注意**：`migrate_status` 字段区分度极低，必须配合联合索引（如 `idx_create_time_migrate_status`）限定扫描区间，避免全表扫描。
- **高频更新兜底**：对于因频繁更新导致多次跳过的记录，设置最大重试次数，超过后强制迁移或人工介入。

## [冷数据如何存储？](https://javaguide.cn/high-performance/data-cold-hot-separation.html#冷数据如何存储)

冷数据存储方案的选型原则是：**容量大、成本低、可靠性高，访问速度可适当牺牲**。

### [中小厂方案](https://javaguide.cn/high-performance/data-cold-hot-separation.html#中小厂方案)

直接使用 **MySQL/PostgreSQL** 即可，保持与热库相同的数据库类型，降低运维复杂度。具体实现方式：

- **同库分表**：在同一数据库中新增冷数据表（如 `order_history`），通过表名区分冷热数据。
- **独立冷库**：部署单独的数据库实例作为冷库，热库与冷库通过应用层路由访问。

**⚠️注意**：独立冷库方案涉及**跨库查询**，若业务存在冷热数据联合查询需求，需评估是否引入数据同步或聚合层。

### [大厂方案](https://javaguide.cn/high-performance/data-cold-hot-separation.html#大厂方案)

大厂通常采用专门针对海量数据优化的存储引擎：

| 存储方案               | 特点                             | 适用场景                         |
| ---------------------- | -------------------------------- | -------------------------------- |
| **HBase**              | 列族存储、高吞吐、支持 PB 级数据 | 日志、用户行为、IoT 数据归档     |
| **RocksDB**            | 高性能 KV 存储、LSM-Tree 结构    | 嵌入式场景、作为其他系统底层存储 |
| **Doris/ClickHouse**   | OLAP 引擎、支持实时分析          | 冷数据需要进行聚合分析的场景     |
| **Cassandra**          | 分布式、高可用、无单点故障       | 跨地域部署、高可用要求的归档场景 |
| **对象存储（OSS/S3）** | 成本极低、无限扩展               | 超大规模冷数据、合规归档         |

### [TiDB 方案（推荐）](https://javaguide.cn/high-performance/data-cold-hot-separation.html#tidb-方案-推荐)

如果公司技术栈允许，可以直接使用 **TiDB** 这类分布式关系型数据库，原生支持冷热分离，一步到位。

TiDB 6.0 引入了 **基于 SQL 接口的数据放置框架（Placement Rules in SQL）** 功能，用于通过 SQL 接口配置数据在 TiKV 集群中的放置位置。

- **热数据**：通过 Placement Rules 指定存储在 **SSD 节点**上，保障查询性能。
- **冷数据**：指定存储在 **HDD 节点**上，降低存储成本。

```sql
-- 创建放置策略：热数据存储在 SSD 节点
CREATE PLACEMENT POLICY hot_data
  CONSTRAINTS="[+disk=ssd]";

-- 创建放置策略：冷数据存储在 HDD 节点
CREATE PLACEMENT POLICY cold_data
  CONSTRAINTS="[+disk=hdd]";

-- 对表或分区应用放置策略
ALTER TABLE orders PLACEMENT POLICY = hot_data;
ALTER TABLE orders PARTITION p2022 PLACEMENT POLICY = cold_data;
```

这种方案的优势在于：**业务无需感知冷热分离逻辑**，数据路由由 TiDB 自动完成，大幅降低了应用层的复杂度。

> **完整实践**：`Placement Rules` 指定了数据存放的介质类型，但数据如何从"热分区"流转到"冷分区"仍需结合**分区表（Range Partitioning）**。按时间跨度创建分区，为历史分区绑定 HDD 放置策略，为当前活跃分区绑定 SSD 放置策略。随着时间推移，只需维护分区的创建与销毁，底层数据即可在不同介质间自然流转。

## [冷数据如何查询？](https://javaguide.cn/high-performance/data-cold-hot-separation.html#冷数据如何查询)

冷数据虽然访问频率低，但一旦需要查询（如审计、对账、年度报表），如何保证查询效率？

### [冷数据查询需求分析](https://javaguide.cn/high-performance/data-cold-hot-separation.html#冷数据查询需求分析)

首先需要明确：**业务是否真的需要查询冷数据？**

- **不需要**：可将冷数据完全移出业务库，仅保留归档（如对象存储），需要时人工提取。
- **需要**：需设计合理的查询方案，平衡性能与成本。

### [冷数据查询优化方案](https://javaguide.cn/high-performance/data-cold-hot-separation.html#冷数据查询优化方案)

| 优化手段             | 实现方式                                            | 适用场景       |
| -------------------- | --------------------------------------------------- | -------------- |
| **冷库独立只读实例** | 冷库部署只读副本，避免冷查询影响热库                | 高频冷查询场景 |
| **查询路由**         | 应用层根据时间范围自动路由到热库或冷库              | 跨冷热查询场景 |
| **预聚合**           | 定期对冷数据生成月度/季度报表，查询时直接查聚合结果 | 统计分析场景   |
| **列式存储**         | 冷库采用 ClickHouse、Doris 等 OLAP 引擎             | 大规模分析查询 |

**跨冷热查询的处理**：

若查询范围同时涉及冷热数据（如"查询近 2 年的订单"），有两种处理方式：

1. **拆分查询**：分别查询热库和冷库，应用层合并结果。
2. **限制范围**：提示用户缩小查询范围，避免跨库查询。

> **防雪崩预警**：若业务包含**全局分页排序**（如 `ORDER BY create_time LIMIT 10000, 20`），应用层必须从冷热库各拉取 `10000 + 20` 条记录进行内存归并，偏移量较大时极易引发 **OOM**。**强制要求**：
>
> - 限制查询时间范围，避免大跨度跨库查询；
> - 或引流至底层同步的宽表（如 ClickHouse）进行计算；
> - 严禁在应用层执行大深度的归并分页。

### [应用层如何路由冷热数据？](https://javaguide.cn/high-performance/data-cold-hot-separation.html#应用层如何路由冷热数据)

| 方案         | 实现方式                                 | 优点               | 缺点                         |
| ------------ | ---------------------------------------- | ------------------ | ---------------------------- |
| **硬编码**   | 代码中直接判断路由                       | 实现简单           | 维护成本高、规则变更需改代码 |
| **配置中心** | 路由规则存入配置中心（如 Nacos、Apollo） | 动态调整、无需重启 | 需要额外组件支持             |
| **Proxy 层** | 引入 ShardingSphere、ProxySQL 等中间件   | 业务无感知         | 架构复杂度高                 |

**推荐做法**：中小规模采用**配置中心**方案，大规模采用**Proxy 层**方案。

> ⚠️ **风险提示**：引入 Proxy 层后，所有跨冷热库的聚合计算（如全局排序、`GROUP BY` 归并分页）都会压在 Proxy 节点的内存与 CPU 上。需严格限制此类操作的最大返回行数，否则极易导致 Proxy 节点 **OOM（内存溢出）**。

## [冷热分离 vs 数据归档 vs 分区表](https://javaguide.cn/high-performance/data-cold-hot-separation.html#冷热分离-vs-数据归档-vs-分区表)

这三个概念容易混淆，需要区分清楚：

| 对比维度           | 冷热分离                   | 数据归档               | 分区表                     |
| ------------------ | -------------------------- | ---------------------- | -------------------------- |
| **数据是否可访问** | 冷数据仍在业务访问路径上   | 归档数据通常移出业务库 | 所有分区均可访问           |
| **存储介质**       | 冷热数据可跨实例、跨存储   | 通常迁移到低成本存储   | 同一实例内                 |
| **实现复杂度**     | 中等                       | 低                     | 低                         |
| **典型场景**       | 订单、日志等有时效性的数据 | 合规留存、数据备份     | 单表数据量大但无需分离存储 |

**分区表的局限性**：MySQL 分区表可以按时间分区，但所有分区仍在同一个实例中，**无法实现存储介质的分离**。如果目标是降低存储成本，分区表无法替代冷热分离。

## [典型业务场景](https://javaguide.cn/high-performance/data-cold-hot-separation.html#典型业务场景)

> **说明**：以下存储策略仅供参考，实际选型需结合数据量、查询需求、团队技术栈和成本预算综合考虑。

### [订单系统](https://javaguide.cn/high-performance/data-cold-hot-separation.html#订单系统)

| 阶段     | 数据范围                | 存储策略                        | 说明                         |
| -------- | ----------------------- | ------------------------------- | ---------------------------- |
| 热数据   | 最近 90 天 + 未完成订单 | MySQL 热库（SSD）               | 高频访问，保障查询性能       |
| 冷数据   | 90 天~3 年              | MySQL 冷库（HDD）或 TiDB        | 可能需要查询，保持关系型存储 |
| 归档数据 | 超过 3 年               | 对象存储 / HBase / 仅保留汇总表 | 极少查询，优先考虑成本       |

### [日志系统](https://javaguide.cn/high-performance/data-cold-hot-separation.html#日志系统)

| 阶段   | 数据范围  | 存储策略                                               | 说明                                      |
| ------ | --------- | ------------------------------------------------------ | ----------------------------------------- |
| 热数据 | 近 7 天   | Elasticsearch 热节点                                   | 实时检索、高频查询                        |
| 温数据 | 7~30 天   | Elasticsearch 温节点                                   | 偶发查询，降低存储成本                    |
| 冷数据 | 30 天以上 | Elasticsearch 冷节点 / 压缩归档至对象存储 / ClickHouse | 根据查询需求选择，ClickHouse 适合分析场景 |

### [内容系统](https://javaguide.cn/high-performance/data-cold-hot-separation.html#内容系统)

| 阶段   | 数据范围                   | 存储策略                      | 说明                           |
| ------ | -------------------------- | ----------------------------- | ------------------------------ |
| 热数据 | 发布后 3 个月内 + 高阅读量 | MySQL 热库                    | 频繁被访问                     |
| 冷数据 | 3 个月后 + 低阅读量        | MySQL 冷库 / HBase / 对象存储 | 访问频率低，可迁移至低成本存储 |

**选型建议**：

- **需要支持事务或复杂查询**：优先选择 MySQL 冷库或 TiDB
- **需要大规模聚合分析**：优先选择 ClickHouse 或 Doris
- **仅需偶尔查询明细**：可选择对象存储（如 OSS/S3），查询时临时加载
- **数据量极大且访问极低**：HBase 或对象存储是性价比最高的选择

## [案例分享](https://javaguide.cn/high-performance/data-cold-hot-separation.html#案例分享)

- [如何快速优化几千万数据量的订单表 - 程序员济癫 - 2023](https://www.cnblogs.com/fulongyuanjushi/p/17910420.html)
- [海量数据冷热分离方案与实践 - 字节跳动技术团队 - 2022](https://mp.weixin.qq.com/s/ZKRkZP6rLHuTE1wvnqmAPQ)

# [优化]深度分页问题

大家好，我是小林。

最近有同学面试字节三面的时候，被问到：“**MySQL 分页有什么性能问题？怎么优化？**”

今天就主要围绕这个问题跟大家说一下 MySQL 分页问题和优化的思路。

> 原文地址：[字节三面：MySQL 分页怎么优化啊？(opens new window)](https://mp.weixin.qq.com/s/i6FL1iRECiWZ1CCf_juxQQ)

我们刷网站的时候，我们经常会遇到需要分页查询的场景。

比如下图红框里的翻页功能。

![图片](https://cdn.xiaolincoding.com//picgo/361faa6c1f37457a96f6a47131152c02.png)

我们很容易能联想到可以用 mySQL 实现。

假设我们的建表 SQL 是这样的

```sql
CREATE TABLE `page` (
  `id`        int          NOT NULL AUTO_INCREMENT COMMENT '自增主键',
  `user_name` varchar(255) NOT NULL                COMMENT '用户名',
   ...
  `content`   varchar(255) NOT NULL                COMMENT '文章内容',
  PRIMARY KEY (`id`),
  KEY `idx_user_name` (`user_name`)
) ENGINE=InnoDB AUTO_INCREMENT=0 DEFAULT CHARSET=utf8mb3;
```

* 建表 SQL 大家也不用扣细节，只需要知道 **id是主键，并且在 user_name 建了个非主键索引**就够了，其他都不重要。

<font color=24b3aa>为了实现分页，很容易联想到下面这样的 SQL 语句。</font>

```sql
SELECT * FROM page ORDER BY id LIMIT offset, size;
```

比如一页有 10 条数据。

![图片](https://cdn.xiaolincoding.com//picgo/07b3951ed10755e3f62f1a143dd55d3f.png)

第一页就是下面这样的 SQL 语句。

```sql
SELECT * FROM page ORDER BY id limit 0, 10;
```

第一百页就是

```sql
SELECT * FROM page ORDER BY id limit 990, 10;
```

那么问题来了。

用这种方式，**<font color=red>同样都是拿 10 条数据，查第一页和第一百页的查询速度是一样的吗？为什么？</font>**

## [#](https://www.xiaolincoding.com/mySQL/index/limit.html#两种limit的执行过程)两种 limit 的执行过程

上面的两种查询方式。对应 `LIMIT OFFSET, size` 和 `limit size` 两种方式。

而其实 `limit size`，相当于 `limit 0, size`。也就是从 0 开始取 size 条数据。

也就是说，两种方式的 **区别在于 offset 是否为 0。**

我们先来看下 limit SQL 的内部执行逻辑。

![图片](https://cdn.xiaolincoding.com//picgo/c90640f7bb3a070bb4195dfee338d84e.png)

mySQL 内部分为 **server层** 和 **存储引擎层**。一般情况下存储引擎都用 innodb。

server 层有很多模块，其中需要关注的是 **执行器** 是用于跟存储引擎打交道的组件。

<font color=24b3aa>执行器可以通过调用存储引擎提供的接口，将一行行数据取出，当这些数据完全符合要求（比如满足其他 WHERE 条件），则会放到 **结果集** 中，最后返回给调用 mySQL 的**客户端（go、java 写的应用程序）**。</font>

我们可以对下面的 SQL 先执行下 `explain`。

```sql
EXPLAIN SELECT * FROM page ORDER BY id limit 0, 10;
```

可以看到，explain 中提示 key 那里，执行的是 **PRIMARY**，也就是走的 **主键索引**。

![img](https://cdn.xiaolincoding.com//picgo/3053b90d0d6ac66e32309af0d0fcf8e7.png)

主键索引本质是一棵 B+ 树，它是放在innodb中的一个数据结构。

我们可以回忆下，B+树大概长这样。

![img](https://cdn.xiaolincoding.com//picgo/6a7feb26fedf7326fba4384efd8aca18.png)

在这个树状结构里，我们需要关注的是，最下面一层节点，也就是 **叶子结点**。而这个叶子结点里放的信息会根据当前的索引是 **主键还是非主键** 有所不同。

- 如果是 **主键索引**，它的叶子节点会存放完整的行数据信息。
- 如果是 **非主键索引**，那它的叶子节点则会存放主键，如果想获得行数据信息，则需要再跑到主键索引去拿一次数据，这叫 **回表**。

比如执行

```sql
SELECT * FROM page WHERE user_name = "小白10";
```

会通过非主键索引去查询 **user_name** 为 "**小白10**" 的数据，然后在叶子结点里找到 "**小白10**" 的数据对应的 **主键为 10**。

此时回表到 **主键索引** 中做查询，最后定位到 **主键为 10 的行数据**。

![img](https://cdn.xiaolincoding.com//picgo/2b24b0c5a5fcf1513df1e63676f6671b.png)

但不管是主键还是非主键索引，他们的叶子结点数据都是 **有序的**。比如在主键索引中，这些数据是根据主键 id 的大小，从小到大，进行排序的。

### [#](https://www.xiaolincoding.com/mySQL/index/limit.html#基于主键索引的limit执行过程)基于主键索引的 limit 执行过程

那么回到文章开头的问题里。

当我们去掉 explain，执行这条 SQL。

```sql
SELECT * FROM page ORDER BY id limit 0, 10;
```

上面 SELECT 后面带的是 `*`，也就是要求获得行数据的 **所有字段信息。**

server 层会调用 innodb 的接口，在 innodb 里的主键索引中获取到第  0 到 10 条 **完整行数据**，依次返回给 server 层，并放到 server 层的结果集中，返回给客户端。

而当我们把 offset 搞离谱点，比如执行的是

```sql
SELECT * FROM page ORDER BY id limit 6000000, 10;
```

<font color=24b3aa>server 层会调用 innodb 的接口，由于这次的 offset=6000000，会在 innodb 里的主键索引中获取到第 0到（6000000 + 10）条**完整行数据**，**返回给 server 层之后根据 offset 的值挨个抛弃，最后只留下最后面的 size 条**，也就是 10 条数据，放到 server 层的结果集中，返回给客户端。</font>

可以看出，当 offset 非 0 时，server 层会从引擎层获取到 **很多无用的数据**，而获取的这些无用数据都是要耗时的。

因此，我们就知道了文章开头的问题的答案，**<font color=blue>mySQL 查询中 `limit 1000,10` 会比 `limit 10` 更慢。原因是 `limit 1000,10` 会取出 1000+10 条数据，并抛弃前 1000 条，这部分耗时更大</font>**。

**那这种 case 有办法优化吗？**

<font color=ef11>可以看出，当 offset 非 0 时，server 层会从引擎层获取到很多无用的数据，而当 SELECT 后面是 `*` 时，就需要拷贝完整的行信息，**拷贝完整数据** 跟 **只拷贝行数据里的其中一两个列字段** 耗时是不同的，这就让原本就耗时的操作变得更加离谱。</font>

<font color=ef11>因此，一个容易想到的优化方案就是先通过主键定位到第 1000 条数据，然后从第 1000 条数据开始读完整数据。</font>

由于前面的 offset 条数据最后都是不要的，就算将完整字段都拷贝来了又有什么用呢，所以我们可以将 SQL 语句修改成下面这样。

```SQL
SELECT * FROM page  
WHERE id >=( # 从第 6000000 条数据开始玩真读，前 6000000 条数据只读 id
    SELECT id FROM page  
    ORDER BY id 
    limit 6000000, 1) 
ORDER BY id LIMIT 10;
```

上面这条 SQL 语句，里面先执行子查询 `SELECT id FROM page ORDER BY id limit 6000000, 1`, 这个操作，其实也是将在 innodb 中的主键索引中获取到 `6000000+1` 条数据，然后 server 层会抛弃前 6000000条，只保留最后一条数据的 id。

但不同的地方在于，在返回 server 层的过程中，只会拷贝数据行内的 id 这一列，而不会拷贝数据行的所有列，当数据量较大时，这部分的耗时还是比较明显的。

在拿到了上面的 id 之后，假设这个 id 正好等于 6000000，那 SQL 就变成了

```sql
SELECT * FROM page  WHERE id >=(6000000) ORDER BY id LIMIT 10;
```

这样 innodb 再走一次 **主键索引**，通过 B+ 树快速定位到 id=6000000 的行数据，时间复杂度是 lg(n)，然后向后取 10 条数据。

这样性能确实是提升了，亲测能快一倍左右，属于那种耗时从 3s 变成 1.5s 的操作。

不过，这属实有些杯水车薪，有点搓，属于没办法中的办法。

### [#](https://www.xiaolincoding.com/mySQL/index/limit.html#基于非主键索引的limit执行过程)基于非主键索引的 limit 执行过程

非主键索引相较于主键索引，在 `SELECT *` 时的区别在于，非主键索引在检索到主键后还需要回表到聚簇（主键）索引处取得完整的行数据。

* 当 offset 较小时，执行器会丢弃掉前面 offset 条数据，然后对剩下的数据一一回表。
* 当 offset 较大时，回表次数可能非常多，此时回表就成了很大的负担，这个负担甚至比直接全盘扫描的代价还要大，因此，这里优化器会将“二级索引+回表”优化为“全盘扫描”。

```sql
SELECT * FROM page
WHERE age >= 18
ORDER BY user_name
LIMIT 6000000, 10;
```

假设只有单列索引 `idx_user_name(user_name)`，并且 `age` 不在这个索引里（也没有合适的复合索引）。

> 可能的执行路径：走 `user_name` 索引来满足 ORDER BY

引擎按 `user_name` 索引顺序扫描，每拿到一条（user_name, id）：

- **回表** 取整行（或至少取 age）判断 `age>=18`
- 若满足，则计入“符合条件的结果流”
- 需要先跳过符合条件的 **6000000** 条，再收集 **10** 条返回

在这种路径下：

- 你为了“计数到 6000000+10 条符合条件的记录”，需要检查很多行的 `age`
- 而检查 `age` 又必须回表
- 因此 **回表次数 ≈ 为了找到 (OFFSET+SIZE) 条合格记录所需要检查的行数**

如果“合格率”接近 100%（几乎所有人都 `age>=18`），那检查的行数就接近 `OFFSET+SIZE`。

但如果合格率只有 50%，为了凑够 6000010 条合格记录，可能要检查约 1200 万行；回表次数会比 OFFSET+SIZE **更大**。

## [#](https://www.xiaolincoding.com/mySQL/index/limit.html#深度分页问题)深度分页问题

### #什么是深度分页问题

<font color=ee11ff>**深度分页问题：**</font>当使用 `LIMIT offset, count` 查询时，**offset 值非常大**（如几十万、几百万甚至更大），导致数据库为了返回少量数据而必须扫描和处理海量无关行的性能问题。

深度分页问题，是个很恶心的问题，恶心就恶心在，这个问题，它其实 **无解**。不管你是用 mySQL 还是 es，你都只能通过一些手段去"减缓"问题的严重性。

<font color=blue>遇到这个问题，我们就该回过头来想想。为什么我们的代码会产生深度分页问题？**它背后的原始需求是什么**，我们可以根据这个做一些规避。</font>

以 MySQL 为例：

```sql
SELECT *
FROM page
ORDER BY user_name
LIMIT 6000000, 10;
```

虽然你只想要 10 行，但数据库为了拿到“第 6000001～6000010 行”，必须先按排序规则找到并 **跳过前 600 万行**，再取 10 行。这个“跳过”不是免费的：

- 需要扫描/读取大量索引或数据页（I/O、CPU 都会涨）
- server 层要一行行计数并丢弃 offset 之前的结果
- 如果还涉及 `SELECT *` 且排序走二级索引，或者有过滤条件不在索引里，可能还会触发大量回表/大量行验证，成本更高
- offset 越大，耗时几乎线性变差，越往后页越慢

<font color=red>所以深度分页的本质是：**你请求的是很后面的一小段数据，但数据库不得不“从头数到那里”。**</font>

常见特征：

- 前几页很快，越往后越慢
- 同样的 `size=10`，但 offset 从 0 变成 6,000,000，耗时可能从毫秒级变成秒级甚至更久
- 还可能让优化器觉得“用索引也不划算”，转而选择全表扫描（更慢/更不稳定）

### #常见解决思路

深度分页的核心矛盾是 **`OFFSET` 必须线性扫描并丢弃前 N 行**。所有解决方案都围绕一个思路：**避免或减少"扫描+丢弃"的开销**。

**<font color=blue>1. 游标分页（Keyset Pagination）⭐ 最优解</font>**

**原理：**不用 `OFFSET`，改用上一页最后一条记录的排序键值作为起点，利用索引直接定位。

```sql
-- ❌ 深分页
SELECT * FROM page WHERE age >= 18 
ORDER BY user_name, id LIMIT 6000000, 10;

-- ✅ 游标分页
SELECT * FROM page 
WHERE age >= 18 AND (user_name, id) > ('Alice', 12345)
ORDER BY user_name, id 
LIMIT 10;
```

**要求**

- 排序键组合**唯一**（通常追加主键 `id` 保证唯一性）
- 需要 `(user_name, id)` 联合索引

**优缺点**

| ✅                               | ❌                             |
| :------------------------------ | :---------------------------- |
| 恒定 O(1) 性能，与页码无关      | 不支持跳页（只能上/下一页）   |
| 无重复/遗漏（不受并发插入影响） | 前端需维护 cursor 状态        |
| 天然走索引范围扫描              | 多列排序时 WHERE 条件写法复杂 |

**适用场景：**无限滚动、移动端列表、API 数据消费、实时流式查询

<font color=blue>**2. 延迟关联（Deferred Join）**</font>

**原理：**将回表操作推迟到定位完目标 ID 之后，子查询只在**覆盖索引**上完成扫描和排序。

```sql
SELECT p.* FROM page p
INNER JOIN (
    -- 子查询只查 id，走覆盖索引，不回表
    SELECT id FROM page 
    WHERE age >= 18 
    ORDER BY user_name 
    LIMIT 6000000, 10
) t ON p.id = t.id
ORDER BY p.user_name;
```

* 原始查询: 扫描600万行 → 每行回表读完整数据 → 丢弃 → 仅10条有用
* 延迟关联: 扫描600万行索引条目(窄) → 仅对10个ID回表 → 大幅减少IO

**优缺点**

| ✅                        | ❌                                      |
| :----------------------- | :------------------------------------- |
| 支持跳页                 | 仍需扫描 offset 行索引条目（只是更窄） |
| 改动小，兼容现有分页接口 | offset 极大时仍然慢（只是比原来好）    |
| 不需要改前端逻辑         | 需要排序字段有索引                     |

**适用场景：**后台管理系统、需要跳页但数据量可控（百万级以内）

<font color=blue>**4. ID 范围分批（Range-based Batch）**</font>

**原理：**用主键区间代替 OFFSET，每次从上次的最大 ID 继续。

```sql
-- 第1批
SELECT * FROM page WHERE age >= 18 AND id > 0 ORDER BY id LIMIT 1000;
-- 第2批（假设上批最大id=5000）
SELECT * FROM page WHERE age >= 18 AND id > 5000 ORDER BY id LIMIT 1000;
-- 第N批...
```

**优缺点**

| ✅                | ❌                      |
| :--------------- | :--------------------- |
| 每批性能恒定     | 只能按主键顺序遍历     |
| 无 OFFSET 开销   | 不适合用户交互分页     |
| 天然支持断点续传 | 非主键排序字段无法使用 |

**适用场景：**数据导出、ETL、全量同步、定时任务分批处理

<font color=blue>**4. 限制最大页码 / 搜索替代**</font>

**策略**

- **硬性限制**：禁止翻超过 1000 页，提示用户缩小筛选范围
- **搜索引擎**：用 Elasticsearch `search_after` 替代 MySQL 深分页
- **缓存热门页**：前 N 页结果缓存，减少重复深查

**适用场景：**面向 C 端用户的搜索/列表页（Google 也只显示前几十页）

### [#](https://www.xiaolincoding.com/mySQL/index/limit.html#如果你是想取出全表的数据)如果你是想取出全表的数据

有些需求是这样的，我们有一张数据库表，但我们希望将这个数据库表里的所有数据取出，异构到 es，或者 hive 里，这时候如果直接执行

```sql
SELECT * FROM page;
```

这个 SQL 一执行，狗看了都摇头。

因为数据量较大，mySQL 根本没办法一次性获取到全部数据，妥妥 **超时报错**。

于是不少 mySQL 小白会通过 `LIMIT OFFSET size` 分页的形式去分批获取，刚开始都是好的，等慢慢地，哪天数据表变得奇大无比，就有可能出现前面提到的 **深度分页** 问题。

这种场景是最好解决的。

我们可以将所有的数据 **根据 id 主键进行排序**，然后分批次取，将当前批次的最大 id 作为下次筛选的条件进行查询。

可以看下伪代码

![img](https://cdn.xiaolincoding.com//picgo/318663c9592dc46cf81e3e784e0ebc52.png)

这个操作，可以通过主键索引，每次定位到 id 在哪，然后往后遍历 100 个数据，这样不管是多少万的数据，查询性能都很稳定。

![img](https://cdn.xiaolincoding.com//picgo/db2daf80158d5dad21238770853af399.png)

### [#](https://www.xiaolincoding.com/mySQL/index/limit.html#如果是给用户做分页展示)如果是给用户做分页展示

如果深度分页背后的原始需求只是产品经理希望做一个展示页的功能，比如商品展示页，那么我们就应该好好跟产品经理 battle 一下了。

<font color=ef11>什么样的翻页，需要翻到 10 多万以后，这明显是不合理的需求。</font>

<font color=ef11>是不是可以改一下需求，让它更接近用户的使用行为？</font>

<font color=ef11>比如，我们在使用谷歌搜索时看到的翻页功能。</font>

![img](https://cdn.xiaolincoding.com//picgo/31e10317db937beccdedcf79a7d03f47.png)

一般来说，谷歌搜索基本上都在 20 页以内，作为一个用户，我就很少会翻到第 10 页之后。

作为参考。

如果我们要做搜索或筛选类的页面的话，就别用 mySQL了，用 es，并且也需要控制展示的结果数，比如一万以内，这样不至于让分页过深。

如果因为各种原因，必须使用 mySQL。那同样，也需要控制下返回结果数量，比如数量 1k 以内。

这样就能勉强支持各种翻页，跳页（比如突然跳到第 6 页然后再跳到第 106 页）。

<font color=ef11>但如果能从产品的形式上就做成不支持跳页会更好，比如 **只支持上一页或下一页**。</font>

![img](https://cdn.xiaolincoding.com//picgo/7cc01cdc245a2777d4c766649ab439ae.png)

**这样我们就可以使用上面提到的 start_id 方式，采用分批获取，每批数据以 start_id 为起始位置。这个解法最大的好处是不管翻到多少页，查询速度永远稳定。**

听起来很挫？

怎么会呢，把这个功能包装一下。

变成像抖音那样只能上划或下划，专业点，叫 **<font color=blue>瀑布流</font>**。

是不是就不挫了？

![img](https://cdn.xiaolincoding.com//picgo/3e1d7768f0fddd4efd99467bc1250d90.png)图片

### [#](https://www.xiaolincoding.com/mySQL/index/limit.html#总结)总结

- `LIMIT OFFSET, size` 比 `limit size` 要慢，且 offset 的值越大，SQL 的执行速度越慢。
- 当 offset 过大，会引发 **深度分页** 问题，目前不管是 mySQL 还是 es 都没有很好的方法去解决这个问题。只能通过限制查询数量或分批获取的方式进行规避。
- 遇到深度分页的问题，多思考其原始需求，大部分时候是不应该出现深度分页的场景的，必要时多去影响产品经理。
- 如果数据量很少，比如 1k 的量级，且长期不太可能有巨大的增长，还是用 `LIMIT OFFSET, size` 的方案吧，整挺好，能用就行。

# [[优化]Show Profile 分析 SQL 执行性能](https://javaguide.cn/high-performance/SQL-optimization.html#show-profile-分析-SQL-执行性能)

为了更精准定位一条 SQL 语句的性能问题，需要清楚地知道这条 SQL 语句运行时消耗了多少系统资源。 [`SHOW PROFILE`](https://dev.mySQL.com/doc/refman/5.7/en/show-profile.html) 和 [`SHOW PROFILES`](https://dev.mySQL.com/doc/refman/5.7/en/show-profiles.html) 展示 SQL 语句的资源使用情况，展示的消息包括 CPU 的使用，CPU 上下文切换，IO 等待，内存使用等。

MySQL 在 5.0.37 版本之后才支持 Profiling，`select @@have_profiling` 命令返回 `YES` 表示该功能可以使用。

```sql
 mySQL> SELECT @@have_profiling;
+------------------+
| @@have_profiling |
+------------------+
| YES              |
+------------------+
1 row in set (0.00 sec)
```

> **注意** ：`SHOW PROFILE` 和 `SHOW PROFILES` 已经被弃用，未来的 MySQL 版本中可能会被删除，取而代之的是使用 [Performance Schema](https://dev.mySQL.com/doc/refman/8.0/en/performance-schema.html)。在该功能被删除之前，我们简单介绍一下其基本使用方法。
>
> **推荐替代方案**：MySQL 5.7+ 推荐使用 Performance Schema 的 `events_statements_history_long` 表：
>
> ```sql
> -- 查询最近执行的 SQL 及其耗时
> SELECT
> EVENT_ID,
> SQL_TEXT,
> TIMER_WAIT/1000000000 AS 'Duration (ms)',
> CPU_USER
> FROM performance_schema.events_statements_history_long
> ORDER BY TIMER_WAIT DESC
> LIMIT 10;
> ```
>
> 此外，MySQL 8.0.18+ 还支持 `EXPLAIN ANALYZE`，可以直接输出 SQL 的实际执行时间和行数统计。

想要使用 Profiling，请确保你的 `profiling` 是开启（on）的状态。

你可以通过 `SHOW VARIABLES` 命令查看其状态：

![img](https://oss.javaguide.cn/github/javaguide/mySQL/mySQL-show-variables-profiling.png)

也可以通过 `SELECT @@profiling`命令进行查看：

```sql
mySQL> SELECT @@profiling;
+-------------+
| @@profiling |
+-------------+
|           0 |
+-------------+
1 row in set (0.00 sec)
```

默认情况下， `Profiling` 是关闭（off）的状态，你直接通过`SET @@profiling=1`命令即可开启。

开启成功之后，我们执行几条 SQL 语句。执行完成之后，使用 `SHOW PROFILES` 可以展示当前 Session 下所有 SQL 语句的简要的信息包括 Query_ID（SQL 语句的 ID 编号） 和 Duration（耗时）。

具体能收集多少个 SQL，由参数 `profiling_history_size` 决定，默认值为 15，最大值为 100。如果设置为 0，等同于关闭 Profiling。

![img](https://oss.javaguide.cn/github/javaguide/mySQL/mySQL-show-profiles-ranking-list-table.png)

如果想要展示一个 SQL 语句的执行耗时细节，可以使用`SHOW PROFILE` 命令。

`SHOW PROFILE` 命令的具体用法如下：

```sql
SHOW PROFILE [type [, type] ... ]
    [FOR QUERY n]
    [LIMIT row_count [OFFSET offset]]

type: {
    ALL
  | BLOCK IO
  | CONTEXT SWITCHES
  | CPU
  | IPC
  | MEMORY
  | PAGE FAULTS
  | SOURCE
  | SWAPS
}
```

在执行`SHOW PROFILE` 命令时，可以加上类型子句，比如 CPU、IPC、MEMORY 等，查看具体某类资源的消耗情况：

```sql
SHOW PROFILE CPU,IPC FOR QUERY 8;
```

如果不加 `FOR QUERY {n}`子句，默认展示最新的一次 SQL 的执行情况，加了 `FOR QUERY {n}`，表示展示 Query_ID 为 n 的 SQL 的执行情况。

![img](https://oss.javaguide.cn/github/javaguide/mySQL/mySQL-show-profiles-cpu-ipc.png)

# [优化]慢 SQL

为了优化慢 SQL ，我们首先要找到哪些 SQL 语句执行速度比较慢。

<FONT COLOR=BLUE>MySQL 慢查询日志</FONT>是用来记录 MySQL 在执行命令中，响应时间超过预设阈值的 SQL 语句。因此，通过分析慢查询日志我们就可以找出执行速度比较慢的 SQL 语句。

<FONT COLOR=24B3AA>出于性能层面的考虑，慢查询日志功能默认是关闭的，你可以通过以下命令开启：</FONT>

```sql
# 开启慢查询日志功能
SET GLOBAL slow_query_log = 'ON';
# 慢查询日志存放位置
SET GLOBAL slow_query_log_file = '/var/lib/mySQL/ranking-list-slow.log';
# 无论是否超时，未被索引的记录也会记录下来。
SET GLOBAL log_queries_not_using_indexes = 'ON';
# 慢查询阈值（秒），SQL 执行超过这个阈值将被记录在日志中。
SET SESSION long_query_time = 1;
# 慢查询仅记录扫描行数大于此参数的 SQL
SET SESSION min_examined_row_limit = 100;
```

设置成功之后，使用 `show variables like 'slow%';` 命令进行查看。

```bash
| Variable_name       | Value                                |
+---------------------+--------------------------------------+
| slow_launch_time    | 2                                    |
| slow_query_log      | ON                                   |
| slow_query_log_file | /var/lib/mySQL/ranking-list-slow.log |
+---------------------+--------------------------------------+
3 rows in set (0.01 sec)
```

我们故意在百万数据量的表(未使用索引)中执行一条排序的语句：

```sql
SELECT `score`,`name` FROM `cus_order` ORDER BY `score` DESC;
```

确保自己有对应目录的访问权限：

```bash
chmod 755 /var/lib/mySQL/
```

查看对应的慢查询日志：

```bash
 cat /var/lib/mySQL/ranking-list-slow.log
```

我们刚刚故意执行的 SQL 语句已经被慢查询日志记录了下来：

```plain
# Time: 2022-10-09T08:55:37.486797Z
# User@Host: root[root] @  [172.17.0.1]  Id:    14
# Query_time: 0.978054  Lock_time: 0.000164 Rows_sent: 999999  Rows_examined: 1999998
SET timestamp=1665305736;
SELECT `score`,`name` FROM `cus_order` ORDER BY `score` DESC;
```

这里对日志中的一些信息进行说明：

- `Time` ：被日志记录的代码在服务器上的运行时间。
- `User@Host`：谁执行的这段代码。
- `Query_time`：这段代码运行时长。
- `Lock_time`：执行这段代码时，锁定了多久。
- `Rows_sent`：慢查询返回的记录。
- `Rows_examined`：慢查询扫描过的行数。

实际项目中，慢查询日志通常会比较复杂，我们需要借助一些工具对其进行分析。像 MySQL 内置的 `mySQLdumpslow` 工具就可以把相同的 SQL 归为一类，并统计出归类项的执行次数和每次执行的耗时等一系列对应的情况。

<FONT COLOR=BLUE>找到了慢 SQL 之后，我们可以通过 `EXPLAIN` 命令分析对应的 `SELECT` 语句：</FONT>

```sql
mySQL> EXPLAIN SELECT `score`,`name` FROM `cus_order` ORDER BY `score` DESC;
+----+-------------+-----------+------------+------+---------------+------+---------+------+--------+----------+----------------+
| id | select_type | table     | partitions | type | possible_keys | key  | key_len | ref  | rows   | filtered | Extra          |
+----+-------------+-----------+------------+------+---------------+------+---------+------+--------+----------+----------------+
|  1 | SIMPLE      | cus_order | NULL       | ALL  | NULL          | NULL | NULL    | NULL | 997572 |   100.00 | Using filesort |
+----+-------------+-----------+------------+------+---------------+------+---------+------+--------+----------+----------------+
1 row in set, 1 warning (0.00 sec)
```

比较重要的字段说明：

- `select_type` ：查询的类型，常用的取值有 SIMPLE（普通查询，即没有联合查询、子查询）、PRIMARY（主查询）、UNION（UNION 中后面的查询）、SUBQUERY（子查询）等。
- `table` ：表示查询涉及的表或衍生表。
- `type` ：执行方式，判断查询是否高效的重要参考指标，结果值从差到好依次是：**ALL**（全表扫描）< **index**（索引全扫描）< **range**（索引范围扫描）< **index_merge**（索引合并）< **ref**（非唯一索引查找）< **eq_ref**（唯一索引查找）< **const**（单行常量）< **system**（系统表）。实际性能还需结合 rows、Extra 等字段综合判断。
- `rows` : SQL 要查找到结果集需要扫描读取的数据行数，原则上 rows 越少越好。
- ……

> **推荐阅读**：[MySQL 执行计划分析](https://javaguide.cn/database/mySQL/mySQL-query-execution-plan.html) 详细介绍了 EXPLAIN 各列的含义（id、select_type、type、key、rows、Extra 等），包括 MySQL 8.0.18+ 新增的 `EXPLAIN ANALYZE` 实际执行分析功能。另外，阿里的 [慢 SQL 治理经验总结](https://mp.weixin.qq.com/s/LZRSQJufGRpRw6u4h_Uyww) 也总结得不错。

# [NoSQL]简介

## [NoSQL 是什么？](https://javaguide.cn/database/nosql.html#nosql-是什么)

NoSQL（Not Only SQL 的缩写）泛指非关系型的数据库，主要针对的是键值、文档以及图形类型数据存储。并且，NoSQL 数据库天生支持分布式，数据冗余和数据分片等特性，旨在提供可扩展的高可用高性能数据存储解决方案。

一个常见的误解是 NoSQL 数据库或非关系型数据库不能很好地存储关系型数据。NoSQL 数据库可以存储关系型数据—它们与关系型数据库的存储方式不同。

NoSQL 数据库代表：HBase、Cassandra、MongoDB、Redis。

![img](https://oss.javaguide.cn/github/javaguide/database/mongodb/sql-nosql-tushi.png)

## [SQL 和 NoSQL 有什么区别？](https://javaguide.cn/database/nosql.html#sql-和-nosql-有什么区别)

|              | SQL 数据库                                                   | NoSQL 数据库                                                 |
| :----------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 数据存储模型 | 结构化存储，具有固定行和列的表格                             | 非结构化存储。文档：JSON 文档，键值：键值对，宽列：包含行和动态列的表，图：节点和边 |
| 发展历程     | 开发于 1970 年代，重点是减少数据重复                         | 开发于 2000 年代后期，重点是提升可扩展性，减少大规模数据的存储成本 |
| 例子         | Oracle、MySQL、Microsoft SQL Server、PostgreSQL              | 文档：MongoDB、CouchDB，键值：Redis、DynamoDB，宽列：Cassandra、 HBase，图表：Neo4j、 Amazon Neptune、Giraph |
| ACID 属性    | 提供原子性、一致性、隔离性和持久性 (ACID) 属性               | 通常不支持 ACID 事务，为了可扩展、高性能进行了权衡，少部分支持比如 MongoDB 。不过，MongoDB 对 ACID 事务 的支持和 MySQL 还是有所区别的。 |
| 性能         | 性能通常取决于磁盘子系统。要获得最佳性能，通常需要优化查询、索引和表结构。 | 性能通常由底层硬件集群大小、网络延迟以及调用应用程序来决定。 |
| 扩展         | 垂直（使用性能更强大的服务器进行扩展）、读写分离、分库分表   | 横向（增加服务器的方式横向扩展，通常是基于分片机制）         |
| 用途         | 普通企业级的项目的数据存储                                   | 用途广泛比如图数据库支持分析和遍历连接数据之间的关系、键值数据库可以处理大量数据扩展和极高的状态变化 |
| 查询语法     | 结构化查询语言 (SQL)                                         | 数据访问语法可能因数据库而异                                 |

## [NoSQL 数据库有什么优势？](https://javaguide.cn/database/nosql.html#nosql-数据库有什么优势)

NoSQL 数据库非常适合许多现代应用程序，例如移动、Web 和游戏等应用程序，它们需要灵活、可扩展、高性能和功能强大的数据库以提供卓越的用户体验。

- **灵活性：** NoSQL 数据库通常提供灵活的架构，以实现更快速、更多的迭代开发。灵活的数据模型使 NoSQL 数据库成为半结构化和非结构化数据的理想之选。
- **可扩展性：** NoSQL 数据库通常被设计为通过使用分布式硬件集群来横向扩展，而不是通过添加昂贵和强大的服务器来纵向扩展。
- **高性能：** NoSQL 数据库针对特定的数据模型和访问模式进行了优化，这与尝试使用关系数据库完成类似功能相比可实现更高的性能。
- **强大的功能：** NoSQL 数据库提供功能强大的 API 和数据类型，专门针对其各自的数据模型而构建。

## [NoSQL 数据库有哪些类型？](https://javaguide.cn/database/nosql.html#nosql-数据库有哪些类型)

NoSQL 数据库主要可以分为下面四种类型：

- **键值**：键值数据库是一种较简单的数据库，其中每个项目都包含键和值。这是极为灵活的 NoSQL 数据库类型，因为应用可以完全控制 value 字段中存储的内容，没有任何限制。Redis 和 DynanoDB 是两款非常流行的键值数据库。
- **文档**：文档数据库中的数据被存储在类似于 JSON（JavaScript 对象表示法）对象的文档中，非常清晰直观。每个文档包含成对的字段和值。这些值通常可以是各种类型，包括字符串、数字、布尔值、数组或对象等，并且它们的结构通常与开发者在代码中使用的对象保持一致。MongoDB 就是一款非常流行的文档数据库。
- **图形**：图形数据库旨在轻松构建和运行与高度连接的数据集一起使用的应用程序。图形数据库的典型使用案例包括社交网络、推荐引擎、欺诈检测和知识图形。Neo4j 和 Giraph 是两款非常流行的图形数据库。
- **宽列**：宽列存储数据库非常适合需要存储大量的数据。Cassandra 和 HBase 是两款非常流行的宽列存储数据库。

下面这张图片来源于 [微软的官方文档 | 关系数据与 NoSQL 数据](https://learn.microsoft.com/en-us/dotnet/architecture/cloud-native/relational-vs-nosql-data)。

![NoSQL 数据模型](https://oss.javaguide.cn/github/javaguide/database/mongodb/types-of-nosql-datastores.png)

## [参考](https://javaguide.cn/database/nosql.html#参考)

- NoSQL 是什么？- MongoDB 官方文档：https://www.mongodb.com/zh-cn/nosql-explained
- 什么是 NoSQL? - AWS：https://aws.amazon.com/cn/nosql/
- NoSQL vs. SQL Databases - MongoDB 官方文档：https://www.mongodb.com/zh-cn/nosql-explained/nosql-vs-sql

NoSQL 一词最早出现于 1998 年，是 Carlo Strozzi 开发的一个轻量、开源、不提供 SQL 功能的关系数据库。

2009 年，Last.fm 的 Johan Oskarsson 发起了一次关于分布式开源数据库的讨论，来自 Rackspace 的 Eric Evans 再次提出了 NoSQL 的概念，这时的 NoSQL 主要指非关系型、分布式、不提供 ACID 的数据库设计模式。

2009 年在亚特兰大举行的"no:sql(east)"讨论会是一个里程碑，其口号是"select fun, profit from real_world where relational=false"。因此，对 NoSQL 最普遍的解释是"非关联型的"，强调 Key-Value Stores 和文档数据库的优点，而不是单纯的反对 RDBMS。

## 什么是 NoSQL

`NoSQL（Not Only SQL）`，意思是"**不仅仅是 SQL**"，指的是非关系型数据库，是对不同于传统的关系型数据库的数据库管理系统的统称。

NoSQL 用于超大规模数据的存储。这些类型的数据存储不需要固定的模式，无需多余操作就可以横向扩展。

## 为什么使用 NoSQL

随着互联网的飞速发展与普及，网民上网冲浪时所产生数据也逐日增多，从 GB 到 TB 到 PB。这些数据有很大一部分都是由关系型数据库管理系统（RDBMS）来进行处理的。

由于关系型数据库的范式约束、事务特性、磁盘 IO 等特点，若服务器使用关系型数据库，当有大量数据产生时，传统的关系型数据库已经无法满足快速查询与插入数据的需求。NoSQL 的出现解决了这一危机。它通过降低数据的安全性，减少对事务的支持，减少对复杂查询的支持，获取性能上的提升。但是，在某些特定场景下 NoSQL 仍然不是最佳人选，比如一些绝对要有事务与安全指标的场景。

NoSQL 是一项全新的数据库革命性运动，早期就有人提出，发展至 2009 年趋势越发高涨。NoSQL 的拥护者们提倡运用非关系型的数据存储，相对于铺天盖地的关系型数据库运用，这一概念无疑是一种全新的思维的注入。

## RDBMS vs. NoSQL

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/image.46aebklwn.webp)

## 常见的数据库管理系统

　　根据 [DB-Engines](https://db-engines.com/en/ranking)：https://db-engines.com/en/ranking 的排行，本文截取了排行榜 Top 50 如下图所示。DB-Engines 排名根据数据库管理系统的受欢迎程度对其进行排名。该排名每月更新一次。

![](https://github.com/QaQOwOQaQ/picx-images-hosting/raw/master/QQ_1777258459677.6wrc0sc57l.webp)

## NoSQL 数据库四大家族

### 1. KV 存储

**特点**：键值数据库就像传统语言中使用的哈希表。通过 Key 添加、查询或者删除数据。

**优点**：查询速度快。

**缺点**：数据无结构化，通常只被当作字符串或者二进制数据存储。

**应用场景**：内容缓存、用户信息比如会话、配置信息、购物车等，主要用于处理大量数据的高访问负载。

**NoSQL 代表**：Redis、Memcached...

### 2. Document-Oriented 存储

**特点**：文档数据库将数据以文档的形式储存，类似 JSON，是一系列数据项的集合。每个数据项都有一个名称与对应的值，值既可以是简单的数据类型，如字符串、数字和日期等；也可以是复杂的类型，如有序列表和关联对象。

**优点**：数据结构要求不严格，表结构可变，不需要像关系型数据库一样需要预先定义表结构。

**缺点**：查询性能不高，缺乏统一的查询语法。

**应用场景**：日志、 Web 应用等。

**NoSQL 代表**：MongoDB、CouchDB...

### 3. Wide Column Store/Column-Family 存储

**特点**：列存储数据库将数据储存在列族（Column Family）中，将多个列聚合成一个列族，键仍然存在，但是它们的特点是指向了多个列。举个例子，如果我们有一个 Person 类，我们通常会一起查询他们的姓名和年龄而不是薪资。这种情况下，姓名和年龄就会被放入一个列族中，而薪资则在另一个列族中。

**优点**：列存储查找速度快，可扩展性强，更容易进行分布式扩展，适用于分布式的文件系统，应对分布式存储的海量数据。

**缺点**：查询性能不高，缺乏统一的查询语法。

**应用场景**：日志、 分布式的文件系统(对象存储)、推荐画像、时空数据、消息/订单等。

**NoSQL 代表**：Cassandra、HBase...

### 4. Graph-Oriented 存储

**特点**：图形数据库允许我们将数据以图的方式储存。

**优点**：图形相关算法。比如最短路径寻址，N 度关系查找等。

**缺点**：很多时候需要对整个图做计算才能得出需要的信息，分布式的集群方案不好做，处理超级节点乏力，没有分片存储机制，国内社区不活跃。

**应用场景**：社交网络，推荐系统等。专注于构建关系图谱。

**NoSQL 代表**：Neo4j、Infinite Graph...

## Nosql 的优缺点

优点：

- 高可扩展性
- 没有标准化
- 分布式计算
- 有限的查询功能（到目前为止）
- 低成本

缺点：

- 最终一致是不直观的程序
- 架构的灵活性，半结构化数据
- 没有复杂的关系

## 总结

NoSQL 数据库在以下几种情况下比较适用：

- 数据模型比较简单
- 需要灵活性更强的 IT 系统
- 对数据库性能要求较高
- 不需要高度的数据一致性
- 对于给定的 Key，比较容易映射复杂值的环境

# [NoSQL]MongoDB

## [MongoDB 基础](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-基础)

### [MongoDB 是什么？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-是什么)

MongoDB 是一个基于 **分布式文件存储** 的开源 NoSQL 数据库系统，由 **C++** 编写的。MongoDB 提供了 **面向文档** 的存储方式，操作起来比较简单和容易，支持“**无模式**”的数据建模，可以存储比较复杂的数据类型，是一款非常流行的 **文档类型数据库** 。

在高负载的情况下，MongoDB 天然支持水平扩展和高可用，可以很方便地添加更多的节点/实例，以保证服务性能和可用性。在许多场景下，MongoDB 可以用于代替传统的关系型数据库或键/值存储方式，皆在为 Web 应用提供可扩展的高可用高性能数据存储解决方案。

### [MongoDB 的存储结构是什么？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-的存储结构是什么)

MongoDB 的存储结构区别于传统的关系型数据库，主要由如下三个单元组成：

- **文档（Document）**：MongoDB 中最基本的单元，由 BSON 键值对（key-value）组成，类似于关系型数据库中的行（Row）。
- **集合（Collection）**：一个集合可以包含多个文档，类似于关系型数据库中的表（Table）。
- **数据库（Database）**：一个数据库中可以包含多个集合，可以在 MongoDB 中创建多个数据库，类似于关系型数据库中的数据库（Database）。

也就是说，MongoDB 将数据记录存储为文档 （更具体来说是[BSON 文档](https://www.mongodb.com/docs/manual/core/document/#std-label-bson-document-format)），这些文档在集合中聚集在一起，数据库中存储一个或多个文档集合。

**SQL 与 MongoDB 常见术语对比**：

| SQL                      | MongoDB                         |
| ------------------------ | ------------------------------- |
| 表（Table）              | 集合（Collection）              |
| 行（Row）                | 文档（Document）                |
| 列（Col）                | 字段（Field）                   |
| 主键（Primary Key）      | 对象 ID（Objectid）             |
| 索引（Index）            | 索引（Index）                   |
| 嵌套表（Embedded Table） | 嵌入式文档（Embedded Document） |
| 数组（Array）            | 数组（Array）                   |

#### [文档](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#文档)

MongoDB 中的记录就是一个 BSON 文档，它是由键值对组成的数据结构，类似于 JSON 对象，是 MongoDB 中的基本数据单元。字段的值可能包括其他文档、数组和文档数组。

![MongoDB 文档](https://oss.javaguide.cn/github/javaguide/database/mongodb/crud-annotated-document..png)

文档的键是字符串。除了少数例外情况，键可以使用任意 UTF-8 字符。

- 键不能含有 `\0`(空字符）。这个字符用来表示键的结尾。
- `.` 和 `$` 有特别的意义，只有在特定环境下才能使用。
- 以下划线`_`开头的键是保留的(不是严格要求的)。

**BSON [bee·sahn]** 是 Binary [JSON](http://json.org/)的简称，是 JSON 文档的二进制表示，支持将文档和数组嵌入到其他文档和数组中，还包含允许表示不属于 JSON 规范的数据类型的扩展。有关 BSON 规范的内容，可以参考 [bsonspec.org](http://bsonspec.org/)，另见[BSON 类型](https://www.mongodb.com/docs/manual/reference/bson-types/)。

根据维基百科对 BJSON 的介绍，BJSON 的遍历速度优于 JSON，这也是 MongoDB 选择 BSON 的主要原因，但 BJSON 需要更多的存储空间。

> 与 JSON 相比，BSON 着眼于提高存储和扫描效率。BSON 文档中的大型元素以长度字段为前缀以便于扫描。在某些情况下，由于长度前缀和显式数组索引的存在，BSON 使用的空间会多于 JSON。

![BSON 官网首页](https://oss.javaguide.cn/github/javaguide/database/mongodb/bsonspec.org.png)

#### [集合](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#集合)

MongoDB 集合存在于数据库中，**没有固定的结构**，也就是 **无模式** 的，这意味着可以往集合插入不同格式和类型的数据。不过，通常情况下，插入集合中的数据都会有一定的关联性。

![MongoDB 集合](https://oss.javaguide.cn/github/javaguide/database/mongodb/crud-annotated-collection.png)

集合不需要事先创建，当第一个文档插入或者第一个索引创建时，如果该集合不存在，则会创建一个新的集合。

集合名可以是满足下列条件的任意 UTF-8 字符串：

- 集合名不能是空字符串`""`。
- 集合名不能含有 `\0` （空字符)，这个字符表示集合名的结尾。
- 集合名不能以"system."开头，这是为系统集合保留的前缀。例如 `system.users` 这个集合保存着数据库的用户信息，`system.namespaces` 集合保存着所有数据库集合的信息。
- 集合名必须以下划线或者字母符号开始，并且不能包含 `$`。

#### [数据库](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#数据库)

数据库用于存储所有集合，而集合又用于存储所有文档。一个 MongoDB 中可以创建多个数据库，每一个数据库都有自己的集合和权限。

MongoDB 预留了几个特殊的数据库。

- **admin** : admin 数据库主要是保存 root 用户和角色。例如，system.users 表存储用户，system.roles 表存储角色。一般不建议用户直接操作这个数据库。将一个用户添加到这个数据库，且使它拥有 admin 库上的名为 dbAdminAnyDatabase 的角色权限，这个用户自动继承所有数据库的权限。一些特定的服务器端命令也只能从这个数据库运行，比如关闭服务器。
- **local** : local 数据库是不会被复制到其他分片的，因此可以用来存储本地单台服务器的任意 collection。一般不建议用户直接使用 local 库存储任何数据，也不建议进行 CRUD 操作，因为数据无法被正常备份与恢复。
- **config** : 当 MongoDB 使用分片设置时，config 数据库可用来保存分片的相关信息。
- **test** : 默认创建的测试库，连接 [mongod](https://mongoing.com/docs/reference/program/mongod.html) 服务时，如果不指定连接的具体数据库，默认就会连接到 test 数据库。

数据库名可以是满足以下条件的任意 UTF-8 字符串：

- 不能是空字符串`""`。
- 不得含有`' '`（空格)、`.`、`$`、`/`、`\`和 `\0` (空字符)。
- 应全部小写。
- 最多 64 字节。

数据库名最终会变成文件系统里的文件，这也就是有如此多限制的原因。

### [MongoDB 有什么特点？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-有什么特点)

- **数据记录被存储为文档**：MongoDB 中的记录就是一个 BSON 文档，它是由键值对组成的数据结构，类似于 JSON 对象，是 MongoDB 中的基本数据单元。
- **模式自由**：集合的概念类似 MySQL 里的表，但它不需要定义任何模式，能够用更少的数据对象表现复杂的领域模型对象。
- **支持多种查询方式**：MongoDB 查询 API 支持读写操作 (CRUD)以及数据聚合、文本搜索和地理空间查询。
- **支持 ACID 事务**：NoSQL 数据库通常不支持事务，为了可扩展和高性能进行了权衡。不过，也有例外，MongoDB 就支持事务。与关系型数据库一样，MongoDB 事务同样具有 ACID 特性。MongoDB 单文档原生支持原子性，也具备事务的特性。MongoDB 4.0 加入了对多文档事务的支持，但只支持复制集部署模式下的事务，也就是说事务的作用域限制为一个副本集内。MongoDB 4.2 引入了分布式事务，增加了对分片集群上多文档事务的支持，并合并了对副本集上多文档事务的现有支持。
- **高效的二进制存储**：存储在集合中的文档，是以键值对的形式存在的。键用于唯一标识一个文档，一般是 ObjectId 类型，值是以 BSON 形式存在的。BSON = Binary JSON， 是在 JSON 基础上加了一些类型及元数据描述的格式。
- **自带数据压缩功能**：存储同样的数据所需的资源更少。
- **支持 mapreduce**：通过分治的方式完成复杂的聚合任务。不过，从 MongoDB 5.0 开始，map-reduce 已经不被官方推荐使用了，替代方案是 [聚合管道](https://www.mongodb.com/docs/manual/core/aggregation-pipeline/)。聚合管道提供比 map-reduce 更好的性能和可用性。
- **支持多种类型的索引**：MongoDB 支持多种类型的索引，包括单字段索引、复合索引、多键索引、哈希索引、文本索引、 地理位置索引等，每种类型的索引有不同的使用场合。
- **支持 failover**：提供自动故障恢复的功能，主节点发生故障时，自动从从节点中选举出一个新的主节点，确保集群的正常使用，这对于客户端来说是无感知的。
- **支持分片集群**：MongoDB 支持集群自动切分数据，让集群存储更多的数据，具备更强的性能。在数据插入和更新时，能够自动路由和存储。
- **支持存储大文件**：MongoDB 的单文档存储空间要求不超过 16MB。对于超过 16MB 的大文件，MongoDB 提供了 GridFS 来进行存储，通过 GridFS，可以将大型数据进行分块处理，然后将这些切分后的小文档保存在数据库中。

### [MongoDB 适合什么应用场景？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-适合什么应用场景)

**MongoDB 的优势在于其数据模型和存储引擎的灵活性、架构的可扩展性以及对强大的索引支持。**

选用 MongoDB 应该充分考虑 MongoDB 的优势，结合实际项目的需求来决定：

- 随着项目的发展，使用类 JSON 格式（BSON）保存数据是否满足项目需求？MongoDB 中的记录就是一个 BSON 文档，它是由键值对组成的数据结构，类似于 JSON 对象，是 MongoDB 中的基本数据单元。
- 是否需要大数据量的存储？是否需要快速水平扩展？MongoDB 支持分片集群，可以很方便地添加更多的节点（实例），让集群存储更多的数据，具备更强的性能。
- 是否需要更多类型索引来满足更多应用场景？MongoDB 支持多种类型的索引，包括单字段索引、复合索引、多键索引、哈希索引、文本索引、 地理位置索引等，每种类型的索引有不同的使用场合。
- ……

## [MongoDB 存储引擎](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-存储引擎)

### [MongoDB 支持哪些存储引擎？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-支持哪些存储引擎)

存储引擎（Storage Engine）是数据库的核心组件，负责管理数据在内存和磁盘中的存储方式。

与 MySQL 一样，MongoDB 采用的也是 **插件式的存储引擎架构** ，支持不同类型的存储引擎，不同的存储引擎解决不同场景的问题。在创建数据库或集合时，可以指定存储引擎。

> 插件式的存储引擎架构可以实现 Server 层和存储引擎层的解耦，可以支持多种存储引擎，如 MySQL 既可以支持 B-Tree 结构的 InnoDB 存储引擎，还可以支持 LSM 结构的 RocksDB 存储引擎。

在存储引擎刚出来的时候，默认是使用 MMAPV1 存储引擎，MongoDB4.x 版本不再支持 MMAPv1 存储引擎。

现在主要有下面这两种存储引擎：

- **WiredTiger 存储引擎**：自 MongoDB 3.2 以后，默认的存储引擎为 [WiredTiger 存储引擎](https://www.mongodb.com/docs/manual/core/wiredtiger/) 。非常适合大多数工作负载，建议用于新部署。WiredTiger 提供文档级并发模型、检查点和数据压缩（后文会介绍到）等功能。
- **In-Memory 存储引擎**：[In-Memory 存储引擎](https://www.mongodb.com/docs/manual/core/inmemory/)在 MongoDB Enterprise 中可用。它不是将文档存储在磁盘上，而是将它们保留在内存中以获得更可预测的数据延迟。

此外，MongoDB 3.0 提供了 **可插拔的存储引擎 API** ，允许第三方为 MongoDB 开发存储引擎，这点和 MySQL 也比较类似。

### [WiredTiger 基于 LSM Tree 还是 B+ Tree？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#wiredtiger-基于-lsm-tree-还是-b-tree)

目前绝大部分流行的数据库存储引擎都是基于 B/B+ Tree 或者 LSM(Log Structured Merge) Tree 来实现的。对于 NoSQL 数据库来说，绝大部分（比如 HBase、Cassandra、RocksDB）都是基于 LSM 树，MongoDB 不太一样。

上面也说了，自 MongoDB 3.2 以后，默认的存储引擎为 WiredTiger 存储引擎。在 WiredTiger 引擎官网上，我们发现 WiredTiger 使用的是 B+ 树作为其存储结构：



```plain
WiredTiger maintains a table's data in memory using a data structure called a B-Tree ( B+ Tree to be specific), referring to the nodes of a B-Tree as pages. Internal pages carry only keys. The leaf pages store both keys and values.
```

此外，WiredTiger 还支持 [LSM(Log Structured Merge)](https://source.wiredtiger.com/3.1.0/lsm.html) 树作为存储结构，MongoDB 在使用 WiredTiger 作为存储引擎时，默认使用的是 B+ 树。

如果想要了解 MongoDB 使用 B+ 树的原因，可以看看这篇文章：[【驳斥八股文系列】别瞎分析了，MongoDB 使用的是 B+ 树，不是你们以为的 B 树](https://zhuanlan.zhihu.com/p/519658576)。

使用 B+ 树时，WiredTiger 以 **page** 为基本单位往磁盘读写数据。B+ 树的每个节点为一个 page，共有三种类型的 page：

- **root page（根节点）**：B+ 树的根节点。
- **internal page（内部节点）**：不实际存储数据的中间索引节点。
- **leaf page（叶子节点）**：真正存储数据的叶子节点，包含一个页头（page header）、块头（block header）和真正的数据（key/value），其中页头定义了页的类型、页中实际载荷数据的大小、页中记录条数等信息；块头定义了此页的 checksum、块在磁盘上的寻址位置等信息。

其整体结构如下图所示：

![WiredTiger B+树整体结构](https://oss.javaguide.cn/github/javaguide/database/mongodb/mongodb-b-plus-tree-integral-structure.png)

如果想要深入研究学习 WiredTiger 存储引擎，推荐阅读 MongoDB 中文社区的 [WiredTiger 存储引擎系列](https://mongoing.com/archives/category/wiredtiger存储引擎系列)。

## [MongoDB 聚合](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-聚合)

### [MongoDB 聚合有什么用？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-聚合有什么用)

实际项目中，我们经常需要将多个文档甚至是多个集合汇总到一起计算分析（比如求和、取最大值）并返回计算后的结果，这个过程被称为 **聚合操作** 。

根据官方文档介绍，我们可以使用聚合操作来：

- 将来自多个文档的值组合在一起。
- 对集合中的数据进行的一系列运算。
- 分析数据随时间的变化。

### [MongoDB 提供了哪几种执行聚合的方法？](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-提供了哪几种执行聚合的方法)

MongoDB 提供了两种执行聚合的方法：

- **聚合管道（Aggregation Pipeline）**：执行聚合操作的首选方法。
- **单一目的聚合方法（Single purpose aggregation methods）**：也就是单一作用的聚合函数比如 `count()`、`distinct()`、`estimatedDocumentCount()`。

绝大部分文章中还提到了 **map-reduce** 这种聚合方法。不过，从 MongoDB 5.0 开始，map-reduce 已经不被官方推荐使用了，替代方案是 [聚合管道](https://www.mongodb.com/docs/manual/core/aggregation-pipeline/)。聚合管道提供比 map-reduce 更好的性能和可用性。

MongoDB 聚合管道由多个阶段组成，每个阶段在文档通过管道时转换文档。每个阶段接收前一个阶段的输出，进一步处理数据，并将其作为输入数据发送到下一个阶段。

每个管道的工作流程是：

1. 接受一系列原始数据文档
2. 对这些文档进行一系列运算
3. 结果文档输出给下一个阶段

![管道的工作流程](https://oss.javaguide.cn/github/javaguide/database/mongodb/mongodb-aggregation-stage.png)

**常用阶段操作符**：

| 操作符   | 简述                                                         |
| -------- | ------------------------------------------------------------ |
| $match   | 匹配操作符，用于对文档集合进行筛选                           |
| $project | 投射操作符，用于重构每一个文档的字段，可以提取字段，重命名字段，甚至可以对原有字段进行操作后新增字段 |
| $sort    | 排序操作符，用于根据一个或多个字段对文档进行排序             |
| $limit   | 限制操作符，用于限制返回文档的数量                           |
| $skip    | 跳过操作符，用于跳过指定数量的文档                           |
| $count   | 统计操作符，用于统计文档的数量                               |
| $group   | 分组操作符，用于对文档集合进行分组                           |
| $unwind  | 拆分操作符，用于将数组中的每一个值拆分为单独的文档           |
| $lookup  | 连接操作符，用于连接同一个数据库中另一个集合，并获取指定的文档，类似于 populate |

更多操作符介绍详见官方文档：https://docs.mongodb.com/manual/reference/operator/aggregation/

阶段操作符用于 `db.collection.aggregate` 方法里面，数组参数中的第一层。



```sql
db.collection.aggregate( [ { 阶段操作符：表述 }, { 阶段操作符：表述 }, ... ] )
```

下面是 MongoDB 官方文档中的一个例子：



```sql
db.orders.aggregate([
   # 第一阶段：$match阶段按status字段过滤文档，并将status等于"A"的文档传递到下一阶段。
    { $match: { status: "A" } },
  # 第二阶段：$group阶段按cust_id字段将文档分组，以计算每个cust_id唯一值的金额总和。
    { $group: { _id: "$cust_id", total: { $sum: "$amount" } } }
])
```

## [MongoDB 事务](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-事务)

> MongoDB 事务想要搞懂原理还是比较花费时间的，我自己也没有搞太明白。因此，我这里只是简单介绍一下 MongoDB 事务，想要了解原理的小伙伴，可以自行搜索查阅相关资料。
>
> 这里推荐几篇文章，供大家参考：
>
> - [技术干货| MongoDB 事务原理](https://mongoing.com/archives/82187)
> - [MongoDB 一致性模型设计与实现](https://developer.aliyun.com/article/782494)
> - [MongoDB 官方文档对事务的介绍](https://www.mongodb.com/docs/upcoming/core/transactions/)

我们在介绍 NoSQL 数据的时候也说过，NoSQL 数据库通常不支持事务，为了可扩展和高性能进行了权衡。不过，也有例外，MongoDB 就支持事务。

与关系型数据库一样，MongoDB 事务同样具有 ACID 特性：

- **原子性**（`Atomicity`）：事务是最小的执行单位，不允许分割。事务的原子性确保动作要么全部完成，要么完全不起作用；
- **一致性**（`Consistency`）：执行事务前后，数据保持一致，例如转账业务中，无论事务是否成功，转账者和收款人的总额应该是不变的；
- **隔离性**（`Isolation`）：并发访问数据库时，一个用户的事务不被其他事务所干扰，各并发事务之间数据库是独立的。WiredTiger 存储引擎支持读未提交（ read-uncommitted ）、读已提交（ read-committed ）和快照（ snapshot ）隔离，MongoDB 启动时默认选快照隔离。在不同隔离级别下，一个事务的生命周期内，可能出现脏读、不可重复读、幻读等现象。
- **持久性**（`Durability`）：一个事务被提交之后。它对数据库中数据的改变是持久的，即使数据库发生故障也不应该对其有任何影响。

关于事务的详细介绍这篇文章就不多说了，感兴趣的可以看看我写的[MySQL 常见面试题总结](https://javaguide.cn/database/mysql/mysql-questions-01.html)这篇文章，里面有详细介绍到。

MongoDB 单文档原生支持原子性，也具备事务的特性。当谈论 MongoDB 事务的时候，通常指的是 **多文档** 。MongoDB 4.0 加入了对多文档 ACID 事务的支持，但只支持复制集部署模式下的 ACID 事务，也就是说事务的作用域限制为一个副本集内。MongoDB 4.2 引入了 **分布式事务** ，增加了对分片集群上多文档事务的支持，并合并了对副本集上多文档事务的现有支持。

根据官方文档介绍：

> 从 MongoDB 4.2 开始，分布式事务和多文档事务在 MongoDB 中是一个意思。分布式事务是指分片集群和副本集上的多文档事务。从 MongoDB 4.2 开始，多文档事务（无论是在分片集群还是副本集上）也称为分布式事务。

在大多数情况下，多文档事务比单文档写入会产生更大的性能成本。对于大部分场景来说， [非规范化数据模型（嵌入式文档和数组）](https://www.mongodb.com/docs/upcoming/core/data-model-design/#std-label-data-modeling-embedding) 依然是最佳选择。也就是说，适当地对数据进行建模可以最大限度地减少对多文档事务的需求。

**注意**：

- 从 MongoDB 4.2 开始，多文档事务支持副本集和分片集群，其中：主节点使用 WiredTiger 存储引擎，同时从节点使用 WiredTiger 存储引擎或 In-Memory 存储引擎。在 MongoDB 4.0 中，只有使用 WiredTiger 存储引擎的副本集支持事务。
- 在 MongoDB 4.2 及更早版本中，你无法在事务中创建集合。从 MongoDB 4.4 开始，您可以在事务中创建集合和索引。有关详细信息，请参阅 [在事务中创建集合和索引](https://www.mongodb.com/docs/upcoming/core/transactions/#std-label-transactions-create-collections-indexes)。

## [MongoDB 数据压缩](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#mongodb-数据压缩)

借助 WiredTiger 存储引擎（ MongoDB 3.2 后的默认存储引擎），MongoDB 支持对所有集合和索引进行压缩。压缩以额外的 CPU 为代价最大限度地减少存储使用。

默认情况下，WiredTiger 使用 [Snappy](https://github.com/google/snappy) 压缩算法（谷歌开源，旨在实现非常高的速度和合理的压缩，压缩比 3 ～ 5 倍）对所有集合使用块压缩，对所有索引使用前缀压缩。

除了 Snappy 之外，对于集合还有下面这些压缩算法：

- [zlib](https://github.com/madler/zlib)：高度压缩算法，压缩比 5 ～ 7 倍
- [Zstandard](https://github.com/facebook/zstd)（简称 zstd）：Facebook 开源的一种快速无损压缩算法，针对 zlib 级别的实时压缩场景和更好的压缩比，提供更高的压缩率和更低的 CPU 使用率，MongoDB 4.2 开始可用。

WiredTiger 日志也会被压缩，默认使用的也是 Snappy 压缩算法。如果日志记录小于或等于 128 字节，WiredTiger 不会压缩该记录。

## [Amazon Document 与 MongoDB 的差异](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#amazon-document-与-mongodb-的差异)

Amazon DocumentDB（与 MongoDB 兼容） 是一种快速、可靠、完全托管的数据库服务。Amazon DocumentDB 可在云中轻松设置、操作和扩展与 MongoDB 兼容的数据库。

### [`$vectorSearch` 运算符](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#vectorsearch-运算符)

Amazon DocumentDB 不支持`$vectorSearch`作为独立运营商。相反，我们在`$search`运营商`vectorSearch`内部支持。有关更多信息，请参阅 [向量搜索 Amazon DocumentDB](https://docs.aws.amazon.com/zh_cn/documentdb/latest/developerguide/vector-search.html)。

### [`OpCountersCommand`](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#opcounterscommand)

Amazon DocumentDB 的`OpCountersCommand`行为偏离于 MongoDB 的`opcounters.command` 如下：

- MongoDB 的`opcounters.command` 计入除插入、更新和删除之外的所有命令，而 Amazon DocumentDB 的 `OpCountersCommand` 也排除 `find` 命令。
- Amazon DocumentDB 将内部命令（例如`getCloudWatchMetricsV2`）对 `OpCountersCommand` 计入。

### [管理数据库和集合](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#管理数据库和集合)

Amazon DocumentDB 不支持管理或本地数据库，MongoDB `system.*` 或 `startup_log` 集合也不支持。

### [`cursormaxTimeMS`](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#cursormaxtimems)

在 Amazon DocumentDB 中，`cursor.maxTimeMS` 重置每个请求的计数器。`getMore`因此，如果指定了 3000MS `maxTimeMS`，则该查询耗时 2800MS，而每个后续`getMore`请求耗时 300MS，则游标不会超时。游标仅在单个操作（无论是查询还是单个`getMore`请求）耗时超过指定值时才将超时`maxTimeMS`。此外，检查游标执行时间的扫描器以五 (5) 分钟间隔尺寸运行。

### [explain()](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#explain)

Amazon DocumentDB 在利用分布式、容错、自修复的存储系统的专用数据库引擎上模拟 MongoDB 4.0 API。因此，查询计划和`explain()` 的输出在 Amazon DocumentDB 和 MongoDB 之间可能有所不同。希望控制其查询计划的客户可以使用 `$hint` 运算符强制选择首选索引。

### [字段名称限制](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#字段名称限制)

Amazon DocumentDB 不支持点“。” 例如，文档字段名称中 `db.foo.insert({‘x.1’:1})`。

Amazon DocumentDB 也不支持字段名称中的 $ 前缀。

例如，在 Amazon DocumentDB 或 MongoDB 中尝试以下命令：



```shell
rs0:PRIMARY< db.foo.insert({"a":{"$a":1}})
```

MongoDB 将返回以下内容：



```shell
WriteResult({ "nInserted" : 1 })
```

Amazon DocumentDB 将返回一个错误：



```shell
WriteResult({
  "nInserted" : 0,
  "writeError" : {
    "code" : 2,
    "errmsg" : "Document can't have $ prefix field names: $a"
  }
})
```

## [参考](https://javaguide.cn/database/mongodb/mongodb-questions-01.html#参考)

- MongoDB 官方文档（主要参考资料，以官方文档为准）：https://www.mongodb.com/docs/manual/
- 《MongoDB 权威指南》
- 技术干货| MongoDB 事务原理 - MongoDB 中文社区：https://mongoing.com/archives/82187
- Transactions - MongoDB 官方文档：https://www.mongodb.com/docs/manual/core/transactions/
- WiredTiger Storage Engine - MongoDB 官方文档：https://www.mongodb.com/docs/manual/core/wiredtiger/
- WiredTiger 存储引擎之一：基础数据结构分析：https://mongoing.com/topic/archives-35143

## [MongoDB 索引](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#mongodb-索引)

### [MongoDB 索引有什么用?](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#mongodb-索引有什么用)

和关系型数据库类似，MongoDB 中也有索引。索引的目的主要是用来提高查询效率，如果没有索引的话，MongoDB 必须执行 **集合扫描** ，即扫描集合中的每个文档，以选择与查询语句匹配的文档。如果查询存在合适的索引，MongoDB 可以使用该索引来限制它必须检查的文档数量。并且，MongoDB 可以使用索引中的排序返回排序后的结果。

虽然索引可以显著缩短查询时间，但是使用索引、维护索引是有代价的。在执行写入操作时，除了要更新文档之外，还必须更新索引，这必然会影响写入的性能。因此，当有大量写操作而读操作少时，或者不考虑读操作的性能时，都不推荐建立索引。

### [MongoDB 支持哪些类型的索引？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#mongodb-支持哪些类型的索引)

**MongoDB 支持多种类型的索引，包括单字段索引、复合索引、多键索引、哈希索引、文本索引、 地理位置索引等，每种类型的索引有不同的使用场合。**

- **单字段索引：** 建立在单个字段上的索引，索引创建的排序顺序无所谓，MongoDB 可以头/尾开始遍历。
- **复合索引：** 建立在多个字段上的索引，也可以称之为组合索引、联合索引。
- **多键索引**：MongoDB 的一个字段可能是数组，在对这种字段创建索引时，就是多键索引。MongoDB 会为数组的每个值创建索引。就是说你可以按照数组里面的值做条件来查询，这个时候依然会走索引。
- **哈希索引**：按数据的哈希值索引，用在哈希分片集群上。
- **文本索引：** 支持对字符串内容的文本搜索查询。文本索引可以包含任何值为字符串或字符串元素数组的字段。一个集合只能有一个文本搜索索引，但该索引可以覆盖多个字段。MongoDB 虽然支持全文索引，但是性能低下，暂时不建议使用。
- **地理位置索引：** 基于经纬度的索引，适合 2D 和 3D 的位置查询。
- **唯一索引**：确保索引字段不会存储重复值。如果集合已经存在了违反索引的唯一约束的文档，则后台创建唯一索引会失败。
- **TTL 索引**：TTL 索引提供了一个过期机制，允许为每一个文档设置一个过期时间，当一个文档达到预设的过期时间之后就会被删除。
- ……

### [复合索引中字段的顺序有影响吗？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#复合索引中字段的顺序有影响吗)

复合索引中字段的顺序非常重要，例如下图中的复合索引由`{userid:1, score:-1}`组成，则该复合索引首先按照`userid`升序排序；然后再每个`userid`的值内，再按照`score`降序排序。

![复合索引](https://oss.javaguide.cn/github/javaguide/database/mongodb/mongodb-composite-index.png)

在复合索引中，按照何种方式排序，决定了该索引在查询中是否能被应用到。

走复合索引的排序：



```sql
db.s2.find().sort({"userid": 1, "score": -1})
db.s2.find().sort({"userid": -1, "score": 1})
```

不走复合索引的排序：



```sql
db.s2.find().sort({"userid": 1, "score": 1})
db.s2.find().sort({"userid": -1, "score": -1})
db.s2.find().sort({"score": 1, "userid": -1})
db.s2.find().sort({"score": 1, "userid": 1})
db.s2.find().sort({"score": -1, "userid": -1})
db.s2.find().sort({"score": -1, "userid": 1})
```

我们可以通过 explain 进行分析：



```sql
db.s2.find().sort({"score": -1, "userid": 1}).explain()
```

### [复合索引遵循左前缀原则吗？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#复合索引遵循左前缀原则吗)

**MongoDB 的复合索引遵循左前缀原则**：拥有多个键的索引，可以同时得到所有这些键的前缀组成的索引，但不包括除左前缀之外的其他子集。比如说，有一个类似 `{a: 1, b: 1, c: 1, ..., z: 1}` 这样的索引，那么实际上也等于有了 `{a: 1}`、`{a: 1, b: 1}`、`{a: 1, b: 1, c: 1}` 等一系列索引，但是不会有 `{b: 1}` 这样的非左前缀的索引。

### [什么是 TTL 索引？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#什么是-ttl-索引)

TTL 索引提供了一个过期机制，允许为每一个文档设置一个过期时间 `expireAfterSeconds` ，当一个文档达到预设的过期时间之后就会被删除。TTL 索引除了有 `expireAfterSeconds` 属性外，和普通索引一样。

数据过期对于某些类型的信息很有用，比如机器生成的事件数据、日志和会话信息，这些信息只需要在数据库中保存有限的时间。

**TTL 索引运行原理**：

- MongoDB 会开启一个后台线程读取该 TTL 索引的值来判断文档是否过期，但不会保证已过期的数据会立马被删除，因后台线程每 60 秒触发一次删除任务，且如果删除的数据量较大，会存在上一次的删除未完成，而下一次的任务已经开启的情况，导致过期的数据也会出现超过了数据保留时间 60 秒以上的现象。
- 对于副本集而言，TTL 索引的后台进程只会在 Primary 节点开启，在从节点会始终处于空闲状态，从节点的数据删除是由主库删除后产生的 oplog 来做同步。

**TTL 索引限制**：

- TTL 索引是单字段索引。复合索引不支持 TTL
- `_id`字段不支持 TTL 索引。
- 无法在上限集合(Capped Collection)上创建 TTL 索引，因为 MongoDB 无法从上限集合中删除文档。
- 如果某个字段已经存在非 TTL 索引，那么在该字段上无法再创建 TTL 索引。

### [什么是覆盖索引查询？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#什么是覆盖索引查询)

根据官方文档介绍，覆盖查询是以下的查询：

- 所有的查询字段是索引的一部分。
- 结果中返回的所有字段都在同一索引中。
- 查询中没有字段等于`null`。

由于所有出现在查询中的字段是索引的一部分， MongoDB 无需在整个数据文档中检索匹配查询条件和返回使用相同索引的查询结果。因为索引存在于内存中，从索引中获取数据比通过扫描文档读取数据要快得多。

举个例子：我们有如下 `users` 集合:



```json
{
   "_id": ObjectId("53402597d852426020000002"),
   "contact": "987654321",
   "dob": "01-01-1991",
   "gender": "M",
   "name": "Tom Benzamin",
   "user_name": "tombenzamin"
}
```

我们在 `users` 集合中创建联合索引，字段为 `gender` 和 `user_name` :



```sql
db.users.ensureIndex({gender:1,user_name:1})
```

现在，该索引会覆盖以下查询：



```sql
db.users.find({gender:"M"},{user_name:1,_id:0})
```

为了让指定的索引覆盖查询，必须显式地指定 `_id: 0` 来从结果中排除 `_id` 字段，因为索引不包括 `_id` 字段。

## [MongoDB 高可用](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#mongodb-高可用)

### [复制集群](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#复制集群)

#### [什么是复制集群？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#什么是复制集群)

MongoDB 的复制集群又称为副本集群，是一组维护相同数据集合的 mongod 进程。

客户端连接到整个 Mongodb 复制集群，主节点机负责整个复制集群的写，从节点可以进行读操作，但默认还是主节点负责整个复制集群的读。主节点发生故障时，自动从从节点中选举出一个新的主节点，确保集群的正常使用，这对于客户端来说是无感知的。

通常来说，一个复制集群包含 1 个主节点（Primary），多个从节点（Secondary）以及零个或 1 个仲裁节点（Arbiter）。

- **主节点**：整个集群的写操作入口，接收所有的写操作，并将集合所有的变化记录到操作日志中，即 oplog。主节点挂掉之后会自动选出新的主节点。
- **从节点**：从主节点同步数据，在主节点挂掉之后选举新节点。不过，从节点可以配置成 0 优先级，阻止它在选举中成为主节点。
- **仲裁节点**：这个是为了节约资源或者多机房容灾用，只负责主节点选举时投票不存数据，保证能有节点获得多数赞成票。

下图是一个典型的三成员副本集群：

![img](https://oss.javaguide.cn/github/javaguide/database/mongodb/replica-set-read-write-operations-primary.png)

主节点与备节点之间是通过 **oplog（操作日志）** 来同步数据的。oplog 是 local 库下的一个特殊的 **上限集合(Capped Collection)** ，用来保存写操作所产生的增量日志，类似于 MySQL 中 的 Binlog。

> 上限集合类似于定长的循环队列，数据顺序追加到集合的尾部，当集合空间达到上限时，它会覆盖集合中最旧的文档。上限集合的数据将会被顺序写入到磁盘的固定空间内，所以，I/O 速度非常快，如果不建立索引，性能更好。

![img](https://oss.javaguide.cn/github/javaguide/database/mongodb/replica-set-primary-with-two-secondaries.png)

当主节点上的一个写操作完成后，会向 oplog 集合写入一条对应的日志，而从节点则通过这个 oplog 不断拉取到新的日志，在本地进行回放以达到数据同步的目的。

副本集最多有一个主节点。 如果当前主节点不可用，一个选举会抉择出新的主节点。MongoDB 的节点选举规则能够保证在 Primary 挂掉之后选取的新节点一定是集群中数据最全的一个。

#### [为什么要用复制集群？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#为什么要用复制集群)

- **实现 failover**：提供自动故障恢复的功能，主节点发生故障时，自动从从节点中选举出一个新的主节点，确保集群的正常使用，这对于客户端来说是无感知的。
- **实现读写分离**：我们可以设置从节点上可以读取数据，主节点负责写入数据，这样的话就实现了读写分离，减轻了主节点读写压力过大的问题。MongoDB 4.0 之前版本如果主库压力不大,不建议读写分离，因为写会阻塞读，除非业务对响应时间不是非常关注以及读取历史数据接受一定时间延迟。

### [分片集群](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#分片集群)

#### [什么是分片集群？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#什么是分片集群)

分片集群是 MongoDB 的分布式版本，相较副本集，分片集群数据被均衡的分布在不同分片中， 不仅大幅提升了整个集群的数据容量上限，也将读写的压力分散到不同分片，以解决副本集性能瓶颈的难题。

MongoDB 的分片集群由如下三个部分组成（下图来源于[官方文档对分片集群的介绍](https://www.mongodb.com/docs/manual/sharding/)）：

![img](https://oss.javaguide.cn/github/javaguide/database/mongodb/sharded-cluster-production-architecture.png)

- **Config Servers**：配置服务器，本质上是一个 MongoDB 的副本集，负责存储集群的各种元数据和配置，如分片地址、Chunks 等
- **Mongos**：路由服务，不存具体数据，从 Config 获取集群配置讲请求转发到特定的分片，并且整合分片结果返回给客户端。
- **Shard**：每个分片是整体数据的一部分子集，从 MongoDB3.6 版本开始，每个 Shard 必须部署为副本集（replica set）架构

#### [为什么要用分片集群？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#为什么要用分片集群)

随着系统数据量以及吞吐量的增长，常见的解决办法有两种：垂直扩展和水平扩展。

垂直扩展通过增加单个服务器的能力来实现，比如磁盘空间、内存容量、CPU 数量等；水平扩展则通过将数据存储到多个服务器上来实现，根据需要添加额外的服务器以增加容量。

类似于 Redis Cluster，MongoDB 也可以通过分片实现 **水平扩展** 。水平扩展这种方式更灵活，可以满足更大数据量的存储需求，支持更高吞吐量。并且，水平扩展所需的整体成本更低，仅仅需要相对较低配置的单机服务器即可，代价是增加了部署的基础设施和维护的复杂性。

也就是说当你遇到如下问题时，可以使用分片集群解决：

- 存储容量受单机限制，即磁盘资源遭遇瓶颈。
- 读写能力受单机限制，可能是 CPU、内存或者网卡等资源遭遇瓶颈，导致读写能力无法扩展。

#### [什么是分片键？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#什么是分片键)

**分片键（Shard Key）** 是数据分区的前提， 从而实现数据分发到不同服务器上，减轻服务器的负担。也就是说，分片键决定了集合内的文档如何在集群的多个分片间的分布状况。

分片键就是文档里面的一个字段，但是这个字段不是普通的字段，有一定的要求：

- 它必须在所有文档中都出现。
- 它必须是集合的一个索引，可以是单索引或复合索引的前缀索引，不能是多索引、文本索引或地理空间位置索引。
- MongoDB 4.2 之前的版本，文档的分片键字段值不可变。MongoDB 4.2 版本开始，除非分片键字段是不可变的 `_id` 字段，否则您可以更新文档的分片键值。MongoDB 5.0 版本开始，实现了实时重新分片（live resharding），可以实现分片键的完全重新选择。
- 它的大小不能超过 512 字节。

#### [如何选择分片键？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#如何选择分片键)

选择合适的片键对 sharding 效率影响很大，主要基于如下四个因素（摘自[分片集群使用注意事项 - - 腾讯云文档](https://cloud.tencent.com/document/product/240/44611)）：

- **取值基数** 取值基数建议尽可能大，如果用小基数的片键，因为备选值有限，那么块的总数量就有限，随着数据增多，块的大小会越来越大，导致水平扩展时移动块会非常困难。 例如：选择年龄做一个基数，范围最多只有 100 个，随着数据量增多，同一个值分布过多时，导致 chunck 的增长超出 chuncksize 的范围，引起 jumbo chunk，从而无法迁移，导致数据分布不均匀，性能瓶颈。
- **取值分布** 取值分布建议尽量均匀，分布不均匀的片键会造成某些块的数据量非常大，同样有上面数据分布不均匀，性能瓶颈的问题。
- **查询带分片** 查询时建议带上分片，使用分片键进行条件查询时，mongos 可以直接定位到具体分片，否则 mongos 需要将查询分发到所有分片，再等待响应返回。
- **避免单调递增或递减** 单调递增的 sharding key，数据文件挪动小，但写入会集中，导致最后一篇的数据量持续增大，不断发生迁移，递减同理。

综上，在选择片键时要考虑以上 4 个条件，尽可能满足更多的条件，才能降低 MoveChunks 对性能的影响，从而获得最优的性能体验。

#### [分片策略有哪些？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#分片策略有哪些)

MongoDB 支持两种分片算法来满足不同的查询需求（摘自[MongoDB 分片集群介绍 - 阿里云文档](https://help.aliyun.com/document_detail/64561.html?spm=a2c4g.11186623.0.0.3121565eQhUGGB#h2--shard-key-3)）：

**1、基于范围的分片**：

![img](https://oss.javaguide.cn/github/javaguide/database/mongodb/example-of-scope-based-sharding.png)

MongoDB 按照分片键（Shard Key）的值的范围将数据拆分为不同的块（Chunk），每个块包含了一段范围内的数据。当分片键的基数大、频率低且值非单调变更时，范围分片更高效。

- 优点：Mongos 可以快速定位请求需要的数据，并将请求转发到相应的 Shard 节点中。
- 缺点：可能导致数据在 Shard 节点上分布不均衡，容易造成读写热点，且不具备写分散性。
- 适用场景：分片键的值不是单调递增或单调递减、分片键的值基数大且重复的频率低、需要范围查询等业务场景。

**2、基于 Hash 值的分片**

![img](https://oss.javaguide.cn/github/javaguide/database/mongodb/example-of-hash-based-sharding.png)

MongoDB 计算单个字段的哈希值作为索引值，并以哈希值的范围将数据拆分为不同的块（Chunk）。

- 优点：可以将数据更加均衡地分布在各 Shard 节点中，具备写分散性。
- 缺点：不适合进行范围查询，进行范围查询时，需要将读请求分发到所有的 Shard 节点。
- 适用场景：分片键的值存在单调递增或递减、片键的值基数大且重复的频率低、需要写入的数据随机分发、数据读取随机性较大等业务场景。

除了上述两种分片策略，您还可以配置 **复合片键** ，例如由一个低基数的键和一个单调递增的键组成。

#### [分片数据如何存储？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#分片数据如何存储)

**Chunk（块）** 是 MongoDB 分片集群的一个核心概念，其本质上就是由一组 Document 组成的逻辑数据单元。每个 Chunk 包含一定范围片键的数据，互不相交且并集为全部数据，即离散数学中**划分**的概念。

分片集群不会记录每条数据在哪个分片上，而是记录 Chunk 在哪个分片上一级这个 Chunk 包含哪些数据。

默认情况下，一个 Chunk 的最大值默认为 64MB（可调整，取值范围为 1~1024 MB。如无特殊需求，建议保持默认值），进行数据插入、更新、删除时，如果此时 Mongos 感知到了目标 Chunk 的大小或者其中的数据量超过上限，则会触发 **Chunk 分裂**。

![Chunk 分裂](https://oss.javaguide.cn/github/javaguide/database/mongodb/chunk-splitting-shard-a.png)

数据的增长会让 Chunk 分裂得越来越多。这个时候，各个分片上的 Chunk 数量可能会不平衡。Mongos 中的 **均衡器(Balancer)** 组件就会执行自动平衡，尝试使各个 Shard 上 Chunk 的数量保持均衡，这个过程就是 **再平衡（Rebalance）**。默认情况下，数据库和集合的 Rebalance 是开启的。

如下图所示，随着数据插入，导致 Chunk 分裂，让 AB 两个分片有 3 个 Chunk，C 分片只有一个，这个时候就会把 B 分配的迁移一个到 C 分片实现集群数据均衡。

![Chunk 迁移](https://oss.javaguide.cn/github/javaguide/database/mongodb/mongo-reblance-three-shards.png)

> Balancer 是 MongoDB 的一个运行在 Config Server 的 Primary 节点上(自 MongoDB 3.4 版本起)的后台进程，它监控每个分片上 Chunk 数量，并在某个分片上 Chunk 数量达到阈值进行迁移。

Chunk 只会分裂，不会合并，即使 chunkSize 的值变大。

Rebalance 操作是比较耗费系统资源的，我们可以通过在业务低峰期执行、预分片或者设置 Rebalance 时间窗等方式来减少其对 MongoDB 正常使用所带来的影响。

#### [Chunk 迁移原理是什么？](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#chunk-迁移原理是什么)

关于 Chunk 迁移原理的详细介绍，推荐阅读 MongoDB 中文社区的[一文读懂 MongoDB chunk 迁移](https://mongoing.com/archives/77479)这篇文章。

## [学习资料推荐](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#学习资料推荐)

- [MongoDB 中文手册|官方文档中文版](https://docs.mongoing.com/)（推荐）：基于 4.2 版本，不断与官方最新版保持同步。
- [MongoDB 初学者教程——7 天学习 MongoDB](https://mongoing.com/archives/docs/mongodb初学者教程/mongodb如何创建数据库和集合)：快速入门。
- [SpringBoot 整合 MongoDB 实战 - 2022](https://www.cnblogs.com/dxflqm/p/16643981.html)：很不错的一篇 MongoDB 入门文章，主要围绕 MongoDB 的 Java 客户端使用进行基本的增删改查操作介绍。

## [参考](https://javaguide.cn/database/mongodb/mongodb-questions-02.html#参考)

- MongoDB 官方文档（主要参考资料，以官方文档为准）：https://www.mongodb.com/docs/manual/
- 《MongoDB 权威指南》
- Indexes - MongoDB 官方文档：https://www.mongodb.com/docs/manual/indexes/
- MongoDB - 索引知识 - 程序员翔仔 - 2022：https://fatedeity.cn/posts/database/mongodb-index-knowledge.html
- MongoDB - 索引: https://www.cnblogs.com/Neeo/articles/14325130.html
- Sharding - MongoDB 官方文档：https://www.mongodb.com/docs/manual/sharding/
- MongoDB 分片集群介绍 - 阿里云文档：https://help.aliyun.com/document_detail/64561.html
- 分片集群使用注意事项 - - 腾讯云文档：https://cloud.tencent.com/document/product/240/44611

## SQL基础

### [#](https://www.xiaolincoding.com/interview/mysql.html#nosql和sql的区别)NOSQL 和 SQL的区别？

SQL 数据库，指关系型数据库 - 主要代表：SQL Server，Oracle，MySQL(开源)，PostgreSQL(开源)。

关系型数据库存储结构化数据。这些数据逻辑上以行列二维表的形式存在，每一列代表数据的一种属性，每一行代表一个数据实体。

![image-20240725232218438](https://cdn.xiaolincoding.com//picgo/image-20240725232218438.png)

NoSQL指非关系型数据库 ，主要代表：MongoDB，Redis。NoSQL 数据库逻辑上提供了不同于二维表的存储方式，存储方式可以是JSON文档、哈希表或者其他方式。

![image-20240725232206455](https://cdn.xiaolincoding.com//picgo/image-20240725232206455.png)

选择 SQL vs NoSQL，考虑以下因素。

> ACID vs BASE

关系型数据库支持 ACID 即原子性，一致性，隔离性和持续性。相对而言，NoSQL 采用更宽松的模型 BASE ， 即基本可用，软状态和最终一致性。

从实用的角度出发，我们需要考虑对于面对的应用场景，ACID 是否是必须的。比如银行应用就必须保证 ACID，否则一笔钱可能被使用两次；又比如社交软件不必保证 ACID，因为一条状态的更新对于所有用户读取先后时间有数秒不同并不影响使用。

对于需要保证 ACID 的应用，我们可以优先考虑 SQL。反之则可以优先考虑 NoSQL。

> 扩展性对比

NoSQL数据之间无关系，这样就非常容易扩展，也无形之间，在架构的层面上带来了可扩展的能力。比如 redis 自带主从复制模式、哨兵模式、切片集群模式。

相反关系型数据库的数据之间存在关联性，水平扩展较难 ，需要解决跨服务器 JOIN，分布式事务等问题。

### [#](https://www.xiaolincoding.com/interview/mysql.html#数据库三大范式是什么)数据库三大范式是什么？

**第一范式（1NF）：要求数据库表的每一列都是不可分割的原子数据项。**

举例说明：

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909201651535-1215699096.png)

在上面的表中，“家庭信息”和“学校信息”列均不满足原子性的要求，故不满足第一范式，调整如下：

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909202243826-1032549277.png)

可见，调整后的每一列都是不可再分的，因此满足第一范式（1NF）；

**第二范式（2NF）：在1NF的基础上，非码属性必须完全依赖于候选码（在1NF基础上消除非主属性对主码的部分函数依赖）**

**第二范式需要确保数据库表中的每一列都和主键相关，而不能只与主键的某一部分相关（主要针对联合主键而言）。**

举例说明：

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909204750951-639647799.png)

在上图所示的情况中，同一个订单中可能包含不同的产品，因此主键必须是“订单号”和“产品号”联合组成，

但可以发现，产品数量、产品折扣、产品价格与“订单号”和“产品号”都相关，但是订单金额和订单时间仅与“订单号”相关，与“产品号”无关，

这样就不满足第二范式的要求，调整如下，需分成两个表：

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909210444227-1008056975.png)

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909210458847-2092897116.png)

**第三范式（3NF）：在2NF基础上，任何非主[属性 (opens new window)](https://baike.baidu.com/item/属性)不依赖于其它非主属性（在2NF基础上消除传递依赖）**

**第三范式需要确保数据表中的每一列数据都和主键直接相关，而不能间接相关。**

举例说明：

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909211311408-1364899740.png)

上表中，所有属性都完全依赖于学号，所以满足第二范式，但是“班主任性别”和“班主任年龄”直接依赖的是“班主任姓名”，

而不是主键“学号”，所以需做如下调整：

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909211539242-1391100354.png)

![img](https://cdn.xiaolincoding.com//picgo/1218459-20180909211602202-1069383439.png)

这样以来，就满足了第三范式的要求。

### [#](https://www.xiaolincoding.com/interview/mysql.html#mysql-怎么连表查询)MySQL 怎么连表查询？

数据库有以下几种联表查询类型：

1. **内连接 (INNER JOIN)**
2. **左外连接 (LEFT JOIN)**
3. **右外连接 (RIGHT JOIN)**
4. **全外连接 (FULL JOIN)**

![img](https://cdn.xiaolincoding.com//picgo/1721710415166-eff24e6c-555c-436c-b1b8-7c6dbb5850d7.webp)

**1. 内连接 (INNER JOIN)**

内连接返回两个表中有匹配关系的行。**示例**:

```sql
SELECT employees.name, departments.name
FROM employees
INNER JOIN departments
ON employees.department_id = departments.id;
```

这个查询返回每个员工及其所在的部门名称。

**2. 左外连接 (LEFT JOIN)**

左外连接返回左表中的所有行，即使在右表中没有匹配的行。未匹配的右表列会包含NULL。**示例**:

```sql
SELECT employees.name, departments.name
FROM employees
LEFT JOIN departments
ON employees.department_id = departments.id;
```

这个查询返回所有员工及其部门名称，包括那些没有分配部门的员工。

**3. 右外连接 (RIGHT JOIN)**

右外连接返回右表中的所有行，即使左表中没有匹配的行。未匹配的左表列会包含NULL。**示例**:

```sql
SELECT employees.name, departments.name
FROM employees
RIGHT JOIN departments
ON employees.department_id = departments.id;
```

这个查询返回所有部门及其员工，包括那些没有分配员工的部门。

**4. 全外连接 (FULL JOIN)**

全外连接返回两个表中所有行，包括非匹配行，在MySQL中，FULL JOIN 需要使用 UNION 来实现，因为 MySQL 不直接支持 FULL JOIN。**示例**:

```sql
SELECT employees.name, departments.name
FROM employees
LEFT JOIN departments
ON employees.department_id = departments.id

UNION

SELECT employees.name, departments.name
FROM employees
RIGHT JOIN departments
ON employees.department_id = departments.id;
```

这个查询返回所有员工和所有部门，包括没有匹配行的记录。

### [#](https://www.xiaolincoding.com/interview/mysql.html#mysql如何避免重复插入数据)MySQL如何避免重复插入数据？

**方式一：使用UNIQUE约束**

在表的相关列上添加UNIQUE约束，确保每个值在该列中唯一。例如：

```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    email VARCHAR(255) UNIQUE,
    name VARCHAR(255)
);
```

如果尝试插入重复的email，MySQL会返回错误。

**方式二：使用INSERT ... ON DUPLICATE KEY UPDATE**

这种语句允许在插入记录时处理重复键的情况。如果插入的记录与现有记录冲突，可以选择更新现有记录：

```sql
INSERT INTO users (email, name) 
VALUES ('example@example.com', 'John Doe')
ON DUPLICATE KEY UPDATE name = VALUES(name);
```

**方式三：使用INSERT IGNORE**： 该语句会在插入记录时忽略那些因重复键而导致的插入错误。例如：

```sql
INSERT IGNORE INTO users (email, name) 
VALUES ('example@example.com', 'John Doe');
```

如果email已经存在，这条插入语句将被忽略而不会返回错误。

选择哪种方法取决于具体的需求：

- 如果需要保证全局唯一性，使用UNIQUE约束是最佳做法。
- 如果需要插入和更新结合可以使用`ON DUPLICATE KEY UPDATE`。
- 对于快速忽略重复插入，`INSERT IGNORE`是合适的选择。

### [#](https://www.xiaolincoding.com/interview/mysql.html#char-和-varchar有什么区别)CHAR 和 VARCHAR有什么区别？

- CHAR是固定长度的字符串类型，定义时需要指定固定长度，存储时会在末尾补足空格。CHAR适合存储长度固定的数据，如固定长度的代码、状态等，存储空间固定，对于短字符串效率较高。
- VARCHAR是可变长度的字符串类型，定义时需要指定最大长度，实际存储时根据实际长度占用存储空间。VARCHAR适合存储长度可变的数据，如用户输入的文本、备注等，节约存储空间。

### [#](https://www.xiaolincoding.com/interview/mysql.html#varchar后面代表字节还是会字符)varchar后面代表字节还是会字符？

`VARCHAR` 后面括号里的数字代表的是字符数，而不是字节数。

比如 `VARCHAR(10)`，这里的 10 表示该字段最多可以存储 10 个字符。字符的字节长度取决于所使用的字符集。

- 如果字符集是 ASCII 字符集：ASCII 字符集每个字符占用 1 个字节，那么 VARCHAR(10) 最多可以存储 10 个 ASCII 字符，同时占用的存储空间最多为 10 个字节（不考虑额外的长度记录开销）。
- 如果字符集是 UTF - 8 字符集，它的每个字符可能占用 1 到 4 个字节，对于 `VARCHAR(10)` 的字段，它最多可以存储 10 个字符，但占用的字节数会根据字符的不同而变化。

### [#](https://www.xiaolincoding.com/interview/mysql.html#int-1-int-10-在mysql有什么不同)int(1) int(10) 在mysql有什么不同？

`INT(1)` 和 `INT(10)` 的区别主要在于 **显示宽度**，而不是存储范围或数据类型本身的大小。以下是核心区别的总结：

- 本质是显示宽度，不改变存储方式：`INT` 的存储固定为 4 字节，所有 `INT`（无论写成 `INT(1)` 还是 `INT(10)`）占用的存储空间 均为 4 字节。括号内的数值（如 `1` 或 `10`）是显示宽度，用于在 特定场景下 控制数值的展示格式。
- 唯一作用场景：`ZEROFILL` 补零显示，当字段设置 `ZEROFILL` 时：数字显示时会用前导零填充至指定宽度。比如，字段类型为 `INT(4) ZEROFILL`，实际存入 `5` → 显示为 `0005`，实际存入 `12345` → 显示仍为 `12345`（宽度超限时不截断）。

举一个例子

```java
-- 创建一个包含 INT(1) 和 INT(10) 字段的表，并设置 ZEROFILL 属性
CREATE TABLE test_int (
    num1 INT(1) ZEROFILL,
    num2 INT(10) ZEROFILL
);

-- 插入数据
INSERT INTO test_int (num1, num2) VALUES (1, 1);

-- 查询数据
SELECT * FROM test_int;
```

结果分析：

- `num1` 字段由于设置为 `INT(1) ZEROFILL`，其显示宽度为 1，插入数据 `1` 时会显示为 `1`。
- `num2` 字段设置为 `INT(10) ZEROFILL`，显示宽度为 10，插入数据 `1` 时会在前面填充零，显示为 `0000000001`。

### [#](https://www.xiaolincoding.com/interview/mysql.html#text数据类型可以无限大吗)Text数据类型可以无限大吗？

MySQL 3 种text类型的最大长度如下：

- TEXT：65,535 bytes ~64kb
- MEDIUMTEXT：16,777,215 bytes ~16Mb
- LONGTEXT：4,294,967,295 bytes ~4Gb

### [#](https://www.xiaolincoding.com/interview/mysql.html#ip地址如何在数据库里存储)IP地址如何在数据库里存储？

IPv4 地址是一个 32 位的二进制数，通常以点分十进制表示法呈现，例如 `192.168.1.1`。

**字符串类型的存储方式**：直接将 IP 地址作为字符串存储在数据库中，比如可以用 `VARCHAR(15)`来存储。

```java
-- 创建一个表，使用VARCHAR类型存储IPv4地址
CREATE TABLE ip_records (
    id INT AUTO_INCREMENT PRIMARY KEY,
    ip_address VARCHAR(15)
);

-- 插入数据
INSERT INTO ip_records (ip_address) VALUES ('192.168.1.1');
```

- **优点**：直观易懂，方便直接进行数据的插入、查询和显示，不需要进行额外的转换操作。
- **缺点**：占用存储空间较大，字符串比较操作的性能相对较低，不利于进行范围查询。

整数类型的存储方式：将 IPv4 地址转换为 32 位无符号整数进行存储，常用的数据类型有 `INT UNSIGNED`。

```java
-- 创建一个表，使用INT UNSIGNED类型存储IPv4地址
CREATE TABLE ip_records (
    id INT AUTO_INCREMENT PRIMARY KEY,
    ip_address INT UNSIGNED
);

-- 插入数据，需要先将IP地址转换为整数
INSERT INTO ip_records (ip_address) VALUES (INET_ATON('192.168.1.1'));

-- 查询时将整数转换回IP地址
SELECT INET_NTOA(ip_address) FROM ip_records;
```

- **优点**：占用存储空间小，整数比较操作的性能较高，便于进行范围查询。
- **缺点**：需要进行额外的转换操作，不够直观，增加了开发的复杂度。

### [#](https://www.xiaolincoding.com/interview/mysql.html#说一下外键约束)说一下外键约束

外键约束的作用是维护表与表之间的关系，确保数据的完整性和一致性。让我们举一个简单的例子：

假设你有两个表，一个是学生表，另一个是课程表，这两个表之间有一个关系，即一个学生可以选修多门课程，而一门课程也可以被多个学生选修。在这种情况下，我们可以在学生表中定义一个指向课程表的外键，如下所示：

```text
CREATE TABLE students (
  id INT PRIMARY KEY,
  name VARCHAR(50),
  course_id INT,
  FOREIGN KEY (course_id) REFERENCES courses(id)
);
```

这里，`students`表中的`course_id`字段是一个外键，它指向`courses`表中的`id`字段。这个外键约束确保了每个学生所选的课程在`courses`表中都存在，从而维护了数据的完整性和一致性。

如果没有定义外键约束，那么就有可能出现学生选了不存在的课程或者删除了一个课程而忘记从学生表中删除选修该课程的学生的情况，这会破坏数据的完整性和一致性。因此，使用外键约束可以帮助我们避免这些问题。

### [#](https://www.xiaolincoding.com/interview/mysql.html#mysql的关键字in和exist)MySQL的关键字in和exist

在MySQL中，`IN` 和 `EXISTS` 都是用来处理子查询的关键词，但它们在功能、性能和使用场景上有各自的特点和区别。

> IN关键字

`IN` 用于检查左边的表达式是否存在于右边的列表或子查询的结果集中。如果存在，则`IN` 返回`TRUE`，否则返回`FALSE`。

语法结构：

```sql
SELECT column_name(s)
FROM table_name
WHERE column_name IN (value1, value2, ...);
```

或

```sql
SELECT column_name(s)
FROM table_name
WHERE column_name IN (SELECT column_name FROM another_table WHERE condition);
```

例子：

```sql
SELECT * FROM Customers
WHERE Country IN ('Germany', 'France');
```

> EXISTS关键字

`EXISTS` 用于判断子查询是否至少能返回一行数据。它不关心子查询返回什么数据，只关心是否有结果。如果子查询有结果，则`EXISTS` 返回`TRUE`，否则返回`FALSE`。

语法结构：

```sql
SELECT column_name(s)
FROM table_name
WHERE EXISTS (SELECT column_name FROM another_table WHERE condition);
```

例子：

```sql
SELECT * FROM Customers
WHERE EXISTS (SELECT 1 FROM Orders WHERE Orders.CustomerID = Customers.CustomerID);
```

区别与选择：

- **性能差异**：在很多情况下，`EXISTS` 的性能优于 `IN`，特别是当子查询的表很大时。这是因为`EXISTS` 一旦找到匹配项就会立即停止查询，而`IN`可能会扫描整个子查询结果集。
- **使用场景**：如果子查询结果集较小且不频繁变动，`IN` 可能更直观易懂。而当子查询涉及外部查询的每一行判断，并且子查询的效率较高时，`EXISTS` 更为合适。
- **NULL值处理**：`IN` 能够正确处理子查询中包含NULL值的情况，而`EXISTS` 不受子查询结果中NULL值的影响，因为它关注的是行的存在性，而不是具体值。

### [#](https://www.xiaolincoding.com/interview/mysql.html#mysql中的一些基本函数-你知道哪些)mysql中的一些基本函数，你知道哪些？

> 一、字符串函数

**CONCAT(str1, str2, ...)**：连接多个字符串，返回一个合并后的字符串。

```sql
SELECT CONCAT('Hello', ' ', 'World') AS Greeting;
```

**LENGTH(str)**：返回字符串的长度（字符数）。

```sql
SELECT LENGTH('Hello') AS StringLength;
```

**SUBSTRING(str, pos, len)**：从指定位置开始，截取指定长度的子字符串。

```sql
SELECT SUBSTRING('Hello World', 1, 5) AS SubStr;
```

**REPLACE(str, FROM_str, to_str)**：将字符串中的某部分替换为另一个字符串。

```sql
SELECT REPLACE('Hello World', 'World', 'MySQL') AS ReplacedStr;
```

> 二、数值函数

**ABS(num)**：返回数字的绝对值。

```sql
SELECT ABS(-10) AS AbsoluteValue;
```

**POWER(num, exponent)**：返回指定数字的指定幂次方。

```sql
SELECT POWER(2, 3) AS PowerValue;
```

> 三、日期和时间函数

**NOW()**：返回当前日期和时间。

```sql
SELECT NOW() AS CurrentDateTime;
```

**CURDATE()**：返回当前日期。

```sql
SELECT CURDATE() AS CurrentDate;
```

> 四、聚合函数

**COUNT(column)**：计算指定列中的非NULL值的个数。

```sql
SELECT COUNT(*) AS RowCount FROM my_table;
```

**SUM(column)**：计算指定列的总和。

```sql
SELECT SUM(price) AS TotalPrice FROM orders;
```

**AVG(column)**：计算指定列的平均值。

```sql
SELECT AVG(price) AS AveragePrice FROM orders;
```

**MAX(column)**：返回指定列的最大值。

```sql
SELECT MAX(price) AS MaxPrice FROM orders;
```

**MIN(column)**：返回指定列的最小值。

```sql
SELECT MIN(price) AS MinPrice FROM orders;
```

### [#](https://www.xiaolincoding.com/interview/mysql.html#sql查询语句的执行顺序是怎么样的)**SQL查询语句的执行顺序是怎么样的？**

![image-20240820114027032](https://cdn.xiaolincoding.com//picgo/image-20240820114027032.png)

所有的查询语句都是从FROM开始执行，在执行过程中，每个步骤都会生成一个虚拟表，这个虚拟表将作为下一个执行步骤的输入，最后一个步骤产生的虚拟表即为输出结果。

```text
(9) SELECT 
(10) DISTINCT <column>,
(6) AGG_FUNC <column> or <expression>, ...
(1) FROM <left_table> 
    (3) <join_type>JOIN<right_table>
    (2) ON<join_condition>
(4) WHERE <WHERE_condition>
(5) GROUP BY <group_by_list>
(7) WITH {CUBE|ROLLUP}
(8) HAVING <having_condtion>
(11) ORDER BY <order_by_list>
(12) LIMIT <limit_number>;
```

### [#](https://www.xiaolincoding.com/interview/mysql.html#sql题-给学生表、课程成绩表-求不存在01课程但存在02课程的学生的成绩)sql题：给学生表、课程成绩表，求不存在01课程但存在02课程的学生的成绩

可以使用SQL的子查询和`LEFT JOIN`或者`EXISTS`关键字来实现，这里我将展示两种不同的方法来完成这个查询。

假设我们有以下两张表：

1. `Student` 表，其中包含学生的`sid`（学生编号）和其他相关信息。
2. `Score` 表，其中包含`sid`（学生编号），`cid`（课程编号）和`score`（分数）。

方法1：使用LEFT JOIN 和 IS NULL

```sql
SELECT s.sid, s.sname, sc2.cid, sc2.score
FROM Student s
LEFT JOIN Score AS sc1 ON s.sid = sc1.sid AND sc1.cid = '01'
LEFT JOIN Score AS sc2 ON s.sid = sc2.sid AND sc2.cid = '02'
WHERE sc1.cid IS NULL AND sc2.cid IS NOT NULL;
```

方法2：使用NOT EXISTS

```sql
SELECT s.sid, s.sname, sc.cid, sc.score
FROM Student s
JOIN Score sc ON s.sid = sc.sid AND sc.cid = '02'
WHERE NOT EXISTS (
    SELECT 1 FROM Score sc1 WHERE sc1.sid = s.sid AND sc1.cid = '01'
);
```

### [#](https://www.xiaolincoding.com/interview/mysql.html#给定一个学生表-student-score-stu-id-subject-id-score-查询总分排名在5-10名的学生id及对应的总分)给定一个学生表 student_score（stu_id，subject_id，score），查询总分排名在5-10名的学生id及对应的总分

可以使用以下 SQL 查询来检索总分排名在 5 到 10 名的学生 ID 及对应的总分。其中我们先计算每个学生的总分，然后为其分配一个排名，最后检索排名在 5 到 10 之间的记录。

```sql
WITH StudentTotalScores AS (
    SELECT 
        stu_id,
        SUM(score) AS total_score
    FROM 
        student_score
    GROUP BY 
        stu_id
),
RankedStudents AS (
    SELECT
        stu_id,
        total_score,
        RANK() OVER (ORDER BY total_score DESC) AS ranking
    FROM
        StudentTotalScores
)
SELECT
    stu_id,
    total_score
FROM
    RankedStudents
WHERE
    ranking BETWEEN 5 AND 10;
```

解释：

1. 子查询 StudentTotalScores 中，我们通过对 student_score 表中的 stu_id 分组来计算每个学生的总分。
2. 子查询 RankedStudents 中，我们使用 RANK() 函数为每个学生分配一个排名，按总分从高到低排序。
3. 最后，我们在主查询中选择排名在 5 到 10 之间的学生。

### [#](https://www.xiaolincoding.com/interview/mysql.html#sql题-查某个班级下所有学生的选课情况)SQL题：查某个班级下所有学生的选课情况

有三张表：学生信息表、学生选课表、学生班级表

学生信息表（students）结构如下：

```sql
CREATE TABLE students (
  student_id INT PRIMARY KEY, //学生的唯一标识，主键。
  student_name VARCHAR(50), //学生姓名。
  class_id INT //学生所属班级的标识，用于关联班级表。
);
```

学生选课表（`course_SELECTions`）结构如下：

```sql
CREATE TABLE course_SELECTions (
    SELECTion_id INT PRIMARY KEY, //选课记录的唯一标识，主键。
    student_id INT, //选课学生的标识，用于关联学生信息表。
    course_name VARCHAR(50), //所选课程的名称。
);
```

学生班级表（`classes`）结构如下：

```sql
CREATE TABLE classes (
    class_id INT PRIMARY KEY, //班级的唯一标识，主键。
    class_name VARCHAR(50) //班级名称。
);
```

要查询某个班级（例如班级名称为 'Class A'）下所有学生的选课情况，可以使用以下 SQL 查询语句：

```sql
SELECT 
    s.student_id,
    s.student_name,
    cs.course_name
FROM 
    students s
JOIN 
    course_SELECTions cs ON s.student_id = cs.student_id
JOIN 
    classes c ON s.class_id = c.class_id
WHERE 
    c.class_name = 'Class A';
```

### [#](https://www.xiaolincoding.com/interview/mysql.html#如何用-mysql-实现一个可重入的锁)如何用 MySQL 实现一个可重入的锁？

创建一个保存锁记录的表：

```java
CREATE TABLE `lock_table` (
    `id` INT AUTO_INCREMENT PRIMARY KEY,
    //该字段用于存储锁的名称，作为锁的唯一标识符。
    `lock_name` VARCHAR(255) NOT NULL, 
    // holder_thread该字段存储当前持有锁的线程的名称，用于标识哪个线程持有该锁。
    `holder_thread` VARCHAR(255),   
    // reentry_count 该字段存储锁的重入次数，用于实现锁的可重入性
    `reentry_count` INT DEFAULT 0
);
```

加锁的实现逻辑

1. 开启事务
2. 执行 SQL SELECT holder_thread, reentry_count FROM lock_table WHERE lock_name =? FOR UPDATE，查询是否存在该记录：
   - 如果记录不存在，则直接加锁，执行 INSERT INTO lock_table (lock_name, holder_thread, reentry_count) VALUES (?,?, 1)
   - 如果记录存在，且持有者是同一个线程，则可冲入，增加重入次数，执行 UPDATE lock_table SET reentry_count = reentry_count + 1 WHERE lock_name =?
3. 提交事务

解锁的逻辑：

1. 开启事务
2. 执行 SQL SELECT holder_thread, reentry_count FROM lock_table WHERE lock_name =? FOR UPDATE，查询是否存在该记录：
   - 如果记录存在，且持有者是同一个线程，且可重入数大于 1 ，则减少重入次数 UPDATE lock_table SET reentry_count = reentry_count - 1 WHERE lock_name =?
   - 如果记录存在，且持有者是同一个线程，且可重入数小于等于 0 ，则完全释放锁，DELETE FROM lock_table WHERE lock_name =?
3. 提交事务
