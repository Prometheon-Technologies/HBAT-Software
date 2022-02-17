# HBAT-Software Private repo

Custom Software Stack files are located in lib and src. 

main.cpp runs the control script, HMSmain library does the heavy lifting. 

**LIB**

[HMSmain Library](HMSmain_Back_End/lib/HMSmain)

**SRC**

[HMSmain](HMSmain_Back_End/src)

# Note to Voxdale

For this project, we need to map the discharge curve of the system, in order to present the user with an accurate representation of charge and discharge states. 
The discharge curve of our battery system is not linear, therefor a basic linear percentage calculation will not work. 

Below i have one strategy, this strategy is simple to implement, however has low accuracy and could be prone to error. 


Take minimum voltage reading of just before esp32 stops functioning, then 
take the max voltage reading of a freshly charged battery (or input source).
The product of this subtraction is then divided by the realtime measured voltage
and multipled by 100. 


```python

def percentage_calc:
	Vmin # minimum possible voltage for the load to function
	Vmax # maximum voltage of the battery when in a fully charged state
	Vreal # real-time measured votlage for the whole system (or the sum of all series connected cells individual readings)
	Vmin - Vmax = X

	x = (X / Vreal) * 100
	return x
```
An alternative approach is to correctly map the voltage readings. Such as the example below, however this must be done in real-time to present accurate data to the user, and for the software to take correct actions based on the data of this curve. 

**Mapping Bezier curve to voltage readings**

According to this paper: [PEMFC Discharge Curve](https://github.com/Prometheon-Technologies/HBAT-Software/blob/main/sustainability-12-08127-v2.pdf)
the discharge curve matches a very predictable Bezier curve. 

![Curve](/imgs/curve.png)

![Curve+Math](/imgs/curve%2Bmath.png)

Currently, i am not able to correctly implement this approach into the software stack. Is Voxdale capable of doing this? Does voxdale have an alternative approach in mind? 

My current thoughts were to map the votlage readings to an array, and set that array equal to the results of a mapping function containing the formula for the curve we wish to fit our data to. 
