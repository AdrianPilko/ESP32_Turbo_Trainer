# ESP32_Turbo_Trainer
ESP32_Turbo_Trainer is an upgrade to ArduinoTurboTrainer as the ESP32 dev board supports wifi and web interface.

Background
==========
A turbo trainer is used to convert temporarily a normal bicycle into a static exercise bike

The turbo trainer used here came from Decathlon (called InRide) had just a magnetic resistance to make it harder or easier to pedal. My upgrade to this involved removing the magnetic resistance and installing a brushless outrigger motor/generator, attached to a board with a bank of 4 and 8 ohm power resistors and a 3 phase full bridge rectifier. The resistor bank is switch in and out using a relay block controlled from the ESP32 microcontroller.

Todo
====
1) add a wheel speed sensor, this will open up a few extra things like total distance travelled
2) change power calculation to use amp sensor, rather than measured voltage and in circuit resistnance
3) make the max power history only update the max for the run if greater than the smallest, and also sort the list in descending order
4) improve web page with colour, and extra features like a side scrolling graphic showing a bike riding along a road which changes in gradient with the resistance, and the speed the "scenary" move at determined by rear wheel rpm... basically a poor mans zwift lol

Parts list
==========
power resistors:  https://www.ebay.co.uk/itm/5pcs-50W-Aluminium-Shell-Clad-Power-Wirewound-Resistor-1-2-3-5-6-8-10-Ohm-UK-/114415360717?_trksid=p2349624.m46890.l49292

12v to 5v regulator:  https://www.ebay.co.uk/itm/164626493544

https://www.ebay.co.uk/itm/352732783398

ESP32 microcontroller: https://www.amazon.co.uk/dp/B071P98VTG/ref=redir_mobile_desktop?_encoding=UTF8&aaxitk=9oCOAybRJhEG4qh63HSXHQ&hsa_cr_id=3943629740502&pd_rd_plhdr=t&pd_rd_r=706b7a75-f591-4c99-bddd-f474437a5b12&pd_rd_w=rtXEG&pd_rd_wg=6NJrS&ref_=sbx_be_s_sparkle_mcd_asin_0_img

Relay Block:
https://www.amazon.co.uk/Shield-Module-Channel-Control-Optocoupler/dp/B07QLBZPK2/ref=sr_1_18?dchild=1&keywords=four+way+relay+block+arduino&qid=1612805836&s=industrial&sr=1-18


future upgrades:
     - connection to ANT+ to integrate to zwift if possible
     - rear wheel rpm to data log equivallent distance travelled, and view on a display in real time
	 - downloadable power data, and storage.
   
