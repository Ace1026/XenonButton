# Introduction

XenonButton is a button driver based on C language, mainly used in embedded systems, supporting multiple button event processing and combo button configuration.

During the project development process, the following projects were referenced: [FlexibleButton], [lwbtn], [easy_button]

The core button management mechanism is based on [lwbtn] and has undergone significant modifications, resulting in slightly different event reporting behavior from the original handling.

When a button is triggered, it will not be interrupted by other buttons, and the combo button will not generate single button events repeatedly. These are two unique features that are different from many button drivers currently on the market.

This button driver is the STM32-RTOS version, which can be used on both STM32-RTOS systems and Linux systems.



## Long press support & Combo button support

In practical projects, various functional requirements may be encountered, such as long pressing for 3 seconds for function A, long pressing for 5 seconds for function B, and long pressing for 30 seconds for function C. Through the design of 'keepalive_cnt' and 'BTN_CFG_TIME_KEEPALIVE_PERIOD', it can support the long pressing function needs of various scenarios.

If 'BTN_CFG_TIME_KEEPALIVE_PERIOD 1000' is defined, then every 1 second, a 'KEEPALIVE (BTN_EVT_KEEPALIVE)' event will be reported. After receiving the reported event, the application layer will execute function A when 'keepalive_cnt==3', function B when 'keepalive_cnt==5', and function C when 'keepalive_cnt==30'.

Embedded button driver | Support single click, double click, multi click, automatic debounce, long press, button trigger without being interrupted by other buttons | Combo button support, do not generate single button events repeatedly | Static registration support



## Simple and flexible event types

