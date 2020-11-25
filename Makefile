config ?= Release
	
CVECTOR_SOURCES  = vector.c
TEST_SOURCES = test/test.c

BUILD_DIR = build/$(config)
BIN_DIR   = $(BUILD_DIR)/bin
LIB_DIR   = $(BUILD_DIR)/lib

# ---- Targets ----

CVECTOR_TARGET = $(LIB_DIR)/libcvector.a
TEST_TARGET = $(BIN_DIR)/test

# ---- Compiler flags ----

ifeq ($(config),Debug)
	CFLAGS += -Wall -Wextra -std=c99 -pedantic -g
else ifeq ($(config),Release)
	CFLAGS += -Wall -Wextra -std=c99 -pedantic -O3 -DNDEBUG
endif

# ---- Rules ----

CVECTOR_OBJECTS = $(CVECTOR_SOURCES:%.c=$(BUILD_DIR)/%.o)
CVECTOR_DEPEND  = $(CVECTOR_OBJECTS:.o=.d)

TEST_OBJECTS   = $(TEST_SOURCES:%.c=$(BUILD_DIR)/%.o)
TEST_DEPEND    = $(TEST_OBJECTS:%.o=$(BUILD_DIR)/%.d)
TEST_BUILD_DIR = $(dir $(TEST_OBJECTS))

.PHONY:
all: $(CVECTOR_TARGET) $(TEST_TARGET)

.PHONY:
cvector: $(CVECTOR_TARGET)

.PHONY:
test: $(TEST_TARGET)

.PHONY:
run_test: $(TEST_TARGET)
	@$(BIN_DIR)/test

$(TEST_TARGET): $(TEST_OBJECTS) $(CVECTOR_TARGET) | $(BIN_DIR)
	@echo "Linking $@"
	@$(CC) $(TEST_OBJECTS) -o $@ -Wl,$(CVECTOR_TARGET)

$(CVECTOR_TARGET): $(CVECTOR_OBJECTS) | $(LIB_DIR)
	@echo "Creating $@"
	@$(AR) rcs $@ $^

$(BUILD_DIR)/%.o:%.c | $(BUILD_DIR)
	@echo "[$(CC)] Compiling $< -> $@"
	@$(CC) $(CFLAGS) -I./ -MP -MMD -c $< -o $@

-include $(CVECTOR_DEPEND)
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
