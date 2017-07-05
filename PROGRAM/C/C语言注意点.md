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
which cc 可以看到 cc /usr/bin/cc
ll /usr/bin/cc 可以看到 /usr/bin/cc -> gcc