Reference [lwbtn](https://github.com/MaJerle/lwbtn) Implementation: When a button event occurs, only four types of events are reported. By using 'click_cnt' and 'keepalive_cnt' to support flexible button clicking and long pressing functions, this design greatly simplifies code behavior and reduces subsequent maintenance costs.


```c
typedef enum
{
    BTN_EVT_ONPRESS = 0x00, /*!< On press event - sent when valid press is detected */
    BTN_EVT_ONRELEASE,      /*!< On release event - sent when valid release event is detected (from active to inactive) */
    BTN_EVT_ONCLICK,        /*!< On click event - sent when valid sequence of on-press and on-release events occurs */
    BTN_EVT_KEEPALIVE,      /*!< Keep alive event - sent periodically when button is active */
} xbtn_evt_t;
```



# Code structure

The code structure is as follows:

- **xenon_button**: Button Driver, mainly including XenonButton management
- **README.md**: Explanation document

```
xenon_button
 ├── xenon_button.c
 ├── xenon_button.h
 └── README.md
```



# Instruction for use

## Easy to use steps

Step1: Define button ID, button parameters, button arrays, and combo button arrays.

```c

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
```

Step2: Define button state acquisition functions and button event handling functions, initialize button drivers.

```c
static uint8_t prv_btn_get_state(xenon_btn_t *btn)
{

}

static void prv_btn_event(xenon_btn_t *btn, xenon_btn_evt_t evt)
{

}

xbtn_init(btns, BTN_ARRAY_SIZE(btns), btns_combo, BTN_ARRAY_SIZE(btns_combo), prv_btn_get_state, prv_btn_event);
```


Step3: Start button scanning, which can be implemented using a timer, task activation, or polling processing. Please note that the current system clock 'get_tick()' needs to be passed to the driver interface 'xbtn_decess'.

```c
while (1)
{
    /* Process forever */
    xbtn_process(get_tick());

    /* Artificial delay */
    HAL_Delay(10);
}
```



## Explanation of Macro Definition for Button Configuration Parameters

The buttons trigger different events at different times, and currently each button can be configured with the following parameters.

| Definition                     | Description                                                                    |
| ------------------------------ | ------------------------------------------------------------------------------ |
| BTN_CFG_TIME_DEBOUNCE_PRESS    | Config to set press debounce time                                              |
| BTN_CFG_TIME_DEBOUNCE_RELEASE  | Config to set release debounce time                                            |
| BTN_CFG_TIME_CLICK_PRESS_MIN   | Config to set minimum pressed time for valid click event                       |
| BTN_CFG_TIME_CLICK_PRESS_MAX   | Config to set maximum pressed time for valid click event                       |
| BTN_CFG_TIME_CLICK_MULTI_MAX   | Config to set maximum time between 2 clicks to be considered consecutive click |
| BTN_CFG_TIME_KEEPALIVE_PERIOD  | Config to set time for periodic keep alive event                               |
| BTN_CFG_TIME_SINGLE_BTN_DELAY  | Config to set delay time after processing a single button                      |
| BTN_CFG_TIME_COMBO_BTN_DELAY   | Config to set delay time after processing a combo button                       |
| BTN_CFG_TIME_INVALID_BTN_DELAY | Config to set delay time after processing invalid buttons                      |
| BTN_CFG_CLICK_MAX_CONSECUTIVE  | Config to set max number of consecutive clicks                                 |
| BTN_CFG_USE_CLICK              | Config to set whether to use the click function                                |
| BTN_CFG_USE_KEEPALIVE          | Config to set whether to use the long press function                           |



## Structure Description

### Button Control Structure Description - xbtn_btn_t

Each button has a management structure for recording the current status, button parameters, and other information of the button.

| Definition          | Description                                                                |
| ------------------- | -------------------------------------------------------------------------- |
| id                  | User defined button ID information, this value is recommended to be unique |
| curr_state          | Used to record the current state of buttons                                |
| last_state          | Used to record the last state of buttons                                   |
| flags               | Private button flags management                                            |
| event_mask          | Private button event mask management                                       |
| is_combo            | Used to record whether the button is a combo button                        |
| time_change         | Time in ms when button state got changed last time after valid debounce    |
| time_state_change   | Time in ms when button state got changed last time                         |
| click_cnt           | Number of consecutive clicks detected                                      |
| click_last_time     | Time in ms of last successfully detected click event                       |
| keepalive_cnt       | Number of keep alive events sent after successful on-press detection       |
| keepalive_last_time | Time in ms of last send keep alive event                                   |



```c
typedef struct xenon_button
{
    uint8_t id;                     /*!< User defined button ID information */

    uint16_t curr_state  : 1;       /*!< Used to record the current state of buttons */
    uint16_t last_state  : 1;       /*!< Used to record the last state of buttons */
    uint16_t flags       : 6;       /*!< Private button flags management */
    uint16_t event_mask  : 5;       /*!< Private button event mask management */
    uint16_t is_combo    : 1;       /*!< Indicate button or combo-button */
    uint16_t reserve     : 2;       /*!< Reserve */

    xbtn_time_t time_change;        /*!< Time in ms when button state got changed last time after valid debounce */
    xbtn_time_t time_state_change;  /*!< Time in ms when button state got changed last time */

#if BTN_CFG_USE_CLICK
    uint8_t click_cnt;              /*!< Number of consecutive clicks detected, respecting maximum timeoutbetween clicks */
    xbtn_time_t click_last_time;    /*!< Time in ms of last successfully detected (not sent!) click event */
#endif

#if BTN_CFG_USE_KEEPALIVE
    uint16_t keepalive_cnt;         /*!< Number of keep alive events sent after successful on-press
                                        detection. Value is reset after on-release */
    xbtn_time_t keepalive_last_time;/*!< Time in ms of last send keep alive event */
#endif

} xenon_btn_t;
```



### Button driver management structure - xbtn_t

The button driver needs to manage all static registered button and combo button information, and record the interface and button status.

| Definition         | Description                       |
| ------------------ | --------------------------------- |
| btn                | Pointer to single-buttons array   |
| btn_cnt            | Number of single-buttons in array |
| btn_combo          | Pointer to combo-buttons array    |
| btn_combo_cnt      | Number of combo-buttons in array  |
| btn_process_cnt    | Number of button in process       |
| btn_process_idx    | Idx of single-button in process   |
| btn_2_process_flag | Flag of two-button in process     |
| cbtn_process_idx   | Idx of combo-button in process    |
| cbtn_process_flag  | Flag of combo-button in process   |
| btn_2_process_time | Time of two-button in process     |
| btn_invalid_time   | Time of button invalid            |
| evt_fn             | Pointer to event function         |
| get_state_fn       | Pointer to get state function     |



```c
typedef struct xbtn_obj
{
    xenon_btn_t *btns;              /*!< Pointer to single-buttons array */
    uint8_t btns_cnt;               /*!< Number of single-buttons in array */
    xenon_btn_t *btns_combo;        /*!< Pointer to combo-buttons array */
    uint8_t btns_combo_cnt;         /*!< Number of combo-buttons in array */
    uint8_t btn_process_cnt;        /*!< Number of button in process */
    uint8_t btn_process_idx : 7;    /*!< Idx of single-button in process */
    uint8_t btn_2_process_flag : 1; /*!< Flag of two-button in process */
    uint8_t cbtn_process_idx : 7;   /*!< Idx of combo-button in process */
    uint8_t cbtn_process_flag : 1;  /*!< Flag of combo-button in process */
    xbtn_time_t btn_2_process_time; /*!< Time of two-button in process */
    xbtn_time_t btn_invalid_time;   /*!< Time of button invalid */

    xbtn_evt_fn evt_fn;             /*!< Pointer to event function */
    xbtn_get_state_fn get_state_fn; /*!< Pointer to get state function */
} xbtn_t;
```



## Operation API

### Core API

The main API is to initialize and run the interface.

```c
int xbtn_init(xenon_btn_t *btn, uint8_t btn_cnt, xenon_btn_t *cbtn,
        uint8_t btn_combo_cnt, xbtn_get_state_fn get_state_fn, xbtn_evt_fn evt_fn);
void xbtn_process(xbtn_time_t mstime);
```



### Other API

Some utility functions can be used as needed.

```c
uint8_t xenon_btn_is_active(const xenon_btn_t *btn);
uint8_t xenon_btn_is_in_process(const xenon_btn_t *btn);
uint8_t xbtn_is_in_process(void);
```



# Button Core Processing Logic Description

Reference [User Manual - LwBTN Document](https://docs.majerle.eu/projects/lwbtn/en/latest/user-manual/index.html#how-it-works) Explain the button implementation mechanism of this driver.

During the button lifetime, application shall anticipate to receive some of the below listed events:

- `BTN_EVT_ONPRESS`(Abbr: `ONPRESS`), event is sent to application whenever input goes from inactive to active state and minimum debounce time passes by.
- `BTN_EVT_ONRELEASE`(Abbr: `ONRELEASE`), event is sent to application whenever input sent `ONPRESS` event prior to that and when input goes from active to inactive state.
- `BTN_EVT_KEEPALIVE`(Abbr: `KEEPALIVE`), event is periodically sent between `ONPRESS` and `ONRELEASE` events.
- `BTN_EVT_ONCLICK`(Abbr: `ONCLICK`), event is sent after `ONRELEASE` and only if active button state was within allowed window for valid click event.



## ONPRESS Event

`ONPRESS` event is the first in a row when input is detected active. With the nature of embedded systems and various buttons connected to devices, it is necessary to filter out potential noise to ignore unintential multiple presses. This is done by checking line to be at the stable level for at least some minimum time, normally called debounce time. Usually it takes around 20ms.



## ONRELEASE Event

`ONRELEASE` event is triggered immediately when input goes from active to inactive state, and only if `ONPRESS` event has been detected prior to that.



## ONCLICK Event

`ONCLICK` event is triggered after a combination of multiple events:

- `ONPRESS` event shall be detected properly, indicating button has been pressed
- `ONRELEASE` event shall be detected, indicating button has been released
- Time between `ONPRESS` and `ONRELEASE` events has to be within time window

When conditions are met, `ONCLICK` event is sent, either immediately after `ONRELEASE` or after certain timeout after `ONRELEASE` event.



## Multi-Click Event

Multi-click feature is where timeout for `ONCLICK` event comes into play. Idea behind timeout feature is to allow multiple presses and to only send `ONCLICK` once for all presses, including the number of detected presses during that time. This let’s the application to react only once with known number of presses.



## KEEPALIVE Event

`KEEPALIVE` event is sent periodically between `ONPRESS` and `ONRELEASE` events. It can be used to detect application is still alive and provides counter how many `KEEPALIVE` events have been sent up to the point of event.




