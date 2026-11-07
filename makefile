FILE ?= examples/hello_world.bf
CXXFLAGS = -std=c++26 -freflection -O0 -g -DEMBED_FILE='"$(FILE)"'

main: main.cpp
	g++ $(CXXFLAGS) main.cpp -o main
