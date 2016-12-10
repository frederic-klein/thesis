#include "../src/SMPC_core.h"
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
  TEST_ASSERT_EQUAL(1779, mod(-2255,2017));
  //TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(1));
  //TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(33));
  //TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(999));
  //TEST_ASSERT_EQUAL(0, FindFunction_WhichIsBroken(-1));
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_basics);
	return UNITY_END();
}
