#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
public:
    WebSocketSession(tcp::socket socket)
        : ws_(std::move(socket)) {}
    void start()
{
    std::cout << "Waiting for handshake..." << std::endl;

    ws_.async_accept(
        [self = shared_from_this()](beast::error_code ec)
        {
            if (!ec)
            {
                std::cout << "Handshake successful!" << std::endl;

                // הוספת כותרת CORS
                self->ws_.set_option(websocket::stream_base::decorator(
                    [](websocket::request_type &req)
                    {
                        req.set(beast::http::field::access_control_allow_origin, "*");
                    }));

                self->read_message();
            }
            else
            {
                std::cerr << "Handshake failed: " << ec.message() << std::endl;
            }
        });
}
private:
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    void read_message() {
        ws_.async_read(buffer_,
            [self = shared_from_this()](beast::error_code ec, std::size_t) {
                if (!ec) {
                    std::cout << "Received: " << beast::buffers_to_string(self->buffer_.data()) << std::endl;
                    // שליחת תשובה ללקוח
                    std::string response = "Hello from server!";
                    beast::error_code write_ec;
                    self->ws_.write(net::buffer(response), write_ec);
                    if(write_ec) {
                        std::cerr << "Write failed: " << write_ec.message() << std::endl;
                        return;
                    }
                    self->buffer_.consume(self->buffer_.size());  // Clear the buffer
                    self->read_message(); // המשך קריאת הודעות
                } else {
                    std::cerr << "Read failed: " << ec.message() << std::endl;
                }
            });
    }
};
class WebSocketServer {
public:
    WebSocketServer(net::io_context& ioc, tcp::endpoint endpoint)
        : acceptor_(ioc, endpoint) {
        accept_connection();
    }
private:
    tcp::acceptor acceptor_;
    void accept_connection() {
        acceptor_.async_accept(
            [this](beast::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<WebSocketSession>(std::move(socket))->start();
                } else {
                    std::cerr << "Accept failed: " << ec.message() << std::endl;
                }
                accept_connection();
            });
    }
};
int main() {
    try {
        net::io_context ioc;
        // Listen on all available interfaces (0.0.0.0) on port 8080
        tcp::endpoint endpoint(tcp::v4(), 8080);
        WebSocketServer server(ioc, endpoint);
        ioc.run(); // Run the io_context to handle async operations
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
