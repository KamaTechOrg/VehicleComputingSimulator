#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "communication.h"
#include <thread>
#include <chrono>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class CommunicationTest {
public:
    CommunicationTest() {
        comm1 = new communication();
        comm2 = new communication();

        std::thread t1([&]() { socket1 = comm1->initConnection(8080); });
        std::thread t2([&]() { socket2 = comm2->initConnection(8081); });

        t1.join();
        t2.join();

        REQUIRE(socket1 != -1);
        REQUIRE(socket2 != -1);
    }

    ~CommunicationTest() {
        delete comm1;
        delete comm2;

        close(socket1);
        close(socket2);
    }

    communication* comm1;
    communication* comm2;
    int socket1;
    int socket2;
};

TEST_CASE_FIXTURE(CommunicationTest, "SendAndReceiveExactSizeMessage") {
    const char* message = "HelloWorld1234"; // 16 bytes, exactly PACKET_SIZE
    std::thread sender([&]() {
        comm1->sendMessages(socket1, (void*)message, strlen(message));
    });

    std::thread receiver([&]() {
        uint8_t buffer[communication::PACKET_SIZE] = {0};
        int valread = recv(socket2, buffer, communication::PACKET_SIZE, 0);
        CHECK(valread == 16);
        std::string received(buffer, buffer + valread);
        CHECK(received == message);
    });

    sender.join();
    receiver.join();
}

TEST_CASE_FIXTURE(CommunicationTest, "SendAndReceiveMultiplePackets") {
    const char* longMessage = "This message is longer than one packet and should be split into multiple packets.";
    std::thread sender([&]() {
        comm1->sendMessages(socket1, (void*)longMessage, strlen(longMessage));
    });

    std::thread receiver([&]() {
        std::string received;
        uint8_t buffer[communication::PACKET_SIZE] = {0};
        while (true) {
            int valread = recv(socket2, buffer, communication::PACKET_SIZE, 0);
            if (valread <= 0) break;
            received.append(buffer, buffer + valread);
        }
        CHECK(received == longMessage);
    });

    sender.join();
    receiver.join();
}

TEST_CASE_FIXTURE(CommunicationTest, "SendEmptyMessage") {
    const char* emptyMessage = "";
    std::thread sender([&]() {
        comm1->sendMessages(socket1, (void*)emptyMessage, strlen(emptyMessage));
    });

    std::thread receiver([&]() {
        uint8_t buffer[communication::PACKET_SIZE] = {0};
        int valread = recv(socket2, buffer, communication::PACKET_SIZE, 0);
        CHECK(valread == 0);
    });

    sender.join();
    receiver.join();
}

TEST_CASE_FIXTURE(CommunicationTest, "SendAndReceiveMessageWithDifferentSizes") {
    const std::vector<std::string> messages = {
        "short",                // small message
        "a bit longer message", // medium message
        std::string(communication::PACKET_SIZE, 'x'), // exactly one packet
        std::string(communication::PACKET_SIZE + 5, 'y') // larger than one packet
    };

    for (const auto& message : messages) {
        std::thread sender([&]() {
            comm1->sendMessages(socket1, (void*)message.c_str(), message.size());
        });

        std::thread receiver([&]() {
            std::string received;
            uint8_t buffer[communication::PACKET_SIZE] = {0};
            while (true) {
                int valread = recv(socket2, buffer, communication::PACKET_SIZE, 0);
                if (valread <= 0) break;
                received.append(buffer, buffer + valread);
            }
            CHECK(received == message);
        });

        sender.join();
        receiver.join();
    }
}

TEST_CASE_FIXTURE(CommunicationTest, "ConnectionErrorHandling") {
    // Close the sockets to simulate error
    close(socket1);
    close(socket2);

    int invalidSocket = socket(AF_INET, SOCK_STREAM, 0);
    REQUIRE(invalidSocket != -1);
    close(invalidSocket);

    // Attempt to send and receive on closed sockets
    std::thread sender([&]() {
        CHECK_THROWS_AS(comm1->sendMessages(socket1, (void*)"test", 4), std::system_error);
    });

    std::thread receiver([&]() {
        uint8_t buffer[communication::PACKET_SIZE] = {0};
        CHECK_THROWS_AS(recv(socket2, buffer, communication::PACKET_SIZE, 0), std::system_error);
    });

    sender.join();
    receiver.join();
}

TEST_CASE_FIXTURE(CommunicationTest, "PacketSizeHandling") {
    const std::string message = "PackSizeHandlingTest"; // 18 bytes, more than one packet
    std::thread sender([&]() {
        comm1->sendMessages(socket1, (void*)message.c_str(), message.size());
    });

    std::thread receiver([&]() {
        std::string received;
        uint8_t buffer[communication::PACKET_SIZE] = {0};
        while (true) {
            int valread = recv(socket2, buffer, communication::PACKET_SIZE, 0);
            if (valread <= 0) break;
            received.append(buffer, buffer + valread);
        }
        CHECK(received == message);
    });

    sender.join();
    receiver.join();
}

TEST_CASE_FIXTURE(CommunicationTest, "MultiThreadedSendReceive") {
    const std::string message = "ConcurrentTestMessage";

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&, i]() {
            if (i % 2 == 0) {
                comm1->sendMessages(socket1, (void*)message.c_str(), message.size());
            } else {
                std::string received;
                uint8_t buffer[communication::PACKET_SIZE] = {0};
                while (true) {
                    int valread = recv(socket2, buffer, communication::PACKET_SIZE, 0);
                    if (valread <= 0) break;
                    received.append(buffer, buffer + valread);
                }
                CHECK(received == message);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

int main(int argc, char** argv) {
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    int res = context.run();

    if (context.shouldExit())
        return res;

    return res;
}
