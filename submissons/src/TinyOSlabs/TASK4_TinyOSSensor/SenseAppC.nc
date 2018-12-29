/* FileName:SenseAppc.nc
 * Author:Hover
 * Data:2018.4.17
 * E-Mail:hover@hust.edu.cn
 * GitHub:HoverWings
 * Description: SensorApp
 * Reference:http://tinyos.stanford.edu/tinyos-wiki/index.php/Sensing
 */

configuration SenseAppC
{

}
implementation 
{
	components SenseC, MainC, LedsC, new TimerMilliC();
	components new DemoSensorC() as Sensor; // generic DemoSensorC component provides the Read<uint16_t> interface to SenseC.nc
	components new SensirionSht11C();
	components new HamamatsuS1087ParC();
	components SerialActiveMessageC as AM;
	// components new AMSenderC(AM_TEST_SCESE_MSG);


	SenseC.Boot -> MainC;
	SenseC.Leds -> LedsC;
	SenseC.Timer -> TimerMilliC;
	SenseC.readTemp -> SensirionSht11C.Temperature;
	SenseC.readHumidity -> SensirionSht11C.Humidity;
	SenseC.readPhoto -> HamamatsuS1087ParC; // ceramic package photodiodes with low dark current
	SenseC.Packet -> AM;
	SenseC.AMPacket -> AM;
	SenseC.Control -> AM;
	SenseC.AMSend -> AM.AMSend[AM_SENSEMSG];

}