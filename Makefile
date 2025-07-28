CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 $(shell pkg-config --cflags opencv4)
LDFLAGS = -lpthread -lwiringPi $(shell pkg-config --libs opencv4)

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: dacl

# Main executable
dacl: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f src/*.o dacl

# Format source code using clang-format
format:
	@echo "Formatting C++ source files..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i; \
		echo "Code formatting complete."; \
	else \
		echo "Warning: clang-format not found. Install with: sudo apt-get install clang-format"; \
	fi

# Check code formatting
format-check:
	@echo "Checking code formatting..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format --dry-run --Werror; \
		echo "Code formatting check passed."; \
	else \
		echo "Warning: clang-format not found. Install with: sudo apt-get install clang-format"; \
	fi

# Generate API documentation using Doxygen
doc:
	@echo "Generating API documentation..."
	@if command -v doxygen >/dev/null 2>&1; then \
		if [ -f Doxyfile ]; then \
			doxygen Doxyfile; \
			echo "Documentation generated in docs/ directory."; \
			echo "Open docs/html/index.html to view the documentation."; \
		else \
			echo "Error: Doxyfile not found. Run 'doxygen -g' to generate a default configuration."; \
		fi \
	else \
		echo "Warning: doxygen not found. Install with: sudo apt-get install doxygen graphviz"; \
	fi

# Clean documentation
doc-clean:
	@echo "Cleaning documentation..."
	rm -rf docs/

# Install development dependencies  
install-deps:
	@echo "Installing development dependencies..."
	sudo apt-get update
	sudo apt-get install -y clang-format doxygen graphviz

# Display help information
help:
	@echo "Available targets:"
	@echo "  all          - Build the dacl executable (default)"
	@echo "  clean        - Clean build artifacts"
	@echo "  format       - Format source code using clang-format"
	@echo "  format-check - Check code formatting without modifying files"
	@echo "  doc          - Generate API documentation using Doxygen"
	@echo "  doc-clean    - Clean generated documentation"
	@echo "  install-deps - Install development dependencies"
	@echo "  help         - Display this help message"

.PHONY: all clean format format-check doc doc-clean install-deps help