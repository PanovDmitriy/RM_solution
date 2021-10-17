
#pragma once

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <ctime>
#include <any>
#include <memory>
#include "rm_messages.h"




namespace rm // rule machine
{
    constexpr bool is_event_log = true;

    class event;
    class state_machine;
    class rule_machine;

    using id_t = int;
    constexpr id_t id_undef_value = -1;

    using shared_event = std::shared_ptr<event>;

    using result_t = std::tuple <bool, std::string>;
    using ptr_action_t = void (*)(const event&);
    using ptr_guard_t = bool (*)(const event&);

    enum class status { enabled, disabled, paused };

    struct IEventHandler
    {
        virtual result_t rise_event(const event& ref_e) = 0;
        virtual result_t rise_event(event&& rlv_e) = 0;
        virtual result_t rise_event(shared_event& she_e) = 0;
        virtual result_t release_event() = 0;
    };

    struct IMachine
    {
        virtual void set_status_enabled(const event& e) = 0;
        virtual void set_status_paused() = 0;
        virtual void set_status_disabled() = 0;
        virtual void clear() = 0;
        virtual IEventHandler* get_event_handler() = 0;
    };


    /// event ///

    //class param_exaple
    //{
    //private:
    //    int* ptr = nullptr;

    //public:
    //    param_exaple()
    //    {
    //        ptr = new int[10];
    //    }

    //    param_exaple(const param_exaple& p)
    //    {
    //        ptr = new int[10];
    //        mem_copy(p, ptr);
    //    }

    //    param_exaple(param_exaple&& p) noexcept
    //    {
    //        ptr = p.ptr;
    //        p.ptr = nullptr;
    //    }

    //    ~param_exaple()
    //    {
    //        delete ptr;
    //    }
    //};

    class event final
    {
    public:
        const id_t id = id_undef_value; // идентификатор
        const std::time_t time = std::time(nullptr); // время создания события
        const std::any param;

        event() = delete;

        event(id_t id_) : 
            id(id_) 
        { 
            if constexpr (is_event_log)
                std::cout << "log: event(id) id: " << id << std::endl;
        }

        template<typename T>
        event(id_t id_, T&& param_) :
            id(id_), param{ std::forward<T>(param_) }
        {
            if constexpr (is_event_log)
                std::cout << "log: event(id, template T&& param) id: " << id << std::endl;
        }

        event(const event& e) :
            id(e.id), param(e.param), time(e.time) 
        { 
            if constexpr (is_event_log)
                std::cout << "log: event(const event&) id: " << id << std::endl;
        }

        event(event&& e) noexcept : 
            id(e.id), time(e.time), param (std::move(const_cast<std::any&>(e.param)))
        {
            const_cast<id_t&>(e.id) = id_undef_value;
            if constexpr (is_event_log)
                std::cout << "log: event(event&&) id: " << id << std::endl;
        }

        void operator= (const event& ref_e) noexcept
        {
            const_cast<id_t&>(id) = ref_e.id;
            const_cast<std::any&>(param) = ref_e.param;
            if constexpr (is_event_log)
                std::cout << "log: op=(const event&) id: " << id << std::endl;
        }
        void operator= (event&& rv_e) noexcept
        {
            const_cast<id_t&>(id) = rv_e.id;
            const_cast<id_t&>(rv_e.id) = id_undef_value;
            const_cast<std::any&>(rv_e.param).swap(const_cast<std::any&>(param));
            if constexpr (is_event_log)
                std::cout << "log: op=(event&&) id: " << id << std::endl;
        }
        
        bool operator== (const event& ref_e)
        {
            return id == ref_e.id;
        }
        bool operator != (const event& ref_e)
        {
            return !(id == ref_e.id);
        }

        ~event() 
        {
            if constexpr (is_event_log)
                std::cout << "log: ~event id: " << id << std::endl;
        };
    };

    /// end event ///

    /// transition ///

    struct transition
    {
        virtual void do_action(const event&)
        {
        }

        virtual bool is_guard_condition(const event&)
        {
            return true;
        }

        virtual ~transition() {}
    };

    /// end transition ///

    /// transition_vecs ///

