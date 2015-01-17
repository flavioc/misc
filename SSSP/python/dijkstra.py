#!/usr/bin/python

from priodict import priorityDictionary
import sys
import struct

def Dijkstra(G,start,end=None):
	D = {}	# dictionary of final distances
	P = {}	# dictionary of predecessors
	Q = priorityDictionary()   # est.dist. of non-final vert.
	Q[start] = 0

	for v in Q:
		D[v] = Q[v]
		if v == end: break

		for w in G[v]:
			vwLength = D[v] + G[v][w]
			if w in D:
				if vwLength < D[w]:
					raise ValueError, "Dijkstra: found better path to already-final vertex"
			elif w not in Q or vwLength < Q[w]:
				Q[w] = vwLength
				P[w] = v

	return (D,P)

if len(sys.argv) != 3:
   print "Usage: dijkstra.py <file> <fraction>"
   sys.exit(1)

graph = {}
f = open(sys.argv[1], 'rb')
fraction = int(sys.argv[2])
print "Using fraction", fraction

while f:
	d1 = f.read(4)
	if d1 is None or d1 == '':
		break
	n1 = struct.unpack("I", d1)[0]
	d2 = f.read(4)
	d3 = f.read(4)
	n2 = struct.unpack("I", d2)[0]
	n3 = struct.unpack("I", d3)[0]
	try:
		neighbors = graph[n2]
		neighbors[n1] = n3
	except KeyError:
		graph[n2] = {n1: n3}
	try:
		neighbors = graph[n1]
	except KeyError:
		graph[n1] = {}

for node, _ in graph.iteritems():
	if node % fraction == 0:
		Dijkstra(graph, node)
