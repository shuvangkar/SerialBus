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

### How Slave decode the master packet
 1. Slave will first check total availabel bytes. 
 2. If data length is 5 bytes, wait until buffer fill up time. After timeout check buffer again. If 5 bytes, slave consider this as query. If more than 5 bytes slave consider the packet as Instruction packet.
 3. If packet length is more than 5 bytes, consider the packet as instructions.
