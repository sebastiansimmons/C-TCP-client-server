
#include "cache.hh"
// Using some code from the example at https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/client/sync/http_client_sync.cpp
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <charconv>
#include <string.h>
#include <memory>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
namespace pt = boost::property_tree;
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>


class Cache::Impl {
  public:
    Impl(std::string host, std::string port): host_(host), port_(port), version_(11), resolver_(ioc_), stream_(ioc_), results_(resolver_.resolve(host_, port_)) {
        stream_.connect(results_);
        req_.version(version_);
        req_.set(http::field::host, host_);
        req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    }
    
    ~Impl() {
    	// Gracefully close the socket
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
    }

    void set(key_type key, Cache::val_type val, Cache::size_type size) {
    	auto const target = "/"+ key + "/" + val;
    	send_request(http::verb::put, target);
        
        std::cout << "Request: " << req_ << std::endl;

        // Send the HTTP request to the remote host
        http::write(stream_, req_);

        // Receive the HTTP response
        http::read(stream_, buffer_, res_);

    	return;
        
    }
    Cache::val_type get(key_type key, Cache::size_type& val_size) {

    	auto const target = "/"+ key;
    	
    	send_request(http::verb::get, target);
    	
        // copybuffer to string
        std::string body = res_.body();

		// Create a stringstream of the response so the property tree can interpret it as json
		std::stringstream ss;
		ss << body << std::endl << "\0";

		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);

		auto value = pt.get<std::string>("value");
		
        char *ret_val = new char[value.length() + 1];     // Potential memory leak. TEST THIS
        strncpy ( ret_val, value.c_str(),value.length() + 1);
        val_size = value.length() + 1;

    	return ret_val;
    }
    bool del(key_type key) {
    	return false;

    }
    Cache::size_type space_used() const {
    	
        Cache::size_type space_used = 32;
    	
        return space_used;

    }
    void reset() {
    	return;
    }
  private:
  	std::string host_;
  	std::string port_;
  	int version_;
  	net::io_context ioc_;
  	//beast::tcp_stream* stream_;

  	tcp::resolver resolver_;
    beast::tcp_stream stream_;
	tcp::resolver::results_type results_;

    beast::flat_buffer buffer_; // (Must persist between reads)
    http::request<http::empty_body> req_;
    http::response<http::string_body> res_;

  	void send_request(http::verb method, std::string target){
  		res_ = http::response<http::string_body>();
    	
    	// Set up an HTTP GET request message
        req_.method(method);
        req_.target(target);

        //std::cout << "Request: " << req_ << std::endl;

        // Send the HTTP request to the remote host
        http::write(stream_, req_);

        // Receive the HTTP response
        http::read(stream_, buffer_, res_);
        return;
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
