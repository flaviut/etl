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

#include "etl/function_wrapper.h"

#include <string>

#include <functional>

namespace
{
  //*****************************************************************************
  enum class FunctionCalled : int
  {
    Not_Called,
    Member_Return_Parameter_Called,
    Member_Return_Parameter_Alternate_Called,
    Member_Return_Parameter_Const_Called,
    Member_Parameter_Called,
    Member_Parameter_Alternate_Called,
    Member_Parameter_Const_Called,
    Member_Return_Called,
    Member_Return_Alternate_Called,
    Member_Return_Const_Called,
    Member_Called,
    Member_Alternate_Called,
    Member_Const_Called,
    Functor_Return_Parameter_Called,
    Functor_Return_Parameter_Alternate_Called,
    Functor_Return_Parameter_Const_Called,
    Functor_Return_Called,
    Functor_Return_Const_Called,
    Lambda_Return_Parameter_Called,
    Lambda_Return_Parameter_Alternate_Called,
    Lambda_Return_Called,
    Static_Return_Parameter_Called,
    Static_Return_Parameter_Alternate_Called,
    Static_Parameter_Called,
    Static_Parameter_Alternate_Called,
    Static_Return_Called,
    Static_Return_Alternate_Called,
    Static_Called,
    Static_Alternate_Called
  };

  FunctionCalled function_called = FunctionCalled::Not_Called;

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

  //***************************************************************************
  static int StaticReturnParameterFunction(int i_, Moveable&& m_)
  {
    function_called = FunctionCalled::Static_Return_Parameter_Called;
    return i_;
  }

  //***************************************************************************
  static int StaticReturnParameterFunctionAlternate(int i_, Moveable&& m_)
  {
    function_called = FunctionCalled::Static_Return_Parameter_Alternate_Called;
    return i_;
  }

  //***************************************************************************
  static void StaticParameterFunction(int i_, Moveable&& m_)
  {
    function_called = FunctionCalled::Static_Parameter_Called;
  }

  //***************************************************************************
  static int StaticReturnFunction()
  {
    function_called = FunctionCalled::Static_Return_Called;
    return 1;
  }

  //***************************************************************************
  static void StaticFunction()
  {
    function_called = FunctionCalled::Static_Called;
  }

  //***************************************************************************
  static constexpr int StaticReturnParameterFunctionConstexpr(int i_)
  {
    return i_;
  }

  //***************************************************************************
  static constexpr void StaticParameterFunctionConstexpr(int i_)
  {
  }

  //***************************************************************************
  static constexpr int StaticReturnFunctionConstexpr()
  {
    return 1;
  }

  //***************************************************************************
  static constexpr void StaticFunctionConstexpr()
  {
  }

  //*****************************************************************************
  struct TestClass
  {
    //***********************************
    TestClass()
      : i(0)
      , m("Empty")
    {
      function_called = FunctionCalled::Not_Called;
    }

    //***********************************
    int MemberReturnParameter(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::Member_Return_Parameter_Called;
      
      i = i_;
      m = std::move(m_);

      return i_;
    }

    //***********************************
    int MemberReturnParameterAlternate(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::Member_Return_Parameter_Alternate_Called;

      return i_;
    }

    //***********************************
    int MemberReturnParameterConst(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::Member_Return_Parameter_Const_Called;

      i = i_;
      m = std::move(m_);

      return i_;
    }

    //***********************************
    void MemberParameter(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::Member_Parameter_Called;

      i = i_;
      m = std::move(m_);
    }

    //***********************************
    void MemberParameterAlternate(int i_, Moveable&& m_)
    {
      function_called = FunctionCalled::Member_Parameter_Alternate_Called;
    }

    //***********************************
    void MemberParameterConst(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::Member_Parameter_Const_Called;

      i = i_;
      m = std::move(m_);
    }

    //***********************************
    int MemberReturn()
    {
      function_called = FunctionCalled::Member_Return_Called;

      return 0;
    }

    //***********************************
    int MemberReturnAlternate()
    {
      function_called = FunctionCalled::Member_Return_Alternate_Called;

      return 1;
    }

