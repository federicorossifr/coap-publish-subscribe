FROM python:2.7
ADD *.py /
ADD *.conf /
RUN git clone https://github.com/federicorossifr/CoAPthon.git
RUN ls CoAPthon
RUN cd CoAPthon && python setup.py sdist
RUN pip install CoAPthon/dist/CoAPthon-4.0.2.tar.gz
EXPOSE 5683
CMD [ "python", "app.py" ]
