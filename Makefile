# Compiler used for compiling into object files
CC = g++
# Linker used for linking into binary executable
LD = g++
# Executable output directory
BINDIR = bin
# Directory for temporary output of lex/yacc files
PARSERDIR = parser
# Directory for object files
ODIR = obj
# Source file directory
SRCDIR = ${CURDIR}/src
# System header files that include the headers for LLVM
ISYSTEMDIR = /usr/local/include
# Yazyk header files
INCLUDEDIR = ${CURDIR}/include
# List of source files
SOURCES = $(shell find src -name '*.cpp')
# Object files to be generated
OBJ=$(SOURCES:src/%.cpp=$(ODIR)/%.o) obj/tokens.o obj/y.tab.o
# Flags used for compilation step
CFLAGS = -fPIC -fvisibility-inlines-hidden -Wall -W \
	-Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers \
	-pedantic -Wno-long-long -Wcovered-switch-default -Wnon-virtual-dtor -Wno-deprecated-register \
	-Wno-unneeded-internal-declaration -Wdelete-non-virtual-dtor -Werror=date-time \
	-std=c++11 -g -fno-exceptions \
	-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -c
# Flags used for linking
LDFLAGS = `llvm-config --ldflags` `llvm-config --libs engine` `llvm-config --system-libs`

default: ${BINDIR}/yazyk

clean:
	rm -rf ${BINDIR} ${ODIR} ${PARSERDIR}

${ODIR}:
	mkdir -p ${ODIR}

${PARSERDIR}:
	mkdir -p ${PARSERDIR}

${BINDIR}:
	mkdir -p ${BINDIR}

${PARSERDIR}/y.tab.c: ${SRCDIR}/parser.ypp | ${PARSERDIR}
	bison -d -o $@ $<

${PARSERDIR}/y.tab.h: ${PARSERDIR}/y.tab.c | ${PARSERDIR}

${PARSERDIR}/tokens.cpp: ${PARSERDIR}/y.tab.h | ${PARSERDIR}
	flex -o $@ ${SRCDIR}/tokens.lpp

$(ODIR)/y.tab.o: ${PARSERDIR}/y.tab.c | ${ODIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} $(CFLAGS) $<

$(ODIR)/tokens.o: ${PARSERDIR}/tokens.cpp | ${ODIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} $(CFLAGS) $<

$(ODIR)/%.o: ${SRCDIR}/%.cpp | ${PARSERDIR}/tokens.cpp ${ODIR}
	$(CC) -o $@ -I$(ISYSTEMDIR) -I${INCLUDEDIR} -I${PARSERDIR} $(CFLAGS) $< 

${BINDIR}/yazyk: $(OBJ) | ${BINDIR}
	$(LD) -o $@ $(LDFLAGS) $^
