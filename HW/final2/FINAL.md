# 期末作業：理解[GNU Assembler Examples](https://cs.lmu.edu/~ray/notes/gasexamples/)
## 參考GNU Assembler範例與陳鍾誠老師的系統程式課程教材，能理解部分程式碼與執行並進行註解（直接註解在此README.MD裡）

>首先要知道 :  
>1.Stack：  
一種先進後出 (First In Last Out, FILO) 的資料結構。  
>2.幾個組合語言指令  :  
push：把資料存進 stack ，rsp 會向低位移動一單位。    
pop：把資料存 stack 內取出 ，rsp 會向高位移動一單位。    
call： 把當前位址 push 進 stack ，然後跳進某個 procedure 。    
ret：　把 stack 中的資料 pop 出，並跳到該位址。    
[參考資料來源](https://ithelp.ithome.com.tw/articles/10227112)
<br>

Assembly | C 語言
-----|-------------------
push  rbp | push(rbp);
mov   rbp,rsp  | rbp = rsp;
sub   rsp,0x20 | rsp -= 0x20;
call  401710      |f_401710();
call  402c40  | f_402c40();
pop   rbp  | pop(&rbp);
ret   |  return;

相似但不完全一樣，可參考用

[表格資料來源](https://ithelp.ithome.com.tw/articles/10227112)

<br>

>GAS, the GNU Assembler, is the default assembler for the GNU Operating System.  
It works on many different architectures and supports several assembly language syntaxes.   
這些範例只能在作業系統 Linux 、 x86-64 processor 使用

## 開始 - Getting Started-hello.s
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
        syscall                         # invoke operating system to do the write （呼叫#syscall像是函數呼叫但以組合語言方式 : 先把參數移到暫存器，再呼叫syscall

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
        ret                             # Return to C library code (回傳值)
message:
        .asciz "Hola, mundo"            # asciz puts a 0 byte at the end
```
執行 hola.s :   
```
gcc -no-pie hola.s -o hola     
./hola
```
>-no-pie 代表 no position independent executable  --> 不要編成與位址無關的目的檔  以減少損失

    call    puts      # puts(message) 呼叫puts函數(c函式庫裡的)



# Calling Conventions for 64-bit C Code-fib.s (呼叫約定、呼叫慣例)

>64-bit calling conventions 在 [AMD64 ABI Reference](http://www.x86-64.org/documentation/abi.pdf) 裡有更詳細的解釋. [維基](https://en.wikipedia.org/wiki/X86_calling_conventions#x86-64_Calling_Conventions)也有關於他的資訊. 更重要的是，給 64-bit Linux用,不是 Windows):

>以下翻譯參照參考資料所使用名稱和google翻譯，但有些名詞仍然有點混淆


從左到右，傳遞盡可能適合暫存器的範圍的參數。暫存器分配的順序是：   
 - 整數 和 pointers , rdi, rsi, rdx, rcx, r8, r9.
－    
 - 浮點數(float, double) , xmm0, xmm1, xmm2,xmm3, xmm4, xmm5, xmm6, xmm7  

參數被從右到左推入堆疊中
 
參數壓入後，進行調用指令，所以當被調用函數獲得控制權時，返回地址在（%rsp），第一個內存參數在8（%rsp），以此類推。  

在進行調用之前，堆疊指針 %RSP 必須與 16 位元邊界對齊。
進行調用的過程會將返回地址（8 個字節）壓入堆疊，因此當函數獲得控制權時，%rsp 未對齊。你必須自己創造額外的空間，通過推動某些東西或從 %rsp 中減去 8。
  
被調用函數需要保留的唯一暫存器（調用保存暫存器）是：rbp、rbx、r12、r13、r14、r15。所有其他的都可以由被調用的函數自由更改。
被調用者還應該保存 XMCSR 的控制位和 x87 控製字，但 x87 指令在 64 位代碼中很少見

整數在 rax 或 rdx:rax 中傳回，浮點值在 xmm0 或 xmm1:xmm0 中傳回。

<Br>

```
# -----------------------------------------------------------------------------
# A 64-bit Linux application that writes the first 90 Fibonacci numbers.  It
# needs to be linked with a C library. 必須跟C函式庫連接
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
        # may destroy eax and ecx so we will save these before the call and (先暫存起來避免衝突
        # restore them afterwards.

        push    %rax                    # caller-save register (先將caller-save register的值存入stack frame。)
        push    %rcx                    # caller-save register (先將caller-save register的值存入stack frame。)

        mov     $format, %rdi           # set 1st parameter (format) 型態範圍
        mov     %rax, %rsi              # set 2nd parameter (current_number) 現在的值
        xor     %rax, %rax              # because printf is varargs (printf是可變動函數 所以xor清0)

        # Stack is already aligned because we pushed three 8 byte registers
        call    printf                  # printf(format, current_number)

        pop     %rcx                    # restore caller-save register (把存入的值取出)
        pop     %rax                    # restore caller-save register (把存入的值取出)

        mov     %rax, %rdx              # save the current number
        mov     %rbx, %rax              # next number is now current (下個值變成當前值)
        add     %rdx, %rbx              # get the new next number
        dec     %ecx                    # count down
        jnz     print                   # if not done counting, do some more

        pop     %rbx                    # restore rbx before returning
        ret
format:
        .asciz  "%20ld\n"
```
>Calling convention :  
規定了函式與函式之間如何互動，例如：如何傳遞參數給函式，如何取得函式結束後回傳的返回值，哪些暫存器的內容值需在函式呼叫前後保持一致，哪些暫存器其值若在函式呼叫結束後仍有需要，則需在函式呼叫前先行儲存起來，若一函式編譯成 object file後，提供外部連結，則須嚴格遵守 Calling convention，才能與其他函式溝通。可分為兩種registers : 

>Caller-save register - 由Caller負責清理或存入stack frame。  
 – Caller再呼叫Callee前，必須先將caller-save  register的值存入stack frame。  
 – 所以callee 便可直接使用caller-save register裡的值，而不須做存入stack frame的動作。

>

> [Calling convention參考資料](http://redbug0314.blogspot.com/2007/06/calling-convention.html)  
  [Calling convention參考資料2](https://tclin914.github.io/77838749/)

執行 fib.s :
```
gcc -no-pie fib.s -o fib
./fib
```
<br>

# Mixing C and Assembly Language -maxofthree.s(將c跟組合語言融合) :
>  They will have been pushed on the stack so that on entry to the function, they will be in rdi, rsi, and rdx, respectively. The return value is an integer so it gets returned in rax.
>這個 64 位程序是一個非常簡單的函數，它接受三個 64 位整數參數並返回最大值。它展示了如何取出整數參數：它們將推入堆疊，以便在進入函數，它們將分別位於 rdi、rsi 和 rdx 中。傳回值是一個整數，因此它以 rax 形式返回。
```
# -----------------------------------------------------------------------------
# A 64-bit function that returns the maximum value of its three 64-bit integer
# arguments.  The function has signature:
#
#   int64_t maxofthree(int64_t x, int64_t y, int64_t z)
#
# Note that the parameters have already been passed in rdi, rsi, and rdx.  We
# just have to return the value in rax.
# -----------------------------------------------------------------------------

        .globl  maxofthree
        
        .text
maxofthree:
        mov     %rdi, %rax              # result (rax) initially holds x (初始先存x(rdi))
        cmp     %rsi, %rax              # is x less than y? (x和y(rsi)進行比較)
        cmovl   %rsi, %rax              # if so, set result to y (如果y較大,將結果改變成y(rsi))
        cmp     %rdx, %rax              # is max(x,y) less than z? (和z(rdx)進行比較)
        cmovl   %rdx, %rax              # if so, set result to z (如果z較大,將結果改成z)
        ret                             # the max will be in eax (將最後結果回傳)
```
>c語言呼叫組合語言(maxofthree)的部分 (C program that calls the assembly language function ) :
>maxofthree是組合語言寫出來的
```
 /*
 * callmaxofthree.c
 *
 * A small program that illustrates how to call the maxofthree function we wrote in
 * assembly language.
 */

#include <stdio.h>
#include <inttypes.h>

int64_t maxofthree(int64_t, int64_t, int64_t);  #先定義一個函數原型才能讓他編譯 之後就會連到maxofthree

int main() {
    printf("%ld\n", maxofthree(1, -4, -7));
    printf("%ld\n", maxofthree(2, -6, 1));
    printf("%ld\n", maxofthree(2, 3, 1));
    printf("%ld\n", maxofthree(-2, 4, 3));
    printf("%ld\n", maxofthree(2, -6, 5));
    printf("%ld\n", maxofthree(2, 4, 6));
    return 0;
}
```
執行callmaxofthree.c maxofthree.s :
```
gcc -std=c99 callmaxofthree.c maxofthree.s 
./a.out
```

<br><br> 
# Command Line Arguments-echo.s   (了解如何存取argc argv) :
>在 C 語言裡，main 只是一個普通的函數，擁有幾個自己的參數：  
int main(int argc, char** argv)　　


>一個程式傳回每個命令列參數：

```
# -----------------------------------------------------------------------------
# A 64-bit program that displays its commandline arguments, one per line.
#
# On entry, %rdi will contain argc and %rsi will contain argv.
# -----------------------------------------------------------------------------

        .global main

        .text
main:
        push    %rdi                    # save registers that puts uses
        push    %rsi
        sub     $8, %rsp                # must align stack before call

        mov     (%rsi), %rdi            # the argument string to display
        call    puts                    # print it   (呼叫puts)

        add     $8, %rsp                # restore %rsp to pre-aligned value
        pop     %rsi                    # restore registers puts used
        pop     %rdi

        add     $8, %rsi                # point to next argument
        dec     %rdi                    # count down
        jnz     main                    # if not done counting keep going

        ret
format:
        .asciz  "%s\n"

```
執行echo.s : 
```
gcc echo.s
./a.out 25782 dog huh $$ 
./a.out 25782 dog huh '$$'
```
>$$ 顯示出shell 的 PID 
>'$$' 純粹 印出$$


<br><br> 

# Treat them as integers-power.s (x的y次方) 將傳進來的參數 char * (字串)轉成整數
>1.就 C Library而言，命令列參數始終是字符串(string)  
2.如果要將它們視為整數，就需要用到 atoi 函式  
3.這個範例另外一個特點就是將值限制在32-bit (?)

```
# -----------------------------------------------------------------------------
# A 64-bit command line application to compute x^y.
#
# Syntax: power x y
# x and y are integers
# -----------------------------------------------------------------------------

        .global main

        .text
main:
        push    %r12                    # save callee-save registers
        push    %r13
        push    %r14
        # By pushing 3 registers our stack is already aligned for calls

        cmp     $3, %rdi                # must have exactly two arguments
        jne     error1

        mov     %rsi, %r12              # argv

# We will use ecx to count down form the exponent to zero, esi to hold the
# value of the base, and eax to hold the running product.

        mov     16(%r12), %rdi          # argv[2]
        call    atoi                    # y in eax
        cmp     $0, %eax                # disallow negative exponents
        jl      error2
        mov     %eax, %r13d             # y in r13d

        mov     8(%r12), %rdi           # argv
        call    atoi                    # x in eax   (呼叫atoi把字串轉成整數)
        mov     %eax, %r14d             # x in r14d

        mov     $1, %eax                # start with answer = 1
check:
        test    %r13d, %r13d            # we're counting y downto 0
        jz      gotit                   # done
        imul    %r14d, %eax             # multiply in another x
        dec     %r13d
        jmp     check
gotit:                                  # print report on success
        mov     $answer, %rdi
        movslq  %eax, %rsi
        xor     %rax, %rax
        call    printf
        jmp     done
error1:                                 # print error message
        mov     $badArgumentCount, %edi
        call    puts
        jmp     done
error2:                                 # print error message
        mov     $negativeExponent, %edi
        call    puts
done:                                   # restore saved registers
        pop     %r14
        pop     %r13
        pop     %r12
        ret

answer:
        .asciz  "%d\n"
badArgumentCount:
        .asciz  "Requires exactly two arguments\n"
negativeExponent:
        .asciz  "The exponent may not be negative\n"
```
atoi用法 : 
>int atoi(const char *str)  
>將字串裡的數字字元(參數str)轉化為整數（int型）。並傳回  
>注意：轉化時跳過前面的空格字元，直到遇上數字或正負符號才開始做轉換，而再次遇到非數字或字串結束時('/0')才結束轉換，並將結果傳回。

執行power.s
```
gcc -no-pie power.s -o power
./power 1 3  (印出1)
./power 2 4  (印出16)
```

參考資料 : 

https://poli.cs.vsb.cz/edu/soj/down/soj-syllabus.pdf

https://hackmd.io/@PJChiou/HJGLKrbYQ/https%3A%2F%2Fhackmd.io%2Fs%2FS1lP1tfF7?type=book