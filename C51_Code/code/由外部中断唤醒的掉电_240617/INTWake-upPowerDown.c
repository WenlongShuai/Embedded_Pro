#include <reg51.h>
#include <intrins.h>

sbit Begin_LED = P2^0;            //Bepin-LED indicator indicates system start-up
unsigned char Is_Power_Down = 0;  //Set this bit before go into Power-down mode
sbit Is_Power_Down_LED_INT0 = P2^1;      //Power-Down wake-up LED indicator on INTO
sbit Not_Power_Down_LED_INT0 = P2^5;     //Not Power-Down wake-up LED indicator on INTO
sbit Is_Power_Down_LED_INT1 = P2^6;     //Power-Down wake-up LED indicator on INT1
sbit Not_Power_Down_LED_INT1 = P2^7;      //Not Power-Down wake-up LED indicator on INT1
sbit Power_Down_Wakeup_Pin_INT0 = P3^2;  //Power-Down wake-up pin on INTO
sbit Power_Down_Wakeup_Pin_INT1 = P3^3;   //Power-Down wake-up pin on INT1
sbit Normal_Work_Flashing_LED = P2^2;      //Nornal work LED indicator

void Normal_Work_Flashing();
void INT_System_init();
void INTO_Routine();
void INT1_Routine();

int main()
{
	unsigned char j = 0;
	unsigned char wakeup_counter = 0;  //clear interrupt wakeup counter variable wakeup_counter
	Begin_LED = 0;  //system start-up LED
	
	INT_System_init();  //Interrupt system initialization
	
	while(1)
	{
		P2 = wakeup_counter;
		wakeup_counter++;
		for(j=0; j<2; j++)
		{
			Normal_Work_Flashing(); //System normal work
		}

		Is_Power_Down = 1;   //Set this bit before go into Power-down mode
		PCON = 0x02;   //after this instruction, MCU will be in power-down made
		//external clock stop
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}

	return 0;
}


void INT_System_init()
{
	IT0 = 0;  //External interrupt O, low electrical level triggered
//	ITO = 1;  //External interrupt O, negative edge triggered
	EX0 = 1;     //enable external interrupt 0

	IT1 = 0;  //External interrupt 1, low electrical level triggered
	//IT1 = 1;  //External interrupt 1, negative edge triggered
	EX1 = 1;   //Enable external interrupt 1

	EA = 1; //Set Global Enable bit

}
 
void INTO_Routine() interrupt 0
{
	if(Is_Power_Down == 1)
	{
		Is_Power_Down = 0;  //Power-Down wakeup on INT0
		Is_Power_Down_LED_INT0 =0;  //open external interrupt 0 Power-Down wake-up LED indicator
		while(Power_Down_Wakeup_Pin_INT0 == 0);  //wait higher
		Is_Power_Down_LED_INT0 = 1;  //close external interrupt 0 Power-Down wake-up LED indicator
	}
	else
	{
		Not_Power_Down_LED_INT0 = 0;  //open external interrupt 0 normal work LED
		while(Power_Down_Wakeup_Pin_INT0 == 0);  //wait higher
		Not_Power_Down_LED_INT0 = 1;   //close external interrupt 0 normal work LED
	}
}

void INT1_Routine() interrupt 2
{
	if(Is_Power_Down == 1)
	{
		Is_Power_Down = 0;  //Power-Down wakeup on INT1
		Is_Power_Down_LED_INT1 =0;  //open external interrupt 1 Power-Down wake-up LED indicator
		while(Power_Down_Wakeup_Pin_INT1 == 0);  //wait higher
		Is_Power_Down_LED_INT1 = 1;  //close external interrupt 1 Power-Down wake-up LED indicator
	}
	else
	{
		Not_Power_Down_LED_INT1 = 0;  //open external interrupt 1 normal work LED
		while(Power_Down_Wakeup_Pin_INT1 == 0);  //wait higher
		Not_Power_Down_LED_INT1 = 1;   //close external interrupt 1 normal work LED
	}
}


void delay()
{
	unsigned int j=0;
	unsigned int  i = 0;
	for(i=0; i<2;i++)
	{
		for(j=0; j<=30000; j++)
		{
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();
		}
	}
}


void Normal_Work_Flashing()
{
	Normal_Work_Flashing_LED = 0;
	delay();
	Normal_Work_Flashing_LED = 1;
	delay();
}
