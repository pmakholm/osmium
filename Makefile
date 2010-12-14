#------------------------------------------------------------------------------
#
#  Osmium Makefile
#
#------------------------------------------------------------------------------

CXX = g++

CXXFLAGS = -O3 -Wall -W -Wredundant-decls -Wdisabled-optimization -pedantic
#CXXFLAGS = -g -fPIC
#CXXFLAGS = -Wpadded -Winline

LDFLAGS = -lexpat -lpthread

LIB_V8       = -L/usr/local/lib -lv8
LIB_SQLITE   = -lsqlite3
LIB_GD       = -lgd -lpng -lz -lm
LIB_SHAPE    = -lshp
LIB_PROTOBUF = -lz -lprotobuf

CPP = XMLParser.cpp wkb.cpp osmium.cpp
CPP_TAGSTAT = HandlerStatistics.cpp
CPP_JS = JavascriptTemplate.cpp

OBJ = XMLParser.o wkb.o protobuf/fileformat.pb.o protobuf/osmformat.pb.o osmium.o 
OBJ_TAGSTAT = HandlerStatistics.o
OBJ_JS = JavascriptTemplate.o

HPP = osmium.hpp Osm.hpp OsmObject.hpp OsmNode.hpp OsmWay.hpp OsmRelation.hpp XMLParser.hpp wkb.hpp StringStore.hpp Handler.hpp HandlerStatistics.hpp HandlerTagStats.hpp HandlerNodeLocationStore.hpp PBFParser.hpp

SRC_CPP = $(patsubst %,src/%,$(CPP))
SRC_OBJ = $(patsubst %,src/%,$(OBJ))
SRC_HPP = $(patsubst %,src/%,$(HPP))

SRC_CPP_TAGSTAT = $(patsubst %,src/%,$(CPP_TAGSTAT))
SRC_OBJ_TAGSTAT = $(patsubst %,src/%,$(OBJ_TAGSTAT))

SRC_CPP_JS = $(patsubst %,src/%,$(CPP_JS))
SRC_OBJ_JS = $(patsubst %,src/%,$(OBJ_JS))
SRC_HPP_JS = $(patsubst %,src/%,$(HPP_JS))
SRC_HPP_JS = $(wildcard src/*Javascript*.hpp)

#sources = src/*.cpp
#include $(sources:.cpp=.d)

.PHONY: all clean doc

all: osmium_js osmium_tagstats

#src/%.d: src/%.cpp
#	$(SHELL) -ec "$(CXX) -MM -MT '$(patsubst %.cpp,%.o,$<) $(patsubst %.cpp,%.d,$<)' $< >$@; [ -s $@ ] || rm -f $@"

src/protobuf/%.pb.o: src/protobuf/%.pb.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

src/protobuf/%.pb.cc: src/protobuf/%.proto
	protoc --proto_path=src/protobuf --cpp_out=src/protobuf $<

src/%.o: src/%.cpp $(SRC_HPP)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

osmium_js: src/osmium_js.cpp $(SRC_OBJ) $(SRC_OBJ_JS) $(SRC_HPP) $(SRC_HPP_JS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(SRC_OBJ) $(SRC_OBJ_JS) $(LDFLAGS) $(LIB_PROTOBUF) $(LIB_V8) $(LIB_SHAPE)

osmium_tagstats: src/osmium_tagstats.cpp $(SRC_OBJ) $(SRC_OBJ_TAGSTAT) $(SRC_HPP)
	$(CXX) $(CXXFLAGS) -o $@ $< $(SRC_OBJ) $(SRC_OBJ_TAGSTAT) $(LDFLAGS) $(LIB_PROTOBUF) $(LIB_GD) $(LIB_SQLITE)
    
clean:
	rm -f src/protobuf/*format.pb.* src/*.o src/*.d osmium_js osmium_tagstats

doc: doc/html/files.html

doc/html/files.html: src/*.hpp src/*.cpp
	doxygen

