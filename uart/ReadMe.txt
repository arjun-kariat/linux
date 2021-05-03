This is a simple application used to get uart data and sends back a response accordingly
The expected uart read data format is
"be"+<1 byte of length information>+<length bytes of data>+"ad"

On receiving the uart data in the above mentioned format the device sends a response back in the following format
<length bytes of data>+"\n\r"


