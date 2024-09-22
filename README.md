# Blam

The objective of Blam is to provide a programming language that combines
ease-of-use, strong security, and interoperability between existing 
technologies.

# Syntax
- file extensions: .blam
- readability left to right and top to down
- reasonability, fast onboarding of new users
- dynamic typing for development, static for production (with sound inference)

# Features
- no data piping or explicit currying of functions (lacks reasonability)
- data first functional programming with svo syntax
- immutability by default (but with in-place mutations similar to rust)
- content addressable code
- eager evaluation
- mostly call by value
- pointer syntax for call by reference
- first class functions (similar to javascript syntax)
- higher order functions (also similar to javascript syntax)
- serializable (in similar manner to javascript)
- tree shakeable (during compile time)
- robust bin inclusion system
- helpful error messages (like cargo)
- generics, algebraic data types, no null, no exceptions by default, no garbage 
collector

# Interoperability
- transpiles to JS, TS, and/or WASM (would be nice)

# Concurrency
- concurrency via goroutines and the async unbounded buffered channels
- aggressively parallelizable and concurrent (like Go)
- scales transparently from single cpu to multi-core devices and distributed 
services without necessitating a refactoring of the code (hard)
- async, event driven, streamable, parallelizable, by default
- async via blocking/sync interface but non-blocking I/O

# Non-Features
- multi paradigm
- meta programming
- DSLs

# Bloop Package Managaer
- transparent upgrades without breaking any changes
- reliable package management

# Documentation
- code compiles into mardown format
- docstrings compile

# Auxiliary Features
- interactive VSCODE plugin, shows content of data structures while coding

# Standard Library
### Argparse
- argument parsing functionality similiar to how Python handles it

### Http
- http functionality similar to Python requests module

### IO
- for standard in, out, and error

### FS
- file system functionality

### JSON
- json serialization similar to Python module

### Re
- regex digestion and matching

### OS
- similar to python os module

### Thread
- some type of threading (may be difficult)

[test](docs/test.md)
