# RadioHead-Issues


I'm using the sample client and server programs for RF95 Reliable Datagram and the header information returned by 

`manager.recvfromAck(buf, &len, &from, &to, &id, &flags) `

is corrupted after the first few messages. buf and len arrive ok but from, to and the rest show up as (usually) zero.
However, if I use the 

`getFrom = manager.headerFrom()`

and similar functions from the Datagram class right after this call, I get the expected results. I discovered this when I tried to send the response to my clients with

`manager.sendtoWait(msg, strlen(msg), from );`

and got errors. Debugging revealed that the from address was getting misplaced. Once I updated the call as follows:

`manager.sendtoWait(msg, strlen(msg), manager.headerFrom() );`

it worked as expected.


| Resources | Versions  |
| -- | -- |
| Hardware: | Adafruit Feather M0 RF95 (SAMD21)|
| IDE: | I tried both 1.8 and 2.3 and got the same result |
| RadioHead:| 1.122.1 |



### Files related to RadioHead library issues

* RF95-ReliableDatagram-Server.ino - server program demonstrating the problem
* Debug.txt - output from server
* M0_RFM95_Satellite_PIR.ino - motion sensor/doorbell remotes
