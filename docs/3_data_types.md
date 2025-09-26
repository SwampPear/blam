# Data Types
## Weak vs. Strong Typing
Blam supports both weakly typed (dynamic) and strongly typed (static) variables. This allows for rapid prototyping with 
weak typing and secure, optimized production code with strong typing.
```
// weak typing
a = 0

// strong typing
i8 a = 0
```


## Primitive Data Types
Blam provides conventional primitive types. Unless specified, all primitives are immutable and use two’s complement for 
signed integers.
```
// i8  - 8 bit integer
// i16 - 16 bit integer
// i32 - 32 bit integer
// i64 - 64 bit integer
// int - alias for 32 bit integer

// u8  - 8 bit unsigned integer
// u16 - 16 bit unsigned integer
// u32 - 32 bit unsigned integer
// u64 - 64 bit unsigned integer
// uint - alias for 32 bit unsigned integer

// bool - 8 bit booleans

// char - 8 bit character
```

## Strings
Strings in Blam are UTF-8 encoded and dynamically sized. They are immutable by default.
```
str a = "this is a string"
```

## Arrays
Arrays are fixed-size, strongly typed, and allocated on the stack. The size must be known at compile time.
```
i16 array[3]
i16 array[] = [1, 2, 3]
```

## Collections
Collections are heterogeneous containers that may hold any type. They are weakly typed and growable.
```
collection = [1, true]
```

## Vectors
Vectors are growable, heap-allocated arrays. They always have a concrete element type but no fixed size.
```
i8 array = <1, 2, 3>
collection = <1, true>
```

## Complex Types
Blam supports union and intersection types.
```
int | str union
int & str intersection
```