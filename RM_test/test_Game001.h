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



class DOM
{
public:
    card_set sc_table;
    card_set sc_player1;
    card_set sc_player2;

    int turn_n = 1;

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
        turn_n = 1;
        sc_table.reset();
        sc_player1.clear();
        sc_player2.clear();
    }

    friend class state_turn_player;
    friend class state_turn_play;
    friend class state_pre_play;

public:
    const id_t init_players_id = 10;
    const id_t init_play_id = 20;
    const id_t next_turn_id = 30;
    const id_t next_sub_turn_id = 40;
    const id_t start_play_id = 50;
    const id_t card_on_table_id = 60;
    const id_t no_card_id = 70;
};

class state_turn_player :
    public state
{
private:
    DOM& dom;

public:
    state_turn_player(i_sm_event_invoker_t& invoker_, DOM& dom_) :
        state(invoker_),
        dom (dom_)
    {
    }

protected:
    void do_entry_action(const event& ref_e) override
    {
        card c1(e_card_mark::Joker);
        card c2(c1);
        
        //карты от игрока на стол (1 для next_turn, 2 для next_sub_turn),
        //если нет, сообщить "no_cards",
        //если есть,
        //положить на стол,
        //сообщить "card_on_table"

        if (ref_e.id == dom.next_turn_id)
        {
            std::cout << "Ход: " << dom.turn_n << std::endl;
            if (!dom.sc_player1.move_card_f2f(dom.sc_table))
            {
                std::cout << "player1: нет карт!" << std::endl;
                this->invoker.invoke(event(dom.no_card_id, "no_card", int(1)), false);
                return;
            }
            std::cout << "player1: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player1.get_size() << std::endl;
            c1 = dom.sc_table.get_first_card();

            if (!dom.sc_player2.move_card_f2f(dom.sc_table))
            {
                std::cout << "player2: нет карт!" << std::endl;
                this->invoker.invoke(event(dom.no_card_id, "no_card", int(2)), false);
                return;
            }
            std::cout << "player2: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player2.get_size() << std::endl;
            c2 = dom.sc_table.get_first_card();
        }
        else if (ref_e.id == dom.next_sub_turn_id)
        {
            std::cout << "Двойной ход: " << dom.turn_n << std::endl;
            if (!dom.sc_player1.move_card_f2f(dom.sc_table))
            {
                std::cout << "player1: нет карт!" << std::endl;
                this->invoker.invoke(event(dom.no_card_id, "no_card", int(1)), false);
                return;
            }
            std::cout << "player1: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player1.get_size() << std::endl;
            if (!dom.sc_player1.move_card_f2f(dom.sc_table))
            {
                std::cout << "player1: нет карт!" << std::endl;
                this->invoker.invoke(event(dom.no_card_id, "no_card", int(1)), false);
                return;
            }
            std::cout << "player1: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player1.get_size() << std::endl;
            c1 = dom.sc_table.get_first_card();
            //
            if (!dom.sc_player2.move_card_f2f(dom.sc_table))
            {
                std::cout << "player2: нет карт!" << std::endl;
                this->invoker.invoke(event(dom.no_card_id, "no_card", int(2)), false);
                return;
            }
            std::cout << "player2: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player2.get_size() << std::endl;
            if (!dom.sc_player2.move_card_f2f(dom.sc_table))
            {
                std::cout << "player2: нет карт!" << std::endl;
                this->invoker.invoke(event(dom.no_card_id, "no_card", int(2)), false);
                return;
            }
            std::cout << "player2: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player2.get_size() << std::endl;
            c2 = dom.sc_table.get_first_card();
        }

        dom.turn_n++;

        this->invoker.invoke(event (dom.card_on_table_id, "card_on_table", std::pair<card, card>(c1, c2)), false);
    }
};

