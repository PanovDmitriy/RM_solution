
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
        const id_t id = -1;
        const std::string name;
        const std::time_t time = std::time(nullptr);
        const bool is_local = false;

    private:
        event()
        {
        }

    public:
        event(const event& e)
            : id(e.id), name(e.name), is_local (e.is_local)
        {
        }

        event(id_t id_, std::string name_, bool is_local_ = false)
            : id(id_), name(name_), is_local (is_local_)
        {
            if (id < 0)
                throw std::exception("event ID must be >= 0 : ", id);
        }

        bool operator == (const event& ref_e)
        {
            return id == ref_e.id;
        }
        bool operator != (const event& ref_e)
        {
            return !(id == ref_e.id);
        }

        virtual ~event() {};
    };

    class event_start :
        public event
    {
    public:
        event_start() :
            event(0, "start event", true)
        {
            const_cast<id_t&>(id) = -1;
        }
    };
   
    /// end event ///

    typedef void(*ptr_action_t)(event&);
    typedef bool(*ptr_guard_t)(event&);

    /// transition ///

    class transition
    {
    public:
        virtual void do_action(event&)
        {
        }

        virtual bool is_guard_condition(event&)
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
        virtual void do_action(event& ref_e)
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

        virtual bool is_guard_condition(event& ref_e)
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

        void do_action(event& ref_e)
        {
            std::cout << "link " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: action" << std::endl;
        }

        bool is_guard_condition(event& ref_e)
        {
            std::cout << "link " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: guard_condition: true" << std::endl;

            return true;
        }
    };

    /// end transition_test ///

    /// state ///

    class state
    {
    protected:
        virtual void do_entry_action(const event&)
        {
        }
        virtual void do_exit_action(const event&)
        {
        }
        virtual void do_internal_action(const event&)
        {
        }

    public:
        virtual void do_activate(const event& ref_e)
        {
            do_entry_action(ref_e);
            do_internal_action(ref_e);
        }
        virtual void do_unactivate(const event& ref_e)
        {
            // todo: break internal action
            do_exit_action(ref_e);
        }

    public:
        virtual std::string to_string()
        {
            return std::to_string (reinterpret_cast<unsigned long long>(this));
        }

    public:
        virtual ~state()
        {
        }
    };

    class initial_state final : //pseudostate
        public state
    {
    public:
        void do_activate(const event&) override
        {
        }
        void do_unactivate(const event&) override
        {
        }

        std::string to_string() override
        {
            return "init state";
        }
    };
       
    class final_state final :
        public state
    {
    public:
        void do_activate(const event&) override
        {
        }
        void do_unactivate(const event&) override
        {
        }

        std::string to_string() override
        {
            return "final state";
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

    //public:
    //    state_vecs() : state()
    //    {
    //    }

    protected:
        virtual void do_entry_action(event& ref_e)
        {
            for (auto action : entry_actions)
                (*action)(ref_e);
        }
        virtual void do_exit_action(event& ref_e)
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
    protected:
        const std::string name;

    public:
        state_test(std::string _name) : 
            name(_name)
        {
        }

    protected:
        void do_entry_action(const event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;
        }
        void do_exit_action(const event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: exit action" << std::endl;
        }
        void do_internal_action(const event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: internal action" << std::endl;
        }

    public:
        std::string to_string() override
        {
            return name;
        }
    };

    /// end state_test ///


    class i_event_invoker
    {
    public:
        virtual void invoke(event& ref_e) = 0;
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
        struct transit_to_state
        {
            transition* ptr_transition = nullptr;
            state* ptr_target_state = nullptr;
        };

        std::map<state* /*source state*/, std::multimap<id_t /*event id*/, transit_to_state>> state_transitions_tab;

        i_event_invoker* invoker;

        state* current_state_ptr = nullptr;
        initial_state* initial_state_ptr = nullptr;
        final_state* final_state_ptr = nullptr;

        status curr_status = status::disabled;

    protected:
        void invoke(event& ref_e)
        {
            if (ref_e.is_local)
                recv_triggering_event(ref_e);
            else
                if (invoker)
                    invoker->invoke(ref_e);
        }

    public:
        std::tuple <bool, std::string> recv_triggering_event(event& ref_e)
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
            transit_to_state* ptr_ls = nullptr;
            for (auto& itr = it2.first; itr != it2.second; ++itr)
            {
                transition* pl = itr->second.ptr_transition;

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

            if (!(ptr_ls->ptr_target_state))
                return { false, "Target state is null" };

            /// - ///

            /// Ok! change state now! ///

            // do unactivate source (current) state
            current_state_ptr->do_unactivate(ref_e);

            // do link actions
            if (ptr_ls->ptr_transition)
                ptr_ls->ptr_transition->do_action(ref_e);

            // update current state index, change current state from source to target
            current_state_ptr = ptr_ls->ptr_target_state;

            if (current_state_ptr == final_state_ptr)
            {
                set_status_disabled();
                return { true, "Current state is Final. State machine is disable" };
            }

            // do activate target (current) state
            current_state_ptr->do_activate(ref_e);

            return { true, "Ok" };
        }

    public:
        bool add_essl(id_t e_id, state* s_source, state* s_target, transition* t = nullptr)
        {
            if (!s_source || !s_target)
                return false;

            transit_to_state ls;
            ls.ptr_transition = t;
            ls.ptr_target_state = s_target;
            state_transitions_tab[s_source].insert(std::pair<id_t /*event id*/, transit_to_state>(e_id, ls));

            return true;
        }

        bool add_essl(id_t e_id, initial_state* s_source, state* s_target, transition* t = nullptr)
        {
            if (initial_state_ptr && initial_state_ptr != s_source)
                return false; // initial state must be single!

            if (add_essl(e_id, static_cast<state*>(s_source), s_target, t))
            {
                initial_state_ptr = s_source;
                return true;
            }

            return false;
        }

        bool add_essl(id_t e_id, state* s_source, final_state* s_target, transition* t = nullptr)
        {
            if (final_state_ptr && final_state_ptr != s_target) // �� ������������ �������, ����� ��� ��������� � initial state
                return false; // final state must be single! �� ��� �� ����� :)

            if (add_essl(e_id, s_source, static_cast<state*>(s_target), t))
            {
                final_state_ptr = s_target;
                return true;
            }

            return false;
        }

        void check_obj()
        {
            std::cout << "state list:" << std::endl;

            //look: std::map<state*, std::multimap<id_t /*event id*/, link_to_state>> state_transitions_tab;

            for (auto& a : state_transitions_tab)
            {
                std::cout << "\tstate " << a.first->to_string() << std::endl;
                for (auto& [event_id, transit_to_state] : a.second)
                {
                    std::cout << "\t\tevent ID: " << event_id;
                    if (!(transit_to_state.ptr_transition))
                        std::cout << " null link to ";
                    else
                        std::cout << " not null link to ";
                    std::cout << transit_to_state.ptr_target_state->to_string() << std::endl;
                }
            }
        }

        /// control methods sm ///
        status get_status()
        {
            return curr_status;
        }

        void set_status_enabled(event& e)
        {
            switch (curr_status)
            {
            case status::enabled:
                break;
            case status::paused:
                curr_status = status::enabled;
                break;
            case status::disabled:
                current_state_ptr = static_cast<state*>(initial_state_ptr);
                initial_state_ptr->do_activate(e);
                curr_status = status::enabled;
                recv_triggering_event(e);
                break;
            }
        }

        void set_status_paused()
        {
            switch (curr_status)
            {
            case status::enabled:
                curr_status = status::paused;
                break;
            case status::paused:
            case status::disabled:
                break;
            }
        }

        void set_status_disabled()
        {
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
        }

        /// - ///

        void clear()
        {
            set_status_disabled ();
            state_transitions_tab.clear();
        }
    };


    /// rule machine

    //class rule_machine :
    //    public i_event_invoker
    //{
    //private:
    //    status curr_status = status::disabled;

    //private:
    //    std::vector<state_machine*> sms;

    //public:
    //    void add_sm(state_machine* sm)
    //    {
    //        sms.push_back(sm);
    //    }

    //public:
    //    /// control methods sm ///
    //    status get_status()
    //    {
    //        return curr_status;
    //    }
    //    void set_status(status new_status)
    //    {
    //        curr_status = new_status;

    //        for (state_machine* sm : sms)
    //        {
    //            if (sm)
    //                sm->set_status(new_status);
    //        }
    //    }
    //    /// - ///

    //public:
    //    void invoke(event& ref_e) override
    //    {
    //        for (state_machine* sm : sms)
    //        {
    //            if (sm)
    //                sm->recv_triggering_event(ref_e);
    //        }
    //    }

    //    void clear()
    //    {
    //        set_status (status::disabled);

    //        for (state_machine* sm : sms)
    //        {
    //            if (sm)
    //                sm->clear();
    //        }

    //        sms.clear();
    //    }
    //};
}




