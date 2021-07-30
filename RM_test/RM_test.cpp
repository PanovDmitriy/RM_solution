// RMachine.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma once

#include "..//RM/rule_machine.h"
#include "..//RM/singleton.h"

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

//#include "test_Game001.h"

#include "mems.h"

using namespace rm;


void action1(event& ref_e)
{
    std::cout << "action1: event: " << ref_e.id << ", " << ref_e.name << std::endl;
}

void action2(event& ref_e)
{
    std::cout << "action2: event: " << ref_e.id << ", " << ref_e.name << std::endl;
}

bool guard_always_false(event& ref_e)
{
    std::cout << "guard_always_false: event: " << ref_e.id << ", " << ref_e.name << std::endl;
    return false;
}

bool guard_always_true(event& ref_e)
{
    std::cout << "guard_always_true: event: " << ref_e.id << ", " << ref_e.name << std::endl;
    return true;
}



bool isPalindrome(int x)
{
    if (x < 0)
        return false;

    std::string s = std::to_string(x);
    int len = s.length();
    if (len == 1)
        return true;

    int i = 0;
    int ib = len - 1;
    while (s[i] == s[ib] && i < ib)
    {
        i++;
        ib--;
    }

    if (i >= ib)
        return true;

    if (!(len & 1))
        if (ib - i < 3)
            return true;

    return false;
}



