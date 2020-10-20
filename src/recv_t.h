#pragma once


#include <vector>
#include <map>
#include <list>
#include <tuple>
#include <stdint.h>

#include "packet_t.h"



class recv_t {
public:
	bool process( const void *packet ) { // receive by one packet
		bool ret = false ;
		const uint8_t *ptr = reinterpret_cast<const uint8_t*>( packet ) ;
		const packet_head_st *ph = reinterpret_cast<const packet_head_st *>( ptr ) ;
		ptr += sizeof( packet_head_st ) ;

		for( auto i = 0 ; i < ph->elements_count; ++i ) {
			const element_head_st *eh = reinterpret_cast<const element_head_st *>( ptr ) ;
			ptr += sizeof( element_head_st ) ;

			if( eh->el_len_bytes == eh->part_bytes ) {	// full packet, add to sequence
				sequence.emplace( std::make_pair( eh->el_num, std::string( reinterpret_cast<const char *>( ptr ), eh->part_bytes ) ) ) ;
				ret = true ;
			} else {
				auto &s = storage[ eh->el_num ] ;
				s.emplace( std::make_pair( eh->part_num, std::string( reinterpret_cast<const char *>( ptr ), eh->part_bytes ) ) ) ;

				uint32_t size = 0 ;
				for( const auto &m : s ) size += m.second.size( ) ;

				if( size == eh->el_len_bytes ) {
					std::string str ;
					for( const auto &m : s ) str.append( m.second ) ;

					sequence.emplace( std::make_pair( eh->el_num, str ) ) ;
					ret = true ;

					storage.erase(  eh->el_num ) ;
				}
			}
			ptr += eh->part_bytes ;
		}

		return ret ;
	}

	const std::map< uint32_t, std::string > &getdata( ) { return sequence ; }
	void resetsequence( ) { sequence.clear( ) ; }

	
	void reset( ) { storage.clear( ) ; } 
private:

	std::map< uint32_t, std::map< uint32_t, std::string > > storage ;
	std::map< uint32_t, std::string > sequence ;

};
