MAPSIZE ?= 16

CC = gcc
OBJDIR = build

CFLAGS = -Wall -g -DMAPSIZE=$(MAPSIZE)

DEFINES = -DMAPSIZE=$(MAPSIZE)

all: $(OBJDIR) $(OBJDIR)/test_allocator_$(MAPSIZE).test

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/test_allocator_$(MAPSIZE).test: test_allocator.c ../allocator.c $(OBJDIR)
	$(CC) $(CFLAGS) $(DEFINES) -o $(OBJDIR)/test_allocator_$(MAPSIZE).test test_allocator.c ../allocator.c -I../
	$(CC) $(DEFINES) -E test_allocator.c -I../ >$(OBJDIR)/test_allocator_$(MAPSIZE).i 2>&1
	$(CC) $(DEFINES) -E ../allocator.c -I../ >$(OBJDIR)/allocator_$(MAPSIZE).i 2>&1

clean:
	rm -rf $(OBJDIR)

run-test: $(OBJDIR) $(OBJDIR)/test_allocator_$(MAPSIZE).test
	./$(OBJDIR)/test_allocator_$(MAPSIZE).test

