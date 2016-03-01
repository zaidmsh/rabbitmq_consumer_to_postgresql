#!/usr/bin/env python
import pika
import logging
import time
import json
logging.basicConfig()

connection = pika.BlockingConnection(pika.ConnectionParameters(host='localhost',port=5672))
channel = connection.channel()
channel.queue_declare(queue='hello')

data = {'username': 'zaid', 'age': int(24)}
data = json.dumps(data)

while True:
    channel.basic_publish(exchange='',
                          routing_key='hello',
                          body=data)
    print("Message sent")
    time.sleep(5)
