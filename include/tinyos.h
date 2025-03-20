#ifndef __TINYOS_H__
#define __TINYOS_H__

#define OS_INVALID_VAL          (0xffffffff)
#define OS_TICK_MAX             (0x7fffffff)

typedef struct os_msg
{
    volatile unsigned int wait_id;
    volatile unsigned int msg;
}os_msg_t;

/**
 * @brief 创建任务
 * @param [in] func 任务入口函数
 * @param [in] param 传递给任务的参数
 * @param [in] stack 任务栈地址
 * @param [in] stack_size 任务栈大小
 * @retval -1: 失败 other: 任务ID
 */
int os_task_create(void (*func)(void *), void *param, void *stack, unsigned int stack_size);

/**
 * @brief 删除任务
 * @param [in] task_id 需要删除的任务ID
 */
void os_task_delete(unsigned int task_id);

/**
 * @brief 系统启动
 */
void os_start(void);

/**
 * @brief 系统时钟更新
 */
void os_tick_update(void);

/**
 * @brief 任务延时
 * @param [in] tick 延时多少个系统tick
 * @note 超过OS_TICK_MAX会溢出
 */
void os_delay(unsigned int tick);

unsigned int os_get_tick(void);
unsigned int os_get_free_stack(void);

/**
 * @brief 接收一个消息
 * @param [in] msg 消息句柄
 * @param [out] data 接收到的数据
 * @param [in] tick 超时周期, OS_INVALID_VAL:永久等待
 * @retval 0:成功 other:超时
 */
int os_msg_recv(os_msg_t *msg, unsigned int *data, unsigned int tick);

/**
 * @brief 发送一个消息
 * @param [in] msg 消息句柄
 * @param [in] data 发送的数据
 */
void os_msg_send(os_msg_t *msg, unsigned int data);

/**
 * @brief 初始化一个消息
 * @param [in] msg 消息句柄
 */
void os_msg_init(os_msg_t *msg);

#endif
