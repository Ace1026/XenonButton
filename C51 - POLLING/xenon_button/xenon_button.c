#include "xenon_button.h"

#define BTN_FLAG_ONPRESS_SENT  ((unsigned char)0x01) /*!< Flag indicates that on-press event has been sent */
#define BTN_FLAG_IN_PROCESS    ((unsigned char)0x02) /*!< Flag indicates that button in process */
#define BTN_FLAG_COMBO_HANDLE  ((unsigned char)0x04) /*!< Flag indicates that combo-button in handle */
#define BTN_FLAG_SINGLE_HANDLE ((unsigned char)0x08) /*!< Flag indicates that single-button in handle */

/* Default button group instance */
xbtn_t xdata xbtn;
extern void btn_evt_fn(unsigned char idx, unsigned char is_combo, xenon_btn_evt_t evt);
extern unsigned char btn_get_state_fn(unsigned char id);

static void prv_process_btn(unsigned char idx, xbtn_time_t mstime)
{
	unsigned char new_state = 0;
	if(btn_get_state_fn == NULL) return;

	new_state = btn_get_state_fn(xbtn.btn[idx].id);
	xbtn.btn[idx].curr_state = new_state;

	if(xbtn.btn[idx].curr_state != xbtn.btn[idx].last_state)
	{
		xbtn.btn[idx].time_state_change = mstime;
		
		if(xbtn.btn[idx].curr_state)
		{
			xbtn.btn[idx].flags |= BTN_FLAG_IN_PROCESS;
		}
	}

	/* Button is still pressed */
	if (xbtn.btn[idx].curr_state)
	{
		if (!(xbtn.btn[idx].flags & BTN_FLAG_ONPRESS_SENT))
		{
			if ((mstime - xbtn.btn[idx].time_state_change) >= (BTN_CFG_TIME_DEBOUNCE_PRESS / BTN_CFG_TIME_INTERVAL))
			{
			#if BTN_CFG_USE_CLICK
				/*
					* Check mutlti click limit reach or not.
					*/
				if ((xbtn.btn[idx].click_cnt > 0) && ((mstime - xbtn.btn[idx].click_last_time) >= (BTN_CFG_TIME_CLICK_MULTI_MAX / BTN_CFG_TIME_INTERVAL)))
				{
				#if BTN_EVT_MASK_ONCLICK
					btn_evt_fn(idx, 0, BTN_EVT_ONCLICK);
				#endif
					xbtn.btn[idx].click_cnt = 0;
				}
			#endif

			#if BTN_CFG_USE_KEEPALIVE
				/* Set keep alive time */
				xbtn.btn[idx].keepalive_last_time = mstime;
				xbtn.btn[idx].keepalive_cnt = 0;
			#endif

				/* Start with new on-press */
				xbtn.btn[idx].flags |= BTN_FLAG_ONPRESS_SENT;
			#if BTN_EVT_MASK_ONPRESS
				btn_evt_fn(idx, 0, BTN_EVT_ONPRESS);
			#endif
				xbtn.btn[idx].time_change = mstime; /* Button state has now changed */
			}
		}
		else
		{
			if ((mstime - xbtn.btn[idx].time_change) > (BTN_CFG_TIME_CLICK_PRESS_MAX / BTN_CFG_TIME_INTERVAL))
			{
				xbtn.btn[idx].flags |= BTN_FLAG_SINGLE_HANDLE;
			}
		
		#if BTN_CFG_USE_KEEPALIVE
			while (((BTN_CFG_TIME_KEEPALIVE_PERIOD / BTN_CFG_TIME_INTERVAL) > 0)
				&& ((mstime - xbtn.btn[idx].keepalive_last_time) >= (BTN_CFG_TIME_KEEPALIVE_PERIOD / BTN_CFG_TIME_INTERVAL)))
			{
				xbtn.btn[idx].keepalive_last_time += (BTN_CFG_TIME_KEEPALIVE_PERIOD / BTN_CFG_TIME_INTERVAL);
				xbtn.btn[idx].keepalive_cnt++;
			#if BTN_EVT_MASK_KEEPALIVE
				btn_evt_fn(idx, 0, BTN_EVT_KEEPALIVE);
			#endif
			}
		#endif

		#if BTN_CFG_USE_CLICK
			// Scene1: multi click end with a long press, need send onclick event.
			if ((xbtn.btn[idx].click_cnt > 0) && ((mstime - xbtn.btn[idx].time_change) > (BTN_CFG_TIME_CLICK_PRESS_MAX / BTN_CFG_TIME_INTERVAL)))
			{
			#if BTN_EVT_MASK_ONCLICK
				btn_evt_fn(idx, 0, BTN_EVT_ONCLICK);
			#endif
				xbtn.btn[idx].click_cnt = 0;
			}
		#endif
		}
	}
	/* Button is still released */
	else
	{
		if (xbtn.btn[idx].flags & BTN_FLAG_ONPRESS_SENT)
		{
			if ((mstime - xbtn.btn[idx].time_state_change) >= (BTN_CFG_TIME_DEBOUNCE_RELEASE / BTN_CFG_TIME_INTERVAL))
			{
				/* Handle on-release event */
				xbtn.btn[idx].flags &= ~BTN_FLAG_ONPRESS_SENT;
			#if BTN_EVT_MASK_ONRELEASE
				btn_evt_fn(idx, 0, BTN_EVT_ONRELEASE);
			#endif

			#if BTN_CFG_USE_CLICK
				/* Check time validity for click event */
				if ((mstime - xbtn.btn[idx].time_change) >= (BTN_CFG_TIME_CLICK_PRESS_MIN / BTN_CFG_TIME_INTERVAL) &&
					(mstime - xbtn.btn[idx].time_change) <= (BTN_CFG_TIME_CLICK_PRESS_MAX / BTN_CFG_TIME_INTERVAL))
				{
					xbtn.btn[idx].click_cnt++;
					xbtn.btn[idx].click_last_time = mstime;
				}
				else
				{
					// Scene2: If last press was too short, and previous sequence of clicks was
					// positive, send event to user.
					if ((xbtn.btn[idx].click_cnt > 0) && ((mstime - xbtn.btn[idx].time_change) < (BTN_CFG_TIME_CLICK_PRESS_MIN / BTN_CFG_TIME_INTERVAL)))
					{
					#if BTN_EVT_MASK_ONCLICK
						btn_evt_fn(idx, 0, BTN_EVT_ONCLICK);
					#endif
					}
					xbtn.btn[idx].click_cnt = 0;
				}

				// Scene3: this part will send on-click event immediately after release event, if
				// maximum number of consecutive clicks has been reached.
				if ((xbtn.btn[idx].click_cnt > 0) && (xbtn.btn[idx].click_cnt == BTN_CFG_CLICK_MAX_CONSECUTIVE))
				{
				#if BTN_EVT_MASK_ONCLICK
					btn_evt_fn(idx, 0, BTN_EVT_ONCLICK);
				#endif
					xbtn.btn[idx].click_cnt = 0;
				}
			#endif

				xbtn.btn[idx].time_change = mstime; /* Button state has now changed */
			}
		}
		else
		{
		#if BTN_CFG_USE_CLICK
			if (xbtn.btn[idx].click_cnt > 0)
			{
				if ((mstime - xbtn.btn[idx].click_last_time) >= (BTN_CFG_TIME_CLICK_MULTI_MAX / BTN_CFG_TIME_INTERVAL))
				{
				#if BTN_EVT_MASK_ONCLICK
					btn_evt_fn(idx, 0, BTN_EVT_ONCLICK);
				#endif
					xbtn.btn[idx].click_cnt = 0;
				}
			}
			else
		#endif
			{
				// check button in process
				if (xbtn.btn[idx].flags & BTN_FLAG_IN_PROCESS)
				{
					xbtn.btn[idx].flags &= ~BTN_FLAG_IN_PROCESS;
				}
				if(xbtn.btn[idx].flags & BTN_FLAG_SINGLE_HANDLE)
				{
					xbtn.btn[idx].flags &= ~BTN_FLAG_SINGLE_HANDLE;
				}
			}
		}
	}

	xbtn.btn[idx].last_state = xbtn.btn[idx].curr_state;
}

