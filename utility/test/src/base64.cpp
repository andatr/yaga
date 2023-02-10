#include "precompiled.h"
#include "utility/base64.h"

#include <boost/test/unit_test.hpp>

using namespace yaga;

BOOST_AUTO_TEST_SUITE(Base64Test)

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(EmptyEncode)
{
  BOOST_TEST(base64::encode("") == "");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(EmptyDecode)
{
  BOOST_TEST(base64::decode("") == "");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(NoPaddingEncode)
{
  BOOST_TEST(base64::encode("superstringlol1") == "c3VwZXJzdHJpbmdsb2wx");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(NoPaddingDecode)
{
  BOOST_TEST(base64::decode("c3VwZXJzdHJpbmdsb2wx") == "superstringlol1");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(OnePaddingEncode)
{
  BOOST_TEST(base64::encode("superstringlol") == "c3VwZXJzdHJpbmdsb2w=");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(OnePaddingDecode)
{
  BOOST_TEST(base64::decode("c3VwZXJzdHJpbmdsb2w=") == "superstringlol");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(TwoPaddingEncode)
{
  BOOST_TEST(base64::encode("superstringlol11") == "c3VwZXJzdHJpbmdsb2wxMQ==");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(TwoPaddingDecode)
{
  BOOST_TEST(base64::decode("c3VwZXJzdHJpbmdsb2wxMQ==") == "superstringlol11");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(BadBase64_NotMultipleOf4)
{
  try {
    base64::decode("123");
    BOOST_TEST(false);
  }
  catch (...) {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(BadBase64_BadCharater)
{
  try {
    base64::decode("^c3VwZXJzdHJpbmdsb2wx");
    BOOST_TEST(false);
  }
  catch (...) {
    BOOST_TEST(true);
  }
}

BOOST_AUTO_TEST_SUITE_END() // !Base64Test
