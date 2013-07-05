BUILD_DIR = Debug
SRC_DIR =sources
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(pathsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o)

CC=gcc
CCFLAGS=-W -Wall -g
REBUILDABLES = $(OBJS) $(TARGET)

#rules
all:
	checkdirs $(BUILD_DIR)/smtp 

%: $(OBJS)
	$(CC) $(CCFLAGS) $^ -o $@

%.o: %.c
	$(CC) -o $@ -c $< $(CCFLAGS)
	
checkdirs : $(BUILD_DIR)
$(BUILD_DIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf *.o 
	echo Clean done

$(eval $(call make-goal,$(BUILD_DIR))))	
