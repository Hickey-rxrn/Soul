//
//  main.c
//  test_5_27
//
//  Created by 张辉 on 2019/5/27.
//  Copyright © 2019年 张辉. All rights reserved.
//

//#include <stdio.h>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    printf("Hello, World!\n");
//    return 0;
//}



#include <stdio.h>
#include <assert.h>
//1>
//int my_strlen(const char* str)
//{
//    int count = 0;
//    assert(str != NULL);
//    while (*str)
//    {
//        count++;
//        str++;
//    }
//    return count;
//}
//2>
//size_t my_strlen(const char* str)
//{
//    const char* start = str;
//    assert(str != NULL);
//    while (*str)
//    {
//        str++;
//    }
//    return str - start;
//}
//3>
//int my_strlen(const char* str)
//{
//    assert(str != NULL);
//    if(*str != '\0')
//        return 1 + my_strlen(str+1);
//    else
//        return 0;
//}
//int main()
//{
//    char *p = "abcdef";
//    size_t len = my_strlen(p);
//    printf("len = %zu\n",len);
//    return 0;
//}

//char* my_strcpy(char* dest,const char* src)
//{
//    char* ret = dest;
//    assert(dest != '\0');
//    assert(src != '\0');
//    while((*dest++ = *src++))
//    {
//        ;
//    }
//    return ret;      // 因为函数返回的是起始位置，所以用ret来代表dest。
//}
//int main()
//{
//    char arr1[20] = {0};
//    char arr2[] = {'b','i','t'};
//    printf("%s\n", my_strcpy(arr1,arr2));
//    return 0;
//}
#include <string.h>
//char* my_strcat(char* dest,const char* src)
//{
//    char* ret = dest;
//    assert(dest != NULL);
//    assert(src != NULL);
//    //1.找目标空间中的‘\0’
//    while(*dest != '\0')  // 不能写成 while（*dest++ != '\0'）
//    {                     // 如果要写成上述形式，则在大括号外要写成 dest --；
//        dest++;           // 因为当 dest='\0','\0'='\0',dest还要++，会指向
//    }                     // '\0'后面的空间
//    //2.追加内容
//    while ((*dest = *src++))
//    {
//        dest++;
//    }
//    return ret;
//}
//使用strcat(字符串追加)需要注意的问题
//1.源字符串必须以'\0'结束。
//2.目标空间必须足够大，能容纳下源字符串的内容。
//3.目标空间必须可以被修改。
//4.如果要strcat自己给自己追加，则须用strncat。
int main()
{
    char arr1[20];
    char arr2[20];
    strcpy(arr1,"hello ");
    strcpy(arr2,"bit!");
    strncat(arr1,arr2,1);
    printf("%s\n",arr1);
    return 0;
}
