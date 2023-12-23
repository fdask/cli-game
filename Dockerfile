FROM php:8.2-cli
COPY . /usr/src/rain
WORKDIR /usr/src/rain
CMD [ "./main.php" ]
