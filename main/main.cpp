#include <esp_heap_caps_init.h>

#include <core/Core.h>
#include <core/system/telemetry/TelemetryService.h>
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <exchange/espnow/EspNowExchange.h>

#ifdef CONFIG_EXCHANGE_BUS_SPI
#include <exchange/spi/SpiExchange.h>
#endif

#ifdef CONFIG_EXCHANGE_BUS_UART
#include <exchange/uart/UartExchange.h>
#endif


class Console : public Application<Console>,
                public TMessageSubscriber<Console, ExchangeMessage, TimerEvent<100>, TimerEvent<101> > {
    FreeRTOSTimer _timerEspNow;
    FreeRTOSTimer _timerUart;

public:
    Console() = default;

    void handle(const TimerEvent<100> &event) {
        std::string ping = "ping " + std::to_string(esp_timer_get_time());
        exchange_message_t buf{
            .if_type = ESP_INTERNAL_IF,
            .if_num = 0x08,
            .pkt_type = PACKET_TYPE_COMMAND_REQUEST,
            .payload = (void *) ping.data()
        };
        buf.length = ping.size() + 1;
        esp_logi(esp_now_exchange, "Send: %s", ping.c_str());
        getRegistry().getService<EspNowExchange>()->send(buf);
    }

    void handle(const TimerEvent<101> &event) {
        std::string ping = "ping " + std::to_string(esp_timer_get_time());
        exchange_message_t buf{
            .if_type = ESP_INTERNAL_IF,
            .if_num = 0x08,
            .pkt_type = PACKET_TYPE_COMMAND_REQUEST,
            .payload = (void *) ping.data()
        };
        buf.length = ping.size() + 1;
        esp_logi(uart_exchange, "Send: %s", ping.c_str());
        getRegistry().getService<UartExchange>()->send(buf);
    }

    void handle(const ExchangeMessage &event) {
        const auto &msg = event.message;
        std::string_view ping((char *) (msg.payload + msg.hdr->offset), msg.hdr->payload_len);
        esp_logi(console, "Recv: %s, %d", ping.data(), ping.length());
        if (ping.starts_with("ping")) {
            std::string pong("pong");
            pong += ping.substr(4);
            exchange_message_t buf{
                .if_type = ESP_INTERNAL_IF,
                .if_num = 0x02,
                .pkt_type = PACKET_TYPE_COMMAND_RESPONSE,
            };
            buf.payload = (void *) pong.data();
            buf.length = pong.length() + 1;

            esp_logi(console, "Send: %s, %d", pong.c_str(), pong.length());

            event.exchange.send(buf);
        }
    }

protected:
    void userSetup() override {
        getRegistry().getEventBus().subscribe(shared_from_this());
#ifdef CONFIG_EXCHANGE_BUS_SPI
        getRegistry().create<SpiExchange>();
#endif
#ifdef CONFIG_EXCHANGE_BUS_UART
        getRegistry().create<UartExchange>();
        _timerUart.fire<101>(10000, true);
#endif
#ifdef CONFIG_EXCHANGE_BUS_I2C
        getRegistry().create<I2cExchange>();
#endif
#ifdef CONFIG_EXCHANGE_BUS_ESP_NOW
        getRegistry().create<EspNowExchange>();
        _timerEspNow.fire<100>(10000, true);
#endif
    }
};

static std::shared_ptr<Console> app;

extern "C" void app_main() {
    esp_log_level_set("*", ESP_LOG_INFO);
    const size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    const size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %zu/%zu", free, total);

    app = std::make_shared<Console>();
    app->setup();
}
