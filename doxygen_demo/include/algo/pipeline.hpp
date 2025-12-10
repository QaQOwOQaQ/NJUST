#pragma once
#include <concepts>
#include <functional>
#include <memory>
#include <sstream>
#include <tuple>
#include <vector>

#include "core/logger.hpp"
#include "core/node.hpp"
#include "io/reader.hpp"

/**
 * @file
 * @brief 可组合管线
 * @ingroup Algo
 */

namespace demo::algo {
using demo::core::Add;
using demo::core::Literal;
using demo::core::Logger;
using demo::core::Mul;
using demo::core::Node;
using demo::io::Reader;

/** @brief 阶段必须能把 int -> int */
template <class F>
concept Stage = requires(F f, int x) {
    { f(x) } -> std::convertible_to<int>;
};

/**
 * @brief 可组合的整数处理管线
 * @tparam S 可变参数阶段（满足 Stage）
 */
template <Stage... S>
class Pipeline {
   public:
    explicit Pipeline(S... s) : stages_{std::move(s)...} {}

    /**
     * @brief 从 Reader 读取、经过各阶段，构建 AST 并求值
     *
     * @dot
     * digraph seq {
     *   rankdir=LR;
     *   node [shape=plaintext];
     *   main[label="run(path)"];
     *   pipe[label="Pipeline::run()"];
     *   reader[label="Reader::read_one()"];
     *   logger[label="Logger::info()"];
     *   main -> pipe -> reader;
     *   pipe -> logger [label="AST dump"];
     * }
     * @enddot
     */
    int run(Reader& r) {
        auto res = r.read_one();
        if (!res.has_value()) {
            Logger::instance().error("read failed: " + res.error());
            return 0;
        }
        int v = res.value();
        std::unique_ptr<Node> ast = std::make_unique<Literal>(v);
        build_ast<0>(ast);
        Logger::instance().info("AST = " + to_string(*ast));
        return ast->eval();
    }

   private:
    std::tuple<S...> stages_;

    template <std::size_t I, std::size_t N = sizeof...(S)>
    requires(I >= N) void build_ast(std::unique_ptr<Node>&) { /* 终止：无操作 */
    }

    template <std::size_t I, std::size_t N = sizeof...(S)>
    requires(I < N) void build_ast(std::unique_ptr<Node>& root) {
        auto& s = std::get<I>(stages_);
        int delta = s(1);  // 利用阶段计算常量增量
        if constexpr (I % 2 == 0) {
            root = std::make_unique<Add>(std::move(root),
                                         std::make_unique<Literal>(delta));
        } else {
            std::shared_ptr<Node> left(std::move(root));
            root =
                std::make_unique<Mul>(left, std::make_shared<Literal>(delta));
        }
        build_ast<I + 1, N>(root);
    }

    static std::string to_string(const Node& n) {
        std::ostringstream os;
        os << n;
        return os.str();
    }
};

}  // namespace demo::algo
