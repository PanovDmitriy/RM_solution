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

    //std::queue<std::pair <std::any, int>> q;

    //const test_param tp;
    //std::any a1 = std::move(const_cast<test_param&>(tp));
    //std::any a2 = std::move(a1);
    //a1.swap(a2);

    //q.emplace(std::move(tp));
    //q.push(std::move(std::pair <test_param, int>{ std::move(tp), 0 }));
    //q.push(std::pair <std::any, int>{ std::move(a1), 0 });
    //q.push(std::pair <test_param, int>{ tp, 0 });

    //event e1(1);
    //event e2(2);
    //rule_machine rmx;
    //state_machine smx(&rmx);
    //smx.rise_event(e1, false);
    //smx.rise_event(std::move(e2), false);
    //smx.rise_event({3}, false);

    return 0;
}

