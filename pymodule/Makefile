SHELL = /bin/bash
CXX = clang++
CXXFLAGS = -Wall

evolv.so: pymodule.cc
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ pymodule.cc

clean:
	@rm -f evolv.so
