# `esp_bin_upload.bat`

Here is the way you have to use the above file.



- Four parameters must be passed along with this file while executing it.
  - `python_path`
    - Full path of `.espressif` directory that consists of `python_env` directory.
    - Example: `C:\Users\<username>\.espressif`
  - `esp_idf_path`
    - Full path of `esp` directory that consists of `esp-idf` directory.
    - Example: `C:\Users\Kalind-ERTS\esp`
  - `com_port`
    - COM port number where the eYFi-Mega board is connected.
  - `proj_name`
    - C file name inside the `main` directory.



- Example usage on command prompt inside project directory:

  `.\esp_bin_upload.bat C:\Users\Kalind-ERTS\.espressif C:\Users\Kalind-ERTS\esp 4 main`