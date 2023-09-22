#pragma once


#include <any>
#include <cassert>
#include "base.h"


namespace Machines
{
    class Event final
    {
    public:
        const ID id;
        const std::time_t time = std::time(nullptr); // время создания события
        const std::any param;

        Event() = delete;

        Event(ID id_) :
            id(id_)
        {
            TraceCon("log: event(id) id: " + id.to_string());
        }

        template<typename T>
        Event(ID id_, T&& param_) :
            id(id_), param{ std::forward<T>(param_) }
        {
            TraceCon("log: event(id, template T&& param) id: " + id.to_string());
        }

        Event(const Event& e) :
            id(e.id), param(e.param), time(e.time)
        {
            TraceCon("log: event(const event&) id: " + id.to_string());
        }

        Event(Event&& e) noexcept :
            id(e.id), time(e.time), param(std::move(const_cast<std::any&>(e.param)))
        {
            TraceCon("log: event(event&&) id: " + id.to_string());
        }

        void operator= (const Event& ref_e) noexcept
        {
            const_cast<ID&>(id).set (ref_e.id.get());
            const_cast<std::any&>(param) = ref_e.param;

            TraceCon("log: op=(const event&) id: " + id.to_string());
        }
        void operator= (Event&& rv_e) noexcept
        {
            const_cast<ID&>(id).set (rv_e.id.get());
            const_cast<std::any&>(rv_e.param) = std::move(const_cast<std::any&>(rv_e.param));

            TraceCon("log: op=(event&&) id: " + id.to_string());
        }

        bool operator== (const Event& ref_e)
        {
            return id == ref_e.id;
        }
        bool operator != (const Event& ref_e)
        {
            return !(id == ref_e.id);
        }

        ~Event()
        {
            TraceCon("log: ~event(id) id: " + id.to_string());
        };
    };

} //namespace Machines


