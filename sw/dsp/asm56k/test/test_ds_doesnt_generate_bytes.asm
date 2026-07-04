
		org 	x:$10
data1:	ds		10
		ds		1
data2:	dc		0.5

		org 	y:$10
data3:	dc		0.99999999
data4:	ds		10
		ds		1

		org 	p:$40
		ds		1
		add		x0,a


		org 	l:$0
ldata1:	ds		10
ldata2:	ds		1
ldata3:	dc		100


		org		x:$200
		dc		ldata1
		dc		ldata2
		dc		ldata3


