// Singleton.h

#pragma once

class Singleton;  // ����������� ����������



class SingletonDestroyer
{
private:
    Singleton* p_instance = nullptr;

public:
    ~SingletonDestroyer();

    void initialize(Singleton* p)
    {
        p_instance = p;
    }
};

class Singleton
{
private:
    inline static Singleton* p_instance = nullptr;
    inline static SingletonDestroyer destroyer;

    Singleton() {};

public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    ~Singleton() {}

    friend class SingletonDestroyer;

    static Singleton& getInstance()
    {
        if (!p_instance) 
        {
            p_instance = new Singleton();
            destroyer.initialize(p_instance);
        }
        return *p_instance;
    }
};

SingletonDestroyer::~SingletonDestroyer()
{
    if (p_instance)
        delete p_instance;
}

