# Sistema de Control de Robot Móvil

Este repositorio contiene el código fuente y la documentación de un sistema de control de Robot Diferencial desarrollado como parte del curso de Mecatrónica en la [UCB (Universidad Católica Boliviana)](https://www.scz.ucb.edu.bo). 
El proyecto está diseñado para ser utilizado con la placa ESP32 DEVKIT V1.

## Tabla de Contenidos
- [Controlador PID](#Controlador-PID)
- [Odometria](#Odometria)
- [Cinematica Directa](#Cinematica-Directa) 
- [Cinematica Inversa](#Cinematica-Inversa)
- [PWM](#PWM)
- [Visión General](#visión-general)
- [Contribuyentes](#contribuyentes)
- [Licencia](#licencia)

## PWM en motor_control.c

El control PWM para el motor se encuentra implementado en el archivo `motor_control.c`. En este archivo, se utilizan las funciones `ledc_set_duty` y `ledc_update_duty` para establecer y actualizar el ciclo de trabajo (duty cycle) del motor. Además, se proporciona una función para calcular el ciclo de trabajo basado en un ángulo y se definen métodos para obtener y establecer el ángulo del motor.

Para más detalles, revisa el código en [`motor_control.c`](ruta/a/motor_control.c).
## Controlador PID de Motores

El controlador PID de motores es una parte fundamental de la funcionalidad de control de dos motores en un robot. Está implementado en el archivo task_utils.c, específicamente en las funciones `Motor1ControlTask` y `Motor2ControlTask`. Estas funciones manejan el control de motores y aplican un controlador PID para mantener las velocidades deseadas.

### Uso

El proceso se desarrolla de la siguiente manera:

1. **Lectura de Velocidad**: Las funciones leen las velocidades actuales de los motores a partir de una cola de mensajes.

2. **Filtrado de Velocidad**: Aplican un filtro de mediana exponencial ponderada (EWMA) a las velocidades para suavizar las lecturas.

3. **Control de Velocidad**: Dependiendo de la estrategia de control seleccionada (ya sea control en lazo abierto o control PID), las funciones calculan el valor del ciclo de trabajo (`duty_cycle`) para controlar los motores.

4. **Controlador PID**: En caso de utilizar el controlador PID, se calcula el error, se aplica el control PID y se satura la señal de control según sea necesario.

5. **Log de Valores**: Se registran los valores actuales, incluyendo la velocidad del motor, la referencia deseada, la señal de control y el error.

6. **Actualización de Motores**: Finalmente, se establece el ciclo de trabajo para los motores según la dirección y se mueven de acuerdo a las señales calculadas.

Estas funciones proporcionan una sólida base para implementar un control de motores preciso y eficiente, lo que es esencial para el movimiento controlado de un robot.



## Cinemática Directa 

La cinemática directa se encuentra implementada en el archivo task_utils.c a través de la tarea `DirectKinematicsTask`. Esta tarea desempeña un papel fundamental en el cálculo de la velocidad lineal y angular de un robot móvil en función de las velocidades de sus ruedas. Estos valores se registran o envían según sea necesario.

### Uso

Para aprovechar esta funcionalidad, es esencial proporcionar los parámetros necesarios a la tarea, incluyendo las colas de velocidad de los motores. También se debe configurar las constantes `DIÁMETRO_RUEDA` y `ANCHO_ROBOT` de acuerdo a las especificaciones particulares del robot. La tarea se ejecuta en un bucle continuo, procesando de manera periódica los datos de velocidad de los motores y actualizando el estado de velocidad del robot.

### Descripción detallada

Este código implementa la cinemática directa para un robot móvil, un cálculo que relaciona las velocidades de las ruedas del robot con su velocidad lineal y angular en el mundo real. La tarea funciona de la siguiente manera:

1. **Lectura de Velocidades**: La tarea lee las velocidades de las ruedas izquierda y derecha en unidades de revoluciones por minuto (rpm) desde las colas de mensajes `motor1SpeedQueue` y `motor2SpeedQueue`.

2. **Conversión de Unidades**: Convierte las velocidades de las ruedas de rpm a radianes por segundo (rad/s).

3. **Cálculo de Velocidades**: Calcula la velocidad lineal y la velocidad angular del robot utilizando las velocidades de las ruedas y parámetros como el diámetro de las ruedas (`DIÁMETRO_RUEDA`) y el ancho del robot (`ANCHO_ROBOT`).

4. **Actualización de Estados**: Establece los estados de velocidad y velocidad angular del robot.

5. **Registro o Envío de Resultados**: Registra o envía los resultados, que incluyen la velocidad lineal y la velocidad angular del robot, para su posterior seguimiento o control.

6. **Control de Frecuencia**: El bucle de la tarea se ejecuta con una frecuencia controlada por un retraso de 100 ms (`pdMS_TO_TICKS(100)`) para regular la tasa de ejecución de la tarea y asegurar una actualización consistente.

Este enfoque proporciona una forma eficaz de obtener la cinemática directa de un robot móvil, lo que es esencial para una amplia variedad de aplicaciones de robótica.

## Cinemática Inversa 

La cinemática inversa, una parte esencial en el control de robots móviles, está implementada en el archivo http_handlers.c, específicamente en la función `handle_set_robot_speed`. Esta función maneja las solicitudes para actualizar la velocidad del robot y calcula las velocidades de las ruedas izquierda y derecha utilizando ecuaciones de cinemática inversa.

### Uso

El proceso es el siguiente:

1. **Recepción de la Solicitud**: La función verifica la solicitud para asegurarse de que sea de tipo POST.

2. **Extracción de Datos JSON**: Luego, extrae los datos JSON de la solicitud, incluyendo la velocidad del robot (`robot_speed`) y el ángulo del robot (`robot_angle`).

3. **Cálculo de Velocidades de Ruedas**: Utilizando la cinemática inversa, la función calcula las velocidades de las ruedas izquierda y derecha en RPM a partir de la velocidad del robot y el ángulo.

4. **Limitación de Velocidades**: Aplica una saturación de velocidad para limitar las velocidades de las ruedas a un máximo de 200 RPM.

5. **Establecimiento de Referencias**: Establece las referencias para las velocidades de los motores izquierdo y derecho, según el control de estrategia seleccionado.

6. **Registro de Valores Actualizados**: Registra los valores actualizados de las velocidades de los motores.

7. **Respuesta**: Finalmente, envía una respuesta indicando que la velocidad del robot ha sido actualizada exitosamente.

Este código proporciona una sólida base para la implementación de la cinemática inversa en aplicaciones de control de robots móviles, lo que es esencial para lograr un control preciso y eficiente del movimiento.


## Visión General

El sistema de control de Robot Diferencial está diseñado para proporcionar capacidades de control y monitoreo para un robot móvil. Incluye características como conectividad Wi-Fi, un servidor web para control y monitoreo remotos, inicialización de encoders, gestión de pines GPIO, inicialización de temporizadores, manejo de interrupciones y control de motores. Esta base de código sirve como fundamento para construir y personalizar proyectos de robots móviles.

## Contribuyentes

- [Leonardo Acha Boiano]((https://github.com/leonardoAB1))
- [Bruno Ramiro Rejas]()
- [Gonzalo Peralta]()
- [Andrés Ayala]()

## Licencia

Este proyecto está bajo la [Licencia MIT](LICENSE).
