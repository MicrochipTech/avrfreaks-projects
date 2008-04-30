<?php


if(isset($_GET['command']))
{
	$command = $_GET['command'];
}

$serial = new phpSerial();

$serial->setDevice("/dev/ttyS0");

$serial->setParam('baud_base', '9600');
$serial->setParam('divisor', '1');

/*
1=Full Up
2=Up
3=Level
4=Down
5=Full Down
6=Full Right
7=45 Right
8=Center
9=45 Left
0=Full Left
a=Auto Pan
*/

switch($command)
{
	case 'fullup':
		$serial->sendMessage('1');
		$serial->flush();
		break;
	
	case 'up':
		$serial->sendMessage('2');
		$serial->flush();
		break;
	
	case 'level':
		$serial->sendMessage('3');
		$serial->flush();
		break;
		
	case 'down':
		$serial->sendMessage('4');
		$serial->flush();
		break;
	
	case 'fulldown':
		$serial->sendMessage('5');
		$serial->flush();
		break;

	case 'fullright':
		$serial->sendMessage('6');
		$serial->flush();
		break;

	case '45right':
		$serial->sendMessage('7');
		$serial->flush();
		break;

	case 'center':
		$serial->sendMessage('8');
		$serial->flush();
		break;

	case '45left':
		$serial->sendMessage('9');
		$serial->flush();
		break;

	case 'fullleft':
		$serial->sendMessage('0');
		$serial->flush();
		break;

	case 'autopan':
		$serial->sendMessage('a');
		$serial->flush();
		break;
}

?>