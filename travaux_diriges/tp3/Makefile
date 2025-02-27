CC = gcc
CXX = g++
MPICXX = mpic++

CXXFLAGS = -std=c++17 -O3 -march=native -Wall


ALL= bucket_sort

default:	help

all: $(ALL)

clean:
	@rm -fr *.o *.exe *~ $(ALL)

.cpp.o:
	$(MPICXX) $(CXXFLAGS) -c $^ -o $@	

bucket_sort: bucket_sort.o
	$(MPICXX) $(CXXFLAGS) $^ -o $@

help:
	@echo "Available targets : "
	@echo "    all            : compile all executables"
	@echo "Add DEBUG=yes to compile in debug"
	@echo "Configuration :"
	@echo "    CXX      :    $(CXX)"
	@echo "    CXXFLAGS :    $(CXXFLAGS)"

%.html: %.md
	pandoc -s --toc $< --css=./github-pandoc.css --metadata pagetitle="OS202 - TD3" -o $@
