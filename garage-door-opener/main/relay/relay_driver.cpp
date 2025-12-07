#include <esp_log.h>
#include <stdlib.h>
#include <string.h>
#include <esp_timer.h>

#include <esp_matter.h>
#include <app-common/zap-generated/attributes/Accessors.h>

#include "relay_driver.h"

using namespace chip::app::Clusters;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "relay_driver";

constexpr gpio_num_t RELAY_GPIO = GPIO_NUM_1;

// Timer data structure
typedef struct {
    relay_handle_t relay;
    uint16_t endpoint_id;
} relay_timer_context_t;

static esp_timer_handle_t relay_timer = NULL;
static relay_timer_context_t relay_timer_context;

// Timer callback to turn off relay and update Matter attribute
static void relay_auto_off_callback(void* arg)
{
    relay_timer_context_t* ctx = (relay_timer_context_t*)arg;

    ESP_LOGI(TAG, "Auto-off timer expired for endpoint %d", ctx->endpoint_id);

    // Turn off the relay GPIO
    relay_gpio_set_on_off(ctx->relay, false);

    // Update the Matter on_off attribute
    esp_matter_attr_val_t val = esp_matter_bool(false);
    attribute_t *attribute = attribute::get(ctx->endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    if (attribute) {
        attribute::update(ctx->endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id, &val);
    }
}

static esp_err_t app_driver_relay_set_power(relay_handle_t handle, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    relay_gpio_set_on_off(handle,val->val.b);
    ESP_LOGI(TAG, "Relay set power: %d", val->val.b);
    return err;
}

esp_err_t app_driver_relay_set_defaults(uint16_t endpoint_id)
{
    esp_err_t err = ESP_OK;
    void *priv_data = endpoint::get_priv_data(endpoint_id);
    relay_handle_t relay = (relay_handle_t)priv_data;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Setting power */
    attribute_t *attribute  = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    attribute::get_val(attribute, &val);
    err |= app_driver_relay_set_power(relay, &val);

    return err;
}

relay_driver_handle_t app_driver_relay_init()
{
    /* Initialize relay */
    relay_handle_t relay = relay_create(RELAY_GPIO);

    /* Create timer for this relay */
    if (relay_timer == NULL) {
        relay_timer_context.relay = relay;
        relay_timer_context.endpoint_id = 0; // Will be set when used

        esp_timer_create_args_t timer_args = {
            .callback = &relay_auto_off_callback,
            .arg = &relay_timer_context,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "relay_auto_off"
        };
        esp_err_t err = esp_timer_create(&timer_args, &relay_timer);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to create timer for relay: %s", esp_err_to_name(err));
        }
    }

    return (relay_driver_handle_t)relay;
}

esp_err_t app_driver_attribute_update(relay_driver_handle_t driver_handle, uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (cluster_id == OnOff::Id) {
        relay_handle_t relay = (relay_handle_t)driver_handle;
        ESP_LOGI(TAG, "Relay");
        if (attribute_id == OnOff::Attributes::OnOff::Id) {
            err = app_driver_relay_set_power(relay, val);
            if (val->val.b)
            {
                // Update endpoint_id in context
                relay_timer_context.endpoint_id = endpoint_id;

                // Stop timer if it's already running
                if (esp_timer_is_active(relay_timer)) {
                    esp_timer_stop(relay_timer);
                }

                // Start the 1-second timer
                err = esp_timer_start_once(relay_timer, 1000000); // 1 second in microseconds
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to start timer: %s", esp_err_to_name(err));
                    return err;
                }
                ESP_LOGI(TAG, "Started auto-off timer for relay");
            }
            else
            {
                // Relay turned off, stop timer if running
                if (esp_timer_is_active(relay_timer)) {
                    esp_timer_stop(relay_timer);
                    ESP_LOGI(TAG, "Stopped auto-off timer for relay");
                }
            }
        }
    }
    return err;
}
