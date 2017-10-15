CC       := gcc
PERL     := perl
PROF     := #-pg

# Flags
OPTFLAGS := -O0
DEFS     :=
#DEFS += -DNDEBUG
CPPFLAGS := -I../include
CFLAGS   := -Wall -g -I../include $(PROF) $(OPTFLAGS) $(DEFS)
LDFLAGS  := 

# Libraries
LDLIBS    := 
LOADLIBES := 


# Target names
TARGET    := profTest
LIBTARGET := libtprof.a

# Directories to build
OBJDIR := obj/
DEPDIR := dep/


# Source files
LIBSRCS  := cbProf.c
SRCS     := profTest.c

# Object files
LIBOBJS := $(addprefix $(OBJDIR),$(LIBSRCS:.c=.o))
OBJS    := $(addprefix $(OBJDIR),$(SRCS:.c=.o))



$(TARGET): $(OBJS) $(LIBTARGET)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(PROF) -o $@

$(LIBTARGET): $(LIBOBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

.PHONY: release
release:
	$(MAKE) DEFS=-DOPTIMIZATION_ON OPTFLAGS=-O3

.PHONY: test
test:
	$(MAKE) clean
	$(MAKE) release
	./profTest

.PHONY: prof
prof:
	$(MAKE) DEFS=-DOPTIMIZATION_ON OPTFLAGS=-O3 PROF=-pg

.PHONY: clean
clean:
	rm -f $(TARGET) $(LIBTARGET) $(OBJS) $(LIBOBJS) *.bak *~


# Include dependency files
include $(addprefix $(DEPDIR),$(SRCS:.c=.d))

$(DEPDIR)%.d : %.c
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed "s@$*.o@$(OBJDIR)& $@@g " > $@'

$(OBJDIR)%.o: %.c $(DEPDIR)%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(OBJDIR)%.o: %.cc $(DEPDIR)%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

$(OBJDIR)%.o: %.cpp $(DEPDIR)%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

%.i : %.c
	$(CC) -E $(CPPFLAGS) $<
