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

#include "etl/member_function.h"

#include <string>

#include <functional>

namespace
{
  //*****************************************************************************
  enum class FunctionCalled : int
  {
    NotCalled,
    TestReturnParameterType,
    TestReturnParameterConstType,
    TestParameterType,
    TestParameterConstType,
    TestReturnType,
    TestReturnConstType,
    TestType,
    TestConstType
  };

  FunctionCalled function_called = FunctionCalled::NotCalled;

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
      function_called = FunctionCalled::NotCalled;
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

  SUITE(test_member_function)
  {
    TEST(functional)
    {
      TestClass testClass;
      const TestClass testClassConst;

      std::function<int(TestClass&, int, Moveable&&)> test1       = &TestClass::TestReturnParameter;
      std::function<int(TestClass&, int, Moveable&&)> test2       = &TestClass::TestReturnParameterConst;
      std::function<int(const TestClass&, int, Moveable&&)> test3 = &TestClass::TestReturnParameterConst;

      test1(testClass, 1, Moveable("Test1"));
      test2(testClass, 2, Moveable("Test2"));
      test3(testClassConst, 3, Moveable("Test3"));
    }

    //*************************************************************************
    TEST(test_return_parameter_default_construction)
    {
      TestClass testClass;

      etl::member_function<int(TestClass::*)(int, Moveable&&)> fa;
      int result = fa(testClass, 1, Moveable("NotCalled"), 2);
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_non_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestReturnParameter);
      int result = fa(testClass, 1, Moveable("TestReturnParameterType"), 2);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestReturnParameterConst);
      int result = fa(testClass, 1, Moveable("TestReturnParameterConstType"), 2);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestReturnParameterConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass::*)(int, Moveable&&)> fa;
      int result = fa(testClass, 1, Moveable("NotCalled"), 2);
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_non_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestReturnParameter);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK_THROW(int result = fa(testClass, 1, Moveable("TestReturnParameterType"), 2), etl::member_function_call_to_non_const);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_return_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestReturnParameterConst);
      int result = fa(testClass, 1, Moveable("TestReturnParameterConstType"), 2);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestReturnParameterConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_default_construction)
    {
      TestClass testClass;

      etl::member_function<int(TestClass::*)(void)> fa;
      int result = fa(testClass, 2);
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_return_non_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass::*)(void)> fa(&TestClass::TestReturn);
      int result = fa(testClass, 2);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestReturnType);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_return_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass::*)(void)> fa(&TestClass::TestReturnConst);
      int result = fa(testClass, 2);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestReturnConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass::*)(void)> fa;
      int result = fa(testClass, 2);
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_return_non_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass::*)(void)> fa(&TestClass::TestReturn);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK_THROW(int result = fa(testClass, 2), etl::member_function_call_to_non_const);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_return_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass::*)(void)> fa(&TestClass::TestReturnConst);
      int result = fa(testClass, 2);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestReturnConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_parameter_default_construction)
    {
      TestClass testClass;

      etl::member_function<void(TestClass::*)(int, Moveable&&)> fa;
      fa(testClass, 1, Moveable("NotCalled"));
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_parameter)
    {
      TestClass testClass;

      etl::member_function<void(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestParameter);
      fa(testClass, 1, Moveable("TestParameterConstType"));
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestParameterType);
    }

    //*************************************************************************
    TEST(test_parameter_const)
    {
      TestClass testClass;

      etl::member_function<void(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestParameterConst);
      fa(testClass, 1, Moveable("TestParameterConstType"));
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestParameterConstType);
    }

    //*************************************************************************
    TEST(test_parameter_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass::*)(int, Moveable&&)> fa;
      fa(testClass, 1, Moveable("NotCalled"));
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_parameter_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestParameter);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK_THROW(fa(testClass, 1, Moveable("TestParameterConstType")), etl::member_function_call_to_non_const);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass::*)(int, Moveable&&)> fa(&TestClass::TestParameterConst);
      fa(testClass, 1, Moveable("TestParameterConstType"));
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestParameterConstType);
    }

    //*************************************************************************
    TEST(test_default_construction)
    {
      TestClass testClass;

      etl::member_function<void(TestClass::*)(void)> fa;
      fa(testClass);
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_non_const)
    {
      TestClass testClass;

      etl::member_function<void(TestClass::*)(void)> fa(&TestClass::Test);
      fa(testClass);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestType);
    }

    //*************************************************************************
    TEST(test_const)
    {
      TestClass testClass;

      etl::member_function<void(TestClass::*)(void)> fa(&TestClass::TestConst);
      fa(testClass);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestConstType);
    }

    //*************************************************************************
    TEST(test_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass::*)(void)> fa;
      fa(testClass);
      CHECK_FALSE(fa.is_valid());
      CHECK_FALSE(fa);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_non_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass::*)(void)> fa(&TestClass::Test);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK_THROW(fa(testClass), etl::member_function_call_to_non_const);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass::*)(void)> fa(&TestClass::TestConst);
      fa(testClass);
      CHECK_TRUE(fa.is_valid());
      CHECK_TRUE(fa);
      CHECK(function_called == FunctionCalled::TestConstType);
    }
  };
}
