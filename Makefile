# compile option
GCC = g++-13
DEFAULT_CPP_STANDARD = -std=c++23
FSANTILIZE = -fsanitize=address -fsanitize=undefined  
BENCHMARK_INC_DIR = -I/opt/
DIR = $(BENCHMARK_INC_DIR)

TEST = -fno-elide-constructors -O0
TEST = 
FLAGS = -Wall -Wextra -g $(DEFAULT_CPP_STANDARD) $(DIR) $(TEST)

PORT = 9190
IP = 127.0.0.1

# bin directory
BIN_DIR = ./bin
$(shell mkdir -p $(BIN_DIR))

# universal rule to build any cpp file 
%: %.cpp
	$(GCC) $< $(FLAGS) -o $(BIN_DIR)/$@
	$(BIN_DIR)/$@

# unique rule to build server and client
.PHONY: server
server: server.c
	$(GCC) $< -o $(BIN_DIR)/$@
	$(BIN_DIR)/$@ $(PORT)

.PHONY: client
client: client.c
	$(GCC) $< -o $(BIN_DIR)/$@
	$(BIN_DIR)/$@ $(IP) $(PORT)

# clean rule 
.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/* *.i *.o *.s *.out app main 
