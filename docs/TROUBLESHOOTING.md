# Troubleshooting VOS9-DOS

## Руководство по устранению неполадок

Этот документ поможет вам решить наиболее распространенные проблемы при работе с VOS9-DOS.

## Проблемы с загрузкой

### Система не загружается

#### Симптомы
- Черный экран после включения
- Сообщение "No bootable device"
- Компьютер зависает на стадии загрузки

#### Возможные причины и решения

**1. Неправильный образ диска**
```bash
# Проверьте размер образа
ls -la vos9.img
# Должен быть 1474560 байт (1.44MB)

# Пересоберите образ
make clean
make all
```

**2. Проблемы с эмулятором**
```bash
# QEMU - используйте правильные параметры
qemu-system-i386 -fda vos9.img -boot a

# VirtualBox - убедитесь что:
# - Тип ОС: Other/DOS
# - Загрузка с Floppy включена
# - Floppy контроллер подключен
```

**3. Проблемы BIOS/UEFI**
- Убедитесь что Legacy Boot включен
- Отключите Secure Boot в UEFI
- Проверьте порядок загрузки

### Зависание на "Loading kernel..."

#### Причины
- Поврежденный kernel.bin
- Неправильная линковка
- Проблемы с памятью

#### Решения
```bash
# Проверьте размер ядра
ls -la build/kernel.bin

# Пересоберите ядро
make clean
cd src/kernel
make

# Проверьте карту памяти в linker script
cat kernel.ld
```

## Проблемы с командной строкой

### "Bad command or file name"

#### Симптомы
```
C:\>help
Bad command or file name

C:\>dir
Bad command or file name
```

#### Причина
Команды введены в неправильном регистре.

#### Решение
VOS9-DOS требует команды в ВЕРХНЕМ регистре:
```
C:\>HELP     ✓ Правильно
C:\>help     ✗ Неправильно

C:\>DIR      ✓ Правильно  
C:\>dir      ✗ Неправильно
```

### Команды не отвечают

#### Симптомы
- Ввод команды не дает результата
- Курсор не перемещается
- Система "зависла"

#### Диагностика и решения

**1. Проверьте клавиатуру**
```
# Попробуйте простые команды
CLS
VER
```

**2. Перезагрузка системы**
- Ctrl+Alt+Del в эмуляторе
- Перезапуск виртуальной машины

**3. Проблемы с драйвером клавиатуры**
```c
// В файле src/drivers/keyboard.c
// Проверьте инициализацию
void keyboard_init(void) {
    // Регистрация обработчика прерываний
    set_irq_handler(1, keyboard_handler);
}
```

## Проблемы с отображением

### Искаженный текст на экране

#### Симптомы
- Неправильные символы
- Смещенный текст
- Мерцание экрана

#### Решения

**1. Проблемы VGA драйвера**
```c
// Проверьте src/drivers/vga.c
// Убедитесь в правильной инициализации
void vga_init(void) {
    vga_buffer = (uint16_t*)0xB8000;
    vga_clear();
}
```

**2. Настройки эмулятора**
```bash
# QEMU - добавьте параметры дисплея
qemu-system-i386 -fda vos9.img -boot a -display sdl

# VirtualBox - проверьте:
# - Видеопамять: минимум 16MB
# - Ускорение 3D: отключено
```

### Неправильные цвета

#### Причина
Неверная настройка цветовой палитры VGA.

#### Решение
```c
// В src/drivers/vga.c
#define VGA_COLOR_BLACK         0
#define VGA_COLOR_BLUE          1
#define VGA_COLOR_GREEN         2
#define VGA_COLOR_CYAN          3
#define VGA_COLOR_RED           4
#define VGA_COLOR_MAGENTA       5
#define VGA_COLOR_BROWN         6
#define VGA_COLOR_LIGHT_GREY    7
#define VGA_COLOR_DARK_GREY     8
#define VGA_COLOR_LIGHT_BLUE    9
#define VGA_COLOR_LIGHT_GREEN   10
#define VGA_COLOR_LIGHT_CYAN    11
#define VGA_COLOR_LIGHT_RED     12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN   14
#define VGA_COLOR_WHITE         15
```

## Проблемы сборки

### Ошибки компиляции

#### "gcc: command not found"
```bash
# Ubuntu/Debian
sudo apt-get install gcc-multilib

# CentOS/RHEL
sudo yum install gcc

# Проверьте установку
gcc --version
```

#### "nasm: command not found"
```bash
# Ubuntu/Debian
sudo apt-get install nasm

# CentOS/RHEL  
sudo yum install nasm

# Проверьте установку
nasm --version
```

