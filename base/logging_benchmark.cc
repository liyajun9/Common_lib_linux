// Copyright 2011 Yunrang Inc. All Rights Reserved.
// Author: jiqinhong@yunrang.com (Jiqin Hong)

#include "base/yr.h"
#include "base/logging.h"
#include "testing/gtest/include/gtest/gtest.h"

void DisableLoggingBenchmarksWrapper(int iteration) {
  LOG_ADDITION_INFO_BUSINESS_ID_IF(12345UL, false);
  for (int i = 0;i< iteration;++i) {
    LOG(INFO) << i;
  }
}
void EnableLoggingBenchmarksWrapper(int iteration) {
  LOG_ADDITION_INFO_BUSINESS_ID(12345UL);
  for (int i = 0;i< iteration;++i) {
    LOG(INFO) << i;
  }
}

BENCHMARK(DisableLoggingBenchmarksWrapper)->ThreadRange(1, (1UL << 10));
BENCHMARK(EnableLoggingBenchmarksWrapper)->ThreadRange(1, (1UL << 10));
