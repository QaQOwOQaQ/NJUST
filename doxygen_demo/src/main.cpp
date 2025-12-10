#include "algo/pipeline.hpp"
#include "io/reader.hpp"
#include "util/scope_guard.hpp"
#include "core/logger.hpp"
#include <iostream>
#include <fstream>

using demo::algo::Pipeline;
using demo::io::make_file_reader;
using demo::core::Logger;
using demo::util::ScopeGuard;

/// @brief 业务主流程
int run(const std::string& path){
  auto reader = make_file_reader(path);
  auto cleanup = ScopeGuard([&]{ Logger::instance().info("cleanup done"); });

  // 三个阶段：+1、*2、+3（通过 lambda 提供策略）
  auto s1 = [](int x){ return x + 1; };
  auto s2 = [](int x){ return x * 2; };
  auto s3 = [](int x){ return x + 3; };

  Pipeline p{s1, s2, s3};
  Logger::instance().set_level(1);
  return p.run(*reader);
}

int main(){
  // 准备输入文件
  std::ofstream("input.txt") << 5;
  int v = run("input.txt");
  std::cout << "result = " << v << "\n";
  return 0;
}
