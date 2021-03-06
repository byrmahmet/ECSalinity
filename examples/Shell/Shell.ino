/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c
 */

#include <ECSalinity.h>
#include "CLI.h"

/*
   This allows you to run various functions on a command-line like interface.
   Enter `config` to see the configuration of the device. Type 'ec' to get a
   measurement in mS, 'data' to see all the data members of the class like uS,
   and S, 'temp' to read the temperature, 'sal' to use salinity parameters for
   a measurment.

   To use an unconfigured device:
   1. k <K constant value eg. 1.0>
   2. dry (while probe is dry)
   3. cal <calibration solution in mS eg 2.77>
   4. ec

   Determine K of a custom probe:
   1. calk <mS of calibration solution>
   2. ec

   Dual Point Calibration:
   1. k <K constant value eg. 1.0>
   2. dry (while probe is dry)
   3. low <calibration solution in mS eg 0.5>
   4. high <calibration solution in mS eg 2.77>
   5. dp 1

   Using Temperature compensation:
   1. reset
   2. tc 1 25 <to adjust readings as if they were at 25 C>
   3. (recalibrate all values using `dry`, `cal`, `low`, `high`)
   4. ec

   Measure saltwater with a configured device:
   1. sal

 */

CLI_COMMAND(reset);  // 'reset' reset all calibration configuration
CLI_COMMAND(temp);   // 'temp' measure temperature
CLI_COMMAND(cal);    // 'cal 2.77' calibrate the offset of the probe using mS of the solution
CLI_COMMAND(k);      // 'k 10.0' sets the K value of the probe being used
CLI_COMMAND(ec);     // 'ec' starts an EC measurement, returns mS
CLI_COMMAND(low);    // 'low 1.0' calibrates the low dual-point calibration settings using mS as solution
CLI_COMMAND(high);   // 'high 10.0'calibrates the high dual-point calibration settings using mS as solution
CLI_COMMAND(config); // 'config' shows configuration information
CLI_COMMAND(tc);     // 'tc 0/1 25' temperature compensation; first argument to use it, second the constant to use
CLI_COMMAND(dp);     // 'dp 0/1' to use dual-point calibration or not
CLI_COMMAND(dry);    // 'dry' determines the probe reading when dry
CLI_COMMAND(sal);    // 'sal' starts a salinity measurement
CLI_COMMAND(data);   // 'data' shows all the data from the latest 'ec' or 'sal' call made
CLI_COMMAND(i2c);    // 'ic2 62' changes the i2c address to 62

EC_Salinity _ec;

void setup()
{
  Serial.begin(9600);
  delay(1000); Serial.println();
  CLI.setDefaultPrompt("> ");
  CLI.onConnect(config);

  CLI.addCommand("reset",  reset);
  CLI.addCommand("temp",   temp);
  CLI.addCommand("cal",    cal);
  CLI.addCommand("k",      k);
  CLI.addCommand("ec",     ec);
  CLI.addCommand("low",    low);
  CLI.addCommand("high",   high);
  CLI.addCommand("config", config);
  CLI.addCommand("tc",     tc);
  CLI.addCommand("dp",     dp);
  CLI.addCommand("sal",    sal);
  CLI.addCommand("data",   data);
  CLI.addCommand("i2c",    i2c);
  CLI.addCommand("dry",    dry);

  CLI.addClient(Serial);
}

void loop()
{
  CLI.process();
}

CLI_COMMAND(reset) {
  _ec.reset();
  return 0;
}

CLI_COMMAND(temp) {
  _ec.measureTemp();
  dev->print("C/F: "); dev->print(_ec.tempC);
  dev->print(" /  "); dev->println(_ec.tempF);
  return 0;
}

CLI_COMMAND(cal) {
  if (argc == 2) {
    _ec.calibrateProbe(atof(argv[1]), _ec.tempCoefEC);
  }

  dev->print("offset: ");
  dev->print(_ec.getCalibrateOffset(), 4);
  return 0;
}

CLI_COMMAND(k) {
  if (argc == 2) {
    _ec.setK(atof(argv[1]));
  }
  dev->print("K: ");
  dev->print(_ec.getK(), 4);
  return 0;
}

