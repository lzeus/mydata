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
####  3.2. 为什么linux中cc与gcc同时存在
which cc 可以看到 /usr/bin/cc <br>
ll /usr/bin/cc 可以看到 /usr/bin/cc -> gcc<br>
说明cc是gcc的链接
问题来了，如果我的c/c++项目是在Unix下编写的，在写makefile文件时自然地用了cc，当将其放到Linux下这无法make了，必须将其中的cc全部修改成gcc。所以，Linux这想了这么一个方便的解决方案：不修改makefile，继续使用cc，这个cc是个“冒牌货”，它实际指向gcc。

