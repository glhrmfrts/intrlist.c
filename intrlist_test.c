#define intrlist_unittest

#include "intrlist.c"

int main(int argc, const char** argv) {
    TEST_BEGIN();
    TEST_RUN(test_listAppendRemove);
    TEST_RUN(test_listSort);
    TEST_RUN(test_listSortReverse);
    TEST_END();
}