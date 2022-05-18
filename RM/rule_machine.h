
#pragma once

#include <functional>
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




namespace Machines
{
constexpr bool is_event_log = false;

class Event;
class StateMachine;
class RuleMachine;
class Transition;
class State;
class InitialState;
class FinalState;
struct IEventHandler;

using EventPtr = std::shared_ptr<Event>;
using EventCPtr = std::shared_ptr<const Event>;
using EventHandlerPtr = std::shared_ptr<IEventHandler>;
using MachinePtr = std::shared_ptr<IMachine>;
using TransitionPtr = std::shared_ptr<Transition>;
using StatePtr = std::shared_ptr<State>;
using InitialStatePtr = std::shared_ptr<InitialState>;
using FinalStatePtr = std::shared_ptr<FinalState>;

using Result = std::tuple <bool, std::string>;

using ActionFn = std::function<void( EventCPtr, EventHandlerPtr )>;
using GuardFn = std::function<bool( EventCPtr, EventHandlerPtr )>;
    using ptr_guard_t = bool (*)(const Event&);

enum class Status
{
    Enabled, 
    Disabled, 
    Paused
};

struct IEventHandler
{
    virtual Result RiseEvent( EventCPtr ) = 0;
    virtual Result ReleaseEvent() = 0;
};
    };
struct IMachine
{
    virtual void SetStatus( Status, EventCPtr = {} ) = 0;
    virtual Status GetStatus() = 0;
    virtual void Erase() = 0;
    virtual EventHandlerPtr GetEventHandler() = 0;
};
        virtual void clear() = 0;
        virtual IEventHandler* get_event_handler() = 0;
class Id
{
public:
    static constexpr int Undef = -1;

protected:
    const int value_ = Undef;

public:
    Id() = default;
    //private:
    Id( int val ) :
        value_( val )
    {
    }
    //    {
    Id( const Id& id ) :
        value_( id.value_ )
    {
    }
    //    {
    explicit Id( Id&& id ) noexcept :
        value_( id.value_ )
    {
        const_cast<int&>( id.value_ ) = Id::Undef;
    }
    //    {
    void operator=( const Id& id ) noexcept
    {
        const_cast<int&>( value_ ) = id.value_;
    }
    void operator=( Id&& id ) noexcept
    {
        const_cast<int&>( value_ ) = id.value_;
        const_cast<int&>( id.value_ ) = Id::Undef;
    }

    bool operator==( const Id& other ) const
    {
        return value_ == other.value_;
    }
    std::weak_ordering operator<=>( const Id& other ) const
    {
        return value_ <=> other.value_;
    }

public:
    const std::string to_string() const
    {
        return std::to_string (value_);
    }
};


