#pragma once

#include <string>
#include <list>
#include <random>
#include <fstream>
#include <iostream>


class stringsequence_t {
public:
	stringsequence_t( ) { }

	void fillfromfile( const std::string &filename ) {
		std::ifstream fs( filename ) ;
		if( !fs.is_open( ) ) return ;

		if( !sequence.empty( ) ) sequence.clear( ) ;
		while( !fs.eof() ) {
			std::string s ;
			fs >> s ;
			addstring( std::move( s ) ) ;
		}
		for( const auto &s : sequence ) std::cout << s << std::endl ;
	}

	void generatesequence( uint32_t stringmaxlen, uint32_t stringcount ) {
		sequence.clear( ) ;

		std::random_device rd;
		std::uniform_int_distribution<int> dist( 1, stringmaxlen ) ;
		std::mt19937 g( rd( ) );
		for( auto i = 0; i < stringcount; ++i ) {
#if 1
			std::string str ;
			uint32_t l = dist( g ) ;

			
			for( auto i = 0; i < l; ++i ) {
				auto k = i % 64 ;
				char ch = 'A' + k;
				str.append( &ch, 1 ) ;
			}
			
			sequence.emplace_back( str ) ;
#else
			sequence.emplace_back( std::string( dist( rd ), '0' + i ) ) ;
#endif
		}
	}

	void savetofile( const std::string &filename ) {
		if( sequence.empty( ) ) {
			std::cerr << "save empty sequence to file." << std::endl ;
			throw std::runtime_error("empty sequence");
		}
		std::ofstream fs( filename, fs.trunc ) ;
		for( const auto &s : sequence ) fs << s << std::endl ;
	}

	void addstring( std::string &&s ) {
		if( !s.empty( ) ) sequence.emplace_back( s ) ;
	}

	const std::list<std::string> &getsequence( ) { return sequence ; }
protected:
	std::list<std::string> sequence ;
} ;


