//levelOneSketch.ino

// #define TUBETWODAT     5//A4
// #define TUBETWOCLK     6//A5
#define TUBETWODAT     A4
#define TUBETWOCLK     A5
#define BUZZERPIN	   9

//LED 74hc164
//vcc pc1; pd5 data; pd6 clk; gnd pc0

// #define LEDDAT       A4//5
// #define LEDCLK       A5//6
#define LEDDAT       6
#define LEDCLK       5
#define LEDVDD       A1 
#define LEDGND       A0     

#define NVCPIN       4   

#define TEMPERATUREPIN 2



uint8_t tubeList[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uint8_t ledList[] ={0xff,0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f,0x00}; //0 - 8
uint16_t temperatureList[] = {

        951/*-10*/, 947/*-9*/, 943/*-8*/, 939/*-7*/, 934/*-6*/, 
        930/*-5*/, 925/*-4*/, 920/*-3*/, 915/*-2*/, 910/*-1*/, 
        904/*0*/, 898/*1*/, 893/*2*/, 887/*3*/, 880/*4*/, 
        874/*5*/, 867/*6*/, 860/*7*/, 853/*8*/, 846/*9*/, 
        839/*10*/, 831/*11*/, 823/*12*/, 816/*13*/, 807/*14*/, 
        799/*15*/, 791/*16*/, 782/*17*/, 773/*18*/, 764/*19*/, 
        755/*20*/, 746/*21*/, 736/*22*/, 727/*23*/, 717/*24*/, 
        707/*25*/, 698/*26*/, 688/*27*/, 679/*28*/, 669/*29*/, 
        659/*30*/, 649/*31*/, 639/*32*/, 629/*33*/, 619/*34*/, 
        609/*35*/, 599/*36*/, 589/*37*/, 579/*38*/, 569/*39*/, 
        559/*40*/, 549/*41*/, 539/*42*/, 529/*43*/, 519/*44*/, 
        509/*45*/, 499/*46*/, 490/*47*/, 480/*48*/, 470/*49*/, 
        461/*50*/, 451/*51*/, 442/*52*/, 433/*53*/, 424/*54*/, 
        415/*55*/, 406/*56*/, 397/*57*/, 388/*58*/, 380/*59*/, 
        371/*60*/, 363/*61*/, 355/*62*/, 347/*63*/, 339/*64*/, 
        331/*65*/, 323/*66*/, 316/*67*/, 309/*68*/, 301/*69*/, 
        294/*70*/, 287/*71*/, 281/*72*/, 274/*73*/, 267/*74*/, 
        261/*75*/, 255/*76*/, 248/*77*/, 242/*78*/, 237/*79*/, 
        231/*80*/, 225/*81*/, 220/*82*/, 214/*83*/, 209/*84*/, 
        204/*85*/, 199/*86*/, 194/*87*/, 189/*88*/, 185/*89*/, 
        180/*90*/, 176/*91*/, 171/*92*/, 167/*93*/, 163/*94*/, 
        159/*95*/, 155/*96*/, 151/*97*/, 148/*98*/, 144/*99*/, 
        140/*100*/

};
unsigned long nowTime;
unsigned long ledOpenTime;
uint8_t ledOpenFlag = false;
uint8_t talkFlag = false;

void setup()
{
	Serial.begin(9600);
	portInit();
	analogReference(DEFAULT);
	attachInterrupt(1, freshOut, RISING);
}

void loop()
{  
	nowTime = millis();
	if(ledOpenFlag){
		writeLedSeven(9);
		if(nowTime - ledOpenTime > 5000){
		    writeLedSeven(0);
		    ledOpenFlag = false;
		}   
	}
	if(talkFlag){
	    talkTemperature();
	    talkFlag = false;
	}

}

void portInit() {
	pinMode(TUBETWOCLK, OUTPUT);
	digitalWrite(TUBETWOCLK, HIGH);
	pinMode(TUBETWODAT, OUTPUT);
	digitalWrite(TUBETWOCLK, HIGH);
	pinMode(BUZZERPIN, OUTPUT);
	digitalWrite(BUZZERPIN, LOW); // BUZZER  PIN

	pinMode(LEDVDD, OUTPUT);
	digitalWrite(LEDVDD, HIGH);
	pinMode(LEDGND, OUTPUT);
	digitalWrite(LEDGND, LOW);
	pinMode(LEDDAT, OUTPUT);
	digitalWrite(LEDDAT, HIGH);
	pinMode(LEDCLK, OUTPUT);
	digitalWrite(LEDCLK, HIGH);

	pinMode(NVCPIN, OUTPUT);
	digitalWrite(NVCPIN, LOW);

	pinMode(TEMPERATUREPIN, INPUT_PULLUP);

}

void freshOut() {
	if(digitalRead(3)){
		delay(1);
		if(digitalRead(3)){
       		talkFlag = true;
       		ledOpenFlag = true;
       		ledOpenTime = millis();

		}
	}
}
void talkTemperature() {
    uint8_t j = 0;
    uint8_t tempVal = 125;
    uint16_t tempSampleVal = analogRead(TEMPERATUREPIN);
    Serial.print("tempSample: ");
    Serial.print(tempSampleVal);
    for(int i = 0; i < 111; i++)
    {
            j = i+1;
            if((tempSampleVal<=temperatureList[i])&&(tempSampleVal>=temperatureList[j])  )
            {                  
                  tempVal = i - 5;
                  i = 111;
                  Serial.print(" temperatureVal: ");
                  Serial.print(tempVal);
                               
            }       
    }
    if(j >= 111){
        errorNVC();
        Serial.print(" error");
    } else {
    	talkNumNVC(tempVal); 
    	delay(1000);
    }
    Serial.println();
}
void errorNVC() {
	writeNVC(23);
}
void writeNVC(uint8_t num) {
	unsigned char number = num;
    digitalWrite(NVCPIN, LOW);
    delay(5);
    for(int i = 0; i < 8; i++)
    {
        digitalWrite(NVCPIN, HIGH);
        if(number&0x01)
        {
           
            delayMicroseconds(2500);
            digitalWrite(NVCPIN, LOW);
            delayMicroseconds(900);
            
        }
        else
        {
            delayMicroseconds(900);
            digitalWrite(NVCPIN, LOW);
            delayMicroseconds(2500);
            
        }
        number >>= 1;
        
    }
    digitalWrite(NVCPIN, HIGH); 
}

void talkNumNVC(uint8_t num) {
	unsigned char gewei = num%10;
    unsigned char shiwei = num/10;
    delay(100);
    writeNVC(0x0c); //当前
    delay(450);
    writeNVC(0x0d);//温度
    delay(1000);
    if(num < 10)
    {
        writeNVC(gewei);
        delay(500);
    }
    else if(num == 10)
    {   
        writeNVC(0x0a);
        delay(500);
    }
    else if(num > 10 && num < 20)
    {
        writeNVC(0x0a);
        delay(500);
        writeNVC(gewei);
        delay(500);
    }
    else if(gewei == 0)
    {
        writeNVC(shiwei);
        delay(500);
        writeNVC(0x0a);
        delay(500);
    }
    else
    {
        writeNVC(shiwei);
        delay(500);
        writeNVC(0x0a); //拾
        delay(500);
        writeNVC(gewei);
        delay(500);
    }   
    writeNVC(0x0e); //摄氏度
}
/*
function: open the LED 74hc164
0-8
 */
void writeLedSeven(uint8_t num) {
	shiftOut(LEDDAT,LEDCLK,MSBFIRST,ledList[num]);
}
/*
@num: the number will be show
@duration: hold time /s
 */
void showNum2(uint8_t num, int duration) {
	
	for(int i=0; i<duration; i++){
	    for(int j=0; j<50; j++){
	    	writeDigitalTube2(num);
		}   
	}
	clearTube2();

}
void clearTube2() {
	shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,1);
	shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,tubeList[11]);
	shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,2);
	shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,tubeList[11]);
}
void errorTube2() {
	for(int i=0; i<50; i++){
	    shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,1);
		shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,tubeList[10]);
		delay(10);
		shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,2);
		shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,tubeList[10]);
		delay(10);
	}
	clearTube2();
}
void writeDigitalTube2(uint8_t number) {
	uint8_t shiwei,gewei;
	if(number > 99){
	    number = number%100;
	}
	shiwei = number / 10;
	gewei = (number) % (10);
	shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,1);
	shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,tubeList[gewei]);
	delay(10);
	if(shiwei != 0){
		shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,2);
		shiftOut(TUBETWODAT,TUBETWOCLK,MSBFIRST,tubeList[shiwei]);
		delay(10);    
	}	
}

void openBuzzer() {
	digitalWrite(BUZZERPIN, HIGH);
}
void closeBuzzer() {
	digitalWrite(BUZZERPIN, LOW);
}



