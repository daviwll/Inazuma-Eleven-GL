# Inazuma Eleven GL

Um simulador de futebol arcade inspirado no clássico Inazuma Eleven, desenvolvido em C++ utilizando OpenGL e GLFW.

## 🎮 Controles

- **WASD:** Movimenta o jogador portador da bola (ou o mais próximo dela).
- **ESPAÇO (Segurar):** Carrega a força do chute e ativa o **Hissatsu** (efeito especial ao redor da bola).
- **ESPAÇO (Soltar):** Chuta a bola na direção do mouse com força proporcional ao tempo segurado.
- **MOUSE:** Mira a direção do chute.

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

## 🎯 Funcionalidades - Requisitos do Projeto

### a) Campo de Futebol
- Campo completo com linhas, círculo central, áreas de goleiro e penalty
- Retas e circunferências pintadas com OpenGL

### b) Bola Movimentável
- Bola controlada pelo jogador mais próximo
- Movimento suave com física de atrito

### c) Placar
- Placar atualizado automaticamente quando a bola cruza a linha do gol
- Bola resetada para o centro após cada gol
- Torcida celebra quando sai gol

### d) Jogadores e Torcida
- 22 jogadores (11 de cada time) com sprites
- Torcida renderizada nas arquibancadas com cheerleaders
- Sistema de IA para controle dos jogadores adversários

### e) Mecânica de Movimento
- Jogadores perseguem a bola automaticamente
- IA ofensiva com passes e avançadas
- Sistema de roubo de bola por colisão

### f) Áudio
- Som de chute
- Som de fundo (música ambiente)
- Som de apito do juiz no início de cada tempo

### g) Hissatsu - Chute Especial
- **Sistema de carregamento visual** ao redor da bola quando segura ESPAÇO
- **Anéis giratórios** de energia ao redor do jogador
- **Cores dinâmicas:**
  - Azul/Ciano: Carregando
  - Laranja: Power moderado (50%+)
  - Dourado: Power máximo (80%+)
- Partículas orbitando com velocidade diferente
- Linhas de velocidade radiando quando power > 50%
- **Super Shot:** Chutes potentes perto do gol ativam efeitos de fogo/gelo aleatórios

### h) Realismo e Qualidade
- **Física com Spin:** Efeito Magnus causa curvas na bola
- **Motion Blur:** Rastro visual quando a bola está em alta velocidade
- **Partículas:**
  - Poeira ao chutar
  - Efeitos de fogo/gelo em chutes especiais
  - Celebração de gol
- **Sombras:** Jogadores têm sombras elípticas no chão
- **Animações:** Sprites de corrida para jogadores em movimento
- **Sprite Rotation:** Bola rotaciona baseada na velocidade
- **Limites de Campo:** Jogadores e bola presos dentro das linhas

## 🎨 Visual Effects

- **Fire/Ice Trail:** Partículas laranja/vermelho ou ciano/azul em chutes especiais
- **Dust Particles:** Poeira marrom ao chutar
- **Goal Celebration:** Explosão de partículas douradas na trave
- **Hissatsu Aura:** 3 anéis concêntricos girando com velocidades diferentes
- **Player Shadows:** Elipses escuras abaixo dos jogadores

## 📁 Estrutura de Assets

```
assets/
├── players_blue/
│   ├── face_blue.png, back_blue.png, left_blue.png, right_blue.png
│   ├── gk_blue_left.png
│   └── running/ (sprites de corrida)
├── players_red/
│   ├── face_Red.png, back_red.png, left_red.png, right_red.png
│   ├── gk_red_right_1.png
│   └── running/ (sprites de corrida)
├── ball/
│   ├── ball1.png, ball2.png, ball4.png
│   └── ball_super1.png
├── sound/
│   ├── background-sound.mp3
│   ├── kick.mp3
│   └── referee-start.mp3
└── (outros assets do campo e torcida)
```

## 🛠️ Tecnologias Utilizadas

- **C++17**
- **OpenGL 3.3 (Compatibilidade)**
- **GLFW 3**
- **GLAD** (Carregador de extensões OpenGL)
- **stb_image** (carregamento de PNG com alpha)
- **miniaudio** (reprodução de áudio)
- **Particle System** customizado
- **Physics Engine** com Magnus Effect

## 📋 Requisitos Implementados

| Requisito | Descrição | Status |
|-----------|-----------|--------|
| a | Campo de Futebol | ✅ |
| b | Bola Movimentável | ✅ |
| c | Placar | ✅ |
| d | Jogadores e Torcida | ✅ |
| e | Mecânica de Movimento | ✅ |
| f | Áudio | ✅ |
| g | Hissatsu (Chute Especial) | ✅ |
| h | Realismo e Qualidade | ✅ |


