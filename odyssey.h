#ifndef HEADER_H
#define HEADER_H

#include <utility>

#include <cstddef>
#include <cstring>

#include <random>

#include <vector>
#include <string>

#include "boost/beast.hpp"
#include "boost/asio.hpp"

#include "boost/json/src.hpp"


using namespace boost;

struct client_context {

		using websock_stream = beast::websocket::stream<asio::ip::tcp::socket>;
		using socket = asio::ip::tcp::socket;
        
		using client_id = std::size_t;
		using client_src = std::vector<std::uint8_t>;
		
        using ibuffer = beast::flat_buffer;
		using obuffer = beast::flat_buffer;

		using reqst_id = std::size_t;
		
		websock_stream ws;
		client_id id;
		client_src src;
		
		client_context (socket &&sock, client_id cid)
		: ws(std::move(sock)), id(cid), gen(std::random_device()()){}
		
		asio::awaitable<void> start() {
			try {
				// Устанавливаем настройки таймаутов
				ws.set_option(beast::websocket::stream_base::timeout::suggested(beast::role_type::server));

				// Асинхронное выполнение WebSocket-рукопожатия
				co_await ws.async_accept(asio::use_awaitable);
				std::cout << "Client " << id << " WebSocket handshake successful!\n";

				// Бесконечный цикл чтения пакетов от этого клиента
				while (true) {
					co_await handler();
				}
			}
			catch (beast::system_error const& se) {
				if (se.code() == beast::websocket::error::closed) {
					std::cout << "Client " << id << " disconnected normally.\n";
				} else {
					std::cerr << "Client " << id << " session error: " << se.code().message() << "\n";
				}
			}
			catch (std::exception const& e) {
				std::cerr << "Client " << id << " session exception: " << e.what() << "\n";
			}
		}
        
	private:
		asio::awaitable<void> handler () {
			
			co_await ws.async_read(ibuf, asio::use_awaitable);
			
			json::value js = json::parse(beast::buffers_to_string(ibuf.data()));
			
			ibuf.consume(ibuf.size());

			switch (str_to_reqid(js.at("type").as_string().c_str())) {
				
				case 0: {
					json::value j_obj = {
						{"type", "Coords"},
						{"data", {
							{"planets", json::array{
								{{"image", "pluto.png"},  {"x", 100}, {"y", 1e+2}, {"vx", 0.5},  {"vy", 1e-7}, {"r", 50}, {"width", 100}, {"height", 200}},
								{{"image", "earth.png"},  {"x", 300}, {"y", 1e+2}, {"vx", 0.05}, {"vy", 1e-3}, {"r", 80}, {"width", 100}, {"height", 200}}
							}},
							{"ships", json::array{
								{{"image", "kolymaga2.png"}, {"x", dist_pos(gen)}, {"y", dist_pos(gen)}, {"angle", 0}, {"vx", 0}, {"vy", 0}, {"w", 0}, {"width", 100}, {"height", 200}}
							}},
							{"self_index", 5}
						}}
					};
					std::string response_str = json::serialize(j_obj);
					 
					obuf.consume(obuf.size());

					asio::buffer_copy(obuf.prepare(response_str.size()), asio::buffer(response_str));

					obuf.commit(response_str.size());
					
					co_await ws.async_write(obuf.data(), asio::use_awaitable);
					break;
				}
				default:
					break;
			}
			
			
			co_return;
		}
		
		static reqst_id str_to_reqid (const char *str) {
			
			std::size_t i = 0;
			for (const char *req : str_req) {
				if (std::strcmp(str, req) == 0)
					return reqst_id(i);
				i++;
			}
			return reqst_id(i);	
		}

		ibuffer ibuf;
		obuffer obuf;
        
		static inline const char* str_req[] {
			"New"
		};
		
		std::mt19937 gen;

		static inline std::uniform_real_distribution<double> dist_pos{0.0, 1000.0};
};




#endif /* HEADER_H */ 