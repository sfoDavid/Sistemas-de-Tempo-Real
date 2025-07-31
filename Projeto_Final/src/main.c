#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "FreeRTOS.h"
#include "task.h"


// ----- CONFIGURAÇÕES -----
#define I2C_PORT i2c0
#define SDA_PIN 4
#define SCL_PIN 5
#define MPU_ADDR 0x68

#define LED_GPIO 6
#define INT_GPIO 10
#define SERVO_GPIO 16

// Ângulos do servo
#define SERVO_FECHADO 130     // Posição inicial (compartimento fechado)
#define SERVO_ABERTO 0    // Posição para liberar paraquedas

// Configurações MPU6050
#define REG_PWR_MGMT_1        0x6B
#define REG_INT_ENABLE        0x38
#define REG_INT_PIN_CFG       0x37
#define REG_MOT_THR           0x1F
#define REG_MOT_DUR           0x20
#define REG_MOT_DETECT_CTRL   0x69
#define REG_ACCEL_XOUT_H      0x3B

#define ACCEL_SCALE_FACTOR 16384.0f
#define DEBOUNCE_MS 50       // Tempo mínimo entre detecções (ultra-rápido)
#define PWM_CLK_DIV 64.f
#define PWM_WRAP 39062       // Para 50Hz (125MHz/(64*50)-1)

// ----- VARIÁVEIS GLOBAIS -----
volatile bool int_triggered = false;
volatile bool queda_detectada = false;
bool sistema_armado = true;  // Sistema começa armado

// ----- INTERRUPÇÃO GPIO -----
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == INT_GPIO && (events & GPIO_IRQ_EDGE_RISE)) {
        int_triggered = true;
    }
}

// ----- CONTROLE DO SERVO -----
void set_servo_angle(uint gpio, float angle) {
    // Limita o ângulo entre 0-180°
    angle = angle < 0 ? 0 : (angle > 180 ? 180 : angle);
    
    // Converte ângulo para pulso PWM (500-2500μs)
    float pulse_width_us = (angle / 180.0f) * 2000 + 500;
    uint16_t pulse = (uint16_t)((pulse_width_us / 20000.0f) * PWM_WRAP);
    pwm_set_gpio_level(gpio, pulse);
}

void config_pwm_servo(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(gpio);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, PWM_CLK_DIV);
    pwm_config_set_wrap(&config, PWM_WRAP);
    pwm_init(slice, &config, true);
    set_servo_angle(gpio, SERVO_FECHADO);  // Inicia fechado
}

// ----- CONFIGURAÇÃO MPU6050 -----
void mpu6050_configurar_para_movimento() {
    uint8_t config[2];

    // Reset e configuração básica
    config[0] = REG_PWR_MGMT_1;
    config[1] = 0x00;  // Sai do sleep mode
    i2c_write_blocking(I2C_PORT, MPU_ADDR, config, 2, false);

    // Configura detecção de movimento
    config[0] = REG_MOT_THR;
    config[1] = 2;     // Threshold mais sensível
    i2c_write_blocking(I2C_PORT, MPU_ADDR, config, 2, false);

    config[0] = REG_MOT_DUR;
    config[1] = 1;     // Duração mínima
    i2c_write_blocking(I2C_PORT, MPU_ADDR, config, 2, false);

    config[0] = REG_INT_ENABLE;
    config[1] = 0x40;  // Habilita interrupção por movimento
    i2c_write_blocking(I2C_PORT, MPU_ADDR, config, 2, false);
}

// ----- TASK: DETECÇÃO DE QUEDA -----
void vTaskDetectaQueda(void *params) {
    uint8_t buffer[6];
    uint8_t reg = REG_ACCEL_XOUT_H;
    int16_t ax_raw, ay_raw, az_raw;
    float ax, ay, az, g_total;

    absolute_time_t ultimo_evento = get_absolute_time();

    for (;;) {
        if (int_triggered && sistema_armado) {
            int_triggered = false;

            // Lê os valores do acelerômetro
            i2c_write_blocking(I2C_PORT, MPU_ADDR, &reg, 1, true);
            i2c_read_blocking(I2C_PORT, MPU_ADDR, buffer, 6, false);

            ax_raw = (buffer[0] << 8) | buffer[1];
            ay_raw = (buffer[2] << 8) | buffer[3];
            az_raw = (buffer[4] << 8) | buffer[5];

            ax = ax_raw / ACCEL_SCALE_FACTOR;
            ay = ay_raw / ACCEL_SCALE_FACTOR;
            az = az_raw / ACCEL_SCALE_FACTOR;

            g_total = sqrtf(ax*ax + ay*ay + az*az);

            // Verifica condições de queda
            absolute_time_t agora = get_absolute_time();
            int tempo_ms = absolute_time_diff_us(ultimo_evento, agora) / 1000;

            if (g_total < 0.6f && az < 0.3f && tempo_ms > DEBOUNCE_MS) {
                printf("[QUEDA] Detectada! g=%.2f, az=%.2f\n", g_total, az);
                queda_detectada = true;
                ultimo_evento = agora;
                
                // Feedback visual
                for (int i = 0; i < 3; i++) {
                    gpio_put(LED_GPIO, 1);
                    vTaskDelay(pdMS_TO_TICKS(100));
                    gpio_put(LED_GPIO, 0);
                    vTaskDelay(pdMS_TO_TICKS(100));
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(1));  // Verificação ultra-rápida
    }
}

// ----- TASK: LIBERAÇÃO DO PARAQUEDAS -----
void vTaskLiberarParaquedas(void *params) {
    bool paraquedas_liberado = false;

    for (;;) {
        if (queda_detectada && !paraquedas_liberado && sistema_armado) {
            printf("[PARAQUEDAS] Liberando...\n");
            
            // Movimento rápido do servo
            set_servo_angle(SERVO_GPIO, SERVO_ABERTO);
            vTaskDelay(pdMS_TO_TICKS(300));  // Tempo para ação completa
            
            // Feedback de liberação
            for (int i = 0; i < 5; i++) {
                gpio_put(LED_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(50));
                gpio_put(LED_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(50));
            }
            
            paraquedas_liberado = true;
            queda_detectada = false;
            sistema_armado = false;  // Desarma após liberação
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// ----- MAIN -----
int main() {
    stdio_init_all();
    sleep_ms(2000);  // Tempo para inicialização

    printf("[SISTEMA] Iniciando controle de paraquedas\n");

    // Configurações iniciais
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);
    gpio_put(LED_GPIO, 0);

    // I2C para MPU6050
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Interrupção e MPU
    gpio_init(INT_GPIO);
    gpio_set_dir(INT_GPIO, GPIO_IN);
    gpio_pull_up(INT_GPIO);
    gpio_set_irq_enabled_with_callback(INT_GPIO, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    
    mpu6050_configurar_para_movimento();
    config_pwm_servo(SERVO_GPIO);

    // Cria tasks
    xTaskCreate(vTaskDetectaQueda, "DetectaQueda", 1024, NULL, 2, NULL);
    xTaskCreate(vTaskLiberarParaquedas, "LiberaParaquedas", 512, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1);
    return 0;
}