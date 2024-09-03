#include <esp_heap_caps_init.h>

#include <core/Core.h>
#include <core/system/storage/NvsStorage.h>
#include <core/system/telemetry/TelemetryService.h>
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <core/system/console/Console.h>
#include <core/system/exchange/spi/SpiExchange.h>
#include <soc/gpio_num.h>
#include <driver/gpio.h>

class Console : public Application<Console> {
public:
    Console() = default;
protected:
    void userSetup() override {
        ///getRegistry().create<NvsStorage>();
        //getRegistry().create<UartConsoleService>();
        getRegistry().create<SpiExchange>();
        //getRegistry().create<WifiService>();
//        getRegistry().create<TelemetryService>();
//        auto &mqtt = getRegistry().create<MqttService>();
//        mqtt.addJsonProcessor<Telemetry>("/user/telemetry");
    }
};

static std::shared_ptr<Console> app;

extern "C" void app_main() {
    // esp_log_level_set("*", ESP_LOG_DEBUG);
    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %zu/%zu", free, total);

    app = std::make_shared<Console>();
    app->setup();
}
