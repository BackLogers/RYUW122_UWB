## Description

This library is designed specifically for the **RYUW122** UWB module from **REYAX Technology**, providing access to all available features.  
It focuses on **ease of use**, **performance optimization** (especially for fast distance measurements), **minimal memory footprint**, and **no external library dependencies**.

---

## Features

- Full support for **bidirectional communication** between modules  
- **Distance measurement** in Anchor ↔ Tag configuration  
- **Reading and modifying** module parameters  

---

## Module Information

- RYUW122 is an easy-to-use UWB module controlled via AT commands over UART.
- In addition to distance measurement, the module supports bidirectional data transfer, but with certain limitations:
  - The maximum message size is 12 bytes.
  - The tag cannot send messages to the anchor — it can only respond when polled by an anchor.
  - The tag does not know which anchor sent the message — it only receives the content and length.
- It is strongly recommended to use the maximum supported baud rate (115200). Using lower values can more than double the time required for distance measurement.
- Note that any change in baud rate is stored in the module’s flash memory. Power cycling does **not** restore default settings.
- When changing parameters stored in flash (e.g., address or mode), the module may become temporarily unresponsive. A small delay (handled by the library) is necessary.
- The maximum distance measurement frequency is approximately 16 Hz.
- For accurate distance readings, messages should have similar lengths (difference of no more than 3 bytes). The library provides automatic padding to the maximum length.
- In theory, an unlimited number of anchors and tags can be used, but the user must handle synchronization of distance measurements. A tag can only respond to one anchor at a time.
- The anchor cannot send empty messages, but the tag is allowed to reply with empty messages.
- To ensure fast and stable distance measurements, messages should be kept short — ideally up to 4 bytes.
- The module operates at 3.3V logic level. When using 5V logic boards, a level shifter is required to prevent damage.
- During rapid data transmission from anchor to tag, the UART interface in the tag can become completely blocked. This is a serious issue, as it prevents any changes or reads of configuration parameters once the tag is powered on.  
  Fortunately, there is a workaround:
  - Perform a hardware reset of the module.
  - Immediately send a command to switch the device to **anchor mode**.
  - Modify or read parameters as needed.
  - Finally, switch the module back to **tag mode**.

- Keep in mind that changing modes writes to the module’s **FLASH memory**, which has a limited lifespan (~100,000 writes according to the documentation).  
  Therefore, avoid performing such operations too frequently.  
  If frequent parameter updates are needed (e.g., dynamic tag reply messages), it's better to restart the module and change only the required values — the library supports this approach.

---

## To-do

- [ ] Asynchronous message transmission  
- [ ] Improved error handling  
- [ ] Complete library documentation  
