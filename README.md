# KrutC
(pronounced 'kroot-see')

(This README is incomplete, but I'm working on it!)
## Forward 
In my last quarter at Stanford University, I had the opportunity to take a compilers class with Professor Fredrik Kjolstad. Surprisingly, I gained a large affinity towards compilers and their complex, yet beautiful design. Mainly, though, I found their study a relaxing break from the challenges of OS design/implementation I had embarked upon in the quarters prior. Compilers need not worry about multithreaded programming, as correctness is vital over performance. I became so fascinated by them, I purchased the optional Compilers textbook (the purple dragon) and began reading it like a novel. At the same time, I heard a Lex Fridman podcast with Chris Lattner, the founder of Modulo, a company whose main goal was to create a compiled Python language called Mojo while allowing for backwards compatibility. I was hooked. I spoke with Kjolstad and asked him what I can do to begin my journey as a SWE who has an interest in compilers, because I looked online and all compiler jobs require 3+ years of experience. He suggested that I create a simple compiler with the help of LLVM to get my name out there. Within the following pages, is my best attempt at creating a compiler.

## Purpose
What makes Python a beautiful language for beginners is two fold: its syntax and its scripted nature. As the projects become more advanced, the package managing and deplyoing becomes really useful. 

KrutC is designed to be a scripted language, like Python, but with C-style syntax. It is also typed and object oriented. In my first year of programming, after enrolling in courses taught in C and C++, the typed system just made sense. However, I knew little regarding Makefiles, cmake, clang, gcc, etc. and it proved challenging to write my own code in C or C++ without the provided repository of the class I was enrolled in. I found this annoying. Why couldn't I write my own C++ scripts like I can do in Python?

I decided to do something about it. 

KrutC is a language for programmers who wish to learn how to program in type-safe and object-oriented languages. It combines some C++ syntax, functionality, and basic objects with Python's ease of writing. 

# Statements
All code in a KrutC file (`.krut`) is organized into one **statement list**. Every **statement** must be one of the following types:
- **class**
- **feature**
- **for**
- **while**
- **if**
- **formal**
- **expression**

The code in a `.krutc` file is executed from top to bottom. 

## Class
A *Class* is a statement of the form: 
```
class <type_1> [inherits <type_2> [, <type_n>]*]? {
	[feature_list]
}
```
In the following sections, I will be using different notations:
* `[...]` -- anything within square brackets is optional
* `<...>` -- anything within carets is a user-defined value
* `?` -- anything before it can appear zero or one times
* `*` -- anything before it can appear zero or more times


In the code above, a **class** is being declared with the name **type_1**. It can either inherit no other **class**es, one **class** of name **type_2**, or a comma-separated list of **class**es. When a **class `<A>` inherits** one or more **class**es, it has access to the intersection of each inheritee's **attributes** and **methods**.

Each **class** contains a **feature_list**, which will be defined in a later section. 

### Requirements
- Naming: each **class** must have their name begin with a capital letter, and must be a unique name among declared classes.
- Inheriting: multiple inheritance is allowed; however, classes cannot inherit in cycles. For example, the following code would not be allowed:
```
class A inherits C {}
class B inherits A {}
class C inherits B {}
```
- Features: All **attributes** defined in a **class** are private, and all **method**s defined in a **class** are public.
- All **class**es must be defined in the outer scope.

## Feature
A **feature** can be one of two types: **method** or **attribute**. 

### Method
A method is a statement of the form:
```
<type> <name>(<formal_list>) {
	[statement_list]
}
```

In the code above, a **method** with name *name* and return type of *type* is being declared in the current scope. The **statement_list** is a list of statements which are executed when the **method** *name* is called (dispatched). Each **method** contains a **formal_list**, which will be defined in a later section.

#### Requirements
* Naming: each **method** must have their name begin with a lowercase letter, and must be a unique **method** name in the current scope.

### Attribute
An attribute is a statement of the form:
```
<type> <name> [= <expression>]?;
```
In the code above, an **attribute** with name *name* of type *type* is being declared in the current scope. Optionally, it can be defined with value *expression*. 

## For
A **for** loop is a statment of the form:
```
for ([statement]; [condition]; [repeat]) {
	[statement_list]
}
```
A **for** loop is made up of four components. The **statement** is executed once at the start of the **for** loop. The **condition**, which is an expression of type bool, is checked before every iteration of the loop. If the **condition** returns false, then the loop returns. The **repeat** is an expression that is executed after every loop. The **statement_list** is a list of statements executed in every loop. 

Execution order of the **for** loop is as follows:
```
1. statement;
2. every statement in statment_list;
3. if condition = false --> break;
4. repeat;
5. goto 2;
```


## While
A **while** loop is a statment of the form:
```
while (condition) {
	[statment_list]
}
```
A **while** loop is made up of 2 components. The **condition** is an expression of type bool, and is checked before every iteration of the loop. If **condition** returns false, the loop breaks. The **statement_list** is executed on every iteration.

## If
An **if** statement is a statement of the form:
```
if (condition) {
	[then statment_list]
} [else {
	[else statement_list]
}]
```
An **if** statement is has a conditional. If the condition evaluates to true, the **then** branch is executed. Otherwise the **else** branch is executed. The **else** branch is optional.

## Formal
A **formal** is a statement of the form:
```
<type> <id>
```
Where **type** is a basic or user defined type, and id is an object id. A **formal** is used in a **method** when expressing the arguments required to call the method. All the **formal**s used as arguments result in the method's **formal_list**.


# Expressions
An **expression** is a type of statement, but can be any one of the following:
- **int_const**
- **string_const**
- **bool_const**
- **char_const**
- **list_const**
- **list_elem_ref**
- **sublist_expr**
- **return**
- **objectid**
- **dispatch**
- **binop**
- **continue**
- **break**
- **kill**

## Integer Constant
An **int** expression is any integer. The **int** is stored as a C++ **long**, with minimum value of -2^(32 - 1) and maximum value of 2^32 - 1.

*Example*:
`int i = 200;`

## String Constant
A **string** expression is represented as characters inside double quotes. 

*Example:*
`string s = "Hello, World!";`. 
The maximum number of characters is 1024.

## Bool Constant
A **bool** expression is represented as a C++ **int**, but can only be set via the keywords **true** and **false**.

*Example*:
`bool b = true;`

## Character Constant
A **char** can only be one character, and is enclosed in single quotes.

*Example*:
`char c = 't';`

## List Constant
A **list** expression is made up of comma-separated values, surrounded by brackets.
*Example*:
`list<int> my_list = [1, 2, 3, 4, 5];`

## Sublist
A **sublist** expression is made up of the original list's name, and two **int** types (will discuss typing in the future), separated by a colon.
*Example*:
```
int n = ...;
int m = ...;
list<int> my_list = [1, 2, 3, 4, 5];
list<int> sublist = my_list[n:m];
```
A sublist `sublist` of list `my_list` contains the all values in `my_list` starting inclusive from index `n` until index `m` exclusive. In this example, `sublist = [3, 4, 5]`. 

Runtime errors occur when:
- `n` < 0
- `n` > `m`
- `m` > `my_list.length()`

The **int**s `n, m` are optional. If `n` is not used, then `n` is given a value of 0. If `m` is unused, then `m` is given a value of `my_list.length()`.



