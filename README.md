# coap-publish-subcribe
Public subscribe implementation for the COnstrained Application Protocol (COAP)

https://tools.ietf.org/pdf/draft-ietf-core-coap-pubsub-05.pdf
# Contributors
[Gioele Carignani](https://github.com/gioelec/)<br>
[Vincenzo Consales](https://github.com/vconsales/)<br>
[Federico Rossi](https://github.com/federicorossifr)<br>


# RUN THE BROKER INSIDE A CONTAINER
Install Docker from the official website [here](https://docs.docker.com/install/linux/docker-ce/ubuntu/#os-requirements)<br>
If it is the first time, install docker-compose:
```sh 
sudo curl -L "https://github.com/docker/compose/releases/download/1.23.1/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
``` 

If not started, start the Docker daemon:

```sh 
sudo systemctl start docker
```

Apply executable permissions to the binary:

```sh
sudo chmod +x /usr/local/bin/docker-compose
```

Run the container (execute command from inside the broker folder, where the docker-compose.yml file is)

```sh 
sudo docker-compose up
```

The broker server will run at the address specified inside the docker-compose.yml at port 5683

If either Broker app or CoAPthon source have been modified, re-build the container image before running it:

```sh 
sudo docker-compose build --no-cache
```
