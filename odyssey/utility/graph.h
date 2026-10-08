#ifndef ODYSSEY_UTILITY_GRAPH_H
#define ODYSSEY_UTILITY_GRAPH_H

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <iterator>
#include <utility>

namespace odyssey {
    
    namespace utility {
        
        //
        //  простой граф.
        //
        
        template <typename Vertex, typename Edge>
        struct graph {
            
            struct incidence {
                
                using incidenceT = std::unordered_map<std::size_t, Edge>;
                
                Vertex vertex;
                
                incidence () = default;
                template <typename V>
                incidence (V&& vtx, incidenceT&& explicit_arcs) : vertex(std::forward<V>(vtx)), arcs(std::move(explicit_arcs)) {}
                
                typename incidenceT::iterator begin () {
                    return arcs.begin();
                }
                typename incidenceT::const_iterator begin () const {
                    return arcs.cbegin();
                }
                typename incidenceT::iterator end () {
                    return arcs.end();
                }
                typename incidenceT::const_iterator end () const {
                    return arcs.cend();
                }
                
                std::size_t size () const {
                    return arcs.size();
                }
                
            private:
                incidenceT arcs;
                
                friend struct graph;
            };
            
            using graphT = std::vector<incidence>;
            
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
            
            incidence& operator[] (std::size_t i) {
                return info[i];
            }
            const incidence& operator[] (std::size_t i) const {
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
                for (incidence& i : info) {
                    i.arcs.erase(to);
                }
            }
            
            template <typename V>
            void push_vertex (V &&vtx) {
                
                info.emplace_back(std::forward<V>(vtx), typename incidence::incidenceT());
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

#endif /* ODYSSEY_UTILITY_GRAPH_H */