class Event final
{
public:
    const Id id;
    const std::time_t time = std::time( nullptr ); // время создания события
    const std::any param;
    //        delete ptr;
    //    }
    //};
    Event( Id id_ ) :
        id( id_ )
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(id) id: " << id.to_string() << std::endl;
        }
    }
        const std::any param;
    template<typename T>
    Event( Id id_, T&& param_ ) :
        id( id_ ), param{ std::forward<T>( param_ ) }
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(id, template T&& param) id: " << id.to_string() << std::endl;
        }
    }
                std::cout << "log: event(id) id: " << id << std::endl;
    Event( const Event& e ) :
        id( e.id ), param( e.param ), time( e.time )
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(const event&) id: " << id.to_string() << std::endl;
        }
    }
            if constexpr (is_event_log)
    Event( Event&& e ) noexcept :
        id( e.id ), time( e.time ), param( std::move( const_cast<std::any&>( e.param ) ) )
    {
        const_cast<Id&>( e.id ) = Id::Undef;
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(event&&) id: " << id.to_string() << std::endl;
        }
    }
                std::cout << "log: event(const event&) id: " << id << std::endl;
    void operator= ( const Event& ref_e ) noexcept
    {
        const_cast<Id&>( id ) = ref_e.id;
        const_cast<std::any&>( param ) = ref_e.param;
        if constexpr ( is_event_log )
        {
            std::cout << "log: op=(const event&) id: " << id.to_string() << std::endl;
        }
    }
    void operator= ( Event&& rv_e ) noexcept
    {
        const_cast<Id&>( id ) = std::move ( const_cast<Id&>( rv_e.id ) );
        const_cast<std::any&>( rv_e.param ) = std::move( const_cast<std::any&>( rv_e.param ) );
        if constexpr ( is_event_log )
        {
            std::cout << "log: op=(event&&) id: " << id.to_string() << std::endl;
        }
    }

    bool operator== ( const Event& ref_e )
    {
        return id == ref_e.id;
    }
    bool operator != ( const Event& ref_e )
    {
        return !( id == ref_e.id );
    }
        }
    ~Event()
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: ~event id: " << id.to_string() << std::endl;
        }
    };
};
        {
            return !(id == ref_e.id);
class Transition
{
protected:
    std::vector<ActionFn> actions_;
    std::vector<GuardFn> guards_;
                std::cout << "log: ~event id: " << id << std::endl;
    EventHandlerPtr eventHandler_; // for rise new events into actions
    };
public:
    Transition() = default;
    /// transition ///
    Transition( std::vector<ActionFn> actions, std::vector<GuardFn> guards, EventHandlerPtr eventHandler = {} ) :
        actions_ ( actions ), 
        guards_ ( guards ),
        eventHandler_ ( eventHandler )
    {
    }
        {
    virtual ~Transition()
    {
    }
        {
    virtual void doAction( EventCPtr event )
    {
        for ( auto action : actions_ )
        {
            action( event, eventHandler_ );
        }
    }

    virtual bool isGuardCondition( EventCPtr event )
    {
        return !std::ranges::any_of( guards_, [&event, this]( GuardFn guard )
        {
            return !( guard( event, eventHandler_ ) );
        } );
        return true;
    }
};
        std::vector <ptr_action_t> actions_;
        std::vector <ptr_guard_t> guards_;
class TransitionTest
    : public Transition
{
public:
    const std::string name_;
        }

        void do_action(const Event& ref_e) override
        {
    TransitionTest( std::string name ) :
        name_( name )
    {
    }
        void add_action(const ptr_action_t func)
    void doAction( EventCPtr event ) override
    {
        std::cout << "link " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: action" << std::endl;
    }
        void clear_actions()
    bool isGuardCondition( EventCPtr event ) override
    {
        std::cout << "link " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: guard_condition: true" << std::endl;

        bool is_guard_condition(const Event& ref_e) override
        {
            for (auto guard : guards_)
            {
                if (!(*guard)(ref_e))
class State
{
protected:
    std::vector<ActionFn> entryActions_;
    std::vector<ActionFn> exitActions_;
    std::vector<ActionFn> internalActions_;

    EventHandlerPtr eventHandler_; // for rise new events into actions

public:
    State() = default;

    State( std::vector<ActionFn> entryActions, std::vector<ActionFn> internalActions, std::vector<ActionFn> exitActions, EventHandlerPtr eventHandler = {} ) :
        entryActions_ ( entryActions ), 
        internalActions_ ( internalActions ), 
        exitActions_ ( exitActions ),
        eventHandler_( eventHandler )
    {
    }

    virtual ~State()
    {
    }

protected:
    virtual void doEntryAction_( EventCPtr event )
    {
        for ( auto action : entryActions_ )
        {
            action( event, eventHandler_ );
        }
    }
    virtual void doExitAction_( EventCPtr event )
    {
        for ( auto action : exitActions_ )
        {
            action( event, eventHandler_ );
        }
    }
    virtual void doInternalAction_( EventCPtr event )
    {
        for ( auto action : internalActions_ )
        {
            action( event, eventHandler_ );
        }
    }

public:
    virtual void doActivate( EventCPtr event )
    {
        doEntryAction_( event );
        doInternalAction_( event );
    }
    virtual void doUnactivate( EventCPtr event )
    {
        // todo: break internal action
        doExitAction_( event );
    }

public:
    virtual std::string to_string()
    {
        return std::to_string( reinterpret_cast<unsigned long long>( this ) ); // address
    }
};
        void do_action(const Event& ref_e) override
        {
            std::cout << "link " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: action" << std::endl;
        }

        bool is_guard_condition(const Event& ref_e) override
        {
            std::cout << "link " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: guard_condition: true" << std::endl;

public:
    void doActivate( EventCPtr ) override
    {
    }
    void doUnactivate( EventCPtr ) override
    {
    }
    class State
    std::string to_string() override
    {
        return "Init state";
    }
};

class FinalState final : //pseudostate
    public State
{
public:
    FinalState()
    {
    }
        virtual void do_activate(const Event& ref_e)
public:
    void doActivate( EventCPtr ) override
    {
    }
    void doUnactivate( EventCPtr ) override
    {
    }
            do_exit_action(ref_e);
    std::string to_string() override
    {
        return "Final state";
    }
};
            return std::to_string (reinterpret_cast<unsigned long long>(this)); // address is ID
        }

class StateTest
    : public State
{
protected:
    const std::string name_;

class InitialState final : //pseudostate
    public State
{
    StateTest( std::string name ) :
        name_( name )
    {
    }

protected:
    void doEntryAction_( EventCPtr event ) override
    {
        std::cout << "state " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: entry action" << std::endl;
    }
    void doExitAction_( EventCPtr event ) override
    {
        std::cout << "state " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: exit action" << std::endl;
    }
    void doInternalAction_( EventCPtr event ) override
    {
        std::cout << "state " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: internal action" << std::endl;
    }
       
    class FinalState final :
        public State
    {
    public:
        FinalState()
        {
        }

    public:
class StateMachine :
    public IEventHandler,
    public IMachine
{
    inline auto msg_()
    {
        return sgt_messages <base_messages>::get_instance().msgs;
    }
        {
private:
    std::queue<EventCPtr> eventQueue_;
    };
private:
    struct LinkToState
    {
        TransitionPtr transition;
        StatePtr targetState;
    };
    protected:
    using Table = std::map<StatePtr /*source state*/, std::multimap<Id /*event id*/, LinkToState>>;
    Table table_;
            event_handler_(event_handler_)
    StatePtr currState_;
    InitialStatePtr initialState_;
    FinalStatePtr finalState_;

    Status currStatus_ = Status::Disabled;

public:
    StateMachine()
    {
        initStatusChanges();
    }
            for (auto action : exit_actions_)
public:
    Result RiseEvent( EventCPtr event ) override // инициировать событие
    {
        eventQueue_.emplace( EventCPtr( event ) );
        return { true, msg_().true_ok };
    }
        {
public:
    EventHandlerPtr GetEventHandler() override
    {
        return EventHandlerPtr( this );
    }
        }
public:
    Result ReleaseEvent() override
    {
        while ( !eventQueue_.empty() )
        {
            EventCPtr event = eventQueue_.front();
            releaseEvent_( event );
            eventQueue_.pop();
        }
    };

    /// state_vecs ///

protected:
    Result releaseEvent_( EventCPtr event )  // реализовать событие
    {
        if ( currStatus_ != Status::Enabled )
        {
            return { true, msg_().true_nenabled };
        }

        if ( !currState_ )
        {
            return { false, msg_().false_curr_state_null };
        }
        StateTest(IEventHandler& event_handler, std::string name) :
        auto itTable = table_.find( currState_ );
        if ( itTable == table_.end() )
        {
            return { false, msg_().false_curr_state_nfaund };
        }

        // find links from curr state to new state by event in tab
        auto& mmap_ref = itTable->second;
        LinkToState* ptrLinkToState = nullptr;
            std::cout << "state " << name_ << " * event [ " << ", " << std::to_string(ref_e.id) << " ]: entry action" << std::endl;
        auto itTransitsByEvent = mmap_ref.equal_range( event->id ); // transits by event may be multiple
        void do_exit_action(const Event& ref_e) override
        // find valid transit
        if ( !std::any_of( itTransitsByEvent.first, itTransitsByEvent.second, [&]( std::pair<const Id, LinkToState>& pr )
        {
            LinkToState& transit = pr.second;
            if ( !transit.transition ) // if link is null, then success. link can be NULL. Guard condition will not test
            {
                ptrLinkToState = &transit;
                return true;
            }
            else
            {
                if ( transit.transition->isGuardCondition( event ) )
                {
                    ptrLinkToState = &transit;
                    return true;
                }
            }
            return false;
        } ) )
        {
            return { true, msg_().true_reject };
        };
        public IMachine
        if ( !( ptrLinkToState->targetState ) )
        {
            return { false, msg_().false_target_state_null };
        }
        }
        /// change state ///
        currState_->doUnactivate( event );
        std::queue<shared_event_t> event_queue_;
        if ( ptrLinkToState->transition )
        {
            ptrLinkToState->transition->doAction( event );
        }
        {
        currState_ = ptrLinkToState->targetState;
            State* ptr_target_state = nullptr;
        if ( currState_ == std::static_pointer_cast<State>( finalState_ ) )
        {
            SetStatus( Status::Disabled );
            return { true, msg_().true_curr_state_final };
        }
        State* ptr_curr_state_ = nullptr;
        currState_->doActivate( event );
        FinalState* ptr_final_state_ = nullptr;
        return { true, msg_().true_ok };
    }

public:
    Result add_link( Id& eventID, StatePtr stateSource, StatePtr stateTarget, TransitionPtr trans = {} )
    {
        if ( !stateSource || !stateTarget )
        {
            return { false, msg_().param_state_is_null };
        }
    public:
        table_[stateSource].insert( { eventID, { trans, stateTarget } } );
        {
            event_queue_.emplace(std::make_shared<Event>(ref_e));
            return { true, msg_().true_ok };
        }
    Result add_link( Id& eventID, InitialStatePtr stateSource, StatePtr stateTarget, TransitionPtr trans = {} )
    {
        if ( initialState_ && initialState_ != stateSource )
        {
            return { false, msg_().init_state_is_already_set }; // initial state must be single!
        }
            return { true, msg_().true_ok };
        auto [res, msg] = add_link( eventID, static_pointer_cast<State>( stateSource ), stateTarget, trans );
        if ( res )
        {
            initialState_ = stateSource;
        }
        IEventHandler* get_event_handler() override
        return { res, msg };
    }
        }
    Result add_link( Id& eventID, StatePtr stateSource, FinalStatePtr stateTarget, TransitionPtr trans = nullptr )
    {
        if ( finalState_ && finalState_ != stateTarget )
        {
            return { false, msg_().final_state_is_already_set }; // final state must be single! но это не точно :)
        }
            while (!event_queue_.empty())
        auto [res, msg] = add_link( eventID, stateSource, std::static_pointer_cast<State>( stateTarget ), trans );
        if ( res )
        {
            finalState_ = stateTarget;
        }
                event_queue_.pop();
        return { res, msg };
    }
        return { true, msg_().true_ok };
    void ViewIntegrity()
    {
        std::cout << "state list:" << std::endl;
        result_t release_event_(const Event& ref_e)  // реализовать событие
        for ( auto& a : table_ )
        {
            std::cout << "\tstate " << a.first->to_string() << std::endl;
            for ( auto& [eventID, linkToState] : a.second )
            {
                std::cout << "\t\tevent ID: " << eventID.to_string();
                if ( !linkToState.transition )
                {
                    std::cout << " null link to ";
                }
                else
                {
                    std::cout << " not null link to ";
                }
                std::cout << linkToState.targetState->to_string() << std::endl;
            }
        }
    }
                return { false, msg_().false_curr_state_nfaund };
    Result check_integrity()
    {
        if ( !initialState_ )
        {
            return { false, "Init state is null" };
        }

        StatePtr iState = initialState_;
        StatePtr fState = finalState_;
        bool isInitAsSource = false;
        std::set<StatePtr> targetStates;
                    if (!transit.ptr_transition) // if link is null, then success. link can be NULL. Guard condition will not test
        for ( auto& [sourceState, toState] : table_ )
        {
            if ( sourceState == nullptr )
            {
                return { false, "Source state is null" };
            }
                    else // check link guard condition
            if ( sourceState == iState )
            {
                isInitAsSource = true;
            }
                        {
            if ( sourceState == fState )
            {
                return { false, "Final state is source state" };
            }
                        }
            for ( auto& [eventID, linkToState] : toState )
            {
                if ( linkToState.targetState == nullptr )
                {
                    return { false, "Target state is null" };
                }
                // не найдены переходы по событию
                if ( linkToState.targetState == iState )
                {
                    return { false, "Init state is target state" };
                }

                targetStates.insert( linkToState.targetState );
            }
        }

        if ( !isInitAsSource )
        {
            return { false, "Init state haven't transitions" };
        }
            // do unactivate source (current) state
        for ( auto& [ptr_source_state, a] : table_ )
        {
            if ( ptr_source_state == iState )
            {
                continue;
            }
                ptr_transit_to_state->ptr_transition->do_action(ref_e);
            if ( targetStates.find( ptr_source_state ) == targetStates.end() )
            {
                return { false, "Not linked state" };
            }
        }

            if (ptr_curr_state_ == ptr_final_state_)
            {
                set_status_disabled();
    Status GetStatus() override
    {
        return currStatus_;
    }
            ptr_curr_state_->do_activate(ref_e);
private:
    std::map<std::pair<Status/*curr*/, Status /*new*/>, std::function<void( EventCPtr )>> statusChanges_;

    void initStatusChanges()
    {
        auto noChange = []( EventCPtr )
        {
        };
        auto changeAny2Disabled = [this]( EventCPtr )
        {
            currStatus_ = Status::Disabled;
        };

        statusChanges_[{Status::Enabled, Status::Enabled}] = noChange;
        statusChanges_[{Status::Paused, Status::Paused}] = noChange;
        statusChanges_[{Status::Disabled, Status::Disabled}] = noChange;
        statusChanges_[{Status::Disabled, Status::Paused}] = noChange;

        statusChanges_[{Status::Enabled, Status::Disabled}] = changeAny2Disabled;
        statusChanges_[{Status::Paused, Status::Disabled}] = changeAny2Disabled;

        statusChanges_[{Status::Enabled, Status::Paused}] = [this]( EventCPtr )
        {
            currStatus_ = Status::Paused;
        };

        statusChanges_[{Status::Paused, Status::Enabled}] = [this]( EventCPtr event )
        {
            currStatus_ = Status::Enabled;
            RiseEvent( event );
        };
                ptr_initial_state_ = state_source;
        statusChanges_[{Status::Disabled, Status::Enabled}] = [this]( EventCPtr event )
        {
            currState_ = initialState_;
            currStatus_ = Status::Enabled;
            RiseEvent( event );
        };
    }
                return { false, msg_().final_state_is_already_set }; // final state must be single! но это не точно :)
public:
    void SetStatus( Status status, EventCPtr event = {} ) override
    {
        statusChanges_[{currStatus_, status}]( event );
    }
            return rez;
    void Erase() override
    {
        SetStatus ( Status::Disabled );
        table_.clear();
    }
};
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
private:
    std::vector<MachinePtr> machines_;
    Status currStatus_ = Status::Disabled;
        {
public:
    void AddMachine( MachinePtr machine )
    {
        machines_.push_back( machine );
    }
            // в каждое состояние (кроме init state) должен существовать переход
public:
    EventHandlerPtr GetEventHandler() override
    {
        return std::make_shared<IEventHandler>( this );
    }
            State* ptr_istate = static_cast<State*>(ptr_initial_state_);
    Result RiseEvent( EventCPtr event ) override
    {
        if ( currStatus_ != Status::Enabled )
        for ( auto m : machines_ )
        {
        {
                    return { false, "Final state is source state" };

                for (auto& [event_id, transit_to_state_t] : a)
            if ( m )
            {
                auto handler = m.get()->GetEventHandler();
                if ( handler )
                        return { false, "Init state is target state" };
                    handler->RiseEvent( event );
                }
            }
        }

        return { true, msg_().true_ok };
    }

    Result ReleaseEvent() override
    {
        for ( auto m : machines_ )
        {
            if ( m )
            {
                auto handler = m.get()->GetEventHandler();
                if ( handler )
                {
                    handler->ReleaseEvent();
                }
            }
        }
        return { true, msg_().true_ok };
    }

        // + control methods sm
public:
    Status GetStatus() override
    {
        return currStatus_;
    }
        void set_status_enabled() override
    void SetStatus( Status status, EventCPtr event = {} ) override
    {
        currStatus_ = status;

        for ( auto m : machines_ )
        {
            if ( m )
            {
                m.get()->SetStatus( status, event );
            }
        }
    }
        }
public:
    void Erase() override
    {
        SetStatus( Status::Disabled);
            {
        for ( auto m : machines_ )
        {
            if ( m )
            {
                m.get()->Erase();
            }
        }
                [[fallthrough]];
            case status::paused:
                curr_status_ = status::enabled;
                rise_event(std::move(rvl_e));
                break;
            }
        }
        
        void set_status_enabled(const Event& ref_e) override
        {
            switch (curr_status_)
            {
            case status::enabled:
                break;
            case status::disabled:
                ptr_curr_state_ = static_cast<State*>(ptr_initial_state_);
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

class RuleMachine :
    public IEventHandler,
    public IMachine
{
    inline auto msg_()
    {
        return sgt_messages <base_messages>::get_instance().msgs;
    }

    private:
        std::vector<IMachine*> machines_;
        status curr_status_ = status::disabled;

    public:
        void add_machine(IMachine* ptr_m)
        {
            machines_.push_back(ptr_m);
        }

    public:
        IEventHandler* get_event_handler() override
        {
            return static_cast<IEventHandler*>(this);
        }

        result_t rise_event(const Event& ref_e) override
        {
            if (curr_status_ != status::enabled)
                return { true, msg_().true_ok };

            for (IMachine* m : machines_)
                if (m)
                {
                    IEventHandler* h = m -> get_event_handler();
                    if (h)
                        h -> rise_event(ref_e);
                }

            return { true, msg_().true_ok };
        }

        result_t rise_event(Event&& rlv_e) override
        {
            if (curr_status_ != status::enabled)
                return { true, msg_().true_ok };

            for (IMachine* m : machines_)
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
            for (IMachine* m : machines_)
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

            for (IMachine* sm : machines_)
                if (sm)
                    sm->set_status_enabled();
        }
        
        void set_status_enabled(Event&& rvl_e) override
        {
            curr_status_ = status::enabled;

            for (IMachine* sm : machines_)
                if (sm)
                    sm->set_status_enabled(rvl_e);
        }
        
        void set_status_enabled(const Event& ref_e) override
        {
            curr_status_ = status::enabled;

            for (IMachine* sm : machines_)
                if (sm)
                    sm->set_status_enabled(ref_e);
        }

        void set_status_paused() override
        {
            curr_status_ = status::paused;

            for (IMachine* sm : machines_)
                if (sm)
                    sm->set_status_paused();
        }

        void set_status_disabled() override
        {
            curr_status_ = status::disabled;

            for (IMachine* sm : machines_)
                if (sm)
                    sm->set_status_disabled();
        }
        // - control methods sm

    public:
        void clear() override
        {
            set_status_disabled();

            for (IMachine* ptr_sm : machines_)
            {
                if (ptr_sm)
                    ptr_sm->clear();
            }

        machines_.clear();
    }
};
}




