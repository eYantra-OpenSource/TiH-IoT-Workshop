@echo off

set python_path=%1
set esp_idf_path=%2
set com_port=%3
set proj_name=%4

set cmd=%python_path%\python_env\idf4.2_py3.8_env\Scripts\python.exe %esp_idf_path%\esp-idf\components\esptool_py\esptool\esptool.py -p COM%com_port% -b 460800 --before default_reset --after hard_reset --chip esp32 write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x110000 build\%proj_name%.bin

%cmd%
