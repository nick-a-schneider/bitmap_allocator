MAPSIZE ?= 16
INDEXSIZE ?= 32
CC = gcc

OBJDIR = build/m_$(MAPSIZE)/i_$(INDEXSIZE)

CFLAGS = -Wall -g

DEFINES = -DMAPSIZE=$(MAPSIZE) -DINDEXSIZE=$(INDEXSIZE)

all: $(OBJDIR) $(OBJDIR)/test_allocator.test

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/test_allocator.test: test_allocator.c ../allocator.c $(OBJDIR)
	$(CC) $(CFLAGS) $(DEFINES) -o $(OBJDIR)/test_allocator.test test_allocator.c ../allocator.c -I../
	$(CC) $(DEFINES) -E test_allocator.c -I../ >$(OBJDIR)/test_allocator.i 2>&1
	$(CC) $(DEFINES) -E ../allocator.c -I../ >$(OBJDIR)/allocator.i 2>&1

clean:
	rm -rf $(OBJDIR)

run-test: $(OBJDIR) $(OBJDIR)/test_allocator.test
	./$(OBJDIR)/test_allocator.test

