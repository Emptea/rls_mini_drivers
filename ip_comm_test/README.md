### IP communication test
Запуск:
```bash
cd build
sudo ./ip_comm_test <test point> <channel> <file_to_read.txt> <file_to_dump.hex>
```

Пример для запуска с 1 тестовой точки и 0 канала файла test.txt из папки build:
```bash
sudo ./ip_comm_test 1 0 test.txt dump_ch0.hex
```

При перезагрузке bitstream через Vivado перед запуском необходимо запустить из папки build файл `reload_driver.sh` через `sudo`:
```bash
cd build
sudo ./reload_driver.sh
```
Также можно это проделать из VSCode через задачу `Reload driver` (сочетание клавиш для вывода списка задач: `Ctrl+Shift+B`).

При модификации кода необходимо в папке `build` вызвать команду `make`
```bash
cd build
make
```

Для завершения программы нажмите в консоли сочетание клавиш `Ctrl+C`. По завершению программы в файл формата `.hex` с заданным именем будут выгружены по 232 отсчетов из буферов DMA приемного канала (всего `RX_BUFFER_COUNT` буферов). В файле `dump_all.hex` будут выгружены полностью структуры `channel_buffer`.

Структуру `channel_buffer` и количество буферов `RX_BUFFER_COUNT` можно посмотреть в файле `./src/dma-proxy.h`.
