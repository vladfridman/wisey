# Compiler used for compiling into object files
CC = g++
# Linker used for linking into binary executable
LD = g++ 
# Executable output directory
BINDIR = bin
# Directory for temporary output of lex/yacc files
PARSERDIR = parser
# Directory for object files and other files produced during build
BUILDDIR = build
# Source file directory
SRCDIR = ${CURDIR}/src
# System header files that include the headers for LLVM
ISYSTEMDIR = /usr/local/include
# Project header files
INCLUDEDIR = ${CURDIR}/include
# Tests header files
TESTINCLUDEDIR = ${CURDIR}/tests/include
# System lib directory
LIBDIR = ${CURDIR}/build
# List of source files
SOURCES = $(shell find src -name '*.cpp')
# Object files to be generated
OBJ = $(SOURCES:src/%.cpp=$(BUILDDIR)/%.o) $(BUILDDIR)/Tokens.o $(BUILDDIR)/y.tab.o
# objects that contain main() funtions
MAINS = $(BUILDDIR)/wiseyrun.o $(BUILDDIR)/wiseyc.o $(BUILDDIR)/wiseylibc.o $(BUILDDIR)/yzc.o
# Objects except the object files that contain main functions
OBJEXCEPTMAINS = $(filter-out $(MAINS), $(OBJ))
# Objects needed for compiler drivers
OBJECTSFORDRIVERS = $(BUILDDIR)/CompilerDriver.o $(BUILDDIR)/Log.o $(BUILDDIR)/WiseycArgumentParser.o $(BUILDDIR)/WiseylibcArgumentParser.o $(BUILDDIR)/CompilerArguments.o 
# Test directory
TESTDIR = tests
# Test sources
TESTSOURCES = $(shell find tests -name 'Test*.cpp')
# Test objects
TESTOBJ=$(TESTSOURCES:tests/%.cpp=$(BUILDDIR)/%.o)
# Flags used for compilation step
CFLAGS = -fPIC -fvisibility-inlines-hidden -Wall -W \
	-Wno-vla-extension -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers \
	-pedantic -Wno-long-long -Wnon-virtual-dtor -Wno-deprecated-register \
	-Wno-unneeded-internal-declaration -Wdelete-non-virtual-dtor -Werror=date-time \
	-std=c++11 -g -fno-rtti \
	-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -c
# Flags used for linking
LDFLAGS = `llvm-config --ldflags --system-libs --libs all`

# This flag is different on different OS
LDTESTFLAGS := -lgtest -lgmock 
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  LDTESTFLAGS += -Wl,--whole-archive -lwisey -Wl,--no-whole-archive
endif
ifeq ($(UNAME_S),Darwin)
  LDTESTFLAGS += -force_load build/libwisey.a
endif

default: ${BINDIR}/yzc

clean:
	rm -rf ${BINDIR} ${BUILDDIR} ${PARSERDIR}

tests: ${BINDIR}/runtests

${BUILDDIR}:
	mkdir -p ${BUILDDIR}

${PARSERDIR}:
	mkdir -p ${PARSERDIR}

${BINDIR}:
	mkdir -p ${BINDIR}

${PARSERDIR}/y.tab.c: ${SRCDIR}/Parser.ypp | ${PARSERDIR}
	bison -d -v -o $@ $<

${PARSERDIR}/y.tab.h: ${PARSERDIR}/y.tab.c | ${PARSERDIR}

${PARSERDIR}/Tokens.cpp: ${PARSERDIR}/y.tab.h | ${PARSERDIR}
	flex -o $@ ${SRCDIR}/Tokens.lpp

$(BUILDDIR)/Test%.o: ${TESTDIR}/Test%.cpp | ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} -I${TESTINCLUDEDIR} $(CFLAGS) -Wno-covered-switch-default $<

$(BUILDDIR)/y.tab.o: ${PARSERDIR}/y.tab.c | ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} $(CFLAGS) $<

$(BUILDDIR)/Tokens.o: ${PARSERDIR}/Tokens.cpp | ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} $(CFLAGS) $<

$(BUILDDIR)/wiseyc.o: ${SRCDIR}/wiseyc.cpp | ${PARSERDIR}/Tokens.cpp ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

$(BUILDDIR)/wiseylibc.o: ${SRCDIR}/wiseylibc.cpp | ${PARSERDIR}/Tokens.cpp ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

$(BUILDDIR)/yzc.o: ${SRCDIR}/yzc.cpp | ${PARSERDIR}/Tokens.cpp ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

$(BUILDDIR)/%.o: ${SRCDIR}/%.cpp ${INCLUDEDIR}/%.hpp | ${PARSERDIR}/Tokens.cpp ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

${BINDIR}/wiseyc: $(OBJECTSFORDRIVERS) ${BUILDDIR}/wiseyc.o | ${BINDIR}
	$(LD) -o $@ $^ $(LDFLAGS)

${BINDIR}/wiseylibc: $(OBJECTSFORDRIVERS) ${BUILDDIR}/wiseylibc.o | ${BINDIR}
	$(LD) -o $@ $^ $(LDFLAGS)

${BINDIR}/yzc: $(OBJEXCEPTMAINS) ${BUILDDIR}/yzc.o | ${BINDIR}
	$(LD) -o $@ $^ $(LDFLAGS)

${BINDIR}/runtests: ${TESTOBJ} $(OBJEXCEPTMAINS) | ${BINDIR} ${BINDIR}/yzc ${BINDIR}/wiseyc ${BINDIR}/wiseylibc
	$(LD) -o $@ $^ $(LDFLAGS) -L$(LIBDIR) ${LDTESTFLAGS}
