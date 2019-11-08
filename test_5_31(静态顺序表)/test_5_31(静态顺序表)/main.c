//
//  main.c
//  test_5_31(静态顺序表)
//
//  Created by 张辉 on 2019/11/8.
//  Copyright © 2019 张辉. All rights reserved.
//

//#include <stdio.h>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    printf("Hello, World!\n");
//    return 0;
//}

//顺序表的静态存储
#define N 7
typedef int SLDataType;
typedef struct SeqList
{
    SLDataType array[N];
    size_t size;
}SeqList;
