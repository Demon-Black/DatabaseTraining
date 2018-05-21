from django.shortcuts import render
from django.shortcuts import render_to_response
from django.http import JsonResponse, HttpResponse
import socket

# Create your views here.
def index(requests):
	if requests.method == 'GET':
		return render_to_response('map.html')
	if requests.method == 'POST':
		query = requests.POST['query']
		client = socket.socket()
		client.connect(('127.0.0.1', 9000))
		client.send(bytes(query, encoding='utf-8'))
		reply = client.recv(1000)
		r = reply.decode('utf-8').split('\n')
		result = {'n': r[0], 'cars': []}
		for i in range(int(r[0])):
			m, p = r[i + 1].split(' ')
			id_, x, y = m.split('_')
			X = []
			Y = []
			p = p.split('_')
			for j in range(int((len(p) - 1) / 2)):
				X.append(p[2 * j])
				Y.append(p[2 * j + 1])
			car = {'id_': id_, 'x': x, 'y': y, 'X': X, 'Y': Y}
			print(car)
			result['cars'].append(car)
		client.shutdown(2)
		client.close()
		return JsonResponse(result)
