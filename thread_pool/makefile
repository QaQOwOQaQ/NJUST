CXX = g++  # 如果你想用 clang，这里改成 clang++
CXXFLAGS = -std=c++23 -pthread -Wextra -Wall -Isrc # 加上 -Isrc 确保能找到头文件
TARGET_EXT = _app
TARGETS = v1 v2 v3 v4

.PHONY: all clean $(TARGETS)

all: $(TARGETS)

$(TARGETS): v%: src/test_v%.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $< -o bin/$@$(TARGET_EXT)
	./bin/$@$(TARGET_EXT)

clean:
	rm -rf bin/*