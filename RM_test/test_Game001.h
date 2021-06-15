#pragma once

//#include "rule_machine.h"
#include "..//RM/rule_machine.h"


using namespace rm;

/////////////////////////////////////////////////// drank //////////////////////////////////////////////////////

enum class e_card_suit
{
    Club = 0,
    Spade = 1,
    Heart = 2,
    Diamond = 3,
    Joker = 4
};
enum class e_card_num
{
    n2 = 0,
    n3 = 1,
    n4 = 2,
    n5 = 3,
    n6 = 4,
    n7 = 5,
    n8 = 6,
    n9 = 7,
    n10 = 8,
    J = 9,
    Q = 10,
    K = 11,
    A = 12,
    Joker = 13
};
enum class e_card_set_type
{
    t54,
    t52,
    t36
};

struct card
{
    const e_card_suit suit;
    const e_card_num number;

    card(const card& c) :
        suit(c.suit),
        number(c.number)
    {
    }

    card(const e_card_suit suit_, const e_card_num number_) :
        suit(suit_),
        number(number_)
    {
        if (suit == e_card_suit::Joker && number != e_card_num::Joker)
            const_cast<e_card_num&>(number) = e_card_num::Joker;

        if (number == e_card_num::Joker && suit != e_card_suit::Joker)
            const_cast<e_card_suit&>(suit) = e_card_suit::Joker;
    }

    std::string get_suit_name()
    {
        switch (suit)
        {
        case e_card_suit::Club: return "Club";
        case e_card_suit::Spade: return "Spade";
        case e_card_suit::Heart: return "Heart";
        case e_card_suit::Diamond: return "Diamond";
        case e_card_suit::Joker: return "Joker";
        }
        return "";
    }

    std::string get_number_name()
    {
        switch (number)
        {
        case e_card_num::n2: return "2";
        case e_card_num::n3: return "3";
        case e_card_num::n4: return "4";
        case e_card_num::n5: return "5";
        case e_card_num::n6: return "6";
        case e_card_num::n7: return "7";
        case e_card_num::n8: return "8";
        case e_card_num::n9: return "9";
        case e_card_num::n10: return "10";
        case e_card_num::J: return "J";
        case e_card_num::Q: return "Q";
        case e_card_num::K: return "K";
        case e_card_num::A: return "A";
        case e_card_num::Joker: return "Joker";
        }
        return "";
    }

    inline std::tuple<std::string, std::string> get_name()
    {
        return { get_number_name(), get_suit_name() };
    }

};

class card_set
{
public:
    const e_card_set_type type;

protected:
    std::list<card> cards;

public:
    card_set(const e_card_set_type type_) :
        type(type_)
    {
    }

    void reset()
    {
        cards.clear();

        int s_begin = 0;
        int s_end = -1;
        int n_begin = 0;
        int n_end = -1;
        switch (type)
        {
        case e_card_set_type::t36:
            s_begin = static_cast<int>(e_card_suit::Club);
            s_end = static_cast<int>(e_card_suit::Diamond);
            n_begin = static_cast<int>(e_card_num::n6);
            n_end = static_cast<int>(e_card_num::A);
            break;
        case e_card_set_type::t54:
            cards.emplace_back(card(e_card_suit::Joker, e_card_num::Joker));
            cards.emplace_back(card(e_card_suit::Joker, e_card_num::Joker));
            [[fallthrough]];
        case e_card_set_type::t52:
            s_begin = static_cast<int>(e_card_suit::Club);
            s_end = static_cast<int>(e_card_suit::Diamond);
            n_begin = static_cast<int>(e_card_num::n2);
            n_end = static_cast<int>(e_card_num::A);
            break;
        }

        for (int s = s_begin; s <= s_end; s++)
            for (int n = n_begin; n <= n_end; n++)
                cards.emplace_back(card(static_cast<e_card_suit>(s), static_cast<e_card_num>(n)));
    }

    bool is_valid(const card& c) // допустима ли карта для текущего типа колоды?
    {
        switch (type)
        {
        case e_card_set_type::t36:
            if (c.suit >= e_card_suit::Club && c.suit <= e_card_suit::Diamond && c.number >= e_card_num::n6 && c.number <= e_card_num::A)
                return true;
            break;
        case e_card_set_type::t52:
            if (c.suit >= e_card_suit::Club && c.suit <= e_card_suit::Diamond && c.number >= e_card_num::n2 && c.number <= e_card_num::A)
                return true;
            break;
        case e_card_set_type::t54:
            if (c.suit >= e_card_suit::Club && c.suit <= e_card_suit::Joker && c.number >= e_card_num::n2 && c.number <= e_card_num::Joker)
                return true;
            break;
        }

        return false;
    }

    bool move_card_f2b(card_set& to_card_set) // first to back
    {
        if (type == to_card_set.type)
        {
            if (cards.size() > 0)
            {
                to_card_set.cards.push_back(cards.front());
                cards.pop_front();
                return true;
            }
        }

        return false;
    }

