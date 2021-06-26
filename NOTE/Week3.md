# 109b 系統程式上課筆記

## 第三周
[系統程式 第三週  編譯器](https://www.facebook.com/ccckmit/videos/10158818166811893/)


>fopen：FILE *fopen(const char *filename, const char *mode)

const char *filename(第一個參數) -- 這是C字符串，其中包含要打開的文件名(檔案名稱)。  
const char *mode(第二個參數) -- 這是C字符串，其中包含文件訪問模式(開啟模式)。
>模式(mode)有r、w、a等等 ：  
r：打開一個文件進行讀取。該文件必須存在。  
w：創建一個空的書麵文件。如果已經存在具有相同名稱的文件，其內容被刪除的文件被認為是一個新的空文件。  
a：附加到文件中。寫入操作的數據追加在文件末尾的。該文件被創建，如果它不存在。  
r+：	打開更新文件讀取和寫入。該文件必須存在。  
w+：	創建一個空文件，讀取和寫入。  
a+：	打開一個文件的​​讀取和追加。  



 　



參考資料：  
https://www.cplusplus.com/reference/cstdio/fopen/  
http://tw.gitbook.net/c_standard_library/c_function_fopen.html
