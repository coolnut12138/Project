#使用openssl生成一个RSA密钥
openssl genrsa 2048 > key.pem
#通过密钥生成证书
openssl req -new -key key.pem | openssl x509 -days 3650 -req -signkey key.pem > cert.pem
