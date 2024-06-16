#include <esp_heap_caps_init.h>

#include <core/Core.h>
#include <core/system/telemetry/TelemetryService.h>
#include <core/system/wifi/WifiService.h>
#include <core/system/mqtt/MqttService.h>
#include <core/system/console/Console.h>

class Console : public Application<Console> {
public:
    Console() = default;
protected:
    void userSetup() override {
        getRegistry().getEventBus().subscribe(shared_from_this());
        getRegistry().create<TelemetryService>();
        getRegistry().create<WifiService>();
        auto &mqtt = getRegistry().create<MqttService>();
        mqtt.addJsonProcessor<Telemetry>("/user/telemetry");

        getRegistry().create<UartConsoleService>();
    }
};

static std::shared_ptr<Console> app;

extern "C" void app_main() {
    size_t free = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    size_t total = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
    esp_logi(app, "heap: %zu/%zu", free, total);

    app = std::make_shared<Console>();
    app->setup();
}
