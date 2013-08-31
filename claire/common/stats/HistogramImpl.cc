#include <claire/common/stats/HistogramImpl.h>

#include <algorithm>

namespace claire {
namespace detail {

/**
 * We compute the "smallest possible k" satisfying two inequalities:
 *    1)   (b - 2) * (2 ^ (b - 2)) + 0.5 <= epsilon * N
 *    2)   k * (2 ^ (b - 1)) >= N
 *
 * For an explanation of these inequalities, please read the Munro-Paterson or
 * the Manku-Rajagopalan-Linday papers.
 */
int ComputeB(double epsilon, int n)
{
    int b = 2;
    while ((static_cast<double>((b - 2) * (1L << (b - 2))) + 0.5) <= epsilon * n)
    {
        b++;
    }

    return b;
}

int ComputeBufferSize(int b, int n)
{
    return static_cast<int>(n / (0x1L << (b-1)));
}

/**
 * collapse two sorted Arrays of different weight
 * ex: [2,5,7] weight 2 and [3,8,9] weight 3
 *     weight x array + concat = [2,2,5,5,7,7,3,3,3,8,8,8,9,9,9]
 *     sort = [2,2,3,3,3,5,5,7,7,8,8,8,9,9,9]
 *     select every nth elems = [3,7,9]  (n = sum weight / 2)
 */
void Collapse(const HistogramImpl::Buffer& left, int left_weigth,
              const HistogramImpl::Buffer& right, int right_weight,
              HistogramImpl::Buffer* output)
{
    int total_weigth = left_weigth + right_weight;
    int half_total_weight = (total_weigth / 2) - 1;

    int i = 0;
    int j = 0;
    int k = 0;
    int cnt = 0;

    while (static_cast<size_t>(i) < left.size() || static_cast<size_t>(j) < right.size())
    {
        int64_t smallest;
        int weight;

        if (static_cast<size_t>(i) < left.size() && (static_cast<size_t>(j) == right.size() || left[i] < right[j]))
        {
            smallest = left[i++];
            weight = left_weigth;
        }
        else
        {
            smallest = right[j++];
            weight = right_weight;
        }

        int cur = (cnt + half_total_weight) / total_weigth;
        cnt += weight;
        int next = (cnt + half_total_weight) / total_weigth;

        for (; cur < next; cur++)
        {
            output->at(k++) = smallest;
        }
    }
}

/**
 * Optimized version of collapse for colapsing two array of the same weight
 * (which is what we want most of the time)
 */
void Collapse1(const HistogramImpl::Buffer& left, const HistogramImpl::Buffer& right, HistogramImpl::Buffer* output)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int cnt = 0;

    while (static_cast<size_t>(i) < left.size() || static_cast<size_t>(j) < right.size())
    {
        int64_t smallest;
        if (static_cast<size_t>(i) < left.size() && (static_cast<size_t>(j) == right.size() || left[i] < right[j]))
        {
            smallest = left[i++];
        }
        else
        {
            smallest = right[j++];
        }

        if (cnt++ % 2 == 1)
        {
            output->at(k++) = smallest;
        }
    }
}

} // namespace claire

HistogramImpl::HistogramImpl(double epsilon, int n)
    : max_depth_(detail::ComputeB(epsilon, n)),
      buffer_size_(detail::ComputeBufferSize(max_depth_, n)),
      current_top_(1)
{
    Clear();
}

void HistogramImpl::Add(int64_t x)
{
    if (leaf_count_ == 2 * buffer_size_)
    {
        std::sort(buffer_[0].begin(), buffer_[0].end());
        std::sort(buffer_[1].begin(), buffer_[1].end());
        RecursiveCollapse(buffer_[0], 1);
        leaf_count_ = 0;
    }

    if (leaf_count_ < buffer_size_)
    {
        buffer_[0][leaf_count_] = x;
    }
    else
    {
        buffer_[1][leaf_count_ - buffer_size_] = x;
    }

    leaf_count_++;
    count_++;

    if (x > max_ || count_ == 1)
    {
        max_ = x;
    }

    if (x < min_ || count_ == 1)
    {
        min_ = x;
    }
}

