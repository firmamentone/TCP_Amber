//Based on "https://www.the-diy-life.com/running-an-artifical-neural-network-on-an-arduino-uno/"
//Author: Ralph Heymsfeld
//28/06/2018
//==========================================================================
//Branch: Heater Test 
//DEC/17/2019 T.I ANN engine Test
//Nov/09/2019 T.I Modified parseCommand 
//Sep/15/2019 T.I Added FCODE Parser
//May/18/2019 T.I Added UART reciveing and PWM output
//====================================
//05/05/2019 T.I. Created it for TCP_Amber project 



#include <math.h>

int ledPin = 13;

/******************************************************************
 * Network Configuration - customized per network 
 ******************************************************************/

const int PatternCount = 1;
const int InputNodes = 5;
const int HiddenNodes = 10;
const int OutputNodes = 1;
const float LearningRate = 0.3;
const float Momentum = 0.9;
const float InitialWeightMax = 0.5;
const float Success = 0.0004;

float Input[PatternCount][InputNodes] = {
  { 0.0, 0.0, 0.0, 0.0, 0.0 },  // 0
}; 




const float HiddenWeights[6][10]={  //InputNodes HiddenNodes
  {-0.899681951656134,-15.8371542421008, 2.70134683699000,  -19.5310880809337, 28.3476591628916,  10.9295450270898,  -4.82512519458506, 0.472814426707092, -17.4408025854934},
  {-24.0229269295204, 18.2890102134664,  -18.0113621241397, -23.9160742889159, -13.0671891053843, -19.6125154894177, -2.95536680085632, 9.67656630776159,  23.8091218684118},
  {3.80596074296199,  16.9469579114765,  1.84468398868810,  2.23084209510088,  -1.55771567586442, -3.57219323137401, -1.76586048775300, -1.86517346129092, -1.47915454358980},
  {-1.49901767998746, 3.35854882088111,  -9.22505372731765, 1.44531045878283,  21.3581080626585,  -13.7504146922347, -2.54739525344600, -6.59281338647536, 1.04200877423785},
  {-7.25510658122490, -0.917006924612914,  -8.85762774038732, 5.01407922673183,  10.4191813952311,  -10.7905385833334, -0.954415554179315,  -0.420951042291907,  7.70640750806352},
  {26.9292,11.3389,20.6941,-5.0427,2.4613,4.3413,-21.7725,-5.703,-4.3389,6.434}//bias
};

const float OutputWeights[11][1]={  //InputNodes HiddenNodes
 {-11.1219 },
 {5.8228},
 {6.6023 },
 {-6.1241 },
 {-10.3558},
 {13.4695  },
 {18.0053 },
 {-0.65064},
 {-11.4355 },
 {21.8802},
 {-6.8881 } //bias
};

/******************************************************************
 * End Network Configuration
 ******************************************************************/


int i, j, p, q, r;
int ReportEvery1000;
int RandomizedIndex[PatternCount];
long  TrainingCycle;
float Rando;
float Error;
float Accum;


float Hidden[HiddenNodes];
float Output[OutputNodes];


int PIN_HeatingOutput = 11; //PB7
int serialIncome= 0;

int PIN_THSensor = A0; 
int THSensorADC = 0 ;

String FcmdRec[9]; //0:FCODE command  1~8 Receving data
//String arg0[5];

void setup(){


  pinMode(PIN_HeatingOutput, OUTPUT); 
  
  Serial.setTimeout(50);
  Serial.begin(9600);
  randomSeed(analogRead(3));
  pinMode(ledPin, OUTPUT);


}  

void loop (){
    static bool LEDStatus = 0;
    float tempC=0.0;
    int heatOuput; //0~255
    
    digitalWrite(ledPin, LEDStatus);
    LEDStatus = !LEDStatus;
  
    if (Serial.available() > 0)
    {
      String RecStr;
      RecStr=Serial.readString();
      parseCommand(RecStr);
      
    }

    THSensorADC=analogRead(A0);


    Input[0][0]=65.0;

    tempC = (0.1153 * float(THSensorADC)) - 34.629;
  
    
    Input[0][4]=Input[0][3];
    Input[0][3]=Input[0][2];
    Input[0][2]=tempC-Input[0][1];
    Input[0][1]=tempC;

    toTerminal();


    if(0.1<Output[0])
    {
      heatOuput=40;
    }
    else
    {
      heatOuput=0;  
    }
    
    analogWrite(PIN_HeatingOutput, heatOuput);//serialIncome


  
}

void toTerminal()
{

    Accum = 0.0;
/******************************************************************
* Compute hidden layer activations
******************************************************************/

    for( i = 0 ; i < HiddenNodes ; i++ ) {    
      Accum = HiddenWeights[InputNodes][i] ; //bias-s
      for( j = 0 ; j < InputNodes ; j++ ) {
        Accum += Input[0][j] * HiddenWeights[j][i] ;
      }
      Hidden[i] = 1.0/(1.0 + exp(-Accum)) ;
    }

/******************************************************************
* Compute output layer activations and calculate errors
******************************************************************/

    for( i = 0 ; i < OutputNodes ; i++ ) {    
      Accum = OutputWeights[HiddenNodes][i] ;  //bias-s
      for( j = 0 ; j < HiddenNodes ; j++ ) {
        Accum += Hidden[j] * OutputWeights[j][i] ;
      }
      Output[i] = Accum;//1.0/(1.0 + exp(-Accum)) ; 
    }
  
}



void parseCommand(String comStr)
{
  String cmdRec[9];  
  cmdRec[0]=comStr.substring(0,4);

  if(cmdRec[0].equals("F101"))
  {
    cmdRec[1]=THSensorADC;
    Serial.println(cmdRec[0]+"Q"+cmdRec[1]);
  }
  else if(cmdRec[0].equals("F102"))
  {
    
    Serial.println (comStr);
    cmdRec[1]=comStr.substring(comStr.indexOf("Q")+1,comStr.indexOf("Q")+4);
    serialIncome=cmdRec[1].toInt();
  }
  
  
}
