#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Pinos dos LEDs
#define LED1 14
#define LED2 15
void initGPIO(){
    gpio_init(LED1);
    gpio_set_dir(LED1, GPIO_OUT);
    gpio_init(LED2);
    gpio_set_dir(LED2, GPIO_OUT);
}
TaskHandle_t T1;
TaskHandle_t T2;
TaskHandle_t CPUUsageTask;
TaskHandle_t moderateCPUTask;
TaskHandle_t VariableLoadTask;

// Contador de ticks em IDLE
static volatile uint32_t ulIdleTickCount = 0;


#define TICK_PERIOD_MS 20000

void delay(){
    unsigned int i;
    for(i = 0; i < TICK_PERIOD_MS; i++)
    asm("nop");
}

void vApplicationIdleHook(void)
{	
	unsigned long int tick = xTaskGetTickCount();
	while (xTaskGetTickCount() == tick);
	ulIdleTickCount++;

}


void taskCPUUsage(void *pvParameters){
    const TickType_t xPeriod = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t ulLastIdleCount = ulIdleTickCount;

    while(1)
    {
        vTaskDelayUntil(&xLastWakeTime, xPeriod);

        uint32_t ulIdleTicks = ulIdleTickCount - ulLastIdleCount;
        ulLastIdleCount = ulIdleTickCount;

        const float fIdleFraction = (float)ulIdleTicks / (float)configTICK_RATE_HZ;
        const float fCPUUsage    = (1.0f - fIdleFraction) * 100.0f;

        printf("CPU Usage: %.2f%% (idle ticks: %u)\n", fCPUUsage, (unsigned)ulIdleTicks);
    }
}
void task1(void *pvParameters) {
    uint led = (uint)(uintptr_t) pvParameters;

    while (1) {
        gpio_put(led, 1);
        //delay();
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_put(led, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
        //delay();
        //printf("Executando task 1\n");
    }
}

void task2(void *pvParameters) {
    uint led = (uint)(uintptr_t) pvParameters;
    while (1) {
        gpio_put(led, 1);
        //delay();
        vTaskDelay(pdMS_TO_TICKS(150));
        gpio_put(led, 0);
        //delay();
        vTaskDelay(pdMS_TO_TICKS(150));
        //printf("Executando task 2\n");
    }
}

void taskModerateCPU(void *pvParameters) {
    for (;;) {
        uint32_t start = to_ms_since_boot(get_absolute_time());
        while (to_ms_since_boot(get_absolute_time()) - start < 200) {
            volatile float x = 1.0;
            for (int i = 0; i < 1000; i++) {
                x = x * 1.0001f + 0.0001f;
            }
        }
        //printf("Task moderada\n");
        vTaskDelay(pdMS_TO_TICKS(800));
    }
}



void taskVariableLoad(void *pvParameters) {
    const TickType_t totalCycle = pdMS_TO_TICKS(1000);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint loadDurationMs = 100;

    for (;;) {
        TickType_t start = xTaskGetTickCount();
        while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(loadDurationMs)) {
            volatile double x = 0;
            for (volatile int i = 0; i < 1000; i++) {
                x += 3.1415 * 2.7182;
                x *= 1.00001;
            }
        }
        TickType_t now = xTaskGetTickCount();
        TickType_t elapsed = now - xLastWakeTime;
        if (elapsed < totalCycle) {
            vTaskDelayUntil(&xLastWakeTime, totalCycle);
        } else {
            xLastWakeTime = now;
        }

        loadDurationMs += 100;
        if (loadDurationMs > 800) {
            //vTaskDelete(moderateCPUTask);
            vTaskDelete(NULL);
        }

        printf("Carga atual: %u ms\n", loadDurationMs);
    }
}



int main() {
    stdio_init_all();
    printf("Iniciando sistema com FreeRTOS\n");
    initGPIO();
    // Criação das tasks
    xTaskCreate(task1, "Task1", configMINIMAL_STACK_SIZE, (void *)LED1, 1, &T1);
    xTaskCreate(task2, "Task2", configMINIMAL_STACK_SIZE, (void *)LED2, 2, &T2);
    xTaskCreate(taskModerateCPU, "ModerateCPU", configMINIMAL_STACK_SIZE, NULL, 2, &moderateCPUTask);
    //xTaskCreate(taskHeavyCPU, "HeavyCPU", configMINIMAL_STACK_SIZE * 2, NULL, 1, NULL);
    xTaskCreate(taskVariableLoad, "VariableLoad", configMINIMAL_STACK_SIZE, NULL, 2, &VariableLoadTask);
    xTaskCreate(taskCPUUsage, "CPUUsage", 512, NULL, 3, &CPUUsageTask);

    vTaskStartScheduler();

    while (true);
}