CLI_COMMAND(ec) {
  // for (;;) {
  _ec.measureEC();
  dev->print("mS: ");
  dev->print(_ec.mS, 4);

  // dev->println();
  // delay(1000);
  // }
  return 0;
}

CLI_COMMAND(low) {
  if (argc == 2) {
    _ec.calibrateProbeLow(atof(argv[1]), _ec.tempCoefEC);
  }

  dev->print("low reference / read: "); dev->print(_ec.getCalibrateLowReference(), 4);
  dev->print(" / "); dev->print(_ec.getCalibrateLowReading(), 4);
  return 0;
}

CLI_COMMAND(high) {
  if (argc == 2) {
    _ec.calibrateProbeHigh(atof(argv[1]), _ec.tempCoefEC);
  }

  dev->print("high reference / read: "); dev->print(_ec.getCalibrateHighReference(), 4);
  dev->print(" / "); dev->print(_ec.getCalibrateHighReading(), 4);
  return 0;
}

CLI_COMMAND(config) {
  dev->println("Config:");
  dev->print("  K: "); dev->println(_ec.getK(), 4);
  dev->print("  offset: "); dev->println(_ec.getCalibrateOffset(), 4);
  dev->print("  dry: "); dev->println(_ec.getCalibrateDry(), 4);
  dev->print("  dual point: "); dev->println(_ec.usingDualPoint(), 4);
  dev->print("  low reference / read: "); dev->print(_ec.getCalibrateLowReference(), 4);
  dev->print(" /  "); dev->println(_ec.getCalibrateLowReading(), 4);
  dev->print("  high reference / read: "); dev->print(_ec.getCalibrateHighReference(), 4);
  dev->print(" / "); dev->println(_ec.getCalibrateHighReading(), 4);
  dev->print("  temp. compensation: "); dev->println(
    _ec.usingTemperatureCompensation());
  dev->print("    constant: "); dev->println(_ec.getTempConstant());
  dev->print("  version: "); dev->print(_ec.getVersion(), HEX);
  dev->println();
  dev->printPrompt();
  return 0;
}

CLI_COMMAND(tc) {
  if (argc == 2) {
    _ec.useTemperatureCompensation(atof(argv[1]));
  }

  if (argc == 3) {
    _ec.useTemperatureCompensation(atof(argv[1]));
    _ec.setTempConstant(atof(argv[2]));
  }

  dev->print("temp. compensation: ");
  dev->println(_ec.usingTemperatureCompensation());
  dev->print("   constant: ");
  dev->print(_ec.getTempConstant());
  return 0;
}

CLI_COMMAND(dp) {
  if (argc == 2) {
    _ec.useDualPoint(atof(argv[1]));
  }

  dev->print("dual point: ");
  dev->print(_ec.usingDualPoint());
  return 0;
}

CLI_COMMAND(sal) {
  _ec.measureSalinity();
  dev->print("salinity PSU / PPT / PPM: "); dev->print(_ec.salinityPSU, 2);
  dev->print(" / "); dev->print(_ec.salinityPPT, 2);
  dev->print(" / "); dev->print(_ec.salinityPPM, 2);
  return 0;
}

CLI_COMMAND(data) {
  dev->print("S: "); dev->println(_ec.S, 4);
  dev->print("mS: "); dev->println(_ec.mS, 4);
  dev->print("uS: "); dev->println(_ec.uS);
  dev->print("TDS 500/640/700: "); dev->print(_ec.PPM_500);
  dev->print(" / "); dev->print(_ec.PPM_640);
  dev->print(" / "); dev->println(_ec.PPM_700);
  dev->print("salinity PSU / PPT: "); dev->print(_ec.salinityPSU, 4);
  dev->print(" / "); dev->print(_ec.salinityPPT, 4);
  return 0;
}

CLI_COMMAND(i2c) {
  if (argc == 2) {
    _ec.setI2CAddress(atof(argv[1]));
  }

  return 0;
}

CLI_COMMAND(dry) {
  _ec.calibrateDry();
  dev->print("dry: "); dev->print(_ec.getCalibrateDry(), 4);

  return 0;
}
