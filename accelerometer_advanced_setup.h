/*
Code contained here is taken from:
https://github.com/algomusic/Mozzi_MPU6050/blob/main/Mozzi_MPU6050.ino
This is a great solution which reads "an MPU6050 accelerometer using 
I2C communication without blocking audio synthesis".
Gyro readings are commented out, and Serial print statements were 
removed from the code.
*/

#include <twi_nonblock.h>

static volatile byte acc_status = 0;
#define ACC_IDLE 0
#define ACC_READING 1
#define ACC_WRITING 2

int accbytedata[14];

#define MPU6050_ADDR                  0x68
#define MPU6050_SMPLRT_DIV_REGISTER   0x19
#define MPU6050_CONFIG_REGISTER       0x1a
#define MPU6050_GYRO_CONFIG_REGISTER  0x1b
#define MPU6050_ACCEL_CONFIG_REGISTER 0x1c
#define MPU6050_PWR_MGMT_1_REGISTER   0x6b
#define MPU6050_ACCEL_OUT_REGISTER    0x3b
#define TEMP_LSB_2_DEGREE     340.0    // [bit/celsius]
#define TEMP_LSB_OFFSET       12412.0

// Writes val to address register on device
void acc_writeTo(byte address, byte val) {
  twowire_beginTransmission(MPU6050_ADDR); // start transmission to device
  twowire_send( address );
  twowire_send( val );
  twowire_endTransmission();
}

void setup_accelero(){
  initialize_twi_nonblock();

  acc_writeTo(MPU6050_SMPLRT_DIV_REGISTER, 0x00);
  acc_writeTo(MPU6050_CONFIG_REGISTER, 0x00);
  acc_writeTo(MPU6050_GYRO_CONFIG_REGISTER, 0x00); //0x08
  acc_writeTo(MPU6050_ACCEL_CONFIG_REGISTER, 0x00);
  acc_writeTo(MPU6050_PWR_MGMT_1_REGISTER, 0x01);
  
  acc_status = ACC_IDLE;
}

/// ---------- non-blocking version ----------
void initiate_read_accelero(){
  // Reads num bytes starting from address register on device in to _buff array
  // set address of targeted slave
  txAddress = MPU6050_ADDR; //MMA7660_ADDR;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;

  // put byte in tx buffer
  txBuffer[txBufferIndex] = MPU6050_ACCEL_OUT_REGISTER; // Start reading from register for X
  ++txBufferIndex;
  // update amount in buffer
  txBufferLength = txBufferIndex;

  twi_initiateWriteTo(txAddress, txBuffer, txBufferLength);
  acc_status = ACC_WRITING;
}

void initiate_request_accelero(){
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;

  byte read = twi_initiateReadFrom(MPU6050_ADDR, 14); // 14 is the number of bytes to read
  acc_status = ACC_READING;
}

void finalise_request_accelero() {
  byte read = twi_readMasterBuffer( rxBuffer, 14 ); 
  // set rx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = read;

  byte i = 0;
  while( rxBufferLength - rxBufferIndex > 0) { // device may send less than requested (abnormal)
    accbytedata[i] = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
    i++;
  }

  acc_status = ACC_IDLE;
}
/// ----end------ non-blocking version ----------



// Run inside Mozzi updateControl()
void updateAccelerometer() {
  ms = millis();
  if (ms > readTime) {
    readTime += 20;
 
    switch( acc_status ){
    case ACC_IDLE:
      accx = (accbytedata[0] << 8 | accbytedata[1]) >> 7; // accelerometer x reading, reduced to 8 bit
      accy = (accbytedata[2] << 8 | accbytedata[3]) >> 7; // accelerometer y reading, 8 bit
      accz = (accbytedata[4] << 8 | accbytedata[5]) >> 7; // accelerometer z reading
      // gyrox = (accbytedata[8] << 8 | accbytedata[9]) >> 7; // gyro x reading, reduced to 8 bit
      // gyroy = (accbytedata[10] << 8 | accbytedata[11]) >> 7; // gyro y reading, 8 bit
      // gyroz = (accbytedata[12] << 8 | accbytedata[13]) >> 7; // gyro z reading

      initiate_read_accelero();

      break;
    case ACC_WRITING:
      if ( TWI_MTX != twi_state ){
        initiate_request_accelero();
      }
      break;
    case ACC_READING:
      if ( TWI_MRX != twi_state ){
        finalise_request_accelero();
      }
      break;
    }
  }
}


