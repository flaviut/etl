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

#ifndef ETL_EXPECTED_INCLUDED
#define ETL_EXPECTED_INCLUDED

#include "platform.h"
#include "type_traits.h"
#include "utility.h"
#include "variant.h"

namespace etl
{
  //***************************************************************************
  /// Unexpected type.
  /// etl::unexpected represents an unexpected value stored in etl::expected.
  //***************************************************************************
  template <typename TError>
  class unexpected
  {
  public:

    typedef TError error_type;

    //*******************************************
    /// Copy constructor.
    //*******************************************
    ETL_CONSTEXPR14 unexpected(const unexpected& other)
      : error_value(other.error_value)
    {
    }

#if ETL_USING_CPP11
    //*******************************************
    /// Move constructor.
    //*******************************************
    ETL_CONSTEXPR unexpected(unexpected&& other)
      : error_value(etl::move(other.error_value))
    {
    }
#endif

#if ETL_USING_CPP11
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr = TError, typename = typename etl::enable_if<!etl::is_same<typename etl::remove_cvref<TErr>::type, etl::unexpected<TErr>>::value &&
                                                                         !etl::is_same<typename etl::remove_cvref<TErr>::type, etl::in_place_t>::value, int>::type>
    constexpr explicit unexpected(TErr&& e)
      : error_value(etl::forward<TErr>(e))
    {
    }
#else
    //*******************************************
    /// Construct from argument.
    //*******************************************
    template <typename TErr>
    explicit unexpected(const TErr& e, typename etl::enable_if<!etl::is_same<typename etl::remove_cvref<TErr>::type, etl::unexpected<TErr> >::value &&
                                                               !etl::is_same<typename etl::remove_cvref<TErr>::type, etl::in_place_t>::value, int>::type = 0)
      : error_value(e)
    {
    }
#endif

#if ETL_USING_CPP11
    //*******************************************
    /// Construct from arguments.
    //*******************************************
    template <typename... Args >
    constexpr explicit unexpected(etl::in_place_t, Args&&... args)
      : error_value(etl::forward<Args>(args)...)
    {
    }
#endif

#if ETL_HAS_INITIALIZER_LIST
    //*******************************************
    /// Construct from initializer_list and arguments.
    //*******************************************
    template <typename U, typename... Args>
    constexpr explicit unexpected(etl::in_place_t, std::initializer_list<U> init, Args&&... args)
      : error_value(init, etl::forward<Args>(args)...)
    {
    }
#endif

    //*******************************************
    /// Assign from etl::unexpected.
    //*******************************************
    ETL_CONSTEXPR14
    etl::unexpected<TError>& operator =(const etl::unexpected<TError>& rhs)
    {
      error_value = rhs.error_value;

      return *this;
    }

#if ETL_USING_CPP11
    //*******************************************
    /// Move assign from etl::unexpected.
    //*******************************************
    ETL_CONSTEXPR14
    etl::unexpected<TError>& operator =(etl::unexpected<TError>&& rhs)
    {
      error_value = etl::move(rhs.error_value);

      return *this;
    }
#endif

#if ETL_USING_CPP11
    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr const TError& error() const& noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr const TError&& error() const&& noexcept
    {
      return etl::move(error_value);
    }

#if ETL_USING_CPP14
    //*******************************************
    /// Get the error.
    //*******************************************
    constexpr TError& error() & noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************    
    constexpr TError&& error() && noexcept
    {
      return etl::move(error_value);
    }
#else
    //*******************************************
    /// Get the error.
    //*******************************************
    TError& error() & noexcept
    {
      return error_value;
    }

    //*******************************************
    /// Get the error.
    //*******************************************    
    TError&& error() && noexcept
    {
      return etl::move(error_value);
    }
#endif

#else
    //*******************************************
    /// Get the error.
    //*******************************************
    const TError& error() const
    {
      return error_value;
    }
#endif

    //*******************************************
    /// Swap with another etl::unexpected.
    //*******************************************
    void swap(etl::unexpected<TError>& other)
    {
      using ETL_OR_STD::swap;

      swap(error_value, other.error_value);
    }

