#include "bsp_key.h"

static volatile uint32_t KeyFlag = 0;

extern uint32_t HAL_GetTick(void);

void KEY_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	KEY1_RCC_CLK_ENABLE();
	KEY2_RCC_CLK_ENABLE(); 
	KEY3_RCC_CLK_ENABLE();
	KEY4_RCC_CLK_ENABLE();	
	KEY5_RCC_CLK_ENABLE(); 

	GPIO_InitStruct.Pin = KEY1_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(KEY1_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = KEY2_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(KEY2_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = KEY3_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(KEY3_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = KEY4_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(KEY4_GPIO, &GPIO_InitStruct);


	GPIO_InitStruct.Pin = KEY5_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(KEY5_GPIO, &GPIO_InitStruct);
}

uint8_t GetPinStateOfKey1(void)
{
	if(HAL_GPIO_ReadPin(KEY1_GPIO, KEY1_GPIO_PIN) == KEY1_DOWNSTATE)
	{
		return KEY_ON;
	}
	else
	{
		return KEY_OFF;
	}
}

uint8_t GetPinStateOfKey2(void)
{
	if(HAL_GPIO_ReadPin(KEY2_GPIO, KEY2_GPIO_PIN) == KEY2_DOWNSTATE)
	{
		return KEY_ON;
	}
	else
	{
		return KEY_OFF;
	}
}

uint8_t GetPinStateOfKey3(void)
{
	if(HAL_GPIO_ReadPin(KEY3_GPIO, KEY3_GPIO_PIN) == KEY3_DOWNSTATE)
	{
		return KEY_ON;
	}
	else
	{
		return KEY_OFF;
	}
}

uint8_t GetPinStateOfKey4(void)
{
	if(HAL_GPIO_ReadPin(KEY4_GPIO, KEY4_GPIO_PIN) == KEY4_DOWNSTATE)
	{
		return KEY_ON;
	}
	else
	{
		return KEY_OFF;
	}
}

uint8_t GetPinStateOfKey5(void)
{
	if(HAL_GPIO_ReadPin(KEY5_GPIO, KEY5_GPIO_PIN) == KEY5_DOWNSTATE)
	{
		return KEY_ON;
	}
	else
	{
		return KEY_OFF;
	}
}

static xenon_btn_t btns[] = {
    BTN_BUTTON_INIT(USER_BUTTON_1),
    BTN_BUTTON_INIT(USER_BUTTON_2),
    BTN_BUTTON_INIT(USER_BUTTON_3),
    BTN_BUTTON_INIT(USER_BUTTON_4),
	BTN_BUTTON_INIT(USER_BUTTON_5),
};


static xenon_btn_t btns_combo[] = {
    BTN_BUTTON_COMBO_INIT(USER_BUTTON_COMBO_1),
    BTN_BUTTON_COMBO_INIT(USER_BUTTON_COMBO_2),
	BTN_BUTTON_COMBO_INIT(USER_BUTTON_COMBO_3),
};

static void key_state_get_all(void)
{
	KeyFlag = 0;
	KeyFlag |= (GetPinStateOfKey1() << USER_BUTTON_1);
	KeyFlag |= (GetPinStateOfKey2() << USER_BUTTON_2);
	KeyFlag |= (GetPinStateOfKey3() << USER_BUTTON_3);
	KeyFlag |= (GetPinStateOfKey4() << USER_BUTTON_4);
	KeyFlag |= (GetPinStateOfKey5() << USER_BUTTON_5);
}

static uint8_t prv_btn_get_state(xenon_btn_t *btn)
{
	if(btn == NULL) return 0;

	switch (btn->id)
	{
		case USER_BUTTON_1:
			return ((KeyFlag & GET_KEY_VALUE(USER_BUTTON_1)) == GET_KEY_VALUE(USER_BUTTON_1));
		case USER_BUTTON_2:
			return ((KeyFlag & GET_KEY_VALUE(USER_BUTTON_2)) == GET_KEY_VALUE(USER_BUTTON_2));
		case USER_BUTTON_3:
			return ((KeyFlag & GET_KEY_VALUE(USER_BUTTON_3)) == GET_KEY_VALUE(USER_BUTTON_3));
		case USER_BUTTON_4:
			return ((KeyFlag & GET_KEY_VALUE(USER_BUTTON_4)) == GET_KEY_VALUE(USER_BUTTON_4));
		case USER_BUTTON_5:
			return ((KeyFlag & GET_KEY_VALUE(USER_BUTTON_5)) == GET_KEY_VALUE(USER_BUTTON_5));

		case USER_BUTTON_COMBO_1:
			return ((KeyFlag & GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_2)) == GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_2));
		case USER_BUTTON_COMBO_2:
			return ((KeyFlag & GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_3)) == GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_3));
		case USER_BUTTON_COMBO_3:
			return ((KeyFlag & GET_KEY_COMBO_VALUE(USER_BUTTON_3,USER_BUTTON_4)) == GET_KEY_COMBO_VALUE(USER_BUTTON_3,USER_BUTTON_4));

		default:
			return 0;
	}
}

static void prv_btn_event(xenon_btn_t *btn, xenon_btn_evt_t evt)
{
	if(btn == NULL) return;
	uint16_t keepalive_cnt = 0;
	keepalive_cnt = xbtn_keepalive_get_count(btn);
	uint16_t click_cnt = 0;
	click_cnt = xbtn_click_get_count(btn);
	uint32_t tick = HAL_GetTick();
	
	switch(btn->id)
	{
		case USER_BUTTON_1:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_1 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_1 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_1 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;
			
		case USER_BUTTON_2:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_2 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_2 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_2 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;            
			
		case USER_BUTTON_3:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_3 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_3 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_3 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;            
			
		case USER_BUTTON_4:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_4 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_4 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_4 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;            
			
		case USER_BUTTON_5:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_5 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_5 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_5 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;            

		case USER_BUTTON_COMBO_1:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_COMBO_1 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_COMBO_1 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_COMBO_1 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;
			
		case USER_BUTTON_COMBO_2:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_COMBO_2 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else iif(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_COMBO_2 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_COMBO_2 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;

		case USER_BUTTON_COMBO_3:
			if(evt == BTN_EVT_ONRELEASE && keepalive_cnt == 0)
			{
				printf("USER_BUTTON_COMBO_3 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else iif(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_COMBO_3 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, keepalive_cnt);
			}
			else if(evt == BTN_EVT_ONCLICK)
			{
				printf("USER_BUTTON_COMBO_3 , ONCLICK ,  tick = %lu , click_cnt = %u \r\n", tick, click_cnt);
			}
			break;

		default:
			break;
	}
}

void app_xbtn_init(void)
{
	xbtn_init(btns, BTN_ARRAY_SIZE(btns), btns_combo, BTN_ARRAY_SIZE(btns_combo), prv_btn_get_state, prv_btn_event);
	printf("app_xbtn_init\r\n");
}

void key_process_task(void)
{
	key_state_get_all();
	xbtn_process(HAL_GetTick());
}



