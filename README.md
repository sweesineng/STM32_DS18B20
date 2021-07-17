<h1>DS18B20 digital thermometer</h1>

<p>STM32 I2C HAL & LL library for the 1-Wire DS18B20<p>
<p>These digital sensors provide 9 to 12-bit (configurable) temperature readings and High/Low temperature alarm</p>
<p>In parasite mode the sensor derives its power from the data line. Only two wires, Data and GND are required.</p>

<img src="Images/Parasite_Mode.jpg" width="50%" height="50%">

<p>This library need to used DwtDelay library as some waiting time need to be in microsecond</p>
<p>Tested on STM32H750 with 2x DS18B20 with alarm trigger</p>
<p>Data are store in data structure</p>

<img src="Images/DS18B20_Live_Exp.jpg" width="50%" height="50%">

<p>Comment/Suggestion are highly welcome!</p>