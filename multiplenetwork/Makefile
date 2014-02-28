CXXFLAGS = -Iinclude -O2 -g -Wall -fmessage-length=0

OBJS = 	test/*.cpp \
		src/datastructures/*.cpp \
		src/measures/*.cpp \
		src/utils/*.cpp \
		src/io/*.cpp \
		src/exceptions/*.cpp \
		src/community/*.cpp \
		
LIBS = -L/usr/local/lib

unit_test: test/run_all.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o runtest $(OBJS) $(LIBS)

#lib: $(OBJS)
#	$(CXX) $(CXXFLAGS) -c -fpic test/test.cpp $(OBJS) $(LIBS); $(CXX) $(CXXFLAGS) -o multiplenetwork.o *.o

#aucs:	aucs/aucs.cpp $(OBJS)
#	$(CXX) $(CXXFLAGS) -o mnet aucs/aucs.cpp $(OBJS) $(LIBS)

all:	unit_test 

clean:
	rm *.o
	
