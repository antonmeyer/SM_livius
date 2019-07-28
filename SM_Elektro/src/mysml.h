#include "driver/uart.h"
#include "endian.h"

portMUX_TYPE EMHparseMutex = portMUX_INITIALIZER_UNLOCKED;
#define ECHO_TEST_TXD (GPIO_NUM_23)
#define ECHO_TEST_RXD (GPIO_NUM_22)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE) //not in use
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE) //not in use

#define BUF_SIZE 1024
uint8_t data[BUF_SIZE];
#define SMLFLAG 0x1b1b1b1b
#define SMLSTART 0x01010101

static QueueHandle_t queue_uart2;

int length = 0;
int pos = 0;
int sml_start_flag = 0;
int len = 0;

void EMH_obisParser(uint8_t *smlmsg);
char EMH_vendor[3] = {'b', 'l', 'a'};
char EMH_serial[9] = {'1','2','3','4','5','6','7','8','9'};
uint64_t EMH_sumVal =5777;
uint32_t EMH_actVal = 63;


void setup_uart () {

    // put your setup code here, to run once:
    /* Configure parameters of an UART driver,
* communication pins and install the driver */

    //Serial.begin(115200);
    //Serial.println("hallo echo");
    uart_config_t conf_uart2 = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
   /* 
uart_param_config(UART_NUM_0, &uart_config0);
uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
uart_driver_install(UART_NUM_0,  BUF_SIZE * 2, 0, 0, NULL, 0);
*/

    // HW Serial 2
    uart_param_config(UART_NUM_2, &conf_uart2);
    uart_set_pin(UART_NUM_2, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_2, BUF_SIZE, 0, 20, &queue_uart2, 0);

    uart_enable_pattern_det_intr(UART_NUM_2, 0x1B, 4, 10000, 10, 10);
    uart_pattern_queue_reset(UART_NUM_2, 5); // keep 5 paatern positions in queue
}

void handle_uart_event () {

 uart_event_t event;

    if (xQueueReceive(queue_uart2, (void *)&event,( TickType_t ) 0))
    {
        Serial.print("we got an event: ");

        switch (event.type)
        {

        case UART_PATTERN_DET:

            Serial.print("pattern found, pos: ");
            pos = uart_pattern_pop_pos(UART_NUM_2);
            Serial.print(pos);
            uart_get_buffered_data_len(UART_NUM_2, (size_t *)&length);
            Serial.print(" / ");
            Serial.println(length);
            if (pos > 0)
            { //the assumption: first pattern is at position 0x0; 
              // so if we are here, we should have the 2nd at the end of the message
              // if it is a short fragment we just read it, check later will fail and we start over
              // as there is always a pause between messages we should hit the next one at pos 0x0 in the FIFO

              // so let´s read the complete message - we had allocated enough BUFF_SIZE - at least we hope so
                len = uart_read_bytes(UART_NUM_2, data, pos, 20 / portTICK_RATE_MS);
                
                //we  check the start of SML frame
                //otherwise, just skipp,
                uint32_t *smlarray = (uint32_t *)data; //we assume that the sml are 32 bit alligned, force casting

                if ((smlarray[0] == SMLFLAG) && (smlarray[1] == SMLSTART))
                {
                    Serial.println("StartFlag");
                    //read the endflag - it should be here, as we got the event and the pos
                    uint8_t len2;
                    len2 = uart_read_bytes(UART_NUM_2, &data[len], 8, 20 / portTICK_RATE_MS);
                    
                    //we could do some checks here, but is boring ..

                    //and now we can search for the values within data
                    EMH_obisParser(data);
                }
                //else we just wait for the next pattern event
            }

            break;
        default:
            Serial.println(event.type, HEX);
        }
    }
};

void EMH_obisParser(uint8_t *smlmsg)
{
//ToDo encapsulate data structure and parsing in a object
// add a kind of msg counter to give a feeling to the log file writer
// that the message has changed
/*
OBIS pattern search
taken from real-life scaned and analysed
we do not have the idea to parse every meter
there are all a bit different and need so interpretation anyhow
we should check if we have the right meter
01 00 00 00 09 could be a kind device serialnr. of the meter
01 A8 15 13 35 77 03 01 02
77 07 01 00 00 00 09 FF 01 010101 0A - 01 A8 15 13 35 77 03 01 02
ServerID; offset 0x7C + 9 Byte, hex string


Hersteller EMH (45 4D 48)
 77 07 81 81 C7 82 03 FF 01 01 01 01 04 45 4D 48 
Hersteller, offset 0x6B + 3 byte, ASCII

the pattern we are looking for is this one
77 07 01 00 01 08 00 FF 64 00 01 00 01 62 1E 52 FF 56 00 03 56 32 5A
the last 5 bytes are the value, 
01 00 01 08 00 tells us it is the overall electrician consumption
Total Power, offset 0x98 + 5 bytes, DEC, scale 10 000, unit kWh

und das hier ist der aktuelle Verbrauch
77070100 100700FF 0101621B 52FF5500 000000
current power , offset 0xD7 4 byte , scale 10, W
*/

 /*   typedef struct // for parsing definitions
    {
        String name;
        uint8_t valoffset;
        uint8_t valsize;
        String valtype;
        uint8_t valscale;
        String valunit;
    } obisdef_t;
*/

portENTER_CRITICAL(&EMHparseMutex);

    //Serial.print("vendor: ");
    for (int i = 0; i<3; i++) {
        //Serial.print((char) smlmsg[0x6B+i]);
        EMH_vendor[i]= smlmsg[0x6B+i];
    }
   // Serial.println();

    //Serial.print("ID:");
    for (int i = 0; i<9; i++) {
        //Serial.print( smlmsg[ 0x7C+i], HEX);
        EMH_serial[i] = smlmsg[ 0x7C+i];
    }
   // Serial.println();

    //we get the main counter
    uint8_t *buf2;

    //this is a uggly hack to convert big endian to little endian
    // as the SML stream has only 5 byte, we can not use standard libs
    buf2 = (uint8_t*) &EMH_sumVal;

    for (int i = 0; i < 5; i++) {
        buf2[4-i] = smlmsg[0x98+i];
    }
    
    
    /*
    double powfl = ((double)pow1) / 10000;
    Serial.print("pow1: ");
    Serial.println(powfl);
    */
    //we get the current power consumption
    
    buf2 = (uint8_t*) &EMH_actVal;

    for (int i = 0; i < 4; i++) {
        buf2[3-i] = smlmsg[0xD7+i];
    }
    
    //EMH_actVal = be32toh(smlmsg[0xD7]);
    /*
    powfl = ((double)powc) / 10;
    Serial.print("powc: ");
    Serial.println(powfl);
    */

   portEXIT_CRITICAL(&EMHparseMutex);
}
