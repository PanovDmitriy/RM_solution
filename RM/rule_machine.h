
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
using MachineControllerPtr = std::shared_ptr<IMachineController>;
using TransitionPtr = std::shared_ptr<Transition>;
using StatePtr = std::shared_ptr<State>;
using InitialStatePtr = std::shared_ptr<InitialState>;
using FinalStatePtr = std::shared_ptr<FinalState>;

using Result = std::tuple <bool, std::string>;

using ActionFn = std::function<void( EventCPtr, EventHandlerPtr )>;
using GuardFn = std::function<bool( EventCPtr, EventHandlerPtr )>;


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

struct IMachineController
{
    virtual void SetStatus( Status, EventCPtr = {} ) = 0;
    virtual Status GetStatus() = 0;
    virtual void Erase() = 0;
    virtual EventHandlerPtr GetEventHandler() = 0;
};


class Id
{
public:
    static constexpr int Undef = -1;

private:
    const int value_ = Undef;

public:
    Id() = default;

    Id( int val ) :
        value_( val )
    {
    }

    Id( const Id& id ) :
        value_( id.value_ )
    {
    }

    explicit Id( Id&& id ) noexcept :
        value_( id.value_ )
    {
        const_cast<int&>( id.value_ ) = Id::Undef;
    }

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

    Event() = delete;

    Event( Id id_ ) :
        id( id_ )
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(id) id: " << id.to_string() << std::endl;
        }
    }

    template<typename T>
    Event( Id id_, T&& param_ ) :
        id( id_ ), param{ std::forward<T>( param_ ) }
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(id, template T&& param) id: " << id.to_string() << std::endl;
        }
    }

    Event( const Event& e ) :
        id( e.id ), param( e.param ), time( e.time )
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(const event&) id: " << id.to_string() << std::endl;
        }
    }

    Event( Event&& e ) noexcept :
        id( e.id ), time( e.time ), param( std::move( const_cast<std::any&>( e.param ) ) )
    {
        const_cast<Id&>( e.id ) = Id::Undef;
        if constexpr ( is_event_log )
        {
            std::cout << "log: event(event&&) id: " << id.to_string() << std::endl;
        }
    }

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

    ~Event()
    {
        if constexpr ( is_event_log )
        {
            std::cout << "log: ~event id: " << id.to_string() << std::endl;
        }
    };
};


class Transition
{
protected:
    std::vector<ActionFn> actions_;
    std::vector<GuardFn> guards_;

    EventHandlerPtr eventHandler_; // for rise new events into actions

public:
    Transition() = default;

    Transition( std::vector<ActionFn> actions, std::vector<GuardFn> guards, EventHandlerPtr eventHandler = {} ) :
        actions_ ( actions ), 
        guards_ ( guards ),
        eventHandler_ ( eventHandler )
    {
    }

