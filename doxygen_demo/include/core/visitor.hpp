#pragma once
#include <string>

/**
 * @file
 * @brief 访问者（CRTP）
 * @ingroup Core
 */

namespace demo::core {

struct Add; struct Mul; struct Literal;

/** @brief 访问者接口（CRTP 便于静态分发） */
template<class Derived>
struct Visitor {
  void operator()(const Add& n)    { static_cast<Derived*>(this)->visit(n); }
  void operator()(const Mul& n)    { static_cast<Derived*>(this)->visit(n); }
  void operator()(const Literal& n){ static_cast<Derived*>(this)->visit(n); }
};

} // namespace demo::core
