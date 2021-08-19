
#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <ctime>
#include <any>
#include "rm_messages.h"





namespace rm // rule machine
{
    class event;
    class state_machine;
    class rule_machine;

    template <class TStateMachine> // CRPT interface for event rise callback
    struct ISMEventInvoker
    {
        void invoke(const event& ref_e, bool is_local)
        {
            static_cast<TStateMachine*>(this)->invoke(ref_e, is_local);
        }
    };

    template <class TRuleMachine> // CRPT interface for event rise callback
    struct IRMEventInvoker
    {
        void invoke(const event& ref_e)
        {
            static_cast<TRuleMachine*>(this)->invoke(ref_e);
        }
    };

    using id_t = int;
    using result_t = std::tuple <bool, std::string>;
    using i_sm_event_invoker_t = ISMEventInvoker<state_machine>;
    using i_rm_event_invoker_t = IRMEventInvoker<rule_machine>;
    using ptr_action_t = void (*)(const event&);
    using ptr_guard_t = bool (*)(const event&);

    enum class status { enabled, disabled, paused };

    /// event ///

    class event
    {
    public:
        const id_t id = -1; // идентификатор
        const std::string name; // имя, не важное
        const std::time_t time = std::time(nullptr); // время создания события

    protected:
        std::any param;

    public:
        event() = delete;

        event(const event& e)
            : id(e.id), name(e.name)
        {
        }

        event(id_t id_, std::string name_)
            : id(id_), name(name_)
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

        std::any get_param()
        {
            return param;
        }

        virtual ~event() {};
    };

    /// end event ///

    /// transition ///

    class transition
    {
    protected:
        i_sm_event_invoker_t& invoker;

    public:
        transition() = delete;

        transition(i_sm_event_invoker_t& invoker_) :
            invoker(invoker_)
        {
        }

        virtual void do_action(const event&)
        {
        }

        virtual bool is_guard_condition(const event&)
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
        transition_vecs(i_sm_event_invoker_t& invoker_) :
            transition(invoker_)
        {
        }

        void do_action(const event& ref_e) override
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

        bool is_guard_condition(const event& ref_e) override
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
        transition_test() = delete;

        transition_test(i_sm_event_invoker_t& invoker_, std::string name_) :
            transition_vecs (invoker_),
            name(name_)
        {
        }

        void do_action(const event& ref_e) override
        {
            std::cout << "link " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: action" << std::endl;
        }

        bool is_guard_condition(const event& ref_e) override
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
        i_sm_event_invoker_t& invoker;

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
        state() = delete;

        state(i_sm_event_invoker_t& invoker_) :
            invoker(invoker_)
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
            return std::to_string (reinterpret_cast<unsigned long long>(this)); // address is ID
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
        initial_state(i_sm_event_invoker_t& invoker_) :
            state(invoker_)
        {
        }

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
        final_state(i_sm_event_invoker_t& invoker_) :
            state(invoker_)
        {
        }

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

    public:
        state_vecs(i_sm_event_invoker_t& invoker_) :
            state(invoker_)
        {
        }

    protected:
        void do_entry_action(const event& ref_e) override
        {
            for (auto action : entry_actions)
                (*action)(ref_e);
        }
        void do_exit_action(const event& ref_e) override
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
        state_test() = delete;

        state_test(i_sm_event_invoker_t& invoker_, std::string _name) :
            state_vecs (invoker_),
            name(_name)
        {
        }

    protected:
        void do_entry_action(const event& ref_e) override
        {
            //std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;
        }
        void do_exit_action(const event& ref_e) override
        {
            //std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: exit action" << std::endl;
        }
        void do_internal_action(const event& ref_e) override
        {
            //std::cout << "state " << name << " * event [ " << ref_e.name << ", " << std::to_string(ref_e.id) << " ]: internal action" << std::endl;
        }

    public:
        std::string to_string() override
        {
            return name;
        }
    };

    /// end state_test ///



    /// state_machine

