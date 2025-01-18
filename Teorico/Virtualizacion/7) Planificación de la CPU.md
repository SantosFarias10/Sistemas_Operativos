# Planificación: Introducción

## Suposiciones Sobre la Carga de Trabajo

Primero plantearemos algunas suposiciones simplificadoras sobre los procesos que se ejecutan en el sistema, a veces denominados colectivamente como la **Carga de Trabajo**. La carga de trabajo es una parte fundamental de la creación de politicas.

Las suposiciones de la carga de trabajo que plantearemos son poco realistas, a medida que avancemos las iremos olvidando.

Haremos las siguientes suposiciones sobre los procesos, a veces llamados **Trabajos**:

1) Cada trabajo se ejecuta durante la misma cantidad de tiempo.

2) Todos los trabajos llegan al mismo tiempo.

3) Una vez iniciado, cada trabajo se ejecuta hasta su finalización.

4) Todos los trabajos usan solo la CPU (o sea, no realizan I/O).

5) Se conode el tiempo de ejecución de cada trabajo.

---

## Métricas de Planificación

Ademas de las suposiciones sobre la carga de trabajo, también nos hace falta algo que nos permita comparar diferentes poíticas de planifiación: una **Métrica de Planificación**. Una métrica es algo que usamos para medir algo.

Por ahora vamos a tener simplemtente una única métrica: el **Tiempo de Entrega**. El tiempo de entrega de un trabajo se define como el momento en que se completa el trabajo menos el momento en que el trabajo llegó al sistema.

$$T_{entrega} = T_{finalización} - T_{llegada}$$

