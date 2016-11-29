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
int  NUM_SIGNAL_READS =   7; // time inbetween each intensity reading
long BIT_READ_DELAY   =   2; // delay inbetween reading each bit
int  DATA_LENGTH      =  15; // length of message in bits

int  ON_THRESHOLD = 100; // sensitivity +/- threshold for interrupt

long ENV_SAMPLE_RATE  =     5; // sampling rate to find ambient light intensity
long RECALIBRATE_RATE =  5000; // environment re-sampling rate
int  REQ_ENV_SAMPLES  =   200; // total number of readings needed to obtain sufficient intensity average for environment

/* Dynamic Global Variables */
long  timeEnvUpdate = 0; // time of last calibration
long  timePrev      = 0; // variable to hold time returned by millis()

float avgEnv     = 0; // current average of ambient light intensity for environment
long  sumEnv     = 0; // sum of intensity readings
long  numReadEnv = 0; // current number of intensity readings recorded

/* Functions */

/* 
 *  void sample_environment()
 *  
 *    Description: Samples environment at a specified rate (SAMPLE_RATE)
 *                 and updates avgCurr so that the current avgEnv value 
 *                 is accurate
 */
void sample_environment(){

  Serial.print( "Calibrating... " );
  Serial.print( "|" );
  avgEnv     = 0; // Reset average
  sumEnv     = 0; // Reset sum 
  numReadEnv = 0; // Reset number of readings

  while( numReadEnv < REQ_ENV_SAMPLES ){
  
    if( get_current_time() - timePrev > ENV_SAMPLE_RATE ) {
      
      timePrev = get_current_time();
      
      sumEnv += get_signal();
      numReadEnv++;
      avgEnv = sumEnv / numReadEnv;
      
      if( numReadEnv % 10 == 0 ){
        Serial.print( "#" ); 
      }
    }
  }
  Serial.print( "| " );
  Serial.println( avgEnv );
  timeEnvUpdate = get_current_time();
}

/* 
 *  void receive_data()
 *  
 *    Description: Receive data
 */
float receive_data(float intensityRead[ 15 /* num bits */ ][ 7 /* number of readings */ ]){
  
  Serial.print("Received");

  for( int i = 0; i < DATA_LENGTH; i++ ){
    
    timePrev = get_current_time();

    for( int k = 0; k < NUM_SIGNAL_READS; k++ ){
      
      timePrev = get_current_time();
      
      while( get_current_time() - timePrev < BIT_READ_DELAY ){
        
        if( get_current_time() - timePrev < ( BIT_READ_DELAY / 2 ) ){

          intensityRead[ i ][ k ] = get_signal();
        }
      }
      Serial.print("(");
      Serial.print(i);
      Serial.print(",");
      Serial.print(k);
      Serial.print(") =");
      Serial.println(intensityRead[i][k]);
    }
  }  
}

/* 
 *  void format_data()
 *  
 *    Description: Format data
 */
int *format_data(float intensityRead[ 15 /* num bits */ ][ 7 /* number of readings */ ], int message[ 15 /* num bits */ ]){

  int on  = 0;
  int off = 0;

  for( int i = 0; i < DATA_LENGTH; i++ ){

    for( int k = 0; k < NUM_SIGNAL_READS; k++ ){
    
      if( intensityRead[ i ][ k ] - avgEnv >= ON_THRESHOLD){
        
        on++; 
      } else {
        
        off++;
      }
    }

    if( on > off ){

      message[ i ] = 1;
    } else {

      message[ i ] = 0;
    }
    Serial.print(message[ i ]);
    on = 0;
    off = 0;
  }
  Serial.println();
  return message;
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

    float intensityRead[ 15 /* num bits */ ][ 7 /* number of readings */ ]; // saves each reading
    int message[ 15 /* num bits */ ];                                        // array of bits received
    
    if( numReadEnv == 0 || get_current_time() - timeEnvUpdate > RECALIBRATE_RATE ) {
      
      sample_environment();
    } else if( get_signal() > ( int( avgEnv ) + ON_THRESHOLD ) ){
      
      receive_data(intensityRead);
      format_data(intensityRead, message);
      sample_environment();
    }
}
