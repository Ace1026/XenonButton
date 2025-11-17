#ifndef XENON_BUTTON_H
#define XENON_BUTTON_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


typedef unsigned short xbtn_time_t;

#define BTN_CFG_TIME_INTERVAL			(xbtn_time_t)10

#define BTN_CFG_TIME_DEBOUNCE_PRESS		(xbtn_time_t)20
#define BTN_CFG_TIME_DEBOUNCE_RELEASE	(xbtn_time_t)10
#define BTN_CFG_TIME_CLICK_PRESS_MIN	(xbtn_time_t)20
#define BTN_CFG_TIME_CLICK_PRESS_MAX	(xbtn_time_t)300
#define BTN_CFG_TIME_CLICK_MULTI_MAX	(xbtn_time_t)400
#define BTN_CFG_TIME_KEEPALIVE_PERIOD	(xbtn_time_t)1000
#define BTN_CFG_TIME_SINGLE_BTN_DELAY	(xbtn_time_t)200
#define BTN_CFG_TIME_COMBO_BTN_DELAY	(xbtn_time_t)400
#define BTN_CFG_TIME_INVALID_BTN_DELAY	(xbtn_time_t)400
#define BTN_CFG_CLICK_MAX_CONSECUTIVE	3

#define BTN_CFG_USE_CLICK				1
#define BTN_CFG_USE_KEEPALIVE			1

#define BTN_CFG_SINGLE_BTN_CNT			5
#define BTN_CFG_COMBO_BTN_CNT			3

typedef enum
{
	BTN_EVT_ONPRESS = 0x00,	/*!< On press event - sent when valid press is detected */
	BTN_EVT_ONRELEASE,		/*!< On release event - sent when valid release event is detected (from
								active to inactive) */

#if BTN_CFG_USE_CLICK
	BTN_EVT_ONCLICK,		/*!< On Click event - sent when valid sequence of on-press and on-release
								events occurs */
#endif

#if BTN_CFG_USE_KEEPALIVE
	BTN_EVT_KEEPALIVE,		/*!< Keep alive event - sent periodically when button is active */
#endif

} xenon_btn_evt_t;

#define BTN_EVT_MASK_ONPRESS			1
#define BTN_EVT_MASK_ONRELEASE			1

#if BTN_CFG_USE_CLICK
#define BTN_EVT_MASK_ONCLICK			1
#endif

#if BTN_CFG_USE_KEEPALIVE
#define BTN_EVT_MASK_KEEPALIVE			1
#endif

#define BTN_IDX_MAX						0x7F

typedef struct xenon_button
{
	unsigned char id;				/*!< User defined custom argument for callback function purpose */
	
	unsigned char curr_state  : 1;	/*!< Used to record the current state of buttons */
	unsigned char last_state  : 1;	/*!< Used to record the last state of buttons */
	unsigned char flags       : 6;	/*!< Private button flags management */
	
	xbtn_time_t time_change;		/*!< Time in ms when button state got changed last time after valid debounce */
	xbtn_time_t time_state_change;	/*!< Time in ms when button state got changed last time */
	
#if BTN_CFG_USE_CLICK
	unsigned char click_cnt;		/*!< Number of consecutive clicks detected, respecting maximum timeoutbetween clicks */
	xbtn_time_t click_last_time;	/*!< Time in ms of last successfully detected (not sent!) click event */
#endif

#if BTN_CFG_USE_KEEPALIVE
    unsigned short keepalive_cnt;	/*!< Number of keep alive events sent after successful on-press
										detection. Value is reset after on-release */
	xbtn_time_t keepalive_last_time;/*!< Time in ms of last send keep alive event */
#endif

} xenon_btn_t;

typedef struct xbtn_obj
{
	xenon_btn_t btn[BTN_CFG_SINGLE_BTN_CNT];	/*!< Array of buttons */
#if (BTN_CFG_COMBO_BTN_CNT > 0)
	xenon_btn_t cbtn[BTN_CFG_COMBO_BTN_CNT];	/*!< Array of combo-buttons */
#endif
	unsigned char btn_process_cnt;			/*!< Number of button in process */
	unsigned char btn_process_idx : 7;		/*!< Idx of single-button in process */
	unsigned char btn_2_process_flag : 1;	/*!< Flag of two-button in process */
	unsigned char cbtn_process_idx : 7;		/*!< Idx of combo-button in process */
	unsigned char cbtn_process_flag : 1;	/*!< Flag of combo-button in process */
	xbtn_time_t btn_2_process_time;			/*!< Time of two-button in process */
	xbtn_time_t btn_invalid_time;			/*!< Time of button invalid */
	xbtn_time_t btn_run_time;				/*!< Time of button run */
} xbtn_t;

#define BTN_BUTTON_INIT(_id) {.id = _id}

#define BTN_ARRAY_SIZE(_arr) sizeof(_arr) / sizeof((_arr)[0])

extern xbtn_t xdata xbtn;

void xbtn_para_init(void);
void xbtn_process(void);
unsigned char xenon_btn_is_active(const xenon_btn_t *btn);
unsigned char xenon_btn_is_in_process(const xenon_btn_t *btn);
unsigned char xbtn_is_in_process(void);
xbtn_time_t xbtn_get_run_time(void);

#define xbtn_click_get_count(btn) ((btn)->click_cnt)
#define xbtn_keepalive_get_count(btn) ((btn)->keepalive_cnt)

#endif
