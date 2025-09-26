# Control Flow
Control flow in Blam is designed to be simple, predictable, and expressive. The language provides clear constructs for 
conditional branching, iteration, loop control, and explicit error handling, while maintaining readability and consistency.

## Conditional Control Statements
Blam supports two primary forms of conditional branching: **if statements** and **switch statements**.

### If Statements
An `if` statement evaluates a condition and executes a block of code if the condition resolves to `true`.

```blam
bool condition = true

# this will always execute
if condition {
  # do something
}
```

### Switches
Switch statements evaluate equality on a variable

```
i8 someCase = 1

sw someCase {
  case 1:
    # do something
  case 2:
    # do something else
  default:
    # default case
}

```

## Loops
### While Loops
A while loop evaluates a condition before each iteration and executes the block while the condition is true.
```
bool condition = true

while condition {
    # do something
}
```

### Do-While Loops
A do-while loop executes a block of code and then evaluates a condition to determine if it should continue executing.
```
bool condition = false

// this will loop indefinitely
do {
    condition = true
} while condition
```

### Condition-Controlled For Loops
Condition-controlled for loops evaluate a condition and iterate a variable until the condition is no longer true.
```
// this will loop 10 times
for i = 0, i < 10, i++ {
    // do something
}
```

### Collection-Controlled For Loops
Collection-controlled for loops iterate over a collection of items.
```
i8 arr[] = [1, 2, 3]

// this will loop 3 times
for i in arr {
    // do something
}
```

## Loop Control
Blam supports a couple different methods of loop control.

### Break
The break keyword immediately breaks out of a loop on invocation.
```
// immediately breaks the loop on the fifth iteration
for i = 0, i < 10, i++ {
    if i == 4 {
        break
    }
}
```

### Continue
The continue keyword immediately continues to the next iteration of a loop on invocation.
```
// skips the fifth iteration
for i = 0, i < 10, i++ {
    if i == 4 {
        continue
    }

    // more code
}
```