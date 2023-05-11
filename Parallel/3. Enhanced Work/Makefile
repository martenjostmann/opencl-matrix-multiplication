CC=g++
INCLUDE_PATH="C:\Users\marte\vcpkg\packages\opencl_x64-windows\include"
LIBRARY_PATH="C:\Users\marte\vcpkg\packages\opencl_x64-windows\lib"
CFLAGS= -std=c++14 -Wall -lboost_system -I$(INCLUDE_PATH)
LDFLAGS= -L$(LIBRARY_PATH) -lOpenCL

SRCS=main.cpp ../../Utils/matrix_init.cpp ../../Utils/general.cpp ../Utils/opencl_general.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

all: main

main: $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@ 

%.o: %.cpp
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f main $(OBJS)