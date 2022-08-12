# search-engine-gui
## 🖋️ Описание
Проект представляет собой реализацию проекта "Поисковый движок" https://github.com/nikita82333/search-engine с графическим интерфейсом. Используется Qt 5.12.2. 

## 🛠️ Используемые технологии
<div>
  <img src="https://github.com/devicons/devicon/blob/master/icons/cplusplus/cplusplus-original.svg" title="C++" alt="C++" width="40" height="40"/>&nbsp;
  <img src="https://github.com/devicons/devicon/blob/master/icons/qt/qt-original.svg" title="Qt" alt="Qt" width="40" height="40"/>&nbsp;
  <img src="https://github.com/devicons/devicon/blob/master/icons/cmake/cmake-original.svg" title="CMake" alt="CMake" width="40" height="40"/>&nbsp;
</div>
Для работы с json-файлами используется библиотека:
https://github.com/nlohmann/json


## 🔨 Сборка проекта
Проект собирается в ОС Windows. Компилятор должен поддерживать стандарт C++17, Cmake 3.11+.
```
cmake --build \search_engine_gui\cmake-build-debug --target search_engine_gui -j 3
```

## 🚀 Запуск
```
\search_engine_gui\cmake-build-debug\search_engine_gui.exe
```
