# C++ Reference Guide

---

## Table of Contents

1. [Glossary](#glossary)
2. [Types and Literals](#types-and-literals)
3. [Pointers](#pointers)
4. [References](#references)
5. [Strings](#strings)
6. [Classes](#classes)
7. [Inheritance and Virtual Functions](#inheritance-and-virtual-functions)
8. [Memory Management and RAII](#memory-management-and-raii)
9. [Arrays, Vectors, and Containers](#arrays-vectors-and-containers)
10. [Loops](#loops)
11. [Namespaces](#namespaces)
12. [Stack vs Heap and Address Space](#stack-vs-heap-and-address-space)
13. [The Build Pipeline](#the-build-pipeline)
14. [Multithreading](#multithreading)
15. [Embedded Software Layers](#embedded-software-layers)
16. [Optimization](#optimization)
17. [Bit Manipulation](#bit-manipulation)
18. [constexpr](#constexpr)
19. [unordered_map](#unordered_map)
20. [Move Semantics](#move-semantics)
21. [Header Guards](#header-guards)

---

## Glossary

Terms are grouped by category. If an interviewer uses a word you don't recognize, it's likely here.

### Object-oriented programming

**Access modifier / access specifier** — `public`, `private`, or `protected`. Controls who can access a class member.
- `public` — accessible from anywhere
- `private` — only accessible inside the class itself
- `protected` — accessible inside the class AND derived classes

```cpp
class Sensor {
private:    // only Sensor methods can access these
    float value;
protected:  // Sensor and any derived class can access these
    bool active;
public:     // anyone can access these
    float getValue() const { return value; }
};
```

**Polymorphism** — "many forms." The ability to treat different types through a common interface. A `TemperatureSensor` and a `PressureSensor` are both `Sensor`s — you can call `describe()` on either through a `Sensor&` reference and each responds correctly. Runtime polymorphism uses virtual functions. Compile time polymorphism uses templates.

**Abstraction** — hiding complexity behind a simple interface. You call `sensor.getValue()` without knowing how it reads hardware.

**Encapsulation** — bundling data and the methods that operate on it into one class, controlling access with `private`/`public`. Prevents outside code from putting an object in an invalid state.

**Inheritance** — a derived class takes on all members of a base class. Models "is-a" relationships: a `TemperatureSensor` IS A `Sensor`. The four pillars of OOP are encapsulation, abstraction, inheritance, and polymorphism.

**Base class / parent class / superclass** — the class being inherited from. All the same thing, different terms.

**Derived class / child class / subclass** — the class that inherits. All the same thing.

**Constructor** — special method called automatically when an object is created. Same name as the class, no return type.

**Destructor** — special method called automatically when an object goes out of scope. Same name as class prefixed with `~`. No arguments, no return type.

**Initializer list** — the `: member(value)` syntax in a constructor. Sets member variables before the constructor body runs. Preferred over assignment inside the body.

**Method / member function** — a function defined inside a class. Same thing, different terms.

**Member variable / data member / field** — a variable defined inside a class. All the same thing.

**Instance / object** — a specific realization of a class. `Sensor s(10.5f)` creates an instance called `s`.

**Instantiation** — the act of creating an instance from a class definition.

**this pointer** — inside a method, `this` is a pointer to the current object. Used to disambiguate member variables from parameters with the same name.

**Virtual function** — a method that can be overridden in derived classes with runtime dispatch via vtable.

**Pure virtual function** — a virtual function with `= 0`. Makes the class abstract — cannot be instantiated directly.

**Abstract class** — a class with at least one pure virtual function. Can only be used as a base class. Defines an interface that derived classes must implement.

**Interface** — in C++ typically a class with only pure virtual functions. Defines what a class must do without saying how.

**Override** — replacing a virtual base class method with a derived class implementation. The `override` keyword makes this explicit and lets the compiler verify it.

**Virtual dispatch / dynamic dispatch** — the mechanism behind runtime polymorphism. When you call a virtual method through a base class reference, the program looks up the correct implementation at runtime via the vtable.

**Vtable (virtual table)** — a hidden lookup table the compiler generates for classes with virtual methods. Each object carries a hidden pointer to its vtable.

**Object slicing** — when you store a derived class object in a base class variable (not pointer), the derived parts get cut off. Reason to store `unique_ptr<Sensor>` in containers instead of `Sensor` directly.

**Friend** — a function or class declared `friend` inside a class gets access to its private members. Used sparingly.

**Operator overloading** — defining custom behavior for operators (`+`, `==`, `<<` etc.) for user-defined types.

---

### Memory and pointers

**Pointer** — a variable that stores a memory address.

**Dereference** — following a pointer to get the value at its address. `*ptr` = go to that address.

**Address-of operator** — `&` when applied to a variable gives its memory address. `&x` = "the address of x."

**Null pointer** — a pointer holding address `0` (`nullptr`). Dereferencing it causes a segfault on Linux or silent corruption on bare metal.

**Dangling pointer** — a pointer that points to memory that has been freed or gone out of scope. Using it is undefined behavior.

**Memory leak** — heap memory that was allocated but never freed. Accumulates over time and can exhaust memory.

**Stack** — fast, automatic memory for local variables. Cleaned up when variables go out of scope. Limited size.

**Heap** — large, dynamic memory. Allocated with `new`/`make_unique`. Must be freed manually or via smart pointer.

**RAII (Resource Acquisition Is Initialization)** — resource lifetime is tied to object lifetime. Acquire in constructor, release in destructor. When the object goes out of scope, cleanup happens automatically.

**Smart pointer** — a class that wraps a raw pointer and manages its lifetime automatically. `unique_ptr`, `shared_ptr`, `weak_ptr`.

**unique_ptr** — single-owner smart pointer. Automatically deletes when it goes out of scope. Cannot be copied, only moved.

**shared_ptr** — multiple-owner smart pointer. Reference counted — freed when the last owner goes out of scope.

**Scope** — the region of code where a variable is valid. Local variables go out of scope at the closing `}` of their block.

**Segfault (segmentation fault)** — crash caused by accessing memory you're not allowed to touch.

**Undefined behavior (UB)** — code the C++ standard makes no guarantees about. Might crash, might corrupt memory, might appear to work.

**Stack overflow** — the stack exceeds its maximum size. On a microcontroller can corrupt hardware state silently.

**Memory-mapped I/O** — hardware peripherals accessed by reading and writing specific memory addresses. Same instructions as normal memory access.

**volatile** — keyword telling the compiler a variable can change outside normal program flow. Prevents the compiler from optimizing away reads. Required for hardware registers and variables shared with interrupt handlers.

**Aliasing** — two pointers referring to overlapping memory. Legal but dangerous — compiler may make incorrect optimization assumptions.

---

### C++ language features

**Declaration** — tells the compiler something exists (`void foo();`). No implementation.

**Definition** — the actual implementation (`void foo() { ... }`). Can only have one per program (One Definition Rule).

**Forward declaration** — declaring something before defining it, so the compiler knows it exists.

**Namespace** — a named scope that groups identifiers to prevent naming conflicts. `std::` is the standard library namespace.

**Template** — a blueprint for generating code for multiple types at compile time. Zero runtime overhead vs virtual functions.

**constexpr** — evaluated at compile time. Guaranteed to be a compile time constant. More strict than `const`.

**const** — cannot be modified after initialization. May still be evaluated at runtime.

**static** (in a class) — belongs to the class itself, not any instance. One copy shared by all objects.

**static** (local variable) — allocated once in data segment, persists between function calls.

**inline** — hint to compiler to expand the function at call site. Eliminates function call overhead.

**explicit** — prevents implicit conversions. A constructor marked `explicit` can't be used for implicit type conversion.

**auto** — compiler infers the type automatically. `auto x = 5;` — x is an `int`.

**nullptr** — the null pointer constant. Type-safe replacement for `NULL` or `0`.

**Reference** — an alias for an existing variable. Same memory, different name. Must be bound at declaration, can't be null, can't be rebound.

**Initializer list (`{}`)** — universal zero initialization. `int x = {}` initializes to 0. Prevents narrowing conversions.

**Range-based for loop** — `for (const auto& x : container)`. Iterates over each element without needing an index.

**Lambda** — an anonymous inline function. `[&](int x) { return x * 2; }`. The `[&]` captures local variables by reference.

**Operator `->>`** — dereference and access member. `ptr->method()` is shorthand for `(*ptr).method()`.

**Scope resolution operator (`::`)** — accesses a name within a namespace or class. `std::cout`, `Sensor::getValue`.

**One Definition Rule (ODR)** — each function, variable, or class can only be defined once across the entire program.

**Header guard / include guard** — `#pragma once` or `#ifndef` pattern. Prevents a header from being included multiple times.

**Preprocessor directive** — lines starting with `#`. Processed before compilation. `#include`, `#define`, `#ifdef`, `#pragma`.

**Macro** — a preprocessor text substitution defined with `#define`. `#define MAX(a,b) ((a)>(b)?(a):(b))`.

**Ternary operator** — `condition ? value_if_true : value_if_false`. One-line if/else that returns a value.

**Cast** — explicitly converting one type to another. `(uint32_t*)0x40000000` — treat this number as a pointer to uint32_t.

**Type qualifier** — `const` or `volatile`. Modifies the behavior of a type.

---

### Concurrency

**Thread** — an independent sequence of execution. A program starts with one thread. Additional threads run concurrently.

**Race condition** — two threads access the same data simultaneously and the result depends on timing.

**Mutex (mutual exclusion)** — a lock. Only one thread can hold it at a time. Others block and wait.

**lock_guard** — RAII wrapper for a mutex. Locks on construction, unlocks automatically when it goes out of scope.

**scoped_lock** — acquires multiple mutexes atomically. Deadlock-safe — handles ordering internally.

**Deadlock** — thread A holds lock 1 waiting for lock 2, thread B holds lock 2 waiting for lock 1. Both wait forever.

**Critical section** — a section of code that accesses shared data and must not be executed by more than one thread at a time.

**std::forward** — conditionally casts to rvalue reference. Used in template functions to preserve whether an argument was originally an lvalue or rvalue. Unlike `std::move` which always casts to rvalue, `std::forward` only casts to rvalue if the original argument was an rvalue. Used for "perfect forwarding" in template wrapper functions.

**std::move** — casts an lvalue to an rvalue reference. Signals "you can steal from this." Doesn't actually move anything — the move constructor or move assignment operator does the actual work.

**Atomic** — an operation guaranteed to complete without interruption. `std::atomic<int>` reads and writes that can't be partially observed by another thread.

**Context switch** — the OS saving one thread's state and loading another's. How the OS switches between threads.

**Thread-safe** — code that behaves correctly when called from multiple threads simultaneously.

**Interrupt** — hardware-triggered event that preempts whatever is running, executes a handler, then returns. Must be fast — no blocking, no heap allocation.

**ISR (Interrupt Service Routine)** — the handler function that runs when an interrupt fires.

**Interrupt vector table** — a table mapping interrupt numbers to handler function addresses. The CPU looks up the handler here when an interrupt fires.

**Interrupt storm** — an interrupt that fires repeatedly because its source was never cleared. CPU gets stuck in the handler.

**Process** — a completely independent running program with its own memory space. Threads exist within a process and share its memory. Killing a process doesn't affect other processes. One thread crashing can kill its entire process.

**Spinlock** — a lock where the waiting thread loops continuously checking if the lock is free. Wastes CPU but avoids context switch overhead. Used in embedded when waits are very short.

---

### Embedded specific

**SWaP** — Size, Weight, and Power. The three binding constraints in fielded military hardware.

**BSP (Board Support Package)** — the lowest software layer. Knows the specifics of a particular circuit board — pin mappings, clock speeds, memory layout.

**HAL (Hardware Abstraction Layer)** — a layer of software that provides a consistent interface to hardware regardless of the specific chip. Often includes the BSP and drivers.

**Driver** — software that exposes a clean interface to a hardware peripheral. Hides register-level details.

**Middleware** — software layer between drivers and application. Protocol handlers, message queues, data formatting.

**RTOS (Real-Time Operating System)** — an OS designed for deterministic, time-critical embedded applications. Examples: FreeRTOS, Zephyr, VxWorks.

**Deterministic** — predictable and consistent timing every time. Real-time systems require deterministic behavior.

**Bare metal** — running directly on hardware with no OS. Maximum control, maximum responsibility.

**Firmware** — software programmed into non-volatile memory on a device. Persists without power.

**Bootloader** — small program that runs before the main application. Initializes hardware and loads firmware. Enables field updates.

**Watchdog timer** — a hardware timer that resets the system if not regularly reset by software. Recovers from software hangs.

**DMA (Direct Memory Access)** — hardware that transfers data between memory and peripherals without CPU involvement. Faster and frees the CPU.

**GPIO (General Purpose Input/Output)** — pins on a microcontroller that can be configured as digital input or output.

**Memory-mapped register** — a hardware register accessed by reading/writing a specific memory address.

**Circular buffer / ring buffer** — a fixed-size buffer where read and write heads wrap around. Used for FIFO queues, especially between interrupt handlers and main thread.

**FIFO (First In First Out)** — data comes out in the order it went in. A queue. Circular buffers implement FIFOs.

**Cache line** — the unit of data the CPU loads from memory at once. Typically 64 bytes. Cache-friendly code keeps related data in the same cache line.

**Cache miss** — accessing data that isn't in the CPU cache. Forces a slow fetch from RAM. Linked lists cause cache misses because nodes are scattered in memory.

**Fixed-point arithmetic** — representing fractional numbers using integers with an implicit decimal point. Used in embedded when there's no hardware FPU. Avoids expensive floating-point operations.

**Endianness** — the order bytes are stored in memory for multi-byte values. Little-endian = least significant byte first (most common). Big-endian = most significant byte first. Matters when sending data over a network or between different systems.

---

### Build and toolchain

**Compiler** — translates source code to machine code. g++, clang++, armcc.

**Linker** — combines compiled object files into an executable. Resolves references between files.

**Object file (.o)** — compiled machine code with unresolved references. Output of compilation, input to linker.

**Toolchain** — the complete set of tools to build software. Compiler + linker + assembler + libraries.

**Cross-compiler** — a compiler that runs on one platform (your PC) but generates code for another (ARM microcontroller).

**Makefile** — a file describing how to build a project. Rules for which files to compile and how to link them.

**CMake** — a build system generator. Generates Makefiles or other build files from a higher-level description.

**Yocto / Buildroot** — tools for building custom embedded Linux distributions from source.

**Debug vs Release build** — debug includes symbols and disables optimizations (easier to debug). Release enables optimizations (faster, smaller). Asserts are typically compiled out in release.

**Optimization level** — `-O0` no optimization, `-O1` basic, `-O2` standard, `-O3` aggressive, `-Os` optimize for size. Embedded often uses `-Os`.

**Static analysis** — analyzing code without running it. Finds bugs like race conditions, null dereferences, memory leaks at compile time. Tools: Coverity, ThreadSanitizer, clang-tidy.

---

### Data structures

**Array** — fixed-size contiguous block of memory. O(1) random access. Cache friendly.

**Vector (std::vector)** — dynamic array. Grows automatically. Heap allocated internally.

**Linked list** — chain of nodes connected by pointers. O(1) insertion, O(n) traversal. Poor cache locality.

**Stack (data structure)** — LIFO (Last In First Out). Push adds to top, pop removes from top.

**Queue** — FIFO (First In First Out). Enqueue adds to back, dequeue removes from front.

**Hash map / unordered_map** — key-value store with O(1) average lookup. Like Python dict. Uses a hash function to find buckets.

**Tree** — hierarchical data structure. Binary search tree gives O(log n) lookup.

**Big O notation** — describes how an algorithm scales with input size. O(1) = constant, O(n) = linear, O(log n) = logarithmic, O(n²) = quadratic.


---

## Types and Literals

```cpp
int x = 10;
float y = 1.1f;      // f suffix = float literal, not double (double is default)
double d = 1.1;      // double by default
bool flag = true;
char c = 'a';        // single character, stored as ASCII value (97)
uint8_t small = 255; // 8-bit unsigned int, 0-255, common in embedded
uint32_t reg = 0;    // 32-bit unsigned, common for hardware registers
```

### Literal suffixes

```cpp
10.5f    // float
10.5     // double (default for decimals)
10.5l    // long double
10u      // unsigned int
10l      // long
10ul     // unsigned long
0xFF     // hexadecimal — common for hardware register values
0b1010   // binary — common for bitmasks
```

Mixing `float` and `double` in embedded can cause implicit promotions to `double`, which is expensive if there is no hardware FPU. Use the `f` suffix consistently.

---

## Pointers

Memory is a long row of numbered boxes (addresses). A pointer is a variable that stores one of those addresses.

```cpp
int x = 10;
int* ptr = &x;   // & = address-of. ptr holds the address of x
*ptr = 99;        // * = dereference. go to address in ptr, change value there
                  // x is now 99
```

Plain english:
- `int* ptr = &x` — find where x lives in memory, store that address in ptr
- `*ptr = 99` — go to the address stored in ptr, put 99 there

### Pointer arithmetic

Incrementing a pointer moves it forward by one element, not one byte. The compiler knows the type size and advances accordingly.

```cpp
int arr[5] = {1, 2, 3, 4, 5};
int* p = arr;        // points to first element
p++;                 // advance to next int (moves 4 bytes)
std::cout << *p;     // prints 2
```

`ptr[i]` and `*(ptr + i)` are identical — array indexing IS pointer arithmetic.

### Null pointer

A pointer holding address `0`. Dereferencing it causes a segfault on Linux or silent memory corruption on bare metal.

```cpp
int* ptr = nullptr;   // explicitly null — good practice when not yet assigned
*ptr = 10;            // crash on Linux, silent corruption on bare metal
```

### Hardware register pointers (embedded)

```cpp
volatile uint32_t* reg = (uint32_t*)0x40000000;
*reg = 0xFF;          // write to hardware register
uint32_t val = *reg;  // read from hardware register
```

`volatile` tells the compiler this value can change outside normal program flow. Without it, the compiler might optimize away repeated reads, assuming the value hasn't changed — wrong for hardware registers.

---

## References

A reference is an alias — the same memory location with a different name.

```cpp
int x = 10;
int& ref = x;    // ref IS x — same address, two names
ref = 50;         // x is now 50
x = 100;          // ref is now 100
```

### References vs pointers

| | Pointer | Reference |
|---|---|---|
| Can be null | Yes | No |
| Can be reassigned | Yes | No — bound at declaration |
| Syntax | `*ptr` or `ptr->` | use like a normal variable |

### Primary use case — function parameters

```cpp
void byValue(int x)           { x = 99; }  // copy — original unchanged
void byRef(int& x)            { x = 99; }  // modifies original
void byConstRef(const int& x) { }          // read only, no copy made

// const T& is the idiomatic way to pass large objects you only need to read
void process(const SensorData& data) { }
```

---

## Strings

### How string literals work

All string literals are processed at compile time and stored in a read-only data segment:

```
address 1000: [h][e][l][l][o][\0]   <- "hello"
address 2000: [w][o][r][l][d][\0]   <- "world"
```

At runtime you only move pointers — no copying, no allocating. Identical literals are deduplicated and stored only once.

### Three ways to declare strings

```cpp
// 1. const char* — pointer to read-only data segment
//    changeable pointer, unchangeable data
const char* cstr = "hello";   // cstr holds address 1000
cstr = "world";               // cstr now holds 2000 — "hello" untouched
// cstr[0] = 'H';             // undefined behavior — read only memory

// 2. char[] — writable copy on the stack
//    unchangeable pointer, changeable data
char buf[32] = "hello";       // 32-byte buffer on stack, characters copied in
buf[0] = 'H';                 // fine — your memory
// buf = "world";             // compiler error — can't reassign an array

// 3. std::string — modern C++
//    changeable everything, manages its own heap memory
std::string s = "hello";      // fresh heap-allocated copy
s[0] = 'H';                   // fine
s += " world";                // resizes automatically
```

### When to use each

| Type | Use when |
|------|----------|
| `std::string` | Default for application-level C++ |
| `const char*` | String constants, passing to C libraries |
| `char[]` | Fixed size buffers, need to modify, embedded |

---

## Classes

```cpp
class Sensor {
private:
    float value;    // only accessible inside the class
    bool active;

public:
    // constructor — initializer list sets members before body runs (preferred)
    Sensor(float initialValue) : value(initialValue), active(false) {
        std::cout << "Sensor initialized" << std::endl;
    }

    // destructor — called automatically when object goes out of scope
    ~Sensor() {
        std::cout << "Sensor destroyed" << std::endl;
    }

    // const after signature = promise not to modify any member variables
    float getValue() const { return value; }
    bool isActive() const  { return active; }

    void activate()        { active = true; }
    void setValue(float v) { value = v; }
};
```

### `this` pointer

Inside a method, `this` is a pointer to the current object. Used when a parameter name conflicts with a member name:

```cpp
void setValue(float value) {
    this->value = value;   // this->value = member, value = parameter
}
```

---

## Inheritance and Virtual Functions

```cpp
class TemperatureSensor : public Sensor {
private:
    std::string unit;

public:
    // must call parent constructor in initializer list
    TemperatureSensor(float initialValue, std::string u)
        : Sensor(initialValue), unit(u) { }

    ~TemperatureSensor() {
        // derived destructor runs FIRST, then base destructor automatically
    }

    // override = replaces parent virtual method
    // compiler error if no matching virtual in base — acts as a safety net
    void describe() const override {
        std::cout << getValue() << unit << std::endl;
    }
};
```

### Virtual functions

```cpp
class Sensor {
public:
    virtual void describe() const { }   // virtual = opt into dynamic dispatch
};

Sensor* s = new TemperatureSensor(98.6f, "F");
s->describe();   // WITHOUT virtual: calls Sensor::describe()        (static dispatch)
                 // WITH virtual:    calls TemperatureSensor::describe() (dynamic dispatch)
```

Without `virtual`, the compiler picks the function at compile time based on the reference/pointer type — always calls the base version. With `virtual`, a vtable lookup at runtime finds the correct version for the actual object type.

### Virtual inheritance — the diamond problem

```cpp
class Animal { public: int age; };
class Dog    : virtual public Animal { };   // virtual prevents duplicate base
class Robot  : virtual public Animal { };
class RoboDog : public Dog, public Robot { };
// without virtual: two copies of Animal, ambiguous access to age
// with virtual:    one shared copy of Animal
```

### Abstract classes

A class with at least one pure virtual function (`= 0`). Cannot be instantiated — only used as a base class interface.

```cpp
class SensorBase {
public:
    virtual float getValue() const = 0;    // pure virtual
    virtual void describe() const = 0;
    virtual ~SensorBase() { }              // always virtual destructor in base classes
};
// any derived class MUST implement all pure virtual methods
```

---

## Memory Management and RAII

### Manual allocation — avoid in modern C++

```cpp
Sensor* s = new Sensor(10.5f);   // heap allocation
delete s;                          // must free manually
s = nullptr;                       // good practice after delete
```

### RAII

Tie resource lifetime to object lifetime. Acquire in constructor, release in destructor. Cleanup is automatic when the object goes out of scope.

### unique_ptr — single owner

```cpp
#include <memory>

std::unique_ptr<Sensor> s = std::make_unique<Sensor>(10.5f);
s->getValue();    // -> because s is a pointer
// no delete needed — destructor frees memory automatically

// transfer ownership with std::move
std::unique_ptr<Sensor> s2 = std::move(s);   // s is now null, s2 owns it
```

### shared_ptr — multiple owners

```cpp
std::shared_ptr<Sensor> sp1 = std::make_shared<Sensor>(10.5f);
std::shared_ptr<Sensor> sp2 = sp1;   // both own it, reference count = 2
// freed when the LAST shared_ptr goes out of scope
```

Internally shared_ptr maintains a control block on the heap containing two atomic reference counts — one for strong refs (shared_ptr) and one for weak refs (weak_ptr). Atomic means thread-safe increment/decrement without a mutex.

### weak_ptr — non-owning observer

Observes an object owned by `shared_ptr` without contributing to the reference count. Lets you check if the object still exists without keeping it alive:

```cpp
std::shared_ptr<Sensor> sp = std::make_shared<Sensor>(10.5f);
std::weak_ptr<Sensor> wp = sp;   // doesn't increment refcount

// must lock() to get a temporary shared_ptr before using
if (auto locked = wp.lock()) {
    locked->getValue();   // object still alive
} else {
    // object was deleted — sp went out of scope somewhere
}
```

Primary use case — breaking circular references. If A holds a `shared_ptr` to B and B holds a `shared_ptr` to A, neither ever gets deleted (refcount never hits 0). Making one of them `weak_ptr` breaks the cycle.

### Why vector\<unique_ptr\<T\>\> instead of vector\<T\>

1. **Polymorphism** — storing base class pointers avoids object slicing. A `TemperatureSensor` stored as a `Sensor` loses its derived parts. A `unique_ptr<Sensor>` pointing to a `TemperatureSensor` keeps the full object intact on the heap.
2. **Stable addresses** — when a vector resizes it moves its elements. Direct objects would change address and invalidate any pointers to them. Heap objects pointed to by `unique_ptr` never move.

---

## Arrays, Vectors, and Containers

```cpp
// raw array — C style, fixed size at compile time, no bounds checking
int rawArr[5] = {1, 2, 3, 4, 5};

// std::array — fixed size, stack allocated, safer
#include <array>
std::array<int, 5> stdArr = {1, 2, 3, 4, 5};
stdArr.at(0);    // bounds checked — throws if out of range
stdArr[0];       // no bounds check — faster

// std::vector — dynamic size, heap buffer, grows automatically
#include <vector>
std::vector<int> vec = {1, 2, 3};
vec.push_back(4);      // like Python append()
vec.size();            // number of elements
```

### When to use each

| Type | Size | Allocation | Use when |
|------|------|------------|----------|
| `int[]` | Fixed, compile time | Stack | Bare metal, maximum performance |
| `std::array` | Fixed, compile time | Stack | Fixed size with safety features |
| `std::vector` | Dynamic, runtime | Heap internally | Size unknown at compile time |

`std::vector` and `std::string` objects sit on the stack but manage an internal heap buffer. Their destructors free it automatically — RAII.

---

## Loops

```cpp
std::vector<int> vec = {1, 2, 3};
int arr[5] = {1, 2, 3, 4, 5};

// range-based for — default when you just need each element
// works on raw arrays (known size), std::array, std::vector, std::string
// does NOT work on raw pointers — no size information
for (const auto& x : vec) {
    std::cout << x << std::endl;
}

// classic for — use when you need the index, non-sequential iteration,
// multiple arrays simultaneously, or a pointer + separate size variable
for (int i = 0; i < 5; i++) {
    std::cout << arr[i] << std::endl;
}

// iterating with a pointer — common in embedded
uint8_t buffer[8] = {0x01, 0x02, 0x03};
for (int i = 0; i < 8; i++) {
    process(buffer[i]);
}
```

---

## Namespaces

```cpp
// defining
namespace myapp {
    class MyClass { };
    int helper = 42;
}

// accessing with prefix
myapp::MyClass obj;

// consuming — drops prefix requirement (avoid in headers)
using namespace myapp;
MyClass obj;
```

`std::` is the standard library namespace. `using namespace std;` is convenient but bad practice in large codebases — causes naming conflicts.

---

## Stack vs Heap and Address Space

### How to know which you're using

```
used new, make_unique, or make_shared?  →  HEAP
just a local variable declaration?      →  STACK
std::vector or std::string?             →  object STACK, contents HEAP
```

### Address space layout

```
HIGH ADDRESSES
+------------------+
|      stack       |  local variables, grows downward
+------------------+
|        |         |
|        v         |
|                  |
|        ^         |
|        |         |
+------------------+
|       heap       |  new/malloc, grows upward
+------------------+
|   data segment   |  globals, string literals, static vars
+------------------+
|   code segment   |  compiled instructions (read only)
+------------------+
LOW ADDRESSES
```

### Where each thing lives

```cpp
int x = 10;                   // stack
new int(10)                   // heap
"hello"                       // data segment (read only)
static int count = 0;         // data segment
int global = 5;               // data segment (if global)
std::vector<int> v;           // v on stack, contents on heap
std::unique_ptr<Sensor> uptr; // uptr on stack, Sensor on heap
```

### Static local variables

```cpp
void countCalls() {
    static int count = 0;   // allocated ONCE in data segment, not stack
    count++;                 // value persists between calls
}
countCalls();   // count = 1
countCalls();   // count = 2
```

### ASLR

OS randomizes where stack and heap start on every run — security measure. On bare metal embedded there is no OS and no ASLR — addresses are fixed and deterministic. That is why you can hardcode hardware register addresses in embedded code.

---

## The Build Pipeline

### Stage 1: Preprocessing
Handles `#` directives before compilation.
- `#include` → paste file contents here
- `#define` → text substitution
- `#ifdef` → conditionally include or exclude code

### Stage 2: Compilation
Converts code to machine code, one `.cpp` at a time. Output: `.o` object file with unresolved references.

Errors here: syntax errors, type mismatches, unknown names.

### Stage 3: Linking
Connects all `.o` files and libraries. Resolves references.

Errors here: `"undefined reference to foo()"` — declared but never defined.

### Stage 4: Runtime
Program is executing.

Errors here: segfault, null dereference, out of bounds. Compiled fine but crashes while running.

### Compile time vs runtime

```cpp
// compile time
const int SIZE = 5;
std::array<int, 5> arr;          // size must be compile time constant
template<typename T> void foo(); // instantiated at compile time

// runtime
std::vector<int> v;              // size decided at runtime
virtual void foo();              // vtable lookup at runtime
new Sensor(10.5f);               // heap allocation at runtime
```

Templates = zero runtime overhead. Virtual functions = small vtable lookup cost. Embedded engineers prefer templates for performance-critical code.

---

## Multithreading

```cpp
#include <thread>
#include <mutex>
```

A thread is an independent sequence of execution. A program starts with one thread. Additional threads run concurrently.

### Basic thread

```cpp
void doWork() { std::cout << "working" << std::endl; }

int main() {
    std::thread t(doWork);   // spawn thread
    t.join();                // wait for it to finish
    return 0;
}
```

### Race condition

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 1000; i++) {
        counter++;   // NOT atomic — read, modify, write are three separate ops
                     // another thread can jump in between any of them
    }
}
// two threads doing this simultaneously will NOT reliably give 2000
```

### Mutex — the fix

```cpp
std::mutex mtx;

void increment() {
    for (int i = 0; i < 1000; i++) {
        mtx.lock();    // acquire — other threads block here
        counter++;
        mtx.unlock();  // release
    }
}
```

### lock_guard — RAII for mutexes (preferred)

```cpp
void increment() {
    for (int i = 0; i < 1000; i++) {
        std::lock_guard<std::mutex> lock(mtx);   // locks here
        counter++;
    }   // unlocks automatically — even if an exception is thrown
}
```

### Deadlock

Thread A holds lock 1 waiting for lock 2. Thread B holds lock 2 waiting for lock 1. Both wait forever. Fix: always acquire multiple locks in the same order everywhere.

### Interrupts vs threads

| | Thread | Interrupt |
|---|---|---|
| Triggered by | Software scheduler | Hardware event |
| Preempts current thread | No | Yes — immediately |
| Can block | Yes | No |
| Heap allocation | Yes | No |
| Shared variables need | `mutex` | `volatile` + `mutex` |

---

## Embedded Software Layers

```
+---------------------------+
|     your application      |
+---------------------------+
|        middleware          |  protocol handling, state machines
+---------------------------+
|      device drivers        |  hardware peripheral interface
+---------------------------+
|  BSP (board support pkg)  |  board-specific configuration
+---------------------------+
|        hardware            |
+---------------------------+
```

**BSP** — knows the specifics of a particular board. Which GPIO maps to which peripheral, clock speeds, memory layout. Your Arduino `setup()` was a tiny BSP.

**Device drivers** — exposes a clean interface to a hardware peripheral (I2C, SPI, UART, GPIO). Hides register-level details. On Linux: `drivers/i2c/`, `drivers/spi/` etc.

**Middleware** — protocol state machines, message queuing, data formatting. Examples: USB stack, TCP/IP stack, CAN bus handler.

---

## Optimization

### Performance

Profile first, optimize second. Tools: `perf`, `gprof`, `valgrind`.

- Avoid unnecessary copies → use `const` references
- Reduce heap allocation → prefer stack, reuse buffers
- Cache-friendly layout → arrays over linked lists
- Avoid virtual dispatch in tight loops → use templates
- Avoid floating point if no hardware FPU
- Use DMA for bulk transfers

### Memory

- Use smallest type that fits: `uint8_t` not `int` for 0-255 values
- Avoid dynamic allocation — static or stack is more predictable
- `static` local variables — allocated once in data segment

### Boot time

- Defer initialization — don't set up things until needed
- Minimize what runs at startup
- On embedded Linux: minimize kernel modules
- On bare metal: start peripherals in parallel where possible

---

## Bit Manipulation

Hardware registers are just numbers. Each bit controls something — a pin state, a flag, a mode. You need to read and write individual bits without touching the others.

### The six bitwise operators

```
&   AND
|   OR
^   XOR
~   NOT (bitwise complement)
<<  left shift
>>  right shift
```

### AND, OR, XOR, NOT

```
AND — 1 only if BOTH bits are 1
  1010
& 1100
  1000

OR — 1 if EITHER bit is 1
  1010
| 1100
  1110

XOR — 1 if bits are DIFFERENT (same = 0, different = 1)
  1010
^ 1100
  0110

NOT — flips every bit
~ 1010
  0101
```

Key XOR properties:
- `x ^ 0 = x` — XOR with 0 is a no-op, value unchanged
- `x ^ 1 = ~x` — XOR with all 1s flips every bit
- XOR with a mask flips only the bits where the mask has a 1

### Shifts

Left shift — moves bits left, fills right with zeros, multiplies by 2 per shift:
```
1 << 3  =  00000001 → 00001000  = 8
```

Right shift — moves bits right, fills left with zeros, divides by 2 per shift:
```
00001000 >> 2  =  00000010  = 2
```

`1 << n` gives you a number with only bit `n` set. This is the foundation of all register manipulation.

### The four fundamental operations

```cpp
// SET a bit — force it to 1 without touching others
reg |= (1 << n);

// CLEAR a bit — force it to 0 without touching others
reg &= ~(1 << n);

// TOGGLE a bit — flip it
reg ^= (1 << n);

// READ a bit — check if it's set
if (reg & (1 << n)) { /* bit n is 1 */ }
```

Why these work:
- **Set**: OR with a mask that has only bit n set. 0|1=1, 1|1=1 — forces the bit to 1, others unchanged.
- **Clear**: AND with inverted mask. `~(1<<n)` has 1s everywhere except bit n. 1&0=0 clears it, 1&1=1 leaves others unchanged.
- **Toggle**: XOR with mask. 0^1=1, 1^1=0 — flips only bit n.
- **Read**: AND isolates bit n. Result is nonzero if set, zero if clear.

### Utility functions

```cpp
void setBit(uint8_t& reg, int bit) {
    reg |= (1 << bit);
}

void clearBit(uint8_t& reg, int bit) {
    reg &= ~(1 << bit);
}

bool isBitSet(uint8_t reg, int bit) {
    return (reg & (1 << bit));
}
```

Note: `reg` is passed by reference in set/clear (need to modify original), by value in read (only reading, `uint8_t` is cheap to copy).

### Named bit positions

Never use magic numbers in production — give bit positions meaningful names:

```cpp
#define PIN_ENABLE    3
#define PIN_DIRECTION 5
#define PIN_STATUS    7

*CONTROL_REG |= (1 << PIN_ENABLE);
*CONTROL_REG &= ~(1 << PIN_DIRECTION);
if (*CONTROL_REG & (1 << PIN_STATUS)) { }
```

### Printing bits (useful for debugging)

```cpp
void printBits(uint8_t val) {
    for (int i = 7; i >= 0; i--) {
        std::cout << ((val >> i) & 1);  // shift bit i to position 0, read it
    }
    std::cout << " (" << (int)val << ")" << std::endl;
}
```

`(val >> i) & 1` — shift bit i down to position 0, then AND with 1 to isolate it. Gives a clean 0 or 1 for each bit position.

### Multi-bit fields — extract and pack

Registers often pack multiple values into one byte:

```
register: [7][6][5][4][3][2][1][0]
            |_upper_|  |_lower_|
```

```cpp
uint8_t reg = 0b10110101;

// extract upper nibble (bits 7-4)
uint8_t upper = (reg >> 4) & 0x0F;   // shift down, mask to 4 bits
// 0x0F = 0b00001111

// extract lower nibble (bits 3-0)
uint8_t lower = reg & 0x0F;           // just mask, no shift needed

// pack two nibbles back into one byte
uint8_t packed = (upper << 4) | lower;
// shift upper into position, OR lower in — they don't overlap so OR combines cleanly
```

### Setting a multi-bit field

```cpp
// set a field of `width` bits starting at `startBit` to `value`
void setField(uint8_t& reg, int startBit, int width, uint8_t value) {
    assert((value >> width) == 0);         // value must fit in width bits
    int mask = (1 << width) - 1;           // create mask of `width` ones
    // (1 << width) = 1 followed by width zeros
    // subtract 1 = width ones
    reg &= ~(mask << startBit);            // clear the field
    reg |= (value << startBit);            // set the field
}
```

### Checking multiple bits

```cpp
uint8_t status = 0b10110100;
uint8_t mask = (1 << 4) | (1 << 2);   // bits 4 and 2

// ALL bits in mask must be set
if ((status & mask) == mask) { }

// ANY bit in mask is set
if ((status & mask) != 0) { }
```

### Hardware register pointers

```cpp
// pointer to a specific memory address — the hardware register
volatile uint32_t* CONTROL_REG = (uint32_t*)0x40000000;

*CONTROL_REG |= (1 << 3);    // set bit 3
*CONTROL_REG &= ~(1 << 3);   // clear bit 3
uint32_t val = *CONTROL_REG; // read register
```

`volatile` is critical here — tells the compiler this value can change outside normal program flow (the hardware itself can change it). Without `volatile` the compiler might optimize away repeated reads, assuming the value hasn't changed. Wrong for hardware registers.

### ~ vs ! (common Python gotcha)

```cpp
~x   // bitwise NOT — flips all bits, result is still an integer
     // ~0 = 0xFF = truthy
     // ~1 = 0xFE = also truthy — this is the gotcha
     // bitwise NOT of any nonzero value is still nonzero

!x   // logical NOT — returns true or false
     // !0 = true
     // !1 = false — matches Python's `not`
```

Use `!` when you want boolean logic. Use `~` only when you want to flip bits.

---

## Unions and Bit Fields

### Struct vs Union

A struct allocates separate space for each member. A union allocates space for only the largest member — everything else overlaps with it.

```cpp
struct MyStruct {
    uint32_t a;   // 4 bytes
    uint8_t  b;   // 1 byte
    // total: 5 bytes — a and b are separate, independent
};

union MyUnion {
    uint32_t a;   // 4 bytes
    uint8_t  b;   // 1 byte
    // total: 4 bytes — a and b SHARE the same memory
};
```

Writing to `a` immediately changes what you read through `b` — they are the same bytes, just interpreted differently.

### Union size — nesting dolls

The union always allocates enough space for the largest member. All members share the same starting address — like nesting dolls, the biggest one sets the size, and smaller ones just don't reach the end.

```
union with three members: 9-bit, 8-bit, 5-bit struct
union size = 2 bytes (minimum to hold 9 bits)

all members start at the same address:
9-bit struct:  [x][x][x][x][x][x][x][x][x][ ][ ][ ][ ][ ][ ][ ]
8-bit struct:  [x][x][x][x][x][x][x][x][ ][ ][ ][ ][ ][ ][ ][ ]
5-bit struct:  [x][x][x][x][x][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]
```

Rule: **union size = size of largest member** (rounded up to alignment). It doesn't matter how many members there are or whether they overlap each other — only the biggest sets the size.

For hardware registers, always match intentionally — `uint32_t raw` for a 32-bit register, bit fields adding up to exactly 32 bits using `reserved` to fill gaps.

`raw` and `reserved` are just naming conventions, not special keywords:
- `raw` — a name for the member that accesses the whole register as one integer. Could be called anything, but `raw` is conventional in embedded code.
- `reserved` — a name for padding bit fields that fill unused bits. Hardware datasheets literally label unused bits as "reserved" — the convention in code matches that language.

```cpp
union ControlReg {
    uint32_t raw;         // "raw" = whole register as one number, named by convention
    struct {
        uint32_t enable    : 1;    // meaningful — controls something
        uint32_t direction : 1;    // meaningful — controls something
        uint32_t speed     : 2;    // meaningful — controls something
        uint32_t reserved  : 28;   // padding — fills remaining bits to reach 32
                                   // matches datasheet "reserved" bits
                                   // 1 + 1 + 2 + 28 = 32 bits = matches uint32_t
    } bits;
};
```

### Bit fields

Bit fields can be used in any struct or class. The `: n` syntax tells the compiler how many bits to allocate for that member:

```cpp
struct Flags {
    uint8_t enable    : 1;   // bit 0
    uint8_t direction : 1;   // bit 1
    uint8_t speed     : 2;   // bits 3-2
    uint8_t reserved  : 4;   // bits 7-4 — fill remaining bits
    // total: 8 bits = 1 byte, packed automatically by compiler
};

Flags f;
f.enable    = 1;      // set enable bit — no manual bit shifting needed
f.speed     = 0b11;   // set speed to 3
f.direction = 0;      // clear direction
```

**Caveat:** bit field layout is implementation defined — can vary between compilers. For portable code use manual bit manipulation. For code tied to a specific compiler and platform (which embedded usually is) bit fields are fine and widely used.

### Union + bit fields — the hardware register pattern

Combining unions and bit fields gives you named bit access AND raw register access at the same time:

```cpp
union ControlReg {
    uint32_t raw;         // access the whole register as one number
    struct {
        uint32_t enable    : 1;    // bit 0
        uint32_t direction : 1;    // bit 1
        uint32_t speed     : 2;    // bits 3-2
        uint32_t reserved  : 28;   // bits 31-4 — fill remaining bits
    } bits;
};

ControlReg reg;
reg.bits.enable = 1;      // set enable by name — no manual bit shifting
reg.bits.speed  = 2;      // set speed field
uint32_t val = reg.raw;   // read whole register as one number
```

You will see this pattern constantly in embedded HAL libraries and microcontroller header files.

### Memory aliasing

Two pointers to overlapping addresses is called aliasing. Legal but dangerous:

```cpp
uint32_t* p32 = (uint32_t*)0x1000;   // reads bytes 1000-1003
uint8_t*  p8  = (uint8_t*) 0x1001;   // reads byte 1001 — inside p32's range

*p8 = 0xFF;   // modifies byte 1001
// p32 now sees different data even though you didn't touch it directly
```

The compiler may not realize the pointers are aliased and can make incorrect optimizations. In embedded work aliasing is sometimes intentional (accessing sub-registers) but generally something to be careful about.

---

## Linked Lists

A linked list is a chain of nodes where each node contains data and a pointer to the next node. Nodes can be anywhere in memory — the pointers stitch them into a logical sequence regardless of physical location.

```
array in memory — contiguous:
[1][2][3][4][5]

linked list in memory — scattered:
[1|ptr] -> [3|ptr] -> [2|ptr] -> [5|ptr] -> null
  ^            ^           ^          ^
addr 1000   addr 5000   addr 2000  addr 8000
```

### Basic node structure

```cpp
struct Node {
    int data;
    Node* next;   // pointer to next node — nullptr if last node
};

Node* head = nullptr;   // start of the list

// insert at front
Node* newNode = new Node();
newNode->data = 42;
newNode->next = head;
head = newNode;

// traverse
Node* current = head;
while (current != nullptr) {
    std::cout << current->data << std::endl;
    current = current->next;
}
```

### Array vs linked list tradeoffs

| Operation | Array | Linked List |
|-----------|-------|-------------|
| Random access | O(1) — direct index | O(n) — must traverse |
| Insert at middle | O(n) — shift elements | O(1) — update pointers |
| Insert at front/back | O(n) / O(1) | O(1) |
| Cache friendliness | High — contiguous memory | Low — nodes scattered |
| Memory overhead | Low | Higher — pointer per node |

**Insertion in a linked list — just update two pointers:**

```
before: A -> C
after:  A -> B -> C

change A's next to point to B
set B's next to point to C
done — nothing moved in memory
```

**Insertion in an array — shift everything:**

```
insert 99 at index 2:
before: [1][2][3][4][5]
after:  [1][2][99][3][4][5]   <- 3, 4, 5 all had to move
```

### Cache locality — why arrays are faster to traverse

When the CPU reads memory it loads chunks called **cache lines** — typically 64 bytes. With an array, loading one element loads several neighbors since they are contiguous. Future accesses to those neighbors are essentially free — already in cache. This is **spatial locality**.

With a linked list, each node can be anywhere in memory. Following a pointer to the next node likely causes a **cache miss** — the CPU has to fetch that memory from RAM, which is slow. On a microcontroller this cost is significant.

This is why the blanket statement "arrays are more cache friendly than linked lists" needs nuance — linked lists are better for middle insertion, arrays are better for traversal and random access.

### Embedded pattern — node pool

Heap allocating each node individually is unpredictable and slow. A common embedded solution is to pre-allocate a fixed pool of nodes from which you link as needed:

```cpp
static constexpr int MAX_NODES = 32;
Node nodePool[MAX_NODES];    // pre-allocated on stack or data segment
bool inUse[MAX_NODES] = {};  // track which nodes are free

// allocate from pool — no heap, deterministic
Node* allocNode() {
    for (int i = 0; i < MAX_NODES; i++) {
        if (!inUse[i]) {
            inUse[i] = true;
            return &nodePool[i];
        }
    }
    return nullptr;   // pool exhausted
}
```

You get the insertion flexibility of a linked list with the predictable memory of a fixed array. The commenter's point about fitting nodes on the same cache line refers to this — if your node pool is an array, nodes allocated sequentially will be contiguous in memory and cache-friendly even though they're linked.

### When to use each in embedded

- **Array / std::array** — default choice. Fixed size, fast traversal, cache friendly, predictable memory.
- **Linked list** — when you need frequent middle insertion/deletion and traversal speed is less critical.
- **Node pool linked list** — when you need linked list flexibility but can't afford heap allocation at runtime.

---

## constexpr

`constexpr` means "evaluate this at compile time." The value is computed and baked into the binary before your program ever runs.

### const vs constexpr

```cpp
const int x = 100;          // const — can't modify, but might be runtime
const int y = getValue();   // const — definitely runtime, getValue() runs at runtime

constexpr int x = 100;          // must be known at compile time AND can't be modified
constexpr int y = getValue();   // only works if getValue() is also constexpr
```

`const` just means "can't modify after initialization." `constexpr` means "must be known at compile time AND can't be modified."

### Basic usage

```cpp
constexpr int MAX_VALUE    = 100;
constexpr int BUFFER_SIZE  = 64 * 1024;   // compiler computes 65536 at compile time
constexpr float PI         = 3.14159f;
constexpr bool DEBUG       = false;
```

### Why it matters — compile time constants unlock things runtime values can't

```cpp
constexpr int SIZE = 256;
uint8_t buffer[SIZE];        // works — array size must be compile time constant

const int size = 256;
uint8_t buffer[size];        // works in practice but technically implementation defined

int size = 256;
uint8_t buffer[size];        // VLA — variable length array, not standard C++
```

### static constexpr for class constants

```cpp
class Sensor {
    // non-static const — every instance gets its own copy, wasteful
    const int m_maxValue = 100;

    // static const — one copy shared across all instances, old style
    static const int MAX_VALUE = 100;

    // static constexpr — one copy, guaranteed compile time, modern preferred style
    static constexpr int MAX_VALUE     = 100;
    static constexpr float THRESHOLD   = 0.5f;
    static constexpr int BUFFER_SIZE   = 64 * 1024;  // compiler does the math
};
```

`static` — belongs to the class, not any instance. One copy shared by all objects.
`constexpr` — guaranteed compile time. Can be used anywhere a compile time constant is needed.

### constexpr functions

Functions can also be `constexpr` — the compiler evaluates them at compile time if all inputs are compile time constants:

```cpp
constexpr int square(int x) {
    return x * x;
}

constexpr int val = square(5);   // computed at compile time — val = 25
int runtime = square(n);         // n unknown at compile time — runs at runtime
```

### Value initialization with {}

Related to constexpr — you'll often see member variables initialized with `{}`:

```cpp
private:
    int m_count = {};      // initialized to 0
    float m_value = {};    // initialized to 0.0f
    bool m_active = {};    // initialized to false
    int* m_ptr = {};       // initialized to nullptr
```

`{}` is universal zero initialization — works for any type without needing to know the specific zero value. Also prevents narrowing conversions:

```cpp
int x = 3.14;    // compiles, silently truncates to 3
int x = {3.14};  // compiler error — narrowing conversion not allowed
```

### m_ naming convention

While not constexpr-specific, commonly seen alongside class member declarations:

```cpp
class Sensor {
private:
    float m_value = {};      // m_ prefix = member variable
    bool  m_active = {};     // immediately distinguishes members from parameters
    int   m_sampleRate = {};

public:
    void setValue(float value) {
        m_value = value;     // clear which is member (m_value) and which is param (value)
    }
};
```

Common prefixes: `m_` = member, `g_` = global, `s_` = static, `p_` = pointer. Style varies by codebase — consistency matters more than which prefix you pick.

---

---

## Interrupts

The CPU normally executes instructions sequentially. An interrupt is a signal from hardware that says "stop what you're doing, handle this urgent event, then come back."

```
normal execution:
instruction 1
instruction 2
instruction 3  <- interrupt fires here
    |
    v
ISR (Interrupt Service Routine) runs
    |
    v
instruction 4  <- resumes here
instruction 5
```

The CPU saves its current state (registers, program counter), jumps to the handler, executes it, restores state, and continues. The interrupted code has no idea this happened.

### Types of interrupts

**Hardware interrupts — triggered by external hardware:**
- Timer interrupt — fires at regular intervals (e.g. every 1ms)
- GPIO interrupt — pin changed state (button pressed, signal received)
- UART interrupt — data arrived on serial port
- I2C interrupt — I2C transaction complete
- ADC interrupt — analog to digital conversion complete
- DMA interrupt — data transfer complete

**Error/fault interrupts:**
- Hard fault — illegal memory access, invalid instruction
- Watchdog interrupt — system hasn't responded in time

**Software interrupts:**
- System calls — requesting OS services
- Exceptions — divide by zero, null pointer dereference

### Timer interrupt vs error interrupt

| | Timer Interrupt | Error Interrupt |
|---|---|---|
| Fires | Predictably, at fixed interval | Unexpectedly, when something goes wrong |
| Expected | Yes — you set it up intentionally | No |
| Handler job | Increment counter, sample sensor, schedule task | Diagnose, recover, or fail safely |
| Risk | Low | High — can cause interrupt storm |

### Interrupt storm — the infinite loop problem

If an error interrupt fires and the handler doesn't clear the error condition, the interrupt fires again immediately after returning — the CPU gets stuck in the handler forever:

```cpp
// BAD — forgot to clear the error flag
void ERROR_IRQHandler() {
    logError();
    // error flag still set — interrupt fires again immediately
}   // infinite loop, CPU never returns to normal code

// GOOD — always clear the interrupt source
void ERROR_IRQHandler() {
    logError();
    clearErrorFlag();      // tell hardware we handled it
    resetErrorSource();    // fix the condition that caused it
}   // returns cleanly, interrupt won't immediately re-fire
```

### Coding interrupts in C++

On bare metal, the handler name must match the interrupt vector table exactly:

```cpp
// extern "C" = use C-style naming — required because C++ mangles function names
// the vector table expects a specific name, mangling would break the lookup
extern "C" void TIMER0_IRQHandler() {
    tickCount++;
    TIMER0->SR &= ~(1 << 0);   // clear interrupt flag — MUST do this
}
```

On embedded Linux, handlers are registered through the kernel driver interface rather than mapping directly to a vector.

### Critical rules for interrupt handlers

```
1. Keep them SHORT — CPU can't do anything else while in the handler
2. Never block — no mutexes, no sleep, no waiting
3. Never allocate heap memory — new/malloc can block
4. Always clear the interrupt flag — or it fires again immediately
5. Mark shared variables volatile — compiler must re-read them every time
6. Use atomics or disable interrupts for shared data access
```

### Shared data between interrupt handler and main thread

You can't use a mutex in an interrupt handler — mutexes can block, handlers must never block. If the mutex is held when the interrupt fires, the handler tries to lock it, blocks, but the thread holding the mutex is now suspended waiting for the interrupt to finish — deadlock.

**Option 1 — volatile + disable interrupts briefly:**

```cpp
volatile int sharedData = 0;
volatile bool dataReady = false;

void UART_IRQHandler() {
    sharedData = UART->DR;     // read hardware register
    dataReady = true;          // signal main thread
    UART->SR &= ~(1 << 5);    // clear interrupt flag
}

void mainLoop() {
    if (dataReady) {
        __disable_irq();           // disable interrupts — critical section
        int data = sharedData;
        dataReady = false;
        __enable_irq();            // re-enable interrupts

        processData(data);
    }
}
```

**Option 2 — std::atomic (preferred in modern C++):**

```cpp
#include <atomic>
std::atomic<int>  sharedData{0};
std::atomic<bool> dataReady{false};

void UART_IRQHandler() {
    sharedData.store(UART->DR);   // atomic write — guaranteed uninterruptible
    dataReady.store(true);
}

void mainLoop() {
    if (dataReady.load()) {        // atomic read
        int data = sharedData.load();
        dataReady.store(false);
        processData(data);
    }
}
```

`std::atomic` guarantees the operation completes without interruption — no mutex needed, no disabling interrupts needed.

**Option 3 — circular buffer (best for streaming data):**

The cleanest solution for interrupt-to-main-thread communication. The interrupt handler writes into it, the main thread reads from it. Designed to be safe for single producer/single consumer without locks. See the Circular Buffer section.

### Interrupts vs threads

| | Thread | Interrupt |
|---|---|---|
| Triggered by | Software scheduler | Hardware event |
| Preempts current thread | No | Yes — immediately |
| Can block | Yes | No |
| Heap allocation | Yes | No |
| Shared variables need | `mutex` | `volatile` + atomic or disable interrupts |
| Response time | Depends on scheduler | Immediate — microseconds |

---

---

## Circular Buffer (Ring Buffer)

A fixed-size buffer where the write and read heads chase each other around in a circle. No shifting, no copying — just incrementing indices with modulo wrap-around. One of the most common data structures in embedded systems.

### The problem it solves

A producer (interrupt handler, hardware) generates data at one rate. A consumer (main thread) processes it at another rate. You need somewhere to store data in between without losing it and without blocking the producer.

### How it works

```
buffer: [ ][ ][ ][ ][ ][ ][ ][ ]
         ^                       ^
        tail                   head
       (read)                 (write)

after writing A, B, C:
[A][B][C][ ][ ][ ][ ][ ]
 ^        ^
tail     head

after reading A, B:
[A][B][C][ ][ ][ ][ ][ ]
       ^  ^
      tail head

after writing D, E, F, G, H, I (head wraps around):
[I][B][C][D][E][F][G][H]
    ^  ^
   tail head  <- head wrapped past end, overwrites old slot
```

Write head and read head chase each other around the buffer. When either reaches the end it wraps back to 0 using modulo. No elements ever move.

### Modulo wrap-around

```cpp
// without modulo — manual wrap
m_head++;
if (m_head >= CAPACITY) m_head = 0;

// with modulo — same thing, one line
m_head = (m_head + 1) % CAPACITY;

// example with CAPACITY = 8:
// 7 % 8 = 7
// 8 % 8 = 0  <- wraps
// 9 % 8 = 1
```

### Implementation

```cpp
#include <cstdint>

class CircularBuffer {
private:
    static constexpr int CAPACITY = 8;
    uint8_t m_buffer[CAPACITY];
    int m_head  = {};   // write index
    int m_tail  = {};   // read index
    int m_count = {};   // number of items currently in buffer

public:
    bool write(uint8_t data) {
        if (m_count == CAPACITY) return false;   // full
        m_buffer[m_head] = data;
        m_head = (m_head + 1) % CAPACITY;
        m_count++;
        return true;
    }

    bool read(uint8_t& data) {   // reference parameter — writes into caller's variable
        if (m_count == 0) return false;   // empty
        data = m_buffer[m_tail];
        m_tail = (m_tail + 1) % CAPACITY;
        m_count--;
        return true;
    }

    bool isFull()  const { return m_count == CAPACITY; }
    bool isEmpty() const { return m_count == 0; }
    int  count()   const { return m_count; }
};
```

### Why read() takes a reference parameter

Returning a value doesn't let you signal both success/failure AND provide the value simultaneously:

```cpp
// reference parameter pattern — idiomatic C++
bool read(uint8_t& data) {
    if (isEmpty()) return false;   // signal failure, data untouched
    data = m_buffer[m_tail];       // write into caller's variable
    return true;                   // signal success
}

// caller checks both:
uint8_t val;
if (buf.read(val)) {
    process(val);   // val is valid
} else {
    // buffer was empty, val is untouched
}
```

In modern C++ you'd use `std::optional` instead, but reference parameters work everywhere including restricted embedded environments.

### Full vs empty distinction

Both states have `m_count == 0` or `m_count == CAPACITY` — use a count to distinguish them. Alternative: waste one slot (buffer full when head is one behind tail) — avoids the count but wastes a slot.

### Why circular buffers are used for interrupt-to-main communication

- **No blocking** — interrupt handler can write without waiting
- **No locks needed** for single producer / single consumer
- **Fixed memory** — no heap allocation, deterministic
- **FIFO ordering** — data comes out in the order it went in

The interrupt handler writes into the buffer, the main thread reads from it at its own pace. As long as the main thread keeps up, no data is lost.

### Properties

| Property | Value |
|----------|-------|
| Write | O(1) |
| Read | O(1) |
| Memory | Fixed, stack allocated |
| Thread safe | Yes for single producer/single consumer |
| Heap allocation | None |

---

## unordered_map

*(coming soon)*

---

## Move Semantics

Move semantics let you transfer ownership of resources instead of copying them. If an object is about to be destroyed anyway, why copy its data? Just take it.

### lvalues vs rvalues

```cpp
int x = 5;        // x is an lvalue — has a name, persists, can take its address (&x)
int y = x + 1;    // (x + 1) is an rvalue — temporary, no name, about to be destroyed
```

- **lvalue** — has a name, persists beyond the expression, can take its address
- **rvalue** — temporary, no name, about to be destroyed

### The problem without move semantics

```cpp
std::vector<int> makeVector() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    return v;   // without move: copies the entire internal buffer
}

std::vector<int> result = makeVector();  // another copy — expensive
```

### Copy vs move

```cpp
std::vector<int> a = {1, 2, 3, 4, 5};

// copy — duplicates all data, both objects own their own buffer
std::vector<int> b = a;              // b gets its own copy, a unchanged

// move — transfers ownership, source left empty
std::vector<int> c = std::move(a);   // c takes a's buffer, a is now empty
// don't use a after moving from it
```

### std::move doesn't actually move anything

`std::move` is just a cast — it converts an lvalue into an rvalue reference, signaling "treat this as a temporary, you can steal from it." The actual stealing happens in the move constructor.

```cpp
std::move(a)   // just says "a is now rvalue, steal from it"
               // the vector's move constructor does the actual work
```

### Rvalue reference — &&

`&&` is an rvalue reference. Only binds to temporaries or things explicitly `std::move`d. The compiler calls the move constructor instead of the copy constructor when it sees `&&`.

```cpp
// copy constructor — takes lvalue reference
Buffer(const Buffer& other) {
    m_data = new uint8_t[other.m_size];
    memcpy(m_data, other.m_data, other.m_size);   // copy every byte — expensive
}

// move constructor — takes rvalue reference
Buffer(Buffer&& other) {
    m_data = other.m_data;    // steal the pointer — O(1)
    m_size = other.m_size;
    other.m_data = nullptr;   // leave source empty — safe to destruct
    other.m_size = 0;
}
```

### Where you'll see this in practice

```cpp
// unique_ptr — can only be moved, not copied
std::unique_ptr<Sensor> s = std::make_unique<Sensor>(10.5f);
std::unique_ptr<Sensor> s2 = std::move(s);   // s is now null, s2 owns it
// std::unique_ptr<Sensor> s3 = s;           // compile error — copy disabled

// inserting into containers
std::vector<std::string> names;
std::string name = "tyler";
names.push_back(std::move(name));   // moves name into vector, name now empty

// emplace_back — constructs directly inside the vector, no temporary at all
names.emplace_back("alice");        // passes "alice" to string constructor in place
// push_back creates a temporary then moves it — emplace_back skips the temporary
```

### push_back vs emplace_back

```cpp
std::vector<Sensor> sensors;

// push_back — constructs a temporary Sensor, then moves it into the vector
sensors.push_back(Sensor(10.5f));

// emplace_back — passes 10.5f directly to Sensor's constructor inside the vector
// no temporary created — constructed in place in the vector's memory
sensors.emplace_back(10.5f);
```

`emplace_back` is preferred — never worse, sometimes better.

### = delete — explicitly disabling copy

```cpp
class NonCopyable {
public:
    NonCopyable(const NonCopyable&) = delete;             // copy constructor disabled
    NonCopyable& operator=(const NonCopyable&) = delete;  // copy assignment disabled
    NonCopyable(NonCopyable&&) = default;                 // move is fine
};
```

`unique_ptr` does exactly this — copying is deleted, moving is allowed. That's why you can't copy a `unique_ptr`.

### The rule of five

If your class manages a resource (heap memory, file handle), define all five:

```cpp
class Buffer {
public:
    ~Buffer();                              // 1. destructor
    Buffer(const Buffer&);                  // 2. copy constructor
    Buffer& operator=(const Buffer&);       // 3. copy assignment
    Buffer(Buffer&&);                       // 4. move constructor
    Buffer& operator=(Buffer&&);            // 5. move assignment
};
```

If you define any one of these, the compiler's assumptions about the others break down — define all five.

### Key takeaways

- Move semantics avoid expensive copies when the source is a temporary or being discarded
- `std::move` is just a cast to rvalue reference — signals "steal from this"
- `&&` = rvalue reference — only binds to temporaries or moved-from objects
- `unique_ptr` can be moved but not copied — enforced at compile time with `= delete`
- `emplace_back` constructs in place — no temporary, preferred over `push_back`
- After `std::move(x)`, don't use `x` — it's in a valid but empty state



---

## Ternary Operator

A one-liner if/else that returns a value. Useful for simple conditional assignments.

```cpp
// syntax: condition ? value_if_true : value_if_false
int x = (a > b) ? a : b;   // x = whichever is larger

// equivalent to:
int x;
if (a > b) x = a;
else       x = b;
```

Common in embedded code for compact assignments:

```cpp
bits.enable = value ? 1 : 0;   // if value is truthy set 1, else set 0
uint8_t led = isOn ? 0xFF : 0x00;
```

Can be nested but gets unreadable fast — only use for simple cases.

---

## Function Syntax

```cpp
returnType functionName(parameters) qualifiers {
    // body
}
```

### Return types

```cpp
void    foo() { }          // returns nothing
int     foo() { return 1; }
float   foo() { return 1.0f; }
bool    foo() { return true; }
Sensor* foo() { return ptr; }   // can return any type including pointers
```

### Qualifiers

Qualifiers go after the parameter list:

```cpp
// const — method promises not to modify any member variables
// required to call the method on a const reference
float getValue() const { return value; }

// override — this method replaces a virtual method in the base class
void describe() const override { }

// = 0 — pure virtual, makes the class abstract, must be implemented by derived class
virtual void describe() const = 0;

// = delete — explicitly disables this function
Sensor(const Sensor&) = delete;   // disables copy constructor

// = default — explicitly use compiler-generated version
Sensor() = default;               // use default constructor
```

### Common function patterns

```cpp
// pass by value — copy made, original unchanged
void foo(int x) { x = 99; }

// pass by reference — modifies original
void foo(int& x) { x = 99; }

// pass by const reference — no copy, read only (best for large objects)
void foo(const Sensor& s) { }

// static method — belongs to the class, not an instance, no `this` pointer
static int getCount() { return count; }

// inline — hint to compiler to expand the function at call site (no call overhead)
inline int add(int a, int b) { return a + b; }
```

### Static methods vs instance methods

```cpp
class Sensor {
    static int count;      // shared across all instances
    float value;           // unique to each instance

public:
    static int getCount()  { return count; }    // no this pointer, accesses class-level data
    float getValue() const { return value; }    // has this pointer, accesses instance data
};

Sensor::getCount();   // call static method on the class itself
Sensor s(10.5f);
s.getValue();         // call instance method on an object
```

---

## Anonymous Unions

A union declared without a name inside a class. Its members get promoted directly into the enclosing class scope — no intermediate name needed to access them.

```cpp
// named union — must go through the union name
class Reg {
    union Data {
        uint32_t raw;
        struct { uint32_t speed : 2; } bits;
    } u;                    // u is the variable name
public:
    void foo() { u.raw = 0xFF; }   // must use u.raw
};

// anonymous union — members promoted directly into class scope
class Reg {
    union {                 // no name
        uint32_t raw;
        struct { uint32_t speed : 2; } bits;
    };                      // no variable name either
public:
    void foo() { raw = 0xFF; }     // access directly, cleaner
};
```

Anonymous unions are useful when the union is just an implementation detail inside a class and you don't want the extra layer of naming. The hardware register class pattern uses this:

```cpp
class ControlRegister {
private:
    union {
        uint32_t raw;
        struct {
            uint32_t enable    : 1;
            uint32_t direction : 1;
            uint32_t speed     : 2;
            uint32_t reserved  : 28;
        } bits;
    };

public:
    void setSpeed(uint8_t value) {
        assert(value <= 3);          // must fit in 2 bits
        bits.speed = value;
    }

    void setEnable(bool value) {
        bits.enable = value ? 1 : 0; // ternary — if value is truthy set 1, else 0
    }

    uint32_t getRaw() const { return raw; }
};
```

---

## Header Guards


Prevents a header from being included multiple times.

```cpp
// traditional
#ifndef MY_HEADER_H
#define MY_HEADER_H
// ... content ...
#endif

// modern — preferred
#pragma once
```

### .h vs .cpp structure

```cpp
// sensor.h
#pragma once
class Sensor {
public:
    Sensor(float initialValue);
    float getValue() const;
private:
    float value;
};

// sensor.cpp
#include "sensor.h"

Sensor::Sensor(float initialValue) : value(initialValue) { }

float Sensor::getValue() const { return value; }
```

`Sensor::getValue()` means "this method belongs to the `Sensor` class." The `::` (scope resolution operator) is how you define methods outside the class body in a `.cpp` file.
