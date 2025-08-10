#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <hal/nrf_saadc.h>

LOG_MODULE_REGISTER(main);

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 200

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

/* ADC channel from devicetree */
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 0);

int main(void)
{
	int ret;
	bool led_state = true;
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	if (!device_is_ready(led.port)) {
		LOG_ERR("LED GPIO device not ready");
		return 0;
	}

	if (!device_is_ready(adc_channel.dev)) {
		LOG_ERR("ADC device not ready");
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		LOG_ERR("Error configuring GPIO");
		return 0;
	}

	ret = adc_channel_setup_dt(&adc_channel);
	if (ret < 0) {
		LOG_ERR("Error setting up ADC channel: %d", ret);
		return 0;
	}

	/* Initialize ADC sequence from devicetree */
	ret = adc_sequence_init_dt(&adc_channel, &sequence);

	if (ret < 0) {
		LOG_ERR("Error initializing ADC sequence: %d", ret);
		return 0;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			LOG_ERR("Error toggling LED");
			return 0;
		}

		led_state = !led_state;

		ret = adc_read_dt(&adc_channel, &sequence);
		if (ret < 0) {
			LOG_ERR("Could not read ADC: %d", ret);
		} else {
			int32_t mv = buf;
			ret = adc_raw_to_millivolts_dt(&adc_channel, &mv);
			if (ret < 0) {
				LOG_ERR("Error converting to millivolts: %d", ret);
			} else {
				LOG_INF("Hello World! %s, ADC(P0.02) = %d mV", CONFIG_BOARD, mv);
			}
		}

		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
