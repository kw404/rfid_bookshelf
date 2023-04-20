# PN5180-Library

Arduino Uno / Arduino ESP-32 library for PN5180-NFC Module from NXP Semiconductors

![PN5180-NFC module](./doc/PN5180-NFC.png)
![PN5180 Schematics](./doc/FritzingLayout.jpg)

Release Notes:

Version 1.5 - xx.xx.2019

	* ISO-14443 protocol, basic support for Mifaire cards
	* Low power card detection
	* handle transceiveCommand timeout

Version 1.4 - 13.11.2019

	* ICODE SLIX2 specific commands, see https://www.nxp.com/docs/en/data-sheet/SL2S2602.pdf
	* Example usage, currently outcommented

Version 1.3 - 21.05.2019

	* Initialized Reset pin with HIGH
	* Made readBuffer static
	* Typo fixes
	* Data type corrections for length parameters

Version 1.2 - 28.01.2019

	* Cleared Option bit in PN5180ISO15693::readSingleBlock and ::writeSingleBlock

Version 1.1 - 26.10.2018

	* Cleanup, bug fixing, refactoring
	* Automatic check for Arduino vs. ESP-32 platform via compiler switches
	* Added open pull requests
	* Working on documentation

Version 1.0.x - 21.09.2018

	* Initial versions
