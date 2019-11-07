//
//  main.c
//  test_5_26
//
//  Created by 张辉 on 2019/5/26.
//  Copyright © 2019年 张辉. All rights reserved.
//

//#include <stdio.h>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    printf("Hello, World!\n");
//    return 0;
//}

//1.求字符串长度
#include<stdio.h>
#include<assert.h>
1>普通方法
size_t my_strlen(const char* str)
{
    int count = 0;
    assert (str!=NULL);    //断言
    while(*str)
    {
        count++;
        str++;
    }
    return count;
}
2>指针减去指针的方法
size_t my_strlen(const char* str)
{
    const char* start = str;
    assert(str != NULL);
    while(*str)
    {
        str++;
    }
    return str - start;
}
3>递归求解
例如求：my_strlen("abc")
1+my_strlen("bc");
1+1+my_strlen("c");
1+1+1+my_strlen("");
1+1+1+0;
3
int my_strlen(const char* str)
{
    assert(str != NULL);
    if(*str != '\0')
    {
        return 1 + my_strlen(str+1);
    }
    else
        return 0;
}

int main()
{
    char *p = "abcdef";
    size_t len = my_strlen(p);
    printf("len = %zu\n",len);
    return 0;
}
2>字符串的拷贝
使用strcpy需要注意的问题：
1.顺序的问题，一个是目标，一个是原。
2.必须保证目标空间足够大。
3.目标空间必须可以被修改，不能是常量。
4.原字符串必须含有\0，不然会无休止的拷贝下去，会出现越界访问等现象。
char* my_strcpy(char* dest, const char* src)
{
    char* ret = dest;
    while((*dest++ = *src++))
    {
        ;
    }
    return ret;
}
int main()
{
    char arr1[20] = { 0 };
    char arr2[] = { 'b','i','t','\0' };
    printf("%s\n",my_strcpy(arr1,arr2));  // 链式访问
    return 0;
}
