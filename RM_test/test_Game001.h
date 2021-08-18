#pragma once

#include <list>
#include <variant>

#include "play_cards.h"
#include "..//RM/rule_machine.h"


using namespace rm;

/////////////////////////////////////////////////// drank //////////////////////////////////////////////////////


class state_turn_player;
class state_pre_play;
class state_turn_play;

class event_no_card :
    public event
{
    int player_index = -1;

public:
    event_no_card() = delete;

    event_no_card(id_t id_, std::string name_) :
        event(id_, name_)
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

class event_card_on_table :
    public event
{
    card player1_card_on_table = e_card_mark::Joker;
    card player2_card_on_table = e_card_mark::Joker;

public:
    event_card_on_table() = delete;

    event_card_on_table(id_t id_, std::string name_) :
        event(id_, name_)
    {
    }

    void set_player1_card_on_table(const card& c)
    {
        player1_card_on_table = c;
    }
    void set_player2_card_on_table(const card& c)
    {
        player2_card_on_table = c;
    }

    card get_player1_card_on_table()
    {
        return player1_card_on_table;
    }
    card get_player2_card_on_table()
    {
        return player2_card_on_table;
    }
};

class DOM
{
public:
    card_set sc_table;

    // player1
    card_set sc_player1;

    // player2
    card_set sc_player2;

public:
    DOM() :
        sc_table(e_card_set_type::t52),
        sc_player1(e_card_set_type::t52),
        sc_player2(e_card_set_type::t52)
    {
        reset();
    }

    void reset()
    {
        sc_table.reset();
        sc_player1.clear();
        sc_player2.clear();
    }

    bool is_all_turned()
    {
        return false;// is_player1_turned&& is_player2_turned;
    }

    friend class state_turn_player;
    friend class state_turn_play;
    friend class state_pre_play;

public:
    event e_init_players = event (10, "init_players");
    event e_init_play = event (20, "init_play");
    event e_next_turn = event(30, "next_turn");
    event e_next_sub_turn = event(40, "next_sub_turn");
    event e_start_play = event(50, "start_play");
    event_card_on_table e_card_on_table = event_card_on_table(60, "card_on_table");
    event_no_card e_no_card = event_no_card(70, "no_card");
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
            {
                std::cout << "player1: no card!" << std::endl;
                this->invoker.invoke(dom.e_no_card.set_player_index(1), false);
            }
            std::cout << "player1: card_on_table: " << dom.sc_table.get_first_card().get_name() << std::endl;
            dom.e_card_on_table.set_player1_card_on_table (dom.sc_table.get_first_card());

            if (!dom.sc_player2.move_card_f2b(dom.sc_table))
            {
                std::cout << "player2: no card!" << std::endl;
                this->invoker.invoke(dom.e_no_card.set_player_index(2), false);
            }
            std::cout << "player2: card_on_table: " << dom.sc_table.get_first_card().get_name() << std::endl;
            dom.e_card_on_table.set_player2_card_on_table(dom.sc_table.get_first_card());

            break;
        case 40: //next_sub_turn
            if (!dom.sc_player1.move_card_f2b(dom.sc_table))
            {
                std::cout << "player1: no card!" << std::endl;
                this->invoker.invoke(dom.e_no_card.set_player_index(1), false);
            }
            std::cout << "player1: card_on_table: " << dom.sc_table.get_first_card().get_name() << std::endl;
            if (!dom.sc_player2.move_card_f2b(dom.sc_table))
            {
                std::cout << "player1: no card!" << std::endl;
                this->invoker.invoke(dom.e_no_card.set_player_index(2), false);
            }
            std::cout << "player1: card_on_table: " << dom.sc_table.get_first_card().get_name() << std::endl;
            dom.e_card_on_table.set_player1_card_on_table(dom.sc_table.get_first_card());

            if (!dom.sc_player1.move_card_f2b(dom.sc_table))
            {
                std::cout << "player2: no card!" << std::endl;
                this->invoker.invoke(dom.e_no_card.set_player_index(2), false);
            }
            std::cout << "player2: card_on_table: " << dom.sc_table.get_first_card().get_name() << std::endl;
            if (!dom.sc_player2.move_card_f2b(dom.sc_table))
            {
                std::cout << "player2: no card!" << std::endl;
                this->invoker.invoke(dom.e_no_card.set_player_index(2), false);
            }
            std::cout << "player2: card_on_table: " << dom.sc_table.get_first_card().get_name() << std::endl;
            dom.e_card_on_table.set_player2_card_on_table(dom.sc_table.get_first_card());

            break;
        }
        //если нет, сообщить "no_cards",

        //если есть,

        //положить на стол,
        
        //сообщить "card_on_table"
        this->invoker.invoke(dom.e_card_on_table, false);
    }
};

class state_pre_play :
    public state_test
{
private:
    DOM& dom;

public:
    state_pre_play(i_sm_event_invoker_t& invoker_, std::string _name, DOM& dom_) :
        state_test(invoker_, _name),
        dom(dom_)
    {
    }

protected:
    void do_entry_action(const event& ref_e) override
    {
        std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;

        //вход 

        //  раздать карты
        dom.reset();
        dom.sc_table.shuffle();
        while (dom.sc_table.move_card_f2f(dom.sc_player1) || dom.sc_table.move_card_f2f(dom.sc_player2));

        //сообщить "next_turn"
        invoker.invoke(dom.e_next_turn, false);
    }
};

class state_turn_play :
    public state_test
{
private:
    DOM& dom;

public:
    state_turn_play(i_sm_event_invoker_t& invoker_, std::string _name, DOM& dom_) :
        state_test(invoker_, _name),
        dom(dom_)
    {
    }

protected:
    void do_entry_action(const event& ref_e) override
    {
        std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;

        //вход 

        //if (ref_e.id == 60)
        //{
        //}
        event_card_on_table* ptr_e = dynamic_cast<event_card_on_table*>(&const_cast<event&>(ref_e));
        if (ptr_e)
        {
            //сравнить
            //если не равны,
            //передать карты
            //сообщить "next_turn"
            //если равны,
            //сообщить "next_sub_turn"
            card c1 = ptr_e->get_player1_card_on_table();
            card c2 = ptr_e->get_player2_card_on_table();
            if (c1 < c2)
            {
                std::cout << c1.get_name() << " < " << c2.get_name() << std::endl;
                std::cout << "cards move to player2" << std::endl;
                while (dom.sc_table.move_card_f2b(dom.sc_player2));
                invoker.invoke(dom.e_next_turn, false);
            }
            else if (c2 < c1)
            {
                std::cout << c1.get_name() << " > " << c2.get_name() << std::endl;
                std::cout << "cards move to player1" << std::endl;
                while (dom.sc_table.move_card_f2b(dom.sc_player1));
                invoker.invoke(dom.e_next_turn, false);
            }
            else if (c1 == c2)
            {
                std::cout << c1.get_name() << " = " << c2.get_name() << std::endl;
                std::cout << "cards stay on table" << std::endl;
                invoker.invoke(dom.e_next_sub_turn, false);
            }
        }
    }
};

/////////////////////////////////////////////////// end drank //////////////////////////////////////////////////////