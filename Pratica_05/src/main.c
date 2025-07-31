#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

#define BUTTON_PIN 14
#define LED_PIN    15

#define MODO_SEMAFORO_BINARIO

#define MAX_COUNT 10 

QueueHandle_t ledQueue;
SemaphoreHandle_t semaforo;

void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN && (events & GPIO_IRQ_EDGE_FALL)) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(semaforo, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void setup_gpio() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
}

void taskBotao(void *pvParameters) {
    static bool estado = false;
    while (1) {
        if (xSemaphoreTake(semaforo, portMAX_DELAY)) {
            estado = !estado;
            xQueueSend(ledQueue, &estado, portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }
}

void taskLed(void *pvParameters) {
    bool estado;
    while (1) {
        if (xQueueReceive(ledQueue, &estado, portMAX_DELAY)) {
            gpio_put(LED_PIN, estado);
            printf("LED: %s\n", estado ? "LIGADO" : "DESLIGADO");
            vTaskDelay(pdMS_TO_TICKS(500)); 
        }
    }
}

int main() {
    stdio_init_all();
    setup_gpio();

#ifdef MODO_SEMAFORO_BINARIO
    semaforo = xSemaphoreCreateBinary();
#else
    semaforo = xSemaphoreCreateCounting(MAX_COUNT, 0);
#endif

    ledQueue = xQueueCreate(5, sizeof(bool));

    xTaskCreate(taskBotao, "TaskBotao", 256, NULL, 2, NULL);
    xTaskCreate(taskLed, "TaskLed", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}
