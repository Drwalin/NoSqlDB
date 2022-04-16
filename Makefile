
OBJECT_FILES = bin/CachedFile.o bin/HeapFile.o bin/TreeSetFile.o
OBJECT_FILES += bin/LinearAllocator.o
INCLUDES = -I/usr/include -Isrc
LIBS = -L/usr/lib -lboost_iostreams
CXXFLAGS = -m64 -std=c++2a -masm=intel -Ofast -DRELEASE_BUILD

rbtree_1: TestRedBlackTree.exe
	./TestRedBlackTree.exe

all: tree allocator heap linear cached

linear: TestLinearAllocator.exe
	./TestLinearAllocator.exe

tree: TestTreeSetFile.exe
	./TestTreeSetFile.exe

allocator: TestBlockAllocator.exe
	./TestBlockAllocator.exe

heap: TestHeapFile.exe
	./TestHeapFile.exe

cached: TestCachedFile.exe
	./TestCachedFile.exe

files_securere: $(OBJECT_FILES) bin/TestCachedFile.o

TestRedBlackTree.exe: bin/TestRedBlackTree.o
	g++ $^ -o $@ $(CXXFLAGS) $(LIBS) tests/Debug.cpp

%.exe: bin/%.o $(OBJECT_FILES)
	g++ $^ -o $@ $(CXXFLAGS) $(LIBS) tests/Debug.cpp

bin/%.o: src/%.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INCLUDES)
	
bin/%.o: tests/%.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INCLUDES)



