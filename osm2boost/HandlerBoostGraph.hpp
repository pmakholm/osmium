#include <map>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <google/sparsetable>


namespace Osmium {
    namespace Handler {

	template<typename Graph>
	class BoostGraph : callbacks {

            typedef typename Graph::vertex_descriptor vertex_t;
            typedef typename Graph::edge_descriptor   edge_t;
            typedef typename Graph::vertex_bundled    vertex_bundle;
            typedef typename Graph::edge_bundled      edge_bundle;

            typedef boost::bundle_property_map<Graph,vertex_t,vertex_bundle,osm_object_id_t> NodeIdMap;
            typedef boost::bundle_property_map<Graph,vertex_t,vertex_bundle,std::pair<int32_t,int32_t> > NodeCoordMap;
            typedef boost::bundle_property_map<Graph,edge_t,edge_bundle,std::string> EdgeTypeMap;

            Graph        &graph;
            NodeIdMap    id_map;       // Graph::vertex_descriptor -> osm_object_id_t
            NodeCoordMap coord_map;    // Graph::vertex_descriptor -> std::pair<int32_t,int32_t>
            EdgeTypeMap  type_map;     // Graph::edge_descriptor   -> std::string

            std::map<long,vertex_t> rev_id;

        protected:

            static int32_t double_to_fix(double c) {
                return c * 10000000;
            }

            static double fix_to_double(int32_t c) {
                return ((double)c) / 10000000;
            }
 
        public:

            BoostGraph(Graph& g, NodeIdMap ids, NodeCoordMap coords, EdgeTypeMap types) : graph(g), id_map(ids), coord_map(coords), type_map(types) {
                //max_id = 100000;
                //rev_id.resize(max_id+1);
            };

            void callback_node(Osmium::OSM::Node *object) {
                const osm_object_id_t osm_id = object->get_id();
                std::pair<int32_t, int32_t> coord( double_to_fix( object->get_lon() ), double_to_fix( object->get_lat() ) );

                vertex_t v = add_vertex(graph);

                rev_id[osm_id] = v;
                put(id_map, v, (int) osm_id);
                put(coord_map, v, coord);
            }

            void callback_way(Osmium::OSM::Way *object) {
                std::string type = object->get_tag_by_key("highway");
                for ( int i = 0 ; i < object->node_count() - 1 ; i++ ) {
                    vertex_t src = rev_id[ object->nodes[i] ];
                    vertex_t dst = rev_id[ object->nodes[i+1] ];

                    // Assume the graph is bidirectionalS, but all ways are bidirectional...
                    edge_t   e1  = add_edge(src, dst, graph).first;
                    edge_t   e2  = add_edge(dst, src,graph).first;

                    put(type_map, e1, type);
                    put(type_map, e2, type);
                }
            }
     
        }; // class BoostGraph
    }  // namespace Handler
}    // namespare Osminum

