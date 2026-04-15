# 1. Versão mínima do CMake e Nome do Projeto
cmake_minimum_required(VERSION 3.10)
project(InazumaElevenGL)

# 2. Especificar o padrão do C++ (C++17 é ótimo para jogos)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Onde o CMake deve procurar os arquivos .h (Headers)
# Incluímos a pasta 'include' do seu projeto
include_directories(include)

# 4. Encontrar as bibliotecas necessárias no Ubuntu
# O CMake vai buscar o GLFW3 e o OpenGL que você instalou via apt
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# 5. Listar os arquivos de código fonte (.cpp e .c)
# Importante: inclua o glad.c aqui para ele ser compilado junto!
set(SOURCES 
    src/main.cpp 
    src/glad.c
    src/player.cpp
    src/field.cpp
    src/stadium.cpp
)

# 6. Criar o executável
add_executable(${PROJECT_NAME} ${SOURCES})

# 7. "Linkar" as bibliotecas (fazer as conexões finais)
# Aqui dizemos quais poderes o seu programa terá:
target_link_libraries(${PROJECT_NAME} 
    glfw 
    OpenGL::GL
    ${CMAKE_DL_LIBS} # Necessário no Linux para carregar funções em tempo de execução
)

# 8. (Opcional) Copiar os assets para a pasta de build
# Isso garante que seus sons e imagens estejam perto do executável
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})