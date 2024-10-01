#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
int main() {
    try {
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        websocket::stream<tcp::socket> ws(ioc);
        // פותר את הכתובת ומבצע חיבור לשרת
        auto const results = resolver.resolve("147.234.64.63","8082");
        net::connect(ws.next_layer(), results);
        // מבצע את לחיצת היד של WebSocket
        ws.handshake("147.234.64.63","/");
        // שליחת הודעה לשרת
        std::string message = "Hello from client!";
        ws.write(net::buffer(message));
        std::cout << "Sent: " << message << std::endl;
        // יצירת לוגיקת קריאה מתמשכת
        std::thread reader([&ws]() {
            beast::flat_buffer buffer;
            while (true) {
                beast::error_code ec;
                ws.read(buffer, ec);
                if (ec) {
                    if(ec == websocket::error::closed) {
                        std::cout << "Connection closed by server." << std::endl;
                        break;
                    }
                    std::cerr << "Read failed: " << ec.message() << std::endl;
                    break;
                }
                std::cout << "Received: " << beast::buffers_to_string(buffer.data()) << std::endl;
                buffer.consume(buffer.size());
            }
        });
        // שליחת הודעות נוספות (למשל מהמשתמש)
        std::string input;
        while (std::getline(std::cin, input)) {
            if(input == "exit") break;
            ws.write(net::buffer(input));
        }
        // סגירת החיבור
        ws.close(websocket::close_code::normal);
        reader.join();
        std::cout << "Connection closed." << std::endl;
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}