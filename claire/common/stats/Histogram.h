#ifndef _CLAIRE_COMMON_STATS_HISTOGRAM_H_
#define _CLAIRE_COMMON_STATS_HISTOGRAM_H_

#include <vector>

#include <boost/scoped_ptr.hpp>

namespace claire {

class HistogramImpl;

class Histogram
{
public:
    Histogram(double epsilon, int n);
    ~Histogram();

    void Add(int64_t x);
    void Clear();

    std::vector<int64_t> GetQuantiles(double* qs, size_t n) const;

    int64_t min() const;
    int64_t max() const;

private:
    boost::scoped_ptr<HistogramImpl> impl_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_STATS_HISTOGRAM_H_
