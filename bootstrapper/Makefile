# Makefile for Blam (Python port)

PYTHON := python3
PIP := $(PYTHON) -m pip
PYTEST := $(PYTHON) -m pytest
VENV := .venv
ACTIVATE := source $(VENV)/bin/activate

# Default target
all: install test

# Create virtual environment
$(VENV)/bin/activate: requirements.txt
	$(PYTHON) -m venv $(VENV)
	$(ACTIVATE) && $(PIP) install -r requirements.txt

# Install dependencies (pytest, etc.)
install: $(VENV)/bin/activate

# Run tests
test:
	$(ACTIVATE) && $(PYTEST) -v tests/

# Clean cache and build artifacts
clean:
	rm -rf __pycache__/ */__pycache__/ .pytest_cache .venv

# Format code with black
format:
	$(ACTIVATE) && black blam/ tests/

# Lint with flake8 (optional)
lint:
	$(ACTIVATE) && flake8 blam/ tests/

# Run an example tokenizer test manually
run-tokenizer:
	$(ACTIVATE) && $(PYTHON) -m tests.test_tokenizer -v

.PHONY: all install test clean format lint run-tokenizer
