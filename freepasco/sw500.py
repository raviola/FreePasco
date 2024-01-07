import serial
from time import sleep

# SW500 interface commands (1 byte opcode)
# See file: doc/SW500_serial_protocol.pdf for protocol details
CMD_ID = b'\x01'
CMD_DWNLD_REC = b'\x02'
CMD_EXEC = b'\x03'
CMD_RESET = b'\x04' 


class SW500():
    def __init__(self,
                port='/dev/ttyUSB1',    # TODO: hardcoded, put in config file
                baudrate=19200,         # Comm parameters fixed by protocol     
                databits=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=0.1             # Do not change! Prevents missing 
                                        # characters in some operations
                ):

        self.error = False
        self.error_msg = ""

        # Configure serial connection
        self._device = serial.Serial()
        self._device.port = port
        self._device.baudrate = baudrate
        self._device.databits = databits
        self._device.parity = parity
        self._device.stopbits = stopbits
        self._device.timeout = timeout
        self._device.exclusive = False
        
        # Open serial connection
        try:
            self._device.open()
            
        except serial.SerialException:
            self.error = True
            self.error_msg = "Can't open serial port " + port
            print(self.error_msg)
        
        else:
            # Connection opened, flush the toilet
            self._device.flush()
            self._device.reset_input_buffer()
            self._device.reset_output_buffer()
            
            # Identify device
            self._device.write(CMD_ID)
            id_str = self._device.read(12)
            self.id_str = id_str
            sleep(0.1)  # Just for safety
            mode_str = self._device.read(3)

            # Check ROM or RAM mode
            if mode_str == b'RAM':
                self.mode = "RAM"
            else:
                self.mode = "ROM"

            print ("Device detected: " 
                    + id_str.decode('utf-8')
                    + " in "
                    + self.mode +
                    " mode.")
            
            # If ROM mode, load firmware
            if self.mode == "ROM":
                print ("Loading firmware...")
                
                # TODO: hardcoded firmware filename, put in config file
                with open('freepasco/firmware/sw500ram.s28', 'r') as file:
                    for line in file:
                        if line[0:2] == 'S2':
                            packet = bytes.fromhex(line[2:-1])
                            checksum = bytes.fromhex(line[-3:-1]) 
                            # Issue one download command for each S2 register
                            self._device.write(CMD_DWNLD_REC)
                            self._device.write(packet)
                            # Device returns a checksum for control
                            checksum_ctrl = self._device.read(1)
                            
                            # TODO: retry n times if checksum fails
                            if checksum != checksum_ctrl:
                                self._device.close()
                                self.error = True
                                self.error_msg = "Wrong checksum: "
                                "bad connection or firmware file corrupted."
                                raise serial.SerialException(self.error_msg)

                # Execute loaded firmware
                self._device.write(CMD_EXEC)
                # Check acknowledge string
                ack_str = self._device.read(20)
                if ack_str == b'RAM code is running.':
                    self.mode = "RAM"
                    print("Device is now running in RAM mode"
                    " and ready to accept sampling setup and"
                    " control commands.")
                else:
                    self.error = True
                    self.error_msg = "Acknowledge error."
                    self._device.write(CMD_RESET)
                    self._device.close()
                    raise serial.SerialException(self.error_msg)
                    
sw500 = SW500()

