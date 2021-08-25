#pragma once

#include "..//RM/rule_machine.h"


using namespace rm;

void action1(const event& ref_e)
{
    std::cout << "action1: event: " << ref_e.id << ", " << ref_e.name << std::endl;
}

void action2(const event& ref_e)
{
    std::cout << "action2: event: " << ref_e.id << ", " << ref_e.name << std::endl;
}

bool guard_always_false(const event& ref_e)
{
    std::cout << "guard_always_false: event: " << ref_e.id << ", " << ref_e.name << std::endl;
    return false;
}

bool guard_always_true(const event& ref_e)
{
    std::cout << "guard_always_true: event: " << ref_e.id << ", " << ref_e.name << std::endl;
    return true;
}

void main_SM()
{
    event e12(102, "event 1->2");
    event e23(203, "event 2->3");
    event e31(301, "event 3->1");
    event e21(201, "event 2->1");
    event e32(302, "event 3->2");
    event e13(103, "event 1->3");
    event einit1(001, "event init->1");
    event einit2(002, "event init->2");
    event einit3(003, "event init->3");
    event e1final(100, "event 1->final");
    event e2final(200, "event 2->final");
    event e3final(300, "event 3->final");

    rule_machine rm;
    state_machine sm(&rm);
    rm.add_sm(&sm);

    state_test s1(sm, "state1");
    state_test s2(sm, "state2");
    state_test s3(sm, "state3");
    initial_state si;
    //final_state sf;

    transition_test tinit1(sm, "trans init->1");
    transition_test tinit2(sm, "trans init->2");
    transition_test tinit3(sm, "trans init->3");
    transition_test tfinal1(sm, "trans 1->final");
    transition_test tfinal2(sm, "trans 2->final");
    transition_test tfinal3(sm, "trans 3->final");
    transition_test t12(sm, "trans 1->2");
    transition_test t21(sm, "trans 2->1");
    transition_test t23(sm, "trans 2->3");
    transition_test t32(sm, "trans 3->2");
    transition_test t13(sm, "trans 1->3");
    transition_test t31(sm, "trans 3->1");

    t31.add_guard(guard_always_false);
    t13.add_action(action1);
    t32.add_guard(guard_always_true);
    t23.add_action(action2);

    sm.add_event_state_state_transition(einit1.id, &si, &s1, &tinit1);
    sm.add_event_state_state_transition(einit2.id, &si, &s2, &tinit2);
    sm.add_event_state_state_transition(einit3.id, &si, &s3, &tinit3);
    sm.add_event_state_state_transition(e12.id, &s1, &s2, &t12);
    sm.add_event_state_state_transition(e21.id, &s2, &s1, &t21);
    sm.add_event_state_state_transition(e32.id, &s3, &s2, &t32);
    sm.add_event_state_state_transition(e23.id, &s2, &s3, &t23);
    sm.add_event_state_state_transition(e13.id, &s1, &s3, &t13);
    sm.add_event_state_state_transition(e31.id, &s3, &s1, &t31);
    //sm.add_event_state_state_transition(e1final.id, &s1, &sf, &tfinal1);
    sm.add_event_state_state_transition(e1final.id, &s1, &s3, &tfinal1);
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
    for (int i = 0; true; i++)
    {
        if (sm.get_status() != rm::status::enabled)
        {
            std::cout << "State machine is not Enabled." << std::endl << std::endl;
            break;
        }

        event* ee = &e12;
        switch (rand() % (b - a + 1) + a)
        {
        case 1: ee = &e12; break;
        case 2: ee = &e21; break;
        case 3: ee = &e32; break;
        case 4: ee = &e23; break;
        case 5: ee = &e31; break;
        case 6: ee = &e13; break;
        case 7: ee = &e1final; break;
        case 8: ee = &e2final; break;
        case 9: ee = &e3final; break;
        }
        std::cout << "strike [" << i+1 << "] event: " << ee->name << std::endl << "<" << std::endl;
        auto [rez_b, rem] = sm.release_event(*ee);
        std::cout << "result: " << (rez_b ? "Success (" : "Error (") << rem << ")" << std::endl << ">" << std::endl << std::endl;

        if (!rez_b)
            break;
    }

    sm.check_obj();

    rm.clear();
}
