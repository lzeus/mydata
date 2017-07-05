#### 1. 示例代码

```c
//helloworld.c
#include <stdio.h>

int main()
{
  printf("hello world ! ");
  return 0;
}
```

#### 2. 直接编译

```c
gcc helloworld.c -o helloworld
```
直接执行编译的四个步骤:预处理(Preprocessing) 编译(Compilation) 汇编(Assembly) 链接(Linking)

#### 3. 分步骤编译

编译的每个步骤都可以单独进行

##### 3.1. 预处理
gcc的-E选项，可以让编译器在预处理后停止，并输出预处理结果。<br>
在本例中，预处理结果就是将stdio.h 文件中的内容插入到helloworld.c中了。
```c
//生成预处理结果文件helloworld.i
gcc -E helloworld.c -o helloworld.i
//生成预处理结果，并输出到标准输出流
gcc -E helloworld.c
```
##### 3.2. 编译
预处理之后，可对生成的helloworld.i文件编译，生成汇编代码
```c
gcc -S helloworld.i -o helloworld.s
```
##### 3.3. 汇编
编译后的汇编代码经过汇编器的处理，生成可链接目标文件
```bash
gcc -c helloworld.s -o helloworld.o
```

##### 3.4. 链接
可链接目标文件经过链接器处理，生成可执行目标文件
```bash
gcc helloworld.o - o helloworld
```

http://www.cnblogs.com/ggjucheng/archive/2011/12/14/2287738.html
