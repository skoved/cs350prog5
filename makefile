OBJ:= ssfs.o superblock.o diskop.o
SRC:= ssfs.cpp superblock.cpp diskop.cpp superblock.hpp
SRC2:= ssfs_mkdsk.cpp superblock.cpp superblock.hpp
OBJ2:= ssfs_mkdsk.o superblock.o
CXX:= g++
CXXFLAGS:= -Wall -std=c++14

ssfs_mkdsk: $(OBJ2)
	$(CXX) $(CXXFLAGS) $(OBJ) -o ssfs_mkdsk

ssfs: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o ssfs

ssfs_mkdsk.o: $(SRC2)
	$(CXX) $(CXXFLAGS) -c -g $(SRC)

ssfs.o: $(SRC)
	$(CXX) $(CXXFLAGS) -c -g $(SRC)
superblock.o: superblock.cpp
	$(CXX) $(CXXFLAGS) -c -g superblock.cpp
diskop.o: diskop.cpp
	$(CXX) $(CXXFLAGS) -c -g diskop.cpp
clean: ssfs
	rm $(OBJ) $(OBJ2) ssfs ssfs_mkdsk
