#include "parseCmdString.h"

#include <unistd.h>
#include <getopt.h>


#include <iostream>
#include <sstream>


bool parsecmdstring_t::parsecmdstring( int argc, char **argv ) {
	static const char* opts = "hgsor";
	static struct option longopt[] = {
										{ "help",     no_argument, nullptr, opts[ 0 ] },
										{ "generate", no_argument, nullptr, opts[ 1 ] },
										{ "split",    no_argument, nullptr, opts[ 2 ] },
										{ "order",    no_argument, nullptr, opts[ 3 ] },
										{ "recv",     no_argument, nullptr, opts[ 4 ] },
										{ "stringmaxlen", required_argument, nullptr, 0 },
										{ "stringcount",  required_argument, nullptr, 0 },
										{ "packetsize",   required_argument, nullptr, 0 },
										{ "infile",       required_argument, nullptr, 0 },
										{ "outfile",      required_argument, nullptr, 0 },
										{ nullptr, 0, nullptr, 0 }  } ;

	parsedstring_t::generatefile_st   generate ;
	parsedstring_t::splitdata_st      split;
	parsedstring_t::order_st          order ;
	parsedstring_t::receivepackets_st recv ;

	int mode = 0 ;

	while( 1 ) {
		int index = -1;
		int c ;
		if( ( c = getopt_long( argc, argv, opts, longopt, &index ) ) == -1 ) break ;

		if( opts[ 0 ] == c ) { // --help
			// help process here, up retrun false
			usage( argv[ 0 ] ) ;
			return false ;
		}
		else if( ( opts[ 1 ] == c ) ||
				 ( opts[ 2 ] == c ) ||
				 ( opts[ 3 ] == c ) ||
				 ( opts[ 4 ] == c ) )	 mode = c ;
		else if( 0 == c ) { // process optionse
			if( 5 == index ) { // stringmaxlen
				std::stringstream ss ;
				ss << optarg ;
				ss >> generate.stringmaxlen ;
			} else if( 6 == index ) { //stringcount
				std::stringstream ss ;
				ss << optarg ;
				ss >> generate.stringcount ; 
			} else if( 7 == index ) { //packetsize
				std::stringstream ss ;
				ss << optarg ;
				ss >> split.packetsize ;
			} else if( 8 == index ) { //infile
				const std::string s( optarg ) ;
				split.inputfilename.assign( s ) ;
				order.inputfilename.assign( s ) ;
				recv.inputfilename.assign( s ) ;
			} else if( 9 == index ) { // outfile
				const std::string s( optarg ) ;
				generate.filename.assign( s ) ;
				split.outputfilename.assign( s ) ;
				order.outputfilename.assign( s ) ;
				recv.outputfilename.assign( s ) ;
			}
		}
	} // end of parse

	if( opts[ 1 ] == mode ) { // generate
		if( generate.filename.empty() || 0 == generate.stringcount  || 0 == generate.stringmaxlen ) return false ;
		p_s.v.emplace<parsedstring_t::generatefile_st>( std::move( generate ) ) ;

		return true ;
	} else if( opts[ 2 ] == mode ) { //split
		if( split.inputfilename.empty() || split.outputfilename.empty() || 0 == split.packetsize ) return false ;
		p_s.v.emplace<parsedstring_t::splitdata_st>( std::move( split ) ) ;
		return true ;
	} else if( opts[ 3 ] == mode ) { // order
		if( order.inputfilename.empty() || order.outputfilename.empty()  ) return false ;
		p_s.v.emplace<parsedstring_t::order_st>( std::move( order ) ) ;

		return true ;
	} else if( opts[ 4 ] == mode ) { // recv
		if( recv.inputfilename.empty() || recv.outputfilename.empty()  ) return false ;
		p_s.v.emplace<parsedstring_t::receivepackets_st>( std::move( recv ) ) ;
		return true ;
	}

	std::cerr << "programm options is incorrect : " ;
	for( auto i = 1; i < argc; ++i ) std::cerr << argv[ i ] << " " ;
	std::cerr << std::endl ;

	usage( argv[ 0 ] ) ;

	return false ;
}


void parsecmdstring_t::usage( const char *name ) {
	std::cout << "usage " << name << "\n" <<
		"--help(-h) this message \n" << 
		"--generate(-g) --stringmaxlen <unsigned value> --stringcount<unsigned value> --outfile <filename>\n" <<
				"\t\tcreate file with sequence [stringcount] len of string random len [1:stringmaxlen]\n" <<
		"--split(-s) --packetsize <unsigned value> --infile<filename> --outfile<filename>\n" <<
				"\t\tsplit inputfile [infile] to packets with len [packetsize] , and write binary file [outfile]\n" <<
		"--order(-o) --infile<filename> --outfile<filename>\n" <<
				"\t\trandomly change order of packet in [infile] and write it to [outfile]\n" <<
		"--recv(-r) --infile<filename> --outfile<filename>\n" <<
				"\t\tread unrodered packets in [infile] and write sequence of string to [outfile]" <<
	std::endl ;
}
