from lcd2usb import LCD
import zmq


def main():
    button_polarity = 0
    
    # invoke LCD instance
    lcd = LCD()
    
    major, minor = lcd.version
    # print('Firmware version %d.%d' % (major, minor))
    if minor == 9:
        button_polarity = 1
 
    lcd.set_contrast(190)
    lcd.set_brightness(255)
 
    # create communication pipe
    context = zmq.Context()
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:55550")
    
    working = True

    while working:
        #  Wait for next request from client
        input = socket.recv().decode("utf-8")
        output = "ok"
        
        s = input.split(",")
        
        event = int(s[0])
        if event == 0:        # clear
            lcd.clear()
            
        elif event == 1:        # text output
            row = int(s[1])
            col = int(s[2])
            text = s[3]
            lcd.goto(col,row)
            lcd.write(text)
            
        elif event == 2:
            brightness = int(s[1])
            contrast = int(s[2])
            lcd.set_brightness(brightness)
            lcd.set_contrast(contrast)
            
        elif event == 3:
            if button_polarity == 0:
                key1, key2 = lcd.keys
            else:
                key2, key1 = lcd.keys
            output = "{}{}".format(int(key1), int(key2))
    
        elif event == 9:     # quit task
            print("EXITING")
            working = False
            
        # send response
        socket.send(output.encode('utf-8'))

if __name__ == '__main__':
    main()
