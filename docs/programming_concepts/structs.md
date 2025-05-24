# Structs

```
// this is a struct
struct Example {
    parameter
}

struct CMD {
    pub CMD(project: str) -> Self     // associated public constructor

    project: str
    workspace: str
    cwd: str

    initWorkspace() -> str            // associated public function
}

// function definition
CMD::CMD(project: str) {
    self.project = project
    self.workspace = self.initWorkspace()
}

// function definition
CMD::initWorkspace(project: str) -> str {
    self.workspace = project
}
```