CC=g++
CFLAGS=-std=c++14 -Wall -lboost_system -L "C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib" -lOpenCL -I"C:\Users\marte\vcpkg\packages\opencl_x64-windows\include"
SRCS = main.cpp ../../Utils/matrix_init.cpp 

TARGET = main
OBJS = $(subst .cpp,.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)