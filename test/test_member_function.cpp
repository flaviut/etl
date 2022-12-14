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
    //***********************************
    TestClass()
      : i(0)
      , m("Empty")
    {
      function_called = FunctionCalled::NotCalled;
    }

    //***********************************
    int TestReturnParameter(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::TestReturnParameterType;
      
      i = i_;
      m = std::move(m_);

      return i_;
    }

    //***********************************
    int TestReturnParameterConst(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::TestReturnParameterConstType;

      i = i_;
      m = std::move(m_);

      return i_;
    }

    //***********************************
    void TestParameter(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::TestParameterType;

      i = i_;
      m = std::move(m_);
    }

    //***********************************
    void TestParameterConst(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::TestParameterConstType;

      i = i_;
      m = std::move(m_);
    }

    //***********************************
    int TestReturn()
    {
      function_called = FunctionCalled::TestReturnType;

      return 0;
    }

    //***********************************
    int TestReturnConst() const
    {
      function_called = FunctionCalled::TestReturnConstType;

      return 1;
    }

    //***********************************
    void Test()
    {
      function_called = FunctionCalled::TestType;
    }

    //***********************************
    void TestConst() const
    {
      function_called = FunctionCalled::TestConstType;
    }

    mutable int i;
    mutable Moveable m;
  };

  //***************************************************************************
  static int TestFunction(int i_, Moveable&& m_)
  {
    function_called = FunctionCalled::TestReturnParameterType;
    return i_;
  }

  //***************************************************************************
  struct Functor
  {
    //***********************************
    Functor()
      : i(0)
      , m("Empty")
    {
      function_called = FunctionCalled::NotCalled;
    }

    //***********************************
    Functor(const Functor& other)
      : i(other.i)
      , m(std::move(other.m))
    {
    }

    //***********************************
    int operator()(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::TestReturnParameterType;

      return i_;
    }

    mutable int i;
    mutable Moveable m;
  };

  Functor functor;

  //***************************************************************************
  SUITE(test_member_function)
  {
    TEST(functional)
    {
      TestClass testClass;
      const TestClass testClassConst;

      std::function<int(TestClass&, int, Moveable&&)>       test1 = &TestClass::TestReturnParameter;
      std::function<int(TestClass&, int, Moveable&&)>       test2 = &TestClass::TestReturnParameterConst;
      std::function<int(const TestClass&, int, Moveable&&)> test3 = &TestClass::TestReturnParameterConst;
      std::function<int(int, Moveable&&)>                   test4 = TestFunction;
      std::function<int(int, Moveable&&)>                   test5 = functor;
      std::function<int(int, Moveable&&)>                   test6 = [](int i_, Moveable&& m_) { return i_; };

      test1(testClass, 1, Moveable("Test1"));
      test2(testClass, 2, Moveable("Test2"));
      test3(testClassConst, 3, Moveable("Test3"));
      test4(4, Moveable("Test4"));
      test5(4, Moveable("Test5"));
      test6(4, Moveable("Test6"));
    }

    //*************************************************************************
    TEST(test_return_parameter_default_construction)
    {
      TestClass testClass;
      int result;

      etl::member_function<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1, Moveable("NotCalled")), etl::member_function_uninitialised);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_return_parameter_non_member)
    {
      etl::member_function<int(int, Moveable&&)> func(TestFunction);
      int result = func(1, Moveable("TestReturnParameterType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_lambda)
    {
      auto lambda = [](int i_, Moveable&& m_) 
      { 
        function_called = FunctionCalled::TestReturnParameterType; 
        return i_; 
      };

      etl::member_function<int(int, Moveable&&)> func(lambda);
      int result = func(1, Moveable("TestReturnParameterType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_functor)
    {
      Functor functor;

      etl::member_function<int(int, Moveable&&)> func(functor);
      int result = func(1, Moveable("TestReturnParameterType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_const_functor)
    {
      const Functor functor;

      etl::member_function<int(int, Moveable&&)> func(functor);
      int result = func(1, Moveable("TestReturnParameterConstType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_non_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func(&TestClass::TestReturnParameter);
      int result = func(testClass, 1, Moveable("TestReturnParameterType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func(&TestClass::TestReturnParameterConst);
      int result = func(testClass, 1, Moveable("TestReturnParameterConstType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_parameter_default_construction_const_object)
    {
      const TestClass testClass;
      int result;

      etl::member_function<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1, Moveable("NotCalled")), etl::member_function_uninitialised);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_return_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(const TestClass&, int, Moveable&&)> func(&TestClass::TestReturnParameterConst);
      int result = func(testClass, 1, Moveable("TestReturnParameterConstType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_default_construction)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func;
      int result = func.call_if(testClass);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK(function_called == FunctionCalled::NotCalled);
      CHECK_EQUAL(int(), result);
    }

    //*************************************************************************
    TEST(test_return_non_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::TestReturn);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnType);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_return_const)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::TestReturnConst);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_return_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass&)> func;
      CHECK_THROW(int result = func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
    }

    //*************************************************************************
    TEST(test_return_non_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::TestReturn);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func(testClass);
      CHECK(function_called == FunctionCalled::TestReturnType);
      CHECK_EQUAL(0, result);
    }

    ////*************************************************************************
    TEST(test_return_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::TestReturnConst);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnConstType);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_parameter_default_construction)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func;
      CHECK_THROW(func(testClass, 1, Moveable("NotCalled")), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_parameter)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func(&TestClass::TestParameter);
      func(testClass, 1, Moveable("TestParameterConstType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestParameterType);
    }

    //*************************************************************************
    TEST(test_parameter_const)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func(&TestClass::TestParameterConst);
      func(testClass, 1, Moveable("TestParameterConstType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestParameterConstType);
    }

    //*************************************************************************
    TEST(test_parameter_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func;
      CHECK_THROW(func(testClass, 1, Moveable("NotCalled")), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func(&TestClass::TestParameterConst);
      func(testClass, 1, Moveable("TestParameterConstType"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestParameterConstType);
    }

    //*************************************************************************
    TEST(test_default_construction)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func;
      CHECK_THROW(func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_non_const)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func(&TestClass::Test);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestType);
    }

    //*************************************************************************
    TEST(test_const)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func(&TestClass::TestConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestConstType);
    }

    //*************************************************************************
    TEST(test_functor)
    {
      struct Functor
      {
        Functor()
        {
          function_called = FunctionCalled::NotCalled;
        }

        void operator()()
        {
          function_called = FunctionCalled::TestReturnParameterType;
        }
      };

      Functor functor;

      etl::member_function<void()> func(functor);
      func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestReturnParameterType);
    }

    //*************************************************************************
    TEST(test_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass&)> func;
      CHECK_THROW(func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK(function_called == FunctionCalled::NotCalled);
    }

    //*************************************************************************
    TEST(test_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(TestClass&)> func(&TestClass::TestConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK(function_called == FunctionCalled::TestConstType);
    }
  };
}
