/*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
*/

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/i2c_client/i2c_client_interface.h"  // Verifica la ruta si es necesario
#include <stdint.h>
#include <stdbool.h>

#define NUM_SENSORS 16

// Variables globales para almacenar los datos recibidos.
// Se esperan 16 valores de 2 bytes cada uno (32 bytes en total).
static uint16_t sensorValues[NUM_SENSORS];
static uint8_t dataBuffer[NUM_SENSORS * 2];  // 32 bytes
static uint8_t byteIndex = 0;

/*
 * Callback para manejo de eventos de transferencia I2C.
 * Se invoca cada vez que ocurre un evento (por ejemplo, cuando se recibe un byte).
 */
bool I2C1_ClientCallback(i2c_client_transfer_event_t clientEvent)
{
    // Lee el byte recibido usando la función del driver
    uint8_t receivedByte = I2C1_Client.ReadByte();
    dataBuffer[byteIndex++] = receivedByte;
    
    // Cuando se han recibido los 32 bytes (16 valores de 2 bytes)
    if (byteIndex >= (NUM_SENSORS * 2))
    {
        for (int i = 0; i < NUM_SENSORS; i++)
        {
            // Combina el byte alto y el byte bajo para formar el valor de 16 bits
            sensorValues[i] = ((uint16_t)dataBuffer[2 * i] << 8) | dataBuffer[2 * i + 1];
        }
        // Reinicia el índice para la próxima transferencia
        byteIndex = 0;
        
        // Aquí puedes procesar sensorValues (por ejemplo, enviar los datos por UART)
    }
    return true;  // Indica que se ha manejado el evento
}

/*
    Main application
*/
int main(void)
{
    SYSTEM_Initialize();
    
    // Habilitar interrupciones globales y periféricas
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    
    // Registrar la función callback para la recepción de datos I2C
    I2C1_Client.CallbackRegister(I2C1_ClientCallback);
    
    while(1)
    {
        // Ejecuta las tareas del driver I2C para gestionar eventos
        I2C1_Client.Tasks();
        
        // Aquí puedes agregar otras tareas en el bucle principal
    }    
}
