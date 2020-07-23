ifndef config
	config=Release
endif
	
LIB_SOURCES  = vector.c
TEST_SOURCES = test/test.c

BUILD_DIR = build/$(config)
BIN_DIR   = $(BUILD_DIR)/bin
LIB_DIR   = $(BUILD_DIR)/lib

# ---- Targets ----

TEST = $(BIN_DIR)/test
LIB  = $(LIB_DIR)/libcvector.a

# ---- Compiler flags ----

ifeq ($(config),Debug)
	CFLAGS += -Wall -Wextra -std=c99 -pedantic -g
else ifeq ($(config),Release)
	CFLAGS += -Wall -Wextra -std=c99 -pedantic -O3 -DNDEBUG
endif

# ---- Rules ----

LIB_OBJECTS = $(LIB_SOURCES:%.c=$(BUILD_DIR)/%.o)
LIB_DEPEND  = $(LIB_OBJECTS:.o=.d)

TEST_OBJECTS   = $(TEST_SOURCES:%.c=$(BUILD_DIR)/%.o)
TEST_DEPEND    = $(TEST_OBJECTS:.o=.d)
TEST_BUILD_DIR = $(dir $(TEST_OBJECTS)) 


.PHONY:
all: $(LIB) $(TEST)

.PHONY:
cvector: $(LIB)

.PHONY:
test: $(TEST)

.PHONY:
run_test: $(TEST)
	@$(BIN_DIR)/test

$(TEST): $(TEST_OBJECTS) $(LIB) | $(BIN_DIR)
	@echo "Linking $@"
	@$(CC) $(TEST_OBJECTS) -o $@ -Wl,$(LIB)

$(LIB): $(LIB_OBJECTS) | $(LIB_DIR)
	@echo "Creating $@"
	@$(AR) rcs $@ $^

$(BUILD_DIR)/%.o:%.c | $(BUILD_DIR)
	@echo "[$(CC)] Compiling $< -> $@"
	@$(CC) $(CFLAGS) -I./ -MP -MMD -c $< -o $@

-include $(LIB_DEPEND)
-include $(TEST_DEPEND)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LIB_DIR):
	@mkdir -p $(LIB_DIR)

$(TEST_BUILD_DIR):
	@mkdir -p $(TEST_BUILD_DIR)

$(BUILD_DIR): | $(TEST_BUILD_DIR)
	@echo "Creating Directories..."
	@mkdir -p $(BUILD_DIR)

.PHONY: clean
clean:
	@echo "Cleaning $(BUILD_DIR)..."
	@rm -rf $(BUILD_DIR)
