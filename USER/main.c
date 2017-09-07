#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "includes.h"
#include "os_app_hooks.h"

//ALIENTEK 探索者STM32F407开发板 UCOSIII实验
//例4-1 UCOSIII UCOSIII移植

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

//定义任务优先级
#define START_TASK_PRIORITY   7
//定义任务堆栈大小
#define START_STACK_SIZE      128
//定义任务块
OS_TCB  start_task_tcb;

CPU_STK start_stack_size[START_STACK_SIZE];

void StartTask(void *parg);


#define LED1_TASK_PRIORITY    8
#define LED1_STACK_SIZE       128
OS_TCB  led1_task_tcb;
CPU_STK led1_stack_size[LED1_STACK_SIZE];
void Led1Task(void *parg);


#define LED2_TASK_PRIORITY    9
#define LED2_STACK_SIZE       128
OS_TCB  led2_task_tcb;
CPU_STK led2_stack_size[LED2_STACK_SIZE];
void Led2Task(void *parg);


void Tmr1Callback(void *ptmr,void *parg);
OS_TMR tmr1;

#define KEY_MESSAGE_NUM            1
OS_Q KeyMessage;

int main(void)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    
    delay_init(168);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    LED_Init();
    KEY_Init();
    OSInit(&err);
    
    OS_CRITICAL_ENTER();
    
    OSTaskCreate((OS_TCB      *)&start_task_tcb,
                 (CPU_CHAR    *)"start task",
                 (OS_TASK_PTR  )StartTask,
                 (void        *)0,
                 (OS_PRIO      )START_TASK_PRIORITY,
                 (CPU_STK     *)&start_stack_size[0],
                 (CPU_STK_SIZE )START_STACK_SIZE/10,
                 (CPU_STK_SIZE )START_STACK_SIZE,
                 (OS_MSG_QTY   )0,
                 (OS_TICK      )0,
                 (void        *)0,
                 (OS_OPT       )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR      *)&err);
     
    OS_CRITICAL_EXIT();              
    OSStart(&err);
                 
    return 0;
}


void StartTask(void *parg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    
    CPU_Init();
    
#if OS_CFG_STAT_TASK_EN >0u
    OSStatTaskCPUUsageInit(&err);
#endif    
    
#ifdef CPU_CFG_INT_DIS_MEAS_EN
       CPU_IntDisMeasMaxCurReset();
#endif
    
#if OS_CFG_SCHED_ROUND_ROBIN_EN
    OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);
#endif
    
    OS_CRITICAL_ENTER();
    
    OSTmrCreate ((OS_TMR      *)&tmr1,
                 (CPU_CHAR    *)"tm1",
                 (OS_TICK      )0,
                 (OS_TICK      )50, //50*10ms
                 (OS_OPT       )OS_OPT_TMR_PERIODIC,
                 (OS_TMR_CALLBACK_PTR)Tmr1Callback,
                 (void        *)0,
                 (OS_ERR      *)&err);
    
    OSQCreate  ((OS_Q     *)&KeyMessage,
                (CPU_CHAR *)"Key Message",
                (OS_MSG_QTY )KEY_MESSAGE_NUM,
                (OS_ERR   *)&err);
                 
   	OSTaskCreate((OS_TCB 	* )&led1_task_tcb,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )Led1Task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIORITY,     
                 (CPU_STK   * )&led1_stack_size[0],	
                 (CPU_STK_SIZE)LED1_STACK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STACK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
    
   	OSTaskCreate((OS_TCB 	* )&led2_task_tcb,		
				 (CPU_CHAR	* )"Task2 task", 		
                 (OS_TASK_PTR )Led2Task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED2_TASK_PRIORITY,     
                 (CPU_STK   * )&led2_stack_size[0],	
                 (CPU_STK_SIZE)LED2_STACK_SIZE/10,	
                 (CPU_STK_SIZE)LED2_STACK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);
   

                
    
    
   OS_CRITICAL_EXIT();
                 
   OSTmrStart(&tmr1,&err);
                 
   OSTaskDel(0,&err);
}


void Led1Task(void *parg)
{
    OS_ERR err;
    CPU_SR_ALLOC();
    
    parg = parg;
    
    
    while(1)
    {
        GPIO_ResetBits(GPIOA,GPIO_Pin_6);
        OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
        GPIO_SetBits(GPIOA,GPIO_Pin_6);
        OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
    }
}


void Led2Task(void *parg)
{
    u8 *key;
    OS_MSG_SIZE size;
    OS_ERR err;
    CPU_SR_ALLOC();
    
    parg = parg;
    
    
    while(1)
    {
        key = OSQPend(&KeyMessage,0,OS_OPT_PEND_BLOCKING,&size,0,&err);
        if(*key==1)
        {
            GPIO_SetBits(GPIOA,GPIO_Pin_7);
        }
        else
        {
            GPIO_ResetBits(GPIOA,GPIO_Pin_7);
        }
    }
}


void Tmr1Callback(void *ptmr,void *parg)
{
    //static u8 key;
    volatile static u8 data[18]; 
    OS_ERR err;
   // key = KeyDetect();
    OSQPost(&KeyMessage,(void *)data,18,OS_OPT_POST_FIFO,&err);
//    switch(key)
//    {
//        case 0:OSQPost(&KeyMessage,&key,1,OS_OPT_POST_FIFO,&err); break;
//        case 1:OSQPost(&KeyMessage,&key,1,OS_OPT_POST_FIFO,&err); break;
//        default: break;
//    }
    
}