int main()
{
    //Singleton& sig = Singleton::getInstance();

    //DOM dom;
    //dom.reset();
    //dom.sc_main.show("dom main set---------------");

    //event ev_next_sub_turn(1, "next_sub_turn");
    //event ev_next_turn(2, "next_turn");
    //event ev_card_on_table(3, "card_on_table");
    //event ev_no_card(4, "no_card");

    //rule_machine rm;

    //sm_player sm_gamer1(1, &rm);
    //rm.add_sm(&sm_gamer1);
    //st_turn st_turn_obj1(sm_gamer1);
    //sm_gamer1.add_item(ev_next_turn.id, nullptr, &st_turn_obj1, &st_turn_obj1, true);
    //sm_gamer1.add_item(ev_next_sub_turn.id, nullptr, &st_turn_obj1, &st_turn_obj1);

    //sm_player sm_gamer2(2, &rm);
    //rm.add_sm(&sm_gamer2);
    //st_turn st_turn_obj2(sm_gamer2);
    //sm_gamer2.add_item(ev_next_turn.id, nullptr, &st_turn_obj2, &st_turn_obj2, true);
    //sm_gamer2.add_item(ev_next_sub_turn.id, nullptr, &st_turn_obj2, &st_turn_obj2);

    //sm_play sm_game(&rm);
    //rm.add_sm(&sm_game);
    //st_pre_game st_pre_game_obj(sm_game);
    //st_game st_game_obj(sm_game);
    //state st_end_obj("state end");
    //ln_end ln_end_obj(sm_game);
    //ln_first_turn ln_first_turn_obj(sm_game);
    //ln_com_turn ln_com_turn_obj(sm_game);
    //sm_game.add_item(ev_card_on_table.id, &ln_first_turn_obj, &st_pre_game_obj, &st_game_obj, true);
    //sm_game.add_item(ev_card_on_table.id, &ln_com_turn_obj, &st_game_obj, &st_game_obj);
    //sm_game.add_item(ev_no_card.id, &ln_end_obj, &st_game_obj, &st_end_obj);

    //sm_gamer1.check_obj();
    //sm_gamer2.check_obj();
    //sm_game.check_obj();

    ////card_set cs1, cs2;
    ////while (true)
    ////{
    ////    if (!cs.move_card_f2b(cs1)) break;
    ////    if (!cs.move_card_f2b(cs2)) break;
    ////}
    ////cs.show("cs");
    ////cs1.show("cs1");
    ////cs2.show("cs2");

    //rm::event e(111, "e111");
    //std::cout << "id=" << e.id << std::endl;
    //std::cout << "name=" << e.name << std::endl; 

    //std::string s = "abcabcbb";
    //std::string s = "au";
    //std::string s="bbbbb";
    std::string s= "acabdkacaa";
    //std::string s="";



    bool bbb = isPalindrome(11);


    return 0;

    event e12(102, "event 1->2", true);
    event e23(203, "event 2->3", true);
    event e31(301, "event 3->1", true);
    event e21(201, "event 2->1", true);
    event e32(302, "event 3->2", true);
    event e13(103, "event 1->3", true);
    event einit1(001, "event init->1", true);
    event einit2(002, "event init->2", true);
    event einit3(003, "event init->3", true);
    event e1final(100, "event 1->final", true);
    event e2final(200, "event 2->final", true);
    event e3final(300, "event 3->final", true);

    state_test s1("state1");
    state_test s2("state2");
    state_test s3("state3");
    initial_state si;
    final_state sf;

    transition_test tinit1("trans init->1");
    transition_test tinit2("trans init->2");
    transition_test tinit3("trans init->3");
    transition_test tfinal1("trans 1->final");
    transition_test tfinal2("trans 2->final");
    transition_test tfinal3("trans 3->final");
    transition_test t12("trans 1->2");
    transition_test t21("trans 2->1");
    transition_test t23("trans 2->3");
    transition_test t32("trans 3->2");
    transition_test t13("trans 1->3");
    transition_test t31("trans 3->1");

    t31.add_guard(guard_always_false);
    t13.add_action(action1);
    t32.add_guard(guard_always_true);
    t23.add_action(action2);

//    rule_machine rm;
    state_machine sm (nullptr);
//    rm.add_sm(&sm);

    sm.add_event_state_state_transition(einit1.id, &si, &s1, &tinit1);
    sm.add_event_state_state_transition(einit2.id, &si, &s2, &tinit2);
    sm.add_event_state_state_transition(einit3.id, &si, &s3, &tinit3);
    sm.add_event_state_state_transition(e12.id, &s1, &s2, &t12);
    sm.add_event_state_state_transition(e21.id, &s2, &s1, &t21);
    sm.add_event_state_state_transition(e32.id, &s3, &s2, &t32);
    sm.add_event_state_state_transition(e23.id, &s2, &s3, &t23);
    sm.add_event_state_state_transition(e13.id, &s1, &s3, &t13);
    sm.add_event_state_state_transition(e31.id, &s3, &s1, &t31);
    sm.add_event_state_state_transition(e1final.id, &s1, &sf, &tfinal1);
    //sm.add_essl(e2final.id, &s2, &sf, &tfinal2);
    //sm.add_essl(e3final.id, &s3, &sf, &tfinal3);

    sm.set_status_enabled (einit1);
    std::cout << std::endl;
    sm.set_status_paused();
    sm.set_status_disabled();
    sm.set_status_enabled(einit2);
    std::cout << std::endl;

    int a = 1;
    int b = 9;
    for (int i = 0; i < 50; i++)
    {
        if (sm.get_status() != rm::status::enabled)
        {
            std::cout << "State machine is not Ensbled." << std::endl << std::endl;
            break;
        }

        event* ee = &e12;
        switch (rand() % (b - a + 1) + a)
        {
        case 1:
            ee = &e12;
            break;

        case 2:
            ee = &e21;
            break;

        case 3:
            ee = &e32;
            break;

        case 4:
            ee = &e23;
            break;

        case 5:
            ee = &e31;
            break;

        case 6:
            ee = &e13;
            break;

        case 7:
            ee = &e1final;
            break;

        case 8:
            ee = &e2final;
            break;

        case 9:
            ee = &e3final;
            break;
        }
        std::cout << "strike [" << i+1 << "] event: " << ee->name << std::endl << "<" << std::endl;
        auto [rez_b, rem] = sm.recv_triggering_event(*ee);
        std::cout << "result: " << (rez_b ? "Success (" : "Error (") << rem << ")" << std::endl << ">" << std::endl << std::endl;

        if (!rez_b)
            break;
    }

    sm.check_obj();

    //rm.clear();

    return 0;
}