    card move_card_f() // first
    {
        card c = cards.front();
        cards.pop_front();
        return c;
    }

    card move_card_b() // back
    {
        card c = cards.back();
        cards.pop_back();
        return c;
    }

    void shuffle()
    {
        for (size_t sz = cards.size(), i = sz; i > 0; i--)
        {
            card c1 = move_card_f();
            auto iter = cards.begin();
            for (int z = rand() % (sz - 2); z > 0; z--)
                iter++;
            cards.insert(iter, c1);

            card c2 = move_card_b();
            iter = cards.begin();
            for (int z = rand() % (sz - 2); z > 0; z--)
                iter++;
            cards.insert(iter, c2);
        }
    }

    void clear()
    {
        cards.clear();
    }

    void show(std::string name)
    {
        std::cout << "card set: " << name << std::endl;
        show();
    }

    void show()
    {
        for (card& c : cards)
            std::cout << c.get_suit_name() << "" << c.get_number_name() << std::endl;
    }
};

class DOM
{
public:
    card_set sc_main;

    // player1
    card_set sc_player1;
    bool is_player1_turned = false;

    // player2
    card_set sc_player2;
    bool is_player2_turned = false;

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
        return is_player1_turned && is_player2_turned;
    }
};

/////////////////////

class sm_player :
    public state_machine
{
protected:
    int player_index;

public:
    sm_player(int player_index_, i_event_invoker* invoker_ = NULL) :
        player_index(player_index_),
        state_machine(invoker_)
    {
    }

public:
    friend class st_turn;
};

class st_turn :
    public state
{
protected:
    sm_player& owner_ref;

public:
    st_turn(sm_player& owner_) :
        owner_ref(owner_), state("state st_turn")
    {
    }

    virtual void do_entry_action(const event& ref_e) override
    {
        std::cout << "st_turn : do_entry_action (" << ref_e.name << ")" << std::endl;

        //вход /
        //    взять карту(1 для next_turn, 2 для next_sub_turn),
        //    если нет, сообщить "no_cards",
        //    если есть,
        //    положить на стол,
        //    сообщить "card_on_table"
        //if (false)
        //{
        //    if (dom->sc_main.move_card_f2b(dom->sc_player1))
        //    {

        //    }
        //    else
        //    {

        //    }
        //}
        //else
        //    std::cout << "DOM is not set" << std::endl;
    }
};

/////////////////////

class sm_play :
    public state_machine
{
public:
    sm_play(i_event_invoker* invoker_ = NULL) :
        state_machine(invoker_)
    {
    }
};

class st_pre_game :
    public state
{
protected:
    sm_play& owner_ref;

public:
    st_pre_game(sm_play& owner_) :
        owner_ref(owner_), state("state st_pre_game")
    {
    }

    virtual void do_entry_action(const event& ref_e) override
    {
        std::cout << "st_pre_game : do_entry_action (" << ref_e.name << ")" << std::endl;

        //вход /
        //    раздать карты
        //    сообщить "next_turn"
    }
};

class st_game :
    public state
{
protected:
    sm_play& owner_ref;

public:
    st_game(sm_play& owner_) :
        owner_ref(owner_), state("state st_game")
    {
    }

    virtual void do_entry_action(const event& ref_e) override
    {
        std::cout << "st_game : do_entry_action (" << ref_e.name << ")" << std::endl;

        //вход /
        //    сравнить
        //    если не равны,
        //    передать карты
        //    сообщить "next_turn"
        //    если равны,
        //    сообщить "next_sub_turn"
    }
};

class ln_end :
    public transition
{
protected:
    sm_play& owner_ref;

public:
    ln_end(sm_play& owner_) :
        owner_ref(owner_)
    {
    }

    virtual void do_action(event& ref_e) override
    {
        std::cout << "ln_end : do_action (" << ref_e.name << ")" << std::endl;

        // get winner
        std::cout << "------------------------------------------------------\n";
        std::cout << "Winner: " << std::endl;
        std::cout << "------------------------------------------------------\n";
    }
};

class ln_first_turn :
    public transition
{
protected:
    sm_play& owner_ref;

public:
    ln_first_turn(sm_play& owner_) :
        owner_ref(owner_)
    {
    }

    virtual bool is_guard_condition(event& ref_e) override
    {
        std::cout << "ln_first_turn : guard_condition (" << ref_e.name << ")" << std::endl;

        // all turned ?

        return true;
    }
};

class ln_com_turn :
    public transition
{
protected:
    sm_play& owner_ref;

public:
    ln_com_turn(sm_play& owner_) :
        owner_ref(owner_)
    {
    }

    virtual bool is_guard_condition(event& ref_e) override
    {
        std::cout << "ln_com_turn : guard_condition (" << ref_e.name << ")" << std::endl;

        // all turned ?

        return true;
    }
};


/////////////////////////////////////////////////// end drank //////////////////////////////////////////////////////