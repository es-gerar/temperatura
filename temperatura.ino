// PINES A CONFIGURAR
#define ONE_WIRE_BUS 2


int maxTemp = 30;
// TEMPERPATURA //

#include <OneWire.h>
#include <DallasTemperature.h>
// Data wire is plugged into port 2 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// arrays to hold device address
DeviceAddress insideThermometer;

///////// DISPLAY LCD PANTALLA DE CRISTAL LÍQUIDO ///////////////////

#include <LiquidCrystal.h>
const int rs = 8;
const int en = 9;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;
const int BL = 10;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Assign address manually. The addresses below will beed to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  // Note that you will need to use your specific address here
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

  // Method 1:
  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0");

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them. It might be a good idea to
  // check the CRC to make sure you didn't get garbage. The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();


  // LCD

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);


  // Print a message to the LCD.
  lcd.print("TEMPERATURA");

  //lcd.blink();

}

// function to print the temperature for a device
float printTemperature(DeviceAddress deviceAddress)
{
  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return -128;
  }
  Serial.print("Temp C: ");
  // Serial.print(tempC);
  //Serial.print(" Temp F: ");
  //  float tempActual = DallasTemperature::toFahrenheit(tempC);
  Serial.println(tempC);
  return tempC;
}


void loop(void)
{

  static bool alarm = 0;

  if (alarm == 1)
  {
    Serial.print(" ALARMA ");
    static unsigned long lastDisplay;
    if (millis() > lastDisplay + 500) {
      lastDisplay = millis();
      static int displayState;
      if (displayState == 1) {
        displayState = 0;
        // Turn off the display:
        lcd.noDisplay();
        Serial.print("*");
      }
      else
      {
        // Turn on the display:
        lcd.display();
        displayState = 1;
        Serial.print("o");
      }
    }


  }
  else
  {
    lcd.display();

  }



  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  lcd.setCursor(0, 1);
  static float tempAnterior;
  float tempActual = printTemperature(insideThermometer);
  if (tempActual != tempAnterior)
  {
    //Actualiza temperatura en LCD
    tempAnterior = tempActual;
    lcd.print(tempActual);
  }
  if (tempAnterior > maxTemp)
  {
    alarm = 1;
  }
  else
    alarm = 0;

}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
