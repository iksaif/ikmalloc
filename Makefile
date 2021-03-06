NAME	= libikmalloc.so

CC	?= gcc

INCDIRS	+=
LDLIBS	+=

HEADERS =	malloc.h
SRCS	=	malloc.c realloc.c calloc.c free.c chunk.c chunk_give_back.c \
		chunk_fusion.c chunk_split.c chunk_size.c chunk_new.c \
		chains_append.c chains_insert.c chains_remove.c chunk_search.c \
		valloc.c memalign.c posix_memalign.c

OBJS	=	$(SRCS:.c=.o)

CFLAGS	?= -O2 -ggdb3
CFLAGS	+= -std=c99 -fPIC
CFLAGS	+= -Wall -W -Wstrict-prototypes -pedantic -Werror -D_GNU_SOURCE -DMALLOC_DEBUG
LFLAGS	+= -shared -lpthread

.PHONY		:	all clean fclean re
.SUFFIXES	:	.c .o

$(NAME)	:	$(OBJS) Makefile
	$(CC) $(CFLAGS) $(LDLIBS) -o $(NAME) $(OBJS) $(LFLAGS)

.c.o	:	$(HEADERS)
	$(CC) $(INCDIRS) $(CFLAGS) -c $*.c

clean	:
	find . \( -name "*.o" -o -name "*~" -o -name "#*#" \) -exec rm {} \;

re	:	mrproper all

mrproper:	clean
	rm -f $(NAME)

all	:	$(NAME)
