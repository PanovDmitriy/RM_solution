#pragma once

#include <list>
#include <variant>

#include "play_cards.h"
#include "..//RM/rule_machine.h"


using namespace rm;

/////////////////////////////////////////////////// drank //////////////////////////////////////////////////////


class state_turn_player;
class event_no_card;

class DOM
{
public:
    card_set sc_table;
    card_set sc_under_table;

    // player1
    card_set sc_player1;

    // player2
    card_set sc_player2;

public:
    DOM() :
        sc_table(e_card_set_type::t52),
        sc_under_table(e_card_set_type::t52),
        sc_player1(e_card_set_type::t52),
        sc_player2(e_card_set_type::t52)
    {
        reset();
    }

    void reset()
    {
        sc_table.clear();
        sc_under_table.reset();
        sc_player1.clear();
        sc_player2.clear();
    }

    bool is_all_turned()
    {
        return false;// is_player1_turned&& is_player2_turned;
    }

    friend class state_turn_player;

public:
    event e_init_players {10, "init_players"};
    event e_init_play {20, "init_play"};
    event e_next_turn {30, "next_turn"};
    event e_next_sub_turn {40, "next_sub_turn"};
    event e_start_play {50, "start_play"};
    event e_card_on_table {60, "card_on_table"};
    event_no_card e_on_card {70, "no_card"};
};

class event_no_card :
    event
{
    int player_index = -1;

public:
    event_no_card() = delete;

    event_no_card(id_t id_, std::string name_) :
        event (id_, name_)
    {
    }

    event_no_card& set_player_index(int player_index_)
    {
        player_index = player_index_;
        return *this;
    }

    int get_player_index()
    {
        return player_index;
    }
};

class state_turn_player :
    public state_test
{
private:
    DOM& dom;

public:
    state_turn_player(i_sm_event_invoker_t& invoker_, std::string _name, DOM& dom_) :
        state_test(invoker_, _name),
        dom (dom_)
    {
    }

protected:
    void do_entry_action(const event& ref_e) override
    {
        std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;

        //вход 

        //карты от игрока на стол (1 для next_turn, 2 для next_sub_turn),
        switch (ref_e.id)
        {
        case 30: //next_turn
            if (!dom.sc_player1.move_card_f2b(dom.sc_table))
                this->invoker.invoke(dom.e_on_card.set_player_index(1), false);
            if (!dom.sc_player2.move_card_f2b(dom.sc_table))
                this->invoker.invoke(dom.e_on_card.set_player_index(2), false);
            break;
        case 40: //next_sub_turn
            break;
        }
        //если нет, сообщить "no_cards",
        
        //если есть,

        //положить на стол,
        
        //сообщить "card_on_table"
    }
};

/////////////////////////////////////////////////// end drank //////////////////////////////////////////////////////