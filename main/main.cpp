#include <esp_heap_caps_init.h>

#include <core/Core.h>
#include <core/system/telemetry/TelemetryService.h>
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <core/system/exchange/spi/SpiExchange.h>
#include <core/system/exchange/i2c/I2cExchange.h>
#include <core/system/exchange/uart/UartExchange.h>
#include <driver/gpio.h>
#include "led/LedStripService.h"

#include "config.h"

class Console : public Application<Console> {
public:
    Console() = default;

protected:
    void userSetup() override {
        ///getRegistry().create<NvsStorage>();
        //getRegistry().create<UartConsoleService>();
        //getRegistry().create<UartExchange>()
        //getRegistry().create<SpiExchange>();
        getRegistry().create<I2cExchange>();
        //getRegistry().create<WifiService>();

        // auto &mqtt = getRegistry().create<MqttService>();
        // mqtt.addJsonProcessor<Telemetry>("/user/telemetry");

        //getRegistry().create<TelemetryService>();
        // auto &led1 = getRegistry().create<LedStripService<Service_App_LedStrip1, 17, 8> >();
        // auto &led2 = getRegistry().create<LedStripService<Service_App_LedStrip2, 16, 8> >();
        //
        //
        // led1.setColor(0, 7, LedColor{255, 255, 255});
        // led1.refresh();
        // led2.setColor(0, 7, LedColor{0, 255, 0});
        // led2.refresh();
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
