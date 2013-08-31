#ifndef _CLAIRE_COMMON_STATS_HISTOGRAMIMPL_H_
#define _CLAIRE_COMMON_STATS_HISTOGRAMIMPL_H_

#include <vector>

#include <boost/noncopyable.hpp>

namespace claire {

class HistogramImpl : boost::noncopyable
{
public:
    typedef std::vector<int64_t> Buffer;

    HistogramImpl(double epsilon, int n);

    void Add(int64_t x);
    void Clear();

    std::vector<int64_t> GetQuantiles(double* qs, size_t n);

    int64_t min() const
    {
        return min_;
    }

    int64_t max() const
    {
        return max_;
    }

    int64_t count() const
    {
        return count_;
    }

private:
    int Smallest(int size0, int size1, const std::vector<int>& ids);
    bool IsBufferEmpty(int level);
    int Weight(int level);

    void RecursiveCollapse(const Buffer& buf, int level);

    const int  max_depth_;
    const int buffer_size_;

    int current_top_;
    int root_weight_;
    int leaf_count_;

    int64_t min_;
    int64_t max_;
    int64_t count_;
    
    std::vector<Buffer> buffer_;
    Buffer pool_[2];

    std::vector<int> indices_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_STATS_HISTOGRAM_H_

