#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>


#define ADC_PIN     26
#define LED_R_PIN   16
#define LED_G_PIN   17
#define LED_B_PIN   18


#define LIMIAR_BAIXO  1365 
#define LIMIAR_ALTO   2730 


uint16_t valorADC = 0;
SemaphoreHandle_t mutex;


void setup_hardware() {
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);

    gpio_init(LED_R_PIN); gpio_set_dir(LED_R_PIN, GPIO_OUT); gpio_put(LED_R_PIN, 0);
    gpio_init(LED_G_PIN); gpio_set_dir(LED_G_PIN, GPIO_OUT); gpio_put(LED_G_PIN, 0);
    gpio_init(LED_B_PIN); gpio_set_dir(LED_B_PIN, GPIO_OUT); gpio_put(LED_B_PIN, 0);
}

void apagar_leds() {
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
}


void taskLeituraADC(void *pvParameters) {
    while (1) {
        uint16_t leitura = adc_read();
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            valorADC = leitura;
            xSemaphoreGive(mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void taskPrintADC(void *pvParameters) {
    while (1) {
        uint16_t leitura;
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            leitura = valorADC;
            xSemaphoreGive(mutex);
        }
        float tensao = (leitura * 3.3f) / 4095.0f;
        printf("ADC: %d | Tensão: %.2f V\n", leitura, tensao);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskLedRGB(void *pvParameters) {
    while (1) {
        uint16_t leitura;
        if (xSemaphoreTake(mutex, portMAX_DELAY)) {
            leitura = valorADC;
            xSemaphoreGive(mutex);
        }

        apagar_leds();
        if (leitura < LIMIAR_BAIXO) {
            gpio_put(LED_R_PIN, 1);
            printf("[LED] VERMELHO\n");
        } else if (leitura > LIMIAR_ALTO) {
            gpio_put(LED_B_PIN, 1);
            printf("[LED] AZUL\n");
        } else {
            gpio_put(LED_G_PIN, 1);
            printf("[LED] VERDE\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


int main() {
    stdio_init_all();
    setup_hardware();

    mutex = xSemaphoreCreateMutex();
    if (mutex == NULL) {
        printf("Erro ao criar mutex.\n");
        while (1);
    }

    xTaskCreate(taskLeituraADC, "LeituraADC", 256, NULL, 3, NULL);
    xTaskCreate(taskPrintADC, "PrintADC", 256, NULL, 2, NULL);
    xTaskCreate(taskLedRGB, "LedRGB", 256, NULL, 1, NULL);

    vTaskStartScheduler();
    while (1);
}
