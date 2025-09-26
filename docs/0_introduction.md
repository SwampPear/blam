# Blam
The objective of Blam is to provide a language combining the ease-of-use of Python, strong security like Rust, 
interoperability with existing technologies, and strong concurrency features (as in Go).

Blam's philosophy:
- Simplicity & Reasonability
- Expressiveness without complexity
- Efficient & Transparent builds
- Performance-conscious, predictable memory

# Bloop Package Manager
Similar to Python's PIP, Bloop should provide reliable package management and versioning, as well as 

- transparent upgrades without breaking any changes
- reliable package management

# Syntax
Blam uses only the '.blam' file extension and emphasizes readability left-right and top-down, reasonability and fast
onboarding. Blam also is capable of dynamic typing for development and static typing for production with a sound type
inference system. Blam projects' directory structure is as follows:

- project/
  - build/    (build system files)
  - src/      (source code)
  - blam.yaml (project configuration)

# Features
## Core Language Philosophy
- data-first functional programming with subject–verb–object (SVO) syntax
- immutability by default; mutability explicit
- dynamic typing for development, static typing for production with type inference
- eager evaluation with mostly call-by-value semantics
- clear, helpful error messages
- no null, no exceptions by default, no garbage collector (predictable memory)

## Expressiveness & Abstractions
- multiple dispatch pattern, including operator overloading
- first-class functions (JavaScript-like syntax)
- higher-order functions with concise syntax
- serialization-friendly (similar to JavaScript)
- generics and algebraic data types for type-safe flexibility

## Code Structure & Compilation
- content-addressable code (functions and const values are identifiable by content)
- tree-shakeable modules (compile-time elimination of unused code)
- robust binary inclusion system for external dependencies
- readable, single-extension (.blam) syntax with standardized project layout

## Performance & Memory Model
- pointer syntax for call-by-reference when needed
- predictable memory model (no GC, explicit control)
- mostly call-by-value semantics, minimizing hidden costs

## Interoperability
- transpiles to JavaScript, TypeScript, WASM, C, and/or C++
- seamless integration with existing ecosystems
- ease of embedding Blam code into modern toolchains

## Concurrency
- goroutine-like lightweight tasks
- async unbounded buffered channels for communication
- event-driven, async, and streamable by default
- transparent scaling from single-core to multi-core to distributed systems (no refactor required)
- aggressively parallelizable, like Go
- async via blocking/sync interface with non-blocking I/O under the hood
- parallelizable pipelines without added complexity

## Non-Features
- multi-paradigm sprawl
- heavy metaprogramming
- DSL proliferation

# Standard Libraries
- http
- socket
- io
- fs
- json
- re
- os
- thx