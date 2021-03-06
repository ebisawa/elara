
kernel_call	equ	80h

relcpu		equ	0
exec		equ	1
spawn		equ	2
getpid		equ	3
getppid		equ	4
getname		equ	5
getstat		equ	6
getprio		equ	7
setprio		equ	8
getwait1	equ	9
getwait2	equ	10
getstack	equ	11
getmuse		equ	12
getargv		equ	13
kill		equ	14
exit		equ	15
disp		equ	16
shutdown	equ	17
adddrv		equ	18
deldrv		equ	19
adddev		equ	20
deldev		equ	21
getuid		equ	22
setuid		equ	23
vsprintf	equ	24
system		equ	25
firstpid	equ	26
nextpid		equ	27
sleep		equ	28
wakeup		equ	29
wait		equ	30
raise		equ	31
malloc		equ	32
free		equ	33
xmalloc		equ	34
xfree		equ	35
open		equ	36
close		equ	37
read		equ	38
write		equ	39
seek		equ	40
ioctl		equ	41
getstdio	equ	42
setstdio	equ	43
mkdir		equ	44
rmdir		equ	45
time		equ	46
localtime	equ	47
ctime		equ	48
getpflag	equ	49
setpflag	equ	50
fileno		equ	51
sendmsg		equ	52
recvmsg		equ	53
checkmsg	equ	54
remove		equ	55
close2		equ	56
procsw		equ	57
rename		equ	58
iolimit		equ	59
mktime		equ	60

