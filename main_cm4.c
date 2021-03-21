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
    vTaskStartScheduler();
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
