// RMachine.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#pragma once

#include "..//RM/rule_machine.h"
#include "..//RM/singleton.h"

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

#include "test_Game001.h"

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

    rm::event e(111, "e111");
    std::cout << "id=" << e.id << std::endl;
    std::cout << "name=" << e.name << std::endl; 

    rm::event e11(101, "e1->2");
    rm::event e12(102, "e2->3");
    rm::event e13(103, "e3->1");
    rm::event e21(201, "e2->1");
    rm::event e22(202, "e3->2");
    rm::event e23(203, "e1->3");

    state_test s1("state1");
    state_test s2("state2");
    state_test s3("state3");

    transition_test l1("trans1");
    transition_test l2("trans2");
    transition_test lb("trans_begin");
    
    transition_test l3_1("trans_l3_1");
    l3_1.add_guard(guard_always_false);
    transition_test l3_2("trans_l3_2");
    l3_2.add_guard(guard_always_true);

    rule_machine rm;
    state_machine sm (&rm);
    rm.add_sm(&sm);

    sm.add_begin_sl(&s1, &lb);
    sm.add_essl(101, &s1, &s2, &l1);
    sm.add_essl(102, &s2, &s3, &l2);
    sm.add_essl(103, &s3, &s1, &l3_1);
    sm.add_essl(103, &s3, &s1, &l3_2);

    sm.add_essl(201, &s2, &s1);
    sm.add_essl(202, &s3, &s2);
    sm.add_essl(203, &s1, &s3);

    sm.set_status(status::enabled);
    std::cout << std::endl;
    sm.set_status(status::paused);
    sm.set_status(status::disabled);
    sm.set_status(status::enabled);
    std::cout << std::endl;

    int a = 1;
    int b = 6;
    for (int i = 0; i < 50; i++)
    {
        std::string res = "";
        switch (rand() % (b - a + 1) + a)
        {
        case 1:
        {
            std::cout << "strike event: e101" << std::endl << "<" << std::endl;
            auto [rez_b, rem] = sm.recv_triggering_event(e11);
            res = rez_b ? "Success" : "Error" + rem;
        }
            break;

        case 2:
        {
            std::cout << "strike event: e102" << std::endl << "<" << std::endl;
            auto [rez_b, rem] = sm.recv_triggering_event(e12);
            res = rez_b ? "Success" : "Error" + rem;
        }
            break;

        case 3:
        {
            std::cout << "strike event: e103" << std::endl << "<" << std::endl;
            auto [rez_b, rem] = sm.recv_triggering_event(e13);
            res = rez_b ? "Success" : "Error" + rem;
        }
            break;

        case 4:
        {
            std::cout << "strike event: e201" << std::endl << "<" << std::endl;
            auto [rez_b, rem] = sm.recv_triggering_event(e21);
            res = rez_b ? "Success" : "Error" + rem;
        }
            break;

        case 5:
        {
            std::cout << "strike event: e202" << std::endl << "<" << std::endl;
            auto [rez_b, rem] = sm.recv_triggering_event(e22);
            res = rez_b ? "Success" : "Error" + rem;
        }
            break;

        case 6:
        {
            std::cout << "strike event: e203" << std::endl << "<" << std::endl;
            auto [rez_b, rem] = sm.recv_triggering_event(e23);
            res = rez_b ? "Success" : "Error" + rem;
        }
            break;
        }
        std::cout << "result: " << res << std::endl << ">" << std::endl << std::endl;
    }

    sm.check_obj();

    rm.clear();

    return 0;
}

