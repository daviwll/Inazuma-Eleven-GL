# Inazuma Eleven GL

Um simulador de futebol arcade inspirado no clássico Inazuma Eleven, desenvolvido em C++ utilizando OpenGL e GLFW.

## 🎮 Controles

- **WASD:** Movimenta o jogador portador da bola (ou o mais próximo dela).
- **ESPAÇO (Segurar):** Carrega a força do chute.
- **ESPAÇO (Soltar):** Chuta a bola na direção em que o jogador está olhando.

## 🚀 Como Rodar

### Pré-requisitos (Linux)

Certifique-se de ter as bibliotecas do GLFW e OpenGL instaladas:

```bash
sudo apt update
sudo apt install build-essential libglfw3-dev libgl1-mesa-dev
```

### Compilação

Para compilar o projeto, basta usar o `Makefile` incluído:

```bash
make
```

### Execução

Após compilar, rode o executável gerado:

```bash
./InazumaElevenGL
```

## 🛠️ Tecnologias Utilizadas

- **C++17**
- **OpenGL 3.3 (Compatibilidade)**
- **GLFW 3**
- **GLAD** (Carregador de extensões OpenGL)

## ⚽ Funcionalidades Atuais

- Campo ampliado para melhor jogabilidade.
- Jogadores com tamanhos ajustados para maior fluidez.
- IA ofensiva: Aliados avançam e buscam espaços vazios para receber passes.
- Sistema de colisão e roubo de bola preciso.
- Placar funcional e estádio renderizado.
