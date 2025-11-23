# 简介

XenonButton 是一个基于C语言的轻量级按键库，主要用于嵌入式系统中，支持多种按键事件处理和组合按键配置。

项目开发过程中参考了如下几个项目[FlexibleButton]、[lwbtn]、[easy_button]

其中核心的按键管理机制借鉴的是[lwbtn]，并在其基础上做了大量改动，事件上报行为和原本处理有些不同。
按键触发时不会被其他按键打断，组合按键不重复产生单按键事件，这是两个很独特的特性，是与目前市面上很多按键驱动不一样的地方。

这个按键驱动有C51轮询、STM32轮询、STM32-RTOS三个版本，针对不同版本进行了调整，Linux系统可以使用STM32-RTOS版本。

主简介以STM32-RTOS版本为例，进行代码结构和使用说明介绍。



## 长按支持 & 组合按键支持

实际项目中会遇到各种功能需求，如长按3s是功能A，长按5s是功能B，长按30s是功能C。通过`keepalive_cnt`和`BTN_CFG_TIME_KEEPALIVE_PERIOD`的设计，能够支持各种场景的长按功能需要。

如定义`BTN_CFG_TIME_KEEPALIVE_PERIOD 1000`，那么每隔1s会上报一个`KEEPALIVE(BTN_EVT_KEEPALIVE)`事件，应用层在收到上报事件后，当`keepalive_cnt==3`时，执行功能A；当`keepalive_cnt==5`时，执行功能B；当`keepalive_cnt==30`时，执行功能C。

嵌入式按键处理驱动 | 支持单击、双击、多连击、自动消抖、长按、超长按、按键触发不被打断 | 组合按键支持、不重复产生单按键事件 | 静态注册支持



## 简易但灵活的事件类型

