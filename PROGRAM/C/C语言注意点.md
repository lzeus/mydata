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
