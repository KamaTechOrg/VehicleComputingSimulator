#ifndef ESP32SOCKET_H
#define ESP32SOCKET_H
#ifdef INADDR_NONE
#undef INADDR_NONE
#endif

#include "Isocket.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

class ESP32Socket : public ISocket
{
private:
    const char *ssid;
    const char *password;
    int s_retry_num;
    bool wifi_connected; // Flag to indicate Wi-Fi connection status

    // Event handler function to handle Wi-Fi events like disconnects, reconnects, etc.
    void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            s_retry_num++;
            Serial.println("Reconnecting to WiFi...");
            esp_wifi_connect();
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            Serial.print("Got IP: ");
            Serial.println(ip4addr_ntoa(reinterpret_cast<const ip4_addr_t *>(&event->ip_info.ip)));
            wifi_connected = true;
            s_retry_num = 0;
        }
    }

public:
    ESP32Socket(const char *wifi_ssid, const char *wifi_password, int retry_num = 10)
        : ssid(wifi_ssid), password(wifi_password), s_retry_num(retry_num), wifi_connected(false)
    {
        initWifi();
    }

    void initWifi()
    {
        // Initialize NVS (Non-Volatile Storage), which is required for Wi-Fi configurations
        ESP_ERROR_CHECK(nvs_flash_init());

        // Initialize the default network interface (esp-netif) required for network operations
        ESP_ERROR_CHECK(esp_netif_init());

        // Create the default event loop required to handle Wi-Fi and other system events
        ESP_ERROR_CHECK(esp_event_loop_create_default());

        // Create the default station (STA) network interface for the ESP32 to connect to a Wi-Fi network
        esp_netif_create_default_wifi_sta();

        // Configure Wi-Fi initialization with default settings
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        // Set up the Wi-Fi configuration for the station (STA) mode
        wifi_config_t wifi_config = {};
        strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
        strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);

        // Set the ESP32 Wi-Fi mode to STA (station) mode to connect to an access point
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

        // Apply the Wi-Fi configuration for the STA mode
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

        // Start the Wi-Fi driver and begin the Wi-Fi process (e.g., scanning for networks, connecting)
        ESP_ERROR_CHECK(esp_wifi_start());

        // Register an event handler for any Wi-Fi event (ESP_EVENT_ANY_ID)
        // The event handler points to the wifi_event_handler method in the ESP32Socket class
        esp_event_handler_instance_t instance_any_id;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, [](void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
                                                            {
                                                                auto self = static_cast<ESP32Socket *>(arg);
                                                                self->wifi_event_handler(arg, event_base, event_id, event_data); }, this, &instance_any_id));

        // Register an event handler for the IP_EVENT_STA_GOT_IP event.
        // This event is triggered when the ESP32 gets an IP address after a successful Wi-Fi connection.
        esp_event_handler_instance_t instance_got_ip;
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, [](void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
                                                            { 
                                                                auto self = static_cast<ESP32Socket *>(arg);   // Cast to ESP32Socket class.
                                                                self->wifi_event_handler(arg, event_base, event_id, event_data); }, this, &instance_got_ip));

        // Initiate the connection to the Wi-Fi network.
        esp_wifi_connect();
    }

    void waitForWifiConnection()
    {
        int retry_count = 0;

        while (!wifi_connected && retry_count < s_retry_num)
        {
            delay(1000);
            Serial.println("Waiting for WiFi connection...");
            retry_count++;
        }

        if (!wifi_connected)
        {
            Serial.println("Failed to connect to WiFi.");
        }
    }

    int socket(int domain, int type, int protocol) override
    {
        int sockFd = lwip_socket(domain, type, protocol);
        if (sockFd < 0)
        {
            Serial.printf("Socket creation error: %s\n", strerror(errno));
        }
        else
        {
            Serial.printf("Created socket: %d\n", sockFd);
        }
        return sockFd;
    }

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override
    {
        int res = lwip_setsockopt(sockfd, level, optname, optval, optlen);
        if (res < 0)
        {
            Serial.printf("setsockopt failed: %s\n", strerror(errno));
        }
        return res;
    }

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int res = lwip_bind(sockfd, addr, addrlen);
        if (res < 0)
        {
            Serial.printf("Bind failed: %s\n", strerror(errno));
        }
        return res;
    }

    int listen(int sockfd, int backlog) override
    {
        int res = lwip_listen(sockfd, backlog);
        if (res < 0)
        {
            Serial.printf("Listen failed: %s\n", strerror(errno));
        }
        return res;
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
    {
        int newSocket = lwip_accept(sockfd, addr, addrlen);
        if (newSocket < 0)
        {
            Serial.printf("Accept failed: %s\n", strerror(errno));
        }
        return newSocket;
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        // Wait for Wi-Fi connection before attempting to connect
        waitForWifiConnection();

        if (!wifi_connected)
        {
            Serial.println("WiFi not connected. Cannot connect to server.");
            return -1;
        }

        int res = lwip_connect(sockfd, addr, addrlen);
        if (res < 0)
        {
            Serial.printf("Connection failed: %s\n", strerror(errno));
            Serial.printf("address: %s\n", addr->sa_data);
        }
        return res;
    }

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override
    {
        ssize_t res = lwip_recv(sockfd, buf, len, flags);
        if (res < 0)
        {
            Serial.printf("Receive failed: %s\n", strerror(errno));
        }
        return res;
    }

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override
    {
        ssize_t res = lwip_send(sockfd, buf, len, flags);
        if (res < 0)
        {
            Serial.printf("Send failed: %s\n", strerror(errno));
        }
        return res;
    }

    int close(int fd) override
    {
        int res = lwip_close(fd);
        if (res < 0)
        {
            Serial.printf("Close failed: %s\n", strerror(errno));
        }
        return res;
    }
};

#endif // ESP32SOCKET_H
