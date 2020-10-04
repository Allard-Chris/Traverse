NAME					:= Traverse
CC						:= gcc
SRC_DIR				:= ./src
BUILD_DIR			:= ./build
BIN_DIR				:= ./bin
ASSETS_DIR		:= $(SRC_DIR)/assets
SRC_FILES			:= $(wildcard $(SRC_DIR)/*.c)
ASSETS_FILES	:= $(wildcard $(SRC_DIR)/assets/*.png)
CFLAGS				:= -Wall `xml2-config --cflags` `pkg-config --cflags gtk+-3.0`
LDFLAGS				:= `xml2-config --libs` `pkg-config --libs gtk+-3.0`

all: clean config release

# assets compilation
ASSETS_BUILD_DIR	:= $(BUILD_DIR)/assets
ASSETS_OBJ_FILES	:= $(patsubst $(ASSETS_DIR)/%.png,$(ASSETS_BUILD_DIR)/%.o,$(ASSETS_FILES))

$(ASSETS_BUILD_DIR)/%.o: $(ASSETS_DIR)/%.png
	@echo "Compiling assets $@"
	ld -r -b binary $< -o $@

# debug
DBG_BIN_DIR		:= $(BIN_DIR)/debug
DBG_BUILD_DIR	:= $(BUILD_DIR)/debug
DBG_DIRS		:= $(DBG_BUILD_DIR) $(DBG_BIN_DIR)
DBG_FLAGS		:= -g -O0 -v -da -Q
DBG_EXE			:= $(NAME)_debug
DBG_OBJ_FILES	:= $(patsubst $(SRC_DIR)/%.c,$(DBG_BUILD_DIR)/%.o,$(SRC_FILES))

debug: $(DBG_EXE)

$(DBG_EXE): $(DBG_OBJ_FILES) $(ASSETS_OBJ_FILES)
	@echo "Linking $(DBG_EXE):"
	$(CC) $(LDFLAGS) -o $(DBG_BIN_DIR)/$@ $^

$(DBG_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling object $@"
	$(CC) -c $(CFLAGS) $(DBG_FLAGS) -o $@ $<

# release
REL_BIN_DIR		:= $(BIN_DIR)/release
REL_BUILD_DIR	:= $(BUILD_DIR)/release
REL_DIRS		:= $(REL_BUILD_DIR) $(REL_BIN_DIR)
REL_FLAGS		:= -O2
REL_OBJ_FILES	:= $(patsubst $(SRC_DIR)/%.c,$(REL_BUILD_DIR)/%.o,$(SRC_FILES))

release: $(NAME)

$(NAME): $(REL_OBJ_FILES) $(ASSETS_OBJ_FILES)
	@echo "Linking $(NAME):"
	$(CC) $(LDFLAGS) -o $(REL_BIN_DIR)/$@ $^

$(REL_BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling object $@"
	$(CC) -c $(CFLAGS) $(REL_FLAGS) -o $@ $<

# config pre build
config:
	@echo create directories
	$(info $(shell mkdir -p $(REL_DIRS) $(DBG_DIRS) $(ASSETS_BUILD_DIR)))

.PHONY: clean

clean:
	@echo "Cleaning:"
	$(RM) $(BUILD_DIR)/release/*
	$(RM) $(BUILD_DIR)/debug/*
	$(RM) $(BUILD_DIR)/assets/*
	$(RM) $(BIN_DIR)/release/*
	$(RM) $(BIN_DIR)/debug/*