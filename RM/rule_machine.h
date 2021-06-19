
#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <ctime>





namespace rm // rule machine
{
    typedef int id_t;

    enum class status { enabled, disabled, paused };

    /// event ///

    class event // abstruct
    {
    public:
        const id_t id;
        const std::string name;
        const std::time_t time = std::time(nullptr);

    private:
        event()
            : id(-1), name("")
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

        bool operator == (const event& ref_e)
        {
            return id == ref_e.id;
        }
        bool operator != (const event& ref_e)
        {
            return !(id == ref_e.id);
        }

        virtual ~event() = 0;
    };

    class global_event :
        public event
    {
    public:
        global_event(const global_event& e)
            : event(e.id, e.name)
        {
        }

        global_event(id_t _id, std::string _name)
            : event (_id, _name)
        {
        }

        ~global_event() {};
    };

    class local_event :
        public event
    {
    public:
        local_event(const global_event& e)
            : event(e.id, e.name)
        {
        }

        local_event(id_t _id, std::string _name)
            : event(_id, _name)
        {
        }

        ~local_event() {};
    };

    class local_change_event :
        public local_event
    {
    };

    class local_time_event :
        public local_event
    {
    };
    
    /// end event ///

    typedef void(*ptr_action_t)(global_event&);
    typedef bool(*ptr_guard_t)(global_event&);

    /// transition ///

    class transition
    {
    public:
        virtual void do_action(global_event&)
        {
        }

        virtual bool is_guard_condition(global_event&)
        {
            return true;
        }

        virtual ~transition() {}
    };

    /// end link ///

    /// link_vecs ///

