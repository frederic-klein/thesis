#include "../src/SMPC_math.h"
#include "../lib/Unity/src/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_basics(void)
{
  /* All of these should pass */
//  TEST_ASSERT_EQUAL(1779, mod(-2255,2017));
  TEST_ASSERT_EQUAL(238, mod(2255,2017));
}

void test_mod(void)
{
    /* All of these should pass */
    TEST_ASSERT_EQUAL(1779, mod(-2255,2017));
    TEST_ASSERT_EQUAL(238, mod(2255,2017));
}

int main(void)
{
	UNITY_BEGIN();
//	RUN_TEST(test_basics);
    RUN_TEST(test_mod);
	return UNITY_END();
}
