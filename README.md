<<<<<<< HEAD
# Blam
[LLVM]()


export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
	export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
	export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"
=======
<p align="center">
  <img alt="dream logo" src=".github/logo.svg" style="width: 25%">   
</p>

<p align="center" style="height: 32px">
  <img align="middle" alt="license" src="https://img.shields.io/github/license/SwampPear/blam.svg">
  <img align="middle" alt="last commit" src="https://img.shields.io/github/last-commit/SwampPear/blam.svg">
  <img align="middle" style="height: 21px" alt="built with love <3" src="http://ForTheBadge.com/images/badges/built-with-love.svg">
</p>

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
- multiple dispatch pattern, including operators
- no data piping or explicit currying of functions (lacks reasonability)
- data first functional programming with svo syntax
- mutability by default but with const values
- content addressable code (for const values and functions)
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

maf00026@mix.wvu.edu

<<<<<<< HEAD
### [Http](docs/http/http.md)
- http functionality similar to Python requests module

### [IO](docs/io/io.md)
- for standard in, out, and error

### [FS](docs/fs/fs.md)
- file system functionality

### [Json](docs/json/json.md)
- json serialization similar to Python module

### [Re](docs/re/re.md)
- regex digestion and matching

### [OS](docs/os/os.md)
- similar to python os module

### Thread
- some type of threading (may be difficult)
>>>>>>> b04542e597f676020691a300e91cef4083ba7a18
=======
>>>>>>> c4cd7027a88e33144e9fc87d6e5e2757dec2b68e
