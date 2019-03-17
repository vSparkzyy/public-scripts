=begin

	Single Host:Port Checker By vSparkzyy;
	Usage: ruby file.rb host port
	
=end

require 'socket' 
host = ARGV[0]
port = ARGV[1]
print "\e[0;31m[\e[0;36mPort\e[0;31m-\e[0;36mChecker\e[0;31m]\x1b[37m Scanner Started... \n"
sleep 1
print "\e[0;31m[\e[0;36mPort\e[0;31m-\e[0;36mChecker\e[0;31m]\x1b[37m Setting Up Socket... \n"
sleep 1 
print "\e[0;31m[\e[0;36mPort\e[0;31m-\e[0;36mChecker\e[0;31m]\x1b[37m Attempting To Connect To Socket... #{host}\x1b[32m:\x1b[37m#{port} \n"
sleep 1

def gscanner(host, port)
    begin
        nigsock = TCPSocket.open(host, port)
		print "\e[0;31m[\e[0;36mPort\e[0;31m-\e[0;36mChecker\e[0;31m]\x1b[37m Host: #{host} \x1b[32mOpen\x1b[37m On Port: #{port} \n"
    rescue
        print "\e[0;31m[\e[0;36mPort\e[0;31m-\e[0;36mChecker\e[0;31m]\x1b[37m Host: #{host} \x1b[31mClosed\x1b[37m On Port: #{port} \n"
        exit
    end
end
gscanner(host, port)