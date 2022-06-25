// RMachine.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma once

#include "..//RM/rule_machine.h"

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <chrono>

#include "Game001_sample.h"
#include "SM_sample.h"

using namespace rm;



int main()
{
    setlocale(LC_ALL, "Russian");

    sgt_messages <base_messages>::get_instance().set(msgs_ru ());

    //main_SM();

    main_Game001();

    //test_param tp;
    //event e1(1);//, tp);
    //event e2(2, test_param {});
    //rule_machine rmx;
    //state_machine smx(&rmx);
    //smx.rise_event(std::move(e1), true);
    //smx.rise_event(std::move(e2), true);
    //smx.rise_event({3}, true);

    //std::cout << "\n\n\nenter char: ";
    //char c = 0;
    //std::cin >> c;

    return 0;
}

