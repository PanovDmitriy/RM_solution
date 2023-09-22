#pragma once

#include <string>
#include <iostream>



namespace Machines
{
    template <class T, T undefault_value> class Id;
    using ID = Id<int, -1>;

    template <class T, T undefault_value>
    class Id final
    {
    public:
        static constexpr T Undef = undefault_value; // default const value

    private:
        T value_ = Undef;

    public:
        Id() = default;

        Id(T&& val) :
            value_(val)
        {
        }

        Id(const Id<T, undefault_value>& id) :
            value_(id.value_)
        {
        }

        explicit Id(Id<T, undefault_value>&& id) noexcept :
            value_(id.value_)
        {
            id.value_ = Id::Undef;
        }

        void operator=(T id_value) = delete;
        void operator=(T&& id_value) = delete;
        void operator=(const Id<T, undefault_value>& id) = delete;
        void operator=(Id<T, undefault_value>&& id) = delete;

        bool operator==(const Id<T, undefault_value>& other) const
        {
            return value_ == other.value_;
        }
        std::weak_ordering operator<=>(const Id<T, undefault_value>& other) const
        {
            return value_ <=> other.value_;
        }

        explicit operator T() const
        { 
            return value_; 
        }

    public:
        inline T get() const
        {
            return value_;
        }

        inline void set(T& val)
        {
            value_ = val;
        }

        inline void set(T&& val)
        {
            value_ = std::move (val);
        }

        inline const std::string to_string() const
        {
            if constexpr (std::is_arithmetic_v<T>)
            {
                return std::to_string(value_);
            }
            else
            {
                return value_.to_string();
            }
        }
    };


    inline void TraceCon(auto s)
    {
#if _DEBUG 
        std::cout << s << std::endl;
#endif
    }


} // namespace Machines
