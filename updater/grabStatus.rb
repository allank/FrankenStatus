require 'json'
require "serialport"

# Setup the Serial port
port_str = "#{settings['serial_port']}"
baud_rate = 9600
data_bits = 8
stop_bits = 1
parity = SerialPort::NONE

sp = SerialPort.new(port_str, baud_rate, data_bits, stop_bits, parity)

#
# Start Kfm and Highveld...
#

output = `curl -X GET 'https://api.newrelic.com/v2/applications/#{settings['new_relic']['mus_app']}.json' -H 'X-Api-Key:#{settings['new_relic']['api_key']}'`

s = JSON.parse(output)

musHealth = s['application']['health_status']
musApdex = s['application']['application_summary']['apdex_score']

mRam = []
mCPU = []
s['application']['links']['servers'].each do |server|
	output = `curl -X GET 'https://api.newrelic.com/v2/servers/#{server}.json' -H 'X-Api-Key:#{settings['new_relic']['api_key']}'`
	t = JSON.parse(output)
	if t['server']['reporting'] == true
		mRam << t['server']['summary']['memory']
		mCPU << t['server']['summary']['cpu']
	end
end

# Get the average of each of the RAM and CPU for Musics
musRam = mRam.inject{ |sum, el| sum + el }.to_f / mRam.size
musCPU = mCPU.inject{ |sum, el| sum + el }.to_f / mCPU.size

#
# Start EWN
#

output = `curl -X GET 'https://api.newrelic.com/v2/applications/#{settings['new_relic']['mus_app']}.json' -H 'X-Api-Key:#{settings['new_relic']['api_key']}'`

s = JSON.parse(output)

ewnHealth = s['application']['health_status']
ewnApdex = s['application']['application_summary']['apdex_score']

eRam = []
eCPU = []
s['application']['links']['servers'].each do |server|
	output = `curl -X GET 'https://api.newrelic.com/v2/servers/#{server}.json' -H 'X-Api-Key:#{settings['new_relic']['api_key']}'`
	t = JSON.parse(output)
	if t['server']['reporting'] == true
		eRam << t['server']['summary']['memory']
		eCPU << t['server']['summary']['cpu']
	end
end

# Get the average of each of the RAM and CPU for EWN
ewnRam = eRam.inject{ |sum, el| sum + el }.to_f / eRam.size
ewnCPU = eCPU.inject{ |sum, el| sum + el }.to_f / eCPU.size

# TODO
# determine the status for each of the sites
# get the concurrent users on the site from Google Analytics
dataString = "1,#{ewnCPU},#{ewnRam},30,500,2,#{musCPU},#{musRam},40,300,2,#{musCPU},#{musRam},60,100;"

sp.puts dataString

sp.close
