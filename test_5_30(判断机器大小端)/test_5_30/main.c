//
//  main.c
//  test_5_30
//
//  Created by 张辉 on 2019/5/30.
//  Copyright © 2019年 张辉. All rights reserved.
//

//#include <stdio.h>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    printf("Hello, World!\n");
//    return 0;
//}

//判断大小端
//#include <stdio.h>
//int main()
//{
//    int a = 1;
//    if( *(char*)&a == 1 )
//    {
//        printf("小端\n");
//    }
//    else
//    {
//        printf("大端\n");
//    }
//    return 0;
//}
//2>
//#include <stdio.h>
//int check_sys()
//{
//    int a = 1;
//    //返回1表示小端
//    //返回0表示大端
//    return (*(char *)&a);
//}
//int main()
//{
//    int ret = check_sys();
//    if(ret == 1)
//    {
//        printf("小端\n");
//    }
//    else
//    {
//
//        printf("大端\n");
//    }
//    return 0;
//}
//3>
//#include <stdio.h>
//int check_sys()
//{
//    union
//    {
//        int i;
//        char c;
//    }un;
//    un.i = 1;
//    return un.c;//char c=(char)i
//}
//int main()
//{
//    int ret = check_sys();
//    if(ret == 1)
//    {
//        printf("小端\n");
//    }
//    else
//    {
//
//        printf("大端\n");
//    }
//    return 0;
//}
//顺序表的静态存储
//#define N 7
//typedef int SLDataType;
//typedef struct SeqList
//{
//    SLDataType array[N];
//    size_t size;
//}SeqList;
//顺序表的动态存储
typedef struct SeqList
{
    SLDataType* array;
    size_t size;
    size_t capicity;
}SeqList;
void SeqListInit(SeqList* ps1,size_t capicity);
void SeqListDestory(SeqList* ps1);
void CheckCapa

