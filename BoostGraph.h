#pragma once
#ifndef BOOSTGRAPH_H
#define BOOSTGRAPH_H

#include <QtCore>

// http://qt-project.org/forums/viewthread/22993/
// https://bugreports.qt-project.org/browse/QTBUG-29331
// http://stackoverflow.com/questions/15056834/qt5-migration-and-boost-if-hpp-macro-argument-mismatch-bug
#ifndef Q_MOC_RUN

#include <boost/property_map/property_map.hpp>
#include <boost/config.hpp>

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/kamada_kawai_spring_layout.hpp>
#include <boost/graph/circle_layout.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/point_traits.hpp>
#include <boost/graph/gursoy_atun_layout.hpp>

#endif // Q_MOC_RUN

/**
 * @brief Point type of the square topology.
 */
typedef boost::square_topology<>::point_type point;
/**
 * @brief Enum-based type for vertices position property map.
 */
enum vertex_position_t { vertex_position };
namespace boost {
    BOOST_INSTALL_PROPERTY(vertex, position);
}

/*struct Vertex {
    int index;
};
struct Edge {
    QString label;
};*/
/**
 * @brief Type name for a adjacency_list fitting my needs.
 */
typedef boost::adjacency_list<
    boost::listS,
    boost::listS,
    boost::directedS,
    // vertex properties
    boost::property<boost::vertex_index_t, int,
    boost::property<vertex_position_t, point> >,
    // edge properties
    boost::property<boost::edge_weight_t, double> /*,
    Vertex,
    Edge*/
> Graph;

/**
 * @brief Type name for vertex descriptors of Graph.
 */
typedef Graph::vertex_descriptor VertexID;
/**
 * @brief Type name for edge descriptors of Graph.
 */
typedef Graph::edge_descriptor EdgeID;

/**
 * @brief Trait-based type name for Graph vertex descriptor.
 */
typedef boost::graph_traits<Graph>::vertex_descriptor
        vertex_descriptor;
/**
 * @brief Trait-based type name for Graph vertex iterator.
 */
typedef boost::graph_traits<Graph>::vertex_iterator
        vertex_iterator;
/**
 * @brief Trait-based type name for Graph vertices size type.
 */
typedef boost::graph_traits<Graph>::vertices_size_type
        vertices_size_type;
/**
 * @brief Trait-based type name for Graph edges size type.
 */
typedef boost::graph_traits<Graph>::edges_size_type
        edges_size_type;
/**
 * @brief Trait-based type name for Graph edge descriptor.
 */
typedef boost::graph_traits<Graph>::edge_descriptor
        edge_descriptor;

/**
 * @brief Position map type name for Graph.
 */
typedef boost::adj_list_vertex_property_map
    <Graph, point, point&, vertex_position_t>
    position_map;

class BoostGraph : public QObject
{
    Q_OBJECT
public:
    explicit BoostGraph(int vertices);

    void set_rect(qreal x, qreal y,
                  qreal width, qreal height);
    position_map get_pos();
    void do_layout(position_map pos);
    int vertex_index(vertex_descriptor v);
    void set_vertex_index(vertex_descriptor v, int index);
    vertex_descriptor descriptor_at(int index);
    std::pair<vertex_iterator, vertex_iterator> &vertices();
    vertices_size_type vertices_count();
    //void eachVertex(std::function<void()>);

    Graph *m_g;

signals:
    
public slots:

private:
    std::pair<vertex_iterator, vertex_iterator> m_iterators;
    boost::rectangle_topology<> *m_topology = nullptr;
};

#endif // BOOSTGRAPH_H
