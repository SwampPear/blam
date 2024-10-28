- no pointer arithmetic, only array accessing
- combination of ownership semantics and smart pointer like syntax
- pointers are type checked according to the type checking syntax
- nullable pointers
- no garbage collection
- combination of ownership semantics and smart pointer like syntax
- concurrency comes later
- nullable pointers can be represented by the ? operator, similar to typescript
- values are given None
- pointer alignment will probably be handles through llvm
- only reference syntax, no pointer syntax
- pointer dereferencing should be handled implicitly
- type checker should ensure pointers cannot be cast unless they are cast to a
parent class

2. Memory Safety
Ensure mechanisms to prevent unsafe memory access, such as buffer overflows, dangling pointers, and null pointer dereferences.
You might want to implement null safety checks or introduce concepts like nullable and non-nullable pointers.
Consider whether to adopt automatic memory management (garbage collection) to prevent dangling pointers or require the programmer to manually manage memory.

4. Pointer Aliasing
Decide how your language handles pointer aliasing—when two or more pointers refer to the same memory location.
Some languages (like Rust) enforce strict aliasing rules to prevent multiple mutable references to the same memory, reducing data races and improving safety in multithreaded environments.

6. Smart Pointers
You could design smart pointers to handle memory allocation and deallocation automatically, reducing the need for manual memory management.
Smart pointers like shared pointers and unique pointers can manage resources (like memory, files) and ensure that resources are properly released.

10. Pointer Security
Take into account security issues, such as pointer dereferencing vulnerabilities (e.g., buffer overflows) and pointer injection attacks.
Using bounds checking and providing mechanisms to check pointer validity can help mitigate security risks.
