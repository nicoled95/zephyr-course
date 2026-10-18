#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define SLEEP_TIME_MS 1000

static const struct device *const my_led = DEVICE_DT_GET(DT_NODELABEL(my_driver0));

int main(void)
{
	struct sensor_value val;

	if (!device_is_ready(my_led)) {
		LOG_ERR("my_driver not ready");
		return 0;
	}

	while (1) {
		int ret = sensor_sample_fetch(my_led);
		LOG_INF("sample_fetch ret=%d (LED ON)", ret);
		k_msleep(SLEEP_TIME_MS);

		ret = sensor_channel_get(my_led, SENSOR_CHAN_ALL, &val);
		LOG_INF("channel_get ret=%d (LED OFF)", ret);
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}