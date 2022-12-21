
CXX = g++
CXXFLAGS = -Wall -W -DNDEBUG -g -O2 -std=c++11
OBJECT = inno
SRC_DIR = src

LIB_PATH = -L./
LIBS =

INCLUDE_PATH = -I./ \
							 -I./include/ \

.PHONY: all clean

BASE_BOJS := $(wildcard $(SRC_DIR)/*.cc)
BASE_BOJS += $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst %.cc,%.o,$(BASE_BOJS))

all: $(OBJECT)
	rm $(SRC_DIR)/*.o

$(OBJECT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE_PATH) $(LIB_PATH) $(LIBS)

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH)

clean:
	rm -rf $(OBJECT) ./a.out
	rm -rf $(SRC_DIR)/*.o
