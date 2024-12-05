# Data Types

## Weak and Strong Types

Variables can either be weakly typed or strongly typed, mostly for ease of
prototyping and then type security for production-level code.

```
// weak typing
a = 0

// strong typing
i8 a = 0
```

## Primitive Data Types

Blam offers conventional primitive data types.

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

Blam only supports one type of dynamic string, as well as a conventional char
array.

```
str a = "this is a string"
```

## Arrays

Arrays should always be strongly typed.

```
i16 array[3]
i16 array[] = [1, 2, 3]
```

## Collections

Collections can be weakly typed and can contain any data type.

```
collection = [1, true]
```

## Vectors

Vectors and vector collections do not have a static size.

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