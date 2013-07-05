OBJDIR := Debug
SRCDIR := sources
CC := gcc
CCFLAGS := -W -Wall -g

#rules
all: $(OBJDIR)/smtp 

$(OBJDIR)/smtp: $(OBJDIR)/main.o $(OBJDIR)/server.o $(OBJDIR)/smtpReplies.o
	$(CC) $(CCFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CCFLAGS) $^ -o $@ 
	
$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(OBJDIR)/*.o 
	echo Clean done

# Dependances
 $(OBJDIR)/smtp: $(OBJDIR)/main.o $(OBJDIR)/server.o $(OBJDIR)/smtpReplies.o | $(OBJDIR)
 $(OBJDIR)/main.o: $(OBJDIR)/server.o $(OBJDIR)/smtpReplies.o | $(OBJDIR)
 $(OBJDIR)/server.o: $(OBJDIR)/smtpReplies.o | $(OBJDIR)