#ifndef ESP32SOCKET_H
#define ESP32SOCKET_H

#include "Isocket.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "../include/packet.h"
#include "../../logger/logger.h"

class ESP32Socket : public ISocket
{
private:
    // Wi-Fi configuration
    const char *ssid;
    const char *password;
    int s_retry_num;
    bool wifi_connected;
    logger log;

    // Event handler function to handle Wi-Fi events like disconnects, reconnects, etc.
    static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        auto *self = static_cast<ESP32Socket *>(arg);
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            self->s_retry_num++;
            self->log.logMessage(logger::LogLevel::ERROR, "Reconnecting to WiFi...");
            esp_wifi_connect();
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            self->log.logMessage(logger::LogLevel::INFO, "Connected to WiFi successfully");
            self->wifi_connected = true;
            self->s_retry_num = 0;
        }
    }

public:
    ESP32Socket(const char *wifi_ssid, const char *wifi_password, int retry_num = 10)
        : ssid(wifi_ssid), password(wifi_password), s_retry_num(retry_num), wifi_connected(false), log("communication")
    {
        initWifi();
    }

    void initWifi()
    {
        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        wifi_config_t wifi_config = {};
        strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
        strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());

        esp_event_handler_instance_t instance_any_id;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &ESP32Socket::wifi_event_handler, this, &instance_any_id));

        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ESP32Socket::wifi_event_handler, this, &instance_got_ip));

        esp_wifi_connect();
    }

    void waitForWifiConnection()
    {
        int retry_count = 0;

        while (!wifi_connected && retry_count < s_retry_num)
        {
            delay(1000);
            log.logMessage(logger::LogLevel::INFO, "Waiting for WiFi connection...");
            retry_count++;
        }

        if (!wifi_connected)
            log.logMessage(logger::LogLevel::ERROR, "Failed to connect to WiFi.");
    }

    int socket(int domain, int type, int protocol) override
    {
        int sockFd = lwip_socket(domain, type, protocol);
        if (sockFd < 0)
            log.logMessage(logger::LogLevel::ERROR, ("Socket creation error: " + std::string(strerror(errno))).c_str());
        else
            log.logMessage(logger::LogLevel::INFO, ("Created a client socket: " + std::to_string(sockFd)).c_str());

        return sockFd;
    }

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override
    {
        int res = lwip_setsockopt(sockfd, level, optname, optval, optlen);
        if (res < 0)
            log.logMessage(logger::LogLevel::ERROR, ("Setsockopt failed: " + std::string(strerror(errno))).c_str());
        else
            log.logMessage(logger::LogLevel::INFO, ("Set socket options: " + std::to_string(sockfd)).c_str());

        return res;
    }

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int res = lwip_bind(sockfd, addr, addrlen);
        if (res < 0)
        {
            log.logMessage(logger::LogLevel::ERROR, ("Bind failed: " + std::string(strerror(errno))).c_str());
            close(sockfd);
        }
        else
            log.logMessage(logger::LogLevel::INFO, ("Bind success: " + std::to_string(sockfd)).c_str());

        return res;
    }

    int listen(int sockfd, int backlog) override
    {
        int res = lwip_listen(sockfd, backlog);
        if (res < 0)
            log.logMessage(logger::LogLevel::ERROR, ("Listen failed: " + std::string(strerror(errno))).c_str());
        else
            log.logMessage(logger::LogLevel::INFO, ("Listen success: " + std::to_string(sockfd)).c_str());

        return res;
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
    {
        int newSocket = lwip_accept(sockfd, addr, addrlen);
        if (newSocket < 0)
            log.logMessage(logger::LogLevel::ERROR, ("Accept failed: " + std::string(strerror(errno))).c_str());
        else
            log.logMessage(logger::LogLevel::INFO, ("Connection succeed to client socket number: " + std::to_string(sockfd)).c_str());

        return newSocket;
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        waitForWifiConnection();

        if (!wifi_connected)
        {
            log.logMessage(logger::LogLevel::ERROR, "WiFi not connected. Cannot connect to server.");
            return -1;
        }

        int res = lwip_connect(sockfd, addr, addrlen);
        if (res < 0)
            log.logMessage(logger::LogLevel::ERROR, ("Connection Failed: " + std::string(strerror(errno))).c_str());
        else
            log.logMessage(logger::LogLevel::INFO, "Connection succeeded.");

        return res;
    }

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override
{
    ssize_t res = lwip_recv(sockfd, buf, len, flags);
    const Packet *p = static_cast<const Packet *>(buf);

    if (res < 0) {
        log.logMessage(logger::LogLevel::ERROR, "Error in socket " + std::to_string(sockfd) + ": " + std::string(strerror(errno)) + " (SrcID: " + std::to_string(p->header.SrcID) + ", DestID: " + std::to_string(p->header.DestID) + ")");
    } else if (res == 0) {
        log.logMessage(logger::LogLevel::INFO, "Connection closed in socket " + std::to_string(sockfd) + ": " + std::string(strerror(errno)) + " (SrcID: " + std::to_string(p->header.SrcID) + ", DestID: " + std::to_string(p->header.DestID) + ")");
    } else {
        if (!p->header.DLC) {
            log.logMessage(logger::LogLevel::INFO, "Received packet number " + std::to_string(p->header.PSN) + " of messageId: " + std::to_string(p->header.ID) + " (SrcID: " + std::to_string(p->header.SrcID) + ")");
        } else {
            log.logMessage(logger::LogLevel::INFO, "Received packet number " + std::to_string(p->header.PSN) + " of messageId: " + std::to_string(p->header.ID) + " Data: " + p->pointerToHex(p->data, p->header.DLC) + " (SrcID: " + std::to_string(p->header.SrcID) + ", DestID: " + std::to_string(p->header.DestID) + ")");
        }
    }

    return res;
}


    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override
    {
        ssize_t res = lwip_send(sockfd, buf, len, flags);
        const Packet *p = static_cast<const Packet *>(buf);
        if (res < 0)
            log.logMessage(logger::LogLevel::ERROR, std::to_string(p->header.SrcID).c_str(), std::to_string(p->header.DestID).c_str(), ("Sending packet number " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + " " + std::string(strerror(errno))).c_str());
        else
            log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID).c_str(), std::to_string(p->header.DestID).c_str(), ("Sending packet number " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + " Data: " + p->pointerToHex(p->data, p->header.DLC)).c_str());

        return res;
    }

    int close(int sockfd) override
    {
        return lwip_close(sockfd);
    }

};

#endif // ESP32SOCKET_H