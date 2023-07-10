#include "tests.h"
#include "playground.h"
#include "dynlist.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "lib/minunit/minunit.h"
#include "errors.h"
#include "utils.h"

#define mu_isok(expression) mu_check(crb_error_isOk(    expression   ))
#define mu_isfail(expression) mu_check(crb_error_isFail(    expression    ))
#define mu_assertok(expression, message) mu_assert(crb_error_isOk(    expression    ), message)
#define mu_assertfail(expression, message) mu_assert(crb_error_isFail(    expression    ), message)
/*
typedef bool (*test_fn)();
typedef struct {
    const char* name;
    test_fn function;
} testdef_t;

bool test_playground()
{
    crb_playground_t pg;
    int retcode;
    if ((retcode = crb_playground_create(5, 5, &pg)))
    {
        printf("Failed to create playground with code: %d", retcode);
        return false;
    }

    for (size_t i = 0; i < 5; i++)
    {
        for (size_t j = 0; j < 5; j++)
        {
            char32_t read, letter = 65 + rand() % 26;
            if ((retcode = crb_playground_write(&pg, j, i, letter)))
            {
                printf("Failed to write with code: %d", retcode);
                return false;
            }
            if ((retcode = crb_playground_read(&pg, j, i, &read)))
            {
                printf("Failed to write with code: %d", retcode);
                return false;
            }
            if (letter != read)
            {
                printf("Write+Read failed, values don't match: %c != %c", letter, read);
                return false;
            }
        }
    }

    return true;
}

bool test_dynlist()
{
    crb_dynlist_t list;
    crb_error_t err;
    if (crb_error_isFail((err = crb_dynlist_create(&list, 2))))
    {
        printf("Failed to create dynlist: %d - %s", err.code, err.message);
        return false;
    }
    if (list.capacity != 2)
    {
        printf("List capacity was wrong: 2 != %d", list.capacity);
        return false;
    }
    if (list.items == NULL)
    {
        printf("List items are null");
        return false;
    }
    if (list.length != 0)
    {
        printf("List length was wrong: 0 != %d", list.length);
        return false;
    }

    int q = 0;
    int f = 1;
    int l = 2;
    crb_dynlist_add(&list, (crb_dynlist_item_t) { .raw = &q, sizeof(q) });
    if (list.length != 1)
    {
        printf("List length was wrong: 1 != %d", list.length);
        return false;
    }

    crb_dynlist_add(&list, (crb_dynlist_item_t) { .raw = &f, sizeof(f) });
    crb_dynlist_insert(&list, (crb_dynlist_item_t) { .raw = &l, sizeof(l) }, 0);
    crb_dynlist_remove(&list, (crb_dynlist_item_t) { .raw = &l });
    crb_dynlist_removeAt(&list, 0);

    return true;
}


void test_tests()
{
    testdef_t tests[] = {
        { .name = "Playground", .function = test_playground },
        { .name = "Dynlist", .function = test_dynlist }
    };
    clock_t start = clock();
    srand(start);
    int total = sizeof(tests)/sizeof(*tests);
    int count = 0;
    for (size_t i = 0; i < total; i++)
    {
        printf("== %s ==\n", tests[i].name);
        if (tests[i].function())
            count++;
    }
    printf(">> Succeeded %d/%d tests in %ld clocks", count, total, clock() - start);
}
*/

MU_TEST(test_playground) {
    crb_playground_t pg;
    int retcode;
    mu_isok(crb_playground_create(5, 5, &pg));

    for (size_t i = 0; i < 5; i++)
    {
        for (size_t j = 0; j < 5; j++)
        {
            char32_t read, letter = 65 + rand() % 26;
            mu_isok(crb_playground_write(&pg, j, i, letter));
            mu_isok(crb_playground_read(&pg, j, i, &read));
            mu_assert_int_eq(letter, read);
        }
    }

    crb_playground_free(&pg);
}

MU_TEST(test_util) {
    crb_util_randseed();
    int a[4] = {0, 0, 0, 0};
    int b[4] = {0, 0, 0, 0};
    for (size_t i = 0; i < 10000; i++)
    {
        a[crb_util_randbyte() % 4]++;
        b[rand() % 4]++;
    }
    printf("\n%i, %i, %i, %i\n%i, %i, %i, %i", a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3]);
}

MU_TEST_SUITE(suite_playground) {
    MU_RUN_TEST(test_playground);
}

MU_TEST_SUITE(suite_util) {
    MU_RUN_TEST(test_util);
}

MU_TEST_SUITE(suite_dynlist) {

}

void crb_tests_run()
{
    srand(time(NULL));
    MU_RUN_SUITE(suite_playground);
    MU_RUN_SUITE(suite_util);
    MU_REPORT();
}