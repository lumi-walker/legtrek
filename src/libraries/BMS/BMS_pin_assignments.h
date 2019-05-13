/* TEENSY 3.6 SPI pins
 * PIN DESCRIPTION:
 *    SPI_MOSI_PIN - MASTER OUT SLAVE IN pin on teensy 3.6
 *    SPI_MISO_PIN - MASTER IN SLAVE OUT pin on teensy 3.6
 *    SPI_CLK_PIN -  SPI clock pin on teensy 3.6
 */
 
#define TEENSY_MOSI_PIN        7
#define TEENSY_MISO_PIN        8
#define TEENSY_CLK_PIN         13
/************************************************************************************************************/
/************************************************************************************************************/

/* power control (relay)
 * BRIEF:
 *    Teensy 3.6 will control current flow to the lift columns and motor.
 *    If over-current,over-voltage, or over-temperature them switch off current flow
 * PIN DESCRIPTION:
 *    POWER_CTRL_PIN_1 - pull high to cut off power to lift columns
 *    POWER_CTRL_PIN_2 - pull high to cut off power to motors
 */

#define PWR_PIN_1    19
#define PWR_PIN_2    18

/************************************************************************************************************/
/************************************************************************************************************/
/* current sensor: ACS722 40AU [OBSELETE]
 * BRIEF:
 *    Obtain current measure by reading voltage at the V_IOUT pin on the ACS722.
 * PIN DESCRIPTION:
 *    I_SENSE_READ_PIN_1 - maps to analog pins A9 on teensy 3.6 and is used to measure V_IOUT on 1st current sensor.
 *    I_SENSE_READ_PIN_2 - maps to analog pins A8 on teensy 3.6 and is used to measure V_IOUT on 2nd current sensor.
 *    I_SENSE_READ_PIN_3 - maps to analog pins A7 on teensy 3.6 and is used to measure V_IOUT on 3rd current sensor.
 */
 
 /*
#define I_SENSE_READ_PIN_1  23
#define I_SENSE_READ_PIN_2  22
#define I_SENSE_READ_PIN_3  21
*/

/************************************************************************************************************/
/************************************************************************************************************/
/* Thermalcouple sensor: MAX31855 IC (ADA269)
 * BRIEF:
 *    Obtain temperature reading from ADA269 via SPI.
 *    Since the ADA269 does not take in data, it does not have a MOSI (data in) pin.
 *    It only has a MISO (data out, DO) pin.
 * PIN DESCRIPTION:
 *    T_SENSE_CHIP_SEL_1 - selects the first ADA269 device for communication with Teensy
 *    T_SENSE_CHIP_SEL_2 - selects the second ADA269 device for communication with Teensy
 *    T_SENSE_CHIP_SEL_3 - selects the third ADA269 device for communication with Teensy
 */

#define T_SENSE_CHIP_SEL_1  2
#define T_SENSE_CHIP_SEL_2  3
#define T_SENSE_CHIP_SEL_3  4
/************************************************************************************************************/
/************************************************************************************************************/
/* Voltage Monitor
 * PIN DESCRIPTION:
 *    V_SENSE_READ_PIN - responsible for reading the voltage at the voltage divider
 */
#define V_SENSE_READ_PIN    20
/************************************************************************************************************/
/************************************************************************************************************/
/* 
 * PIN DESCRIPTION:
 *    DUE_INTERRUPT_PIN - triggers an interrupt in the DUE which will read voltage at DUE_MESSAGE_PIN
 *    DUE_MESSAGE_PIN - communicates error by writing some voltage which maps to an error message
 */
 
#define TEENSY2DUE_CALL_PIN  39  // interrupt
#define DUE2TEENSY_CALL_PIN  38 
#define TEENSY_TX_PIN	 26  //transmit on teensy
#define DUE_RX_PIN	 19 //receive on due 

/************************************************************************************************************/
/************************************************************************************************************/
