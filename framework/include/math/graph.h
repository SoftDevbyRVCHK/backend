#ifndef ODYSSEY_FRAMEWORK_MATH_GRAPH_H
#define ODYSSEY_FRAMEWORK_MATH_GRAPH_H

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <iterator>
#include <utility>

namespace odyssey {
    
    namespace framework {
    
        namespace math {
            
            //
            //  простой граф.
            //
        
            template <typename vertex_t, typename edge_t>
            struct graph {
                
                struct incidence_t {
                    
                    using container_t = std::unordered_map<std::size_t, edge_t>;
                    
                    vertex_t vertex;
                    
                    incidence_t () = default;
                    template <typename V>
                    incidence_t (V&& vtx, container_t&& explicit_arcs) : vertex(std::forward<V>(vtx)), arcs(std::move(explicit_arcs)) {}
                    
                    typename container_t::iterator begin () {
                        return arcs.begin();
                    }
                    typename container_t::const_iterator begin () const {
                        return arcs.cbegin();
                    }
                    typename container_t::iterator end () {
                        return arcs.end();
                    }
                    typename container_t::const_iterator end () const {
                        return arcs.cend();
                    }
                    
                    std::size_t size () const {
                        return arcs.size();
                    }
                    
                private:
                    container_t arcs;
                    
                    friend struct graph;
                };
                
                using graphT = std::vector<incidence_t>;
                
                graph () = default;
                
                typename graphT::iterator begin () {
                    return info.begin();
                }
                typename graphT::const_iterator begin () const {
                    return info.cbegin();
                }
                typename graphT::iterator end () {
                    return info.end();
                }
                typename graphT::const_iterator end () const {
                    return info.cend();
                }
                
                incidence_t& operator[] (std::size_t i) {
                    return info[i];
                }
                const incidence_t& operator[] (std::size_t i) const {
                    return info[i];
                }
                
                std::size_t size () const {
                    return info.size();
                }
                
                void clear () {
                    info.clear();
                }
                
                template <typename E>
                void add_edge(std::size_t from, std::size_t to, E&& edge) {
                    if (from < info.size() && to < info.size()) {
                        info[from].arcs[to] = std::forward<E>(edge);
                    }
                }
                void remove_edge(std::size_t from, std::size_t to) {
                    if (from < info.size() && to < info.size()) {
                        info[from].arcs.erase(to);
                    }
                }
                
                void remove_edges(std::size_t to) {
                    for (incidence_t& i : info) {
                        i.arcs.erase(to);
                    }
                }
                
                template <typename V>
                void push_vertex (V &&vtx) {
                    
                    info.emplace_back(std::forward<V>(vtx), typename incidence_t::container_t());
                }
                void pop_vertex () {
                    if (info.empty()) return;

                    remove_edges(info.size() - 1);

                    info.pop_back();
                }
                
            private:
                graphT info;
            };
        }
    }
}

#endif /* ODYSSEY_FRAMEWORK_MATH_GRAPH_H */