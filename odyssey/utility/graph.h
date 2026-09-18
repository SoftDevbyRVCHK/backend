#ifndef ODYSSEY_FRAMEWORK_UTILITY_GRAPH_H
#define ODYSSEY_FRAMEWORK_UTILITY_GRAPH_H

#include <vector>
#include <cstddef>
#include <utility>

namespace odyssey {
    
    namespace utility {
        
        /*
            простой граф.
        */
        
        /*template <typename Vertex, typename Edge, typename indexT = std::uint32_t>
        class graph {
        
        public:
            struct incidence {
                indexT enter;
                indexT exit;
                Edge   edge;
                
                template <typename E>
                incidence (E&& e, indexT enter, indexT exit) : edge(std::forward<E>(e)), enter(enter), exit(exit) {}
            };
            
            graph () = default;
        
            const std::vector<Vertex>& getVertexes () const {return vrtxs;}
            const std::vector<incidence>& getIncidences () const {return incds;}
            
            template <typename V>
            void push_vertex (V&& vertex) {
                vrtxs.push_back(std::forward<V>(vertex));
            }
            template <typename... Args>
            void emplace_vertex (Args&&... args) {
                vrtxs.emplace_back(std::forward<Args>(args)...);         
            }
            
            template <typename I>
            void push_incidence (I&& incidenc) {
                incds.push_back(std::forward<I>(incidenc));
            }
            template <typename... Args>
            void emplace_incidence (Args&&... args) {
                incds.emplace_back(std::forward<Args>(args)...);
            }
            
            void pop_vertex () {
                indexT lastV = vrtxs.size() - 1;
                indexT i = 0;
                
                while(i != incds.size())
                    if(incds[i].enter == lastV || incds[i].exit == lastV) {
                        incds[i] = std::move(incds.back());
                        pop_incidence();
                    }
                    else
                        i++;
                vrtxs.pop_back();
            }
            
            void pop_incidence () {
                incds.pop_back();
            }
            
            ~graph () = default;
        
        private:    
            std::vector<Vertex>    vrtxs;
            std::vector<incidence> incds;
        };*/
        
        template <typename Vertex, typename Edge, typename IndexT = std::uint32_t>
        class graph {
        public:
            struct edgeData {
            
                Edge edge;
                IndexT target;
            };

            struct vertexData {
                Vertex vertex;
                std::vector<edgeData> edges;
            };    
            
            graph() = default;
            
            const std::vector<VertexData>& getVrtxData () {return vrtxs;}
            
            
        
        private:
            std::vector<vertexData> vrtxs;
        };

    }
}

#endif /* ODYSSEY_FRAMEWORK_UTILITY_GRAPH_H */