#include "BoostGraph.h"

#include <functional>
#include <typeinfo>

/**
 * @class BoostGraph
 * @brief A small wrapper for Boost Graphs.
 *
 * **Small BGL doc**
 * @code
 * // g - Graph, adjacency_list etc.
 *
 * VertexID vid = boost::add_vertex(g); // vertex_descriptor vid;
 * g[vid] // - structura definită de mine în .h
 * g[vid].index = index;
 *
 * // asta o fi accesarea prin indice (vertex_index)
 * a = vertex(0, g);
 * b = vertex(1, g);
 *
 * vertex_iterator vi = boost::vertices(g).first; // început
 * vertices_size_type count = (model->count() - un int sau ceva de acest fel);
 * typename graph_traits<Graph>::vertex_iterator vi_end; // ca și vertex_iterator, cred
 *
 * edge_descriptor ed; // pt. muchii
 * bool ok;
 * boost::tie(ed, ok) = boost::add_edge(a, b, g);
 *
 * boost::tie(vi, vi_end) = vertices(g);
 * // echivalent cu vi = vertices(g).first; vi_end = vertices(g).second
 *
 * for (vertices_size_type i = 0; i < count; ++i, ++vi) {
 *   boost::put(vertex_index, g, *vi, i);
 *   // setez proprietatea vertex_index a elementului *vi din graful g la i
 * }
 *
 * Graph(int x); // ctor - graph with x nodes
 * @endcode
 */

/**
 * @brief Creates a graph with @a vertices vertices.
 * @param vertices Number of vertices of the new graph.
 */
BoostGraph::BoostGraph(int vertices) :
    QObject()
{
    m_g = new Graph(vertices);
}

/**
 * @brief Sets the topology of the graph layout as the rectangle defined by
 * @a x, @a y, @a width and @a height.
 * @param x
 * @param y
 * @param width
 * @param height
 */
void BoostGraph::set_rect(qreal x, qreal y,
                          qreal width, qreal height)
{
    m_topology = new boost::rectangle_topology<>(x, y, width, height);
}

/** @brief Returns the position map of the graph.
 *
 * @returns The position map of the graph.
 *
 * @note Random info:
 * @code
 * #include <typeinfo>
 * typeid(variable).name(); // => Mangled type name. Use c++flit to demangle it.
 * @endcode
 *
 * @note Demangled return value of `get(vertex_position, *g)`:
 * @code
 * boost::adj_list_vertex_property_map
 * <boost::adjacency_list
 * <boost::listS, boost::listS, boost::directedS,
 * boost::property<boost::vertex_index_t, int,
 * boost::property<vertex_position_t, boost::convex_topology<2ul>::point,
 * boost::no_property> >, boost::property<boost::edge_weight_t, double,
 * boost::no_property>, boost::no_property, boost::listS>,
 * boost::convex_topology<2ul>::point,
 * boost::convex_topology<2ul>::point&,
 * vertex_position_t>
 * @endcode
 * And so I `typedef`'d it as `position_map`.
 */
position_map BoostGraph::get_pos()
{
    return boost::get(vertex_position, *m_g);
}

/**
 @brief Calls gursoy_atun_layout (Boost).

 @pre @a topology should be set with setRectangle().

 @param pos The position map in which to store the coordinates.

 If I ever want to use a circle layout:
 @code
 boost::circle_graph_layout(g, get(vertex_position, g), radius);
 @endcode

 Without `_T` at the end, these macros don't use trait detection but the actual
 given type (in this case, `Graph`):
 @code
 BGL_FORALL_VERTICES(v, g, Graph) { }
 BGL_FORALL_EDGES(e, g, Graph) { }
 @endcode
 */
void BoostGraph::do_layout(position_map pos)
{
    Q_ASSERT(m_topology != nullptr);

    boost::gursoy_atun_layout(*m_g, *m_topology, pos);
}

/**
 * @brief Returns the index of the vertex @a v.
 * @param v A vertex descriptor.
 * @return Index of the vertex @a v.
 */
int BoostGraph::vertex_index(vertex_descriptor v)
{
    return get(boost::vertex_index, *m_g, v);
}

/**
 * @brief Sets @a index as the index of the vertex @a v.
 * @param v A vertex descriptor
 * @param index The index to set for @a v.
 */
void BoostGraph::set_vertex_index(vertex_descriptor v, int index)
{
    // setez proprietatea vertex_index a elementului *vi
    // din graful g la i
    boost::put(boost::vertex_index, *m_g, v, index);
}

/**
 * @brief Returns the vertex descriptor at @a index.
 * @param index The index of the requested vertex descriptor.
 * @return The vertex descriptor at @a index.
 */
vertex_descriptor BoostGraph::descriptor_at(int index)
{
    return boost::vertex(index, *m_g);
}

/**
 * @brief Returns a reference to a pair of vertex iterators, the beginning and
 * the end of the vertices list.
 * @return A reference to a pair of vertex iterators, the beginning and the end
 * of the vertices list.
 */
std::pair<vertex_iterator, vertex_iterator> &BoostGraph::vertices()
{
    this->m_iterators = boost::vertices(*m_g);
    return this->m_iterators;
}

/**
 * @brief Returns the number of vertices in the graph.
 * @return The number of vertices in the graph.
 */
vertices_size_type BoostGraph::vertices_count()
{
    return boost::num_vertices(*m_g);
}
