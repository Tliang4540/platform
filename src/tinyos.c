#include <tinyos.h>
#include <log.h>

#ifndef OS_GET_FREE_STACK_ENABLE
#define OS_GET_FREE_STACK_ENABLE (0)
#endif

#ifndef OS_TASK_NUM_MAX
#define OS_TASK_NUM_MAX         (5)
#endif

struct os_tcb
{
    unsigned int tick;
    unsigned int state;
    void *stack;
#if (OS_GET_FREE_STACK_ENABLE)
    void *stack_end;
#endif
};

static unsigned int os_cur_task_idx = 0;             // 当前任务索引
static struct os_tcb os_task_list[OS_TASK_NUM_MAX];  // 任务池

static unsigned int os_sys_tick = 0;   //系统时间

void os_tick_update(void)
{
    os_sys_tick++;
}

void _os_switch(void);
void _os_start(void);
void *_os_stack_init(void (*func)(void *), void *param, void *stack, unsigned int stack_size);

void *_os_cur_sp;    // 当前使用的任务栈
/**
 * @brief 更新就绪任务栈指针
 */
void os_update_sp(void)
{
    unsigned int i;
    os_task_list[os_cur_task_idx].stack = _os_cur_sp;

    for (i = 0; i < (OS_TASK_NUM_MAX - 1); i++)
    {
        // 为当前任务、空任务、挂起任务直接跳过
        if ((i == os_cur_task_idx) || (os_task_list[i].state == 0))
            continue;

        // 时间到执行
        if ((os_sys_tick - os_task_list[i].tick) < (unsigned int)(OS_TICK_MAX))
        {
            break;
        }
    }
    os_cur_task_idx = i;

    _os_cur_sp = os_task_list[os_cur_task_idx].stack;
}

int os_task_create(void (*func)(void *), void *param, void *stack, unsigned int stack_size)
{
    unsigned int task_id;

    for (task_id = 0; task_id < (OS_TASK_NUM_MAX - 1); task_id++)
    {
        if (os_task_list[task_id].state == 0)
        {
            break;
        }
    }
    // 有一个是空闲任务
    if (task_id >= (OS_TASK_NUM_MAX - 1))
        return -1;

    // 初始化任务栈
    #if (OS_GET_FREE_STACK_ENABLE)
    os_task_list[task_id].stack_end = stack;
    for (unsigned int i = 0; i < stack_size / 4; i++)
    {
        ((unsigned int *)stack)[i] = 0xA5A5A5A5;
    }
    #endif
    os_task_list[task_id].stack = _os_stack_init(func, param, stack, stack_size);
    os_task_list[task_id].tick = os_sys_tick;
    os_task_list[task_id].state = 1;

    return task_id;
}

void os_task_delete(unsigned int task_id)
{
    os_task_list[task_id].state = 0;
    if (task_id == os_cur_task_idx)
        _os_switch();
}

static void os_idle_task(void *param)
{
    (void)param;
    while(1)
    {
       _os_switch();
    }
}

void os_start(void)
{
    static unsigned int idle_stack[32];

    os_cur_task_idx = OS_TASK_NUM_MAX - 1;

    os_task_list[os_cur_task_idx].stack = _os_stack_init(os_idle_task, 0, idle_stack, sizeof(idle_stack));

    _os_cur_sp = os_task_list[os_cur_task_idx].stack;

    _os_start();
}

void os_delay(unsigned int tick)
{
    LOG_ASSERT(tick <= OS_TICK_MAX);
    os_task_list[os_cur_task_idx].tick = os_sys_tick + tick;
    _os_switch();
}

unsigned int os_get_tick(void)
{
    return os_sys_tick;
}

#if (OS_GET_FREE_STACK_ENABLE)
unsigned int os_get_free_stack(void)
{
    unsigned int *stack_end = os_task_list[os_cur_task_idx].stack_end;
    unsigned int free_size = 0;
    
    while (*stack_end++ == 0xA5A5A5A5)
        free_size += 4;

    return free_size;
}
#endif

void os_msg_init(os_msg_t *msg)
{
    msg->msg = OS_INVALID_VAL;
    msg->wait_id = OS_INVALID_VAL;
}

void os_msg_send(os_msg_t *msg, unsigned int data)
{
    msg->msg = data;
    // 如果有任务等待唤醒任务
    if (msg->wait_id < (OS_TASK_NUM_MAX - 1))
    {
        os_task_list[msg->wait_id].state = 1;
        os_task_list[msg->wait_id].tick = os_sys_tick;
    }
}

int os_msg_recv(os_msg_t *msg, unsigned int *data, unsigned int tick)
{
    int ret = 0;
    
    //先设置等待参数
    msg->wait_id = os_cur_task_idx; 
    if (tick != OS_INVALID_VAL)
    {
        LOG_ASSERT(tick <= OS_TICK_MAX);
        os_task_list[os_cur_task_idx].tick = os_sys_tick + tick;
    }
    else
    {
        os_task_list[os_cur_task_idx].state = 0;
    }

    // 如果没有消息切换任务
    if (msg->msg == OS_INVALID_VAL)
    {
        _os_switch();
        if (msg->msg == OS_INVALID_VAL)
        {
            ret = -1;
        }
        else if (data)
        {
            *data = msg->msg;   
        }
    }
    else if (data)
    {
        *data = msg->msg;
        os_task_list[os_cur_task_idx].state = 1;
    }

    msg->msg = OS_INVALID_VAL;
    msg->wait_id = OS_INVALID_VAL;
    return ret;
}
