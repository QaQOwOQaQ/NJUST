#include "io/reader.hpp"
#include <fstream>
#include <sstream>

namespace demo::io {

namespace {
struct FileReader : Reader {
  explicit FileReader(std::string path): path_(std::move(path)) {}
  Result<int> read_one() override {
    std::ifstream ifs(path_);
    if (!ifs) return Result<int>::err("open failed");
    int x{};
    ifs >> x;
    if (!ifs) return Result<int>::err("parse failed");
    return Result<int>::ok(x);
  }
  std::string path_;
};
}

std::unique_ptr<Reader> make_file_reader(const std::string& path) {
  return std::make_unique<FileReader>(path);
}

} // namespace demo::io
