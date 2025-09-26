# Structs
Structs group related data into organized segments. They are useful for modeling domain-specific entities, like user 
accounts or events.
```
struct MyStruct {
    i8 field1
    str field2
}
```

## Methods
Structs can also have member methods defined for them. Take note that these methods are private by default, but can be 
made public with the pub keyword.
```
struct MyStruct {
    i8 field1
    str field2

    pub publicMethod()
    privateMethod()

    returnI8() -> i8
}

MyStruct::publicMethod() { }
MyStruct::privateMethod() { }

MyStruct::returnI8() {
    return 8
}
```

## Constructors
A constructor doesn't necessarily need a return type, because the return type is self.

```
struct MyStruct {
    i8 field1
    str field2

    pub publicMethod()
    privateMethod()

    returnI8() -> i8
}

...

MyStruct::Mystruct(i8 field1, str field2) {
    self.field1 = field1
    self.field2 = field2
}
```