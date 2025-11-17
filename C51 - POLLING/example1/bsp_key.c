#include "bsp_key.h"

// Each key corresponds to one bit in the register, with a one-to-one mapping from high to low
// KeyFlag corresponds to Key31, Key30, ..., Key1, Key0
// The corresponding bit is 1 when the key is valid and 0 when invalid
extern volatile unsigned long int xdata KeyFlag;

static unsigned int data key_value = 0;
static unsigned int idata key_keepalive_cnt = 0;

static void key_state_get_all(void)
{
	key_value = 0;

	if (KeyFlag & GET_KEY_VALUE(TK1))
	{
		key_value |= GET_KEY_VALUE(USER_BUTTON_1);
	}
	if (KeyFlag & GET_KEY_VALUE(TK2))
	{
		key_value |= GET_KEY_VALUE(USER_BUTTON_2);
	}
	if (KeyFlag & GET_KEY_VALUE(TK3))
	{
		key_value |= GET_KEY_VALUE(USER_BUTTON_3);
	}
	if (KeyFlag & GET_KEY_VALUE(TK4))
	{
		key_value |= GET_KEY_VALUE(USER_BUTTON_4);
	}
	if (KeyFlag & GET_KEY_VALUE(TK5))
	{
		key_value |= GET_KEY_VALUE(USER_BUTTON_5);
	}
}

unsigned char btn_get_state_fn(unsigned char id)
{
	switch (id)
	{
		case USER_BUTTON_1:
			return ((key_value & GET_KEY_VALUE(USER_BUTTON_1)) == GET_KEY_VALUE(USER_BUTTON_1));
		case USER_BUTTON_2:
			return ((key_value & GET_KEY_VALUE(USER_BUTTON_2)) == GET_KEY_VALUE(USER_BUTTON_2));
		case USER_BUTTON_3:
			return ((key_value & GET_KEY_VALUE(USER_BUTTON_3)) == GET_KEY_VALUE(USER_BUTTON_3));
		case USER_BUTTON_4:
			return ((key_value & GET_KEY_VALUE(USER_BUTTON_4)) == GET_KEY_VALUE(USER_BUTTON_4));
		case USER_BUTTON_5:
			return ((key_value & GET_KEY_VALUE(USER_BUTTON_5)) == GET_KEY_VALUE(USER_BUTTON_5));

		case USER_BUTTON_COMBO_1:
			return ((key_value & GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_2)) == GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_2));
		case USER_BUTTON_COMBO_2:
			return ((key_value & GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_3)) == GET_KEY_COMBO_VALUE(USER_BUTTON_1,USER_BUTTON_3));
		case USER_BUTTON_COMBO_3:
			return ((key_value & GET_KEY_COMBO_VALUE(USER_BUTTON_3,USER_BUTTON_4)) == GET_KEY_COMBO_VALUE(USER_BUTTON_3,USER_BUTTON_4));

		default:
			return 0;
	}
}

void btn_evt_fn(unsigned char idx, unsigned char is_combo, xenon_btn_evt_t evt)
{	
	unsigned char id = 0;
	xbtn_time_t tick = xbtn_get_run_time();
	
	if(is_combo)
	{
		id = xbtn.cbtn[idx].id;
		key_keepalive_cnt = xbtn.cbtn[idx].keepalive_cnt;
	}
	else
	{
		id = xbtn.btn[idx].id;
		key_keepalive_cnt = xbtn.btn[idx].keepalive_cnt;
	}

	switch(id)
	{
		case USER_BUTTON_1:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_1 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_1 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;
			
		case USER_BUTTON_2:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_2 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_2 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;            
			
		case USER_BUTTON_3:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_3 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_3 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;            
			
		case USER_BUTTON_4:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_4 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_4 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;            
			
		case USER_BUTTON_5:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_5 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_5 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;            

		case USER_BUTTON_COMBO_1:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_COMBO_1 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else if(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_COMBO_1 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;
			
		case USER_BUTTON_COMBO_2:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_COMBO_2 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else iif(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_COMBO_2 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;

		case USER_BUTTON_COMBO_3:
			if(evt == BTN_EVT_ONRELEASE && key_keepalive_cnt == 0)
			{
				printf("USER_BUTTON_COMBO_3 , ONRELEASE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			else iif(evt == BTN_EVT_KEEPALIVE)
			{
				printf("USER_BUTTON_COMBO_3 , KEEPALIVE ,  tick = %lu , keepalive_cnt = %u \r\n", tick, key_keepalive_cnt);
			}
			break;

		default:
			break;
	}
}

void app_xbtn_init(void)
{
	xbtn_para_init();
	
	xbtn.btn[0].id = USER_BUTTON_1;
	xbtn.btn[1].id = USER_BUTTON_2;
	xbtn.btn[2].id = USER_BUTTON_3;
	xbtn.btn[3].id = USER_BUTTON_4;
	xbtn.btn[4].id = USER_BUTTON_5;

	xbtn.cbtn[0].id = USER_BUTTON_COMBO_1;
	xbtn.cbtn[1].id = USER_BUTTON_COMBO_2;
	xbtn.cbtn[2].id = USER_BUTTON_COMBO_3;
}

void key_process_task(void)
{
	key_state_get_all();
	xbtn_process();
}

unsigned int get_key_keepalive_cnt(void)
{
	return key_keepalive_cnt;
}