  private:

    TError error_value;
  };

#if ETL_USING_CPP17
  template <typename TError>
  unexpected(TError) -> unexpected<TError>;
#endif

  //*****************************************************************************
  /// unexpect_t
  //*****************************************************************************
  struct unexpect_t
  {
    ETL_CONSTEXPR14 explicit unexpect_t()
    {
    }
  };

#if ETL_CPP17_SUPPORTED
  inline constexpr unexpect_t unexpect{};
#else
  static const unexpect_t unexpect;
#endif

  //*****************************************************************************
/// Expected type.
//*****************************************************************************
  template <typename TValue, typename TError>
  class expected
  {
  public:

    typedef TValue                  value_type;
    typedef TError                  error_type;
    typedef etl::unexpected<TError> unexpected_type;

#if ETL_CPP11_SUPPORTED
    template <typename U>
    using rebind = expected<U, TError>;
#endif

    //*******************************************
    /// Default constructor
    //*******************************************
    ETL_CONSTEXPR14 expected() ETL_NOEXCEPT
      : data(value_type())
    {
    }

    //*******************************************
    /// Constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(const value_type& value_) ETL_NOEXCEPT
      : data(value_)
    {
    }

#if ETL_CPP11_SUPPORTED
    //*******************************************
    /// Constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(value_type&& value_) ETL_NOEXCEPT
      : data(etl::move(value_))
    {
    }
#endif

    //*******************************************
    /// Copy constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(const expected& other) ETL_NOEXCEPT
      : data(other.data)
    {
    }

#if ETL_CPP11_SUPPORTED
    //*******************************************
    /// Move constructor
    //*******************************************
    ETL_CONSTEXPR14 expected(expected&& other) ETL_NOEXCEPT
      : data(etl::move(other.data))
    {
    }
#endif

    template <typename F>
    ETL_CONSTEXPR14 explicit expected(const etl::unexpected<F>& ue)
      : data(ue)
    {
    }

#if ETL_CPP11_SUPPORTED
    template <typename F>
    ETL_CONSTEXPR14 explicit expected(etl::unexpected<F>&& ue)
      : data(etl::move(ue))
    {
    }
#endif

    ETL_CONSTEXPR14 explicit expected(etl::in_place_t) ETL_NOEXCEPT
      : data(value_type())
    {
    }

    template <typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::in_place_t, Args&&... args)
      : data(etl::forward<Args>(args)...)
    {
    }

    template <typename U, typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::in_place_t, std::initializer_list<U> il, Args&&... args)
      : data(il, etl::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::unexpect_t, Args&&... args)
      : data(etl::unexpected<error_type>(args...))
    {
    }

    template <typename U, typename... Args>
    ETL_CONSTEXPR14 explicit expected(etl::unexpect_t, std::initializer_list<U> il, Args&&... args)
      : data(etl::unexpected<error_type>(il, args...))
    {
    }

    //*******************************************
    /// Copy assign from value
    //*******************************************
    expected& operator =(const value_type& value)
    {
      data = value;
      return *this;
    }

    //*******************************************
    /// Move assign from value
    //*******************************************
    expected& operator =(value_type&& value)
    {
      data = etl::move(value);
      return *this;
    }

    //*******************************************
    /// Copy assign from error
    //*******************************************
    expected& operator =(const error_type& error)
    {
      data = error;
      return *this;
    }

    //*******************************************
    /// Move assign from error
    //*******************************************
    expected& operator =(error_type&& error)
    {
      data = etl::move(error);
      return *this;
    }

