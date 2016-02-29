#!/usr/bin/env python
import pika
import logging

logging.basicConfig()

connection = pika.BlockingConnection(pika.ConnectionParameters(host='localhost',port=5672))

channel = connection.channel()

channel.queue_declare(queue='hello')

channel.basic_publish(exchange='',
                      routing_key='hello',
                      body='Hello World!')
print("Message sent")
