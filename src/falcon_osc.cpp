
/* Stephen Sinclair, 2008 */
/* Using libnifalcon and liblo. */

#include <falcon/core/FalconDevice.h>
#include <falcon/firmware/FalconFirmwareNovintSDK.h>
#include <falcon/util/FalconFirmwareBinaryNvent.h>
#include <falcon/util/FalconCLIBase.h>
#include <falcon/kinematic/FalconKinematicStamper.h>

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <lo/lo.h>

using namespace libnifalcon;
namespace po = boost::program_options;

bool stop = false;

void sigproc(int i)
{
	if(!stop)
	{
		stop = true;
		std::cout << "Quitting" << std::endl;
	}
	else exit(0);
}

boost::array<double, 3> force;

int force_handler(const char *path, const char *types, lo_arg **argv,
                  int argc, void *data, void *user_data)
{
    force[0] = argv[0]->f;
    force[1] = argv[1]->f;
    force[2] = argv[2]->f;
}

class FalconOSC : public FalconCLIBase
{
public:
	void addOptions(int value)
	{
		FalconCLIBase::addOptions(value);
		po::options_description osc("OSC Options");
		osc.add_options()
			("osc_host", "Host address")
			("osc_out_port", "Port for OSC message output")
			("osc_in_port", "Port for OSC message input");
		m_progOptions.add(osc);
	}

	bool parseOptions(int argc, char** argv)
	{
		if(!FalconCLIBase::parseOptions(argc, argv)) return false;
		m_falconDevice->setFalconKinematic<libnifalcon::FalconKinematicStamper>();

		const char *listen_port = "9001";
		const char *port = "57120";
		const char *host = "localhost";
		lo_server los = 0;
		lo_address loaddr = 0;

		if(m_varMap.count("osc_host"))
		{
			host = m_varMap["osc_host"].as<std::string>().c_str();
		}
		if(m_varMap.count("osc_out_port"))
		{
			port = m_varMap["osc_out_port"].as<std::string>().c_str();
		}
		if(m_varMap.count("osc_in_port"))
		{
			listen_port = m_varMap["osc_in_port"].as<std::string>().c_str();
		}

		loaddr = lo_address_new(host, port);
		los = lo_server_new(listen_port, 0);
		if (!loaddr || !los) {
			printf("Could not initialize OSC server.\n");
			if (loaddr) lo_address_free(loaddr);
			if (los) lo_server_free(los);
			return false;
		}

		lo_server_add_method(los, "/falcon/force", "fff", force_handler, 0);

		printf("Running.\n");
		while (!stop)
		{
			if(m_falconDevice->runIOLoop())
			{
				boost::array<double, 3> pos = m_falconDevice->getPosition();
				lo_send(loaddr, "/falcon/position", "fff", pos[0], pos[1], pos[2]);
				lo_server_recv_noblock(los, 0);
				m_falconDevice->setForce(force);			
			}
		}

		lo_address_free(loaddr);
		loaddr = 0;
		lo_server_free(los);
		los = 0;


		return true;
	}
};
	
int main(int argc, char** argv)
{

	signal(SIGINT, sigproc);
#ifndef WIN32
	signal(SIGQUIT, sigproc);
#endif
	
	FalconOSC f;
	f.addOptions(FalconOSC::DEVICE_OPTIONS | FalconOSC::COMM_OPTIONS | FalconOSC::FIRMWARE_OPTIONS);
	f.parseOptions(argc, argv);
	return 0;
}
