# GNU Assembler Examples
>GAS, the GNU Assembler, is the default assembler for the GNU Operating System. It works on many different architectures and supports several assembly language syntaxes. These examples are only for operating systems using the Linux kernel and an x86-64 processor, however.

## Getting Started-hello.s
>Hello World program that uses Linux System calls, for a 64-bit installation:

```
# ----------------------------------------------------------------------------------------
# Writes "Hello, World" to the console using only system calls. Runs on 64-bit Linux only.
# To assemble and run:
#
#     gcc -c hello.s && ld hello.o && ./a.out
#
# or
#
#     gcc -nostdlib hello.s && ./a.out
# ----------------------------------------------------------------------------------------

        .global _start

        .text
_start:
        # write(1, message, 13)  相當於函數呼叫
        mov     $1, %rax                # system call 1 is write 把1移到rax
        mov     $1, %rdi                # file handle 1 is stdout  把1移到rdi
        mov     $message, %rsi          # address of string to output 把message移到rsi
        mov     $13, %rdx               # number of bytes 把13移到rdx
        syscall                         # invoke operating system to do the write 呼叫#syscall像是函數呼叫但以組合語言方式 : 先把參數移到暫存器，再呼叫syscall

        # exit(0)  參數0代表正常 非0都不正常
        mov     $60, %rax               # system call 60 is exit 把60移到rax
        xor     %rdi, %rdi              # we want return code 0 把自己跟自己xor --> 歸 0
        syscall                         # invoke operating system to exit 再呼叫syscall
        
        #第一個呼叫是印出hello world 第二個是要跳開
message:
        .ascii  "Hello, world\n"

```

>先組譯再連結才能執行
```
gcc -c hello.s       #編出hello.o
ld hello.o -o hello  #hello.o連結變成hello才是執行檔  
./hello              #執行
```
>組譯出有格式的碼 在linux叫elf 還沒跟別的函式庫連接 ld 才會連接

#  Working with the C Library-hola.s : 
>GCC而言可以自動連接 (gcc -c hola.s 時只編譯不連接 所以把-c拿掉會自動連接(既編譯又連結) C Library
```
# ----------------------------------------------------------------------------------------
# Writes "Hola, mundo" to the console using a C library. Runs on Linux or any other system
# that does not use underscores for symbols in its C library. To assemble and run:
#
#     gcc hola.s && ./a.out
# ----------------------------------------------------------------------------------------

        .global main

        .text
main:                                   # This is called by C library's startup code
        mov     $message, %rdi          # First integer (or pointer) parameter in %rdi 這裡mov message到rdi
        call    puts                    # puts(message) 呼叫puts函數(gcc時候自動連結C函式庫)
        ret                             # Return to C library code
message:
        .asciz "Hola, mundo"            # asciz puts a 0 byte at the end
```
執行 hola.s :   
```
gcc -no-pie hola.s -o hola     
./hola
```
>-no-pie 代表 no position independent executable  --> 不要編成與位址無關的目的檔  


        call    puts      # puts(message) 呼叫puts函數(c函式庫裡的)

```
# Calling Conventions for 64-bit C Code-fib.s
# -----------------------------------------------------------------------------
# A 64-bit Linux application that writes the first 90 Fibonacci numbers.  It
# needs to be linked with a C library.
#
# Assemble and Link:
#     gcc fib.s
# -----------------------------------------------------------------------------

        .global main

        .text
main:
        push    %rbx                    # we have to save this since we use it (先資料存進stack)

        mov     $90, %ecx               # ecx will countdown to 0
        xor     %rax, %rax              # rax will hold the current number (rax存目前資料)
        xor     %rbx, %rbx              # rbx will hold the next number (rbx存下個)
        inc     %rbx                    # rbx is originally 1
print:
        # We need to call printf, but we are using eax, ebx, and ecx.  printf
        # may destroy eax and ecx so we will save these before the call and
        # restore them afterwards.

        push    %rax                    # caller-save register
        push    %rcx                    # caller-save register

        mov     $format, %rdi           # set 1st parameter (format)
        mov     %rax, %rsi              # set 2nd parameter (current_number)
        xor     %rax, %rax              # because printf is varargs

        # Stack is already aligned because we pushed three 8 byte registers
        call    printf                  # printf(format, current_number)

        pop     %rcx                    # restore caller-save register
        pop     %rax                    # restore caller-save register

        mov     %rax, %rdx              # save the current number
        mov     %rbx, %rax              # next number is now current
        add     %rdx, %rbx              # get the new next number
        dec     %ecx                    # count down
        jnz     print                   # if not done counting, do some more

        pop     %rbx                    # restore rbx before returning
        ret
format:
        .asciz  "%20ld\n"
```

Mixing C and Assembly Language -maxofthree.s(將c跟組合語言融合)

