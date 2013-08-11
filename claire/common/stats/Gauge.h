#ifndef _CLAIRE_COMMON_STATS_GAUGE_H_
#define _CLAIRE_COMMON_STATS_GAUGE_H_

#include <string>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

namespace claire {

class Gauge
{
public:
    virtual ~Gauge() {}
    virtual std::string ToString() const = 0;
    virtual std::string name() const = 0;
};

template<typename T>
class GaugeBindFunction : public Gauge
{
public:
    GaugeBindFunction(const char* nameArg, const boost::function<T()>& func)
        : name_(nameArg),
          func_(func)
    { }

    virtual std::string ToString() const
    {
        return boost::lexical_cast<std::string>(func_());
    }

    virtual std::string name() const
    {
        return name_;
    }

private:
    const std::string name_;
    boost::function<T()> func_;
};

template<typename T>
class GaugeBindVariable : public Gauge
{
public:
    GaugeBindVariable(const char* nameArg, const T* v)
        : name_(nameArg),
          value_(v)
    { }

    virtual std::string ToString() const
    {
        return boost::lexical_cast<std::string>(*value_);
    }

    virtual std::string name() const
    {
        return name_;
    }

private:
    const std::string name_;
    const T* value_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_STATS_GAUGE_H_