#if (BTN_CFG_COMBO_BTN_CNT > 0)
static void prv_process_cbtn(unsigned char idx, xbtn_time_t mstime)
{
	unsigned char new_state = 0;
	if(btn_get_state_fn == NULL) return;

	new_state = btn_get_state_fn(xbtn.cbtn[idx].id);
	xbtn.cbtn[idx].curr_state = new_state;

	if(xbtn.cbtn[idx].curr_state != xbtn.cbtn[idx].last_state)
	{
		xbtn.cbtn[idx].time_state_change = mstime;
		
		if(xbtn.cbtn[idx].curr_state)
		{
			xbtn.cbtn[idx].flags |= BTN_FLAG_IN_PROCESS;
		}
	}

	/* Button is still pressed */
	if (xbtn.cbtn[idx].curr_state)
	{
		if (!(xbtn.cbtn[idx].flags & BTN_FLAG_ONPRESS_SENT))
		{
			if ((mstime - xbtn.cbtn[idx].time_state_change) >= (BTN_CFG_TIME_DEBOUNCE_PRESS / BTN_CFG_TIME_INTERVAL))
			{
			#if BTN_CFG_USE_CLICK
				/*
					* Check mutlti click limit reach or not.
					*/
				if ((xbtn.cbtn[idx].click_cnt > 0) && ((mstime - xbtn.cbtn[idx].click_last_time) >= (BTN_CFG_TIME_CLICK_MULTI_MAX / BTN_CFG_TIME_INTERVAL)))
				{
				#if BTN_EVT_MASK_ONCLICK
					btn_evt_fn(idx, 1, BTN_EVT_ONCLICK);
				#endif
					xbtn.cbtn[idx].click_cnt = 0;
				}
			#endif

			#if BTN_CFG_USE_KEEPALIVE
				/* Set keep alive time */
				xbtn.cbtn[idx].keepalive_last_time = mstime;
				xbtn.cbtn[idx].keepalive_cnt = 0;
			#endif

				/* Start with new on-press */
				xbtn.cbtn[idx].flags |= BTN_FLAG_ONPRESS_SENT;
			#if BTN_EVT_MASK_ONPRESS
				btn_evt_fn(idx, 1, BTN_EVT_ONPRESS);
			#endif
				xbtn.cbtn[idx].flags |= BTN_FLAG_COMBO_HANDLE;
				xbtn.cbtn[idx].time_change = mstime; /* Button state has now changed */
			}
		}
		else
		{
		#if BTN_CFG_USE_KEEPALIVE
			while (((BTN_CFG_TIME_KEEPALIVE_PERIOD / BTN_CFG_TIME_INTERVAL) > 0)
				&& ((mstime - xbtn.cbtn[idx].keepalive_last_time) >= (BTN_CFG_TIME_KEEPALIVE_PERIOD / BTN_CFG_TIME_INTERVAL)))
			{
				xbtn.cbtn[idx].keepalive_last_time += (BTN_CFG_TIME_KEEPALIVE_PERIOD / BTN_CFG_TIME_INTERVAL);
				xbtn.cbtn[idx].keepalive_cnt++;
			#if BTN_EVT_MASK_KEEPALIVE
				btn_evt_fn(idx, 1, BTN_EVT_KEEPALIVE);
			#endif
			}
		#endif

		#if BTN_CFG_USE_CLICK
			// Scene1: multi click end with a long press, need send onclick event.
			if ((xbtn.cbtn[idx].click_cnt > 0) && ((mstime - xbtn.cbtn[idx].time_change) > (BTN_CFG_TIME_CLICK_PRESS_MAX / BTN_CFG_TIME_INTERVAL)))
			{
			#if BTN_EVT_MASK_ONCLICK
				btn_evt_fn(idx, 1, BTN_EVT_ONCLICK);
			#endif
				xbtn.cbtn[idx].click_cnt = 0;
			}
		#endif
		}
	}
	/* Button is still released */
	else
	{
		if (xbtn.cbtn[idx].flags & BTN_FLAG_ONPRESS_SENT)
		{
			if ((mstime - xbtn.cbtn[idx].time_state_change) >= (BTN_CFG_TIME_DEBOUNCE_RELEASE / BTN_CFG_TIME_INTERVAL))
			{
				/* Handle on-release event */
				xbtn.cbtn[idx].flags &= ~BTN_FLAG_ONPRESS_SENT;
			#if BTN_EVT_MASK_ONRELEASE
				btn_evt_fn(idx, 1, BTN_EVT_ONRELEASE);
			#endif

			#if BTN_CFG_USE_CLICK
				/* Check time validity for click event */
				if ((mstime - xbtn.cbtn[idx].time_change) >= (BTN_CFG_TIME_CLICK_PRESS_MIN / BTN_CFG_TIME_INTERVAL) &&
					(mstime - xbtn.cbtn[idx].time_change) <= (BTN_CFG_TIME_CLICK_PRESS_MAX / BTN_CFG_TIME_INTERVAL))
				{
					xbtn.cbtn[idx].click_cnt++;
					xbtn.cbtn[idx].click_last_time = mstime;
				}
				else
				{
					// Scene2: If last press was too short, and previous sequence of clicks was
					// positive, send event to user.
					if ((xbtn.cbtn[idx].click_cnt > 0) && ((mstime - xbtn.cbtn[idx].time_change) < (BTN_CFG_TIME_CLICK_PRESS_MIN / BTN_CFG_TIME_INTERVAL)))
					{
					#if BTN_EVT_MASK_ONCLICK
						btn_evt_fn(idx, 1, BTN_EVT_ONCLICK);
					#endif
					}
					xbtn.cbtn[idx].click_cnt = 0;
				}

				// Scene3: this part will send on-click event immediately after release event, if
				// maximum number of consecutive clicks has been reached.
				if ((xbtn.cbtn[idx].click_cnt > 0) && (xbtn.cbtn[idx].click_cnt == BTN_CFG_CLICK_MAX_CONSECUTIVE))
				{
				#if BTN_EVT_MASK_ONCLICK
					btn_evt_fn(idx, 1, BTN_EVT_ONCLICK);
				#endif
					xbtn.cbtn[idx].click_cnt = 0;
				}
			#endif

				xbtn.cbtn[idx].time_change = mstime; /* Button state has now changed */
			}
		}
		else
		{
		#if BTN_CFG_USE_CLICK
			if (xbtn.cbtn[idx].click_cnt > 0)
			{
				if ((mstime - xbtn.cbtn[idx].click_last_time) >= (BTN_CFG_TIME_CLICK_MULTI_MAX / BTN_CFG_TIME_INTERVAL))
				{
				#if BTN_EVT_MASK_ONCLICK
					btn_evt_fn(idx, 1, BTN_EVT_ONCLICK);
				#endif
					xbtn.cbtn[idx].click_cnt = 0;
				}
			}
			else
		#endif
			{
				// check button in process
				if (xbtn.cbtn[idx].flags & BTN_FLAG_IN_PROCESS)
				{
					xbtn.cbtn[idx].flags &= ~BTN_FLAG_IN_PROCESS;
				}
				if(xbtn.cbtn[idx].flags & BTN_FLAG_COMBO_HANDLE)
				{
					xbtn.cbtn[idx].flags &= ~BTN_FLAG_COMBO_HANDLE;
				}
			}
		}
	}

	xbtn.cbtn[idx].last_state = xbtn.cbtn[idx].curr_state;
}
#endif

