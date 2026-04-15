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

Para compilar o projeto, use o `Makefile` da raiz:

```bash
make
```

### Execução

Após compilar, rode o executável gerado:

```bash
./InazumaElevenGL
```

### Limpar build

```bash
make clean
```

### Observação

- O binário `./RonaldinhoSoccer` é legado e não faz parte do fluxo atual.
- O projeto usa somente `make` como fluxo de build.

## 🛠️ Tecnologias Utilizadas

- **C++17**
- **OpenGL 3.3 (Compatibilidade)**
- **GLFW 3**
- **GLAD** (Carregador de extensões OpenGL)
- **stb_image** (carregamento de PNG com alpha)

## ⚽ Funcionalidades Atuais

- Campo ampliado para melhor jogabilidade.
- Jogadores com tamanhos ajustados para maior fluidez.
- IA ofensiva: Aliados avançam e buscam espaços vazios para receber passes.
- Sistema de colisão e roubo de bola preciso.
- Placar funcional e estádio renderizado.
- Torcida com cheerleaders em PNG nas arquibancadas.
- Animação de celebração da torcida quando sai gol.

## 📁 Assets necessários

O jogo espera estes arquivos em `assets/`:

- `assets/removedbg.png`
- `assets/removedbg2.png`
- `assets/removedbg3.png`
