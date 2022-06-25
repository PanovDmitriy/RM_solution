#pragma once

#include "..//RM/rule_machine.h"
#include "play_cards.h"
#include <algorithm>

constexpr bool is_test_cardset = true;

using namespace Machines;


class state_turn_player;
class state_pre_play;
class state_turn_play;
class DOM;


struct TestParam
{
    int val = -1;
    const int time = rand();

    TestParam() :
        val(1)
    {
        std::cout << time << "::test_param.construct() val: " << val << std::endl;
    }

    TestParam(const TestParam& tp) :
        val(tp.val)
    {
        std::cout << time << "::test_param.construct(& " << tp.time << ") val: " << val << std::endl;
    }

    TestParam( TestParam&& tp) noexcept :
        val(tp.val)
    {
        std::cout << time << "::test_param.construct(&& " << tp.time << ") val: " << val << std::endl;
        tp.val = 0;
    }

    ~TestParam()
    {
        std::cout << time << "::test_param.destruct() val: " << val << std::endl;
        int val = -1;
    }
};


class StateTurnPlayer :
    public State
{
public:
    DOM* ptr_dom = nullptr;

public:
    StateTurnPlayer() = default;

    void setDom(DOM* dom_)
    {
        ptr_dom = dom_;
    }

protected:
    void doEntryAction_( EventCPtr event ) override;

};


class StatePrePlay :
    public State
{
public:
    DOM* ptr_dom = nullptr;

public:
    void setDom(DOM* dom_)
    {
        ptr_dom = dom_;
    }

protected:
    void doEntryAction_( EventCPtr event ) override;

};


class StateTurnPlay :
    public State
{
private:
    DOM* ptr_dom = nullptr;

public:
    void setDom(DOM* dom_)
    {
        ptr_dom = dom_;
    }

protected:
    void doEntryAction_( EventCPtr event ) override;

};

class TransitionGameOver
    : public Transition
{
private:
    Id noCardID_;

protected:
    void getWinner( EventCPtr event )
    {
        if ( event->id == noCardID_ && noCardID_ >= 0 )
        {
            int gamerID = -1;
            try
            {
                gamerID = std::any_cast<int>( event->param );
            }
            catch ( const std::bad_any_cast& )
            {
            }

            std::cout << "Победитель: player" << (( gamerID == -1) ? "?" : std::to_string( gamerID )) << std::endl;
        }
    }

public:
    void setNoCardID( Id noCardID )
    {
        noCardID_ = noCardID;
    }

    void doAction( EventCPtr event ) override
    {
        getWinner( event );
    }

};

class DOM
{
public:
    card_set sc_table;
    card_set sc_player1;
    card_set sc_player2;

    int turn_n = 1;
    const int max_turn = std::numeric_limits<int>::max() - 1;

public:
    RuleMachine rm;

    StateMachine sm_players;
    StateMachine sm_play;

    InitialState st_init_players;
    State st_await_player_starts;
    StateTurnPlayer st_turn_players;

    InitialState st_init_play;
    FinalState st_final_play;
    State st_await_play_start;
    StatePrePlay st_preplay;
    StateTurnPlay st_turn_play;

    TransitionGameOver tr_game_over;

public:
    DOM() :
        sc_table(e_card_set_type::t52),
        sc_player1(e_card_set_type::t52),
        sc_player2(e_card_set_type::t52)
    {
        reset();
    }

    bool bit()
    {
        if (turn_n == max_turn)
        {
            std::cout << "BREAK! MAX TURN = " << max_turn << std::endl;
            return false;
        }

        if (sm_players.GetStatus() == Status::Enabled && sm_play.GetStatus() == Status::Enabled)
        {
            rm.ReleaseEvent();
            return true;
        }

        return false;
    }

    void init();

