#pragma once
#include <string>
#include <memory>
#include "util/result.hpp"

/**
 * @file
 * @brief IO Reader 接口
 * @ingroup IO
 */

namespace demo::io {
using demo::util::Result;

/** @brief 把一行字符串解析为整数 */
struct Reader {
  virtual ~Reader() = default;
  virtual Result<int> read_one() = 0;
};

std::unique_ptr<Reader> make_file_reader(const std::string& path);

} // namespace demo::io
