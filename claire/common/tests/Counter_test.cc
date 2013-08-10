#include <claire/common/stats/Stats.h>
#include <claire/common/logging/Logging.h>

using namespace claire;

DEFINE_COUNTER(test);

int main()
{
    COUNTER_test.incr();

    LOG(INFO) << COUNTER_test.get_count();
    return 0;
}

