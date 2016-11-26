all: parser

clean:
	rm parser tokens.cpp y.tab.c y.tab.h *.o

y.tab.c: parser.ypp
	bison -d -o $@ $^

y.tab.h: y.tab.c

tokens.cpp: tokens.lpp y.tab.h
	flex -o $@ $^

objects: y.tab.c codegen.cpp main.cpp tokens.cpp
	g++ -I/usr/local/include  -fPIC -fvisibility-inlines-hidden -Wall -W -Wno-unused-parameter -Wwrite-strings -Wcast-qual -Wmissing-field-initializers -pedantic -Wno-long-long -Wcovered-switch-default -Wnon-virtual-dtor -Wdelete-non-virtual-dtor -Werror=date-time -std=c++11 -g  -fno-exceptions -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -c *.cpp *.c

parser: objects
	g++ -o $@ `llvm-config --ldflags` -o parser *.o `llvm-config --libs engine` `llvm-config --system-libs`
