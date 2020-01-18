
// CAT
// >> FA; << FA14250000; (query frequency)
// >> FA14250000; (set frequency)
// >> RM6; << RM6008; (query stationary)


#define BUFFER_SIZE 64    // The max lenght of a CAT command in FT450D is the KM command with size of 43+1
#define CMD_SIZE    2     // Every command have size of 2
//#define DEBUG true

char buffer[BUFFER_SIZE];
int lenBuffer;
char bufferResponse[BUFFER_SIZE];
bool validCmd;

void cleanOut()
{
  memset(buffer, 0, sizeof(buffer));
  lenBuffer=0;
  memset(bufferResponse, 0, sizeof(buffer));  
  validCmd=false;
}

void printArray(char *b, int size)
{ 
  for(int i=0; i<size; i++)
    Serial.print(b[i]);
}
    
void printlnArray(char *b, int size)
{  
  for(int i=0; i<size; i++)
    Serial.print(b[i]);
  Serial.println();
}

int montaResponse(char *result, int size)
{
  int index=0;

  bufferResponse[index++]=buffer[0];
  bufferResponse[index++]=buffer[1];
  for(int i=0; i<size; i++)
    bufferResponse[index++]=result[i];
  bufferResponse[index++]=';';

  return index;
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);
#ifdef DEBUG
  Serial.println("CAT emulator started");
#endif
  cleanOut();
#ifdef DEBUG
  Serial.println("Cleaned buffer");
#endif
}

void loop() {
  if(Serial.available() > 0)
  {         
    cleanOut();
    int r=0;
    
    // If dont have bytes to read, Serial.read() returns -1 
    /*while( (c=Serial.read()) > -1 )
    {
      buffer[index++]=c; 
      //Serial.print("index=");Serial.println(index);
      if(c == ';')
      {                
        Serial.print("s");
        // We have received the terminator character
        // If received at least 3 chars, its a reading, eg "FA;" querying the frequency
        if(index == 3){        
          queryCmd=true;
          memcpy(cmd, buffer, CMD_SIZE);
        }     
        else if(index > 3){     
          setCmd=true;
          memcpy(cmd, buffer, CMD_SIZE);
        }

        Serial.print("BufferReceived=");printArray(buffer, index);Serial.println();
        break;        
      }
    }*/
    r=Serial.readBytesUntil(';', buffer, BUFFER_SIZE);
    if(r > 1){  
      // We have received the terminator character ';', but it does not enter into buffer
      // If received at least 2 chars, its a reading, eg "FA" + ";" querying the frequency      
      validCmd=true;
      lenBuffer=r;
    }     
#ifdef DEBUG
    Serial.print("BufferReceived=");printlnArray(buffer, r);
#endif    
  }

  if(validCmd==true)
  {
    int r=0;
    if(buffer[0] == 'F' && buffer[1] == 'A'){
      // FA - FREQUENCY ADJUST
      if( (lenBuffer-2) <= 2){
        // Fa querying...
        r=montaResponse("14250000", 8);
      }
      else{
        // The frequency is being setted      
      }
      printArray(bufferResponse, r);
    }
    else if(buffer[0] == 'R' && buffer[1] == 'M' && buffer[2] != 0){
      // RM - READ METER
      // S METER
      if(buffer[2] == '1') r=montaResponse("1089", 4);
      // ALC METER
      if(buffer[2] == '4') r=montaResponse("4022", 4);
      // POWER METER
      if(buffer[2] == '5') r=montaResponse("5255", 4);
      // SWR METER
      if(buffer[2] == '6') r=montaResponse("6008", 4);
      printArray(bufferResponse, r);
    }
    else{
      Serial.print("?");
    }
    
    validCmd=false;
  }
  
}
