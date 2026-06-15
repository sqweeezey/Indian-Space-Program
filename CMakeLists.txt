cmake_minimum_required(VERSION 3.10)
project(IndianSpaceProgram)

# Настройки стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Указываем точный путь к cmake-файлам внутри SFML
set(SFML_DIR "C:/Users/yakun/OneDrive/SFML/SFML++/SFML2.0/SFML-2.6.1-windows-vc17-64-bit/SFML-2.6.1/lib/cmake/SFML")
set(SFML_INCLUDE_DIR "C:/Users/yakun/OneDrive/SFML/SFML++/SFML2.0/SFML-2.6.1-windows-vc17-64-bit/SFML-2.6.1/include")
set(SFML_LIB_DIR "C:/Users/yakun/OneDrive/SFML/SFML++/SFML2.0/SFML-2.6.1-windows-vc17-64-bit/SFML-2.6.1/lib")
# 2. Находим пакет (эта команда САМА заполнит переменную SFML_INCLUDE_DIR)
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED PATHS "${SFML_DIR}" NO_DEFAULT_PATH)

# 3. Объявляем исполняемый файл и исходники
add_executable(Game src/main.cpp src/Game.cpp)

# 4. Подключаем заголовочные файлы вашего проекта и SFML
target_include_directories(Game PRIVATE include "${SFML_INCLUDE_DIR}")

# 5. Сливаем библиотеки с вашим проектом
target_link_libraries(Game PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# 6. Автоматическое копирование DLL в папку с .exe (только для Windows)
if(WIN32)
    add_custom_command(TARGET Game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:Game>
        $<TARGET_FILE_DIR:Game>
        COMMAND_EXPAND_LISTS
    )
endif()

# --- АВТОКОПИРОВАНИЕ БЕЗ ИЗМЕНЕНИЯ ПУТЕЙ В КОДЕ ---
add_custom_command(TARGET Game POST_BUILD
    # Высыпаем содержимое папки textures прямо к .exe
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_SOURCE_DIR}/assets/textures" 
    "$<TARGET_FILE_DIR:Game>"
    
    # Высыпаем шрифты
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_SOURCE_DIR}/assets/fonts" 
    "$<TARGET_FILE_DIR:Game>"
    
    # Высыпаем звуки (если папка уже есть)
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_SOURCE_DIR}/assets/sounds" 
    "$<TARGET_FILE_DIR:Game>"
)