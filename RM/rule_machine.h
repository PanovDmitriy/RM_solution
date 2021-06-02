
#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>





namespace rm // rule machine
{
    typedef int id_t;

    enum class status { enabled, disabled, paused };

    /// event ///

    class event
    {
    public:
        const id_t id;
        const std::string name;

    private:
        event()
            : id(-1), name("*")
        {
        }

    public:
        event(const event& e)
            : id(e.id), name(e.name)
        {
        }

        event(id_t _id, std::string _name)
            : id(_id), name(_name)
        {
        }

        bool operator == (event e)
        {
            return id == e.id;
        }
        bool operator != (event e)
        {
            return id != e.id;
        }

        virtual ~event() {}
    };

    //exaples:
    // 
    //class event_str
    //    : public event
    //{
    //private:
    //    std::string param;

    //public:
    //    event_str(std::string _param)
    //        : event (100, "test1"), param (_param)
    //    {
    //    }

    //    std::string get_param()
    //    {
    //        return param;
    //    }

    //    void set_param(std::string p)
    //    {
    //        param = p;
    //    }
    //};

    //template <typename TParam>
    //class event_test_t
    //    : public event
    //{
    //private:
    //    TParam param;

    //public:
    //    event_test_t(TParam _param)
    //        : event(-2, std::string ("template_") + std::string (typeid(TParam).name())), param(_param)
    //    {
    //    }

    //    TParam get_param()
    //    {
    //        return param;
    //    }

    //    void set_param(TParam p)
    //    {
    //        param = p;
    //    }
    //};

    //template <class CParam>
    //class event_test_c
    //    : public event
    //{
    //private:
    //    CParam param;

    //public:
    //    event_test_c(CParam _param)
    //        : event(-2, std::string("template_") + std::string(typeid(CParam).name())), param(_param)
    //    {
    //    }

    //    CParam get_param()
    //    {
    //        return param;
    //    }

    //    void set_param(CParam p)
    //    {
    //        param = p;
    //    }
    //};

    /// end event ///

    typedef void(*ptr_action_t)(event*);
    typedef bool(*ptr_guard_t)(event*);

    /// link ///

    class link
    {
    public:
        virtual void do_action(event*)
        {
        }

        virtual bool guard_condition(event*)
        {
            return true;
        }

        virtual ~link() {}
    };

    /// end link ///

    /// link_vecs ///

    class link_vecs
        : public link
    {
    protected:
        std::vector <ptr_action_t> actions;
        std::vector <ptr_guard_t> guards;

    public:
        virtual void do_action(event* ptr_e)
        {
            for (auto action : actions)
                (*action)(ptr_e);
        }

        void add_action(const ptr_action_t func)
        {
            actions.push_back(func);
        }

        void clear_actions()
        {
            actions.clear();
        }

        virtual bool guard_condition(event* ptr_e)
        {
            for (auto guard : guards)
            {
                if (!(*guard)(ptr_e))
                    return false;
            }
            return true;
        }

        void add_guard(const ptr_guard_t func)
        {
            guards.push_back(func);
        }
        void clear_guards()
        {
            guards.clear();
        }
    };

    /// end link_vecs ///

    /// link_test ///

    class link_test
        : public link_vecs
    {
    public:
        const std::string name;

    public:
        link_test(std::string _name)
            : name(_name)
        {
        }

        void do_action(event* ptr_e)
        {
            std::string event_name;
            if (ptr_e)
                event_name = ptr_e->name + ", " + std::to_string (ptr_e->id);
            else
                event_name = "NULL";

            std::cout << "link " << name << " * event [ " << event_name << " ]: action" << std::endl;
        }

        bool guard_condition(event* ptr_e)
        {
            std::string event_name;
            if (ptr_e)
                event_name = ptr_e->name + ", " + std::to_string(ptr_e->id);
            else
                event_name = "NULL";

            std::cout << "link " << name << " * event [ " << event_name << " ]: guard_condition: true" << std::endl;

            return true;
        }
    };

