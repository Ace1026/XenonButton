#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "xenon_button.h"


#define	GET_KEY_VALUE(X)				(0x01u << X)
#define	GET_KEY_COMBO_VALUE(X,Y)		((0x01u << X)|(0x01u << Y))

enum
{
	TK0,
	TK1,
	TK2,
	TK3,
	TK4,
	TK5,
	TK6,
	TK7,
	TK8,
	TK9,
	TK10,
	TK11,
	TK12,
	TK13,
	TK14,
	TK15,
	TK16,
	TK17,
	TK18,
	TK19,
	TK20,
	TK21,
	TK22,
	TK23,
	TK24,
	TK25,
	TK26,
	TK27,
	TK28,
	TK29,
	TK30,
	TK31,
};

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
unsigned int get_key_keepalive_cnt(void);

#endif
