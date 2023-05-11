CC=g++
CFLAGS=-std=c++14 -Wall -lboost_system
SRCS = main.cpp ../Utils/matrix_init.cpp ../Utils/general.cpp

TARGET = main
OBJS = $(subst .cpp,.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)