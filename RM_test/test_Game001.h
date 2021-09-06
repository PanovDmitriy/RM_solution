#pragma once

#include "..//RM/rule_machine.h"
#include "play_cards.h"
#include <algorithm>

using namespace rm;


class state_turn_player;
class state_pre_play;
class state_turn_play;


struct test_param
{
    int val = -1;

    test_param() :
        val(1)
    {
        std::cout << "test_param.construct() val: " << val << std::endl;
    }

    test_param(const test_param& tp) :
        val(tp.val)
    {
        std::cout << "test_param.construct(&) val: " << val << std::endl;
    }

    test_param(test_param&& tp) :
        val(tp.val)
    {
        std::cout << "test_param.construct(&&) val: " << val << std::endl;
        tp.val = 0;
    }

    ~test_param()
    {
        std::cout << "test_param.destruct() val: " << val << std::endl;
        int val = -1;
    }
};

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
protected:
    i_sm_event_riser_t& riser;

private:
    DOM& dom;

public:
    state_turn_player(i_sm_event_riser_t& riser_, DOM& dom_) :
        riser(riser_),
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
            if (!dom.sc_player1.move_card_first2first(dom.sc_table))
            {
                std::cout << "player1: нет карт!" << std::endl;
                this->riser.rise_event(event(dom.no_card_id, int(1)));
                return;
            }
            std::cout << "player1: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player1.get_size() << std::endl;
            c1 = dom.sc_table.get_first_card();

            if (!dom.sc_player2.move_card_first2first(dom.sc_table))
            {
                std::cout << "player2: нет карт!" << std::endl;
                this->riser.rise_event(event(dom.no_card_id, int(2)));
                return;
            }
            std::cout << "player2: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player2.get_size() << std::endl;
            c2 = dom.sc_table.get_first_card();
        }
        else if (ref_e.id == dom.next_sub_turn_id)
        {
            std::cout << "Двойной ход: " << dom.turn_n << std::endl;
            if (!dom.sc_player1.move_card_first2first(dom.sc_table))
            {
                std::cout << "player1: нет карт!" << std::endl;
                this->riser.rise_event(event(dom.no_card_id, int(1)));
                return;
            }
            std::cout << "player1: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player1.get_size() << std::endl;
            if (!dom.sc_player1.move_card_first2first(dom.sc_table))
            {
                std::cout << "player1: нет карт!" << std::endl;
                this->riser.rise_event(event(dom.no_card_id, int(1)));
                return;
            }
            std::cout << "player1: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player1.get_size() << std::endl;
            c1 = dom.sc_table.get_first_card();
            //
            if (!dom.sc_player2.move_card_first2first(dom.sc_table))
            {
                std::cout << "player2: нет карт!" << std::endl;
                this->riser.rise_event(event(dom.no_card_id, int(2)));
                return;
            }
            std::cout << "player2: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player2.get_size() << std::endl;
            if (!dom.sc_player2.move_card_first2first(dom.sc_table))
            {
                std::cout << "player2: нет карт!" << std::endl;
                this->riser.rise_event(event(dom.no_card_id, int(2)));
                return;
            }
            std::cout << "player2: " << dom.sc_table.get_first_card().get_name();
            std::cout << " осталось: " << dom.sc_player2.get_size() << std::endl;
            c2 = dom.sc_table.get_first_card();
        }

        dom.turn_n++;

        this->riser.rise_event(event (dom.card_on_table_id, std::pair<card, card>(c1, c2)));
    }
};


class state_pre_play :
    public state
{
protected:
    i_sm_event_riser_t& riser;

private:
    DOM& dom;

public:
    state_pre_play(i_sm_event_riser_t& riser_, DOM& dom_) :
        riser(riser_),
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

        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::Q));
        dom.sc_player1.add_card_back(card(e_card_suit::Heart, e_card_num::n4));
        dom.sc_player1.add_card_back(card(e_card_suit::Heart, e_card_num::n9));
        dom.sc_player1.add_card_back(card(e_card_suit::Heart, e_card_num::Q));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::K));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::J));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::n2));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::n8));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::n9));
        dom.sc_player1.add_card_back(card(e_card_suit::Diamond, e_card_num::n4));
        dom.sc_player1.add_card_back(card(e_card_suit::Heart, e_card_num::n6));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::n10));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::A));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::n8));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::n4));
        dom.sc_player1.add_card_back(card(e_card_suit::Diamond, e_card_num::J));
        dom.sc_player1.add_card_back(card(e_card_suit::Heart, e_card_num::n7));
        dom.sc_player1.add_card_back(card(e_card_suit::Diamond, e_card_num::A));
        dom.sc_player1.add_card_back(card(e_card_suit::Heart, e_card_num::n3));
        dom.sc_player1.add_card_back(card(e_card_suit::Club, e_card_num::n3));
        dom.sc_player1.add_card_back(card(e_card_suit::Diamond, e_card_num::K));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::n5));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::J));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::K));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::n10));
        dom.sc_player1.add_card_back(card(e_card_suit::Spade, e_card_num::n3));

        dom.sc_player2.add_card_back(card(e_card_suit::Club, e_card_num::Q));
        dom.sc_player2.add_card_back(card(e_card_suit::Spade, e_card_num::A));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n7));
        dom.sc_player2.add_card_back(card(e_card_suit::Spade, e_card_num::n4));
        dom.sc_player2.add_card_back(card(e_card_suit::Club, e_card_num::n6));
        dom.sc_player2.add_card_back(card(e_card_suit::Spade, e_card_num::n9));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::K));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::n2));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::n5));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n10));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n3));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::Q));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n9));
        dom.sc_player2.add_card_back(card(e_card_suit::Club, e_card_num::n5));
        dom.sc_player2.add_card_back(card(e_card_suit::Club, e_card_num::n7));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::n10));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::J));
        dom.sc_player2.add_card_back(card(e_card_suit::Club, e_card_num::n2));
        dom.sc_player2.add_card_back(card(e_card_suit::Spade, e_card_num::n6));
        dom.sc_player2.add_card_back(card(e_card_suit::Spade, e_card_num::n7));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n2));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n6));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::n8));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n5));
        dom.sc_player2.add_card_back(card(e_card_suit::Diamond, e_card_num::n8));
        dom.sc_player2.add_card_back(card(e_card_suit::Heart, e_card_num::A));

        dom.sc_player1.show("player 1 cards");
        dom.sc_player2.show("player 2 cards");

        std::cout << "Карты розданы: " << std::endl;
        std::cout << "Player1 карты: " << dom.sc_player1.get_size() << std::endl;
        std::cout << "Player2 карты: " << dom.sc_player2.get_size() << std::endl;

        //сообщить "next_turn"
        riser.rise_event(event(dom.next_turn_id));
    }
};


