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

    id.set(std::move(d));

    d = 55.5;

    std::cout << "Hello World!\n" << "id: " << id.to_string();
}