    inline EventHandlerPtr GetEventHandler()
    {
        return rm.GetEventHandler();
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
    const Id init_players_id = 10;
    const Id init_play_id = 20;
    const Id next_turn_id = 30;
    const Id next_sub_turn_id = 40;
    const Id start_play_id = 50;
    const Id card_on_table_id = 60;
    const Id no_card_id = 70;
};

void DOM::init()
{
    st_turn_players.setDom(this);
    st_preplay.setDom(this);
    st_turn_play.setDom(this);

    tr_game_over.setNoCardID(no_card_id);

    rm.addMachine(std::make_shared<IMachineController>(sm_players));
    rm.addMachine( std::make_shared<IMachineController>( sm_play));

    sm_players.add_link(init_players_id, &st_init_players, &st_await_player_starts);
    sm_players.add_link(next_turn_id, &st_await_player_starts, &st_turn_players);
    sm_players.add_link(next_turn_id, &st_turn_players, &st_turn_players);
    sm_players.add_link(next_sub_turn_id, &st_turn_players, &st_turn_players);

    sm_play.add_link(init_play_id, &st_init_play, &st_await_play_start);
    sm_play.add_link(start_play_id, &st_await_play_start, &st_preplay);
    sm_play.add_link(card_on_table_id, &st_preplay, &st_turn_play);
    sm_play.add_link(card_on_table_id, &st_turn_play, &st_turn_play);
    sm_play.add_link(no_card_id, &st_turn_play, &st_final_play, &tr_game_over);

    auto [sm_players_cib, sm_players_cis] = sm_players.check_integrity();
    auto [sm_play_cib, sm_play_cis] = sm_play.check_integrity();
    std::cout << "sm_players check integrity: " << sm_players_cis << std::endl;
    std::cout << "sm_play check integrity: " << sm_play_cis << std::endl;

    sm_players.set_status_enabled(Event(init_players_id, "init_players"));
    sm_play.set_status_enabled(Event(init_play_id, "init_play"));
    rm.set_status_enabled({ Id::Undef });
    Event e_start(start_play_id);
    rm.rise_event(e_start);
}

void state_turn_player::do_entry_action(const Event& ref_e)
{
    card c1(e_card_mark::Joker);
    card c2(c1);

    //карты от игрока на стол (1 для next_turn, 2 для next_sub_turn),
    //если нет, сообщить "no_cards",
    //если есть,
    //положить на стол,
    //сообщить "card_on_table"

    if (ref_e.id == ptr_dom->next_turn_id)
    {
        std::cout << "Ход: " << ptr_dom->turn_n << std::endl;
        if (!ptr_dom->sc_player1.move_card_first2first(ptr_dom->sc_table))
        {
            std::cout << "player1: нет карт!" << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->no_card_id, int(1)));
            return;
        }
        std::cout << "player1: " << ptr_dom->sc_table.get_first_card().get_name();
        std::cout << " осталось: " << ptr_dom->sc_player1.get_size() << std::endl;
        c1 = ptr_dom->sc_table.get_first_card();

        if (!ptr_dom->sc_player2.move_card_first2first(ptr_dom->sc_table))
        {
            std::cout << "player2: нет карт!" << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->no_card_id, int(2)));
            return;
        }
        std::cout << "player2: " << ptr_dom->sc_table.get_first_card().get_name();
        std::cout << " осталось: " << ptr_dom->sc_player2.get_size() << std::endl;
        c2 = ptr_dom->sc_table.get_first_card();
    }
    else if (ref_e.id == ptr_dom->next_sub_turn_id)
    {
        std::cout << "Двойной ход: " << ptr_dom->turn_n << std::endl;
        if (!ptr_dom->sc_player1.move_card_first2first(ptr_dom->sc_table))
        {
            std::cout << "player1: нет карт!" << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->no_card_id, int(1)));
            return;
        }
        std::cout << "player1: " << ptr_dom->sc_table.get_first_card().get_name();
        std::cout << " осталось: " << ptr_dom->sc_player1.get_size() << std::endl;
        if (!ptr_dom->sc_player1.move_card_first2first(ptr_dom->sc_table))
        {
            std::cout << "player1: нет карт!" << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->no_card_id, int(1)));
            return;
        }
        std::cout << "player1: " << ptr_dom->sc_table.get_first_card().get_name();
        std::cout << " осталось: " << ptr_dom->sc_player1.get_size() << std::endl;
        c1 = ptr_dom->sc_table.get_first_card();
        //
        if (!ptr_dom->sc_player2.move_card_first2first(ptr_dom->sc_table))
        {
            std::cout << "player2: нет карт!" << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->no_card_id, int(2)));
            return;
        }
        std::cout << "player2: " << ptr_dom->sc_table.get_first_card().get_name();
        std::cout << " осталось: " << ptr_dom->sc_player2.get_size() << std::endl;
        if (!ptr_dom->sc_player2.move_card_first2first(ptr_dom->sc_table))
        {
            std::cout << "player2: нет карт!" << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->no_card_id, int(2)));
            return;
        }
        std::cout << "player2: " << ptr_dom->sc_table.get_first_card().get_name();
        std::cout << " осталось: " << ptr_dom->sc_player2.get_size() << std::endl;
        c2 = ptr_dom->sc_table.get_first_card();
    }

    ptr_dom->turn_n++;

    ptr_dom->get_event_handler()->rise_event(Event(ptr_dom->card_on_table_id, std::pair<card, card>(c1, c2)));
}

