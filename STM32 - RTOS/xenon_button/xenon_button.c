#include "xenon_button.h"

#define BTN_FLAG_ONPRESS_SENT  ((uint8_t)0x01) /*!< Flag indicates that on-press event has been sent */
#define BTN_FLAG_IN_PROCESS    ((uint8_t)0x02) /*!< Flag indicates that button in process */
#define BTN_FLAG_COMBO_HANDLE  ((uint8_t)0x04) /*!< Flag indicates that combo-button in handle */
#define BTN_FLAG_SINGLE_HANDLE ((uint8_t)0x08) /*!< Flag indicates that single-button in handle */

/* Default button group instance */
static xbtn_t xbtn_default;

static void prv_process_btn(xbtn_t* obj, xenon_btn_t *btn, xbtn_time_t mstime)
{
	if(obj == NULL || obj->get_state_fn == NULL) return;
	if(btn == NULL) return;

	uint8_t new_state = 0;

	new_state = obj->get_state_fn(btn);
	btn->curr_state = new_state;

	if(btn->curr_state != btn->last_state)
	{
		btn->time_state_change = mstime;
		
		if(btn->curr_state)
		{
			btn->flags |= BTN_FLAG_IN_PROCESS;
		}
	}

	/* Button is still pressed */
	if (btn->curr_state)
	{
		/*
			* Handle debounce and send on-press event
			*
			* This is when we detect valid press
			*/
		if (!(btn->flags & BTN_FLAG_ONPRESS_SENT))
		{
			/*
				* Run if statement when:
				*
				* - Runtime mode is enabled -> user sets its own config for debounce
				* - Config debounce time for press is more than `0`
				*/
			if ((xbtn_time_t)(mstime - btn->time_state_change) >= BTN_CFG_TIME_DEBOUNCE_PRESS)
			{
			#if BTN_CFG_USE_CLICK
				/*
					* Check mutlti click limit reach or not.
					*/
				if ((btn->click_cnt > 0) && ((xbtn_time_t)(mstime - btn->click_last_time) >= BTN_CFG_TIME_CLICK_MULTI_MAX))
				{
					if (btn->event_mask & BTN_EVT_MASK_ONCLICK)
					{
						if(obj->evt_fn != NULL)
						{
							obj->evt_fn(btn, BTN_EVT_ONCLICK);
						}
					}
					btn->click_cnt = 0;
				}
			#endif

			#if BTN_CFG_USE_KEEPALIVE
				/* Set keep alive time */
				btn->keepalive_last_time = mstime;
				btn->keepalive_cnt = 0;
			#endif

				/* Start with new on-press */
				btn->flags |= BTN_FLAG_ONPRESS_SENT;
				if (btn->event_mask & BTN_EVT_MASK_ONPRESS)
				{
					if(obj->evt_fn != NULL)
					{
						obj->evt_fn(btn, BTN_EVT_ONPRESS);
					}
				}
				if(btn->is_combo)
				{
					btn->flags |= BTN_FLAG_COMBO_HANDLE;
				}

				btn->time_change = mstime; /* Button state has now changed */
			}
		}

		/*
			* Handle keep alive, but only if on-press event has been sent
			*
			* Keep alive is sent when valid press is being detected
			*/
		else
		{
			if ((xbtn_time_t)(mstime - btn->time_change) > BTN_CFG_TIME_CLICK_PRESS_MAX)
			{
				if(btn->is_combo == 0)
				{
					btn->flags |= BTN_FLAG_SINGLE_HANDLE;
				}
			}
		
		#if BTN_CFG_USE_KEEPALIVE
			while ((BTN_CFG_TIME_KEEPALIVE_PERIOD > 0) && ((xbtn_time_t)(mstime - btn->keepalive_last_time) >= BTN_CFG_TIME_KEEPALIVE_PERIOD))
			{
				btn->keepalive_last_time += BTN_CFG_TIME_KEEPALIVE_PERIOD;
				btn->keepalive_cnt++;
				if (btn->event_mask & BTN_EVT_MASK_KEEPALIVE)
				{
					if(obj->evt_fn != NULL)
					{
						obj->evt_fn(btn, BTN_EVT_KEEPALIVE);
					}
				}
			}
		#endif

		#if BTN_CFG_USE_CLICK
			// Scene1: multi click end with a long press, need send onclick event.
			if ((btn->click_cnt > 0) && ((xbtn_time_t)(mstime - btn->time_change) > BTN_CFG_TIME_CLICK_PRESS_MAX))
			{
				if (btn->event_mask & BTN_EVT_MASK_ONCLICK)
				{
					if(obj->evt_fn != NULL)
					{
						obj->evt_fn(btn, BTN_EVT_ONCLICK);
					}
				}

				btn->click_cnt = 0;
			}
		#endif
		}
	}
	/* Button is still released */
	else
	{
		/*
			* We only need to react if on-press event has even been started.
			*
			* Do nothing if that was not the case
			*/
		if (btn->flags & BTN_FLAG_ONPRESS_SENT)
		{
			/*
				* Run if statement when:
				*
				* - Runtime mode is enabled -> user sets its own config for debounce
				* - Config debounce time for release is more than `0`
				*/
			if ((xbtn_time_t)(mstime - btn->time_state_change) >= BTN_CFG_TIME_DEBOUNCE_RELEASE)
			{
				/* Handle on-release event */
				btn->flags &= ~BTN_FLAG_ONPRESS_SENT;
				if (btn->event_mask & BTN_EVT_MASK_ONRELEASE)
				{
					if(obj->evt_fn != NULL)
					{
						obj->evt_fn(btn, BTN_EVT_ONRELEASE);
					}
				}

			#if BTN_CFG_USE_CLICK
				/* Check time validity for click event */
				if ((xbtn_time_t)(mstime - btn->time_change) >= BTN_CFG_TIME_CLICK_PRESS_MIN &&
					(xbtn_time_t)(mstime - btn->time_change) <= BTN_CFG_TIME_CLICK_PRESS_MAX)
				{
					btn->click_cnt++;
					btn->click_last_time = mstime;
				}
				else
				{
					// Scene2: If last press was too short, and previous sequence of clicks was
					// positive, send event to user.
					if ((btn->click_cnt > 0) && ((xbtn_time_t)(mstime - btn->time_change) < BTN_CFG_TIME_CLICK_PRESS_MIN))
					{
						if (btn->event_mask & BTN_EVT_MASK_ONCLICK)
						{
							if(obj->evt_fn != NULL)
							{
								obj->evt_fn(btn, BTN_EVT_ONCLICK);
							}
						}
					}
					/*
						* There was an on-release event, but timing
						* for click event detection is outside allowed window.
						*
						* Reset clicks counter -> not valid sequence for click event.
						*/
					btn->click_cnt = 0;
				}

				// Scene3: this part will send on-click event immediately after release event, if
				// maximum number of consecutive clicks has been reached.
				if ((btn->click_cnt > 0) && (btn->click_cnt == BTN_CFG_CLICK_MAX_CONSECUTIVE))
				{
					if (btn->event_mask & BTN_EVT_MASK_ONCLICK)
					{
						if(obj->evt_fn != NULL)
						{
							obj->evt_fn(btn, BTN_EVT_ONCLICK);
						}
					}
					btn->click_cnt = 0;
				}
			#endif

				btn->time_change = mstime; /* Button state has now changed */
			}
		}
		else
		{
		#if BTN_CFG_USE_CLICK
			/*
				* Based on te configuration, this part of the code
				* will send on-click event after certain timeout.
				*
				* This feature is useful if users prefers multi-click feature
				* that is reported only after last click event happened,
				* including number of clicks made by user
				*/
			if (btn->click_cnt > 0)
			{
				if ((xbtn_time_t)(mstime - btn->click_last_time) >= BTN_CFG_TIME_CLICK_MULTI_MAX)
				{
					if (btn->event_mask & BTN_EVT_MASK_ONCLICK)
					{
						if(obj->evt_fn != NULL)
						{
							obj->evt_fn(btn, BTN_EVT_ONCLICK);
						}
					}
					btn->click_cnt = 0;
				}
			}
			else
		#endif
			{
				// check button in process
				if (btn->flags & BTN_FLAG_IN_PROCESS)
				{
					btn->flags &= ~BTN_FLAG_IN_PROCESS;
				}
				if(btn->flags & BTN_FLAG_COMBO_HANDLE)
				{
					btn->flags &= ~BTN_FLAG_COMBO_HANDLE;
				}
				if(btn->flags & BTN_FLAG_SINGLE_HANDLE)
				{
					btn->flags &= ~BTN_FLAG_SINGLE_HANDLE;
				}
			}
		}
	}

	btn->last_state = btn->curr_state;
}