    virtual ~Transition()
    {
    }

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


class TransitionTest
    : public Transition
{
public:
    const std::string name_;

public:
    TransitionTest() = delete;

    TransitionTest( std::string name ) :
        name_( name )
    {
    }

    void doAction( EventCPtr event ) override
    {
        std::cout << "link " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: action" << std::endl;
    }

    bool isGuardCondition( EventCPtr event ) override
    {
        std::cout << "link " << name_ << " * event [ " << ", " << event->id.to_string() << " ]: guard_condition: true" << std::endl;

        return true;
    }
};


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

class InitialState final : //pseudostate
    public State
{
public:
    InitialState()
    {
    }

public:
    void doActivate( EventCPtr ) override
    {
    }
    void doUnactivate( EventCPtr ) override
    {
    }

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

public:
    void doActivate( EventCPtr ) override
    {
    }
    void doUnactivate( EventCPtr ) override
    {
    }

    std::string to_string() override
    {
        return "Final state";
    }
};



class StateTest
    : public State
{
protected:
    const std::string name_;

public:
    StateTest() = delete;

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

public:
    std::string to_string() override
    {
        return name_;
    }
};



class StateMachine :
    public IEventHandler,
    public IMachineController
{
    inline auto msg_()
    {
        return sgt_messages <base_messages>::get_instance().msgs;
    }

private:
    std::queue<EventCPtr> eventQueue_;

private:
    struct TransitToState
    {
        TransitionPtr transition;
        StatePtr targetState;
    };

    using Table = std::map<StatePtr /*source state*/, std::multimap<Id /*event id*/, TransitToState>>;
    Table table_;

    StatePtr currState_;
    InitialStatePtr initialState_;
    FinalStatePtr finalState_;

    Status currStatus_ = Status::Disabled;

public:
    StateMachine()
    {
        initStatusChanges();
    }

public:
    Result RiseEvent( EventCPtr event ) override // инициировать событие
    {
        eventQueue_.emplace( EventCPtr( event ) );
        return { true, msg_().true_ok };
    }

public:
    EventHandlerPtr GetEventHandler() override
    {
        return EventHandlerPtr( this );
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

        return { true, msg_().true_ok };
    }

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

        auto itTable = table_.find( currState_ );
        if ( itTable == table_.end() )
        {
            return { false, msg_().false_curr_state_nfaund };
        }

        // find links from curr state to new state by event in tab
        auto& mmap_ref = itTable->second;
        TransitToState* ptrTransitToState = nullptr;

        auto itTransitsByEvent = mmap_ref.equal_range( event->id ); // transits by event may be multiple

        // find valid transit
        if ( !std::any_of( itTransitsByEvent.first, itTransitsByEvent.second, [&]( std::pair<const Id, TransitToState>& pr )
        {
            TransitToState& transit = pr.second;
            if ( !transit.transition ) // if link is null, then success. link can be NULL. Guard condition will not test
            {
                ptrTransitToState = &transit;
                return true;
            }
            else
            {
                if ( transit.transition->isGuardCondition( event ) )
                {
                    ptrTransitToState = &transit;
                    return true;
                }
            }
            return false;
        } ) )
        {
            return { true, msg_().true_reject };
        };

        if ( !( ptrTransitToState->targetState ) )
        {
            return { false, msg_().false_target_state_null };
        }

        /// Ok! change state now! ///
        currState_->doUnactivate( event );

        if ( ptrTransitToState->transition )
        {
            ptrTransitToState->transition->doAction( event );
        }

        currState_ = ptrTransitToState->targetState;

        if ( currState_ == std::static_pointer_cast<State>( finalState_ ) )
        {
            SetStatus( Status::Disabled );
            return { true, msg_().true_curr_state_final };
        }

        currState_->doActivate( event );

        return { true, msg_().true_ok };
    }

public:
    Result add_link( Id& eventID, StatePtr stateSource, StatePtr stateTarget, TransitionPtr trans = {} )
    {
        if ( !stateSource || !stateTarget )
        {
            return { false, msg_().param_state_is_null };
        }

        table_[stateSource].insert( { eventID, { trans, stateTarget } } );

        return { true, msg_().true_ok };
    }

    Result add_link( Id& eventID, InitialStatePtr stateSource, StatePtr stateTarget, TransitionPtr trans = {} )
    {
        if ( initialState_ && initialState_ != stateSource )
        {
            return { false, msg_().init_state_is_already_set }; // initial state must be single!
        }

        auto [res, msg] = add_link( eventID, static_pointer_cast<State>( stateSource ), stateTarget, trans );
        if ( res )
        {
            initialState_ = stateSource;
        }

        return { res, msg };
    }

    Result add_link( Id& eventID, StatePtr stateSource, FinalStatePtr stateTarget, TransitionPtr trans = nullptr )
    {
        if ( finalState_ && finalState_ != stateTarget )
        {
            return { false, msg_().final_state_is_already_set }; // final state must be single! но это не точно :)
        }

        auto [res, msg] = add_link( eventID, stateSource, std::static_pointer_cast<State>( stateTarget ), trans );
        if ( res )
        {
            finalState_ = stateTarget;
        }

        return { res, msg };
    }