class state_turn_play :
    public state
{
protected:
    i_sm_event_riser_t& riser;

private:
    DOM& dom;

public:
    state_turn_play(i_sm_event_riser_t& riser_, DOM& dom_) :
        riser(riser_),
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
            auto [c1, c2] = std::any_cast<std::pair<card, card>>(const_cast<event&>(ref_e).param);
            if (c1 < c2)
            {
                std::cout << c1.get_name() << " < " << c2.get_name() << std::endl;
                std::cout << dom.sc_table.get_size() << " карты уходят к Player2" << std::endl;
                dom.sc_table.move_all_cards_first2back(dom.sc_player2);
                std::cout << "Карт: " << dom.sc_player1.get_size() << " + " << dom.sc_player2.get_size() << " + стол: " << dom.sc_table.get_size() << std::endl;
                riser.rise_event(event (dom.next_turn_id));
            }
            else if (c2 < c1)
            {
                std::cout << c1.get_name() << " > " << c2.get_name() << std::endl;
                std::cout << dom.sc_table.get_size () << " карты уходят к Player1" << std::endl;
                dom.sc_table.move_all_cards_first2back(dom.sc_player1);
                std::cout << "Карт: " << dom.sc_player1.get_size() << " + " << dom.sc_player2.get_size() << " + стол: " << dom.sc_table.get_size() << std::endl;
                riser.rise_event(event(dom.next_turn_id, std::move(test_param ())));
            }
            else if (c1 == c2)
            {
                std::cout << c1.get_name() << " = " << c2.get_name() << std::endl;
                std::cout << "карты остаются на столе" << std::endl;
                riser.rise_event(event(dom.next_sub_turn_id));
            }
        }
    }
};


void main_Game001()
{
    DOM dom;

    rule_machine rm;
    state_machine sm_players(&rm);
    state_machine sm_play(&rm);
    rm.add_sm(&sm_players);
    rm.add_sm(&sm_play);

    initial_state st_init_players;
    state st_await_player_starts;
    state_turn_player st_turn_players(sm_players, dom);

    sm_players.add_event_state_state_transition(dom.init_players_id, &st_init_players, &st_await_player_starts);
    sm_players.add_event_state_state_transition(dom.next_turn_id, &st_await_player_starts, &st_turn_players);
    sm_players.add_event_state_state_transition(dom.next_turn_id, &st_turn_players, &st_turn_players);
    sm_players.add_event_state_state_transition(dom.next_sub_turn_id, &st_turn_players, &st_turn_players);

    initial_state st_init_play;
    final_state st_final_play;
    state st_await_play_start;
    state_pre_play st_preplay(sm_play, dom);
    state_turn_play st_turn_play(sm_play, dom);

    sm_play.add_event_state_state_transition(dom.init_play_id, &st_init_play, &st_await_play_start);
    sm_play.add_event_state_state_transition(dom.start_play_id, &st_await_play_start, &st_preplay);
    sm_play.add_event_state_state_transition(dom.card_on_table_id, &st_preplay, &st_turn_play);
    sm_play.add_event_state_state_transition(dom.card_on_table_id, &st_turn_play, &st_turn_play);
    sm_play.add_event_state_state_transition(dom.no_card_id, &st_turn_play, &st_final_play);

    sm_players.set_status_enabled(event(dom.init_players_id, "init_players"));
    sm_play.set_status_enabled(event(dom.init_play_id, "init_play"));
    rm.set_status_enabled();
    rm.rise_event(event(dom.start_play_id));

    auto begin = std::chrono::steady_clock::now();

    while (sm_players.get_status() == status::enabled && sm_play.get_status() == status::enabled)
        rm.release_events();

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "\n24.08.2021 time: 16000\n   current time: " << elapsed_ms.count() << "\n";

    //sm_players.check_obj();
    //sm_play.check_obj();
}


