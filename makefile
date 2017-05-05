OBJ:= ssfs.o  diskop.o controller.o inode.o
SRC:= ssfs.cpp diskop.cpp controller.cpp inode.cpp
SRC2:= ssfs_mkdsk.cpp 
OBJ2:= ssfs_mkdsk.o 
CXX:= g++
CXXFLAGS:= -Wall -std=c++14

all: ssfs_mkdsk ssfs

ssfs_mkdsk: $(OBJ2)
	$(CXX) $(CXXFLAGS) $(OBJ2) -o ssfs_mkdsk

ssfs: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o ssfs

ssfs_mkdsk.o: $(SRC2)
	$(CXX) $(CXXFLAGS) -c -g $(SRC2)

ssfs.o: $(SRC)
	$(CXX) $(CXXFLAGS) -c -g $(SRC)
superblock.o: superblock.cpp
	$(CXX) $(CXXFLAGS) -c -g superblock.cpp
diskop.o: diskop.cpp
	$(CXX) $(CXXFLAGS) -c -g diskop.cpp
controller.o: controller.cpp
	$(CXX) $(CXXFLAGS) -c -g controller.cpp
inode.o: inode.cpp
	$(CXX) $(CXXFLAGS) -c -g inode.cpp
clean: 
	rm $(OBJ) $(OBJ2) ssfs ssfs_mkdsk *.ssfs
test:
	make;
	./ssfs_mkdsk 1024 128 disk.txt;
	./ssfs disk.txt thing;