    class transition_vecs
        : public transition
    {
    protected:
        IEventHandler& event_handler;

    protected:
        std::vector <ptr_action_t> actions;
        std::vector <ptr_guard_t> guards;

    public:
        transition_vecs(IEventHandler& event_handler_) :
            event_handler(event_handler_)
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

        transition_test(IEventHandler& event_handler_, std::string name_) :
            transition_vecs (event_handler_),
            name(name_)
        {
        }

        void do_action(const event& ref_e) override
        {
            std::cout << "link " << name << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: action" << std::endl;
        }

        bool is_guard_condition(const event& ref_e) override
        {
            std::cout << "link " << name << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: guard_condition: true" << std::endl;

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
        initial_state()
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
        final_state()
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
        IEventHandler& event_handler;

    protected:
        std::vector <ptr_action_t> entry_actions;
        std::vector <ptr_action_t> exit_actions;

    public:
        state_vecs(IEventHandler& event_handler_) :
            event_handler(event_handler_)
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

        state_test(IEventHandler& event_handler_, std::string _name) :
            state_vecs (event_handler_),
            name(_name)
        {
        }

    protected:
        void do_entry_action(const event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;
        }
        void do_exit_action(const event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: exit action" << std::endl;
        }
        void do_internal_action(const event& ref_e) override
        {
            std::cout << "state " << name << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: internal action" << std::endl;
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
        public IEventHandler,
        public IMachine
    {
        inline auto msg()
        {
            return sgt_messages <base_messages>::get_instance().msgs;
        }

    private:
        //std::queue<event> event_queue;
        std::queue<std::variant<event, shared_event>> event_queue;

    private:
        struct transit_to_state
        {
            transition* ptr_transition = nullptr;
            state* ptr_target_state = nullptr;
        };

        using state_transitions_tab_t = std::map<state* /*source state*/, std::multimap<id_t /*event id*/, transit_to_state>>;
        state_transitions_tab_t state_transitions_tab;

        state* current_state_ptr = nullptr;
        initial_state* initial_state_ptr = nullptr;
        final_state* final_state_ptr = nullptr;

        status curr_status = status::disabled;

    public:
        //const id_t id;

        state_machine() = default;

    public:
        result_t rise_event(const event& ref_e) override // инициировать событие
        {
            event_queue.push(ref_e);
            return { true, msg().true_ok };
        }

        result_t rise_event(event&& rlv_e) override // инициировать событие
        {
            event_queue.emplace(std::move(rlv_e));
            return { true, msg().true_ok };
        }

        result_t rise_event(shared_event& she_e) override
        {
            std::variant 
        }

    public:
        IEventHandler* get_event_handler() override
        {
            return static_cast<IEventHandler*>(this);
        }

    public:
        result_t release_event() override
        {
            while (!event_queue.empty())
            {
                event& e = event_queue.front();
                inside_release_event(e);
                event_queue.pop();
            }

            return { true, msg().true_ok };
        }

    protected:
        result_t inside_release_event(const event& ref_e)  // реализовать событие
        {
            // return: success - true, error - false

            // + pre conditions
            if (curr_status != status::enabled)
                return { true, msg().true_nenabled };

            // test curr state
            if (!current_state_ptr)
                return { false, msg().false_curr_state_null };

            // find curr state in tab
            auto it_curr_state_in_tab = state_transitions_tab.find(current_state_ptr);
            if (it_curr_state_in_tab == state_transitions_tab.end())
                return { false, msg().false_curr_state_nfaund };

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
            if (!ptr_transit_to_state) // не найдены переходы по событию
                return { true, msg().true_reject };

            if (!(ptr_transit_to_state->ptr_target_state))
                return { false, msg().false_target_state_null };
            // - pre conditions

            /// Ok! change state now! ///

            // do unactivate source (current) state
            current_state_ptr->do_unactivate(ref_e);

            // do transition actions
            if (ptr_transit_to_state->ptr_transition)
                ptr_transit_to_state->ptr_transition->do_action(ref_e);

            // update current state index, change current state from source to target
            current_state_ptr = ptr_transit_to_state->ptr_target_state;

            if (current_state_ptr == final_state_ptr)
            {
                set_status_disabled();
                return { true, msg().true_curr_state_final };
            }

            // do activate target (current) state
            current_state_ptr->do_activate(ref_e);

            return { true, msg().true_ok };
        }

    public:
        result_t add_event_state_state_transition(id_t e_id, state* s_source, state* s_target, transition* t = nullptr)
        {
            if (!s_source || !s_target)
                return { false, msg().param_state_is_null };

            transit_to_state ls;
            ls.ptr_transition = t;
            ls.ptr_target_state = s_target;
            state_transitions_tab[s_source].insert(std::pair<id_t /*event id*/, transit_to_state>(e_id, ls));

            return { true, msg().true_ok };
        }

        result_t add_event_state_state_transition(id_t e_id, initial_state* s_source, state* s_target, transition* t = nullptr)
        {
            if (initial_state_ptr && initial_state_ptr != s_source)
                return { false, msg().init_state_is_already_set }; // initial state must be single!

            result_t rez = add_event_state_state_transition(e_id, static_cast<state*>(s_source), s_target, t);
            if (std::get<0>(rez))
                initial_state_ptr = s_source;

            return rez;
        }

        result_t add_event_state_state_transition(id_t e_id, state* s_source, final_state* s_target, transition* t = nullptr)
        {
            if (final_state_ptr && final_state_ptr != s_target) // не обязательное условие, чисто для симметрии с initial state
                return { false, msg().final_state_is_already_set }; // final state must be single! но это не точно :)

            result_t rez = add_event_state_state_transition(e_id, s_source, static_cast<state*>(s_target), t);
            if (std::get<0>(rez))
                final_state_ptr = s_target;

            return rez;
        }

        void view_integrity()
        {
            std::cout << "state list:" << std::endl;

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

        result_t check_integrity()
        {
            // final не может быть исходным
            // init не может быть целевым
            // init не может быть null
            // все исходные и целевые должы быть не null
            // из init должен быть переход
            // в каждое состояние (кроме init state) должен существовать переход
            // примечение: состояние (кролме init) не обязано быть исходным

            if (!initial_state_ptr)
                return { false, "Init state is null" };

            state* istate_ptr = static_cast<state*>(initial_state_ptr);
            state* fstate_ptr = static_cast<state*>(final_state_ptr);
            bool is_init_as_source = false;
            std::set<state*> target_states;

            for (auto& [ptr_source_state, a] : state_transitions_tab)
            {
                if (ptr_source_state == nullptr)
                    return { false, "Source state is null" };

                if (ptr_source_state == istate_ptr)
                    is_init_as_source = true;

                if (ptr_source_state == fstate_ptr)
                    return { false, "Final state is source state" };

                for (auto& [event_id, transit_to_state] : a)
                {
                    if (transit_to_state.ptr_target_state == nullptr)
                        return { false, "Target state is null" };

                    if (transit_to_state.ptr_target_state == istate_ptr)
                        return { false, "Init state is target state" };

                    target_states.insert(transit_to_state.ptr_target_state);
                }
            }

            if (!is_init_as_source)
                return { false, "Init state haven't transitions" };

            for (auto& [ptr_source_state, a] : state_transitions_tab)
            {
                if (ptr_source_state == istate_ptr)
                    continue;

                if (target_states.find(ptr_source_state) == target_states.end())
                    return { false, "Not linked state" };
            }

            return { true, msg().true_ok };
        }

        // + control methods sm
        status get_status()
        {
            return curr_status;
        }

        void set_status_enabled(const event& e) override
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
                rise_event(e);
                break;
            }
        }

        void set_status_paused() override
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

        void set_status_disabled() override
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
        // - control methods sm

        void clear() override
        {
            set_status_disabled ();
            state_transitions_tab.clear();
        }
    };


    /// rule machine

    class rule_machine :
        public IEventHandler,
        public IMachine
    {
        inline auto msg()
        {
            return sgt_messages <base_messages>::get_instance().msgs;
        }

    private:
        std::vector<IMachine*> inside_machines;
        status curr_status = status::disabled;

    public:
        void add_machine(IMachine* ptr_m)
        {
            inside_machines.push_back(ptr_m);
        }

    public:
        IEventHandler* get_event_handler() override
        {
            return static_cast<IEventHandler*>(this);
        }

        result_t rise_event(const event& ref_e) override
        {
            if (curr_status != status::enabled)
                return { true, msg().true_ok };

            for (IMachine* m : inside_machines)
                if (m)
                {
                    IEventHandler* h = m -> get_event_handler();
                    if (h)
                        h -> rise_event(ref_e);
                }

            return { true, msg().true_ok };
        }

        result_t rise_event(event&& rlv_e) override
        {
            if (curr_status != status::enabled)
                return { true, msg().true_ok };

            for (IMachine* m : inside_machines)
                if (m)
                {
                    IEventHandler* h = m -> get_event_handler();
                    if (h)
                        h->rise_event(rlv_e);
                        //h->rise_event(std::move(rlv_e));
                }

            return { true, msg().true_ok };
        }

        result_t release_event() override
        {
            for (IMachine* m : inside_machines)
                if (m)
                {
                    IEventHandler* h = m->get_event_handler();
                    if (h)
                        h->release_event();
                }

            return { true, msg().true_ok };
        }

    public:
        // + control methods sm
        status get_status()
        {
            return curr_status;
        }
        void set_status_enabled(const event& e)
        {
            curr_status = status::enabled;

            //for (state_machine* sm : sms)
            //    if (sm)
            //        sm->set_status_enabled(e); // переделать! каждый автомат запускается своим событием!
        }
        void set_status_paused()
        {
            curr_status = status::paused;

            for (IMachine* sm : inside_machines)
                if (sm)
                    sm->set_status_paused();
        }
        void set_status_disabled()
        {
            curr_status = status::disabled;

            for (IMachine* sm : inside_machines)
                if (sm)
                    sm->set_status_disabled();
        }
        // - control methods sm

    public:
        void clear()
        {
            set_status_disabled();

            for (IMachine* ptr_sm : inside_machines)
            {
                if (ptr_sm)
                    ptr_sm->clear();
            }

            inside_machines.clear();
        }
    };
}




