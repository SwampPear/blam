# Bloop

`Bloop` is Blam's dedicated package manager and CLI tool. To create a new Blam
package, run the following command to create a package in the current directory:

```
bloop new <package name>
```

Building and running the package can be handled via the following commands:

```
bloop build
bloop run
```

### Package Management

Installing external packages can be done by either manually updating 
`blam.yaml` or by running this following command:

```
bloop install <package name>
```