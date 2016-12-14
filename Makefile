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
# Yazyk header files
INCLUDEDIR = ${CURDIR}/include
# Tests header files
TESTINCLUDEDIR = ${CURDIR}/tests/include
# System lib directory
LIBDIR = ${CURDIR}/lib
# List of source files
SOURCES = $(shell find src -name '*.cpp')
# Object files to be generated
OBJ = $(SOURCES:src/%.cpp=$(BUILDDIR)/%.o) $(BUILDDIR)/tokens.o $(BUILDDIR)/y.tab.o
# file name that contains the main() function
MAIN = main 
# Objects except the main
OBJEXCEPTMAIN = $(filter-out $(BUILDDIR)/main.o, $(OBJ))
# Test directory
TESTDIR = tests
# Test sources
TESTSOURCES = $(shell find tests -name 'Test*.cpp')
# Test objects
TESTOBJ=$(TESTSOURCES:tests/%.cpp=$(BUILDDIR)/%.o)
# Flags used for compilation step
CFLAGS = -fPIC -fvisibility-inlines-hidden -Wall -W \
	-Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers \
	-pedantic -Wno-long-long -Wcovered-switch-default -Wnon-virtual-dtor -Wno-deprecated-register \
	-Wno-unneeded-internal-declaration -Wdelete-non-virtual-dtor -Werror=date-time \
	-std=c++11 -g -fno-exceptions \
	-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -c
# Flags used for linking
LDFLAGS = `llvm-config --ldflags` `llvm-config --libs engine` `llvm-config --system-libs` -L$(LIBDIR)

default: ${BINDIR}/yazyk

clean:
	rm -rf ${BINDIR} ${BUILDDIR} ${PARSERDIR}

tests: ${BINDIR}/runtests

${BUILDDIR}:
	mkdir -p ${BUILDDIR}

${PARSERDIR}:
	mkdir -p ${PARSERDIR}

${BINDIR}:
	mkdir -p ${BINDIR}

${PARSERDIR}/y.tab.c: ${SRCDIR}/parser.ypp | ${PARSERDIR}
	bison -d -o $@ $<

${PARSERDIR}/y.tab.h: ${PARSERDIR}/y.tab.c | ${PARSERDIR}

${PARSERDIR}/tokens.cpp: ${PARSERDIR}/y.tab.h | ${PARSERDIR}
	flex -o $@ ${SRCDIR}/tokens.lpp

$(BUILDDIR)/Test%.o: ${TESTDIR}/Test%.cpp | ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} -I${TESTINCLUDEDIR} $(CFLAGS) -Wno-covered-switch-default $<

$(BUILDDIR)/y.tab.o: ${PARSERDIR}/y.tab.c | ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} $(CFLAGS) $<

$(BUILDDIR)/tokens.o: ${PARSERDIR}/tokens.cpp | ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} $(CFLAGS) $<

$(BUILDDIR)/%.o: ${SRCDIR}/%.cpp | ${PARSERDIR}/tokens.cpp ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

${BINDIR}/yazyk: $(OBJ) | ${BINDIR}
	$(LD) -o $@ $(LDFLAGS) $^

${BINDIR}/runtests: ${TESTOBJ} $(OBJEXCEPTMAIN) | ${BINDIR}
	$(CC) -o ${BINDIR}/runtests $(LDFLAGS) -lgtest -lgmock $^
