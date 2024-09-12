#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Handle สำหรับ task ทั้งสอง
TaskHandle_t MyFirstTaskHandle = NULL;
TaskHandle_t MySeconeTaskHandle = NULL;

// Task แรก
void My_First_Task(void * arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My First Task %d\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i++;

        // ลบ task ที่สองเมื่อ i เท่ากับ 5
        if(i == 5)
        {
            vTaskDelete(MySeconeTaskHandle);
            printf("Second Task deleted\n");
        }
        if(i == 10)
		{
			vTaskResume(MySecondTaskHandle);
			printf("Second Task Resumed\n");
		}

		if(i == 15)
		{
			vTaskDelete(MySecondTaskHandle);
			printf("Second Task deleted\n");
		}

		if(i == 20)
		{
			printf("MyFirstTaskHandle will suspend itself\n");
			vTaskSuspend(NULL);
		}
    }
}

// Task ที่สอง
void My_Second_Task(void * arg)
{
    uint16_t i = 0;
    while(1)
    {
        printf("Hello My Second Task %d\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        i++;
    }
}

// ฟังก์ชันหลักของโปรแกรม
void app_main(void)
{
    // สร้างและรัน task แรก
    xTaskCreate(My_First_Task, "First_Task", 4096, NULL, 10, &MyFirstTaskHandle);

    // สร้างและรัน task ที่สอง
    xTaskCreate(My_Second_Task, "Second_Task", 4096, NULL, 10, &MySeconeTaskHandle);
}