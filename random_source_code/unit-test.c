  /*
  // Initialise each sensor
  IIC_ERRORS iicSensorInit();
  // Get the raw acceleration data from the sensor
  IIC_ERRORS getRawDataAccel(AccelRaw *raw_data);
  // Get the raw magnetic data from the sensor
  IIC_ERRORS getRawDataMagnet(MagRaw *raw_data);
  // Get the raw gyro data from the sensor
  IIC_ERRORS getRawDataGyro(GyroRaw *raw_data);

  void test_gyros(void);
  void test_accelerometers(void);
  void test_magnetometers(void);
  */



/*
void test_accelerometers(){
  AccelRaw *raw_data
  // read from accel, check there are no
  // errors
  assert(getRawDataAccel(raw_data) == NO_ERROR);
  ...
  // sanity check the bounds of the
  // accel values
  assert (raw_data.x < 500 && raw_data.x > -500);
}
*/

/*
void test_gyros() {
  GyroRaw *raw_data
  // read from gyro, check there are no
  // errors
  assert(getRawDataGyro(raw_data) == NO_ERROR);
  ...
  // sanity check the bounds of the
  // gyro values
  assert (raw_data.x < 500 && raw_data.x > -500);
}
*/



/*
void test_magnetometers(){
  MagRaw *raw_data
  // read from mag, check there are no
  // errors
  assert(getRawDataMag(raw_data) == NO_ERROR);
  ...
  // sanity check the bounds of the
  // mag values
  assert (raw_data.x < 500 && raw_data.x > -500);
}
*/

/*
// data structures containing the raw values
typedef struct MagRaw {
  int x;
  int y;
  int z;
} MagRaw;
