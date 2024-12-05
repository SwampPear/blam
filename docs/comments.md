# Comments

Comments are simply pieces of text that can be ignored during the compilation 
process. Anything in a comment will not affect the functionality of the code.

## Single Line Comment

A comment can be added to a single line of code.

```
// this is a single line comment
```

## Multi Line Comment

A comment can also span over multiple lines.

```
/*
* multi line comment
*/
```

## Docstring

A docstring is a special type of comment that is used to formally describe a 
function or struct.

```
/**
* This is a docstring.
*
* @param a - this is a param called a
*/
const someFunc = (str a) -> str {
    return a
}
```