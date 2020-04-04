
#include "cache.hh"
// Using some code from the example at https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/client/sync/http_client_sync.cpp
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <charconv>
#include <string.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
namespace pt = boost::property_tree;
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>


class Cache::Impl {
  public:
    Impl(std::string host, std::string port): host_(host), port_(port), version_(11) {}
    
    ~Impl() {}

    void set(key_type key, Cache::val_type val, Cache::size_type size) {
    	// The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(host_, port_);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);
    	auto const target = "/"+ key + "/" + val;
    	// Set up an HTTP GET request message
        http::request<http::empty_body> req{http::verb::put, target, version_};
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        std::cout << "Request: " << req << std::endl;

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

		
    	return;
        
    }
    Cache::val_type get(key_type key, Cache::size_type& val_size) const {
    	// The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(host_, port_);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);
    	auto const target = "/"+ key;
    	// Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, target, version_};
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Write the message to standard out
        //auto body = std::to_string(res.body().data());
        

        // copybuffer to string

        std::string body = beast::buffers_to_string(res.body().data());
        const char c_body[] = body.data();


        std::cout <<"TESTING THIS" << body << std::endl;


        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    	return c_body;

    }
    bool del(key_type key) {
    	return false;

    }
    Cache::size_type space_used() const {
    	// The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(host_, port_);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);
    	auto const target = "/";
    	// Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::head, target, version_};
        req.set(http::field::host, host_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;
        std::cout << "Space-Used: " << res["Space-Used"] << std::endl;

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        auto strv_space_used = res["Space-Used"];

        uint32_t int_space_used;
        auto result = std::from_chars(strv_space_used.data(), strv_space_used.data() + strv_space_used.size(), int_space_used);
        Cache::size_type space_used = int_space_used;
    	
        return space_used;

    }
    void reset() {
    	return;
    }
  private:
  	std::string host_;
  	std::string port_;
  	int version_;
  	beast::tcp_stream* stream_;

  	void open_tcp_stream_() {
  		// The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);
        stream_ = &stream;

        // Look up the domain name
        auto const results = resolver.resolve(host_, port_);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);
  	}

  	void close_tcp_stream_() {
  		// Gracefully close the socket
        beast::error_code ec;
        stream_->socket().shutdown(tcp::socket::shutdown_both, ec);
  	}



  	bool test_tcp_stream(){
  		auto const host = host_;
        auto const port = port_;
        auto const target = "/";
        int version = 11;

        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);

        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::head, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};

        // If we get here then the connection is closed gracefully

        return true;
    }
};


// Ape the given Fridge example
Cache::Cache(std::string host, std::string port) : pImpl_(new Impl(host, port)) {}

Cache::~Cache() {}

void Cache::set(key_type key, val_type val, Cache::size_type size){
    pImpl_->set(key, val, size);
}

Cache::val_type Cache::get(key_type key, Cache::size_type& val_size) const {
    return pImpl_->get(key, val_size);
}

bool Cache::del(key_type key) {
    return pImpl_->del(key);
}

Cache::size_type Cache::space_used() const {
    return pImpl_->space_used();
}

void Cache::reset() {
    pImpl_->reset();
}
