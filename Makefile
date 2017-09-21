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
LIBDIR = ${CURDIR}/lib
# List of source files
SOURCES = $(shell find src -name '*.cpp')
# Object files to be generated
OBJ = $(SOURCES:src/%.cpp=$(BUILDDIR)/%.o) $(BUILDDIR)/Tokens.o $(BUILDDIR)/y.tab.o
# objects that contain main() funtions
MAINS = $(BUILDDIR)/wiseyrun.o $(BUILDDIR)/wiseyc.o $(BUILDDIR)/wiseylibc.o
# Objects except the object files that contain main functions
OBJEXCEPTMAINS = $(filter-out $(MAINS), $(OBJ))
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
	-std=c++11 -g -fno-exceptions -fno-rtti \
	-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -c
# Flags used for linking
LDFLAGS = `llvm-config --ldflags --system-libs --libs engine` -L$(LIBDIR)

default: ${BINDIR}/wiseyc

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

$(BUILDDIR)/%.o: ${SRCDIR}/%.cpp ${INCLUDEDIR}/wisey/%.hpp | ${PARSERDIR}/Tokens.cpp ${BUILDDIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

${BINDIR}/wiseyc: $(OBJEXCEPTMAINS) ${BUILDDIR}/wiseyc.o | ${BINDIR}
	$(LD) -o $@ $(LDFLAGS) $^

${BINDIR}/wiseylibc: $(OBJEXCEPTMAINS) ${BUILDDIR}/wiseylibc.o | ${BINDIR}
	$(LD) -o $@ $(LDFLAGS) $^

${BINDIR}/runtests: ${TESTOBJ} $(OBJEXCEPTMAINS) | ${BINDIR} ${BINDIR}/wiseyc
	$(CC) -o ${BINDIR}/runtests $(LDFLAGS) -lgtest -lgmock $^
