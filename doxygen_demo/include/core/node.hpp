#pragma once
#include <memory>
#include <variant>
#include <ostream>
#include <string>
#include <vector>

/**
 * @file
 * @brief AST 节点层次
 * @ingroup Core
 */

namespace demo::core {

/// @brief 节点种类
enum class Kind { Literal, Add, Mul };

struct Node {
  virtual ~Node() = default;
  virtual Kind kind() const noexcept = 0;
  virtual int  eval() const = 0;    ///< 计算值
  virtual void dump(std::ostream&) const = 0;
};

struct Literal final : Node {
  int value;
  explicit constexpr Literal(int v) : value(v) {}
  Kind kind() const noexcept override { return Kind::Literal; }
  int eval() const override { return value; }
  void dump(std::ostream& os) const override { os << value; }
};

struct Add final : Node {
  std::unique_ptr<Node> a, b;
  Add(std::unique_ptr<Node> x, std::unique_ptr<Node> y): a(std::move(x)), b(std::move(y)) {}
  Kind kind() const noexcept override { return Kind::Add; }
  int eval() const override { return a->eval() + b->eval(); }
  void dump(std::ostream& os) const override { os << "("; a->dump(os); os << " + "; b->dump(os); os << ")"; }
};

struct Mul final : Node {
  std::shared_ptr<Node> a, b; // 故意与 Add 用不同智能指针
  Mul(std::shared_ptr<Node> x, std::shared_ptr<Node> y): a(std::move(x)), b(std::move(y)) {}
  Kind kind() const noexcept override { return Kind::Mul; }
  int eval() const override { return a->eval() * b->eval(); }
  void dump(std::ostream& os) const override { os << "("; a->dump(os); os << " * "; b->dump(os); os << ")"; }
};

inline std::ostream& operator<<(std::ostream& os, const Node& n){ n.dump(os); return os; }

} // namespace demo::core
