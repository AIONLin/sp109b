# 109b 系統程式上課筆記

## 第十二週
系統程式 第 12 週 -- posix: fork, fs, shell   [FB連接](https://www.facebook.com/ccckmit/videos/10158962564926893/)  
<br>
本周精華註解在下面

執行 08posix-03-fork 01-hello  

```  
gcc fork1.c -o fork1
ls
./fork1
```
<img src="./picture/ForkX2.jpg">

```
 gcc fork2.c -o fork2
 ./fork2
```
```
gcc fork3.c -o fork3
./fork3
```
要結束fork3執行結果要  kill PID


08-03fork-02-child 
>fork傳回值是0 --> 子行程  
>fork傳回值非0 --> 父行程

08posix-03-fork-03-exec-execvp1.c
>execvp 行程替換 

```
int main() {
  char *arg[] = {"ls", "-l", NULL }; //參數設定
  printf("execvp():before\n");  //execvp 執行之前 所以印出before
  execvp(arg[0], arg); // execvp執行 ls -l
  printf("execvp():after\n"); // main被替換下場
}

```
>先印出before 再印出 ls -l
main程式被替換成execvp 所以after 不會跑


UNIX 設計時就是 用 fork+execvp 合成一個載入行程的動作
bash 本身也會呼叫 execvp 執行打進去的指令

UNIX用C語言改寫 所以關係緊密 兩者互相扶持


08-03fork-04system :
```
gcc system1.c -o system1
./system1
```
system1.c
```
int main() {
  system("ls -l");   // 執行system指令的時候會fork出一個行程 再用execvp去呼叫ls -l
  printf("main end!\n");
}
```

08-03-04-mysystem1.c
```
int mysystem(char *arg[]) {
  if (fork()==0) {       // fork == 0 是子行程
    execvp(arg[0], arg); // child : exec("ls -l")
  }
  int status;
  wait(&status); // 等子代執行完 再回到父代
  return status;
}

int main() {
  char *arg[] = {"ls", "-l", NULL }; //傳參數 (ls -l)
  mysystem(arg); (main進來後呼叫mysystem執行ls -l)
  printf("main end!\n");
}
```
>自己用linux系統函數呼叫創建一個system函數叫mysystem



1.04.30  講解myssystem1
跟system幾乎一樣

1.06.00  08-03-05zombie 執行zombie.c 
child_pid>0 子行程

1.12.30 帶到下個 08-posix-04-fs 
1.13.00 08-posix-04-fs -01-echo
echo1.c 
1.18.00 回答n是甚麼數字
int n = read(0, line, SMAX);
n 是指 read 讀到多少個 byte
1.19.20 開始解釋 08-posix-04-fs -01echo的echo1.c
讀到line裡面 xxxx印出來
unix標準系統函數 -->read write
1.23.00 獎 012的預設功能 帶到08-posix-04-fs -02 fecho
1.25.00 26.00 開始講解 fecho1
中間老師遇到點問題患系統跑
1.40.00 fecho2.c
a取代0 b取代1
1.42.20 筆記一下
1.44.00 08-05-v1 -myshell.c
1.47.30 v2 解決路徑切換問題


 
googlemeet 問題

mysystem1:
fork出子行程
如果fork=0 子行程 被替換掉
父行程沒被替換 main執行完
一個再執行的程式-->process
fork會傳回行程代號 (整數且通常大於0







參考資料：  
