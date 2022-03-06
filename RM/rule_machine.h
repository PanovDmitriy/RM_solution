
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
#include <algorithm>
#include "rm_messages.h"




namespace rm // rule machine
{
    constexpr bool is_event_log = false;

    class event;
    class state_machine;
    class rule_machine;

    using id_t = int;
    constexpr id_t id_undef_cvalue = -1;

    using shared_event_t = std::shared_ptr<event>;

    using result_t = std::tuple <bool, std::string>;
    using ptr_action_t = void (*)(const event&);
    using ptr_guard_t = bool (*)(const event&);

    enum class status { enabled, disabled, paused };

    struct IEventHandler
    {
        virtual result_t rise_event(event&&) = 0;
        virtual result_t rise_event(const event&) = 0;
        virtual result_t release_event() = 0;
    };

    struct IMachine
    {
        virtual void set_status_enabled() = 0;
        virtual void set_status_enabled(event&&) = 0;
        virtual void set_status_enabled(const event&) = 0;
        virtual void set_status_paused() = 0;
        virtual void set_status_disabled() = 0;
        virtual status get_status() = 0;
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
        const id_t id = id_undef_cvalue; // идентификатор
        const std::time_t time = std::time(nullptr); // время создания события
        const std::any param;

        event() = delete;

        event(id_t _id) : 
            id(_id) 
        { 
            if constexpr (is_event_log)
                std::cout << "log: event(id) id: " << id << std::endl;
        }

        template<typename T>
        event(id_t _id, T&& param_) :
            id(_id), param{ std::forward<T>(param_) }
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
            const_cast<id_t&>(e.id) = id_undef_cvalue;
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
            const_cast<id_t&>(rv_e.id) = id_undef_cvalue;
            const_cast<std::any&>(rv_e.param) = std::move(const_cast<std::any&>(rv_e.param));
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
        IEventHandler& event_handler_;

    protected:
        std::vector <ptr_action_t> actions_;
        std::vector <ptr_guard_t> guards_;

    public:
        transition_vecs(IEventHandler& event_handler) :
            event_handler_(event_handler)
        {
        }

        void do_action(const event& ref_e) override
        {
            for (auto action : actions_)
                (*action)(ref_e);
        }

        void add_action(const ptr_action_t func)
        {
            actions_.push_back(func);
        }

        void clear_actions()
        {
            actions_.clear();
        }

        bool is_guard_condition(const event& ref_e) override
        {
            for (auto guard : guards_)
            {
                if (!(*guard)(ref_e))
                    return false;
            }
            return true;
        }

        void add_guard(const ptr_guard_t func)
        {
            guards_.push_back(func);
        }
        void clear_guards()
        {
            guards_.clear();
        }
    };

    /// end transition_vecs ///

    /// transition_test ///

