/*
tcp_server.cc 
*/
#include <iostream>
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include "cache.hh"
#include "lru_evictor.hh"

// Unless specified otherwise, assume the following code was adapted from
// https://www.boost.org/doc/libs/1_72_0/libs/beast/example/http/server/async/http_server_async.cpp
// Because this is an asynchronous server using TCP like the prompt requests.
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/algorithm/string.hpp> 
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
// Declare global cache variable, initialise it in main
Cache* test_cache;


// Almost everything we have to implement is in the following function.

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<
    class Body, class Allocator,
    class Send>
void handle_request(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send) {
    // BEGIN STUFF I THINK WE CAN IGNORE
    // Returns a bad request response
    auto const bad_request =
    [&req](beast::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    // Returns a not found response
    auto const not_found =
    [&req](beast::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    // Returns a server error response (we aren't using this yet)
    /*auto const server_error =
    [&req](beast::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };*/
    // END STUFF I THINK WE CAN IGNORE

    switch (req.method()) {
        case http::verb::get:  //https://stackoverflow.com/questions/5685471/error-jump-to-case-label
            {
                uint32_t s = -1;    //TODO: REPLACE THIS WITH BETTER TYPING


                std::string target = std::string(req.target());
                target = target.substr(1, target.size());         // Removes leading '/'
                std::cout << "GET " << target << std::endl;


                const char* retp = test_cache->get(target, s);
                if (retp == nullptr) {
                    return send(not_found(target));
                } else {
                    // This is heavily relying on the cache data being valid (i.e. ending with \eof)
                    std::string rets(retp);
                    http::response<http::string_body> res{http::status::ok, req.version()};
                    // A note to the user: if you are relying on this, this is insanely vulernable to a code injection from, e.g., a malicious cache. (Since you supply req.target() yourself, any malicious behavior there is your own darn fault).
                    res.body() =  "{\"key\": \"" + target + "\", \"value\": \"" + rets + "\"}";
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "application/json");
                    //res.content_length(s);                                        // THIS SEEMS INCORRECT
                    res.keep_alive(req.keep_alive());
                    //res.set(http::field::content_type, "application/json");
                    res.prepare_payload();
                    return send(std::move(res));
                }
            }
        case http::verb::put:
            {
                std::vector<std::string> target; 
                boost::split(target, std::string(req.target()), boost::is_any_of("/"));
                
                std::cout << "PUT ";
                for (unsigned i = 1; i < target.size(); i++){
                    std::cout << target[i] << ' '; 
                }        
                std::cout << std::endl;
                
                std::string key = target[1];
                Cache::val_type val = target[2].data();
                Cache::size_type val_size = target[2].size() + 1;

                test_cache->set(key, val, val_size);

                http::response<http::empty_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.prepare_payload();
                return send(std::move(res));
            }
        case http::verb::delete_:
            {
                std::string target = std::string(req.target());
                target = target.substr(1, target.size());         // Removes leading '/'
                std::cout << "DELETE " << target << std::endl;

                test_cache->del(target);

                http::response<http::empty_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.prepare_payload();
                return send(std::move(res));
            }
        case http::verb::head:
            {
                std::cout << "HEAD" << std::endl;
                http::response<http::empty_body> res{http::status::ok, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.insert("Space-Used", "Value"); 
                res.set("Space-Used", std::to_string(test_cache->space_used()));
                res.keep_alive(req.keep_alive());
                res.set(http::field::content_type, "application/json");
                res.prepare_payload();
                return send(std::move(res));
            }
        case http::verb::post:
            {
                std::string target = std::string(req.target());
                if(target == "/reset"){
                    std::cout << "POST /reset " << target << std::endl;
                    test_cache->reset();
                    http::response<http::empty_body> res{http::status::ok, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.insert("Space-Used", "Value"); 
                    res.set("Space-Used", std::to_string(test_cache->space_used()));
                    res.keep_alive(req.keep_alive());
                    res.set(http::field::content_type, "application/json");
                    res.prepare_payload();
                    return send(std::move(res));
                } else{
                    return send(not_found(target));
                }
            }
        default:
            {
                // default for unknown requests
                return send(bad_request("Unknown HTTP-method"));
            }
    }
}




// IGNORE THIS STUFF
// Report a failure
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
class session : public std::enable_shared_from_this<session> {
    // This is the C++11 equivalent of a generic lambda.
    // The function object is used to send an HTTP message.
    struct send_lambda {
        session& self_;
        explicit send_lambda(session& self) : self_(self) {}

        template<bool isRequest, class Body, class Fields> void operator()(http::message<isRequest, Body, Fields>&& msg) const {
            // The lifetime of the message has to extend
            // for the duration of the async operation so
            // we use a shared_ptr to manage it.
            auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));

            // Store a type-erased version of the shared
            // pointer in the class to keep it alive.
            self_.res_ = sp;

            // Write the response
            http::async_write(self_.stream_, *sp, beast::bind_front_handler(&session::on_write, self_.shared_from_this(), sp->need_eof()));
        }
    };

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    std::shared_ptr<void> res_;
    send_lambda lambda_;

public:
    // Take ownership of the stream
    session(tcp::socket&& socket) : stream_(std::move(socket)), lambda_(*this) {}

    // Start the asynchronous operation
    void run() {
        // We need to be executing within a strand to perform async operations
        // on the I/O objects in this session. Although not strictly necessary
        // for single-threaded contexts, this example code is written to be
        // thread-safe by default.
        net::dispatch(stream_.get_executor(), beast::bind_front_handler(&session::do_read, shared_from_this()));
    }

    void do_read() {
        // Make the request empty before reading,
        // otherwise the operation behavior is undefined.
        req_ = {};

        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));

        // Read a request
        http::async_read(stream_, buffer_, req_, beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if(ec == http::error::end_of_stream) {
            return do_close();
        }
        if(ec) {
            return fail(ec, "read");
        }
        // Send the response
        handle_request(std::move(req_), lambda_);
    }

    void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if(ec) {
            return fail(ec, "write");
        }
        if(close) {
            // This means we should close the connection, usually because
            // the response indicated the "Connection: close" semantic.
            return do_close();
        }

        // We're done with the response so delete it
        res_ = nullptr;

        // Read another request
        do_read();
    }

    void do_close() {
        // Send a TCP shutdown
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
};


// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

  public:
    listener(
        net::io_context& ioc,
        tcp::endpoint endpoint)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc)) {

        beast::error_code ec;

        // Open the acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec) {
            fail(ec, "open");
            return;
        }

        // Allow address reuse
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if(ec) {
            fail(ec, "set_option");
            return;
        }

        // Bind to the server address
        acceptor_.bind(endpoint, ec);
        if(ec) {
            fail(ec, "bind");
            return;
        }

        // Start listening for connections
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if(ec) {
            fail(ec, "listen");
            return;
        }
    }

    // Start accepting incoming connections
    void run() {
        do_accept();
    }
  private:
    void do_accept() {
        // The new connection gets its own strand
        acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(&listener::on_accept, shared_from_this()));
    }
    void on_accept(beast::error_code ec, tcp::socket socket) {
        if(ec) {
            fail(ec, "accept");
        } else {
            // Create the session and run it
            std::make_shared<session>(std::move(socket))->run();
        }
        // Accept another connection
        do_accept();
    }
};


int main(int argc, char **argv){
    int opt;
    int m = 1024;
    int p = 6300;
    int t = 1;
    std::string s = "127.0.0.1";
    while ((opt = getopt(argc,argv,"m:s:p:t:d")) != EOF) {
        switch(opt)
        {
            case 'm': m = atoi(optarg); std::cout << "cache size set to "<< optarg << std::endl ; break;
            case 's': s = std::string(optarg); std::cout << "running server on "<< optarg << std::endl ; break;
            case 'p': p = atoi(optarg); std::cout << "with port "<< optarg << std::endl ; break;
            case 't': t = atoi(optarg); std::cout << "on "<< optarg << " threads" << std::endl ; break;
            default: /* '?' */
                   fprintf(stderr, "Usage: %s [-m <bytes>] maxmem [-s <ip>] server ip [-p <int>] port number [-t <int>] number of threads\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
        }
    }
    std::cout << "starting server..." << std::endl;
            
    
    // TODO: CLEAN THIS UP
    auto const address = net::ip::make_address(s);
    auto const portna = static_cast<unsigned short>(p);
    Lru_evictor* evictor_lru = new Lru_evictor();
    test_cache = new Cache(m, 0.75, evictor_lru);
    // The io_context is reportedly required for all I/O
    net::io_context ioc{t};
    std::make_shared<listener>(ioc, tcp::endpoint{address, portna})->run();
    ioc.run();
    return 0;
}
