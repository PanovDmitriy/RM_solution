// singleton.h

#pragma once

template <class TDerivedSingleton> class singleton;  // опережающее объ€вление


template <class TSingleton>
class singleton_destroyer
{
private:
    TSingleton* p_instance = nullptr;

public:
    ~singleton_destroyer();

    void initialize(TSingleton* p)
    {
        p_instance = p;
    }
};

template <class TDerivedSingleton>
class singleton
{
protected:
    inline static TDerivedSingleton* p_instance = nullptr;
    inline static singleton_destroyer<TDerivedSingleton> destroyer;

    singleton() {};

public:
    singleton(const singleton<TDerivedSingleton>&) = delete;
    singleton(const TDerivedSingleton&) = delete;
    singleton<TDerivedSingleton>& operator=(const singleton<TDerivedSingleton>&) = delete;
    TDerivedSingleton& operator=(const TDerivedSingleton&) = delete;
    ~singleton() {}

    friend class singleton_destroyer<TDerivedSingleton>;

    static TDerivedSingleton& get_instance()
    {
        if (!p_instance) 
        {
            p_instance = new TDerivedSingleton();
            destroyer.initialize(p_instance);
        }
        return *p_instance;
    }
};

template <class TSingleton>
singleton_destroyer<TSingleton>::~singleton_destroyer()
{
    if (p_instance)
        delete p_instance;
}

