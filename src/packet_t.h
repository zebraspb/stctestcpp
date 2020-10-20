#pragma once

#include <stdint.h>
#include <string.h>
#include <tuple>
#include <vector>
#include <functional>

struct packet_head_st {
	uint32_t packet_num ;
	uint32_t elements_count ;

	packet_head_st( uint32_t packet_num ) 
		: packet_num( packet_num ), elements_count( 0 )
	{}
} ;


struct element_head_st {
	uint32_t el_num ;	// element number in orig sequence
	uint32_t el_len_bytes ;
	uint32_t part_num ;
	uint32_t part_bytes ;

	element_head_st( uint32_t el_num, uint32_t el_len_bytes )
		: el_num( el_num ), el_len_bytes( el_len_bytes )
		, part_num( 0 ), part_bytes( el_len_bytes )
	{}
} ;

struct lastelem_head_st {
	element_head_st eh ;
	uint32_t part_progress ;

	lastelem_head_st( ) : eh( 0, 0 ), part_progress( 0 ) { }
};


class packet_t {

public:

	enum class addelemret : uint32_t {
		add_full,
		add_part,
		add_null,

		last = 255
	};

	packet_t( uint32_t packet_num, uint32_t max_packet_payload ) 
		: max_packet_payload( max_packet_payload )
		, payload( max_packet_payload )
		, progress( sizeof( packet_head_st ) )
	{
		packet_head = new( payload.data( ) ) packet_head_st( packet_num ) ;
	}

	addelemret add_elem( const void *data, uint32_t data_size, uint32_t elemnum ) {
		std::cout << "add elem num " << elemnum << " with length " << data_size << std::endl ;

		if( progress + sizeof( element_head_st ) + 1 >= max_packet_payload ) 
				return addelemret::add_null ; //can't add one byte of body

		element_head_st *elem_head = new( payload.data( ) + progress ) element_head_st( elemnum, data_size ) ;
		progress += sizeof( element_head_st ) ;

		uint32_t already_send = last_element.part_progress ;
		uint32_t need_to_send = data_size - already_send ;
		uint32_t packet_left  = max_packet_payload - progress ;

		uint32_t part_bytes = std::min<uint32_t>( need_to_send, packet_left ) ;

		elem_head->part_num      = last_element.eh.part_num ;
		elem_head->part_bytes    = part_bytes ;

//		std::cout << "already_send " << already_send << " need_to_send " << need_to_send << " packet_left " << packet_left <<
//					 " elem_head->part_num " << elem_head->part_num << " elem_head->part_bytes " << elem_head->part_bytes << std::endl ;


		const uint8_t *p = reinterpret_cast<const uint8_t *>( data ) ;
		memcpy( payload.data( ) + progress, p + already_send, part_bytes ) ;
		progress += part_bytes ;

		++packet_head->elements_count ;

		last_element.part_progress = already_send + part_bytes ;


//		std::cout << "last_element.part_progress " << last_element.part_progress << " data_size " << data_size << std::endl ;

		if( last_element.part_progress == data_size ) {
			reset_last_element( ) ;
			return addelemret::add_full ;
		}

		last_element.eh = *elem_head ;
		++last_element.eh.part_num ;

		return addelemret::add_part ;
	}

	void reset_last_element( ) { 
		memset( &last_element, 0, sizeof( lastelem_head_st ) ) ; 
	}

	std::tuple<std::vector<uint8_t> &, uint32_t> get_packet( ) {
		return std::make_tuple( std::ref( payload ), progress ) ;
	}

private:
	uint32_t max_packet_payload ;
	std::vector<uint8_t> payload ;
	uint32_t progress ;

	packet_head_st  *packet_head ;

	static lastelem_head_st last_element ;
} ;

lastelem_head_st packet_t::last_element ;