void HistogramImpl::Clear()
{
    min_ = 0;
    max_ = 0;
    count_ = 0;

    leaf_count_ = 0;
    root_weight_ = 1;

    buffer_.resize(max_depth_+1);
    for (int i = 0;i < max_depth_; i++)
    {
        buffer_[i].resize(buffer_size_);
    }

    pool_[0].resize(buffer_size_);
    pool_[1].resize(buffer_size_);

    indices_.resize(buffer_size_);
}

std::vector<int64_t> HistogramImpl::GetQuantiles(double* qs, size_t n)
{
    std::vector<int64_t> output(n, 0);
    if (count_ == 0)
    {
        return output;
    }

    // the two leaves are the only buffer that can be partially filled
    int size0 = std::min(buffer_size_, leaf_count_);
    int size1 = std::max(0, leaf_count_ - size0);

    int64_t sum = 0;
    int i = 0;
    int id = 0;
    int io = 0;

    std::sort(&buffer_[0][0], &buffer_[0][size0]);
    std::sort(&buffer_[1][0], &buffer_[1][size1]);
    indices_.assign(buffer_size_, 0);

    while (static_cast<size_t>(io) < n)
    {
        i = Smallest(size0, size1, indices_);
        id = indices_[i];
        indices_[i]++;
        sum += Weight(i);
        while (static_cast<size_t>(io) < n && static_cast<int64_t>(qs[io]) * count_ <= sum)
        {
            output[io++] = buffer_[i][id];
        }
    }

    return output;
}

/**
 * Return the level of the smallest element (using the indices array 'ids'
 * to track which elements have been already returned). Every buffers has
 * already been sorted at this point.
 */
int HistogramImpl::Smallest(int size0, int size1, const std::vector<int>& ids)
{
    int64_t smallest = 0xffffffffffffffff;
    int64_t x = smallest;

    int id0 = ids[0];
    int id1 = ids[1];

    int idx = 0;

    if (0 < leaf_count_ && id0 < size0)
    {
        smallest = buffer_[0][id0];
    }

    if (buffer_size_ < leaf_count_ && id1 < size1)
    {
        x = buffer_[1][id1];
        if (x < smallest)
        {
            smallest = x;
            idx = 1;
        }
    }

    for (int i = 2;i < current_top_ + 1; i++)
    {
        if (!IsBufferEmpty(i) && ids[i] < buffer_size_)
        {
            x = buffer_[i][ids[i]];
        }

        if (x < smallest)
        {
            smallest = x;
            idx = i;
        }
    }

    return idx;
}

/**
 * Based on the number of elements inserted we can easily know if a buffer
 * is empty or not
 */
bool HistogramImpl::IsBufferEmpty(int level)
{
    if (level == current_top_)
    {
        return false; // root buffer (if present) is always full
    }
    else
    {
        return (count_ / (buffer_size_ * Weight(level))) % 2 == 1;
    }
}

/**
 * return the weight of the level ie. 2^(i-1) except for the two tree
 * leaves (weight=1) and for the root
 */
int HistogramImpl::Weight(int level)
{
    int w;
    if (level < 2)
    {
        w = 1;
    }
    else if (level == max_depth_)
    {
        w = root_weight_;
    }
    else
    {
        w = 1 << (level - 1);
    }
    return w;
}

void HistogramImpl::RecursiveCollapse(const Buffer& buf, int level)
{
    // if we reach the root, we can't add more buffer
    if (level == max_depth_)
    {
        // Weight() return the weight of the root, in that case we need the
        // weight of merge result
        auto merged_weight = 1 << (level - 1);
        auto idx = level % 2;
        auto& merged = pool_[idx];
        detail::Collapse(buf, merged_weight, buffer_[level], root_weight_, &merged);
        buffer_[level].swap(merged);
        root_weight_ += merged_weight;
    }
    else
    {
        if (level == current_top_)
        {
            detail::Collapse1(buf, buffer_[level], &buffer_[level+1]);
            current_top_++;
            root_weight_ *= 2;
        }
        else if (IsBufferEmpty(level + 1))
        {
            detail::Collapse1(buf, buffer_[level], &buffer_[level+1]);
        }
        else
        {
            auto merged = pool_[level % 2];
            detail::Collapse1(buf, buffer_[level], &merged);
            RecursiveCollapse(merged, level + 1);
        }
    }
}

} // namespace claire

