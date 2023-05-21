CC=g++
CFLAGS= -std=c++14 -Wall -lboost_system
LDFLAGS= -L$(LIBRARY_PATH) -lOpenCL
SRCS=main.cpp ../../Utils/matrix_init.cpp ../../Utils/general.cpp ../Utils/opencl_general.cpp ../Utils/opencl_params.cpp
OBJS = $(subst .cpp,.o,$(SRCS))

ifdef INCLUDE_PATH
CFLAGS += -I$(INCLUDE_PATH)
endif

ifdef LIBRARY_PATH
LDFLAGS= -L$(LIBRARY_PATH) -lOpenCL
endif

all: main

main: $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@ 

%.o: %.cpp
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f main $(OBJS)