class state_pre_play :
    public state
{
private:
    DOM& dom;

public:
    state_pre_play(i_sm_event_invoker_t& invoker_, DOM& dom_) :
        state(invoker_),
        dom(dom_)
    {
    }

protected:
    void do_entry_action(const event& ref_e) override
    {
        //  раздать карты
        //dom.reset();
        //dom.sc_table.shuffle();
        ////dom.sc_table.shuffle();
        ////dom.sc_table.shuffle();
        ////dom.sc_table.shuffle();
        //while (true)
        //{
        //    if (!dom.sc_table.move_card_f2f(dom.sc_player1))
        //        break;
        //    if (!dom.sc_table.move_card_f2f(dom.sc_player2))
        //        break;
        //}

        dom.sc_table.clear(); 
        dom.sc_player1.clear();
        dom.sc_player2.clear();

        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::Q));
        dom.sc_player1.add_card(card(e_card_suit::Heart, e_card_num::n4));
        dom.sc_player1.add_card(card(e_card_suit::Heart, e_card_num::n9));
        dom.sc_player1.add_card(card(e_card_suit::Heart, e_card_num::Q));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::K));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::J));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::n2));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::n8));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::n9));
        dom.sc_player1.add_card(card(e_card_suit::Diamond, e_card_num::n4));
        dom.sc_player1.add_card(card(e_card_suit::Heart, e_card_num::n6));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::n10));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::A));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::n8));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::n4));
        dom.sc_player1.add_card(card(e_card_suit::Diamond, e_card_num::J));
        dom.sc_player1.add_card(card(e_card_suit::Heart, e_card_num::n7));
        dom.sc_player1.add_card(card(e_card_suit::Diamond, e_card_num::A));
        dom.sc_player1.add_card(card(e_card_suit::Heart, e_card_num::n3));
        dom.sc_player1.add_card(card(e_card_suit::Club, e_card_num::n3));
        dom.sc_player1.add_card(card(e_card_suit::Diamond, e_card_num::K));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::n5));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::J));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::K));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::n10));
        dom.sc_player1.add_card(card(e_card_suit::Spade, e_card_num::n3));

        dom.sc_player2.add_card(card(e_card_suit::Club, e_card_num::Q));
        dom.sc_player2.add_card(card(e_card_suit::Spade, e_card_num::A));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n7));
        dom.sc_player2.add_card(card(e_card_suit::Spade, e_card_num::n4));
        dom.sc_player2.add_card(card(e_card_suit::Club, e_card_num::n6));
        dom.sc_player2.add_card(card(e_card_suit::Spade, e_card_num::n9));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::K));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::n2));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::n5));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n10));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n3));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::Q));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n9));
        dom.sc_player2.add_card(card(e_card_suit::Club, e_card_num::n5));
        dom.sc_player2.add_card(card(e_card_suit::Club, e_card_num::n7));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::n10));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::J));
        dom.sc_player2.add_card(card(e_card_suit::Club, e_card_num::n2));
        dom.sc_player2.add_card(card(e_card_suit::Spade, e_card_num::n6));
        dom.sc_player2.add_card(card(e_card_suit::Spade, e_card_num::n7));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n2));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n6));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::n8));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n5));
        dom.sc_player2.add_card(card(e_card_suit::Diamond, e_card_num::n8));
        dom.sc_player2.add_card(card(e_card_suit::Heart, e_card_num::A));

        dom.sc_player1.show("player 1 cards");
        dom.sc_player2.show("player 2 cards");

        std::cout << "Карты розданы: " << std::endl;
        std::cout << "Player1 карты: " << dom.sc_player1.get_size() << std::endl;
        std::cout << "Player2 карты: " << dom.sc_player2.get_size() << std::endl;

        //сообщить "next_turn"
        invoker.invoke(event (dom.next_turn_id, "next_turn"), false);
    }
};

class state_turn_play :
    public state
{
private:
    DOM& dom;

public:
    state_turn_play(i_sm_event_invoker_t& invoker_, DOM& dom_) :
        state(invoker_),
        dom(dom_)
    {
    }

protected:
    void do_entry_action(const event& ref_e) override
    {
        if (ref_e.id == dom.card_on_table_id)
        {
            //сравнить
            //если не равны,
            //передать карты
            //сообщить "next_turn"
            //если равны,
            //сообщить "next_sub_turn"
            auto [c1, c2] = std::any_cast<std::pair<card, card>>(const_cast<event&>(ref_e).get_param());
            if (c1 < c2)
            {
                std::cout << c1.get_name() << " < " << c2.get_name() << std::endl;
                std::cout << dom.sc_table.get_size() << " карты уходят к Player2" << std::endl;
                dom.sc_table.move_all_cards_f2b(dom.sc_player2);
                std::cout << "Карт: " << dom.sc_player1.get_size() << " + " << dom.sc_player2.get_size() << " + стол: " << dom.sc_table.get_size() << std::endl;
                invoker.invoke(event (dom.next_turn_id, "next_turn"), false);
            }
            else if (c2 < c1)
            {
                std::cout << c1.get_name() << " > " << c2.get_name() << std::endl;
                std::cout << dom.sc_table.get_size () << " карты уходят к Player1" << std::endl;
                dom.sc_table.move_all_cards_f2b(dom.sc_player1);
                std::cout << "Карт: " << dom.sc_player1.get_size() << " + " << dom.sc_player2.get_size() << " + стол: " << dom.sc_table.get_size() << std::endl;
                invoker.invoke(event(dom.next_turn_id, "next_turn"), false);
            }
            else if (c1 == c2)
            {
                std::cout << c1.get_name() << " = " << c2.get_name() << std::endl;
                std::cout << "карты остаются на столе" << std::endl;
                invoker.invoke(event(dom.next_sub_turn_id, "next_sub_turn"), false);
            }
        }
    }
};

/////////////////////////////////////////////////// end drank //////////////////////////////////////////////////////