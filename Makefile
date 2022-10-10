#GLADE_SRCS = $(wildcard res/*.glade)
#GLADE_OBJS = ${GLADE_SRCS:.glade=.o}
SRCS = $(wildcard src/*.cpp)
OBJS = ${SRCS:.cpp=.o}
all: ${OBJS}
	g++ ${OBJS} -o ichess -lchess -g -lboost_program_options
	rm -f ${OBJS}
clean:
	rm -f ${OBJS} ${GLADE_OBJS} ichess
#%.o: %.glade
#	objcopy -I binary -O elf64-x86-64 $< $@
%.o: %.cpp
	g++ -c $< -o $@ -Iinclude `pkg-config gtk+-3.0 --cflags` -g