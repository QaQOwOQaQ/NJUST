# compile option
DEFAULT_CPP_STANDARD = -std=c++20
FSANTILIZE = -fsanitize=address -fsanitize=undefined  
BENCHMARK_INC_DIR = -I/opt/
DIR = $(BENCHMARK_INC_DIR)
LIBURING = -luring # include liburing
PORT = 9190
IP = 127.0.0.1
TEST = # -fno-elide-constructors -O0

FLAGS = -Wall -Wextra -g $(DEFAULT_CPP_STANDARD) $(DIR) $(TEST) $(LIBURING)


# mkdir bin 
BIN_DIR = ./bin
$(shell mkdir -p $(BIN_DIR))

# universal rule to build any cpp file 
%: %.cpp
	g++ $< $(FLAGS) -o $(BIN_DIR)/$@
	$(BIN_DIR)/$@

# unique rule to build server and client
.PHONY: server
server: server.c
	gcc $< -o $(BIN_DIR)/$@
	$(BIN_DIR)/$@ $(PORT)

.PHONY: client
client: client.c
	gcc $< -o $(BIN_DIR)/$@
	$(BIN_DIR)/$@ $(IP) $(PORT)

# clean rule 
.PHONY: clean
clean:
	rm -rf *.i *.o *.s app a.out $(BIN_DIR)/* 