void state_pre_play::do_entry_action(const Event& ref_e)
{
    //  раздать карты
    if constexpr (!is_test_cardset)
    {
        ptr_dom->reset();
        ptr_dom->sc_table.shuffle();
        ptr_dom->sc_table.shuffle();
        ptr_dom->sc_table.shuffle();
        ptr_dom->sc_table.shuffle();
        while (true)
        {
            if (!ptr_dom->sc_table.move_card_first2first(ptr_dom->sc_player1))
                break;
            if (!ptr_dom->sc_table.move_card_first2first(ptr_dom->sc_player2))
                break;
        }
    }
    else
    {
        ptr_dom->sc_table.clear();
        ptr_dom->sc_player1.clear();
        ptr_dom->sc_player2.clear();

        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::Q));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Heart, e_card_num::n4));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Heart, e_card_num::n9));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Heart, e_card_num::Q));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::K));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::J));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::n2));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::n8));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::n9));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Diamond, e_card_num::n4));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Heart, e_card_num::n6));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::n10));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::A));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::n8));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::n4));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Diamond, e_card_num::J));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Heart, e_card_num::n7));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Diamond, e_card_num::A));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Heart, e_card_num::n3));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Club, e_card_num::n3));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Diamond, e_card_num::K));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::n5));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::J));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::K));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::n10));
        ptr_dom->sc_player1.push_last(card(e_card_suit::Spade, e_card_num::n3));

        ptr_dom->sc_player2.push_last(card(e_card_suit::Club, e_card_num::Q));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Spade, e_card_num::A));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n7));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Spade, e_card_num::n4));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Club, e_card_num::n6));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Spade, e_card_num::n9));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::K));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::n2));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::n5));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n10));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n3));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::Q));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n9));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Club, e_card_num::n5));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Club, e_card_num::n7));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::n10));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::J));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Club, e_card_num::n2));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Spade, e_card_num::n6));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Spade, e_card_num::n7));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n2));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n6));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::n8));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n5));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Diamond, e_card_num::n8));
        ptr_dom->sc_player2.push_last(card(e_card_suit::Heart, e_card_num::A));
    }

    ptr_dom->sc_player1.show("player 1 cards");
    ptr_dom->sc_player2.show("player 2 cards");

    std::cout << "Карты розданы: " << std::endl;
    std::cout << "Player1 карты: " << ptr_dom->sc_player1.get_size() << std::endl;
    std::cout << "Player2 карты: " << ptr_dom->sc_player2.get_size() << std::endl;

    //сообщить "next_turn"
    ptr_dom->get_event_handler()->rise_event(ptr_dom->next_turn_id);
}

void state_turn_play::do_entry_action(const Event& ref_e)
{
    if (ref_e.id == ptr_dom->card_on_table_id)
    {
        //сравнить
        //если не равны,
        //передать карты
        //сообщить "next_turn"
        //если равны,
        //сообщить "next_sub_turn"
        auto [c1, c2] = std::any_cast<std::pair<card, card>>(const_cast<Event&>(ref_e).param);
        if (c1 < c2)
        {
            std::cout << c1.get_name() << " < " << c2.get_name() << std::endl;
            std::cout << ptr_dom->sc_table.get_size() << " карты уходят к Player2" << std::endl;
            ptr_dom->sc_table.move_all_cards_first2back(ptr_dom->sc_player2);
            std::cout << "Карт: " << ptr_dom->sc_player1.get_size() << " + " << ptr_dom->sc_player2.get_size() << " + стол: " << ptr_dom->sc_table.get_size() << std::endl;
            ptr_dom->get_event_handler()->rise_event(Event{ ptr_dom->next_turn_id });
        }
        else if (c2 < c1)
        {
            std::cout << c1.get_name() << " > " << c2.get_name() << std::endl;
            std::cout << ptr_dom->sc_table.get_size() << " карты уходят к Player1" << std::endl;
            ptr_dom->sc_table.move_all_cards_first2back(ptr_dom->sc_player1);
            std::cout << "Карт: " << ptr_dom->sc_player1.get_size() << " + " << ptr_dom->sc_player2.get_size() << " + стол: " << ptr_dom->sc_table.get_size() << std::endl;
            //riser.rise_event(event(dom->next_turn_id, std::move(test_param {})));
            ptr_dom->get_event_handler()->rise_event(Event{ ptr_dom->next_turn_id });
        }
        else if (c1 == c2)
        {
            std::cout << c1.get_name() << " = " << c2.get_name() << std::endl;
            std::cout << "карты остаются на столе" << std::endl;
            ptr_dom->get_event_handler()->rise_event(ptr_dom->next_sub_turn_id);
        }
    }
}

void main_Game001()
{
    DOM ptr_dom;
    ptr_dom.init();

    int i = 0;

    auto begin = std::chrono::steady_clock::now();

    while (ptr_dom.bit())
    {
        i++;
    };

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << "\n24.08.2021 time: 16000\n   current time: " << elapsed_ms.count() << " turns: " << i << "\n";

}


