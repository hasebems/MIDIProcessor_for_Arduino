//
//  midiprocessor_class.h
//
//  2018.10.08 by kigakudoh
//

const static int MAX_SERIAL_NUMBER = 3;
const static int MAX_DATA_SIZE = 128;

// for debug
extern void blinkLedWhenMidiCome( void );

inline void error( void )
{
  while(1);
}


class MidiProcessor {

private:
    struct RcvData {
        unsigned char   buf[MAX_DATA_SIZE];
        int             pointer;
    } rcvData[MAX_SERIAL_NUMBER];

    struct SndData {
        unsigned char   buf[MAX_DATA_SIZE*MAX_SERIAL_NUMBER];
        int             readPointer;
        int             writePointer;
    } sndData[MAX_SERIAL_NUMBER];

public:
    MidiProcessor() : rcvData(), sndData() {}
    ~MidiProcessor(){}

    void init( void ){}

    void writeData( RcvData& rd )
    {
        for ( int j=0; j<MAX_SERIAL_NUMBER; j++ ){
            SndData& sd = sndData[j];
            for ( int i=0; i<rd.pointer; i++ ){
                sd.buf[sd.writePointer] = rd.buf[i];
                sd.writePointer += 1;
                if ( sd.writePointer >= MAX_DATA_SIZE*MAX_SERIAL_NUMBER ){
                    sd.writePointer = 0;
                }
            }
        }
    }

    void input( unsigned char dt, int serialNumber )
    {
        if ( serialNumber >= MAX_SERIAL_NUMBER ){ return;}

        RcvData& rd = rcvData[serialNumber];

        //  realtime message
        if ( dt >= 0xf8 ){ return; }

        //  Status Byte
        if ( dt & 0x80 ){
            if ( rd.pointer == 0 ) {
                rd.buf[0] = dt;
                rd.pointer = 1;
            }
            else {
                if ( dt == 0xf7 ){
                    rd.buf[rd.pointer++] = dt;
                    writeData(rd);
                    rd.pointer = 0;
                }
                else { error(); }
            }
        }

        //  Data Byte
        else {
            if ( rd.pointer == 0 ){
                //  running status
                rd.pointer = 1;
            }
            switch ( rd.buf[0] & 0xf0 ){
                case 0xc0: case 0xd0:{
                    rd.buf[rd.pointer] = dt;
                    writeData(rd);
                    rd.pointer = 0;
                    break;
                }
                case 0x80: case 0x90: case 0xa0: case 0xb0: case 0xe0:{
                    rd.buf[rd.pointer++] = dt;
                    if ( rd.pointer == 3 ){
                        writeData(rd);
                        rd.pointer = 0;
                    }
                    break;
                }
                case 0xf0:{
                    rd.buf[rd.pointer++] = dt;
                    if ( rd.pointer >= MAX_DATA_SIZE ){ error(); }
                    break;
                }
                default: break;
            }
        }
    }

    bool hasOutputData( int serialNumber )
    {
        bool retValue = false;

        if ( serialNumber < MAX_SERIAL_NUMBER ){
            SndData& sd = sndData[serialNumber];
            if ( sd.readPointer != sd.writePointer ){ retValue = true;}
        }
        return retValue;
    }

    unsigned char output( int serialNumber )
    {
        unsigned char outputData = 0;
        if ( serialNumber < MAX_SERIAL_NUMBER ){
            SndData& sd = sndData[serialNumber];
            outputData = sd.buf[sd.readPointer];
            sd.readPointer += 1;
            if ( sd.readPointer >= MAX_DATA_SIZE*MAX_SERIAL_NUMBER ){
                sd.readPointer = 0;
            }
        }
        return outputData;
    }
};
