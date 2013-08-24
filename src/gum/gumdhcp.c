#include "gum.h"

struct dhcp_trans* trans_list;

void gum_dhcp_discover(struct phys_dev* physd){
	struct dhcp_args args;
	struct dhcp_trans* trans;
	struct dhcphdr* dhcp;
	struct dhcpoption* option_param;
	struct dhcpoption_255 option_255;
	int offset = 0;
	char* buf;
	int len;

	args.mac[0] = 0x01;
	args.mac[1] = 0x02;
	args.mac[2] = 0x03;
	args.mac[3] = 0x04;
	args.mac[4] = 0x05;
	args.mac[5] = 0x06;

	args.req_ip = 0;
	args.option_offset = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct dhcphdr);

	len = 0;
	buf = create_dhcp_discover(&dhcp, 0, 0, 0, &len, &args);

	/* Options */
	offset = args.option_offset;

	// Requested Ip
	if(args.has_req_ip){
		struct dhcpoption_50 option_50;
		option_50.opt_num = 50;
		option_50.size = 4;
		option_50.req_ip = args.req_ip;
		dhcp_add_option(&buf, (struct dhcpoption*)&option_50, &offset, &len);
	}

	/* Params */
	option_param = (struct dhcpoption*) malloc(sizeof(struct dhcpoption) + 7);
	option_param->opt_num = 55;
	option_param->size = 7;
	*((char*)option_param + 1) = (char)DHCP_PARAM_SUBNETMASK;
	*((char*)option_param + 2) = (char)DHCP_PARAM_BROADCAST;
	*((char*)option_param + 3) = (char)DHCP_PARAM_TIMEOFFSET;
	*((char*)option_param + 4) = (char)DHCP_PARAM_ROUTER;
	*((char*)option_param + 5) = (char)DHCP_PARAM_DOMAIN;
	*((char*)option_param + 6) = (char)DHCP_PARAM_DNS;
	*((char*)option_param + 7) = (char)DHCP_PARAM_HOST;
	dhcp_add_option(&buf, option_param, &offset, &len);
	/* FREE IT SOMEWHERE!!! */

	option_255.opt_num = 255;
	option_255.size = 0;
	dhcp_add_option(&buf, (struct dhcpoption*)&option_255, &offset,	 &len);

	/* End Options */

	/* Add to dhcp transactions list */
	trans = (struct dhcp_trans*)malloc(sizeof(struct dhcp_trans));
	memset(trans, 0, sizeof(struct dhcp_trans));
	trans->next = trans_list;
	trans->state = GUM_DHCP_STATE_DISCOVER;
	trans->xid = dhcp->xid;
	trans->params_option = option_param;
	trans->physd = physd;
	trans_list = trans;

	printf("SENDING DHCP\n");
	send_to_phys(physd, buf, len);
}

void gum_dhcp_request(struct dhcp_trans* trans, struct dhcphdr* offer){
	struct dhcp_args args;
	struct dhcpoption_255 option_255;
	struct dhcpoption_54 option_54;
	struct dhcpoption_50 option_50;
	struct dhcphdr* dhcp;
	struct iphdr* ip;
	char* buf;
	int len;

	memcpy(args.mac, trans->physd->mac, 6);
	args.req_ip = 0;
	args.option_offset = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) + sizeof(struct dhcphdr);

	len = 0;
	buf = create_dhcp_request(&dhcp, 0, &ip, 0, &len, &args);

	if(trans->params_option != 0)
		dhcp_add_option(&buf, trans->params_option, &args.option_offset, &len);

	option_54.opt_num = 54;
	option_54.size 	= 4;
	option_54.server = ip->saddr;
	dhcp_add_option(&buf, (struct dhcpoption*)&option_54, &args.option_offset, &len);

	option_50.opt_num = 50;
	option_50.size 	= 4;
	option_50.req_ip = offer->your_addr;
	dhcp_add_option(&buf, (struct dhcpoption*)&option_50, &args.option_offset, &len);

	option_255.opt_num = 255;
	option_255.size = 0;
	dhcp_add_option(&buf, (struct dhcpoption*)&option_255, &args.option_offset,	 &len);

	trans->state = GUM_DHCP_STATE_REQUEST;

	send_to_phys(trans->physd, buf, len);
}


void gum_handle_dhcp(char* buf, int len){
	struct dhcphdr* dhcp;
	struct dhcp_trans* trans;
	struct dhcpoption option;
	int offset;
	int type;

	type = -1;

	dhcp = (struct dhcphdr*)(buf + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr));


	/* Find Transaction */
	trans = trans_list;
	while(trans != 0){
		if(trans->xid == dhcp->xid)
			break;
	}

	// Return if transaction not found
	if(trans == 0)
		return;

	// Read DHCP Type
	offset = 0;
	while(dhcp_read_option(buf, &option, &offset, len) == 0){
		if(option.opt_num == DHCP_OPTION_TYPE){
			struct dhcpoption_53* type_option;
			type_option = (struct dhcpoption_53*)&option;
			type = type_option->dhcp;
			break;
		}
	}

	if(type == -1)
		return;

	if(offset == DHCP_TYPE_OFFER && trans->state == GUM_DHCP_STATE_DISCOVER){
		gum_dhcp_request(trans, dhcp);
	}else if(offset == DHCP_TYPE_ACK && trans->state == GUM_DHCP_STATE_REQUEST){

	}
}

void init_gum_dhcp(){
	trans_list = 0;
}