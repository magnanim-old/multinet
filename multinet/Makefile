CXXFLAGS = -Iinclude -O2 -g -Wall -fmessage-length=0 -std=gnu++11

OBJS = 	src/datastructures/*.cpp \
		src/datastructures/components/*.cpp \
		src/datastructures/attributes/*.cpp \
		src/randomwalks/*.cpp \
		src/io/*.cpp \
		src/measures/*.cpp \
		src/transformation/*.cpp \
		src/community/*.cpp \
		src/layout/*.cpp \
		src/dynamics/*.cpp \
		src/generation/*.cpp \
		src/exceptions/*.cpp \
		src/utils/*.cpp \

TEST = test/*.cpp

LIBS = -L/usr/local/lib

unit_test: test/main.cpp $(OBJS) $(TEST)
	$(CXX) $(CXXFLAGS) -o runtest $(OBJS) $(TEST) $(LIBS)

lib: $(OBJS)
	$(CXX) -c -fPIC  $(CXXFLAGS) $(OBJS)
	$(CXX) $(LIBS) -shared -o libmlnetwork.so *.o
	rm *.o

all: unit_test

clean:
	rm *.o

