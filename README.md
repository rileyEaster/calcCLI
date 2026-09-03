# calc CLI

A lightweight calculator and solver for the command line, implementing recipes from *Numerical Recipes* for C++.

**Version 0.1.1**

## Overview

Initially, this project started as a way to simultaneously learn C++ and numerical methods beyond those of Berkeley's undergraduate coursework. Quickly, it also became a kind of memoir of higher mathematical education.

**Includes:**
- Pratt parser for numerical expressions, including functions such as `gamma`
- MATLAB-style vector and matrix literals
- Session variables and basic linear-algebra operations (`+`, `*`)
- `tensorlib` containers (`ccVector` / `ccMatrix`) aimed at Numerical Recipes–style algorithms and, later, higher-rank tensors

## Features

- Scalar arithmetic: `+`, `-`, `*`, `/`, `^`, unary `+/-`
- Built-ins: `sin`, `cos`, `tan`, `log` / `ln`, `exp`, `sqrt`, `gamma`
- Constants: `pi`, `e`
- Vectors: `[x y z]`
- Matrices: `[x1 x2 x3; y1 y2 y3; z1 z2 z3]`
- Assignment (silent): `v = [1 0 0]`
- Vector / matrix addition and multiplication (including scalar scaling, mat·vec, and vector dot product)
- Interactive REPL; type `exit` to quit (`Exit`, `exit()`, and `ext` also work)

## Project structure

```text
.
├── calcCLI.cpp    # Lexer, parser, AST, REPL
├── tensorlib.h    # ccVector / ccMatrix
├── README.md
└── calc           # Built binary (local; not required in git)
```

## Build

Requires a C++17 compiler (`clang++` or `g++`).

```bash
c++ -std=c++17 -o calc calcCLI.cpp
```

## Usage

```bash
./calc
```

### Examples

Scalars:

```text
2 + 2
sin(pi / 2)
gamma(5)
```

Vectors and matrices (MATLAB-style):

```text
v = [1 0 0]
w = [0 1 0]
v + w
2 * v
v * w

A = [1 2; 3 4]
B = [0 1; 1 0]
A + B
A * B
A * [1 0]
```

Assignments do not echo; evaluate a name (or expression) to print a value:

```text
v = [1 0 0]
v
```

```text
[1 0 0]
```

### Session commands

| Input | Effect |
|-------|--------|
| `exit` / `Exit` / `exit()` / `ext` | End the session |
| `admin` | Enter debug/admin mode (first prompt only) |

## Roadmap

- [ ] User-defined functions, e.g. `f(x) = x^2 + x + 1`
- [ ] More Numerical Recipes algorithms wired through `tensorlib`
- [ ] Richer tensor operations and higher-rank tensors
- [ ] Center banner / polish REPL presentation
- [ ] Explore AST → lambda conversion

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgments

- *Numerical Recipes* for C++
