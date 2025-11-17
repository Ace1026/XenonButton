#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f4xx_hal.h"
#include "xenon_button.h"

#define KEY_ON                        1
#define KEY_OFF                       0

#define KEY1_DOWNSTATE                0
#define KEY2_DOWNSTATE                0
#define KEY3_DOWNSTATE                0
#define KEY4_DOWNSTATE                0
#define KEY5_DOWNSTATE                0

#define KEY1_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY1_GPIO_PIN                 GPIO_PIN_0
#define KEY1_GPIO                     GPIOE

#define KEY2_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY2_GPIO_PIN                 GPIO_PIN_1
#define KEY2_GPIO                     GPIOE

#define KEY3_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY3_GPIO_PIN                 GPIO_PIN_2
#define KEY3_GPIO                     GPIOE

#define KEY4_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY4_GPIO_PIN                 GPIO_PIN_3
#define KEY4_GPIO                     GPIOE

#define KEY5_RCC_CLK_ENABLE           __HAL_RCC_GPIOE_CLK_ENABLE
#define KEY5_GPIO_PIN                 GPIO_PIN_4
#define KEY5_GPIO                     GPIOE

void KEY_GPIO_Init(void);
uint8_t GetPinStateOfKey1(void);
uint8_t GetPinStateOfKey2(void);
uint8_t GetPinStateOfKey3(void);
uint8_t GetPinStateOfKey4(void);
uint8_t GetPinStateOfKey5(void);

#define	GET_KEY_VALUE(X)				(0x01ul << X)
#define	GET_KEY_COMBO_VALUE(X,Y)		((0x01ul << X)|(0x01ul << Y))

typedef enum
{
	USER_BUTTON_0 = 0,
	USER_BUTTON_1,
	USER_BUTTON_2,
	USER_BUTTON_3,
	USER_BUTTON_4,
	USER_BUTTON_5,
	USER_BUTTON_6,
	USER_BUTTON_7,
	USER_BUTTON_8,
	USER_BUTTON_9,

	USER_BUTTON_COMBO_0,
	USER_BUTTON_COMBO_1,
	USER_BUTTON_COMBO_2,
	USER_BUTTON_COMBO_3,

	USER_BUTTON_MAX,
} user_button_t;

void app_xbtn_init(void);
void key_process_task(void);

#endif  // __BSP_KEY_H__
