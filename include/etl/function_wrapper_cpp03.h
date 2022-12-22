///\file

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

#ifndef ETL_MEMBER_FUNCTION_INCLUDED
#define ETL_MEMBER_FUNCTION_INCLUDED

#include "platform.h"
#include "nullptr.h"
#include "utility.h"
#include "optional.h"
#include "exception.h"
#include "error_handler.h"

//*****************************************************************************
///\defgroup function_wrapper function_wrapper
/// A set of wrapper templates to allow a function to be stored and called based on its const/non-const definition.
///\ingroup utilities
//*****************************************************************************

namespace etl
{
  //***************************************************************************
  /// The base for function_wrapper exceptions.
  //***************************************************************************
  class function_wrapper_exception : public exception
  {
  public:

    function_wrapper_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The uninitialised exception.
  //***************************************************************************
  class function_wrapper_uninitialised : public function_wrapper_exception
  {
  public:

    function_wrapper_uninitialised(string_type file_name_, numeric_type line_number_)
      : function_wrapper_exception(ETL_ERROR_TEXT("function_wrapper:uninitialised", ETL_MEMBER_FUNCTION_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// Prototype
  //***************************************************************************
  template <typename T>
  class function_wrapper;

  //***************************************************************************
  /// Template to wrap free functions, lambdas and functors.
  //***************************************************************************
  template <typename TReturn, typename TParameter>
  class function_wrapper<TReturn(TParameter)>
  {
  public:

    typedef function_wrapper<TReturn(TParameter)> this_type;
    typedef TReturn(*)(TParameter)                function_type;

    //*************************************************************************
    /// Default constructor
    //*************************************************************************
    function_wrapper()
      : invocation()
    {
    }

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    function_wrapper(const function_wrapper& other)
      : invocation(other.invocation)
    {
    }

    //*************************************************************************
    /// Assignment
    //*************************************************************************
    function_wrapper& operator =(const function_wrapper& other)
    {
      invocation = other.invocation;
    }

    //*************************************************************************
    /// Create from function.
    //*************************************************************************
    function_wrapper(function_type function_)
      : invocation(function_, function_stub)
    {
    }

    //*************************************************************************
    /// Create from a const lambda or functor.
    //*************************************************************************
    template <typename TLambda>
    function_wrapper(TLambda& instance_, typename etl::enable_if<etl::is_class<TLambda>::value && !etl::is_same<this_type, TLambda>::value, void*>::type = 0)
      : invocation((void*)(&instance_), lambda_stub<TLambda>)
    {
    }

    //*************************************************************************
    /// Create from a const lambda or functor.
    //*************************************************************************
    template <typename TLambda>
    function_wrapper(const TLambda& instance_, typename etl::enable_if<etl::is_class<TLambda>::value && !etl::is_same<this_type, TLambda>::value, void*>::type = 0)
      : invocation((void*)(&instance_), const_lambda_stub<TLambda>)
    {
    }

    //*************************************************************************
    /// Returns true if initialised
    //*************************************************************************
    bool is_valid() const
    {
      return (invocation.stub != ETL_NULLPTR);
    }

    //*************************************************************************
    /// Returns true if initialised
    //*************************************************************************
    operator bool() const
    {
      return is_valid();
    }

    //*************************************************************************
    /// Equality operator
    //*************************************************************************
    bool operator ==(const function_wrapper& rhs) const
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Inequality operator
    //*************************************************************************
    bool operator !=(const function_wrapper& rhs) const
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    /// Call the wrapped function if initialsed
    //*************************************************************************
    bool call_if(TParameter params, typename etl::enable_if_t<is_void<TReturn>::value, int>::type = 0) const
    {
      if (is_valid())
      {
        operator()(TParameter params);
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    ///
    //*************************************************************************
    TReturn call_if(TParameter params, typename etl::enable_if_t<!is_void<TReturn>::value, int>::type = 0) const
    {
      etl::optional<TReturn> result;

      if (is_valid())
      {
        result = operator()(TParameter params);
      }

      return result;
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative>
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, TParameter params) const
    {
      if (is_valid())
      {
        return operator()(TParameter params);
      }
      else
      {
        return alternative(TParameter params);
      }
    }

    //*************************************************************************
    /// Execute the function.
    //*************************************************************************
    TReturn operator()(TParameter params) const
    {
      return (*invocation.stub)(invocation, etl::forward<TParameter>(params)...);
    }

  private:

    struct invocation_element;

    using stub_type = TReturn(*)(const invocation_element&, TParameter);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;
      invocation_element(const invocation_element&) = default;
      invocation_element& operator=(const invocation_element&) = default;

      //*******************************
      invocation_element(void* object_, stub_type stub_)
        : alternate(object_)
        , stub(stub_)
      {
      }

      //*******************************
      invocation_element(function_type func_, stub_type stub_)
        : alternate(func_)
        , stub(stub_)
      {
      }

      //*******************************
      bool operator ==(const invocation_element& rhs) const
      {
        return (stub == rhs.stub) &&
                 ((stub == ETL_NULLPTR) ||
                  ((stub == function_stub) && (rhs.alternate.func == alternate.func)) ||
                  (rhs.alternate.object == alternate.object));
      }

      //*******************************
      bool operator !=(const invocation_element& rhs) const
      {
        return !(*this == rhs);
      }

      //*******************************
      union alternate_method_t
      {
        alternate_method_t()
          : object(ETL_NULLPTR)
        {
        }

        alternate_method_t(void* object_)
          : object(object_)
        {
        }

        alternate_method_t(function_type func_)
          : func(func_)
        {
        }

        void*         object;
        function_type func;
      } alternate;

      stub_type stub = ETL_NULLPTR;
    };

    //*************************************************************************
    /// Stub call for a free function.
    //*************************************************************************
    static TReturn function_stub(const invocation_element& invocation, TParameter params)
    {
      return (*invocation.alternate.func)(etl::forward<TParameter>(params)...);
    }

    //*************************************************************************
    /// Stub call for a lambda or functor.
    //*************************************************************************
    template <typename TLambda>
    static TReturn lambda_stub(const invocation_element& invocation, TParameter params)
    {
      TLambda* p = static_cast<TLambda*>(invocation.alternate.object);
      return (p->operator())(etl::forward<TParameter>(params)...);
    }

    //*************************************************************************
    /// Stub call for a lambda or functor.
    //*************************************************************************
    template <typename TLambda>
    static TReturn const_lambda_stub(const invocation_element& invocation, TParameter params)
    {
      const TLambda* p = static_cast<const TLambda*>(invocation.alternate.object);
      return (p->operator())(etl::forward<TParameter>(params)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };

  //***************************************************************************
  /// Template to wrap member functions.
  //***************************************************************************
  template <typename TObject, typename TReturn, typename... TParameter>
  class function_wrapper<TReturn(TObject&, TParameter)>
  {
  public:

    using method_type = TReturn(TObject::*)(TParameter);

    //*************************************************************************
    /// Default constructor
    //*************************************************************************
    ETL_CONSTEXPR14
    function_wrapper() = default;

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    
    function_wrapper(const function_wrapper& other) = default;

    //*************************************************************************
    /// Assignment
    //*************************************************************************
    function_wrapper& operator =(const function_wrapper& other) = default;

    //*************************************************************************
    /// Construct from method
    //*************************************************************************
    
    function_wrapper(method_type method_)
      : invocation(method_, method_stub)
    {
    }

    //*************************************************************************
    /// Returns true if the function_wrapper is initialised
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    bool is_valid() const
    {
      return (invocation.stub != ETL_NULLPTR);
    }

    //*************************************************************************
    /// Returns true if the function_wrapper is initialised
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    operator bool() const
    {
      return is_valid();
    }

    //*************************************************************************
    /// Equality operator
    //*************************************************************************
    ETL_NODISCARD
    
    bool operator ==(const function_wrapper& rhs) const
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Inequality operator
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    bool operator !=(const function_wrapper& rhs) const
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<is_void<TRet>::value, bool>
      call_if(TObject& object, TParameter params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameter>(params)...);
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<!is_void<TRet>::value, etl::optional<TRet>>
      call_if(TObject& object, TParameter params) const
    {
      etl::optional<TRet> result;

      if (is_valid())
      {
        result = operator()(object, etl::forward<TParameter>(params)...);
      }

      return result;
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, TObject& object, TParameter params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameter>(params)...);
      }
      else
      {
        alternative(etl::forward<TParameter>(params)...);
      }
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, TObject& object, TParameter params) const
    {
      if (is_valid())
      {
        return operator()(object, etl::forward<TParameter>(params)...);
      }
      else
      {
        return alternative(etl::forward<TParameter>(params)...);
      }
    }

    //*************************************************************************
    /// Execute the function.
    //*************************************************************************
    ETL_CONSTEXPR14
    TReturn operator()(TObject& object, TParameter params) const
    {
      ETL_ASSERT(is_valid(), ETL_ERROR(etl::function_wrapper_uninitialised));

      return (*invocation.stub)(object, invocation.method, etl::forward<TParameter>(params)...);
    }

  private:

    using stub_type = TReturn(*)(TObject&, method_type, TParameter);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;
      invocation_element(const invocation_element&) = default;
      invocation_element& operator =(const invocation_element&) = default;
      
      //*******************************
      invocation_element(method_type method_, stub_type stub_)
        : method(method_)
        , stub(stub_)
      {
      }

      //*********************************
      bool operator ==(const invocation_element& rhs) const
      {
        return (rhs.method == method) && (rhs.stub == stub);
      }

      //*********************************
      bool operator !=(const invocation_element& rhs) const
      {
        return !(*this == rhs);
      }

      method_type method = ETL_NULLPTR;
      stub_type   stub   = ETL_NULLPTR;
    };

    //*************************************************************************
    /// Stub call for a member function.
    //*************************************************************************
    static TReturn method_stub(TObject& object, method_type method, TParameter params)
    {
      return (object.*method)(etl::forward<TParameter>(params)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };

  //***************************************************************************
  /// Template to wrap const member functions.
  //***************************************************************************
  template <typename TObject, typename TReturn, typename... TParameter>
  class function_wrapper<TReturn(const TObject&, TParameter)>
  {
  public:

    using const_method_type = TReturn(TObject::*)(TParameter) const;

    //*************************************************************************
    /// Default constructor
    //*************************************************************************
    function_wrapper() = default;

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    function_wrapper(const function_wrapper& other) = default;

    //*************************************************************************
    /// Assignment
    //*************************************************************************
    function_wrapper& operator =(const function_wrapper& other) = default;

    //*************************************************************************
    /// Construct from method
    //*************************************************************************
    function_wrapper(const_method_type const_method_)
      : invocation(const_method_, const_method_stub)
    {
    }

    //*************************************************************************
    /// Has this been initialised
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    bool is_valid() const
    {
      return (invocation.const_stub != ETL_NULLPTR);
    }

    //*************************************************************************
    /// Has this been initialised
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    operator bool() const
    {
      return is_valid();
    }

    //*************************************************************************
    /// Equality operator
    //*************************************************************************
    ETL_NODISCARD
    bool operator ==(const function_wrapper& rhs) const
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Inequality operator
    //*************************************************************************
    ETL_NODISCARD
    bool operator !=(const function_wrapper& rhs) const
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<is_void<TRet>::value, bool>
      call_if(const TObject& object, TParameter params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameter>(params)...);
        return true;
      }
      else
      {
        return false;
      }
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<!is_void<TRet>::value, etl::optional<TRet>>
      call_if(const TObject& object, TParameter params) const
    {
      etl::optional<TRet> result;

      if (is_valid())
      {
        result = operator()(object, etl::forward<TParameter>(params)...);
      }

      return result;
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, const TObject& object, TParameter params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameter>(params)...);
      }
      else
      {
        alternative(etl::forward<TParameter>(params)...);
      }
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    ETL_CONSTEXPR14
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, const TObject& object, TParameter params) const
    {
      if (is_valid())
      {
        return operator()(object, etl::forward<TParameter>(params)...);
      }
      else
      {
        return alternative(etl::forward<TParameter>(params)...);
      }
    }

    //*************************************************************************
    /// Execute the function.
    //*************************************************************************
    ETL_CONSTEXPR14
    TReturn operator()(const TObject& object, TParameter params) const
    {
      ETL_ASSERT(is_valid(), ETL_ERROR(etl::function_wrapper_uninitialised));

      return (*invocation.const_stub)(object, invocation.const_method, etl::forward<TParameter>(params)...);
    }

  private:

    using const_stub_type = TReturn(*)(const TObject&, const_method_type, TParameter);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;
      invocation_element(const invocation_element&) = default;
      invocation_element& operator =(const invocation_element&) = default;
      
      //*******************************
      invocation_element(const_method_type method_, const_stub_type stub_)
        : const_method(method_)
        , const_stub(stub_)
      {
      }

      //*********************************
      bool operator ==(const invocation_element& rhs) const
      {
        return (rhs.const_method == const_method) && (rhs.const_stub == const_stub);
      }

      //*********************************
      bool operator !=(const invocation_element& rhs) const
      {
        return !(*this == rhs);
      }

      const_method_type const_method = ETL_NULLPTR;
      const_stub_type   const_stub   = ETL_NULLPTR;
    };

    //*************************************************************************
    /// Stub call for a const member function.
    //*************************************************************************
    static TReturn const_method_stub(const TObject& object, const_method_type const_method, TParameter params)
    {
      return (object.*const_method)(etl::forward<TParameter>(params)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };
}

#endif
