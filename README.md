# MdButton

MdButton是一个纯C实现的按键处理库，支持按下、单击、双击、连击、短按、长按，基于事件驱动。

本按键库可用于裸机及RTOS场景，只作为中间层代码，不依赖具体硬件平台。

## 资源统计

很少，待补充...

## 示例程序说明

示例程序是基于RT-Thread下实现的，当然可以用于裸机平台(待补充...)

### 定义按键对象

```C
typedef enum
{
    USER_BUTTON_0 = 0, // 对应 IoT Board 开发板的 PIN_KEY0
    USER_BUTTON_MAX
} user_button_t;

static md_button_t user_button[USER_BUTTON_MAX];
```

定义了 1 个按键，存储在 `user_button` 中。

### 按键属性配置

`user_button_init();` 初始化代码如下所示：

```C
static void test_button_init(void)
{
    int i;
    uint32_t button_cnt = 0;
    rt_memset(&user_button[0], 0x0, sizeof(user_button));

    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    md_button_set_scan_freq(100); // 配置按键扫描频率
    LOG_D("button scan freqence: %dHz", 100);

    for (i = 0; i < USER_BUTTON_MAX; i ++)
    {
        user_button[i].usr_button_read = common_btn_read;
        user_button[i].cb = common_btn_evt_cb;
        user_button[i].active_level = 0;
        user_button[i].short_press_duration = md_button_ms2cnt(1500);
        user_button[i].long_press_duration = md_button_ms2cnt(3000);
        user_button[i].max_multiple_clicks_interval = md_button_ms2cnt(200);
        button_cnt = md_button_add(&user_button[i]);
    }
    LOG_D("button add ok, total num: %d", button_cnt);
}
```

核心的配置如下：

|属项|说明|
| :---- | :----|
| usr_button_read | 键值读取回调 |
| cb | 按键事件回调函数 |
| active_level | 按键按下时的逻辑电平 |
| short_press_duration | 短按判定时间，使用 md_button_ms2cnt函数转化为实际扫描次数 |
| long_press_duration | 长按判定时间，使用 md_button_ms2cnt函数转化为实际扫描次数 |
| max_multiple_clicks_interval | 最大连击判定间隔时间，使用 md_button_ms2cnt函数转化为实际扫描次数 |

### 按键事件回调

```C
static void common_btn_evt_cb(void *arg)
{
    md_button_t *btn = (md_button_t *)arg;

    LOG_I("id: [%d - %s]  event: [%25s]  repeat: %d", 
        btn->id, enum_btn_id_string[btn->id],
        enum_event_string[btn->event],
        btn->click_cnt);
}
```

回调函数，会传入md_button_t 对象，包含了按键的id，触发的事件以及重复次数

![image-20230323000838721](E:\freetime_works\md_button\figures\按键事件回调.png)

## 代码说明

### 事件定义

```C
typedef enum
{
    MD_BTN_PRESS_DOWN = 0, // 按下就会触发
    MD_BTN_CLICK, // 单击
    MD_BTN_DOUBLE_CLICK, // 双击
    MD_BTN_REPEAT_CLICK, // 三连击及以上
    MD_BTN_SHORT_PRESS_START, // 短按开始
    MD_BTN_SHORT_PRESS_UP, // 短按结束
    MD_BTN_LONG_PRESS_START, // 长按开始
    MD_BTN_LONG_PRESS_UP, // 长按结束
    MD_BTN_EVT_MAX,
    MD_BTN_EVT_NONE,
} md_button_event_t;
```

### API说明

```C
void md_button_set_scan_freq(uint32_t freq);
```

```c
uint32_t md_button_ms2cnt(uint32_t ms);
```

```c
int32_t md_button_add(md_button_t *btn);
```

```C
md_button_event_t md_button_get_event(md_button_t* btn);
```

```C
uint32_t md_button_scan(void);
```

## 其他

- 回调函数

    按键事件回调以及键值读取回调，都是在md_button_scan中进行调用的。因此请注意以下两点:

    1. 事件回调函数请不要阻塞，即使阻塞也不要阻塞太长时间，不要影响按键扫描频率
    2. 请注意栈需求，裸机下，请不要让回调函数消耗太多栈；RTOS下，请注意调用md_button_scan的线程栈的大小

- 中断使用

    请注意，如果你使用中断方式触发按键扫描，那你便不要想实现短按、长按了，毕竟谁家程序会一直在中断里啊！

    scan过程中，会一次确定所有按键的状态，你可以将所有按键中断统一为一个入口，然后执行按键扫描。或者说，你可以将所有按键，硬件上接一个或门，然后接到一个单独的IO口进行中断触发。

- 组合按键

    组合按键的定义，这个应该有业务进行定义与实现，因此不会实现在此中间层（我也不知道你需要哪些按键组合）。由于在一次扫描中，便扫描到了所有按键状态，所以你可以在按键事件回调中，再调用md_button_get_event函数获取指定按键的事件，再进行具体的判断。示例程序，待补充...

## 支持

<img src="E:/LiJiaJia/LizCubic/repo/LizCubic/doc/img/赞赏.png" style="zoom:50%;" /> 

如果 MdButton 解决了你的问题，不妨扫描上面二维码请我 **喝杯咖啡**~ 

## 许可

采用 GPL-2.0 开源协议，细节请阅读项目中的 LICENSE 文件内容。
