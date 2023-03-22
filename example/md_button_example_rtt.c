#include <rtthread.h>
#include <board.h>
#include "md_button.h"

#define DBG_TAG    "md_button"
#define DBG_LVL    DBG_LOG
// #define DBG_LVL    DBG_INFO
#include <rtdbg.h>

#ifndef PIN_KEY0
#define PIN_KEY0 GET_PIN(A, 0)
#endif

#define ENUM_TO_STR(e) (#e)

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_MAX
} user_button_t;

static char *enum_event_string[] = {
    ENUM_TO_STR(MD_BTN_PRESS_DOWN),
    ENUM_TO_STR(MD_BTN_CLICK),
    ENUM_TO_STR(MD_BTN_DOUBLE_CLICK),
    ENUM_TO_STR(MD_BTN_REPEAT_CLICK),
    ENUM_TO_STR(MD_BTN_SHORT_PRESS_START),
    ENUM_TO_STR(MD_BTN_SHORT_PRESS_UP),
    ENUM_TO_STR(MD_BTN_LONG_PRESS_START),
    ENUM_TO_STR(MD_BTN_LONG_PRESS_UP),
    ENUM_TO_STR(MD_BTN_EVT_MAX),
    ENUM_TO_STR(MD_BTN_EVT_NONE),
};

static char *enum_btn_id_string[] = {
    ENUM_TO_STR(USER_BUTTON_0),
    ENUM_TO_STR(USER_BUTTON_MAX),
};

static md_button_t user_button[USER_BUTTON_MAX];

static uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;

    md_button_t *btn = (md_button_t *)arg;

    switch (btn->id)
    {
    case USER_BUTTON_0:
        value = rt_pin_read(PIN_KEY0);
        break;
    default:
        RT_ASSERT(0);
    }

    return value;
}

static void common_btn_evt_cb(void *arg)
{
    md_button_t *btn = (md_button_t *)arg;

    LOG_I("id: [%d - %s]  event: [%25s]  repeat: %d", 
        btn->id, enum_btn_id_string[btn->id],
        enum_event_string[btn->event],
        btn->click_cnt);
}

static void button_scan(void *arg)
{
    while(1)
    {
        md_button_scan();
        rt_thread_mdelay(10); // 20 ms
    }
}

static void test_button_init(void)
{
    int i;
    uint32_t button_cnt = 0;
    rt_memset(&user_button[0], 0x0, sizeof(user_button));

    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP); /* set KEY pin mode to input */
    md_button_set_scan_freq(100);
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

int md_button_test(void)
{
    rt_thread_t tid = RT_NULL;

    test_button_init();

    /* Create background ticks thread */
    tid = rt_thread_create("btn_test", button_scan, RT_NULL, 1024, 10, 10);
    if(tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
INIT_APP_EXPORT(md_button_test);

int main(void *arg)
{
    return 0;
}
