#pragma once

#include "..//RM/rule_machine.h"


using namespace rm;

void action1(const Event& ref_e)
{
    std::cout << "action1: event: " << ref_e.id << std::endl;
}

void action2(const Event& ref_e)
{
    std::cout << "action2: event: " << ref_e.id << std::endl;
}

bool guard_always_false(const Event& ref_e)
{
    std::cout << "guard_always_false: event: " << ref_e.id << std::endl;
    return false;
}

bool guard_always_true(const Event& ref_e)
{
    std::cout << "guard_always_true: event: " << ref_e.id << std::endl;
    return true;
}

void main_SM()
{
    Event e12(102, "event 1->2");
    Event e23(203, "event 2->3");
    Event e31(301, "event 3->1");
    Event e21(201, "event 2->1");
    Event e32(302, "event 3->2");
    Event e13(103, "event 1->3");
    Event einit1(001, "event init->1");
    Event einit2(002, "event init->2");
    Event einit3(003, "event init->3");
    Event e1final(100, "event 1->final");
    Event e2final(200, "event 2->final");
    Event e3final(300, "event 3->final");

    RuleMachine rm;
    StateMachine sm;
    rm.add_machine(&sm);

    StateTest s1(rm, "state1");
    StateTest s2(rm, "state2");
    StateTest s3(rm, "state3");
    InitialState si;
    //final_state sf;

    TransitionTest tinit1(rm, "trans init->1");
    TransitionTest tinit2(rm, "trans init->2");
    TransitionTest tinit3(rm, "trans init->3");
    TransitionTest tfinal1(rm, "trans 1->final");
    TransitionTest tfinal2(rm, "trans 2->final");
    TransitionTest tfinal3(rm, "trans 3->final");
    TransitionTest t12(rm, "trans 1->2");
    TransitionTest t21(rm, "trans 2->1");
    TransitionTest t23(rm, "trans 2->3");
    TransitionTest t32(rm, "trans 3->2");
    TransitionTest t13(rm, "trans 1->3");
    TransitionTest t31(rm, "trans 3->1");

    t31.add_guard(guard_always_false);
    t13.add_action(action1);
    t32.add_guard(guard_always_true);
    t23.add_action(action2);

    sm.add_link(einit1.id, &si, &s1, &tinit1);
    sm.add_link(einit2.id, &si, &s2, &tinit2);
    sm.add_link(einit3.id, &si, &s3, &tinit3);
    sm.add_link(e12.id, &s1, &s2, &t12);
    sm.add_link(e21.id, &s2, &s1, &t21);
    sm.add_link(e32.id, &s3, &s2, &t32);
    sm.add_link(e23.id, &s2, &s3, &t23);
    sm.add_link(e13.id, &s1, &s3, &t13);
    sm.add_link(e31.id, &s3, &s1, &t31);
    //sm.add_link(e1final.id, &s1, &sf, &tfinal1);
    sm.add_link(e1final.id, &s1, &s3, &tfinal1);
    //sm.add_link(e2final.id, &s2, &sf, &tfinal2);
    //sm.add_link(e3final.id, &s3, &sf, &tfinal3);

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

        Event* ee = &e12;
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
        std::cout << "strike [" << i+1 << "] event: " << ee->id << std::endl << "<" << std::endl;
        sm.rise_event(*ee);
        auto [rez_b, rem] = sm.release_event();
        std::cout << "result: " << (rez_b ? "Success (" : "Error (") << rem << ")" << std::endl << ">" << std::endl << std::endl;

        if (!rez_b)
            break;
    }

    sm.view_integrity();

    rm.clear();
}
