# :coffee: Reliable File Transfer Peer to Peer with UDP
Reliable File Transfer with UDP protocol.

## :mag: Requirements
To run the code you need three virtual machines with the Linux operating system installed (the one used by us was Linux Mint 19) with an updated repository and gcc installed (command: sudo apt get install gcc). The network of each virtual machine must be configured in “bridge mode” and each one must have a different ipv4 and, if not, it must be manually configured (excluding the DHCP option to generate the ip automatically).

## :bell: Information
The server.c file must be placed on the first virtual machine, in addition the banco.txt(the bank) file must also be placed on the same virtual machine.
The cliente.c file must be placed on a second virtual machine.
The cliente2.c file must be placed on a third virtual machine along with some text or video or image file for the transfer.

In banco.txt, the following information must be placed on each line:
file_name.extension ip_where_is_the_file port

The bank is responsible for knowing the files of each node.

The gif.gif, video.mp4, planeta.jpg, mickey.png and aa.txt files can be used as a transfer test by the cliente2.

## :pencil2: Change
The files and the ip must be changed in banco.txt if necessary.
The IP_SERVER in #define at cliente2.c must be changed to the ip of the virtual machine that has the server.c
The IP_LOCAL in #define at cliente.c must changed to the ip of the virtual machine that is with the cliente.c

## :microscope: To compile
To compile, open a terminal in the files folder and use the command:

> gcc filename.c -o executable_name

## :bicyclist: Run
To run server.c and cliente2.c you must open a terminal in the folder that have the archives.c already compiled(in the respective virtual machine of the .c) and use the command:

> ./executable_name

To run cliente.c you must open a terminal in the folder that have the archives.c already compiled(in the respective virtual machine of the .c) and use the command:

> ./executable_name ip_server filename.extension

## :eyeglasses: Explanation of .c
**server.c:** it is a file used by the server to send responses to clients, it manages who is going to send the file to whoever is requesting it and saves information in banco.txt of the files that the nodes have.

**cliente.c:** is a file used for the client that will request a file from the server, it requests and waits for a response and a probable sending of the file by another node.

**cliente2.c:** is a file used to wait for the server's request to send a file to some node, so it waits for the communication with the server and then connects to a node that requested a file and sends it.

**Note:** the program has a package number, acknowledgment, checksum and data timer to guarantee the delivery of the packages and the formation of the file in UDP (sock_dgram).
If you have some questions or suggestions, or found an error, please contact me. Thanks.

-------------------------------------------------------------------------------------------------------
**PT-BR**
# Transferência Confiável de Arquivos Ponto a Ponto com UDP

## :mag: Requisitos
Para executar o código precisa de três máquinas virtuais com o sistema operacional Linux instalado(o utilizado por nós foi o Linux Mint 19) com repositório atualizado e gcc instalado(comando: sudo apt get install gcc). A rede de cada máquina virtual deve ser configurada para em “modo bridge” e cada um deve ter um ipv4 diferente e caso não tenha deve ser configurado manualmente(tirando a opção do DHCP gerar o ip automaticamente).

## :bell: Informações
O arquivo server.c deve ser colocado na primeira máquina virtual, além disso o arquivo banco.txt também deve ser colocado na mesma máquina virtual.
O arquivo cliente.c deve ser colocado em uma segunda máquina virtual.
O arquivo cliente2.c deve ser colocado em uma terceira máquina virtual junto com algum arquivo de texto ou vídeo ou imagem para a transferência.

No banco.txt deve ser colocado em cada linha as seguintes informações:
nome_do_arquivo.extensão ip_onde_está_o_arquivo porta

O banco é responsável por saber os arquivos de cada nó.

Os arquivos gif.gif, video.mp4, planeta.jpg, mickey.png e aa.txt podem ser usados como teste de transferência pelo cliente2.

## :pencil2: Alterar
Deve ser alterado no banco.txt os arquivos e o ip caso seja necessário.
Deve ser alterado o IP_SERVER no #define do cliente2.c para o ip da máquina virtual que estiver com o server.c
Deve ser alterado o IP_LOCAL do #define do cliente.c para o ip da máquina virtual que estiver com o cliente.c

## :microscope: Compilar
Para compilar deve abrir um terminal na pasta dos arquivos e utilizar o comando:

> gcc nome_arquivo.c -o nome_executavel

## :bicyclist: Executar
Para executar o server.c e o cliente2.c deve abrir um terminal na pasta dos arquivos(já compilados) e em suas respectivas máquinas virtuais e utilizar o comando:

> ./nome_executavel

Para executar o cliente.c deve abrir um terminal na pasta dos arquivos(já compilados) e em sua respectiva máquina virtual e utilizar o comando:

> ./nome_executavel ip_do_server nome_do_arquivo.extensão

## :eyeglasses: Explicação dos .c
**server.c:** é um arquivo utilizado pelo servidor para enviar respostas aos clientes, ele que administra quem vai enviar o arquivo para quem está requisitando e salva informações no banco.txt dos arquivos que os nós possuem.

**cliente.c:** é um arquivo utilizado para o cliente que irá requisitar algum arquivo ao server, ele requisita e espera uma resposta e um provável envio do arquivo por parte de um outro nó.

**cliente2.c:** é um arquivo utilizado para esperar a solicitação do server para o envio de algum arquivo para algum nó, ou seja, ele espera a comunicação com o servidor e depois se conecta a um nó que requisitou um arquivo e o envia.

**Obs.:** o programa apresenta número de pacote, reconhecimento, checksum e temporizador de dados para garantir a entrega dos pacotes e a formação do arquivo em UDP(sock_dgram).
Se você tiver alguma dúvida ou sugestão ou encontrar algum erro, contate-me porfavor. Obrigado.
