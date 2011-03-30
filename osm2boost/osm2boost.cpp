#include <osmium.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "HandlerBoostGraph.hpp"
#include "MapType.hpp"

MapType map;
typedef graph_traits<MapType>::vertex_iterator vertex_iter;

Osmium::Handler::BoostGraph<MapType> *osmium_handler_boost_graph;

void node_handler(Osmium::OSM::Node *node) {
    osmium_handler_boost_graph->callback_node(node);
}

void way_handler(Osmium::OSM::Way *way) {
    osmium_handler_boost_graph->callback_way(way);
}

struct callbacks *setup_callbacks() {
    static struct callbacks cb;
    cb.node             = node_handler;
    cb.way              = way_handler;
    return &cb;
}

bool debug;

int main(int argc, char *argv[]) {
    debug = false;

    osmium_handler_boost_graph = new Osmium::Handler::BoostGraph<MapType>(map, get(&Node::osm_id, map), get(&Node::coord, map), get(&Edge::type, map) );
    Osmium::OSM::Node     *node     = new Osmium::OSM::Node;
    Osmium::OSM::Way      *way      = new Osmium::OSM::Way;
    Osmium::OSM::Relation *relation = new Osmium::OSM::Relation;

    parse_osmfile(argv[1], setup_callbacks(), node, way, relation);

        std::pair<vertex_iter, vertex_iter> vp; 
        for (vp = vertices(map); vp.first != vp.second; ++vp.first) {
            MapType::vertex_descriptor v = *vp.first;
            std::cout << map[v].osm_id << ": (lat: " << map[v].coord.first << ", lon: " << map[v].coord.second << ") deg: " << out_degree(v,map) << std::endl;
        }


    std::ofstream os( argv[2], std::ofstream::out );
    boost::archive::text_oarchive ar(os);

    ar << map;

    return 0;
}


