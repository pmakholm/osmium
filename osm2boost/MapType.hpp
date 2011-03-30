/*
 *  MapType.hpp
 *  GraphProject
 *
 *  Created by Peter Makholm on 10/03/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MapType_H
#define MapType_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/serialization/utility.hpp> // for std::pair

using namespace boost;

struct Node {
    int32_t osm_id;
    std::pair<int32_t,int32_t> coord;
};

struct Edge {
    int length;             // length in meters
    std::string type;       // road type
};

struct Graph {
    std::string name;
};

typedef adjacency_list<listS, vecS, bidirectionalS, Node, Edge, Graph> MapType;

namespace boost { namespace serialization {
    template<class Archive>
    void serialize(Archive & ar, Node & n, const unsigned int version) {
        ar & n.osm_id & n.coord;
    }
    
    template<class Archive>
    void serialize(Archive & ar, Edge & e, const unsigned int version) {
        ar & e.length & e.type ;
    }
    
    template<class Archive>
    void serialize(Archive & ar, Graph & g, const unsigned int version) {
        ar & g.name;
    }
}}
    
#endif
