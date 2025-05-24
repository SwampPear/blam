# Structs

It's often very useful to be able to group data into organized segments, which 
is where we introduce the concept of structs. Take the following example, using 
user authorization data.

```
struct Auth {
    int id
    str username
    str password
}
```

## Methods

Structs can also have member methods defined for them. Take note that these
methods are private by default, but can be made public with the pub keyword.

```
struct Auth {
    int id
    str username
    str password

    pub login()

    // private method
    getPasswordHash() -> str
}

Auth::login() {
    login(this.username, this.getPasswordHash())
}

Auth::getPasswordHash() {
    // this keyword used to denote this object
    return this.password.hash()
}
```

## Constructors

A constructor doesn't necessarily need a return type, because the return type is 
self.

```
struct Auth {
    int id
    str username
    str password

    // constructor
    pub Auth()

    pub login()

    // private method
    getPasswordHash() -> str
}

Auth::Auth(int id, str username, str password) {
    // set each
}

Auth::login() {
    login(self.username, self.getPasswordHash())
}

Auth::getPasswordHash() {
    // this keyword used to denote this object
    return self.password.hash()
}
```

## Constructor Builder Notation

Structs can also be instantiated without a constructor using builder notation.

```
// for example, this would be valid if no constructor was defined
a = Auth() : {
    id: 0
    username: "bigbug27"
    password: "bad_password"
}

// this would also be valid if a constructor was not defined
a = Auth()

a.id = 0
a.username = "bigbug27"
a.password = "bad_password"
```
