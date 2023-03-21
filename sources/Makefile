#LIBGSL = -lgsl -lgslcblas # to be commented out if no GSL are installed
#LIBLAPACK = -llapack -lblas # to be commented out if no LAPACK are installed

#CDEFINE = -DLAPACK -DGSL

SHELL=/bin/tcsh

SYSTEM  = $(shell uname)

BINDIR  = ../bin
OBJDIR  = ../objs

OUT     =  $(BINDIR)/Cells.$(SYSTEM)

ifeq "$(SYSTEM)" "Linux"
CC 	= gcc
FC	= g77
LFLAGS	= -lm $(LIBGSL) $(LIBLAPACK)
endif

# Remember: -g with -O not allowed 
CFLAGS  = -O
FFLAGS  = -O

CSOURCES = $(wildcard *.c)
SOURCES  = $(CSOURCES)
ALLH     = $(wildcard *.h)

COBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(CSOURCES))

OBJS = $(COBJS)

all: $(OUT)

# use $(FC) instead of $(CC) because of lapack anomaly 
$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@;
	etags *.[ch];
	./Store

$(OBJS): $(ALLH) Makefile

# compiling procedures
$(COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CDEFINE) $< -o $@

.PHONY: clean

clean:
	rm $(OBJS) $(OUT)
