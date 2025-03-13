#include <Wire.h>
// Dirección del esclavo (defínela según tu configuración en el PIC16F15323)
#define SLAVE_ADDR 8
// Definir el número de sensores
#define NUM_SENSORS 16
// Variables globales para los valores de los sensores
int sensorValues[NUM_SENSORS];
// Pesos asignados a cada sensor (ajústalos según la importancia o ubicación de cada sensor)
int sensorWeights[NUM_SENSORS] = {10, 9, 8, 7, 6, 5, 5, 6, 7, 8, 9, 10, 9, 8, 7, 6};
// Variables para manejar el temporizador
unsigned long previousMillis = 0;
const long interval = 1000;  // Intervalo de 1 segundo (1000 milisegundos)
// Función para obtener valores de los sensores (simulando con valores aleatorios)
void getSensorValues(int sensorValues[]) {
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = random(0, 1025); // Valor aleatorio entre 0 y 1024
  }
}
// Función para imprimir los valores de los sensores en el monitor serie
void printSensorValues(int sensorValues[]) {
  Serial.print("Sensor #  ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(i + 1); // Imprimir el número del sensor (1 a 16)
    Serial.print("    ");
  }
  Serial.println();
  
  Serial.print("Valor:    ");
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensorValues[i]);
    Serial.print("    ");
  }
  Serial.println("\n");
}
// Función para calcular el flujo ponderado de los sensores
int calculateWeightedFlow(int sensorValues[], int sensorWeights[]) {
  long sum = 0;  // Sumar los productos de los valores de los sensores y sus pesos
  long weightSum = 0;  // Sumar los pesos
  for (int i = 0; i < NUM_SENSORS; i++) {
    sum += sensorValues[i] * sensorWeights[i];  // Multiplicar el valor del sensor por su peso
    weightSum += sensorWeights[i];  // Sumar el peso
  }
  return sum / weightSum;  // Promedio ponderado
}
// Función para enviar los valores de los sensores al esclavo
void sendSensorData(int sensorValues[]) {
  Wire.beginTransmission(SLAVE_ADDR);  // Inicia la transmisión al esclavo
  for (int i = 0; i < NUM_SENSORS; i++) {
    Wire.write(sensorValues[i] >> 8);  // Enviar el byte alto del valor
    Wire.write(sensorValues[i] & 0xFF);  // Enviar el byte bajo del valor
  }
  byte error = Wire.endTransmission();  // Termina la transmisión
  if (error == 0) {
    Serial.println("Datos enviados correctamente.");
  } else {
    Serial.print("Error al enviar datos: ");
    Serial.println(error);
  }
}
// Función de configuración
void setup() {
  // Iniciar la comunicación I2C
  Wire.begin();  
  // Configurar el puerto serie para monitorear la comunicación
  Serial.begin(9600);
  
  // Esperar que el serial se inicie
  while (!Serial);
  Serial.println("Iniciando comunicación I2C...");
  Serial.println("Valores de los sensores:");
}
// Función principal (loop)
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Guardar el tiempo actual
    // Obtener los nuevos valores de los sensores
    getSensorValues(sensorValues);
    // Imprimir los valores de los sensores en forma de tabla
    printSensorValues(sensorValues);
    // Calcular el valor ponderado total del flujo
    int totalFlow = calculateWeightedFlow(sensorValues, sensorWeights);  
    Serial.print("Valor total ponderado del flujo: ");
    Serial.println(totalFlow);
    // Controlador proporcional (P) para la bomba
    int targetFlow = 512;  // El flujo deseado (ajústalo según tu aplicación)
    int error = targetFlow - totalFlow;  // Diferencia entre el flujo deseado y el flujo real
    int Kp = 2;  // Constante proporcional (ajusta según lo necesario)
    int controlSignal = Kp * error;  // Señal de control para la bomba
    // Imprimir la señal de control (puedes usarla para controlar la bomba)
    Serial.print("Señal de control para la bomba: ");
    Serial.println(controlSignal);
    // Enviar los valores al esclavo (si es necesario)
    sendSensorData(sensorValues);
  }
}
