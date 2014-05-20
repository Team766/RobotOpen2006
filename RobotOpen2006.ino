#include <SPI.h>
#include <Ethernet.h>
#include <RobotOpen.h>

/*
 * RobotOpen code for 2006 robot.
 * It's not the easiest to drive,
 * but we live with the limitations 
 * of having only one joystick.
 * It works and with practice, 
 * the robot can be driven with
 * full functionality.
*/

//static byte ip[]  = { 192, 168, 1, 22 };
static byte ip[] = { 10, 7, 66, 22 };
int leftDrive;
int rightDrive;
int Hopper;
int Belt;
int Shooter = 127;
int ShooterS = 127; // saved value

/* I/O Setup */
USBJoystick usb1('0');  // Assign the logitech USBJoystick object to bundle 0

void setup()
{
  // Serial.begin( 115200); // Serial Monitor commented out 
  /* Initiate comms */
  RobotOpen.begin(ip);
  printf("This is working\n");
}
/* This is your primary robot loop - all of your code
 * should live here that allows the robot to operate
 */
void enabled() {
  // Constantly update PWM values with joystick values
  // arcade drive
  leftDrive = usb1.makePWM(ANALOG_LEFTY, INVERT) - (127 - usb1.makePWM(ANALOG_LEFTX, NORMAL));
  rightDrive = usb1.makePWM(ANALOG_LEFTY, INVERT) + (127 - usb1.makePWM(ANALOG_LEFTX, NORMAL));
  
  leftDrive = constrain( leftDrive, 0, 255);  
  rightDrive = constrain( rightDrive, 0, 255);
  
// Now shooter speed 
// Uses the right joystick
// Point up for 50-100%
// point down for 0-50%
// Since this is very awkward,
// save with left shoulder 1
// use saved values with left shoulder 2
  if( usb1.getBtn(BTN5, NORMAL))
    ShooterS = Shooter; // save for reuse if Btn 5 
  else if (usb1.getBtn(BTN7, NORMAL))
    Shooter = ShooterS; // if Btn pressed use previous value
  else if (usb1.makePWM(ANALOG_RIGHTY, NORMAL) > 150)
    Shooter = (((usb1.makePWM(ANALOG_RIGHTY, NORMAL)-127)/2) + 127);
  else if (usb1.makePWM(ANALOG_RIGHTY, NORMAL) < 115)
    Shooter = (((usb1.makePWM(ANALOG_RIGHTY, INVERT)-127)/2) + 191);
  else
    Shooter = 127; 
  
// now for Belt and Hopper
  if (usb1.getBtn(BTN2, NORMAL)){
    Belt = 0;
    Hopper = 0;
  }
  else if (usb1.getBtn(BTN3, NORMAL)){
    Belt = 255;
    Hopper = 255;
  }
  else if (usb1.getBtn(BTN1, NORMAL)){
    Belt = 0;
    Hopper = 255;
  }
  else if (usb1.getBtn(BTN4, NORMAL)){
    Belt = 255;
    Hopper = 64;
  }
  else{
    Belt = 127;
    Hopper = 127;
  } 
  RobotOpen.setPWM(SIDECAR_PWM1, leftDrive);
  RobotOpen.setPWM(SIDECAR_PWM2, rightDrive);
  RobotOpen.setPWM(SIDECAR_PWM3, Hopper);
  RobotOpen.setPWM(SIDECAR_PWM4, Belt);
  RobotOpen.setPWM(SIDECAR_PWM5, Shooter); // Shooter has two left and right motors, used on splitter
}

/* This is called while the robot is disabled
 * You must make sure to set all of your outputs
 * to safe/disable values here
 */
void disabled() {
  // PWMs are automatically disabled
}

/* This loop ALWAYS runs - only place code here that can run during a disabled state
 * This is also a good spot to put driver station publish code
 * You can use either publishAnalog, publishDigital, publishByte, publishShort, or publishLong
 * Specify a bundle ID with a single character (a-z, A-Z, 0-9) - Just make sure not to use the same twice!
 */
void timedtasks() {
  //Publish the analog pins because they jitter
  //it's a good indication of comms
  RobotOpen.publishAnalog(ANALOG0, 'A');   // Bundle A
  RobotOpen.publishAnalog(ANALOG1, 'B');   // Bundle B
  RobotOpen.publishAnalog(ANALOG2, 'C');   // Bundle C
  RobotOpen.publishAnalog(ANALOG3, 'D');   // Bundle D
  RobotOpen.publishAnalog(ANALOG4, 'E');   // Bundle E
  RobotOpen.publishAnalog(ANALOG5, 'F');   // Bundle F 
  //publish the current shooter value and saved shooter value
  RobotOpen.publishInt(Shooter, 'G');
  RobotOpen.publishInt(ShooterS, 'H');
}

/* This is the main program loop that keeps comms operational
 * There's no need to touch anything here!!!
 */
void loop() {
  RobotOpen.pollDS();
  if (RobotOpen.enabled())
    enabled();
  else
    disabled();
  timedtasks();
  RobotOpen.outgoingDS();
}


