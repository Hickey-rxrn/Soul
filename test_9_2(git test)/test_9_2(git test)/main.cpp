//
//  main.cpp
//  test_9_2(git test)
//
//  Created by 张辉 on 2020/9/2.
//  Copyright © 2020 可乐. All rights reserved.
//

//#include <iostream>
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//    std::cout << "Hello, World!\n";
//    return 0;
//}

#include <iostream>
using namespace std;
#include <string>
#include <map>
void TestMap()
{
    map<string, string> m;
    m.insert(make_pair("banan", "香蕉"));
    for (auto& e : m)
    {
        cout << e.first << endl << e.second << endl;
    }
}


int main()
{
    TestMap();
    return 0;
}
