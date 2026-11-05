# Virtual Machine

A small stack-based virtual machine written in C++. The repository includes an assembler/compiler that turns a text program into a binary code frame, plus a VM that executes it. The VM also has 16×16 video memory and renders its output in a window using [raylib](https://www.raylib.com/).

## Components

- **Compiler** (`compiler.cpp`) — parses assembly-like source, resolves labels, and writes a binary executable.
- **Virtual machine** (`vm.cpp`, `vm_instructions.cpp`) — loads and runs the compiled program.
- **Disassembler** (`disassembler.cpp`) — converts numeric instruction listings into instruction names. Its input format is a text listing of instruction codes and arguments.
- **Stack** (`stack.cpp`, `stack.h`) — stack implementation used by the VM.

## Requirements

- A C++ compiler with support for C++14 or later.
- [raylib](https://www.raylib.com/) and its platform dependencies to build and run the graphical VM.
- The checked-in `.bat` scripts are configured for Windows. In particular, the Clang scripts use an MSYS2 UCRT64 sysroot; adjust their compiler and linker options for your local setup if needed.

The repository includes raylib headers and a static library, but the library is platform-specific. Use a raylib build compatible with your compiler and target platform.

## Build

On Windows, the repository provides build scripts:

- `clang_flags_compiler.bat` builds the compiler.
- `clang_flags_vm.bat` builds the VM.
- `clang_flags.bat` contains an additional VM build/run command using `g++`.

Review and edit the compiler paths, target/sysroot, and raylib linker options in these scripts to match your environment before running them. The compiler source consists of `compiler.cpp` and `stack.cpp`; the VM consists of `vm.cpp`, `vm_instructions.cpp`, and `stack.cpp`, and must be linked with raylib and the system libraries required by raylib.

## Run a program

Create a text source file using the instruction names below, then compile it and pass the generated binary to the VM:

```sh
compiler.exe program.asm program.bin
vm.exe program.bin
```

The compiler expects two arguments: the source file and the output executable file. The VM expects one argument: the compiled executable file. When execution completes successfully, the VM opens a window to display the video-memory output; close the window to exit.

## Instruction set

Arguments shown in angle brackets are required; instructions without an argument take none.

| Instruction | Argument | Purpose |
| --- | --- | --- |
| `PUSH` | `<number>` | Push a numeric value onto the stack. |
| `RPUSH`, `RPOP` | `<register>` | Move a value between the stack and a register. Registers are `AX`, `BX`, `CX`, and `DX`. |
| `ADD`, `SUB`, `MUL`, `DIV`, `SQRT` | — | Perform arithmetic using stack values. |
| `OUT` | — | Output a stack value. |
| `JMP` | `<label>` | Jump to a label. |
| `JA`, `JAE`, `JB`, `JBE`, `JE`, `JNE` | `<label>` | Conditional jumps based on stack values. |
| `CALL`, `RET` | `<label>` for `CALL`; none for `RET` | Call a label and return from it. |
| `MPUSH`, `MPOP` | `[<index>]` | Transfer values between the stack and RAM. |
| `MOUT` | — | Output RAM. |
| `VPUSH`, `VPOP` | `[<index>]` | Transfer values between the stack and video memory. |
| `VOUT` | — | Output video memory. |
| `HLT` | — | Halt execution. |

Labels are declared on their own line with a leading colon, for example `:loop`. The VM has 256 RAM cells and 16×16 video memory. Consult `vm_instructions.cpp` for the precise behavior and operand order of each instruction.

## Example

```asm
PUSH 2
PUSH 3
ADD
OUT
HLT
```

Save the program as `program.asm`, then compile and run it using the commands above.

## Repository contents

```text
compiler.cpp          Assembly compiler
vm.cpp                Virtual machine runner
vm_instructions.cpp   Instruction implementations
vm_instructions.h     VM declarations and instruction tables
stack.cpp / stack.h   Stack implementation
common.h              Instruction definitions and shared types
disassembler.cpp      Instruction-listing converter
raylib.h, raymath.h,
rlgl.h, libraylib.a   Bundled raylib files
```