#if ETL_CPP11_SUPPORTED
    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 value_type& value()&
    {
      return etl::get<value_type>(data);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 const value_type& value() const&
    {
      return etl::get<value_type>(data);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 value_type&& value()&&
    {
      return etl::move(etl::get<value_type>(data));
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 const value_type&& value() const&&
    {
      return etl::move(etl::get<value_type>(data));
    }
#else
    //*******************************************
    /// Returns a const reference to the value.
    /// Undefined if the expected does not contain an value.
    //*******************************************
    ETL_CONSTEXPR14 const value_type& value() const
    {
      return etl::get<value_type>(data);
    }
#endif

    //*******************************************
    /// 
    //*******************************************
    bool has_value()
    {
      return (data.index() == 0U);
    }

#if ETL_CPP11_SUPPORTED
    //*******************************************
    /// 
    //*******************************************
    template <typename U>
    ETL_CONSTEXPR14 value_type value_or(U&& default_value) const&
    {
      if (has_value())
      {
        return value();
      }
      else
      {
        return default_value;
      }
    }

    //*******************************************
    /// 
    //*******************************************
    template <typename U>
    ETL_CONSTEXPR14 value_type value_or(U&& default_value)&&
    {
      if (has_value())
      {
        return etl::move(value());
      }
      else
      {
        return etl::move(default_value);
      }
    }
#else
    //*******************************************
    /// 
    //*******************************************
    template <typename U>
    value_type value_or(const U& default_value) const
    {
      if (has_value())
      {
        return value();
      }
      else
      {
        return default_value;
      }
    }
#endif

#if ETL_CPP11_SUPPORTED
    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 const error_type& error() const& ETL_NOEXCEPT
    {
      return etl::get<error_type>(data);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 error_type& error()& ETL_NOEXCEPT
    {
      return etl::get<error_type>(data);
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 const error_type&& error() const&& ETL_NOEXCEPT
    {
      return etl::move(etl::get<error_type>(data));
    }

    //*******************************************
    /// 
    //*******************************************
    ETL_CONSTEXPR14 error_type&& error() && ETL_NOEXCEPT
    {
      return etl::move(etl::get<error_type>(data));
    }
#else
    //*******************************************
    /// Returns a const reference to the error.
    /// Undefined if the expected does not contain an error.
    //*******************************************
    ETL_CONSTEXPR14 const error_type& error() const
    {
      return etl::get<error_type>(data);
    }
#endif

#if ETL_CPP11_SUPPORTED
    //*******************************************
    /// 
    //*******************************************
    template <typename... Args>
    ETL_CONSTEXPR14 value_type& emplace(Args&&... args) ETL_NOEXCEPT
    {
      data.emplace(args...);
    }

    //*******************************************
    /// 
    //*******************************************
    template <typename U, typename... Args>
    ETL_CONSTEXPR14 value_type& emplace(std::initializer_list<U>& il, Args&&... args) ETL_NOEXCEPT
    {
      data.emplace(il, args...);
    }
#endif

    //*******************************************
    /// 
    //*******************************************
    value_type* operator ->()
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return etl::addressof(data.get<value_type>());
    }

    //*******************************************
    /// 
    //*******************************************
    const value_type* operator ->() const
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return etl::addressof(data.get<value_type>());
    }

    //*******************************************
    /// 
    //*******************************************
    value_type& operator *()
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return data.get<value_type>();
    }

    //*******************************************
    /// 
    //*******************************************
    const value_type& operator *() const
    {
#if ETL_IS_DEBUG_BUILD
      ETL_ASSERT(valid, ETL_ERROR(expected_invalid));
#endif

      return data.get<value_type>();
    }

  private:

    etl::variant<value_type, error_type> data;
  };

  //*****************************************************************************
  /// Specialisation for void value type.
  //*****************************************************************************
  template<typename TError>
  class expected<void, TError>
  {
  public:

    typedef void                    value_type;
    typedef TError                  error_type;
    typedef etl::unexpected<TError> unexpected_type;

  private:

    error_type error;
  };
}

//*******************************************
/// Equivalence operator.
//*******************************************
template <typename TError>
ETL_CONSTEXPR14
bool operator ==(const etl::unexpected<TError>& lhs, const etl::unexpected<TError>& rhs)
{
  return lhs.error_value == rhs.error_value;
}

//*******************************************
/// Swap etl::unexpected.
//*******************************************
template <typename TError>
ETL_CONSTEXPR14
void swap(etl::unexpected<TError>& lhs, etl::unexpected<TError>& rhs)
{
  lhs.swap(rhs);
}

#endif