static void xenon_btn_all_flag_clean(void)
{
	unsigned char i = 0;
	
	for (i = 0; i < BTN_CFG_SINGLE_BTN_CNT; i++)
	{
		xbtn.btn[i].curr_state = 0;
		xbtn.btn[i].last_state = 0;
		xbtn.btn[i].flags = 0;
	}
}

#if (BTN_CFG_COMBO_BTN_CNT > 0)
static void xenon_cbtn_all_flag_clean(void)
{
	unsigned char i = 0;
	
	for (i = 0; i < BTN_CFG_COMBO_BTN_CNT; i++)
	{
		xbtn.cbtn[i].curr_state = 0;
		xbtn.cbtn[i].last_state = 0;
		xbtn.cbtn[i].flags = 0;
	}
}
#endif

static void xbtn_process_all_btn(xbtn_time_t mstime)
{
	unsigned char i = 0;

	/* Process all buttons */
	if(xbtn.btn_process_idx != BTN_IDX_MAX)
	{
		i = xbtn.btn_process_idx;
		if(xbtn.btn[i].flags & BTN_FLAG_SINGLE_HANDLE)
		{
			prv_process_btn(i, mstime);
		}
		else
		{
			if((mstime - xbtn.btn[i].time_change) >= (BTN_CFG_TIME_SINGLE_BTN_DELAY / BTN_CFG_TIME_INTERVAL))
			{
				xbtn.btn_process_idx = BTN_IDX_MAX;
				xbtn.btn_process_cnt = 0;
			}
		}
		return;
	}
#if (BTN_CFG_COMBO_BTN_CNT > 0)
	if(xbtn.cbtn_process_idx != BTN_IDX_MAX)
	{
		i = xbtn.cbtn_process_idx;
		if(xbtn.cbtn[i].flags & BTN_FLAG_COMBO_HANDLE)
		{
			prv_process_cbtn(i, mstime);
		}
		else
		{
			if((mstime - xbtn.cbtn[i].time_change) >= (BTN_CFG_TIME_COMBO_BTN_DELAY / BTN_CFG_TIME_INTERVAL))
			{
				xbtn.btn_2_process_flag = 0;
				xbtn.cbtn_process_idx = BTN_IDX_MAX;
				xbtn.cbtn_process_flag = 0;
				xbtn.btn_process_cnt = 0;
			}
		}
		return;
	}
#endif
	if(xbtn.btn_process_cnt > 2)
	{
		if((mstime - xbtn.btn_invalid_time) >= (BTN_CFG_TIME_INVALID_BTN_DELAY / BTN_CFG_TIME_INTERVAL))
		{
			xbtn.btn_process_cnt = 0;
		}
		return;
	}
	if(xbtn.btn_process_cnt == 2)
	{
		if(xbtn.cbtn_process_flag == 0)
		{
			if((mstime - xbtn.btn_2_process_time) >= 
				((BTN_CFG_TIME_DEBOUNCE_PRESS + BTN_CFG_TIME_DEBOUNCE_RELEASE) / BTN_CFG_TIME_INTERVAL))
			{
				xbtn.btn_process_cnt = 3;
				xenon_btn_all_flag_clean();
				xbtn.btn_2_process_flag = 0;
				xbtn.btn_invalid_time = mstime;
				return;
			}
		}
	}
#if (BTN_CFG_SINGLE_BTN_CNT > 0)
	xbtn.btn_process_cnt = 0;
	for (i = 0; i < BTN_CFG_SINGLE_BTN_CNT; i++)
	{
		prv_process_btn(i, mstime);
		if(xbtn.btn[i].flags & BTN_FLAG_IN_PROCESS)
		{
			xbtn.btn_process_cnt++;
		}
		if(xbtn.btn[i].flags & BTN_FLAG_SINGLE_HANDLE)
		{
			xbtn.btn_process_idx = i;
		}
	}
#endif
	if(xbtn.btn_process_cnt > 2)
	{
		xenon_btn_all_flag_clean();
	#if (BTN_CFG_COMBO_BTN_CNT > 0)
		xenon_cbtn_all_flag_clean();
	#endif
		xbtn.btn_2_process_flag = 0;
		xbtn.cbtn_process_flag = 0;
		xbtn.btn_invalid_time = mstime;
		return;
	}
#if (BTN_CFG_COMBO_BTN_CNT > 0)
	if(xbtn.btn_process_cnt == 2)
	{
		if(xbtn.btn_2_process_flag == 0 || xbtn.cbtn_process_flag == 1)
		{
			xbtn.btn_2_process_flag = 1;
			xbtn.btn_2_process_time = mstime;
			for (i = 0; i < BTN_CFG_COMBO_BTN_CNT; i++)
			{
				prv_process_cbtn(i, mstime);
				if(xbtn.cbtn[i].flags & BTN_FLAG_IN_PROCESS)
				{
					if(xbtn.cbtn_process_flag == 0)
					{
						xbtn.cbtn_process_flag = 1;
					}
				}
				if(xbtn.cbtn[i].flags & BTN_FLAG_COMBO_HANDLE)
				{
					xbtn.cbtn_process_idx = i;
					xenon_btn_all_flag_clean();
				}
			}
		}
	}
#endif
}

void xbtn_para_init(void)
{
	memset(&xbtn, 0x00, sizeof(xbtn));
	xbtn.btn_process_idx = BTN_IDX_MAX;
	xbtn.cbtn_process_idx = BTN_IDX_MAX;
}

void xbtn_process(void)
{
	xbtn.btn_run_time++;
	xbtn_process_all_btn(xbtn.btn_run_time);
}

unsigned char xenon_btn_is_active(const xenon_btn_t *btn)
{
	return btn != NULL && (btn->flags & BTN_FLAG_ONPRESS_SENT);
}

unsigned char xenon_btn_is_in_process(const xenon_btn_t *btn)
{
	return btn != NULL && (btn->flags & BTN_FLAG_IN_PROCESS);
}

unsigned char xbtn_is_in_process(void)
{
	return (xbtn.btn_process_cnt != 0);
}

xbtn_time_t xbtn_get_run_time(void)
{
	return xbtn.btn_run_time;
}

