# Mini-Shell-ESIS

Proyecto: intérprete de comandos (mini-shell) en C++ para Linux — Trabajo práctico Unidad 01

Este repositorio contiene una mini-shell escrita en C++. Este README describe el estado actual del proyecto, cómo compilarlo, ejecutarlo y las pruebas disponibles.

## Estado del proyecto

- El intérprete arranca con un prompt personalizado: `Mini-Shell-ESIS:`.
- Soporta ejecución de comandos externos mediante `fork()` + `exec*()` y espera en foreground con `waitpid()`.
- Resolución de comandos mediante la variable de entorno `PATH` (con fallback a `/bin` y `/usr/bin`).
- Redirecciones: `>`, `>>` y `<` funcionan.
- Pipes múltiples: `cmd1 | cmd2 | cmd3` son soportados.
- Background jobs con `&`: el shell no bloquea y recolecta hijos con `waitpid(..., WNOHANG)` mostrando notificaciones.
- Built-ins implementados: `cd`, `pwd`, `help`, `history`, `echo`, `meminfo`, `salir`/`exit`.
- Manejo de señales: el shell captura `SIGINT` para no terminar accidentalmente y maneja `SIGCHLD` para recolección de hijos.
- Historial persistente en archivo (configurable en `include/config.h`).

## Estructura del repositorio

- `src/` - Código fuente C++.
- `include/` - Headers y configuración (`config.h`).
- `obj/` - Objetos compilados (generado por Makefile).
- `tests/` - Scripts de prueba básicos (`test_basico.sh`, `test_pipes.sh`).
- `Makefile` - Reglas de compilación.
- `mini-shell` - Ejecutable producido por `make`.

## Requisitos

- Linux (probado en Ubuntu). Se recomienda tener `g++` y herramientas estándar de desarrollo.

## Cómo compilar

Abre una terminal en la raíz del proyecto y ejecuta:

```bash
make -j4
```

## Cómo ejecutar

Ejecuta el intérprete con:

```bash
./mini-shell
```

## Ejecutar los tests incluidos

Los scripts en `tests/` automatizan pruebas básicas y de pipes.

```bash
chmod +x tests/*.sh
./tests/test_basico.sh
./tests/test_pipes.sh
```

Los scripts ejecutan el binario `./mini-shell` y pasan comandos por stdin;