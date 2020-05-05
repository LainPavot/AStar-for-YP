
DEBUG		=		0

RM 			=		rm -f

CC			=		gcc

CFLAGS		+=	-Wextra -Wall -Werror -Wpedantic -Wshadow -fPIC
CFLAGS		+=	-pedantic
CFLAGS		+=	-I. -I/usr/include/python2.7 -lpython2.7 `/usr/bin/python2.7-config --cflags`
# CFLAGS		+=	-I. `/usr/bin/python2.7-config --cflags`

LDFLAGS		+=	-lm `/usr/bin/python2.7-config --ldflags`

BINDIR		=		.
TARGET		=		a_star.so
TEST		=		test_a_star
DEPENDS		=		chained_list.o

SRCDIR		=		.
SRCS		=		a_star.c

OBJS		=		$(SRCS:.c=.o)
GCH			=		$(SRCS:.c=.h.gch)


ifeq ($(DEBUG),1)
	CFLAGS	+=	-g
endif


## $@					= nom de la regle
## $^					= dependances de la regle
## $<					= 1er dependance de la regle
## $?					= dependances plus recentes que la cible
## $*					= nom du fichier (sans extension)
## @					= devant une command -> devient silentieuse
## -					= devant une command -> ignore les erreurs
## %.o	:	%.c	= regle generique aux X.o depend de X.c

##  CIBLE [CIBLE ...]	:	[PENDANCE ...]
##  				COMMANDE1
##  				COMMANDE2



all						:		$(TARGET) $(TEST)


$(TARGET)			:		a_star.o
			@echo building $(TARGET)
			@$(CC) a_star.o -shared -o $(BINDIR)/$@ $(DEPENDS) $(LDFLAGS) $(CFLAGS)
			@echo moving $(TARGET) to toplevel
			@mv ./$(TARGET) ..

$(TEST)				:		$(addprefix $(BINDIR)/, $(OBJS))
			@echo building $(TEST)
			@$(CC) $(TEST).c -o $(BINDIR)/$@ $^ $(DEPENDS) $(LDFLAGS) $(CFLAGS)



$(BINDIR)/%.o	:		$(SRCDIR)/%.c
			@echo compiling $(?)
			@$(CC) -c $? -o $@ $(CFLAGS)


clean	:
			@echo removing object files
			@$(RM) $(addprefix $(BINDIR)/,$(OBJS))
			@echo removing gch files
			@$(RM) $(addprefix $(SRCDIR)/,$(GCH))


fclean				:		clean
			@echo removing $(BINDIR)/$(TARGET)
			@$(RM) $(BINDIR)/$(TARGET)
			@echo removing $(BINDIR)/$(TEST)
			@$(RM) $(BINDIR)/$(TEST)


re						:		fclean all


.PHONY				:		all clean fclean re

