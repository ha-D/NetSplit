#include "gum.h"

void gum_request_dhcp(){
	struct dhcp_args args;
	char* buf;
	int len;

	args.mac[0] = 0x01;
	args.mac[1] = 0x02;
	args.mac[2] = 0x03;
	args.mac[3] = 0x04;
	args.mac[4] = 0x05;
	args.mac[5] = 0x06;

	args.req_ip = 0;

	len = 0;
	buf = create_dhcp_discover(0, 0, 0, 0, &len, &args);
	//send_to_phys(buf, len);
}