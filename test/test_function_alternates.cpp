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

#include "etl/function_alternates.h"

#include <string>

namespace
{
  //*****************************************************************************
  enum class FunctionCalled : int
  {
    UnknownType,
    TestReturnParameterType,
    TestReturnParameterConstType,
    TestParameterType,
    TestParameterConstType,
    TestReturnType,
    TestReturnConstType,
    TestType,
    TestConstType
  };

  FunctionCalled function_called = FunctionCalled::UnknownType;

  //*****************************************************************************
  struct Moveable
  {
    //*********************************
    explicit Moveable(const std::string& text_)
      : text(text_)
      , was_moved(false)
    {
    }

    //*********************************
    Moveable(Moveable&& other)
      : text(std::move(other.text))
    {
      other.was_moved = true;
    }

    //*********************************
    Moveable& operator =(Moveable&& other)
    {
      text = std::move(other.text);
      other.was_moved = true;

      return *this;
    }

    Moveable(const Moveable& other) = delete;
    Moveable& operator =(const Moveable& other) = delete;

    std::string text;
    bool was_moved;
  };

  //*****************************************************************************
  struct TestClass
  {
    TestClass()
      : i(0)
      , m("Empty")
    {
      function_called = FunctionCalled::UnknownType;
    }

    void ClearResult()
    {
      function_called = FunctionCalled::UnknownType;
    }

    int TestReturnParameter(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::TestReturnParameterType;
      
      i = i_;
      m = std::move(m_);

      return i_;
    }

    int TestReturnParameterConst(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::TestReturnParameterConstType;

      i = i_;
      m = std::move(m_);

      return i_;
    }

    void TestParameter(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::TestParameterType;

      i = i_;
      m = std::move(m_);
    }

    void TestParameterConst(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::TestParameterConstType;

      i = i_;
      m = std::move(m_);
    }

    int TestReturn()
    {
      function_called = FunctionCalled::TestReturnType;

      return 0;
    }

    int TestReturnConst() const
    {
      function_called = FunctionCalled::TestReturnConstType;

      return 1;
    }

    void Test()
    {
      function_called = FunctionCalled::TestType;
    }

    void TestConst() const
    {
      function_called = FunctionCalled::TestConstType;
    }

    mutable int i;
    mutable Moveable m;
  };

  SUITE(test_function_alternates)
  {
    //*************************************************************************
    TEST(test_return_parameter_default_construction)
    {
      TestClass testClass;

      etl::function_alternates<TestClass, int, int, Moveable> fa;
      int result = fa.call(testClass, 1, Moveable("UnknownType"), 2);
      CHECK(function_called == FunctionCalled::UnknownType);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_non_const_construction)
    {
      TestClass testClass;

      etl::function_alternates<TestClass, int, int, Moveable&&> fa(&TestClass::TestReturnParameter);
      int result = fa.call(testClass, 1, Moveable("TestReturnParameterType"), 2);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_const_construction)
    {
      TestClass testClass;

      etl::function_alternates<TestClass, int, int, Moveable&&> fa(&TestClass::TestReturnParameterConst);
      int result = fa.call(testClass, 1, Moveable("TestReturnParameterConstType"), 2);
      CHECK(function_called == FunctionCalled::TestReturnParameterConstType);
      CHECK_EQUAL(1, result);
    }
  };
}
