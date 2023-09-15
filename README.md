
# Trabalho I de Sistemas Operacionais I

## Setup

```sh
mkdir build && cmake -B build
```

## Compile and Run

```sh
make -C build
./build/escalonador <option>
```

As opções para o escalonador são
- f: First Come First Served
- s: Shortest Job First
- p: Fila de prioridade com preempção
- n: Fila de prioridade sem preempção
- r: Round-robin