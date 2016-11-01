#include <assert.h>
#include <map>
#include <limits>

template<class K, class V>
class interval_map {
    friend void IntervalMapTest();

private:
    std::map<K, V> m_map;

public:
    // constructor associates whole range of K with val by inserting (K_min, val)
    // into the map
    interval_map(V const& val)
    {
        m_map.insert(m_map.begin(), std::make_pair(std::numeric_limits<K>::lowest(), val));
    };

    // Assign value val to interval [keyBegin, keyEnd). 
    // Overwrite previous values in this interval. 
    // Do not change values outside this interval.
    // Conforming to the C++ Standard Library conventions, the interval 
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval, 
    // and assign must do nothing.
    void assign(K const& keyBegin, K const& keyEnd, const V& val)
    {
        if (!(keyBegin < keyEnd))
            return;
        // insert keyBegin
        V endValue;
        bool erase = true;
        auto begin = m_map.lower_bound(keyBegin); // insertion position for keyBegin - O(logN)
        auto eraseBegin = begin;
        if (begin != m_map.end() && begin->first == keyBegin) // keyBegin already exists, change the value
        {
            endValue = begin->second;
            begin->second = val;
            ++eraseBegin;
            if (eraseBegin == m_map.end())
                erase = false;
        }
        else // begin's key is bigger then keyBegin, so new element (with key keyBegin) should be inserted right before begin
        {
            auto prev = begin;
            --prev;
            endValue = prev->second;
            if (prev->second != val) // not redundant
            {
                auto it = m_map.insert(begin, std::make_pair(keyBegin, val)); // insert with hint - O(1)
                begin = it;
                eraseBegin = ++it;
                //if ((++it) != m_map.end() && it->second == val) // next is redundant
                //    m_map.erase(it);
            }
        }

        // insert keyEnd
        auto end = m_map.lower_bound(keyEnd); // insertion position for keyEnd - O(logN)
        auto eraseEnd = end;
        if (eraseEnd == eraseBegin)
            erase = false;
        if (end == m_map.end() || end->first != keyEnd) // end's key is bigger then keyEnd, so new element (with key keyEnd and value of end's previous iterator) should be inserted right before end
        {
            auto prev = end;
            --prev;
            if (prev != begin)
                endValue = prev->second;
            if (endValue != val) // not redundant
            {
                auto it = m_map.insert(end, std::make_pair(keyEnd, endValue)); // insert with hint - O(1)
                eraseEnd = it;
                if ((++it) != m_map.end() && it->second == endValue) // next is redundant
                    m_map.erase(it);
            }
        }
        if (erase && eraseBegin != m_map.end() && eraseEnd != m_map.end() && eraseBegin->first < eraseEnd->first)
            m_map.erase(eraseBegin, eraseEnd); // remove internal elements (intervals)
    }
    // look-up of the value associated with key
    V const& operator[](K const& key) const
    {
        return (--m_map.upper_bound(key))->second;
    }
};

#include <iostream>
#include <random>

void IntervalMapTest()
{
    interval_map<unsigned int, unsigned int> m(0);
    int A[10]{ 0 };

    int count = 1 << 20;
    while (count--)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 9);
        int begin = dis(gen);
        int end = dis(gen);
        int value = dis(gen);
        //std::cout << begin << ' ' << end << ' ' << value << std::endl;

        m.assign(begin, end, value);
        for (int i = begin; i < end; ++i)
            A[i] = value;
    }

    for (auto i = 0; i < 10; ++i)
    {
        std::cout << i << ' ' << A[i] << std::endl;
    }
    std::cout << std::endl;
    for (auto it = m.m_map.begin(); it != m.m_map.end(); ++it)
    {
        std::cout << it->first << ' ' << it->second << std::endl;
    }
}

int main(int argc, char* argv[])
{
    IntervalMapTest();
    return 0;
}
