// RMachine.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma once

#include "..//RM/rule_machine.h"

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <chrono>

#include "test_Game001.h"
#include "test_SM.h"


using namespace rm;


int main()
{
    setlocale(LC_ALL, "Russian");

    sgt_messages <base_messages>::get_instance().set(msgs_ru ());

    //main_SM();

    main_Game001();

    //test_param tp;
    //event e(1, std::move(tp));
    //event e(1, tp);

    return 0;
}

