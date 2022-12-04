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

#ifndef ETL_FUNCTION_ALTERNATES_INCLUDED
#define ETL_FUNCTION_ALTERNATES_INCLUDED

#include "platform.h"
#include "nullptr.h"
#include "utility.h"

//*****************************************************************************
///\defgroup function_alternates function_alternates
/// A set of wrapper templates to allow a function to be stored and called based on its const/non-const definition.
///\ingroup utilities
//*****************************************************************************

namespace etl
{
  //***************************************************************************
  /// Template to wrap one of two member functions, one const, one non const.
  //***************************************************************************
  template <typename TObject, typename TReturn, typename... TParameter>
  class function_alternates
  {
  public:

    typedef TReturn(TObject::* const non_const_function_t)(TParameter...);
    typedef TReturn(TObject::* const const_function_t)(TParameter...) const;

    static ETL_CONSTANT char No_Function        = 0;
    static ETL_CONSTANT char Non_Const_Function = 1;
    static ETL_CONSTANT char Const_Function     = 2;

    //*******************************
    ETL_CONSTEXPR14 function_alternates()
      : function_union()
      , function_type(No_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 explicit function_alternates(non_const_function_t function_)
      : function_union(function_)
      , function_type(Non_Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 explicit function_alternates(const_function_t function_)
      : function_union(function_)
      , function_type(Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const function_alternates& other)
      : function_union(other.function_union)
      , function_type(other.function_type)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates& operator =(const function_alternates& rhs)
    {
      if (this != &other)
      {
        function_union = other.function_union;
        function_type  = other.function_type;
      }

      return *this;
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator ==(const function_alternates& rhs) const
    {
      return (function_union == other.function_union) && (function_type == other.function_type);
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator !=(const function_alternates& rhs) const
    {
      return (function_union != other.function_union) || (function_type != other.function_type);
    }

    //*******************************
    TReturn call(TObject& object, TParameter... parameter, const TReturn& default_value) const
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

  //***************************************************************************
  /// Template to wrap one of two member functions, one const, one non const.
  //***************************************************************************
  template <typename TObject, typename TReturn>
  class function_alternates<TObject, TReturn, void>
  {
  public:

    typedef TReturn(TObject::* const non_const_function_t)();
    typedef TReturn(TObject::* const const_function_t)() const;

    static ETL_CONSTANT char No_Function = 0;
    static ETL_CONSTANT char Non_Const_Function = 1;
    static ETL_CONSTANT char Const_Function = 2;

    //*******************************
    ETL_CONSTEXPR14 function_alternates()
      : function_union()
      , function_type(No_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(non_const_function_t function_)
      : function_union(function_)
      , function_type(Non_Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const_function_t function_)
      : function_union(function_)
      , function_type(Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const function_alternates& other)
      : function_union(other.function_union)
      , function_type(other.function_type)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates& operator =(const function_alternates& rhs)
    {
      if (this != &other)
      {
        function_union = other.function_union;
        function_type = other.function_type;
      }

      return *this;
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator ==(const function_alternates& rhs) const
    {
      return (function_union == other.function_union) && (function_type == other.function_type);
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator !=(const function_alternates& rhs) const
    {
      return (function_union != other.function_union) || (function_type != other.function_type);
    }

    //*******************************
    TReturn call(TObject& object, const TReturn& default_value) const
    {
      switch (function_type)
      {
        case Non_Const_Function:
        {
          return (object.*function_union.non_const_function)();
          break;
        }

        case Const_Function:
        {
          return (object.*function_union.const_function)();
          break;
        }

        case No_Function:
        default:
        {
          return default_value;
          break;
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

  //***************************************************************************
  /// Template to wrap one of two member functions, one const, one non const.
  //***************************************************************************
  template <typename TObject, typename... TParameter>
  class function_alternates<TObject, void, TParameter...>
  {
  public:

    typedef void(TObject::* const non_const_function_t)(TParameter...);
    typedef void(TObject::* const const_function_t)(TParameter...) const;

    static ETL_CONSTANT char No_Function = 0;
    static ETL_CONSTANT char Non_Const_Function = 1;
    static ETL_CONSTANT char Const_Function = 2;

    //*******************************
    ETL_CONSTEXPR14 function_alternates()
      : function_union()
      , function_type(No_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(non_const_function_t function_)
      : function_union(function_)
      , function_type(Non_Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const_function_t function_)
      : function_union(function_)
      , function_type(Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const function_alternates& other)
      : function_union(other.function_union)
      , function_type(other.function_type)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates& operator =(const function_alternates& rhs)
    {
      if (this != &other)
      {
        function_union = other.function_union;
        function_type = other.function_type;
      }

      return *this;
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator ==(const function_alternates& rhs) const
    {
      return (function_union == other.function_union) && (function_type == other.function_type);
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator !=(const function_alternates& rhs) const
    {
      return (function_union != other.function_union) || (function_type != other.function_type);
    }

    //*******************************
    void call(TObject& object, TParameter... parameter) const
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

  //***************************************************************************
  /// Template to wrap one of two member functions, one const, one non const.
  //***************************************************************************
  template <typename TObject>
  class function_alternates<TObject, void, void>
  {
  public:

    typedef void(TObject::* const non_const_function_t)();
    typedef void(TObject::* const const_function_t)() const;

    static ETL_CONSTANT char No_Function    = 0;
    static ETL_CONSTANT char Non_Const_Function = 1;
    static ETL_CONSTANT char Const_Function = 2;

    //*******************************
    ETL_CONSTEXPR14 function_alternates()
      : function_union()
      , function_type(No_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(non_const_function_t function_)
      : function_union(function_)
      , function_type(Non_Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const_function_t function_)
      : function_union(function_)
      , function_type(Const_Function)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates(const function_alternates& other)
      : function_union(other.function_union)
      , function_type(other.function_type)
    {
    }

    //*******************************
    ETL_CONSTEXPR14 function_alternates& operator =(const function_alternates& rhs)
    {
      if (this != &other)
      {
        function_union = other.function_union;
        function_type = other.function_type;
      }

      return *this;
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator ==(const function_alternates& rhs) const
    {
      return (function_union == other.function_union) && (function_type == other.function_type);
    }

    //*******************************
    ETL_CONSTEXPR14 bool operator !=(const function_alternates& rhs) const
    {
      return (function_union != other.function_union) || (function_type != other.function_type);
    }

    //*******************************
    void call(TObject& object) const
    {
      switch (function_type)
      {
        case Non_Const_Function:
        {
          (object.*function_union.non_const_function)();
          break;
        }

        case Const_Function:
        {
          (object.*function_union.const_function)();
          break;
        }

        case No_Function:
        default:
        {
          break;
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
