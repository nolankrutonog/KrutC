# KrutC
(pronounced 'kroot-see')

(This README is incomplete, but I'm working on it!)
## Forward 
In my last quarter at Stanford University, I had the opportunity to take a compilers class with Professor Fredrik Kjolstad. Surprisingly, I gained a large affinity towards compilers and their complex, yet beautiful design. Mainly, though, I found their study a relaxing break from the challenges of OS design/implementation I had embarked upon in the quarters prior. Compilers need not worry about multithreaded programming, as correctness is vital over performance. I became so fascinated by them, I purchased the optional Compilers textbook (the purple dragon) and began reading it like a novel. At the same time, I heard a Lex Fridman podcast with Chris Lattner, the founder of Modulo, a company whose main goal was to create a compiled Python language called Mojo while allowing for backwards compatibility. I was hooked. I spoke with Kjolstad and asked him what I can do to begin my journey as a SWE who has an interest in compilers, because I looked online and all compiler jobs require 3+ years of experience. He suggested that I create a simple compiler with the help of LLVM to get my name out there. Within the following pages, is my best attempt at creating a compiler.

## Purpose
What makes Python a beautiful language for beginners is two fold: its syntax and its scripted nature. As the projects become more advanced, the package managing and deplyoing becomes really useful. 

KrutC is designed to be a scripted language, like Python, but with C-style syntax. It is also typed and object oriented. In my first year of programming, after enrolling in courses taught in C and C++, the typed system just made sense. However, I knew little regarding Makefiles, cmake, clang, gcc, etc. and it proved challenging to write my own code in C or C++ without the provided repository of the class I was enrolled in. I found this annoying. Why couldn't I write my own C++ scripts like I can do in Python?

I decided to do something about it. 

KrutC is a language for programmers who wish to learn how to program in type-safe and object-oriented languages. Hopefully, it combines some C++ syntax, functionality, and basic objects with Python's ease of writing. 

# Table of Contents
1. Introduction
2. Statements
3. Expressions
4. Builtin Methods
5. Type System

# Introduction 
All code in a KrutC file (`.krut`) is organized into one **statement list**. The code is executed from top to bottom. 

## Notation
* `[...]` -- optional elements
* `<...>` -- user-defined values
* `?` -- preceding element can appear zero or one times
* `*` -- preceding element can appear zero or more times

# Statements
Every **statement** must be one of the following types:
- **class**
- **feature**
- **for**
- **while**
- **if**
- **formal**
- **expression**

The code in a `.krutc` file is executed from top to bottom. 

## Class
A *Class* is defined as: 
```
class <type_1> [inherits <type_2> [, <type_n>]*]? {
	[feature_list]
}
```

### Requirements
* Class names must begin with a capital letter and be unique.
* Multiple inheritance is allowed, but cyclic inheritance is prohibited.
* All attributes are private, and all methods are public.
* Classes must be defined in the outer scope.

### Single Inheritance
```
class Vehicle {
	int model_year;
	string owner;
	void accelerate() {...}
	void brake() {...}
	void fillup(int size) {...}
	...
}

class Car inherits Vehicle {
	int gas_level;
	void fillup(int gallons) {...}
	...
}

class ElectricCar inherits Vehicle {
	int charge_level;
	void fillup(int megawatts) {...}
	...
}
```
ElectricCar and Car both inherit Vehicle, which means they have the attributes *model_year* and *owner*. They also have the methods *accelerate, brake, and fillup*. Methods in inherited classes can be redefined as long as the method name, return type, and formal list are the same. ElectricCar will use megawatts to *fillup()*, and Car will use gallons to *fillup()*.

### Multiple Inheritance
```
class A {
	int a1;
	int m1() {
		return 1
	}
}

class B {
	int a2;
	int m1() {
		return 2;
	}
}

class C inherits A, B {}

C my_c = new C;
int i = my_c.m1();
print(to_string(i)); // returns 1
```
Class **C** has access to attributes *a1* and *a2*. However, it only has one method *m1()* which it inherits from class **A** because **A** is listed first in the inherited list.
## Feature
Features can be either methods or attributes. 

### Method
```
<type> <name>(<formal_list>) {
	[statement_list]
}
```

#### Requirements
* Method names must begin with lowercase letters and be unique within their scope
* Return type must be consistent with method's implementation

### Attribute
```
<type> <name> [= <expression>]?;
```

## For
```
for ([statement]; [condition]; [repeat]) {
	[statement_list]
}
```
* **statement**: executed once
* **condition**: a bool, which if evaluated to false will break the loop
* **repeat**: a statement executed every loop


## While

```
while (condition) {
	[statment_list]
}
```
* **condition**: bool, which if evaluated to false will break the loop

For and while loops can be broken via keyword **break**.
For and while loops can be sent to the next iteration via keyword **continue**.

## If
```
if (condition) {
	[then statment_list]
} [else {
	[else statement_list]
}]
```
The else branch is optional.

## Formal
A **formal** is a statement of the form:
```
<type> <id>
```
Formal list -- a comma separated list of formals used in method definition.


# Expressions
An **expression** is a type of statement, but can be any one of the following:
- **int_const**
- **deci_const**
- **string_const**
- **bool_const**
- **char_const**
- **list**
- **dispatch**
- **binary operator**
- **new**

## Constants
* **Integer**: stored as C++ long (ex: `int i = 200;`)
* **Decimal**: stored as C++ double (ex: `deci i = 1.23;`)
* **String**: maximum 1024 characters (ex: `string s = "Hello, world!";`)
* **Boolean**: can only be `true` or `false` (ex: `bool b = true;`)
* **Character**: one character, only in single quotes (ex: `char c = 'c';`)

## List
A **list** expression is made up of comma-separated values, surrounded by brackets.
*Example*:
`list<int> my_list = [1, 2, 3, 4, 5];`
### List Methods
* syntax: `my_list.method();`
```
int length() // returns number of elements in the list
void clear() // empties the list
bool is_empty() // returns true is list is empty, false otherwise
void push_back(object o) // appends an element to the end of the list
void push_front(object o) // appends an element to the front of the list
void pop_back() // removes last element from list
void pop_front() // removes first element from list
		// runtime errors occur if list is empty
object front() // returns the object at the front of the list
object back() // returns the object at the back of the list
int contains(object o) // if o is in list, returns the index of o, else -1
```
### Bracket Operations
`my_list[n]...` -- the nth element in the list (runtime errors if list.length() < n)
`my_list[n:m]...` -- returns a sublist starting at index `n` until index `m`. (n & m are both optional)

The **int**s `n, m` are optional. If `n` is not used, then `n` is given a value of 0. If `m` is unused, then `m` is given a value of `my_list.length()`.

## Dispatch
Calling a method
```
int method() {...}

int i = method();
```

## Binary Operators

### Assigning Operators
* `=, +=, -=, *=, /=`
The left hand side of these binary operators must be in reference to a variable, and the left and right hand sides must be of the same type, evaluating to the type of the left hand side

### Boolean Operators
* `<, >, <=, >=, ==, !=, &&, ||`
The left and right hand sides of these operators must be of the same type, and the entire expression will evaluate to a bool

### Arithmetic Operators
* `+, -, *, /`
The left and right hand sides must be of the same type, and the entire expression will evaluate to the type of the left hand side. 

### Special Cases
* `int 'op' deci;` will result in a compiler warning, turning the int into type deci
* `string1 + string2; string1 += string2;` are the only operators allowed on strings, and will append `string2` to the end of `string1`

## New
Creating an instance of a class
```
class A {...}
A a = new A;
```

# Builtin Methods
`int main(list<string> argv) {...}`
* The *main* method is an optional method. It must return an int and have one argument `list<string> argv`.
* When a `KrutC` program is called, the entire file is executed top to bottom, then the *main* method is invoked if *main* is present.

`void print(string s)`
* input must be a string, and will print to console

`string input()`
* awaits string input from the console

`string to_string(object o)`
* turns any basic object into its string representation

`object type_of(object o)`
* returns the type of `o`

`deci abs(deci d)`
* returns the absolute value of d

`deci sum(list<deci> l)`
* returns the sum of `l`

`deci min(list<deci> l)`
* returns the min value of `l`

`deci max(list<deci> l)`
* returns the max value of `l`

`void kill(string err)`
* immediately kills the program, printing `err` to the console

# Type System
## Basic Types:
* `int`
* `deci`
* `string`
* `bool`
* `char`
* `list<object>`




