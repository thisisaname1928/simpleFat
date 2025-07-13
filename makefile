CC=gcc

CSRC=$(shell find ./ -type f -name "*.c")
COBJ=$(patsubst %.c, %.o, $(CSRC))
OUTPUT=test

run: $(OUTPUT)
	@./$<

$(OUTPUT): $(COBJ)
	@$(CC) $(COBJ) -o $@

%.o: %.c
	@echo "CC $< $@"
	@$(CC) -c $< -o $@

clean:
	@rm $(OUTPUT) $(shell find ./ -type f -name "*.o")