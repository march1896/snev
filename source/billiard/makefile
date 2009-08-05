# Makefile for this project, 
# Maintained by Tangel
#
#
# This is of cource not a good makefile, cause that the prerequisites are not include, for example, if vector.h is change, most of the files in these project should be 
# recompiled, but not. if a *.cpp file include a *.h file, we should add the .h file into the prerequisites
#

source := $(wildcard *.cpp)
objects := $(source:.cpp=.o)

billiard: $(objects)
	@echo building projects...
	g++ -o billiard $(objects)
%.d:%.cpp
	$(CXX) $(CPPFLAGS) -MM -MF $@ $<
-include $(objects:.o=.d)
.PHONY: clean
clean:
	rm *.o
