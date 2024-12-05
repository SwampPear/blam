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

<h2 class="content__h2">Loops</h2>
<p class="content__p">
    Here's an example of a while loop, which functions by evaluating a condition.
</p>
<pre class="content__code">
bool condition = true

while condition {
    // do something
    condition = false
}

for i = 0, i < 10, i++ {
    
}
</pre>
<p class="content__p">
    Blam also supports two different types of for loops, condition-controlled
    loops and collection-controlled loops.
</p>
<pre class="content__code">
// condition-controlled count loop
for i = 0, i < 10, i++ {
    // do something
}

// condition-controlled iterator loop using linked lists
LLNode a = LLNode(1)
LLNode b = LLNode(2)
LLNode c = LLNode(3)
a.next = b
b.next = c

for node = a, a != null, a = a.next {
    // do something
}

// collection-controlled loops
i8 arr[] = [1, 2, 3]
for i in arr {
    // do something
}
</pre>
<h2 class="content__h2">Loop Control</h2>
<p class="content__p">
    Blam has a few different keywords for controlling flow within a loop. The
    break keyword immediately breaks out of a loop on invocation, the redo keyword
    resets the loop from the current iteration, and the retry keyword restarts
    the loop from its initial state.
    
</p>
<pre class="content__code">
// immediately breaks the loop on the fifth iteration
for i = 0, i < 10, i++ {
    if i == 4 {
        break
    }
}

// resets the loop to initial state on fifth iteration (infinite)
for i = 0, i < 10, i++ {
    if i == 4 {
        redo
    }
}

// resets the loop to fifth iteration state on fifth iteration (infinite)
for i = 0, i < 10, i++ {
    if i == 4 {
        retry
    }
}
</pre>
