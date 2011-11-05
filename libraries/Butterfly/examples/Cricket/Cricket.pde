#include <butterfly_temp.h>

// Simulate a Snowy Tree Cricket. The cricket will chirp if it is 
// not too bright, and will stop chirping if the light level increases
// during the song. The number of chirps in 13 seconds plus 40 indicates
// the temperature in degrees F.

// Uncomment this #define to see various parameters on the serial port
// #define debug

// This is close to the number of microseconds per half-cycle of the
// cricket's 2.9kHz carrier frequency, which is about 172uS. Because
// the pizeo speaker is being driven with a square wave the audible
// pitch does not sound like a 2.9kHz sine wave. The 125uS delay sounds
// closer to the cricket chirp, and seems to work well with the piezo
// speaker as well. Feel free to adjust it up or down to hear the change.
int chirpFreq = 125;
int light;

void setup()
{
#ifdef debug
  Serial.begin(9600); 
#endif
  pinMode( SPEAKER, OUTPUT );

  // Use the light sensor for a random seed
  randomSeed( analogRead(LIGHT) );
}

// A chirp chip is composed of a few mS of 2.9kHz carrier.
void pulse(int duration)
{  
  unsigned long term = millis() + duration;
 
  while ( millis() < term)
  {
    digitalWrite( SPEAKER, HIGH );
    delayMicroseconds( chirpFreq );
    digitalWrite( SPEAKER, LOW );
    delayMicroseconds( chirpFreq );
  }
}

// chipA is a two-pulse construct.
void chipA()
{
  pulse( 10 );
  delay( 6 );
  pulse( 13 ); // last pulse in a chip is longer than other pulses
  delay( 9 );  // include a few extra mS for delay between chips
}

// chipB is a three-pulse construct.
void chipB()
{
  pulse( 10 );
  delay( 6 );
  pulse( 10 );
  delay( 6 );
  pulse( 13 ); // last pulse in a chip is longer than other pulses
  delay( 9 );  // include a few extra mS for delay between chips
}

// chirp1 is a chip sequence of A,B
void chirp1()
{
  chipA();
  chipB();
}

// chirp2 is a chip sequence of A,B,B
void chirp2()
{
  chipA();
  // return early once in a while
  if (random(100)>2)
  {
    chipB();
    chipB();
  }
}

// The chirp rate depends on temperature. The temperature in
// degees F is equal to the number of chirps in 13 seconds, plus 40.
// Chirp rate variation can be approximated by adjusting the delay
// between chirps.
int findChirpDelay( int temp)
{
    int temp_delay[] = {  // temperature to delay map
    50, 1300,   52, 1083,   54, 929,   56, 812,   58, 722,
    60, 650,    62,  590,   64, 541,   66, 500,   68, 464,   
    70, 433,    72,  406,   74, 382,   76, 361,   78, 342,
    80, 325,    82,  309,   84, 295,   86, 282,   88, 271,
    90, 260,    92,  250,   94, 241,   96, 232,   98, 224,
  };
  
  int i;
  for (i=0; i<48; i+=2)
  {
    if (temp <= temp_delay[i])
      break;
  }
  return temp_delay[i+1];
}

// A song is a long sequence of chirps, about a minute.
// The number of chirps will vary depending on the chirpDelay
// and the length of the song.
void song(long songLength)
{
  int temp = TempSense.getTemp( FAHRENHEIT );
  int chirpDelay = findChirpDelay( temp );
  int chirpCount = songLength / ( chirpDelay + 50 );
  int breakLight = analogRead(LIGHT) - 80;
  
#ifdef debug
  Serial.print("Temp: ");
  Serial.println(temp, DEC);
  Serial.print("Chirps: ");
  Serial.println(chirpCount, DEC);
  Serial.print("Chirp delay: ");
  Serial.println(chirpDelay, DEC);
#endif
  
  for (int i=0; i < chirpCount; i++)
  {
    if (random(100) < 5)
      chirp1();
    else
      chirp2();     
      
    delay( chirpDelay );
      
    // Crickets go quiet if it gets too bright.
    int lite = analogRead(LIGHT);
#ifdef debug
    Serial.print("Light: ");
    Serial.println(lite, DEC);
#endif
    if ( lite < breakLight )
    {
#ifdef debug
      Serial.println("Scared!");
#endif
      return;
    }
  }    
}

void loop()
{
  long duration;

  // check the ambiant light
  light = analogRead( LIGHT );
  
#ifdef debug
  Serial.print("Light: ");
  Serial.println(light, DEC);
#endif
  
  // if it's dark enough
  if (light > 500)  
  {
    // Sing for 10 to 90 seconds
    duration = random( 10 * 1000, 90 * 1000 );
#ifdef debug
    Serial.print("Sing: ");
    Serial.println(duration, DEC);
#endif    
    song( duration );
  }
  
  // Then be silent for 30 to 300 seconds
  duration = random( 30, 300 ) * 1000;
#ifdef debug
  Serial.print("Rest: ");
  Serial.println(duration, DEC);
#endif  
  delay( duration );      
}
