//levelOneSketch.ino

#include <SPI.h>
#include <RFID.h>
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

#define TEMPERATUREPIN A6

#define FANPIN      A3
#define BUTTONPIN   2
#define RECORDPIN   A2 

#define INFRAREDPIN    A7  //红外检测 

#define TRIGPIN      7
#define ECHOPIN      8

//D10 - 读卡器CS引脚、D5 - 读卡器RST引脚
RFID rfid(10,0); 

uint8_t tubeList[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uint8_t ledList[] ={0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00}; //0 - 8
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

//the max user num for store rfid card
unsigned long userId[50];
uint8_t str[16];
uint8_t  cardCount = 1;

unsigned long nowTime;
unsigned long ledOpenTime;
bool ledOpenFlag = false;
bool talkFlag = false;
bool distanceFlag = false;
uint8_t filterFlag;
uint16_t filterBuffer[10];
uint16_t filterSum;
uint16_t flowCount;

int m, n; //按键状态机

void setup()
{
	Serial.begin(9600);
	portInit();
	analogReference(DEFAULT);
	SPI.begin();
  	rfid.init();
	attachInterrupt(1, freshOut, RISING);
	userId[0] = 0;
	flowCount = 0;
	filterFlag = 0;
	m = 0;
	clearTube2();
	writeLedSeven(0);
}

void loop()
{  
	// nowTime = millis();
	// if(!digitalRead(BUTTONPIN)){
	//     delay(10);
	//     if(!digitalRead(BUTTONPIN)){
	//         m++;
	//         n = m%4;
	//         showNum2(n, 1);
	//     }
	// }
	// if(n == 3){
	//     fastTrainer();
	// }
	// if(ledOpenFlag){
	// 	openFan();
	// 	writeLedSeven(8);
	// 	if(nowTime - ledOpenTime > 5000){
	// 	    writeLedSeven(0);
	// 	    closeFan();
	// 	    ledOpenFlag = false;
	// 	} 
	// }
	// if(talkFlag){
	//     talkTemperature();
	//     talkFlag = false;
	// }

	// if(rfid.findCard(PICC_REQIDL, str) == MI_OK){ // 寻卡
	// 	//防冲突检测,读取卡序列号
	//  	if (rfid.anticoll(str) == MI_OK){
	      
	//     	Serial.print("The card's number is  : ");
	//         //显示卡序列号
	// 	    for(int i = 0; i < 4; i++){
	// 	        Serial.print(0x0F & (str[i] >> 4),HEX);
	// 	        Serial.print(0x0F & str[i],HEX);
	// 	    }
	// 	    Serial.println("");
	// 	    memcpy(rfid.serNum,str,5);
	// 	    //register card id to system
	//         unsigned long tempSerNum;
	//         tempSerNum = (((unsigned long)str[0] << 24) + ((unsigned long)str[1] << 16) + ((unsigned long)str[2] << 8) + str[3]);

	//           //选卡，返回卡容量（锁定卡片，防止多次读写）
	// 	    rfid.selectTag(rfid.serNum);
	// 	    checkCardNumber(tempSerNum);
	// 	    rfid.halt();

	//     }  
	// }
	// if(analogRead(INFRAREDPIN) < 10){
	//     delay(5);
	//     if(analogRead(INFRAREDPIN) < 10){
	//         while(analogRead(INFRAREDPIN) < 10){
	//             // statement
	//         }
	//         flowCount++;
	//         showNum2(flowCount,2);
	//         openRecord();
	//     }
	// }

	// if(n == 2){
	//     uint16_t tDistance = filter(getDistance());
	//     talkNumNVC(tDistance, 1);
	//     delay(500);
	// }
	for(int i=0; i<10; i++){
	    
	    Serial.print(analogRead(TEMPERATUREPIN));
	    Serial.print(' ');
	    delay(1000);
	}
	Serial.println();

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

	pinMode(FANPIN, OUTPUT);
	digitalWrite(FANPIN, LOW);
	pinMode(BUTTONPIN, INPUT_PULLUP);

	pinMode(TRIGPIN, OUTPUT);
	pinMode(ECHOPIN, INPUT);

	pinMode(RECORDPIN, OUTPUT);
	digitalWrite(RECORDPIN, LOW);
}

void fastTrainer() {
	for(int i=0; i<3; i++){
	    writeLedSeven(8);
	    delay(1000);
	    writeLedSeven(0);
	    delay(1000);
	}
	uint16_t tmp = random(1000, 5000);
	delay(tmp);
	writeLedSeven(8);
	delay(50);
	writeLedSeven(0);
	for(int i=1; i<9; i++){
	    writeLedSeven(i);
	    delay(50);
	    if(!digitalRead(BUTTONPIN)){
	        delay(5);
	        if(!digitalRead(BUTTONPIN)){
	            break;
	        }
	    }
	}
	delay(5000);
	writeLedSeven(0);
	n = 0;
}
bool checkCardNumber(unsigned long sernum) {

	uint8_t cardId = 0;
	uint8_t i;
	//遍历搜索
	for(i = 0; i < cardCount; i++)
    {

        if(userId[i] == sernum)
        {
            cardId = i;
            showNum2(cardId, 2);
            openBuzzer();
            delay(500);
            closeBuzzer();
            //run the elec fan
            uint8_t stat = digitalRead(FANPIN);
            stat ^= 1;
            digitalWrite(FANPIN, stat);
            return true;
        }

    }
    //注册卡号
    if(i == cardCount && n == 1)
    {
        userId[i - 1] = sernum;
        userId[i] = 0;
        cardId = i - 1;
        //call beep 
        openBuzzer();
        delay(500);
        closeBuzzer();
        //show card id that stored
        //Serial.println(cardId);
        showNum2(cardId, 2);
        if(cardCount == 50)
        {
            cardCount = 1;
        }else {
            cardCount++;
        }
        return true;
        
    }else {
    	errorTube2();
    	return false;
    }
    
} 
void freshOut() {
	if(digitalRead(3)){
		delay(10);
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
    	talkNumNVC(tempVal, 0); 
    }
    Serial.println();
}
void errorNVC() {
	writeNVC(0x0c);
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
/*
@num: 0-999
@type: 0:temperature 1: 距离
 */
bool talkNumNVC(uint16_t num, uint8_t type) {
	uint8_t gewei = num%10;
    uint8_t shiwei = num%100/10;
    uint8_t baiwei = num/100;
    Serial.print("baiwei: ");
    Serial.print(baiwei);
    Serial.print(" shiwei: ");
    Serial.print(shiwei);
    Serial.print(" gewei: ");
    Serial.println(gewei);
    if(num > 999){
        errorNVC();
        return false;
    }

    if(type == 0){
		writeNVC(0x0c); //当前
	    delay(450);
	    writeNVC(0x0d);//温度
	    delay(1000);        
    } else {

    }
    if(num < 10)
    {
        writeNVC(gewei);
        delay(300);
    } else if(num == 10) {   
        writeNVC(0x0a);  //拾
        delay(300);
    } else if(num > 10 && num < 20) {
        writeNVC(0x0a);
        delay(400);
        writeNVC(gewei);
        delay(300);
    } else if(num >= 20){
    	
    	if(baiwei != 0){
        writeNVC(baiwei);
        delay(300);
        writeNVC(0x0b);
        delay(300);
	    } 
	    if(shiwei != 0){
	        writeNVC(shiwei);
	        delay(500);
	        writeNVC(0x0a);
	        delay(500);
	        if(gewei != 0){
	            writeNVC(gewei);
	            delay(300);
	        }
	    } else {
	    	if(gewei != 0){
	    		writeNVC(0x00);
	    		delay(300);
	    		writeNVC(gewei);   
	    	}
	    }    
    }

    if(type == 0){
    	writeNVC(0x0e); //摄氏度    
    }
    return true;
    
    // if(num < 10)
    // {
    //     writeNVC(gewei);
    //     delay(500);
    // }
    // else if(num == 10)
    // {   
    //     writeNVC(0x0a);  //拾
    //     delay(500);
    // }
    // else if(num > 10 && num < 20)
    // {
    //     writeNVC(0x0a);
    //     delay(500);
    //     writeNVC(gewei);
    //     delay(500);
    // }
    // else if(gewei == 0)
    // {
    //     writeNVC(shiwei);
    //     delay(500);
    //     writeNVC(0x0a);
    //     delay(500);
    // }
    // else
    // {
    //     writeNVC(shiwei);
    //     delay(500);
    //     writeNVC(0x0a); //拾
    //     delay(500);
    //     writeNVC(gewei);
    //     delay(500);
    // }  
    
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
void openFan() {
	digitalWrite(FANPIN, HIGH);
}
void closeFan() {
	digitalWrite(FANPIN, LOW);
}
void openRecord() {
	digitalWrite(RECORDPIN, HIGH);
	delay(20);
	digitalWrite(RECORDPIN, LOW);
}

uint16_t getDistance() {
	float distance = 0;
	digitalWrite(TRIGPIN, HIGH);
	delayMicroseconds(20);
	digitalWrite(TRIGPIN, LOW);
	distance = pulseIn(ECHOPIN, HIGH, 100000)/58.00;
	// Serial.println(distance);
	return (uint16_t)distance;
}

uint16_t filter(uint16_t m) {
	const int buffMax = 10;
	int i;
	Serial.print("m: ");
	Serial.print(m);
	if(filterFlag == 0)
	{
		filterFlag = 1;

		for(i = 0, filterSum = 0; i < buffMax; i++)
		{
		  filterBuffer[i] = m;
		  filterSum += filterBuffer[i];
		}
		return m;
	} else {
		filterSum -= filterBuffer[0];
		for(i = 0; i < (buffMax - 1); i++)
		{
		  filterBuffer[i] = filterBuffer[i + 1];
		}
		filterBuffer[9] = m;
		filterSum += filterBuffer[9];
		Serial.print(" Sum: ");
		Serial.println(filterSum);
		return  (filterSum / 10);
	  
	}
}