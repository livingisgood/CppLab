#pragma once


template<typename T>
class TObjectKeeper
{
public:

    int SharedRef {0};
    T* Object {nullptr};
};


template<typename T>
class TMySharedPtr
{
public:

    explicit TMySharedPtr(T* Object)
    {
        Keeper = new TObjectKeeper<T>;
        Keeper->Object = Object;
        Keeper->SharedRef = 1;
    }

    ~TMySharedPtr()
    {
        DecreaseRef();
    }

    TMySharedPtr(const TMySharedPtr& Other)
    {
        Keeper = Other.Keeper;
        Keeper->SharedRef += 1;
    }

    TMySharedPtr& operator= (const TMySharedPtr& Other)
    {
        if(this == &Other)
            return *this;

        DecreaseRef();

        Keeper = Other.Keeper;
        Keeper->SharedRef += 1;

        return *this;
    }

    T* operator->()
    {
        if(Keeper)
            return Keeper->Object;

        return nullptr;
    }

    int GetRefCount() const
    {
        return Keeper? Keeper->SharedRef : 0;
    }

private:

    void DecreaseRef()
    {
        Keeper->SharedRef -= 1;
        if(Keeper->SharedRef == 0)
        {
            delete Keeper->Object;
            delete Keeper;
        }
    }

    TObjectKeeper<T>* Keeper {nullptr};
};