    //***********************************
    int MemberReturnConst() const
    {
      function_called = FunctionCalled::Member_Return_Const_Called;

      return 1;
    }

    //***********************************
    void Member()
    {
      function_called = FunctionCalled::Member_Called;
    }

    //***********************************
    void MemberAlternate()
    {
      function_called = FunctionCalled::Member_Alternate_Called;
    }

    //***********************************
    void MemberConst() const
    {
      function_called = FunctionCalled::Member_Const_Called;
    }

    mutable int i;
    mutable Moveable m;
  };

  //*****************************************************************************
  struct TestClassConstexpr
  {
    //***********************************
    constexpr TestClassConstexpr()
      : i(0)
    {
    }

    //***********************************
    constexpr int MemberReturnParameter(int i_)
    {
      return i_;
    }

    //***********************************
    constexpr int MemberReturnParameterAlternate(int i_)
    {
      return i_;
    }

    //***********************************
    constexpr int MemberReturnParameterConst(int i_) const
    {
      return i_;
    }

    //***********************************
    constexpr void MemberParameter(int i_)
    {
    }

    //***********************************
    constexpr void MemberParameterAlternate(int i_)
    {
    }

    //***********************************
    constexpr void MemberParameterConst(int i_) const
    {
    }

    //***********************************
    constexpr int MemberReturn()
    {
      return 0;
    }

    //***********************************
    constexpr int MemberReturnAlternate()
    {
      return 1;
    }

    //***********************************
    constexpr int MemberReturnConst() const
    {
      return 1;
    }

    //***********************************
    constexpr void Member()
    {
    }

    //***********************************
    constexpr void MemberAlternate()
    {
    }

    //***********************************
    constexpr void MemberConst() const
    {
    }

    mutable int i;
  };

  //***************************************************************************
  struct Functor
  {
    //***********************************
    Functor()
      : i(0)
      , m("Empty")
    {
      function_called = FunctionCalled::Not_Called;
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
      function_called = FunctionCalled::Functor_Return_Parameter_Called;

      return i_;
    }

    //***********************************
    int operator()(int i_, Moveable&& m_) const
    {
      function_called = FunctionCalled::Functor_Return_Parameter_Const_Called;

      return i_;
    }

    //***********************************
    int operator()()
    {
      function_called = FunctionCalled::Functor_Return_Called;

      return 0;
    }

    //***********************************
    int operator()() const
    {
      function_called = FunctionCalled::Functor_Return_Const_Called;

      return 1;
    }

    mutable int i;
    mutable Moveable m;
  };

  //***************************************************************************
  struct FunctorConstexpr
  {
    //***********************************
    constexpr FunctorConstexpr()
      : i(0)
    {
    }

    //***********************************
    constexpr FunctorConstexpr(const Functor& other)
      : i(other.i)
    {
    }

    //***********************************
    constexpr int operator()(int i_)
    {
      return i_;
    }

    //***********************************
    constexpr int operator()(int i_) const
    {
      return i_;
    }

    //***********************************
    constexpr int operator()()
    {
      return 0;
    }

    //***********************************
    constexpr int operator()() const
    {
      return 1;
    }

    mutable int i;
  };

