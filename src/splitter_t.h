#pragma once

#include "packet_t.h"

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>


class splitter_t {
public:
	splitter_t( uint32_t max_packet_payload ) : max_packet_payload( max_packet_payload ) { }
	splitter_t( ) : max_packet_payload( 1000 ) { }	// default consructor ?
 
	void process( const std::list<std::string> &sequence ) {
		auto it = sequence.begin( ) ;
		for( uint32_t i = 0, j = 0; it != sequence.end( ); ++i ) {
			packet_t packet( i, max_packet_payload ) ;

			for( ;it != sequence.end( ); ++it, ++j ) {
				auto ret = packet.add_elem( it->data(), it->size(), j ) ;
				if( ret != packet_t::addelemret::add_full ) break ;
			}
			auto p = packet.get_packet( ) ;
			packets.emplace_back( std::get<0>( p ) ) ;

			auto &v = packets.back( ) ; 
			uint32_t size = std::get<1>( p ) ;
			v.resize( size ) ; v.shrink_to_fit( ) ;
		}

		std::cout << "split sequence to a " << packets.size( ) << " packets\n" ;
	}

	std::vector< std::vector<uint8_t> > &getdata( ) { return packets ; }


	void savetofile( const std::string &filename ) {
		if( packets.empty( ) ) {
			std::cerr << "save empty sequence to file." << std::endl ;
			throw std::runtime_error("empty sequence");
		}
		std::ofstream fs( filename, fs.trunc | fs.binary ) ;
		fs.write( reinterpret_cast<const char*>( &max_packet_payload ), sizeof( uint32_t ) ) ;
		for( const auto &d : packets ) fs.write( reinterpret_cast<const char*>( d.data( ) ), d.size( ) ) ;
	}

	void fillfromfile( const std::string &filename ) {
		if( !packets.empty( ) ) packets.clear( ) ;

		std::ifstream fs( filename, fs.binary ) ;
		if( !fs.is_open( ) ) return ;

		fs.read( reinterpret_cast<char*>( &max_packet_payload ), sizeof( uint32_t ) ) ;	// fill max_packet_payload

		auto curpos = fs.tellg( ) ;
		fs.seekg( 0, std::fstream::end ) ;
		auto filelen = fs.tellg( ) - curpos ;
		fs.seekg( curpos ) ;
		auto fileprogress = 0 ;

		while( fileprogress < filelen ) {
			std::vector<uint8_t> vec( max_packet_payload ) ;
			uint32_t progress = 0 ;

			fs.read( reinterpret_cast<char*>( vec.data() + progress ), sizeof( packet_head_st ) ) ;
			packet_head_st *ph = reinterpret_cast<packet_head_st *>( vec.data() + progress ) ;
			progress += sizeof( packet_head_st ) ;

			for( uint32_t i = 0 ; i < ph->elements_count; ++i ) {
				fs.read( reinterpret_cast<char*>( vec.data() + progress ), sizeof( element_head_st ) ) ;
				element_head_st *eh = reinterpret_cast<element_head_st *>( vec.data() + progress ) ;
				progress += sizeof( element_head_st ) ;
				fs.read( reinterpret_cast<char *>( vec.data() + progress ), eh->part_bytes ) ;
				progress += eh->part_bytes ;
			}

			fileprogress += progress ;
			vec.resize( progress ) ; vec.shrink_to_fit( ) ;
			packets.emplace_back( vec ) ;
		}
	}

private:
	std::vector< std::vector<uint8_t> > packets ;
	uint32_t max_packet_payload ;
} ;


