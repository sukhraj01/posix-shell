CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lreadline
TARGET = shell
SRCS = $(wildcard *.cpp)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
