/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2022 John Wellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#include "unit_test_framework.h"

#include "etl/expected.h"
#include "etl/type_traits.h"

#include <string>

namespace
{
  struct Value
  {
    std::string v;
  };

  struct ValueM
  {
    ValueM()
    {
    }

    ValueM(const std::string& v_) 
      : v(v_)
    {
    }

    ValueM(ValueM&&) = default;
    ValueM& operator =(ValueM&&) = default;

    ValueM(const ValueM&) = delete;
    ValueM& operator =(const ValueM&) = delete;

    std::string v;
  };

  struct Error
  {
    std::string e;
  };

  struct ErrorM
  {
    ErrorM()
    {
    }

    ErrorM(const std::string& e_)
      : e(e_)
    {
    }

    ErrorM(ErrorM&& other) = default;
    ErrorM& operator =(ErrorM&& rhs) = default;

    ErrorM(const ErrorM& other) = delete;
    ErrorM& operator =(const ErrorM& rhs) = delete;

    std::string e;
  };

  using Expected  = etl::expected<Value, Error>;
  using ExpectedV = etl::expected<void, Error>;
  using ExpectedM = etl::expected<ValueM, ErrorM>;
}

namespace
{
  SUITE(test_result)
  {
    //*************************************************************************
    TEST(test_constructor_for_result_with_value)
    {
      Value input = { "value 1" };
      Expected expected(input);

      Value output = expected.value();

      CHECK(expected.has_value());
      CHECK(output.v == input.v);
    }

    ////*************************************************************************
    //TEST(test_constructor_for_const_result_with_value)
    //{
    //  Value input = { "value 1" };
    //  const Expected expected(input);

    //  const Value& output = expected.value();

    //  CHECK(expected.has_value());
    //  CHECK(output.v == input.v);
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_moveable_result_with_value)
    //{
    //  ValueM input = { "value 1" };
    //  ExpectedM expected(etl::move(input));

    //  ValueM output = etl::move(expected.value());

    //  CHECK(expected.has_value());
    //  CHECK(output.v == std::string("value 1"));
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_result_with_error)
    //{
    //  Error input = { "error 1" };
    //  Expected expected(input);

    //  Error output = expected.error();

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == input.e);
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_const_result_with_error)
    //{
    //  Error input = { "error 1" };
    //  const Expected expected(input);

    //  const Error& output = expected.error();

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == input.e);
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_moveable_result_with_error)
    //{
    //  ErrorM input = { "error 1" };
    //  ExpectedM expected(etl::move(input));

    //  ErrorM output = etl::move(expected.error());

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == std::string("error 1"));
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_result_void_value_with_value)
    //{
    //   ExpectedV expected;

    //   CHECK(!expected.has_value());
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_const_result_void_value_with_value)
    //{
    //   const ExpectedV expected;

    //   CHECK(!expected.has_value());
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_result_void_value_with_error)
    //{
    //  Error input = { "error 1" };
    //  ExpectedV expected(input);

    //  Error output = expected.error();

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == input.e);
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_const_result_void_value_with_error)
    //{
    //  Error input = { "error 1" };
    //  const ExpectedV expected(input);

    //  const Error& output = expected.error();

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == input.e);
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_moveable_result_void_value_with_error)
    //{
    //  Error input = { "error 1" };
    //  ExpectedV expected(etl::move(input));

    //  Error output = etl::move(expected.error());

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == std::string("error 1"));
    //}

    ////*************************************************************************
    //TEST(test_constructor_for_moveable_const_result_void_value_with_error)
    //{
    //  Error input = { "error 1" };
    //  const ExpectedV expected(etl::move(input));

    //  Error output = etl::move(expected.error());

    //  CHECK(!expected.has_value());
    //  CHECK(output.e == std::string("error 1"));
    //}

    ////*************************************************************************
    //TEST(test_copy_construct_result)
    //{
    //  Value input = { "value 1" };
    //  Expected expected(input);

    //  Expected result2(expected);

    //  Value output = result2.value();

    //  CHECK(expected.has_value());
    //  CHECK(result2.has_value());
    //  CHECK(output.v == input.v);
    //}

    ////*************************************************************************
    //TEST(test_move_construct_result)
    //{
    //  Value input = { "value 1" };
    //  Expected expected(input);

    //  Expected result2(etl::move(expected));

    //  Value output = expected.value();
    //  Value output2 = result2.value();

    //  CHECK(expected.has_value());
    //  CHECK(result2.has_value());
    //  CHECK(output.v != input.v);
    //  CHECK(output2.v == input.v);
    //}

    ////*************************************************************************
    //TEST(test_move_assign_result)
    //{
    //  Value input = { "value 1" };
    //  Expected expected(input);

    //  Value input2 = { "value 2" };
    //  Expected result2(expected);

    //  result2 = etl::move(expected);

    //  Value output  = expected.value();
    //  Value output2 = result2.value();

    //  CHECK(expected.has_value());
    //  CHECK(result2.has_value());
    //  CHECK(!result2.is_error());
    //  CHECK(output.v != input.v);
    //  CHECK(output2.v == input.v);
    //}
  };
}
