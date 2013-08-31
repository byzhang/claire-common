#include <claire/common/stats/Histogram.h>
#include <claire/common/stats/HistogramImpl.h>

using namespace claire;

Histogram::Histogram(double epsilon, int n)
    : impl_(new HistogramImpl(epsilon, n))
{ }

Histogram::~Histogram()
{ }

void Histogram::Add(int64_t x)
{
    impl_->Add(x);
}

void Histogram::Clear()
{
    impl_->Clear();
}

std::vector<int64_t> Histogram::GetQuantiles(double* qs, size_t n) const
{
    return impl_->GetQuantiles(qs, n);
}

int64_t Histogram::min() const
{
    return impl_->min();
}

int64_t Histogram::max() const
{
    return impl_->max();
}

