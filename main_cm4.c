/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
//https://github.com/Markedydoodle/Labo4.cydsn 

#include "project.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "params.h"
#include "queue.h"

//Part 2 variable globale
SemaphoreHandle_t bouton_semph=NULL; //...
int button_count = 0;

//Part 1
void LED_TASK()
{
    for(;;)
    {
        Cy_GPIO_Write(RED_PORT,RED_NUM,1);
        vTaskDelay(pdMS_TO_TICKS(500));
        Cy_GPIO_Write(RED_PORT,RED_NUM,0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}


//Part 2
void isr_bouton()
{
    CyDelay(50);
    if(button_count==2)
    {button_count=0;}
    
    button_count++;
    xSemaphoreGiveFromISR(bouton_semph,NULL);
    Cy_GPIO_ClearInterrupt(Bouton_0_PORT,Bouton_0_NUM);
    NVIC_ClearPendingIRQ(bouton_isr_cfg.intrSrc);
    
}

void bouton_task()
{     
    for(;;)
    {
        if(bouton_semph!=NULL)          //s'il a ete creer et peut etre utilise
        {
            if(xSemaphoreTake(bouton_semph,pdMS_TO_TICKS(20))== pdTRUE)       //la 2e variable représente le temps en ticks pr attendre que sémaphore devient 
            {
                if(button_count==1)
                {UART_PutString(" Bouton appuye ");
                UART_PutString("\r\n");}
                else if (button_count==2)
                {UART_PutString(" Bouton relache ");
                UART_PutString("\r\n");}
            }
        }
    }
    xSemaphoreGive(bouton_semph);
}

//Part 3a
task_params_t task_A = {
    .delay = 1000,
    .message = "Tache A en cours\n\r"
};

task_params_t task_B = {
    .delay = 999,
    .message = "Tache B en cours\n\r"
};

void print_loop(task_params_t * params)
{
    for(;;)
    {
        vTaskDelay(pdMS_TO_TICKS(params->delay));
        UART_PutString(params->message);                  //Consiste à la fin du code original pour 3a
        //xQueueSend(print_queue, (task_params_t*) &params->message, pdMS_TO_TICKS(params->delay));           //Consiste à un essaie de faire 3b
    }
}

//Part 3b
QueueHandle_t print_queue;

void print()
{
    char*message;
    for (;;) {
        xQueueReceive(print_queue, &message, portMAX_DELAY);
        UART_PutString(message);
    }
}

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    //Part 2
    bouton_semph = xSemaphoreCreateBinary();
    
    Cy_SysInt_Init(&bouton_isr_cfg, isr_bouton);
    NVIC_ClearPendingIRQ(bouton_isr_cfg.intrSrc);
    NVIC_EnableIRQ(bouton_isr_cfg.intrSrc);
    
    UART_Start();
    
    //Part 1
    xTaskCreate(LED_TASK,"LED_TASK",80,NULL,3,NULL);
    xTaskCreate(bouton_task,"samaphore_TASK",400,NULL,1,NULL);
    
    //Part 3a
    xTaskCreate(
        print_loop,
        "Task A",
        configMINIMAL_STACK_SIZE,
        (void *) &task_A,
        1,
        NULL);  
    xTaskCreate(
        print_loop,
        "Task B",
        configMINIMAL_STACK_SIZE,
        (void *) &task_B,
        1,
        NULL);
    
    //Part 3b
    print_queue = xQueueCreate(2, sizeof(char *));
    
    
    vTaskStartScheduler();
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
