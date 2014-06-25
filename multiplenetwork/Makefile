CXXFLAGS = -Iinclude -O2 -g -std=c++11 -Wall -fmessage-length=0

OBJS = 	src/datastructures/*.cpp \
		src/exceptions/*.cpp \
		src/utils/*.cpp \
		src/io/*.cpp \
		src/transformation/*.cpp \
		src/evolution/*.cpp \
		src/measures/degree.cpp src/measures/neighborhood.cpp \
		src/measures/relevance.cpp src/measures/network_comparison.cpp \
		
TEST = test/FailedUnitTestException.cpp test/run_all.cpp test/testNetwork.cpp \
	   test/testMultilayerNetwork.cpp test/testMultiplex.cpp test/testIO.cpp test/testMeasures.cpp  \
	   test/testTransformations.cpp test/testRandom.cpp test/testEvolution.cpp \
		
LIBS = -L/usr/local/lib

unit_test: test/run_all.cpp $(OBJS) $(TEST)
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
	
