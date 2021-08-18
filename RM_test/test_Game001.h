#pragma once

#include <list>
#include <variant>

#include "play_cards.h"
#include "..//RM/rule_machine.h"


using namespace rm;

/////////////////////////////////////////////////// drank //////////////////////////////////////////////////////




class DOM
{
public:
    card_set sc_main;

    // player1
    card_set sc_player1;
    //bool is_player1_turned = false;

    // player2
    card_set sc_player2;
    //bool is_player2_turned = false;

public:
    DOM() :
        sc_main(e_card_set_type::t52),
        sc_player1(e_card_set_type::t52),
        sc_player2(e_card_set_type::t52)
    {
    }

    void reset()
    {
        sc_main.reset();
        sc_player1.clear();
        sc_player2.clear();
    }

    bool is_all_turned()
    {
        return false;// is_player1_turned&& is_player2_turned;
    }
};

/////////////////////
//
//class sm_player :
//    public state_machine
//{
//protected:
//    int player_index;
//
//public:
//    sm_player(int player_index_, i_sm_event_invoker* invoker_ = NULL) :
//        player_index(player_index_),
//        state_machine(invoker_)
//    {
//    }
//
//public:
//    friend class st_turn;
//};
//
//class st_turn :
//    public state
//{
//protected:
//    sm_player& owner_ref;
//
//public:
//    st_turn(sm_player& owner_) :
//        owner_ref(owner_)
//    {
//    }
//
//    std::string to_string() override
//    {
//        return "state st_turn";
//    }
//
//    void do_entry_action(const event& ref_e) override
//    {
//        std::cout << "st_turn : do_entry_action (" << ref_e.name << ")" << std::endl;
//
//        //вход /
//        //    взять карту(1 для next_turn, 2 для next_sub_turn),
//        //    если нет, сообщить "no_cards",
//        //    если есть,
//        //    положить на стол,
//        //    сообщить "card_on_table"
//        //if (false)
//        //{
//        //    if (dom->sc_main.move_card_f2b(dom->sc_player1))
//        //    {
//
//        //    }
//        //    else
//        //    {
//
//        //    }
//        //}
//        //else
//        //    std::cout << "DOM is not set" << std::endl;
//    }
//};
//
///////////////////////
//
//class sm_play :
//    public state_machine
//{
//public:
//    sm_play(i_sm_event_invoker* invoker_ = NULL) :
//        state_machine(invoker_)
//    {
//    }
//};
//
//class st_pre_game :
//    public state
//{
//protected:
//    sm_play& owner_ref;
//
//public:
//    st_pre_game(sm_play& owner_) :
//        owner_ref(owner_)
//    {
//    }
//
//    std::string to_string() override
//    {
//        return "state st_pre_game";
//    }
//
//    virtual void do_entry_action(const event& ref_e) override
//    {
//        std::cout << "st_pre_game : do_entry_action (" << ref_e.name << ")" << std::endl;
//
//        //вход /
//        //    раздать карты
//        //    сообщить "next_turn"
//    }
//};
//
//class st_game :
//    public state
//{
//protected:
//    sm_play& owner_ref;
//
//public:
//    st_game(sm_play& owner_) :
//        owner_ref(owner_)
//    {
//    }
//
//    std::string to_string() override
//    {
//        return "state st_game";
//    }
//
//    virtual void do_entry_action(const event& ref_e) override
//    {
//        std::cout << "st_game : do_entry_action (" << ref_e.name << ")" << std::endl;
//
//        //вход /
//        //    сравнить
//        //    если не равны,
//        //    передать карты
//        //    сообщить "next_turn"
//        //    если равны,
//        //    сообщить "next_sub_turn"
//    }
//};
//
//class ln_end :
//    public transition
//{
//protected:
//    sm_play& owner_ref;
//
//public:
//    ln_end(sm_play& owner_) :
//        owner_ref(owner_)
//    {
//    }
//
//    void do_action(const event& ref_e) override
//    {
//        std::cout << "ln_end : do_action (" << ref_e.name << ")" << std::endl;
//
//        // get winner
//        std::cout << "------------------------------------------------------\n";
//        std::cout << "Winner: " << std::endl;
//        std::cout << "------------------------------------------------------\n";
//    }
//};
//
//class ln_first_turn :
//    public transition
//{
//protected:
//    sm_play& owner_ref;
//
//public:
//    ln_first_turn(sm_play& owner_) :
//        owner_ref(owner_)
//    {
//    }
//
//    bool is_guard_condition(const event& ref_e) override
//    {
//        std::cout << "ln_first_turn : guard_condition (" << ref_e.name << ")" << std::endl;
//
//        // all turned ?
//
//        return true;
//    }
//};
//
//class ln_com_turn :
//    public transition
//{
//protected:
//    sm_play& owner_ref;
//
//public:
//    ln_com_turn(sm_play& owner_) :
//        owner_ref(owner_)
//    {
//    }
//
//    bool is_guard_condition(const event& ref_e) override
//    {
//        std::cout << "ln_com_turn : guard_condition (" << ref_e.name << ")" << std::endl;
//
//        // all turned ?
//
//        return true;
//    }
//};


/////////////////////////////////////////////////// end drank //////////////////////////////////////////////////////