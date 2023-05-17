# fusee-nano
A minimalist re-implementation of the Fusée Gelée exploit, designed to run on embedded Linux devices. (Zero dependencies)

### Usage example:
```
sudo ./fusee-nano fusee.bin 
[*] device id: 008403040000002xxxxxxxxxxxxxxx62
[*] Read 92 bytes from intermezzo.bin
[*] Read 38168 bytes from fusee.bin
[+] Sent 0x1b000 bytes
```
Note: This currently uses a relatively large amount of memory (~200k) to store
the entire payload. I plan to improve this by generating the payload on-the-fly
as it is sent.
