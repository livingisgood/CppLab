#pragma once

namespace BC
{
    namespace Intrusive
    {
        template <typename T>
        class TNode
        {
            template <typename S> friend class TNodeIterator;
            template <typename S> friend class TNodeConstIterator;
            template <typename S, TNode<S> S::*> friend class TNodeList;

        public:

            explicit TNode(T* InOwner) : Owner(InOwner) {}

            ~TNode() { UnLink(); }

            TNode(const TNode& Other) = delete;
            TNode& operator=(const TNode& Other) = delete;

            bool IsLinked() const { return Prev != nullptr && Owner != nullptr; }

            void UnLink()
            {
                if(!IsLinked())
                    return;

                Prev->Next = Next;
                Next->Prev = Prev;

                Next = nullptr;
                Prev = nullptr;
            }

        private:

            T* Owner {nullptr};
            TNode* Prev {nullptr};
            TNode* Next {nullptr};
        };

        template <typename T>
        class TNodeConstIterator
        {
            template <typename S, TNode<S> S::*> friend class TNodeList;

        public:

            TNodeConstIterator(TNode<T>* Node, TNode<T>* ListHead) : NodePtr(Node), Head(ListHead)
            {
                Next = NodePtr->Next;
                Prev = NodePtr->Prev;
            }

            const T& operator*() const noexcept
            {
                return *(NodePtr->Owner);
            }

            const T* operator->() const noexcept
            {
                return NodePtr->Owner;
            }

            const T* Get() const noexcept
            {
                return NodePtr->Owner;
            }

            bool operator==(const TNodeConstIterator& Other) const noexcept
            {
                return NodePtr == Other.NodePtr && Head == Other.Head;
            }

            bool operator!=(const TNodeConstIterator& Other) const noexcept
            {
                return !(*this == Other);
            }

            TNodeConstIterator& operator++() noexcept// ++ as prefix, increment and then return self
            {
                MoveForward();
                return *this;
            }

            TNodeConstIterator operator++(int) noexcept// ++ as suffix, read self value into a copy. increment self and return the copy.
            {
                TNodeIterator Ret(*this);
                MoveForward();
                return Ret;
            }

            TNodeConstIterator& operator--() noexcept
            {
                MoveBackward();
                return *this;
            }

            TNodeConstIterator operator--(int) noexcept
            {
                TNodeConstIterator Ret(*this);
                MoveBackward();
                return Ret;
            }

            void MoveForward() noexcept
            {
                //NodePtr = NodePtr->Next;

                NodePtr = Next;
                Next = NodePtr->Next;
                Prev = NodePtr->Prev;
            }

            void MoveBackward() noexcept
            {
                //NodePtr = NodePtr->Prev;

                NodePtr = Prev;
                Next = NodePtr->Next;
                Prev = NodePtr->Prev;
            }

        protected:

            TNode<T>* Prev;
            TNode<T>* Next;
            TNode<T>* NodePtr;
            TNode<T>* Head;
        };


        template <typename T>
        class TNodeIterator : public TNodeConstIterator<T>
        {
            template <typename S, TNode<S> S::*> friend class TNodeList;

        public:

            TNodeIterator(TNode<T>* Node, TNode<T>* ListHead) : TNodeConstIterator<T>(Node, ListHead) {}

            T& operator*() const noexcept
            {
                return *(this->NodePtr->Owner);
            }

            T* operator->() const noexcept
            {
                return this->NodePtr->Owner;
            }

            T* Get() const noexcept
            {
                return this->NodePtr->Owner;
            }

            bool operator==(const TNodeIterator& Other) const noexcept
            {
                return this->NodePtr == Other.NodePtr && this->Head == Other.Head;
            }

            bool operator!=(const TNodeIterator& Other) const noexcept
            {
                return !(*this == Other);
            }

            TNodeIterator& operator++() noexcept  // ++ as prefix, increment and then return self
            {
                this->MoveForward();
                return *this;
            }

            TNodeIterator operator++(int) noexcept // ++ as suffix, read self value into a copy. increment self and return the copy.
            {
                TNodeIterator Ret(*this);
                this->MoveForward();
                return Ret;
            }
        };


        template <typename T, TNode<T> T::*LinkMember>
        class TNodeList
        {
        public:

            TNodeList()
            {
                Root.Prev = &Root;
                Root.Next = &Root;
            }

            ~TNodeList()
            {
                TNode<T>* Node = Root.Next;

                while (Node != &Root)
                {
                    TNode<T>* Temp = Node->Next;
                    Node->Next = nullptr;
                    Node->Prev = nullptr;

                    Node = Temp;
                }
            }

            TNodeIterator<T> begin() { return TNodeIterator<T>(Root.Next, &Root); }
            TNodeIterator<T> end() { return TNodeIterator<T>(&Root, &Root); }

            TNodeConstIterator<T> begin() const { return TNodeIterator<T>(Root.Next, const_cast<TNode<T>*>(&Root)); }
            TNodeConstIterator<T> end() const { return TNodeIterator<T>(const_cast<TNode<T>*>(&Root), const_cast<TNode<T>*>(&Root)); }

            T& Front() { return *(Root.Next->Owner); }
            T& Back() { return *(Root.Prev->Owner); }

            bool Empty() const { return Root.Next == &Root; }

            void Insert(TNodeIterator<T> Pos, T& UserData)
            {
                if(Pos.Head != &Root)
                    return;

                TNode<T>& Node = UserData.*LinkMember;
                Node.UnLink();

                Node.Next = Pos.NodePtr;
                Node.Prev = Pos.NodePtr->Prev;

                Node.Prev->Next = &Node;
                Node.Next->Prev = &Node;
            }

            void PushBack(T& UserData)
            {
                Insert(end(), UserData);
            }

            void PushFront(T& UserData)
            {
                Insert(begin(), UserData);
            }

            TNodeIterator<T> Erase(TNodeIterator<T> Pos)
            {
                TNode<T>* NextNode = Pos.NodePtr->Next;

                Pos.NodePtr->UnLink();
                return TNodeIterator<T>(NextNode, &Root);
            }

        private:

            TNode<T> Root {nullptr};
        };
    }
}
