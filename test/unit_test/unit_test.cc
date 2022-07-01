//
// Created by HominSu on 2022/7/1.
//

#include "unit_test.h"

int main(int _argc, char *_argv[]) {
  ::testing::InitGoogleTest(&_argc, _argv);

#ifdef _MSC_VER
  _CrtMemState memoryState = { 0 };
    (void)memoryState;
    _CrtMemCheckpoint(&memoryState);
    //_CrtSetBreakAlloc(X);
    //void *testWhetherMemoryLeakDetectionWorks = malloc(1);
#endif

  int ret = RUN_ALL_TESTS();

#ifdef _MSC_VER
  // Current gtest constantly leak 2 blocks at exit
    _CrtMemDumpAllObjectsSince(&memoryState);
#endif
  return ret;
}
