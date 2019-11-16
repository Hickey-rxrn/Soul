//
//  main.cpp
//  test_11_16(newcode刷题)
//
//  Created by 张辉 on 2019/11/16.
//  Copyright © 2019 可乐. All rights reserved.
//

//#include <iostream>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return 0;
//}

//请编写一个函数（允许增加子函数），计算n x m的棋盘格子（n为横向的格子数，m为竖向的格子数）沿着各自边缘线从左上角走到右下角，总共有多少种走法，要求不能走回头路，即：只能往右和往下走，不能往左和往上走。
//   例如 2 2   输出 6
#include <iostream>
using namespace std;
int pathNum(int n, int m)
{
    if (n > 1 && m > 1)
        //b情况，递归
        return pathNum(n - 1, m) + pathNum(n, m - 1);
    else if (((n >= 1) && (m == 1)) || ((n == 1) && (m >= 1)))
        // a情况，终止条件
        return n + m;
    else
        //格子为0时， 路径为0
        return 0;
}

int main()
{
   int ret = pathNum(2, 2);
    cout << ret << endl;
    
    return 0;
}


//思路

//如果n或者m为1，则只有一行或者一列，从左上角走到右下角的路径数为n + m 比如： 1 * 1格子，可以先向下走，再向右走，到达右下角； 或者先向右走，再向下走，到达右下角，共两条，即 1 + 1 = 2，
//  对于1 * m 和 n * m 的情况同学们自己画一下
//如果n,m都大于1，那么走到[n][m]格子的右下角只有两条路径，
//1.: 从[n - 1][m]格子的右下角向下走，到达
//2.: 从[n][m - 1]格子的右下角向右走，到达
//所以走到[n][m]格子的右下角的数量为[n-1][m] + [n][m-1], 可以通过递归实现，情况a为递归的终止条件。
