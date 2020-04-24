# Makefile for the smash program
CC = g++
CFLAGS = -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG
CCLINK = $(CC) $(CFLAGS)
OBJS = smash.o commands.o signals.o Job.o
RM = rm -f
# Creating the  executable
smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)
# Creating the object files
Job.o: Job.cpp Job.H
signals.o: signals.cpp signals.H extrn.H
commands.o: commands.cpp commands.H
smash.o: smash.cpp commands.H
# Cleaning old files before new make
clean:
	$(RM) $(TARGET) *.o *~ "#"* core.*
