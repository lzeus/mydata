###   1. const
```c
//p指向的对象不可修改
void m(const int *p)
{
  int j = 100;
  *p = 0;//WRONG
  p = &j//RIGHT
}
//p的值不可修改
void m(int *const p)
{
  int j = 100;
  *p = 0;//RIGHT
  p = &j;//WRONG 
}
```

###   2. 多维数组的理解
```c
int A[5][4];

//等价于
typedef int int_arr[4]
int_arr A[5];

T D[R][C]
&D[i][j] = Address(D) + TLength * (i*C + j)
```

###   3. linux中cc gcc区别
####  3.1. cc来自于unix，gcc来自于linux
cc 是 c compiler的缩写，是unix平台的C编译器。<br>
gcc 是 GNU compiler collection 的缩写，是linux的编译器集合，不仅仅是C编译器。
####  3.2. linux中的cc是啥
说明cc是gcc的链接<br>
which cc 可以看到 /usr/bin/cc <br>
ll /usr/bin/cc 可以看到 /usr/bin/cc -> gcc<br>
当一个c/c++工程是在Unix下编写的，需要在linux平台编译的时候，就需要cc这个别名了。因为unix工程的makefile文件中用了cc。

