// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// This is a public header file, it must only include public header files.

#ifndef _CLAIRE_COMMON_EVENTLOOP_TIMERID_H_
#define _CLAIRE_COMMON_EVENTLOOP_TIMERID_H_

namespace claire {

class TimerId
{
public:
    TimerId(int64_t id = 0)
        : id_(id)
    { }

    void Reset()
    {
        id_ = 0;
    }

    bool Valid() const
    {
        return id_ > 0;
    }

    int64_t get() const
    {
        return id_;
    }

    void swap(TimerId& other)
    {
        std::swap(id_, other.id_);
    }

private:
    int64_t id_;
};

inline bool operator<(TimerId lhs, TimerId rhs)
{
    return lhs.get() < rhs.get();
}

inline bool operator==(TimerId lhs, TimerId rhs)
{
    return lhs.get() == rhs.get();
}

} // namespace claire

namespace std {
template<>
inline void swap(claire::TimerId& lhs, claire::TimerId& rhs)
{
    lhs.swap(rhs);
}

} // namespace std

#endif // _CLAIRE_COMMON_EVENTLOOP_TIMERID_H_

