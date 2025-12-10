# Demo 文档总览

本项目展示了一个含 **C++17/20** 特性的示例工程，包含命名空间、模板、concepts、继承、多态、PImpl、RAII、智能指针、管线模式、访问者等。

- **模块**：Core / IO / Algo / Util
- **命名空间**：`demo::core` / `demo::io` / `demo::algo` / `demo::util`

## 架构图（DOT）

@dot
digraph G {
  rankdir=LR;
  node [shape=box, fontsize=11];

  subgraph cluster_core {
    label = "Core";
    Logger; Node; Literal; Add; Mul;
  }
  subgraph cluster_io {
    label = "IO";
    Reader; FileReader;
  }
  subgraph cluster_algo {
    label = "Algo";
    Pipeline;
  }
  subgraph cluster_util {
    label = "Util";
    ScopeGuard; ResultT [label="Result<T>"];
  }

  Pipeline -> Reader    [label="read_one()"];
  Pipeline -> Logger    [label="info()/error()"];
  Pipeline -> Node      [label="build_ast()/eval()"];
  Reader  -> ResultT    [label="返回值"];
  Logger  -> "std::ostream";
  Add     -> Node;
  Mul     -> Node;
  Literal -> Node;
  ScopeGuard -> Logger  [style=dashed, label="清理日志"];
}
@enddot
