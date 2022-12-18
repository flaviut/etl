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
    Not_Called,
    Member_Return_Parameter_Called,
    Member_Return_Parameter_Const_Called,
    Member_Parameter_Called,
    Member_Parameter_Const_Called,
    Member_Return_Called,
    Member_Return_Const_Called,
    Member_Called,
    Member_Const_Called,
    Functor_Return_Parameter_Called,
    Functor_Return_Parameter_Const_Called,
    Functor_Return_Called,
    Functor_Return_Const_Called,
    Lambda_Return_Parameter_Called,
    Lambda_Return_Called,
    Static_Return_Parameter_Called,
    Static_Parameter_Called,
    Static_Return_Called,
    Static_Called
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
    void MemberConst() const
    {
      function_called = FunctionCalled::Member_Const_Called;
    }

    mutable int i;
    mutable Moveable m;
  };

  //***************************************************************************
  static int StaticReturnParameterFunction(int i_, Moveable&& m_)
  {
    function_called = FunctionCalled::Static_Return_Parameter_Called;
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

  Functor functor;

  //***************************************************************************
  SUITE(test_member_function)
  {
    //*************************************************************************
    // Test Static Functions
    //*************************************************************************
    TEST(test_static_return_parameter)
    {
      etl::member_function<int(int, Moveable&&)> func(StaticReturnParameterFunction);
      int result = func(1, Moveable("Static_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_static_return)
    {
      etl::member_function<int()> func(StaticReturnFunction);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_static)
    {
      etl::member_function<void()> func(StaticFunction);
      func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Static_Called);
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

      etl::member_function<int(int, Moveable&&)> func(lambda);
      int result = func(1, Moveable("Lambda_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Lambda_Return_Parameter_Called);
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

      etl::member_function<int()> func(lambda);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Lambda_Return_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    // Test Functor Functions
    //*************************************************************************
    TEST(test_functor_return_parameter)
    {
      Functor functor;

      etl::member_function<int(int, Moveable&&)> func(functor);
      int result = func(1, Moveable("Functor_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_functor_return_parameter_const)
    {
      const Functor functor;

      etl::member_function<int(int, Moveable&&)> func(functor);
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

      etl::member_function<int()> func(functor);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Called);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_functor_return_const)
    {
      const Functor functor;

      etl::member_function<int()> func(functor);
      int result = func();
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Functor_Return_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    // Test Member Return Parameter Function
    //*************************************************************************
    TEST(test_member_return_parameter_default_construction)
    {
      TestClass testClass;
      int result;

      etl::member_function<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_call_if)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      etl::optional<int> result = func.call_if(testClass, 1, Moveable("Not_Called"));
      CHECK_FALSE(result.has_value());
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_default_construction_call_or)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Static_Return_Parameter_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameter);
      int result = func(testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_call_if)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameter);
      etl::optional<int> result = func.call_if(testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_call_or)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameter);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Member_Return_Parameter_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const)
    {
      TestClass testClass;

      etl::member_function<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
      int result = func(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const_call_if)
    {
      TestClass testClass;

      etl::member_function<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
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

      etl::member_function<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
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

      etl::member_function<int(const TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(const TestClass&, int, Moveable&&)> func(&TestClass::MemberReturnParameterConst);
      int result = func(testClass, 1, Moveable("Member_Return_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Parameter_Const_Called);
      CHECK_EQUAL(1, result);
    }

    //*************************************************************************
    // Test Member Return Function
    //*************************************************************************
    TEST(test_member_return_default_construction)
    {
      TestClass testClass;
      int result;

      etl::member_function<int(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_THROW(result = func(testClass), etl::member_function_uninitialised);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_call_if)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func;
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

      etl::member_function<int(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Called);
    }

    //*************************************************************************
    TEST(test_member_return)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::MemberReturn);
      int result = func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_member_return_call_if)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::MemberReturn);
      etl::optional<int> result = func.call_if(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
      CHECK_TRUE(result.has_value());
      CHECK_EQUAL(0, result);
    }

    //*************************************************************************
    TEST(test_member_return_call_or)
    {
      TestClass testClass;

      etl::member_function<int(TestClass&)> func(&TestClass::MemberReturn);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Called);
    }

    //*************************************************************************
    TEST(test_member_return_const)
    {
      TestClass testClass;

      etl::member_function<int(const TestClass&)> func(&TestClass::MemberReturnConst);
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

      etl::member_function<int(const TestClass&)> func(&TestClass::MemberReturnConst);
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

      etl::member_function<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(const TestClass&)> func;
      CHECK_THROW(int result = func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
    }

    //*************************************************************************
    TEST(test_member_return_default_construction_const_object_call_if)
    {
      const TestClass testClass;

      etl::member_function<int(const TestClass&)> func;
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

      etl::member_function<int(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Called);
    }

    //*************************************************************************
    TEST(test_member_return_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<int(const TestClass&)> func(&TestClass::MemberReturnConst);
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

      etl::member_function<int(const TestClass&)> func(&TestClass::MemberReturnConst);
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

      etl::member_function<int(const TestClass&)> func(&TestClass::MemberReturnConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      int result = func.call_or(StaticReturnFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Return_Const_Called);
    }

    //*************************************************************************
    // Test Member Parameter Function
    //*************************************************************************
    TEST(test_member_parameter_default_construction)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func;
      CHECK_THROW(func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_call_if)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_if(testClass, 1, Moveable("Not_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_call_or)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_or(StaticParameterFunction, testClass, 1, Moveable("Not_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Static_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func(&TestClass::MemberParameter);
      func(testClass, 1, Moveable("Member_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_call_if)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func(&TestClass::MemberParameter);
      func.call_if(testClass, 1, Moveable("Member_Parameter_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_call_or)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&, int, Moveable&&)> func(&TestClass::MemberParameter);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticParameterFunction, testClass, 1, Moveable("Member_Parameter_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const)
    {
      TestClass testClass;

      etl::member_function<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      func(testClass, 1, Moveable("Member_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&, int, Moveable&&)> func;
      CHECK_THROW(func(testClass, 1, Moveable("Not_Called")), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_default_construction_const_object_call_if)
    {
      const TestClass testClass;

      etl::member_function<int(const TestClass&, int, Moveable&&)> func;
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

      etl::member_function<int(const TestClass&, int, Moveable&&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      int result = func.call_or(StaticReturnParameterFunction, testClass, 1, Moveable("Not_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Static_Return_Parameter_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      func(testClass, 1, Moveable("Test_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const_const_object_call_if)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      func.call_if(testClass, 1, Moveable("Member_Parameter_Const_Called"));
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_parameter_const_const_object_call_or)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&, int, Moveable&&)> func(&TestClass::MemberParameterConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticParameterFunction, testClass, 1, Moveable("Member_Parameter_Const_Called"));
      CHECK_TRUE(function_called == FunctionCalled::Member_Parameter_Const_Called);
    }

    //*************************************************************************
    // Test Member Function
    //*************************************************************************
    TEST(test_member_default_construction)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func;
      CHECK_THROW(func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_call_if)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_call_or)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Called);
    }

    //*************************************************************************
    TEST(test_member)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func(&TestClass::Member);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_call_if)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func(&TestClass::Member);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_call_or)
    {
      TestClass testClass;

      etl::member_function<void(TestClass&)> func(&TestClass::Member);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Called);
    }

    //*************************************************************************
    TEST(test_member_const)
    {
      TestClass testClass;

      etl::member_function<void(const TestClass&)> func(&TestClass::MemberConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_call_if)
    {
      TestClass testClass;

      etl::member_function<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_call_or)
    {
      TestClass testClass;

      etl::member_function<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&)> func;
      CHECK_THROW(func(testClass), etl::member_function_uninitialised);
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_const_object_call_if)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Not_Called);
    }

    //*************************************************************************
    TEST(test_member_default_construction_const_object_call_or)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&)> func;
      CHECK_FALSE(func.is_valid());
      CHECK_FALSE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Static_Called);
    }

    //*************************************************************************
    TEST(test_member_const_const_object)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&)> func(&TestClass::MemberConst);
      func(testClass);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_const_object_call_if)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_if(testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }

    //*************************************************************************
    TEST(test_member_const_const_object_call_or)
    {
      const TestClass testClass;

      etl::member_function<void(const TestClass&)> func(&TestClass::MemberConst);
      CHECK_TRUE(func.is_valid());
      CHECK_TRUE(func);
      func.call_or(StaticFunction, testClass);
      CHECK_TRUE(function_called == FunctionCalled::Member_Const_Called);
    }
  };
}