    /// end link_test ///

    /// state ///

    class state
    {
    public:
        const std::string name;

    public:
        state(std::string _name)
            : name(_name)
        {
        }

        virtual void do_entry_action(event*)
        {
        }
        virtual void do_exit_action(event*)
        {
        }

        virtual ~state() {}
    };

    /// end state ///

    /// state_test ///

    class state_test
        : public state
    {
    public:
        state_test(std::string _name)
            : state(_name)
        {
        }

        virtual void do_entry_action(event* ptr_e)
        {
            std::string event_name;
            if (ptr_e)
                event_name = ptr_e->name + ", " + std::to_string(ptr_e->id);
            else
                event_name = "NULL";

            std::cout << "state " << name << " * event [ " << event_name << " ]: entry action" << std::endl;
        }
        virtual void do_exit_action(event* ptr_e)
        {
            std::string event_name;
            if (ptr_e)
                event_name = ptr_e->name + ", " + std::to_string(ptr_e->id);
            else
                event_name = "NULL";

            std::cout << "state " << name << " * event [ " << event_name << " ]: exit action" << std::endl;
        }
    };

    /// end state_test ///

    /// state_vecs ///

    class state_vecs
        : public state
    {
    protected:
        std::vector <ptr_action_t> entry_actions;
        std::vector <ptr_action_t> exit_actions;

    public:
        const std::string name;

    public:
        state_vecs(std::string _name)
            :state(_name)
        {
        }

        virtual void do_entry_action(event* ptr_e)
        {
            for (auto action : entry_actions)
                (*action)(ptr_e);
        }
        virtual void do_exit_action(event* ptr_e)
        {
            for (auto action : exit_actions)
                (*action)(ptr_e);
        }

        void add_entry_action(const ptr_action_t func)
        {
            entry_actions.push_back(func);
        }
        void clear_entry_actions()
        {
            entry_actions.clear();
        }

        void add_exit_action(const ptr_action_t func)
        {
            exit_actions.push_back(func);
        }
        void clear_exit_actions()
        {
            exit_actions.clear();
        }
    };

    /// state_vecs ///


    class i_event_invoker
    {
    public:
        virtual void invoke(event* e) = 0;
    };


    /// state_machine

    class state_machine
    {
    public:
        //const id_t id;

    private:
        state_machine()
            : invoker(nullptr)
        {
        }

    public:
        state_machine(i_event_invoker* invoker_ )
            : invoker(invoker_)
        {
        }

    private:
        struct link_to_state
        {
            link* ptr_link = nullptr;
            state* ptr_state = nullptr;
        };

        std::map<state*, std::multimap<id_t /*event id*/, link_to_state>> state_transitions_tab;

        i_event_invoker* invoker;

        state* current_state_ptr = nullptr;
        state* begin_state_ptr = nullptr;

        status curr_status = status::disabled;

    protected:
        void invoke(event* e)
        {
            if (invoker)
                invoker->invoke(e);
        }

    public:
        std::string recv_triggering_event(event* e)
        {
            /// pre conditions ///

            if (curr_status != status::enabled)
                return "success: event reject. sm status: not enabled";

            if (!e)
                return "error: event is null";

            // test curr state
            if (!current_state_ptr)
                return "error: current state is null";

            // find curr state in tab
            auto it1 = state_transitions_tab.find(current_state_ptr);
            if (it1 == state_transitions_tab.end())
                return "error: current state is not faund";

            // find links from curr state to new state by event in tab
            auto& mmap_ref = it1->second;
            auto it2 = mmap_ref.equal_range(e->id);
            link_to_state* ptr_ls = nullptr;
            for (auto& itr = it2.first; itr != it2.second; ++itr)
            {
                link* pl = itr->second.ptr_link;

                if (!pl) // if link is null, then success. link can be NULL. Guard condition will not test
                {
                    ptr_ls = &itr->second;
                    break;
                }

                // link guard condition 
                if (pl)
                    if (pl->guard_condition(e) == true)
                    {
                        ptr_ls = &itr->second;
                        break;
                    }
            }
            if (!ptr_ls)
                return "success: event reject or guard condition reject";

            if (!(ptr_ls->ptr_state))
                return "error: new state is null";


            auto it3 = state_transitions_tab.find(ptr_ls->ptr_state);
            if (it3 == state_transitions_tab.end())
                return "error: new state is not faund";

            /// - ///

            /// Ok! change state now! ///

            // do exit actions
            current_state_ptr->do_exit_action(e);

            // do link actions
            if (ptr_ls->ptr_link)
                ptr_ls->ptr_link->do_action(e);

            // update current state index
            current_state_ptr = ptr_ls->ptr_state;

            // do entry actions
            current_state_ptr->do_entry_action(e);

            return "success: ok";
        }

