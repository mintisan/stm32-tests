#include "button.h"

/* */

xQueueHandle xQueue;
bool exti0_falling;

/* */

void exti0_isr(void)
{
	unsigned char msg;

	exti_reset_request(EXTI0);

	if (exti0_falling) {
		msg = 'D';
		xQueueSendToFrontFromISR(xQueue, &msg, NULL);
		exti0_falling = false;
		exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
	} else {
		msg = 'E';
		xQueueSendToFrontFromISR(xQueue, &msg, NULL);
		exti0_falling = true;
		exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
	}
}

void button_init(void)
{
	xQueue = xQueueCreate( 10, sizeof( unsigned char ) );
	exti0_falling = false;
}

void button_task(void *Parameters)
{
	unsigned char c;

	gpio_clear(GPIOD, GPIO14);

	while(1) {
		xQueueReceive(xQueue, &c, 10000);

		switch (c) {
			case 'E':
				gpio_set(GPIOD, GPIO15);
				break;
			case 'D':
				gpio_clear(GPIOD, GPIO15);
				break;
			default:
				break;
		}
	}
}