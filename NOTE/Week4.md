# 109b 系統程式上課筆記

## 第四周
系統程式 第四週  ubuntu multipass + compiler + 組合語言    [FB連接](https://www.facebook.com/ccckmit/videos/10158835739151893/)  
<br>
本周精華註解在下面

<br>
ubuntu multipass若無法順利執行,只要有linux環境就好<br>到linux環境裡開終端機之後


```
git clone https://gitlab.com/ccc109/sp/ 
```

(centos7最後要加.git) 最好先mkdir一個資料夾 方便使用
<br><br>



>cd到 09linux下想執行的程式資料夾 執行程式

>先組譯再連結才能執行
```
gcc -c hello.s       #編出hello.o
ld hello.o -o hello  #hello.o連結變成hello才是執行檔  
./hello              #執行
```
>組譯出有格式的碼 在linux叫elf 還沒跟別的函式庫連接 ld 才會連接

<br><br>

註解版 : 只能在x86版的linux跑的hello world :
```
# #字號都是註解----------------------------------------------------------------------------------------
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

helloMacro.s 改用巨集方式寫 : 
```
        .include "../myMacro.s"
        .global _start

        .text
_start:
        WRITES $1, $message, $13  #要知道WRITES得看上上一層的myMacro.s
        EXIT
message:
        .ascii  "Hello, world\n"

```
註解版myMacro.s : 
```
# write(1, message, 13)
#規定開頭要用.
.macro WRITES fd, msg, len     #這行規定了WRITES是個巨集 有三個參數 第一個參數是檔案描述值 第二個是內容  第三個是長度
  mov     $1, %rax            # system call 1 is write 呼叫syscall做write的動作 因為在這裡傳了1給他
  mov     \fd, %rdi           # file handle 1 is stdout 把fd傳給rdi 告訴syscall要開啟的檔案代號是1
  mov     \msg, %rsi          # address of string to output 把msg指標傳給rsi 他就知道要印哪個字串
  mov     \len, %rdx          # number of bytes
  syscall 把長度傳給 rdx 他就知道傳多少字
.endm #結束要用.endm(規定)

.macro EXIT #讓系統停掉 沒有參數
  mov 	$60, %rax       # 60移給rax (60就是EXIT代號)
  xor	%rdi, %rdi      # 把rdi清0
  syscall
.endm

.macro PUTS msg
  mov   \msg, %rdi
  call  puts
.endm


.macro PRINTF fmt, p1
  # We need to call printf, but we are using eax, ebx, and ecx.  printf
  # may destroy eax and ecx so we will save these before the call and
  # restore them afterwards.

  push    %rax                    # caller-save register
  push    %rcx                    # caller-save register

  mov     \fmt, %rdi              # set 1st parameter (format)
  mov     \p1, %rsi               # set 2nd parameter (current_number)
  xor     %rax, %rax              # because printf is varargs

  # Stack is already aligned because we pushed three 8 byte registers
  call    printf                  # printf(format, current_number)

  pop     %rcx                    # restore caller-save register
  pop     %rax                    # restore caller-save register
.endm
```
使用巨集處理：　
>將要做的事情先寫好，之後只要直接引用，這樣helloMacro.s就可以用少少的行數寫出

註解版 09linux-02asm-02hola的hola.s : 
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
        call    puts                    # puts(message) 呼叫puts函數
        ret                             # Return to C library code
message:
        .asciz "Hola, mundo"            # asciz puts a 0 byte at the end
```
執行09linux-02asm-02hola的hola.s :   
```
gcc -no-pie hola.s -o hola     
./hola
```
>-no-pie 代表 no position independent executable  --> 不要編成與位址無關的目的檔  


        call    puts      # puts(message) 呼叫puts函數(c函式庫裡的)

[Working with the C Library原始來源](https://cs.lmu.edu/~ray/notes/gasexamples/)  :
>裡面有更多範例，例如power(x,y)引用atoi

<br><br>
gcc vs. llvm (llvm為蘋果資助研究用在自家產品)

<br><br>

[查看更詳細的老師巨集說明](https://www.slideshare.net/ccckmit/6-128033146)
　

<br>



參考資料：  
https://www.slideshare.net/ccckmit/6-128033146
https://cs.lmu.edu/~ray/notes/gasexamples/

