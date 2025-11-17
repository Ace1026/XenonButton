#ifndef XENON_BUTTON_H
#define XENON_BUTTON_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


typedef uint16_t xbtn_time_t;

#define BTN_CFG_TIME_INTERVAL			(xbtn_time_t)10

#define BTN_CFG_TIME_DEBOUNCE_PRESS		(xbtn_time_t)20
#define BTN_CFG_TIME_DEBOUNCE_RELEASE	(xbtn_time_t)10
#define BTN_CFG_TIME_CLICK_PRESS_MIN	(xbtn_time_t)20
#define BTN_CFG_TIME_CLICK_PRESS_MAX	(xbtn_time_t)400
#define BTN_CFG_TIME_CLICK_MULTI_MAX	(xbtn_time_t)400
#define BTN_CFG_TIME_KEEPALIVE_PERIOD	(xbtn_time_t)1000
#define BTN_CFG_TIME_SINGLE_BTN_DELAY	(xbtn_time_t)200
#define BTN_CFG_TIME_COMBO_BTN_DELAY	(xbtn_time_t)600
#define BTN_CFG_TIME_INVALID_BTN_DELAY	(xbtn_time_t)600
#define BTN_CFG_CLICK_MAX_CONSECUTIVE	3

#define BTN_CFG_USE_CLICK				1
#define BTN_CFG_USE_KEEPALIVE			1

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

#define BTN_EVT_MASK_ONPRESS	(1 << BTN_EVT_ONPRESS)
#define BTN_EVT_MASK_ONRELEASE	(1 << BTN_EVT_ONRELEASE)

#if BTN_CFG_USE_CLICK
#define BTN_EVT_MASK_ONCLICK	(1 << BTN_EVT_ONCLICK)
#endif

#if BTN_CFG_USE_KEEPALIVE
#define BTN_EVT_MASK_KEEPALIVE	(1 << BTN_EVT_KEEPALIVE)
#endif

#if BTN_CFG_USE_CLICK && BTN_CFG_USE_KEEPALIVE
#define BTN_EVT_MASK_ALL (BTN_EVT_MASK_ONPRESS | BTN_EVT_MASK_ONRELEASE | BTN_EVT_MASK_ONCLICK | BTN_EVT_MASK_KEEPALIVE)
#elif BTN_CFG_USE_CLICK
#define BTN_EVT_MASK_ALL (BTN_EVT_MASK_ONPRESS | BTN_EVT_MASK_ONRELEASE | BTN_EVT_MASK_ONCLICK)
#elif BTN_CFG_USE_KEEPALIVE
#define BTN_EVT_MASK_ALL (BTN_EVT_MASK_ONPRESS | BTN_EVT_MASK_ONRELEASE | BTN_EVT_MASK_KEEPALIVE)
#else
#define BTN_EVT_MASK_ALL (BTN_EVT_MASK_ONPRESS | BTN_EVT_MASK_ONRELEASE)
#endif

#define BTN_IDX_MAX				0x7F

typedef struct xenon_button
{
	uint8_t id;						/*!< User defined custom argument for callback function purpose */
	
	uint16_t curr_state  : 1;		/*!< Used to record the current state of buttons */
	uint16_t last_state  : 1;		/*!< Used to record the last state of buttons */
	uint16_t flags       : 6;		/*!< Private button flags management */
	uint16_t event_mask  : 5;		/*!< Private button event mask management */
	uint16_t is_combo    : 1;		/*!< Indicate button or combo-button */
	uint16_t reserve     : 2;		/*!< Reserve */
	
	xbtn_time_t time_change;		/*!< Time in ms when button state got changed last time after valid debounce */
	xbtn_time_t time_state_change;	/*!< Time in ms when button state got changed last time */
	
#if BTN_CFG_USE_CLICK
	uint8_t click_cnt;				/*!< Number of consecutive clicks detected, respecting maximum timeoutbetween clicks */
	xbtn_time_t click_last_time;	/*!< Time in ms of last successfully detected (not sent!) click event */
#endif

#if BTN_CFG_USE_KEEPALIVE
    uint16_t keepalive_cnt;			/*!< Number of keep alive events sent after successful on-press
										detection. Value is reset after on-release */
	xbtn_time_t keepalive_last_time;/*!< Time in ms of last send keep alive event */
#endif

} xenon_btn_t;

typedef void (*xbtn_evt_fn)(xenon_btn_t *btn, xenon_btn_evt_t evt);
typedef uint8_t (*xbtn_get_state_fn)(xenon_btn_t *btn);

typedef struct xbtn_obj
{
	xenon_btn_t *btn;				/*!< Pointer to buttons array */
	uint8_t btn_cnt;				/*!< Number of buttons in array */
	xenon_btn_t *cbtn;				/*!< Pointer to combo-buttons array */
	uint8_t btn_combo_cnt;			/*!< Number of combo-buttons in array */
	uint8_t btn_process_cnt;		/*!< Number of button in process */
	uint8_t btn_process_idx : 7;	/*!< Idx of single-button in process */
	uint8_t btn_2_process_flag : 1;	/*!< Flag of two-button in process */
	uint8_t cbtn_process_idx : 7;	/*!< Idx of combo-button in process */
	uint8_t cbtn_process_flag : 1;	/*!< Flag of combo-button in process */
	xbtn_time_t btn_2_process_time;	/*!< Time of two-button in process */
	xbtn_time_t btn_invalid_time;	/*!< Time of button invalid */
	xbtn_time_t btn_run_time;		/*!< Time of button run */

	xbtn_evt_fn evt_fn;				/*!< Pointer to event function */
	xbtn_get_state_fn get_state_fn;	/*!< Pointer to get state function */
} xbtn_t;

#define BTN_BUTTON_INIT_RAW(_id, _mask, _is_combo) {.id = _id, .event_mask = _mask, .is_combo = _is_combo}

#define BTN_BUTTON_INIT(_id) BTN_BUTTON_INIT_RAW(_id, BTN_EVT_MASK_ALL, false)
#define BTN_BUTTON_COMBO_INIT(_id) BTN_BUTTON_INIT_RAW(_id, BTN_EVT_MASK_ALL, true)

#define BTN_ARRAY_SIZE(_arr) sizeof(_arr) / sizeof((_arr)[0])

int xbtn_init(xenon_btn_t *btn, uint8_t btn_cnt, xenon_btn_t *cbtn,
		uint8_t btn_combo_cnt, xbtn_get_state_fn get_state_fn, xbtn_evt_fn evt_fn);
void xbtn_process(void);
uint8_t xenon_btn_is_active(const xenon_btn_t *btn);
uint8_t xenon_btn_is_in_process(const xenon_btn_t *btn);
uint8_t xbtn_is_in_process(void);
xbtn_time_t xbtn_get_run_time(void);

#define xbtn_click_get_count(btn) ((btn)->click_cnt)
#define xbtn_keepalive_get_count(btn) ((btn)->keepalive_cnt)

#endif
