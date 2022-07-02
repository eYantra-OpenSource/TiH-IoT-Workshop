# Tools required

Below is the list of required tools for this workshop and step-by-step process of how to use them.



---

### Laptop Specifications

- OS: 64-bit Windows 10 or 11
- Processor: i3 or later
- HDD space: 10 GB or more
- RAM: 4 GB or more



---

> **NOTE**
>
> - Username or System name **must not** have space. For e.g., IIT Bombay is **not** allowed and IIT_Bombay is allowed.
>
>   - If the System name contains a space, then
>
>     - Kindly create a new login account on the system for this workshop.
>
>       ***OR***
>
>     - Install all the software in the drive apart from **C**, maybe **D** or **E**.
>
> - System must have at least **one** drive of *min. 100 GB* should be available on system apart from **C** drive.



Following software are required to be installed:

#### 1. **Arduino IDE v1.8.19**

- Download the `arduino-1.8.19-windows.exe` from this <a href="https://downloads.arduino.cc/arduino-1.8.19-windows.exe" target="_blank">link</a>. Install the software.
- If you already have Arduino IDE installed on the system, make sure its updated to `v1.8.19`.



#### 2. Visual Studio Code

- Download `VSCodeSetup-x64-1.68.1.exe` from this <a href="https://code.visualstudio.com/sha/download?build=stable&os=win32-x64" target="_blank">link</a>. Install the software.
- If you already have VS Code installed on the system, make sure its updated to `v1.68.1`.



#### 3. Extensions in VS Code

- We will be installing two extensions in VS Code.

  #### A. Arduino extension

  - Launch VS Code and press `Ctrl + P`, paste the following command and press enter.

    >  `ext install vscode-arduino`

  - You will see below window, click on *Install*.

    ![](https://raw.githubusercontent.com/kalindkaria/typora-md-assets/master/tih_workshop_july_22/assets/1_vscode_arduino_ext.png)

    <br>

  - After installation, click on the wheel icon and select *Extension Settings* as shown below.

    ![](https://raw.githubusercontent.com/kalindkaria/typora-md-assets/master/tih_workshop_july_22/assets/2_vscode_arduino_ext_settings.png)

    <br>

  - Settings window shall open. Scroll to the field of `Arduino: Path` and provide the full path where you installed the Arduino IDE.

  - If you installed Arduino IDE in the default location, the path should be: `C:\Program Files (x86)\Arduino`. Verify the path in your system and paste it in the given text box.

  - With this, the installed Arduino IDE is linked with the VS Code. You can edit, compile, upload code to target platform from VS Code.

  <br>

  #### B. Espressif extension

  - Follow this <a href='https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/tutorial/install.md' target="_blank">link</a>, it mentions step-by-step process to install and configure the extension successfully.
    - You can skip the steps 5 and 6.
    - In step 10
      - Select the ESP-IDF version as `v4.2.3 (release version)` from the options.
      - Let the directory path for ESP-IDF container and ESP-IDF tools be as default, **do not** change it.
      - Ensure that the path does not have a space, as mentioned in Note at the top. If it does have, then change the directory paths to some location in **D** or **E** drive.
    - Now, sit back and relax while the installation is proceeding. Based on the network speed, it might take around 30 to 40 mins for installation to complete.
    - Skip the steps 12, 13 and 15. You should see the message as in step 14 if the installation was successful.



Great! We are now ready to use these tools for learning something new :smile:

---

