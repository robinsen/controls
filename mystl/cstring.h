#pragma once
#include <iostream>
#include <string>

class CString {
public:
    CString(const char* str = nullptr) {
        if (str == nullptr)
        {
            m_data = new char[1];
            m_data[0] = '\0';
        }
        else
        {
            int len = strlen(str);
            delete[] m_data;
            m_data = new char[len+1];
            strcpy(m_data, str);
        }
    }
    CString(const CString& str) {
            int len = strlen(str.m_data);
            m_data = new char[len+1];
            strcpy(m_data, str.m_data);
    }
    //移动构造
    CString(CString&& str) {
            m_data = str.m_data;
            str.m_data = nullptr;
    }
    ~CString()
    {
        if (m_data)
            delete[] m_data;
    }

    CString operator+(const CString& str) {
        CString newStr;
        newStr.m_data = new char[strlen(m_data)+strlen(str.m_data)];
        strcpy(newStr.m_data, m_data);
        strcpy(newStr.m_data+strlen(m_data), str.m_data);
        std::cout<<" operator+ "<<std::endl;
        return newStr;
    }
    //拷贝赋值
    CString& operator=(const CString& str) {
        if (this != &str)
        {
            delete[] m_data;
            int len = strlen(str.m_data);
            m_data = new char(len+1);
            strcpy(m_data, str.m_data);
        }
        std::cout<<" operator= "<<std::endl;
        return *this;
    }
    //移动赋值   右值引用
    CString& operator=(CString&& str) {
        if (this != &str)
        {
            delete[] m_data;
            m_data = str.m_data;
            str.m_data = nullptr;
        }
        std::cout<<" move"<<std::endl;
        return *this;
    }

    char* m_data=nullptr;
    int   m_len;
};