CXXFLAGS = -Iinclude -O2 -g -std=c++11 -Wall -fmessage-length=0

OBJS = 	src/datastructures/*.cpp \
		src/datastructures/components/*.cpp \
		src/datastructures/attributes/*.cpp \
		src/io/*.cpp \
		src/measures/*.cpp \
		src/community/*.cpp \
		src/evolution/*.cpp \
		src/exceptions/*.cpp \
		src/utils/*.cpp \
		
TEST = test/*.cpp
		
LIBS = -L/usr/local/lib

unit_test: test/test_all.cpp $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) -o runtest $(OBJS) $(TEST) $(LIBS)

#lib: $(OBJS)
#	$(CXX) $(CXXFLAGS) -c -fpic test/test.cpp $(OBJS) $(LIBS); $(CXX) $(CXXFLAGS) -o multiplenetwork.o *.o

#aucs:	aucs/aucs.cpp $(OBJS)
#	$(CXX) $(CXXFLAGS) -o mnet aucs/aucs.cpp $(OBJS) $(LIBS)

lib: $(OBJS)
	$(CXX) -c -fPIC  $(CXXFLAGS) $(OBJS)  
	$(CXX) $(LIBS) -stdlib=libc++ -shared -o libmultiplenetwork.so *.o 
	rm *.o
	 
all:	unit_test 

clean:
	rm *.o
	
