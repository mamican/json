// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_JSON_CONTRIB_SHARED_PTR_TRAITS_HPP
#define TAO_JSON_CONTRIB_SHARED_PTR_TRAITS_HPP

#include <memory>
#include <type_traits>

#include "../forward.hpp"

#include "internal/indirect_traits.hpp"
#include "internal/type_traits.hpp"

namespace tao
{
   namespace json
   {
      namespace internal
      {
         template< typename T, typename U >
         struct shared_ptr_traits
         {
            template< template< typename... > class Traits, typename Base >
            static auto as( const basic_value< Traits, Base >& v ) -> typename std::enable_if< use_first_ptr_as< T, Traits, Base >::value, std::shared_ptr< U > >::type
            {
               if( v == null ) {
                  return std::shared_ptr< U >();
               }
               return std::make_shared< T >( v );
            }

            template< template< typename... > class Traits, typename Base >
            static auto as( const basic_value< Traits, Base >& v ) -> typename std::enable_if< use_second_ptr_as< T, Traits, Base >::value || use_fourth_ptr_as< T, Traits, Base >::value, std::shared_ptr< U > >::type
            {
               if( v == null ) {
                  return std::shared_ptr< U >();
               }
               return std::make_shared< T >( Traits< T >::as( v ) );
            }

            template< template< typename... > class Traits, typename Base >
            static auto as( const basic_value< Traits, Base >& v ) -> typename std::enable_if< use_third_ptr_as< T, Traits, Base >::value, std::shared_ptr< U > >::type
            {
               if( v == null ) {
                  return std::shared_ptr< U >();
               }
               auto t = std::make_shared< T >();
               Traits< T >::to( v, *t );
               return t;
            }

            template< template< typename... > class Traits, typename Producer >
            static auto consume( Producer& parser ) -> typename std::enable_if< use_first_ptr_consume< T, Traits, Producer >::value || use_third_ptr_consume< T, Traits, Producer >::value, std::shared_ptr< U > >::type
            {
               if( parser.null() ) {
                  return std::shared_ptr< U >();
               }
               return Traits< T >::template consume< Traits >( parser );
            }

            template< template< typename... > class Traits, typename Producer >
            static auto consume( Producer& parser ) -> typename std::enable_if< use_second_ptr_consume< T, Traits, Producer >::value, std::shared_ptr< U > >::type
            {
               if( parser.null() ) {
                  return std::shared_ptr< U >();
               }
               auto t = std::make_shared< T >();
               Traits< T >::template consume< Traits >( parser, *t );
               return t;
            }
         };

      }  // namespace internal

      template< typename T, typename U = T >
      struct shared_ptr_traits
         : public internal::shared_ptr_traits< T, U >
      {
         template< template< typename... > class Traits, typename Base >
         static void assign( basic_value< Traits, Base >& v, const std::shared_ptr< U >& o )
         {
            v = static_cast< const T& >( *o );  // Unconditional dereference for use in binding::factory.
         }

         template< template< typename... > class Traits, typename Consumer >
         static void produce( Consumer& c, const std::shared_ptr< U >& o )
         {
            json::events::produce< Traits >( c, static_cast< const T& >( *o ) );  // Unconditional dereference for use in binding::factory.
         }
      };

      template< typename T >
      struct shared_ptr_traits< T, T >
         : public internal::shared_ptr_traits< T, T >,
           public internal::indirect_traits< std::shared_ptr< T > >
      {
         template< typename V >
         using with_base = shared_ptr_traits< T, V >;
      };

   }  // namespace json

}  // namespace tao

#endif