    class transition_test
        : public transition_vecs
    {
    public:
        const std::string name_;

    public:
        transition_test() = delete;

        transition_test(IEventHandler& event_handler, std::string name) :
            transition_vecs (event_handler),
            name_(name)
        {
        }

        void do_action(const event& ref_e) override
        {
            std::cout << "link " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: action" << std::endl;
        }

        bool is_guard_condition(const event& ref_e) override
        {
            std::cout << "link " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: guard_condition: true" << std::endl;

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
        IEventHandler& event_handler_;

    protected:
        std::vector <ptr_action_t> entry_actions_;
        std::vector <ptr_action_t> exit_actions_;

    public:
        state_vecs(IEventHandler& event_handler_) :
            event_handler_(event_handler_)
        {
        }

    protected:
        void do_entry_action(const event& ref_e) override
        {
            for (auto action : entry_actions_)
                (*action)(ref_e);
        }
        void do_exit_action(const event& ref_e) override
        {
            for (auto action : exit_actions_)
                (*action)(ref_e);
        }

    public:
        void add_entry_action(const ptr_action_t func)
        {
            entry_actions_.push_back(func);
        }
        void clear_entry_actions()
        {
            entry_actions_.clear();
        }

        void add_exit_action(const ptr_action_t func)
        {
            exit_actions_.push_back(func);
        }
        void clear_exit_actions()
        {
            exit_actions_.clear();
        }
    };

    /// state_vecs ///

    /// state_test ///

    class state_test
        : public state_vecs
    {
    protected:
        const std::string name_;

    public:
        state_test() = delete;

        state_test(IEventHandler& event_handler, std::string name) :
            state_vecs (event_handler),
            name_(name)
        {
        }

    protected:
        void do_entry_action(const event& ref_e) override
        {
            std::cout << "state " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;
        }
        void do_exit_action(const event& ref_e) override
        {
            std::cout << "state " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: exit action" << std::endl;
        }
        void do_internal_action(const event& ref_e) override
        {
            std::cout << "state " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: internal action" << std::endl;
        }

    public:
        std::string to_string() override
        {
            return name_;
        }
    };

    /// end state_test ///



    /// state_machine

    class state_machine :
        public IEventHandler,
        public IMachine
    {
        inline auto msg_()
        {
            return sgt_messages <base_messages>::get_instance().msgs;
        }

    private:
        std::queue<shared_event_t> event_queue_;

    private:
        struct transit_to_state_t
        {
            transition* ptr_transition = nullptr;
            state* ptr_target_state = nullptr;
        };

        using state_transitions_tab_t = std::map<state* /*source state*/, std::multimap<id_t /*event id*/, transit_to_state_t>>;
        state_transitions_tab_t state_transitions_tab_;

        state* ptr_current_state_ = nullptr;
        initial_state* ptr_initial_state_ = nullptr;
        final_state* ptr_final_state_ = nullptr;

        status curr_status_ = status::disabled;

    public:
        //const id_t id;

        state_machine() = default;

    public:
        result_t rise_event(const event& ref_e) override // инициировать событие
        {
            event_queue_.emplace(std::make_shared<event>(ref_e));
            return { true, msg_().true_ok };
        }

        result_t rise_event(event&& rlv_e) override // инициировать событие
        {
            event_queue_.emplace(std::make_shared<event>(std::move(rlv_e)));
            return { true, msg_().true_ok };
        }

    public:
        IEventHandler* get_event_handler() override
        {
            return static_cast<IEventHandler*>(this);
        }

    public:
        result_t release_event() override
        {
            while (!event_queue_.empty())
            {
                shared_event_t& se = event_queue_.front();
                release_event_(*se);
                event_queue_.pop();
            }

            return { true, msg_().true_ok };
        }

    protected:
        result_t release_event_(const event& ref_e)  // реализовать событие
        {
            // return: success - true, error - false

            // + pre conditions
            if (curr_status_ != status::enabled)
                return { true, msg_().true_nenabled };

            // test curr state
            if (!ptr_current_state_)
                return { false, msg_().false_curr_state_null };

            // find curr state in tab
            auto it_curr_state_in_tab = state_transitions_tab_.find(ptr_current_state_);
            if (it_curr_state_in_tab == state_transitions_tab_.end())
                return { false, msg_().false_curr_state_nfaund };

            // find links from curr state to new state by event in tab
            auto& mmap_ref = it_curr_state_in_tab->second; // mmap_ref - std::multimap<id_t /*event id*/, transit_to_state>
            transit_to_state_t* ptr_transit_to_state = nullptr;

            auto it_transits_by_event = mmap_ref.equal_range(ref_e.id); // transits by event may be multiple

            // find valid transit
            if (!std::any_of(it_transits_by_event.first, it_transits_by_event.second, [&](std::pair<const id_t, transit_to_state_t>& pr)
                {
                    transit_to_state_t& transit = pr.second;
                    if (!transit.ptr_transition) // if link is null, then success. link can be NULL. Guard condition will not test
                    {
                        ptr_transit_to_state = &transit;
                        return true;
                    }
                    else // check link guard condition
                    {
                        if (transit.ptr_transition->is_guard_condition(ref_e))
                        {
                            ptr_transit_to_state = &transit;
                            return true;
                        }
                    }
                    return false;
                }))
            {
                // не найдены переходы по событию
                return { true, msg_().true_reject };
            };

            if (!(ptr_transit_to_state->ptr_target_state))
                return { false, msg_().false_target_state_null };
            // - pre conditions

            /// Ok! change state now! ///

            // do unactivate source (current) state
            ptr_current_state_->do_unactivate(ref_e);

            // do transition actions
            if (ptr_transit_to_state->ptr_transition)
                ptr_transit_to_state->ptr_transition->do_action(ref_e);

            // update current state index, change current state from source to target
            ptr_current_state_ = ptr_transit_to_state->ptr_target_state;

            if (ptr_current_state_ == ptr_final_state_)
            {
                set_status_disabled();
                return { true, msg_().true_curr_state_final };
            }

            // do activate target (current) state
            ptr_current_state_->do_activate(ref_e);

            return { true, msg_().true_ok };
        }

    public:
        result_t add_link(id_t event_id, state* state_source, state* state_target, transition* trans = nullptr)
        {
            if (!state_source || !state_target)
                return { false, msg_().param_state_is_null };

            transit_to_state_t ls;
            ls.ptr_transition = trans;
            ls.ptr_target_state = state_target;
            state_transitions_tab_[state_source].insert(std::pair<id_t /*event id*/, transit_to_state_t>(event_id, ls));

            return { true, msg_().true_ok };
        }

        result_t add_link(id_t event_id, initial_state* state_source, state* state_target, transition* trans = nullptr)
        {
            if (ptr_initial_state_ && ptr_initial_state_ != state_source)
                return { false, msg_().init_state_is_already_set }; // initial state must be single!

            result_t rez = add_link(event_id, static_cast<state*>(state_source), state_target, trans);
            if (std::get<0>(rez))
                ptr_initial_state_ = state_source;

            return rez;
        }

        result_t add_link(id_t event_id, state* state_source, final_state* state_target, transition* trans = nullptr)
        {
            if (ptr_final_state_ && ptr_final_state_ != state_target) // не обязательное условие, чисто для симметрии с initial state
                return { false, msg_().final_state_is_already_set }; // final state must be single! но это не точно :)

            result_t rez = add_link(event_id, state_source, static_cast<state*>(state_target), trans);
            if (std::get<0>(rez))
                ptr_final_state_ = state_target;

            return rez;
        }

        void view_integrity()
        {
            std::cout << "state list:" << std::endl;

            for (auto& a : state_transitions_tab_)
            {
                std::cout << "\tstate " << a.first->to_string() << std::endl;
                for (auto& [event_id, transit_to_state_t] : a.second)
                {
                    std::cout << "\t\tevent ID: " << event_id;
                    if (!(transit_to_state_t.ptr_transition))
                        std::cout << " null link to ";
                    else
                        std::cout << " not null link to ";
                    std::cout << transit_to_state_t.ptr_target_state->to_string() << std::endl;
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

            if (!ptr_initial_state_)
                return { false, "Init state is null" };

            state* ptr_istate = static_cast<state*>(ptr_initial_state_);
            state* ptr_fstate = static_cast<state*>(ptr_final_state_);
            bool is_init_as_source = false;
            std::set<state*> target_states;

            for (auto& [ptr_source_state, a] : state_transitions_tab_)
            {
                if (ptr_source_state == nullptr)
                    return { false, "Source state is null" };

                if (ptr_source_state == ptr_istate)
                    is_init_as_source = true;

                if (ptr_source_state == ptr_fstate)
                    return { false, "Final state is source state" };

                for (auto& [event_id, transit_to_state_t] : a)
                {
                    if (transit_to_state_t.ptr_target_state == nullptr)
                        return { false, "Target state is null" };

                    if (transit_to_state_t.ptr_target_state == ptr_istate)
                        return { false, "Init state is target state" };

                    target_states.insert(transit_to_state_t.ptr_target_state);
                }
            }

            if (!is_init_as_source)
                return { false, "Init state haven't transitions" };

            for (auto& [ptr_source_state, a] : state_transitions_tab_)
            {
                if (ptr_source_state == ptr_istate)
                    continue;

                if (target_states.find(ptr_source_state) == target_states.end())
                    return { false, "Not linked state" };
            }

            return { true, msg_().true_ok };
        }

        // + control methods sm
        status get_status() override
        {
            return curr_status_;
        }

        void set_status_enabled() override
        {
            switch (curr_status_)
            {
            case status::enabled:
                break;
            case status::disabled:
                ptr_current_state_ = static_cast<state*>(ptr_initial_state_);
                [[fallthrough]];
            case status::paused:
                curr_status_ = status::enabled;
                break;
            }
        }

        void set_status_enabled(event&& rvl_e) override
        {
            switch (curr_status_)
            {
            case status::enabled:
                break;
            case status::disabled:
                ptr_current_state_ = static_cast<state*>(ptr_initial_state_);
                ptr_initial_state_->do_activate(rvl_e); // for debug only. init_state.do_activate is clear
                [[fallthrough]];
            case status::paused:
                curr_status_ = status::enabled;
                rise_event(std::move(rvl_e));
                break;
            }
        }
        
        void set_status_enabled(const event& ref_e) override
        {
            switch (curr_status_)
            {
            case status::enabled:
                break;
            case status::disabled:
                ptr_current_state_ = static_cast<state*>(ptr_initial_state_);
                ptr_initial_state_->do_activate(ref_e); // for debug only. init_state.do_activate is clear
                [[fallthrough]];
            case status::paused:
                curr_status_ = status::enabled;
                rise_event(ref_e);
                break;
            }
        }

        void set_status_paused() override
        {
            switch (curr_status_)
            {
            case status::enabled:
                curr_status_ = status::paused;
                break;
            case status::paused:
            case status::disabled:
                break;
            }
        }

        void set_status_disabled() override
        {
            curr_status_ = status::disabled;
        }
        // - control methods sm

        void clear() override
        {
            set_status_disabled ();
            state_transitions_tab_.clear();
        }
    };


    /// rule machine

    class rule_machine :
        public IEventHandler,
        public IMachine
    {
        inline auto msg_()
        {
            return sgt_messages <base_messages>::get_instance().msgs;
        }

    private:
        std::vector<IMachine*> inside_machines_;
        status curr_status_ = status::disabled;

    public:
        void add_machine(IMachine* ptr_m)
        {
            inside_machines_.push_back(ptr_m);
        }

    public:
        IEventHandler* get_event_handler() override
        {
            return static_cast<IEventHandler*>(this);
        }

        result_t rise_event(const event& ref_e) override
        {
            if (curr_status_ != status::enabled)
                return { true, msg_().true_ok };

            for (IMachine* m : inside_machines_)
                if (m)
                {
                    IEventHandler* h = m -> get_event_handler();
                    if (h)
                        h -> rise_event(ref_e);
                }

            return { true, msg_().true_ok };
        }

        result_t rise_event(event&& rlv_e) override
        {
            if (curr_status_ != status::enabled)
                return { true, msg_().true_ok };

            for (IMachine* m : inside_machines_)
                if (m)
                {
                    IEventHandler* h = m -> get_event_handler();
                    if (h)
                        h->rise_event(rlv_e);
                }

            return { true, msg_().true_ok };
        }

        result_t release_event() override
        {
            for (IMachine* m : inside_machines_)
                if (m)
                {
                    IEventHandler* h = m->get_event_handler();
                    if (h)
                        h->release_event();
                }

            return { true, msg_().true_ok };
        }

    public:
        // + control methods sm
        status get_status() override
        {
            return curr_status_;
        }

        void set_status_enabled() override
        {
            curr_status_ = status::enabled;

            for (IMachine* sm : inside_machines_)
                if (sm)
                    sm->set_status_enabled();
        }
        
        void set_status_enabled(event&& rvl_e) override
        {
            curr_status_ = status::enabled;

            for (IMachine* sm : inside_machines_)
                if (sm)
                    sm->set_status_enabled(rvl_e);
        }
        
        void set_status_enabled(const event& ref_e) override
        {
            curr_status_ = status::enabled;

            for (IMachine* sm : inside_machines_)
                if (sm)
                    sm->set_status_enabled(ref_e);
        }

        void set_status_paused() override
        {
            curr_status_ = status::paused;

            for (IMachine* sm : inside_machines_)
                if (sm)
                    sm->set_status_paused();
        }

        void set_status_disabled() override
        {
            curr_status_ = status::disabled;

            for (IMachine* sm : inside_machines_)
                if (sm)
                    sm->set_status_disabled();
        }
        // - control methods sm

    public:
        void clear() override
        {
            set_status_disabled();

            for (IMachine* ptr_sm : inside_machines_)
            {
                if (ptr_sm)
                    ptr_sm->clear();
            }

            inside_machines_.clear();
        }
    };
}




