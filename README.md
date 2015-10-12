# mock-c-compiler

This compiler parses a c-like imperative language into llvm bytecodes. The base of this compiler is taken from lsegal's "my_toy_compiler" article and git repository. Huge thanks to him for his jumpstart on a normally very unapproachable task. Read more at [his article](http://gnuu.org/2009/09/18/writing-your-own-toy-compiler/) and his project repo on [github](https://github.com/lsegal/my_toy_compiler). 

Ideally future development will move from its currently lightweight to a more compelte C-compiler.

| *Quick Facts*     |                   |
|-------------------|------------------:|
| Language Spec     |       Subset of C |
| Developed On      |         OSX 10.10 |
| Required Packages | LLVM, Bison, Flex |
| LLVM Version      |               3.5 |
| GNU Bison         |               2.3 |
| Apple Flex        |            2.5.35 |
| CMake             |             3.3.1 |

## Table of Contents

* [LLVM Setup](#LLVM-Setup)
  * [Background](#Background)
  * [Setup](#Setup)
  * [Testing](#Testing)
* [Functionality so Far](#Basic Functionality)

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

##### Background
Taken from its official description:

  > LLVM is a library that is used to construct, optimize and produce intermediate and/or binary machine code. LLVM can be used as a compiler framework, where you provide the "front end" (parser and lexer) and the "back end" (code that converts LLVM's representation to actual machine code).

Before using this repo or developing for the repo, llvm must be configured on your system. All code files are specifically developed alongside version `3.5.0` and will, more than likely, result in many errors when built with older or newer versions. Specifically, the libraries included in header files will differ in both name and location that how they are identified in `.cpp` files here. Other errors are bound to occur as well.

##### Setup
The repo comes with a setup script to automate the process.

        script/setup_llvm

 The script's main jobs are to:
  * download the source code of llvm
  * configure and build llvm
  * install build to the user's /usr/local/include directory

##### Test for Successful Installation

Being able to run `make test` inside the repo should produce a result. If it fails, setup was unsuccessful. Perhaps the wrong version of llvm was installed or your $PATH needs to be adjusted. Any errors will be compile time errors usually with concerns to library file pathnames.

### Basic Functionality

Right now, the repo supports a compile test via  Makefile in the `src` directory. It can parse a basic function call supports some variable processing.
