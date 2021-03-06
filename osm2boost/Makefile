#------------------------------------------------------------------------------
#
#  Osmium nodedensity makefile
#
#------------------------------------------------------------------------------

CXX = g++

CXXFLAGS = -g
#CXXFLAGS = -O3

CXXFLAGS += -std=c++0x -Wall -W -Wredundant-decls -Wdisabled-optimization -pedantic
CXXFLAGS += -Wno-deprecated
#CXXFLAGS += -Wpadded -Winline

CXXFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CXXFLAGS += -DWITH_GEOS $(shell geos-config --cflags)

CXXFLAGS += -I../include -I../pbf

LDFLAGS = -L/usr/local/lib -lexpat -lpthread
LDFLAGS += $(shell geos-config --libs)

LIB_PROTOBUF = -lz -lprotobuf
LIB_BOOST    = -lboost_serialization

CPP = osmium.cpp

OBJ = osmium.o 
OBJ_PBF = fileformat.pb.o osmformat.pb.o

HPP = osmium.hpp Osm.hpp OsmObject.hpp OsmNode.hpp OsmWay.hpp OsmRelation.hpp XMLParser.hpp wkb.hpp Handler.hpp PBFParser.hpp

SRC_CPP = $(patsubst %,../src/%,$(CPP))
SRC_OBJ = $(patsubst %,../src/%,$(OBJ))
SRC_HPP = $(patsubst %,../include/%,$(HPP))

SRC_OBJ_PBF = $(patsubst %,../pbf/%,$(OBJ_PBF))

.PHONY: all clean doc protobuf

all: protobuf osm2boost

protobuf:
	$(MAKE) -C ../pbf CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"

%.o: ../src/%.cpp $(SRC_HPP)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

osm2boost: osm2boost.cpp $(OBJ) $(SRC_HPP) HandlerBoostGraph.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(OBJ) $(SRC_OBJ_PBF) $(LDFLAGS) $(LIB_PROTOBUF) $(LIB_BOOST)

install:
	install -m 755 -g root -o root osm2boost $(DESTDIR)/usr/bin/nodedensity
    
clean:
	rm -f *.o osm2boost

