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
enum class e_card_null
{
    Null = 0
};

enum class e_card_set_type
{
    custom,
    t54,
    t52,
    t36
};

struct card_name_en // f() list for std::variant + std::visit
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

    std::string operator()(const e_card_null& v)
    {
        switch (v)
        {
        case e_card_null::Null: return "Null";
        }

        return "";
    }
};

struct card
{
    using card_variant_t = std::variant<s_card_suitnum, e_card_mark, e_card_null>;

    const card_variant_t c = e_card_null::Null;

    card()
    {
    }
    card(const card& c_) :
        c(c_.c) 
    {
    }
    card(card&& c_) noexcept : 
        c(c_.c) 
    {
        const_cast<card_variant_t&>(c_.c) = e_card_null::Null;
    }
    card(const e_card_null n_) :
        c(n_)
    {
    }
    card(const e_card_mark m_) :
        c(m_)
    {
    }
    card(const s_card_suitnum sn_) :
        c(sn_) 
    {
    }
    card(const e_card_suit suit_, const e_card_num number_) :
        c(s_card_suitnum(suit_, number_))
    {
    }

    std::string get_name()
    {
        return std::visit(card_name_en{}, c);
    }

    card& operator= (const card& ref_c)
    {
        const_cast<card_variant_t&>(c) = ref_c.c;
        return *this;
    }
    card& operator= (card&& rvl_c) noexcept
    {
        const_cast<card_variant_t&>(c) = std::move(const_cast<card_variant_t&>(rvl_c.c));
        return *this;
    }

    bool operator< (const card& ref_c)
    {
        const auto ptr_sn = std::get_if<s_card_suitnum>(&ref_c.c);
        const auto ptr_sn_c = std::get_if<s_card_suitnum>(&c);
        if (ptr_sn && ptr_sn_c)
        {
            return (ptr_sn_c->num < ptr_sn->num);
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
    const e_card_set_type type = e_card_set_type::custom;

protected:
    std::list<card> cards;

public:
    card_set() :
        type(e_card_set_type::custom)
    {
    }
    card_set(const e_card_set_type type_) :
        type(type_)
    {
    }
    card_set(const card_set& cs) : 
        type (cs.type),
        cards (cs.cards)
    {
    }
    card_set(card_set&& cs) noexcept :
        type(cs.type)
    {
        cards.swap(cs.cards);
        cs.cards.clear();
    }
    card_set(const std::initializer_list<card>& list) :
        type (e_card_set_type::custom)
    {
        for (auto& c : list)
            cards.push_back(c);
    }


    card_set& operator= (const card_set& ref_cs)
    {
        const_cast<e_card_set_type&>(type) = ref_cs.type;
        cards = ref_cs.cards;
        return *this;
    }
    card_set& operator= (card_set&& rvl_cs) noexcept
    {
        const_cast<e_card_set_type&>(type) = rvl_cs.type;
        cards.swap(rvl_cs.cards);
        rvl_cs.cards.clear ();
        return *this;
    }


    void swap(card_set& cs)
    {
        cards.swap(cs.cards);
        e_card_set_type t = cs.type;
        const_cast<e_card_set_type&>(cs.type) = type;
        const_cast<e_card_set_type&>(type) = t;
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
        case e_card_set_type::custom:
            return;
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
        case e_card_set_type::custom:
            return true;
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

    void push_last(card&& c)
    {
        cards.emplace_back(std::move(c));
    }
    void push_last(card& c)
    {
        cards.push_back(c);
    }

    void push_first(card&& c)
    {
        cards.emplace_front(std::move(c));
    }
    void push_first(card& c)
    {
        cards.push_front(c);
    }

    bool move_card_first2back(card_set& to_card_set) // first to back
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

    bool move_all_cards_first2back(card_set& to_card_set) // first to back // переделать в инт - кол-во переданных карт
    {
        if (type == to_card_set.type)
        {
            while (cards.size() > 0)
            {
                to_card_set.cards.splice(to_card_set.cards.end(), cards, cards.begin());
            }
            return true;
        }

        return false;
    }

    bool move_card_first2first(card_set& to_card_set) // first to first
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

    card pop_first() // first
    {
        card c = cards.front();
        cards.pop_front();
        return c;
    }

    card pop_last() // back
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

    int get_size()
    {
        return int (cards.size());
    }

    void show(std::string name_)
    {
        std::cout << "card set: " << name_ << std::endl;
        show();
    }

    void show()
    {
        for (card& c : cards)
            std::cout << c.get_name() << std::endl;
    }
};
