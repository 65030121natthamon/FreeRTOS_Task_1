   #include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// กำหนดหมายเลขของ GPIO pins
#define LED_PIN 27
#define PUSH_BUTTON_PIN 33

// Handle สำหรับ task
TaskHandle_t ISR = NULL;

// Task ที่จะทำงานเมื่อ interrupt เกิดขึ้น
void interrupt_task(void *arg)
{
    bool led_status = false;
    while (1)
    {
        vTaskSuspend(NULL);  // Suspend ตัวเองไว้จนกว่าจะได้รับการปลุก
        led_status = !led_status;
        gpio_set_level(LED_PIN, led_status);
        printf("Button pressed!\n");
    }
}

// Interrupt handler สำหรับปุ่มกด
void IRAM_ATTR button_isr_handler(void *arg)
{
    BaseType_t high_task_awoken = pdFALSE;
    xTaskResumeFromISR(ISR);
    portYIELD_FROM_ISR(high_task_awoken);
}

void app_main(void)
{
    gpio_config_t io_conf;
    
    // ตั้งค่า GPIO สำหรับปุ่มกด
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PUSH_BUTTON_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    // ตั้งค่า GPIO สำหรับ LED
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // ติดตั้ง ISR service
    if (gpio_install_isr_service(0) != ESP_OK)
    {
        printf("Failed to install ISR service\n");
        return;
    }

    // ลงทะเบียน ISR handler
    if (gpio_isr_handler_add(PUSH_BUTTON_PIN, button_isr_handler, NULL) != ESP_OK)
    {
        printf("Failed to add ISR handler\n");
        return;
    }

    // สร้างและรัน task สำหรับจัดการ interrupt
    if (xTaskCreate(interrupt_task, "interrupt_task", 4096, NULL, 10, &ISR) != pdPASS)
    {
        printf("Failed to create interrupt task\n");
        return;
    }
}
