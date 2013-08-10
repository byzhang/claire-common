#include <claire/common/stats/Stats.h>
#include <claire/common/logging/Logging.h>

using namespace claire;

DEFINE_METRIC(test);

int main()
{
    METRIC_test.Add(1.0);
    METRIC_test.Add(20.0);
    METRIC_test.Add(13.0);
    METRIC_test.Add(23213.0);

    LOG(INFO) << METRIC_test.min() << " " << METRIC_test.max() << " " << METRIC_test.median() << " "<< METRIC_test.sum();
    return 0;
}
