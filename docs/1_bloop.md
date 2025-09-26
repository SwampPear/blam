# Bloop Package Manager
Bloop is the package, build, and publishing tool for Blam projects. It emphasizes transparent upgrades, reproducible 
builds, and deterministic toolchains with content-addressable artifacts.

# Quick Start
bloop init my-app
...

# Quick Start
```
# 1) create a new project
bloop init my-app
cd my-app

# 2) add a dependency
bloop add @std/logging ^1.2

# 3) build & run
bloop build
bloop run src/main.blam

# 4) update safe patches/minors per policy
bloop upgrade
```

# blam.yaml Example
```
name: my-app
version: 0.1.0
blam: '>=0.1 <1.0'          # required toolchain range

entry: src/main.blam        # default entrypoint
license: MIT

dependencies:
  '@std/logging': ^1.2
  'acme/utils': ~2.4.3

devDependencies:
  'acme/test': ^0.6

bin:                         # “robust bin inclusion system”
  my-app: src/main.blam

build:
  target: wasm               # wasm | js | ts | native (impl-dependent)
  optimization: size         # size | speed | none
  treeShake: true
  contentAddressable: true   # lock artifacts by content hash
```

# Common Commands
```
bloop init [path]                # scaffold a new project
bloop add <pkg> [range]          # add dep (writes manifest & lock)
bloop remove <pkg>               # remove dep
bloop update <pkg|all>           # resolve to newest within range
bloop upgrade [--minor|--patch]  # safe in-range upgrades (no breaks)
bloop lock                       # (re)write lockfile deterministically
bloop verify                     # verify lockfile & integrity hashes
bloop build [--release]          # compile; tree-shake; CA artifacts
bloop run [file|bin] [-- args]   # run entry or specific bin
bloop test [pattern]             # run tests (if present)
bloop publish                    # publish package (with checks)
bloop info <pkg>                 # show resolved version & metadata
bloop why <pkg>                  # explain inclusion (dep graph)
bloop cache clean                # prune artifact & module caches
```
