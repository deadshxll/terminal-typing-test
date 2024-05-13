# Terminal Typing Test
A command line typing test for the terminal similar to monkeytype.com for Windows

![demo-ezgif com-optimize](https://github.com/deadshxll/terminal-typing-test/assets/67878277/f2b817e1-a77c-4ab1-b6e6-806275ed345d)

### What is a Typing Test?
A typing test is a 15-60 second test that determines your typing speed, AKA words per minute (WPM). Typing tests are a fun way to practice your typing skills and web based variations exist, one of which is a very well known one; https://monkeytype.com/

### Why a Terminal Typing Test?
Many developers and other roles (I included) enjoy using a terminal as their workspace due to its power and flexibility, and sometimes enjoy doing typing tests to warm up their typing or to just practice in general, so integrating it into a terminal command is fitting for the group of developers that use a terminal as their workspace.

## How to compile
This small project is written in C, which means a compiler like `gcc` or `clang` is required to compile. If you do not want to compile the project, you can download it from the [releases section on this repository](https://github.com/deadshxll/terminal-typing-test/releases/tag/release).

**! Keep in mind that this variation works for Windows due to the libraries used! !**

<br>

1) First clone and change directory:
```batch
git clone https://github.com/deadshxll/terminal-typing-test.git
cd terminal-typing-test
```

2) Then compile it using the compiler you have on your system

Clang (LLVM):
```batch
clang terminal_typing_test.c -o tt.exe
```

GCC (MinGW): 
```batch
gcc terminal_typing_test.c -o tt.exe
```

## Honourable mentions

- 'tt' by lemnos ([click here](https://github.com/lemnos/tt))
> Lemnos's repository provides a similar but elegant solution for Linux and OSX

<br>

- 'monkeytype' ([click here](https://github.com/monkeytypegame/monkeytype))
> Monkeytype is the original motivator for this project, it's a highly configurable, clean, web based typing test.
