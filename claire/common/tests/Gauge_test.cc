#include <boost/bind.hpp>

#include <claire/common/stats/Stats.h>
#include <claire/common/logging/Logging.h>

using namespace claire;

class X
{
public:
    X() : x_(0) {}

    int get()
    {
        return x_;
    }

    void incr()
    {
        x_++;
    }

    static int getS()
    {
        return 10;
    }
private:
    int x_;
};

int main()
{
    int x = 12;
    DEFINE_GAUGE(thread_id, &ThisThread::tid);
    DEFINE_GAUGE(test, &x);

    auto xx = new X();
    DEFINE_GAUGE(xx_test, &X::get, xx);
    xx->incr();
    xx->incr();

    DEFINE_GAUGE(xx_stats_test, &X::getS);

    auto v = GetAllGauges();
    x++;
    for (auto it = v.cbegin(); it != v.cend(); ++it)
    {
        LOG(INFO) << (*it)->name() << ", " << (*it)->ToString();
    }

    return 0;
}
