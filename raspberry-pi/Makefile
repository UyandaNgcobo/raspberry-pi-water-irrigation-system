# Makefile for Water Irrigation System

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = irrigation_system
SOURCE = main.cpp

# Directories
BUILD_DIR = build
SRC_DIR = .

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Main target
$(BUILD_DIR)/$(TARGET): $(BUILD_DIR) $(SRC_DIR)/$(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$(TARGET) $(SRC_DIR)/$(SOURCE)
	@echo "✅ Compilation successful!"
	@echo "📍 Executable: $(BUILD_DIR)/$(TARGET)"

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Test the program
test: $(BUILD_DIR)/$(TARGET)
	@echo "🧪 Testing irrigation system..."
	./$(BUILD_DIR)/$(TARGET)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	@echo "🧹 Build directory cleaned"

# Install dependencies (for actual Raspberry Pi)
install-deps:
	@echo "📦 Installing dependencies..."
	@echo "For actual Pi: sudo apt-get install build-essential"
	@echo "For development: Dependencies already met"

# Run the system
run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(BUILD_DIR)/$(TARGET)
	@echo "🐛 Debug build complete"

# Help
help:
	@echo "🌱 Water Irrigation System - Build Commands"
	@echo ""
	@echo "Commands:"
	@echo "  make          - Build the irrigation system"
	@echo "  make test     - Build and test the system"
	@echo "  make run      - Build and run the system"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make clean    - Clean build files"
	@echo "  make help     - Show this help message"
	@echo ""

.PHONY: all test clean install-deps run debug help