    class state_machine :
        public i_sm_event_invoker_t
    {
        using msg = sgt_messages <base_messages>;

    public:
        //const id_t id;

        state_machine() = delete;

        state_machine(i_rm_event_invoker_t* global_invoker_)
            : global_invoker(global_invoker_)
        {
        }

    private:
        struct transit_to_state
        {
            transition* ptr_transition = nullptr;
            state* ptr_target_state = nullptr;
        };

        std::map<state* /*source state*/, std::multimap<id_t /*event id*/, transit_to_state>> state_transitions_tab;

        i_rm_event_invoker_t* global_invoker = nullptr;

        state* current_state_ptr = nullptr;
        initial_state* initial_state_ptr = nullptr;
        final_state* final_state_ptr = nullptr;

        status curr_status = status::disabled;

    public:
        void invoke(const event& ref_e, bool is_local)
        {
            if (is_local)
                recv_triggering_event(ref_e);
            else
                if (global_invoker)
                    global_invoker->invoke(ref_e);
        }

    public:
        result_t recv_triggering_event(const event& ref_e)
        {

            // return: success - true, error - false

            /// pre conditions ///

            if (curr_status != status::enabled)
                return { true, msg::get_instance().msgs.true_nenabled };

            // test curr state
            if (!current_state_ptr)
                return { false, msg::get_instance().msgs.false_curr_state_null };

            // find curr state in tab
            auto it_curr_state_in_tab = state_transitions_tab.find(current_state_ptr);
            if (it_curr_state_in_tab == state_transitions_tab.end())
                return { false, msg::get_instance().msgs.false_curr_state_nfaund };

            // find links from curr state to new state by event in tab
            auto& mmap_ref = it_curr_state_in_tab->second; // mmap_ref - std::multimap<id_t /*event id*/, transit_to_state>
            auto it_transits_by_event = mmap_ref.equal_range(ref_e.id); // transits by event may be multiple
            transit_to_state* ptr_transit_to_state = nullptr;
            for (auto& it_transit = it_transits_by_event.first; it_transit != it_transits_by_event.second; ++it_transit) // find valid transit
            {
                transition* ptr_transit = it_transit->second.ptr_transition;

                if (!ptr_transit) // if link is null, then success. link can be NULL. Guard condition will not test
                {
                    ptr_transit_to_state = &it_transit->second;
                    break;
                }

                // check link guard condition 
                if (ptr_transit)
                    if (ptr_transit->is_guard_condition(ref_e))
                    {
                        ptr_transit_to_state = &it_transit->second;
                        break;
                    }
            }
            if (!ptr_transit_to_state)
                return { true, msg::get_instance().msgs.true_reject };

            if (!(ptr_transit_to_state->ptr_target_state))
                return { false, msg::get_instance().msgs.false_target_state_null };

            /// - ///

            /// Ok! change state now! ///

            // do unactivate source (current) state
            current_state_ptr->do_unactivate(ref_e);

            // do link actions
            if (ptr_transit_to_state->ptr_transition)
                ptr_transit_to_state->ptr_transition->do_action(ref_e);

            // update current state index, change current state from source to target
            current_state_ptr = ptr_transit_to_state->ptr_target_state;

            if (current_state_ptr == final_state_ptr)
            {
                set_status_disabled();
                return { true, msg::get_instance().msgs.true_curr_state_final };
            }

            // do activate target (current) state
            current_state_ptr->do_activate(ref_e);

            return { true, msg::get_instance().msgs.true_ok };
        }

    public:
        result_t add_event_state_state_transition(id_t e_id, state* s_source, state* s_target, transition* t = nullptr)
        {
            if (!s_source || !s_target)
                return { false, msg::get_instance().msgs.param_state_is_null };

            transit_to_state ls;
            ls.ptr_transition = t;
            ls.ptr_target_state = s_target;
            state_transitions_tab[s_source].insert(std::pair<id_t /*event id*/, transit_to_state>(e_id, ls));

            return { true, msg::get_instance().msgs.true_ok };
        }

        result_t add_event_state_state_transition(id_t e_id, initial_state* s_source, state* s_target, transition* t = nullptr)
        {
            if (initial_state_ptr && initial_state_ptr != s_source)
                return { false, msg::get_instance().msgs.init_state_is_already_set }; // initial state must be single!

            result_t rez = add_event_state_state_transition(e_id, static_cast<state*>(s_source), s_target, t);
            if (std::get<0>(rez))
                initial_state_ptr = s_source;

            return rez;
        }

        result_t add_event_state_state_transition(id_t e_id, state* s_source, final_state* s_target, transition* t = nullptr)
        {
            if (final_state_ptr && final_state_ptr != s_target) // не обязательное условие, чисто для симметрии с initial state
                return { false, msg::get_instance().msgs.final_state_is_already_set }; // final state must be single! но это не точно :)

            result_t rez = add_event_state_state_transition(e_id, s_source, static_cast<state*>(s_target), t);
            if (std::get<0>(rez))
                final_state_ptr = s_target;

            return rez;
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

        void set_status_enabled(const event& e)
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

    class rule_machine :
        public i_rm_event_invoker_t
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
        void set_status_enabled() //const event& e)
        {
            curr_status = status::enabled;

            //for (state_machine* sm : sms)
            //    if (sm)
            //        sm->set_status_enabled(e); // переделать! каждый автомат запускается своим событием!
        }
        void set_status_paused()
        {
            curr_status = status::paused;

            for (state_machine* sm : sms)
                if (sm)
                    sm->set_status_paused();
        }
        void set_status_disabled()
        {
            curr_status = status::disabled;

            for (state_machine* sm : sms)
                if (sm)
                    sm->set_status_disabled();
        }
        /// - ///

    public:
        void invoke(const event& ref_e)
        {
            for (state_machine* sm : sms)
            {
                if (sm)
                    sm->recv_triggering_event(ref_e);
            }
        }

        void clear()
        {
            set_status_disabled();

            for (state_machine* sm : sms)
            {
                if (sm)
                    sm->clear();
            }

            sms.clear();
        }
    };
}




