
/* Stephen Sinclair, 2008 */
/* Using libnifalcon and liblo. */

#include <falcon/core/FalconDevice.h>
#include <falcon/firmware/FalconFirmwareNovintSDK.h>
#include <falcon/util/FalconFirmwareBinaryNvent.h>
#include <falcon/kinematic/FalconKinematicStamper.h>

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <lo/lo.h>

libnifalcon::FalconDevice dev;

bool quit = false;

lo_server los = 0;
lo_address loaddr = 0;

boost::array<double, 3> force;

int force_handler(const char *path, const char *types, lo_arg **argv,
                  int argc, void *data, void *user_data)
{
    force[0] = argv[0]->f;
    force[1] = argv[1]->f;
    force[2] = argv[2]->f;
}

void on_quit(int)
{
    quit = true;
}

int main(int argc, char *argv[])
{
    const char *listen_port = "9001";
    const char *port = "9000";
    const char *host = "localhost";
    int i;

    for (i=1; (i+1) < argc; i+=2)
    {
        if (strcmp(argv[i],"--host")==0)
            host = argv[i+1];
        if (strcmp(argv[i],"--port")==0)
            port = argv[i+1];
        if (strcmp(argv[i],"--listen_port")==0)
            listen_port = argv[i+1];
    }

    printf("Initializing OSC server.\n");

    loaddr = lo_address_new(host, port);
    los = lo_server_new(listen_port, 0);
    if (!loaddr || !los) {
        printf("Could not initialize OSC server.\n");
        if (loaddr) lo_address_free(loaddr);
        if (los) lo_server_free(los);
        return -1;
    }

    lo_server_add_method(los, "/falcon/force", "fff", force_handler, 0);

    printf("Initializing device.\n");

    dev.setFalconFirmware<libnifalcon::FalconFirmwareNovintSDK>();
    if (!dev.open(0)) {
        printf("Couldn't open device 0.\n");
        return 1;
    }

    printf("Uploading firmware.\n");
    for (i=0; i<10; i++) {
        if (dev.getFalconFirmware()->loadFirmware(
                true, libnifalcon::NOVINT_FALCON_NVENT_FIRMWARE_SIZE,
                const_cast<uint8_t*>(libnifalcon::NOVINT_FALCON_NVENT_FIRMWARE))
            && dev.isFirmwareLoaded())
            break;
        printf(".");
        fflush(stdout);
    }

    if (i==10) {
        printf("Couldn't upload device firmware.\n");

        printf("Error Code: %d\n", dev.getErrorCode());
        if (dev.getErrorCode() == 2000)
            printf("Device Error Code: %d\n",
                   dev.getFalconComm()->getDeviceErrorCode());

        return 2;
    }

    dev.setFalconKinematic<libnifalcon::FalconKinematicStamper>();

    signal(SIGINT, on_quit);

    printf("Running.\n");
    while (!quit)
    {
        dev.runIOLoop();
		boost::array<double, 3> pos = dev.getPosition();
        lo_send(loaddr, "/falcon/position", "fff", pos[0], pos[1], pos[2]);
        lo_server_recv_noblock(los, 0);
        dev.setForce(force);
    }

    lo_address_free(loaddr);
    loaddr = 0;
    lo_server_free(los);
    los = 0;

    return 0;
}
