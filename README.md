format class
A C++ class for converting numbers to strings in a printf-like fashion
=========

[Japanese](READMEja.md)

## overview

 In C++, there is a culture of not using functions that handle the variable arguments common in the C language.   
   
 The main problem is that the arguments are passed through the stack and you don't know how many arguments are stored on the stack, which can cause the stack to overflow and adversely affect your system.
   
A typical implementation is the "printf" function.
 The compiler parses formatting statements and checks the integrity of the arguments, but it cannot check them completely.
 On the other hand, "printf" provides greater flexibility and makes it easier to work with characters and numbers.
      
Boost has a format.hpp which is flexible and safe for printf.
 It can take multiple arguments using the "%" operator overloading mechanism.
 boost::format is a good implementation, but it relies on iostream and is problematic for embedded microcontrollers.
   
 If you import iostream, the capacity will be enlarged.
   
```
    std::cout << "Hello!" << std::endl;

   text    data     bss     dec     hex filename
 508864   47644    8812  565320   8a048 hello.elf

---
    printf("Hello!\n");

   text    data     bss     dec     hex filename
  13864      48    1924   15836    3ddc hello.elf

---
    utils::format("Hello!\n");

   text    data     bss     dec     hex filename
   6700      48    2136    8884    22b4 hello.elf
```

So I implemented the format class instead of printf, in imitation of the boost::format mechanism.
   
---
## Specification

 - The basic specification conforms to "printf.
 - The specification for passing parameters is almost the same as boost::format.
 - There are some parts that differ from printf, and some extended specifications.
 - The namespace is "utils".
 - For string output, a functor is defined and used as a template parameter.   
   The standard functor "stdout_buffered_chaout" class is defined and typedef'd as follows   
   typedefed.   

```
    typedef basic_format<stdout_buffered_chaout<256> > format;
    typedef basic_format<stdout_chaout> nformat;
    typedef basic_format<memory_chaout> sformat;
    typedef basic_format<null_chaout> null_format;
    typedef basic_format<size_chaout> size_format;
```

Normally, the formatted string is output to the standard output "stdout".   
The standard output is via buffer buffered at 256 bytes.

If buffering is not used, "nformat" can be used.

To output strings in memory, use "sformat".
Equivalent to "sprintf

If you want to discard strings, use "null_format".

If you want to know the size of the output, use "size_format".

The above functor is implemented in "format.hpp".

Considering the use in an embedded microcontroller, "exceptions" are not raised in error-related processing.
Errors that occur during input conversion can be retrieved as error types.

- In general, using exceptions consumes a lot of memory.
- Using exceptions can cause fatal problems if an error occurs and there is no correct recipient.
- If multiple conversions cause errors at the same time, the last error will remain.
- It must be sturdy, and not crash or fall into an endless loop due to formatting inconsistencies or misuse.

---
## Usage

### Include format.hpp.

```
#include "format.hpp"
```

The only header needed to use all features is "format.hpp".

### The namespace is "utils".

### Sample

- Display the contents of "a" on standard output.

```
    int a = 1000;
    utils::format("%d\n") % a;
```

- Output the contents of "a" to the string "res".

```
    int a = 1000;
    char res[64];
    utils::sformat("%d\n", res, sizeof(res)) % a;
```

- Inspect for errors in the conversion process.

```
    int a = 1000;
    auto err = (utils::format("%d\n") % a).get_error();
    if(err == utils::error::none) {
        // OK!
    } else {
        // NG!
    }
```

- Get output character size only

```
    int a = 1000;
    auto size = (utils::size_format("%d\n") % a).size();
```

- Fixed-point display   
In embedded microcontrollers, there are many cases where you want to display A/D-converted integers in normalized form.   
Therefore, the fixed-decimal-point display is implemented as an extended function.   
- In addition, 8-/16-bit microcontrollers consume an extreme amount of memory when handling floating-point numbers.   
- In the example below, the decimal point is 10 bits.
- After the display digit (2 decimal places in the example below), the value is rounded off, but if the number of bits is insufficient, the value is equivalent to the truncated value.   
- If you need to display 3 decimal places, 11 bits are necessary considering rounding.

```
    uint16_t a = 1000;
    utils::format("%3.2:10y") % a;
```

---

- Sweeping Buffered Characters
To increase the speed of processing characters, a buffer has been provided for some time now.   
   
In the prototype

```
typedef basic_format<stdout_buffered_chaout<256> > format;
```

and is a 256-byte buffer.   
   
Normally, the buffer is flushed with the "\n" (newline) code, but this may cause timing problems.   
If you want to flush the buffer, specify it explicitly as follows   


```
    utils::format::chaout().flush();
```
   
---

## Customize

When used in projects with limited resources, such as 8/16-bit microcontrollers, the software can be customized.   
The following definitions can be used to optimize the amount of code by eliminating unused functions.   

```
// disable float (to save memory on 8-bit microcontrollers)
// #define NO_FLOAT_FORM

// if you do not support binary display (to save memory)
// #define NO_BIN_FORM

// if octal display is not supported (memory saving)
// #define NO_OCTAL_FORM

```

---
## Customizing standard output

Normally, standard output uses the stdout handle and calls the write function.   
Some processors may force you to use the putchar function, in which case use the following definition.   
- Uncomment it out.   

```
// Enable if putchar is used as the final output (usually the write [stdout] function)
#define USE_PUTCHAR
```

---
## Project (Overall Test)

- The format class is provided with the overall test.   
- It is compiled using clang++ in a mingw64 environment.   

```
make
```

The overall test is compiled with.   
The full test is compiled with.   

```
make run
```

- If the overall test runs and all tests pass, the program exits normally.   
- If the test fails, -1 is returned.   

---
Translated with www.DeepL.com/Translator (free version)

---
      
## License

[MIT](LICENSE)

```
Copyright (c) 2020, Hiramatsu Kunihito
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the <organization> nor the　names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
