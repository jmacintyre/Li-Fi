/*
 * Author: Jordan MacIntyre
 * Created: Nov 26, 2016 
 * Revision: 1.0
 * Last Revision Date: Nov 27, 2016
 * 
 *
 * Li-Fi_Receiver:
 * 
 *    This program is designed to handle the reception of light modulation signals using a photoresistor. 
 *    The ambient light intensity if calculated by taking the average intensity over a specified number 
 *    of readings set by the 'AVG_COMPLETE' constant. A interrupt occurs once a light intensity is read 
 *    that falls outside of the average by a difference greater than the defined 'SENSITIVITY' constant.
 */

/* Pin Setup */
int PR = A0; // photoresisitor pin

/* Static Global Variables */
long BIT_TIME_BLOCK =  1000; // time duration to receive a bit
int  DATA_LENGTH    =    15; // length of message in bits
int  THRESHOLD      =   200; // sensitivity +/- threshold for interrupt

long ENV_SAMPLE_RATE  =     5; // sampling rate to find ambient light intensity
long RECALIBRATE_RATE =  5000; // environment re-sampling rate
int  REQ_ENV_SAMPLES  =   100; // total number of readings needed to obtain sufficient intensity average for environment

/* Dynamic Global Variables */
long  timeEnvUpdate = 0; // time of last calibration
long  timePrev      = 0; // variable to hold time returned by millis()

float avgEnv     = 0; // current average of ambient light intensity for environment
long  sumEnv     = 0; // sum of intensity readings
long  numReadEnv = 0; // current number of intensity readings recorded

float avgRec = 0; // average intensity of received signal
long  sumRec = 0; // sum of received signal's intensities
long  numRec = 0; // number of readings received

/* Functions */

/* 
 *  void sample_environment()
 *  
 *    Description: Samples environment at a specified rate (SAMPLE_RATE)
 *                 and updates avgCurr so that the current avgEnv value 
 *                 is accurate
 */
void sample_environment(){

  Serial.println("Calibrating...");
  avgEnv     = 0; // Reset average
  sumEnv     = 0; // Reset sum 
  numReadEnv = 0; // Reset number of readings

  while( numReadEnv < REQ_ENV_SAMPLES){
  
    if( get_current_time() - timePrev > ENV_SAMPLE_RATE ) {
      
      timePrev = get_current_time();
      
      sumEnv += get_signal();
      numReadEnv++;
      avgEnv = sumEnv / numReadEnv;
    }
  }
  Serial.print("Current ENV: ");
  Serial.println(avgEnv);
  timeEnvUpdate = get_current_time();
}

/* 
 *  void receive_data()
 *  
 *    Description: Receive data
 */
void receive_data(){
  
  Serial.println("DATA");
  Serial.print("Received: ");

  for(int i = 0; i < DATA_LENGTH; i++){
        
    avgRec = 0; // Reset average
    sumRec = 0; // Reset sum 
    numRec = 0; // Reset number of readings
    
    timePrev = get_current_time();
    
    while( get_current_time() - timePrev < BIT_TIME_BLOCK ) {
      
      sumRec += get_signal();
      numRec++;
      avgRec = sumRec / numRec;
    }
    if(avgRec - avgEnv > THRESHOLD){
      Serial.print(1); 
    } else{
      Serial.print(0);
    }
  }
  Serial.println();
}

/* 
 *  float get_signal()
 *  
 *    Description: Checks the value of PR 
 */
float get_signal(){
  float intensity = analogRead( PR );
  return intensity;
}

/* 
 *  unsigned long get_current_time()
 *  
 *    Description: Returns the current operation time
 */
unsigned long get_current_time(){
  unsigned long timeCurr = millis();
  return timeCurr;
}

/* Setup */  
void setup() {
  
  pinMode( PR, INPUT );
  Serial.begin( 9600 );
}

/* Main Loop */
void loop() {  
    
    if( numReadEnv == 0 || get_current_time() - timeEnvUpdate > RECALIBRATE_RATE ) {
      sample_environment();
    } else if( get_signal() > ( int( avgEnv ) + THRESHOLD ) ){
      receive_data();
      sample_environment();
    }
}