参考[lwbtn](https://github.com/MaJerle/lwbtn)实现，当有按键事件发生时，所上报的事件类型只有4种，通过`click_cnt`和`keepalive_cnt`来支持灵活的按键点击和长按功能需要，这样的设计大大简化了代码行为，也大大降低了后续维护成本。


```c
typedef enum
{
    BTN_EVT_ONPRESS = 0x00, /*!< On press event - sent when valid press is detected */
    BTN_EVT_ONRELEASE,      /*!< On release event - sent when valid release event is detected (from active to inactive) */
    BTN_EVT_ONCLICK,        /*!< On Click event - sent when valid sequence of on-press and on-release events occurs */
    BTN_EVT_KEEPALIVE,      /*!< Keep alive event - sent periodically when button is active */
} xbtn_evt_t;
```



# 代码结构

代码结构如下所示：

- **xenon_button**：驱动库，主要包含XenonButton管理
- **README.md**：说明文档

```
xenon_button
 ├── xenon_button.c
 ├── xenon_button.h
 └── README.md
```



# 使用说明

## 使用简易步骤

Step1：定义按键ID、按键参数和按键数组和组合按键数组。

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

Step2：定义按键状态获取函数和按键事件处理函数，初始化按键驱动。

```c
static uint8_t prv_btn_get_state(xenon_btn_t *btn)
{

}

static void prv_btn_event(xenon_btn_t *btn, xenon_btn_evt_t evt)
{

}

xbtn_init(btns, BTN_ARRAY_SIZE(btns), btns_combo, BTN_ARRAY_SIZE(btns_combo), prv_btn_get_state, prv_btn_event);
```


Step3：启动按键扫描，具体实现可以用定时器做，也可以启任务或者轮询处理。需要注意需要将当前系统时钟`get_tick()`传给驱动接口`xbtn_process`。

```c
while (1)
{
    /* Process forever */
    xbtn_process(get_tick());

    /* Artificial delay */
    HAL_Delay(10);
}
```



## 按键配置参数宏定义说明

按键根据不同的时间触发不同的事件，目前每个按键可以配置的参数如下。

| 名称                           | 说明                                                                           |
| ------------------------------ | ----------------------------------------------------------------------------- |
| BTN_CFG_TIME_DEBOUNCE_PRESS    | 防抖处理，按下防抖超时，配置为0，代表不启动                                       |
| BTN_CFG_TIME_DEBOUNCE_RELEASE  | 防抖处理，松开防抖超时，配置为0，代表不启动                                       |
| BTN_CFG_TIME_CLICK_PRESS_MIN   | 按键超时处理，按键最短时间，配置为0，代表不检查最小值                              |
| BTN_CFG_TIME_CLICK_PRESS_MAX   | 按键超时处理，按键最长时间，配置为0xFFFF，代表不检查最大值，用于区分长按和按键事件   |
| BTN_CFG_TIME_CLICK_MULTI_MAX   | 多击处理，两个按键之间认为是连击的超时时间                                        |
| BTN_CFG_TIME_KEEPALIVE_PERIOD  | 长按处理，长按周期，每个周期增加keepalive_cnt计数                                 |
| BTN_CFG_TIME_SINGLE_BTN_DELAY  | 单个按键延时处理，单个按键处理结束后的延时时间                                     |
| BTN_CFG_TIME_COMBO_BTN_DELAY   | 组合按键延时处理，组合按键处理结束后的延时时间                                     |
| BTN_CFG_TIME_INVALID_BTN_DELAY | 无效按键延时处理，无效按键处理结束后的延时时间                                     |
| BTN_CFG_CLICK_MAX_CONSECUTIVE  | 最大连击次数，配置为0，代表不进行连击检查                                         |
| BTN_CFG_USE_CLICK              | 是否使用连击功能，配置为0，代表不使用连击功能                                      |
| BTN_CFG_USE_KEEPALIVE          | 是否使用长按功能，配置为0，代表不使用长按功能                                      |



## 结构体说明

### 按键控制结构体说明-xbtn_btn_t

每个按键有一个管理结构体，用于记录按键当前状态，按键参数等信息。

| 名称                | 说明                                                          |
| ------------------- | ------------------------------------------------------------ |
| id                  | 用户定义的按键ID信息，该值建议唯一                             |
| curr_state          | 用于记录按键当前的状态                                         |
| last_state          | 用于记录按键上次的状态                                         |
| flags               | 用于记录一些状态                                               |
| event_mask          | 用于记录按键事件的掩码                                         |
| is_combo            | 用于记录按键是否为组合按键                                      |
| time_change         | 记录按键按下或者松开状态的时间点                                |
| time_state_change   | 记录按键状态切换时间点（并不考虑防抖，单纯记录状态切换时间点）    |
| click_cnt           | 多击的次数                                                     |
| click_last_time     | 点击最后一次松开状态的时间点，用于管理click_cnt                  |
| keepalive_cnt       | 长按的KEEP_ALIVE次数                                           |
| keepalive_last_time | 长按最后一次上报长按时间的时间点，用于管理keepalive_cnt          |



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



### 按键驱动管理结构体-xbtn_t

按键驱动需要管理所有静态注册的按键和组合按键信息，并且记录接口以及按键状态。

| 名称               | 说明                         |
| ------------------ | --------------------------- |
| btn                | 管理静态注册按键的指针        |
| btn_cnt            | 记录静态注册按键的个数        |
| btn_combo          | 管理静态注册组合按键的指针     |
| btn_combo_cnt      | 记录静态注册组合按键的个数     |
| btn_process_cnt    | 记录按键正在处理的个数         |
| btn_process_idx    | 记录单个按键处理的序列号       |
| btn_2_process_flag | 记录两个按键处理的标志位       |
| cbtn_process_idx   | 记录组合按键处理的序列号       |
| cbtn_process_flag  | 记录组合按键处理的标志位       |
| btn_2_process_time | 记录两个按键按下状态的时间     |
| btn_invalid_time   | 记录无效按键状态的时间         |
| evt_fn             | 事件上报的回调接口             |
| get_state_fn       | 按键状态获取的回调接口         |



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



## 操作API

### 核心API

主要的就是初始化和运行接口。

```c
int xbtn_init(xenon_btn_t *btn, uint8_t btn_cnt, xenon_btn_t *cbtn,
        uint8_t btn_combo_cnt, xbtn_get_state_fn get_state_fn, xbtn_evt_fn evt_fn);
void xbtn_process(xbtn_time_t mstime);
```



### 其他API

一些工具函数，按需使用。

```c
uint8_t xenon_btn_is_active(const xenon_btn_t *btn);
uint8_t xenon_btn_is_in_process(const xenon_btn_t *btn);
uint8_t xbtn_is_in_process(void);
```



# 按键核心处理逻辑说明

这里参考[用户手册 — LwBTN 文档](https://docs.majerle.eu/projects/lwbtn/en/latest/user-manual/index.html#how-it-works)对本驱动的按键实现机制进行说明。

在驱动运行中，应用程序可以会接收到如下事件：

- `BTN_EVT_ONPRESS`(简称：`ONPRESS`)，每当输入从非活动状态变为活动状态并且最短去抖动时间过去时，都会将事件发送到应用程序
- `BTN_EVT_ONRELEASE`(简称：`ONRELEASE`)，每当输入发送 `ONPRESS`事件时，以及当输入从活动状态变为非活动状态时，都会将事件发送到应用程序
- `BTN_EVT_KEEPALIVE`(简称：`KEEPALIVE`)，事件在 `ONPRESS` 和`ONRELEASE`事件之间定期发送
- `BTN_EVT_ONCLICK`(简称：`ONCLICK`)，事件在`ONRELEASE`后发送，并且仅当活动按钮状态在有效单击事件的允许窗口内时发送



## ONPRESS事件

`ONPRESS` 事件是检测到按键处于活动状态时的第一个事件。 由于嵌入式系统的性质和连接到设备的各种按钮，有必要过滤掉潜在的噪音，以忽略无意的多次按下。 这是通过检查输入至少在一些最短时间内处于稳定水平来完成的，通常称为*消抖时间*，通常需要大约`20ms` 。

按键*消抖时间*分为按下消抖时间`BTN_CFG_TIME_DEBOUNCE_PRESS`和松开消抖时间`BTN_CFG_TIME_DEBOUNCE_RELEASE`。



## ONRELEASE事件

当按键从活动状态变为非活动状态时，才会立即触发 `ONRELEASE`事件，前提是在此之前检测到`ONPRESS` 事件。也就是 `ONRELEASE`事件是伴随着`ONPRESS` 事件发生的。



## ONCLICK事件

`ONCLICK`事件在多个事件组合后触发：

- 应正确检测到`ONPRESS` 事件，表示按钮已按下
- 应检测到`ONRELEASE`事件，表示按钮已松开
- `ONPRESS`和`ONRELEASE`事件之间的时间必须在时间窗口内，也就是在`BTN_CFG_TIME_CLICK_PRESS_MIN`和`BTN_CFG_TIME_CLICK_PRESS_MAX`之间时

当满足条件时，在`ONRELEASE`事件之后的`BTN_CFG_TIME_CLICK_MULTI_MAX`时间，发送`ONCLICK`事件。



## Multi-Click事件

实际需求除了单击需求外，还需要满足多击需求。本驱动是靠`BTN_CFG_TIME_CLICK_MULTI_MAX`来满足此功能，虽然有多次点击，但是只发送**一次 `ONCLICK`事件**。

注意：想象一下，有一个按钮可以在单击时切换一盏灯，并在双击时关闭房间中的所有灯。 通过超时功能和单次点击通知，用户将只收到**一次点击**，并且会根据连续按压次数值，来执行适当的操作。

**Multi-Click**相关说明：`click_cnt`表示检测到的**Multi-Click** 事件数，将在最终的`ONCLICK`事件中上报。

需要注意前一个按键的`ONRELEASE`事件和下次的`ONPRESS`事件间隔时间应小于`BTN_CFG_TIME_CLICK_MULTI_MAX`，`ONCLICK`事件会在最后一次按键的`ONRELEASE`事件之后`BTN_CFG_TIME_CLICK_MULTI_MAX`时间上报。



## KEEPALIVE事件

`KEEPALIVE`事件在 `ONPRESS`事件和`ONRELEASE`事件之间定期发送，它可用于长按处理，根据过程中有多少`KEEPALIVE`事件以及`BTN_CFG_TIME_KEEPALIVE_PERIOD`可以实现各种复杂的长按功能需求。

需要注意这里根据配置的时间参数的不同，可能会出现`KEEPALIVE`事件和`ONCLICK`事件在一次按键事件都上报的情况。这个情况一般发生在按下保持时间（`ONPRESS`事件和`ONRELEASE`事件之间）大于`BTN_CFG_TIME_KEEPALIVE_PERIOD`却小于`BTN_CFG_TIME_CLICK_PRESS_MAX`的场景下。

而当按下保持时间大于`BTN_CFG_TIME_CLICK_PRESS_MAX`时，就不会上报`ONCLICK`事件。




