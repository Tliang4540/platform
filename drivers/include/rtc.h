#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

struct rtc_tm
{
    unsigned int sec;
    unsigned int min;
    unsigned int hour;
    unsigned int wday;
    unsigned int mday;
    unsigned int mon;
    unsigned int year;
};

void rtc_init(void);
void rtc_wakeup_time_open(unsigned int ms, void (*callback)(void));
void rtc_wakeup_time_close(void);
void rtc_set_time(struct rtc_tm *t);
void rtc_get_time(struct rtc_tm *t);
void rtc_localtime(unsigned int time, struct rtc_tm *t);
unsigned int rtc_mktime(struct rtc_tm *time);

#ifdef __cplusplus
}
#endif

#endif
