/* lorina: C++ parsing library
 * Copyright (C) 2017-2018  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*! \cond PRIVATE */

#pragma once

#include "format.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <locale>
#include <memory>
#include <numeric>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef _WIN32
#include <libgen.h>
#include <wordexp.h>
#endif

namespace lorina
{

namespace detail
{

/* std::apply in C++14 taken from https://stackoverflow.com/a/36656413 */
template<typename Function, typename Tuple, size_t ... I>
auto apply(Function f, Tuple t, std::index_sequence<I ...>)
{
  return f(std::get<I>(t) ...);
}

template<typename Function, typename Tuple>
auto apply(Function f, Tuple t)
{
  static constexpr auto size = std::tuple_size<Tuple>::value;
  return apply(f, t, std::make_index_sequence<size>{});
}

template<typename... Args>
class call_in_topological_order
{
public:
  call_in_topological_order( std::function<void(Args...)> f )
    : f( f )
  {
  }

  void declare_known( const std::string& known )
  {
    _known.emplace( known );
  }

  void call_deferred( const std::vector<std::string>& inputs, const std::string& output, Args... params )
  {
    /* do we have all inputs */
    std::unordered_set<std::string> unknown;
    for ( const auto& input : inputs )
    {
      if ( _known.find( input ) != _known.end() )
        continue;

      auto it = _waits_for.find( input );
      if ( it == _waits_for.end() || !it->second.empty() )
      {
        unknown.insert( input );
      }
    }

    std::tuple<Args...> args = std::make_tuple( params... );
    _stored_params.emplace( output, args );

    if ( !unknown.empty() )
    {
      /* defer computation */
      for ( const auto& input : unknown )
      {
        _triggers[input].insert( output );
        _waits_for[output].insert( input );
      }
      return;
    }

    /* trigger dependency computation */
    compute_dependencies( output );
  }

  void compute_dependencies( const std::string& output )
  {
     /* init empty, makes sure nothing is waiting for this output */
    _waits_for[output];
    std::stack<std::string> computed;
    computed.push( output );
    while ( !computed.empty() )
    {
      auto next = computed.top();
      computed.pop();

      // C++17: std::apply( f, _stored_params[next] );
      detail::apply( f, _stored_params[next] );

      for ( const auto& other : _triggers[next] )
      {
        _waits_for[other].erase( next );
        if ( _waits_for[other].empty() )
        {
          computed.push( other );
        }
      }
      _triggers[next].clear();
    }
  }

  std::vector<std::pair<std::string,std::string>> unresolved_dependencies()
  {
    std::vector<std::pair<std::string,std::string>> deps;
    for ( const auto& w : _waits_for )
    {
      if ( !w.second.empty() )
      {
        for ( const auto& v : w.second )
        {
          deps.push_back( std::make_pair( w.first, v ) );
        }
      }
    }
    return deps;
  }

private:
  std::unordered_set<std::string> _known;
  std::unordered_map<std::string, std::unordered_set<std::string>> _triggers;
  std::unordered_map<std::string, std::unordered_set<std::string>> _waits_for;
  std::function<void(Args...)> f;
  std::unordered_map<std::string, std::tuple<Args...>> _stored_params;
}; /* call_in_topological_order */

template<typename T>
inline std::string join( const T& t, const std::string& sep )
{
  return std::accumulate(
      std::next( t.begin() ), t.end(), std::string( t[0] ),
      [&]( const std::string& s, const typename T::value_type& v ) { return s + sep + std::string( v ); } );
}

/* string utils are from https://stackoverflow.com/a/217605 */

inline void ltrim( std::string& s )
{
  s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( int ch ) {
             return !std::isspace( ch );
           } ) );
}

inline void rtrim( std::string& s )
{
  s.erase( std::find_if( s.rbegin(), s.rend(), []( int ch ) {
             return !std::isspace( ch );
           } )
               .base(),
           s.end() );
}

inline void trim( std::string& s )
{
  ltrim( s );
  rtrim( s );
}

inline std::string trim_copy( std::string s )
{
  trim( s );
  return s;
}

inline void foreach_line_in_file_escape( std::istream& in, const std::function<bool( const std::string& )>& f )
{
  std::string line, line2;

  while ( getline( in, line ) )
  {
    trim( line );

    while ( line.back() == '\\' )
    {
      line.pop_back();
      trim( line );
      if ( !getline( in, line2 ) )
      {
        assert( false );
      }
      line += line2;
    }

    if ( !f( line ) )
    {
      break;
    }
  }
}

// https://stackoverflow.com/a/14266139
inline std::vector<std::string> split( const std::string& str, const std::string& sep )
{
  std::vector<std::string> result;

  size_t last = 0;
  size_t next = 0;
  std::string substring;
  while ( ( next = str.find( sep, last ) ) != std::string::npos )
  {
    substring = str.substr( last, next - last );
    if ( substring.length() > 0 )
    {
      std::string sub = str.substr( last, next - last );
      sub.erase( std::remove( sub.begin(), sub.end(), ' ' ), sub.end() );
      result.push_back( sub );
    }
    last = next + 1;
  }

  substring = str.substr( last );
  substring.erase( std::remove( substring.begin(), substring.end(), ' ' ), substring.end() );
  result.push_back( substring );

  return result;
}

#ifndef _WIN32
inline std::string word_exp_filename( const std::string& filename )
{
  std::string result;

  wordexp_t p;
  wordexp( filename.c_str(), &p, 0 );

  for ( auto i = 0u; i < p.we_wordc; ++i )
  {
    if ( !result.empty() )
    {
      result += " ";
    }
    result += std::string( p.we_wordv[i] );
  }

  wordfree( &p );

  return result;
}
#else
inline const std::string& word_exp_filename( const std::string& filename )
{
  return filename;
}
#endif

#ifndef _WIN32
inline std::string basename( const std::string& filepath )
{
  return std::string( ::basename( const_cast<char*>( filepath.c_str() ) ) );
}
#endif

inline bool starts_with( std::string const& s, std::string const& match )
{
  return ( s.substr( 0, match.size() ) == match );
}

} // namespace detail
} // namespace lorina

/*! \endcond */