    class transition_vecs
        : public transition
    {
    protected:
        std::vector <ptr_action_t> actions;
        std::vector <ptr_guard_t> guards;

    public:
        virtual void do_action(global_event& ref_e)
        {
            for (auto action : actions)
                (*action)(ref_e);
        }

        void add_action(const ptr_action_t func)
        {
            actions.push_back(func);
        }

        void clear_actions()
        {
            actions.clear();
        }

        virtual bool is_guard_condition(global_event& ref_e)
        {
            for (auto guard : guards)
            {
                if (!(*guard)(ref_e))
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

    /// end transition_vecs ///

    /// transition_test ///

    class transition_test
        : public transition_vecs
    {
    public:
        const std::string name;

    public:
        transition_test(std::string _name)
            : name(_name)
        {
        }

        void do_action(global_event& ref_e)
        {
            std::cout << "link " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: action" << std::endl;
        }

        bool is_guard_condition(global_event& ref_e)
        {
            std::cout << "link " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: guard_condition: true" << std::endl;

            return true;
        }
    };

    /// end transition_test ///

    /// state ///

    class pseudostate
    {
    public:
        virtual void do_activate(const global_event&)
        {
        }
        virtual void do_unactivate(const global_event&)
        {
        }

        virtual ~pseudostate()
        {
        }
    };

    class initial_state :
        public pseudostate
    {
    public:
        const bool is_default = false;

    public:
        initial_state(bool is_default_ = false) :
            is_default(is_default_)
        {
        }

        void do_activate(const global_event& ref_e) override
        {
        }
        void do_unactivate(const global_event& ref_e) override
        {
        }
    };

    class initial_state_test :
        public initial_state
    {
    public:
        initial_state_test(bool is_default_ = false) :
            initial_state(is_default_)
        {
        }

        void do_activate(const global_event& ref_e) override
        {
            std::cout << "pseudostate initial_state " << (is_default ? "(default): " : "(not default): ") << "activate" << std::endl;
        }
        void do_unactivate(const global_event& ref_e) override
        {
            std::cout << "pseudostate initial_state " << (is_default ? "(default): " : "(not default): ") << "unactivate" << std::endl;
        }
    };

    class final_state :
        public pseudostate
    {
    public:
        void do_activate(const global_event& ref_e) override
        {
        }
        void do_unactivate(const global_event& ref_e) override
        {
        }
    };

    class final_state_test :
        public final_state
    {
    public:
        void do_activate(const global_event& ref_e) override
        {
            std::cout << "pseudostate final_state: activate" << std::endl;
        }
        void do_unactivate(const global_event& ref_e) override
        {
            std::cout << "pseudostate final_state: unactivate" << std::endl;
        }
    };

    class state :
        public pseudostate
    {
    public:
        state()
        {
        }

    protected:
        virtual void do_entry_action(const global_event&)
        {
        }
        virtual void do_exit_action(const global_event&)
        {
        }
        virtual void do_internal_action(const global_event&)
        {
        }

    public:
        void do_activate(const global_event& ref_e) override
        {
            do_entry_action(ref_e);
            do_internal_action(ref_e);
        }
        void do_unactivate(const global_event& ref_e) override
        {
            // todo: break internal action
            do_exit_action(ref_e);
        }

        virtual ~state() 
        {
        }
    };

    /// end state ///

    /// state_vecs ///

    class state_vecs
        : public state
    {
    protected:
        std::vector <ptr_action_t> entry_actions;
        std::vector <ptr_action_t> exit_actions;

    public:
        state_vecs()
        {
        }

    protected:
        virtual void do_entry_action(global_event& ref_e)
        {
            for (auto action : entry_actions)
                (*action)(ref_e);
        }
        virtual void do_exit_action(global_event& ref_e)
        {
            for (auto action : exit_actions)
                (*action)(ref_e);
        }

    public:
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

    /// state_test ///

    class state_test
        : public state_vecs
    {
    public:
        const std::string name;

    public:
        state_test(std::string _name)
        {
        }

    protected:
        void do_entry_action(const global_event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;
        }
        void do_exit_action(const global_event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: exit action" << std::endl;
        }
        void do_internal_action(const global_event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: internal action" << std::endl;
        }
    };

    /// end state_test ///


    class i_event_invoker
    {
    public:
        virtual void invoke(global_event& ref_e) = 0;
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
            transition* ptr_link = nullptr;
            state* ptr_state = nullptr;
        };

        std::map<state*, std::multimap<id_t /*event id*/, link_to_state>> state_transitions_tab;

        i_event_invoker* invoker;

        state* current_state_ptr = nullptr;
        state* begin_state_ptr = nullptr;

        status curr_status = status::disabled;

    protected:
        void invoke(global_event& ref_e)
        {
            if (invoker)
                invoker->invoke(ref_e);
        }

    public:
        std::tuple <bool, std::string> recv_triggering_event(global_event& ref_e)
        {
            // return: success - true, error - false

            /// pre conditions ///

            if (curr_status != status::enabled)
                return { true, "Event reject. sm status: not enabled" };

            // test curr state
            if (!current_state_ptr)
                return { false, "Current state is null" };

            // find curr state in tab
            auto it1 = state_transitions_tab.find(current_state_ptr);
            if (it1 == state_transitions_tab.end())
                return { false, "Current state is not faund" };

            // find links from curr state to new state by event in tab
            auto& mmap_ref = it1->second;
            auto it2 = mmap_ref.equal_range(ref_e.id);
            link_to_state* ptr_ls = nullptr;
            for (auto& itr = it2.first; itr != it2.second; ++itr)
            {
                transition* pl = itr->second.ptr_link;

                if (!pl) // if link is null, then success. link can be NULL. Guard condition will not test
                {
                    ptr_ls = &itr->second;
                    break;
                }

                // check link guard condition 
                if (pl)
                    if (pl->is_guard_condition(ref_e))
                    {
                        ptr_ls = &itr->second;
                        break;
                    }
            }
            if (!ptr_ls)
                return { true, "State reject event or Link guard condition reject" };

            if (!(ptr_ls->ptr_state))
                return { false, "Target state is null" };


            auto it3 = state_transitions_tab.find(ptr_ls->ptr_state);
            if (it3 == state_transitions_tab.end())
                return { false, "Target state is not faund" };

            /// - ///

            /// Ok! change state now! ///

            // do unactivate prev (current) state
            current_state_ptr->do_unactivate(ref_e);

            // do link actions
            if (ptr_ls->ptr_link)
                ptr_ls->ptr_link->do_action(ref_e);

            // update current state index, change current state from prev to next
            current_state_ptr = ptr_ls->ptr_state;

            // do activate next (current) state
            current_state_ptr->do_activate(ref_e);

            return { true, "Ok" };
        }

    public:
        bool add_essl(id_t e_id, state* s_from, state* s_to, transition* l = nullptr)
        {
            if (!s_from || !s_to)
                return false;

            link_to_state ls;
            ls.ptr_link = l;
            ls.ptr_state = s_to;
            state_transitions_tab[s_from].insert(std::pair<id_t /*event id*/, link_to_state>(e_id, ls));

            return true;
        }

        bool add_begin_sl(state* s_to, transition* l = nullptr)
        {
            if (!s_to)
                return false;

            link_to_state ls;
            ls.ptr_link = l;
            ls.ptr_state = s_to;
            state_transitions_tab[s_from].insert(std::pair<id_t /*event id*/, link_to_state>(e_id, ls));

            current_state_ptr = s;
            begin_state_ptr = current_state_ptr;

            return true;
        }

        bool add_end_sl(state* s_from, transition* l = nullptr)
        {
            if (!s_from)
                return false;

            link_to_state ls;
            ls.ptr_link = l;
            ls.ptr_state = s_to;
            state_transitions_tab[s_from].insert(std::pair<id_t /*event id*/, link_to_state>(e_id, ls));

            return true;
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
                    current_state_ptr->do_activate();
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
        void invoke(global_event& ref_e) override
        {
            for (state_machine* sm : sms)
            {
                if (sm)
                    sm->recv_triggering_event(ref_e);
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




