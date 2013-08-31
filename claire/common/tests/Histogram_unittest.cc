#include <gtest/gtest.h>
#include <claire/common/stats/HistogramImpl.h>

namespace claire {
namespace detail {

void Collapse(const std::vector<int64_t>&, int,
              const std::vector<int64_t>&, int,
              std::vector<int64_t>*);
}
}

namespace {

void AddToHist(claire::HistogramImpl* hist, int n)
{
    for (int i = 0;i < n; i++)
        hist->Add(i);
}

}

using namespace claire;

TEST(HistogramImpl, Collapse)
{
    std::vector<int64_t> buf1({2,5,7});
    std::vector<int64_t> buf2({3,8,9});
    std::vector<int64_t> expected({3,7,9});
    std::vector<int64_t> result(3, 0);
    // [2,5,7] weight 2 and [3,8,9] weight 3
    // weight x array + concat = [2,2,5,5,7,7,3,3,3,8,8,8,9,9,9]
    // sort = [2,2,3,3,3,5,5,7,7,8,8,8,9,9,9]
    // select every nth elems = [3,7,9]  (n = sum weight / 2, ie. 5/3 = 2)
    // [2,2,3,3,3,5,5,7,7,8,8,8,9,9,9]
    //  . . ^ . . . . ^ . . . . ^ . .
    //  [-------] [-------] [-------] we make 3 packets of 5 elements and take the middle

    detail::Collapse(buf1, 2, buf2, 3, &result);
    EXPECT_EQ(memcmp(&result[0], &expected[0], sizeof(result[0]) * result.size()), 0);

    std::vector<int64_t> buf3({2, 5, 7, 9});
    std::vector<int64_t> buf4({3, 8, 9, 12});
    std::vector<int64_t> expected2({3, 7, 9, 12});
    std::vector<int64_t> result2(4, 0);
    detail::Collapse(buf3, 2, buf4, 2, &result2);
    EXPECT_EQ(memcmp(&expected2[0], &result2[0], sizeof(result2[0]) * result2.size()), 0);
}