static void xenon_btn_all_flag_clean(xbtn_t* obj)
{
	if(obj == NULL) return;
	
	uint8_t i = 0;
	for (i = 0; i < obj->btn_cnt; i++)
	{
		obj->btn[i].curr_state = 0;
		obj->btn[i].last_state = 0;
		obj->btn[i].flags = 0;
	}
}

static void xenon_cbtn_all_flag_clean(xbtn_t* obj)
{
	if(obj == NULL) return;
	
	uint8_t i = 0;
	for (i = 0; i < obj->btn_combo_cnt; i++)
	{
		obj->cbtn[i].curr_state = 0;
		obj->cbtn[i].last_state = 0;
		obj->cbtn[i].flags = 0;
	}
}

static void xbtn_process_all_btn(xbtn_t* obj, xbtn_time_t mstime)
{
	if(obj == NULL) return;

	/* Process all buttons */
	uint8_t i = 0;
	if(obj->btn_process_idx != BTN_IDX_MAX)
	{
		i = obj->btn_process_idx;
		if(obj->btn[i].flags & BTN_FLAG_SINGLE_HANDLE)
		{
			prv_process_btn(obj, &obj->btn[i], mstime);
		}
		else
		{
			if((mstime - obj->btn[i].time_change) >= BTN_CFG_TIME_SINGLE_BTN_DELAY)
			{
				obj->btn_process_idx = BTN_IDX_MAX;
				obj->btn_process_cnt = 0;
			}
		}
		return;
	}
	if(obj->cbtn_process_idx != BTN_IDX_MAX)
	{
		i = obj->cbtn_process_idx;
		if(obj->cbtn[i].flags & BTN_FLAG_COMBO_HANDLE)
		{
			prv_process_btn(obj, &obj->cbtn[i], mstime);
		}
		else
		{
			if((mstime - obj->cbtn[i].time_change) >= BTN_CFG_TIME_COMBO_BTN_DELAY)
			{
				obj->btn_2_process_flag = 0;
				obj->cbtn_process_idx = BTN_IDX_MAX;
				obj->cbtn_process_flag = 0;
				obj->btn_process_cnt = 0;
			}
		}
		return;
	}
	if(obj->btn_process_cnt > 2)
	{
		if((mstime - obj->btn_invalid_time) >= BTN_CFG_TIME_INVALID_BTN_DELAY)
		{
			obj->btn_process_cnt = 0;
		}
		return;
	}
	if(obj->btn_process_cnt == 2)
	{
		if(obj->cbtn_process_flag == 0)
		{
			if((mstime - obj->btn_2_process_time) >= (BTN_CFG_TIME_DEBOUNCE_PRESS + BTN_CFG_TIME_DEBOUNCE_RELEASE))
			{
				obj->btn_process_cnt = 3;
				xenon_btn_all_flag_clean(obj);
				obj->btn_2_process_flag = 0;
				obj->btn_invalid_time = mstime;
				return;
			}
		}
	}
	if(obj->btn != NULL && obj->btn_cnt > 0)
	{
		obj->btn_process_cnt = 0;
		for (i = 0; i < obj->btn_cnt; i++)
		{
			prv_process_btn(obj, &obj->btn[i], mstime);
			if(obj->btn[i].flags & BTN_FLAG_IN_PROCESS)
			{
				obj->btn_process_cnt++;
			}
			if(obj->btn[i].flags & BTN_FLAG_SINGLE_HANDLE)
			{
				obj->btn_process_idx = i;
			}
		}
	}
	if(obj->btn_process_cnt > 2)
	{
		xenon_btn_all_flag_clean(obj);
		xenon_cbtn_all_flag_clean(obj);
		obj->btn_2_process_flag = 0;
		obj->cbtn_process_flag = 0;
		obj->btn_invalid_time = mstime;
		return;
	}
	if(obj->cbtn != NULL && obj->btn_combo_cnt > 0 && obj->btn_process_cnt == 2)
	{
		if(obj->btn_2_process_flag == 0 || obj->cbtn_process_flag == 1)
		{
			obj->btn_2_process_flag = 1;
			obj->btn_2_process_time = mstime;
			for (i = 0; i < obj->btn_combo_cnt; i++)
			{
				prv_process_btn(obj, &obj->cbtn[i], mstime);
				if(obj->cbtn[i].flags & BTN_FLAG_IN_PROCESS)
				{
					if(obj->cbtn_process_flag == 0)
					{
						obj->cbtn_process_flag = 1;
					}
				}
				if(obj->cbtn[i].flags & BTN_FLAG_COMBO_HANDLE)
				{
					obj->cbtn_process_idx = i;
					xenon_btn_all_flag_clean(obj);
				}
			}
		}
	}
}

