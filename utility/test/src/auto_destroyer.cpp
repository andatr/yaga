#include "precompiled.h"

#include <boost/test/unit_test.hpp>

#include "utility/auto_destroyer.h"

using namespace yaga;

BOOST_AUTO_TEST_SUITE(AutoDestroyerTest)

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerEmpty)
{
  {
    AutoDestroyer<int> testObject;
  }
  BOOST_TEST(true);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerCtor)
{
  int counter = 0;
  auto DestroyInt = [&counter](int){ ++counter; };
  {
    int testInt = 0;
    AutoDestroyer<int> testObject(testInt, DestroyInt);
  }
  BOOST_TEST(counter == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerAssign)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestroyer<int> testObject;
    testObject.Assign(testInt, DestroyInt);
  }
  BOOST_TEST(counter == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerReset)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestroyer<int> testObject(testInt, DestroyInt);
    testObject.Reset();
  }
  BOOST_TEST(counter == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerDoubleAssign)
{
  int counter = 0;
  int counter2 = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  auto DestroyInt2 = [&counter2](int) { ++counter2; };
  {
    int testInt = 0;
    int testInt2 = 0;
    AutoDestroyer<int> testObject(testInt, DestroyInt);
    testObject.Assign(testInt2, DestroyInt2);
    BOOST_TEST(counter == 1);
    BOOST_TEST(counter2 == 0);
  }
  BOOST_TEST(counter == 1);
  BOOST_TEST(counter2 == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerAssignCtor)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 0;
    AutoDestroyer<int> testObject(testInt, DestroyInt);
    AutoDestroyer<int> testObject2(std::move(testObject));
    BOOST_TEST(counter == 0);
  }
  BOOST_TEST(counter == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerCopyCtor)
{
  int counter = 0;
  int counter2 = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  auto DestroyInt2 = [&counter2](int) { ++counter2; };
  {
    int testInt = 0;
    int testInt2 = 0;
    AutoDestroyer<int> testObject(testInt, DestroyInt);
    AutoDestroyer<int> testObject2(testInt2, DestroyInt2);
    testObject = std::move(testObject2);
    BOOST_TEST(counter == 1);
    BOOST_TEST(counter2 == 0);
  }
  BOOST_TEST(counter == 1);
  BOOST_TEST(counter2 == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerGetValue)
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  {
    int testInt = 169;
    AutoDestroyer<int> testObject(testInt, DestroyInt);
    BOOST_TEST(*testObject == 169);
  }
  BOOST_TEST(counter == 1);
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerEmptyValue)
{
  AutoDestroyer<int> testObject;
  try
  {
    testObject.operator*();
    BOOST_TEST(false);
  }
  catch (...)
  {
    BOOST_TEST(true);
  }
}

// -------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AutoDestroyerResetEmptyValue )
{
  int counter = 0;
  auto DestroyInt = [&counter](int) { ++counter; };
  int testInt = 0;
  AutoDestroyer<int> testObject(testInt, DestroyInt);
  testObject.Reset();
  BOOST_TEST(counter == 1);
  try
  {
    testObject.operator*();
    BOOST_TEST(false);
  }
  catch(...)
  {
    BOOST_TEST(true);
  }    
}

BOOST_AUTO_TEST_SUITE_END() // !AutoDestroyerTest