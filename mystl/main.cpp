#include <iostream>
#include <memory>
#include <cstring>
#include <vector>
#include "vector.h"
#include "cstring.h"
#include "simpletemplate.h"
using namespace std;

int main()
{
    //test
    const char* pc = "hello my string";
    CString str(pc);

    cout<<str.m_data<<endl;
    CString str1 = str;
    cout<<str1.m_data<<endl;

    //添加字符
    CString str2 = str1 + CString("+ world");
    cout<<str2.m_data<<endl;

    //移动构造
    CString str3(std::move(str2));
    cout<<str3.m_data<<endl;

    //拷贝赋值
    CString str4;
    str4 = str1;

    //移动
    CString str5;
    str5 = std::move(str3);
    
    SimpleT<int> test1;
    int i = test1.add(2,4);

    SimpleT<string> test2;
    string str6 = test2.add("hello"," simplet");
    cout<<"simplet string ="<<str6<<endl;

    CVector<int> iv(1);
    iv.push_back(2);
    iv.push_back(4);
    cout<<"vector size ="<<iv.size()<<" capacity = "<<iv.capacity()<<endl;
 
    CVector<int> fv(1);
    fv.push_back(2.1);
    fv.push_back(4.2);
    cout<<"fv vector size ="<<fv.size()<<" capacity = "<<fv.capacity()<<endl;

    CVector<string> strv(1,"cc");
    strv.push_back(string("aa"));
    strv.push_back(string("bb"));
    // strv.push_back(string("dd"));
    strv.insert(strv.beging()+2, string("dd"));
    //strv.insert(strv.end() - 2, 5, "abc");
    // strv.insert(strv.beging()+4, string("abc"));
    cout<<"vector str size ="<<strv.size()<<" capacity = "<<strv.capacity()<<endl;
    string* begin = strv.beging();
    do
    {
        std::cout<<*begin<<"; ";
    } while (++begin != strv.end());
    std::cout<<std::endl;

    TestTemplate<int, int> test4;
    TestTemplate<int*, char*> test5;
    TestTemplate<const int&, const float&> test6;

    system("pause");

    return 0;
}
