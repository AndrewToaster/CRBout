CC = clang
OUT_DIR = out
OUT_FILE = app
LIBS = -lm
INCLUDES = -I.
WARNS = -Wall
STD = 
SOURCES := $(wildcard **/*.c)
OBJECTS := $(patsubst %.c,$(OUT_DIR)/release/%.o,$(SOURCES))
OBJECTS_DEBUG := $(patsubst %.c,$(OUT_DIR)/debug/%.o,$(SOURCES))

ifeq ($(OS),Windows_NT)
include windows.mk
else
include linux.mk
endif

.PHONY: release debug valgrind clean

release: $(OBJECTS)
	$(call mkdir,$(OUT_DIR)/release)
	$(CC) -o $(OUT_DIR)/release/$(OUT_FILE) $(LIBS) $^ $(WARNS) $(STD)

debug: $(OBJECTS_DEBUG)
	$(call mkdir,$(OUT_DIR)/debug)
	$(CC) -g -o $(OUT_DIR)/debug/$(OUT_FILE) $(LIBS) $^ $(WARNS) $(STD)

valgrind: $(OBJECTS_DEBUG)
	$(call mkdir,$(OUT_DIR)/debug-valgrind/)
	$(CC) -g -gdwarf-4 -o $(OUT_DIR)/debug-valgrind/$(OUT_FILE) $(LIBS) $^ $(WARNS) $(STD)
	cat in.txt | valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./app.exe run code.txt > out.txt

clean:
	$(call rm,$(OUT_DIR)/debug)
	$(call rm,$(OUT_DIR)/release)

$(OUT_DIR)/release/%.o: %.c
	$(call mkdir,$(dir $@))
	$(CC) -c $< -o $@ $(WARNS) $(INCLUDES) $(STD)

$(OUT_DIR)/debug/%.o: %.c
	$(call mkdir,$(dir $@))
	$(CC) -g -c $< -o $@ $(WARNS) $(INCLUDES) $(STD)
