#pragma once

#include <vector>
#include <memory>
#include <algorithm>

namespace buffer
{

    template <typename DATA_TYPE>
    class SingletonVector
    {
        SingletonVector() = default;

    public:
        static SingletonVector &instance()
        {
            static auto singleton = std::shared_ptr<SingletonVector>(new SingletonVector);
            return (*singleton);
        }
        std::vector<DATA_TYPE> &get() { return m_vec; }
        int64_t size() { return m_vec.size(); }
        bool empty() { return m_vec.empty(); }
        void push_back(DATA_TYPE data) { m_vec.push_back(data); }
        void pop_back() { m_vec.pop_back(); }
        void pop_front() { m_vec.pop_front(); }
        template <typename COMPARATOR_FUNC>
        void sort(COMPARATOR_FUNC func) { std::sort(m_vec.begin(), m_vec.end(), func); }
        DATA_TYPE operator[](int idx) { return m_vec[idx]; }

    private:
        std::vector<DATA_TYPE> m_vec;
    };

}