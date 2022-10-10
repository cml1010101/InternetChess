GLADE_SRCS = $(wildcard res/*.glade)
GLADE_OBJS = ${GLADE_SRCS:.glade=.o}
SVG_SRCS = $(wildcard res/*.svg)
SVG_OBJS = ${SVG_SRCS:.svg=.o}
SRCS = $(wildcard src/*.cpp)
OBJS = ${SRCS:.cpp=.o}
all: ${OBJS} ${GLADE_OBJS} ${SVG_OBJS}
	g++ ${OBJS} ${GLADE_OBJS} ${SVG_OBJS} -o ichess -lchess -g -lboost_program_options
	rm -f ${OBJS} ${GLADE_OBJS} ${SVG_OBJS}
clean:
	rm -f ${OBJS} ${GLADE_OBJS} ${SVG_OBJS} ichess
%.o: %.glade
	objcopy -I binary -O elf64-x86-64 $< $@
%.o: %.svg
	objcopy -I binary -O elf64-x86-64 $< $@
%.o: %.cpp
	g++ -c $< -o $@ -Iinclude `pkg-config gtk+-3.0 --cflags` -g