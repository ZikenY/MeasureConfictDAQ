void setup()
{
    // initialize digital pin 13 as an output.
    pinMode(13, OUTPUT);
    
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
}

void prepareframe(unsigned char* buff)
{
    memset(buff, 0, 18);
    buff[0] = 0xff; buff[1] = 0x02;
    buff[16] = 0xff; buff[17] = 0x03;
}

// the loop routine runs over and over again forever:
void loop()
{
    static bool grabing = false;
    static unsigned char buff[18];
    static int sampleinterval = 300;

    if (Serial.available() > 0)
    {    
        //incoming command
        int readbytes = Serial.readBytes(buff, 18);
        
        if (readbytes != 18)
        {
            Serial.flush();
            digitalWrite(13, LOW);delay(300);
            digitalWrite(13, HIGH);delay(300);
            digitalWrite(13, LOW);delay(300);
            digitalWrite(13, HIGH);
            return;
        }
        
        bool ok = true;
        if ((buff[0] == 0xff) && (buff[1] == 0x02))
        {
            switch (buff[2])
            {
            case 0x10:            {
                //bind hands
                unsigned char ccc = buff[3];
                sampleinterval = ccc * 100;  
                prepareframe(buff);
                buff[2] = 0x11;
                Serial.write(buff, 18);
                digitalWrite(13, HIGH);
                break;
            };

            case 0x12:            {
                //unbind hands
                prepareframe(buff);
                buff[2] = 0x13;
                Serial.write(buff, 18);
                digitalWrite(13, LOW);
                break;
            };

            case 0x20:            {
                //start collect
                grabing = true;
                prepareframe(buff);
                buff[2] = 0x24;
                Serial.write(buff, 18);
                delay(200);
                break;
            }
            
            case 0x21:            {
                //stop collect
                grabing = false;
                prepareframe(buff);
                buff[2] = 0x25;
                Serial.write(buff, 18);
                break;
            }            
            }
        }

        //clear the buffer
        Serial.flush();

    }
    
    if (grabing)
    {
        prepareframe(buff);
        buff[2] = 0x12;    //transfer data

        unsigned short sensorValue[6];
        // read the input on analog pin 0-5:
        sensorValue[0] = analogRead(A0);
        sensorValue[1] = analogRead(A1);
        sensorValue[2] = analogRead(A2);
        sensorValue[3] = analogRead(A3);
        sensorValue[4] = analogRead(A4);
        sensorValue[5] = analogRead(A5);
    
        for (long i=0; i< 6; i++)
        {
            //divided the short into high and low bytes
            unsigned short maskhigh = 0x00ff;  //conserve low bits
            buff[3+i*2] = maskhigh & sensorValue[i];  //low byte
            buff[4+i*2] = sensorValue[i] >> 8;        //high byte           
        }
        
        //write xor varifying part
        buff[15] = buff[3];
        for (long i=3; i<15; i++)
        {
            buff[15] ^= buff[i];
        }

    //      voltages = sensorValue[0] * (5.0 / 1023.0);

        Serial.write(buff, 18);

        static bool ledflag = false;
        digitalWrite(13, ledflag ? HIGH:LOW);   // turn the LED on/off (HIGH is the voltage level)
        ledflag = !ledflag;
        
        delay(sampleinterval);
    }
}
