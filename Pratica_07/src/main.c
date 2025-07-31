#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>

#define LED_WARNING 16
#define TAMANHO_BLOCO 2048
#define MAX_BLOCOS 55

void *blocos[MAX_BLOCOS];

void setup_led() {
    gpio_init(LED_WARNING);
    gpio_set_dir(LED_WARNING, GPIO_OUT);
    gpio_put(LED_WARNING, 0);
}

void taskMonitorHeap(void *pvParameters) {
    const size_t heap_total = configTOTAL_HEAP_SIZE;

    while (1) {
        size_t heap_livre = xPortGetFreeHeapSize();
        float porcentagem = 100.0f * ((float)heap_livre / heap_total);

        printf("[Monitor] Heap livre: %u bytes (%.1f%%)\n", (unsigned int)heap_livre, porcentagem);

        gpio_put(LED_WARNING, heap_livre < heap_total / 10 ? 1 : 0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskAlocadora(void *pvParameters) {
    const size_t heap_total = configTOTAL_HEAP_SIZE;

    while (1) {
        printf("[Alocadora] Iniciando alocação...\n");

        int i;
        for (i = 0; i < MAX_BLOCOS; i++) {
            blocos[i] = pvPortMalloc(TAMANHO_BLOCO);
            if (!blocos[i]) {
                printf("[Alocadora] Falha ao alocar bloco %d\n", i);
                break;
            }

            size_t livre = xPortGetFreeHeapSize();
            printf("[Alocadora] Alocado bloco %d, heap livre: %u bytes\n", i, (unsigned int)livre);

            if (livre < heap_total / 10) {
                printf("[Alocadora] Heap <10%%, parando alocação\n");
                break;
            }

            vTaskDelay(pdMS_TO_TICKS(200));
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); // mantém alocado

        printf("[Alocadora] Liberando blocos...\n");
        for (int j = 0; j < MAX_BLOCOS; j++) {
            if (blocos[j]) {
                vPortFree(blocos[j]);
                blocos[j] = NULL;
            }
        }

        printf("[Alocadora] Liberação concluída. Recomeçando em 3s...\n");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

int main() {
    stdio_init_all();
    setup_led();

    xTaskCreate(taskMonitorHeap, "MonitorHeap", 256, NULL, 1, NULL);
    xTaskCreate(taskAlocadora, "Alocadora", 512, NULL, 2, NULL);

    vTaskStartScheduler();
    while (1);
}
