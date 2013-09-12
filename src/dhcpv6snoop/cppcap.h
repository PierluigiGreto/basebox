/*
 * cppcap.h
 *
 *  Created on: 11.09.2013
 *      Author: andreas
 */

#ifndef CPPCAP_H_
#define CPPCAP_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#include <pcap.h>
#include <pthread.h>
#ifdef __cplusplus
}
#endif

#include <string>
#include <exception>

#include "cdhcpmsg_relay.h"

namespace rutils
{

class ePcapBase : public std::exception {};
class ePcapThreadFailed : public ePcapBase {};

class cppcap
{
	pthread_t			tid;
	bool 				keep_going;

	std::string 		devname;
	pcap_t 				*pcap_handle;
	struct bpf_program	bpfp;
	char 				pcap_errbuf[PCAP_ERRBUF_SIZE];

public:

	cppcap();

	~cppcap();

public:

	void
	start(std::string const& devname);

	void
	stop();

private:

	static void*
	run_pcap_thread(void *arg);

	void*
	do_capture();
};

}; // end of namespace

#endif /* CPPCAP_H_ */