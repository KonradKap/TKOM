# -Wall == all warnings enabled
#  # -Werror == treat warnings as ERRORS!
TARGET = kappalang
CXX = c++
CUR_DIR = $(shell pwd)
CFLAGS = -std=c++17 -Wall -O0 -g
INCLUDES =  -I"$(CUR_DIR)/src" -I"$(CUR_DIR)/tests"
LINKFLAGS = -std=c++17 -lboost_program_options -lboost_filesystem -lboost_system -lstdc++

app_SOURCES=$(shell find src/ -name *.cpp)
app_OBJECTS=$(app_SOURCES:%.cpp=bin/%.o)

tests_SOURCES=$(shell find tests/ src/ -name *.cpp -not -name main.cpp)
tests_OBJECTS=$(tests_SOURCES:%.cpp=bin/%.o)
tests_EXECUTABLE = bin/tests_bin

#DEPS := $(shell find . -name *.d)
DEPS := $(app_OBJECTS:.o=.d) $(tests_OBJECTS:.o=.d)

all: directories $(TARGET) tests

-include $(DEPS)

directories:
	@mkdir -p bin

bin/%.d : %.cpp
	@$(CXX) $(CFLAGS) -MM -MT '$(patsubst %.cpp,bin/%.o,$<)' $< -MF $@ $(INCLUDES)

bin/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) -c $(CFLAGS) -o $@ $< $(INCLUDES)

$(tests_EXECUTABLE): $(tests_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(INCLUDES)

tests: $(tests_EXECUTABLE)
	@./$(tests_EXECUTABLE)

run: $(TARGET)
	@./$(TARGET)

$(TARGET): $(app_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) -o $@ $^ $(LINKFLAGS) $(INCLUDES)

clean:
	@find . -name "*.o" -delete -or -name "*.d" -delete || true
	@find bin -type d -empty -delete || true

cleaner: clean
	@rm $(TARGET) $(tests_EXECUTABLE) || true

.PHONY: clean cleaner tests directories run