    void ViewIntegrity()
    {
        std::cout << "state list:" << std::endl;

        for ( auto& a : table_ )
        {
            std::cout << "\tstate " << a.first->to_string() << std::endl;
            for ( auto& [eventID, transitToState] : a.second )
            {
                std::cout << "\t\tevent ID: " << eventID.to_string();
                if ( !transitToState.transition )
                {
                    std::cout << " null link to ";
                }
                else
                {
                    std::cout << " not null link to ";
                }
                std::cout << transitToState.targetState->to_string() << std::endl;
            }
        }
    }

    Result check_integrity()
    {
        // final не может быть исходным
        // init не может быть целевым
        // init не может быть null
        // все исходные и целевые должы быть не null
        // из init должен быть переход
        // в каждое состояние (кроме init state) должен существовать переход
        // примечение: состояние (кролме init) не обязано быть исходным

        if ( !initialState_ )
        {
            return { false, "Init state is null" };
        }

        StatePtr iState = initialState_;
        StatePtr fState = finalState_;
        bool isInitAsSource = false;
        std::set<StatePtr> targetStates;

        for ( auto& [sourceState, toState] : table_ )
        {
            if ( sourceState == nullptr )
            {
                return { false, "Source state is null" };
            }

            if ( sourceState == iState )
            {
                isInitAsSource = true;
            }

            if ( sourceState == fState )
            {
                return { false, "Final state is source state" };
            }

            for ( auto& [eventID, transitToState] : toState )
            {
                if ( transitToState.targetState == nullptr )
                {
                    return { false, "Target state is null" };
                }

                if ( transitToState.targetState == iState )
                {
                    return { false, "Init state is target state" };
                }

                targetStates.insert( transitToState.targetState );
            }
        }

        if ( !isInitAsSource )
        {
            return { false, "Init state haven't transitions" };
        }

        for ( auto& [ptr_source_state, a] : table_ )
        {
            if ( ptr_source_state == iState )
            {
                continue;
            }

            if ( targetStates.find( ptr_source_state ) == targetStates.end() )
            {
                return { false, "Not linked state" };
            }
        }

        return { true, msg_().true_ok };
    }

    // + control methods sm
    Status GetStatus() override
    {
        return currStatus_;
    }

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

        statusChanges_[{Status::Disabled, Status::Enabled}] = [this]( EventCPtr event )
        {
            currState_ = initialState_;
            currStatus_ = Status::Enabled;
            RiseEvent( event );
        };
    }

public:
    void SetStatus( Status status, EventCPtr event = {} ) override
    {
        statusChanges_[{currStatus_, status}]( event );
    }

    void Erase() override
    {
        SetStatus ( Status::Disabled );
        table_.clear();
    }
};


/// rule machine

class RuleMachine :
    public IEventHandler,
    public IMachineController
{
    inline auto msg_()
    {
        return sgt_messages <base_messages>::get_instance().msgs;
    }

private:
    std::vector<MachineControllerPtr> machines_;
    Status currStatus_ = Status::Disabled;

public:
    void add_machine( MachineControllerPtr machine )
    {
        machines_.push_back( machine );
    }

public:
    EventHandlerPtr GetEventHandler() override
    {
        return std::make_shared<IEventHandler>( this );
    }

    Result RiseEvent( EventCPtr event ) override
    {
        if ( currStatus_ != Status::Enabled )
        for ( auto m : machines_ )
        {
        {
            return { true, msg_().true_ok };
        }

            if ( m )
            {
                auto handler = m.get()->GetEventHandler();
                if ( handler )
                {
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

public:
    // + control methods sm
    Status GetStatus() override
    {
        return currStatus_;
    }

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

public:
    void Erase() override
    {
        SetStatus( Status::Disabled);

        for ( auto m : machines_ )
        {
            if ( m )
            {
                m.get()->Erase();
            }
        }

        machines_.clear();
    }
};
}




