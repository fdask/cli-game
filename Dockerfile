FROM php:8.2-cli
COPY . /usr/src/rain
WORKDIR /usr/src/rain
CMD [ "php", "./main.php" ]
