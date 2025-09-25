# Functions

Here's an example of a function in Blam. Take note that the start of execution
is always inside a main function.

```
someFunc(str a) -> str {
    return a
}

// starts Here
main() {
    someFunc()
}
```

## Void and Type-Optional Functions
You may notice that the main function above does not follow the same convention
as the someFunc function. This is because for void functions in Blam, the return
type can be omitted. You may ask however, "isn't Blam type-optional?" Functions
that return something can either omit the return type or return the any type.

```
// returns a string
someFunc(str a) -> str {
    return a
}

// returns nothing
someVoidFunc() {
    // nothing
}

// returns something
someTypeOptionalFunc() -> any {
    a = 10
    a = bool

    return a
}
```

## First-Class Functions

Functions in Blam are first-class meaning they can be passed as objects and
can be called with () syntax.

```
someFunc(str a) -> str {
    return a
}

callFunc(callback) {
    callback()
}

main() {
    str a = "this is a string"

    callFunc(someFunc(a))
}
```