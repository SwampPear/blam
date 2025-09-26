# Functions
Execution always begins in `main`. `main` cannot return a value.

## Void and Type-Optional Functions
Functions that do not return a value may omit a return type. For functions where the return type is dynamic or unknown, 
the `any` type can be used. Blam also supports omitting explicit return types in weak typing mode.

- Functions with no return omit type.
- Functions with dynamic return use `any`.
- Otherwise, types are inferred or explicit.

```
# returns a string
someFunc(str a) -> str {
  return a
}

# returns nothing (void)
someVoidFunc() {}

# returns a dynamically typed value
someTypeOptionalFunc() -> any {
  a = 10
  a = true
  return a
}
```

## First-Class Functions
Functions in Blam are first-class meaning they can be passed as objects and can be called with () syntax.
```
someFunc(str a) -> str {
  return a
}

callFunc((str) -> str callback) {
  callback()
}

main() {
  str a = "this is a string"
  callFunc(someFunc(a))
}
```