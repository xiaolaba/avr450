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
