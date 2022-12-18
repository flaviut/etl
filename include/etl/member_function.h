///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl
https://www.etlcpp.com

Copyright(c) 2014 John Wellbelove

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
///\defgroup member_function member_function
/// A set of wrapper templates to allow a function to be stored and called based on its const/non-const definition.
///\ingroup utilities
//*****************************************************************************

#if !ETL_USING_CPP11
  #error etl::member_function requires C++11 or above
#endif

namespace etl
{
  //***************************************************************************
  /// The base for member_function exceptions.
  //***************************************************************************
  class member_function_exception : public exception
  {
  public:

    member_function_exception(string_type reason_, string_type file_name_, numeric_type line_number_)
      : exception(reason_, file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// The uninitialised exception.
  //***************************************************************************
  class member_function_uninitialised : public member_function_exception
  {
  public:

    member_function_uninitialised(string_type file_name_, numeric_type line_number_)
      : member_function_exception(ETL_ERROR_TEXT("member_function:uninitialised", ETL_MEMBER_FUNCTION_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  //***************************************************************************
  /// 
  //***************************************************************************
  template <typename T>
  class member_function;

  //***************************************************************************
  /// Template to wrap free functions, lambdas and functors.
  //***************************************************************************
  template <typename TReturn, typename... TParameters>
  class member_function<TReturn(TParameters...)>
  {
  public:

    using this_type     = member_function<TReturn(TParameters...)>;
    using function_type = TReturn(*)(TParameters...);

    //*************************************************************************
    ///
    //*************************************************************************
    ETL_CONSTEXPR14 member_function() = default;

    //*************************************************************************
    ///
    //*************************************************************************
    ETL_CONSTEXPR14 member_function(const member_function& other) = default;

    //*************************************************************************
    /// Create from function.
    //*************************************************************************
    ETL_CONSTEXPR14 explicit member_function(function_type function_)
    {
      invocation.alternate.func = function_;
      invocation.stub           = function_stub;
    }

    //*************************************************************************
    /// Create from a const lambda or functor.
    //*************************************************************************
    template <typename TLambda, typename = etl::enable_if_t<etl::is_class<TLambda>::value && !etl::is_same<this_type, TLambda>::value, void>>
    ETL_CONSTEXPR14 explicit member_function(TLambda& instance_)
    {
      invocation.alternate.object = (void*)(&instance_);
      invocation.stub             = lambda_stub<TLambda>;
    }

    //*************************************************************************
    /// Create from a const lambda or functor.
    //*************************************************************************
    template <typename TLambda, typename = etl::enable_if_t<etl::is_class<TLambda>::value && !etl::is_same<this_type, TLambda>::value, void>>
    ETL_CONSTEXPR14 explicit member_function(const TLambda& instance_)
    {
      invocation.alternate.object = (void*)(&instance_);
      invocation.stub             = const_lambda_stub<TLambda>;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    bool is_valid() const
    {
      return (invocation.stub != ETL_NULLPTR);
    }

    //*************************************************************************
    ///
    //*************************************************************************
    operator bool() const
    {
      return is_valid();
    }

    //*************************************************************************
    /// Equality operator
    //*************************************************************************
    ETL_CONSTEXPR14 bool operator ==(const member_function& rhs) const
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Inequality operator
    //*************************************************************************
    ETL_CONSTEXPR14 bool operator !=(const member_function& rhs) const
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    etl::enable_if_t<is_void<TRet>::value, TRet>
      call_if(TParameters... params) const
    {
      if (is_valid())
      {
        operator()(TParameters... params);
      }
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_if(TParameters... params) const
    {
      if (is_valid())
      {
        return operator()(TParameters... params);
      }
      else
      {
        return TReturn();
      }
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, TParameters... params) const
    {
      if (is_valid())
      {
        return operator()(TParameters... params);
      }
      else
      {
        return alternative;
      }
    }

    //*************************************************************************
    /// Execute the function.
    //*************************************************************************
    TReturn operator()(TParameters... params) const
    {
      return (*invocation.stub)(invocation, etl::forward<TParameters>(params)...);
    }

  private:

    struct invocation_element;

    using stub_type = TReturn(*)(const invocation_element&, TParameters...);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;
      invocation_element(const invocation_element&) = default;

      //*************************************************************************
      ///
      //*************************************************************************
      ETL_CONSTEXPR14 invocation_element(void* object_, stub_type stub_)
        : alternate(object_)
        , stub(stub_)
      {
      }

      //*************************************************************************
      ///
      //*************************************************************************
      ETL_CONSTEXPR14 invocation_element(function_type func_, stub_type stub_)
        : alternate(func_)
        , stub(stub_)
      {
      }

      //*************************************************************************
      ///
      //*************************************************************************
      ETL_CONSTEXPR14 bool operator ==(const invocation_element& rhs) const
      {
        return (rhs.object == object) &&  (rhs.stub == stub) && (rhs.func == func);
      }

      //*************************************************************************
      ///
      //*************************************************************************
      ETL_CONSTEXPR14 bool operator !=(const invocation_element& rhs) const
      {
        return !(*this == rhs);
      }

      //*************************************************************************
      ///
      //*************************************************************************
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
    static ETL_CONSTEXPR14 TReturn function_stub(const invocation_element& invocation, TParameters... params)
    {
      return (*invocation.alternate.func)(etl::forward<TParameters>(params)...);
    }

    //*************************************************************************
    /// Stub call for a lambda or functor.
    //*************************************************************************
    template <typename TLambda>
    static ETL_CONSTEXPR14 TReturn lambda_stub(const invocation_element& invocation, TParameters... params)
    {
      TLambda* p = static_cast<TLambda*>(invocation.alternate.object);
      return (p->operator())(etl::forward<TParameters>(params)...);
    }

    //*************************************************************************
    /// Stub call for a lambda or functor.
    //*************************************************************************
    template <typename TLambda>
    static ETL_CONSTEXPR14 TReturn const_lambda_stub(const invocation_element& invocation, TParameters... params)
    {
      const TLambda* p = static_cast<const TLambda*>(invocation.alternate.object);
      return (p->operator())(etl::forward<TParameters>(params)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };

  //***************************************************************************
  /// Template to wrap member functions.
  //***************************************************************************
  template <typename TObject, typename TReturn, typename... TParameters>
  class member_function<TReturn(TObject&, TParameters...)>
  {
  public:

    using method_type       = TReturn(TObject::*)(TParameters...);
    using const_method_type = TReturn(TObject::*)(TParameters...) const;

    //*************************************************************************
    /// Default constructor
    //*************************************************************************
    ETL_CONSTEXPR14 
    member_function() = default;

    //*************************************************************************
    /// Copy constructor
    //*************************************************************************
    ETL_CONSTEXPR14 
    member_function(const member_function& other) = default;

    //*************************************************************************
    /// Construct from method
    //*************************************************************************
    ETL_CONSTEXPR14 
    explicit member_function(method_type method_)
    {
      invocation.alternate_method.method = method_;
      invocation.alternate_stub.stub     = method_stub;
    }

    //*************************************************************************
    /// Returns true if the member_function is initialised
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    bool is_valid() const
    {
      return (invocation.alternate_stub.stub != ETL_NULLPTR) && (invocation.alternate_stub.const_stub != ETL_NULLPTR);
    }

    //*************************************************************************
    /// Returns true if the member_function is initialised
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
    ETL_CONSTEXPR14 
    bool operator ==(const member_function& rhs) const
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Inequality operator
    //*************************************************************************
    ETL_NODISCARD
    ETL_CONSTEXPR14
    bool operator !=(const member_function& rhs) const
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    etl::enable_if_t<is_void<TRet>::value, bool>
      call_if(TObject& object, TParameters... params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameters>(params)...);
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
    etl::enable_if_t<!is_void<TRet>::value, etl::optional<TRet>>
      call_if(TObject& object, TParameters... params) const
    {
      etl::optional<TRet> result;

      if (is_valid())
      {
        result = operator()(object, etl::forward<TParameters>(params)...);
      }

      return result;
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    etl::enable_if_t<is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, TObject& object, TParameters... params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameters>(params)...);
      }
      else
      {
        alternative(etl::forward<TParameters>(params)...);
      }
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, TObject& object, TParameters... params) const
    {
      if (is_valid())
      {
        return operator()(object, etl::forward<TParameters>(params)...);
      }
      else
      {
        return alternative(etl::forward<TParameters>(params)...);
      }
    }

    //*************************************************************************
    /// Execute the function.
    //*************************************************************************
    TReturn operator()(TObject& object, TParameters... params) const
    {
      ETL_ASSERT(is_valid(), ETL_ERROR(etl::member_function_uninitialised));

      return (*invocation.alternate_stub.stub)(object, invocation, etl::forward<TParameters>(params)...);
    }

  private:

    struct invocation_element;

    using stub_type       = TReturn(*)(TObject&, const invocation_element&, TParameters...);
    using const_stub_type = TReturn(*)(const TObject&, const invocation_element&, TParameters...);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;
      invocation_element(const invocation_element&) = default;

      //*************************************************************************
      /// Equality operator
      //*************************************************************************
      ETL_CONSTEXPR14 bool operator ==(const invocation_element& rhs) const
      {
        return ((rhs.alternate_stub.stub       == stub)       && (rhs.alternate_stub.method == method)) ||
               ((rhs.alternate_stub.const_stub == const_stub) && (rhs.alternate_stub.const_method == const_method))
      }

      //*************************************************************************
      /// Inequality operator
      //*************************************************************************
      ETL_CONSTEXPR14 bool operator !=(const invocation_element& rhs) const
      {
        return !(*this == rhs);
      }

      //*************************************************************************
      /// Union of alternate methods
      //*************************************************************************
      union alternate_method_t
      {
        alternate_method_t()
          : method(ETL_NULLPTR)
        {
        }

        alternate_method_t(method_type method_)
          : method(method_)
        {
        }

        alternate_method_t(const_method_type const_method_)
          : const_method(const_method_)
        {
        }

        method_type       method;
        const_method_type const_method;
      } alternate_method;

      //*************************************************************************
      /// Union of alternate stubs
      //*************************************************************************
      union alternate_stub_t
      {
        alternate_stub_t()
          : stub(ETL_NULLPTR)
        {
        }

        alternate_stub_t(method_type stub_)
          : stub(stub_)
        {
        }

        alternate_stub_t(const_method_type const_stub_)
          : const_stub(const_stub_)
        {
        }

        stub_type       stub;
        const_stub_type const_stub;
      } alternate_stub;    
    };

    //*************************************************************************
    /// Stub call for a member function.
    //*************************************************************************
    static ETL_CONSTEXPR14 TReturn method_stub(TObject& object, const invocation_element& invocation, TParameters... params)
    {
      method_type pf = invocation.alternate_method.method;
      return (object.*pf)(etl::forward<TParameters>(params)...);
    }

    //*************************************************************************
    /// Stub call for a const member function.
    //*************************************************************************
    static ETL_CONSTEXPR14 TReturn const_method_stub(const TObject& object, const invocation_element& invocation, TParameters... params)
    {
      const_method_type pf = invocation.alternate_method.const_method;
      return (object.*pf)(etl::forward<TParameters>(params)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };

  //***************************************************************************
  /// Template to wrap member functions.
  //***************************************************************************
  template <typename TObject, typename TReturn, typename... TParameters>
  class member_function<TReturn(const TObject&, TParameters...)>
  {
  public:

    using const_method_type = TReturn(TObject::*)(TParameters...) const;

    //*************************************************************************
    ///
    //*************************************************************************
    ETL_CONSTEXPR14 member_function() = default;

    //*************************************************************************
    ///
    //*************************************************************************
    ETL_CONSTEXPR14 member_function(const member_function& other) = default;

    //*************************************************************************
    ///
    //*************************************************************************
    ETL_CONSTEXPR14 explicit member_function(const_method_type const_method_)
    {
      invocation.alternate_method.const_method = const_method_;
      invocation.alternate_stub.const_stub     = const_method_stub;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    bool is_valid() const
    {
      return (invocation.alternate_stub.const_stub != ETL_NULLPTR);
    }

    //*************************************************************************
    ///
    //*************************************************************************
    operator bool() const
    {
      return is_valid();
    }

    //*************************************************************************
    /// Equality operator
    //*************************************************************************
    ETL_CONSTEXPR14 bool operator ==(const member_function& rhs) const
    {
      return invocation == rhs.invocation;
    }

    //*************************************************************************
    /// Inequality operator
    //*************************************************************************
    ETL_CONSTEXPR14 bool operator !=(const member_function& rhs) const
    {
      return invocation != rhs.invocation;
    }

    //*************************************************************************
    ///
    //*************************************************************************
    template <typename TRet = TReturn>
    etl::enable_if_t<is_void<TRet>::value, bool>
      call_if(const TObject& object, TParameters... params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameters>(params)...);
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
    etl::enable_if_t<!is_void<TRet>::value, etl::optional<TRet>>
      call_if(const TObject& object, TParameters... params) const
    {
      etl::optional<TRet> result;

      if (is_valid())
      {
        result = operator()(object, etl::forward<TParameters>(params)...);
      }

      return result;
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    etl::enable_if_t<is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, const TObject& object, TParameters... params) const
    {
      if (is_valid())
      {
        operator()(object, etl::forward<TParameters>(params)...);
      }
      else
      {
        alternative(etl::forward<TParameters>(params)...);
      }
    }

    //*************************************************************************
    /// 
    //*************************************************************************
    template <typename TAlternative, typename TRet = TReturn>
    etl::enable_if_t<!is_void<TRet>::value, TRet>
      call_or(TAlternative alternative, const TObject& object, TParameters... params) const
    {
      if (is_valid())
      {
        return operator()(object, etl::forward<TParameters>(params)...);
      }
      else
      {
        return alternative(etl::forward<TParameters>(params)...);
      }
    }

    //*************************************************************************
    /// Execute the function.
    //*************************************************************************
    TReturn operator()(const TObject& object, TParameters... params) const
    {
      ETL_ASSERT(is_valid(), ETL_ERROR(etl::member_function_uninitialised));

      return (*invocation.alternate_stub.const_stub)(object, invocation, etl::forward<TParameters>(params)...);
    }

  private:

    struct invocation_element;

    using const_stub_type = TReturn(*)(const TObject&, const invocation_element&, TParameters...);

    //*************************************************************************
    /// The internal invocation object.
    //*************************************************************************
    struct invocation_element
    {
      invocation_element() = default;
      invocation_element(const invocation_element&) = default;

      //*************************************************************************
      ///
      //*************************************************************************
      ETL_CONSTEXPR14 bool operator ==(const invocation_element& rhs) const
      {
        return (rhs.stub == stub) && (rhs.method == method);
      }

      //*************************************************************************
      ///
      //*************************************************************************
      ETL_CONSTEXPR14 bool operator !=(const invocation_element& rhs) const
      {
        return !(*this == rhs);
      }

      //*************************************************************************
      ///
      //*************************************************************************
      union alternate_method_t
      {
        alternate_method_t()
          : const_method(ETL_NULLPTR)
        {
        }

        alternate_method_t(const_method_type const_method_)
          : const_method(const_method_)
        {
        }

        const_method_type const_method;
      } alternate_method;

      //*************************************************************************
      ///
      //*************************************************************************
      union alternate_stub_t
      {
        alternate_stub_t()
          : const_stub(ETL_NULLPTR)
        {
        }

        alternate_stub_t(const_stub_type const_stub_)
          : const_stub(const_stub_)
        {
        }

        const_stub_type const_stub;
      } alternate_stub;
    };

    //*************************************************************************
    /// Stub call for a const member function.
    //*************************************************************************
    static ETL_CONSTEXPR14 TReturn const_method_stub(const TObject& object, const invocation_element& invocation, TParameters... params)
    {
      const_method_type pf = invocation.alternate_method.const_method;
      return (object.*pf)(etl::forward<TParameters>(params)...);
    }

    //*************************************************************************
    /// The invocation object.
    //*************************************************************************
    invocation_element invocation;
  };
}

#endif
