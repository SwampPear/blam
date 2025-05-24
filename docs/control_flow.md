# Control Flow

## Conditional Control Statements

There are two types of conditional control statements in Blam, if statements and
switches.

### If Statements

An if statement is a control statement that evaluates a condition and executes
a block of code if the condition is true.

```
bool condition = true

// this will always execute
if condition {
    // do something
}
```

### Switches

A switch statement is a control statement that evaluates an enum specific case
and executes a block of code if the case matches.

```
enum SomeEnum {
    CASE_ONE,
    CASE_TWO,
}

SomeEnum someCase = CASE_ONE

// only the block under CASE_ONE will execute
sw someCase {
case CASE_ONE:
    // do something
case CASE_TWO:
    // do something else
}
```

## Loops

Blam supports two different types of while loops and two different types of for 
loops.

### While Loops

A standard while loop evaluates a condition and executes a block of code while 
the condition is true.

```
bool condition = true

// this will loop indefinitely
while condition {
    // do something
}
```

### Do-While Loops

A do-while loop executes a block of code and then evaluates a condition to
determine if it should continue executing.

```
bool condition = false

// this will loop indefinitely
do {
    condition = true
} while condition
```

### Condition-Controlled For Loops

Condition-controlled for loops evaluate a condition and iterate a variable
until the condition is no longer true.

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

The continue keyword immediately continues to the next iteration of a loop on
invocation.

```
// skips the fifth iteration
for i = 0, i < 10, i++ {
    if i == 4 {
        continue
    }

    // more code
}
```