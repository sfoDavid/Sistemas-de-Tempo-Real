#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <stdio.h>


#define LED1 14
#define LED2 15
#define LED3 13



void task1(void *pvParameters) {
    uint led = (uint) pvParameters;
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    while (1) {
        gpio_put(led, 1);
        vTaskDelay(pdMS_TO_TICKS(300));;
        gpio_put(led, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
        printf("Executando task 1\n");
    }
}

void task2(void *pvParameters) {
    uint led = (uint) pvParameters;
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    while (1) {
        gpio_put(led, 1);
        vTaskDelay(pdMS_TO_TICKS(150));
        gpio_put(led, 0);
        vTaskDelay(pdMS_TO_TICKS(150));
        printf("Executando task 2\n");
    }
}

void task3(void *pvParameters) {
    uint led = (uint) pvParameters;
    gpio_init(led);
    gpio_set_dir(led, GPIO_OUT);

    for (int i = 0; i < 3; i++) {
        gpio_put(led, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_put(led, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("Executando task 3\n");
    }
    vTaskDelete(NULL);
}

int main() {
    stdio_init_all(); 
    printf("Criando tasks!!!\n");

    xTaskCreate(task1, "Task1",
                configMINIMAL_STACK_SIZE,
                (void *)(uintptr_t)LED1, 1, NULL);

    xTaskCreate(task2, "Task2",
                configMINIMAL_STACK_SIZE,
                (void *)(uintptr_t)LED2, 1, NULL);

    xTaskCreate(task3, "Task3",
                configMINIMAL_STACK_SIZE,
                (void *)(uintptr_t)LED3, 1, NULL);
    vTaskStartScheduler();

    while (true);
}