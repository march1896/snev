# Makefile for this project, 
# Maintained by Tangel

source := $(wildcard *.cpp)
objects := $(source:.cpp=.o)
# I found it is hard to move *.o files to a specific folder, 
# It's hard to generate *.d file, and when *.d file is included, *.o file is been fixed

# CPPFLAGS += -D_MSC_VER
# LDFLAGS := -L"C:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\"
LDFLAGS := -L"C:\\MinGW\\lib"
LDFLAGS += -lwinmm -lgdi32 -lopengl32 -lglu32 -lglaux -lglew32

billiard: $(objects)
	@echo building projects...
	$(CXX) -o billiard.exe $(objects) $(LDFLAGS)
	@del -f *.d
%.d:%.cpp
	@$(CXX) $(CPPFLAGS) -MM -MF $@ $<
-include $(objects:.o=.d)
.PHONY: clean
clean:
	@echo removing *.o and *.d files
	@del -f *.d
	@del -f *.o
