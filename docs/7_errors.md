# Errors
Error handling in Blam is explicit and predictable. Errors fall into two categories:

- Compiler Errors — Raised at compile time when code violates Blam’s type system, syntax rules, or safety constraints. These must be resolved before execution.
- Runtime Errors — Raised during execution when invalid operations occur (e.g., division by zero). Runtime errors can be raised manually and caught using structured handling.

## Raising Errors
The raise keyword creates and propagates an error value. Error values carry a type and message.
```
raiseError() {
  raise Error("This is an error message")
}
```

## Handling Errors
Errors are caught with try and catch blocks. Multiple catch clauses allow handling of different error types.
```
main() {
  try {
    raiseError()
  }
  catch Error e {
    # handle Error
    print("Caught error: ", e)
  }
  catch OtherError oe {
    # handle other error types
    print("Caught other error: ", oe)
  }
}
```

