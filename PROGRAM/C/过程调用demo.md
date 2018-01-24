```c
int swap_add(int *xp,int *yp)
{
	                                        // pushl %ebp
	                                        // movl %esp,%ebp
                        
	                                        // pushl %ebx
	int x = *xp;                            // movl 8(%ebp),%ecx
	int y = *yp;                            // movl 12(%ebp),%edx
                        
	*xp = y;                                // movl (%ecx),%eax
	*yp = x;                                // movl (%edx),%ebx
	                                        // movl %eax,(%edx)
	                                        // movl %ebx,(%ecx)
                        
	return x + y;                           // andl %ebx,%eax
	                                        // popl %ebx
	                                        // popl %ebp
	                                        // return
}

int caller()
{
                                            	// pushl %ebp  占用当前栈帧4个字节
	                                        // movl %esp,%ebp

						// subl $24,%esp  占用当前栈帧24个字节

	int arg1 = 534;                         // movl $534,-4(%ebp)
	int arg2 = 1057;			// movl $1057,-8(%ebp)

	int sum = swap_add(&arg1,&arg2);        // leal -4(%ebp),%eax
						// movl %eax,(%esp)
						// leal -8(%ebp),%eax
						// movl %eax,4(%esp)
						// call swap_add


	int diff = arg1 - arg2;			// movl -4(%ebp),%ecx
						// subl -8(%ebp),%ecx

	return sum * diff;			// imull %ecx,%eax
						// movl %ebp,%esp
						// popl %ebp
						// return
}
```
