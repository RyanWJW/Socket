TARGET := server client
CXXFLAGS := -g -pthread
cxx := g++
all:$(TARGET)

server: server.cpp
client: client.cpp
	$(cxx) $(CXXFLAGS) -o server server.cpp
	$(cxx) $(CXXFLAGS) -o client client.cpp

