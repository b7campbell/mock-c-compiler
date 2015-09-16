# mock-c-compiler

This compiler parses a c-like imperative language into llvm bytecodes. Ideally in the future, this compiler will be very close to a complete C compiler.

| *Quick Facts*     |                   |
|-------------------|------------------:|
| Language Spec     |       Subset of C |
| Developed On      |         OSX 10.10 |
| Required Packages | LLVM, Bison, Flex |
| LLVM Version      |               3.5 |
| GNU Bison         |               2.3 |
| Apple Flex        |            2.5.35 |

## Table of Contents

* [LLVM Setup](#LLVM-Setup)

### LLVM Setup

| *Dependencies for LLVM*    | Minimum Version  | Version Used for Development |
|----------------------------|-----------------:|-----------------------------:|
| GNU Make                   |        >= 3.79   |                        3.81  |
| GCC or Clang               |    >= 4.7 or 3.8 |               (Clang) 3.8.0  |
| Python                     |        >= 2.7    |                      2.7.10  |
| GNU M4                     |           1.4    |                       1.4.6  |
| GNU Autoconf               |           2.6    |                        2.69  |
| GNU Automake               |           1.9    |                       1.9.6  |
| libtool                    |       >= 1.5.22  |                       2.4.6  |
| zlib                       |       >= 1.2.3.4 |                       1.2.8  |


##### (1) Background
Taken from its official description:

  > LLVM is a library that is used to construct, optimize and produce intermediate and/or binary machine code. LLVM can be used as a compiler framework, where you provide the "front end" (parser and lexer) and the "back end" (code that converts LLVM's representation to actual machine code).

Before using this repo or developing for the repo, llvm must be configured on your system. All code files are specifically developed alongside version `3.5.0` and will, more than likely, result in many errors when built with older or newer versions. Specifically, the libraries included in header files will differ in both name and location that how they are identified in `.cpp` files here. Other errors are bound to occur as well.

##### (2) Setup
The repo comes with a setup script to automate the process. The script's main jobs are to:
  * download the source code of llvm
  * configure and build llvm
  * install the results to the user's include directory

        script/setup_llvm

##### (3) Test for Successful Installation

TODO


