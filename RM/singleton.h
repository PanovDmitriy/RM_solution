// singleton.h

#pragma once



template <class TSingleton>
class singleton_destroyer
{
private:
    TSingleton* p_instance = nullptr;

public:
    void initialize(TSingleton* p)
    {
        p_instance = p;
    }

    ~singleton_destroyer()
    {
        if (p_instance)
            delete p_instance;
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

template <typename TData>
struct singleton_const_data :
    singleton<singleton_const_data<TData>>
{
    const TData data;

    void set(const TData& data_)
    {
        const_cast<TData&>(data) = data_;
    }
};

// // example: using diff. data sets
//
//struct const_strings
//{
//    const std::string str1;
//    const std::string str2;
//};
//
//struct const_strings_a :
//    const_strings
//{
//    const_strings_a() :
//        const_strings
//    {
//    "aaa1",
//    "aaa2"
//    }
//    {
//    }
//};
//
//struct const_strings_b :
//    const_strings
//{
//    const_strings_b() :
//        const_strings
//    {
//    "bbb1",
//    "bbb2"
//    }
//    {
//    }
//};
//
//using sgt_strings = singleton_const_data<const_strings>;
//
//main()
//{
//    sgt_strings::get_instance().set(const_strings_a());
//    std::cout << sgt_strings::get_instance().data.str1 << std::endl;
//    sgt_strings::get_instance().set(const_strings_b());
//    std::cout << sgt_strings::get_instance().data.str1 << std::endl;
//}

