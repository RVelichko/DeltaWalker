/*!
 * \brief  Обёртка для работы с таймером.
 * \author R.N.Velichko rnvelich@mts.com
 * \date   05.03.2020
 */

#pragma once

#include <sys/queue.h>
#include <sys/types.h>

#include <string>


namespace testing {
namespace internal {
// enum GTestColor { COLOR_DEFAULT, COLOR_RED, COLOR_GREEN, COLOR_YELLOW };
extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
}  // namespace internal
}  // namespace testing

namespace ut_utils {

void Gout(const std::string& s) {
    testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] ");
    testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "%s\n", s.c_str());
}
}  // namespace ut_utils
