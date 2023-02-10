#include "precompiled.h"
#include "utility/auto_destructor.h"

#include <boost/test/unit_test.hpp>

using namespace yaga;

BOOST_AUTO_TEST_SUITE(AutoDestructorTest)

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Empty)
{
  {
    AutoDestructor<int> testObject;
  }
  BOOST_TEST(true);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Ctor)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestructor<int> testObject(testInt, DestroyInt);
  }
  BOOST_TEST(counter == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Assign)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestructor<int> testObject;
    testObject.set(testInt, DestroyInt);
  }
  BOOST_TEST(counter == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(Reset)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestructor<int> testObject(testInt, DestroyInt);
    testObject.set();
  }
  BOOST_TEST(counter == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DoubleAssign)
{
  int counter = 0;
  int counter2 = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  auto DestroyInt2 = [&counter2](int) { ++counter2; };
  {
    int testInt = 0;
    int testInt2 = 0;
    AutoDestructor<int> testObject(testInt, DestroyInt);
    testObject.set(testInt2, DestroyInt2);
    BOOST_TEST(counter == 1);
    BOOST_TEST(counter2 == 0);
  }
  BOOST_TEST(counter == 1);
  BOOST_TEST(counter2 == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AssignCtor)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestructor<int> testObject(testInt, DestroyInt);
    AutoDestructor<int> testObject2(std::move(testObject));
    BOOST_TEST(counter == 0);
  }
  BOOST_TEST(counter == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(CopyCtor)
{
  int counter = 0;
  int counter2 = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  auto DestroyInt2 = [&counter2](int) { ++counter2; };
  {
    int testInt = 0;
    int testInt2 = 0;
    AutoDestructor<int> testObject(testInt, DestroyInt);
    AutoDestructor<int> testObject2(testInt2, DestroyInt2);
    testObject = std::move(testObject2);
    BOOST_TEST(counter == 1);
    BOOST_TEST(counter2 == 0);
  }
  BOOST_TEST(counter == 1);
  BOOST_TEST(counter2 == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(GetValue)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 169;
    AutoDestructor<int> testObject(testInt, DestroyInt);
    BOOST_TEST(*testObject == 169);
  }
  BOOST_TEST(counter == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(EmptyValue)
{
  AutoDestructor<int> testObject;
  try {
    testObject.operator*();
    BOOST_TEST(false);
  } catch (...) {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ResetEmptyValue)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  int testInt = 0;
  AutoDestructor<int> testObject(testInt, DestroyInt);
  testObject.set();
  BOOST_TEST(counter == 1);
  try {
    testObject.operator*();
    BOOST_TEST(false);
  } catch (...) {
    BOOST_TEST(true);
  }
}

BOOST_AUTO_TEST_SUITE_END() // !AutoDestructorTest