    public:
        void add_item(id_t e_id, link* l, state* s_from, state* s_to, bool is_begin_state_from = false)
        {
            link_to_state ls;
            ls.ptr_link = l;
            ls.ptr_state = s_to;
            state_transitions_tab[s_from].insert(std::pair<id_t /*event id*/, link_to_state>(e_id, ls)); // set w/o test

            if (is_begin_state_from)
            {
                current_state_ptr = s_from;
                begin_state_ptr = current_state_ptr;
            }
        }

        void check_obj()
        {
            std::cout << "state list:" << std::endl;

            //look: std::map<state*, std::multimap<id_t /*event id*/, link_to_state>> state_transitions_tab;

            for (auto& a : state_transitions_tab)
            {
                std::cout << "\tstate " << a.first->name << std::endl;
                for (auto& [event_id, link_to_state] : a.second)
                {
                    std::cout << "\t\tevent ID: " << event_id;
                    if (!(link_to_state.ptr_link))
                        std::cout << " null link to ";
                    else
                        std::cout << " not null link to ";
                    std::cout << link_to_state.ptr_state->name << std::endl;
                }
            }
        }

        /// control methods sm ///
        status get_status()
        {
            return curr_status;
        }

        void set_status(status new_status)
        {
            switch (new_status)
            {
            case status::enabled:
                switch (curr_status)
                {
                case status::enabled:
                    break;
                case status::paused:
                    curr_status = status::enabled;
                    break;
                case status::disabled:
                    current_state_ptr = begin_state_ptr;
                    current_state_ptr->do_entry_action(nullptr);
                    curr_status = status::enabled;
                    break;
                }
                break;

            case status::disabled:
                switch (curr_status)
                {
                case status::enabled:
                    curr_status = status::disabled;
                    break;
                case status::paused:
                    curr_status = status::disabled;
                    break;
                case status::disabled:
                    break;
                }
                break;

            case status::paused:
                switch (curr_status)
                {
                case status::enabled:
                    curr_status = status::paused;
                    break;
                case status::paused:
                case status::disabled:
                    break;
                }
                break;
            }
        }
        /// - ///

        void clear()
        {
            set_status(status::disabled);
            state_transitions_tab.clear();
        }
    };


    /// rule machine

    class rule_machine :
        public i_event_invoker
    {
    private:
        status curr_status = status::disabled;

    private:
        std::vector<state_machine*> sms;

    public:
        void add_sm(state_machine* sm)
        {
            sms.push_back(sm);
        }

    public:
        /// control methods sm ///
        status get_status()
        {
            return curr_status;
        }
        void set_status(status new_status)
        {
            curr_status = new_status;

            for (state_machine* sm : sms)
            {
                if (sm)
                    sm->set_status(new_status);
            }
        }
        /// - ///

    public:
        void invoke(event* e) override
        {
            for (state_machine* sm : sms)
            {
                if (sm)
                    sm->recv_triggering_event(e);
            }
        }

        void clear()
        {
            set_status (status::disabled);

            for (state_machine* sm : sms)
            {
                if (sm)
                    sm->clear();
            }

            sms.clear();
        }
    };
}




