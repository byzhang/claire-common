#ifndef _CLAIRE_COMMON_STATS_GAUGE_H_
#define _CLAIRE_COMMON_STATS_GAUGE_H_

#include <string>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

namespace claire {

class Gauge : boost::noncopyable
{
public:
	virtual ~Gauge() {}
	virtual std::string ToString() const = 0;
};

template<typename T>
class GaugeBindFunction : public Gauge
{
public:
	GaugeBindFunction(const boost::function<T()>& func)
		: func_(func)
	{ }

	virtual std::string ToString() const
	{
		return boost::lexical_cast<std::string>(func_());
	}

private:
	boost::function<T()> func_;
}; 

template<typename T>
class GaugeBindVariable : public Gauge
{
public:
	GaugeBindVariable(const T* v)
		: value(v)
	{ }

	virtual std::string ToString() const
	{
		return boost::lexical_cast<std::string>(*value_);
	}

private:
	const T* value_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_STATS_GAUGE_H_