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

**Polymorphism** — "many forms." The ability to treat different types through a common interface. A `TemperatureSensor` and a `PressureSensor` are both `Sensor`s — you can call `describe()` on either through a `Sensor&` reference and each responds correctly. Runtime polymorphism uses virtual functions. Compile time polymorphism uses templates.

**Abstraction** — hiding complexity behind a simple interface. You call `sensor.getValue()` without knowing how it reads hardware.

**Encapsulation** — bundling data and the methods that operate on it into one class, controlling access with `private`/`public`. Prevents outside code from putting an object in an invalid state.

**Inheritance** — a derived class takes on all members of a base class. Models "is-a" relationships: a `TemperatureSensor` IS A `Sensor`.

**Virtual dispatch / dynamic dispatch** — the mechanism behind runtime polymorphism. When you call a virtual method through a base class reference, the program looks up the correct implementation at runtime via the vtable.

**Vtable (virtual table)** — a hidden lookup table the compiler generates for classes with virtual methods. Each object carries a hidden pointer to its vtable. The vtable maps virtual method calls to the correct implementation for that type.

**Static vs Dynamic** — static = decided at compile time. Dynamic = decided at runtime. Static dispatch = compiler picks the function. Dynamic dispatch = vtable lookup at runtime.

**RAII (Resource Acquisition Is Initialization)** — resource lifetime is tied to object lifetime. Acquire in constructor, release in destructor. When the object goes out of scope, cleanup happens automatically. `unique_ptr` is RAII for heap memory.

**Object slicing** — when you store a derived class object in a base class variable (not pointer), the derived parts get cut off. Only the base class portion survives. Reason to store `unique_ptr<Sensor>` in containers instead of `Sensor` directly.

**Scope** — the region of code where a variable is valid. Local variables go out of scope at the closing `}` of their block. Going out of scope triggers destructors.

**Dereference** — following a pointer to get the value at its address. `*ptr` = go to that address. `ptr->method()` = dereference and call method.

**Undefined behavior (UB)** — code the C++ standard makes no guarantees about. Might crash, might corrupt memory, might appear to work. Common causes: null pointer dereference, out of bounds access, writing to read-only memory, using memory after `delete`.

**Overhead** — extra cost in time or memory. Vtable lookup = small runtime overhead. Heap allocation = more overhead than stack. Embedded engineers minimize overhead because resources are constrained.

**Deterministic** — predictable and consistent every time. Stack allocation is deterministic. Heap allocation is not. Real-time embedded systems require deterministic behavior.

**SWaP** — Size, Weight, and Power. The three binding constraints in fielded military hardware.

**Segfault** — crash caused by accessing memory you're not allowed to touch. Most common cause: null pointer dereference. On Linux the OS kills the process. On bare metal embedded it might corrupt memory silently.

**Instantiation** — creating an actual object from a class definition, or generating code from a template for a specific type.

**Declaration vs Definition** — declaration tells the compiler something exists (`void foo();`). Definition is the actual implementation (`void foo() { ... }`). You can declare many times, define only once.

**Template** — a blueprint for generating code for multiple types at compile time. Zero runtime overhead vs virtual functions.

**Race condition** — two threads access the same data simultaneously and the result depends on timing. `counter++` is actually three operations — read, modify, write. Another thread can jump in between any of them.

**Mutex (mutual exclusion)** — a lock. Only one thread can hold it at a time. Others block and wait. Prevents race conditions.

**Deadlock** — thread A holds lock 1 waiting for lock 2, thread B holds lock 2 waiting for lock 1. Both wait forever. Fix: always acquire multiple locks in the same order.

**Interrupt** — hardware-triggered event that preempts whatever is running, executes a handler, then returns. Must be fast — no blocking, no heap allocation. Variables shared with interrupt handlers need `volatile`.

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

## constexpr

*(coming soon)*

---

## unordered_map

*(coming soon)*

---

## Move Semantics

*(coming soon)*

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
