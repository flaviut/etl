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
  /// The for member_function exceptions.
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
  /// The out of range exceptions.
  //***************************************************************************
  class member_function_call_to_non_const : public member_function_exception
  {
  public:

    member_function_call_to_non_const(string_type file_name_, numeric_type line_number_)
      : member_function_exception(ETL_ERROR_TEXT("member_function:call to non-const", ETL_MEMBER_FUNCTION_FILE_ID"A"), file_name_, line_number_)
    {
    }
  };

  template <typename T>
  class member_function;

  //***************************************************************************
  /// Template to wrap one of two member functions, one const, one non const.
  //***************************************************************************
  template <typename TObject, typename TReturn, typename... TParameter>
  class member_function<TReturn(TObject::*)(TParameter...)>
  {
  public:

    typedef TReturn(TObject::* const non_const_function_t)(TParameter...);
    typedef TReturn(TObject::* const const_function_t)(TParameter...) const;

    static ETL_CONSTANT char No_Function        = 0;
    static ETL_CONSTANT char Non_Const_Function = 1;
    static ETL_CONSTANT char Const_Function     = 2;

    //*******************************
    ETL_CONSTEXPR14 member_function()
      : function_union()
      , function_type(No_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 explicit member_function(non_const_function_t function_)
      : function_union(function_)
      , function_type(Non_Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 explicit member_function(const_function_t function_)
      : function_union(function_)
      , function_type(Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 member_function(const member_function& other)
      : function_union(other.function_union)
      , function_type(other.function_type)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 member_function& operator =(const member_function& rhs)
    {
      if (this != &other)
      {
        function_union = other.function_union;
        function_type  = other.function_type;
      }

      return *this;
    }

    //*******************************
    bool is_valid() const
    {
      return (function_type != No_Function);             
    }

    //*******************************
    operator bool() const
    {
      return is_valid();
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator ==(const member_function& rhs) const
    {
      return (function_union == other.function_union) && (function_type == other.function_type);
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator !=(const member_function& rhs) const
    {
      return (function_union != other.function_union) || (function_type != other.function_type);
    }

    //*******************************
    template <typename UReturn = TReturn>
    etl::enable_if_t<etl::is_void<UReturn>::value, UReturn>
      operator()(TObject& object, TParameter... parameter) const
    {
      switch (function_type)
      {
        case Non_Const_Function:
        {
          (object.*function_union.non_const_function)(etl::forward<TParameter>(parameter)...);
          break;
        }

        case Const_Function:
        {
          (object.*function_union.const_function)(etl::forward<TParameter>(parameter)...);
          break;
        }

        case No_Function:
        default:
        {
          break;
        }
      }
    }

    //*******************************
    template <typename UReturn = TReturn>
    etl::enable_if_t<etl::is_void<UReturn>::value, UReturn>
      operator()(const TObject& object, TParameter... parameter) const
    {
      switch (function_type)
      {
        case Non_Const_Function:
        {
          ETL_ASSERT_FAIL(ETL_ERROR(member_function_call_to_non_const));
          break;
        }
        
        case Const_Function:
        {
          (object.*function_union.const_function)(etl::forward<TParameter>(parameter)...);
          break;
        }

        case No_Function:
        default:
        {
          break;
        }
      }
    }

    //*******************************
    template <typename UReturn = TReturn>
    etl::enable_if_t<!etl::is_void<UReturn>::value, UReturn>
      operator()(TObject& object, TParameter... parameter, const UReturn& default_value) const
    {
      switch (function_type)
      {
        case Non_Const_Function:
        {
          return (object.*function_union.non_const_function)(etl::forward<TParameter>(parameter)...);
          break;
        }

        case Const_Function:
        {
          return (object.*function_union.const_function)(etl::forward<TParameter>(parameter)...);
          break;
        }

        case No_Function:
        default:
        {
          return default_value;
        }
      }
    }

    //*******************************
    template <typename UReturn = TReturn>
    etl::enable_if_t<!etl::is_void<UReturn>::value, UReturn>
      operator()(const TObject& object, TParameter... parameter, const UReturn& default_value) const
    {
      switch (function_type)
      {
        case Non_Const_Function:
        {
          ETL_ASSERT_FAIL(ETL_ERROR(member_function_call_to_non_const));
          break;
        }

        case Const_Function:
        {
          return (object.*function_union.const_function)(etl::forward<TParameter>(parameter)...);
          break;
        }

        case No_Function:
        default:
        {
          return default_value;
        }
      }
    }

  private:

    union fu
    {
      //*******************************
      ETL_CONSTEXPR14 fu()
        : non_const_function(ETL_NULLPTR)
      {
      }

      //*******************************
      ETL_CONSTEXPR14 fu(non_const_function_t function_)
        : non_const_function(function_)
      {
      }

      //*******************************
      ETL_CONSTEXPR14 fu(const_function_t function_)
        : const_function(function_)
      {
      }

      non_const_function_t non_const_function;
      const_function_t     const_function;
    } function_union;

    char function_type;
  };
}

#endif
