#pragma once

template<typename T>
class typed_data {
public:
    T data;
    typed_data() {}
    typed_data(T&& d) : data(d) {}
};