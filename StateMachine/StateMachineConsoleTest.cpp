// StateMachine.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// 
// Для тестирования
//


#include <iostream>

#include "base.h"


int main()
{
    Machines::Id<double, 0.0> id = 11.1;

    //id = 12;

    double d = 33.3;

    id = std::move (1.2);

    //d = 55.5;

    std::cout << "id: " << id.to_string() << std::endl;
    std::cout << "d: " << d << std::endl;
}



