SHELL = /bin/bash
CXX = clang++
CXXFLAGS = -Wall

evolv.so: pymodule.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o $@ pymodule.cpp

clean:
	@rm -f evolv.so
