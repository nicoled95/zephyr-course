#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#include "my_driver.h"

LOG_MODULE_REGISTER(my_driver, LOG_LEVEL_INF);

#define DT_DRV_COMPAT my_driver

struct my_driver_cfg {
	struct gpio_dt_spec led;
};

struct my_driver_data {
	bool inverted;   /* dynamic parameter, changed at runtime */
};

static int my_driver_write_led(const struct device *dev, bool on)
{
	const struct my_driver_cfg *cfg = dev->config;
	const struct my_driver_data *data = dev->data;

	return gpio_pin_set_dt(&cfg->led, data->inverted ? !on : on);
}

static int my_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	ARG_UNUSED(chan);
	return my_driver_write_led(dev, true);     /* LED ON */
}

static int my_driver_channel_get(const struct device *dev, enum sensor_channel chan,
				 struct sensor_value *val)
{
	int ret;

	ARG_UNUSED(chan);
	ret = my_driver_write_led(dev, false);     /* LED OFF */
	if (ret < 0) {
		return ret;
	}
	val->val1 = 0;
	val->val2 = 0;
	return 0;
}

/* Extension API implementation */
int my_driver_set_inverted(const struct device *dev, bool inverted)
{
	struct my_driver_data *data = dev->data;

	data->inverted = inverted;
	LOG_INF("inverted = %s", inverted ? "true" : "false");
	return 0;
}

static DEVICE_API(sensor, my_driver_api) = {
	.sample_fetch = my_driver_sample_fetch,
	.channel_get = my_driver_channel_get,
};

static int my_driver_init(const struct device *dev)
{
	const struct my_driver_cfg *cfg = dev->config;

	LOG_INF("==DRIVER_INITIALIZED==");

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}
	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define MY_DRIVER_INIT(inst)                                                \
	static const struct my_driver_cfg my_driver_cfg_##inst = {          \
		.led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),             \
	};                                                                  \
	static struct my_driver_data my_driver_data_##inst;                 \
	DEVICE_DT_INST_DEFINE(inst, my_driver_init, NULL,                   \
			      &my_driver_data_##inst, &my_driver_cfg_##inst,\
			      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,     \
			      &my_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MY_DRIVER_INIT)