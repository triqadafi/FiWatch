

# Table of contents
* **[How to install](#how-to-install)**
	- [Boards Manager Installation](#boards-manager-installation)
	- [Manual Installation](#manual-installation)
	- [PlatformIO](#platformio)
* **[Getting started with TRQDFCore_AVR](#getting-started-with-trqdfcore_avr)**

## How to install
#### Boards Manager Installation
This installation method requires Arduino IDE version 1.6.4 or greater.
* Open the Arduino IDE.
* Open the **File > Preferences** menu item.
* Enter the following URL in **Additional Boards Manager URLs**:

    ```
    https://triqadafi.github.io/trqdfcore/package_trqdfcore_index.json
    ```

* Open the **Tools > Board > Boards Manager...** menu item.
* Wait for the platform indexes to finish downloading.
* Scroll down until you see the **TRQDFCore_AVR** entry and click on it.
* Click **Install**.
* After installation is complete close the **Boards Manager** window.

#### Manual Installation
Click on the "Download ZIP" button in the upper right corner. Exctract the ZIP file, and move the extracted folder to the location "**~/Documents/Arduino/hardware**". Create the "hardware" folder if it doesn't exist.
Open Arduino IDE, and a new category in the boards menu called "**TRQDFCore_AVR**" will show up.

#### PlatformIO
[PlatformIO](http://platformio.org) is an original environtment when developing this core. Feel free to contact me, if you eager to try :)


## Getting started with TRQDFCore_AVR
Ok, so you're downloaded and installed **TRQDFCore_AVR**, but how to get started? Here's a quick guide:
* The easiest way is to connect it with FTDI programmer as shown in the [pinout diagram](#pinout). (for this version only support FTDI programmer and USBasp)
* Open the **Tools > Board** menu item, and select a **TRQDFCore_AVR** compatible board. In this version select **fiWATCH 4.0+**.
* Try blank sketch, or select an example if you wish **File > Examples > Examples from fiWATCH 4.0+**.
There are some userful examples:
    - Clock (how to use real time clock)
    - Display (how to operate RGB LED)
    - EEPROM (permanent memory just like small hard drive)
    - Input (how to use button)
    - Power (optimized deep sleep comes from this **TRQDFCore**)
    - Serial (very useful, just serial print like usual)
* Hit **Upload** button. Kindly wait :)
Your code should now be running on your microcontroller! If you experience any issues related to bootloader burning or serial uploading, please feel free to contact me or create an issue on Github.


## Pinout
This core uses the standard Arduino UNO pinout and will not break compatibility of any existing code or libraries.
![Current Device](https://raw.githubusercontent.com/triqadafi/fiWATCH/master/img/pinout.jpg)
