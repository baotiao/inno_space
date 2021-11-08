
CXX = g++
CXXFLAGS = -Wall -W -DDEBUG -g -O2
OBJECT = inno
SRC_DIR = src

LIB_PATH = -L./
LIBS =

INCLUDE_PATH = -I./

.PHONY: all clean

BASE_BOJS := $(wildcard $(SRC_DIR)/*.cc)
BASE_BOJS += $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst %.cc,%.o,$(BASE_BOJS))

all: $(OBJECT)
	rm $(SRC_DIR)/*.o

$(OBJECT): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(INCLUDE_PATH) $(LIB_PATH) $(LIBS)

$(OBJS) : %.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE_PATH)

clean:
	rm -rf $(OBJECT) ./a.out