#### Ошибки линковки
```
ld: cannot find -lgcc
```

**Решение:**
```bash
# Установите необходимые библиотеки
sudo apt-get install gcc-multilib libc6-dev-i386

# Или используйте кросс-компилятор
export CC=i686-elf-gcc
export LD=i686-elf-ld
```

### Ошибки Make

#### "make: *** No targets specified"
```bash
# Убедитесь что находитесь в правильной директории
pwd
ls Makefile

# Запустите сборку
make all
```

#### Права доступа
```bash
# Если ошибки прав доступа
chmod +x tools/build.sh
sudo chown -R $USER:$USER .
```

## Проблемы эмуляции

### QEMU не запускается

#### Проверьте установку QEMU
```bash
# Установка QEMU
sudo apt-get install qemu-system-x86

# Проверка версии
qemu-system-i386 --version
```

#### Правильные параметры запуска
```bash
# Базовый запуск
qemu-system-i386 -fda vos9.img -boot a

# С дополнительными опциями
qemu-system-i386 \
    -fda vos9.img \
    -boot a \
    -m 32M \
    -display sdl \
    -rtc base=localtime
```

### VirtualBox проблемы

#### Настройка виртуальной машины
1. **Тип операционной системы**: Other → DOS
2. **Память**: 32-64 MB (достаточно)
3. **Жесткий диск**: Не нужен
4. **Дискета**: Подключить vos9.img

#### Настройки загрузки
1. **Порядок загрузки**: Floppy первым
2. **Enable I/O APIC**: Отключить
3. **Enable PAE/NX**: Отключить

## Проблемы производительности

### Медленная работа

#### В эмуляторе
```bash
# QEMU - включите KVM (если доступно)
qemu-system-i386 -enable-kvm -fda vos9.img -boot a

# Увеличьте память
qemu-system-i386 -m 64M -fda vos9.img -boot a
```

#### На реальном железе
- Убедитесь что компьютер поддерживает загрузку с дискеты
- Проверьте состояние дискеты и дисковода
- Попробуйте загрузку с USB (используя ISO образ)

## Отладка проблем

### Добавление отладочной информации

#### В kernel.c
```c
void debug_print(const char* message) {
    vga_puts("[DEBUG] ");
    vga_puts(message);
    vga_puts("\n");
}

// Использование
debug_print("Kernel started");
debug_print("VGA initialized");
debug_print("Keyboard initialized");
```

#### В shell.c
```c
void shell_debug_command(int argc, char* argv[]) {
    vga_puts("Shell debug info:\n");
    vga_puts("Commands available: ");
    
    for(int i = 0; i < num_commands; i++) {
        vga_puts(commands[i].name);
        vga_puts(" ");
    }
    vga_puts("\n");
}
```

### Журналирование

#### Простое логирование в память
```c
#define LOG_SIZE 1024
char debug_log[LOG_SIZE];
int log_pos = 0;

void log_message(const char* msg) {
    // Простая реализация логирования
    int len = strlen(msg);
    if(log_pos + len < LOG_SIZE) {
        strcpy(&debug_log[log_pos], msg);
        log_pos += len;
    }
}
```

## Контрольный список диагностики

### Перед обращением за помощью проверьте:

- [ ] Правильность сборки (`make clean && make all`)
- [ ] Размер образа vos9.img (должен быть 1474560 байт)
- [ ] Команды вводятся в ВЕРХНЕМ регистре
- [ ] Настройки эмулятора соответствуют требованиям
- [ ] Установлены все необходимые инструменты сборки
- [ ] Права доступа к файлам проекта
- [ ] Достаточно места на диске для сборки

### Сбор информации для отчета об ошибке:

1. **Версия VOS9-DOS**
2. **Операционная система хоста**
3. **Версии инструментов** (gcc, nasm, make)
4. **Используемый эмулятор** и его версия
5. **Точные симптомы** проблемы
6. **Шаги для воспроизведения**
7. **Сообщения об ошибках** (полный текст)

## Получение помощи

### Документация
- **РУКОВОДСТВО_ПОЛЬЗОВАТЕЛЯ.md** - базовое использование
- **РУКОВОДСТВО_РАЗРАБОТЧИКА.md** - техническая информация
- **FAQ.md** - часто задаваемые вопросы

### Самодиагностика
1. Внимательно изучите сообщения об ошибках
2. Попробуйте минимальный набор действий
3. Проверьте, работает ли проблема воспроизводимо
4. Используйте отладочные возможности

---

*При возникновении проблем, не описанных в этом руководстве, обратитесь к другим файлам документации или изучите исходный код системы.*
