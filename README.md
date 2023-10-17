# Sistema de Control de Robot Móvil

Este repositorio contiene el código fuente y la documentación de un sistema de control de Robot Diferencial desarrollado como parte del curso de Mecatrónica en la [UCB (Universidad Católica Boliviana)](https://www.scz.ucb.edu.bo). 
El proyecto está diseñado para ser utilizado con la placa ESP32 DEVKIT V1.

## Tabla de Contenidos
- [Visión General](#visión-general)
- [Contribuyentes](#contribuyentes)
- [Licencia](#licencia)

## PWM en motor_control.c

El control PWM para el motor se encuentra implementado en el archivo `motor_control.c`. En este archivo, se utilizan las funciones `ledc_set_duty` y `ledc_update_duty` para establecer y actualizar el ciclo de trabajo (duty cycle) del motor. Además, se proporciona una función para calcular el ciclo de trabajo basado en un ángulo y se definen métodos para obtener y establecer el ángulo del motor.

Para más detalles, revisa el código en [`motor_control.c`](ruta/a/motor_control.c).

## Visión General

El sistema de control de Robot Diferencial está diseñado para proporcionar capacidades de control y monitoreo para un robot móvil. Incluye características como conectividad Wi-Fi, un servidor web para control y monitoreo remotos, inicialización de encoders, gestión de pines GPIO, inicialización de temporizadores, manejo de interrupciones y control de motores. Esta base de código sirve como fundamento para construir y personalizar proyectos de robots móviles.

## Contribuyentes

- [Leonardo Acha Boiano]((https://github.com/leonardoAB1))
- [Bruno Ramiro Rejas]()
- [Gonzalo Peralta]()
- [Andrés Ayala]()

## Licencia

Este proyecto está bajo la [Licencia MIT](LICENSE).
