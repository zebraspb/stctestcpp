#pragma once

#include <stdint.h>
#include <vector>
#include <random>
#include <algorithm>

#include "packet_t.h"

class order_t {

public:
	void process( std::vector< std::vector<uint8_t> > &packets ) {
		std::random_device rd;
		std::mt19937 g( rd( ) );
 
		std::shuffle( packets.begin(), packets.end(), g );

		std::cout << "new packets order is : " ;
		for( auto& p : packets ) {
			packet_head_st *ph = reinterpret_cast<packet_head_st *> ( p.data( ) ) ;
			std::cout << ph->packet_num << " " ;
		}
		std::cout << std::endl;
	}
private:
} ;


