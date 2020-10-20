#include <iostream>
#include <list>


#include "stringsequence_t.h"
#include "parseCmdString.h"

#include "order_t.h"
#include "splitter_t.h"
#include "recv_t.h"


void processaction( const parsedstring_t &p_s ) {
	stringsequence_t stringsequence ;

	switch( p_s.action ) {
		case parsedstring_t::action_en::help:			
		break;

		case parsedstring_t::action_en::generate:
		{
			auto pval = std::get_if<parsedstring_t::generatefile_st>( &p_s.v ) ;
			if( !pval )  throw std::runtime_error( "Error with generate get_if" ) ;
			std::cout << "Generate file <" << pval->filename << "> with stringmaxlen " << pval->stringmaxlen << " with stringcount " << pval->stringcount << std::endl ;
			stringsequence.generatesequence( pval->stringmaxlen, pval->stringcount ) ;
			stringsequence.savetofile( pval->filename ) ;
			std::cout << "Done...\n" ;
		}
		break ;

		case parsedstring_t::action_en::split:
		{
			auto pval = std::get_if<parsedstring_t::splitdata_st>( &p_s.v ) ;
			if( !pval )  throw std::runtime_error( "Error with generate get_if" ) ;

			std::cout << "Split infile " << pval->inputfilename << " to packets with size " << pval->packetsize << " and write it to " << pval->outputfilename << std::endl ;
			stringsequence.fillfromfile( pval->inputfilename ) ;
			splitter_t splitter( pval->packetsize ) ;
			splitter.process( stringsequence.getsequence( ) ) ;

			splitter.savetofile( pval->outputfilename ) ;
		}
		break ;

		case parsedstring_t::action_en::order:
		{
			auto pval = std::get_if<parsedstring_t::order_st>( &p_s.v ) ;
			if( !pval )  throw std::runtime_error( "Error with generate get_if" ) ;

			std::cout << "Change order file infile " << pval->inputfilename << " and write it to " << pval->outputfilename << std::endl ;
			order_t order ;
			splitter_t splitter ;
			splitter.fillfromfile( pval->inputfilename ) ;
			order.process( splitter.getdata( ) ) ;
			splitter.savetofile( pval->outputfilename ) ;
		}
		break ;

		case parsedstring_t::action_en::recv:
		{
			auto pval = std::get_if<parsedstring_t::receivepackets_st>( &p_s.v ) ;
			if( !pval )  throw std::runtime_error( "Error with generate get_if" ) ;

			std::cout << "Receive packets infile " << pval->inputfilename << " and write it to " << pval->outputfilename << std::endl ;
			recv_t recv ;
			splitter_t splitter ;
			splitter.fillfromfile( pval->inputfilename ) ;
			const auto &packets = splitter.getdata( ) ;
			for( const auto &p : packets ) {
				if( recv.process( p.data( ) ) ) {
					const auto &m = recv.getdata( ) ;
					for( const auto &s : m ) {
						//receive message not in a order.....

						//std::cout << "recv element num " << s.first << " [" << s.second << "]\n" ;
						
						// delete received messages
						//recv.resetsequence( ) ;
					}
				}
			}
			const auto &m = recv.getdata( ) ; //all packets allready sort by map key (elem_num)

			stringsequence_t seq ;
			for( const auto &s : m ) {
				std::cout << "recv element num " << s.first << " [" << s.second << "]\n" ;
				seq.addstring( std::string( s.second ) ) ;
			}
			seq.savetofile( pval->outputfilename ) ;
		}
		break ;
	}

}


int main( int argc, char **argv ) {
	parsecmdstring_t parser ;

	if( parser.parsecmdstring( argc, argv ) ) {
		try {
			processaction( parser.get() ) ;
		} catch(  std::runtime_error &e ) {
			std::cerr << e.what( ) << std::endl ;
		}
	} else {
		std::cerr << "programm options is incorrect : " ;
		for( auto i = 1; i < argc; ++i ) std::cerr << argv[ i ] << " " ;
		std::cerr << std::endl ;

		parser.usage( argv[ 0 ] ) ;
	}
	return 0 ;
}







#if 0


int main( void ) {
	std::list<std::string> sequence ;

	const int cnt_elems = 1000 ;
	std::random_device rd;
	std::uniform_int_distribution<int> dist( 0, 100 ) ;

	for( auto i = 0 ; i < cnt_elems; ++ i ) sequence.emplace_back( std::string( dist( rd ), 'A' ) ) ;
	
	auto it = sequence.begin( ) ;
	for( int i = 0; it != sequence.end( ); ++i ) {
		packet_t packet( i ) ;

		for( ;it != sequence.end( ); ++it ) {
			if( !packet.add_elem( it->data( ), it->size( ) ) ) break ;
		}
		auto p = packet.get_packet( ) ;
		sync_send( std::get<0>( p ), std::get<1>( p ) ) ;
	}

	return 0 ;
}
#endif

