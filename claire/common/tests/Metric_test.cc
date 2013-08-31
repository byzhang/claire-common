#include <claire/common/stats/Stats.h>
#include <claire/common/logging/Logging.h>

using namespace claire;

DEFINE_METRIC(test);

int main()
{
    METRIC_test.Add(1);
    METRIC_test.Add(20);
    METRIC_test.Add(13);
    METRIC_test.Add(23213);

    LOG(INFO) << METRIC_test.min() << " " << METRIC_test.max() << " " << METRIC_test.median();
    return 0;
}
