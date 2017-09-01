CXX=g++ -std=c++11 -O2 -D_FILE_OFFSET_BITS=64
DEBUG = -g -DBOOST_SYSTEM_NO_DEPRECATED
RM=rm -f
CXXFLAGS=$(DEBUG) -Wall -Wsign-compare #-march=native
INCLUDES := -I/usr/local/include/boost/
LDFLAGS=$(DEBUG) -Wall -L/usr/local/lib/ -lboost_program_options -lboost_system -lboost_filesystem -lboost_regex -lboost_iostreams
LDLIBS=
EXECUTABLE=black-hole-clustering

SRCS=$(shell find . -maxdepth 1 -name '*.cpp' -print | sort)
OBJS=$(subst .cpp,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) -o $(EXECUTABLE) $(OBJS) $(LDLIBS) $(LDFLAGS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^>>./.depend;

clean:
	$(RM) *~ $(OBJS) $(EXECUTABLE)

dist-clean: clean
	$(RM) *~ .depend

include .depend
