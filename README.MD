# SerialBus Library
The purpose of the library is to build a common communication bus for multiple microcontrollers. The bus can be RS485 or RS232 bus.
- This is a master slave communication
- master generates request and slave just responses

## Master Packet Format

### Instruction

| Packet Length|Slave ID|Function Code|Control Byte|Payload|Checksum|
|:------------:|:-----------:|:-----:|:----------:|:-----:|:-------|
| 1 Byte       | 1 Byte      | 1 Byte| 1 Byte     |n byte |1 byte  |
### Query
| Slave Address|Function Code|Control|Payload|Checksum|
|:------------:|:-----------:|:-----:|:-----:|:------- |
| 1 Byte       | 1 Byte      | 1 Byte|n byte |1 byte  |

- Master will send any type of instructions or data  to slave via instructions packet format 
- Master requests data from slave via query packet format.

### Function Code Description
|Function Code|Description|
|:-----------:|:---------:|
|0~9          |Reserved   |
|10           |Master Sends multiple bytes to slave|
|11           |Master Request Slave for Multiple Bytes|


