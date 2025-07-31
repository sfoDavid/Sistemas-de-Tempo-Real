#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <stdio.h>

#define BUTTON_PIN    14
#define ADC_PIN       26

#define LED_R_PIN     15
#define LED_G_PIN     16
#define LED_B_PIN     17
#define LED_EXTRA_PIN 19

#define LED_BTN_PIN   18

#define X1 1500
#define X2 2730

QueueHandle_t ledQueue;
TaskHandle_t taskLedBtnHandle;

void init_gpio() {
    gpio_init(LED_R_PIN); 
    gpio_set_dir(LED_R_PIN, GPIO_OUT); 
    gpio_put(LED_R_PIN, 0);
    gpio_init(LED_G_PIN); 
    gpio_set_dir(LED_G_PIN, GPIO_OUT); 
    gpio_put(LED_G_PIN, 0);
    gpio_init(LED_B_PIN); 
    gpio_set_dir(LED_B_PIN, GPIO_OUT); 
    gpio_put(LED_B_PIN, 0);
    gpio_init(LED_BTN_PIN);
    gpio_set_dir(LED_BTN_PIN, GPIO_OUT); 
    gpio_put(LED_BTN_PIN, 0);

    gpio_init(LED_EXTRA_PIN);
    gpio_set_dir(LED_EXTRA_PIN, GPIO_OUT);
    gpio_put(LED_EXTRA_PIN, 0);

    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);
}

void apagar_leds() {
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
}

void taskLedVermelho(void *pvParameters) {
    uint8_t cor;
    while (1) {
        if (xQueueReceive(ledQueue, &cor, portMAX_DELAY) && cor == 0) {
            apagar_leds();
            gpio_put(LED_R_PIN, 1);
        }
    }
}

void taskLedVerde(void *pvParameters) {
    uint8_t cor;
    while (1) {
        if (xQueueReceive(ledQueue, &cor, portMAX_DELAY) && cor == 1) {
            apagar_leds();
            gpio_put(LED_G_PIN, 1);
        }
    }
}

void taskLedAzul(void *pvParameters) {
    uint8_t cor;
    while (1) {
        if (xQueueReceive(ledQueue, &cor, portMAX_DELAY) && cor == 2) {
            apagar_leds();
            gpio_put(LED_B_PIN, 1);
        }
    }
}
void taskLedExtra(void *pvParameters) {
    uint8_t cor;
    static bool estado = false;

    while (1) {
        if (xQueueReceive(ledQueue, &cor, portMAX_DELAY) && cor == 4) {
            estado = !estado;
            gpio_put(LED_EXTRA_PIN, estado);
            printf("LED EXTRA: %s\n", estado ? "LIGADO" : "DESLIGADO");
        }
    }
}


void taskADC(void *pvParameters) {
    uint16_t adc_val;
    float voltage;
    uint8_t cor;

    while (1) {
        adc_val = adc_read();
        voltage = (adc_val * 3.3f) / 4095.0f;

        if (adc_val < X1) {
            cor = 0; // vermelho
            printf("ADC: %.2f V → VERMELHO\n", voltage);
        } else if (adc_val > X2) {
            cor = 2; // azul
            printf("ADC: %.2f V → AZUL\n", voltage);
        } else {
            cor = 1; // verde
            printf("ADC: %.2f V → VERDE\n", voltage);
        }

        xQueueSend(ledQueue, &cor, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void taskBotao(void *pvParameters) {
    uint8_t cor;
    static bool led_state = false;

    while (1) {
        if (xQueueReceive(ledQueue, &cor, portMAX_DELAY) && cor == 4) {
            led_state = !led_state;
            gpio_put(LED_BTN_PIN, led_state);
            printf("Botão pressionado: LED do BOTÃO %s\n", led_state ? "LIGADO" : "DESLIGADO");
        }
    }
}


void gpio_callback(uint gpio, uint32_t events) {
    uint8_t cor = 4;

    if (gpio == BUTTON_PIN) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(ledQueue, &cor, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


void setup_button_interrupt() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

int main() {
    stdio_init_all();
    init_gpio();
    setup_button_interrupt();

    ledQueue = xQueueCreate(5, sizeof(uint8_t));

    xTaskCreate(taskLedVermelho, "LedRed", 256, NULL, 1, NULL);
    xTaskCreate(taskLedVerde, "LedGreen", 256, NULL, 1, NULL);
    xTaskCreate(taskLedAzul, "LedBlue", 256, NULL, 1, NULL);
    xTaskCreate(taskADC, "TaskADC", 256, NULL, 2, NULL);
    xTaskCreate(taskLedExtra, "LedExtra", 256, NULL, 1, NULL);
    xTaskCreate(taskBotao, "Botao", 256, NULL, 3, &taskLedBtnHandle);

    vTaskStartScheduler();

    while (1);
}
