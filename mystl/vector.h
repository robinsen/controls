#pragma once

#include <memory>
#include <iostream>

template<typename T>
class CVector {
public:
    CVector():
    _start(nullptr),
    _end(nullptr),
    _endofStorage(nullptr)
    {

    }
    CVector(size_t n) {
        _start = _alloc.allocate(n);
        _end = _start+n;
        _endofStorage = _end;
        std::uninitialized_fill(_start, _start + n, 0);
    }
    CVector(size_t n, const T& value) {
        _start = _alloc.allocate(n);
        _end = _start+n;
        _endofStorage = _end;
        std::uninitialized_fill(_start, _start+n, value);
    }
    CVector(const CVector& value) {
        size_t n = value.capacity();
        _start = _alloc.allocate(n);
        _endofStorage = _start+n;;
        _end = std::uninitialized_copy(value.beging(), value.beging()+value.size(), _start);
    }

    T* beging() {
        return _start;
    }

    T* end() {
        return _end;
    }

    T& front() {
        return *beging();
    }

    T& back() {
        return *(end()-1);
    }

    T& operator[](size_t n) {
        return *(beging()+n);
    }

    size_t size() {
        if (_end >= _start)
            return _end - _start;
        return 0;
    }

    size_t capacity() {
        if (_endofStorage >= _start)
            return _endofStorage - _start;
        return 0;
    }
    void destroy() {
        if (_start)
        {
            while (_end != _start)
            {
                _alloc.destroy(--_end);
            }
        }
        _alloc.deallocate(_start, _endofStorage - _start);
        _start = _end = _endofStorage = nullptr;
    }

    void push_back(const T& value);
    void insert(T* ite, const T& value);
    void insert(T* ite, size_t n, const T& value);

    T*   erase(T* ite);
    
private:
    T*    _start=nullptr;
    T*    _end=nullptr;
    T*    _endofStorage=nullptr; 
    std::allocator<T> _alloc;
};

template<typename T>
void CVector<T>::push_back(const T& value)
{
    if (_end != _endofStorage)
    {
        _alloc.construct(_end, value);
        ++_end;
    }
    else
    {
        //内存不够要重新分配内存
        size_t n = capacity();
        T* _p = _alloc.allocate(2*n);
        T* _newEnd = std::uninitialized_copy(_start, _end, _p);
        _alloc.construct(_newEnd, value);
        destroy();
        _start = _p;
        _end = _newEnd;
        _endofStorage = _p + 2*n;
        ++_end;
    }
}

template<typename T>
void CVector<T>::insert(T* ite, const T& value)
{
    insert(ite, 1, value);  
}

template<typename T>
void CVector<T>::insert(T* ite, size_t n, const T& value)
{
    if (n+size()>capacity())
    {
        //比容量大，则要重新分配内存
        size_t oldCapacity = capacity();
        size_t newCapacity = 2*(oldCapacity + std::max(n, oldCapacity));
        T* _newStart = _alloc.allocate(newCapacity);
        T* newEnd = std::uninitialized_copy(_start, ite, _newStart);
        newEnd = std::uninitialized_fill_n(newEnd, n, value);
        newEnd = std::uninitialized_copy(ite, _end, newEnd);
        destroy();
        _start = _newStart;
        _end = newEnd;
        _endofStorage = _start+newCapacity;
    }
    else if (_end != _endofStorage)
    {
        std::copy_backward(ite, _end, _end+n);
        std::fill_n(ite, n, value);    
        _end += n;
    }
}

template<typename T>
T* CVector<T>::erase(T* ite)
{
    if ((ite+1) != _end)
    {
        std::copy(ite+1, end(), ite);
    }
    --_end;
    _alloc.destroy(_end);
    return ite;
}