#pragma once

#include <iostream>

template<typename T>
T foo(T l)
{
    if (std::is_same<T, int>::value)
    {
        std::cout << "int" << std::endl;
        return 42;
    }
    else if (std::is_same<T, std::string>::value)
    {
        std::cout << "string" << std::endl;
        return "42";
    }
    else
    {
        std::cout << "ni to ne se" << std::endl;
        return T();
    }
}


