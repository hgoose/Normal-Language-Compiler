<h1 style="text-align: center;"> Normal Language Compiler </h1>

<h3 style="text-align: center;"> Elegant compilation to x86-64 written in cpp20</h3>

> nlc is a compiler written in C++ which targets Intel x86-64 architecture. The language, which I call `normal` is equipped with the following features:

> This project is an exercise in Compiler Theory and demonstrates buffering source text, lexing, parsing, semantic analysis, and code generation.

#### Vim syntax highlighting

If your interested in getting syntax highlighting for Vim / Neovim. Simply move `ftdetect/`, `ftplugin/`, and `syntax/` to `$HOME/.config/vim` or `$HOME/.config/nvim`. All three directories can be found in `syntax/`. 

If you already manage these three directories on your machine, simply move the files that they contain.

Note that Normal programs must contain the file extension `.nl`

#### Comments

Normal defines both line comments and block comments:
```
// Line comment

/* 
    Block comment
*/
```

Which are both of course ignored by the compiler.


#### Types and initialization / assignment

The datatypes are partitioned by the following two categories

1. **Assignable**: int
2. **Unassignable**: booleans (true/false), and strings

Booleans are denoted by the keywords `true`, `false`, and are implemented under the hood as `1` (true) and `0` (false).

Variables can be initialized by specifying an assignable type followed by an identifier and an optional initializer. More declarations can follow the first by use of a comma. These further initializations assume the type of the first.

```
int a = 1;
int b = 2, c = 3;
```

Variables are assigned values as you would expect

```
int a,b;
a = 20, b = 30;
```

Global variables are assigned static memory, whereas function scoped variables (and variables that reside in scopes inside a function) reside in stack memory. Uninitialized integer globals take on the value zero by default. 

#### Scope
Scope works in the usual way. Scope blocks can be created with `{...}`.

```
int a = 10;
{
    int a = 20;
    print(a); // 20
}
print(a); // 10
```

#### Keywords

* print, read, 
* int, true, false, void, 
* call, invoke, fn, proc, procedure, return,
* while, for, if, else,

#### Printing

We can print arithmetic, logical, relational expressions, rvalue strings, and variables with the `print` builtin.

```C
int a = 10;
print(a*2); // 20
print("\u000048ello, world!\n"); // Hello, world!
```

Print statements can take a variable number of arguments of any type.

```
print("Hello,", "world", "\n", 20, true);
```

Notice that statements must be delimited by a semicolon.

#### Operators
Normal carries the standard set of operations.

* +,-
* *,/,mod
* ^ (exponentiation)
* & (and), | (or), ! (not)
* ==, !=, <,<=,>,>=

**Note**: +,- both serve as binary addition / subtraction and unary plus / unary negation. For example,

```C
print(+2--2+-4); // 0
```
Unary plus and unary negation cannot be chained.

```C
print(--2); // Illegal
```
A relational expression requires that both operands be arithmetic.

#### Reading input

A `read` statement can read in an integer from stdin and place in an integer variable.

```C
int a;
read(a);
print(a + 20);

> 10
30
```

Reading another other than an integer is UB.

#### Branching

Normal admits the standard if-else branching logic.

```
int a = 10;

if (a < 10) {
    print("Hello.");
} 

else if (a > 10) {
    print("Goodbye.");
} 

else {
    print("a is equal to 10");
}
```

Conditionals can of course be nested:

```C
int a = 11;
if (a < 10) {
    int b = 20;

    if (b < 20) {
        print("Hello");
    }

    else if (b > 20) {
        print("goodbye");
    }

    else {
        print("B is 20");
    }
} 

else if (a > 10) { 
    print("a>10");
}

else {
    print("a==10");
}
```

They can also be either terminated immediately, or followed by a single statement:

```C
int a = 10;
if (a==10);
if (a==10) print(a);
```


#### Looping

Normal demonstrates both `while` and `for`, with the usual C-like syntax.

```C
int a = 5;
while (a > 0) {
    print(a);
    a=a-1;
}

while (true) {
    print("Infinite loop");
}
```

```C
for (int i = 0; i < 5; i=i+1) {...}

int i=0;
for (; i<5; i+i+1) {...}

for(;;) print("Infinite loop");

for(;;);
```

The update statement can be any statement you like.

```C
for (;;print("infinite loop"));
for (;;if(true)print("Hello, world!"));
```


#### Functions

Functions assume the following form

```C
fn name(int a1, int a2, ...) -> rv {...}
proc name(int a1, int a2, ...) -> rv {...}
procedure name(int a1, int a2, ...) -> rv {...}
```

No two functions can bear the same name, even if their argument lists or return values differ. The return value of a function can be either `int` or `void`.

Functions can be invoked simply by specifying its name and parameter pack.

```C
fn main(int x, int y) -> void {
    print(x,",",y);
}
main(1,2); // 1,2
```

There can be no default arguments and the number of parameters must equal the number of arguments.

Functions can also be called with the keywords `call` and `invoke`.

```C
fn main(int x, int y) -> void {
    print(x,",",y);
}
call main(1,2); // 1,2
invoke main(1,2); // 1,2
```

We can return a value with the `return` keyword.

```C
fn main(int x) -> int {
    return x+5;
}
```

When a function is called with keywords `call` or `invoke`, it is not possible to use the return value in an expression or statement, unlike calling by name.

```C
int x = invoke main(10); // Illegal
int y = main(10); // Good
```

A function can also be empty

```
fn main() -> void;
```

A function name will not clash with a variable name in the internal symbol table. Furthermore, a function can be declared  prior to its definiton.

```
fn main() -> void;
fn main() -> void {}

int f = 20;
fn f() -> void {}
```


#### Future features

The following features are under implementation.

1. Arrays
2. Float, string, bool, char types
3. Bitwise operations
4. Break, continue
5. Reserved word aliasing
6. More involved function signatures
7. The += operator and its siblings
8. Pre/post increment/decrement

#### Foreword

At the moment, source text is read and compiled to x86 bytes, which are placed in an executable buffer and executed just before compiler termination. Hence, When a user runs the Normal compiler against a Normal program, the output is sent to stdout and no exectuable is produced.

On top of receiving output to stdout, a view of the programs abstract syntax tree is shown, as well as the total bytes that the compiler emitted.

Producing an ELF compliant executable is under implementation.

An example of output is:

```C
Code tree:
block
  print
    Hello, world!
Code size: 28 bytes.
Code execution: 
Hello, world!
```

with respect to the program

```
print("Hello, world!");
```
