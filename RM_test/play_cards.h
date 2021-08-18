#pragma once

#include <iostream>
#include <list>
#include <string>
#include <variant>


enum class e_card_suit
{
    Club = 0,
    Spade = 1,
    Heart = 2,
    Diamond = 3
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
    A = 12
};
struct s_card_suitnum
{
    e_card_suit suit = e_card_suit::Club;
    e_card_num num = e_card_num::n2;
};
enum class e_card_mark
{
    Joker = 0
};

enum class e_card_set_type
{
    t54,
    t52,
    t36
};

struct card_name
{
    std::string operator()(const s_card_suitnum& sm)
    {
        std::string res;
        switch (sm.suit)
        {
        case e_card_suit::Club: res = "Club"; break;
        case e_card_suit::Spade: res = "Spade"; break;
        case e_card_suit::Heart: res = "Heart"; break;
        case e_card_suit::Diamond: res = "Diamond"; break;
        }
        switch (sm.num)
        {
        case e_card_num::n2: res += " 2"; break;
        case e_card_num::n3: res += " 3"; break;
        case e_card_num::n4: res += " 4"; break;
        case e_card_num::n5: res += " 5"; break;
        case e_card_num::n6: res += " 6"; break;
        case e_card_num::n7: res += " 7"; break;
        case e_card_num::n8: res += " 8"; break;
        case e_card_num::n9: res += " 9"; break;
        case e_card_num::n10: res += " 10"; break;
        case e_card_num::J: res += " J"; break;
        case e_card_num::Q: res += " Q"; break;
        case e_card_num::K: res += " K"; break;
        case e_card_num::A: res += " A"; break;
        }

        return res;
    }

    std::string operator()(const e_card_mark& m)
    {
        switch (m)
        {
        case e_card_mark::Joker: return "Joker";
        }

        return "";
    }
};

struct card
{
    using cvar = std::variant<s_card_suitnum, e_card_mark>;

    const cvar c;

    card() = delete;
    card(const card& c_) : c(c_.c) {}
    card(const e_card_mark m_) : c(m_) {}
    card(const s_card_suitnum sn_) : c(sn_) {}
    card(const e_card_suit suit_, const e_card_num number_) :
        c(s_card_suitnum(suit_, number_))
    {
    }

    std::string get_name()
    {
        return std::visit(card_name{}, c);
    }

    card& operator= (const card& ref_c)
    {
        const_cast<cvar&>(c) = ref_c.c;
        return *this;
    }

    bool operator< (const card& ref_c)
    {
        const auto ptr_sn = std::get_if<s_card_suitnum>(&ref_c.c);
        const auto ptr_sn_c = std::get_if<s_card_suitnum>(&c);
        if (ptr_sn && ptr_sn_c)
        {
            return (ptr_sn->num < ptr_sn_c->num);
        }

        return false;
    }

    bool operator== (const card& ref_c)
    {
        const auto ptr_sn = std::get_if<s_card_suitnum>(&ref_c.c);
        const auto ptr_sn_c = std::get_if<s_card_suitnum>(&c);
        if (ptr_sn && ptr_sn_c)
        {
            return (ptr_sn->num == ptr_sn_c->num);
        }

        const auto ptr_m = std::get_if<e_card_mark>(&ref_c.c);
        const auto ptr_m_c = std::get_if<e_card_mark>(&c);
        if (ptr_sn && ptr_sn_c)
        {
            return true;
        }

        return false;
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
            cards.emplace_back(card(e_card_mark::Joker));
            cards.emplace_back(card(e_card_mark::Joker));
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
            if (const auto ptr_sn = std::get_if<s_card_suitnum>(&c.c); ptr_sn)
            {
                if (ptr_sn->suit >= e_card_suit::Club && ptr_sn->suit <= e_card_suit::Diamond && ptr_sn->num >= e_card_num::n6 && ptr_sn->num <= e_card_num::A)
                    return true;
            }
            break;
        case e_card_set_type::t52:
            if (const auto ptr_sn = std::get_if<s_card_suitnum>(&c.c); ptr_sn)
            {
                if (ptr_sn->suit >= e_card_suit::Club && ptr_sn->suit <= e_card_suit::Diamond && ptr_sn->num >= e_card_num::n2 && ptr_sn->num <= e_card_num::A)
                    return true;
            }
            break;
        case e_card_set_type::t54:
            if (const auto ptr_sn = std::get_if<s_card_suitnum>(&c.c); ptr_sn)
            {
                if (ptr_sn->suit >= e_card_suit::Club && ptr_sn->suit <= e_card_suit::Diamond && ptr_sn->num >= e_card_num::n2 && ptr_sn->num <= e_card_num::A)
                    return true;
            }
            else if (const auto ptr_m = std::get_if<e_card_mark>(&c.c); ptr_m)
            {
                if (*ptr_m == e_card_mark::Joker)
                    return true;
            }
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
                to_card_set.cards.splice(to_card_set.cards.end(), cards, cards.begin());
                return true;
            }
        }

        return false;
    }

    bool move_card_f2f(card_set& to_card_set) // first to first
    {
        if (type == to_card_set.type)
        {
            if (cards.size() > 0)
            {
                to_card_set.cards.splice(to_card_set.cards.begin(), cards, cards.begin());
                return true;
            }
        }

        return false;
    }

    card get_first_card()
    {
        return cards.front();
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
        for (int n = 3; n > 0; n--)
            for (size_t sz = cards.size(), i = sz; i > 0; i--)
            {
                auto iter = cards.begin();
                for (int z = rand() % sz; z >= 0; z--)
                    iter++;

                cards.splice(iter, cards, cards.begin());
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
            std::cout << c.get_name() << std::endl;
    }
};
