#ifndef MY_DRIVER_H_
#define MY_DRIVER_H_

#include <zephyr/device.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Extension API: change the inverted-polarity parameter at runtime */
int my_driver_set_inverted(const struct device *dev, bool inverted);

#ifdef __cplusplus
}
#endif

#endif /* MY_DRIVER_H_ */