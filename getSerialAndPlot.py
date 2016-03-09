import serial
import select
import numpy as np

from sys import stdin
from matplotlib import pyplot as plt

serialPort = '/dev/ttyACM0'
baudRate = 9600

ser = serial.Serial(serialPort, baudRate)

plt.ion() # set plot to animated

desiredRPM = [0] * 50
realRPM = [0] * 50
correctedRPM = [0] * 50

error = [0] * 50

integralError = [0] * 50

ax1 = plt.axes()

plt.figure(1).subplots_adjust(hspace = .5)

plt.subplot(311).set_title("RPM's")
dRPM, = plt.plot(desiredRPM)
rRPM, = plt.plot(realRPM)
cRPM, = plt.plot(correctedRPM)

plt.subplot(312).set_title("Error")
err, = plt.plot(error)

plt.subplot(313).set_title("Integral Error")
inErr, = plt.plot(integralError)

plt.ylim([0,0])

rawData = ''
rawDataSplitted = []

read = False

print ''

while True:

	if select.select([ser,],[],[],0.0)[0]:
		read = True
		rawData = ser.readline().rstrip()	# llegim accel, gyro i temp
		rawDataSplitted = rawData.split (",")

		#if len (rawDataSplitted) != 7:
		print rawData
	else:
		read = False

	while select.select([stdin,],[],[],0.0)[0]:
		ser.write(stdin.readline())

	if len(rawDataSplitted) == 5 and read:

		desiredRPM.append(float (str(rawDataSplitted[0])))
		realRPM.append(float (str(rawDataSplitted[1])))
		correctedRPM.append(float (str(rawDataSplitted[2])))

	 	error.append(float (str(rawDataSplitted[3])))

		integralError.append(float (str(rawDataSplitted[4])))

		plt.subplot(311)
		plt.ylim([\
				min (min (desiredRPM), min (realRPM), min (correctedRPM)) - 5, \
				max (max (desiredRPM), max (realRPM), max (correctedRPM)) + 5])

		plt.subplot(312)
		plt.ylim([min (error) - 3, max (error) + 3])

		plt.subplot(313)
		plt.ylim([min (integralError) - 3,max (integralError)+3])

		del desiredRPM [0]
		del realRPM [0]
		del correctedRPM [0]

		del error [0]
		del integralError [0]

		dRPM.set_xdata(np.arange(len(desiredRPM)))
		dRPM.set_ydata(desiredRPM)

		rRPM.set_xdata(np.arange(len(realRPM)))
		rRPM.set_ydata(realRPM)

		cRPM.set_xdata(np.arange(len(correctedRPM)))
		cRPM.set_ydata(correctedRPM)


		err.set_xdata(np.arange(len(error)))
		err.set_ydata(error)

		inErr.set_xdata(np.arange(len(integralError)))
		inErr.set_ydata(integralError)

		plt.draw()
