#include <iostream>
#include <list>


#include "stringsequence_t.h"
#include "parseCmdString.h"

#include "order_t.h"
#include "splitter_t.h"
#include "recv_t.h"


void processaction( const parsedstring_t &p_s ) {
	stringsequence_t stringsequence ;

	if(	auto pval = std::get_if<parsedstring_t::generatefile_st>( &p_s.v ) ) {
		std::cout << "Generate file <" << pval->filename << "> with stringmaxlen " << pval->stringmaxlen << " with stringcount " << pval->stringcount << std::endl ;
		stringsequence.generatesequence( pval->stringmaxlen, pval->stringcount ) ;
		stringsequence.savetofile( pval->filename ) ;
		std::cout << "Done...\n" ;
	} else if( auto pval = std::get_if<parsedstring_t::splitdata_st>( &p_s.v ) ) {
		std::cout << "Split infile " << pval->inputfilename << " to packets with size " << pval->packetsize << " and write it to " << pval->outputfilename << std::endl ;
		stringsequence.fillfromfile( pval->inputfilename ) ;
		splitter_t splitter( pval->packetsize ) ;
		splitter.process( stringsequence.getsequence( ) ) ;

		splitter.savetofile( pval->outputfilename ) ;
	} else if( auto pval = std::get_if<parsedstring_t::order_st>( &p_s.v ) ) {
		std::cout << "Change order file infile " << pval->inputfilename << " and write it to " << pval->outputfilename << std::endl ;
		order_t order ;
		splitter_t splitter ;
		splitter.fillfromfile( pval->inputfilename ) ;
		order.process( splitter.getdata( ) ) ;
		splitter.savetofile( pval->outputfilename ) ;
	} else if( auto pval = std::get_if<parsedstring_t::receivepackets_st>( &p_s.v ) ) {
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
	} else throw std::runtime_error("Error! with std::variant!") ;
}


int main( int argc, char **argv ) {
	parsecmdstring_t parser ;

	if( parser.parsecmdstring( argc, argv ) ) {
		try {
			processaction( parser.get() ) ;
		} catch(  std::runtime_error &e ) {
			std::cerr << e.what( ) << std::endl ;
		}
	}
	return 0 ;
}
