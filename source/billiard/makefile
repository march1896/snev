# Makefile for this project, 
# Maintained by Tangel

source := $(wildcard *.cpp)
objects := $(source:.cpp=.o)
# I found it is hard to move *.o files to a specific folder, 
# It's hard to generate *.d file, and when *.d file is included, *.o file is been fixed

billiard: $(objects)
	@echo building projects...
	@g++ -o billiard $(objects)
	@rm *.d
%.d:%.cpp
	@$(CXX) $(CPPFLAGS) -MM -MF $@ $<
-include $(objects:.o=.d)
.PHONY: clean
clean:
	@echo removing .o files
	@rm -f *.o