int xbtn_init(xenon_btn_t *btn, uint8_t btn_cnt, xenon_btn_t *cbtn,
		uint8_t btn_combo_cnt, xbtn_get_state_fn get_state_fn, xbtn_evt_fn evt_fn)
{
	xbtn_t *obj = &xbtn_default;

	/* Parameter is a must only in callback-only mode */
	if (evt_fn == NULL || get_state_fn == NULL)
	{
		return 0;
	}

	memset(obj, 0x00, sizeof(*obj));
	obj->btn = btn;
	obj->btn_cnt = btn_cnt;
	obj->cbtn = cbtn;
	obj->btn_combo_cnt = btn_combo_cnt;
	obj->btn_process_idx = BTN_IDX_MAX;
	obj->cbtn_process_idx = BTN_IDX_MAX;
	obj->evt_fn = evt_fn;
	obj->get_state_fn = get_state_fn;

	return 1;
}

void xbtn_process(xbtn_time_t mstime)
{
	xbtn_process_all_btn(&xbtn_default, mstime);
}

uint8_t xenon_btn_is_active(const xenon_btn_t *btn)
{
	return btn != NULL && (btn->flags & BTN_FLAG_ONPRESS_SENT);
}

uint8_t xenon_btn_is_in_process(const xenon_btn_t *btn)
{
	return btn != NULL && (btn->flags & BTN_FLAG_IN_PROCESS);
}

uint8_t xbtn_is_in_process(void)
{
	xbtn_t *obj = &xbtn_default;
	return (obj->btn_process_cnt != 0);
}

