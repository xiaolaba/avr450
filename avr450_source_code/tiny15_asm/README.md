# avr450

## how to assemble the asm files.
Ubuntu 20.04, install AVRA first and then 'make all' 
```
sudo apt update
sudo apt upgrade
sudo apt-get install avrdude avra
cd $HOME
git clone https://github.com/xiaolaba/avr450
cd $HOME/avr450/avr450_source_code/tiny15_asm
make clean
make all

```





Atmel / microchip pulled off this application note and source code, and no longer available for download, but why.  

this is collection from the internet, however do not remember where pulled.  

it is a reference design for li-polymer battery charging with code/hardware design.  

![avr450_tiny15.JPG](avr450_tiny15.JPG)  

![tiny_charger.JPG](tiny_charger.JPG)  

[SI4425, P-MOS](SI4425_P-MOS.pdf)  

[LSM345_Schottky_diode](LSM345_MicrosemiCorporation.pdf)
