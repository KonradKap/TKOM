# -Wall == all warnings enabled
#  # -Werror == treat warnings as ERRORS!
TARGET = kappalang
CXX = c++
CUR_DIR = $(shell pwd)
CPPFLAGS = -std=c++17 -Wall -O0 -g
INCLUDES =  -I"$(CUR_DIR)/app" -I"$(CUR_DIR)/lib" -I"$(CUR_DIR)/tests"
LINKFLAGS = --std=c++17
APPLFLAGS = -lboost_program_options
TESTLFLAGS =

app_SOURCES=$(shell find app/ -name *.cpp)
app_OBJECTS=$(app_SOURCES:app/%.cpp=bin/app/%.o)

lib_SOURCES=$(shell find lib/ -name *.cpp)
lib_OBJECTS=$(lib_SOURCES:lib/%.cpp=bin/lib/%.o)

tests_SOURCES=$(shell find tests/ -name *.cpp)
tests_OBJECTS=$(tests_SOURCES:tests/%.cpp=bin/tests/%.o)
tests_EXECUTABLE = bin/tests_bin

#DEPS := $(shell find . -name *.d)
DEPS := $(app_OBJECTS:.o=.d) $(lib_OBJECTS:.o=.d) $(tests_OBJECTS:.o=.d)

all: directories $(TARGET) tests

-include $(DEPS)

directories:
	@mkdir -p bin

bin/%.d : %.cpp
	@$(CXX) $(CPPFLAGS) -MM -MT '$(patsubst %.cpp,bin/%.o,$<)' $< -MF $@ $(INCLUDES)

bin/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CPPFLAGS) -o $@ $< $(INCLUDES)

$(tests_EXECUTABLE): $(tests_OBJECTS) $(lib_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(tests_OBJECTS) $(lib_OBJECTS) $(LINKFLAGS) $(TESTLFLAGS) $(INCLUDES)

tests: $(tests_EXECUTABLE)
	@./$(tests_EXECUTABLE)

run: $(TARGET)
	@./$(TARGET)

$(TARGET): $(app_OBJECTS) $(lib_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(app_OBJECTS) $(lib_OBJECTS) $(LINKFLAGS) $(APPLFLAGS) $(INCLUDES)

clean:
	@find . -name "*.o" -delete -or -name "*.d" -delete
	@find bin -type d -empty -delete

cleaner:
	@rm -rf bin

.PHONY: clean cleaner tests directories run
.SECONDARY: $(lib_OBJECTS) $(app_SHARED)
