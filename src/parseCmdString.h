#pragma once

#include <variant>
#include <string>

#include <stdint.h>

class parsedstring_t {
public:
	enum class action_en : uint32_t {
		help,
		generate,
		split,
		order,
		recv,

		unknown = 254,
		last = 255
	};

	struct generatefile_st {
		uint32_t stringmaxlen ;
		uint32_t stringcount ;
		std::string filename ;

		generatefile_st( ) : stringmaxlen( 0 ), stringcount( 0 ) { }
	} ;

	struct splitdata_st {
		uint32_t packetsize ;
		std::string inputfilename ;
		std::string outputfilename ;
		splitdata_st( ) : packetsize( 0 ) { }
	} ;

	struct order_st {
		std::string inputfilename ;
		std::string outputfilename ;
	} ;

	struct receivepackets_st {
		std::string inputfilename ;
		std::string outputfilename ;
	} ;

	std::variant<generatefile_st, splitdata_st, order_st, receivepackets_st> v ;
} ;



class parsecmdstring_t {
public:
	bool parsecmdstring( int argc, char **argv ) ;
	void usage( const char * ) ;

	const parsedstring_t& get( ) { return p_s ; }
private:
	parsedstring_t p_s ;
} ;


