WFLAGS = -O3 -mfpmath=sse -fstack-protector-all -g \
-W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors \
-Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute \
-Wformat=1 -Wwrite-strings -Wno-long-long -Woverloaded-virtual \
-Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format \
-std=c++14 -fno-exceptions

CPPFLAGS ?= -I01Ex -I02Ex -I03Ex -I04Ex -I05Ex -I06Ex -Itests
LINK_FLAGS ?=
PYTHON ?= python3
OBJECT_HEADER ?= tests/some_object.h

ifeq ($(OS),Windows_NT)
	EXE = exe
	CLEAN = del
else
	EXE = out
	CLEAN = rm -f
endif

TARGET = a.$(EXE)
PROBE = tests/elapsed_warning_probe.$(EXE)
OBJ = 01Ex/unique_pointer_object.o 02Ex/unique_pointer_array.o 03Ex/flexible_pointer.o \
04Ex/shared_pointer.o 05Ex/copiable_unique_pointer.o 06Ex/elapsed_warning.o
TEST_OBJ = tests/test_classes.o tests/some_object.o
HEADERS = 01Ex/unique_pointer_object.h 02Ex/unique_pointer_array.h 03Ex/flexible_pointer.h \
04Ex/shared_pointer.h 05Ex/copiable_unique_pointer.h 06Ex/elapsed_warning.h

all: $(TARGET) $(PROBE)

%.o: %.cpp
	g++ $(CPPFLAGS) $(WFLAGS) -c $< -o $@

$(TARGET): $(OBJ) $(TEST_OBJ)
	g++ $(WFLAGS) $(LINK_FLAGS) $^ -o $@

$(PROBE): 06Ex/elapsed_warning.o tests/elapsed_warning_probe.o
	g++ $(WFLAGS) $(LINK_FLAGS) $^ -o $@

$(OBJ) tests/test_classes.o tests/elapsed_warning_probe.o: $(HEADERS)

01Ex/unique_pointer_object.o 03Ex/flexible_pointer.o 04Ex/shared_pointer.o \
05Ex/copiable_unique_pointer.o $(TEST_OBJ): $(OBJECT_HEADER)

test: all
	$(PYTHON) tests/test_output.py $(TARGET) $(PROBE)

clean:
	$(CLEAN) $(OBJ) $(TEST_OBJ) tests/elapsed_warning_probe.o $(TARGET) $(PROBE)

.PHONY: all test clean
