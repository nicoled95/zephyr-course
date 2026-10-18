#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(my_driver, LOG_LEVEL_INF);

#define DT_DRV_COMPAT my_driver

struct my_driver_cfg {
	struct gpio_dt_spec led;
};

static int my_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct my_driver_cfg *cfg = dev->config;

	ARG_UNUSED(chan);
	return gpio_pin_set_dt(&cfg->led, 1);   /* LED ON */
}

static int my_driver_channel_get(const struct device *dev, enum sensor_channel chan,
				 struct sensor_value *val)
{
	const struct my_driver_cfg *cfg = dev->config;
	int ret;

	ARG_UNUSED(chan);
	ret = gpio_pin_set_dt(&cfg->led, 0);   /* LED OFF */
	if (ret < 0) {
		return ret;
	}
	val->val1 = 0;
	val->val2 = 0;
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
	DEVICE_DT_INST_DEFINE(inst, my_driver_init, NULL, NULL,             \
			      &my_driver_cfg_##inst, POST_KERNEL,           \
			      CONFIG_SENSOR_INIT_PRIORITY, &my_driver_api);

DT_INST_FOREACH_STATUS_OKAY(MY_DRIVER_INIT)