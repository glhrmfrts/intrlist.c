#pragma once

static int g_test_assertions;

#define TEST_ASSERT(cond, message)                                                                                     \
    if (!(cond)) {                                                                                                     \
        fprintf(stderr, "\tAssertion failed: \"%s\" - %s\n", #cond, message);                                          \
        return false;                                                                                                  \
    } else {                                                                                                           \
        g_test_assertions += 1;                                                                                        \
    }

#define TEST_COMPARE(pred, a, b, printarg, message)                                                                    \
    if (!(pred(a, b))) {                                                                                               \
        fprintf(stderr, "\tCompare failed: %s(%s, %s) - ", #pred, #a, #b);                                             \
        fprintf(stderr, "%s(", #pred);                                                                                 \
        printarg(a, stderr);                                                                                           \
        fprintf(stderr, ", ");                                                                                         \
        printarg(b, stderr);                                                                                           \
        fprintf(stderr, ") - %s\n", message);                                                                          \
        return false;                                                                                                  \
    } else {                                                                                                           \
        g_test_assertions += 1;                                                                                        \
    }

#define TEST_RUN(func)                                                                                                 \
    {                                                                                                                  \
        printf("test %s\n", #func);                                                                                    \
        bool ok = func();                                                                                              \
        printf("test %s.\n\n", ok ? "\033[1;32mOK\033[0m" : "\033[1;31mERROR\033[0m");                                 \
        if (ok) {                                                                                                      \
            oktests += 1;                                                                                              \
        } else {                                                                                                       \
            errtests += 1;                                                                                             \
        }                                                                                                              \
    }

#define TEST_BEGIN()                                                                                                   \
    size_t oktests = 0;                                                                                                \
    size_t errtests = 0

#define TEST_END()                                                                                                     \
    printf("\033[1;37mTEST RESULTS: %zu tests (%d assertions)\033[0m, \033[1;32m%zu passed\033[0m, \033[1;31m%zu "     \
           "failed\033[0m.\n",                                                                                         \
           oktests + errtests, g_test_assertions, oktests, errtests);                                                  \
    if (errtests > 0) {                                                                                                \
        return 1;                                                                                                      \
    } else {                                                                                                           \
        return 0;                                                                                                      \
    }

