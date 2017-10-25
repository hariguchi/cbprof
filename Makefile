# Languages
CC       := gcc
PERL     := perl

COMPILE.c   = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc  = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@


# Directories
OBJDIR := obj
DEPDIR := dep
BOOSTDIR := /usr/include/boost


# Flags
DEPFLAGS      = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
OPTFLAGS     := -O0
INCLUDEFLAGS := -I../include
PROFFLAGS    := #-pg
DEFS         += 
CFLAGS       := -Wall -g $(PROFFLAGS) $(INCLUDEFLAGS) $(OPTFLAGS) $(DEFS)
CXXFLAGS     := $(CFLAGS) -std=c++11
LDFLAGS      := 

# Libraries
LDLIBS    := 
LOADLIBES := 


# Target names
TARGET    := profTest
LIBTARGET := libtprof.a


# Directories to build
OBJDIR := obj
DEPDIR := dep


# Source files
LIBSRCS   := cbProf.c
SRCS      := profTest.c

# Object files
LIBOBJS   := $(addprefix $(OBJDIR)/,$(LIBSRCS:.c=.o))
OBJS      := $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))


$(TARGET): $(OBJS) $(LIBTARGET)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) $(PROF) -o $@

$(LIBTARGET): $(LIBOBJS)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

.PHONY: perf
perf:
	$(MAKE) -f $(lastword $(MAKEFILE_LIST)) OPTFLAGS=-O3 DEFS=-DNODEBUG

.PHONY: clean
clean:
	rm -f $(TARGET) $(TARGET).exe $(LIBTARGET) $(OBJS) $(LIBOBJS) \
	$(DEPDIR)/*.d *.bak *.exe.* *~


$(OBJDIR)/%.o : %.c
$(OBJDIR)/%.o : %.c $(DEPDIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : %.cpp
$(OBJDIR)/%.o : %.cpp $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : %.cc
$(OBJDIR)/%.o : %.cc $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o : %.cxx
$(OBJDIR)/%.o : %.cxx $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

%.i : %.c
	$(CC) -E $(CPPFLAGS) $<

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

# Include dependency files
include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