  //***************************************************************************
  SUITE(test_member_function)
  {
    //*************************************************************************
    // Test Static Functions
    //*************************************************************************
    TEST(test_static_return_parameter)
    {
      etl::function_wrapper<int(int, Moveable&&)> func(StaticReturnParameterFunction);
      int result = func(1, Moveable("Static_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_static_return_parameter_constexpr)
    {
      constexpr etl::function_wrapper<int(int)> func(StaticReturnParameterFunctionConstexpr);
      constexpr int result = func(1);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_static_parameter)
    {
      etl::function_wrapper<void(int, Moveable&&)> func(StaticParameterFunction);
      func(1, Moveable("Static_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Static_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_static_parameter_constexpr)
    {
      constexpr etl::function_wrapper<void(int)> func(StaticParameterFunctionConstexpr);
      func(1);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
    }

    //*************************************************************************
    TEST(test_static_return)
    {
      etl::function_wrapper<int()> func(StaticReturnFunction);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_static_return_constexpr)
    {
      constexpr etl::function_wrapper<int()> func(StaticReturnFunctionConstexpr);
      constexpr int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_static)
    {
      etl::function_wrapper<void()> func(StaticFunction);
      func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
    }

    //*************************************************************************
    TEST(test_static_constexpr)
    {
      constexpr etl::function_wrapper<void()> func(StaticFunctionConstexpr);
      func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
    }

    //*************************************************************************
    TEST(test_static_return_parameter_assignment)
    {
      etl::function_wrapper<int(int, Moveable&&)> func1 = StaticReturnParameterFunction;
      etl::function_wrapper<int(int, Moveable&&)> func2 = StaticReturnParameterFunctionAlternate;

      func2 = func1;

      int result;

      result = func1(1, Moveable("Static_Return_Parameter_Called"));
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
      CHECK_EQUAL(1, result);

      result = func2(2, Moveable("Static_Return_Parameter_Called"));
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_static_return_parameter_equality)
    {
      etl::function_wrapper<int(int, Moveable&&)> func1;
      etl::function_wrapper<int(int, Moveable&&)> func2;
      etl::function_wrapper<int(int, Moveable&&)> func3(StaticReturnParameterFunction);
      etl::function_wrapper<int(int, Moveable&&)> func4(StaticReturnParameterFunction);
      etl::function_wrapper<int(int, Moveable&&)> func5(StaticReturnParameterFunctionAlternate);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }

    //*************************************************************************
    // Test Lambda Functions
    //*************************************************************************
    TEST(test_lambda_return_parameter)
    {
      auto lambda = [](int i_, Moveable&& m_) 
      { 
        function_called = FunctionCalled::Lambda_Return_Parameter_Called;
        return i_; 
      };

      etl::function_wrapper<int(int, Moveable&&)> func(lambda);
      int result = func(1, Moveable("Lambda_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Lambda_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_lambda_return_parameter_constexpr)
    {
      static constexpr auto lambda = [](int i_)
      {
        return i_;
      };

      using LambdaType = decltype(lambda);

      constexpr etl::function_wrapper<int(LambdaType&, int)> func(&LambdaType::operator());
      constexpr int result = func(lambda, 1);

      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_lambda)
    {
      auto lambda = []()
      {
        function_called = FunctionCalled::Lambda_Return_Called;
        return 1;
      };

      etl::function_wrapper<int()> func(lambda);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Lambda_Return_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_lambda_constexpr)
    {
      static constexpr auto lambda = []()
      {
        return 1;
      };

      using LambdaType = decltype(lambda);

      constexpr etl::function_wrapper<int(LambdaType&)> func(&LambdaType::operator());
      constexpr int result = func(lambda);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_lambda_return_parameter_assignment)
    {
      auto lambda1 = [](int i_, Moveable&& m_)
      {
        function_called = FunctionCalled::Lambda_Return_Parameter_Called;
        return i_;
      };

      auto lambda2 = [](int i_, Moveable&& m_)
      {
        function_called = FunctionCalled::Lambda_Return_Parameter_Alternate_Called;
        return i_;
      };

      etl::function_wrapper<int(int, Moveable&&)> func1 = lambda1;
      etl::function_wrapper<int(int, Moveable&&)> func2 = lambda2;

      func2 = func1;

      int result;

      result = func1(1, Moveable("Lambda_Return_Parameter_Called"));
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Lambda_Return_Parameter_Called);
      CHECK_EQUAL(1, result);

      result = func2(2, Moveable("Lambda_Return_Parameter_Called"));
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Lambda_Return_Parameter_Called);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_lambda_return_parameter_equality)
    {
      auto lambda1 = [](int i_, Moveable&& m_)
      {
        return i_;
      };

      auto lambda2 = [](int i_, Moveable&& m_)
      {
        return i_;
      };

      etl::function_wrapper<int(int, Moveable&&)> func1;
      etl::function_wrapper<int(int, Moveable&&)> func2;
      etl::function_wrapper<int(int, Moveable&&)> func3(lambda1);
      etl::function_wrapper<int(int, Moveable&&)> func4(lambda1);
      etl::function_wrapper<int(int, Moveable&&)> func5(lambda2);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }

    //*************************************************************************
    // Test Functor Functions
    //*************************************************************************
    TEST(test_functor_return_parameter)
    {
      Functor functor;

      etl::function_wrapper<int(int, Moveable&&)> func(functor);
      int result = func(1, Moveable("Functor_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_functor_return_parameter_constexpr)
    {
      constexpr FunctorConstexpr functor;

      constexpr etl::function_wrapper<int(const FunctorConstexpr&, int)> func(&FunctorConstexpr::operator());
      constexpr int result = func(functor, 1);

      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_functor_return_parameter_const)
    {
      const Functor functor;

      etl::function_wrapper<int(int, Moveable&&)> func(functor);
      int result = func(1, Moveable("Functor_Return_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_functor_return)
    {
      Functor functor;

      etl::function_wrapper<int()> func(functor);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Called);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_functor_return_constexpr)
    {
      static constexpr FunctorConstexpr functor;

      constexpr etl::function_wrapper<int()> func(functor);
      /*constexpr*/ int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_functor_return_const)
    {
      const Functor functor;

      etl::function_wrapper<int()> func(functor);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_functor_return_parameter_assignment)
    {
      Functor functor1;
      Functor functor2;

      etl::function_wrapper<int(int, Moveable&&)> func1 = functor1;
      etl::function_wrapper<int(int, Moveable&&)> func2 = functor2;

      func2 = func1;

      int result;

      result = func1(1, Moveable("Functor_Return_Parameter_Called"));
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Parameter_Called);
      CHECK_EQUAL(1, result);

      result = func2(2, Moveable("Functor_Return_Parameter_Called"));
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Parameter_Called);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_functor_return_parameter_equality)
    {
      Functor functor1;
      Functor functor2;

      etl::function_wrapper<int(int, Moveable&&)> func1;
      etl::function_wrapper<int(int, Moveable&&)> func2;
      etl::function_wrapper<int(int, Moveable&&)> func3(functor1);
      etl::function_wrapper<int(int, Moveable&&)> func4(functor1);
      etl::function_wrapper<int(int, Moveable&&)> func5(functor2);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }

    //*************************************************************************
    // Test Member Return Parameter Function
    //*************************************************************************
    TEST(test_member_return_parameter_default_construction)
    {
      TestClass testClass;
      int result;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_constexpr)
    {
      constexpr TestClassConstexpr testClass;
      int result;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&, int)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1), etl::member_function_uninitialised);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      etl::optional<int> result = func.call_if(testClass, 1, Moveable("Not_Called"));
      CHECK_FALSE(result.has_value());
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_call_if_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&, int)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      constexpr etl::optional<int> result = func.call_if(testClass, 1);
      CHECK_FALSE(result.has_value());
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Static_Return_Parameter_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_call_or_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&, int)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      constexpr int result = func.call_or(StaticReturnParameterFunctionConstexpr, testClass, 1);
    }

    //*************************************************************************
    TEST(test_member_return_parameter)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameter);
      int result = func(testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&, int)> func(&TestClassConstexpr::MemberReturnParameterConst);
      constexpr int result = func(testClass, 1);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameter);
      etl::optional<int> result = func.call_if(testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_call_if_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&, int)> func(&TestClassConstexpr::MemberReturnParameterConst);
      constexpr etl::optional<int> result = func.call_if(testClass, 1);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameter);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_call_or_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&, int)> func(&TestClassConstexpr::MemberReturnParameterConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      constexpr int result = func.call_or(StaticReturnParameterFunctionConstexpr, testClass, 1);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
      int result = func(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const_assignment)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func1(&TestClass::MemberReturnParameterConst);
      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func2;

      func2 = func1;

      int result;

      result = func1(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);

      result = func2(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
      etl::optional<int> result = func.call_if(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_const_object)
    {
      const TestClass testClass;
      int result;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
      int result = func(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_assignment)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func1 = &TestClass::MemberReturnParameter;
      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func2 = &TestClass::MemberReturnParameterAlternate;

      func2 = func1;

      int result;

      result = func1(testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
      CHECK_EQUAL(1, result);

      result = func2(testClass, 2, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
      CHECK_EQUAL(2, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_equality)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func1;
      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func2;
      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func3(&TestClass::MemberReturnParameter);
      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func4(&TestClass::MemberReturnParameter);
      etl::function_wrapper<int(TestClass&, int, Moveable&&)> func5(&TestClass::MemberReturnParameterAlternate);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }

    //*************************************************************************
    // Test Member Return Function
    //*************************************************************************
    TEST(test_member_return_default_construction)
    {
      TestClass testClass;
      int result;

      etl::function_wrapper<int(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass), etl::member_function_uninitialised);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      etl::optional<int> result = func.call_if(testClass);
      CHECK_FALSE(result.has_value());
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Called);
    }

    //*************************************************************************
    TEST(test_member_return)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func(&TestClass::MemberReturn);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_member_return_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&)> func(&TestClassConstexpr::MemberReturnConst);
      constexpr int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func(&TestClass::MemberReturn);
      etl::optional<int> result = func.call_if(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_member_return_call_if_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&)> func(&TestClassConstexpr::MemberReturnConst);
      constexpr etl::optional<int> result = func.call_if(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func(&TestClass::MemberReturn);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
    }

    //*************************************************************************
    TEST(test_member_return_call_or_constexpr)
    {
      constexpr TestClassConstexpr testClass;

      constexpr etl::function_wrapper<int(const TestClassConstexpr&)> func(&TestClassConstexpr::MemberReturnConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      constexpr int result = func.call_or(StaticReturnFunctionConstexpr, testClass);
    }

    //*************************************************************************
    TEST(test_member_return_const)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_const_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      etl::optional<int> result = func.call_if(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_const_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func;
      CHECK_THROW(int result = func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_const_object_call_if)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      etl::optional<int> result = func.call_if(testClass);
      CHECK_FALSE(result.has_value());
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_const_object_call_or)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Called);
    }

    //*************************************************************************
    TEST(test_member_return_const_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_const_const_object_call_if)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      etl::optional<int> result = func.call_if(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_const_const_object_call_or)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_return_assignment)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func1 = &TestClass::MemberReturn;
      etl::function_wrapper<int(TestClass&)> func2 = &TestClass::MemberReturnAlternate;

      func2 = func1;

      int result;

      result = func1(testClass);
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
      CHECK_EQUAL(0, result);

      result = func2(testClass);
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_member_return_equality)
    {
      TestClass testClass;

      etl::function_wrapper<int(TestClass&)> func1;
      etl::function_wrapper<int(TestClass&)> func2;
      etl::function_wrapper<int(TestClass&)> func3(&TestClass::MemberReturn);
      etl::function_wrapper<int(TestClass&)> func4(&TestClass::MemberReturn);
      etl::function_wrapper<int(TestClass&)> func5(&TestClass::MemberReturnAlternate);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }

    //*************************************************************************
    // Test Member Parameter Function
    //*************************************************************************
    TEST(test_member_parameter_default_construction)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func;
      CHECK_THROW(func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_if(testClass, 1, Moveable("Not_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_or(StaticParameterFunction, testClass, 1, Moveable("Not_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Static_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func(&TestClass::MemberParameter);
      func(testClass, 1, Moveable("Member_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_constexpr)
    {
      TestClassConstexpr testClass;

      constexpr etl::function_wrapper<void(const TestClassConstexpr&, int)> func(&TestClassConstexpr::MemberParameterConst);
      func(testClass, 1);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
    }

    //*************************************************************************
    TEST(test_member_parameter_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func(&TestClass::MemberParameter);
      func.call_if(testClass, 1, Moveable("Member_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_call_if_constexpr)
    {
      TestClassConstexpr testClass;

      etl::function_wrapper<void(const TestClassConstexpr&, int)> func(&TestClassConstexpr::MemberParameterConst);
      func.call_if(testClass, 1);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
    }

    //*************************************************************************
    TEST(test_member_parameter_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func(&TestClass::MemberParameter);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticParameterFunction, testClass, 1, Moveable("Member_Parameter_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_call_or_constexpr)
    {
      TestClassConstexpr testClass;

      etl::function_wrapper<void(const TestClassConstexpr&, int)> func(&TestClassConstexpr::MemberParameterConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticParameterFunctionConstexpr, testClass, 1);
    }

    //*************************************************************************
    TEST(test_member_parameter_const)
    {
      TestClass testClass;

      etl::function_wrapper<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      func(testClass, 1, Moveable("Member_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&, int, Moveable&&)> func;
      CHECK_THROW(func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_const_object_call_if)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      etl::optional<int> result = func.call_if(testClass, 1, Moveable("Not_Called"));
      CHECK_FALSE(result.has_value());
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_const_object_call_or)
    {
      const TestClass testClass;

      etl::function_wrapper<int(const TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Not_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      func(testClass, 1, Moveable("Test_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const_const_object_call_if)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      func.call_if(testClass, 1, Moveable("Member_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const_const_object_call_or)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticParameterFunction, testClass, 1, Moveable("Member_Parameter_Const_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_return_assignment)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func1 = &TestClass::MemberParameter;
      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func2 = &TestClass::MemberParameterAlternate;

      func2 = func1;

      func1(testClass, 1, Moveable("Member_Return_Called"));
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);

      func2(testClass, 2, Moveable("Member_Return_Called"));
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_equality)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func1;
      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func2;
      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func3(&TestClass::MemberParameter);
      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func4(&TestClass::MemberParameter);
      etl::function_wrapper<void(TestClass&, int, Moveable&&)> func5(&TestClass::MemberParameterAlternate);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }

    //*************************************************************************
    // Test Member Function
    //*************************************************************************
    TEST(test_member_default_construction)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func;
      CHECK_THROW(func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Called);
    }

    //*************************************************************************
    TEST(test_member)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func(&TestClass::Member);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_constexpr)
    {
      TestClassConstexpr testClass;

      etl::function_wrapper<void(const TestClassConstexpr&)> func(&TestClassConstexpr::MemberConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
    }

    //*************************************************************************
    TEST(test_member_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func(&TestClass::Member);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_call_if_constexpr)
    {
      TestClassConstexpr testClass;

      etl::function_wrapper<void(const TestClassConstexpr&)> func(&TestClassConstexpr::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
    }

    //*************************************************************************
    TEST(test_member_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func(&TestClass::Member);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_call_or_constexpr)
    {
      TestClassConstexpr testClass;

      etl::function_wrapper<void(const TestClassConstexpr&)> func(&TestClassConstexpr::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunctionConstexpr, testClass);
    }

    //*************************************************************************
    TEST(test_member_const)
    {
      TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func(&TestClass::MemberConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_call_if)
    {
      TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_call_or)
    {
      TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func;
      CHECK_THROW(func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_const_object_call_if)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_const_object_call_or)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Called);
    }

    //*************************************************************************
    TEST(test_member_const_const_object)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func(&TestClass::MemberConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_const_object_call_if)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_const_object_call_or)
    {
      const TestClass testClass;

      etl::function_wrapper<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_assignment)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func1 = &TestClass::Member;
      etl::function_wrapper<void(TestClass&)> func2 = &TestClass::MemberAlternate;

      func2 = func1;

      func1(testClass);
      CHECK_TRUE(func1.is_valid());
      CHECK_TRUE(func1);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);

      func2(testClass);
      CHECK_TRUE(func2.is_valid());
      CHECK_TRUE(func2);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_equality)
    {
      TestClass testClass;

      etl::function_wrapper<void(TestClass&)> func1;
      etl::function_wrapper<void(TestClass&)> func2;
      etl::function_wrapper<void(TestClass&)> func3(&TestClass::Member);
      etl::function_wrapper<void(TestClass&)> func4(&TestClass::Member);
      etl::function_wrapper<void(TestClass&)> func5(&TestClass::MemberAlternate);

      CHECK_TRUE(func1 == func2);
      CHECK_TRUE(func2 == func1);
      CHECK_TRUE(func3 == func4);
      CHECK_TRUE(func4 == func3);
      CHECK_TRUE(func1 != func3);
      CHECK_TRUE(func3 != func1);
      CHECK_TRUE(func1 != func4);
      CHECK_TRUE(func4 != func1);
      CHECK_TRUE(func4 != func5);
      CHECK_TRUE(func5 != func4);
    }
  };
}
