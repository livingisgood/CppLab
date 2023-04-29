#pragma once

namespace BC
{
    template <typename T>
    class FContainerInserter
    {
    public:
        virtual ~FContainerInserter() = default;
        virtual void Insert(const T& Item) = 0;
    };

    template <typename ContainerT>
    class FContainerInserterImpl : public FContainerInserter<typename ContainerT::value_type>
    {
    public:

        using ValueType = typename ContainerT::value_type;

        explicit FContainerInserterImpl(ContainerT& Container) : MyContainer(Container) {}

        FContainerInserterImpl& operator=(const ValueType& Value)
        {
            MyContainer.push_back(Value);
            return *this;
        }

        virtual void Insert(const ValueType& Item) override
        {
            MyContainer.push_back(Item);
        }

    private:

        ContainerT& MyContainer;
    };

    template<typename T>
    class FPrinter : public FContainerInserter<T>
    {
    public:

        virtual void Insert(const T& Item)
        {
            std::cout << "[" << Item << "]";
        }
    };

    template<typename T>
    class FAccumulator : public FContainerInserter<T>
    {
    public:

        virtual void Insert(const T& Item)
        {
            Sum += Item;
        }

        T GetSum() const { return Sum; }

    private:
        T Sum {};
    };
}