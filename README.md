# coap-publish-subcribe
Public subscribe implementation for the COnstrained Application Protocol (COAP)

https://tools.ietf.org/pdf/draft-ietf-core-coap-pubsub-05.pdf
# Contributors
[Gioele Carignani](https://github.com/gioelec/)<br>
[Vincenzo Consales](https://github.com/vconsales/)<br>
[Federico Rossi](https://github.com/federicorossifr)<br>


# RUN THE BROKER INSIDE A CONTAINER
If it is the first time, install docker-compose:
```console 
sudo curl -L "https://github.com/docker/compose/releases/download/1.23.1/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
``` 

If not started, start the Docker daemon:

```console 
sudo systemctl start docker
```

Apply executable permissions to the binary:

```console
sudo chmod +x /usr/local/bin/docker-compose
```

Run the container (execute command from inside the broker folder, where the docker-compose.yml file is)

```console 
sudo docker-compose up
```

The broker server will run at the address specified inside the docker-compose.yml at port 5683
